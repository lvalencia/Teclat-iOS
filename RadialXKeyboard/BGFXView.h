//
//  BGFXView.h
//  RadialXKeyboard
//
//  Created by LuisValencia on 2/24/21.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface BGFXView : UIView {
    CADisplayLink* displayLink;
}
- (void) start;
- (void) stop;

@end

NS_ASSUME_NONNULL_END
