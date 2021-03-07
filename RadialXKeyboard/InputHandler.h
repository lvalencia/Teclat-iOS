//
// Created by LuisValencia on 3/6/21.
//

#import <Foundation/Foundation.h>
#import "InputHandler.hpp"

@protocol UITextDocumentProxy;

@interface InputHandler : NSObject {
    id <UITextDocumentProxy> textDocumentProxy;
}

- (id)initWithTextDocumentProxy:(id <UITextDocumentProxy>)proxy;

- (void)forwardInputToTextDocumentProxy:(std::string)string;

@end