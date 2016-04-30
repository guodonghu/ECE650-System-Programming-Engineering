#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include "potato.h"


void make_fifos(char *fifos_name[], int index) {
  if ((mkfifo(fifos_name[index], FILE_MODE) < 0) && (errno != EEXIST)) {
    perror("mkfifo() error");
    exit(1);
  }
}

void build_left_fifos(char *fifos_name[], int player_id, int total_players) {
  if (player_id == 0) {
    int left_neighbor = total_players - 1;
    sprintf(fifos_name[2], "/tmp/p%d_p%d", left_neighbor, player_id);
    sprintf(fifos_name[3], "/tmp/p%d_p%d", player_id, left_neighbor);
  }
  else {
    int left_neighbor = player_id - 1;
    sprintf(fifos_name[2], "/tmp/p%d_p%d", left_neighbor, player_id);
    sprintf(fifos_name[3], "/tmp/p%d_p%d", player_id, left_neighbor);
  }
  make_fifos(fifos_name, 2);
  make_fifos(fifos_name, 3);
}

void build_right_fifos(char *fifos_name[], int player_id, int total_players) {
  if (player_id == total_players - 1) {
    int right_neighbor = 0;
    sprintf(fifos_name[4], "/tmp/p%d_p%d", right_neighbor, player_id);
    sprintf(fifos_name[5], "/tmp/p%d_p%d", player_id, right_neighbor);
  }
  else {
    int right_neighbor = player_id + 1;
    sprintf(fifos_name[4], "/tmp/p%d_p%d", right_neighbor, player_id);
    sprintf(fifos_name[5], "/tmp/p%d_p%d", player_id, right_neighbor);
  }
  make_fifos(fifos_name, 4);
  make_fifos(fifos_name, 5);
}



int main(int argc, char *argv[]) {
  //check command line argument
  if (argc != 2) {
    printf("wrong input, should be: ./player <player_id>\n");
    return EXIT_FAILURE;
  }
  int player_id = atoi(argv[1]);
  //printf("player:%d\n", player_id);
  //initilize the game
  int total_players;
  int fifos[6];
  char *fifos_name[6];
  int m;
  for (m = 0; m < 6; m++) {
    fifos_name[m] = malloc(20 * sizeof(char));
  }
  POTATO_T potato;
  sprintf(fifos_name[0], "/tmp/master_p%d", player_id);     //make fifos with master: 0 from, 1 to master
  make_fifos(fifos_name, 0);
  fifos[0] = open(fifos_name[0], O_RDWR);
  if (fifos[0] < 0) {
    printf("player open1 %s failed\n", fifos_name[0]);
    perror("player open1");
    return EXIT_FAILURE;
  }
  sprintf(fifos_name[1], "/tmp/p%d_master", player_id);
  make_fifos(fifos_name, 1);
  fifos[1] = open(fifos_name[1], O_RDWR);
  if (fifos[1] < 0) {
    printf("open1 %s failed\n", fifos_name[1]);
    perror("open1");
    return EXIT_FAILURE;
  }
  //give init message to ringmaster
  potato.msg_type = PLAYER_INIT;
  potato.hop_trace[0] = player_id;
  write(fifos[1], &potato, sizeof(struct potato));
  //read from ring master
  for (;;) {
    fd_set fds;
    int fd = fifos[0];
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    select(fd + 1, &fds, NULL, NULL, NULL);
    if (FD_ISSET(fd, &fds)) {
      read(fifos[0], &potato, sizeof(struct potato));
      if (potato.msg_type == RING_MASTER_INIT) {
        total_players = potato.total_players;
        if (player_id >= total_players) {
          printf("player_id should be less than %d\n", total_players);
          return EXIT_FAILURE;
        }
        printf("Connected as player %d out of %d total players\n", player_id, total_players);
        close(fifos[0]);
        close(fifos[1]);
        //build fifos with left neighbor
        build_left_fifos(fifos_name, player_id, total_players);
        //build fifos with right neighbor
        build_right_fifos(fifos_name, player_id, total_players);
        goto open;
      }
    } 
  }
  open: ;
  /* int x;
  for (x = 0; x < 6; x++) {
    printf("%s\n", fifos_name[x]);
    }*/
  int i;
  for (i = 0; i < 3; i++) {
    fifos[2 * i] = open(fifos_name[2 * i], O_RDWR);
    if (fifos[2 * i] < 0) {
      printf("open2 %s failed\n", fifos_name[2 * i]);
      perror("open2");
      return EXIT_FAILURE;
    }
    //printf("player %d try to open %s\n", player_id, fifos_name[2 * i]);
    fifos[2 * i + 1] = open(fifos_name[2 * i + 1], O_RDWR);
    if (fifos[2 * i + 1] < 0) {
      printf("open3 %s failed\n", fifos_name[2 * i + 1]);
      perror("open3");
      return EXIT_FAILURE;
    }
    //printf("player %d try to open %s\n", player_id, fifos_name[2 * i + 1]);
  }
  //printf("open finish\n");
  //receive potato
  for (;;) {
    int k;
    for(k = 0; k < 3; k++) {
      fd_set fds;
      int fd = fifos[2 * k];
      FD_ZERO(&fds);
      FD_SET(fd, &fds);
      struct timeval tv;
      tv.tv_sec = 0.1;
      tv.tv_usec = 0;
      select(fd + 1, &fds, NULL, NULL, &tv);
      POTATO_T hot_potato;
      if (FD_ISSET(fd, &fds)) {
        read(fd, &hot_potato, sizeof(struct potato));
        if (hot_potato.msg_type == GAME_STARTED) {
          //printf("player %d receive potato\n", player_id);
          --hot_potato.total_hops;
          if (hot_potato.total_hops < 0) {
            write(fifos[1], &hot_potato, sizeof(struct potato));
          }
          else {
            hot_potato.hop_trace[hot_potato.hop_count++] = player_id;
            //hot_potato.hop_count++;
            if (hot_potato.total_hops == 0) {
              printf("I'm it\n"); 
              write(fifos[1], &hot_potato, sizeof(struct potato));
            }
            if (hot_potato.total_hops > 0) {
              srand(hot_potato.total_hops);
              int random = rand() % 2;
              //printf("random is %d\n", random);
              if (random == 1) {                        //send to left neighbor
                if (player_id == 0) {
                  printf("Sending potato to %d\n", total_players -1);
                }
                else {
                  printf("Sending potato to %d\n", player_id - 1);
                }
                write(fifos[3], &hot_potato, sizeof(struct potato));
              }
              else {                                    //send to right neighbor
                if (player_id == total_players - 1) {
                  printf("Sending potato to %d\n", 0);
                }
                else {
                  printf("Sending potato to %d\n", player_id + 1);
                }
                write(fifos[5], &hot_potato, sizeof(struct potato));
              }
              //printf("send potato finish\n");
            }
          }
        }
        if (hot_potato.msg_type == GAME_OVER) {
          goto over;
        }
      }
    }
  }
  over:;
  int j;
  //printf("after open\n");
  //close(fifos[0]);
  //close(fifos[1]);
  for(j = 0 ; j < 6; j++) {
    //printf("players fifos: %s\n", fifos_name[j]);
    close(fifos[j]); 
    unlink(fifos_name[j]);
    free(fifos_name[j]);
  }
  //printf("player %d exit\n", player_id);
  return EXIT_SUCCESS;
}
