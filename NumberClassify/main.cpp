#include <iostream>
#include <direct.h>
#include "canny.h"
#include "SimpleImgProcess.h"
#include "HoughEdgeDetect.h"
#include "PaperModification.h"
#include "ImageSegmentation.h"
#include "SVMPredictor.h"
using namespace std;

//������Ҫ���ļ���
void createAllNeededFolders() {
	if (_access("Output", 0) == -1)
		_mkdir("Output");
	if (_access("Output/Modification", 0) == -1)
		_mkdir("Output/Modification");
	if (_access("Output/BinaryImage", 0) == -1)
		_mkdir("Output/BinaryImage");
	if (_access("Output/HistoDividedImage", 0) == -1)
		_mkdir("Output/HistoDividedImage");
	if (_access("Output/ColoredNumberDividedImg", 0) == -1)
		_mkdir("Output/ColoredNumberDividedImg");
	if (_access("Output/NumberDividedCircledImg", 0) == -1)
		_mkdir("Output/NumberDividedCircledImg");
	if (_access("Output/SingleNumberImage", 0) == -1)
		_mkdir("Output/SingleNumberImage");
	
}

int main() {
	createAllNeededFolders();

	time_t start, midterm, end;
	start = time(NULL);

	CImg<int> SrcImg, GrayImg, DownSampledImg, CannyImg, HoughSpaceImg, 
		Edge_Point_Img, SrcImgWithVertexAndEdge, PaperModifiedImg;
	
	SrcImg.load_bmp("Input/7.bmp");

	//------------------------- ��ȡ���㲿�� -------------------------//

	GrayImg = grayTheImg(SrcImg);
	GrayImg.display("GrayImg");
	DownSampledImg = downSampleTheImg(GrayImg, DownSampledSquareSize);
	//DownSampledImg.display("DownSampledImg");
	CannyImg = canny(DownSampledImg, DownSampledImg._width, DownSampledImg._height);
	//CannyImg.display("EdgeImg");
	
	HoughEdgeDetect myHoughEdgeDetect(CannyImg);
	myHoughEdgeDetect.HoughTransAndDetectEdge();
	HoughSpaceImg = myHoughEdgeDetect.getHoughSpaceImage();
	//HoughSpaceImg.display("houghArray");
	
	Edge_Point_Img = myHoughEdgeDetect.getCannyGrayImageWithEdge();
	//Edge_Point_Img.display("Edge_Point_Img");

	SrcImgWithVertexAndEdge = myHoughEdgeDetect.getFinallyProcessedImage(SrcImg);
	SrcImgWithVertexAndEdge.display("Finally Processed Image");

	
	//------------------------- A��ֽ�������� -------------------------//

	PaperModification myPaperModification(SrcImg);
	myPaperModification.setVertexSet(myHoughEdgeDetect.getTopFourVertexSet(), myHoughEdgeDetect.getDownSampledSize());

	PaperModifiedImg = myPaperModification.getModifiedPaper();

	end = time(NULL);
	cout << endl << endl << "==== PaperModification End: " << end - start << " seconds is used." << endl << endl;

	PaperModifiedImg.display("Paper Modified Image");
	//PaperModifiedImg.save("Output/Modification/7-Modification.bmp");
	

	
	//����ģʽ��ȡ�ļ�
	//PaperModifiedImg.load_bmp("Output/Modification/7-Modification.bmp");

	//------------------------- ������ȡ�ָ�� -------------------------//

	CImg<int> NewGraySrcImg, BinaryImg, HistogramImage, DividingImg, 
		ColoredNumberDividedImg, NumberDividedCircledImg;
	NewGraySrcImg = grayTheImg(PaperModifiedImg);
	//NewGraySrcImg.display("New Gray SrcImg");

	ImageSegmentation myImageSegmentation(NewGraySrcImg);
	myImageSegmentation.processBinaryImage();
	BinaryImg = myImageSegmentation.getBinaryImage();
	BinaryImg.display("Binary Img");
	//BinaryImg.save("Output/BinaryImage/7-BinaryImage.bmp");
	
	myImageSegmentation.numberSegmentationMainProcess("Output/SingleNumberImage/7");
	HistogramImage = myImageSegmentation.getHistogramImage();
	//HistogramImage.display("Histogram Image");
	//HistogramImage.save("Output/HistoDividedImage/7-HistogramImage.bmp");
	DividingImg = myImageSegmentation.getImageWithDividingLine();
	DividingImg.display("Image With Dividing Line");
	//DividingImg.save("Output/HistoDividedImage/7-DividingImg.bmp");

	ColoredNumberDividedImg = myImageSegmentation.getColoredNumberDividedImg();
	ColoredNumberDividedImg.display("Colored Number Divided Img");
	//ColoredNumberDividedImg.save("Output/ColoredNumberDividedImg/7-ColoredNumberDividedImg.bmp");
	NumberDividedCircledImg = myImageSegmentation.getNumberDividedCircledImg();
	NumberDividedCircledImg.display("Number Divided Circled Img");
	//NumberDividedCircledImg.save("Output/NumberDividedCircledImg/7-NumberDividedCircledImg.bmp");
	

	//------------------------- ����ʶ��Ԥ�ⲿ�� -------------------------//
	
	SVMPredictor svmPredictor("Output/SingleNumberImage/7");
	svmPredictor.readPredictImagelist("predict_imagelist.txt");
	svmPredictor.predict_mainProcess("TrainedModels/trainData_scaled_c=4_g=0.015625.model");
	
	
	cout << endl << "Press any key to end..." << endl;
	int pause;
	cin >> pause;
	return 0;
}
