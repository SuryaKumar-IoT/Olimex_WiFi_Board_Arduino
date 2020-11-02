#include "max6675.h"
#include "ESP8266WiFi.h"

WiFiClient client;

char MY_SSID[32], MY_PWD[32],url[50],i_val[10];
int i_val1=0;
int temp=0;
int bat=0;
int thermoDO = 12;//13
int thermoCS = 15;//15
int thermoCLK = 14;//

String data;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

void setup()
{
  Serial.begin(9600);
  Serial.println("WELCOME TO USER GUI..");
  delay(2000);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");
  delay(1000);
  ScanWifi();
  delay(1000);
  Serial.println("ENTER WIFI SSID: ");
  ReadString(MY_SSID);
  Serial.print("ENTERED USER NAME: ");
  Serial.println(MY_SSID);
  delay(500);
  Serial.println("ENTER WIFI PASSWORD: ");
  ReadString(MY_PWD);
  Serial.print("ENTERED PASSWORD: ");
  Serial.println(MY_PWD);
  delay(500);
  Serial.println("ENTER URL: ");
  ReadString(url);
  Serial.print("ENTERED URL: ");
  Serial.println(url);
  Serial.println("ENTER INTERVAL TIME: ");
  ReadString(i_val);
  Serial.print("ENTERED INTERVAL TIME: ");
  Serial.println(i_val);
  i_val1=atoi(i_val);
  connectWifi();
  delay(10000); // GIVE THE SENSOR SOME TIME TO START
  data = "";
}

void loop()
{ 
 ReadValue(temp,bat);
 delay(200);
 data = "boot_value="+String(temp)+"&boot_value1="+String(bat);
 data += "\r\n\r\n";
  if (client.connect(url,80))
  { // REPLACE WITH YOUR SERVER ADDRESS
    client.println("POST /mlitlee/d1mini.php HTTP/1.1"); 
    Serial.println("POST /mlitlee/d1mini.php HTTP/1.1"); 
    client.print("Host:"); // SERVER ADDRESS HERE TOO
    client.println(url);
    Serial.print("Host:"); // SERVER ADDRESS HERE TOO
    Serial.println(url);
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

  delay(i_val1); // WAIT FIVE MINUTES BEFORE SENDING AGAIN
}
void ReadString (char Str[])
{
  int Index = 0;
  char c[32];
  while(!Serial.available());
  while(Serial.available())
  {
    c[Index]=Serial.read();
    Str[Index]=c[Index];
    Index++;
    delay(1000);
  }
  Str[Index]='\0';
  Serial.println(Str);
  delay(500);
}
void ScanWifi()
{
  Serial.println("scan start");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
    }
  }
  Serial.println("");
  delay(2000);
}
void connectWifi()
{
  Serial.print("Connecting to ");
  Serial.println(MY_SSID);
  delay(200);
  WiFi.begin(MY_SSID, MY_PWD);
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Connected");
  Serial.println("");
  Serial.println("IP ADDRESS");
  Serial.print(WiFi.localIP());
  Serial.println("");  
}
void ReadValue(int t,int b)
{
  temp=thermocouple.readCelsius();
  bat=analogRead(A0);
  bat=(bat-24)/10;
  if(bat<0)
    bat=0;
  Serial.print("TEMP: ");
  Serial.print(temp);
  Serial.println("(`C)");
  Serial.print("BAT: ");
  Serial.print( bat);
  Serial.println("(%)");
  delay(200);
}


