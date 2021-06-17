#include "Orientation.h"

namespace cinder { namespace app {

    // how much to rotate from PORTRAIT_ORIENTATION to the given orientation
    float getAngleForOrientation(const Orientation &orientation)
    {
        return 0.0f;
        
        switch ( orientation )
        {
            case UPSIDE_DOWN_PORTRAIT_ORIENTATION:
                return M_PI;
            case LANDSCAPE_LEFT_ORIENTATION:
                return M_PI/2.0f;
            case LANDSCAPE_RIGHT_ORIENTATION:
                return -M_PI/2.0f;
            default:
                // if in doubt, just return the normal one
                return 0.0f;
        }
    }

    // if you usually use Vec3f::yAxis() for up on your CameraPersp, this will help
    Vec3f getUpVectorForOrientation(const Orientation &orientation)
    {
        return Vec3f::yAxis();
        
        switch ( orientation )
        {
            case PORTRAIT_ORIENTATION:
                return Vec3f::yAxis();
            case UPSIDE_DOWN_PORTRAIT_ORIENTATION:
                return -Vec3f::yAxis();
            case LANDSCAPE_LEFT_ORIENTATION:
                return Vec3f::xAxis();
            case LANDSCAPE_RIGHT_ORIENTATION:
                return -Vec3f::xAxis();
            default:
                // if in doubt, just return the normal one
                return Vec3f::yAxis();                    
        }  
    }

    // if you're doing 2D drawing, this matrix moves the origin to the correct device corner
    // to get the window size, use app::getWindowSize(), test for 
    // isLandscape(event.getInterfaceOrientation()) and apply a .yx() swizzle 
    Matrix44f getOrientationMatrix44(const Orientation &orientation, const Vec2f &deviceSize)
    {
        Matrix44f orientationMtx;
        switch ( orientation )
        {
            case UPSIDE_DOWN_PORTRAIT_ORIENTATION:
                orientationMtx.translate( Vec3f( deviceSize.x, deviceSize.y, 0 ) );            
                orientationMtx.rotate( Vec3f( 0, 0, M_PI ) );
                break;
            case LANDSCAPE_LEFT_ORIENTATION:
                orientationMtx.translate( Vec3f( deviceSize.x, 0, 0 ) );
                orientationMtx.rotate( Vec3f( 0, 0, M_PI/2.0 ) );
                break;
            case LANDSCAPE_RIGHT_ORIENTATION:
                orientationMtx.translate( Vec3f( 0, deviceSize.y, 0 ) );
                orientationMtx.rotate( Vec3f( 0, 0, -M_PI/2.0 ) );
                break;
            default:
                break;
        }
        
        return orientationMtx;          
    }

    std::string getOrientationString(const Orientation &orientation)
    {
        switch (orientation) {
            case PORTRAIT_ORIENTATION:
                return "Portrait";
            case UPSIDE_DOWN_PORTRAIT_ORIENTATION:
                return "Upside Down Portrait";
            case LANDSCAPE_LEFT_ORIENTATION:
                return "Landscape Left";
            case LANDSCAPE_RIGHT_ORIENTATION:
                return "Landscape Right";
            case FACE_UP_ORIENTATION:
                return "Face Up";
            case FACE_DOWN_ORIENTATION:
                return "Face Down";
            case UNKNOWN_ORIENTATION:
                break;
        }
        return "Unknown";
    }
    
    int getRotationSteps(const Orientation &from, const Orientation &to)
    {
        return 0;
        
        if (from == to) {
            return 0;
        }
        
        switch(from) {
            case PORTRAIT_ORIENTATION:
                switch(to) {
                    case LANDSCAPE_LEFT_ORIENTATION: return 1;
                    case LANDSCAPE_RIGHT_ORIENTATION: return -1;
                    case UPSIDE_DOWN_PORTRAIT_ORIENTATION: return 2;
                    default: return 0;
                }
            case LANDSCAPE_LEFT_ORIENTATION:
                switch(to) {
                    case PORTRAIT_ORIENTATION: return -1;
                    case LANDSCAPE_RIGHT_ORIENTATION: return 2;
                    case UPSIDE_DOWN_PORTRAIT_ORIENTATION: return 1;
                    default: return 0;
                }
            case LANDSCAPE_RIGHT_ORIENTATION:
                switch(to) {
                    case PORTRAIT_ORIENTATION: return 1;
                    case LANDSCAPE_LEFT_ORIENTATION: return 2;
                    case UPSIDE_DOWN_PORTRAIT_ORIENTATION: return -1;
                    default: return 0;
                }
            case UPSIDE_DOWN_PORTRAIT_ORIENTATION:
                switch(to) {
                    case PORTRAIT_ORIENTATION: return 2;
                    case LANDSCAPE_LEFT_ORIENTATION: return -1;
                    case LANDSCAPE_RIGHT_ORIENTATION: return 1;
                    default: return 0;
                }
            default:
                return 0;
        }
    }
    
} }
