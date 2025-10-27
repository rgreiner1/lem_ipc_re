#include "lem_ipc.h"


t_data_per_player data_player = {0, 0, 0, 0, 0,{-1, -1 ,-1, -1}};


void print_map(t_ipc *ipc){
for (int i = 0; i < HEIGHT; i++){
    for (int j = 0; j < WEIGHT; j++)
        printf("%d", ipc->map[i][j]);
    printf("\n");
    }
}

int main(){
    int shm_id;
    char buffer[1024];
    t_ipc *shm_ptr;
    key_t key = ftok("/tmp", 'x');
    if (key == -1){
        printf("Error ftok\n");
        exit(1);
    }
    shm_id = shmget(key, SHM_SIZE, IPC_CREAT | IPC_EXCL | 0600);
    if (shm_id == -1){
        if (errno == EEXIST){
            shm_id = shmget(key, SHM_SIZE, 0600);
            if (shm_id == -1){
                printf("Error when linking\n");
                exit(1);
            }
            shm_ptr = (t_ipc*)shmat(shm_id, NULL, 0);
            init_joining_player(shm_ptr);
        }
        else{
            printf("Error shmget\n");
            exit(1);
        }   
    }
    else{
        shm_ptr = (t_ipc*)shmat(shm_id, NULL, 0);
        init_first_player(shm_ptr);
    }
    while(shm_ptr->game_started == false){
        if (data_player.index == 1){
            printf("%s\n", "Use the command 'start' to start the game");
            scanf("%s", buffer);
            while(strcmp(buffer, "start") != 0){
                printf("%s\n", "Use the command 'start' to start the game");
                scanf("%s", buffer);
            }
            shm_ptr->game_started = true;
        }
    }
    do_game(shm_ptr);
    for (int i = 0; i < MAX_TEAMS; i++){
        if (msgctl(shm_ptr->teams[i].id_msg, IPC_RMID, NULL) == -1) {
            perror("msgctl (remove)");
            return 1;
        }
    }
    shmdt(shm_ptr);
    shmctl(shm_id, IPC_RMID, NULL);
}