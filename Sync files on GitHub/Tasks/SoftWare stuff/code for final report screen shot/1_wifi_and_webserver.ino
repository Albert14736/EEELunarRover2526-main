#include <WiFiWebServer.h>

// --- Network credentials (a phone hotspot) ---
const char ssid[] = "Albert iPhone";
const char pass[] = "12345678";

WiFiWebServer server(80);          // HTTP server, listening on port 80

// Helper: every handler uses this to reply to the browser with a short text string
void replyAPI(const String& msg) {
  server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
  server.send(200, F("text/plain"), msg);
}

void setup() {
  // Connect to WiFi with a FIXED IP, so the browser address is always the same
  WiFi.config(IPAddress(172, 20, 10, 10), IPAddress(172, 20, 10, 1),
              IPAddress(172, 20, 10, 1), IPAddress(255, 255, 255, 240));
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) { delay(500); }

  // Map each URL the browser can request to the function that handles it
  server.on("/",        handleRoot);      // "/"        -> send the control web page
  server.on("/forward", moveForward);     // "/forward" -> drive the rover forward
  server.on("/sensors", handleSensors);   // "/sensors" -> return all sensor readings
  // ... every other drive command and sensor is registered the same way

  server.begin();                         // start listening for requests
}

void loop() {
  server.handleClient();   // check for an incoming request and run its handler
}
