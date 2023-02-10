/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA2 encryption. For insecure
 WEP or WPA, change the Wifi.begin() call and use Wifi.setMinSecurity() accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 abc
 */

#include <WiFi.h>

const char* ssid = "n";
const char* password = "7279658668796900n!";

int AugenListeStelle = 0;

WiFiServer server(80);

int led[7] = { 23, 22, 21, 33, 27, 26, 32 };

void setup() {
  Serial.begin(115200);

  for (int index = 0; index <= 6; index++) {

    pinMode(led[index], OUTPUT);
  }

  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

int Wuerfe[5] = { 0, 0, 0, 0, 0 };

int AnzahlDerWuerfe = 0;

int augen[6][7] = {
  { 0, 0, 0, 0, 0, 0, 1 },  //Augenzahl 1
  { 0, 0, 1, 0, 0, 1, 0 },  //Augenzahl 2
  { 0, 0, 1, 0, 0, 1, 1 },  //Augenzahl 3
  { 1, 0, 1, 1, 0, 1, 0 },  //Augenzahl 4
  { 1, 0, 1, 1, 0, 1, 1 },  //Augenzahl 5
  { 1, 1, 1, 1, 1, 1, 0 }   //Augenzahl 6
};

int abc[5] = { 0, 0, 0, 0, 0 };

void ZufallsZahl(int Zahl) {

  Serial.print("AnzahlDerWuerfe: ");
  Serial.println(AnzahlDerWuerfe);
  Serial.println(Wuerfe[0]);
  Serial.println(Wuerfe[1]);
  Serial.println(Wuerfe[2]);
  Serial.println(Wuerfe[3]);
  Serial.println(Wuerfe[4]);

  for (int index1 = 0; index1 <= 50; index1++) {
    AugenListeStelle = random(0, 6);

    for (int index2 = 0; index2 <= 6; index2++) {
      digitalWrite(led[index2], augen[AugenListeStelle][index2]);

      Wuerfe[AnzahlDerWuerfe] = AugenListeStelle + 1;
    }
    delay(50);
    Zahl = AugenListeStelle + 1;
  }
  if (Wuerfe[4] != 0) {
    abc[0] = Wuerfe[1];
    abc[1] = Wuerfe[2];
    abc[2] = Wuerfe[3];
    abc[3] = Wuerfe[4];
    abc[4] = AugenListeStelle + 1;
    
    Wuerfe[0] = abc[0];
    Wuerfe[1] = abc[1];
    Wuerfe[2] = abc[2];
    Wuerfe[3] = abc[3];
    Wuerfe[4] = abc[4];
  }
  AnzahlDerWuerfe = AnzahlDerWuerfe + 1;
}


void loop() {
  WiFiClient client = server.available();  // listen for incoming clients

  int augenzahl = 0;

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        if (c == '\n') {            // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print("<html style=\"text-align:center\">");

            // the content of the HTTP response follows the header:
            client.print("<h3 style=\" font-size:48px \">");
            client.print("Druecke <a href=\"/H\">hier</a>  um zu Wuerfeln<br>");
            //*client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 19 off.<br>");
            client.print("</h3>");


            if (Wuerfe[0] != 0) {

              client.print("<h2 style=\" font-size:72px \">");
              client.print("Die Letzten 5 Ergebnisse");
              client.print("</h2>");
              for (int ind = 4; ind >= 0; ind--) {
                if (Wuerfe[ind] != 0) {
                  client.print("<h1 style=\" font-size:72px; color:red  \">");
                  client.print(Wuerfe[ind]);
                  client.print("</h1>");
                }
              }
            }

            client.print("</html>");
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          ZufallsZahl(augenzahl);
        }
        if (currentLine.endsWith("GET /L")) {
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
