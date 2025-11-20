#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/video/tracking.hpp"
#include <iostream>

using namespace cv;
using namespace std;
//计算轮廓中心
void findContourCenter(const vector<Point>& contour, Point2f& center) {
    Moments M = moments(contour);
    double cX_double = M.m10 / M.m00;
    double cY_double = M.m01 / M.m00;
        cX_double = M.m10 / M.m00;
        cY_double = M.m01 / M.m00;
    center = Point2f(cX_double, cY_double);
}
//过滤红色
void filterRedColor(const Mat& hsv, Mat& output_mask ) {
    Mat mask1, mask2;
    cv::inRange(hsv, Scalar(0, 100, 100), Scalar(10, 255, 225), mask1);
    cv::inRange(hsv, Scalar(160, 100, 100), Scalar(179, 255, 235), mask2);//奇怪的参数，只有这样才能不计算下面桌子的反光（

    cv::add(mask1, mask2, output_mask);
}

int main()
{   
    string name = "提高视频.avi";
    VideoCapture vid(name, CAP_ANY);
    Mat frame;
    Point2f last_center(-1, -1);
    double last_ax = 0.0;
    double last_ay = 0.0;
    bool f = true;
    while(true){
        vid >> frame;
        //转换颜色空间
        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);
        //过滤红色
        filterRedColor(hsv,hsv);
        Mat GAUSSfixed;
        Mat fixed;  
        vector<vector<Point>> contours;
        //降噪
        GaussianBlur(hsv, GAUSSfixed, Size(5,5) , 9);
        //边缘检测
        Canny(hsv, fixed, 30, 200);//奇怪的参数
        //优化
        morphologyEx(fixed , fixed, MORPH_CLOSE, getStructuringElement(MORPH_OPEN, Size(5,5)));
        morphologyEx(fixed , fixed, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(5,5)));
        //轮廓检测
        findContours(fixed, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        for(auto contour : contours){
            if(contourArea(contour) > 300){
                Point2f center;
                double ax,ay;
                //中心
                findContourCenter(contour, center);
                //速度计算
                ax = (last_center.x - center.x);
                ay = (last_center.y - center.y);
                //预测
                Point2f next_point;
                next_point.x = center.x + (-2) * (ax + last_ax) / 2;
                next_point.y = center.y + (-2) * (ay + last_ay) / 2;
                //文本
                string po = "point: " + to_string(center.x) + "," + to_string(center.y);
                string v = "v: " + to_string(ax) + "," + to_string(ay);
                string p = "predicted point: " + to_string(next_point.x) + "," + to_string(next_point.y);
                //画圈
                circle(frame, center, 9, Scalar(0,255,0), -1);
                circle(frame, next_point, 9, Scalar(255,0,0), -1);
                // 信息
                putText(frame, "Red Kuang", Point(center.x + 15, center.y -10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0), 2);
                putText(frame, "Predicted Position", Point(next_point.x + 20, next_point.y), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,0,0), 2);
                putText(frame, po, Point(0,20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0), 2);
                putText(frame, v, Point(0,40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0), 2);
                putText(frame, p,Point(0,60), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0), 2);
                //转移
                last_center = center;
                last_ax = ax;
                last_ay = ay;
            }
        }
        //这框真帅吧
        imshow("This kuang is really cool, isn't it?", frame);
        if(waitKey(33) == 'q' ) break;
    }
    vid.release();
    return 0;
}


