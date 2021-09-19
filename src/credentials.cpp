#include <Arduino.h>
#include <credentials.h>
#include <EEPROM.h>

void clearEEprom()
{
    for (int i = 0; i < 96; ++i)
    {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
    Serial.println("EEPROM deleted!");
}

void setCredentials(String Wssid, String Wpass)
{

    Serial.println("writing eeprom ssid:");
    clearEEprom();
    for (int i = 0; i < Wssid.length(); ++i)
    {
        EEPROM.write(i, Wssid[i]);
        Serial.print("Wrote: ");
        Serial.println(Wssid[i]);
    }
    Serial.println("writing eeprom pass:");
    for (int i = 0; i < Wpass.length(); ++i)
    {
        EEPROM.write(32 + i, Wpass[i]);
        Serial.print("Wrote: ");
        Serial.println(Wpass[i]);
    }
    EEPROM.commit();
}

String getSsid()
{
    String ssid;
    Serial.println("Reading EEPROM ssid");
    for (int i = 0; i < 32; ++i)
    {
        ssid += char(EEPROM.read(i));
    }
    Serial.print("SSID: ");
    Serial.println(ssid.c_str());
    return ssid;
}

String getPassword()
{
    String password;
    Serial.println("Reading EEPROM pass");
    for (int i = 32; i < 96; ++i)
    {
        password += char(EEPROM.read(i));
    }
    Serial.print("PASS: ");
    Serial.println(password.c_str());
    return password;
}
