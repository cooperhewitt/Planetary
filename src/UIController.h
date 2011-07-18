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
    
    // FIXME: allow null OrientationHelper for desktop use
    UIController(ci::app::AppCocoaTouch *app, ci::app::OrientationHelper *orientationHelper);
    virtual ~UIController();
    
    ci::Vec2f getInterfaceSize() { return mInterfaceSize; }
    void setInterfaceSize( ci::Vec2f interfaceSize ) { mInterfaceSize = interfaceSize; }
    
    ci::app::Orientation getInterfaceOrientation() { return mInterfaceOrientation; }
    void setInterfaceOrientation( const ci::app::Orientation &orientation );
    
    virtual void draw();
    
    // override from UINode to stop infinite mParent recursion
    ci::Matrix44f getConcatenatedTransform() const;
    
	template<typename T>
    ci::CallbackId registerUINodeTouchBegan( T *obj, bool (T::*callback)(UINodeRef) )
	{
		return mCbUINodeTouchBegan.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}    
	template<typename T>
    ci::CallbackId registerUINodeTouchMoved( T *obj, bool (T::*callback)(UINodeRef) )
	{
		return mCbUINodeTouchMoved.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}    
	template<typename T>
    ci::CallbackId registerUINodeTouchEnded( T *obj, bool (T::*callback)(UINodeRef) )
	{
		return mCbUINodeTouchEnded.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}    
    
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

	ci::CallbackMgr<bool(UINodeRef)> mCbUINodeTouchBegan;
	ci::CallbackMgr<bool(UINodeRef)> mCbUINodeTouchMoved;
	ci::CallbackMgr<bool(UINodeRef)> mCbUINodeTouchEnded;
    
};