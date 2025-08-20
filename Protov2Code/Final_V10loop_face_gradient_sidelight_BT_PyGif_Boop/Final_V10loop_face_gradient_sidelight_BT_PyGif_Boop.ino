#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
// #include <Wire.h>                 // For I2C communication
// #include <Adafruit_LIS3DH.h>      // For accelerometer
// #include <Adafruit_PixelDust.h>   // For sand simulation
#include <Adafruit_Protomatter.h> // For RGB matrix
#include <cstdint>
#include <cmath>
#include <SD.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_VCNL4040.h>




//// Side Light Stuff
#define NEOPIX_PIN       A0      // Signal pin is A0
#define NEOPIX_NUMPIXELS 24      // 24 LEDs on your ring
const int NEOPIX_NUMPIXELS2 = (int)(NEOPIX_NUMPIXELS/2);
#define NEOPIX_BRIGHTNESS 200     // value between 0 and 255 
#define NUM_STEPS_SIDE_ITER 2    // Number of timesteps to wait for each iteration
int SIDE_INCREMENT_VALUE = 1;        // Value to increment each NUM_STEPS_SIDE_ITER
bool side_iter = true;
const bool rotate = true;
const bool invert_side = false;
// Initialize side light
Adafruit_NeoPixel neopixels(NEOPIX_NUMPIXELS, NEOPIX_PIN, NEO_GRBW + NEO_KHZ800);
// Stores the RGBW timesteps
int side_light_WRGB_time[NEOPIX_NUMPIXELS];
// Used to count the number of steps taken for the side
unsigned long num_steps_side = 0;
uint16_t zero_pix = 0x0;




//// Bluetooth stuff
// BLE Service and Characteristic UUIDs
#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define MODE_CHAR_UUID      "87654321-4321-4321-4321-0987654321ba"
#define BLE_service_name    "Protogen UwU >w<"

BLEServer *bleServer = nullptr;
BLECharacteristic *modeCharacteristic = nullptr;
bool deviceConnected = false;

// Modes
// 1 - rainbow
// 2 - gradient
// side
String currentMode = "1"; // Default mode

void RestartBLE() {
  // Start the service
  // bleService->start();

  // Start advertising
  bleServer->getAdvertising()->start();
  Serial.println("BLE Advertising started!");
}

// BLE Server Callbacks
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Device connected!");
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Device disconnected!");

    // If disconnected, readvertise
    RestartBLE();
  }
};

// Characteristic Callback
class ModeCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();
    if (value.length() > 0) {
      Serial.println(value);
      Serial.println(value.c_str());
      currentMode = value.c_str(); // Convert to integer
      Serial.print("Mode changed to: ");
      Serial.println(currentMode);
    }
  }
};







//// MATRIX STUFF
#define HEIGHT  32 // Matrix height (pixels)
#define WIDTH   128 // Matrix width (pixels)
#define WIDTH2  64

// Used for face gradient
const uint8_t gradient_R1 = 156;
const uint8_t gradient_G1 = 9;
const uint8_t gradient_B1 = 185;
const uint8_t gradient_R2 = 55;
const uint8_t gradient_G2 = 249;
const uint8_t gradient_B2 = 120;

// Loop mode can either be a loop or one that reverses itself
char* LOOP_MODE = "reversed_loop"; // Either "loop" or "reversed_loop"
int loop_addition = 1;


// Load in the bitmaps
#include "bitmaps/default.h"
// #include "bitmaps/bk_lick.h"


// Boop bitmap
#include "bitmaps/boop_face.h"
// Threshold for the boop to be positive
const int BOOP_PROXIMITY_THRESH = 5;
// Is a boop currently happening
bool is_booping = false;
bool is_booping_prev = false; // Used to know if the face screen should be reset
// Keeps count of how many boops have happened in a row.
int is_booping_counter = 0;
// delay steps to see if the boop is noise or not
int boop_delay_steps_pos = 0; // Num iters for a positive signal to turn flag true
int boop_delay_steps_neg = 0;// Num iters for a negative signal to turn flag false
// Keeps count for the number of frames in boop mode for interpolating
int num_steps_boop_mode = 0;
// Number of iters to interpolate into boop mode
int num_iters_boop = 0;
// Number of steps to skip during transition
const int NUM_TRANSITION_SKIP_STEPS = 0;
// A value of 1 means the entire screen will get noisy.
// A value of 10 means less noise.
// Increas this value for less noise filling the screen.
// Use this to control the max noise amount during transition
const int MAX_TRANSITION_NOISE_AMT = 0;
// Same but for the removal of nonzero pixels
const float MAX_TRANSITION_REM_AMT = MAX_TRANSITION_NOISE_AMT / 2;
// Used to store the intermediate boop face (no noise)
uint16_t boop_face_intermed[HEIGHT][WIDTH2];
// Proximity device
Adafruit_VCNL4040 vcnl4040 = Adafruit_VCNL4040();
bool boop_sensor = true;


// Constant unit of time for the clock in seconds
const double time_unit = 0.01;
// Current
double cur_frame_time;
// Mode to display frame: "rainbow", "gradient", ""
String frame_mode = "rainbow";
// Reset screen when changing frames
bool resetScreen = false;
// Does time go backward or forward?
String time_mode = "backward";

// EMA to measure the time between loop cyles
float loop_EMA = 0.0;
float update_face_EMA = 0.0;
// Used to keep track of the EMA time
unsigned long startTime;
unsigned long endTime;
double elapsedTime;


uint16_t t_vals[HEIGHT][WIDTH2]; // Holds the time for each pixel to form a wave
uint16_t frame_val = 0; // Holds the "time" value for the frame.
int16_t cur_frame = 0; // Holds the current frame number
uint16_t new_bitmap[HEIGHT][WIDTH2];


#define MAX_FPS 120 // Maximum redraw rate, frames/second/home/gmongaras/Documents/MyStuff/Protogen/Programming/LCD_Eye_Blink_4/LCD_Eye_Blink_4.ino
#if defined(_VARIANT_MATRIXPORTAL_M4_) // MatrixPortal M4
uint8_t rgbPins[]  = {7, 8, 9, 10, 11, 12};
uint8_t addrPins[] = {17, 18, 19, 20, 21};
uint8_t clockPin   = 14;
uint8_t latchPin   = 15;
uint8_t oePin      = 16;
#else // MatrixPortal ESP32-S3
uint8_t rgbPins[]  = {42, 41, 40, 38, 39, 37};
uint8_t addrPins[] = {45, 36, 48, 35, 21};
uint8_t clockPin   = 2;
uint8_t latchPin   = 47;
uint8_t oePin      = 14;
#endif
#if HEIGHT == 16
#define NUM_ADDR_PINS 3
#elif HEIGHT == 32
#define NUM_ADDR_PINS 4
#elif HEIGHT == 64
#define NUM_ADDR_PINS 5
#endif
Adafruit_Protomatter matrix(
  WIDTH, 4, 1, rgbPins, NUM_ADDR_PINS, addrPins,
  clockPin, latchPin, oePin, true);
// Default face color
const uint16_t DEFAULT_COLOR = 0x17e0;












// Helper: Convert RGB888 to RGB565
uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    uint16_t r5 = (r >> 3) & 0x1F;
    uint16_t g6 = (g >> 2) & 0x3F;
    uint16_t b5 = (b >> 3) & 0x1F;
    return (r5 << 11) | (b5 << 6) | g6;
}

// Helper: HSV to RGB conversion
// h - hue
// s - saturation
// v - value/brightness
template <typename int_type>
void hsv_to_rgb(float h, float s, float v, int_type& r, int_type& g, int_type& b) {
    // chroma calculation (colorfulness)
    float c = v * s;
    // intermediate for m
    float x = c * (1 - std::fabs(fmod(h * 6, 2) - 1));
    // match for r g and b. Adjusts the "lightness" of each color
    float m = v - c;

    // Translate to RGB
    float r_, g_, b_;
    // hue 0 to 60 degrees - red to yellow
    if (h < 1.0 / 6) {
        r_ = c; g_ = x; b_ = 0;
    } 
    // hue 60 to 120 degrees - yellow to green
    else if (h < 2.0 / 6) {
        r_ = x; g_ = c; b_ = 0;
    } 
    // hue 120 to 180 degrees - green to cyan
    else if (h < 3.0 / 6) {
        r_ = 0; g_ = c; b_ = x;
    } 
    // hue 180 to 240 degrees - cyan to blue
    else if (h < 4.0 / 6) {
        r_ = 0; g_ = x; b_ = c;
    } 
    // hue 240 to 300 degrees - blue to magenta
    else if (h < 5.0 / 6) {
        r_ = x; g_ = 0; b_ = c;
    } 
    // hue 300 to 360 degrees - magenta to red
    else {
        r_ = c; g_ = 0; b_ = x;
    }

    r = static_cast<int_type>((r_ + m) * 255);
    g = static_cast<int_type>((g_ + m) * 255);
    b = static_cast<int_type>((b_ + m) * 255);
}

// Compute rainbow color and return RGB565
uint16_t rainbow_rgb565(int t, int max_) {
    if (t < 0 || t > max_) return 0;

    float hue = static_cast<float>(t) / max_;  // hue ∈ [0, 1]
    uint8_t r, g, b;
    hsv_to_rgb<uint8_t>(hue, 1.0f, 1.0f, r, g, b);
    return rgb888_to_rgb565(r, g, b);
}

uint16_t rainbow_rgb565_flipgb(int t, int max_) {
    if (t < 0 || t > max_) return 0;

    float hue = static_cast<float>(t) / max_;  // hue ∈ [0, 1]
    uint8_t r, g, b;
    hsv_to_rgb<uint8_t>(hue, 1.0f, 1.0f, r, g, b);
    return rgb888_to_rgb565(r, b, g);
}

uint16_t gradient_rgb565(int t, int max_, 
                         uint8_t r1, uint8_t g1, uint8_t b1, 
                         uint8_t r2, uint8_t g2, uint8_t b2) {
    if (t < 0 || t > max_) return 0;

    float alpha = static_cast<float>(t) / max_;  // alpha ∈ [0, 1]

    uint8_t r = static_cast<uint8_t>((1 - alpha) * r1 + alpha * r2);
    uint8_t g = static_cast<uint8_t>((1 - alpha) * g1 + alpha * g2);
    uint8_t b = static_cast<uint8_t>((1 - alpha) * b1 + alpha * b2);

    return rgb888_to_rgb565(r, g, b);
}

void assert_or_halt(bool condition, const char* message = "") {
  if (!condition) {
    Serial.println("Assertion failed!");
    if (message[0] != '\0') {
      Serial.println(message);
    }
    while (true); // Infinite loop: halts execution
  }
}

// used to turn a timestep into a WBGR value
uint32_t rainbow_WRGB(int t, int max_) {
    // For some reason, putting these here fixes problems with the sides?
    // If the serial monitor is on, this breaks and the sides flash
    // I don't know why, but it's """fixed""" so whatever.
    assert_or_halt(max_ > 0, "max_ must be > 0");
    assert_or_halt(t >= 0 && t <= max_, "t out of range");

    float hue = static_cast<float>(t) / max_;  // hue ∈ [0, 1]
    uint32_t r, g, b;
    hsv_to_rgb<uint32_t>(hue, 1.0f, 1.0f, r, g, b);

    // Convert to WBGR
    return 0x00000000 | (r << 16) | (g << 8) | b;
}





static inline void unpack565(uint16_t pix, uint8_t& r5, uint8_t& g6, uint8_t& b5) {
  r5 = (pix >> 11) & 0x1F;
  g6 = (pix >> 5)  & 0x3F;
  b5 =  pix        & 0x1F;
}

static inline uint16_t pack565(uint8_t r5, uint8_t g6, uint8_t b5) {
  return ((uint16_t)(r5 & 0x1F) << 11) | ((uint16_t)(g6 & 0x3F) << 5) | (uint16_t)(b5 & 0x1F);
}

static inline uint8_t clamp_u8(int v, int lo, int hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return (uint8_t)v;
}

// Linear blend in RGB565 (per-channel)
static inline uint16_t lerp565(uint16_t a, uint16_t b, float t) {
  uint8_t ar, ag, ab, br, bg, bb;
  unpack565(a, ar, ag, ab);
  unpack565(b, br, bg, bb);
  float it = 1.0f - t;
  uint8_t r = (uint8_t)clamp_u8((int)roundf(ar * it + br * t), 0, 31);
  uint8_t g = (uint8_t)clamp_u8((int)roundf(ag * it + bg * t), 0, 63);
  uint8_t b_ = (uint8_t)clamp_u8((int)roundf(ab * it + bb * t), 0, 31);
  return pack565(r, g, b_);
}

// Uniform integer noise per channel. Amplitudes are in *channel steps*:
//   rAmp, bAmp in 0..31; gAmp in 0..63.
// Use small values (1–2) for subtle grain.
static inline uint16_t add_noise_rgb565(uint16_t pix, int rAmp, int gAmp, int bAmp) {
  uint8_t r, g, b;
  unpack565(pix, r, g, b);
  int rn = (rAmp > 0) ? random(-rAmp, rAmp + 1) : 0;
  int gn = (gAmp > 0) ? random(-gAmp, gAmp + 1) : 0;
  int bn = (bAmp > 0) ? random(-bAmp, bAmp + 1) : 0;
  r = clamp_u8((int)r + rn, 0, 31);
  g = clamp_u8((int)g + gn, 0, 63);
  b = clamp_u8((int)b + bn, 0, 31);
  return pack565(r, g, b);
}





void drawPixel(int& y, int& x, uint16_t& pixval) {
  matrix.drawPixel(WIDTH2-x, HEIGHT-y-1, pixval);
  matrix.drawPixel(WIDTH2+x, HEIGHT-y-1, pixval);
}

// Function used to change pixel value based on current mode
void change_pixval(uint16_t& pixval, int& y, int& x) {
  // Get the time value.
  uint16_t time_val = t_vals[y][x];

  // Change bitmap based on color mode
  if (frame_mode == "gradient") {
    pixval = gradient_rgb565(time_val, WIDTH2, gradient_R1, gradient_G1, gradient_B1, gradient_R2, gradient_G2, gradient_B2);
  } else if (frame_mode == "rainbow") {
    pixval = rainbow_rgb565_flipgb(time_val, WIDTH2);
  }
  else {
    pixval = DEFAULT_COLOR;
  }
  // Otherwise, don't change the pixel value
  return;
}

uint32_t fastSeed = 2463534242;
inline uint32_t fastRand() {
  if (fastSeed == 0) fastSeed = 2463534242; // recover from zero
  fastSeed ^= fastSeed << 13;
  fastSeed ^= fastSeed >> 17;
  fastSeed ^= fastSeed << 5;
  return fastSeed;
}

uint16_t perturbRGB565(uint16_t color, float scale) {
  // Clamp scale to [0.0, 1.0]
  if (scale < 0.0f) scale = 0.0f;
  if (scale > 1.0f) scale = 1.0f;

  // Extract RGB components
  int r = (color >> 11) & 0x1F;
  int g = (color >> 5) & 0x3F;
  int b = color & 0x1F;

  // Compute max deltas
  int maxR = (int)(31 * scale);
  int maxG = (int)(63 * scale);
  int maxB = (int)(31 * scale);

  // Perturb using fastRand()
  int dr = (int)(fastRand() % (2 * maxR + 1)) - maxR;
  int dg = (int)(fastRand() % (2 * maxG + 1)) - maxG;
  int db = (int)(fastRand() % (2 * maxB + 1)) - maxB;

  r = constrain(r + dr, 0, 31);
  g = constrain(g + dg, 0, 63);
  b = constrain(b + db, 0, 31);

  return (r << 11) | (g << 5) | b;
}


// Generate the rainbow bitmap
template <size_t N>
void generate_bitmap_(uint16_t (&bitmap_to_use)[N][HEIGHT][WIDTH2], bool increment, bool transition_boop, int cur_frame_) {
  if (transition_boop && num_steps_boop_mode < num_iters_boop) {
    // Increase the number of frames in boop mode to interpolate into it
    num_steps_boop_mode++;
  }

  // Iterate over all pixels
  for (int y = 0; y < HEIGHT; ++y) {
    for (int x = 0; x < WIDTH2; ++x) {
      // Increment t values
      if (increment) {
        if (time_mode == "backward") {
          // Time moves backward. Since negatives are annoying, this trick works
          // t_vals[y][x] = WIDTH2 - (((WIDTH2 - t_vals[y][x]) + 1) % (WIDTH2+1));
          t_vals[y][x] = (t_vals[y][x] + WIDTH2 - 1) % WIDTH2;
        } else {
          t_vals[y][x] = (t_vals[y][x] + 1) % WIDTH2;
        }
      }

      // Get current frame pixel value
      uint16_t pixval = bitmap_to_use[cur_frame_][y][x];

      // We are transitioning in boop mode
      if (transition_boop) {
        // Only change face every NUM_TRANSITION_SKIP_STEPS steps
        if (num_steps_boop_mode % NUM_TRANSITION_SKIP_STEPS == 0) {
          // As the number of steps increases, the proportion increases, decreasing the SNR

          // Get interpolation proportion
          float interp_prop = (float)num_steps_boop_mode / (float)num_iters_boop / MAX_TRANSITION_NOISE_AMT;

          // float interp_prop = 1.0 / (float)num_iters_boop;
          if (num_steps_boop_mode < num_iters_boop) {
            // Random chance for a nonzero pixel value to become zero
            if (pixval != zero_pix && (fastRand() & 0xFFFFFF) * (1.0f / 16777216.0f) < interp_prop * MAX_TRANSITION_REM_AMT) {
              pixval = 0;
            }

            // Random chance to change the pixel value to a random value
            if ((fastRand() & 0xFFFFFF) * (1.0f / 16777216.0f) < interp_prop) {
              pixval = perturbRGB565(pixval, interp_prop);
            }

            resetScreen = true;
          }
        } 
        // Skip some iterations so the pixels are a little more sticky
        else {
          continue;
        }
      }

      // Skip zero pixels to make the display faster
      if (pixval == zero_pix) {
        if (resetScreen){
          drawPixel(y, x, zero_pix);
        }
        continue;
      }

      // Change the pixel value based on the mode.
      change_pixval(pixval, y, x);

      drawPixel(y, x, pixval);
    }
  }
}

// Helper used to call generate_bitmap with different numer of frames
void generate_bitmap(bool increment=false) {
  // if (is_booping && num_steps_boop_mode >= num_iters_boop) {
  if (boop_sensor && is_booping && boop_sensor && num_steps_boop_mode >= num_iters_boop) {
    // Reset screen
    if (num_steps_boop_mode == num_iters_boop) {
      resetScreen = true;
      num_steps_boop_mode += 1;
    }

    // We are in boop mode, completely transitioned
    generate_bitmap_<NUM_FRAMES_BOOP>(bitmaps_boop, increment, false, 0);
  } else {
    // True if we are in normal mode, transitioning to boop mode
    bool transition_boop = is_booping && num_steps_boop_mode < num_iters_boop;

    // We are in normal mode
    generate_bitmap_<NUM_FRAMES>(bitmaps, increment, transition_boop, cur_frame);
  }

  // Change reset screen back
  if (resetScreen) {
    resetScreen = false;
  }
}

// t values initialization
void get_t_vals_init(uint16_t t_vals[HEIGHT][WIDTH2]) {
  for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH2; ++x) {
            int t = x;
            t_vals[y][x] = t;
        }
    }
}





// Function to initialize the matrix from a bitmap
void initMatrixFromBitmap(bool initialize_pix) {
  // Update the matrix screen with pixels
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH2; x++) {
      // Write the pixel to the matrix
      drawPixel(y, x, bitmaps[0][y][x]);
    }
  }
  matrix.show(); // Display the bitmap on the matrix
}



// Used to get a random time given a range array
long getRandomNumber(const uint16_t range_[2]) {
  if (range_[0] == range_[1]) {
    return range_[0];
  }
  return random(range_[0], range_[1]);
}




int boopIters = 0;
const int BOOP_NUM_ITERS_TO_CHECK = 10;


void setup() {
  // Initialize Serial Monitor
  Serial.begin(921600);
  // while (!Serial) { delay(4); }
  delay(1);

  






  Wire.begin();
  Wire.setClock(400000);   // 400 kHz fast mode

  // Setup boop proximity
  if (!vcnl4040.begin()) {
    Serial.println("Couldn't find VCNL4040 chip");
    boop_sensor = false;
  } else {
    Serial.println("Found VCNL4040 chip");
    boop_sensor = true;

    vcnl4040.setProximityIntegrationTime(VCNL4040_PROXIMITY_INTEGRATION_TIME_1T);
    // vcnl4040.setProximityResolution(VCNL4040_PS_RES_12_BIT);
    vcnl4040.setProximityLEDDutyCycle(VCNL4040_LED_DUTY_1_40);

    vcnl4040.enableAmbientLight(false);
    vcnl4040.enableWhiteLight(false);
    vcnl4040.setProximityHighResolution(false);
  }

  // NOTE: I can't do interrupts unless I solder the interrupt pin
  // on the proximity sensor
  // Attach interrupt on both rising and falling edges
  // pinMode(VCNL_INT_PIN, INPUT_PULLUP);
  // int VCNL_INT_PIN = 2;
  // // Setup proximity interrupt thresholds
  // uint16_t low_thresh = 0;                      // below this: signal "not close"
  // uint16_t high_thresh = BOOP_PROXIMITY_THRESH; // above this: signal "close"
  // vcnl4040.enableProximityInterrupts(VCNL4040_PROXIMITY_INT_CLOSE_AWAY);
  // vcnl4040.setProximityLowThreshold(low_thresh);
  // vcnl4040.setProximityHighThreshold(high_thresh);
  // vcnl4040.setProximityHighResolution(false);
  // attachInterrupt(digitalPinToInterrupt(VCNL_INT_PIN), boopISR, CHANGE);










  // Initialize BLE Device
  BLEDevice::init(BLE_service_name);

  // Create BLE Server
  bleServer = BLEDevice::createServer();
  bleServer->setCallbacks(new MyServerCallbacks());

  // Create BLE Service
  BLEService *bleService = bleServer->createService(SERVICE_UUID);

  // Create BLE Characteristic for Mode
  modeCharacteristic = bleService->createCharacteristic(
    MODE_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  modeCharacteristic->setValue("1"); // Default to Mode 1
  modeCharacteristic->setCallbacks(new ModeCallbacks());
  modeCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  bleService->start();

  // Start advertising
  RestartBLE();














  // Initialize the matrix
  ProtomatterStatus status = matrix.begin();

  // Get t values
  get_t_vals_init(t_vals);
  generate_bitmap();

  // Initialize frame values
  // get_frame_vals_init()

  initMatrixFromBitmap(false);
  Serial.printf("Protomatter begin() status: %d\n", status);

  // Initialize the current time being looked at
  cur_frame_time = getRandomNumber(frame_time_unit_ranges[0]);



  // Side light setup
  // Set brightness (0 to 255)
  if (side_iter) {
    neopixels.setBrightness(NEOPIX_BRIGHTNESS);
    neopixels.begin(); // Initialize NeoPixel ring
    // Initialize timesteps
    for (int i = 0; i < NEOPIX_NUMPIXELS; i++) {
      side_light_WRGB_time[i] = i;
    }
    // Initialize display
    if (rotate) {
      for (int i = 0; i < NEOPIX_NUMPIXELS; i++) {
        neopixels.setPixelColor(i, rainbow_WRGB(side_light_WRGB_time[i], NEOPIX_NUMPIXELS));
      }
    } else {
      for (int i = 0; i < NEOPIX_NUMPIXELS2; i++) {
        neopixels.setPixelColor(i, rainbow_WRGB(side_light_WRGB_time[i], NEOPIX_NUMPIXELS2));
        neopixels.setPixelColor(NEOPIX_NUMPIXELS-i-1, rainbow_WRGB(side_light_WRGB_time[i], NEOPIX_NUMPIXELS2));
      }
    }
    neopixels.show();
  }
  else {
    neopixels.setBrightness(0);
    neopixels.begin(); // Initialize NeoPixel ring
    neopixels.show();
  }
}

void loop() {
  // How long does it take for each loop cycle
  startTime = millis();









  // What is the current mode?
  String CurMode = currentMode;

  if (deviceConnected) {
    // Send notifications based on the mode
    static unsigned long lastNotifyTime = 0;
    if (millis() - lastNotifyTime > 1000) { // Every 1 second
      lastNotifyTime = millis();

      String notificationMessage;
      if (currentMode == "1") {
        notificationMessage = "Mode 1: Rainbow ^w^";
        frame_mode = "rainbow";
      }
      else if (currentMode == "2") {
        notificationMessage = "Mode 2: Gradient";
        frame_mode = "gradient";
      } else if (currentMode == "3") {
        notificationMessage = "Mode 3: Normal";
        frame_mode = "";
      } else if (currentMode == "side off") {
        if (side_iter != false) {
          notificationMessage = "Turning off side light";
          side_iter = false;
          neopixels.setBrightness(0);
          neopixels.show();
        } else {
          notificationMessage = "Side light already off";
        }
      } else if (currentMode == "side on") {
        if (side_iter != true) {
          notificationMessage = "Turning on side light";
          side_iter = true;
          neopixels.setBrightness(NEOPIX_BRIGHTNESS);
          neopixels.show();
        } else {
          notificationMessage = "Side light already on";
        }
      } else {
        notificationMessage = "Unknown Mode";
      }

      //matrix.drawPixel(WIDTH2-currentMode-1, currentMode, 0x000F);

      modeCharacteristic->setValue(notificationMessage.c_str());
      modeCharacteristic->notify();
      Serial.println(notificationMessage);
    }
  }








  // Is a boop happening?
  if (boop_sensor) {
    boopIters++;
    if (boopIters > BOOP_NUM_ITERS_TO_CHECK) {
      boopIters = 0;

      // Sensor picked up a boop. Change modes
      if (vcnl4040.getProximity() > BOOP_PROXIMITY_THRESH) {
        // if (boopStateChange) {
        // To add logic. For now, we instantly go into boop mode
        is_booping_prev = is_booping;
        is_booping = true;
      } 
      // Sensor did not pick up a boop. Change modes
      else {
        // To add logic
        is_booping_prev = is_booping;
        is_booping = false;
      }
      // Reset screen if state change
      if (is_booping_prev != is_booping) {
        resetScreen = true;
      }
      // If we were booping and now we're not, then reset the boop counter.
      if (is_booping_prev && !is_booping) {
        num_steps_boop_mode = 0;
      }
      // Serial.printf("%d, %d, %d %4f\n", is_booping_prev, is_booping, resetScreen, loop_EMA);
    }
  }


  // if (boopStateChange) {
  //   is_booping = !is_booping;
  //   resetScreen = true;
  //   boopStateChange = false;
  // }








  // Update every number of timesteps
  if (side_iter) {
    if (num_steps_side >= NUM_STEPS_SIDE_ITER) {
      // Used to rotate around the ring
      if (rotate) {
        // Iterate sidelight loop
        for (int i = 0; i < NEOPIX_NUMPIXELS; i++) {
          side_light_WRGB_time[i] = (side_light_WRGB_time[i] + SIDE_INCREMENT_VALUE) % NEOPIX_NUMPIXELS;
          if (invert_side){
            neopixels.setPixelColor(NEOPIX_NUMPIXELS-i-1, rainbow_WRGB(side_light_WRGB_time[i], NEOPIX_NUMPIXELS));
          } else {
            neopixels.setPixelColor(i, rainbow_WRGB(side_light_WRGB_time[i], NEOPIX_NUMPIXELS));
          }
        }
      }
      // Used to make the ring iterate linearly like a pannel from right to left
      else {
        // Iterate sidelight loop
        for (int i = 0; i < NEOPIX_NUMPIXELS2; i++) {
          side_light_WRGB_time[i] = (side_light_WRGB_time[i] + SIDE_INCREMENT_VALUE) % NEOPIX_NUMPIXELS2;
          neopixels.setPixelColor(i, rainbow_WRGB(side_light_WRGB_time[i], NEOPIX_NUMPIXELS2));
          //neopixels.setPixelColor(NEOPIX_NUMPIXELS-i-1, rainbow_WRGB(side_light_WRGB_time[i], NEOPIX_NUMPIXELS2));
        }
      }
      // Serial.println("\n");
      // neopixels.setBrightness(NEOPIX_BRIGHTNESS);
      neopixels.show();
      num_steps_side = 0;
    }
    num_steps_side++;
  }





  // neopixels.setPixelColor(0, 255, 255, 255, 255);
  // neopixels.setPixelColor(11, 255, 0, 255);
  // neopixels.show();  // Turn off all pixels initially
  // matrix.fillScreen(0x0);
  // initMatrixFromBitmap(bitmap, false);
  // matrix.fillScreen(0x0);


  // // Get a random random probability. Do we enter random mode
  // if (((double)random(0, 100)) / 100 < random_bitmap_prob) {
  //   // Set random frame mode to true
  //   random_frame_mode = true;
  //   // Get the random time to stay in this mode
  //   cur_frame_time_random = getRandomNumber(random_frame_time_unit_range);
  // }

  // We are not entering random mode
  // Increment frame value every time we reach the time to stay on this frame
  if (frame_val > cur_frame_time) {
    frame_val = 0;
    cur_frame += loop_addition;
    // Modulo if we are doing a loop
    if (LOOP_MODE == "loop") {
      cur_frame = cur_frame % NUM_FRAMES;
    }
    // Otherwise we invert the loop, alternating between frames
    else if (LOOP_MODE == "reversed_loop") {
      // End of the loop, go backward
      if (cur_frame == NUM_FRAMES) {
        cur_frame -= 1;
        loop_addition = -1;
      }
      // Beginning of the loop, go forward
      else if (cur_frame < 0) {
        cur_frame += 1;
        loop_addition = 1;
      }
    }

    // Set the reset screen flag
    resetScreen = true;

    // Get a new random time for this frame
    cur_frame_time = getRandomNumber(frame_time_unit_ranges[cur_frame]);
  }
  frame_val += 1;

  // Increment by the unit of time
  // Note the delay is in miliseconds
  delay(time_unit*1000.0 - loop_EMA);

  generate_bitmap(true);
  matrix.show(); // Display the bitmap on the matrix
  // initMatrixFromBitmap(new_bitmap, false);

  // End time of loop
  endTime = millis();

  // Total time of loop in milliseconds
  elapsedTime = (float)(endTime - startTime);

  // Update EMA
  loop_EMA = min(max(0.9*loop_EMA + 0.1*elapsedTime, time_unit), 0.01);
}
