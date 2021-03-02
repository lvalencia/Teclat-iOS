//
//  KeyboardView.m
//  RadialXKeyboard
//
//  Created by LuisValencia on 2/24/21.
//

#import "KeyboardView.h"
#import <bgfx/platform.h>
#import <Metal/Metal.h>

#pragma mark static references

static id <MTLDevice> metalDevice = NULL;

#pragma mark BGFXView implementation

@implementation KeyboardView

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

- (id)initWithFrame:(CGRect)rect {
    self = [super initWithFrame:rect];

    if (nil == self) {
        return nil;
    }

    NSBundle *main = [NSBundle mainBundle];
    NSString *resourcePath = [main resourcePath];
    RendererArgs rendererArgs;
    rendererArgs.resourcePath = [resourcePath UTF8String];
    rendererArgs.layer = [self layer];
    rendererArgs.device = metalDevice;
    rendererArgs.width = static_cast<uint32_t>(rect.size.width);
    rendererArgs.height = static_cast<uint32_t>(rect.size.height);

    keyboardRenderer = new KeyboardRenderer(rendererArgs);
    keyboardRenderer->init();

    return self;
}

- (void)renderFrame {
    // This renders in the view (if it's not rendering the whole view will be green)
    keyboardRenderer->update();
    // bgfx::renderFrame();
}

- (void)start {
    if (nil == displayLink) {
        displayLink = [self.window.screen displayLinkWithTarget:self selector:@selector(renderFrame)];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
    }
}

- (void)stop {
    if (nil != displayLink) {
        [displayLink invalidate];
        displayLink = nil;
    }
}

@end
