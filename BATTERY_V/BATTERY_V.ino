void setup()
{
  Serial.begin(9600);
}

void loop() 
{
  int n=analogRead(A0);
  Serial.println(n);
  delay(500);
}
