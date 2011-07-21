//
//  UIController.h
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include <map>
#include "cinder/Function.h"
#include "UINode.h"
#include "Orientation.h"

// forward declare to avoid full include
namespace cinder { namespace app {
    class AppCocoaTouch;
    class OrientationHelper;
    class OrientationEvent;
} }

class UIController : public UINode {
    
public:
    
    // FIXME: allow null OrientationHelper for desktop use
    UIController(ci::app::AppCocoaTouch *app, ci::app::OrientationHelper *orientationHelper);
    virtual ~UIController();
    
    ci::Vec2f getInterfaceSize() { return mInterfaceSize; }
    void setInterfaceSize( ci::Vec2f interfaceSize ) { mInterfaceSize = interfaceSize; }
    
    ci::app::Orientation getInterfaceOrientation() { return mInterfaceOrientation; }
    void setInterfaceOrientation( const ci::app::Orientation &orientation, bool animate = true );
    
    // UIController draw/update starts the chain off, very much does *not* draw/update itself :)
    virtual void draw();
    virtual void update();
    
    // override from UINode to stop infinite mParent recursion
    virtual ci::Matrix44f getConcatenatedTransform() const;
    
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
    void unregisterUINodeTouchBegan( ci::CallbackId cbId )
	{
		mCbUINodeTouchBegan.unregisterCb( cbId );
	}    
    void unregisterUINodeTouchMoved( ci::CallbackId cbId )
	{
		mCbUINodeTouchMoved.unregisterCb( cbId );
	}    
    void unregisterUINodeTouchEnded( ci::CallbackId cbId )
	{
		mCbUINodeTouchEnded.unregisterCb( cbId );
	}  
    
    // FIXME: is there a better pattern for this (UINode handles own events? bubbling?)
    void onUINodeTouchBegan( UINodeRef nodeRef )
	{
		mCbUINodeTouchBegan.call( nodeRef );
	}    
    void onUINodeTouchMoved( UINodeRef nodeRef )
	{
		mCbUINodeTouchMoved.call( nodeRef );
	}    
    void onUINodeTouchEnded( UINodeRef nodeRef )
	{
		mCbUINodeTouchEnded.call( nodeRef );
	}      
    
protected:

    bool touchesBegan( ci::app::TouchEvent event );
    bool touchesMoved( ci::app::TouchEvent event );
    bool touchesEnded( ci::app::TouchEvent event );
	bool orientationChanged( ci::app::OrientationEvent event );
    
    float getOrientationAngle( const ci::app::Orientation &orientation );
    
    ci::app::AppCocoaTouch *mApp;
    ci::app::OrientationHelper *mOrientationHelper;
    
    ci::CallbackId cbTouchesBegan, cbTouchesMoved, cbTouchesEnded, cbOrientationChanged;

    ci::app::Orientation mInterfaceOrientation;
    ci::Matrix44f        mOrientationMatrix;    // animated based on targets
    ci::Vec2f            mInterfaceSize;        // animated based on target
    float                mInterfaceAngle;       // animated, not always right-angle   
    ci::Vec2f            mTargetInterfaceSize;  // depends on mInterfaceOrientation
    float                mTargetInterfaceAngle; // normalized for shortest rotation animation

    // for lerping:
    float mLastOrientationChangeTime;
    float mOrientationAnimationDuration;
    float mPrevInterfaceAngle;
    ci::Vec2f mPrevInterfaceSize;
    
	ci::CallbackMgr<bool(UINodeRef)> mCbUINodeTouchBegan;
	ci::CallbackMgr<bool(UINodeRef)> mCbUINodeTouchMoved;
	ci::CallbackMgr<bool(UINodeRef)> mCbUINodeTouchEnded;
    
};