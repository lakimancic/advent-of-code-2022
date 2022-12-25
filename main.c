#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <time.h>
#include <unistd.h>

// Consts
#define SIZE_X 80
#define SIZE_Y 20

const float BOX_SIZE = 5;
const float CAM_DIST_OBJ = 100;
const float CAM_DIST_VIEW = 40;

// color
typedef struct
{
  int r, g, b;
}Color;

// Functions
int cmp_color(Color c1, Color c2) {
  return c1.r == c2.r && c1.b == c2.b && c1.g == c2.g;
}

void set_color(Color c) {
  printf("\033[38;2;%d;%d;%dm", c.r, c.g, c.b);
}

void get_projection(float res[2], float point[3]) {
  res[0] = point[0];
  res[1] = point[1];
}

float get_rotated_x(float x, float y, float z, float a, float b, float c) {
  return y * sin(a) * sin(b) * cos(c) - z * cos(a) * sin(b) * cos(c) + y * cos(a) * sin(c) + z * sin(a) * sin(c) + x * cos(b) * cos(c);
}

float get_rotated_y(float x, float y, float z, float a, float b, float c) {
  return y * cos(a) * cos(c) + z * sin(a) * cos(c) - y * sin(a) * sin(b) * sin(c) + z * cos(a) * sin(b) * sin(c) - x * cos(b) * sin(c);
}

float get_rotated_z(float x, float y, float z, float a, float b, float c) {
  return z * cos(a) * cos(b) - y * sin(a) * cos(b) + x * sin(b);
}

void set_point(float x, float y, float z, float a, float b, float c, char ch, Color col, char output[SIZE_Y][SIZE_X], int zbuffer[SIZE_Y][SIZE_X], Color colors[SIZE_Y][SIZE_X]) {
  float nx = get_rotated_x(x, y, z, a, b, c);
  float ny = get_rotated_y(x, y, z, a, b, c);
  float nz = get_rotated_z(x, y, z, a, b, c);
  float point[] = {nx, ny, nz};
  float proj[2]; get_projection(proj, point);
  int xi = round(proj[0] * 2.5) + SIZE_X / 2, yi = round(proj[1]) + SIZE_Y / 2;
  if(yi >= 0 && yi < SIZE_Y && xi >= 0 && xi < SIZE_X) {
    if(nz > zbuffer[yi][xi]) {
      zbuffer[yi][xi] = nz;
      output[yi][xi] = ch;
      colors[yi][xi] = col;
    }
  }
}

int main(int argc, char *argv[]) {
  char output[SIZE_Y][SIZE_X];
  int zbuffer[SIZE_Y][SIZE_X];
  Color colors[SIZE_Y][SIZE_X];
  char snowflakes[SIZE_Y][SIZE_X];
  printf("\e[?25l");
  float a = 0, b = 0, c = 0;
  Color temp = (Color){ 255, 0, 0 };
  for(int i=0;i<SIZE_Y;i++) for(int j=0;j<SIZE_X;j++) snowflakes[i][j] = 0;
  while (1) {
    for(int i=SIZE_Y-1;i>0;i--) {
        for(int j=0;j<SIZE_X;j++) snowflakes[i][j] = snowflakes[i-1][j];
    }
    for(int j=0;j<SIZE_X;j++) snowflakes[0][j] = 0;
    snowflakes[0][rand() % SIZE_X] = 1;
    for (int i = 0; i < SIZE_Y; i++)
      for (int j = 0; j < SIZE_X; j++) output[i][j] = snowflakes[i][j] ? '*' : ' ', zbuffer[i][j] = -1000, colors[i][j] = (Color){255,255,255};
    for(float x = -BOX_SIZE; x <= BOX_SIZE; x += 0.2) {
      for(float y = -BOX_SIZE; y <= BOX_SIZE; y += 0.2) {
        if((x > -BOX_SIZE/5-1 && x < BOX_SIZE/5+1) || (y > -BOX_SIZE/5-1 && y < BOX_SIZE/5+1)) temp = (Color){ 0, 255, 0 };
        else temp = (Color){ 255, 0, 0 };
        set_point(-x, y, BOX_SIZE, a, b, c, '@', temp, output, zbuffer, colors);
        set_point(x, y, -BOX_SIZE, a, b, c, '$', temp, output, zbuffer, colors);
        set_point(x, BOX_SIZE, y, a, b, c, '#', temp, output, zbuffer, colors);
        set_point(x, -BOX_SIZE, -y, a, b, c, '~', temp, output, zbuffer, colors);
        set_point(BOX_SIZE, y, x, a, b, c, '+', temp, output, zbuffer, colors);
        set_point(-BOX_SIZE, y, -x, a, b, c, ';', temp, output, zbuffer, colors);
      }
    }
    temp = (Color){ 0, 255, 0 };
    for(float dr = -BOX_SIZE/6; dr <= BOX_SIZE/6; dr += 0.2) {
      for(float da = -M_PI_2; da <= M_PI_2; da += 0.01) {
        float r = sin(2*da) * BOX_SIZE;
        float xz = r * cos(da) * sqrt(2) / 2;
        float y =  -r * sin(da) - BOX_SIZE;
        set_point(xz, y, dr, a, b, c, '&', temp, output, zbuffer, colors);
      }
    }
    system("cls");
    temp = (Color){ 0, 0, 0 };
    for (int i = 0; i < SIZE_Y; i++) {
      for (int j = 0; j < SIZE_X; j++) {
        if(output[i][j] != ' ' && !cmp_color(temp, colors[i][j])) set_color(colors[i][j]);
        putchar(output[i][j]);
      }
      puts("");
    }
    a += 0.05; if(a >= 2 * M_PI) a -= 2 * M_PI;
    b += 0.04; if(b >= 2 * M_PI) b -= 2 * M_PI;
    c += 0.07; if(c >= 2 * M_PI) c -= 2 * M_PI;
  }
  return 0;
}