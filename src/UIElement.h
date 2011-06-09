//
//  UIElement.h
//  Kepler
//
//  Created by Tom Carden on 5/29/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/Rect.h"

using namespace ci;

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
    
    void setRect(const Rectf &rect) { mRect = rect; }
    void setRect(const float &x1, const float &y1, const float &x2, const float &y2) { mRect.set(x1,y1,x2,y2); }
    const Rectf& getRect() const { return mRect; }
    
    virtual void draw() = 0;
    
protected:
    
    int mId;    
    Rectf mRect;
    
};
