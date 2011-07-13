//
//  UIElement.h
//  Kepler
//
//  Created by Tom Carden on 5/29/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/Rect.h"
#include "cinder/app/TouchEvent.h"

// UIElement is an "abstract" class: draw() is pure virtual
class UIElement {
public:
    
    UIElement() {}
    virtual ~UIElement() {}    
    
    void setup(const int &id)
    {
        mId = id;
    }
    
    const int& getId() const { return mId; }
    
    virtual void setRect(const ci::Rectf &rect) { mRect = rect; }
    virtual void setRect(const float &x1, const float &y1, const float &x2, const float &y2) { mRect.set(x1,y1,x2,y2); }
    const ci::Rectf& getRect() const { return mRect; }
    
    virtual void draw() = 0;
    
//    virtual bool touchBegan( ci::app::TouchEvent::Touch touch ) { return false; };
//    virtual bool touchMoved( ci::app::TouchEvent::Touch touch ) { return false; };
//    virtual bool touchEnded( ci::app::TouchEvent::Touch touch ) { return false; };
    
protected:
    
    int mId;    
    ci::Rectf mRect;
    
};
