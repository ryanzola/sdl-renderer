#include "display.h"
#include "triangle.h"

void int_swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
  // find the slope of the left and right sides of the triangle 
  float inv_slope_1 = (x1 - x0) / (float)(y1 - y0);
  float inv_slope_2 = (x2 - x0) / (float)(y2 - y0);

  // start x_start and x_end from the top verticies (x0, y0) 
  float x_start = x0;
  float x_end = x0;

  // loop all scanlines from top to bottom 
  for(int y = y0; y <= y2; y++) {
    draw_line(x_start, y, x_end, y, color);

    x_start += inv_slope_1;
    x_end += inv_slope_2;
  }
}

void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
  // find the slop of the left and right sides of the triangle
  float inv_slope_1 = (x2 - x0) / (float)(y2 - y0);
  float inv_slope_2 = (x2 - x1) / (float)(y2 - y1);

  // start x_start and x_end from the bottom verticies (x2, y2)
  float x_start = x2;
  float x_end = x2;

  // loop all scanlines from bottom to top 
  for(int y = y2; y >= y0; y--) {
    draw_line(x_start, y, x_end, y, color);

    x_start -= inv_slope_1;
    x_end -= inv_slope_2;
  }
}

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
  // sort the verticies by y coordinate ascending (y0 <= y1 <= y2)
  if(y0 > y1) {
    int_swap(&y0, &y1);
    int_swap(&x0, &x1);
  }

  if(y1 > y2) {
    int_swap(&y1, &y2);
    int_swap(&x1, &x2);
  }

  if(y0 > y1) {
    int_swap(&y0, &y1);
    int_swap(&x0, &x1);
  }

  if(y1 == y2) {
    // just draw a flat bottom triangle
    fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
  }
  else if(y0 == y1) {
    // just draw a flat top triangle
    fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
  } else {
    // draw a flat bottom triangle and a flat top triangle
    // calculate the new vertex (Mx, My) using triangle similarity
    int My = y1;
    int Mx = ((float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0)) + x0;

    // draw flat bottom triangle
    fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);

    // draw flat top triangle 
    fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
  }
}
