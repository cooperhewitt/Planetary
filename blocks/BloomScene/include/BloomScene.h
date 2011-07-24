//
//  BloomScene.h
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include <map>
#include "cinder/Function.h"
#include "BloomNode.h"

// forward declare to avoid full include
namespace cinder { namespace app {
    class AppCocoaTouch;
} }

class BloomScene : public BloomNode {
    
public:
    
    // use static create function and private constructor
    // for reasons why, see:
    // http://www.boost.org/doc/libs/1_47_0/libs/smart_ptr/sp_techniques.html#in_constructor 
    // http://www.boost.org/doc/libs/1_47_0/libs/smart_ptr/weak_ptr.htm#FAQ
    static BloomSceneRef create( ci::app::AppCocoaTouch *app );
    
    virtual ~BloomScene();
    
    ci::Vec2f getInterfaceSize() { return mInterfaceSize; }
    void setInterfaceSize( ci::Vec2f interfaceSize ) { mInterfaceSize = interfaceSize; }
    
    // BloomScene draw/update starts the chain off, very much does *not* draw/update itself :)
    virtual void draw();
    virtual void update();
    
    // override from BloomNode to stop infinite mParent recursion
    virtual ci::Matrix44f getConcatenatedTransform() const;
    
	template<typename T>
    ci::CallbackId registerBloomNodeTouchBegan( T *obj, bool (T::*callback)(BloomNodeRef) )
	{
		return mCbBloomNodeTouchBegan.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}    
	template<typename T>
    ci::CallbackId registerBloomNodeTouchMoved( T *obj, bool (T::*callback)(BloomNodeRef) )
	{
		return mCbBloomNodeTouchMoved.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}    
	template<typename T>
    ci::CallbackId registerBloomNodeTouchEnded( T *obj, bool (T::*callback)(BloomNodeRef) )
	{
		return mCbBloomNodeTouchEnded.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}    
    void unregisterBloomNodeTouchBegan( ci::CallbackId cbId )
	{
		mCbBloomNodeTouchBegan.unregisterCb( cbId );
	}    
    void unregisterBloomNodeTouchMoved( ci::CallbackId cbId )
	{
		mCbBloomNodeTouchMoved.unregisterCb( cbId );
	}    
    void unregisterBloomNodeTouchEnded( ci::CallbackId cbId )
	{
		mCbBloomNodeTouchEnded.unregisterCb( cbId );
	}  
    
    // FIXME: is there a better pattern for this (BloomNode handles own events? bubbling?)
    void onBloomNodeTouchBegan( BloomNodeRef nodeRef )
	{
		mCbBloomNodeTouchBegan.call( nodeRef );
	}    
    void onBloomNodeTouchMoved( BloomNodeRef nodeRef )
	{
		mCbBloomNodeTouchMoved.call( nodeRef );
	}    
    void onBloomNodeTouchEnded( BloomNodeRef nodeRef )
	{
		mCbBloomNodeTouchEnded.call( nodeRef );
	}      
    
protected:

    bool touchesBegan( ci::app::TouchEvent event );
    bool touchesMoved( ci::app::TouchEvent event );
    bool touchesEnded( ci::app::TouchEvent event );
    
    ci::app::AppCocoaTouch *mApp;
    
    ci::Vec2f mInterfaceSize;    
    
    ci::CallbackId cbTouchesBegan, cbTouchesMoved, cbTouchesEnded;
    
	ci::CallbackMgr<bool(BloomNodeRef)> mCbBloomNodeTouchBegan;
	ci::CallbackMgr<bool(BloomNodeRef)> mCbBloomNodeTouchMoved;
	ci::CallbackMgr<bool(BloomNodeRef)> mCbBloomNodeTouchEnded;

    
private:
  
    BloomScene( ci::app::AppCocoaTouch *app );
    
};