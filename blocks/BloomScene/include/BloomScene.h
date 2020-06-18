//
//  BloomScene.h
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
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
    void draw();
    
    bool touchesBegan( ci::app::TouchEvent event );
    bool touchesMoved( ci::app::TouchEvent event );
    bool touchesEnded( ci::app::TouchEvent event );
    
protected:

    
    ci::app::AppCocoaTouch *mApp;
    
    ci::Vec2f mInterfaceSize;    
    
    ci::CallbackId cbTouchesBegan, cbTouchesMoved, cbTouchesEnded;
    
private:
  
    BloomScene( ci::app::AppCocoaTouch *app );
    
};
