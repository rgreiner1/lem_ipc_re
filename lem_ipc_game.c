#include "lem_ipc.h"


bool next_to_target(){
    int dx[] = {-1, -1,  0, 1, 1, 1,  0, -1};
    int dy[] = { 0,  1,  1, 1, 0,-1, -1, -1};

    for (int i = 0; i < 8; i++){
        int pos_next_x = data_player.target.pos_x + dx[i];
        int pos_next_y = data_player.target.pos_y + dy[i];
        if (pos_next_x < 0 || pos_next_x >= WEIGHT || pos_next_y < 0 || pos_next_y >= HEIGHT)
            continue;
        if (data_player.pos_x == pos_next_x && data_player.pos_y == pos_next_y){
            write(1, "Next to target\n", 16);
            return true;
        }
    }
    return false;
}

void find__new_target(t_ipc *ipc, int team){
    size_t distance = 9999;
    size_t tmp_distance;
    int tmp_team;
    int tmp_player_index;
    for (int team_index = 0; team_index < MAX_TEAMS; team_index++){
        if (team - 1 == team_index){
            continue;
        }
        for (int player_index = 0; player_index < ipc->teams[team_index].nbr_player_team; player_index++){
            if (ipc->teams[team_index].teams_pos_x[player_index] == -1)
                continue;
            tmp_distance = abs(data_player.pos_x - ipc->teams[team_index].teams_pos_x[player_index]) 
                + abs(data_player.pos_y - ipc->teams[team_index].teams_pos_y[player_index]);
            if (tmp_distance < distance){
                distance = tmp_distance;
                tmp_team = team_index;
                tmp_player_index = player_index;
            }
        }
    }
    t_msg msg;
    msg.pos_x = ipc->teams[tmp_team].teams_pos_x[tmp_player_index];
    msg.pos_y = ipc->teams[tmp_team].teams_pos_y[tmp_player_index];
    msg.target_index = tmp_player_index;
    msg.team_target = tmp_team;
    msg.target_died = false;
    printf("pos_X : %d | pos_Y : %d | target_index %d | target %d\n", msg.pos_x, msg.pos_y, msg.target_index, msg.team_target);
    printf("msgsnd ret : %d\n", msgsnd(ipc->teams[team - 1].id_msg, &msg, sizeof(t_msg), 0));
    printf("msgsnd : %s\n", strerror(errno));
    data_player.target.pos_x = msg.pos_x;
    data_player.target.pos_y =  msg.pos_y;
    data_player.target.target_index = 1;
    data_player.target.team_target = tmp_team;
}


void update_target(t_ipc *ipc, int team){
    if (ipc->teams[data_player.target.team_target].teams_pos_x[data_player.target.target_index] == -1)
        find__new_target(ipc, team);
    t_msg msg;
    msg.pos_x = ipc->teams[data_player.target.team_target].teams_pos_x[data_player.target.target_index];
    msg.pos_y = ipc->teams[data_player.target.team_target].teams_pos_y[data_player.target.target_index];
    msg.target_index = data_player.target.target_index;
    msg.team_target = data_player.target.team_target;
    msg.target_died = false;
    msgsnd(ipc->teams[team - 1].id_msg, &msg, sizeof(t_msg), 0);
    data_player.target.pos_x = msg.pos_x;
    data_player.target.pos_y =  msg.pos_y;
}

bool game_over(t_ipc *ipc){
    int team_alive = 0;
    for (int i = 0; i < MAX_TEAMS; i++){
        if (ipc->teams[i].alive_players > 0)
            team_alive++;
    }
    if (team_alive == 1)
        return true;
    return false;
}

void add_to_dead_players(t_ipc *ipc){
    int i = 0;
    while (ipc->dead_players[i])
        i++;
    ipc->dead_players[i] = data_player.index;

}

void do_move(t_ipc *ipc, int team){
    if (data_player.target.target_index == -1)
        find__new_target(ipc, team);
    else
        update_target(ipc, team);
    if (next_to_target() == true)
        return ;
    int dx[] = {-1, -1,  0, 1, 1, 1,  0, -1};
    int dy[] = { 0,  1,  1, 1, 0,-1, -1, -1};
    int best_target_x = -1;
    int best_target_y = -1;
    int best_distance = 9999;
    int tmp_distance;

    for (int i = 0; i < 8; i++){
        int target_x = data_player.target.pos_x + dx[i];
        int target_y = data_player.target.pos_x + dy[i];
        if (target_x < 0 || target_x >= WEIGHT || target_y < 0 || target_y >= HEIGHT)
            continue;
        if (ipc->map[target_x][target_y] == 0){
            tmp_distance = abs( target_x - data_player.pos_x) + abs(target_y - data_player.pos_y);
            if (tmp_distance < best_distance){
                best_distance = tmp_distance;
                best_target_x = target_x;
                best_target_y = target_y;
            }
        }
    }
    if (best_target_x != -1 && best_target_y != -1){
        srand(time(NULL));
        int rand_nbr = rand();
        if (rand_nbr % 2 == 0){
            if (data_player.pos_x < best_target_x){
                if (ipc->map[data_player.pos_x + 1][data_player.pos_y] == 0){
                    ipc->map[data_player.pos_x][data_player.pos_y] = 0;
                    ipc->map[data_player.pos_x + 1][data_player.pos_y] = team;
                    data_player.pos_x++;
                    ipc->teams[team - 1].teams_pos_x[data_player.index_team - 1]++;
                    printf("-------------------------\n");
                    print_map(ipc);
                    return;
                }
            }
            if (data_player.pos_x > best_target_x){
                if (ipc->map[data_player.pos_x - 1][data_player.pos_y] == 0){
                    ipc->map[data_player.pos_x][data_player.pos_y] = 0;
                    ipc->map[data_player.pos_x - 1][data_player.pos_y] = team;
                    data_player.pos_x--;
                    ipc->teams[team - 1].teams_pos_x[data_player.index_team - 1]--;
                    printf("-------------------------\n");
                    print_map(ipc);
                    return;
                }
            }
        }
        if (data_player.pos_y < best_target_y){
            if (ipc->map[data_player.pos_x][data_player.pos_y + 1] == 0 ){
                ipc->map[data_player.pos_x][data_player.pos_y] = 0;
                ipc->map[data_player.pos_x][data_player.pos_y + 1] = team;
                ipc->teams[team - 1].teams_pos_y[data_player.index_team - 1]++;
                data_player.pos_y++;
                printf("-------------------------\n");
                print_map(ipc);
                return;
            }
        }
        if (data_player.pos_y > best_target_y){
            if (ipc->map[data_player.pos_x - 1][data_player.pos_y] == 0){
                ipc->map[data_player.pos_x][data_player.pos_y] = 0;
                ipc->map[data_player.pos_x][data_player.pos_y - 1] = team;
                ipc->teams[team - 1].teams_pos_y[data_player.index_team - 1]--;
                data_player.pos_y--;
                printf("-------------------------\n");
                print_map(ipc);
                return;
            }
        }
    }
}

int find_killer_team(t_ipc *ipc, int dead_x, int dead_y, int dead_team)
{
    int dx[] = {-1, -1,  0, 1, 1, 1,  0, -1};
    int dy[] = { 0,  1,  1, 1, 0,-1, -1, -1};
    int team_adj_count[MAX_TEAMS + 1] = {0};

    for (int i = 0; i < 8; i++) {
        int x = dead_x + dx[i];
        int y = dead_y + dy[i];
        if (x >= 0 && x < WEIGHT && y >= 0 && y < HEIGHT) {
            int tile_team = ipc->map[x][y];
            if (tile_team != 0 && tile_team != dead_team) {
                team_adj_count[tile_team]++;
            }
        }
    }
    for (int team = 1; team <= MAX_TEAMS; team++) {
        if (team_adj_count[team] >= 2) {
            return team;
        }
    }
    return -1;
}

void check_kill(t_ipc *ipc, int team){
    size_t suronding_players = 0;

    int dx[] = {-1, -1,  0, 1, 1, 1,  0, -1};
    int dy[] = { 0,  1,  1, 1, 0,-1, -1, -1};

    for (int i = 0; i <= 8; i++){
        int x_to_check = data_player.pos_x + dx[i];
        int y_to_check = data_player.pos_y + dy[i];

        if (x_to_check >= 0 && x_to_check < WEIGHT && y_to_check >= 0 && y_to_check < HEIGHT){
            if (ipc->map[x_to_check][y_to_check] != team && ipc->map[x_to_check][y_to_check] != 0)
                suronding_players++;
        }
    }
    if (suronding_players >= 2){
        for (int i = 0; i < MAX_PLAYERS; i++){
            if (ipc->teams[team - 1].teams_pos_x[i] == data_player.pos_x &&
                ipc->teams[team - 1].teams_pos_y[i] == data_player.pos_y){
                    ipc->teams[team - 1].teams_pos_x[i] = -1;
                    ipc->teams[team - 1].teams_pos_y[i] = -1;
            }
        }
        ipc->map[data_player.pos_x][data_player.pos_y] = 0;
        ipc->teams[team - 1].alive_players--;
        t_msg msg;
        msg.target_died = true;
        msg.killer_team = find_killer_team(ipc,data_player.pos_x ,data_player.pos_y, team);
        msg.team_target = team;
        int killer_team = msg.killer_team - 1;
        msgsnd(ipc->teams[killer_team].id_msg, &msg, sizeof(t_msg), 0);
        int i = 0;
        while (ipc->dead_players[i] != -1)
            i++;
        ipc->dead_players[i] = data_player.index;
        sem_post(&ipc->sem);
        exit(0);
    }
}

void check_target(t_ipc *ipc, t_msg *msg_recv){
    printf("Target (team %d) is dead! Finding new one...\n", msg_recv->team_target);
    find__new_target(ipc, data_player.team);
}

void do_game(t_ipc *ipc){
    t_msg *msg_recv = malloc(sizeof(t_msg));
    while(game_over(ipc) == false){
        if (ipc->playing_player == data_player.index){
            sem_wait(&ipc->sem);
            ipc->turn_nbr++;
            printf("turn nbr : %d\n", ipc->turn_nbr);
            check_kill(ipc, data_player.team);
            do_move(ipc, data_player.team);
            ipc->playing_player++;
            sleep(1);
            sem_post(&ipc->sem);
        }
        else{
            sem_wait(&ipc->sem);
            int ret = msgrcv(ipc->teams[data_player.team - 1].id_msg, msg_recv, sizeof(t_msg), 0, IPC_NOWAIT);
            if (ret != -1){
                printf("MSG : %d | %d | %d | %d\n", msg_recv->pos_x, msg_recv->pos_y, msg_recv->target_index, msg_recv->team_target);
                if (msg_recv->target_died == true)
                    check_target(ipc,  msg_recv);
                else{
                    data_player.target.pos_x = msg_recv->pos_x;
                    data_player.target.pos_y = msg_recv->pos_y;
                    data_player.target.target_index = msg_recv->target_index;
                    data_player.target.team_target = msg_recv->team_target;
                }
            }
            for (int i = 0; i < MAX_PLAYERS && ipc->dead_players[i] != -1; i++){
                if (ipc->dead_players[i] == ipc->playing_player)
                    ipc->playing_player++;
            }
            sem_post(&ipc->sem);
        }
        if (ipc->playing_player > ipc->nbr_player){
            sem_wait(&ipc->sem);
            ipc->playing_player = 1;
            sem_post(&ipc->sem);
        }
    }
}