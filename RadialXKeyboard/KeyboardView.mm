//
//  KeyboardView.m
//  RadialXKeyboard
//
//  Created by LuisValencia on 2/24/21.
//

#import "KeyboardView.h"
#import "InputHandler.h"
#import <Metal/Metal.h>

#pragma mark static references

static id <MTLDevice> metalDevice = NULL;

#pragma mark BGFXView implementation

@implementation KeyboardView

#pragma mark KeyboardView overrides

+ (Class)layerClass {
    Class metalClass = NSClassFromString(@"CAMetalLayer");    //is metal runtime sdk available
    if (metalClass != nil) {
        metalDevice = MTLCreateSystemDefaultDevice(); // is metal supported on this device (is there a better way to do this - without creating device ?)
        if (metalDevice) {
            [metalDevice retain];
            return metalClass;
        } else {
            [NSException raise:@"Failed to create default device" format:@"%@ is null", metalDevice];
        }
    }
    [NSException raise:@"Metal not Supported" format:@"%@ is null", metalClass];
    return nil; // Should never get here
}

- (id)initWithFrame:(CGRect)rect andDocumentProxy:(id <UITextDocumentProxy>)proxy {
    self = [super initWithFrame:rect];

    if (nil == self) {
        return nil;
    }

    inputHandler = [[InputHandler alloc] initWithTextDocumentProxy:proxy];
    eventsQueue = new ThreadSafeQueue<CGPoint>();

    NSBundle *main = [NSBundle mainBundle];
    NSString *resourcePath = [main resourcePath];
    RendererArgs rendererArgs;
    rendererArgs.resourcePath = [resourcePath UTF8String];
    rendererArgs.layer = [self layer];
    rendererArgs.device = metalDevice;
    rendererArgs.width = static_cast<uint32_t>(rect.size.width);
    rendererArgs.height = static_cast<uint32_t>(rect.size.height);
    rendererArgs.eventsQueue = eventsQueue;

    keyboardRenderer = new KeyboardRenderer(rendererArgs);
    keyboardRenderer->init();

    return self;
}

#pragma mark touch events

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    [self handleTouchEvent:touches withEvent:event];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    [self handleTouchEvent:touches withEvent:event];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    [self handleTouchEvent:touches withEvent:event];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    [self handleTouchEvent:touches withEvent:event];
}

- (void)handleTouchEvent:(NSSet *)touches withEvent:(UIEvent *)event {
    BX_UNUSED(touches);
    UITouch *touch = [[event allTouches] anyObject];
    CGPoint touchLocation = [touch locationInView:self];
    touchLocation.x *= self.contentScaleFactor;
    touchLocation.y *= self.contentScaleFactor;

    eventsQueue->enqueue(touchLocation);
}

#pragma mark KeyboardView methods

- (void)renderFrame {
    // This renders in the view (if it's not rendering the whole view will be green)
    keyboardRenderer->update(inputHandler);
}

- (void)start {
    if (nil == displayLink) {
        displayLink = [self.window.screen displayLinkWithTarget:self selector:@selector(renderFrame)];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
    }
}

- (void)stop {
    if (nil != displayLink) {
        keyboardRenderer->teardown();
        [displayLink invalidate];
        displayLink = nil;
    }
}

@end
