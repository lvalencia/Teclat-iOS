//
// Created by LuisValencia on 3/6/21.
//

#import "InputHandler.h"
#import "KeyboardView.h"

@implementation InputHandler

void insertText(void *self, std::string input) {
    [(id) self forwardInputToTextDocumentProxy:input];
}

- (id)initWithTextDocumentProxy:(id <UITextDocumentProxy>)proxy {
    self = [super init];
    if (nil == self) {
        return nil;
    }
    textDocumentProxy = proxy;
    return self;
}

- (void)forwardInputToTextDocumentProxy:(std::string)string {
    [textDocumentProxy insertText:
            [NSString stringWithCString:string.c_str()
                               encoding:[NSString defaultCStringEncoding]]
    ];
}

@end