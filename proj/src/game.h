#ifndef __GAME_H
#define __GAME_H

#include "tools.h"

int draw_board();
//int unsub_game();
int init_players(unsigned int num_players);
int draw_player(unsigned int n, state st);
void change_player_state(unsigned int i, unsigned long data);
void update_player(unsigned int num_players);
int draw_handler(unsigned int num_players);
int playgame(unsigned int num_players);
int start_multigame(unsigned int num_players);
int test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) ;

#endif /* GAME_H */
