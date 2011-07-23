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

// forward declare to avoid full include
namespace cinder { namespace app {
    class AppCocoaTouch;
} }

class UIController : public UINode {
    
public:
    
    UIController( ci::app::AppCocoaTouch *app );
    virtual ~UIController();
    
    ci::Vec2f getInterfaceSize() { return mInterfaceSize; }
    void setInterfaceSize( ci::Vec2f interfaceSize ) { mInterfaceSize = interfaceSize; }
    
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
    
    ci::app::AppCocoaTouch *mApp;
    
    ci::Vec2f mInterfaceSize;    
    
    ci::CallbackId cbTouchesBegan, cbTouchesMoved, cbTouchesEnded;
    
	ci::CallbackMgr<bool(UINodeRef)> mCbUINodeTouchBegan;
	ci::CallbackMgr<bool(UINodeRef)> mCbUINodeTouchMoved;
	ci::CallbackMgr<bool(UINodeRef)> mCbUINodeTouchEnded;
    
};