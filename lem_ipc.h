#ifndef LEMIPC_H
#define LEMIPC_H

#define SHM_SIZE 4096
#define HEIGHT 21
#define WEIGHT 21

#define MAX_TEAMS 4
#define MAX_PLAYERS 8

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <semaphore.h>
#include <time.h>
#include <sys/msg.h>


typedef struct t_msg{
    long mtype;
    int pos_x;
    int pos_y;
    int team_target;
    int target_index;
}t_msg;

typedef struct t_teams{
    int nbr_player_team;
    int alive_players;
    int id_msg;
    int teams_pos_x[MAX_PLAYERS];
    int teams_pos_y[MAX_PLAYERS];
}t_teams;

typedef struct t_target{
    int pos_x;
    int pos_y;
    int team_target;
    int target_index;
}t_target;

typedef struct t_ipc{
    sem_t sem;
    bool game_started;
    int nbr_player;
    int playing_player;
    int map[HEIGHT][WEIGHT];
    t_target targets[MAX_TEAMS];
    t_teams teams[MAX_TEAMS];
    int turn_nbr;
}t_ipc;

typedef struct t_data_per_player{
    int index;
    int index_team;
    int team;
    int pos_x;
    int pos_y;
    t_target target;
}t_data_per_player;

extern t_data_per_player data_player;

void init_first_player(t_ipc *ipc);
void init_joining_player(t_ipc *ipc);
void do_game(t_ipc *ipc);
void print_map(t_ipc *ipc);

#endif