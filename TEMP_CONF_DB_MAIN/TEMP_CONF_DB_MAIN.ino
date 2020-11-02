#include "max6675.h"
#include "FS.h"
#include "ESP8266WiFi.h"

WiFiClient client;
char MY_SSID[20], MY_PWD[20],url[50],i_val[10];
String ssid1;
String pass1;
String url1;
String i_val1;
char W_SSID[20],W_PWD[20],W_URL[50],W_VAL[10];

int i_val2=0;
int temp=0;
int bat=0;
int thermoDO = 12;//13
int thermoCS = 15;//15
int thermoCLK = 14;//

String data;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

void setup() 
{
  SPIFFS.begin();
  Serial.begin(9600);
  delay(500);
  Serial.println(" ");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  check_config();
  delay(200);
  
  i_val2=atoi(W_VAL);
  connectWifi();
  delay(10000);
  
}
void loop()
{
 ReadValue(temp,bat);
 delay(200);
 data = "boot_value="+String(temp)+"&boot_value1="+String(bat);
 data += "\r\n\r\n";
 
 if (client.connect(W_URL,80))
  { // REPLACE WITH YOUR SERVER ADDRESS
    client.println("POST /mlitlee/d1mini.php HTTP/1.1"); 
    Serial.println("POST /mlitlee/d1mini.php HTTP/1.1"); 
    client.print("Host:"); // SERVER ADDRESS HERE TOO
    client.println(W_URL);
    Serial.print("Host:"); // SERVER ADDRESS HERE TOO
    Serial.println(W_URL);
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
  delay(i_val2);
}
void check_config()
{
  Serial.println(" CHECKING CONFIG.TXT");
  delay(500);
  File myFile = SPIFFS.open("/config.txt","r");
  if (myFile)
  {
    Serial.println("READING CONFIG.TXT");
    delay(500);
    while(!myFile.available());
    char ch=myFile.read();
    myFile.close();
    Serial.println(ch);
    if(ch=='A')
    {
      Serial.println("USER CONFIGURATION MODE");
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
      
      String conf = String(MY_SSID) + ","+String(MY_PWD)  + "," + String(url)+","+String(i_val);

      //String buf = String(SURYA)+ ","+"KUMAR"+ "," +"WWW.MLITSOL.COM"+ ","+"SS";
      File dataFile1 = SPIFFS.open("/ssid.txt","w");
      if (dataFile1) 
      {
            dataFile1.println(conf);
            dataFile1.close();
            Serial.println("DATA WRITE IN SDCARD");
            delay(1000);
      }  
      else 
      {
          Serial.println("SD CARD NOT WORKING");
          delay(1000);
      }
      
      File f = SPIFFS.open("/config.txt", "w");
      if (!f) {
          Serial.println("file open failed config.txt");
      }
      else
      {
          Serial.println("FILE OPEN SUCCESS CONFIG.TXT");
          f.println("B");
          f.close();
      }
    }
    else
    {
      Serial.println("ALREADY CONFIGURED");
      fileRead();
    }
  }
  else
  {
    Serial.println("error opening text.txt");
  }
}
void ScanWifi()
{
  Serial.println("WIFI NETWORK SCANNING STARTS");
  int n = WiFi.scanNetworks();
  Serial.println("SCANNING COMPLETED");
  if (n == 0)
    Serial.println("NO NETWORKS FOUND");
  else
  {
    Serial.print(n);
    Serial.println(" NETWORK FOUNDS");
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
void fileRead()
{
  File dataFile1 = SPIFFS.open("/ssid.txt","r");
  if(!dataFile1)
  {
    Serial.println("SD CARD NOT WORKING");
    delay(1000);
  }
  String l_line;
  while (!dataFile1.available()); 
  l_line = dataFile1.readStringUntil('\n');
  if (l_line != "")
  {
      int commaIndex=l_line.indexOf(',');
      int secondCommaIndex = l_line.indexOf(',', commaIndex+1);
      int thirdCommaIndex=l_line.indexOf(',',secondCommaIndex+1);
      int fourthCommaIndex=l_line.indexOf(',',thirdCommaIndex+1);
      
      ssid1 = l_line.substring(0, commaIndex);
      pass1 =l_line.substring(commaIndex+1, secondCommaIndex);
      url1 = l_line.substring(secondCommaIndex+1,thirdCommaIndex);
      i_val1 = l_line.substring(thirdCommaIndex+1,fourthCommaIndex);
      String sixthValue = l_line.substring(fourthCommaIndex+1);

      ssid1.toCharArray(W_SSID,20);
      delay(200);
      pass1.toCharArray(W_PWD,20);
      delay(200);
      url1.toCharArray(W_URL,50);
      delay(200);
      i_val1.toCharArray(W_VAL,10);
      delay(200);
      Serial.print("WIFI SSID: ");
      Serial.println(W_SSID);
      Serial.print("WIFI PASSWORD: ");
      Serial.println(W_PWD);
      Serial.print("URL: ");
      Serial.println(W_URL);
      Serial.print("INTERVAL TIME: ");
      Serial.println(W_VAL);
      delay(1000);
  }
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
void connectWifi()
{
  Serial.print("Connecting to ");
  Serial.println(ssid1);
  delay(200);
  WiFi.begin(W_SSID, W_PWD);
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

void fileWrite()
{
  File f = SPIFFS.open("/config.txt", "w");
  if (!f)
  {
     Serial.println("file open failed config.txt");
  }
  else
  {
     Serial.println("FILE OPEN SUCCESS CONFIG.TXT");
     f.println("A");
     f.close();
  }
}

