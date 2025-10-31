#include "lem_ipc.h"

void place_player(t_ipc *ipc, int team, int team_index){
    int pos_x = 0;
    int pos_y = 0;
    printf("%s %d\n","Choose your pos_X, between 0 and", WEIGHT - 1);
    int result = scanf("%d", &pos_x);
    if (result != 1) {
        while (getchar() != '\n');
        pos_x = -1;
    }
    while (pos_x < 0 || pos_x > WEIGHT){
        printf("%s %d\n","Choose your pos_X, between 0 and", WEIGHT - 1);
        result = scanf("%d", &pos_x);
        if (result != 1) {
            while (getchar() != '\n');
            pos_x = -1;
        }
    }
    printf("%s %d\n","Choose your pos_y, between 0 and", HEIGHT - 1);
    result = scanf("%d", &pos_y);
    if (result != 1) {
        while (getchar() != '\n');
        pos_y = -1;
    }
    while (pos_y < 0 || pos_y > HEIGHT){
        printf("%s %d\n","Choose your pos_Y, between 0 and", HEIGHT - 1);
        result = scanf("%d", &pos_y);
        if (result != 1) {
            while (getchar() != '\n');
            pos_y = -1;
        }
    }
    if (ipc->map[pos_x][pos_y] == 0){
        ipc->map[pos_x][pos_y] = team;
        ipc->teams[team - 1].teams_pos_x[team_index] = pos_x;
        ipc->teams[team - 1].teams_pos_y[team_index] = pos_y;
        data_player.pos_x = pos_x;
        data_player.pos_y = pos_y;
    }
    else{
        printf("%s\n","There is already a player in this case");
        place_player(ipc, team, team_index);
    }
    print_map(ipc);
}

void init_teams(t_ipc *ipc){
    for (size_t i = 0; i < MAX_TEAMS; i++){
        ipc->teams[i].nbr_player_team = 0;
        ipc->teams[i].alive_players = 0;
        key_t key = ftok("/tmp", i + 1);
        ipc->teams[i].id_msg = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
        if (ipc->teams[i].id_msg == -1){
            write(2, "Error msgget\n", 14);
            exit(1);
        }
    }
}

void init_joining_player(t_ipc *ipc){
    int team = 0;
    printf("Choose your team, max nbr of teams : %d\n", MAX_TEAMS);
    scanf("%d", &team);
    while(team > MAX_TEAMS || team <= 0){
        printf("Choose your team, max nbr of teams : %d\n", MAX_TEAMS);
        scanf("%d", &team);
    }
    ipc->teams[team - 1].nbr_player_team++;
    data_player.index_team = ipc->teams[team - 1].nbr_player_team;
    ipc->teams[team - 1].alive_players++;
    place_player(ipc, team, ipc->teams[team - 1].nbr_player_team - 1);
    ipc->nbr_player++;
    data_player.index = ipc->nbr_player;
    data_player.team = team;
}

void init_first_player(t_ipc *ipc){
    int team = 0;
    ipc->nbr_player = 1;
    ipc->playing_player = 1;
    memset(ipc->map, 0, sizeof(int) * HEIGHT * WEIGHT);
    ipc->game_started = false;
    ipc->turn_nbr = 0;
    printf("Choose your team, max nbr of teams : %d\n", MAX_TEAMS);
    scanf("%d", &team);
    while(team > MAX_TEAMS || team <= 0){
        printf("Choose your team, max nbr of teams : %d\n", MAX_TEAMS);
        scanf("%d", &team);
    }
    sem_init(&ipc->sem, 1, 1);
    init_teams(ipc);
    ipc->teams[team - 1].nbr_player_team++;
    data_player.index_team = ipc->teams[team - 1].nbr_player_team;
    ipc->teams[team - 1].alive_players++;
    place_player(ipc, team, ipc->teams[team - 1].nbr_player_team - 1);
    data_player.index = ipc->nbr_player;
    data_player.team = team;
    for (int i = 0; i < MAX_PLAYERS; i++){
        ipc->dead_players[i] = -1;
    }
}
