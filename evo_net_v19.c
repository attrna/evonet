#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define group_persons 2
#define genes_per_person 2
#define max_gene_count 1000

int debug_prints=1;

/*
step 1: create array of floats nxn where n is genes per person (done but problem with srand doesnt create distinct random values FIXED)
step 2: create array of genes per person (done rand works creates different sets of random values)
step 3: create pointers to each of the above mentioned data structures (done)
step 4: create person arrays each entry must have pointer to step 1 and step 2 (done)
step 5: create population hash-table with pointers to each group_persons(done)
step 6: check for errors in each malloc (done)
step 7: create the kill functions, you will specify how many people must die and will get removed. Printed before and after
step 8: create the personal matrixes R1 ,R2 (done)
step 9: figure out on each gene R1 and R2 balance (ipologizetai swsta to balance alla otan to pernaw mesa stous pinakes mou emfanizei mono to teleutaio.
    paizei na exw kanei malakia me to print. ) (DOne)
step 10: check matrix multiplication working correctly
step 11: ta orismata pou pairnw gia create new gne counts einai kala?
step 12: na mpoun oi free (done)
step 13: na mpoun comments giati kapoia stigmi 8a to xaseis kai mesa stis sinartiseis gia na mporw na to diabasw meta(Done)
step 14: na mpoun oi dikleides asfaleies se oles tis malloc min skasei kati kai psaxnomai (done)
step 15: na kanw print ola ta balance oolwn twn gonidiwn
*/

/*Logic of program
    Create array named population which points to each each group of persons. In order for the program to be scalable each bucket contains N persons
    Each bucket points to an array where all the people from that group have 2 pointers: 1 pointer to their personal gene counts and 1 pointer to their
    personal gene interactions

                min<id<max           personal_id
    population-------------->bucket---------------->personal reconds

    Basically population is a hash table
*/


typedef struct personal_gene_interaction_matrix{
    float gene_inter_matrix[genes_per_person][genes_per_person];
}personal_gene_interaction_matrix;

typedef struct personal_gene_count{
    int indi_gene_array[genes_per_person][2];
}personal_gene_count;

typedef struct personal_gene_list{
    int indi_gene_array[genes_per_person][2];
    struct personal_gene_list *next;
}personal_gene_list;

typedef struct personal_r1{
    char *personal_gene_r1[genes_per_person];
    int balance[genes_per_person];
}personal_r1;

typedef struct personal_r2{
     char *personal_gene_r2[genes_per_person];
     int balance[genes_per_person];
}personal_r2;

typedef struct personal_records{
    int personal_id;
    personal_gene_count *ptr_to_genes_count;
    personal_gene_list *ptr_to_new_genes_count;
    personal_gene_interaction_matrix *gene_dependancies;    /*need a better name*/
    personal_r1* ptr_to_personal_r1;
    personal_r2* ptr_to_personal_r2;
}personal_records;

typedef struct array_of_group{
    personal_records* personal_record[group_persons];
}array_of_group;

typedef struct population_list{
    int max_id;
    array_of_group* groups;
    struct population_list *next;
}population_list;

struct population_list *plist_head = NULL;
int total_population=0;

/* convert integer to binary. Max integer (2^8)-1. Returns pointer to string representing the binary form*/
char *int_to_binary(int n){
    int c, d, count;
    char *pointer;

    count = 0;
    pointer = (char*)malloc(32+1);

    if ( pointer == NULL ){
        printf("No memory left for pointer on making integer_to_binary\n");
        exit(EXIT_FAILURE);
    }
    for ( c = 7 ; c >= 0 ; c-- ){
        d = n >> c;
        if ( d & 1 )
          *(pointer+count) = 1 + '0';
       else
          *(pointer+count) = 0 + '0';
       count++;
    }
    *(pointer+count) = '\0';
    return  pointer;
}

/*take the first 4 bits of 8-bit string. Return pointer to that string + null terminated*/
char *break_binary_to_first_part(char *pointer){
    char first_part[5];
    char *ptr_to_first_part;
    strncpy(first_part, pointer,4);
    first_part[4] = '\0';

    ptr_to_first_part = (char *)malloc(sizeof(char));
    if(ptr_to_first_part==NULL){
        printf("No memory left for ptr_to_first_part on break_binary_to_first_part\n");
        exit(EXIT_FAILURE);
    }

    ptr_to_first_part=first_part;

   return ptr_to_first_part;
}

/*take the last 4 bits of 8-bit string. Return pointer to that string + null terminated*/
char *break_binary_to_second_part(char *pointer){
    char second_part[5];
    char *ptr_to_second_part;
    strncpy(second_part, pointer + 4, 7 - 2);
    second_part[4] = '\0';

     ptr_to_second_part = (char *)malloc(sizeof(char));
     if ( ptr_to_second_part == NULL ){
        printf("No memory left for ptr_to_second_part on break_binary_to_second_part\n");
        exit(EXIT_FAILURE);
    }

    ptr_to_second_part=second_part;

   return ptr_to_second_part;
}

/*count how many 1s on first part and on second part and subtract them return the result*/
int create_balance(char *pointer){
    int count_first_part=0;
    int count_second_part=0;
    int i,j,balance;
    char *ptr_first_part;
    char *ptr_second_part;

    ptr_first_part=break_binary_to_first_part(pointer);
    for(i=0; ptr_first_part[i]; i++) {
        if(ptr_first_part[i] != '0') {
            count_first_part ++;
        }
    }

    ptr_second_part=break_binary_to_second_part(pointer);
    for(j=0; ptr_second_part[j]; j++) {
        if(ptr_second_part[j] != '0') {
            count_second_part ++;
        }
    }


    balance=count_first_part-count_second_part;
    return balance;
}

/*returns a random number from a normal_distribution based on the mu and the sigma*/
double random_normal_distrubution (double mu, double sigma){
  double U1, U2, W, mult;
  static double X1, X2;
  static int call = 0;
 /*got no clue how it does what it does but it works perfectly and that is good enough for me*/
    if(call == 1){
      call = !call;
      return (mu + sigma * (double) X2);
    }

    do{
      U1 = -1 + ((double) rand () / RAND_MAX) * 2;
      U2 = -1 + ((double) rand () / RAND_MAX) * 2;
      W = pow (U1, 2) + pow (U2, 2);
    }while (W >= 1 || W == 0);

    mult = sqrt ((-2 * log (W)) / W);
    X1 = U1 * mult;
    X2 = U2 * mult;

    call = !call;

    return (mu + sigma * (double) X1);
}

/*initialize personal nxn interaction matrix. Where n=gene_counts. Return pointer*/
personal_gene_interaction_matrix* initialize_personal_gene_interaction_matrix(){
    int i,j;
    personal_gene_interaction_matrix *ptr_to_gene_inter_matrix;
    ptr_to_gene_inter_matrix=(personal_gene_interaction_matrix*)malloc(sizeof(personal_gene_interaction_matrix));

    if ( ptr_to_gene_inter_matrix == NULL ){
        printf("No memory left for gene_interaction_matrix on initialize_personal_gene_counts\n");
        exit(EXIT_FAILURE);
    }

    for (i=0;i<genes_per_person;i++){
        for (j=0;j<genes_per_person;j++){
          /* ptr_to_gene_inter_matrix->gene_inter_matrix[i][j]=((((float)rand()/(float)(RAND_MAX)) * max_gene_interaction)+ (-minimum_gene_interaction));
          */
          /*all values from gaussian distrubution*/
            ptr_to_gene_inter_matrix->gene_inter_matrix[i][j]=random_normal_distrubution(0,sqrt(10));
          }
    }

    return ptr_to_gene_inter_matrix;
}

/*initialize personal gene counts,and put them on nx1 return pointer*/
personal_gene_count* initialize_personal_gene_count(){
    int i;
    personal_gene_count *ptr_to_indi_gene_count;
    ptr_to_indi_gene_count = (personal_gene_count *)malloc(sizeof(personal_gene_count));


    if(ptr_to_indi_gene_count==NULL){
        printf("Not enough memory for personal gene counts matrix\n");
        exit(EXIT_FAILURE);
    }
    /*first column of the array stores the number of the gene the second store the random generated gene_count*/
    for (i=0;i<genes_per_person;i++){
        ptr_to_indi_gene_count->indi_gene_array[i][0]=i;
        ptr_to_indi_gene_count->indi_gene_array[i][1]=rand()%500;
    }

    return ptr_to_indi_gene_count;
}

/*create personal r1 using rand%256*/
personal_r1* initialize_r1(){
    int j;
    personal_r1 *ptr_to_personal_r1;
    ptr_to_personal_r1 = (personal_r1 *)malloc(sizeof(personal_r1));
    if ( ptr_to_personal_r1 == NULL ){
        printf("No memory left ptr_to_personal_r1 on initialize R1\n");
        exit(EXIT_FAILURE);
    }

    /*For every gene create a R1 balance and save it*/
    for (j=0;j<genes_per_person;j++){
        ptr_to_personal_r1->personal_gene_r1[j]=int_to_binary(rand()%256);
        ptr_to_personal_r1->balance[j]=create_balance(ptr_to_personal_r1->personal_gene_r1[j]);
    }
    return ptr_to_personal_r1;
}

/*create personal r2 using rand%256*/
personal_r2* initialize_r2(){
    int j;
    personal_r2 *ptr_to_personal_r2;
    ptr_to_personal_r2 = (personal_r2 *)malloc(sizeof(personal_r2));
    if ( ptr_to_personal_r2 == NULL ){
        printf("No memory left ptr_to_personal_r2 on initialize R2\n");
        exit(EXIT_FAILURE);
    }

    /*For every gene create a R2 balance and save it*/
    for (j=0;j<genes_per_person;j++){
        ptr_to_personal_r2->personal_gene_r2[j]=int_to_binary(rand()%256);
        ptr_to_personal_r2->balance[j]=create_balance(ptr_to_personal_r2->personal_gene_r2[j]);
    }
    return ptr_to_personal_r2;
}

/*create new personal gene counts by multiplying the last gene counts with the personal interaction matrix.Put the new gene counts
on the head of the list of personal new_gene_counts_list*/
void create_new_gene_counts(personal_records *personal_record,int generation){
    int i,j,b;
    float a;
    personal_gene_list *ptr_to_new_personal_gene_counts;

    ptr_to_new_personal_gene_counts= (personal_gene_list *)malloc(sizeof(personal_gene_list));
    if ( ptr_to_new_personal_gene_counts == NULL ){
        printf("No memory left ptr_to_new_personal_gene_counts\n");
        exit(EXIT_FAILURE);
    }

    /*place the new_counts at the head of the new_gene_counts*/
    ptr_to_new_personal_gene_counts->next=personal_record->ptr_to_new_genes_count;
    personal_record->ptr_to_new_genes_count=ptr_to_new_personal_gene_counts;

    /*if it is the first generation there are no new_gene_counts, thus we have to create them from the original gene counts*/
    if (generation==0){
        for (i=0;i<genes_per_person;i++){
            for(j=0;j<genes_per_person;j++){
                a=personal_record->gene_dependancies->gene_inter_matrix[i][j];
                b=personal_record->ptr_to_genes_count->indi_gene_array[j][1];
                ptr_to_new_personal_gene_counts->indi_gene_array[i][0]=i;
                ptr_to_new_personal_gene_counts->indi_gene_array[i][1]=(a*b);
            }
        }
    }
    /*Now we have some new_gene_counts so the next generation is produced using the last generations gene_counts*/
    else{
        for (i=0;i<genes_per_person;i++){
            for(j=0;j<genes_per_person;j++){
                a=personal_record->gene_dependancies->gene_inter_matrix[i][j];
                b=personal_record->ptr_to_new_genes_count->next->indi_gene_array[j][1];
                ptr_to_new_personal_gene_counts->indi_gene_array[i][0]=i;
                ptr_to_new_personal_gene_counts->indi_gene_array[i][1]=(a*b);
            }
        }
    }

}

/*create new generations by calling create_new_gene_counts num_of_gens times for everyone on the population*/
void aging(int num_of_gens){
    int i,j;
    population_list *temp=plist_head;
    for(i=0;i<num_of_gens;i++){
       /*"Advance the whole population*/
        while(temp!=NULL){
            for(j=0;j<group_persons;j++){
                create_new_gene_counts(temp->groups->personal_record[j],i);
            }
            temp=temp->next;
        }
        temp=plist_head;
    }
}

/*check each individual for plasticity of genes by comparing each new_genes_count record with all the others personal gene_counts*/
void check_plasticity(personal_records *personal_record){
    personal_gene_list *temp1=personal_record->ptr_to_new_genes_count;
    personal_gene_list *temp2=temp1;
    int i,plastic_flag=0;

    if (temp1==NULL){
        printf("No new gene counts. Will exit now\n");
        return;
    }
    /*actual checking takes place here.For everyone, for every generation check*/
    while (temp1->next!=NULL){
        while (temp2->next!=NULL){
            for (i=0;i<genes_per_person;i++){
                if(temp1->indi_gene_array[i][1]!=temp2->indi_gene_array[i][1]){
                    plastic_flag=1;
                }
            }
            if (plastic_flag) printf("Plasticity\n");
            temp2=temp2->next;
        }
        temp1=temp1->next;
    }
}

/*wrapper function to call check_plasticity for scaning each on the population for plasticity*/
void general_plasticity(){
    int j;
    population_list *temp=plist_head;
        while(temp!=NULL){
            for(j=0;j<group_persons;j++){
               /* For everyone for every generation passed */
                check_plasticity(temp->groups->personal_record[j]);
            }
            temp=temp->next;
        }
}

/*create personal records for each individual. (no new_gene_count since there is no aging when they are "born")*/
personal_records *create_personal_records(int personal_id){
    personal_records *ptr_to_personal_records;
    ptr_to_personal_records = (personal_records *)malloc(sizeof(personal_records));
    if ( ptr_to_personal_records == NULL ){
        printf("No memory left ptr_to_personal_records on create_personal_records\n");
        exit(EXIT_FAILURE);
    }

        /*fill the ptr_personal_records fields*/
        ptr_to_personal_records->personal_id=personal_id;
        ptr_to_personal_records->ptr_to_genes_count=initialize_personal_gene_count();
        ptr_to_personal_records->gene_dependancies=initialize_personal_gene_interaction_matrix();
        ptr_to_personal_records->ptr_to_personal_r1=initialize_r1();
        ptr_to_personal_records->ptr_to_personal_r2=initialize_r2();
        ptr_to_personal_records->ptr_to_new_genes_count=NULL;

        return ptr_to_personal_records;
}

/*for everyone in a group call create_personal_records to create them*/
array_of_group *create_group_persons_array(int max_id){
    int i;
    array_of_group *ptr_grp_person_array;
    ptr_grp_person_array = (array_of_group *)malloc(sizeof(array_of_group));
    if ( ptr_grp_person_array == NULL ){
        printf("No memory left ptr_grp_person_array on create_group_persons_array\n");
        exit(EXIT_FAILURE);
    }

    /*fill the ptr_to_grp_person_array*/
    for (i=0;i<group_persons;i++){
        ptr_grp_person_array->personal_record[i]=create_personal_records(max_id-i);
    }

    return ptr_grp_person_array;
}

/*add new population on population_list the new people always on the head of the population list*/
void add_population_node(int max_id){
    struct population_list *new_node = (population_list*)malloc(sizeof(population_list));

    if ( new_node == NULL ){
        printf("No memory left new_node on add_population_node\n");
        exit(EXIT_FAILURE);
    }

    /*fill the new_node fields*/
    new_node->max_id = max_id;
    new_node->next = NULL;
    new_node->groups=create_group_persons_array(max_id);

    new_node->next = plist_head; /*place on head of the list*/
    plist_head = new_node;
}

/*determine how many people_groups are needed to store the new people then call add_population_node to actually add them*/
void add_population(int population){
    int i;
    total_population=population+total_population;     /*add to global used for delete*/
    for (i=0;i<population;i=i+group_persons){
        add_population_node(i+1);
    }
}

/*delete people checking if the number of them exceeds the current population*/
void delete_population(int population_to_delete){
    population_list *temp=plist_head;
    int j;

    if(total_population<=population_to_delete){
        printf("Population to be removed exceeds total_population. Will Delete all of the active population.\n");
        while (plist_head->next!=NULL){
            temp=plist_head;
            plist_head=plist_head->next;
            free(temp);
        }
        plist_head=NULL;
    }

    for(j=0;j<population_to_delete;j=j+group_persons){
        temp=plist_head;
        plist_head=plist_head->next;
        free(temp->groups->personal_record[j]->gene_dependancies);
        free(temp->groups->personal_record[j]->ptr_to_genes_count);
        free(temp->groups->personal_record[j]->ptr_to_new_genes_count);
        free(temp->groups->personal_record[j]->gene_dependancies);
        free(temp->groups->personal_record[j]->ptr_to_personal_r1);
        free(temp->groups->personal_record[j]->ptr_to_personal_r2);
        free(temp->groups);
        free(temp);
    }
}

/*Print everything*/
void print_dump(){
    population_list *temp=plist_head;
    int i,j,k,l,u;
    while(temp!=NULL){
        printf("Gia to group me max id %d\n",temp->max_id);
        for(i=0;i<group_persons;i++){
            printf("Gia to atomo me id %d\n",temp->groups->personal_record[i]->personal_id);
            for (j=0;j<genes_per_person;j++){
/*                printf("Gene No %d ",temp->groups->personal_record[i]->ptr_to_genes_count->indi_gene_array[j][0]);
                printf("Gene Count %d ",temp->groups->personal_record[i]->ptr_to_genes_count->indi_gene_array[j][1]);
                printf("R1 of Gene %s ", temp->groups->personal_record[i]->ptr_to_personal_r1->personal_gene_r1[j]);
                printf("Balance of R1 %d ", temp->groups->personal_record[i]->ptr_to_personal_r1->balance[j]);
                printf("R2 of Gene %s ", temp->groups->personal_record[i]->ptr_to_personal_r2->personal_gene_r2[j]);
                printf("Balance of R2 %d ",temp->groups->personal_record[i]->ptr_to_personal_r2->balance[j]);
*/              printf("\n");
                while(temp->groups->personal_record[i]->ptr_to_new_genes_count!=NULL){
                    for(u=0;u<genes_per_person;u++){
                            printf("New Gene No %d ",temp->groups->personal_record[i]->ptr_to_new_genes_count->indi_gene_array[u][0]);
                            printf("New Gene Count %d ",temp->groups->personal_record[i]->ptr_to_new_genes_count->indi_gene_array[u][1]);
                            printf("\n");
                    }
                    temp->groups->personal_record[i]->ptr_to_new_genes_count=temp->groups->personal_record[i]->ptr_to_new_genes_count->next;
                }
            }
/*
            printf("Pinakas Dependencies Sigkekrimenou Atomou\n");
            for (k=0;k<genes_per_person;k++){
                for (l=0;l<genes_per_person;l++){
                    printf(" %f ",temp->groups->personal_record[i]->gene_dependancies->gene_inter_matrix[k][l]);
                }
                printf("\n");
            }
*/            printf("\n");
        }
        temp=temp->next;
    }
}

int main(){
   /* int num_for_delete,num_for_insert,num_of_gens;
    int input; */
    srand (time(NULL));
/*

    printf("\n");
    printf("For Insert Event press 1\n");
    printf("For Delete press 2\n");
    printf("For Print_Dump press 3\n");
    printf("For Aging press 4\n");
    printf("For Termination press 5\n");
    printf("\n");


    do{
        printf("Waiting for new event\n");
        input=getchar();
        getchar();
        switch(input){
            case'1':
                printf("Please enter number of insertions\n");
                scanf("%d", &num_for_insert);
                add_population(num_for_insert);
                printf("\n");
                break;
            case'2':
                printf("Please enter number of deletions\n");
                scanf("%d", &num_for_delete);
                delete_population(num_for_delete);
                printf("\n");
                break;
            case'3':
                print_dump();
                printf("\n");
                break;
            case'4':
                printf("Please enter number of generations\n");
                scanf("%d", &num_of_gens);
                aging(num_of_gens);
                printf("\n");
                break;
            default:
                printf("\n");
                break;
        }

    } while(input != '5');
*/

    add_population(6);
    aging(5);
    delete_population(2);
    general_plasticity();
    print_dump();
    return 0;
}