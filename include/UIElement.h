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
    Rectf getRect() { return mRect; }
    
    virtual void draw() = 0;
    
protected:
    
    void drawTextureRect(Rectf textureRect);
    
    int mId;    
    Rectf mRect;
    
};
