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
#import "CardFinder.hpp"

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
    cv::Mat org;
    cv::Mat edges;

    // Convert UIImage to cv::Mat
    UIImageToMat(image, org);
    
    // Call the C++ helper function
    edges = detectEdgesInRGBImage(org);
    

    // Convert cv::Mat back to UIImage
    UIImage *resultImage = MatToUIImage(edges);

    return resultImage;
}



+ (UIImage *)blur:(UIImage *)image radius:(double)radius {
    cv::Mat mat;
    UIImageToMat(image, mat);
    UIImage *Image = MatToUIImage(mat);
    return Image;
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
