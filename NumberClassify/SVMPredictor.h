#ifndef SVMPREDICTOR_H
#define SVMPREDICTOR_H

#include "fstream"
#include "svm.h"
#include <iostream>
#include <string>
#include <vector>
#include <list>
using namespace std;

#define NormalizeLow 0.0    //��ά��������һ������Сֵ
#define NormalizeUp 1.0     //��ά��������һ�������ֵ

class SVMPredictor
{
public:
	SVMPredictor();
	~SVMPredictor();

	SVMPredictor(string _basePath);

	void readPredictImagelist(string predictImagelistFile);    //��ȡͼƬ����txt�б�

	void predict_mainProcess(string LibSVMModelFile);          //��Ԥ�������

private:
	//��ÿ����ͼ��Ԥ��
	void predict_foreach_subImage(string LibSVMModelFile, list<string> predictImgList);

private:
	string basePath = "";                      //��������ͼ����ͼ�������б��ı��Ĵ洢·��

	string predictResult = "\n";               //Ԥ�����ִ�
	string predictResultInDetail = "";         //Ԥ����ϸ�����ÿ�����ֶ�Ӧ��Ԥ��

	vector<list<string>> predictImgListSet;    //Ԥ��ͼ�������б��ϣ�ÿ����ͼ��Ӧһ��list��
};

#endif
