//
//  KeyboardViewController.m
//  RadialXKeyboard
//
//  Created by LuisValencia on 2/20/21.
//

#import "KeyboardViewController.h"
#import "KeyboardView.h"

#define DEBUG NO

@interface KeyboardViewController ()
@property(nonatomic, strong) UIButton *nextKeyboardButton;
@property(nonatomic, strong) KeyboardView *keyboardView;
@end

@implementation KeyboardViewController

- (void)updateViewConstraints {
    [super updateViewConstraints];
}

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (void)viewWillAppear:(BOOL)animated {
    // Hack with a nil check
    if (nil == self.keyboardView){
        CGRect rect = [self.view frame];

        self.keyboardView = [[KeyboardView alloc] initWithFrame:rect andDocumentProxy: self.textDocumentProxy];
        float scaleFactor = (float) [self.view contentScaleFactor];
        [self.keyboardView setContentScaleFactor:scaleFactor];

        // Visual Testing our BGFX View is Green
        if (DEBUG) {
            [self.keyboardView setBackgroundColor:[UIColor colorWithRed:0
                                                                  green:1
                                                                   blue:0
                                                                  alpha:1]];
        }

        [self.view addSubview:self.keyboardView];
        [self.keyboardView start];
    }
}

-(void) willMoveToParentViewController:(UIViewController *)parent{
    // being removed from view
    if (nil == parent) {
        [self.keyboardView stop];
        [self.keyboardView removeFromSuperview];
        [self.keyboardView dealloc];
        self.keyboardView = nil;
    }
}

- (void)viewWillLayoutSubviews {
    self.nextKeyboardButton.hidden = !self.needsInputModeSwitchKey;
    [super viewWillLayoutSubviews];
}

- (void)textWillChange:(id <UITextInput>)textInput {
    // The app is about to change the document's contents. Perform any preparation here.
}

- (void)textDidChange:(id <UITextInput>)textInput {
    // The app has just changed the document's contents, the document context has been updated.

    UIColor *textColor = nil;
    if (self.textDocumentProxy.keyboardAppearance == UIKeyboardAppearanceDark) {
        textColor = [UIColor whiteColor];
    } else {
        textColor = [UIColor blackColor];
    }
    [self.nextKeyboardButton setTitleColor:textColor forState:UIControlStateNormal];
}

@end
