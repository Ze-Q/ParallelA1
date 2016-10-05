// Group 21

#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


int rectify(int input)
{
  if (input < 0)
  {
    return 0;
  }
  return input;
}

int center_image(int input)
{
  if (input < 127)
  {
    return 127;
  }
  return input;
}

void process(char *input_filename, char *output_filename, int num_threads)
{
  unsigned error;
  unsigned char *image, *new_image;
  unsigned width, height;
  unsigned output_width, output_height;

  // load image to process
  error = lodepng_decode32_file(&image, &width, &height, input_filename);
  if (error)
    printf("error %u: %s\n", error, lodepng_error_text(error));

  output_width = width;
  output_height = height;
  new_image = malloc(output_width * output_height * 4 * sizeof(unsigned char));

// process image in parallel, by giving rows of data to different threads
#pragma omp parallel for num_threads(num_threads)
  for (int i = 0; i < height; i++)
  {
    int output_i = i;

    for (int j = 0; j < width; j++)
    {

      int output_j = j;

      // repeat for rgb separately
      for (int offset = 0; offset <= 2; offset++)
      {
        // apply rectify
        int newValue = rectify(center_image(image[4 * (output_width * output_i + output_j) + offset]));
        new_image[4 * (output_width * output_i + output_j) + offset] = newValue;
      }

      // write the same alpha to the output
      new_image[4 * (output_width * output_i + output_j) + 3] = image[4 * (width * i + j) + 3];
    }
  }

  lodepng_encode32_file(output_filename, new_image, output_width, output_height);

  free(image);
  free(new_image);
}

int main(int argc, char *argv[])
{
  char *input_filename = argv[1];
  char *output_filename = argv[2];

  // get in the number of threads and check if it's valid (>0)
  int num_threads = atoi(argv[3]);
  if (num_threads <= 0)
    printf("error: invalid number of threads, please input a positive integer\n");

  process(input_filename, output_filename, num_threads);

  return 0;
}
