//
//  KeyboardViewController.m
//  RadialXKeyboard
//
//  Created by LuisValencia on 2/20/21.
//

#import "KeyboardViewController.h"
#import "BGFXView.h"

@interface KeyboardViewController ()
@property(nonatomic, strong) UIButton *nextKeyboardButton;
@property(nonatomic, strong) BGFXView *keyboardView;
@end

@implementation KeyboardViewController

- (void)updateViewConstraints {
    [super updateViewConstraints];

    // Add custom view sizing constraints here
}

- (void)viewWillAppear:(BOOL)animated {
    // Visual Testing, root view is red
    self.view.backgroundColor = [UIColor colorWithRed:1
                                                green:0
                                                 blue:0
                                                alpha:1];

    CGRect rect = [self.view frame];
    self.keyboardView = [[BGFXView alloc] initWithFrame:rect];
    float scaleFactor = [self.view contentScaleFactor];
    [self.keyboardView setContentScaleFactor:scaleFactor];
    [self.view addSubview:self.keyboardView];

    // Visual Testing our BGFX View is Green
    [self.keyboardView setBackgroundColor:[UIColor colorWithRed:0
                                                          green:1
                                                           blue:0
                                                          alpha:1]];
    [self.keyboardView start];
}

- (void)viewDidDisappear:(BOOL)animated {
    [((BGFXView *) self.view) stop];
}

- (void)viewDidLoad {
    [super viewDidLoad];


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
