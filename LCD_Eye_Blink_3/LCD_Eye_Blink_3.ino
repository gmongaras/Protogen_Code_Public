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






bool eye_disp1_1[2][ROW_SIZE][ROW_SIZE] = {
  {
    {0,0,0,0,0,0,1,1},
    {0,0,0,0,0,1,1,1},
    {0,0,0,0,1,1,1,1},
    {0,0,0,1,1,1,1,0},
    {0,0,1,1,1,1,0,0},
    {0,0,1,1,1,0,0,0},
    {0,0,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {1,1,1,1,0,0,0,0},
    {1,1,1,1,1,0,0,0},
    {1,1,1,1,1,1,0,0},
    {0,0,0,1,1,1,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
};

bool eye_disp1_2[2][ROW_SIZE][ROW_SIZE] = {
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,1},
    {0,0,0,0,1,1,1,1},
    {0,0,0,1,1,1,1,0},
    {0,0,0,1,1,1,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {1,1,1,1,1,0,0,0},
    {1,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
};

bool mouth_disp1[4][ROW_SIZE][ROW_SIZE] = {
  {
    {0,0,1,1,1,1,1,1},
    {0,1,1,0,0,0,0,0},
    {1,1,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,1},
    {1,1,0,0,0,0,1,1},
    {0,1,1,0,0,1,1,0},
    {0,0,1,1,1,1,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1},
    {0,0,0,0,1,1,0,1},
    {0,0,0,1,1,0,0,0},
    {1,0,1,1,0,0,0,0},
    {1,1,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,0},
    {0,0,0,1,1,0,1,1},
    {0,0,1,1,0,0,0,1},
    {0,1,1,0,0,0,0,0},
    {1,1,0,0,0,0,0,0},
  },
};

bool nose_disp1[1][ROW_SIZE][ROW_SIZE] = {
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0},
    {0,0,0,0,0,1,1,1},
    {1,0,0,0,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,0},
    {1,1,1,1,1,1,0,0},
    {0,1,1,1,1,0,0,0},
  },
};




bool eye_disp2_1[NUM_MATRICES][ROW_SIZE][ROW_SIZE] = {
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

bool eye_disp2_2[NUM_MATRICES][ROW_SIZE][ROW_SIZE] = {
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

bool mouth_disp2[NUM_MATRICES][ROW_SIZE][ROW_SIZE] = {
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
};

bool nose_disp2[NUM_MATRICES][ROW_SIZE][ROW_SIZE] = {
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
};


// Flip matrix vertically
void flipVerticle(bool image_disp[][ROW_SIZE][ROW_SIZE], int disp_num) {
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
void flipHorizontal(bool image_disp[][ROW_SIZE][ROW_SIZE], int disp_num) {
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
void createSecond(bool image_disp1[][ROW_SIZE][ROW_SIZE], bool image_disp2[][ROW_SIZE][ROW_SIZE], int num_matrices) {
  // Iterate over all displays. The second set will be updated in reverse order
  for (int disp_num = 0; disp_num < num_matrices; disp_num++) {
    for (int i = 0; i < ROW_SIZE; i++) { // rows
      for (int j = 0; j < ROW_SIZE; j++) { // columns
        image_disp2[num_matrices-1-disp_num][i][j] = image_disp1[disp_num][i][j];
      }
    }
  }
}




void setup() {
  myDisplay.begin();
  myDisplay.control(MD_MAX72XX::INTENSITY, 0); // set brightness level (0 is min, 15 is max)
  myDisplay.clear();

  // Eye flip vertically
  flipVerticle(eye_disp1_1, 0);
  flipVerticle(eye_disp1_1, 1);
  flipVerticle(eye_disp1_2, 0);
  flipVerticle(eye_disp1_2, 1);
  // Mouth flip horizontally
  flipHorizontal(mouth_disp1, 0);
  flipHorizontal(mouth_disp1, 1);
  flipHorizontal(mouth_disp1, 2);
  flipHorizontal(mouth_disp1, 3);
  // Nose flip vertically
  flipVerticle(nose_disp1, 0);

  // Create the second set of matrices
  createSecond(eye_disp1_1, eye_disp2_1, 2);
  createSecond(eye_disp1_2, eye_disp2_2, 2);
  createSecond(mouth_disp1, mouth_disp2, 4);
  createSecond(nose_disp1, nose_disp2, 1);
  // // Eye flip horizontally
  flipHorizontal(eye_disp2_1, 0);
  flipHorizontal(eye_disp2_1, 1);
  flipHorizontal(eye_disp2_2, 0);
  flipHorizontal(eye_disp2_2, 1);
  // Mouth flip horizontally
  flipHorizontal(mouth_disp2, 0);
  flipHorizontal(mouth_disp2, 1);
  flipHorizontal(mouth_disp2, 2);
  flipHorizontal(mouth_disp2, 3);
  // Nose flip horizontally
  flipHorizontal(nose_disp2, 0);

  // Display everything
  displayLines(eye_disp1_1, 0, 2);
  displayLines(mouth_disp1, 2, 4);
  displayLines(nose_disp1, 6, 1);
  displayLines(nose_disp2, 7, 1);
  displayLines(mouth_disp2, 8, 4);
  displayLines(eye_disp2_1, 12, 2);
}



void loop() {
  // Update eyes
  displayLines(eye_disp1_2, 0, 2);
  displayLines(eye_disp2_2, 12, 2);
  delay(2000);
  displayLines(eye_disp1_1, 0, 2);
  displayLines(eye_disp2_1, 12, 2);
  delay(2000);
}


void displayLines(bool image_disp[][ROW_SIZE][ROW_SIZE], int disp_offset, int num_matrices) {
  for (int row = 0; row < ROW_SIZE; row++) {
    for (int d = 0; d < num_matrices; d++) {
      for (int col = 0; col < ROW_SIZE; col++) {
        myDisplay.setPoint(row, (d+disp_offset) * ROW_SIZE + col, image_disp[d][row][col]);
      }
    }
  }
}
