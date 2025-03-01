#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <ESP32Servo.h>
#include <Adafruit_NeoPixel.h>

using namespace websockets;

// ---------------------- USER CONFIGURATIONS ----------------------
const char* SSID     = "Hacker";           // Replace with your Wi-Fi SSID
const char* PASSWORD = "mo123456";         // Replace with your Wi-Fi password

// WebSocket server settings (e.g., ws://192.168.52.34:8080/)
const char* WS_HOST  = "192.168.52.34";    // Replace with your WebSocket server IP
const uint16_t WS_PORT = 8080;             // Replace with your WebSocket server port
const char* WS_PATH  = "/";                // WebSocket path (often just "/")

// Timeouts (ms)
#define WIFI_TIMEOUT 15000  // 15 seconds
#define WS_TIMEOUT   15000  // 15 seconds

WebsocketsClient client;

// ---------------------- Servo Configuration ------------------------
#define SERVO_PIN 13    // Continuous (360°) servo on GPIO 13
Servo myServo;          // Servo object

// ------------------- NeoPixel Matrix Configuration -----------------
#define LED_PIN    12   // NeoPixel matrix on GPIO 12
#define NUM_PIXELS 64   // 8x8 LED matrix (64 LEDs)
Adafruit_NeoPixel strip(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// ---------------- NeoPixel Rapid Color Cycling Globals --------------
unsigned long lastMatrixUpdate = 0;
const unsigned long matrixInterval = 10;    // Update interval in ms for rapid color changes
uint16_t hue = 0;

// ---------------- Global Variables for Blinking Indicators -----------
unsigned long previousBlinkMillis = 0;
const unsigned long blinkInterval = 500;    // 0.5 second toggle interval
bool blinkState = false;                    // Used to track LED on/off in blinking

// ---------------- Activation Flag for Servo & NeoPixel ----------------
bool activated = false;  // true => servo spins & NeoPixels cycle; false => all off

// ------------------- Function Declarations -------------------------
void connectToWifi();
void connectToWebSocket();
void onMessageCallback(WebsocketsMessage message);
void spinServoContinuously();
void rapidNeoPixelColors();
void stopServoAndNeopixel();
void setNeoPixelColor(uint32_t color);
void blinkYellow();
void blinkPurple();

// -------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("ESP32: Servo + Rapid NeoPixel Color Cycling with WebSocket Control");

  // Initialize servo and set it to neutral (stopped) position
  myServo.attach(SERVO_PIN);
  myServo.write(90); // Neutral position for a continuous servo

  // Initialize NeoPixel matrix
  strip.begin();
  strip.setBrightness(255); // Max brightness (for testing)
  strip.show();             // Ensure all pixels are off initially

  // Connect to Wi-Fi (blinking yellow while connecting)
  connectToWifi();

  // Connect to WebSocket server (blinking purple while connecting)
  connectToWebSocket();
}

// -------------------------------------------------------------------
void loop() {
  // Continuously poll for incoming WebSocket messages
  client.poll();

  // If activated, spin the servo and cycle NeoPixels rapidly
  if (activated) {
    spinServoContinuously();
    rapidNeoPixelColors();
  } else {
    // Otherwise, stop the servo and turn off NeoPixels
    stopServoAndNeopixel();
  }
}

// --------------------- Connection Functions --------------------------
void connectToWifi() {
  Serial.printf("Connecting to WiFi SSID: %s\n", SSID);
  WiFi.begin(SSID, PASSWORD);

  unsigned long startAttemptTime = millis();
  previousBlinkMillis = 0;  // Reset blink timer

  // Blink yellow until connected or timeout
  while (WiFi.status() != WL_CONNECTED && (millis() - startAttemptTime < WIFI_TIMEOUT)) {
    blinkYellow();
    delay(10);  // Small delay to avoid watchdog reset
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected!");
    // Show solid green for 2 seconds
    setNeoPixelColor(strip.Color(0, 255, 0));
    delay(2000);
    // Clear
    setNeoPixelColor(strip.Color(0, 0, 0));
  } else {
    Serial.println("WiFi connection failed!");
    // Turn red and halt
    setNeoPixelColor(strip.Color(255, 0, 0));
    while(true) { delay(1000); }  // Stop here
  }
}

void connectToWebSocket() {
  Serial.println("Connecting to WebSocket...");
  client.onMessage(onMessageCallback);

  // Build the full ws:// URL: e.g. "ws://192.168.52.34:8080/"
  String wsUrl = String("ws://") + WS_HOST + ":" + WS_PORT + WS_PATH;

  unsigned long startAttemptTime = millis();
  previousBlinkMillis = 0;  // Reset blink timer
  bool wsConnected = false;

  // Attempt connection until success or timeout
  while (!wsConnected && (millis() - startAttemptTime < WS_TIMEOUT)) {
    wsConnected = client.connect(wsUrl);
    if (!wsConnected) {
      blinkPurple();
      delay(10);
    }
  }

  if (wsConnected) {
    Serial.println("WebSocket connected!");
    // Show solid green for 2 seconds
    setNeoPixelColor(strip.Color(0, 255, 0));
    delay(2000);
    // Clear
    setNeoPixelColor(strip.Color(0, 0, 0));
  } else {
    Serial.println("WebSocket connection failed!");
    // Turn red and halt
    setNeoPixelColor(strip.Color(255, 0, 0));
    while(true) { delay(1000); }  // Stop here
  }
}

// ---------------- WebSocket Message Handling -------------------------
void onMessageCallback(WebsocketsMessage message) {
  String msg = message.data();
  Serial.print("Message received: ");
  Serial.println(msg);

  if (msg == "1") {
    // Activate servo + rapid NeoPixel cycling
    activated = true;
  } else if (msg == "0") {
    // Deactivate: stop servo and turn off NeoPixels
    activated = false;
  } else {
    Serial.println("Unknown command!");
  }
}

// ---------------- Servo & NeoPixel Functions -------------------------
void spinServoContinuously() {
  // For a continuous servo, 180 is typically full speed forward
  myServo.write(180);
}

void rapidNeoPixelColors() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastMatrixUpdate >= matrixInterval) {
    lastMatrixUpdate = currentMillis;
    // Convert the current hue to an RGB color (full saturation & brightness)
    uint32_t color = strip.ColorHSV(hue, 255, 255);
    for (int i = 0; i < NUM_PIXELS; i++) {
      strip.setPixelColor(i, color);
    }
    strip.show();
    hue += 3000;  // Change the hue quickly for rapid color cycling
  }
}

void stopServoAndNeopixel() {
  // Neutral (90) for a continuous servo is typically "stop"
  myServo.write(90);
  setNeoPixelColor(strip.Color(0, 0, 0));
}

// ------------------- NeoPixel Utility Functions ----------------------
void setNeoPixelColor(uint32_t color) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

// ------------------- Blinking Indicator Functions --------------------
// Blink yellow (R=255, G=255, B=0) for WiFi connection indication
void blinkYellow() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkMillis >= blinkInterval) {
    previousBlinkMillis = currentMillis;
    blinkState = !blinkState;
    if (blinkState) {
      setNeoPixelColor(strip.Color(255, 255, 0));
    } else {
      setNeoPixelColor(strip.Color(0, 0, 0));
    }
  }
}

// Blink purple (R=128, G=0, B=128) for WebSocket connection indication
void blinkPurple() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkMillis >= blinkInterval) {
    previousBlinkMillis = currentMillis;
    blinkState = !blinkState;
    if (blinkState) {
      setNeoPixelColor(strip.Color(128, 0, 128));
    } else {
      setNeoPixelColor(strip.Color(0, 0, 0));
    }
  }
}
