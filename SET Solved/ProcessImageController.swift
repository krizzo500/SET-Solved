//
//  ProcessImageController.swift
//  SET Solved
//
//  Created by Timothy Poulsen on 11/29/18.
//  Copyright © 2018 Timothy Poulsen. All rights reserved.
//

import UIKit
import CoreImage

class ProcessImageController: UIViewController {

    public var source_image: UIImage? = nil

    @IBOutlet weak var imgView: UIImageView!

    @IBAction func processImage(_ seg_ctrl: UISegmentedControl) {
        if let img = self.source_image {
            switch seg_ctrl.selectedSegmentIndex {
            case 0: // b&w
                 showImage(image: OpenCVWrapper.convert(toGrayscale: img.normalized!))
            case 1: // edges
                showImage(image: OpenCVWrapper.detectEdges(inRGBImage: img.normalized!))
            case 2: // blur
                // the OpenCV blur is significantly slower (like 5x slower)
                //showImage(image: self.blurImage(with: img.normalized!, radius: 20.0))
                showImage(image: OpenCVWrapper.blur(img.normalized!, radius: 20.0))
            case 3: // red channel
                showImage(image: OpenCVWrapper.getChannel(img.normalized!, channel: "R"))
            default:
                showImage(image: img)
            }
        }
    }
    
    func showImage(image: UIImage) {
        if let resized = image.resizeTo(width: imgView?.frame.width) {
            imgView.image = resized
        }
    }

    func blurImage(with sourceImage: UIImage, radius: Float) -> UIImage {
        let context = CIContext(options: nil)
        let inputImage = CIImage(cgImage: sourceImage.cgImage! as CGImage)
        let filter = CIFilter(name: "CIGaussianBlur")
        filter?.setValue(inputImage, forKey: kCIInputImageKey)
        filter?.setValue(radius, forKey: "inputRadius")
        let result = filter?.value(forKey: kCIOutputImageKey) as? CIImage
        let cgImage = context.createCGImage(result ?? CIImage(), from: inputImage.extent)
        let retVal = UIImage(cgImage: cgImage!)
        return retVal
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        if let src_img = self.source_image {
            showImage(image: src_img)
        }
    }
    

}
