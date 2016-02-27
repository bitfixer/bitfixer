//
//  ViewController.h
//  iosserial
//
//  Created by Michael Hill on 2/14/16.
//  Copyright (c) 2016 Michael Hill. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AudioSerial.hpp"

@interface ViewController : UIViewController <UITextViewDelegate>

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text;

@end

