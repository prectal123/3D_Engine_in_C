#include <iostream>
#include <opencv2/highgui.hpp>
#include <windows.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <Eigen/Dense>
#include <vector>

using namespace cv;
using namespace std;
using namespace Eigen;

string canvasName = "영상보기";
#define ROW 700
#define COL 700
#define CHUNK 1
#define frameRate 1000
#define PI 3.141592653

Mat canvas(ROW*CHUNK, COL*CHUNK, CV_8UC1, cv::Scalar(255));
uchar matrix[ROW][COL];

Vector3d camPosition(0.0 , 0.0,15.0);
Vector3d camCenter(0.0, 0.0, -1);
Vector3d delX(0.005, 0.0, 0.0);
Vector3d delY(0.0, 0.005, 0.0);

vector<vector<Vector3d>> vertices;
vector<Vector3d> normals;
vector<uchar> colors;

void randomMatrix() {
    for (int i = 0; i < ROW * COL; i++) {
		matrix[i / COL][i % COL] = rand() % 256;
    }
}

void updateCanvas() {
    for (int i = 0; i < COL * ROW * CHUNK * CHUNK; i++) {
		
        canvas.at<uchar>(i / (COL * CHUNK), i % (COL * CHUNK)) = matrix[i / (COL * CHUNK * CHUNK)][(i % (COL * CHUNK)) / CHUNK];
    }
    imshow(canvasName, canvas);
    int key = waitKey(1000 / frameRate);
    if (key == 27) waitKey(0);
}

Vector3d detPrint(int index, Vector3d raycast) {
	Matrix3d vertex;
	vertex << 
		vertices[index][0].x() - camPosition.x(), vertices[index][1].x() - camPosition.x(), vertices[index][2].x() - camPosition.x(),
		vertices[index][0].y() - camPosition.y(), vertices[index][1].y() - camPosition.y(), vertices[index][2].y() - camPosition.y(),
		vertices[index][0].z() - camPosition.z(), vertices[index][1].z() - camPosition.z(), vertices[index][2].z() - camPosition.z();
	 
	Vector3d coefficient = vertex.colPivHouseholderQr().solve(raycast);
	return coefficient;
		
}

void updateMatrix() { // Calculate the coefficients of every pixel rays -> Inefficient, Not used anymore
    for (int i = 0; i < COL; i++) {
        for (int j = 0; j < ROW; j++) {
			Vector3d raycast = camCenter + delX * (i - (COL / 2)) + delY * (j - (ROW / 2));
			matrix[j][i] = 0;
			double maxCo = -12345678;
            for (int k = 0; k < vertices.size(); k++) {
				Vector3d coefficient = detPrint(k, raycast);
				if (!(coefficient[0] < 0.0 || coefficient[1] < 0.0 || coefficient[2] < 0.0) && (coefficient[0] + coefficient[1] + coefficient[2]) > maxCo) {
					matrix[j][i] = colors[k];
					maxCo = (coefficient[0] + coefficient[1] + coefficient[2]);
				}
            }
        }
    }
}

//Throughput increasement test

vector <Vector3d> solvedDels;

void loadSolvedDels() { // Calculate the coefficients of the camera center ray and x y delta rays. Then linearly combining them generates the pixel ray coefficients
	solvedDels.clear();
	Matrix3d vertex;
	for (int index = 0; index < vertices.size(); index++) {
		vertex <<
			vertices[index][0].x() - camPosition.x(), vertices[index][1].x() - camPosition.x(), vertices[index][2].x() - camPosition.x(),
			vertices[index][0].y() - camPosition.y(), vertices[index][1].y() - camPosition.y(), vertices[index][2].y() - camPosition.y(),
			vertices[index][0].z() - camPosition.z(), vertices[index][1].z() - camPosition.z(), vertices[index][2].z() - camPosition.z();
		Vector3d coefficient = vertex.colPivHouseholderQr().solve(delX);
		solvedDels.push_back(coefficient);
		coefficient = vertex.colPivHouseholderQr().solve(delY);
		solvedDels.push_back(coefficient);
		coefficient = vertex.colPivHouseholderQr().solve(camCenter);
		solvedDels.push_back(coefficient);
	}
}

void updateMatrix2() {
	loadSolvedDels();
	for (int i = 0; i < COL; i++) {
		for (int j = 0; j < ROW; j++) {
			matrix[j][i] = 0;
			double maxCo = -12345678;
			for (int k = 0; k < vertices.size(); k++) {
				Vector3d coefficient = solvedDels[k*3 + 2] + solvedDels[k*3] * (i - (COL / 2)) + solvedDels[k*3+1] * (j - (ROW / 2)); /*detPrint(k, raycast);*/
				if (!(coefficient[0] < 0.0 || coefficient[1] < 0.0 || coefficient[2] < 0.0) && (coefficient[0] + coefficient[1] + coefficient[2]) > maxCo) {
					matrix[j][i] = colors[k];
					maxCo = (coefficient[0] + coefficient[1] + coefficient[2]);
				}
			}
		}
	}
}

void updateCam(int lap) {
	lap = lap * 30;
	double R = 10.0;
	camPosition = Vector3d(0,R*sin(PI*lap / 1000),R*cos(PI*lap / 1000));
	camCenter = Vector3d(0, -sin(PI * lap / 1000), -cos(PI * lap / 1000));
	delY = Vector3d(-1.0, 0.0, 0.0) * 0.005;
	delX = delY.cross(camCenter);
}
//////////////////////////////


void loadVertices() {
    vector<Vector3d> tempList;
    Vector3d temp = { -2.0, 3.0, 0.0 };
    tempList.push_back(temp);
    temp = { 4.0, 0.0,0.0 };
    tempList.push_back(temp);
    temp = { 0.0, -8.0, 0.0 };
    tempList.push_back(temp);
    vertices.push_back(tempList);
    Vector3d tempNorm = { 0.0, 0.0, 1.0 };
    normals.push_back(tempNorm);
	colors.push_back(255);

	tempList.clear();
    temp = { 2.0, 0.0, 10.0 };
	tempList.push_back(temp);
	temp = { -1.0, 1.0, -5.0 };
	tempList.push_back(temp);
	temp = { -1.0, -1.0, -5.0 };
	tempList.push_back(temp);
	vertices.push_back(tempList);
	tempNorm = { -26.0, 0.0, 6.0 };
	normals.push_back(tempNorm);
	colors.push_back(150);

	tempList.clear();
	temp = { 2.0, 5.0, -5.0 };
	tempList.push_back(temp);
	temp = { -5.0, -5.0, -2.0 };
	tempList.push_back(temp);
	temp = { 1.0, -1.0, -7.0 };
	tempList.push_back(temp);
	vertices.push_back(tempList);
	tempNorm = { 132.0, -138.0, 20.0 };
	normals.push_back(tempNorm);
	colors.push_back(200);

	tempList.clear();
	temp = { 3.0, 5.0, -7.0 };
	tempList.push_back(temp);
	temp = { -3.0, 0.0, -2.0 };
	tempList.push_back(temp);
	temp = { 1.0, -6.0, -7.0 };
	tempList.push_back(temp);
	vertices.push_back(tempList);
	tempNorm = { 132.0, -138.0, 20.0 };
	normals.push_back(tempNorm);
	colors.push_back(230);

	tempList.clear();
	temp = { 6.0,1.0, -1.0 };
	tempList.push_back(temp);
	temp = { -2.0, -6.0, 0.0 };
	tempList.push_back(temp);
	temp = { -2.0, -0.0, -4.0 };
	tempList.push_back(temp);
	vertices.push_back(tempList);
	tempNorm = { 132.0, -138.0, 20.0 };
	normals.push_back(tempNorm);
	colors.push_back(50);

	tempList.clear();
	temp = { 5.0, 2.0, -2.0 };
	tempList.push_back(temp);
	temp = { -1.0, -1.0, -1.0 };
	tempList.push_back(temp);
	temp = { 1.0, -3.0, -3.0 };
	tempList.push_back(temp);
	vertices.push_back(tempList);
	tempNorm = { 132.0, -138.0, 20.0 };
	normals.push_back(tempNorm);
	colors.push_back(70);

	tempList.clear();
	temp = { 7.0, 1.0, -1.0 };
	tempList.push_back(temp);
	temp = { -5.0, -3.0, -3.0 };
	tempList.push_back(temp);
	temp = { 1.0, -2.0, -7.0 };
	tempList.push_back(temp);
	vertices.push_back(tempList);
	tempNorm = { 132.0, -138.0, 20.0 };
	normals.push_back(tempNorm);
	colors.push_back(100);

	tempList.clear();
	temp = { 1.0, 1.0, 2.0 };
	tempList.push_back(temp);
	temp = { 3.0, 5.0, 2.0 };
	tempList.push_back(temp);
	temp = { 1.0, 10.0, -2.0 };
	tempList.push_back(temp);
	vertices.push_back(tempList);
	tempNorm = { 132.0, -138.0, 20.0 };
	normals.push_back(tempNorm);
	colors.push_back(120);

	tempList.clear();
	temp = { 4.0, 2.0, -2.0 };
	tempList.push_back(temp);
	temp = { -1.0, -5.0, -6.0 };
	tempList.push_back(temp);
	temp = { 1.0, -1.0, -12.0 };
	tempList.push_back(temp);
	vertices.push_back(tempList);
	tempNorm = { 132.0, -138.0, 20.0 };
	normals.push_back(tempNorm);
	colors.push_back(180);

	tempList.clear();
	temp = { 1.0, 1.0, 1.0 };
	tempList.push_back(temp);
	temp = { 5.0, 5.0, 2.0 };
	tempList.push_back(temp);
	temp = { 1.0, -1.0, -7.0 };
	tempList.push_back(temp);
	vertices.push_back(tempList);
	tempNorm = { 132.0, -138.0, 20.0 };
	normals.push_back(tempNorm);
	colors.push_back(200);
};

int main()
{
	clock_t start, end;
	start = clock();
    loadVertices();
	int laps = 1000;

    while (laps--) {
		camPosition += Vector3d(0.0, 0.0, -0.1);
		updateCam(laps);
		updateMatrix2();
		updateCanvas();
    }
	end = clock();
	cout << "Elapsed Time : " << (double)(end - start) << "ms" << endl;
}