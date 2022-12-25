#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

// Consts
#define SIZE_X 80
#define SIZE_Y 20

const char* CHARS = ".,-~:;=!*#$@";
const float BOX_SIZE = 5;

const float PROJ_MAT[][3] = {
    { 1, 0, 0 },
    { 0, 1, 0 }
};

// Functions
void get_projection(float res[2], float point[3]) {
    for(int i=0;i<2;i++) {
        res[i] = 0;
        for(int j=0;j<3;j++) res[i] += PROJ_MAT[i][j] * point[j];
    }
}

void rotate_mul(float point[3], float r[][3]) {
    float new_point[3];
    for(int i=0;i<3;i++) {
        new_point[i] = 0;
        for(int j=0;j<3;j++) new_point[i] += r[i][j] * point[j];
    }
    for(int i=0;i<3;i++) point[i] = new_point[i];
}

void rotate_x(float point[3], float angle) {
    float r_x[][3] = {
        { 1, 0, 0 },
        { 0, cos(angle), -sin(angle) },
        { 0, sin(angle), cos(angle) }
    };
    rotate_mul(point, r_x);
}

void rotate_y(float point[3], float angle) {
    float r_y[][3] = {
        { cos(angle), 0, sin(angle) },
        { 0, 1, 0 },
        { -sin(angle), 0, cos(angle) }
    };
    rotate_mul(point, r_y);
}

void rotate_z(float point[3], float angle) {
    float r_z[][3] = {
        { cos(angle), -sin(angle), 0 },
        { sin(angle), cos(angle), 0 },
        { 0, 0, 1 }
    };
    rotate_mul(point, r_z);
}

void render_connection(float p1[2], float p2[3], char output[SIZE_Y][SIZE_X]) {
    float x1 = round(p1[0]), y1 = round(p1[1]);
    float pp2[2]; get_projection(pp2, p2);
    float x2 = round(pp2[0]), y2 = round(pp2[1]);
    for(int i=fmin(x1, x2)+1;i<fmax(x1, x2);i++) {
        int y = round((y2 - y1) / (pp2[0] - p1[0]) * (i - p1[0]) + y1);
        output[y + SIZE_Y / 2][(int)round(i * 2.5) + SIZE_X / 2] = '+';
    }
    for(int i=fmin(y1, y2); i<fmax(y1, y2); i++) {
        float x = (x2 - x1) / (y2 - y1) * (i - y1) + x1;
        output[i + SIZE_Y / 2][(int)round(x * 2.5) + SIZE_X / 2] = '+';
    }
}

int main() {
    char output[SIZE_Y][SIZE_X];
    for(int i=0;i<SIZE_Y;i++) for(int j=0;j<SIZE_X;j++) output[i][j] = ' ';
    float ps[][3] = {
        { BOX_SIZE, BOX_SIZE, -BOX_SIZE },
        { -BOX_SIZE, BOX_SIZE, -BOX_SIZE },
        { -BOX_SIZE, -BOX_SIZE, -BOX_SIZE },
        { BOX_SIZE, -BOX_SIZE, -BOX_SIZE },
        { BOX_SIZE, BOX_SIZE, BOX_SIZE },
        { -BOX_SIZE, BOX_SIZE, BOX_SIZE },
        { -BOX_SIZE, -BOX_SIZE, BOX_SIZE },
        { BOX_SIZE, -BOX_SIZE, BOX_SIZE }
    };
    printf("\e[?25l");
    float angle = 0.05;
    while(1) {
        for(int i=0;i<SIZE_Y;i++) for(int j=0;j<SIZE_X;j++) output[i][j] = ' ';
        float pp[2];
        for(int i=0;i<8;i++) {
            rotate_z(ps[i], angle);
            // rotate_x(ps[i], angle);
            rotate_y(ps[i], angle);
            get_projection(pp, ps[i]);
            output[(int)round(pp[1]) + SIZE_Y / 2][(int)round(pp[0] * 2.5) + SIZE_X / 2] = 'O';
        }
        render_connection(ps[0], ps[1], output);
        render_connection(ps[1], ps[2], output);
        render_connection(ps[2], ps[3], output);
        render_connection(ps[3], ps[0], output);
        render_connection(ps[4], ps[5], output);
        render_connection(ps[5], ps[6], output);
        render_connection(ps[6], ps[7], output);
        render_connection(ps[7], ps[4], output);
        render_connection(ps[0], ps[4], output);
        render_connection(ps[1], ps[5], output);
        render_connection(ps[2], ps[6], output);
        render_connection(ps[3], ps[7], output);
        system("cls");
        for(int i=0;i<SIZE_Y;i++) {
            for(int j=0;j<SIZE_X;j++) putchar(output[i][j]);
            puts("");
        }
        // sleep(1);
    }
    return 0;
}