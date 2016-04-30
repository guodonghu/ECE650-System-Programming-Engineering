#include <stdlib.h>
#include <stdio.h>
#include "exerciser.h"



int main (int argc, char *argv[]) 
{
  //Allocate & initialize a MYSQL object
  MYSQL *conn = mysql_init(NULL); 

  if (conn == NULL) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    return 1;
  }

  mysql_options(conn, MYSQL_OPT_LOCAL_INFILE, 0);
  //Establish a connection to the database
  //Parameters: connection handler, host name, user name, password,
  //            database name, port numbrer, unix socket, client flag
  if (mysql_real_connect(conn, "localhost", "myuser", "passw0rd", "ACC_BBALL", 0, NULL, 0) == NULL) {
    exit_with_error(conn);
  }


  //TODO: create PLAYER, TEAM, STATE, and COLOR tables in the ACC_BBALL database
  //      load each table with rows from the provided source txt files
  //CREATE PLAYER
 if (mysql_query(conn, "DROP TABLE IF EXISTS PLAYER")) {
    exit_with_error(conn);
  }

  if (mysql_query(conn, "CREATE TABLE PLAYER(PLAYER_ID INT auto_increment, TEAM_ID INT, UNIFORM_NUM  INT, FIRST_NAME TEXT, LAST_NAME TEXT, MPG INT, PPG INT, RPG INT, APG INT, SPG FLOAT, BPG FLOAT, PRIMARY KEY(PLAYER_ID))")) {
    exit_with_error(conn);
  }
  
  if (mysql_query(conn, "LOAD DATA LOCAL INFILE './player.txt' INTO TABLE PLAYER COLUMNS TERMINATED BY ' '")) {
    exit_with_error(conn);
  }
  //CREATE TEAM
  if (mysql_query(conn, "DROP TABLE IF EXISTS TEAM")) {
    exit_with_error(conn);
  }

  if (mysql_query(conn, "CREATE TABLE TEAM(TEAM_ID INT auto_increment, NAME TEXT, STATE_ID INT, COLOR_ID INT, WINS INT, LOSSES INT, PRIMARY KEY(TEAM_ID))")) {
    exit_with_error(conn);
  }
  
  if (mysql_query(conn, "LOAD DATA LOCAL INFILE './team.txt' INTO TABLE TEAM COLUMNS TERMINATED BY ' '")) {
    exit_with_error(conn);
  }
  //CERATE STATE
  if (mysql_query(conn, "DROP TABLE IF EXISTS STATE")) {
    exit_with_error(conn);
  }

  if (mysql_query(conn, "CREATE TABLE STATE(STATE_ID INT auto_increment, NAME TEXT, PRIMARY KEY(STATE_ID))")) {
    exit_with_error(conn);
  }
  
  if (mysql_query(conn, "LOAD DATA LOCAL INFILE './state.txt' INTO TABLE STATE COLUMNS TERMINATED BY ' '")) {
    exit_with_error(conn);
  }
  //CREATE COLOR
  if (mysql_query(conn, "DROP TABLE IF EXISTS COLOR")) {
    exit_with_error(conn);
  }

  if (mysql_query(conn, "CREATE TABLE COLOR(COLOR_ID INT auto_increment, NAME TEXT, PRIMARY KEY(COLOR_ID))")) {
    exit_with_error(conn);
  }
  
  if (mysql_query(conn, "LOAD DATA LOCAL INFILE './color.txt' INTO TABLE COLOR COLUMNS TERMINATED BY ' '")) {
    exit_with_error(conn);
  }
  

  exercise(conn);


  //Close database connection
  mysql_close(conn);

  return 0;
}


