#include <iostream>
#include <string>
#include <cstdlib>
#include <set>
#include <vector>
#include <algorithm>
// #include <cv2
using namespace std;

int ColorDetect(int color, const Mat &inputImage)
{
    bool a = true;
    int point_x;
    int iLowH, iHighH;
    int iLowS = 43, iHighS = 255, iLowV = 46, iHighV = 255;

    switch(color)
    {
    case RED:
        iLowH = 0;
        iHighH = 10; //10
        break;
    case GREEN:
        iLowH = 33; //35
        iHighH = 80; //77
        break;
    case BLUE:
        iLowH = 100; //100
        iHighH = 134;
        break;
    }

    //Mat img = imread("color.jpg",1);
    Mat img, imgHSV;
    inputImage.copyTo(img);

    cvtColor(img, imgHSV, COLOR_BGR2HSV);//转为HSV

    //imwrite("hsv.jpg",imgHSV);

    Mat imgThresholded;

    inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

    //开操作 (去除一些噪点)  如果二值化后图片干扰部分依然很多，增大下面的size
    Mat element = getStructuringElement(MORPH_RECT, Size(40, 40));
    morphologyEx(imgThresholded, imgThresholded, MORPH_OPEN, element);

    //闭操作 (连接一些连通域)
    morphologyEx(imgThresholded, imgThresholded, MORPH_CLOSE, element);

    //namedWindow("Thresholded Image",CV_WINDOW_NORMAL);
    //imshow("Thresholded Image", imgThresholded);


    vector<vector <Point> >contours;
    vector<Vec4i>hierarchy;
    findContours(imgThresholded, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);//查找轮廓

    int count = 0;
    Point pt[512];//存储连通区域个数
    Moments moment;//矩
    vector<Point>Center;//创建一个向量保存重心坐标

    for (int i=0;i<contours.size();i++)//读取每一个轮廓求取重心
    {
        Mat temp(contours.at(i));
        Scalar color(255, 255, 255);
        moment = moments(temp, false);

        if (moment.m00 != 0)//除数不能为0
        {
            pt[i].x = cvRound(moment.m10 / moment.m00);//计算重心横坐标
            pt[i].y = cvRound(moment.m01 / moment.m00);//计算重心纵坐标
            point_x = pt[i].x;
        }
        Point p = Point(pt[i].x, pt[i].y);//重心坐标
        circle(img, p, 1, color, 5, 8);//原图画出重心坐标
        count++;//重心点数或者是连通区域数
        Center.push_back(p);//将重心坐标保存到Center向量中
    }

    //cout << "重心点个数：" << Center.size() << endl;
    //cout << "轮廓数量：" << contours.size() << endl;

    //显示染色检测结果，并保存图片
    //imshow("result", inputImage);
    string name = "end"+int2string(color)+".jpg";
    imwrite(name, img);
    int result = (point_x > WIDTH_MIN) && (point_x < WIDTH_MAX) ? color : 0;
    point_x = 0;
    return result;
}