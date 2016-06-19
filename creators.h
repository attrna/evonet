float create_gene_interactions(int R1,int R2){
    float interaction=0;
    float power_of_interaction=0;
    int R1_binary;
    int R2_binary;

    unsigned lastbitR1;
    unsigned lastbitR2;

    R1_binary=decimal_to_binary(R1);
    R2_binary=decimal_to_binary(R2);

    lastbitR1=R1%2;
    lastbitR2=R2%2;


/*
    printf("Mpika me R1 = %d , R2 = %d  \n",R1,R2);

    printf("R1 se binary=%010d \n",R1_binary);
    
    printf("R2 se binary=%010d \n",R2_binary);

    printf("lastbitR1=%d \n",lastbitR1);
    printf("lastbitR2=%d \n",lastbitR2); 
*/
    if(lastbitR2==lastbitR1){ /*ri8misi*/
	   power_of_interaction=NumberOfSetBits(R1_binary & R2_binary);
      /*  printf("Interaction:%f \n",power_of_interaction); */
        power_of_interaction = power_of_interaction / genes_per_person; /*kanonikopoihsh*/

        if(lastbitR1){
            interaction=power_of_interaction;
 /*           printf("Positive Interaction:%f \n",interaction); */
        }
        else {
            interaction=-power_of_interaction;
 /*           printf("Negative Interaction:%f \n",interaction); */
        }
    }
    /*printf("%f ",interaction);*/
    return interaction;
}


/*Create personal records, return pointer to person */
person *create_person(int id){
    int i,j;

    person *new_person;
    new_person = (person*)calloc(1, sizeof(person));

    new_person->id=id;

   /* printf("Sto atomo me id: %d \n",new_person->id); */
    for(i=0;i<genes_per_person;i++){
        new_person->gene_counts[i]=(int)random_normal_distrubution(100,sqrt(100));
    }

    for(i=0;i<genes_per_person;i++){
        new_person->vector_of_signs[i]=1;  /* first generation so gene_counts always positive on the vector -> 1 */
    }

    for (i=0;i<genes_per_person;i++){
        new_person->gene_R1[i]=rand()%1024; /*return an integer*/
        new_person->gene_R2[i]=rand()%1024; /*return an integer*/
        /*printf("atomou R1: %d R2: %d\n",new_person->gene_R1[i],new_person->gene_R2[i]); */
    }

    for(i=0;i<genes_per_person;i++){
        for(j=0;j<genes_per_person;j++){
            /*new_person->gene_interactions[i][j]=random_normal_distrubution(0,sqrt(10));*/
            new_person->gene_interactions[i][j]=create_gene_interactions(new_person->gene_R1[i],new_person->gene_R2[j]);
           /* printf("%d ",new_person->gene_interactions[i][j]); */
        }
     /*   printf("\n"); */
    }
  /*  printf("Created Person: %d\n", new_person->id); */
    return new_person;
}

/*return pointer to new group. New group is array of pointers to persons.*/
group *create_group(int group_num){
    int i;
    group *new_group;
    new_group = (group*)calloc(1, sizeof(group));
    new_group->group_number=group_num;
    for(i=0;i<persons_per_group;i++){
        new_group->person_in_group[i]=create_person(i); /*create pointer to person, save on the groups array , argument is the personal id */
    }
    return new_group;
}



/*return pointer to new population. New population is array of pointers to groups.*/
population *create_population(int groups_wanted){
    int i;
    /*int j;*/
    group *temp;

    population *new_population;
    new_population = (population*)calloc(1, sizeof(population));
    
    temp=NULL;
    for(i=0;i<groups_wanted;i++){
        if(i==0){
           /* printf("Head on the group_list of the generation 0\n"); */          
            new_population->groups_list=create_group(i); /*create pointer to group_list, save on the population array*/
            new_population->groups_list->next=NULL;
            new_population->groups_list->prev=NULL;
        }
        else{
            temp=new_population->groups_list;
            while(temp->next!=NULL){
                temp=temp->next;
            }
            temp->next=create_group(i);
            temp->next->next=NULL;
            temp->next->prev=temp;
        }
       /* printf("Created Group: %d\n",curr_num_of_groups); */
        curr_num_of_groups++;
    }
/*
    temp=new_population->groups_list;
    for(i=0;i<groups_wanted;i++){
        printf("Group Num:%d\n",i);
        for(j=0;j<persons_per_group;j++){
            printf("personal id %d\n", temp->person_in_group[j]->id);
        }
        temp=temp->next;
    }
*/
    return new_population;
}