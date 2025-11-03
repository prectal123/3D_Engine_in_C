#include <iostream>
#include <opencv2/highgui.hpp>
#include <windows.h>
#include <math.h>
#include <string.h>
#include <time.h>


using namespace cv;
using namespace std;

string canvasName = "영상보기";
#define ROW 100
#define COL 100
#define CHUNK 7
#define frameRate 50

Mat canvas(ROW*CHUNK, COL*CHUNK, CV_8UC1, cv::Scalar(255));
uchar matrix[ROW][COL];

void randomMatrix() {
    for (int i = 0; i < ROW * COL; i++) {
		matrix[i / COL][i % COL] = rand() % 256;
    }
}

void updateCanvas() {
    for (int i = 0; i < COL * ROW * CHUNK * CHUNK; i++) {
        canvas.at<uchar>(i / (COL * CHUNK), i % (COL * CHUNK)) = matrix[i / (COL * CHUNK * CHUNK)][(i % (COL * CHUNK)) / CHUNK];
    }
    imshow("영상보기", canvas);
    int key = waitKey(frameRate);
    if (key == 27) {
        waitKey(0);
        exit(0);
    }
}

void updateMatrix() {}

int main()
{
    randomMatrix();
    while (1) {
        updateMatrix();
		updateCanvas(); 
    }
}