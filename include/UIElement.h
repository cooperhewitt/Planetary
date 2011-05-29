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

// UIElement is an interface class, draw() is pure virtual
class UIElement {
public:
    
    UIElement() {}
    virtual ~UIElement() {}    
    
    void setup(int id)
    {
        mId = id;
    }
    
    int getId() { return mId; }
    
    void setRect(Rectf rect) { mRect = rect; }
    void setRect(const float &x1, const float &y1, const float &x2, const float &y2) { mRect.set(x1,y1,x2,y2); }
    Rectf getRect() { return mRect; }
    
    virtual void draw() = 0;
    
protected:
    
    void drawTextureRect(Rectf textureRect);
    
    int mId;    
    Rectf mRect;
    
};
