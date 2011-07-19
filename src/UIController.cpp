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
    cbTouchesBegan = mApp->registerTouchesBegan( this, &UIController::touchesBegan );
    cbTouchesMoved = mApp->registerTouchesMoved( this, &UIController::touchesMoved );
    cbTouchesEnded = mApp->registerTouchesEnded( this, &UIController::touchesEnded );
    cbOrientationChanged = mOrientationHelper->registerOrientationChanged( this, &UIController::orientationChanged );    
    setInterfaceOrientation( mOrientationHelper->getInterfaceOrientation() );
}

UIController::~UIController()
{
    mApp->unregisterTouchesBegan( cbTouchesBegan );
    mApp->unregisterTouchesMoved( cbTouchesMoved );
    mApp->unregisterTouchesEnded( cbTouchesEnded );
    mOrientationHelper->unregisterOrientationChanged( cbOrientationChanged );
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

void UIController::setInterfaceOrientation( const Orientation &orientation )
{
    mInterfaceOrientation = orientation;
    mOrientationMatrix = getOrientationMatrix44(mInterfaceOrientation, getWindowSize());
    
    mInterfaceSize = app::getWindowSize();
    
    if ( isLandscapeOrientation( mInterfaceOrientation ) ) {
        mInterfaceSize = mInterfaceSize.yx(); // swizzle it!
    }        
}

Matrix44f UIController::getConcatenatedTransform() const
{
    return mOrientationMatrix * mTransform;
}

void UIController::draw()
{
    glPushMatrix();
    glMultMatrixf(mOrientationMatrix); // FIXME only push/mult/pop if mOrientationMatrix isn't identity

    glMultMatrixf(mTransform); // FIXME only mult if mTransform isn't identity
    // draw children
    for (std::vector<UINodeRef>::const_iterator i = mChildren.begin(); i != mChildren.end(); i++) {
        (*i)->privateDraw();
    }
    // dont' draw self or we'll recurse

    glPopMatrix();
}