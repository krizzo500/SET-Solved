//
//  OpenCVWrapper.h
//  SET Solved
//
//  Created by Timothy Poulsen on 11/27/18.
//  Copyright © 2018 Timothy Poulsen. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface OpenCVWrapper : NSObject

+ (NSString *)openCVVersionString;
+ (UIImage *)convertToGrayscale:(UIImage *)image;
+ (UIImage *)detectEdgesInRGBImage:(UIImage *)image;
+ (UIImage *)blur:(UIImage *)image radius:(double)radius;
+ (UIImage *)getChannel:(UIImage *)image channel:(NSString *)channel;

@end

NS_ASSUME_NONNULL_END
