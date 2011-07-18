//
//  UIController.h
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include <map>
#include "cinder/app/AppCocoaTouch.h"
#include "OrientationHelper.h"
#include "UINode.h"

class UIController : public UINode {
    
public:
    
    UIController(ci::app::AppCocoaTouch *app, ci::app::OrientationHelper *orientationHelper);
    virtual ~UIController();
    
    ci::Vec2f getInterfaceSize() { return mInterfaceSize; }
    ci::app::Orientation getInterfaceOrientation() { return mInterfaceOrientation; }
    
    virtual void draw();
    
    // override from UINode to stop infinite mParent recursion
    ci::Matrix44f getConcatenatedTransform() const;
    
protected:

    bool touchesBegan( ci::app::TouchEvent event );
    bool touchesMoved( ci::app::TouchEvent event );
    bool touchesEnded( ci::app::TouchEvent event );
	bool orientationChanged( ci::app::OrientationEvent event );

    ci::app::AppCocoaTouch *mApp;
    ci::app::OrientationHelper *mOrientationHelper;
    
    ci::CallbackId cbTouchesBegan, cbTouchesMoved, cbTouchesEnded, cbOrientationChanged;

    ci::app::Orientation mInterfaceOrientation;
    ci::Matrix44f mOrientationMatrix;
    ci::Vec2f mInterfaceSize;
    
    std::map<uint64_t, UINodeRef> activeTouches;
    
};