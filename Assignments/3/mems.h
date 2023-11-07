#define _GNU_SOURCE
/*
All the main functions with respect to the MeMS are inplemented here
read the function discription for more details

NOTE: DO NOT CHANGE THE NAME OR SIGNATURE OF FUNCTIONS ALREADY PROVIDED
you are only allowed to implement the functions 
you can also make additional helper functions a you wish

REFER DOCUMENTATION FOR MORE DETAILS ON FUNSTIONS AND THEIR FUNCTIONALITY
*/
// add other headers as required
#include<stdio.h>
#include<stdlib.h>
#include <sys/mman.h>
#include <math.h>
 



/*
Use this macro where ever you need PAGE_SIZE.
As PAGESIZE can differ system to system we should have flexibility to modify this 
macro to make the output of all system same and conduct a fair evaluation. 
*/
#define PAGE_SIZE 4096


/*
Initializes all the required parameters for the MeMS system. The main parameters to be initialized are:
1. the head of the free list i.e. the pointer that points to the head of the free list
2. the starting MeMS virtual address from which the heap in our MeMS virtual address space will start.
3. any other global variable that you want for the MeMS implementation can be initialized here.
Input Parameter: Nothing
Returns: Nothing
*/
struct main
{
    int pages;
    struct main* next;
    struct main* prev;
    struct sub* right;
    size_t mems_virtual_address;
    size_t offset;
};

struct  sub{
    size_t size;
    char *type;
    struct sub* next;
    struct sub* prev;
};
struct main* head;
size_t Mems_Starting_Virtual_Address;

void mems_init(){
    void *linked_list_starting_address=mmap(NULL,PAGE_SIZE,PROT_READ | PROT_WRITE | PROT_EXEC , MAP_PRIVATE|MAP_ANONYMOUS ,-1,0);
    if(linked_list_starting_address==MAP_FAILED){
        perror("mmap failed");
        exit(1);
    }
    munmap(linked_list_starting_address+sizeof(struct main),PAGE_SIZE-sizeof(struct main));
    head=(struct main*)linked_list_starting_address;
    head->next=NULL;
    head->prev=NULL;
    head->pages=0;
    head->right=NULL;
    

    Mems_Starting_Virtual_Address=1000;
    head->mems_virtual_address=Mems_Starting_Virtual_Address;
       
}


/*
This function will be called at the end of the MeMS system and its main job is to unmap the 
allocated memory using the munmap system call.
Input Parameter: Nothing
Returns: Nothing
*/
void mems_finish(){
    
    struct main *remove_main=head;
    while(remove_main!=NULL){
        if(remove_main->right!=NULL){
            struct sub* remove_sub=remove_main->right;
            while (remove_sub!=NULL){
                struct sub *temp=remove_sub->next;
                munmap(remove_sub,sizeof(struct sub));
                remove_sub=temp;
            }
            munmap((void *)(remove_main->mems_virtual_address+remove_main->offset),(size_t)(remove_main->pages*PAGE_SIZE));
        }
        struct main *temp=remove_main->next;
        munmap(remove_main,sizeof(struct main));
        remove_main=temp;
    }
    
}


/*
Allocates memory of the specified size by reusing a segment from the free list if 
a sufficiently large segment is available. 

Else, uses the mmap system call to allocate more memory on the heap and updates 
the free list accordingly.

Note that while mapping using mmap do not forget to reuse the unused space from mapping
by adding it to the free list.
Parameter: The size of the memory the user program wants
Returns: MeMS Virtual address (that is created by MeMS)
*/ 
void* mems_malloc(size_t size){
    
    struct main *iterator=head;
    struct main *temp=head;
    while(temp->next!=NULL){
        temp=temp->next;
    }
    int pages=0;
    if(iterator->next!=NULL){
        iterator=iterator->next;
    }
    while(pages*PAGE_SIZE<size){
        pages++;
    }
    size_t mems_virtual_memory=iterator->mems_virtual_address;
    while(iterator!=NULL){

        struct sub* sub_iterator=iterator->right;
        void * mems_virtual_memory=(void *)iterator->mems_virtual_address;

        while(sub_iterator!=NULL){

            if(sub_iterator->size>=size && sub_iterator->type=="HOLE"){
                
                void *linked_list_starting_address=mmap(NULL,PAGE_SIZE,PROT_READ | PROT_WRITE | PROT_EXEC , MAP_PRIVATE|MAP_ANONYMOUS ,0,0);
                if(linked_list_starting_address==MAP_FAILED){
                    perror("mmap failed");
                    exit(1);
                }
                munmap(linked_list_starting_address+sizeof(struct sub),PAGE_SIZE-sizeof(struct sub));
                struct sub *new_sub_node=(struct sub*)linked_list_starting_address;

                sub_iterator->type="PROCESS";
                new_sub_node->type="HOLE";

                new_sub_node->prev=sub_iterator;
                new_sub_node->size=sub_iterator->size-size;
                sub_iterator->size=size;

                if(sub_iterator->next!=NULL){
                    new_sub_node->next=sub_iterator->next;
                }else{
                    new_sub_node->next=NULL;
                }
                sub_iterator->next=new_sub_node;

                return mems_virtual_memory;
            }else{
                mems_virtual_memory+=sub_iterator->size;
            }
            sub_iterator=sub_iterator->next;
        }
        iterator=iterator->next;
    }
    if(iterator==NULL){
        
        void *main_node_starting_address=mmap(NULL,PAGE_SIZE,PROT_READ | PROT_WRITE | PROT_EXEC , MAP_PRIVATE|MAP_ANONYMOUS ,0,0);
        if(main_node_starting_address==MAP_FAILED){
        perror("mmap failed");
        exit(1);
        }
        munmap(main_node_starting_address+sizeof(struct main),PAGE_SIZE-sizeof(struct main));
        struct main *new_main_node=(struct main*)main_node_starting_address;
        mems_virtual_memory=temp->mems_virtual_address;
        
        temp->next=new_main_node;
        new_main_node->prev=temp;
        new_main_node->pages=pages;
        new_main_node->mems_virtual_address=mems_virtual_memory+(temp->pages*PAGE_SIZE);
        mems_virtual_memory=new_main_node->mems_virtual_address;
        
        void *sub_node_1=mmap(NULL,PAGE_SIZE,PROT_READ | PROT_WRITE | PROT_EXEC , MAP_PRIVATE|MAP_ANONYMOUS ,0,0);
        if(sub_node_1==MAP_FAILED){
        perror("mmap failed");
        exit(1);
        }
        munmap(sub_node_1+sizeof(struct sub),PAGE_SIZE-sizeof(struct sub));
        struct sub *new_sub_node_1=(struct sub*)sub_node_1;

        void *sub_node_2=mmap(NULL,PAGE_SIZE,PROT_READ | PROT_WRITE | PROT_EXEC , MAP_PRIVATE|MAP_ANONYMOUS ,0,0);
        if(sub_node_1==MAP_FAILED){
        perror("mmap failed");
        exit(1);
        }
        munmap(sub_node_2+sizeof(struct sub),PAGE_SIZE-sizeof(struct sub));
        struct sub *new_sub_node_2=(struct sub*)sub_node_2;
        
        void *new_mapping=mmap(NULL,pages*PAGE_SIZE,PROT_READ | PROT_WRITE | PROT_EXEC , MAP_PRIVATE|MAP_ANONYMOUS ,0,0);
        if(new_mapping==MAP_FAILED){
        perror("mmap failed");
        exit(1);
        }

        
        new_main_node->offset=(unsigned long)new_mapping-mems_virtual_memory;
        new_main_node->right=new_sub_node_1;
        
        new_sub_node_1->prev=NULL;
        new_sub_node_1->next=new_sub_node_2;
        new_sub_node_1->type="PROCESS";
        new_sub_node_1->size=size;
        
        new_sub_node_2->type="HOLE";
        new_sub_node_2->size=(pages*PAGE_SIZE)-size;
        new_sub_node_2->next=NULL;
        new_sub_node_2->prev=new_sub_node_1;
        
        
        return (void *)mems_virtual_memory;
    }


}


/*
this function print the stats of the MeMS system like
1. How many pages are utilised by using the mems_malloc
2. how much memory is unused i.e. the memory that is in freelist and is not used.
3. It also prints details about each node in the main chain and each segment (PROCESS or HOLE) in the sub-chain.
Parameter: Nothing
Returns: Nothing but should print the necessary information on STDOUT
*/
void mems_print_stats(){
    printf("----- MEMS SYSTEM STATS -----\n");
    int pages=0;
    size_t freesize=0;
    struct main *iterator=head;
    if (iterator->pages==0){
        iterator=iterator->next;
    }
    
    int main_count=0;
    while(iterator!=NULL){
        main_count++;
        iterator=iterator->next;
    }
    iterator=head->next;
    int sub_length_array[main_count];
    
    main_count=0;


    while(iterator!=NULL){
        void* mems_virtual_address=(void *)iterator->mems_virtual_address;
        printf("MAIN[%lu:%lu]-> ",(unsigned long)iterator->mems_virtual_address,(unsigned long)iterator->mems_virtual_address+iterator->pages*PAGE_SIZE-1);
        
        pages+=iterator->pages;
        struct sub *sub_iterator=iterator->right;
        int sub_count=0;
        while(sub_iterator!=NULL){
            sub_count++;
            if(sub_iterator->type=="PROCESS"){
                printf("P[%lu:%lu] <-> ",(unsigned long)mems_virtual_address,(unsigned long)mems_virtual_address+sub_iterator->size-1);
                mems_virtual_address+=sub_iterator->size;
            }else{
                freesize+=sub_iterator->size;
                printf("H[%lu:%lu] <-> ",(unsigned long)mems_virtual_address,(unsigned long)mems_virtual_address+sub_iterator->size-1);
                mems_virtual_address+=sub_iterator->size;

            }
            sub_iterator=sub_iterator->next;
            
        }
        sub_length_array[main_count]=sub_count;
        iterator=iterator->next;
        main_count++;

        printf("NULL");
        printf("\n");    
    }
    printf("Pages used:\t%d\n",pages);
    printf("Space unused:\t%ld\n",freesize);
    printf("Main Chain Length:\t%d\n",main_count);
    printf("Sub-Chain Length Array: ");
    printf("[");
    for(int i=0;i<main_count;i++){
        if(sub_length_array[i]!=0){
            printf("%d, ",sub_length_array[i]);
        }
    }
    printf("]\n");
    printf("-----------------------------\n");
                                            
    
}


/*
Returns the MeMS physical address mapped to ptr ( ptr is MeMS virtual address).
Parameter: MeMS Virtual address (that is created by MeMS)
Returns: MeMS physical address mapped to the passed ptr (MeMS virtual address).
*/
void *mems_get(void* v_ptr){
    
    struct main *iterator=head;
    if(iterator->next!=NULL){
        iterator=iterator->next;
    }else{
        printf("Does not exist");
    }
    while(iterator!=NULL){
        if(((unsigned long)v_ptr >= (unsigned long)(iterator->mems_virtual_address)) && ((unsigned long)v_ptr < (unsigned long)(iterator->next->mems_virtual_address))){
            
            void * ptr=(void *)v_ptr+iterator->offset;
        
            
            return ptr;
        }else{
            iterator=iterator->next;
        }
    }
    if(iterator==NULL){
        printf("Virtual Memory does not exist");
    }
    
}


/*
this function free up the memory pointed by our virtual_address and add it to the free list
Parameter: MeMS Virtual address (that is created by MeMS) 
Returns: nothing
*/
void mems_free(void *v_ptr){
    
    struct main *iterator=head;
    if(iterator->next!=NULL){
        iterator=iterator->next;
    }else{
        printf("Does not exist");
    }
    while(iterator!=NULL){
        if((unsigned long)v_ptr>=(unsigned long)iterator->mems_virtual_address && (unsigned long)v_ptr<(unsigned long)iterator->next->mems_virtual_address){
            void *mems_virtual_memory=(void *)iterator->mems_virtual_address;
            

            struct sub *sub_iterator=iterator->right;
            while(sub_iterator!=NULL){
                if(v_ptr>=mems_virtual_memory && v_ptr<mems_virtual_memory+sub_iterator->size){
                    sub_iterator->type="HOLE";
                    if(sub_iterator->next!=NULL && sub_iterator->next->type=="HOLE"){
                        sub_iterator->size+=sub_iterator->next->size;
                        if(sub_iterator->next->next!=NULL){
                            sub_iterator->next=sub_iterator->next->next;
                        }else{
                            sub_iterator->next=NULL;
                        }
                    }
                    if(sub_iterator->prev!=NULL && sub_iterator->prev->type=="HOLE"){
                        sub_iterator->size+=sub_iterator->prev->size;
                        if(sub_iterator->prev->prev!=NULL){
                            sub_iterator->prev=sub_iterator->prev->prev;
                        }else{
                            sub_iterator->prev=NULL;
                        }
                    }
                    return;
                }else{
                    mems_virtual_memory+=sub_iterator->size;
                    sub_iterator=sub_iterator->next;
                }
            }
        }else{
            iterator=iterator->next;
        }
    }
    if(iterator==NULL){
        printf("Virtual Memory does not exist");
    }
    
}