/******************************************
* Copyright (C) 2015 HolaMirai(HolaMirai@163.com)
* All rights reserved.
* 
* �ļ�����CFeatures.cpp
* ժҪ��ͼ��ǰ�ô������ڻ�ȡ������
* ��ǰ�汾��V2.0,2015��9��14�գ�HolaMirai,��������������ȡ����
* ��ʷ��¼��V1.0,2015��9��13�գ�HolaMirai,�������ļ�
******************************************/

#include "CFeatures.h"
#include "highgui.h"
#include <opencv2/nonfree/nonfree.hpp>  
using namespace cv;

/***********************************************/

/*
* �������ƣ�CFeatures
* �������ܣ�CFeatures�๹�캯��
* ������ڣ� 
* ��������� ��Ҫ��ȡ������������������ͣ�ֵΪenum feature_type����һ��
* �����������
* �� �� ֵ��
* ����˵���� 
*/
CFeatures::CFeatures(int feature_type)
{
	m_type = feature_type;
	initModule_nonfree();//if use SIFT or SURF
}

/*
* �������ƣ�~CFeatures
* �������ܣ�CFeatures����������
* ������ڣ� 
* ��������� 
* ���������
* �� �� ֵ��
* ����˵���� 
*/
CFeatures::~CFeatures()
{

}

/*
* �������ƣ�getFeatures
* �������ܣ���ȡ������
* ������ڣ� 
* ��������� ������ͼ��ĻҶ�ͼ grayImg ָ��, ����������ĻҶ�ͼ��ָ�� featuresImg, featuresImg�� grayImg�ߴ���ͬ
* ���������������ĻҶ�ͼ�� featuresImg
* �� �� ֵ��
* ����˵���� 
*/
void CFeatures::getFeatures(IplImage *grayImg, IplImage *featuresImg)
{
	switch (m_type)
	{
	case FEATURE_CANNY:
		{
			assert(grayImg->width == featuresImg->width &&
				grayImg->height == featuresImg->height &&
				grayImg->nChannels == grayImg->nChannels);

			cvCanny(grayImg,featuresImg,50,150);
			break;
		}
	case FEATURE_HARRIS:
		{
			//std::vector<cv::KeyPoint> vecPoints;
			Ptr<FeatureDetector> detector = FeatureDetector::create( "HARRIS" );    //������Ѱ��
			detector->detect(grayImg, m_vecPoints);
			//m_vecPoints = std::move(vecPoints);
			vecPointsToImage(featuresImg);
			break;
		}

	case FEATURE_SIFT:
		{
			std::vector<cv::KeyPoint> vecPoints;
			Ptr<FeatureDetector> detector = FeatureDetector::create( "SIFT" );    //������Ѱ��
			detector->detect(grayImg, vecPoints);
			m_vecPoints = std::move(vecPoints);
			vecPointsToImage(featuresImg);
			break;
		}

	case FEATURE_SURF:
		{
			std::vector<cv::KeyPoint> vecPoints;
			Ptr<FeatureDetector> detector = FeatureDetector::create( "SURF" );    //������Ѱ��
			detector->detect(grayImg, vecPoints);
			m_vecPoints = std::move(vecPoints);
			vecPointsToImage(featuresImg);
			break;
		}
	default:
		break;
	}
} /*getFeatures()*/

/*
* �������ƣ�vecPointsToImage
* �������ܣ���vector���͵�������任�� IplImage ͼ������
* ������ڣ� 
* ��������� ����������m_vecPoints, ����������ĻҶ�ͼ��ָ�� featuresImg
* ���������������ĻҶ�ͼ�� featuresImg
* �� �� ֵ��
* ����˵���� 
*/
void CFeatures::vecPointsToImage(IplImage *featuresImg)
{
	vector<KeyPoint>::iterator it;
	vector<KeyPoint>::iterator end_it = m_vecPoints.end();

	cvZero(featuresImg);

	int w,h;
	uchar *ptr;
	for (it = m_vecPoints.begin(); it != end_it; it++)
	{
		w= (int)it->pt.x;
		h = (int)it->pt.y;
		ptr = (uchar *)(featuresImg->imageData + h*featuresImg->widthStep + w);
		// ��������255��������0
		*ptr = 255;
	}
	ptr = NULL;
}/*vecPointsToImage()*/

/*
* �������ƣ�drawFeatures
* �������ܣ�����������
* ������ڣ� 
* ��������� ��ʾ���� name, ���ĸ�ͼ���ϻ����������ͼ��ָ�� img
* ���������
* �� �� ֵ��void
* ����˵���������������һ�δ����ͼƬ��������
*/
void CFeatures::drawFeatures(const char *name, IplImage *img)
{
	IplImage *m_imgShow = cvCloneImage(img);

	vector<KeyPoint>::iterator it;
	vector<KeyPoint>::iterator end_it = m_vecPoints.end();

	CvPoint center;
	for (it = m_vecPoints.begin(); it != end_it; it++)
	{
		center.x = (int)it->pt.x;
		center.y  = (int)it->pt.y;

		cvCircle(m_imgShow,center,3,cvScalar(rand()%255,rand()%255,rand()%255));
	}
	cvShowImage(name,m_imgShow);
	cvReleaseImage(&m_imgShow);
}/*drawFeatures()*/