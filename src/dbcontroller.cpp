#include <db.h>
#include <Arduino.h>
#include <typeinfo>
#include <SPIFFS.h>

const char *data = "Callback function called";
char *zErrMsg = 0;
sqlite3 *db1;

String resultData;

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

void insert(String tempC, String tempF)
{
    String insertText = "INSERT INTO tempmonitor (date,tempF,tempC) VALUES (";
    insertText += "1631781339";
    insertText += ",";
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

String selectDateRange(String d1, String d2)
{
    String selectDrangeText = "SELECT * from tempmonitor where date(date,'unixepoch','localtime') >= \'";
    selectDrangeText += d1.c_str();
    selectDrangeText += "\' AND date(date,'unixepoch','localtime') <= \'";
    selectDrangeText += d2.c_str();
    selectDrangeText += "\'";

    int rc = db_exec(db1, selectDrangeText.c_str());

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
