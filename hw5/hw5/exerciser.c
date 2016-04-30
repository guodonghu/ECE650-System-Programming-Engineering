#include "exerciser.h"

void exercise(MYSQL *conn)
{
  /*printf("query1.1\n");
  query1(conn, 
         1, 20, 30,
         1, 10, 20,
         0, 0, 0,
         0, 0, 0,
         0, 0, 0,
         0, 0, 0);
  printf("query1.2\n");
  query1(conn, 
         1, 20, 30,
         1, 10, 20,
         1, 10, 20,
         1, 1, 3,
         1, 1, 1.5,
         1, 1, 1.5);
  printf("query1.3\n");
  query1(conn, 
         0, 20, 30,
         0, 10, 20,
         0, 10, 20,
         3, 3, 5,
         0, 1, 1.5,
         1, 1, 1.5);
  
  printf("query2.1 %s\n", "red");
  query2(conn, "Red");
  printf("query2.2 %s\n", "kkk");
  query2(conn, "kkk");
  
  printf("query3.1 %s\n", "UNC");
  query3(conn, "UNC");
  printf("query3.2 %s\n", "kkk");
  query3(conn, "kkk");

  printf("query4.1 %s\n", "NC Red");
  query4(conn, "NC", "Red");
  printf("query4.2 %s\n", "kkk SSS");
  query4(conn, "kkk", "SSS");

  printf("query5.1 %d\n", 12);
  query5(conn, 12);
  printf("query5.2 %d\n", 100);
  query5(conn, 100);
  */
  // printf("query5.3 %d\n", -1);
  //query5(conn, -1);
  add_player(conn, 16, 12, "guodong", "huu", 23, 12, 100, 100, 100, 100);
  add_team(conn, "god", 11, 9, 40, 100);
  add_state(conn, "NY");
  add_color(conn, "BROWN");
  query1(conn, 
         1, 20, 30,
         1, 10, 20,
         0, 0, 0,
         0, 0, 0,
         0, 0, 0,
         0, 0, 0);
  query2(conn, "BROWN");
  query3(conn, "god");
  query4(conn, "NY", "BROWN");
  query5(conn, 12);
}
