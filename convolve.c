// Group 21

#include "lodepng.h"
#include "wm.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// this function puts an input in the range of 0 to 255
int normalize(int input) 
{
  if (input < 0) {
    return 0;
  } 
  else if (input > 255) {
    return 255;
  } 

  return input;
}

void process(char* input_filename, char* output_filename, int num_threads)
{
  unsigned error;
  unsigned char *image, *new_image;
  unsigned width, height;
  unsigned output_width, output_height;

  // load image to process
  error = lodepng_decode32_file(&image, &width, &height, input_filename);
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

  // create output image which is slightly smaller
  output_width = width - 2;
  output_height = height - 2;
  new_image = malloc(output_width * output_height * 4 * sizeof(unsigned char));

  // process image in parallel, by giving rows of data to different threads
  #pragma omp parallel for num_threads(num_threads)
  for (int i = 1; i < height-1; i++) {

    // the output row number if smaller by 1, since we are cutting i=0
    int output_i = i - 1;

    for (int j = 1; j < width-1; j++) {	

      // the output column number if smaller by 1, since we are cutting j=0
      int output_j = j - 1;

      // repeat for rgb separately
      for (int offset = 0; offset <= 2; offset ++) {
        int newValue = 0; // this will store the result of the formula

        // apply the convolve formula
        for (int ii = 0; ii <= 2; ii++) {
          for (int jj = 0; jj <=2; jj++) {
            newValue += image[4 * (width * (i + ii - 1) + (j + jj -1)) + offset] * w[ii][jj];
          }
        }
        
        new_image[4*(output_width*output_i + output_j) + offset] = normalize(newValue);
      }

      // write the same alpha to the output
      new_image[4*(output_width*output_i + output_j) + 3] = image[4*(width*i + j) + 3];
    }
  }

  lodepng_encode32_file(output_filename, new_image, output_width, output_height);

  free(image);
  free(new_image);
}

int main(int argc, char *argv[])
{
  char* input_filename = argv[1];
  char* output_filename = argv[2];

  // get in the number of threads and check if it's valid (>0)
  int num_threads = atoi(argv[3]);
  if(num_threads <= 0) printf("error: invalid number of threads, please input a positive integer\n");

  process(input_filename, output_filename, num_threads);

  return 0;
}
