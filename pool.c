// Group 21

#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

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

  output_width = width / 2;
  output_height = height / 2;
  new_image = malloc(output_width * output_height * 4 * sizeof(unsigned char));

  // process image in parallel, by giving each byte of RGBA data to different threads
  #pragma omp parallel for num_threads(num_threads)
  for (int offset = 0; offset < 4; offset++)
  {
    for (int i = 2; i < height + 2; i += 2)
    {
      for (int j = 2; j < width + 2; j += 2)
      {
        int maxVal = 0;
        
        // apply 2x2 max-pooling
        //#pragma omp parallel for reduction(max : maxVal)
        for (int ii = i - 2; ii < i; ii++)
        {
          //#pragma omp parallel for reduction(max : maxVal)
          for (int jj = j - 2; jj < j; jj++)
          {
            int idx = 4 * (width * ii + jj) + offset;
            if (image[idx] > maxVal)
            {
              maxVal = image[idx];
            }
          }
        }
        new_image[4 * (output_width * (i / 2 - 1) + (j / 2 - 1)) + offset] = maxVal;
      }
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
