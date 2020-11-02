void ReadString (char Str[])
{
  int Index = 0;
  char c;
  while (1)
  {
    if (Serial.available () > 0)
    {
      c = Serial.read ();
      if (c == '\r' || c == '\n')
        break;
      else
        Str[Index++] = c;
    }
  }
  Str[Index] = 0;

  while (Serial.available () > 0)
    Serial.read ();
}

void Print_Menu (void)
{
  Serial.println ("\n------------------------------");
  Serial.println ("0) Custom command");
  Serial.println ("1) List Access Points");
  Serial.println ("2) Connect to AP");
  Serial.println ("3) Disconnect from AP");
  Serial.println ("4) Get IP Address");
  Serial.println ("5) Get server index page via TCP");
  Serial.println ("6) LED action (ON/OFF/Toggle)");
  Serial.println ("7) Get server peripheral data");
  Serial.println ("------------------------------");
}

void List_Access_Points (void)
{
  Serial1.print ("AT+CWLAP\r");
}

void Connect_to_WiFi_Network (char Net[], char Pass[])
{
  String Command;
  Command = "AT+CWJAP=\"";
  Command = Command + Net;
  Command = Command + "\",\"";
  Command = Command + Pass;
  Command = Command + "\"\r";
  Serial1.print (Command);
}

void Disconnect_from_WiFi_Network (void)
{
  Serial1.print ("AT+CWQAP\r");
}

void Get_IP_Address (void)
{
  long int Timeout = 10000000;
  Serial1.print ("AT+CIFSR\r");
  delay (1);
  while (Timeout --)
  {
    if (Serial1.available ())
    {
      Serial.print ((char)Serial1.read ());
      Timeout=1000000;
    }
  }
}

// the default server IP address is 192.168.0.30
// if you want to change it edit the initial value of the string IP
String IP="AT+CIPSTART=\"TCP\",\"192.168.0.130\",80";
void Get_Server_Page (char IP_Server[])
{
  // by default index.htm (/<empty>) otherwise should be the subpage name
  // for example ("GET /forms.htm HTTP/1.0\r\n\r\n"
  String Cmd = "GET / HTTP/1.0\r\n\r\n";
  long int Timeout = 10000000;
  if (IP_Server[0] != 0)
  {
    IP = "AT+CIPSTART=\"TCP\",\"";
    IP += IP_Server;
    IP += "\",80";
  }
  Serial1.println (IP);
  Serial.println (IP);  // debug purpose
  if (Serial1.find ("Error"))
    return;
  Serial1.print("AT+CIPSEND=");
  Serial1.println (Cmd.length());
  if (Serial1.find (">"))
  {
    Serial.print ('>');
  }
  else
  {
    Serial1.println ("AT+CIPCLOSE");
    Serial.println ("Connection Timeout!");
    delay (1000);
    return;
  }
  
  Serial1.print(Cmd);
  
  // original but delay is too long and during that time the data
  // sent by the server to the ESP module are lost;
  // replaced by timeout instead with initial value 10M
  // and after first reading updating to 1M after each character
  //delay (2000);

  while (Timeout--)
  {
    static bool FirstTime=true;
    int Number=0, i=0;

    // filtering metadata so only page data will be printed
    if (FirstTime)
      FirstTime=false;
    else
      Serial1.find ("OK");

    Serial1.find ("+IPD,");
    delay (1);
    while (1)
    {
      char c=0;
      if (Serial1.available())
      {
        c = Serial1.read ();
        if (c == ':')
          break;
        else
          Number = Number * 10 + (c & 0x0F);
      }
      else
      {
        Timeout--;
        if (!Timeout)
          break;
      }
    }
    for (i=0; i<Number;)
      if (Serial1.available ())
      {
        Serial.print ((char)Serial1.read ());
        Timeout = 100000;
        i++;
      }
  }
  Serial.println ("Transfer complete!");
}

void LED_Action (char IP_Server[], int State)
{
  String Cmd;
  long int Timeout = 10000000;
  if (IP_Server[0] != 0)
  {
    IP = "AT+CIPSTART=\"TCP\",\"";
    IP += IP_Server;
    IP += "\",80";
  }
  Serial1.println (IP);
  Serial.println (IP);  // debug purpose
  if (Serial1.find ("Error"))
    return;
  switch (State)
  {
    case 0:  // Turn OFF
    Cmd = "GET /forms.htm?led1=1 HTTP/1.0\r\n\r\n";
    break;
    case 1:  // Turn ON
    Cmd = "GET /forms.htm?led1=0 HTTP/1.0\r\n\r\n";
    break;
    case 2:  // Toggle
    Cmd = "GET /leds.cgi?led=1 HTTP/1.0\r\n\r\n";
    break;
    default:
    break;
  }

  Serial1.print("AT+CIPSEND=");
  Serial1.println (Cmd.length());
  if (Serial1.find (">"))
  {
    Serial.print ('>');
  }
  else
  {
    Serial1.println ("AT+CIPCLOSE");
    Serial.println ("Connection Timeout!");
    delay (1000);
    return;
  }
  
  Serial1.print(Cmd);
  while (Timeout--)
  {
    if (Serial1.available ())
    {
      char c = Serial1.read ();
      //Serial.print ((char)c);
      Timeout = 100000;
    }
  }
  Serial.println ("Transfer complete!");
}

void Update_Server_Status (char IP_Server[], bool &LED, bool &Button, int &Temperature, int &Potentometer)
{
  String Cmd;
  long int Timeout = 100000;
  int Var=0;
  char c;
  if (IP_Server[0] != 0)
  {
    IP = "AT+CIPSTART=\"TCP\",\"";
    IP += IP_Server;
    IP += "\",80";
  }
  Serial1.println (IP);
  Serial.println (IP);  // debug purpose
  if (Serial1.find ("Error"))
    return;
  Cmd = "GET /status.xml HTTP/1.0\r\n\r\n";  //GET / HTTP/1.0
  Serial1.print("AT+CIPSEND=");
  Serial1.println (Cmd.length());
  if (Serial1.find (">"))
  {
    Serial.print ('>');
  }
  else
  {
    Serial1.println ("AT+CIPCLOSE");
    Serial.println ("Connection Timeout!");
    delay (1000);
    return;
  }
  
  Serial1.print(Cmd);
  
  // original but delay is too long and during that time the data
  // sent by the server to the ESP module are lost;
  // replaced by timeout instead with value 100K
  // updated after each character
  //delay (2000);

  while (1)
  {
    switch (Var)
    {
      case 0:
        Serial1.find ("<led1>");
        delay (1);
        if (Serial1.available())
          LED = !(Serial1.read () & 0x0F);
        Var++;
      break;
      case 1:
        Serial1.find ("<btn0>");
        delay (1);
        if (Serial1.available())
          if (Serial1.read () == 'd')
            Button = 1;
          else
            Button = 0;
        Var++;
      break;
      case 2:
        Serial1.find ("<temp0>");
        delay (1);
        Temperature = 0;
        while (Serial1.available ())
        {
          c = Serial1.read ();
          if (c == '<')
            break;
          Temperature = Temperature * 10 + (c & 0x0F);
        }
        Var++;
      break;
      case 3:
        Serial1.find ("<pot0>");
        delay (1);
        Potentometer = 0;
        while (Serial1.available ())
        {
          c = Serial1.read ();
          if (c == '<')
            break;
          Potentometer = Potentometer * 10 + (c & 0x0F);
        }
        Var++;
      break;
      default:
        while (Timeout--)
        {
          if (Serial1.available ())
          {
            Serial1.read ();
            Timeout = 100000;
          }
        }
        Serial.println ("Transfer complete!");
        return;
      break;
    }
  }
}

void setup()
{
  Serial.begin (9600);
  Serial1.begin (115200);
  delay (1000);
  Serial1.print ("AT+CWMODE=3\r");
  delay (1000);
  Serial1.print ("AT+RST\r");

  Print_Menu ();
}

char Network[32], Password[32], Buffer[64], IP_Address[16];
long int UpdateTimeout = 100000;
bool LED_Status, Button_Status;
int Temperature, Potentiometer;

void loop ()
{
  char c=0;
  if (Serial.available () > 0)
  {
    c = Serial.read ();
    while (Serial.available () > 0)
      Serial.read ();
  }

  while (Serial1.available () > 0)
    Serial.print ((char)Serial1.read());

  switch (c)
  {
    case '0':
      Serial.print ("Input your custom command: ");
      ReadString (Buffer);
      Serial1.print (Buffer);
      Serial1.print ('\r');
    break;
    case '1':
      List_Access_Points ();
    break;
    case '2':
      Serial.print ("Input SSID: ");
      ReadString (Network);
      Serial.println (Network);

      Serial.print ("Input Password: ");
      ReadString (Password);
      Serial.println (Password);
      Connect_to_WiFi_Network (Network, Password);
    break;
    case '3':
      Serial.println ("Disconnect");
      Disconnect_from_WiFi_Network ();
    break;
    case '4':
      Get_IP_Address ();
    break;
    case '5':
      Serial.print ("Input IP address (leave this field empty if you want last used IP): ");
      ReadString (IP_Address);
      Serial.println (IP_Address);

      Get_Server_Page (IP_Address);
    break;
    case '6':
      int x;
      Serial.print ("Input IP address (leave this field empty if you want last used IP): ");
      ReadString (IP_Address);
      Serial.println (IP_Address);

      Serial.print ("Input Action (0-OFF; 1-ON; 2-Toggle): ");
      while (!Serial.available ());
      x = Serial.read () & 0x0F;
      Serial.println (x);

      LED_Action (IP_Address, x);
    break;
    case '7':
      Serial.print ("Input IP address (leave this field empty if you want last used IP): ");
      ReadString (IP_Address);
      Serial.println (IP_Address);

      Update_Server_Status (IP_Address, LED_Status, Button_Status, Temperature, Potentiometer);
      Serial.print ("LED status: "); Serial.println (LED_Status);
      Serial.print ("Button status: "); Serial.println (Button_Status);
      Serial.print ("Temperature: "); Serial.println (Temperature);
      Serial.print ("Potentiometer: "); Serial.println (Potentiometer);
    break;
    default:
    break;
  }
  if (c)
    Print_Menu ();  
}


