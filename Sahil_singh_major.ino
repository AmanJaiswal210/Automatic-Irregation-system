#include <DHT.h>
#include <ESP8266WiFi.h>
String apiKey = "I5XQRCEOWB4N5LL0";                                       //  Enter your Write API key here
const char* server = "api.thingspeak.com";
const char *ssid =  "Aman";                                               // Enter your WiFi Name
const char *pass =  "Aman@123";                                           // Enter your WiFi Password
#define DHTPIN D3                                                         // GPIO Pin where the dht11 is connected
DHT dht(DHTPIN, DHT11);
WiFiClient client;

const int moisturePin = A0;                                               // moisteure sensor pin
const int motorPin = D0;
float moisturePercentage;                                                 //moisture reading
float h;                                                                  // humidity reading
float t;                                                                  //temperature reading

void setup()
{
  Serial.begin(115200);
  delay(10);
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);                                              // keep motor off initally
  dht.begin();
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");                                                     // print ... till not connected
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop()
{
  h = dht.readHumidity();                                                  // read humiduty
  t = dht.readTemperature();                                               // read temperature

  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  
 moisturePercentage = ( 100.00 - ( (analogRead(moisturePin) / 1023.00) * 100.00 ) );        //converting anlog value into percentage
if (moisturePercentage < 50) {
  digitalWrite(motorPin, HIGH);                                                            // tun on motor
}
else {
  digitalWrite(motorPin, LOW);                                                            // turn off mottor
}
sendThingspeak();                                                                      //send data to thing speak
delay(5000);                                                                           //after 5 seconds


}

void sendThingspeak() {
  if (client.connect(server, 80))
  {
    String postStr = apiKey;                                                            // add api key in the postStr string
    postStr += "&field1=";
    postStr += String(moisturePercentage);                                              // add mositure readin
    postStr += "&field2=";
    postStr += String(t);                                                               // add tempr readin
    postStr += "&field3=";
    postStr += String(h);                                                               // add humidity readin
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());                                                     //send lenght of the string
    client.print("\n\n");
    client.print(postStr);                                                              // send complete string
    Serial.print("Moisture Percentage: ");
    Serial.print(moisturePercentage);
    Serial.print("%. Temperature: ");
    Serial.print(t);
    Serial.print(" C, Humidity: ");
    Serial.print(h);
    Serial.println("%. Sent to Thingspeak.");
  }
}
