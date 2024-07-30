//
//  PracticeViewController.swift
//  SET Solved
//
//  Created by Karl Rizzo on 7/25/24.
//  Copyright © 2024 Timothy Poulsen. All rights reserved.
//

import UIKit

class PracticeViewController: UIViewController {
    
    var cardImages : [String] = []{
        didSet {
            updateLabels()
        }
    }
    
    var isSelected: [UIButton : Bool] = [:]
    var selectedButtons = Set<UIButton>()
    var buttonTypes: [UIButton : [Int]] = [:]
    

    @IBOutlet weak var timerLabel: UILabel!
    
    @IBOutlet weak var cardsRemainingLabel: UILabel!
    
    @IBOutlet weak var setsFoundLabel: UILabel!
    var timer: Timer?
    var seconds: Int = 0
    
    @IBOutlet weak var card1: UIButton!
    @IBOutlet weak var card2: UIButton!
    @IBOutlet weak var card3: UIButton!
    @IBOutlet weak var card4: UIButton!
    @IBOutlet weak var card5: UIButton!
    @IBOutlet weak var card6: UIButton!
    @IBOutlet weak var card7: UIButton!
    @IBOutlet weak var card8: UIButton!
    @IBOutlet weak var card9: UIButton!
    @IBOutlet weak var card10: UIButton!
    @IBOutlet weak var card11: UIButton!
    @IBOutlet weak var card12: UIButton!
    @IBOutlet weak var card13: UIButton!
    @IBOutlet weak var card14: UIButton!
    @IBOutlet weak var card15: UIButton!
        
    override func viewDidLoad() {
        super.viewDidLoad()
        cardImages = ["card1111.png", "card1112.png", "card1113.png", "card1121.png", "card1122.png", "card1123.png", "card1131.png", "card1132.png", "card1133.png", "card1211.png", "card1212.png", "card1213.png", "card1221.png", "card1222.png", "card1223.png", "card1231.png", "card1232.png", "card1233.png", "card1311.png", "card1312.png", "card1313.png", "card1321.png", "card1322.png", "card1323.png", "card1331.png", "card1332.png", "card1333.png", "card2111.png", "card2112.png", "card2113.png", "card2121.png", "card2122.png", "card2123.png", "card2131.png", "card2132.png", "card2133.png", "card2211.png", "card2212.png", "card2213.png", "card2221.png", "card2222.png", "card2223.png", "card2231.png", "card2232.png", "card2233.png", "card2311.png", "card2312.png", "card2313.png", "card2321.png", "card2322.png", "card2323.png", "card2331.png", "card2332.png", "card2333.png", "card3111.png", "card3112.png", "card3113.png", "card3121.png", "card3122.png", "card3123.png", "card3131.png", "card3132.png", "card3133.png", "card3211.png", "card3212.png", "card3213.png", "card3221.png", "card3222.png", "card3223.png", "card3231.png", "card3232.png", "card3233.png", "card3311.png", "card3312.png", "card3313.png", "card3321.png", "card3322.png", "card3323.png", "card3331.png", "card3332.png", "card3333.png"]
        let cards = [card1, card2, card3, card4, card5, card6, card7, card8, card9, card10, card11, card12]
        cards.forEach { card in
           if let card = card {
               isSelected[card] = false
               card.addTarget(self, action: #selector(buttonPressed(_:)), for: .touchUpInside)
           }
        }
        setRandomBackgroundImages()
        startTimer()
        updateLabels()
        play()
        
      
    }
    
    func play(){
    
    }
    
    @objc func buttonPressed(_ sender: UIButton) {
       guard let currentState = isSelected[sender] else { return }
       if currentState {
           // Animate to scale back up to original size
           UIView.animate(withDuration: 0.1, animations: {
               sender.transform = CGAffineTransform.identity
           })
           selectedButtons.remove(sender)
           isSelected[sender] = false
       } else {
           // Animate to scale down
           UIView.animate(withDuration: 0.1, animations: {
               sender.transform = CGAffineTransform(scaleX: 0.8, y: 0.8)
           })
           selectedButtons.insert(sender)
           isSelected[sender] = true
       }
       if selectedButtons.count == 3{
           if isSet(buttons: selectedButtons){
               for button in selectedButtons {
                   let randomIndex = Int(arc4random_uniform(UInt32(cardImages.count)))
                   let randomImageName : String = cardImages[randomIndex]
                   let attributes = [Int(getSubstring(from: randomImageName, start: 4, end:5)),
                                     Int(getSubstring(from: randomImageName, start: 5, end:6)),
                                     Int(getSubstring(from: randomImageName, start: 6, end:7)),
                                     Int(getSubstring(from: randomImageName, start: 7, end:8))]
                   let nonOptionalAttributes: [Int] = attributes.compactMap { $0 }

                   cardImages.remove(at: randomIndex)
                   if let randomImage = UIImage(named: randomImageName) {
                       button.setBackgroundImage(randomImage, for: .normal)
                       buttonTypes[button] = nonOptionalAttributes
                   }
               }
           }
           for button in selectedButtons {
               UIView.animate(withDuration: 0.1, animations: {
                   button.transform = CGAffineTransform.identity
               })
               selectedButtons.remove(button)
               isSelected[button] = false
           }
       }
       // Toggle the state
       
    }
    
//    func doesSetExist(){
//        for
//    }

    func isSet(buttons: Set<UIButton>) -> Bool{
        var attributes : [[Int]] = [[], [], []]
        var i = 0
        for button in buttons {
            attributes[i] = buttonTypes[button]!
            i += 1
        }
        for j in 0...3{
            if !((attributes[0][j] == attributes[1][j] && attributes[1][j] == attributes[2][j]) || (attributes[0][j] != attributes[1][j] && attributes[1][j] != attributes[2][j] && attributes[0][j] != attributes[2][j])){
                return false
            }
        }
        return true
    }
    
    func getBackgroundImageName(for button: UIButton) -> String? {
        if let backgroundImage = button.backgroundImage(for: .normal) {
            return backgroundImage.accessibilityIdentifier
        }
        return nil
    }
    
    func getSubstring(from str: String, start: Int, end: Int) -> String {
        let startIndex = str.index(str.startIndex, offsetBy: start)
        let endIndex = str.index(str.startIndex, offsetBy: end)
        return String(str[startIndex..<endIndex])
    }
    
    func updateLabels() {
        cardsRemainingLabel.text = String(cardImages.count)
        setsFoundLabel.text = String((69-cardImages.count)/3)
    }
       
    
    func startTimer() {
        timer = Timer.scheduledTimer(timeInterval: 1.0, target: self, selector: #selector(updateTimer), userInfo: nil, repeats: true)
    }
    
    @objc func updateTimer() {
        seconds += 1
        timerLabel.text = formatTime(seconds: seconds)
    }
    
    func formatTime(seconds: Int) -> String {
        let minutes = (seconds % 3600) / 60
        let seconds = seconds % 60
        return String(format: "%02d:%02d", minutes, seconds)
    }
   
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        
        // Invalidate the timer when the view is about to disappear
        timer?.invalidate()
    }
    
    func setRandomBackgroundImages() {
            // List of image names
    
            // Array of buttons
            let buttons = [card1, card2, card3, card4, card5, card6, card7, card8, card9, card10, card11, card12]
            
            // Iterate through each button and set a random background image
            for button in buttons {
                let randomIndex = Int(arc4random_uniform(UInt32(cardImages.count)))
                let randomImageName : String = cardImages[randomIndex]
                var attributes = [Int(getSubstring(from: randomImageName, start: 4, end:5)),
                                  Int(getSubstring(from: randomImageName, start: 5, end:6)),
                                  Int(getSubstring(from: randomImageName, start: 6, end:7)),
                                  Int(getSubstring(from: randomImageName, start: 7, end:8))]
                var nonOptionalAttributes: [Int] = attributes.compactMap { $0 }

                cardImages.remove(at: randomIndex)
                if let randomImage = UIImage(named: randomImageName) {
                    button?.setBackgroundImage(randomImage, for: .normal)
                    buttonTypes[button!] = nonOptionalAttributes
                }
            }
        }
    
    
   
    @IBAction func card(_ sender: Any) {
        
    }
    
    
    @IBAction func tapCard13(_ sender: Any) {
        print("card13 pressed")
    }
    
    

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destination.
        // Pass the selected object to the new view controller.
    }
    */

}
