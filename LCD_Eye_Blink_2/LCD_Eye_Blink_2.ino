#include <MD_MAX72xx.h>
#include <SPI.h>

#define NUM_MATRICES 7
#define ROW_SIZE 8

// Define the number of devices we have in the chain and the hardware interface
#define CLK_PIN   13  // or SCK (52 on mega)
#define DATA_PIN  11  // or MOSI or DIN (51 on mega)
#define CS_PIN    3  // or SS

// Hardware SPI connection
MD_MAX72XX myDisplay = MD_MAX72XX(MD_MAX72XX::FC16_HW, CS_PIN, ROW_SIZE*2);

// Global variables
int currentRow = 0;
bool movingUp = true;






bool image_disp1[NUM_MATRICES][ROW_SIZE][ROW_SIZE] = {
  {
    {0,0,0,0,0,0,0,0}, // Eye left
    {0,0,0,0,0,1,1,1},
    {0,0,0,0,1,1,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,1,1,0,0,0,0},
    {0,0,1,0,0,0,0,0},
    {0,0,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0}, // Eye right
    {1,1,0,0,0,0,0,0},
    {0,1,1,0,0,0,0,0},
    {0,0,1,1,0,0,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,0,1,0,0,0},
    {0,0,0,0,1,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0}, // Mouth right
    {0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0},
    {0,1,1,0,0,0,0,0},
    {0,0,1,1,0,0,0,0},
    {0,0,0,1,1,1,1,1},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0}, // Mought right 2
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,1},
    {0,0,0,0,0,1,1,0},
    {1,0,0,0,1,1,0,0},
    {1,1,0,1,1,0,0,0},
    {0,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0}, // Mouth left 2
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,0},
    {0,0,0,1,1,0,1,1},
    {0,0,1,1,0,0,0,1},
    {0,1,1,0,0,0,0,0},
    {1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0}, // Mouth left 1
    {0,0,0,1,0,0,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,0,1,1,0,0},
    {0,0,0,0,0,1,1,0},
    {0,0,0,0,0,0,1,1},
    {0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0}, // Nose
    {0,0,0,0,0,0,0,0},
    {0,0,0,1,1,1,1,0},
    {0,0,1,1,0,0,0,0},
    {0,1,1,0,0,0,0,0},
    {1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
};




bool image_disp2[NUM_MATRICES][ROW_SIZE][ROW_SIZE] = {
  {
    {0,0,0,0,0,0,0,0}, // Nose
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0}, // Mouth right 1
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0}, // Mouth right 2
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0}, // Mouth left 1
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0}, // Mouth left 2
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0}, // Eye left 1
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0}, // Eye left 2
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
};


// Flip matrix vertically
void flipVerticle(bool image_disp[NUM_MATRICES][ROW_SIZE][ROW_SIZE], int disp_num) {
  bool tmp[ROW_SIZE][ROW_SIZE] = {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  };
  for (int i = 0; i < ROW_SIZE; i++) {
    for (int j = 0; j < ROW_SIZE; j++) {
      tmp[i][j] = image_disp[disp_num][i][j];
    }
  }
  for (int i = 0; i < ROW_SIZE; i++) {
    for (int j = 0; j < ROW_SIZE; j++) {
      image_disp[disp_num][i][j] = tmp[ROW_SIZE-1-i][j];
    }
  }
}



// Flip matrix horizontally
void flipHorizontal(bool image_disp[NUM_MATRICES][ROW_SIZE][ROW_SIZE], int disp_num) {
  bool tmp[ROW_SIZE][ROW_SIZE] = {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  };
  for (int i = 0; i < ROW_SIZE; i++) {
    for (int j = 0; j < ROW_SIZE; j++) {
      tmp[i][j] = image_disp[disp_num][i][j];
    }
  }
  for (int i = 0; i < ROW_SIZE; i++) {
    for (int j = 0; j < ROW_SIZE; j++) {
      image_disp[disp_num][i][j] = tmp[i][ROW_SIZE-1-j];
    }
  }
}



// Used to create the second set of matrices
void createSecond() {
  // Iterate over all displays. The second set will be updated in reverse order
  for (int disp_num = 0; disp_num < NUM_MATRICES; disp_num++) {
    for (int i = 0; i < ROW_SIZE; i++) { // rows
      for (int j = 0; j < ROW_SIZE; j++) { // columns
        image_disp2[NUM_MATRICES-1-disp_num][i][j] = image_disp1[disp_num][i][j];
      }
    }
  }
}




void setup() {
  myDisplay.begin();
  myDisplay.control(MD_MAX72XX::INTENSITY, 0); // set brightness level (0 is min, 15 is max)
  myDisplay.clear();

  // Eye flip vertically
  flipVerticle(image_disp1, 0);
  flipVerticle(image_disp1, 1);
  // Mouth flip horizontally
  flipHorizontal(image_disp1, 2);
  flipHorizontal(image_disp1, 3);
  flipHorizontal(image_disp1, 4);
  flipHorizontal(image_disp1, 5);
  // Nose flip vertically
  flipVerticle(image_disp1, 6);

  // Create the second set of matrices
  createSecond();
  // Nose flip horizontally
  flipHorizontal(image_disp2, 0);
  // Mouth flip horizontally
  flipHorizontal(image_disp2, 1);
  flipHorizontal(image_disp2, 2);
  flipHorizontal(image_disp2, 3);
  flipHorizontal(image_disp2, 4);
  // Eye flip horizontally
  flipHorizontal(image_disp2, 5);
  flipHorizontal(image_disp2, 6);
}



void loop() {
  displayLines(image_disp1, 0);
  displayLines(image_disp2, 7);
  delay(2000);
}


void displayLines(bool image_disp[NUM_MATRICES][ROW_SIZE][ROW_SIZE], int disp_offset) {
  for (int row = 0; row < ROW_SIZE; row++) {
    for (int d = 0; d < NUM_MATRICES; d++) {
      for (int col = 0; col < ROW_SIZE; col++) {
        myDisplay.setPoint(row, (d+disp_offset) * ROW_SIZE + col, image_disp[d][row][col]);
      }
    }
  }
}
