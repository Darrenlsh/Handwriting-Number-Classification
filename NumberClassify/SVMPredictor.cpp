#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#include "SVMPredictor.h"
#include <opencv2/opencv.hpp>
using namespace cv;

const bool doScaling = true;

SVMPredictor::SVMPredictor() {
}

SVMPredictor::~SVMPredictor() {
}

SVMPredictor::SVMPredictor(string _basePath) {
	basePath = _basePath + "/";
}

void SVMPredictor::readPredictImagelist(string predictImagelistFile) {
	ifstream readData(basePath + predictImagelistFile);  //���ز���ͼƬ����
	string buffer;
	list<string> subimglist;
	while (readData) {
		if (getline(readData, buffer)) {
			if (buffer[0] == '*') {    //��ͼ֮��ָ���
				predictImgListSet.push_back(subimglist);
				subimglist.clear();
			}
			else {
				subimglist.push_back(buffer);
			}   
		}
	}
	readData.close();
	cout << "Read Predict Data Complete" << endl;
}

void SVMPredictor::predict_mainProcess(string LibSVMModelFile) {
	for (int i = 0; i < predictImgListSet.size(); i++) {
		predict_foreach_subImage(LibSVMModelFile, predictImgListSet[i]);
	}

	cout << predictResult << endl;
	ofstream predictResultOutput(basePath + "predict_result.txt");
	predictResultOutput << predictResultInDetail.c_str();
	predictResultOutput << predictResult.c_str();
	predictResultOutput.close();
}

void SVMPredictor::predict_foreach_subImage(string LibSVMModelFile, list<string> predictImgList) {
	svm_model * svm = svm_load_model(LibSVMModelFile.c_str());

	IplImage* testImage;
	IplImage* tempImage;

	vector<double> maxDescriptors, minDescriptors;    //������ÿһά�������Сֵ
	vector<list<float>> descriptorListSet;            //����������

	for (int i = 0; i < 324; i++) {         //������ά��324
		maxDescriptors.push_back(-1.0);
		minDescriptors.push_back(10000.0);
	}

	//��һ��ѭ��
	//���α������еĴ����ͼƬ���Ȱ����ݴ洢���������ҵ�ÿһά�������Сֵ
	for (list<string>::iterator iter = predictImgList.begin(); iter != predictImgList.end(); iter++) {
		testImage = cvLoadImage((basePath + *iter).c_str(), 1);

		if (testImage == NULL) {
			cout << " can not load the image: " << (basePath + *iter).c_str() << endl;
			continue;
		}
		tempImage = cvCreateImage(cvSize(20, 20), 8, 3);
		cvZero(tempImage);
		cvResize(testImage, tempImage);

		HOGDescriptor *hog = new HOGDescriptor(cvSize(20, 20), cvSize(10, 10), cvSize(5, 5), cvSize(5, 5), 9);
		cv::Mat tempImageMat = cv::cvarrToMat(tempImage, true);
		vector<float> tmpDescriptor;
		hog->compute(tempImageMat, tmpDescriptor, Size(1, 1), Size(0, 0));

		list<float> descriptorList;

		//�Ȱ����ݴ洢���������ҵ�ÿһά�������Сֵ
		for (int j = 0; j < tmpDescriptor.size(); j++) {
			double value = tmpDescriptor[j];
			descriptorList.push_back(value);
			maxDescriptors[j] = value > maxDescriptors[j] ? value : maxDescriptors[j];
			minDescriptors[j] = value < minDescriptors[j] ? value : minDescriptors[j];
		}
		descriptorListSet.push_back(descriptorList);
	}

	//�ڶ���ѭ��
	//��ͼ�����ݹ����svm�ܹ�����Ľṹ
	int i = 0;
	for (list<string>::iterator iter = predictImgList.begin(); iter != predictImgList.end(); iter++) {
		list<int> indexList;
		list<double> valueList;

		int n = 0;
		for (list<float>::iterator iter = descriptorListSet[i].begin(); iter != descriptorListSet[i].end(); iter++) {
			
			if (!doScaling) {  //����scale��ȫ������
				indexList.push_back(n + 1);
				valueList.push_back(*iter);
			}
			else {             //��scale��ֵΪ0��ȫ����ͬ�ģ���ĸΪ0��������
				double oriValue = *iter;
				double maxMinGap = (double)maxDescriptors[n] - (double)minDescriptors[n];
				if (maxMinGap != 0) {
					double newValue = (oriValue - minDescriptors[n]) / maxMinGap * (NormalizeUp - NormalizeLow) + NormalizeLow;
					if (newValue != 0) {
						indexList.push_back(n + 1);
						valueList.push_back(newValue);
					}
				}
			}

			n++;
		}
		indexList.push_back(-1);        //ע������-1��������־
		valueList.push_back(-1.0);

		//��ÿһ��ͼƬ���������svm�ṹsvm_node
		svm_node * inputVector = new svm_node[indexList.size()];
		list<int>::iterator iIter = indexList.begin();
		list<double>::iterator dIter = valueList.begin();
		int j = 0;
		for (; iIter != indexList.end(); iIter++, dIter++) {
			inputVector[j].index = *iIter;
			if (*iIter != -1) {
				inputVector[j].value = *dIter;
			}
			else
				break;

			j++;
		}

		int resultLabel = svm_predict(svm, inputVector);    //������

		char tmpStr[200];
		sprintf(tmpStr, "%d", resultLabel);
		predictResult += string(tmpStr);

		sprintf(tmpStr, "%s  %d\n", (*iter).c_str(), resultLabel);
		predictResultInDetail += string(tmpStr);

		delete[] inputVector;

		i++;
	}
	predictResult += "\n";
	predictResultInDetail += "*\n";

	svm_free_model_content(svm);
	cvReleaseImage(&testImage);
	cvReleaseImage(&tempImage);
}
