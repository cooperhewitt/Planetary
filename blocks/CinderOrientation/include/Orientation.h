
#pragma once

#include <UIKit/UIDevice.h> // for UIDeviceOrientation enum
#include "cinder/Matrix.h"
#include "cinder/Vector.h"

namespace cinder { namespace app {

    // not sure if it's correct to use Objective-C values in this header?
    enum Orientation { 
        UNKNOWN_ORIENTATION = UIDeviceOrientationUnknown,
        // Device oriented vertically, home button on the bottom:
        PORTRAIT_ORIENTATION = UIDeviceOrientationPortrait, 
        // Device oriented vertically, home button on the top:
        UPSIDE_DOWN_PORTRAIT_ORIENTATION = UIDeviceOrientationPortraitUpsideDown,  
        // Device oriented horizontally, home button on the right:
        LANDSCAPE_LEFT_ORIENTATION = UIDeviceOrientationLandscapeLeft,       
        // Device oriented horizontally, home button on the left:
        LANDSCAPE_RIGHT_ORIENTATION = UIDeviceOrientationLandscapeRight,      
        // Device oriented flat, face up:
        FACE_UP_ORIENTATION = UIDeviceOrientationFaceUp,              
        // Device oriented flat, face down:
        FACE_DOWN_ORIENTATION = UIDeviceOrientationFaceDown             
    };

    // how much to rotate from PORTRAIT_ORIENTATION to the given orientation
    float getAngleForOrientation(const Orientation &orientation);    

    // if you usually use Vec3f::yAxis() for up on your CameraPersp, this will help
    Vec3f getUpVectorForOrientation(const Orientation &orientation);

    // if you're doing 2D drawing, this matrix moves the origin to the correct device corner
    // to get the window size, use app::getWindowSize(), test for 
    // isLandscape(event.getInterfaceOrientation()) and apply a .yx() swizzle 
    Matrix44f getOrientationMatrix44(const Orientation &orientation, const Vec2f &deviceSize);

    std::string getOrientationString(const Orientation &orientation);

    int getRotationSteps(const Orientation &from, const Orientation &to);
    
} }