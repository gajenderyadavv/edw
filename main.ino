#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//************************
#define SS_PIN  4  //D8
#define RST_PIN 5 //D3

//************************
MFRC522 mfrc522(SS_PIN, RST_PIN);

//************************

const char *ssid = "abc";
const char *password = "12345678";
const char* device_token  = "1dd73d5535e4175f";


//************************
String URL = "http://13.127.122.42/rfidattendance/getdata.php"; //computer IP or the server domain
String getData, Link;
String OldCardID = "";


//************************
void setup() {
  delay(1000);
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();

  //---------------------------------------------
  connectToWiFi();
}



//************************
void loop() {
  if(!WiFi.isConnected()){
    connectToWiFi();
  }
  //---------------------------------------------
  if (millis() - previousMillis >= 15000) {
    previousMillis = millis();
    OldCardID="";
  }
  delay(50);
  //---------------------------------------------
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  String CardID ="";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    CardID += mfrc522.uid.uidByte[i];
  }
  //---------------------------------------------
  if( CardID == OldCardID ){
    return;
  }
  else{
    OldCardID = CardID;
  }
  //---------------------------------------------
  Serial.println(CardID);
  SendCardID(CardID);
  delay(1000);
}

void SendCardID( String Card_uid ){
  Serial.println("Sending the Card ID");
  if(WiFi.isConnected()){
    HTTPClient http;
    //GET Data
    getData = "?card_uid=" + String(Card_uid) + "&device_token=" + String(device_token); // Add the Card ID to the GET array in order to send it
    //GET methode
    WiFiClient client;
    Link = URL + getData;
    if(!http.begin(client, Link));
    
    
    
    int httpCode = http.GET();
    String payload = http.getString();

    Serial.println(Link);
    Serial.println(httpCode);
    Serial.println(Card_uid);
    Serial.println(payload);

    if (httpCode == 200) {
      if (payload.substring(0, 5) == "login") {
        String user_name = payload.substring(5);
      }
      else if (payload.substring(0, 6) == "logout") {
        String user_name = payload.substring(6);
        
      }
      else if (payload == "succesful") {

      }
      else if (payload == "available") {

      }
      delay(100);
      http.end();
    }
  }
}


//*******connect to the WiFi*******
void connectToWiFi(){
    WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
    delay(1000);
    WiFi.mode(WIFI_STA);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("Connected");
  
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  //IP address assigned to your ESP
    
    delay(1000);
}
//=======================================================================
