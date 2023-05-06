#include <WiFi.h>
#include <ThingSpeak.h>
int  trigPin = 5;     
int  echoPin = 18;

const char* ssid="Uddipan";
const char* password="uddi@2000";

// ThingSpeak API endpoint and API key
const char* server = "api.thingspeak.com";
const char* apiKey = "3Y6X3YLE9690BKA5";

WiFiClient client;
typedef struct data1 
{
  float a;
  bool b;
} data1;

//created a structured object
data1 data_set;
void setup()
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //setting up serial monitor at 115200 as the baud rate of wifi is higher than normal
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  int networksFound = WiFi.scanNetworks();
  if (networksFound == 0) {
    Serial.println("No networks found");
  } else {
    Serial.print(networksFound);
    Serial.println(" networks found:");
    for (int i = 0; i < networksFound; i++) {
      Serial.print("  ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      delay(10);
    }
  }
  WiFi.begin("Uddipan", "uddi@2000");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");
  WiFi.setAutoReconnect(true);
  ThingSpeak.begin(client);
}
void loop()
{
  if (WiFi.status() != WL_CONNECTED) 
  {
    Serial.println("WiFi connection lost");
  }
 delay(1000);
 
 int duration; 
 float distance;   
 digitalWrite(trigPin, HIGH);
 delayMicroseconds(1000);  
 digitalWrite(trigPin, LOW);
 
 duration = pulseIn(echoPin,HIGH);
 distance = ( duration / 2) / 29.1;
 if ( distance < 0 ) 
 {
  Serial.println("Distance =0");
  data_set.a=distance;
  data_set.b=false;
 }
 
 if ( (distance >= 0) && (distance <= 5 ))
 {
  Serial.println(distance);
  data_set.a=distance;
  data_set.b=true;
 }
 if (distance > 5)
 {
  Serial.println(distance);
  data_set.a=distance;
  data_set.b=false; 
 }
 Serial.println("");
 delay(2000);
 ThingSpeak.setField(1, data_set.a);
 ThingSpeak.setField(2, data_set.b);
  int httpCode = ThingSpeak.writeFields(2114790,apiKey);
  Serial.println(httpCode);
  if (httpCode == 200) {
    Serial.println("Data sent to ThingSpeak");
  } else {
    Serial.println("Failed to send data to ThingSpeak");
    ThingSpeak.begin(client);
  }
  delay(50000);  // Send data every minute
}
