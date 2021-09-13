#include <db.h>
#include <Arduino.h>
#include <typeinfo>

const char *data = "Callback function called";
char *zErrMsg = 0;
sqlite3 *db1;

String resultData;
static int callback(void *data, int argc, char **argv, char **azColName)
{
    String result;
    int i;
    // Serial.printf("%s: ", (const char *)data);
    for (i = 0; i < argc; i++)
    {
        result += i == 0 ? "{" : "";
        result += "\"";
        result += azColName[i];
        result += "\"";
        result += ":";
        result += "\"";
        result += argv[i];
        result += "\"";
        result += i == sizeof(argc) - 1 ? "}" : ",";
        // Serial.printf("%s = %s  \n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    resultData += result;
    resultData += ",";
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
    {
        Serial.printf("Opened database successfully\n");
    }
    return rc;
}

int db_exec(sqlite3 *db, const char *sql)
{

    // Serial.println(sql);
    long start = micros();
    int rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        Serial.printf("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        Serial.printf("Operation done successfully\n");
    }
    Serial.print(F("Time taken:"));
    Serial.println(micros() - start);
    // Serial.println(rc);
    return rc;
}

void dbInit()
{
    if (db_open("/spiffs/test1.db", &db1))
        return;
    createTable();
}

void createTable()
{
    int rc = db_exec(db1, "CREATE TABLE IF NOT EXISTS tempmonitor (id INTEGER, date VARCHAR, tempF REAL, tempC REAL );");
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db1);
        Serial.println("table creation error");
        return;
    }
}

void insert()
{

    int rc = db_exec(db1, "INSERT INTO tempmonitor VALUES (1, '28-5-1998',98.5,33.4);");
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
    int rc = db_exec(db1, "SELECT * FROM tempmonitor");

    if (rc != SQLITE_OK)
    {
        sqlite3_close(db1);
        Serial.println("fail to select  db");
    }
    String selectData = "[";
    selectData += resultData.substring(0, resultData.length() - 1);
    selectData += "]";
    resultData = "";
    return selectData;
}
