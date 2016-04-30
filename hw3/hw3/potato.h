#define BUFF_LEN   512
#define MAX_POTATO 512
#define RING_MASTER_INIT 'R'
#define PLAYER_INIT 'P'
#define GAME_OVER 'O'
#define GAME_STARTED 'S'
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)


typedef struct potato {
  char msg_type;
  int total_players;

  int total_hops;
  int hop_count;
  
  unsigned long hop_trace[MAX_POTATO];
} POTATO_T;


void clear_string( char *s ) {
  int i;
  for ( i=0; i < BUFF_LEN; i++ ) {
    s[i] = '\0';
  }
}
