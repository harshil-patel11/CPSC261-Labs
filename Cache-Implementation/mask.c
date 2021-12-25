#include <stdio.h>
#include <stdlib.h>
#include "mask.h"

// When completed this file will contain several versions of the
// mask() function and conditional compilation will be used to
// determine which version runs.  Each version will be named mask1,
// mask2, and so on. To select the version of the mask function to
// use, change the following declaration to the version of the mask()
// to be used.  All versions of mask() must produce a correct result.

// The provided Makefile also includes the possibility of creating an
// executable called "timemask-maskN" (replacing maskN with any
// version you'd like to test). You may compile it with "make
// timemask-maskN" then run "./timemask-maskN".

#ifndef MASK_VERSION
#define MASK_VERSION mask6
#endif

// You are only allowed to change the contents of this file with
// respect to improving the performance of this program. You may not
// change the data structure for each image or the parameters for the
// function mask. You may, however, change the structure of the code
// and the types/implementation of local variables (col, row, weight,
// etc.).


// You are required to document the changes you make in the README.txt
// file. For each entry in the README.txt file there is to be a
// version of the matching mask() function here such that the markers
// can see, and run if needed, the actual code that you used to
// produce your results for the logged change to the mask function.

static inline long mask0(long oldImage[N][N], long newImage[N][N], int rows, int cols) {
  return baseMask(oldImage, newImage, rows, cols);
}

// For each version of mask you are testing duplicate the function
// below, rename it to a different name, and make your changes. To use
// the new version, change the #define above to use the current version.

static inline long mask1(long oldImage[N][N], long newImage[N][N], int rows, int cols) {

  // TODO This function should contain code that produces the same
  // output as baseMask, but is expected to run faster than mask0 by
  // making better use of caching.
  
  int i, j;
  int col, row;
  long check = 0;

  long (*weight)[N] = calloc(N * N, sizeof(long));
  
  //initialize the new image
  for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++) {
      newImage[i][j] = WEIGHT_CENTRE * oldImage[i][j];
      weight[i][j] = WEIGHT_CENTRE;
    }
  
  // Count the cells to the top left
  for (i = 1; i < rows; i++) {
    row = i - 1;
    for (j = 1; j < cols; j++) {
      col = j - 1;
      newImage[i][j] += WEIGHT_CORNER * oldImage[row][col];
      weight[i][j] += WEIGHT_CORNER;
    }
  }
  
  // Count the cells immediately above
  for (i = 1; i < rows; i++) {
    row = i - 1;
    for (j = 0; j < cols; j++) {      
      newImage[i][j] += WEIGHT_SIDE * oldImage[row][j];
      weight[i][j] += WEIGHT_SIDE;
    }
  }
  
  // Count the cells to the top right
  for (i = 1; i < rows; i++) {
    row = i - 1;
    for (j = 0; j < cols-1; j++) {
      col = j + 1;
      newImage[i][j] += WEIGHT_CORNER * oldImage[row][col];
      weight[i][j] += WEIGHT_CORNER;
    }
  }
  
  // Count the cells to the immediate left
  for (i = 0; i < rows; i++) {
    for (j = 1; j < cols; j++) {
      col = j - 1;
      newImage[i][j] += WEIGHT_SIDE * oldImage[i][col];
      weight[i][j] += WEIGHT_SIDE;
    }
  }
  
  // Count the cells to the immediate right
  for (i = 0; i < rows; i++) {    
    for (j = 0; j < cols-1; j++) {
      col = j + 1;
      newImage[i][j] += WEIGHT_SIDE * oldImage[i][col];
      weight[i][j] += WEIGHT_SIDE;
    }
  }
  
  // Count the cells to the bottom left
  for (i = 0; i < rows - 1; i++) {
    row = i + 1;
    for (j = 1; j < cols; j++) {      
      col = j - 1;
      newImage[i][j] += WEIGHT_CORNER * oldImage[row][col];
      weight[i][j] += WEIGHT_CORNER;
    }
  }
  
  // Count the cells immediately below
  for (i = 0; i < rows - 1; i++) {
    row = i + 1;
    for (j = 0; j < cols; j++) {      
      newImage[i][j] += WEIGHT_SIDE * oldImage[row][j];
      weight[i][j] += WEIGHT_SIDE;
    }
  }
  
  // Count the cells to the bottom right
  for (i = 0; i < rows - 1; i++) {
    row = i + 1;
    for (j = 0; j < cols - 1; j++) {
      col = j + 1;
      newImage[i][j] += WEIGHT_CORNER * oldImage[row][col];
      weight[i][j] += WEIGHT_CORNER;
    }
  }

  // Produce the final result
  for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++) {
      newImage[i][j] = newImage[i][j] / weight[i][j];
      check += newImage[i][j];
    }
  
  return check;
}



static inline long mask2(long oldImage[N][N], long newImage[N][N], int rows, int cols) {

  // TODO This function should contain code that produces the same
  // output as baseMask, but is expected to run faster than mask1 (or
  // mask0) by making better use of caching.
  
  int i, j;
  //int col, row;
  long check = 0;

  long (*weight)[N] = calloc(N * N, sizeof(long));
  
  //initialize the new image
  for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++) {
      newImage[i][j] = WEIGHT_CENTRE * oldImage[i][j];
      weight[i][j] = WEIGHT_CENTRE;
    }
  
  // Count the cells to the top left
  for (i = 1; i < rows; i++) {
    // row = i - 1;
    for (j = 1; j < cols; j++) {
      //col = j - 1;
      newImage[i][j] += WEIGHT_CORNER * oldImage[i - 1][j - 1];
      weight[i][j] += WEIGHT_CORNER;
    }
  }
  
  // Count the cells immediately above
  for (i = 1; i < rows; i++) {
    //row = i - 1;
    for (j = 0; j < cols; j++) {      
      newImage[i][j] += WEIGHT_SIDE * oldImage[i - 1][j];
      weight[i][j] += WEIGHT_SIDE;
    }
  }
  
  // Count the cells to the top right
  for (i = 1; i < rows; i++) {
    //row = i - 1;
    for (j = 0; j < cols-1; j++) {
      //col = j + 1;
      newImage[i][j] += WEIGHT_CORNER * oldImage[i - 1][j + 1];
      weight[i][j] += WEIGHT_CORNER;
    }
  }
  
  // Count the cells to the immediate left
  for (i = 0; i < rows; i++) {
    for (j = 1; j < cols; j++) {
      // col = j - 1;
      newImage[i][j] += WEIGHT_SIDE * oldImage[i][j - 1];
      weight[i][j] += WEIGHT_SIDE;
    }
  }
  
  // Count the cells to the immediate right
  for (i = 0; i < rows; i++) {    
    for (j = 0; j < cols-1; j++) {
      //col = j + 1;
      newImage[i][j] += WEIGHT_SIDE * oldImage[i][j + 1];
      weight[i][j] += WEIGHT_SIDE;
    }
  }
  
  // Count the cells to the bottom left
  for (i = 0; i < rows - 1; i++) {
    //row = i + 1;
    for (j = 1; j < cols; j++) {      
      // col = j - 1;
      newImage[i][j] += WEIGHT_CORNER * oldImage[i + 1][j - 1];
      weight[i][j] += WEIGHT_CORNER;
    }
  }
  
  // Count the cells immediately below
  for (i = 0; i < rows - 1; i++) {
    //row = i + 1;
    for (j = 0; j < cols; j++) {      
      newImage[i][j] += WEIGHT_SIDE * oldImage[i + 1][j];
      weight[i][j] += WEIGHT_SIDE;
    }
  }
  
  // Count the cells to the bottom right
  for (i = 0; i < rows - 1; i++) {
    //row = i + 1;
    for (j = 0; j < cols - 1; j++) {
      //col = j + 1;
      newImage[i][j] += WEIGHT_CORNER * oldImage[i + 1][j + 1];
      weight[i][j] += WEIGHT_CORNER;
    }
  }

  // Produce the final result
  for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++) {
      newImage[i][j] = newImage[i][j] / weight[i][j];
      check += newImage[i][j];
    }
  
  return check;
}



static inline long mask3(long oldImage[N][N], long newImage[N][N], int rows, int cols) {

  // TODO This function should contain code that produces the same
  // output as baseMask, but is expected to run faster than mask1 (or
  // mask0) by making better use of caching.
  
  int i, j;
  long check = 0;

  long (*weight)[N] = calloc(N * N, sizeof(long));
  
  //initialize the new image
  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      newImage[i][j] = WEIGHT_CENTRE * oldImage[i][j];
      weight[i][j] = WEIGHT_CENTRE;

      // Count the cells to the top left
      if (i-1 >= 0 && j-1 >= 0) {
        newImage[i][j] += WEIGHT_CORNER * oldImage[i - 1][j - 1];
        weight[i][j] += WEIGHT_CORNER;
      }

      // Count the cells immediately above
      if (i-1 >= 0) {
        newImage[i][j] += WEIGHT_SIDE * oldImage[i - 1][j];
        weight[i][j] += WEIGHT_SIDE;
      }

      // Count the cells to the top right
      if (i-1 >= 0 && j+1 < cols) {
        newImage[i][j] += WEIGHT_CORNER * oldImage[i - 1][j + 1];
        weight[i][j] += WEIGHT_CORNER;
      }

      // Count the cells to the immediate left
      if (j-1>=0) {
        newImage[i][j] += WEIGHT_SIDE * oldImage[i][j - 1];
        weight[i][j] += WEIGHT_SIDE;
      }
      
      // Count the cells to the immediate right
      if (j+1 < cols) {
        newImage[i][j] += WEIGHT_SIDE * oldImage[i][j + 1];
        weight[i][j] += WEIGHT_SIDE;
      }

      // Count the cells to the bottom left
      if(i+1 < rows && j-1 >= 0) {
        newImage[i][j] += WEIGHT_CORNER * oldImage[i + 1][j - 1];
        weight[i][j] += WEIGHT_CORNER;
      }

      // Count the cells immediately below
      if (i+1 < rows) {
        newImage[i][j] += WEIGHT_SIDE * oldImage[i + 1][j];
        weight[i][j] += WEIGHT_SIDE;
      }

      // Count the cells to the bottom right
      if (i+1 < rows && j+1 < cols) {
        newImage[i][j] += WEIGHT_CORNER * oldImage[i + 1][j + 1];
        weight[i][j] += WEIGHT_CORNER;
      }
    }
  }

  // Produce the final result
  for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++) {
      newImage[i][j] = newImage[i][j] / weight[i][j];
      check += newImage[i][j];
    }
  
  return check;
}



static inline long mask4(long oldImage[N][N], long newImage[N][N], int rows, int cols) {

  // TODO This function should contain code that produces the same
  // output as baseMask, but is expected to run faster than mask1 (or
  // mask0) by making better use of caching.
  
  register int i;
  register int j;
  register long check = 0;

  register long (*weight)[N] = calloc(N * N, sizeof(long));
  
  //initialize the new image
  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      newImage[i][j] = WEIGHT_CENTRE * oldImage[i][j];
      weight[i][j] = WEIGHT_CENTRE;

      // Count the cells to the top left
      if (i-1 >= 0 && j-1 >= 0) {
        newImage[i][j] += WEIGHT_CORNER * oldImage[i - 1][j - 1];
        weight[i][j] += WEIGHT_CORNER;
      }

      // Count the cells immediately above
      if (i-1 >= 0) {
        newImage[i][j] += WEIGHT_SIDE * oldImage[i - 1][j];
        weight[i][j] += WEIGHT_SIDE;
      }

      // Count the cells to the top right
      if (i-1 >= 0 && j+1 < cols) {
        newImage[i][j] += WEIGHT_CORNER * oldImage[i - 1][j + 1];
        weight[i][j] += WEIGHT_CORNER;
      }

      // Count the cells to the immediate left
      if (j-1>=0) {
        newImage[i][j] += WEIGHT_SIDE * oldImage[i][j - 1];
        weight[i][j] += WEIGHT_SIDE;
      }
      
      // Count the cells to the immediate right
      if (j+1 < cols) {
        newImage[i][j] += WEIGHT_SIDE * oldImage[i][j + 1];
        weight[i][j] += WEIGHT_SIDE;
      }

      // Count the cells to the bottom left
      if(i+1 < rows && j-1 >= 0) {
        newImage[i][j] += WEIGHT_CORNER * oldImage[i + 1][j - 1];
        weight[i][j] += WEIGHT_CORNER;
      }

      // Count the cells immediately below
      if (i+1 < rows) {
        newImage[i][j] += WEIGHT_SIDE * oldImage[i + 1][j];
        weight[i][j] += WEIGHT_SIDE;
      }

      // Count the cells to the bottom right
      if (i+1 < rows && j+1 < cols) {
        newImage[i][j] += WEIGHT_CORNER * oldImage[i + 1][j + 1];
        weight[i][j] += WEIGHT_CORNER;
      }
    }
  }

  // Produce the final result
  for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++) {
      newImage[i][j] = newImage[i][j] / weight[i][j];
      check += newImage[i][j];
    }
  
  return check;
}


static inline long mask5(long oldImage[N][N], long newImage[N][N], int rows, int cols) {

  // TODO This function should contain code that produces the same
  // output as baseMask, but is expected to run faster than mask1 (or
  // mask0) by making better use of caching.
  
  register int i;
  register int j;
  register long check = 0;

  register long (*weight)[N] = calloc(N * N, sizeof(long));
  
  //initialize the new image
  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      newImage[i][j] = WEIGHT_CENTRE * oldImage[i][j];
      weight[i][j] = WEIGHT_CENTRE;

      // Count the cells immediately above
      if (i-1 >= 0) {
        newImage[i][j] += WEIGHT_SIDE * oldImage[i - 1][j];
        weight[i][j] += WEIGHT_SIDE;
        
        // Count the cells to the top right
        if (j+1 < cols) {
          newImage[i][j] += WEIGHT_CORNER * oldImage[i - 1][j + 1];
          weight[i][j] += WEIGHT_CORNER;
        }
      }      

      // Count the cells to the immediate left
      if (j-1 >= 0) {
        newImage[i][j] += WEIGHT_SIDE * oldImage[i][j - 1];
        weight[i][j] += WEIGHT_SIDE;
        
        // Count the cells to the top left
        if (i-1 >= 0) {
          newImage[i][j] += WEIGHT_CORNER * oldImage[i - 1][j - 1];
          weight[i][j] += WEIGHT_CORNER;
        }
      }
      
      // Count the cells to the immediate right
      if (j+1 < cols) {
        newImage[i][j] += WEIGHT_SIDE * oldImage[i][j + 1];
        weight[i][j] += WEIGHT_SIDE;

        // Count the cells to the bottom right
        if (i+1 < rows) {
          newImage[i][j] += WEIGHT_CORNER * oldImage[i + 1][j + 1];
          weight[i][j] += WEIGHT_CORNER;
        }
      }      

      // Count the cells immediately below
      if (i+1 < rows) {
        newImage[i][j] += WEIGHT_SIDE * oldImage[i + 1][j];
        weight[i][j] += WEIGHT_SIDE;

        // Count the cells to the bottom left
        if(j-1 >= 0) {
          newImage[i][j] += WEIGHT_CORNER * oldImage[i + 1][j - 1];
          weight[i][j] += WEIGHT_CORNER;
        }
      }
    }
  }

  // Produce the final result
  for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++) {
      newImage[i][j] = newImage[i][j] / weight[i][j];
      check += newImage[i][j];
    }
  
  return check;
}



static inline long mask6(long oldImage[N][N], long newImage[N][N], int rows, int cols) {

  // TODO This function should contain code that produces the same
  // output as baseMask, but is expected to run faster than mask1 (or
  // mask0) by making better use of caching.
  
  register int i;
  register int j;
  register long check = 0;

  register long (*weight)[N] = calloc(N * N, sizeof(long));
  
  //middle
  for (i = 1; i < rows-1; i++) {
    for (j = 1; j < cols-1; j++) {
      newImage[i][j] = WEIGHT_CENTRE * oldImage[i][j];
      
      newImage[i][j] += WEIGHT_SIDE * (oldImage[i - 1][j] + oldImage[i][j - 1]
                        + oldImage[i][j + 1] + oldImage[i + 1][j]);
      
      newImage[i][j] += WEIGHT_CORNER * (oldImage[i - 1][j - 1] + oldImage[i - 1][j + 1]
                        + oldImage[i + 1][j - 1] + oldImage[i + 1][j + 1]);

      weight[i][j] = WEIGHT_CENTRE + 4*(WEIGHT_SIDE) + 4*(WEIGHT_CORNER);
    }
  }

  //sides
  //bottom
  for (j = 1; j < cols-1; j++) {
    weight[rows-1][j] = WEIGHT_CENTRE + 3*(WEIGHT_SIDE) + 2*WEIGHT_CORNER;
    newImage[rows-1][j] = WEIGHT_CENTRE * oldImage[rows-1][j] + WEIGHT_SIDE*(oldImage[rows-1][j-1] 
                    + oldImage[rows-2][j] + oldImage[rows-1][j+1]) + 
                    WEIGHT_CORNER*(oldImage[rows-2][j-1] + oldImage[rows-2][j+1]);
  }

  //top
  for (j = 1; j < cols-1; j++) {
    weight[0][j] = WEIGHT_CENTRE + 3*(WEIGHT_SIDE) + 2*WEIGHT_CORNER;
    newImage[0][j] = WEIGHT_CENTRE * oldImage[0][j] + WEIGHT_SIDE*(oldImage[0][j-1] 
                    + oldImage[1][j] + oldImage[0][j+1]) + 
                    WEIGHT_CORNER*(oldImage[1][j-1] + oldImage[1][j+1]);
  }  

  //left
  for (i = 1; i < rows-1; i++) {
    weight[i][0] = WEIGHT_CENTRE + 3*(WEIGHT_SIDE) + 2*WEIGHT_CORNER;
    newImage[i][0] = WEIGHT_CENTRE * oldImage[i][0] + WEIGHT_SIDE*(oldImage[i-1][0] 
                    + oldImage[i][1] + oldImage[i+1][0]) + 
                    WEIGHT_CORNER*(oldImage[i-1][1] + oldImage[i+1][1]);
  }

  //right
  for (i = 1; i < rows-1; i++) {
    weight[i][cols-1] = WEIGHT_CENTRE + 3*(WEIGHT_SIDE) + 2*WEIGHT_CORNER;
    newImage[i][cols-1] = WEIGHT_CENTRE * oldImage[i][cols-1] + WEIGHT_SIDE*(oldImage[i-1][cols-1] 
                    + oldImage[i][cols-2] + oldImage[i+1][cols-1]) + 
                    WEIGHT_CORNER*(oldImage[i-1][cols-2] + oldImage[i+1][cols-2]);
  }

  //corners
  //top left  
  newImage[0][0] = WEIGHT_CENTRE * oldImage[0][0] + WEIGHT_CORNER * oldImage[1][1]
                  + WEIGHT_SIDE * (oldImage[0][1] + oldImage[1][0]);
  weight[0][0] = WEIGHT_CENTRE + 2*(WEIGHT_SIDE) + WEIGHT_CORNER;                

  //top right
  newImage[0][cols-1] = WEIGHT_CENTRE * oldImage[0][cols-1] + WEIGHT_CORNER * oldImage[1][cols-2]
                  + WEIGHT_SIDE * (oldImage[0][cols-2] + oldImage[1][cols-1]);
  weight[0][cols-1] = WEIGHT_CENTRE + 2*(WEIGHT_SIDE) + WEIGHT_CORNER;

  //bottom left
  newImage[rows-1][0] = WEIGHT_CENTRE * oldImage[rows-1][0] + WEIGHT_CORNER * oldImage[rows-2][1]
                  + WEIGHT_SIDE * (oldImage[rows-1][1] + oldImage[rows-2][0]);
  weight[rows-1][0] = WEIGHT_CENTRE + 2*(WEIGHT_SIDE) + WEIGHT_CORNER;

  //bottom right
  newImage[rows-1][cols-1] = WEIGHT_CENTRE * oldImage[rows-1][cols-1] + WEIGHT_CORNER * oldImage[rows-2][cols-2]
                  + WEIGHT_SIDE * (oldImage[rows-1][cols-2] + oldImage[rows-2][cols-1]);                                
  weight[rows-1][cols-1] = WEIGHT_CENTRE + 2*(WEIGHT_SIDE) + WEIGHT_CORNER;


  // Produce the final result
  for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++) {
      newImage[i][j] = newImage[i][j] / weight[i][j];
      check += newImage[i][j];
    }    
  
  return check;
}


long mask(long oldImage[N][N], long newImage[N][N], int rows, int cols) {
  return MASK_VERSION(oldImage, newImage, rows, cols);
}

