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

string canvasName = "Window";
#define ROW 700
#define COL 700
#define CHUNK 1
#define frameRate 1000
#define PI 3.141592653

double camSpeed = 0.5;
double camRotationSpeed = 0.01;

double moveForward = 0.0;
double moveRight = 0.0;
double moveUp = 0.0;

double SpinRight = 0.0;
double SpinUp = 0.0;

Point mousePos = Point(500, 500); // Fixed mouse position -> Global Position.
/*
 There is a UI-Based bias between the position inside the canvas, and the global position of the screen.
 Even though we set the cursor position as (700,700) and set the canvas position as (200,200), the mouse event become
 triggered at position (492,469), not (500,500). So this bias needs to be adjusted.
*/
Point canvasPos = Point(200, 200);
Point canvasBias = Point(8,31); // (492,469) + (8,31) => (500,500)

Mat canvas(ROW*CHUNK, COL*CHUNK, CV_8UC1, cv::Scalar(255));
uchar matrix[ROW][COL];

Vector3d camPosition(0.0 , 0.0,15.0);

Vector3d camCenter(0.0, 0.0, -1); // Direct Forward
Vector3d delX(0.005, 0.0, 0.0); // Direct Right
Vector3d delY(0.0, 0.005, 0.0); // Direct Up

vector<vector<Vector3d>> vertices;
vector<Vector3d> normals;
vector<uchar> colors;

void randomMatrix() {
    for (int i = 0; i < ROW * COL; i++) {
		matrix[i / COL][i % COL] = rand() % 256;
    }
}

Matrix3d camHorizontalRotation(double radian){ // 3x3 Matrix to calculate horizontal rotation
	double c = cos(radian);
	double s = sin(radian);
	Vector3d del = delY.normalized();
	double x = del[0];
	double y = del[1];
	double z = del[2];
	Matrix3d rot;
	rot <<
		c+x*x*(1-c), x*y*(1-c)-z*s, x*z*(1-c)+y*s,
		y*x*(1-c)+z*s, c+y*y*(1-c), y*z*(1-c)-x*s,
		z*x*(1-c)-y*s, z*y*(1-c)+x*s, c+z*z*(1-c);
	// camCenter = rot*camCenter;
	// delX = rot*delX;
	return rot;
}

Matrix3d camVerticalRotation(double radian){ // 3x3 Matrix to calculate vertical rotation
	double c = cos(radian);
	double s = sin(radian);
	Vector3d del = delX.normalized();
	double x = del[0];
	double y = del[1];
	double z = del[2];
	Matrix3d rot;
	rot <<
		c+x*x*(1-c), x*y*(1-c)-z*s, x*z*(1-c)+y*s,
		y*x*(1-c)+z*s, c+y*y*(1-c), y*z*(1-c)-x*s,
		z*x*(1-c)-y*s, z*y*(1-c)+x*s, c+z*z*(1-c);
	// camCenter = rot*camCenter;
	// delY = rot*delY;
	return rot;
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

void mouseCallback(int event, int x, int y, int flags, void*){
	if(event == EVENT_MOUSEMOVE){
		x = x + canvasBias.x;
		y = y + canvasBias.y;
		cout << "x : " << x << " y : " << y << endl;
		int dx = x - mousePos.x;
		int dy = y - mousePos.y;
		if(dx==0 && dy==0) return;
		Matrix3d h = camHorizontalRotation(dx * camRotationSpeed * -1);
		Matrix3d v = camVerticalRotation(dy * camRotationSpeed);

		camCenter = v * camCenter;
		camCenter = h * camCenter;
		delX = h * delX;
		delY = v * delY;
		// mousePos.x = x;
		// mousePos.y = y;
	}
	SetCursorPos(canvasPos.x + mousePos.x, canvasPos.y + mousePos.y);
}

bool keyBoardCallback(int key){
	if(GetAsyncKeyState(VK_ESCAPE) & 0x8000) return true; // ESC Key to escape :(
	if(GetAsyncKeyState('W') & 0x8000) camPosition += camCenter.normalized() * camSpeed;
	if(GetAsyncKeyState('S') & 0x8000) camPosition -= camCenter.normalized() * camSpeed;
	if(GetAsyncKeyState('A') & 0x8000) camPosition -= delX.normalized() * camSpeed;
	if(GetAsyncKeyState('D') & 0x8000) camPosition += delX.normalized() * camSpeed;
	if(GetAsyncKeyState(VK_SHIFT) & 0x8000) camPosition -= delY.normalized() * camSpeed;
	if(GetAsyncKeyState(VK_CONTROL) & 0x8000) camPosition += delY.normalized() * camSpeed;
	return false;
}

void updateDynamicCam(){

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
	namedWindow(canvasName, WINDOW_NORMAL);
	resizeWindow(canvasName, COL*CHUNK, ROW*CHUNK);
	moveWindow(canvasName, canvasPos.x, canvasPos.y);
	SetCursorPos(canvasPos.x + mousePos.x, canvasPos.y + mousePos.y); // Hehe I don't know where to put it
	setMouseCallback(canvasName, mouseCallback);

    while (laps--) {
		// camPosition += Vector3d(0.0, 0.0, -0.1);
		// updateCam(laps);
		updateMatrix2();
		updateCanvas();
		
		
		int key = waitKey(1);
    	if(keyBoardCallback(key)) break;
    }
	end = clock();
	cout << "Elapsed Time : " << (double)(end - start) << "ms" << endl;
}