/*
 * 亚像素级别的角点检测
 * 亚像素的角点检测主要用于测量而非提取特征
 * 
 * 之前的角点检测算法只能获取像素的整数坐标，现在想要获取浮点数坐标
 * 
 * 本文件直接在shi tomasi的代码上进行增加内容
*/

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

Mat img, gray;
int max_corner_num = 30;
int max_trackbar_num = 500;
RNG rng(123456);

void on_good_feature_to_track(int, void *);

int main()
{
    img = imread("test.jpg", IMREAD_COLOR);
    cvtColor(img, gray, COLOR_BGR2GRAY);
    namedWindow("dst", WINDOW_FREERATIO);
    createTrackbar("max_num", "dst", &max_corner_num, max_trackbar_num, on_good_feature_to_track, 0);

    imshow("dst", img);
    on_good_feature_to_track(0, 0);
    waitKey(-1);
}

void on_good_feature_to_track(int, void *)
{
    max_corner_num = max_corner_num > 1 ? max_corner_num : 1;

    //参数准备
    vector<Point2f> corners;
    double quality_level = 0.01; //角点检测能接受的最小特征值
    double min_distance = 10;    //角点直接最小距离
    int blocksize = 3;           //计算导数自相关矩阵时指定的邻域范围
    double k = 0.04;
    Mat copy = img.clone();

    goodFeaturesToTrack(gray, corners, max_corner_num, quality_level, min_distance, Mat(), blocksize, false, k);
    /*
     * 确定图像强角点函数，是harris角点检测的改进版本
     * 第一个参数是输入图像，单通道8位或浮点
     * 第二个参数是输出的角点向量（vector数组）
     * 第三个参数是角点的最多数目
     * 第四个参数是角点检测接受的最小特征值（一般取0.1或0.01）
     * 第五个参数是角点之间最小距离
     * 第六个参数是掩模mask，单通道同尺寸
     * 第七个参数是导数自相关矩阵的邻域范围
     * 第八个参数是是否使用harris检测
     * 第九个参数是设置自相关矩阵行列式的权重，有默认值0.04
     * 
     * 此函数还可以初始化基于点的对象跟踪操作
    */

    printf("角点检测数目 : %d \n", corners.size());

    //绘制角点
    int r = 4;
    for (auto p : corners)
        //circle(copy, p, r, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), -1, LINE_AA, 0);
        circle(copy, p, r, Scalar(0, 0, 255), -1, LINE_AA, 0);

    //*********以下是增加的代码
    //亚像素检测参数准备
    Size win_size = Size(5, 5), zero_zone = Size(-1, -1);
    TermCriteria criteria = TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 40, 0.001);

    cornerSubPix(gray, corners, win_size, zero_zone, criteria);

    for (int i = 0; i < corners.size(); i++)
    {
        printf("精确像素位置 : %f, %f\n", corners[i].x, corners[i].y);
    }

    imshow("dst", copy);
}