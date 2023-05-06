//Library Included
#include <esp_now.h>
#include <WiFi.h>

//creating the same data structure as transmitting code
typedef struct data1 {
  float a;
  bool b;
} data1;

//creating structure object
data1 data_set;

//implementing callback fxn everytime we receive new data.It has three parameters.
//mac is a pointer of unsigned int of 8 bits that has the MAC Address of the sender ESP32. Const indicateds that it is read-only
//memcpy copies the incoming data which is passed as argument in the callback fxn as pointer of an unsigned int to pre-defined structure data_set.
//so the size of both transmitting and receiving code much be identical.
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&data_set, incomingData, sizeof(data_set));
  Serial.print("MAC Address of initiator: ");
  for(int i = 0; i < 6; i++) {
    Serial.print(mac[i], HEX);
    if(i < 5) Serial.print(":");
  }
  Serial.println();
  Serial.print("Data received: ");
  Serial.println(len);
  Serial.print("Distance ");
  Serial.println(data_set.a);
  Serial.print("Boolean Value: ");
  Serial.println(data_set.b);
  Serial.println();
}
void setup() {
  // Set up Serial Monitor
  Serial.begin(9600);
  
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
 
  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
 
}
