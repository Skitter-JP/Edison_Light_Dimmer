#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <I2C_Anything.h>

#define ERRORLED 13
#define WIFILED 15

bool intial = true;

int RTriac1 = 0;
int RTriac2 = 0;

float current = 0;

float RelayTemp1 = 0;
float RelayTemp2 = 0;

int Triac1 = 100;
int Triac2 = 100;

float TriTemp1 = 50;
float IndTemp1 = 50;

float TriTemp2 = 0;
float IndTemp2 = 0;

char controldata[2] = {100,100}; //Used for sending data to Triac Controller

float templimit = 70;   //Internal Temperature limit
float currentlimit = 4.2; //Internal Current Limit

unsigned long previousMillis = 0;        // will store last time pushdata was called
const long interval = 1000;              // How often pushdata will be called

unsigned long IpreviousMillis = 0;        // will store last time pushdata was called
const long Tinterval = 2000;              // How often pushdata will be called

bool CurrentError = false;
bool TempError = false;
bool SensorsError = false;
bool RecievedDataError = false;


// Update these with values suitable for your network.

const char* ssid = "Wifi_name_here";
const char* password = "Wifi_Password_here";
const char* mqtt_server = "MQTT_IP_address_here";

WiFiClient espClient;
PubSubClient client(espClient);


void errorLED(bool state){
  if(state){
    digitalWrite(ERRORLED,HIGH);
  }else{
    digitalWrite(ERRORLED,LOW);
  }
}

void wifiLED(bool state){
  if(state){
    digitalWrite(WIFILED,HIGH);
  }else{
    digitalWrite(WIFILED,LOW);
  }
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros()); // Why ?

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  wifiLED(0);   
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String value ="";
  int DIMval;
  Serial.print("Recieved Message :");
  for (int i=0;i<length;i++) {
        value += (char)payload[i];
    }
  DIMval=value.toInt();
  Serial.print("INT Message: ");
  Serial.println(DIMval);

   

  
  if (strcmp(topic,"OUT1")==0){
        if(DIMval == 0){
          Triac1 = 100;
          RTriac1 = 100;
          Serial.println("Triac 1 OFF");
          RecievedDataError = false;
        }
        if(DIMval >= 0 && DIMval < 101){
          RTriac1 = DIMval;
          Triac1 = map(DIMval,0,100,100,75); //Should never go below 75
          Serial.print("Value to Triac 1 Controller: ");
          Serial.println(Triac1);
          RecievedDataError = false;
        }else{
          RecievedDataError = true;
          Triac1 = 100; // TURN OFF
        }
  }
  
  if (strcmp(topic,"OUT2")==0){
    if(DIMval == 0){
          Triac2 = 100;
          RTriac2 = 100;
          Serial.println("Triac 2 OFF");
          RecievedDataError = false;
        }
        if(DIMval >= 0 && DIMval < 101){
          RTriac2 = DIMval;
          Triac2 = map(DIMval,0,100,100,75); //Should never go below 75
          Serial.print("Value to Triac 2 Controller: ");
          Serial.println(Triac2);
          RecievedDataError = false;
        }else{
          RecievedDataError = true;
          Triac2 = 100; // TURN OFF
        }    
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("OUT1");
      client.subscribe("OUT2");
    } else {
      wifiLED(0);
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(ERRORLED, OUTPUT);     
  pinMode(WIFILED, OUTPUT);
  errorLED(0);
  wifiLED(0);     
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Wire.begin();
  Wire.setClockStretchLimit(1500);
  Wire.beginTransmission(9); // transmit to device #9
  Wire.write(controldata,2); // sends x
  Wire.endTransmission(); // stop transmitting

}

void loop() {

  if (!client.connected()) {
    wifiLED(0);  
    reconnect();
    wifiLED(1);  
  }
  client.loop();
  if(intial){
    //This will push zero values to the Homebridge server if this device turns on
    pushdimvalue();
    intial = false;
  } 
  updatetriaccontroller();
  pushdata();
  getTempCur();
  tempcheck();
  currentcheck();
  errorcheck();

}

void pushdimvalue(){
    StaticJsonDocument<40> doc;
    
    doc["T1P"] = RTriac1;
    doc["T2P"] = RTriac2;
    //serializeJsonPretty(doc, Serial);
    Serial.print("Push Dim1: ");
    Serial.println(RTriac1);
    Serial.print("Push Dim2: ");
    Serial.println(RTriac2);
    //Serial.println(doc.memoryUsage());
    char buffer[40];
    serializeJson(doc, buffer);
    client.publish("dimmer/dim", buffer);
    Serial.println("Pushed Dim Val");
  
}

void pushdata(){
  
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval){
      
    previousMillis = currentMillis;
    
    
    StaticJsonDocument<150> doc;
    doc["I"] = current;
    doc["RT1"] = RelayTemp1;
    doc["RT2"] = RelayTemp2;
    doc["T1T"] = TriTemp1;
    doc["L1T"] = IndTemp1;
    doc["T2T"] = TriTemp2;
    doc["L2T"] = IndTemp2;
    //serializeJsonPretty(doc, Serial);
    //Serial.println();
    //Serial.println(doc.memoryUsage());
    char buffer[150];
    serializeJson(doc, buffer);
    client.publish("dimmer/data", buffer);
    }
}

void updatetriaccontroller(){
    byte temp[2];
    bool change = false;    
      if(controldata[0] != Triac1){
        
          Serial.println("Triac 1 Changed");
          delay(1);
          controldata[0] = Triac1;
          change = true;
      }
      if(controldata[1] != Triac2){
          Serial.println("Triac 2 Changed");
          delay(1);
          controldata[1] = Triac2;
          change = true;
      }
      if(change){
          temp[0] = (byte)controldata[0];
          temp[1] = (byte)controldata[1];
          Wire.beginTransmission(9); // transmit to device #9
          Wire.write(temp,2); // sends x
          Wire.endTransmission(); // stop transmitting
          pushdimvalue();  
      }
      
}

void getTempCur(){
  
  //Wire.requestFrom(8, 32);    // request 6 bytes from slave device #8
    // Request two floats and a integer, that makes 10 bytes..what?
  int n = Wire.requestFrom(8, 28);
  //Serial.println(n);
  if( n == 28)          // is the number of bytes received that was requested ?
  {
    I2C_readAnything(TriTemp1);
    I2C_readAnything(TriTemp2);
    I2C_readAnything(IndTemp1);
    I2C_readAnything(IndTemp2);
    I2C_readAnything(RelayTemp1);
    I2C_readAnything(RelayTemp2);
    I2C_readAnything(current);
    SensorsError = false;
  }
  else
  {
    Serial.println("Fail Getting Temps");
    SensorsError = true;
  } 
}

void tempcheck(){
  //Turns off Triacs if the temperature limit has been met
  if(RelayTemp1 > templimit || RelayTemp2 > templimit || TriTemp1 > templimit || TriTemp2 > templimit || IndTemp1 > templimit || IndTemp2 > templimit ){
    
      TempError = true;
      Triac1 = 100;
      Triac2 = 100;
      RTriac1 = 0;
      RTriac2= 0;
  }else{
    TempError = false;
  }
}

void currentcheck(){
  
  
    //Turns off Triacs if the current limit has been met
  if(current > currentlimit){
    unsigned long IcurrentMillis = millis();
      if (IcurrentMillis - IpreviousMillis >= Tinterval){   
      IpreviousMillis = IcurrentMillis;
      CurrentError = true;
      Triac1 = 100;
      Triac2 = 100;
      RTriac1 = 0;
      RTriac2= 0;
      updatetriaccontroller();
      }
  }else{
    CurrentError = false;
  }
}

void errorcheck(){
  if(CurrentError == true || TempError == true || SensorsError == true || RecievedDataError == true) {
    errorLED(1);  
  }else{
    errorLED(0);  
  }

}
