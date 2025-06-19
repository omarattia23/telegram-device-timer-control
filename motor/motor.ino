#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <auth.h> // Your authentication file 
// ------------------------------------------------------------------------------------
// DEFINES
// ------------------------------------------------------------------------------------
String chat_id;

// ------------------------------------------------------------------------------------
// This is the Wifi client that supports HTTPS
// ------------------------------------------------------------------------------------
WiFiClientSecure client;
UniversalTelegramBot motor_bot(motor_bot_token, client);


// ------------------------------------------------------------------------------------
unsigned long lastTimeChecked = 0;
const int delayBetweenChecks = 1000;
unsigned long timerExpires = 0;
bool timerActive = false;

// ------------------------------------------------------------------------------------
void handleNewMessages(int numNewMessages, UniversalTelegramBot& bot);
void setupWifi();
void update_msgTime(int chat_id, int msg_id);


void setup() {
  Serial.begin(9600);

  setupWifi();

  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(DOOR_PIN, OUTPUT);
  pinMode(WIFI_LED_PIN, OUTPUT);

  digitalWrite(MOTOR_PIN, HIGH);  // Assume HIGH is OFF
  digitalWrite(DOOR_PIN, HIGH);   // Assume HIGH is OFF



  client.setInsecure();

  motor_bot.longPoll = 60;
}


void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    digitalWrite(WIFI_LED_PIN, LOW);
    setupWifi();
  }
  digitalWrite(WIFI_LED_PIN, HIGH);

  unsigned long currentTime = millis();

  if (currentTime - lastTimeChecked > delayBetweenChecks) {
    int numNewMessages = motor_bot.getUpdates(motor_bot.last_message_received + 1);
    if (numNewMessages) {
      Serial.println("Got response from motor bot");
      handleNewMessages(numNewMessages, motor_bot);
    }


    lastTimeChecked = currentTime;

    if (timerActive && currentTime > timerExpires) {
      digitalWrite(MOTOR_PIN, HIGH);  // Turn off motor
      timerActive = false;

      motor_bot.sendMessage(chat_id, "Motor is currently OFF ❌");
    }
  }
}
//===========================================================================================================
//============================================= Logic ======================================================
//===========================================================================================================
// connect WI-FI
void setupWifi() {
  digitalWrite(WIFI_LED_PIN, LOW);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nWiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());
  digitalWrite(WIFI_LED_PIN, HIGH);
}
// Handle the new msgs
void handleNewMessages(int numNewMessages, UniversalTelegramBot& bot) {
  String motorKeyboardJson = F("[[{ \"text\" : \"OFF\", \"callback_data\" : \"OFF\" }],"
                               "[{ \"text\" : \"3 Mins\", \"callback_data\" : \"TIME3\" },{ \"text\" : \"5 Mins\", \"callback_data\" : \"TIME5\" },{ \"text\" : \"10 Mins\", \"callback_data\" : \"TIME10\" }],"
                               "[{ \"text\" : \"20 Mins\", \"callback_data\" : \"TIME20\" }],"
                               "[{ \"text\" : \"Fill the empty Tank\", \"callback_data\" : \"TIME35\" }],"
                               "[{\"text\" : \"Open The Door\", \"callback_data\":\"OPEN\"}]]");

  for (int i = 0; i < numNewMessages; i++) {
    chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    Serial.print("Text = ");
    Serial.println(text);
    Serial.println("type = " + bot.messages[i].type);
    if (bot.messages[i].type == F("message")) {

      Serial.print("button pressed with data: ");
      Serial.println(text);

      if (text == F("OFF")) {
        digitalWrite(MOTOR_PIN, HIGH);
        timerActive = false;
        // motor_bot.sendMessageWithReplyKeyboard(chat_id, "Motor is currently OFF ❌", "", motorKeyboardJson);
        bot.sendMessage(chat_id, "Motor is currently OFF ❌");
      } else if (text == "3 Mins" || text == "5 Mins" || text == "10 Mins" || text == "20 Mins" || text == "Fill the empty Tank") {
        if (text == "Fill the empty Tank") text = "35";
        text.replace("Mins ", "");
        int timeRequested = text.toInt();
        digitalWrite(MOTOR_PIN, LOW);
        timerActive = true;
        timerExpires = millis() + (timeRequested * 1000 * 60);
        // bot.sendMessageWithReplyKeyboard(chat_id, "Motor is currently set to " + text + " Mins ✅", "", motorKeyboardJson);
        String msg = String("Motor is currently set to " + text + " Mins ✅");
        Serial.println(msg);
        bot.sendMessage(chat_id, msg);

      } else if (text == F("Open The Door")) {
        Serial.println("Test");
        digitalWrite(DOOR_PIN, LOW);
        bot.sendMessage(chat_id, "Door is OPEN ✅");
        delay(200);
        digitalWrite(DOOR_PIN, HIGH);

      } else if (text == F("/start")) {
        bot.sendMessageWithReplyKeyboard(chat_id, "Mr.Bot", "", motorKeyboardJson, true);
      }
      // } else if (text == F("/Door")) {

      //   String keyboardJson = F("[[{\"text\": \"OPEN\", \"callback_data\": \"OPEN\"}]]");
      //   bot.sendMessageWithReplyKeyboard(chat_id, "DOOR", "", keyboardJson,true);
    }
  }
}
