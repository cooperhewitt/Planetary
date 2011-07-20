//
//  UIController.cpp
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "UIController.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;

UIController::UIController( AppCocoaTouch *app, OrientationHelper *orientationHelper ): mApp(app), mOrientationHelper(orientationHelper)
{
    mParent = UINodeRef(this); // FIXME: shared_from_this() but in a subclass
    mRoot = UIControllerRef(this); // FIXME: shared_from_this() but for a subclass
    // FIXME: should mRoot be static (essentially, should UIController be a singleton?)
    cbTouchesBegan = mApp->registerTouchesBegan( this, &UIController::touchesBegan );
    cbTouchesMoved = mApp->registerTouchesMoved( this, &UIController::touchesMoved );
    cbTouchesEnded = mApp->registerTouchesEnded( this, &UIController::touchesEnded );
    if (mOrientationHelper) {
        cbOrientationChanged = mOrientationHelper->registerOrientationChanged( this, &UIController::orientationChanged );    
    }
    mOrientationAnimationDuration = 0.25f;
    setInterfaceOrientation( mOrientationHelper->getInterfaceOrientation(), false );
}

UIController::~UIController()
{
    mApp->unregisterTouchesBegan( cbTouchesBegan );
    mApp->unregisterTouchesMoved( cbTouchesMoved );
    mApp->unregisterTouchesEnded( cbTouchesEnded );
    if (mOrientationHelper) {
        mOrientationHelper->unregisterOrientationChanged( cbOrientationChanged );
    }
}

bool UIController::touchesBegan( TouchEvent event )
{
    for (std::vector<TouchEvent::Touch>::const_iterator i = event.getTouches().begin(); i != event.getTouches().end(); i++) {
        privateTouchBegan(*i); // recurses to children
    }    
    return false;
}

bool UIController::touchesMoved( TouchEvent event )
{
    for (std::vector<TouchEvent::Touch>::const_iterator i = event.getTouches().begin(); i != event.getTouches().end(); i++) {
        privateTouchMoved(*i); // recurses to children
    }
    return false;
}

bool UIController::touchesEnded( TouchEvent event )
{
    for (std::vector<TouchEvent::Touch>::const_iterator i = event.getTouches().begin(); i != event.getTouches().end(); i++) {
        privateTouchEnded(*i); // recurses to children
    }    
    return false;
}

bool UIController::orientationChanged( OrientationEvent event )
{
    setInterfaceOrientation( event.getInterfaceOrientation() );
    return false;
}

void UIController::setInterfaceOrientation( const Orientation &orientation, bool animate )
{
    mInterfaceOrientation = orientation;

    // get the facts
    Vec2f deviceSize = mApp->getWindowSize();
    float orientationAngle = getOrientationAngle(mInterfaceOrientation);
    
    // normalize interface angle (could be many turns)
    while (mInterfaceAngle < 0.0) mInterfaceAngle += 2.0f * M_PI;
    while (mInterfaceAngle > 2.0f * M_PI) mInterfaceAngle -= 2.0f * M_PI;
    
    // assign new targets
    mTargetInterfaceSize.x = fabs(deviceSize.x * cos(orientationAngle) + deviceSize.y * sin(orientationAngle));
    mTargetInterfaceSize.y = fabs(deviceSize.y * cos(orientationAngle) + deviceSize.x * sin(orientationAngle));
    mTargetInterfaceAngle = 2.0f*M_PI-orientationAngle;
    
    // make sure we're turning the right way
    if (abs(mTargetInterfaceAngle-mInterfaceAngle) > M_PI) {
        if (mTargetInterfaceAngle < mInterfaceAngle) {
            mTargetInterfaceAngle += 2.0f * M_PI;
        }
        else {
            mTargetInterfaceAngle -= 2.0f * M_PI;
        }
    }
    
    mLastOrientationChangeTime = mApp->getElapsedSeconds();

    if (!animate) {
        // jump to animation end
        mInterfaceSize = mTargetInterfaceSize;
        mInterfaceAngle = mTargetInterfaceAngle;
        mLastOrientationChangeTime = -1;                
    }
    
    // remember current values for lerping later
    mPrevInterfaceAngle = mInterfaceAngle;
    mPrevInterfaceSize = mInterfaceSize;    
}

Matrix44f UIController::getConcatenatedTransform() const
{
    return mOrientationMatrix * mTransform;        
}

void UIController::draw()
{
    glPushMatrix();
    glMultMatrixf( getConcatenatedTransform() );    
    // draw children
    for (std::vector<UINodeRef>::const_iterator i = mChildren.begin(); i != mChildren.end(); i++) {
        (*i)->privateDraw();
    }
    // dont' draw self or we'll recurse
    glPopMatrix();
}

void UIController::update()
{
    // animate transition
    float t = mApp->getElapsedSeconds() - mLastOrientationChangeTime;
    if (t < mOrientationAnimationDuration) {
        float p = t / mOrientationAnimationDuration;
        mInterfaceSize = lerp(mPrevInterfaceSize, mTargetInterfaceSize, p);
        mInterfaceAngle = lerp(mPrevInterfaceAngle, mTargetInterfaceAngle, p);
    }
    else {
        // ensure snap to final values
        mInterfaceSize = mTargetInterfaceSize;
        mInterfaceAngle = mTargetInterfaceAngle;        
    }
    
    Vec2f deviceSize = mApp->getWindowSize();

    // update matrix (for globalToLocal etc)
    mOrientationMatrix.setToIdentity();
    mOrientationMatrix.translate( Vec3f(deviceSize/2.0f,0) );
    mOrientationMatrix.rotate( Vec3f(0,0,mInterfaceAngle) );
    mOrientationMatrix.translate( Vec3f(-mInterfaceSize/2.0f,0) );    
    
    // update children
    for (std::vector<UINodeRef>::const_iterator i = mChildren.begin(); i != mChildren.end(); i++) {
        (*i)->privateUpdate();
    }
    // dont' update self or we'll recurse
}

float UIController::getOrientationAngle( const Orientation &orientation )
{
    switch (orientation) {
        case PORTRAIT_ORIENTATION:
            return 0.0;
        case LANDSCAPE_LEFT_ORIENTATION:
            return M_PI * 3.0f / 2.0f;
        case UPSIDE_DOWN_PORTRAIT_ORIENTATION:
            return M_PI;
        case LANDSCAPE_RIGHT_ORIENTATION:
            return M_PI / 2.0f;
    }
    return 0.0f;
}