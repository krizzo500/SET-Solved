//
//  OpenCVWrapper.m
//  SET Solved
//
//  Created by Timothy Poulsen on 11/27/18.
//  Copyright © 2018 Timothy Poulsen. All rights reserved.
//

#import <opencv2/opencv.hpp>
#import "OpenCVWrapper.h"
#import <opencv2/imgcodecs/ios.h>
#import <UIKit/UIKit.h>

using namespace cv;

@implementation OpenCVWrapper

//typedef NS_ENUM(NSUInteger, )
//COLOR_BGR2GRAY     = 6, //!< convert between RGB/BGR and grayscale, @ref color_convert_rgb_gray "color conversions"
//COLOR_RGB2GRAY     = 7,


+ (NSString *)openCVVersionString {
    return [NSString stringWithFormat:@"OpenCV Version %s",  CV_VERSION];
}

+ (UIImage *)convertToGrayscale:(UIImage *)image {
    cv::Mat mat;
    UIImageToMat(image, mat);
    cv::Mat gray;
    cv::cvtColor(mat, gray, cv::COLOR_BGR2GRAY);
    UIImage *grayscale = MatToUIImage(gray);
    return grayscale;
}

+ (UIImage *)detectEdgesInRGBImage:(UIImage *)image {
//    cv::Mat mat;
//    UIImageToMat(image, mat);
//    cv::Mat gray;
//    cv::cvtColor(mat, gray, cv::COLOR_BGR2GRAY);
////    cv::Laplacian(gray, gray, gray.depth());
//    cv::Sobel(gray, gray, gray.depth(), 1, 0);
//    UIImage *grayscale = MatToUIImage(gray);
//    return grayscale;
    Mat org;
    mat gray;
    Mat blur;
    Mat thresh;
    
}

+ (UIImage *)blur:(UIImage *)image radius:(double)radius {
    cv::Mat mat;
    UIImageToMat(image, mat);
    cv::GaussianBlur(mat, mat, cv::Size(NULL, NULL), radius);
    UIImage *blurredImage = MatToUIImage(mat);
    return blurredImage;
}

+ (UIImage *)getChannel:(UIImage *)image channel:(NSString *)channel {
    cv::Mat mat;
    UIImageToMat(image, mat);
    cv::Mat baseImage = cv::Mat::zeros(mat.size(), CV_8UC3);
    // UIImage is RGB, so our default here is blue
    int ch = 2;
    if ([channel isEqual: @"r"] || [channel isEqual: @"R"]) {
        ch = 0;
    } else if ([channel isEqual: @"g"] || [channel isEqual: @"G"]) {
        ch = 1;
    }
    int from_to[] = { ch,ch };
    cv::mixChannels(&mat, 1, &baseImage, 1, from_to, 1);
    UIImage *retImage = MatToUIImage(baseImage);
    return retImage;
}

@end
