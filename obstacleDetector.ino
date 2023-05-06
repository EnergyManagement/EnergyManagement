//Library Included
#include <esp_now.h>
#include <WiFi.h>
int  trigPin = 5;     
int  echoPin = 18;
//universal variables
float data;
bool obstacle;

//using unidentified int because MAC Address is always positive and for normal int it ranges from -128 to 127 whreas unsigned from 0 to 255 so it increases the range.
uint8_t MAC[]={0x30, 0xC6, 0xF7, 0x14, 0xFC, 0x9C};

//esp_now allows data transfers only in the form of packets hence we use structure
typedef struct data1 
{
  float a;
  bool b;
} data1;

//created a structured object
data1 data_set;

//  esp_now_peer_info_t structure  contains information about the peer being added, 
//such as its MAC address and communication settings and peerInfo is an instance of this structure it has many mamebers like peer_addr.
esp_now_peer_info_t peerInfo;

//callback fxn execution every time data is sent.
//MAC_Address is the pointer of unsigned in constaining receiver MAC.
//status is of type "esp_now_send_status_t"- is an eunmerated data type in the ESP-IDF((Espressif IoT Development Framework)API.
//(An enumerated data type is like a list of things that you can use in your program. 
//Each thing in the list has a name that you choose, and a number that the computer assigns to it. 
//You can use the names in your program instead of the numbers, which makes your program easier to read and understand.)
void Call_Back(const uint8_t *MAC_Address,esp_now_send_status_t status)
{
  Serial.print("MAC Adress of Receiver");
   for(int i = 0; i < 6; i++) 
   {
    Serial.print(MAC_Address[i], HEX);
    if(i < 5) Serial.print(":");
  }
  Serial.println();
  if(status==ESP_NOW_SEND_SUCCESS)
  {
    Serial.println("Data delivered Successfully");
  }
  else
  {
   Serial.println("Error delivering Data"); 
  }
}
//ESP_NOW_SEND_SUCCESS is not an inbuilt fxn but a predefined constant.
//It is used to represent the status of a data packet sent using the ESP-NOW protocol.
//It is a part of the esp_now_send_status_t enumerated data type.
void setup(){
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
  //setting up serial monitor at 115200 as the baud rate of wifi is higher than normal
  Serial.begin(9600);

 //set ESP32 as wifi_station or client in order to initialize the espnow protocol
  WiFi.mode(WIFI_STA);

//esp_now_init() is a function provided by the ESP-IDF.It is used to initilze the ESP-NOW Protocol
//function sets up the necessary internal data structures and resources required by the ESP-NOW protocol, including registering the callback functions.
//If the initialization is successful, the function returns ESP_OK
  if(esp_now_init()== ESP_OK)
  {
    Serial.print("Successful Initialization");
  }
  else
  {
   if(esp_now_init()== ESP_ERR_NO_MEM)
   Serial.print("Not Enough Memory");
   else if(esp_now_init() == ESP_ERR_ESPNOW_NOT_INIT)
   Serial.print("Wifi not initialized");
   else if(esp_now_init() == ESP_ERR_INVALID_STATE)
   Serial.print("invalid state");
   else
   Serial.print("Error initializing ESP_NOW");
  }


//Regestering callback fxn
  esp_now_register_send_cb(Call_Back);

//memcpy is a function of standard C library that copiesthe second block to the first block with a particular size.
//here it copies from MAC array to peer_addr
  memcpy(peerInfo.peer_addr, MAC, 6);
  peerInfo.channel = 0; //The ESP32 supports 13 wifi channel,with 0 used for ESP_NOW.
  peerInfo.encrypt = false; //This ensures there is no encryption 

  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 void loop()
 {
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
 
 esp_err_t result = esp_now_send(MAC, (uint8_t *) &data_set, sizeof(data_set));
   
  if (result == ESP_OK) {
    Serial.println("Sending confirmed");
  }
  else {
    Serial.println("Sending error");
  }
  delay(2000);

 }
 
