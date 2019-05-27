#include  <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include     <BlynkSimpleEsp8266.h>


// your network SSID (name) & network password
char myssid[] = "insert your SSID";
char mypass[] = "insert your Password$";
char tokenBlynk[] = "Insert your Blynk Token";
String UnwiredLabsToken = "Insert your UnwiredLabs token";
//global variables 
int indexPoint = 0;
BlynkTimer timer;
WidgetMap myMap(V0);

//prototype functions
int getPosition ( double *latitude , double *longitude);
void UpdateLocationInMaps(void);

void UpdateLocationInMaps() {
  double latitude = 0.0;
  double longitude = 0.0;
  getPosition(&latitude, &longitude);
  Blynk.virtualWrite(V1,  String(latitude, 6));
  Blynk.virtualWrite(V2, String(longitude, 6));
  String LocationName = "Esp8266 location: " + String(indexPoint);
  myMap.location(indexPoint, latitude, longitude,LocationName);
  indexPoint++;
}

int getPosition ( double *latitude , double *longitude) { 
  StaticJsonBuffer<200> jsonBuffer;
  String jsonString = "{\n";
  char bssid[6];
  DynamicJsonBuffer jsonBuffer_;
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0 ) {
    Serial.println("No networks available");
  } 
  else {
    Serial.print(n);
    Serial.println(" networks found");
  }
  // now build the jsonString...
  jsonString = "{\n";
  jsonString += "\"token\" : \"";
  jsonString += UnwiredLabsToken;
  jsonString += "\",\n";
  jsonString += "\"wifi\": [\n";
  for (int j = 0; j < n; ++j) {
    jsonString += "{\n";
    jsonString += "\"bssid\" : \"";
    jsonString += (WiFi.BSSIDstr(j));
    jsonString += "\",\n";
    jsonString += "\"channel\" : \"";
    jsonString += (WiFi.channel(j));
    jsonString += "\",\n";
    jsonString += "\"frequency\" : \"";
    jsonString += (2412);
    jsonString += "\",\n";
    jsonString += "\"signal\": ";
    jsonString += WiFi.RSSI(j);
    jsonString += "\n";
    if (j < n - 1) {
      jsonString += "},\n";
    } 
    else {
      jsonString += "}\n";
    }
  }
  jsonString += ("]\n");
  jsonString += ("}\n");
  Serial.println(jsonString);
  HTTPClient http;    //Declare object of class HTTPClient
  //Connect to the client and make the api call
  if (http.begin("http://us1.unwiredlabs.com/v2/process.php") ) {
    Serial.println("Connected");
    http.addHeader("Content-Type", "application/json");  //Specify content-type header
    int httpCode = http.POST(jsonString);   //Send the request
    String payload = http.getString();                  //Get the response payload
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload
    JsonObject& root = jsonBuffer_.parseObject(payload);
    // Test if parsing succeeds.
    if (!root.success()) {
      Serial.println("parseObject() failed");
      return -1;
    }
    // Fetch values.
    *latitude = root["lat"];
    *longitude = root["lon"];
  }
  else {
    http.end();  //Close connection
    Serial.println("Error");
    return -2;
  }
  return 1;
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(tokenBlynk, myssid, mypass);
  //timer.setInterval(3000000L, checkGPS);
  timer.setInterval(10000L, UpdateLocationInMaps);
  myMap.clear();
}

void loop() {
  Blynk.run();
  timer.run();
}
