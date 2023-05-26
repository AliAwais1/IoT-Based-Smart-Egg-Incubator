/*
                                                                    ***SMART EGG INCUBATOR*** 
                                                                 
                                                                     Group Members:
                                                                      191380  Ali Awais
                                                                      191379  Tehreem Jamal
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <math.h>

short DHTPIN =D1;         
#define   Heater  D3
#define   Fan  D0
#define   Turning D6
#define   LEDPIN D8 


#define   Heater_ON   digitalWrite(Heater,HIGH);
#define   Heater_OFF  digitalWrite(Heater,LOW);
#define   Fan_ON      digitalWrite(Fan,HIGH);
#define   Fan_OFF     digitalWrite(Fan,LOW);
#define   Turning_ON  digitalWrite(Turning,HIGH);
#define   Turning_OFF digitalWrite(Turning,LOW);

 
DHT dht(DHTPIN, DHT11);

float t,h;
short samplingInterval=4; 
float sum=0;
short counter=0;
float avgTemp, avgHum;

float sum1=0;
short counter1=0;

const char* ssid = "TP-Link_EEDF";
const char* password = "60597298";

const char* mqtt_server = "91.121.93.94";

WiFiClient espClient;

PubSubClient mqttPub(espClient);

void connectToWiFi(){

Serial.println("");
    Serial.print("WiFi connected. IP address is .....");
    // Print the IP address of client
    Serial.println(WiFi.localIP());
    WiFi.begin(ssid, password); 
    
    while (WiFi.status() != WL_CONNECTED) {
      
    Serial.print(".");
    
   for(int i=0; i<5; i++){
  blink();       
  }
  

    }
}


void reconnect() {

 short retryCnt=0;
  Serial.println("Mqtt connecting...");
  while (!mqttPub.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttPub.connect("Egg Incubator"))//client ID
    {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttPub.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
      retryCnt++;
    }

    if(retryCnt>2)
    break;
  }
}

void noteReadings(float temp,float h){

if (isnan(temp)) 
           return;

           if (isnan(h)) 
           return;
           
 sum+=temp;
 counter++;

  sum1+=h;
 counter1++;

 if(counter >= samplingInterval)
 {
  avgTemp=sum/counter;
  avgHum=sum1/counter1;
  Serial.printf("Avg Temp =%f\n",avgTemp);
    Serial.printf("Avg Humidity =%f\n",avgHum);
   sendData(avgTemp,avgHum);
  sum=0; 
  counter=0;

   sum1=0; 
  counter1=0;
 }
 
}
void Temperature_control()
{
  if (avgTemp>= 25)
  {
    Heater_OFF;
    Fan_ON;
    
  }
  else if (avgTemp <= 22.5)
  {
    Heater_ON;
    Fan_OFF;
  }
}

void timerIsr()
{
 Turning_ON;
 delay(9500);

}
void blink(){
  digitalWrite(LEDPIN, HIGH);   
  delay(100);                       
  digitalWrite(LEDPIN, LOW);    
  delay(100); 
}

void sendData(float t, float h){

   char msg[10];
   
  
  
sprintf(msg,"%.2f",h);

  mqttPub.publish("hum", msg);
  
  sprintf(msg,"%.2f",t);
  mqttPub.publish("temp", msg);
 
 delay(1000);
}

void setup() {
  
  Serial.begin(115200);
 pinMode(Heater, OUTPUT);
  pinMode(Fan, OUTPUT);
  pinMode(Turning, OUTPUT);
  pinMode(LEDPIN, OUTPUT);   
  blink();
   connectToWiFi(); 
    dht.begin();
    delay(10);
    
  mqttPub.setServer(mqtt_server, 1883);
}

void loop() {
  if (!mqttPub.connected()) {
    reconnect();
    timerIsr();}
    t = dht.readTemperature();
    h = dht.readHumidity();
     

    Serial.printf("temp=%.2f\t",t);
    Serial.printf("humidity=%.2f\n",h);
    
    noteReadings(t,h);
    Temperature_control();
  delay(1000);
}
