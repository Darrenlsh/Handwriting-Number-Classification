#ifndef PAPERMODIFICATION_H
#define PAPERMODIFICATION_H

#include "CImg.h"
#include "HoughEdgeDetect.h"
#include <iostream>
#include <vector>
#include <set>
using namespace std;
using namespace cimg_library;

#define A4width 210    //A4ֽ��
#define A4height 297   //A4ֽ��
#define Scalar 2       //A4ֽ����ͼ�����A4ֽ��/�ߵı���

enum PaperType { PaperHori, PaperVert, PaperDiag };

class PaperModification {
public:
	PaperModification(const CImg<int>& _SrcImg);
	~PaperModification();

	void setVertexSet(const vector<Vertex>& _vertexSet, const double DownSampledSize);    //���ö��㼯
	CImg<int> getModifiedPaper();                           //��ȡA4ֽ����ͼ������

private:
	CImg<int> SrcImg;
	int imgW, imgH;
	int paperType = -1;          //a4ֽ��ʽ��0Ϊ���š�1Ϊ���š�2Ϊб��
	vector<Vertex> vertexSet;    //���㼯

	void resetVertexSet();                                       //ʶ��4�����λ�ò���˳��ź�
	int getPointNearestToOrigin();                               //��ȡ��������ĵ�A
	int getPointAtCrossline(int point);                          //��ȡ��A�ĶԽǵ�C
	void computeThePaperType(int pointA, int pointC);            //����ֽ����/��/б
	int findPointB(int pointA, int pointC);                      //���B
	vector<int> getInterpolationRGB(double scrX, double scrY);   //˫���Բ�ֵ��ȡRGBֵ
};


#endif