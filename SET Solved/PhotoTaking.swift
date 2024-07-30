import UIKit
import MobileCoreServices

protocol PhotoTaking: UIImagePickerControllerDelegate & UINavigationControllerDelegate {
    func presentImagePicker()
}

extension PhotoTaking where Self: UIViewController {
    func presentImagePicker() {
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
