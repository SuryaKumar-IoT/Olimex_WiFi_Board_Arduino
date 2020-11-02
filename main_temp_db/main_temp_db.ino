#include "max6675.h"
#include <ESP8266WiFi.h>

WiFiClient client;
const char* MY_SSID = "T- HUB"; 
const char* MY_PWD = "Catalyst@123";
String data;

int thermoDO = 12;//13
int thermoCS = 15;//15
int thermoCLK = 14;//

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

void setup() { 
  Serial.begin(9600);
  connectWifi();

  delay(10000); // GIVE THE SENSOR SOME TIME TO START
  data = "";
}

void loop()
{ 
 int h = thermocouple.readCelsius();
 int t = thermocouple.readFarenheit();
 
 data = "boot_value="+String(t)+"&boot_value1="+String(h);
 data += "\r\n\r\n";
  if (client.connect("www.mlitsol.com",80))
  { // REPLACE WITH YOUR SERVER ADDRESS
    client.println("POST /mlitlee/d1mini.php HTTP/1.1"); 
    Serial.println("POST /mlitlee/d1mini.php HTTP/1.1"); 
    client.println("Host:www.mlitsol.com"); // SERVER ADDRESS HERE TOO
    Serial.println("Host:www.mlitsol.com"); // SERVER ADDRESS HERE TOO
    client.println("Content-Type: application/x-www-form-urlencoded");
    Serial.println("Content-Type: application/x-www-form-urlencoded"); 
    client.print("Content-Length: ");
    Serial.print("Content-Length: "); 
    client.println(data.length()); 
    Serial.println(data.length()); 
    client.println(); 
    client.print(data);
    delay(1000);
    Serial.println(data); 
  } 

  if (client.connected()) { 
    client.stop();  // DISCONNECT FROM THE SERVER
  }

  delay(3000); // WAIT FIVE MINUTES BEFORE SENDING AGAIN
}
void connectWifi()
{
  Serial.print("Connecting to "+*MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Connected");
  Serial.println("");  
}

