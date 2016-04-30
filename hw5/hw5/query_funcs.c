#include "query_funcs.h"
#include "string.h"


void exit_with_error(MYSQL *conn)
{
  fprintf(stderr, "%s\n", mysql_error(conn));
  mysql_close(conn);
  exit(1);
}

void print_result(MYSQL *conn) {
  MYSQL_RES *result = mysql_store_result(conn);
  if (result == NULL) {
    exit_with_error(conn);
  }
  int num_fields = mysql_num_fields(result);
  MYSQL_ROW row;
  MYSQL_FIELD *field;
  int counter = 0;
  while (row = mysql_fetch_row(result)) {
    ++counter;
    for (int i=0; i < num_fields; i++) {
      if (i == 0) {
        while (field = mysql_fetch_field(result)) {
          printf("%s ", field->name);
        }
        printf("\n");
      }
      printf("%s ", row[i] ? row[i] : "NULL");
    }
  }
  
  if (!counter) {
    while (field = mysql_fetch_field(result)) {
      printf("%s ", field->name);
    }
    printf("\n");
  }
  else {
    printf("\n");
  }
  //Free result set
  mysql_free_result(result);
}

void add_player(MYSQL *conn, int team_id, int jersey_num, char *first_name, char *last_name,
                int mpg, int ppg, int rpg, int apg, double spg, double bpg)
{
  char query[1000];
  sprintf(query, "INSERT INTO PLAYER (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG) VALUES (%d, %d, \"%s\", \"%s\", %d, %d, %d, %d, %f, %f)", team_id, jersey_num, first_name, last_name, mpg, ppg, rpg, apg, spg, bpg);
  //printf("%s\n", query);
  if (mysql_query(conn, query)) {
    exit_with_error(conn);
  }
}


void add_team(MYSQL *conn, char *name, int state_id, int color_id, int wins, int losses)
{
  char query[1000];
  sprintf(query, "INSERT INTO TEAM (NAME, STATE_ID, COLOR_ID, WINS, LOSSES) VALUES (\"%s\", %d, %d, %d, %d)", name, state_id, color_id, wins, losses);
  //printf("%s\n", query);
  if (mysql_query(conn, query)) {
    exit_with_error(conn);
  }
}


void add_state(MYSQL *conn, char *name)
{
  char query[1000];
  sprintf(query, "INSERT INTO STATE (NAME) VALUES (\"%s\")", name);
  //printf("%s\n", query);
  if (mysql_query(conn, query)) {
    exit_with_error(conn);
  }
}


void add_color(MYSQL *conn, char *name)
{
  char query[1000];
  sprintf(query, "INSERT INTO COLOR (NAME) VALUES (\"%s\")", name);
  //printf("%s\n", query);
  if (mysql_query(conn, query)) {
    exit_with_error(conn);
  }
}


void query1(MYSQL *conn,
	    int use_mpg, int min_mpg, int max_mpg,
            int use_ppg, int min_ppg, int max_ppg,
            int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg,
            int use_spg, double min_spg, double max_spg,
            int use_bpg, double min_bpg, double max_bpg
            )
{
  char query[1000];
  char *sel = "SELECT * FROM PLAYER WHERE";
  strcpy(query, sel);
  int counter = 0;
  if (use_mpg != 0) {
    ++counter;
    if (counter == 1) {
      char condition[50];
      sprintf(condition, " (MPG BETWEEN %d AND %d)", min_mpg, max_mpg);
      strcat(query, condition);
    }
    else {
      char condition[50];
      sprintf(condition, " AND (MPG BETWEEN %d AND %d)", min_mpg, max_mpg);
      strcat(query, condition);
    }
  }
  if (use_ppg != 0) {
    ++counter;
    if (counter == 1) {
      char condition[50];
      sprintf(condition, " (PPG BETWEEN %d AND %d)", min_ppg, max_ppg);
      strcat(query, condition);
    }
    else {
      char condition[50];
      sprintf(condition, " AND (PPG BETWEEN %d AND %d)", min_ppg, max_ppg);
      strcat(query, condition);
    }
  }
  if (use_rpg != 0) {
    ++counter;
    if (counter == 1) {
      char condition[50];
      sprintf(condition, " (RPG BETWEEN %d AND %d)", min_rpg, max_rpg);
      strcat(query, condition);
    }
    else {
      char condition[50];
      sprintf(condition, " AND (RPG BETWEEN %d AND %d)", min_rpg, max_rpg);
      strcat(query, condition);
    }
  }
  if (use_apg != 0) {
    ++counter;
    if (counter == 1) {
      char condition[50];
      sprintf(condition, " (APG BETWEEN %d AND %d)", min_apg, max_apg);
      strcat(query, condition);
    }
    else {
      char condition[50];
      sprintf(condition, " AND (APG BETWEEN %d AND %d)", min_apg, max_apg);
      strcat(query, condition);
    }
  }
  if (use_spg != 0) {
    ++counter;
    if (counter == 1) {
      char condition[50];
      sprintf(condition, " (SPG BETWEEN %f AND %f)", min_spg, max_spg);
      strcat(query, condition);
    }
    else {
      char condition[50];
      sprintf(condition, " AND (SPG BETWEEN %f AND %f)", min_spg, max_spg);
      strcat(query, condition);
    }
  }
  if (use_bpg != 0) {
    ++counter;
    if (counter == 1) {
      char condition[50];
      sprintf(condition, " (BPG BETWEEN %f AND %f)", min_bpg, max_bpg);
      strcat(query, condition);
    }
    else {
      char condition[50];
      sprintf(condition, " AND (BPG BETWEEN %f AND %f)", min_bpg, max_bpg);
      strcat(query, condition);
    }
  }
  //printf("query is:%s\n", query);
  if (mysql_query(conn, query)) {
    exit_with_error(conn);
  }
  //print the result
  print_result(conn);
}


void query2(MYSQL *conn, char *team_color)
{
  char query[1000];
  sprintf(query, "SELECT TEAM.NAME FROM TEAM, COLOR  WHERE (COLOR.NAME = \"%s\") AND (TEAM.COLOR_ID = COLOR.COLOR_ID)", team_color);
  if (mysql_query(conn, query)) {
    exit_with_error(conn);
  }
  print_result(conn);
}


void query3(MYSQL *conn, char *team_name)
{
  char query[1000];
  sprintf(query, "SELECT FIRST_NAME, LAST_NAME FROM PLAYER, TEAM WHERE (PLAYER.TEAM_ID = TEAM.TEAM_ID) AND (TEAM.NAME = \"%s\") ORDER BY PPG DESC;", team_name);
  if (mysql_query(conn, query)) {
    exit_with_error(conn);
  }
  print_result(conn);
}


void query4(MYSQL *conn, char *team_state, char *team_color)
{
  char query[1000];
  sprintf(query, "SELECT FIRST_NAME, LAST_NAME, UNIFORM_NUM FROM PLAYER, TEAM, STATE, COLOR WHERE (PLAYER.TEAM_ID = TEAM.TEAM_ID) AND (TEAM.COLOR_ID = COLOR.COLOR_ID) AND (TEAM.STATE_ID = STATE.STATE_ID) AND (STATE.NAME = \"%s\") AND (COLOR.NAME = \"%s\");", team_state, team_color);
  if (mysql_query(conn, query)) {
    exit_with_error(conn);
  }
  print_result(conn);
  
}


void query5(MYSQL *conn, int num_wins)
{
  char query[1000];
  sprintf(query, "SELECT FIRST_NAME, LAST_NAME, TEAM.NAME, WINS FROM PLAYER, TEAM WHERE (PLAYER.TEAM_ID = TEAM.TEAM_ID) AND (WINS > %d);", num_wins);
  if (mysql_query(conn, query)) {
    exit_with_error(conn);
  }
  print_result(conn);
  
}
