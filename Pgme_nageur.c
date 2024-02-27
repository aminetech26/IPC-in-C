#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <semaphore.h>
#include <sys/wait.h>

#define NC 2
#define NP 5

struct sembuf Pscab = {0, -1, 0};
struct sembuf Vscab = {0, 1, 0};

struct sembuf Pmutex = {1, -1, 0};
struct sembuf Vmutex = {1, 1, 0};

struct sembuf Pspan = {2, -1, 0};
struct sembuf Vspan = {2, 1, 0};


//les fonction
void Demander_panier(int* Nump);
void Liberer_panier(int i, int* Nump);
void Demander_cabine(void);
void Liberer_cabine(void);


int main(int argc, char *argv[]) {
    
    int Nump;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <i>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Récupérer la valeur de i depuis la ligne de commande
    int i = atoi(argv[1]);
    printf("\nValeur de i : %d\n", i);

    Demander_panier(&Nump);

    Demander_cabine();
    sleep(4);

    Liberer_cabine();
    sleep(7);

    printf("\nJe suis le nageur numero %d, j'occupe le panier %d\n",i,Nump);

    Demander_cabine();
    sleep(4);

    Liberer_cabine();
    Liberer_panier(i,&Nump);

    printf("\nJe suis le nageur numero %d, j'ai liberer un panier, il reste %d paniers libres\n",i,NP-Nump);

    return 0;


}




void Demander_panier(int* Nump){
    
    int sem, memp;

    // CREATION DES SEMAPHORES
    key_t clef; 


    /*Récuperation des sémaphores */
    clef = ftok("TP.c", 1);   
    sem=semget(clef, 3, IPC_CREAT|0666);
    if(sem==-1) {printf("\n Erreur de récuperation des sémaphores\n"); exit(1);}

    // Récupération DU SEGMENT MEMOIRE
    clef = ftok("TP.c", 2); 

    memp=shmget(clef,2*sizeof(int),IPC_CREAT|0666);
    if(memp==-1) {printf("\n Erreur de récupération de la mémoire partagée\n"); exit(2);}
    
    /* Attachement */
    int *var_partages= shmat(memp,0,0);

    
    //Demander panier
    semop(sem,&Pmutex,1);
    var_partages[0]++;
    if (var_partages[1] == NP )
    {
        semop(sem,&Vmutex,1);
        semop(sem,&Pspan,1);
    }
    var_partages[0]--;
    var_partages[1]++;

    *Nump= var_partages[1];
    semop(sem,&Vmutex,1);

    /* Détachement des segements */
    shmdt(var_partages);


}


void Liberer_panier(int i, int* Nump)
{

        
    int sem, memp;

    // CREATION DES SEMAPHORES
    key_t clef; 


    /*Récuperation des sémaphores */
    clef = ftok("TP.c", 1);   
    sem=semget(clef, 3, IPC_CREAT|0666);
    if(sem==-1) {printf("\n Erreur de récuperation des sémaphores"); exit(1);}

    // Récupération DU SEGMENT MEMOIRE
    clef = ftok("TP.c", 2); 

    memp=shmget(clef,2*sizeof(int),IPC_CREAT|0666);
    if(memp==-1) {printf("\n Erreur de récupération de la mémoire partagée"); exit(2);}
    
    /* Attachement */
    int *var_partages= shmat(memp,0,0);

    
    //Libérer Panier
    semop(sem,&Pmutex,1);
    var_partages[1]--;
    *Nump = var_partages[1];
    if (var_partages[0] >0)
    {
        printf("\nNageur %d va liberer un panier et il y a %d demandes en attene\n",i,var_partages[0]);
        semop(sem,&Vspan,1);
    }
    else{
        semop(sem,&Vmutex,1);
    }
    
    /* Détachement des segements */
    shmdt(var_partages);

}



void Demander_cabine(void)
{

        
    int sem, memp;

    // CREATION DES SEMAPHORES
    key_t clef; 


    /*Récuperation des sémaphores */
    clef = ftok("TP.c", 1);   
    sem=semget(clef, 3, IPC_CREAT|0666);
    if(sem==-1) {printf("\n Erreur de récuperation des sémaphores"); exit(1);}

    
    //demander Cabine
    semop(sem,&Pscab,1);
    

}


void Liberer_cabine(void)
{

        
    int sem, memp;

    // CREATION DES SEMAPHORES
    key_t clef; 

    /*Récuperation des sémaphores */
    clef = ftok("TP.c", 1);   
    sem=semget(clef, 3, IPC_CREAT|0666);
    if(sem==-1) {printf("\n Erreur de récuperation des sémaphores"); exit(1);}

    
    //demander Cabine
    semop(sem,&Vscab,1);
    

}