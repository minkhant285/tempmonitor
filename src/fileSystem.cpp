
#include <SPIFFS.h>
#include <fscheck.h>

String fsCheck()
{
    unsigned int totalBytes = SPIFFS.totalBytes();
    unsigned int usedBytes = SPIFFS.usedBytes();
    // Open dir folder
    File dir = SPIFFS.open("/");
    // List file at root
    String filelist = listFilesInDir(dir);

    String json = "{\"totalSpace\":";
    json += totalBytes;
    json += ",\"usedBytes\":";
    json += usedBytes;
    json += ",";
    json += filelist.substring(0, filelist.length() - 1);
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

void deleteFile(String filename)
{
    SPIFFS.remove(filename.c_str());
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

String listFilesInDir(File dir)
{
    String filelist;
    while (true)
    {

        File entry = dir.openNextFile();
        if (!entry)
        {
            break;
        }
        String filename = entry.name();
        filename.remove(0, 1);
        filelist += "\"";
        filelist += filename;
        filelist += "\":";
        if (entry.isDirectory())
        {
            listFilesInDir(entry);
        }
        else
        {
            filelist += entry.size();
            filelist += ",";
        }
        entry.close();
    }
    return filelist.c_str();
}
