/******************************************
* Copyright (C) 2015 HolaMirai(HolaMirai@163.com)
* All rights reserved.
* 
* �ļ�����CFeatures.h
* ժҪ��ͼ��ǰ�ô������ڻ�ȡ������
* ��ǰ�汾��V1.0,2015��9��13�գ�HolaMirai,�������ļ�
* ��ʷ��¼��...
******************************************/

/*
* �ඨ��˵��
*/
/********************************************
* CFeatures��
* CFeatures�����һ��intֵ����ʾ����������
* ʹ��getFeatures()�������Եõ�������Ķ�ֵ��ͼ��
* һ������ֻ�趨��һ��CFeatures���ʵ��
********************************************/

#ifndef CFEATURES_H
#define CFEATURES_H

enum feature_type
{
	FEATURE_CANNY,		//������ΪCANNY��Ե��
	FEATURE_HARRIS,		//������ΪHarris������
	FEATURE_SIFT,		//������ΪSIFT������
	FEATURE_SURF,		//������ΪSURF������
};

#include "cv.h"
#include <vector>

/*
* �ඨ��
*/
class CFeatures
{
public:
	CFeatures(int feature_type);
	~CFeatures();

public:
	void getFeatures(IplImage *grayImg, IplImage *featuresImg);
	void drawFeatures(const char *name, IplImage *img);

private:
	void vecPointsToImage(IplImage *featuresImg);

public:
	IplImage *m_featuresImg;
	IplImage *m_imgShow;
	std::vector<cv::KeyPoint> m_vecPoints;

private:
	int m_type;
};

#endif