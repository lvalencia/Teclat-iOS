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

BOOL firstTime = YES;

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
    return nil; // Should never get here
}

- (id)initWithFrame:(CGRect)rect {
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

    bgfx::Init init;
    init.type     = bgfx::RendererType::Count;
    init.vendorId = BGFX_PCI_ID_NONE;
    init.resolution.width  = rect.size.width;
    init.resolution.height = rect.size.height;
    init.resolution.reset  = BGFX_RESET_VSYNC;
    bgfx::init(init);

    // Enable debug text.
    bgfx::setDebug(BGFX_DEBUG_TEXT);

    // Set view 0 clear state.
    bgfx::setViewClear(0
            , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
            , 0x303030ff
            , 1.0f
            , 0
    );

    return self;
}

- (void)renderFrame {
    // This renders in the view (if it's not rendering the whole view will be green)
    CGRect frame = [self frame];
    bgfx::setViewRect(0, 0, 0, uint16_t(frame.size.width), uint16_t(frame.size.height));
    
    // This dummy draw call is here to make sure that view 0 is cleared
    // if no other draw calls are submitted to view 0.
    bgfx::touch(0);

    

    bgfx::frame();
     
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
