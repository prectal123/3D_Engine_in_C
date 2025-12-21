#include <iostream>
#include <opencv2/highgui.hpp>
#include <windows.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <Eigen/Dense>
#include <vector>
#include <fstream>

using namespace cv;
using namespace std;
using namespace Eigen;

string canvasName = "Window";
#define ROW 700
#define COL 700
#define CHUNK 1
#define frameRate 1000
#define PI 3.141592653

#define PARTITION_ROW 10
#define PARTITION_COL 10

double camSpeed = 0.5;
double camRotationSpeed = 0.005;

double moveForward = 0.0;
double moveRight = 0.0;
double moveUp = 0.0;

double SpinRight = 0.0;
double SpinUp = 0.0;

int partitionRowSize = ROW / PARTITION_ROW;
int partitionColSize = COL / PARTITION_COL;

double scale = 10;

Point mousePos = Point(500, 500); // Fixed mouse position -> Global Position.
/*
 There is a UI-Based bias between the position inside the canvas, and the global position of the screen.
 Even though we set the cursor position as (700,700) and set the canvas position as (200,200), the mouse event become
 triggered at position (492,469), not (500,500). So this bias needs to be adjusted.
*/
Point canvasPos = Point(200, 200);
Point canvasBias = Point(8,31); // (492,469) + (8,31) => (500,500)


Mat canvas(ROW*CHUNK, COL*CHUNK, CV_8UC1, cv::Scalar(255));
Mat canvas2(ROW*CHUNK, COL*CHUNK, CV_8UC1, cv::Scalar(255));

uchar matrix[ROW][COL];
vector<int> partitions[PARTITION_COL][PARTITION_ROW];

Vector3d camPosition(0.0 , 0.0,15.0);

Vector3d camCenter(0.0, 0.0, -1); // Direct Forward
double delXSize = 0.005;
double delYSize = 0.005;
Vector3d delX(1.0, 0.0, 0.0); // Direct Right
Vector3d delY(0.0, 1.0, 0.0); // Direct Up

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
void updateCanvas2() {
    for (int i = 0; i < COL * ROW * CHUNK * CHUNK; i++) {
		
        canvas.at<uchar>(i / (COL * CHUNK), i % (COL * CHUNK)) = matrix[i / (COL * CHUNK * CHUNK)][(i % (COL * CHUNK)) / CHUNK];
    }
    imshow("TEST", canvas);
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

void DrawPartitions(){
	for(int i=0;i<COL;i++)
		for(int j=0;j<ROW;j++)
			if(i%partitionColSize == 0 || j%partitionRowSize == 0) matrix[j][i] = 255;
}

void unskewBasisVectors(){
	delY.normalize();
	camCenter.normalize();
	Vector3d newDelY = (delY - camCenter * camCenter.dot(delY)).normalized();
	Vector3d newDelX = camCenter.cross(newDelY); 
	newDelY.normalize();
	newDelX.normalize();
	delX = newDelX;
	delY = newDelY;
}

void fillPartitions(){

	for(int i=0;i<PARTITION_COL;i++)
		for(int j=0;j<PARTITION_ROW;j++)
			partitions[i][j].clear();

	for(int i=0;i<vertices.size();i++){
		int minX = 123456789, maxX = -123456789, minY = 123456789, maxY = -123456789;
		for (int j=0;j<vertices[i].size();j++ ){
			int delXCount = 0, delYCount = 0;
			delXCount = (int) ( (((vertices[i][j] - camPosition).dot(delX)) / delXSize)) / (((vertices[i][j] - camPosition).dot(camCenter)))  + ( COL / 2 ) ; // delSize : 픽셀 한칸의 크기
			delYCount = (int) ((((vertices[i][j] - camPosition).dot(delY)) / delYSize) ) / (((vertices[i][j] - camPosition).dot(camCenter))) + ( ROW / 2 ) ;
			
			if(minX > delXCount) minX = delXCount;
			if(maxX < delXCount) maxX = delXCount;
			if(minY > delYCount) minY = delYCount;
			if(maxY < delYCount) maxY = delYCount;
		}
		// minX++;
		// minY++;
		int minXPart = minX / partitionColSize, maxXpart = maxX / partitionColSize; // Divide by chunk size
		int minYPart = minY / partitionRowSize, maxYPart = maxY / partitionRowSize;
		// cout << "pixels : " << minX << " " << maxX << " " << minY << " " << maxY << " " << endl;
		// cout << "minMAX xy : "<< minXPart << " " << maxXpart << " " << minYPart << " " << maxYPart << endl << endl;
		for(int j = (minXPart>0?minXPart:0) ; j < (maxXpart>=PARTITION_COL?PARTITION_COL:maxXpart+1) ; j++){
			for(int k = (minYPart>0?minYPart:0) ; k < (maxYPart>=PARTITION_ROW?PARTITION_ROW:maxYPart+1) ; k++){
				partitions[j][k].push_back(i);
			}
		}
	}
}

void updateMatrix() { // Calculate the coefficients of every pixel rays -> Inefficient, Not used anymore
    for (int i = 0; i < COL; i++) {
        for (int j = 0; j < ROW; j++) {
			Vector3d raycast = camCenter + delX * (i - (COL / 2)) * delXSize + delY * (j - (ROW / 2)) * delYSize;
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
		// cout << normals[index].dot(camCenter) << ": LOL" << endl;
		vertex <<
			vertices[index][0].x() - camPosition.x(), vertices[index][1].x() - camPosition.x(), vertices[index][2].x() - camPosition.x(),
			vertices[index][0].y() - camPosition.y(), vertices[index][1].y() - camPosition.y(), vertices[index][2].y() - camPosition.y(),
			vertices[index][0].z() - camPosition.z(), vertices[index][1].z() - camPosition.z(), vertices[index][2].z() - camPosition.z();
		Vector3d coefficient = vertex.colPivHouseholderQr().solve(delX * delXSize);
		solvedDels.push_back(coefficient);
		coefficient = vertex.colPivHouseholderQr().solve(delY * delYSize);
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
				if(normals[k].dot(camCenter + delX * (i - (COL / 2)) * delXSize + delY * (j - (ROW / 2)) * delYSize) <= 0.0 ) continue;
				Vector3d coefficient = solvedDels[k*3 + 2] + solvedDels[k*3] * (i - (COL / 2)) + solvedDels[k*3+1] * (j - (ROW / 2)); /*detPrint(k, raycast);*/
				if (!(coefficient[0] < 0.0 || coefficient[1] < 0.0 || coefficient[2] < 0.0) && (coefficient[0] + coefficient[1] + coefficient[2]) > maxCo) {
					matrix[j][i] = colors[k];
					maxCo = (coefficient[0] + coefficient[1] + coefficient[2]);
				}
			}
		}
	}
}

void updateMatrix3() {
	loadSolvedDels();
	fillPartitions();
	for (int i = 0; i < COL; i++) {
		for (int j = 0; j < ROW; j++) {
			matrix[j][i] = 0;
			double maxCo = -12345678;
			for (int u = 0; u < partitions[i/partitionColSize][j/partitionRowSize].size(); u++) {
				int k = partitions[i/partitionColSize][j/partitionRowSize][u];
				if(normals[k].dot(camCenter + delX * (i - (COL / 2)) * delXSize + delY * (j - (ROW / 2)) * delYSize) <= 0.0 ) continue;
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
		// cout << "x : " << x << " y : " << y << endl;
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
	unskewBasisVectors();
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

void readModel(){

	vector<Vector3d> dots;
	ifstream file("C:/Users/user/PrivateProject/CMake_Projects/models/tree.obj");
	string line;

	while (getline(file, line)) {
		istringstream iss(line);
		string type;
		iss >> type;

		if (type == "v") {
			double x, y, z;
			iss >> x >> y >> z;
			dots.push_back(Eigen::Vector3d(scale * x, -1 * scale * y, scale * z));
		}
		else if (type == "f") {
			string v1, v2, v3;
			iss >> v1 >> v2 >> v3;

			auto parseIndex = [](const string& token) {
				size_t pos = token.find('/');
				if (pos != string::npos)
					return stoi(token.substr(0, pos));
				return stoi(token);
			};
			// cout << "dots: " << dots.size() << endl;

			// cout << "indexs : " << (parseIndex(v1) - 1) << " " << (parseIndex(v2) - 1) << " " << (parseIndex(v3) - 1) << endl;
			if((parseIndex(v1) - 1) >= dots.size() || (parseIndex(v2) - 1) >= dots.size() || (parseIndex(v3) - 1) >= dots.size()) cout <<"oops" << endl;
			vector <Vector3d> face = { dots[parseIndex(v1) - 1], dots[parseIndex(v2) - 1], dots[parseIndex(v3) - 1] };
			vertices.push_back(face);
			normals.push_back((face[1] - face[0]).cross(face[2] - face[0]));
			colors.push_back(normals[normals.size()-1].normalized()[1]*127 + 128);
		}
	}
	// cout << "Model Read done. Vertices : " << vertices.size() << " normals : " << normals.size() << " colors: " << colors.size() << endl;
	// cout << "read Donce" <<endl;
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
    Vector3d tempNorm = { 0.0, 0.0, -1.0 };
    normals.push_back(tempNorm);
	colors.push_back(255);
};

int main()
{
	clock_t start, end;
	start = clock();
    // loadVertices();
	readModel();
	int laps = 1000;
	namedWindow(canvasName, WINDOW_NORMAL);
	resizeWindow(canvasName, COL*CHUNK, ROW*CHUNK);
	moveWindow(canvasName, canvasPos.x, canvasPos.y);

	// namedWindow("TEST", WINDOW_NORMAL);
	// resizeWindow("TEST", COL*CHUNK, ROW*CHUNK);
	// moveWindow("TEST", canvasPos.x, canvasPos.y);
	
	SetCursorPos(canvasPos.x + mousePos.x, canvasPos.y + mousePos.y); // Hehe I don't know where to put it
	setMouseCallback(canvasName, mouseCallback);
	// setMouseCallback("TEST", mouseCallback);

    while (laps--) {
		// camPosition += Vector3d(0.0, 0.0, -0.1);
		// updateCam(laps);
		updateMatrix3();
		//DrawPartitions();
		updateCanvas();

		// updateMatrix2();
		// DrawPartitions();
		// updateCanvas2();
		
		
		int key = waitKey(1);
    	if(keyBoardCallback(key)) break;
    }
	end = clock();
	cout << "Elapsed Time : " << (double)(end - start) << "ms" << endl;
}