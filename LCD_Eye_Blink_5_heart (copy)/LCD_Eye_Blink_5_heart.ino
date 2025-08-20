#include <MD_MAX72xx.h>
#include <SPI.h>

#define NUM_MATRICES 7
#define ROW_SIZE 8
#define NUM_FRAMES 3
#define NUM_FRAMES_HEART 2
#define NUM_ITERS_HEART 5

// Define the number of devices we have in the chain and the hardware interface
#define CLK_PIN   13  // or SCK (52 on mega)
#define DATA_PIN  11  // or MOSI or DIN (51 on mega)
#define CS_PIN    3  // or SS

// Hardware SPI connection
MD_MAX72XX myDisplay = MD_MAX72XX(MD_MAX72XX::FC16_HW, CS_PIN, ROW_SIZE*2);






bool eye_disp1[NUM_FRAMES][2][ROW_SIZE][ROW_SIZE] = {
  {
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
  },
  {
    {
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,1,1,1},
      {0,0,0,0,1,1,1,1},
      {0,0,0,1,1,1,1,0},
      {0,0,0,1,1,1,0,0},
      {0,0,0,1,1,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
    },
    {
      {0,0,0,0,0,0,0,0},
      {1,1,1,1,1,0,0,0},
      {1,1,1,1,1,1,0,0},
      {0,0,0,1,1,1,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
    },
  },
  {
    {
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,1,1,1},
      {0,0,0,0,1,1,1,1},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
    },
    {
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {1,1,1,1,1,0,0,0},
      {1,1,1,1,1,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
    },
  },
};

bool mouth_disp1[4][ROW_SIZE][ROW_SIZE] = {
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
    {1,0,0,0,0,0,0,1},
    {1,1,0,0,0,0,1,1},
    {0,1,1,0,0,1,1,0},
    {0,0,1,1,1,1,0,0},
    {0,0,0,0,0,0,0,0},
  },
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
};

bool nose_disp1[1][ROW_SIZE][ROW_SIZE] = {
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,0,0},
    {0,0,0,1,1,1,1,0},
    {0,0,1,0,0,0,0,1},
    {0,1,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
};


bool eye_disp_heart1[NUM_FRAMES][2][ROW_SIZE][ROW_SIZE] = {
  {
    {
      {0,0,0,0,1,1,0,0},
      {0,0,0,1,1,1,1,0},
      {0,0,0,1,1,1,1,1},
      {0,0,0,1,1,1,1,1},
      {0,0,0,0,1,1,1,1},
      {0,0,0,0,0,1,1,1},
      {0,0,0,0,0,0,1,1},
      {0,0,0,0,0,0,0,1},
    },
    {
      {0,0,1,1,0,0,0,0},
      {0,1,1,1,1,0,0,0},
      {1,1,1,1,1,0,0,0},
      {1,1,1,1,1,0,0,0},
      {1,1,1,1,0,0,0,0},
      {1,1,1,0,0,0,0,0},
      {1,1,0,0,0,0,0,0},
      {1,0,0,0,0,0,0,0},
    },
  },
  {
    {
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,1,0,0},
      {0,0,0,0,1,1,1,0},
      {0,0,0,0,1,1,1,1},
      {0,0,0,0,0,1,1,1},
      {0,0,0,0,0,0,1,1},
      {0,0,0,0,0,0,0,1},
      {0,0,0,0,0,0,0,0},
    },
    {
      {0,0,0,0,0,0,0,0},
      {0,0,1,0,0,0,0,0},
      {0,1,1,1,0,0,0,0},
      {1,1,1,1,0,0,0,0},
      {1,1,1,0,0,0,0,0},
      {1,1,0,0,0,0,0,0},
      {1,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
    },
  }
};

bool mouth_disp_heart1[4][ROW_SIZE][ROW_SIZE] = {
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1},
    {0,0,0,0,0,1,1,1},
    {0,0,1,1,1,1,0,0},
    {1,1,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,1,1,0,0,0,0},
    {0,1,1,0,0,0,0,0},
    {1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
};




bool eye_disp2[NUM_MATRICES][2][ROW_SIZE][ROW_SIZE];

bool mouth_disp2[NUM_MATRICES][ROW_SIZE][ROW_SIZE];

bool nose_disp2[NUM_MATRICES][ROW_SIZE][ROW_SIZE];

bool eye_disp_heart2[NUM_MATRICES][2][ROW_SIZE][ROW_SIZE];

bool mouth_disp_heart2[NUM_MATRICES][ROW_SIZE][ROW_SIZE];


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
void createSecond(bool image_disp1[][ROW_SIZE][ROW_SIZE], bool image_disp2[][ROW_SIZE][ROW_SIZE], int num_matrices, bool invert = true) {
  // Iterate over all displays. The second set will be updated in reverse order
  int cur;
  for (int disp_num = 0; disp_num < num_matrices; disp_num++) {
    for (int i = 0; i < ROW_SIZE; i++) { // rows
      for (int j = 0; j < ROW_SIZE; j++) { // columns
        if (invert) {
          cur = num_matrices-1-disp_num;
        } else {
          cur = disp_num;
        }
        image_disp2[cur][i][j] = image_disp1[disp_num][i][j];
      }
    }
  }
}




void setup() {
  myDisplay.begin();
  myDisplay.control(MD_MAX72XX::INTENSITY, 0); // set brightness level (0 is min, 15 is max)
  myDisplay.clear();

  // Eye flip vertically
  // for (int i = 0; i < NUM_FRAMES; i++) {
  //   flipVerticle(eye_disp1[i], 0);
  //   flipVerticle(eye_disp1[i], 1);
  // }
  // Mouth flip vertically
  flipVerticle(mouth_disp1, 0);
  flipVerticle(mouth_disp1, 1);
  flipVerticle(mouth_disp1, 2);
  flipVerticle(mouth_disp1, 3);
  // Nose flip vertically
  // flipVerticle(nose_disp1, 0);
  // Mouth flip heart vertically
  flipVerticle(mouth_disp_heart1, 0);
  flipVerticle(mouth_disp_heart1, 1);
  flipVerticle(mouth_disp_heart1, 2);
  flipVerticle(mouth_disp_heart1, 3);

  // Create the second set of matrices
  // Since the second eye is backward
  for (int i = 0; i < NUM_FRAMES; i++) {
    createSecond(eye_disp1[i], eye_disp2[i], 2);
  }
  createSecond(mouth_disp1, mouth_disp2, 4, false);
  createSecond(nose_disp1, nose_disp2, 1);
  for (int i = 0; i < NUM_FRAMES_HEART; i++) {
    createSecond(eye_disp_heart1[i], eye_disp_heart2[i], 2);
  }
  createSecond(mouth_disp_heart1, mouth_disp_heart2, 4, false);
  // Eye flip horizontally
  for (int i = 0; i < NUM_FRAMES; i++) {
    flipHorizontal(eye_disp2[i], 0);
    flipHorizontal(eye_disp2[i], 1);
    // flipVerticle(eye_disp2[i], 0);
    // flipVerticle(eye_disp2[i], 1);

    flipHorizontal(eye_disp_heart2[i], 0);
    flipHorizontal(eye_disp_heart2[i], 1);
  }
  // Mouth flip vertically
  flipVerticle(mouth_disp2, 0);
  flipVerticle(mouth_disp2, 1);
  flipVerticle(mouth_disp2, 2);
  flipVerticle(mouth_disp2, 3);
  // Nose flip horizontally and vertically
  flipHorizontal(nose_disp2, 0);
  // Mouth flip heart vertically
  flipVerticle(mouth_disp_heart2, 0);
  flipVerticle(mouth_disp_heart2, 1);
  flipVerticle(mouth_disp_heart2, 2);
  flipVerticle(mouth_disp_heart2, 3);

  // Display everything
  displayLines(eye_disp1[0], 0, 2);
  displayLines(mouth_disp1, 2, 4);
  displayLines(nose_disp1, 6, 1);
  displayLines(nose_disp2, 7, 1);
  displayLines(mouth_disp2, 8, 4);
  displayLines(eye_disp2[0], 12, 2);
}



void loop() {
  // Set heart mode to have a 1% chance
  bool heartMode = (random(20) == 0);
  // bool heartMode = (random(2) == 1); 

  // Base display
  if (heartMode) { // Show heart
    displayLines(eye_disp_heart1[0], 0, 2);
    displayLines(eye_disp_heart2[0], 12, 2);

    displayLines(mouth_disp_heart1, 2, 4);
    displayLines(mouth_disp_heart2, 8, 4);
  } else { // Show normal
    displayLines(eye_disp1[0], 0, 2);
    displayLines(eye_disp2[0], 12, 2);

    displayLines(mouth_disp1, 2, 4);
    displayLines(mouth_disp2, 8, 4);

    // Delay before blink
    delay(random(2000, 10000));
  }

  // Blink loop
  if (heartMode) {
    for (int iter = 0; iter < NUM_ITERS_HEART; iter++) {
      for (int i = 1; i < NUM_FRAMES_HEART; i++) {
        displayLines(eye_disp_heart1[i], 0, 2);
        displayLines(eye_disp_heart2[i], 12, 2);
        delay(50);
      }
      delay(2000);
      for (int i = NUM_FRAMES_HEART-1; i >= 0; i--) {
        displayLines(eye_disp_heart1[i], 0, 2);
        displayLines(eye_disp_heart2[i], 12, 2);
        delay(50);
      }
      delay(2000);
    }
  } else {
    for (int i = 1; i < NUM_FRAMES; i++) {
      displayLines(eye_disp1[i], 0, 2);
      displayLines(eye_disp2[i], 12, 2);
      delay(50);
    }
    delay(20);
    for (int i = NUM_FRAMES-1; i >= 0; i--) {
      displayLines(eye_disp1[i], 0, 2);
      displayLines(eye_disp2[i], 12, 2);
      delay(50);
    }
  }
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
