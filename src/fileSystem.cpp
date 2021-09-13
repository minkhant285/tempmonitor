
#include <SPIFFS.h>
#include <fscheck.h>

String fsCheck()
{
    unsigned int totalBytes = SPIFFS.totalBytes();
    unsigned int usedBytes = SPIFFS.usedBytes();
    Serial.println("===== File system info =====");

    Serial.print("Total space:      ");
    Serial.print(totalBytes);
    Serial.println("byte");

    Serial.print("Total space used: ");
    Serial.print(usedBytes);
    Serial.println("byte");

    Serial.println();

    // Open dir folder
    File dir = SPIFFS.open("/");
    // List file at root
    // listFilesInDir(dir);

    String json = "{\"totalSpace\":";
    json += totalBytes;
    json += ",\"usedBytes\":";
    json += usedBytes;
    json += "}";
    return json;
}

void writeFile(String sensorData)
{
    File file = SPIFFS.open("/data.csv", "a");
    if (!file)
    {
        // File not found
        Serial.println("Failed to open counter file");
        return;
    }
    else
    {
        file.println();
        file.println(sensorData);
        file.close();
    }
}

void clearFile()
{
    File file = SPIFFS.open("/data.csv", "w");
    if (!file)
    {
        // File not found
        Serial.println("Failed to open  file");
        return;
    }
    else
    {
        file.println("id,date,tempC,tempF");
        file.close();
    }
}

void listFilesInDir(File dir, int numTabs)
{
    while (true)
    {

        File entry = dir.openNextFile();
        if (!entry)
        {
            break;
        }
        for (uint8_t i = 0; i < numTabs; i++)
        {
            Serial.print('\t');
        }
        Serial.print(entry.name());
        if (entry.isDirectory())
        {
            Serial.println("/");
            listFilesInDir(entry, numTabs + 1);
        }
        else
        {
            // display zise for file, nothing for directory
            Serial.print("\t\t");
            Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
}
