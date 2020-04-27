/* ESP8266 Moisture Sensor
   This sketch uses an ESP8266 to read the analog signal from a moisture sensor. The data is then displayed
   using the serial console or a web browser. Based on the moisture reading, the ESP8266 will blink a RGB LED
   red, green or blue.

   Red = Dry
   Green = In between Wet and Dry
   Blue = Wet

     Viewing the data via web browser by going to the ip address. In this sketch the address is
     http://192.168.1.221

      The browser data includes a Google Chart to visually illustrate the moisture reading as a guage.

   ///////////////////////////////////////
   Arduino IDE Setup
   File:
      Preferences
        Add the following link to the "Additional Boards Manager URLs" field: 
        http://arduino.esp8266.com/stable/package_esp8266com_index.json
   Tools:
      board: NodeMCU 1.0 (ESP-12 Module)
      programmer: USBtinyISP

      
  ///////////////////////////////
*/
#include <ESP8266WiFi.h>

const char* ssid = "Headam";
const char* password = "Maminette666";

const int redPin = 4;     // ~D2
const int greenPin = 12;  // ~D6
const int bluePin = 14;   // ~D5
const int dryAnalog = 630; // Maximum reading on probe for completely dry reading                - DG
const int wetAnalog = 250; // Minimum reading on probe for completely wet reading, i.e. in water - DG


int WiFiStrength = 0;

// dg ##########
const char index_html[] PROGMEM = R"rawliteral(
HTTP/1.1 200 OK
Content-Type: text/html

<!DOCTYPE HTML>
<html>
 <head>
  <meta http-equiv=\"refresh\" content=\"60\">"
  <script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>"
  <script type=\"text/javascript\">"
    google.charts.load('current', {'packages':['gauge']});"
    google.charts.setOnLoadCallback(drawChart);"
   function drawChart() {"
      var data = google.visualization.arrayToDataTable([ "
        ['Label', 'Value'], "
  client.print("        ['Moisture',  "
  client.print(chartValue 
  " ], "
  "       ]); "
  // setup the google chart options here
  "    var options = {"
  "      width: 400, height: 120,"
  "      redFrom: 0, redTo: 25,"
  "      redColor: '#FF3232',"
  "      greenFrom: 25, greenTo: 75,"
  "      greenColor: '#46C646',"
  "      yellowFrom: 75, yellowTo: 100,"
  "      yellowColor: '#1589FF',"
  "       minorTicks: 5"
  "    };"

  "   var chart = new google.visualization.Gauge(document.getElementById('chart_div'));"

  "  chart.draw(data, options);"

  "  setInterval(function() {"
  client.print("  data.setValue(0, 1, "
  client.print(chartValue
  "    );"
  "    chart.draw(data, options);"
  "    }, 13000);"


  "  }"
  " </script>"

  "  </head>"
  "  <body>"

  client.print("<h1 style=\"size:12px;\">ESP8266 Soil Moisture</h1>"

  // show some data on the webpage and the guage
  "<table><tr><td>"

  client.print("WiFi Signal Strength: "
  WiFiStrength
  "dBm<br>"
  client.print("Analog Raw: "
  analogValue
  client.print("<br>Analog Volts: "
  analogVolts
  "<br><a href=\"/REFRESH\"\"><button>Refresh</button></a>"

  "</td><td>"
  // below is the google chart html
  "<div id=\"chart_div\" style=\"width: 300px; height: 120px;\"></div>"
  "</td></tr></table>"

  "<body>"
  "</html>"
  delay(1);
  Serial.println("Client disconnected"
  Serial.println("");
  
xxxxxxxxxxxxx
  <!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP8266 DHT Server</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">%</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var) {
  //Serial.println(var);
  if (var == "TEMPERATURE") {
    return String(t);
  }
  else if (var == "HUMIDITY") {
    return String(h);
  }
  return String();
}
// xxxxxxxxxxxx

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);


  // color while waiting to connect
  analogWrite(redPin, 280);
  analogWrite(greenPin, 300);
  analogWrite(bluePin, 300);


  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  // Set the ip address of the webserver
  // WiFi.config(WebServerIP, Gatway, Subnet)
  // or comment out the line below and DHCP will be used to obtain an IP address
  // which will be displayed via the serial console

  WiFi.config(IPAddress(192, 168, 1, 221), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));

  // connect to WiFi router
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

}

double analogValue = 0.0;
double analogVolts = 0.0;
unsigned long timeHolder = 0;


void loop() {

  WiFiStrength = WiFi.RSSI(); // get dBm from the ESP8266
  analogValue = analogRead(A0); // read the analog signal

  // convert the analog signal to voltage
  // the ESP2866 A0 reads between 0 and ~3 volts, producing a corresponding value
  // between 0 and 1024. The equation below will convert the value to a voltage value.



  analogVolts = (analogValue * 3.08) / 1024;  // DG - not accurate ... can maybe impprove if necessary

  // now get our chart value by converting the analog (0-1024) value to a value between 0 and 100.
  // the value of the const "dryAnalog" is derived by using a dry moisture sensor (not in soil, just in air) and "wetAnalog" was determined by putting the sensor is a glass of water.-DG
    
    int chartValue = (dryAnalog - analogValue) * 100 / (dryAnalog - wetAnalog);

  // set a "blink" time interval in milliseconds.
  // for example, 15000 is 15 seconds. However, the blink will not always be 15 seconds due to other
  // delays set in the code.

  if (millis() - 15000 > timeHolder)
  {
    timeHolder = millis();

    // determine which color to use with the LED based on the chartValue.
    // note: PWM is used so any color combo desired can be set by changing the values sent to each pin
    // between 0 and 1024 - 0 being OFF and 1024 being full power
    ////   yellowish
    //  analogWrite(redPin, 900);
    //  analogWrite(greenPin, 1010);
    //  analogWrite(bluePin, 100);

    if (chartValue <= 25) {  // 0-25 is red "dry"

      analogWrite(redPin, 1000);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 0);

    } else if (chartValue > 25 && chartValue <= 75) // 26-75 is green
    {

      analogWrite(redPin, 0);
      analogWrite(greenPin, 1000);
      analogWrite(bluePin, 0);

    }
    else if (chartValue > 75 ) // 76-100 is blue
    {

      analogWrite(redPin, 0);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 1000);

    }

    delay(1500); // this is the duration the LED will stay ON

    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);

  }

  // Serial data
  Serial.print("Chart Value: ");
  Serial.println(chartValue);
  Serial.print("Analog raw: ");
  Serial.println(analogValue);
  Serial.print("Analog V: ");
  Serial.println(analogVolts);
  Serial.print("TimeHolder: ");
  Serial.println(timeHolder);
  Serial.print("millis(): ");
  Serial.println(millis());
  Serial.print("WiFi Strength: ");
  Serial.print(WiFiStrength); Serial.println("dBm");
  Serial.println(" ");
  delay(2000); // slows amount of data sent via serial   - DG

  // check to for any web server requests. ie - browser requesting a page from the webserver
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Return the response
  "HTTP/1.1 200 OK");
  "Content-Type: text/html");
  ""); //  do not forget this one
  "<!DOCTYPE HTML>");

  "<html>");
  " <head>");
  "<meta http-equiv=\"refresh\" content=\"60\">");
  " <script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>");
  "  <script type=\"text/javascript\">");
  "    google.charts.load('current', {'packages':['gauge']});");
  "    google.charts.setOnLoadCallback(drawChart);");
  "   function drawChart() {");

  "      var data = google.visualization.arrayToDataTable([ ");
  "        ['Label', 'Value'], ");
  client.print("        ['Moisture',  ");
  client.print(chartValue);
  " ], ");
  "       ]); ");
  // setup the google chart options here
  "    var options = {");
  "      width: 400, height: 120,");
  "      redFrom: 0, redTo: 25,");
  "      redColor: '#FF3232',");
  "      greenFrom: 25, greenTo: 75,");
  "      greenColor: '#46C646',");
  "      yellowFrom: 75, yellowTo: 100,");
  "      yellowColor: '#1589FF',");
  "       minorTicks: 5");
  "    };");

  "   var chart = new google.visualization.Gauge(document.getElementById('chart_div'));");

  "  chart.draw(data, options);");

  "  setInterval(function() {");
  client.print("  data.setValue(0, 1, ");
  client.print(chartValue);
  "    );");
  "    chart.draw(data, options);");
  "    }, 13000);");


  "  }");
  " </script>");

  "  </head>");
  "  <body>");

  client.print("<h1 style=\"size:12px;\">ESP8266 Soil Moisture</h1>");

  // show some data on the webpage and the guage
  "<table><tr><td>");

  client.print("WiFi Signal Strength: ");
  WiFiStrength);
  "dBm<br>");
  client.print("Analog Raw: ");
  analogValue);
  client.print("<br>Analog Volts: ");
  analogVolts);
  "<br><a href=\"/REFRESH\"\"><button>Refresh</button></a>");

  "</td><td>");
  // below is the google chart html
  "<div id=\"chart_div\" style=\"width: 300px; height: 120px;\"></div>");
  "</td></tr></table>");

  "<body>");
  "</html>");
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");


}
