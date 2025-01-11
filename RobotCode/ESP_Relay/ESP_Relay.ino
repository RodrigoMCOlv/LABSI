#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "RODRIGO_PC";       // Replace with your Wi-Fi network name
const char* password = "123456789";    // Replace with your Wi-Fi password

WebServer server(80);

String receivedDataComputer = "No data available"; // Variable to store data from the computer
String receivedDataArduino = "No data available"; // Variable to store data from Arduino

// Function to handle the root page


// Handle POST requests from the computer
void handlePost()
{
  if (server.hasArg("plain")) 
  {
    receivedDataComputer = server.arg("plain");

    Serial.print("Got a POST from computer: ");
    Serial.println(receivedDataComputer);

    // Relay data to Arduino
    Serial2.println(receivedDataComputer); // Send to Arduino via Serial2
    server.send(200, "text/plain", "I sent " + receivedDataComputer + "to the Arduino");

    } else {
    server.send(400, "text/plain", "No data received");
  }
}



void setup() {
  Serial.begin(115200);          // For debugging and temporary Arduino simulation
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX: GPIO16, TX: GPIO17 (Connect to Arduino)

  WiFi.begin(ssid, password);    // Connect to Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, []() {server.send(200, "text/html", "<h1>Hello World!</h1>");}); //serve HTML Page
  server.on("/post", HTTP_POST, handlePost); // Endpoint for receiving data from the computer	
  server.on("/getData", handleGetData); // Endpoint for fetching Arduino data
  
  server.begin(); // Start the HTTP server
  Serial.println("HTTP server started");
}

// Handle GET requests to fetch the latest data from the Arduino
void handleGetData() 
{
    Serial.print("Got a GET request from computer and sent: ");
    Serial.println(receivedDataArduino);

    server.send(200, "text/plain", receivedDataArduino);

    receivedDataArduino = "No data available"; // Reset the data after sending it
}


void loop() {
  server.handleClient(); // Handle HTTP requests

  // Get data from Arduino 
  if (Serial2.available()) {
    receivedDataArduino = Serial2.readStringUntil('\n'); // Read data from Arduino or Serial Monitor

    Serial.print("Got data from Arduino: ");
    Serial.println(receivedDataArduino); // Debugging
  }

}

