#ifndef HOUGHEDGEDETECT_H
#define HOUGHEDGEDETECT_H

#include "CImg.h"
#include <iostream>
#include <vector>
#include <set>
using namespace std;
using namespace cimg_library;

#define Pi 3.14
#define ThetaScale 10
#define RhoScale 1

#define TopK 13          //���߽�������TopK���ȡ��Ѳ�����
#define VertexGap 36     //ֱ�߽��㣬������VertexGap�ڣ�����ͬһ����
#define DownSampledSquareSize 500.0

typedef multiset<int, less<int>> IntHeap;

struct Vertex {
	int x;
	int y;
	int crossTimes = 0;
	Vertex(int posX, int posY): x(posX), y(posY) {}
	void setXY(int _x, int _y) { 
		x = _x;
		y = _y;
	}
	void addCrossTimes() {
		crossTimes++;
	}
};

class HoughEdgeDetect {
public:
	HoughEdgeDetect(const CImg<int>& CannyImg);
	~HoughEdgeDetect();

	void HoughTransAndDetectEdge();               //��Ե��⴦����Ҫ����
	
	CImg<int> getHoughSpaceImage();               //��ȡHoughSpace��ͼ��
	CImg<int> getCannyGrayImageWithEdge();        //��ȡ������Ե�ĻҶ�ͼ

	CImg<int> getFinallyProcessedImage(const CImg<int>& SrcImg);  //��ȡ�������ɵĲ�ɫԭ��Сͼ��

	vector<Vertex> getTopFourVertexSet();
	double getDownSampledSize();

private:
	CImg<int> SrcCannyImg;
	int imgW, imgH;
	int **houghArray;

	IntHeap myHeap;
	double k_TopList[TopK]{ 0 };
	double b_TopList[TopK]{ 0 };
	vector<Vertex> vertexSet;
	vector<Vertex> top4vertexSet;

	double DownSampledSize = 1;

	void accumulateTheHoughArray(int x, int y);   //��HoughSpace�������ۼ�
	void findTheTopKParaPair();                   //���߽�������K��rho/theta������
	void modifyTheParaArray(int i, int j);
	void findTheNearest4Points();                 //�ҵ���ӽ���4������
	void getValidCrossPointAndIncrement(double k1, double b1, double k2, double b2);
	void drawLinesBetweenVertex(CImg<int>& img);
};


#endif