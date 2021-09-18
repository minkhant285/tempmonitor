#include <Arduino.h>

String fsCheck();
void writeFile(String sensorData);
void clearFile();
String listFilesInDir(File dir);
void deleteFile(String filename);
String readCSV();
