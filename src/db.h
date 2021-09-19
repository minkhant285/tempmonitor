#include <sqlite3.h>
#include <Arduino.h>

void dbInit();
int db_open(const char *filename, sqlite3 **db);
static int callback(void *data, int argc, char **argv, char **azColName);
int db_exec(sqlite3 *db, const char *sql);
void insert(String date, String tempC, String tempF);
String select();
String selectDateRange(String d1, String d2);
void deleteData();
