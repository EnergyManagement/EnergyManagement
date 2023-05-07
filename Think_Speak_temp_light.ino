#include <WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>
#include <Wire.h>   // Include the I2C library
#include <BH1750.h> // Include the BH1750 library

BH1750 lightMeter;   // Create a BH1750 object

int  trigPin = 5;     
int  echoPin = 18;

#define DHTPIN 19
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const char* ssid="Sid";
const char* password="12345678";

// ThingSpeak API endpoint and API key
const char* server = "api.thingspeak.com";
const char* apiKey = "8KXCI7AE15WCR87Z";

WiFiClient client;
typedef struct data1 
{
  float a;
  float c;
  bool b;
} data1;

//created a structured object
data1 data_set;

void setup()
{
  Wire.begin();      // Initialize the I2C communication

  lightMeter.begin();// Initialize the light sensor
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //setting up serial monitor at 115200 as the baud rate of wifi is higher than normal
  Serial.begin(9600);
  dht.begin();
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
  WiFi.begin("Sid", "12345678");
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
 delay(500);

 data_set.c =dht.readTemperature();
 Serial.println( data_set.c);
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
 
 if ( (distance >= 0) && (distance <= 10 ))
 {
  Serial.println(distance);
  data_set.a=distance;
  data_set.b=true;
 }
 if (distance > 10)
 {
  Serial.println(distance);
  data_set.a=distance;
  data_set.b=false; 
 }
 Serial.println("");
 delay(1000);

  if ( isnan(data_set.c)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("%  Temperature: ");
  Serial.print(data_set.c);
  Serial.println("°C");
  delay(1000);

  uint16_t lux = lightMeter.readLightLevel();// Read the light level in lux
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lux");
  delay(1000);// Wait for a second before reading again

  
 ThingSpeak.setField(1, data_set.a);
 ThingSpeak.setField(2, data_set.b);
 ThingSpeak.setField(3, data_set.c);
 ThingSpeak.setField(4, lux);
  int httpCode = ThingSpeak.writeFields(2118302,apiKey);
  Serial.println(httpCode);
  if (httpCode == 200) {
    Serial.println("Data sent to ThingSpeak");
  } else {
    Serial.println("Failed to send data to ThingSpeak");
    ThingSpeak.begin(client);
  }
  delay(25000);  // Send data every minute
}
