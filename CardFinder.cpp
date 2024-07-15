//
//  CardFinder.cpp
//  SET Solved
//
//  Created by Karl Rizzo on 7/14/24.
//  Copyright © 2024 Timothy Poulsen. All rights reserved.
//

#include "CardFinder.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int THRESHOLD_MINIMUM = 150


bool compareContourAreas(const vector<Point>& contour1, const vector<Point>& contour2) {
    double i = contourArea(contour1);
    double j = contourArea(contour2);
    return (i > j);
}


/*vector<vector<Point>>*/void removeContourOutliers(vector<vector<Point>> contours){
    int median_area = 0;
    
}



void findCards(const Mat& image) {
    Mat gray;
    Mat blur;
    Mat thresh;
    cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    GaussianBlur(gray, blur, cv::Size(1, 1), 1000);
    threshold(blur, thresh, THRESHOLD_MINIMUM, 255, 0);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(thresh, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    //sort contours
    sort(contours.begin(), contours.end(), compareContourAreas);
    //contours = removeContourOutliers(contours);
    for(int i = 0; i < contours.size(); i++){
        cout << contourArea(contours[i]) << endl;
    }
    
}


Mat detectEdgesInRGBImage(const Mat& image) {
    findCards(image);
    Mat gray;
    Mat blur;
    Mat thresh;

    cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    GaussianBlur(gray, blur, cv::Size(1, 1), 1000);
    threshold(blur, thresh, 180, 255, 0);
    
    return thresh;
}










