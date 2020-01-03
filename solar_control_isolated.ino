#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_MCP3008.h>
#include "read-file.h"

const char* ssid = "";
const char* password = "";

IPAddress local_ip(192, 168, 12, 1);
IPAddress gateway(192, 168, 12, 1);
IPAddress subnet(255, 255, 255, 0);

ESP8266WebServer server(80);
Adafruit_MCP3008 adc;
bool motor[4] = {true, false, false, false};

String main_template;
String homepage_body;
String settings_body;
String notification;
String debug_body;

String setting = "Motor on auto mode.";
int resolution;
float minutes;
int cutoff;

bool write_minutes = false;
bool write_resolution = false;
bool write_cutoff = false;

unsigned long previousTime = 0;
unsigned long timeElapsed = 0;

//render a webpage for this project:
String render(String body, String refresh = "") {
  String main_temp = main_template;
  main_temp.replace("{{refresh}}", refresh);
  main_temp.replace("{{body}}", body);
  return main_temp;
}

//homepage
String homepage;
String settingspage;
String debugpage;

void motorLeft() {
  digitalWrite(D4, LOW);
  digitalWrite(D3, HIGH);
}

void motorRight() {
  digitalWrite(D3, LOW);
  digitalWrite(D4, HIGH);
}

void motorStop() {
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);
}

void setup() {
  SPIFFS.begin();
  Serial.begin(115200);
  Serial.println("");
  
  readFile("/header.html", main_template);
  readFile("/homepage.html", homepage_body);
  readFile("/settings.html", settings_body);
  readFile("/notification.html", notification);
  readFile("/debug.html", debug_body);

  String _buffer;
  if (readFile("/minuites.config", _buffer)) {
    minutes = 10.0f;
    writeFile("/minuites.config", "10.0");
  }
  else {
    minutes = _buffer.toFloat();
  }
  
  if (readFile("/resolution.config", _buffer)) {
    resolution = 1;
    writeFile("/resolution.config", "1");
  }
  else {
    resolution = _buffer.toInt();
  }
  
  if (readFile("/cutoff.config", _buffer)) {
    cutoff = 100;
    writeFile("/cutoff.config", "100");
  }
  else {
    cutoff = _buffer.toInt();
  }
 
  homepage = render(homepage_body);
  settingspage = render(settings_body);
  debugpage = render(debug_body, "<meta http-equiv=\"refresh\" content=\"5\">");

  // Clock pin: D8, D-Out: D7, D-In: D6, CS: D5.
  adc.begin(D8, D6, D7, D5);

  //motor driver pins
  pinMode(D4, OUTPUT);
  pinMode(D3, OUTPUT);

  //Communication setup.

  // Create Access Point
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);

  server.on("/", handle_home);
  server.on("/settings", handle_settings);
  server.on("/debug", handle_debug);
  server.begin();
  Serial.println("Server listening:");

}

void loop() {
  if (write_minutes) {
    writeFile("/minuites.config", (String)minutes);
    write_minutes = false;
  }
  else if (write_resolution) {
    writeFile("/resolution.config", (String)resolution);
    write_resolution = false;
  }
  else if (write_cutoff) {
    writeFile("/cutoff.config", (String)cutoff);
    write_cutoff = false;
  }

  server.handleClient();
  unsigned long currentTime = millis();
  unsigned long elapsedTime =  currentTime - previousTime;

  static int8_t motor_dir = 0;
  if (!motor[0]) { motor_dir = 0; }

  if (motor[0]) {
    timeElapsed += elapsedTime;

    if (motor_dir == 1) {
      if ((adc.readADC(0) <= cutoff || adc.readADC(1) <= cutoff) || (adc.readADC(0) / resolution <= adc.readADC(1) / resolution)) {
        motorStop();
        motor_dir = 0;
      }
    }
    else if (motor_dir) {
      if ((adc.readADC(0) <= cutoff || adc.readADC(1) <= cutoff) || (adc.readADC(0) / resolution >= adc.readADC(1) / resolution)) {
        motorStop();
        motor_dir = 0;
      }
    }
    else {
      // Only activate function periodically depending on the time set by the user.
      if (float(timeElapsed) / 60000.0f >= minutes) {
        motorStop();
        timeElapsed = 0;
        if (adc.readADC(0) > cutoff || adc.readADC(1) > cutoff) {
          if (adc.readADC(0) / resolution > adc.readADC(1) / resolution) {
            motorRight();
            motor_dir = 1;
          }
          if (adc.readADC(0) / resolution < adc.readADC(1) / resolution) {
            motorLeft();
            motor_dir = -1;
          }
        }
      }
    }
  }
  else if (motor[1]) {
    motorLeft();
  }
  else if (motor[2]) {
    motorRight();
  }
  else if (motor [3]) {
    motorStop();
  }
  previousTime = currentTime;
}

void handle_home() {
  String temp_note = notification;
  String temp_homepage = homepage;
  if (server.arg("left-att") == "left") {
    setting = "Motor moving left.";
    motor[1] = true;
    motor[0] = motor[2] = motor[3] = false;
  }
  if (server.arg("right-att") == "right") {
    setting = "Motor moving right.";
    motor[2] = true;
    motor[1] = motor[0] = motor[3] = false;
  }
  if (server.arg("auto-att") == "auto") {
    setting = "Motor on auto mode.";
    motor[0] = true;
    motor[1] = motor[2] = motor[3] = false;
  }
  if (server.arg("stop-att") == "stop") {
    setting = "Motor is stopped.";
    motor[3] = true;
    motor[1] = motor[2] = motor[0] = false;
  }
  temp_note.replace("{{note}}", setting);
  temp_homepage.replace("{{motor_status}}", temp_note);
  server.send(200, "text/html", temp_homepage);
}

void handle_settings() {
  //main settings page.
  String temp_settingspage = settingspage;

  //notification versions
  String temp_note_res = notification;
  String temp_note_min = notification;
  String temp_note_cutoff = notification;

  //get server variables.
  int temp_res = server.arg("resoultion").toInt();
  float temp_min = server.arg("minutes").toFloat();
  int temp_cutoff = server.arg("cutoff").toInt();

  if (temp_res and temp_res != resolution) {
    if (temp_res >= 1 and temp_res <= 64) {
      resolution = temp_res;
      write_resolution = true;
    }
  }

  if (temp_min and temp_min != minutes) {
    if (temp_min >= 0.10f) {
      minutes = temp_min;
      write_minutes = true;
    }
  }

  if (temp_cutoff and temp_cutoff != cutoff) {
    if (temp_cutoff >= 0) {
      cutoff = temp_cutoff;
      write_cutoff = true;
    }
  }

  String min_message = "Minutes: " + String(minutes);
  String res_message = "Resolution: " + String(resolution);
  String cutoff_message = "Cutoff: " + String(cutoff);

  temp_note_res.replace("{{note}}", res_message);
  temp_note_min.replace("{{note}}", min_message);
  temp_note_cutoff.replace("{{note}}", cutoff_message);

  temp_settingspage.replace("{{res_num}}", temp_note_res);
  temp_settingspage.replace("{{min_num}}", temp_note_min);
  temp_settingspage.replace("{{cutoff_num}}", temp_note_cutoff);

  server.send(200, "text/html", temp_settingspage);
}

void handle_debug() {
  //debug html
  String temp_debugpage = debugpage;
  String s1_note = notification;
  String s2_note = notification;

  String message_1 = "Solar 1 value: " + String(adc.readADC(0));
  String message_2 = "Solar 2 Value: " + String(adc.readADC(1));

  s1_note.replace("{{note}}", message_1);
  s2_note.replace("{{note}}", message_2);

  temp_debugpage.replace("{{sensor_1}}", s1_note);
  temp_debugpage.replace("{{sensor_2}}", s2_note);

  server.send(200, "text/html", temp_debugpage);
}
