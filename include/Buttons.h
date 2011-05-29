//
//  Buttons.h
//  Kepler
//
//  Created by Tom Carden on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "UIElement.h"

using namespace ci;

// toggle button is either on or off (e.g. show orbits)
class ToggleButton : public UIElement {
public:
    
    ToggleButton() {}
    ~ToggleButton() {}
    
    void setup(int buttonId, 
               bool on, 
               Rectf onTextureRect, 
               Rectf offTextureRect)
    {
        UIElement::setup(buttonId);
        mOn = on;
        mOnTextureRect = onTextureRect;
        mOffTextureRect = offTextureRect; 
    }
    
    virtual void draw();
    
    bool isOn() { return mOn; }
    void setOn(bool on) { mOn = on; }
    
protected:
    
    bool mOn;
    Rectf mOnTextureRect, mOffTextureRect;
    
};

// simple button is either being pressed or not (e.g. next, prev)
class SimpleButton : public UIElement {
public:
    
    SimpleButton() {}
    ~SimpleButton() {}
    
    void setup(int buttonId, 
               Rectf downTextureRect, 
               Rectf upTextureRect)
    {
        UIElement::setup(buttonId),
        mDown = false;
        mDownTextureRect = downTextureRect;
        mUpTextureRect = upTextureRect;
    }

    bool isDown() { return mDown; }
    void setDown(bool down) { mDown = down; }
    
    virtual void draw();
    
protected:
    
    bool mDown;
    Rectf mUpTextureRect, mDownTextureRect;
    
};

// two-state button is either being pressed or not and has two possible states (e.g. play/pause)
class TwoStateButton : public UIElement {
public:
    
    TwoStateButton() {}
    ~TwoStateButton() {}
    
    void setup(int buttonId, 
               bool on, 
               Rectf offDownTextureRect, Rectf offUpTextureRect,
               Rectf onDownTextureRect, Rectf onUpTextureRect)
    {
        UIElement::setup(buttonId),
        mOn = on;
        mDown = false;
        mOnDownTextureRect = onDownTextureRect;
        mOnUpTextureRect = onUpTextureRect;
        mOffDownTextureRect = offDownTextureRect;
        mOffUpTextureRect = offUpTextureRect;
    }
    
    bool isDown() { return mDown; }
    void setDown(bool down) { mDown = down; }
    bool isOn() { return mOn; }
    void setOn(bool on) { mOn = on; }
    
    virtual void draw();
    
protected:
    
    bool mDown, mOn;
    Rectf mOnUpTextureRect, mOnDownTextureRect, mOffUpTextureRect, mOffDownTextureRect;
    
};


