#define _CRT_SECURE_NO_WARNINGS

#include "ImageSegmentation.h"
#include <direct.h>

int lineColor[3]{ 255, 0, 0 };

ImageSegmentation::ImageSegmentation(const CImg<int>& _SrcImg) {
	SrcGrayImg = CImg<int>(_SrcImg._width, _SrcImg._height, 1, 1, 0);
	imgW = _SrcImg._width;
	imgH = _SrcImg._height;
	cimg_forXY(SrcGrayImg, x, y) {
		SrcGrayImg(x, y, 0) = _SrcImg(x, y, 0);
	}
}

ImageSegmentation::~ImageSegmentation() {
}

//XY�����������
int getDilationIntensityXY(const CImg<int>& Img, int x, int y) {
	int intensity = Img(x, y, 0);
	if (intensity == 255) {
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				if (0 <= x + i && x + i < Img._width
					&& 0 <= y + j && y + j < Img._height) {
					if (i != -1 && j != -1 || i != 1 && j != 1 || i != 1 && j != -1 || i != -1 && j != 1)
						if (Img(x + i, y + j, 0) == 0) {
							intensity = 0;
							break;
						}
				}
			}
			if (intensity != 255)
				break;
		}
	}
	return intensity;
}
//X�����������
int getDilationIntensityX(const CImg<int>& Img, int x, int y) {
	int intensity = Img(x, y, 0);
	if (intensity == 255) {
		for (int i = -1; i <= 1; i++) {
			if (0 <= x + i && x + i < Img._width) {
				if (Img(x + i, y, 0) == 0) {
					intensity = 0;
					break;
				}
			}
		}
	}
	return intensity;
}
//Y�����������
int getDilationIntensityY(const CImg<int>& Img, int x, int y) {
	int intensity = Img(x, y, 0);
	if (intensity == 255) {
		for (int j = -1; j <= 1; j++) {
			if (0 <= y + j && y + j < Img._height) {
				if (Img(x, y + j, 0) == 0) {
					intensity = 0;
					break;
				}
			}
		}
	}
	return intensity;
}

//X����2����λ�ĸ����ţ�Y����2����λ��������
int getDilationIntensityXXYY(const CImg<int>& Img, int x, int y) {
	int intensity = Img(x, y, 0);
	if (intensity == 255) {    //���м��Ϊ��ɫ
		int blackAccu = 0;

		for (int i = -2; i <= 2; i++) {
			if (0 <= y + i && y + i < Img._height) {    //��ֱ�����ۼ�
				if (Img(x, y + i, 0) == 0)
					blackAccu++;
			}
			if (0 <= x + i && x + i < Img._width) {     //ˮƽ�����ۼ�
				if (Img(x + i, y, 0) == 0)
					blackAccu--;
			}
		}

		if (blackAccu > 0)
			intensity = 0;
	}
	return intensity;
}

//X����2����λ�ĸ����ţ�Y����1����λ��������
int getDilationIntensityXXY(const CImg<int>& Img, int x, int y) {
	int intensity = Img(x, y, 0);
	if (intensity == 255) {    //���м��Ϊ��ɫ
		int blackAccu = 0;

		for (int i = -1; i <= 1; i++) {
			if (0 <= y + i && y + i < Img._height) {    //��ֱ�����ۼ�
				if (Img(x, y + i, 0) == 0)
					blackAccu++;
			}
		}
		for (int i = -2; i <= 2; i++) {
			if (0 <= x + i && x + i < Img._width) {     //ˮƽ�����ۼ�
				if (Img(x + i, y, 0) == 0)
					blackAccu--;
			}
		}

		if (blackAccu > 0)
			intensity = 0;
	}
	return intensity;
}

//X����1����λ�ĸ����ţ�Y����1����λ��������
int getDilationIntensityXrY(const CImg<int>& Img, int x, int y) {
	int intensity = Img(x, y, 0);
	if (intensity == 255) {    //���м��Ϊ��ɫ
		int blackAccu = 0;

		for (int i = -1; i <= 1; i++) {
			if (0 <= y + i && y + i < Img._height) {    //��ֱ�����ۼ�
				if (Img(x, y + i, 0) == 0)
					blackAccu++;
			}
			if (0 <= x + i && x + i < Img._width) {     //ˮƽ�����ۼ�
				if (Img(x + i, y, 0) == 0)
					blackAccu--;
			}
		}

		if (blackAccu > 0)
			intensity = 0;
	}
	return intensity;
}

void ImageSegmentation::processBinaryImage() {
	CImg<int> answer = CImg<int>(SrcGrayImg._width, SrcGrayImg._height, 1, 1, 0);

	cimg_forXY(answer, x, y) {
		int intensity = SrcGrayImg(x, y, 0);

		//��ȥ����ɫ��Ե
		if (x <= BoundaryRemoveGap || y <= BoundaryRemoveGap
			|| x >= imgW - BoundaryRemoveGap || y >= imgH - BoundaryRemoveGap) {
			answer(x, y, 0) = 255;
		}
		else {
			if (intensity < BinaryGap)
				answer(x, y, 0) = 0;
			else
				answer(x, y, 0) = 255;
		}
	}

	BinaryImg = answer;
}

CImg<int> ImageSegmentation::getBinaryImage() {
	return BinaryImg;
}

void ImageSegmentation::numberSegmentationMainProcess(const string baseAddress) {
	if (_access(baseAddress.c_str(), 0) == -1)
		_mkdir(baseAddress.c_str());
	basePath = baseAddress + "/";

	findDividingLine();
	divideIntoBarItemImg();

	//for (int i = 0; i < subImageSet.size(); i++) {
	//	subImageSet[i].display("subImageSet");
	//}

	connectedRegionsTagging();
}

void ImageSegmentation::findDividingLine() {
	HistogramImage = CImg<int>(BinaryImg._width, BinaryImg._height, 1, 3, 0);
	DividingImg = CImg<int>(BinaryImg._width, BinaryImg._height, 1, 3, 0);
	int lineColor[3]{ 255, 0, 0 };

	cimg_forY(HistogramImage, y) {
		int blackPixel = 0;
		cimg_forX(BinaryImg, x) {
			HistogramImage(x, y, 0) = 255;
			HistogramImage(x, y, 1) = 255;
			HistogramImage(x, y, 2) = 255;
			DividingImg(x, y, 0) = BinaryImg(x, y, 0);
			DividingImg(x, y, 1) = BinaryImg(x, y, 0);
			DividingImg(x, y, 2) = BinaryImg(x, y, 0);
			if (BinaryImg(x, y, 0) == 0)
				blackPixel++;
		}

		cimg_forX(HistogramImage, x) {
			if (x < blackPixel) {
				HistogramImage(x, y, 0) = 0;
				HistogramImage(x, y, 1) = 0;
				HistogramImage(x, y, 2) = 0;
			}
		}

		//�ж��Ƿ�Ϊ�յ�
		if (y > 0) {
			if (blackPixel <= YHistogramValleyMaxPixelNumber
				&& HistogramImage(YHistogramValleyMaxPixelNumber, y - 1, 0) == 0) {    //�°��Ϻڣ�ȡ��
				inflectionYPointSet.push_back(y);
				//HistogramImage.draw_line(0, y, HistogramImage._width - 1, y, lineColor);
			}
			else if (blackPixel > YHistogramValleyMaxPixelNumber
				&& HistogramImage(YHistogramValleyMaxPixelNumber, y - 1, 0) != 0) {    //�º��ϰף�ȡ��
				inflectionYPointSet.push_back(y - 1);
				//HistogramImage.draw_line(0, y - 1, HistogramImage._width - 1, y - 1, lineColor);
			}
		}
	}

	divideLinePointSet.push_back(PointPos(0, -1));
	//���յ��м����ָ�
	if (inflectionYPointSet.size() > 2) {
		for (int i = 1; i < inflectionYPointSet.size() - 1; i = i + 2) {
			int divideLinePointY = (inflectionYPointSet[i] + inflectionYPointSet[i + 1]) / 2;
			divideLinePointSet.push_back(PointPos(0, divideLinePointY));
		}
	}
	divideLinePointSet.push_back(PointPos(0, BinaryImg._height - 1));
}

//����X����ֱ��ͼ�ж���ʵ�Ĺյ�
vector<int> getInflectionPosXs(const CImg<int>& XHistogramImage) {
	vector<int> resultInflectionPosXs;
	vector<int> tempInflectionPosXs;
	int totalDist = 0, avgDist;
	int distNum = 0;
	//���ҹյ�
	cimg_forX(XHistogramImage, x) {
		if (x >= 1) {
			//��ת��
			if (XHistogramImage(x, 0, 0) == 0 && XHistogramImage(x - 1, 0, 0) == 255) {
				tempInflectionPosXs.push_back(x - 1);
			}
			//��ת��
			else if (XHistogramImage(x, 0, 0) == 255 && XHistogramImage(x - 1, 0, 0) == 0) {
				tempInflectionPosXs.push_back(x);
			}
		}
	}
	for (int i = 2; i < tempInflectionPosXs.size() - 1; i = i + 2) {
		int dist = tempInflectionPosXs[i] - tempInflectionPosXs[i - 1];
		if (dist <= 0)
			distNum--;
		totalDist += dist;
	}

	//������ƽ������
	distNum += (tempInflectionPosXs.size() - 2) / 2;
	avgDist = totalDist / distNum;
	//cout << "avgDist " << avgDist << endl;

	resultInflectionPosXs.push_back(tempInflectionPosXs[0]);    //ͷ
	//��ĳ��������ƽ�������һ������ʱ����Ϊ�ָ�����ڼ��
	for (int i = 2; i < tempInflectionPosXs.size() - 1; i = i + 2) {
		int dist = tempInflectionPosXs[i] - tempInflectionPosXs[i - 1];
		//cout << "dist " << dist << endl;
		if (dist > avgDist * XHistogramValleyMaxPixelNumber) {
			resultInflectionPosXs.push_back(tempInflectionPosXs[i - 1]);
			resultInflectionPosXs.push_back(tempInflectionPosXs[i]);
		}
	}
	resultInflectionPosXs.push_back(tempInflectionPosXs[tempInflectionPosXs.size() - 1]);  //β

	return resultInflectionPosXs;
}

//��ȡһ���е���ͼ��ˮƽ�ָ���
vector<int> getDivideLineXofSubImage(const CImg<int>& subImg) {
	vector<int> InflectionPosXs;

	//�Ȼ���X����Ҷ�ֱ��ͼ
	CImg<int> XHistogramImage = CImg<int>(subImg._width, subImg._height, 1, 3, 0);
	cimg_forX(subImg, x) {
		int blackPixel = 0;
		cimg_forY(subImg, y) {
			XHistogramImage(x, y, 0) = 255;
			XHistogramImage(x, y, 1) = 255;
			XHistogramImage(x, y, 2) = 255;
			if (subImg(x, y, 0) == 0)
				blackPixel++;
		}
		//����ÿһ��x��ֻ�к�ɫ���ض���һ��ֵ���Ż�����ֱ��ͼ��
		if (blackPixel >= XHistogramValleyMaxPixelNumber) {
			cimg_forY(subImg, y) {
				if (y < blackPixel) {
					XHistogramImage(x, y, 0) = 0;
					XHistogramImage(x, y, 1) = 0;
					XHistogramImage(x, y, 2) = 0;
				}
			}
		}
	}

	InflectionPosXs = getInflectionPosXs(XHistogramImage);    //��ȡ�յ�
	cout << "InflectionPosXs.size() " << InflectionPosXs.size() << endl;
	for (int i = 0; i < InflectionPosXs.size(); i++)
		XHistogramImage.draw_line(InflectionPosXs[i], 0, InflectionPosXs[i], XHistogramImage._height - 1, lineColor);
	//XHistogramImage.display("XHistogramImage");

	//���յ��м����ָ�
	vector<int> dividePosXs;
	dividePosXs.push_back(-1);
	if (InflectionPosXs.size() > 2) {
		for (int i = 1; i < InflectionPosXs.size() - 1; i = i + 2) {
			int divideLinePointX = (InflectionPosXs[i] + InflectionPosXs[i + 1]) / 2;
			dividePosXs.push_back(divideLinePointX);
		}
	}
	dividePosXs.push_back(XHistogramImage._width - 1);

	return dividePosXs;
}

//�ָ�����ͼ���õ�����ͼ
//@_dividePosXset ��-1����lineImg._width����
vector<CImg<int>> getRowItemImgSet(const CImg<int>& lineImg, vector<int> _dividePosXset) {
	vector<CImg<int>> result;
	for (int i = 1; i < _dividePosXset.size(); i++) {
		int rowItemWidth = _dividePosXset[i] - _dividePosXset[i - 1];
		CImg<int> rowItemImg = CImg<int>(rowItemWidth, lineImg._height, 1, 1, 0);
		cimg_forXY(rowItemImg, x, y) {
			rowItemImg(x, y, 0) = lineImg(x + _dividePosXset[i - 1] + 1, y, 0);
		}
		result.push_back(rowItemImg);
	}

	return result;
}


void ImageSegmentation::divideIntoBarItemImg() {
	vector<PointPos> newDivideLinePointSet;

	for (int i = 1; i < divideLinePointSet.size(); i++) {
		int barHeight = divideLinePointSet[i].y - divideLinePointSet[i - 1].y;
		int blackPixel = 0;
		CImg<int> barItemImg = CImg<int>(BinaryImg._width, barHeight, 1, 1, 0);
		cimg_forXY(barItemImg, x, y) {
			barItemImg(x, y, 0) = BinaryImg(x, divideLinePointSet[i - 1].y + 1 + y, 0);
			if (barItemImg(x, y, 0) == 0)
				blackPixel++;
		}

		double blackPercent = (double)blackPixel / (double)(BinaryImg._width * barHeight);
		//cout << "blackPercent " << blackPercent << endl;

		//ֻ�е���ɫ���ظ�������ͼ���Сһ������ʱ���ſ�����������
		if (blackPercent > SubImgBlackPixelPercentage) {
			vector<int> dividePosXset = getDivideLineXofSubImage(barItemImg);
			vector<CImg<int>> rowItemImgSet = getRowItemImgSet(barItemImg, dividePosXset);

			for (int j = 0; j < rowItemImgSet.size(); j++) {
				subImageSet.push_back(rowItemImgSet[j]);
				newDivideLinePointSet.push_back(PointPos(dividePosXset[j], divideLinePointSet[i - 1].y));
			}

			if (i > 1) {
				HistogramImage.draw_line(0, divideLinePointSet[i - 1].y,
					HistogramImage._width - 1, divideLinePointSet[i - 1].y, lineColor);
				DividingImg.draw_line(0, divideLinePointSet[i - 1].y,
					HistogramImage._width - 1, divideLinePointSet[i - 1].y, lineColor);
			}
			//��������
			for (int j = 1; j < dividePosXset.size() - 1; j++) {
				DividingImg.draw_line(dividePosXset[j], divideLinePointSet[i - 1].y,
					dividePosXset[j], divideLinePointSet[i].y, lineColor);
			}
		}
	}

	divideLinePointSet.clear();
	for (int i = 0; i < newDivideLinePointSet.size(); i++)
		divideLinePointSet.push_back(newDivideLinePointSet[i]);
}

void ImageSegmentation::connectedRegionsTagging() {
	//��ÿ����ͼ����
	for (int i = 0; i < subImageSet.size(); i++) {
		doDilationForEachBarItemImg(i);
		connectedRegionsTaggingOfBarItemImg(i);

		saveSingleNumberImageAndImglist(i);
		cout << imglisttxt.c_str() << endl;
	}

	ofstream predictImageListOutput(basePath + "predict_imagelist.txt");
	predictImageListOutput << imglisttxt.c_str();
	predictImageListOutput.close();
}

void ImageSegmentation::doDilationForEachBarItemImg(int barItemIndex) {
	//����Dilation -X-X-X-XYY����
	CImg<int> answerXXY = CImg<int>(subImageSet[barItemIndex]._width, subImageSet[barItemIndex]._height, 1, 1, 0);
	cimg_forXY(subImageSet[barItemIndex], x, y) {
		int intensity = getDilationIntensityXXY(subImageSet[barItemIndex], x, y);
		answerXXY(x, y, 0) = intensity;
	}

	//����Dilation -X-X-X-XYY����
	CImg<int> answerXXY2 = CImg<int>(answerXXY._width, answerXXY._height, 1, 1, 0);
	cimg_forXY(answerXXY, x, y) {
		int intensity = getDilationIntensityXXY(answerXXY, x, y);
		answerXXY2(x, y, 0) = intensity;
	}

	//����Dilation XY����
	CImg<int> answerXY = CImg<int>(answerXXY2._width, answerXXY2._height, 1, 1, 0);
	cimg_forXY(answerXXY2, x, y) {
		int intensity = getDilationIntensityXY(answerXXY2, x, y);
		answerXY(x, y, 0) = intensity;
	}

	cimg_forXY(subImageSet[barItemIndex], x, y) {
		subImageSet[barItemIndex](x, y, 0) = answerXY(x, y, 0);
	}
}

void ImageSegmentation::connectedRegionsTaggingOfBarItemImg(int barItemIndex) {
	TagImage = CImg<int>(subImageSet[barItemIndex]._width, subImageSet[barItemIndex]._height, 1, 1, 0);
	tagAccumulate = -1;

	cimg_forX(subImageSet[barItemIndex], x)
		cimg_forY(subImageSet[barItemIndex], y) {
		//��һ�к͵�һ��
		if (x == 0 || y == 0) {
			int intensity = subImageSet[barItemIndex](x, y, 0);
			if (intensity == 0) {
				addNewClass(x, y, barItemIndex);
			}
		}
		//������к���
		else {
			int intensity = subImageSet[barItemIndex](x, y, 0);
			if (intensity == 0) {
				//������ϡ����ϡ����С��������ĸ��ڵ�

				int minTag = Infinite;        //��С��tag
				PointPos minTagPointPos(-1, -1);

				//������С�ı��
				findMinTag(x, y, minTag, minTagPointPos, barItemIndex);

				//�����ϡ����ϡ����С��������ĸ��ڵ��к�ɫ��ʱ���ϲ���
				if (minTagPointPos.x != -1 && minTagPointPos.y != -1) {
					mergeTagImageAndList(x, y - 1, minTag, minTagPointPos, barItemIndex);
					for (int i = -1; i <= 1; i++) {
						if (y + i < subImageSet[barItemIndex]._height)
							mergeTagImageAndList(x - 1, y + i, minTag, minTagPointPos, barItemIndex);
					}

					//��ǰλ��
					TagImage(x, y, 0) = minTag;
					PointPos cPoint(x + divideLinePointSet[barItemIndex].x + 1, y + divideLinePointSet[barItemIndex].y + 1);
					pointPosListSet[minTag].push_back(cPoint);

				}
				//������Ϊ����
				else {
					addNewClass(x, y, barItemIndex);
				}
			}
		}
	}
}

void ImageSegmentation::addNewClass(int x, int y, int barItemIndex) {
	tagAccumulate++;
	//cout << "tagAccumulate " << tagAccumulate << endl;
	TagImage(x, y, 0) = tagAccumulate;
	classTagSet.push_back(tagAccumulate);
	list<PointPos> pList;
	PointPos cPoint(x + divideLinePointSet[barItemIndex].x + 1, y + divideLinePointSet[barItemIndex].y + 1);
	pList.push_back(cPoint);
	pointPosListSet.push_back(pList);
}

void ImageSegmentation::findMinTag(int x, int y, int &minTag, PointPos &minTagPointPos, int barItemIndex) {
	if (subImageSet[barItemIndex](x, y - 1, 0) == 0) {     //����
		if (TagImage(x, y - 1, 0) < minTag) {
			minTag = TagImage(x, y - 1, 0);
			minTagPointPos.x = x;
			minTagPointPos.y = y - 1;
		}
	}
	for (int i = -1; i <= 1; i++) {        //���ϡ����С�����
		if (y + i < subImageSet[barItemIndex]._height) {
			if (subImageSet[barItemIndex](x - 1, y + i, 0) == 0 && TagImage(x - 1, y + i, 0) < minTag) {
				minTag = TagImage(x - 1, y + i, 0);
				minTagPointPos.x = x - 1;
				minTagPointPos.y = y + i;
			}
		}
	}
}

void ImageSegmentation::mergeTagImageAndList(int x, int y, const int minTag, const PointPos minTagPointPos, int barItemIndex) {
	//������С��ǣ��鲢�б�
	if (subImageSet[barItemIndex](x, y, 0) == 0) {
		int tagBefore = TagImage(x, y, 0);
		if (tagBefore != minTag) {    //������С��tag
			//������ͬһ���tag�滻Ϊ��Сtag����list�ӵ���Сtag��list
			list<PointPos>::iterator it = pointPosListSet[tagBefore].begin();
			for (; it != pointPosListSet[tagBefore].end(); it++) {
				TagImage((*it).x - divideLinePointSet[barItemIndex].x - 1, (*it).y - divideLinePointSet[barItemIndex].y - 1, 0) = minTag;
			}
			pointPosListSet[minTag].splice(pointPosListSet[minTag].end(), pointPosListSet[tagBefore]);
		}
	}
}


CImg<int> do_Y_Erosion(CImg<int>& Img) {
	CImg<int> result = CImg<int>(Img._width, Img._height, 1, 1, 0);
	cimg_forXY(Img, x, y) {
		result(x, y, 0) = Img(x, y, 0);
		if (Img(x, y, 0) == 255) {
			if (y - 1 >= 0) {
				if (Img(x, y - 1, 0) == 0)
					result(x, y, 0) = 0;
			}

			if (y + 1 < Img._height) {
				if (Img(x, y + 1, 0) == 0)
					result(x, y, 0) = 0;
			}
		}
	}
	return result;
}
void ImageSegmentation::saveSingleNumberImageAndImglist(int barItemIndex) {
	//��ͳ��ÿ������ͼ���ɫ���ظ���ƽ��ֵ
	int totalBlacks = 0, numberCount = 0;
	for (int i = 0; i < pointPosListSet.size(); i++) {
		if (pointPosListSet[i].size() != 0) {
			//cout << "pointPosListSet[i].size() " << pointPosListSet[i].size() << endl;
			totalBlacks += pointPosListSet[i].size();
			numberCount++;
		}
	}
	int avgBlacks = totalBlacks / numberCount;
	//cout << endl << "avgBlacks " << avgBlacks << endl << endl;

	for (int i = 0; i < pointPosListSet.size(); i++) {
		//ֻ�к�ɫ���ظ������� ƽ��ֵ��һ���������ſ���Ϊ����ͼ��ȥ���ϵ㣩
		if (pointPosListSet[i].size() != 0 && pointPosListSet[i].size() > avgBlacks * NumberImageBlackPixelPercentage) {
			//���ҵ����ֵİ�Χ��
			int xMin, xMax, yMin, yMax;
			getBoundingOfSingleNum(i, xMin, xMax, yMin, yMax);

			int width = xMax - xMin;
			int height = yMax - yMin;

			//������������䵽��ͼ�����䵽������
			int imgSize = (width > height ? width : height) + SingleNumberImgBoundary * 2;
			CImg<int> singleNum = CImg<int>(imgSize, imgSize, 1, 1, 0);

			list<PointPos>::iterator it = pointPosListSet[i].begin();
			for (; it != pointPosListSet[i].end(); it++) {
				int x = (*it).x;
				int y = (*it).y;
				int singleNumImgPosX, singleNumImgPosY;
				if (height > width) {
					singleNumImgPosX = (x - xMin) + (imgSize - width) / 2;
					singleNumImgPosY = (y - yMin) + SingleNumberImgBoundary;
				}
				else {
					singleNumImgPosX = (x - xMin) + SingleNumberImgBoundary;
					singleNumImgPosY = (y - yMin) + (imgSize - height) / 2;
				}
				singleNum(singleNumImgPosX, singleNumImgPosY, 0) = 255;
			}

			//������������䵽��ͼ��ԭ�����
			//int imgSizeH = height + SingleNumberImgBoundary * 2;
			//int imgSizeW = width + SingleNumberImgBoundary * 2;
			//CImg<int> singleNum = CImg<int>(imgSizeW, imgSizeH, 1, 1, 0);
			//list<PointPos>::iterator it = pointPosListSet[i].begin();
			//for (; it != pointPosListSet[i].end(); it++) {
			//	int x = (*it).x;
			//	int y = (*it).y;
			//	int singleNumImgPosX, singleNumImgPosY;
			//	singleNumImgPosX = (x - xMin) + SingleNumberImgBoundary;
			//	singleNumImgPosY = (y - yMin) + SingleNumberImgBoundary;
			//	singleNum(singleNumImgPosX, singleNumImgPosY, 0) = 255;
			//}


			//�Ե�������ͼ����Y����ʴ����
			singleNum = do_Y_Erosion(singleNum);

			//singleNum.display("single Number");
			string postfix = ".bmp";
			char shortImgName[200];
			sprintf(shortImgName, "%d_%d%s\n", barItemIndex, classTagSet[i], postfix.c_str());
			imglisttxt += string(shortImgName);

			char addr[200];
			sprintf(addr, "%s%d_%d%s", basePath.c_str(), barItemIndex, classTagSet[i], postfix.c_str());
			singleNum.save(addr);

			pointPosListSetForDisplay.push_back(pointPosListSet[i]);
		}
	}
	imglisttxt += "*\n";

	//��tag����ÿһ���������ݼ����
	classTagSet.clear();
	for (int i = 0; i < pointPosListSet.size(); i++) {
		pointPosListSet[i].clear();
	}
	pointPosListSet.clear();
}

CImg<int> ImageSegmentation::getHistogramImage() {
	return HistogramImage;
}

CImg<int> ImageSegmentation::getImageWithDividingLine() {
	return DividingImg;
}

CImg<int> ImageSegmentation::getColoredNumberDividedImg() {
	vector<int*> ColorSet;
	int red[3]{ 255, 0, 0 };
	int green[3]{ 0, 255, 0 };
	int blue[3]{ 0, 0, 255 };
	int yellow[3]{ 255, 255, 0 };
	int purple[3]{ 255, 0, 255 };
	int qing[3]{ 0, 255, 255 };
	ColorSet.push_back(red);
	ColorSet.push_back(green);
	ColorSet.push_back(blue);
	ColorSet.push_back(yellow);
	ColorSet.push_back(purple);
	ColorSet.push_back(qing);

	CImg<int> answer = CImg<int>(BinaryImg._width, BinaryImg._height, 1, 3, 0);
	cimg_forXY(answer, x, y) {
		answer(x, y, 0) = 255;
		answer(x, y, 1) = 255;
		answer(x, y, 2) = 255;
	}

	int classTagNum = 0;
	for (int i = 0; i < pointPosListSetForDisplay.size(); i++) {
		if (pointPosListSetForDisplay[i].size() != 0) {
			classTagNum++;

			int colorIndex = i % ColorSet.size();
			list<PointPos>::iterator it = pointPosListSetForDisplay[i].begin();
			for (; it != pointPosListSetForDisplay[i].end(); it++) {
				for (int k = 0; k < 3; k++) {
					answer((*it).x, (*it).y, k) = ColorSet[colorIndex][k];
				}
			}
		}
	}
	cout << "classTagNum " << classTagNum << endl;

	return answer;
}

void ImageSegmentation::getBoundingOfSingleNum(int listIndex, int& xMin, int& xMax, int& yMin, int& yMax) {
	xMin = yMin = Infinite;
	xMax = yMax = -1;

	if (!pointPosListSet.empty()) {
		list<PointPos>::iterator it = pointPosListSet[listIndex].begin();
		for (; it != pointPosListSet[listIndex].end(); it++) {
			int x = (*it).x;
			int y = (*it).y;
			xMin = x < xMin ? x : xMin;
			yMin = y < yMin ? y : yMin;
			xMax = x > xMax ? x : xMax;
			yMax = y > yMax ? y : yMax;
		}
	}
	else {
		list<PointPos>::iterator it = pointPosListSetForDisplay[listIndex].begin();
		for (; it != pointPosListSetForDisplay[listIndex].end(); it++) {
			int x = (*it).x;
			int y = (*it).y;
			xMin = x < xMin ? x : xMin;
			yMin = y < yMin ? y : yMin;
			xMax = x > xMax ? x : xMax;
			yMax = y > yMax ? y : yMax;
		}
	}
}

CImg<int> ImageSegmentation::getNumberDividedCircledImg() {
	int lineColor[3]{ 255, 0, 0 };

	CImg<int> answer = CImg<int>(BinaryImg._width, BinaryImg._height, 1, 3, 0);
	cimg_forXY(answer, x, y) {
		answer(x, y, 0) = 255;
		answer(x, y, 1) = 255;
		answer(x, y, 2) = 255;
	}
	
	for (int i = 0; i < pointPosListSetForDisplay.size(); i++) {
		if (pointPosListSetForDisplay[i].size() != 0) {
			//�Ȼ�������
			list<PointPos>::iterator it = pointPosListSetForDisplay[i].begin();
			for (; it != pointPosListSetForDisplay[i].end(); it++) {
				for (int k = 0; k < 3; k++) {
					answer((*it).x, (*it).y, k) = 0;
				}
			}

			//�ҵ����ֵİ�Χ��
			int xMin, xMax, yMin, yMax;
			getBoundingOfSingleNum(i, xMin, xMax, yMin, yMax);

			//Ȼ����ư�Χ��
			answer.draw_line(xMin, yMin, xMax, yMin, lineColor);
			answer.draw_line(xMin, yMin, xMin, yMax, lineColor);
			answer.draw_line(xMin, yMax, xMax, yMax, lineColor);
			answer.draw_line(xMax, yMin, xMax, yMax, lineColor);
		}
	}

	return answer;
}