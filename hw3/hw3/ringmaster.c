#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/select.h>
#include <fcntl.h>

#include "potato.h"


int main(int argc, char *argv[]) {
  //check command line argument
  if (argc != 3) {
    printf("wrong input, should be: ./ringmaster <number_of_players> <number_of_hops>\n");
    return EXIT_FAILURE;
  }
  int total_players = atoi(argv[1]);
  int total_hops = atoi(argv[2]);
  if (total_players < 3) {
    printf("<number_of_players> should be greater than 2!\n");
    return EXIT_FAILURE;
  }
  if (total_hops < 0 || total_hops > 512) {
    printf("<number_of_hops> should be greater than or equal to 0 and less than or equal to 512!\n");
    return EXIT_FAILURE;
  }
  // initialize the game
  int fifos[2 * total_players];
  char *fifos_name[2 * total_players];
  /* printf("meg_type is %c\n", potato.msg_type);
  printf("HOPS is %d\n", potato.total_hops);
  printf("count is %d\n", potato.hop_count);
  printf("trace[0] is %ld\n", potato.hop_trace[0]);
  printf("players:%d\n", potato.total_players);
  */
  printf("Potato Ringmaster\n");
  printf("Players = %d\n", total_players);
  printf("Hops = %d\n", total_hops);
  int i;
  for (i = 0 ; i < total_players; i++) {
    POTATO_T  potato;
    fifos_name[2 * i] = malloc(20 * sizeof(char));
    fifos_name[2 * i + 1] = malloc(20 * sizeof(char));
    sprintf(fifos_name[2 * i], "/tmp/master_p%d", i);
    sprintf(fifos_name[2 * i + 1], "/tmp/p%d_master", i);
    if ((mkfifo(fifos_name[2 * i], FILE_MODE) < 0) && (errno != EEXIST)) {
      perror("mkfifo() error");
      return EXIT_FAILURE;
    } 
    if ((mkfifo(fifos_name[2 * i + 1], FILE_MODE) < 0) && (errno != EEXIST)) {
      perror("mkfifo() error");
      return EXIT_FAILURE;
    }
    fifos[2 * i] = open(fifos_name[2 * i], O_RDWR);
    if (fifos[2 * i] < 0) {
      printf("open file %s for write error\n", fifos_name[2 * i]);
      return EXIT_FAILURE;
    }
    fifos[2 * i + 1] = open(fifos_name[2 * i + 1], O_RDWR);
    if (fifos[2 * i + 1] < 0) {
      printf("open file for read %s error\n", fifos_name[2 * i + 1]);
      return EXIT_FAILURE;
    }
    for (;;) {
      fd_set fds;
      int fd = fifos[2 * i + 1];
      FD_ZERO(&fds);
      FD_SET(fd, &fds);
      select(fd + 1, &fds, NULL, NULL, NULL);
      if (FD_ISSET(fd, &fds)) {
        read(fd, &potato, sizeof(struct potato));
        if (potato.msg_type == PLAYER_INIT) {
          printf("Player %ld is ready to play\n", potato.hop_trace[0]);
          potato.msg_type = RING_MASTER_INIT;
          potato.total_players = total_players;
          write(fifos[2 * i], &potato, sizeof(struct potato));
          goto next;
        }
      }
    }
    next:;
  } 
  
  
  //send potato to first randomly selected player
  srand((unsigned)time(NULL));
  int random = rand() % total_players;
  if (random >= total_players || random < 0) {
    printf("random %d is not expected\n", random);
  }
  POTATO_T hot_potato = {GAME_STARTED, total_players, total_hops, 0};
  if (total_hops > 0) {
    printf("All players present, sending potato to player %d\n", random);
  }
  write(fifos[2 * random], &hot_potato, sizeof(struct potato));
  //printf("sent\n");
  for (;;) {
    int k;
    for(k = 0; k < total_players; k++) {
      fd_set fds;
      int fd = fifos[2 * k + 1];
      FD_ZERO(&fds);
      FD_SET(fd, &fds);
      struct timeval tv;
      tv.tv_sec =0.1;
      tv.tv_usec = 0;
      select(fd + 1, &fds, NULL, NULL, &tv);
      POTATO_T potato_trace;
      if (FD_ISSET(fd, &fds)) {
        read(fd, &potato_trace, sizeof(struct potato));
        if (potato_trace.total_hops == 0 && potato_trace.msg_type == GAME_STARTED) {
          printf("Trace of potato:\n");
          int m;
          for (m = 0; m < total_hops; m++) {
            printf("%ld,", potato_trace.hop_trace[m]);
          } 
          goto over;
        }
        if (potato_trace.total_hops < 0 && potato_trace.msg_type == GAME_STARTED) {
          goto over;
        }
      }
    }
  }
  over:;
  hot_potato.msg_type = GAME_OVER;
  int l;
  for (l = 0 ; l < total_players; l++) {
    write(fifos[2 * l], &hot_potato, sizeof(struct potato));
  }
  int j;
  for (j = 0; j < total_players; j++) {
    if (close(fifos[2 * j]) != 0){
      perror("close");
    }
    if (close(fifos[2 * j + 1]) != 0) {
      perror("close");
    }
    //unlink(fifos_name[2 * j]);
    //unlink(fifos_name[2 * j + 1]);
    free(fifos_name[2 * j]);
    free(fifos_name[2 * j + 1]);
  }
  //printf("game is over\n");
  return EXIT_SUCCESS;
}

