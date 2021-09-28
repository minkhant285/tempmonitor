#include <db.h>
#include <Arduino.h>
#include <typeinfo>
#include <SPIFFS.h>

const char *data = "Callback function called";
char *zErrMsg = 0;
sqlite3 *db1;
int rc;
sqlite3_stmt *res;
int rec_count = 0;
const char *tail;

char resultData[10000];

void dbInit()
{

    sqlite3_initialize();

    if (db_open("/spiffs/archer.db", &db1))
    {
        return;
    }

    int rc = db_exec(db1, "CREATE TABLE IF NOT EXISTS tempmonitor (tid INTEGER PRIMARY KEY, date TEXT, tempF REAL, tempC REAL)");
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db1);
        Serial.println("fail to create table");
    }
}

static int callback(void *data, int argc, char **argv, char **azColName)
{

    return 0;
}

int db_open(const char *filename, sqlite3 **db)
{
    int rc = sqlite3_open(filename, db);
    if (rc)
    {
        Serial.printf("Can't open database: %s\n", sqlite3_errmsg(*db));
        return rc;
    }
    else
        return rc;
}

int db_exec(sqlite3 *db, const char *sql)
{

    // long start = micros();
    int rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        Serial.printf("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    // Serial.print(F("Time taken:"));
    // Serial.println(micros() - start);
    return rc;
}

void insert(String date, String tempC, String tempF)
{
    String insertText = "INSERT INTO tempmonitor (date,tempF,tempC) VALUES (\'";
    insertText += date.c_str();
    insertText += "\',";
    insertText += tempC.c_str();
    insertText += ",";
    insertText += tempF.c_str();
    insertText += ")";
    int rc = db_exec(db1, insertText.c_str());
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db1);
        Serial.println("fail to insertdata to db");
    }
}

void deleteData()
{

    int rc = db_exec(db1, "DELETE FROM tempmonitor");
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db1);
        Serial.println("fail to delete to db");
    }
}

String select()
{

    rc = sqlite3_prepare_v2(db1, "SELECT * FROM tempmonitor limit 100 offset 1", 1000, &res, &tail);
    char *result = (char *)malloc(20000);
    strcpy(result, "");

    if (rc != SQLITE_OK)
    {
        Serial.printf("fail");
        sqlite3_close(db1);
    }
    else
    {
        rec_count = 0;
        while (sqlite3_step(res) == SQLITE_ROW)
        {

            strcat(result, "{\"tid\" :\"");
            strcat(result, (const char *)sqlite3_column_text(res, 0));
            strcat(result, "\",\"date\":\"");
            strcat(result, (const char *)sqlite3_column_text(res, 1));
            strcat(result, "\",\"tempC\":\"");
            strcat(result, (const char *)sqlite3_column_text(res, 2));
            strcat(result, "\",\"tempF\":\"");
            strcat(result, (const char *)sqlite3_column_text(res, 3));
            strcat(result, "\"},");
            // Serial.printf("id => %s, date=> %s, TempC => %s, TempF => %s \n", (const char *)sqlite3_column_text(res, 0), (const char *)sqlite3_column_text(res, 1), (const char *)sqlite3_column_text(res, 2), (const char *)sqlite3_column_text(res, 3));
            rec_count++;
        }
        sqlite3_finalize(res);
    }
    result[strlen(result) - 1] = '\0';

    delete result;
    return result;
}

String selectDateRange(String d1, String d2)
{
    String selectDrangeText = "SELECT * from tempmonitor where date(date,'unixepoch','localtime') >= \'";
    selectDrangeText += d1.c_str();
    selectDrangeText += "\' AND date(date,'unixepoch','localtime') <= \'";
    selectDrangeText += d2.c_str();
    selectDrangeText += "\'";

    rc = sqlite3_prepare_v2(db1, "SELECT * FROM tempmonitor limit 100 offset 1", 1000, &res, &tail);
    char *result = (char *)malloc(50000);

    if (rc != SQLITE_OK)
    {
        Serial.printf("fail");
        sqlite3_close(db1);
    }
    else
    {
        rec_count = 0;
        while (sqlite3_step(res) == SQLITE_ROW)
        {

            strcat(result, "{\"tid\" :\"");
            strcat(result, (const char *)sqlite3_column_text(res, 0));
            strcat(result, "\",\"date\":\"");
            strcat(result, (const char *)sqlite3_column_text(res, 1));
            strcat(result, "\",\"tempC\":\"");
            strcat(result, (const char *)sqlite3_column_text(res, 2));
            strcat(result, "\",\"tempF\":\"");
            strcat(result, (const char *)sqlite3_column_text(res, 3));
            strcat(result, "\"},");
            // Serial.printf("id => %s, date=> %s, TempC => %s, TempF => %s \n", (const char *)sqlite3_column_text(res, 0), (const char *)sqlite3_column_text(res, 1), (const char *)sqlite3_column_text(res, 2), (const char *)sqlite3_column_text(res, 3));
            rec_count++;
        }
        sqlite3_finalize(res);
    }
    result[strlen(result) - 1] = '\0';

    delete result;
    return result;
}
