#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <Adafruit_NeoPixel.h>

using namespace websockets;

// ----------- USER CONFIGURATIONS -------------
const char* SSID     = "Hacker";      // <-- Replace with your Wi-Fi SSID
const char* PASSWORD = "mo123456";  // <-- Replace with your Wi-Fi password

// For example: ws://192.168.1.50:8080
// The library wants just the host and port separately (and optional path).
const char* WS_HOST  = "192.168.52.34";   // <-- Replace with your WebSocket server IP
const uint16_t WS_PORT = 8080;           // <-- Replace with your WebSocket server port
const char* WS_PATH = "/";               // WebSocket path (often just "/")

// ----------- MOTOR DRIVER PIN DEFINITIONS -------------
#define M1A 13
#define M1B 12
#define M2A 11
#define M2B 10

// ----------- NEOPIXEL DEFINITIONS -------------
#define NEOPIXEL_PIN 14
#define NUM_PIXELS   64  // 8x8 LED array

// Create NeoPixel strip object
Adafruit_NeoPixel strip(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// ----------- GLOBALS FOR BLINKING ORANGE -------------
unsigned long previousBlinkMillis = 0;
const unsigned long blinkInterval = 500;  // 0.5 second toggle
bool orangeOn = false;

// ----------- GLOBALS FOR MOVING ARROWS -------------
unsigned long previousArrowMillis = 0;
const unsigned long arrowAnimInterval = 150;  // Arrow animation speed (ms)
int arrowPosition = 0;  // Current position in the animation
bool isMovingForward = false;  // Flag to track if we're in forward motion mode

// ----------- WEBSOCKET CLIENT -------------
WebsocketsClient client;

// ----------- FUNCTION DECLARATIONS -------------
void setupMotors();
void moveForward();
void stopMotors();
void setupNeopixel();
void setNeoPixelColor(uint32_t color);
void blinkOrange();
void connectToWifi();
void connectToWebSocket();
void onMessageCallback(WebsocketsMessage message);
void drawGreenArrows();
void drawStopSign();
void updateArrowAnimation();
int getRotatedPixelIndex(int row, int col);

// ------------------------------------------------

void setup() {
  Serial.begin(115200);

  // Initialize hardware
  setupMotors();
  setupNeopixel();

  // Connect to Wi-Fi (blinking orange while connecting)
  connectToWifi();

  // Connect to WebSocket server (still blinking orange while connecting)
  connectToWebSocket();

  // Once connected: go solid green for 1 second
  Serial.println("Connected! Showing solid green...");
  setNeoPixelColor(strip.Color(0, 255, 0)); // Green
  delay(1000);

  // After the 1s green indication, clear or show something neutral if desired
  // We'll turn off all pixels to start "idle"
  setNeoPixelColor(strip.Color(0, 0, 0)); // Off
}

void loop() {
  // Continuously listen for incoming WebSocket messages
  // This is non-blocking and has no extra delay
  client.poll();
  
  // If we're in forward motion mode, update the arrow animation
  if (isMovingForward) {
    updateArrowAnimation();
  }
}

// ------------------------------------------------
//          PIXEL COORDINATE CONVERSION FUNCTION
// ------------------------------------------------

// Convert row and column to pixel index with 90-degree rotation
// Rotation is clockwise, assuming standard 8x8 arrangement
int getRotatedPixelIndex(int row, int col) {
  // In a 90-degree clockwise rotation:
  // new_row = col
  // new_col = 7 - row
  
  int newRow = col;
  int newCol = 7 - row;
  
  // Calculate index based on the new coordinates
  return newRow * 8 + newCol;
}

// ------------------------------------------------
//                CONNECTION FUNCTIONS
// ------------------------------------------------

void connectToWifi() {
  Serial.printf("Connecting to WiFi SSID: %s\n", SSID);
  WiFi.begin(SSID, PASSWORD);

  // Blink orange until connected
  while (WiFi.status() != WL_CONNECTED) {
    blinkOrange(); // Non-blocking blink
    delay(10);     // Small yield to avoid watchdog reset
  }
  Serial.println("WiFi connected!");
}

void connectToWebSocket() {
  Serial.println("Connecting to WebSocket...");

  // Optional: set event handlers/callbacks
  client.onMessage(onMessageCallback);

  // Attempt connection
  while (!client.connect(WS_HOST, WS_PORT, WS_PATH)) {
    blinkOrange(); // Non-blocking blink
    delay(10);     // Small yield
  }
  Serial.println("WebSocket connected!");
}

// ------------------------------------------------
//         WEBSOCKET MESSAGE HANDLING
// ------------------------------------------------

void onMessageCallback(WebsocketsMessage message) {
  // Read the text message
  String msg = message.data();
  Serial.print("Message received: ");
  Serial.println(msg);

  if (msg == "1") {
    // Move forward (animate green arrows)
    moveForward();
  } else if (msg == "0") {
    // Stop (display stop sign)
    stopMotors();
  } else {
    Serial.println("Unknown command!");
  }
}

// ------------------------------------------------
//                MOTOR FUNCTIONS
// ------------------------------------------------

void setupMotors() {
  pinMode(M1A, OUTPUT);
  pinMode(M1B, OUTPUT);
  pinMode(M2A, OUTPUT);
  pinMode(M2B, OUTPUT);
}

// Move motors forward and set animation flag
void moveForward() {
  digitalWrite(M1A, HIGH);
  digitalWrite(M1B, LOW);
  digitalWrite(M2A, HIGH);
  digitalWrite(M2B, LOW);
  
  // Set the flag to enable arrow animation in the loop
  isMovingForward = true;
  
  // Reset animation position
  arrowPosition = 0;
  
  // Immediately draw the initial frame
  drawGreenArrows();
}

// Stop motors and display stop sign
void stopMotors() {
  digitalWrite(M1A, LOW);
  digitalWrite(M1B, LOW);
  digitalWrite(M2A, LOW);
  digitalWrite(M2B, LOW);
  
  // Disable the arrow animation
  isMovingForward = false;
  
  // Show stop sign
  drawStopSign();
}

// ------------------------------------------------
//               NEOPIXEL FUNCTIONS
// ------------------------------------------------

void setupNeopixel() {
  strip.begin();
  strip.show(); // Initialize all pixels to off
}

// Set the entire NeoPixel array to a single color
void setNeoPixelColor(uint32_t color) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

// Blink orange (non-blocking) by toggling every blinkInterval ms
void blinkOrange() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkMillis >= blinkInterval) {
    previousBlinkMillis = currentMillis;
    orangeOn = !orangeOn;

    if (orangeOn) {
      // Orange color = (R=255, G=165, B=0) => approximate
      setNeoPixelColor(strip.Color(255, 165, 0));
    } else {
      // Turn off
      setNeoPixelColor(strip.Color(0, 0, 0));
    }
  }
}

// ------------------------------------------------
//              ARROW ANIMATION FUNCTIONS
// ------------------------------------------------

// Non-blocking arrow animation update
void updateArrowAnimation() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousArrowMillis >= arrowAnimInterval) {
    previousArrowMillis = currentMillis;
    
    // Move to next frame in the animation
    arrowPosition = (arrowPosition + 1) % 8;  // Cycle through 8 positions
    
    // Draw the new frame
    drawGreenArrows();
  }
}

// Draw the green arrows at current position, rotated 90 degrees clockwise
void drawGreenArrows() {
  // First clear the display
  strip.clear();
  
  // The bright green color for the arrows
  uint32_t arrowColor = strip.Color(0, 255, 0);  // Bright green
  uint32_t dimColor = strip.Color(0, 40, 0);     // Dim green for background arrows
  
  // With 90-degree rotation, our arrows will now point to the right (not up)
  // Drawing logic must be updated
  
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      // First, set a dim background pattern for all arrows
      // Since we're rotated 90 degrees, we're drawing right-pointing arrows
      
      // Middle horizontal arrow lines (previously vertical)
      if ((col == 3 || col == 4) && (row >= 2 && row <= 5)) {
        strip.setPixelColor(getRotatedPixelIndex(row, col), dimColor);
      }
      // Top and bottom diagonals (previously left and right)
      else if ((col == 2 && row == 1) || (col == 5 && row == 1) || 
               (col == 1 && row == 0) || (col == 6 && row == 0) ||
               (col == 2 && row == 6) || (col == 5 && row == 6) ||
               (col == 1 && row == 7) || (col == 6 && row == 7)) {
        strip.setPixelColor(getRotatedPixelIndex(row, col), dimColor);
      }
      
      // Now, highlight the current animated arrow based on position
      // Animation will now move from left to right
      int highlightCol = (arrowPosition + col) % 8;  // Animation moves rightward
      
      // Middle horizontal arrow lines - highlight current position
      if ((highlightCol == 3 || highlightCol == 4) && (row >= 2 && row <= 5)) {
        strip.setPixelColor(getRotatedPixelIndex(row, col), arrowColor);
      }
      // Top and bottom diagonals - highlight current position
      else if ((highlightCol == 2 && row == 1) || (highlightCol == 5 && row == 1) || 
               (highlightCol == 1 && row == 0) || (highlightCol == 6 && row == 0) ||
               (highlightCol == 2 && row == 6) || (highlightCol == 5 && row == 6) ||
               (highlightCol == 1 && row == 7) || (highlightCol == 6 && row == 7)) {
        strip.setPixelColor(getRotatedPixelIndex(row, col), arrowColor);
      }
    }
  }
  
  // Show the updated pattern
  strip.show();
}

// ------------------------------------------------
//              STOP SIGN DISPLAY FUNCTION
// ------------------------------------------------

// Draw a stop sign on the 8x8 matrix, rotated 90 degrees clockwise
void drawStopSign() {
  // First clear the display
  strip.clear();
  
  // Red color for the stop sign
  uint32_t stopColor = strip.Color(255, 0, 0);  // Bright red
  uint32_t exclamationColor = strip.Color(255, 255, 255);  // White for the exclamation mark
  
  // Create an octagonal border and fill for the stop sign
  // For an 8x8 matrix, we'll make a simplified stop sign pattern
  // This pattern is already designed in the original orientation
  // Our rotation function will handle the 90-degree turn
  
  // Octagon outline - simplified for 8x8
  uint8_t stopSignPattern[8][8] = {
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 0, 0, 1, 1, 1},
    {1, 1, 1, 0, 0, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0}
  };
  
  // Exclamation mark inside the stop sign
  // 1 in the pattern means red part of stop sign
  // 2 in the pattern means white exclamation mark
  
  // Add exclamation mark
  stopSignPattern[2][3] = 2;
  stopSignPattern[2][4] = 2;
  stopSignPattern[3][3] = 2;
  stopSignPattern[3][4] = 2;
  stopSignPattern[4][3] = 2;
  stopSignPattern[4][4] = 2;
  stopSignPattern[6][3] = 2;
  stopSignPattern[6][4] = 2;
  
  // Apply the pattern to the LED array with rotation
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      // Get the rotated pixel index
      int pixelIndex = getRotatedPixelIndex(row, col);
      
      if (stopSignPattern[row][col] == 1) {
        strip.setPixelColor(pixelIndex, stopColor);
      } else if (stopSignPattern[row][col] == 2) {
        strip.setPixelColor(pixelIndex, exclamationColor);
      }
      // Pixels with 0 remain off
    }
  }
  
  // Show the stop sign
  strip.show();
}