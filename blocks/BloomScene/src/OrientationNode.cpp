//
//  BloomScene.cpp
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppCocoaTouch.h" // for getElapsedSeconds, getWindowSize
#include "cinder/gl/gl.h"
#include "OrientationHelper.h"
#include "OrientationNode.h"
#include "BloomScene.h"

using namespace ci;
using namespace ci::app;

OrientationNodeRef OrientationNode::create( OrientationHelper *orientationHelper )
{
    return OrientationNodeRef( new OrientationNode( orientationHelper ) );    
}

OrientationNode::OrientationNode( OrientationHelper *orientationHelper ): 
    mOrientationHelper(orientationHelper),
    mInterfaceAngle(0.0f),
    mTargetInterfaceSize(0.0f,0.0f),
    mTargetInterfaceAngle(0.0f),
    mLastOrientationChangeTime(0.0f),
    mOrientationAnimationDuration(0.25f),
    mPrevInterfaceAngle(0.0f),
    mPrevInterfaceSize(0.0f,0.0f),
    mEnableAnimation(true),
    mCurrentlyAnimating(false)
{
    cbOrientationChanged = mOrientationHelper->registerOrientationChanged( this, &OrientationNode::orientationChanged );    
    // initialize orientation *without animating*
    setInterfaceOrientation( mOrientationHelper->getInterfaceOrientation(), false );
}

OrientationNode::~OrientationNode()
{
    mOrientationHelper->unregisterOrientationChanged( cbOrientationChanged );
}

bool OrientationNode::orientationChanged( OrientationEvent event )
{
    setInterfaceOrientation( event.getInterfaceOrientation(), mEnableAnimation );
    return false;
}

void OrientationNode::setInterfaceOrientation( const Orientation &orientation, bool animate )
{
    mInterfaceOrientation = orientation;
    
    std::cout << "setting orientation node orientation to: " << getOrientationDescription(mInterfaceOrientation) << std::endl;
    
    const float TWO_PI = 2.0f * M_PI;
    
    // normalize current interface angle
    if (mInterfaceAngle < 0.0){
        float turns = floor( fabs( mInterfaceAngle / TWO_PI ) );
		mInterfaceAngle += turns * TWO_PI;
	}
    else if (mInterfaceAngle > TWO_PI){
        float turns = floor( mInterfaceAngle / TWO_PI );
		mInterfaceAngle -= turns * TWO_PI;
	}
    
    // get the facts
    Vec2f deviceSize = app::getWindowSize();
    float orientationAngle = getOrientationAngle( mInterfaceOrientation );
    
    // assign new targets
    mTargetInterfaceSize.x = fabs(deviceSize.x * cos(orientationAngle) + deviceSize.y * sin(orientationAngle));
    mTargetInterfaceSize.y = fabs(deviceSize.y * cos(orientationAngle) + deviceSize.x * sin(orientationAngle));
    mTargetInterfaceAngle = TWO_PI - orientationAngle;
    
    // make sure we're turning the right way
    if (abs(mTargetInterfaceAngle-mInterfaceAngle) > M_PI) {
        if (mTargetInterfaceAngle < mInterfaceAngle) {
            mTargetInterfaceAngle += TWO_PI;
        }
        else {
            mTargetInterfaceAngle -= TWO_PI;
        }
    }
    
    if (animate && mLastOrientationChangeTime >= 0.0f) {
        // remember current values for lerping later
        mPrevInterfaceAngle = mInterfaceAngle;
        mPrevInterfaceSize = getRoot()->getInterfaceSize();
        // and reset the counter
        mLastOrientationChangeTime = app::getElapsedSeconds();
    }
    else {
        // just jump to the animation's end in next update
        mLastOrientationChangeTime = -1;                
    }

    // ensure update() does the right thing
    mCurrentlyAnimating = true;
}

void OrientationNode::update()
{
    // animate transition
    if (mCurrentlyAnimating) {
        
        float t = app::getElapsedSeconds() - mLastOrientationChangeTime;

        if (t < mOrientationAnimationDuration) {
            float p = t / mOrientationAnimationDuration;
            getRoot()->setInterfaceSize( lerp(mPrevInterfaceSize, mTargetInterfaceSize, p) );
            mInterfaceAngle = lerp(mPrevInterfaceAngle, mTargetInterfaceAngle, p);
        }
        else {
            // ensure snap to final values
            getRoot()->setInterfaceSize( mTargetInterfaceSize );
            mInterfaceAngle = mTargetInterfaceAngle;
            mLastOrientationChangeTime = 0.0f; // used as sentinel in setInterfaceOrientation
            mCurrentlyAnimating = false;
        }
        
        // update matrix (for globalToLocal etc)
        mTransform.setToIdentity();
        mTransform.translate( Vec3f( app::getWindowCenter(), 0 ) );
        mTransform.rotate( Vec3f( 0, 0, mInterfaceAngle ) );
        mTransform.translate( Vec3f( getRoot()->getInterfaceSize() * -0.5f, 0 ) );                        
    }    
}

float OrientationNode::getOrientationAngle( const Orientation &orientation )
{
    switch (orientation) {
        case LANDSCAPE_LEFT_ORIENTATION:
            return M_PI * 1.5f;
        case UPSIDE_DOWN_PORTRAIT_ORIENTATION:
            return M_PI;
        case LANDSCAPE_RIGHT_ORIENTATION:
            return M_PI * 0.5f;
        case PORTRAIT_ORIENTATION:
        default:
            return 0.0;
    }
}

std::string OrientationNode::getOrientationDescription( const Orientation &orientation )
{
    switch (orientation) {
        case LANDSCAPE_LEFT_ORIENTATION:
            return "LANDSCAPE_LEFT_ORIENTATION";
        case UPSIDE_DOWN_PORTRAIT_ORIENTATION:
            return "UPSIDE_DOWN_PORTRAIT_ORIENTATION";
        case LANDSCAPE_RIGHT_ORIENTATION:
            return "LANDSCAPE_RIGHT_ORIENTATION";
        case PORTRAIT_ORIENTATION:
            return "PORTRAIT_ORIENTATION";
        default:
            return "UNKNOWN_ORIENTATION";
    }
}