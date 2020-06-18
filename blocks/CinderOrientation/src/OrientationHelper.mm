
#include <UIKit/UIDevice.h>
#include "OrientationHelper.h"
#import <Foundation/Foundation.h>
#import <objc/runtime.h>

@interface UIViewController (HomeButtonSwiz) {
}
- (BOOL)xxx_prefersHomeIndicatorAutoHidden;

@end

@implementation UIViewController (HomeButtonSwiz)
- (BOOL)xxx_prefersHomeIndicatorAutoHidden {
    return YES;
}
@end

namespace cinder { namespace app {
    
    void setupNotifications(OrientationHelper *helper)
    {
        [[NSNotificationCenter defaultCenter] addObserverForName:@"UIDeviceOrientationDidChangeNotification"
                                              object:nil 
                                              queue:nil 
                                              usingBlock: ^(NSNotification *notification) {
            UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
            if (UIDeviceOrientationIsValidInterfaceOrientation(orientation)) {
            // let's always make sure the task bar is shown on the correct side of the device
                
                // KEMAL - Dperacted on iOS 13.  Rotation is pretty messed up in this cinder version
                // so I'm locking it to landscape
                [UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientation(orientation);
            }
            // and then tell everyone else
            helper->onOrientationChanged(Orientation(orientation));
        }];
        [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];            
    }

void homebuttonHack() {
     
    // KEMAL - This is so nasty, dont want to fix an old cinder so that I can access it's viewController
    // jsut to set a property.  Since this is an obj-c++ file that is used early on launch I'm gonna hook
    // in here to hide the home bar.

    Class cls = [UIViewController class];
    SEL originalSelector = @selector(prefersHomeIndicatorAutoHidden);
    SEL swizzledSelector = @selector(xxx_prefersHomeIndicatorAutoHidden);
    
    Method originalMethod = class_getInstanceMethod(cls, originalSelector);
    Method swizzledMethod = class_getInstanceMethod(cls, swizzledSelector);
    
    BOOL didAddMethod =
        class_addMethod(cls,
            originalSelector,
            method_getImplementation(swizzledMethod),
            method_getTypeEncoding(swizzledMethod));

    if (didAddMethod) {
        class_replaceMethod(cls,
            swizzledSelector,
            method_getImplementation(originalMethod),
            method_getTypeEncoding(originalMethod));
    } else {
        method_exchangeImplementations(originalMethod, swizzledMethod);
    }
}
    
    void OrientationHelper::setup()
    {
        // KEMAL - Facepalm
        homebuttonHack();
        
        // set this here to ensure it's accessible from getDeviceOrientation in setup:
        mDeviceOrientation = Orientation([[UIDevice currentDevice] orientation]);

        // use status bar orientation to find a valid interface orientation on startup:
        mInterfaceOrientation = Orientation([UIApplication sharedApplication].statusBarOrientation);
        
        setupNotifications(this);
    }
    
    void OrientationHelper::onOrientationChanged(const Orientation &orientation)
    {
        Orientation lastInterfaceOrientation = mInterfaceOrientation;
        Orientation lastDeviceOrienation = mDeviceOrientation;
        
        // always update device orientation
        mDeviceOrientation = orientation;
        
        // only update interface orientation if it's valid
        if (isValidInterfaceOrientation(orientation)) {
            mInterfaceOrientation = orientation;
        }
        
        OrientationEvent event( mDeviceOrientation, lastDeviceOrienation, mInterfaceOrientation, lastInterfaceOrientation );
        
        mCallbacksOrientationChanged.call(event);
        
        // NB:- this is what AppCocoaTouch would do in privateOrientationChanged__():
        //            bool handled = false;
        //            for( CallbackMgr<bool (OrientationEvent)>::iterator cbIter = mCallbacksOrientationChanged.begin(); ( cbIter != mCallbacksOrientationChanged.end() ) && ( ! handled ); ++cbIter )
        //                handled = (cbIter->second)( event );		
        //            if( ! handled )	
        //                orientationChanged( event );
    }
    
} }
