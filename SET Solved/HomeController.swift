//
//  TakePhotoController.swift
//  SET Solved
//
//  Created by Timothy Poulsen on 11/27/18.
//  Copyright © 2018 Timothy Poulsen. All rights reserved.
//

import UIKit
import MobileCoreServices

class HomeController: UIViewController, UIImagePickerControllerDelegate, UINavigationControllerDelegate {

    override func viewDidLoad() {
        super.viewDidLoad()
        print("\(OpenCVWrapper.openCVVersionString())")
    }

    @IBOutlet weak var photoButton: UIButton! {
        didSet {
            photoButton.isEnabled = UIImagePickerController.isSourceTypeAvailable(.camera)
        }
    }
    
    @IBAction func Practice(_ sender: Any) {
        print("hello lets practice")
    }
    
    @IBAction func takePhoto(_ sender: UIButton) {
        let picker = UIImagePickerController()
        picker.sourceType = .camera
        picker.mediaTypes = [kUTTypeImage as String]
        picker.allowsEditing = false
        picker.delegate = self
        present(picker, animated: true)
    }
    
    // user hit cancel
    func imagePickerControllerDidCancel(_ picker: UIImagePickerController) {
        picker.presentingViewController?.dismiss(animated: true)
    }
    
    // user took a photo
    func imagePickerController(_ picker: UIImagePickerController, didFinishPickingMediaWithInfo info: [UIImagePickerController.InfoKey : Any]) {
        if let image = (info[UIImagePickerController.InfoKey.originalImage] as? UIImage?) {
            picker.presentingViewController?.dismiss(animated: true)

            let storyboard = UIStoryboard(name: "Main", bundle: nil)
            let pic = storyboard.instantiateViewController(withIdentifier: "ProcessImageController") as! ProcessImageController
            pic.source_image = image
            self.navigationController?.pushViewController(pic, animated: true)
        }
    }

}

