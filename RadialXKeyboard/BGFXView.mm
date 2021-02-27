//
//  BGFXView.m
//  RadialXKeyboard
//
//  Created by LuisValencia on 2/24/21.
//

#import "BGFXView.h"
#import <bgfx/bgfx.h>
#import <bgfx/platform.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

static id<MTLDevice> m_device = NULL;

@implementation BGFXView

+ (Class)layerClass
{
    Class metalClass = NSClassFromString(@"CAMetalLayer");    //is metal runtime sdk available
    if (metalClass != nil) {
        m_device = MTLCreateSystemDefaultDevice(); // is metal supported on this device (is there a better way to do this - without creating device ?)
        if (m_device) {
            [m_device retain];
            return metalClass;
        } else {
            [NSException raise:@"Failed to create default device" format:@"%@ is null", m_device];
        }
    }
    [NSException raise:@"Metal not Supported" format:@"%@ is null", metalClass];
    return nil;
}

- (id)initWithFrame:(CGRect)rect
{
    self = [super initWithFrame:rect];

    if (nil == self) {
        return nil;
    }

    bgfx::PlatformData platformData;
    platformData.ndt          = NULL;
    platformData.nwh          = self.layer;
    platformData.context      = m_device;
    platformData.backBuffer   = NULL;
    platformData.backBufferDS = NULL;
    bgfx::setPlatformData(platformData);
    
    return self;
}

- (void)renderFrame {
    bgfx::renderFrame();
}

- (void) start {
    if (nil == displayLink) {
        displayLink = [self.window.screen displayLinkWithTarget:self selector:@selector(renderFrame)];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
    }
}

- (void)stop
{
    if (nil != displayLink)
    {
        [displayLink invalidate];
        displayLink = nil;
    }
}

@end
