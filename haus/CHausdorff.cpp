/******************************************
* Copyright (C) 2015 HolaMirai(HolaMirai@163.com)
* All rights reserved.
* 
* �ļ�����hausdorff.cpp
* ժҪ��Ѱ��Hausdorff Distance��������ƥ��λ�ú���ƥ��λ�õľ���
* ��ǰ�汾�� v2.1.1,2016��1��1�գ�������CHausdorff��Ĺ��캯��δ���մ�ƥ��ͼ��Ĵ���
*			ΪHarris��SIFT��SURF�����㷵��ʱ���std::move�Ż���VS2010�¾���Ч��������
* ��ʷ��¼��V2.1, 2015��9��11�գ�HolaMirai, ��� createTable()��freeTable()���������ƽ��hausdorff���뷽��,���ע��
* ��ʷ��¼��V2.0, 2015��9��10�գ�HolaMirai, ������ɸ��ļ�&���meanDistance()����
*		  ��V1.0, 2015��9��7�գ� HolaMirai, �������ļ�
******************************************/

/*
*              �ɸĽ���
* ����Voronoiͼ����Hausdorff�������
*
*/

#include "CHausdorff.h"
#include "highgui.h"

/*
* �������ƣ�CHausdorff
* �������ܣ��๹�캯��
* ������ڣ� 
* ��������� ��ƥ��ͼ��ģ���ֵ��ͼ�񣬴�ƥ��ᡢ������ɨ���� xInterval��yInterval 
*            ǰ��������� frontRange������������� backRange
* �����������
* �� �� ֵ��
* ����˵���� 
*/
CHausdorff::CHausdorff(IplImage *srcImg, IplImage *modelImge, int xInterval, int yInterval, double frontRange, double backRange)
{
	m_srcImg = srcImg;
	m_modelImg = cvCloneImage(modelImge);

	m_frontRange = frontRange;
	m_backRange = backRange;
	m_xInterval = xInterval;
	m_yInterval = yInterval;

	createTable();
}

CHausdorff::~CHausdorff()
{
	freeTable();
	//cvReleaseImage(&m_srcImg);
	cvReleaseImage(&m_modelImg);

	
}

/*
* �������ƣ�setScanInterval_X
* �������ܣ����ô�ƥ��X����ɨ����
* ������ڣ� 
* ��������� ��ƥ�������ɨ���� xInterval
* �����������
* �� �� ֵ��void
* ����˵���� 
*/
void CHausdorff::setScanInterval_X(int xInterval)
{
	m_xInterval = xInterval;
}

/*
* �������ƣ�setScanInterval_Y
* �������ܣ����ô�ƥ��Y����ɨ����
* ������ڣ� 
* ��������� ��ƥ�������ɨ���� yInterval
* �����������
* �� �� ֵ��void
* ����˵���� 
*/
void CHausdorff::setScanInterval_Y(int yInterval)
{
	m_yInterval = yInterval;
}

/*
* �������ƣ�setFrontRange
* �������ܣ�����ǰ��ƥ���������
* ������ڣ� 
* ��������� ǰ��ƥ��������� Range
* �����������
* �� �� ֵ��void
* ����˵���� 
*/
void CHausdorff::setFrontRange(double Range)
{
	m_frontRange = Range;
}

/*
* �������ƣ�setFrontRange
* �������ܣ����ú���ƥ���������
* ������ڣ� 
* ��������� ǰ��ƥ��������� Range
* �����������
* �� �� ֵ��void
* ����˵���� 
*/
void CHausdorff::setBackRange(double Range)
{
	m_backRange = Range;
}

/*
* �������ƣ�searchPoints
* �������ܣ���������������������ĸ���
* ������ڣ� 
* ��������� �Ҷ�ͼ img�� ���������������ָ��
* �����������
* �� �� ֵ�� ���������
* ����˵���� ֧��ͼƬ ROI
*/
int CHausdorff::searchPoints(IplImage *img, CvPoint pPoints[])
{
	CvRect rect;
	if ( !img->roi)
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = img->width;
		rect.height = img->height;
	}
	else
	{
		rect.x = img->roi->xOffset;
		rect.y = img->roi->yOffset;
		rect.width = img->roi->width;
		rect.height = img->roi->height;
	}

	//ͳ��������
	uchar *ptr;
	int num = 0;
	for (int i = rect.y; i < rect.y + rect.height; i++)
	{
		ptr = (uchar *) (img->imageData + i * img->widthStep);
		for (int j = rect.x; j < rect.x + rect.width; j++)
		{
			if (ptr[j] != 0)
			{
				num++;
			}
		}
	}

	if (num == 0)
	{
		return 0;
	}

	num = 0;
	for (int i = rect.y; i < rect.y + rect.height; i++)
	{
		ptr = (uchar *) (img->imageData + i * img->widthStep);
		for (int j = rect.x; j < rect.x + rect.width; j++)
		{
			if (ptr[j] != 0)
			{
				pPoints[num].x = j - rect.x;
				pPoints[num].y = i - rect.y;
				num++;
			}
		}
	}

	//�������
	return num;
}/*searchPoints()*/


/*
* �������ƣ�match
* �������ܣ����� Hausdorff distance ֵ��ƥ��ͼƬ
* ������ڣ� 
* �����������
* ������������ƥ��λ�� �����ƥ�����
* �� �� ֵ�� void
* ����˵���� 
*/
void CHausdorff::match()
{
	int ws, hs, wm, hm, numModel, numSrc;
	CvRect rect;
	double *matchDis = new double[100];  //�ٶ�������100�����λ�ã����ﲢδʵ�֣������Ժ�Ľ�
	CvPoint *matchPoints = new CvPoint[100];
	double temp = MAX_DISTANCE;
	double dis = MAX_DISTANCE;
	int n = 0;  //ֻ��һ�����λ�ã����ĵļ���ǰ��


	ws = m_srcImg->width;
	hs = m_srcImg->height;
	wm = m_modelImg->width;
	hm = m_modelImg->height;

	// �洢ģ���������ԭͼ��������
	CvPoint *pMPoints = new CvPoint[wm * hm];
	CvPoint *pSPoints = new CvPoint[wm * hm];

	//���ģ��������
	numModel = searchPoints(m_modelImg,pMPoints);

	if (numModel == 0)
	{
		printf_s("find no points in model");
		return;
	}

	//�������ƥ��
	rect.width = wm;
	rect.height = hm;
	
	// ��ƥ��
	//#pragma omp parallel for firstprivate(m_srcImg)     //��������
	for (int i = 0; i < ws; i += m_xInterval)
	{
		rect.x = i;
		//#pragma omp parallel for 
		for (int j = 0; j < hs; j += m_yInterval)
		{
			rect.y = j;
			cvSetImageROI(m_srcImg,rect);
			numSrc = searchPoints(m_srcImg,pSPoints);
			if (numSrc == 0)
			{
				continue;
			}
			// ���������������̫������Ը�λ�ã����������������������
			if (double(numSrc)/numModel < 0.8 || (double)numSrc/numModel > 1.25)
			{
				continue;
			}

			temp = distance(pMPoints, pSPoints, numModel, numSrc);
			// min
			if (temp < dis)
			{
				dis = temp;
				matchDis[n] = temp;
				matchPoints[n].x = i;
				matchPoints[n].y = j;
			} 	
		}
	}

	//��ƥ��
	for (int i = matchPoints[n].x - m_xInterval; i < matchPoints[n].x + m_xInterval; i++)
	{
		rect.x = i;
		for (int j = matchPoints[n].y - m_yInterval; j < matchPoints[n].y + m_yInterval; j++)
		{
			rect.y = j;
			cvSetImageROI(m_srcImg,rect);
			numSrc = searchPoints(m_srcImg,pSPoints);
			if (numSrc == 0)
			{
				continue;
			}

			temp = distance(pMPoints, pSPoints, numModel, numSrc);
			// min
			if (temp < dis)
			{
				dis = temp;
				matchDis[n] = temp;
				matchPoints[n].x = i;
				matchPoints[n].y = j;
			} 	
		}
	}


	m_matchPoint = matchPoints[0];
	m_matchDistance = matchDis[0];

	cvResetImageROI(m_srcImg);
	drawMatch();

	delete []pMPoints;
	delete []pSPoints;
}/* match() */


/*
* �������ƣ�distance
* �������ܣ���ȡĳλ�õ�Hausdorff distanceֵ
* ������ڣ� 
* �����������������������ָ�룬��������� numA, numB
* ���������
* �� �� ֵ�� ��λ�õ�Hausdorff distanceֵ
* ����˵���� ֧��ͼƬROI
*/
double CHausdorff::distance(CvPoint Apoints[], CvPoint Bpoints[], int numA, int numB)
{
	double hAB;
	double hBA;
	hAB = computeDirDistance(Apoints, Bpoints, numA, numB, m_frontRange);
	hBA = computeDirDistance(Bpoints, Apoints, numB, numA, m_backRange);

	// H(A, B) = max(h(A, B), h(B,A))
	return std::max(hAB, hBA);	
}/*distance()*/

/*
* �������ƣ�computeDirDistance
* �������ܣ����㵥�� Hausdorff distance ֵ
* ������ڣ� 
* �����������������������ָ�� Apoints, Bpoints, ���������numA, int numB, ����������� Range
* ���������
* �� �� ֵ�� ���� Hausdorff distance ֵ
* ����˵����
*/
double CHausdorff::computeDirDistance(CvPoint Apoints[], CvPoint Bpoints[], int numA, int numB, double Range)
{
	double *disA = new double[numA];

	double temp;
	double aB = MAX_DISTANCE;
	for (int i = 0; i < numA; i++)
	{
		for (int j = 0; j < numB; j++)
		{
			temp = (Apoints[i].x - Bpoints[j].x) * (Apoints[i].x - Bpoints[j].x)
					+ (Apoints[i].y - Bpoints[j].y) * (Apoints[i].y - Bpoints[j].y);
			//����ʹ�ò��
			//temp = m_disTable[abs(Apoints[i].x - Bpoints[j].x)][abs(Apoints[i].y - Bpoints[j].y)];

			// while b in B, get min|| a - b ||
			aB = std::min(temp,aB);
		}
		disA[i] = aB;
		// ע�������������
		aB = MAX_DISTANCE;
	}

	sort(disA, numA);
	double dis;
	// hausdorff ����Ľ��棺����hausdorff����
	dis = disA[(int)(numA * Range) - 1];
	// Ҳ���Գ���ƽ������:meanDistance()��ʹ��meanDistance()����Ҫ�Ծ�����������
	// dis = meanDistance(disA, numA);

	delete []disA;
	return dis;
}/*computeDirDistance()*/

/*
* �������ƣ�drawMatch()
* �������ܣ��ڴ�ƥ��ͼ�л���ƥ��������
* ������ڣ� 
* ��������� imgָ��Ĭ��ָ��m_srcImg�����Խ���ƥ��ͼ���jpeg��ʽͼ��ָ�봫�룬��ʹ�������ֱ��
* ���������
* �� �� ֵ�� void
* ����˵���� 
*/
void CHausdorff::drawMatch(IplImage *img)
{
	CvPoint pt;
	pt.x = m_matchPoint.x + m_modelImg->width;
	pt.y = m_matchPoint.y + m_modelImg->height;

	if (!img)
	{
		img = m_srcImg;
	}

	cvRectangle(img, m_matchPoint, pt, cvScalar(0,0,255));
	cvShowImage("resultMatch", img);
}

/*
* �������ƣ�sort
* �������ܣ������������������
* ������ڣ� 
* �������������������ָ�뼰�������
* ���������
* �� �� ֵ�� void
* ����˵���� 
*/
void CHausdorff::sort(double a[],int n) //����
{ 
	double temp;
	for(int i=0;i<n;i++) 
		for(int j=i+1;j<n;j++) 
		{ 
			if(a[i]>a[j]) 
			{  
				temp=a[i]; 
				a[i]=a[j]; 
				a[j]=temp; 
			} 
		} 
}

/*
* �������ƣ�meanDistance
* �������ܣ��Ծ���������ƽ��ֵ
* ������ڣ� 
* �����������������ָ�뼰�������
* ���������
* �� �� ֵ�� ���������ƽ��ֵ
* ����˵���� 
*/
double CHausdorff::meanDistance(double a[], int n)
{
	double sum = 0;

	for (int i = 0; i < n; i++)
	{
		sum += a[i];
	}
	sum /= n;
	return sum;
}

/*
* �������ƣ�createTable
* �������ܣ���������任��
* ������ڣ� 
* ���������NULL
* �������������任���ָ��
* �� �� ֵ�� void
* ����˵���� 
*/
void CHausdorff::createTable()
{
	int wm, hm, r;
	wm = m_modelImg->width;
	hm = m_modelImg->height;

	r = std::max(wm,hm);

	m_disTable = new double*[r];
	for (int i = 0; i < r; i++)
	{
		m_disTable[i] = new double[r];
	}

	for (int j = 0; j < r; j++)
	{
		for(int i = 0; i < r; i++)
		{
			m_disTable[j][i] = i*i + j*j;
			m_disTable[i][j] = m_disTable[j][i];
		}
	}
}

/*
* �������ƣ�freeTable
* �������ܣ��ͷž���任���ڴ�
* ������ڣ� 
* ���������
* ���������
* �� �� ֵ�� void
* ����˵���� 
*/
void CHausdorff::freeTable()
{
	int wm, hm, r;
	wm = m_modelImg->width;
	hm = m_modelImg->height;

	r = std::max(wm,hm);

	for (int i = 0; i < r; i++)
	{
		delete []m_disTable[i];
	}
	delete []m_disTable;
}