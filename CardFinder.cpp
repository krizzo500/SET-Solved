//
//  CardFinder.cpp
//  SET Solved
//
//  Created by Karl Rizzo on 7/14/24.
//  Copyright © 2024 Timothy Poulsen. All rights reserved.
//

#include "CardFinder.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <filesystem>

using namespace cv;
using namespace std;

int THRESHOLD_MINIMUM = 150;
double CONTOUR_AREA_TOLERANCE = 2.0;


bool compareContourAreas(const vector<Point>& contour1, const vector<Point>& contour2) {
    double i = contourArea(contour1);
    double j = contourArea(contour2);
    return (i > j);
}

vector<vector<Point>> removeContourOutliers(vector<vector<Point>> contours){
    sort(contours.begin(), contours.end(), compareContourAreas);
    int median_area = contourArea(contours[7]); //7 is somewhat arbitrary here, maybe add more sophisticated way to find median card area
    int start = 0;
    while(contourArea(contours[start]) > median_area*CONTOUR_AREA_TOLERANCE){
        start++;
    }
    int end = start;
    while(contourArea(contours[end]) > median_area*(1/CONTOUR_AREA_TOLERANCE)){
        end++;
    }
    return vector<vector<Point>>(contours.begin() + start, contours.begin() + end);
}

vector<Point2f> rectify(vector<Point>& points) {
    // Ensure the points are ordered as top-left, top-right, bottom-right, bottom-left
    vector<Point2f> rect(4);
    vector<int> sum(4), diff(4);
    for (int i = 0; i < 4; ++i) {
        sum[i] = points[i].x + points[i].y;
        diff[i] = points[i].x - points[i].y;
    }
    rect[0] = points[min_element(sum.begin(), sum.end()) - sum.begin()]; // top-left
    rect[1] = points[max_element(diff.begin(), diff.end()) - diff.begin()]; // top-right
    rect[2] = points[max_element(sum.begin(), sum.end()) - sum.begin()]; // bottom-right
    rect[3] = points[min_element(diff.begin(), diff.end()) - diff.begin()]; // bottom-left
    return rect;
}

//returns one image of all cards with corners circled
Mat highlightCards(const Mat& image) {
    Mat gray;
    Mat blur;
    Mat thresh;
    cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    GaussianBlur(gray, blur, cv::Size(1, 1), 1000);
    threshold(blur, thresh, THRESHOLD_MINIMUM, 255, 0);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(thresh, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    contours = removeContourOutliers(contours);
    Scalar color(0, 0, 0);
    for(int i = 0; i < contours.size(); i++){ //make upper bound min of contours.size() and 15(max num cards)?
        vector<Point> curve_approx;
        vector<Point> card = contours[i];
        double perimeter = arcLength(card, true);
        double precision = 0.1*perimeter;
        approxPolyDP(card, curve_approx, precision, true);
        if (curve_approx.size() == 4){ //only quadralaterals
            int k = 0;
            //rectify points in approx so points for all cards are ordered uniformly
            vector<Point2f> rect_curve_approx = rectify(curve_approx);
            for(Point point : rect_curve_approx){
                circle(image, point, 10+k, color, 50);
                k += 15;
            }
        }
    }
    return image;
}

//returns list of cards images with dimentiosn width, height corrected for warp
vector<Mat> findCards(const Mat& image, int width = 450, int height = 300) {
    vector<Mat> corrected_cards;
    Mat gray;
    Mat blur;
    Mat thresh;
    cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    GaussianBlur(gray, blur, cv::Size(1, 1), 1000);
    threshold(blur, thresh, THRESHOLD_MINIMUM, 255, 0);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(thresh, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    contours = removeContourOutliers(contours);
    for(int i = 0; i < contours.size(); i++){
        vector<Point> curve_approx;
        vector<Point> card = contours[i];
        double perimeter = arcLength(card, true);
        double precision = 0.1*perimeter;
        approxPolyDP(card, curve_approx, precision, true);
        if (curve_approx.size() == 4){ //only quadralaterals
            vector<Point2f> rect_curve_approx = rectify(curve_approx);
//            vector<Point2f> src_points;
//            for (const auto& point : rect_curve_approx) {
//                src_points.push_back(Point2f(point.x, point.y));
//            }
            vector<Point2f> dst_points = {Point2f(0, 0), Point2f(width - 1, 0), Point2f(width - 1, height - 1), Point2f(0, height - 1)};
            Mat transform = getPerspectiveTransform(/*src_points*/rect_curve_approx,dst_points);
            Mat corrected_card;
            cv::warpPerspective(image, corrected_card, transform, Size(width,height));
            corrected_cards.push_back(corrected_card); //should this be a ref to corrected_card?
        }
    }
    return corrected_cards;
}


vector<Mat> findShapes(const Mat& image, int width = 100, int height = 200) {
    vector<Mat> corrected_shapes;
    Mat gray;
    Mat blur;
    Mat thresh;
    Mat thresh_invert;
    cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    GaussianBlur(gray, blur, cv::Size(1, 1), 1000);
    threshold(blur, thresh, THRESHOLD_MINIMUM, 255, 0);
    bitwise_not(thresh, thresh_invert);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(thresh_invert, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    sort(contours.begin(), contours.end(), compareContourAreas);
    int range = std::min(static_cast<int>(contours.size()), 3);
    for(int i = 0; i < range; i++){
        RotatedRect rect = minAreaRect(contours[i]);
        vector<Point> corners;
        boxPoints(rect, corners);
        vector<Point2f> rect_corners = rectify(corners);
        vector<Point2f> dst_points = {Point2f(0, 0), Point2f(width - 1, 0), Point2f(width - 1, height - 1), Point2f(0, height - 1)};
        Mat transform = getPerspectiveTransform(rect_corners,dst_points);
        Mat corrected_shape;
        cv::warpPerspective(image, corrected_shape, transform, Size(width,height));
        corrected_shapes.push_back(corrected_shape); //should this be a ref to corrected_shape?
    }
    return corrected_shapes;
}



Mat detectEdgesInRGBImage(const Mat& image) {
    Mat gray;
    Mat blur;
    Mat thresh;

    cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    GaussianBlur(gray, blur, cv::Size(1, 1), 1000);
    threshold(blur, thresh, 180, 255, 0);
    vector<Mat> corrected_cards = findCards(image);
    return highlightCards(image);
}









