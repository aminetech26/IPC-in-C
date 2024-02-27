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

int sem, memp;

struct sembuf Pcpplein = {0, -1, 0};
struct sembuf Vcpplein = {0, 1, 0};
struct sembuf Pcpvide = {1, -1, 0};
struct sembuf Vcpvide = {1, 1, 0};

int main()
{

    int p;
    // matensach initialisation var partagé

    // CREATION DES SEMAPHORES
    key_t clef;

    /* Création des sémaphores */

    clef = ftok("TP.c", 1);
    sem = semget(clef, 3, IPC_CREAT | 0666);
    if (sem == -1)
    {
        printf("\n Erreur de creation des sémaphores");
        exit(1);
    }

    /* Initialisation des sémaphores */

    semctl(sem, 0, SETVAL, NC); // scab
    semctl(sem, 1, SETVAL, 1);  // mutex
    semctl(sem, 2, SETVAL, 0);  // span

    // CREATION DU SEGMENT MEMOIRE
    clef = ftok("TP.c", 2);

    memp = shmget(clef, 2 * sizeof(int), IPC_CREAT | 0666);
    if (memp == -1)
    {
        printf("\n Erreur de creation de la mémoire partagée");
        exit(2);
    }

    /* Attachement */
    int *var_partages = shmat(memp, 0, 0);

    /* Initialisation des variables */

    var_partages[0] = 0; // ndp
    var_partages[1] = 0; // npo

    /* Détachement des segements */
    shmdt(var_partages);

    // CREATION DES PROCESSUS

    for (int i = 0; i < 10; i++)
    {
        p = fork();
        if (p == -1)
        {
            printf("\n Erreur de creation du processus");
            exit(1);
        }

        if (p == 0)
        { // printf("Execution process producteur");
            char arg[10];
            sprintf(arg, "%d", i);

            // Execute the child process
            int err = execlp("./Pgme_nageur", "Pgme_nageur", arg, NULL);
            printf("Error while executing the command, please try again %d \n", err >> 8);
            exit(2);
        }
    }

    // ATTANDRE LA FIN DE TOUT LES FILS
    while (wait(NULL) != -1)
        ;

    // Destrcution des sémaphores et des segments partagés
    semctl(sem, IPC_RMID, 0);
    shmctl(memp, IPC_RMID, 0);

    sleep(2);
    printf("\n Fin du processus\n");

    return 0;
}