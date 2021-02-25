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

- (id)initWithFrame:(CGRect)rect
{
    self = [super initWithFrame:rect];

    if (nil == self) {
        return nil;
    }
    
    Class metalClass = NSClassFromString(@"CAMetalLayer");    //is metal runtime sdk available
    if (metalClass != nil) {
        m_device = MTLCreateSystemDefaultDevice(); // is metal supported on this device (is there a better way to do this - without creating device ?)
        if (m_device) {
            [m_device retain];
        } else {
            [NSException raise:@"Failed to create default device" format:@"%@ is null", m_device];
        }
    } else {
        [NSException raise:@"Metal not Supported" format:@"%@ is null", metalClass];
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

@end
