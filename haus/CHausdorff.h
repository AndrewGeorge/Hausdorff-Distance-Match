/******************************************
* Copyright (C) 2015 HolaMirai(HolaMirai@163.com)
* All rights reserved.
* 
* �ļ�����hausdorff.h
* ժҪ��Ѱ��Hausdorff Distance��������ƥ��λ�ú���ƥ��λ�õľ���
* ��ǰ�汾��<V1.0>,<������ڣ�2015��9��7��>��<HolaMirai>,<�������ļ�>
* ��ʷ��¼��...
******************************************/


/*
* �ඨ��˵��
*/
/********************************************
* CHausforff��
* CHausdorff����ܴ�ƥ��ͼ��ģ��ͼ�Ķ�ֵ��ͼ����������������Ĭ��ֵ
* ʹ��match��������ֱ�ӻ�ȡƥ����
* hausdorff distance ���ò��־��뷨�����ڵ�������Ч������
* ƥ����̷�Ϊ��ƥ��;�ƥ�������
* �������Ѿ�������Ե�������ĳ�������Թ۲�hausdorff distance �����ĸ�����
********************************************/

#ifndef CHAUSDORFF_H
#define CHAUSDORFF_H

/*
* �궨��
*/
// ������
#define MAX_DISTANCE INT_MAX

#include "cv.h"

/*
*  �ඨ��
*/
class CHausdorff
{
public:
	CHausdorff(){};
	CHausdorff(IplImage *srcImg, IplImage *modelImge, 
				int xInterval = 1, int yInterval = 1, 
				double frontRange = 0.8, double backRange = 0.7);
	~CHausdorff();

public:
	void setScanInterval_X(int xInterval);
	 void setScanInterval_Y(int yInterval);

	 void setFrontRange(double Range);
	 void setBackRange(double Range);

	 double computeDirDistance(CvPoint Apoints[], CvPoint Bpoints[], int numA, int numB, double Range);
	 double distance(CvPoint Apoints[], CvPoint Bpoints[], int numA, int numB);
	 
	 void match();
	 void drawMatch(IplImage *img = NULL);

private:
	int searchPoints(IplImage *img, CvPoint pPoints[]);

	void sort(double a[],int n);
	double meanDistance(double a[], int n);

	void createTable();
	void freeTable();

public:
	//��¼ƥ����
	CvPoint m_matchPoint;
	double  m_matchDistance;

private:
	int m_xInterval;
	int m_yInterval;
	double m_frontRange;
	double m_backRange;

	double **m_disTable;

public:
	IplImage *m_srcImg;
	IplImage *m_modelImg;
};

#endif