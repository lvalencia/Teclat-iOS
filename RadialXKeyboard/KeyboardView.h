//
//  KeyboardView.h
//  RadialXKeyboard
//
//  Created by LuisValencia on 2/24/21.
//

#import <UIKit/UIKit.h>
#import "KeyboardRenderer.hpp"

NS_ASSUME_NONNULL_BEGIN

@interface KeyboardView : UIView {
    KeyboardRenderer* keyboardRenderer;
    CADisplayLink* displayLink;
}
- (void) start;
- (void) stop;

@end

NS_ASSUME_NONNULL_END
