#include  <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include     <BlynkSimpleEsp8266.h>
BlynkTimer timer;
WidgetMap myMap(V0);

// your network SSID (name) & network password
char myssid[] = "insert your SSID";
char mypass[] = "insert your Password";
char token_blynk[] = "Insert your Blynk Token";
// UnwiredLabs API_Token. Signup here to get a free token https://unwiredlabs.com/trial
String token = "Insert your UnwiredLabs token";

String jsonString = "{\n";

// Variables to store unwiredlabs response
double latitude = 0.0;
double longitude = 0.0;
int indexPoint = 0;
void checkGPS() {
  StaticJsonBuffer<200> jsonBuffer;
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
  jsonString += token;
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
      return;
    }
    // Fetch values.
    latitude = root["lat"];
    longitude = root["lon"];
    // Print values
    Serial.println(" ###################");
    Serial.println(latitude, 6);
    Serial.println(longitude, 6);
    Serial.println(" ###################");
  }
  else {
    http.end();  //Close connection
    Serial.println("Error");
  }
  /*latitude = random(1, 500) / 100.0;
  longitude = random(1, 500) / 100.0;
  Serial.println("closing connection");
  Serial.println();
  */
  Blynk.virtualWrite(V1,  String(latitude, 6));
  Blynk.virtualWrite(V2, String(longitude, 6));
  String LocationName = "Esp8266 location: " + String(indexPoint);
  myMap.location(indexPoint, latitude, longitude,LocationName);
  indexPoint++;
}
void setup() {
  Serial.begin(115200);
  Blynk.begin(token_blynk, myssid, mypass);
  //timer.setInterval(3000000L, checkGPS);
  timer.setInterval(10000L, checkGPS);
  myMap.clear();
}
void loop() {
  Blynk.run();
  timer.run();
}
