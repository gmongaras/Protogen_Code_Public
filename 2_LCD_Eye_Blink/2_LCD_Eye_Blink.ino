/*
#include <MD_MAX72xx.h>
#include <SPI.h>

// Define the number of devices we have in the chain and the hardware interface
#define MAX_DEVICES 2
#define CLK_PIN   13  // or SCK
#define DATA_PIN  11  // or MOSI
#define CS_PIN    3  // or SS

// Hardware SPI connection
MD_MAX72XX myDisplay = MD_MAX72XX(MD_MAX72XX::FC16_HW, CS_PIN, MAX_DEVICES);

// Global variables
int currentRow = 0;
bool movingUp = true;

void setup() {
  myDisplay.begin();
  myDisplay.control(MD_MAX72XX::INTENSITY, 0); // set brightness level (0 is min, 15 is max)
  myDisplay.clear();
}

void loop() {
  displayLine(currentRow);
  delay(100);  // adjust speed of moving line
  updatePosition();
}


void displayLine(int row) {
  myDisplay.clear();
  for (int i = 0; i < MAX_DEVICES; i++) {
    myDisplay.setPoint(row, i * 8 + 0, true);
    myDisplay.setPoint(row, i * 8 + 1, true);
    myDisplay.setPoint(row, i * 8 + 2, true);
    myDisplay.setPoint(row, i * 8 + 3, true);
    myDisplay.setPoint(row, i * 8 + 4, true);
  }
}

void updatePosition() {
  if (movingUp) {
    currentRow--;
    if (currentRow <= 0) {
      movingUp = false;
    }
  } else {
    currentRow++;
    if (currentRow >= 7) {
      movingUp = true;
    }
  }
}
*/







#include <MD_MAX72xx.h>
#include <SPI.h>

// Define the number of devices we have in the chain and the hardware interface
#define MAX_DEVICES 3
#define CLK_PIN   13  // or SCK (52 on mega)
#define DATA_PIN  11  // or MOSI or DIN (51 on mega)
#define CS_PIN    3  // or SS

// Hardware SPI connection
MD_MAX72XX myDisplay = MD_MAX72XX(MD_MAX72XX::FC16_HW, CS_PIN, MAX_DEVICES);

// Global variables
int currentRow = 0;
bool movingUp = true;

void setup() {
  myDisplay.begin();
  myDisplay.control(MD_MAX72XX::INTENSITY, 0); // set brightness level (0 is min, 15 is max)
  myDisplay.clear();
}








bool image_disp1[3][8][8] = {
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0},
    {1,1,1,0,0,0,0,0},
    {0,0,1,1,0,0,0,0},
    {0,0,0,1,1,0,0,0},
    {1,0,0,0,1,1,0,0},
    {0,0,0,0,0,1,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1},
    {0,0,0,0,1,1,1,1},
    {0,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,0,1},
    {0,1,1,0,0,0,0,1},
    {0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  }
};





bool image_disp2[3][8][8] = {
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {1,1,1,0,0,0,0,0},
    {1,1,1,1,0,0,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,1},
    {0,0,0,1,1,1,1,1},
    {0,0,1,1,0,0,0,1},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  }
};


bool image_disp3[3][8][8] = {
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {1,1,1,1,0,0,0,0},
    {1,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,1,1,1,1,1},
    {0,0,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  }
};









void loop() {
  displayLines(image_disp1);
  delay(2000);
  displayLines(image_disp2);
  delay(100);
  displayLines(image_disp3);
  delay(300);
  displayLines(image_disp2);
  delay(100);
}


void displayLines(bool image_disp[3][8][8]) {
  for (int row = 0; row < 8; row++) {
    for (int d = 0; d < MAX_DEVICES; d++) {
      for (int col = 0; col < 8; col++) {
        myDisplay.setPoint(row, d * 8 + 7-col, image_disp[d][row][col]);
      }
    }
  }
}
