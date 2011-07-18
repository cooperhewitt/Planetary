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
    mParent = UINodeRef(this);
    mRoot = UIControllerRef(this);
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
    // FIXME: UIController only does one level of touches (UINode doesn't pass on touches to children yet)
    for (std::vector<TouchEvent::Touch>::const_iterator i = event.getTouches().begin(); i != event.getTouches().end(); i++) {
        bool consumed = false;
        for (std::vector<UINodeRef>::const_iterator j = mChildren.begin(); j != mChildren.end(); j++) {
            if ((*j)->touchBegan(*i)) {
                consumed = true;
                activeTouches[i->getId()] = *j;
                mCbUINodeTouchBegan.call(*j);
                break; // check next touch
            }
        }    
        if (!consumed) {
            // check self
            if (touchBegan(*i)) {
                UINodeRef thisRef = UINodeRef(this);
                activeTouches[i->getId()] = thisRef;                
                mCbUINodeTouchBegan.call(thisRef);
            }
        }
    }    
    return false;
}

bool UIController::touchesMoved( TouchEvent event )
{
    // in this current implementation, children only receive touchMoved calls 
    // if they returned true for the touch with the same ID in touchesBegan
    for (std::vector<TouchEvent::Touch>::const_iterator i = event.getTouches().begin(); i != event.getTouches().end(); i++) {
        if ( activeTouches.find(i->getId()) != activeTouches.end() ) {
            UINodeRef nodeRef = activeTouches[i->getId()];
            nodeRef->touchMoved(*i);
            mCbUINodeTouchMoved.call(nodeRef);
        }
    }
    return false;
}

bool UIController::touchesEnded( TouchEvent event )
{
    // in this current implementation, children only receive touchEnded calls 
    // if they returned true for the touch with the same ID in touchesBegan    
    for (std::vector<TouchEvent::Touch>::const_iterator i = event.getTouches().begin(); i != event.getTouches().end(); i++) {
        if ( activeTouches.find(i->getId()) != activeTouches.end() ) {
            UINodeRef nodeRef = activeTouches[i->getId()];
            nodeRef->touchEnded(*i);
            activeTouches.erase(i->getId());
            mCbUINodeTouchEnded.call(nodeRef);            
        }
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

void UIController::draw()
{
    // NB:- if you override this to draw extra things, 
    //      apply the interface orientation before mTransform
    glPushMatrix();
    glMultMatrixf(mOrientationMatrix);    
    UINode::draw(); // draws children
    glPopMatrix();
}

Matrix44f UIController::getConcatenatedTransform() const
{
    return mOrientationMatrix * mTransform;
}


