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

int THRESHOLD_MINIMUM = 180;
int SHAPE_THRESHOLD_MINIMUM = 200;
double CONTOUR_AREA_TOLERANCE = 2.0;


bool compareContourAreas(const vector<Point>& contour1, const vector<Point>& contour2) {
    double i = contourArea(contour1);
    double j = contourArea(contour2);
    return (i > j);
}

vector<vector<Point>> removeContourOutliers(vector<vector<Point>> contours, string type){
    sort(contours.begin(), contours.end(), compareContourAreas);
    int median_area;
    if(type == "SHAPES"){
        median_area = 10000;
    }
    else if(type == "CARDS"){
        int med = std::min(static_cast<int>(contours.size())-1, 7);
        median_area = contourArea(contours[med]); //7 is somewhat arbitrary here, maybe add more sophisticated way to find median card area
    }
    int start = 0;
    while(start < contours.size() && contourArea(contours[start]) > median_area*CONTOUR_AREA_TOLERANCE){
        start++;
    }
    int end = start;
    while(end < contours.size() && contourArea(contours[end]) > median_area*(1/CONTOUR_AREA_TOLERANCE)){
        end++;
    }
    return vector<vector<Point>>(contours.begin() + start, contours.begin() + end);
}

float distance(const Point2f& point1, const Point2f& point2) {
    return sqrt(pow(point2.x - point1.x, 2) + pow(point2.y - point1.y, 2));
}

vector<Point2f> rectify(vector<Point2f>& points) {
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
    if (distance(rect[0], rect[1]) > distance(rect[1], rect[2])){
        vector<Point2f> rect2(4);
        rect2[0] = rect[3];
        rect2[1] = rect[0];
        rect2[2] = rect[1];
        rect2[3] = rect[2];
        return rect2;
    }
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
    contours = removeContourOutliers(contours, "CARDS");
    Scalar color(0, 0, 0);
    for(int i = 0; i < contours.size(); i++){ //make upper bound min of contours.size() and 15(max num cards)?
        vector<Point2f> curve_approx;
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
vector<tuple<vector<Point2f>, Mat>>findCards(const Mat& image, int width = 450, int height = 300) {
    vector<tuple<vector<Point2f>, Mat>> cards;
    Mat gray;
    Mat blur;
    Mat thresh;
    cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    GaussianBlur(gray, blur, cv::Size(1, 1), 1000);
    threshold(blur, thresh, THRESHOLD_MINIMUM, 255, 0);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(thresh, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    contours = removeContourOutliers(contours, "CARDS");
    for(int i = 0; i < contours.size(); i++){
        vector<Point2f> curve_approx;
        vector<Point> card = contours[i];
        double perimeter = arcLength(card, true);
        double precision = 0.1*perimeter;
        approxPolyDP(card, curve_approx, precision, true);
        if (curve_approx.size() == 4){ //only quadralaterals
            vector<Point2f> corners = rectify(curve_approx);
//            vector<Point2f> src_points;
//            for (const auto& point : rect_curve_approx) {
//                src_points.push_back(Point2f(point.x, point.y));
//            }
            vector<Point2f> dst_points = {Point2f(0, 0), Point2f(width - 1, 0), Point2f(width - 1, height - 1), Point2f(0, height - 1)};
            Mat transform = getPerspectiveTransform(corners,dst_points);
            Mat corrected_card;
            cv::warpPerspective(image, corrected_card, transform, Size(width,height));
            tuple<vector<Point2f>, Mat> card = make_tuple(corners, corrected_card);
            cards.push_back(card);
        }
    }
    return cards;
}


vector<Mat> findShapes(const Mat& card_image, int width = 100, int height = 200) {
    vector<Mat> corrected_shapes;
    Mat gray;
    Mat blur;
    Mat thresh;
    Mat thresh_invert;
    cvtColor(card_image, gray, cv::COLOR_BGR2GRAY);
    GaussianBlur(gray, blur, cv::Size(1, 1), 1000);
    threshold(blur, thresh, THRESHOLD_MINIMUM, 255, 0);
    bitwise_not(thresh, thresh_invert);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(thresh_invert, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    contours = removeContourOutliers(contours, "SHAPES");
    int range = std::min(static_cast<int>(contours.size()), 3);
    for(int i = 0; i < range; i++){
        Rect rect = boundingRect(contours[i]);
        std::vector<cv::Point2f> corners;
        corners.push_back(cv::Point2f(rect.x, rect.y));
        corners.push_back(cv::Point2f(rect.x + rect.width, rect.y));
        corners.push_back(cv::Point2f(rect.x, rect.y + rect.height));
        corners.push_back(cv::Point2f(rect.x + rect.width, rect.y + rect.height));
        vector<Point2f> rectified_corners = rectify(corners);
        vector<Point2f> dst_points = {Point2f(0, 0), Point2f(width - 1, 0), Point2f(width - 1, height - 1), Point2f(0, height - 1)};
        Mat transform = getPerspectiveTransform(rectified_corners,dst_points);
        Mat corrected_shape;
        cv::warpPerspective(card_image, corrected_shape, transform, Size(width,height));
        corrected_shapes.push_back(corrected_shape); //should this be a ref to corrected_shape?
    }
    return corrected_shapes;
}

vector<Point> getShapeContour(const Mat& shape_image){
    Mat gray;
    Mat blur;
    Mat thresh;
    Mat thresh_invert;
    cvtColor(shape_image, gray, cv::COLOR_BGR2GRAY); //error: shape image may be coming in empty at times
    GaussianBlur(gray, blur, cv::Size(1, 1), 1000);
    threshold(blur, thresh, THRESHOLD_MINIMUM, 255, 0);
    bitwise_not(thresh, thresh_invert);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(thresh_invert, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    contours = removeContourOutliers(contours, "SHAPES");
    return contours[0];
}

int getShape(const Mat& shape_image){
    vector<Point> contour = getShapeContour(shape_image);
    float area = contourArea(contour);
    if (area > 8000 && area <= 12400){
        return 1; //diamond
    }
    else if(area > 12400 && area <= 15600){
        return 2; //squiggle
    }
    else if(area > 15600 && area < 20000){
        return 3; //oval
    }
    return -1;
}

int getColor(const Mat& shape_image){
    vector<Point> contour = getShapeContour(shape_image);
    int mask_width = 3;
    Mat mask = Mat::zeros(shape_image.size(), CV_8UC1);
    drawContours(mask, vector<vector<Point>>{contour}, -1, Scalar(255), -1); // Fill the contour
    Mat erodedMask;
    erode(mask, erodedMask, Mat(), Point(-1, -1), mask_width); // Erode the mask by x pixels to get the region within x distance
    mask = mask - erodedMask; // Subtract the eroded mask from the original mask to get the ring-like region
    Scalar averageColor = mean(shape_image, mask);
    Mat rgbMat(1, 1, CV_8UC3, averageColor);
    Mat hsvMat;
    cvtColor(rgbMat, hsvMat, COLOR_BGR2HSV);
    Vec3b hsvColor = hsvMat.at<Vec3b>(0, 0);

    int hue = hsvColor[0];
    int saturation = hsvColor[1];
    int value = hsvColor[2];
    //purple - 2??? does it loop?,175, 175, 169, 167, 168, 171
    //green - 45, 56, 54, 54,
    //red - 127, 124, 122, 122, 123
    
    //p - 147, 155, 144
    //g - 50, 48, 51
    //r - 118, 118, 119
    
    if (84 <= hue && hue <= 132){
        return 1; //red
    }
    else if (hue < 84){
        return 2; //green
    }
    else if (hue > 132){
        return 3; //purple
    }
    return -1;
    
}

int getFill(const Mat& shape_image){
    Mat gray;
    Mat blur;
    Mat thresh;
    cvtColor(shape_image, gray, cv::COLOR_BGR2GRAY);
    GaussianBlur(gray, blur, cv::Size(1, 1), 1000);
    threshold(blur, thresh, THRESHOLD_MINIMUM, 255, 0);
    
    Rect roi(50, 100, 20, 20);
    Mat imageRoi = thresh(roi);
    Scalar average_intensity = mean(imageRoi);
    int intensity = average_intensity[0];
    if (intensity > 240){
        return 1; // empty
    }
    else if (intensity >= 15 && intensity <= 240){
        return 2; // shaded
    }
    else if (intensity < 15){
        return 3;// filled
    }
    return -1;
}

vector<tuple<vector<Point2f>, vector<int>>> identifyCards(const Mat& image){
    vector<tuple<vector<Point2f>, vector<int>>> card_types;
    vector<tuple<vector<Point2f>, Mat>> card_images = findCards(image);
    cout << "found " << card_images.size() << " cards" << endl;
    for(int i = 0; i < card_images.size(); i++){
        vector<Mat> shapes = findShapes(get<1>(card_images[i]));
        int number = shapes.size();
        int shape;
        int color;
        int fill;
        if (number > 0){
            shape = getShape(shapes[0]);
            color = getColor(shapes[0]);
            fill = getFill(shapes[0]);
        }
        else{
            cout << "no shapes found on card" << endl;
        }
        string number_str;
        string shape_str;
        string color_str;
        string fill_str;
        if (number == 1){
            number_str = "one";
        }
        if (number == 2){
            number_str = "two";
        }
        if (number == 3){
            number_str = "three";
        }
        if (shape == 1){
            shape_str = "diamond";
        }
        if (shape == 2){
            shape_str = "squiggle";
        }
        if (shape == 3){
            shape_str = "oval";
        }
        if (color == 1){
            color_str = "red";
        }
        if (color == 2){
            color_str = "green";
        }
        if (color == 3){
            color_str = "purple";
        }
        if (fill == 1){
            fill_str = "empty";
        }
        if (fill == 2){
            fill_str = "shaded";
        }
        if (fill == 3){
            fill_str = "solid";
        }
        cout << "found card with attributes " << number_str << " " << shape_str << " " << color_str << " " << fill_str << endl;
        vector<int> attributes = {number, shape, color, fill};
        vector<Point2f> corners = get<0>(card_images[i]);
        tuple<vector<Point2f>, vector<int>> card_type = make_tuple(corners, attributes);
        card_types.push_back(card_type);
    }
    return card_types;
}

bool isSet(vector<int> card1, vector<int> card2, vector<int> card3){
    
    for(int i = 0; i < 4; ++i){
        if(card1[i] == 0 || card2[i] == 0 || card3[i] == 0){
            return false;
        }
        if (!((card1[i] == card2[i] && card2[i] == card3[i]) ||
            (card1[i] != card2[i] && card2[i] != card3[i] && card1[i] != card3[i]))){
            return false;
        }
    }
    return true;
}

vector<Point2f> scaleCorners(const vector<Point2f> points, double distance){
    if (distance == 0){
        return points;
    }
    double center_x = 0.0, center_y = 0.0;
    for (const Point2f& point : points) {
        center_x += point.x;
        center_y += point.y;
    }
    center_x /= points.size();
    center_y /= points.size();
    vector<Point2f> shifted_points;
    for (const Point2f& point : points) {
        double vector_x = point.x - center_x;
        double vector_y = point.y - center_y;
        double vector_length = sqrt(vector_x * vector_x + vector_y * vector_y);
        double unit_vector_x = vector_x / vector_length;
        double unit_vector_y = vector_y / vector_length;
        double new_point_x = point.x + unit_vector_x * distance;
        double new_point_y = point.y + unit_vector_y * distance;
        shifted_points.emplace_back(new_point_x, new_point_y);
    }

    return shifted_points;

}

vector<Scalar> getColors(){
    return{
        cv::Scalar(0, 0, 255, 255),      // Red
        cv::Scalar(0, 255, 0, 255),      // Green
        cv::Scalar(255, 0, 0, 255),      // Blue
        cv::Scalar(0, 255, 255, 255),    // Yellow
        cv::Scalar(255, 0, 255, 255),    // Magenta
        cv::Scalar(255, 255, 0, 255),    // Cyan
        cv::Scalar(0, 128, 255, 255),    // Orange
        cv::Scalar(255, 128, 0, 255),    // Light Blue
        cv::Scalar(128, 0, 255, 255),    // Violet
        cv::Scalar(0, 255, 128, 255),    // Light Green
        cv::Scalar(128, 255, 0, 255),    // Lime
        cv::Scalar(255, 0, 128, 255),    // Pink
        cv::Scalar(128, 128, 128, 255),  // Gray
        cv::Scalar(0, 0, 128, 255),      // Dark Red
        cv::Scalar(0, 128, 0, 255),      // Dark Green
        cv::Scalar(128, 0, 0, 255),      // Dark Blue
        cv::Scalar(0, 128, 128, 255),    // Teal
        cv::Scalar(128, 128, 0, 255),    // Olive
        cv::Scalar(128, 0, 128, 255),    // Purple
        cv::Scalar(192, 192, 192, 255)   // Silver
        
    };
}


Mat findSets(const Mat& image, vector<tuple<vector<Point2f>, vector<int>>> cards){
    unordered_map<int, vector<int>> card_in_sets;
    Mat retImage = image.clone();
    int n = cards.size();
    int set_num = 0;
    for(int i = 0; i < n; ++i){
        for(int j = i+1; j < n; ++j){
            for(int k = j+1; k < n; ++k){
                
                if (isSet(get<1>(cards[i]), get<1>(cards[j]), get<1>(cards[k]))){
                    cout << "found a set" << endl;
                    ++set_num;
                    card_in_sets[i].push_back(set_num);
                    card_in_sets[j].push_back(set_num);
                    card_in_sets[k].push_back(set_num);
                }
            }
        }
    }
    vector<Scalar> colors = getColors();
    for(int i = 0; i < n; ++i){
        vector<Point2f> corners = get<0>(cards[i]);
        int num_boxes = 0;
        for(int set : card_in_sets[i]){
            cout << set << endl;
            vector<Point2f> scaled_corners = scaleCorners(corners, num_boxes*35);
            Scalar color = colors[(set - 1) % colors.size()];
            line(retImage, scaled_corners[0], scaled_corners[1], color, 30);
            line(retImage, scaled_corners[1], scaled_corners[2], color, 30);
            line(retImage, scaled_corners[2], scaled_corners[3], color, 30);
            line(retImage, scaled_corners[3], scaled_corners[0], color, 30);
            ++num_boxes;
        }
    }
    return retImage;
}

Mat detectEdgesInRGBImage(const Mat& image) {
    Mat retImage = findSets(image, identifyCards(image));
////    
    return retImage;
    
    Mat gray;
    Mat blur;
    Mat thresh;
    cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    GaussianBlur(gray, blur, cv::Size(1, 1), 1000);
    threshold(blur, thresh, THRESHOLD_MINIMUM, 255, 0);
    return thresh;

//    vector<tuple<vector<Point2f>, Mat>> cards = findCards(image);
//    vector<Mat> shapes = findShapes(get<1>(cards[0]));
//    vector<tuple<vector<Point2f>, vector<int>>> card_types = identifyCards(image);
//    vector<int> attributes = get<1>(card_types[0]);
//    cout << "number: " << attributes[0] << " shape: " << attributes[1]
//    << " color: " << attributes[2] << " fill: " << attributes[3] << endl;
//    getColor(shapes[0]);
//    Mat gray;
//    Mat blur;
//    Mat thresh;
//    cvtColor(shapes[0], gray, cv::COLOR_BGR2GRAY);
//    GaussianBlur(gray, blur, cv::Size(1, 1), 1000);
//    threshold(blur, thresh, THRESHOLD_MINIMUM, 255, 0);
//    return shapes[0];
//    return get<1>(cards[0]);
}









