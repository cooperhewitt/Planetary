//
//  Buttons.h
//  Kepler
//
//  Created by Tom Carden on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "cinder/Rect.h"

using namespace ci;

// button is an interface class, draw() is pure virtual
class Button {
public:

    Button() {}
    virtual ~Button() {}    

    void setup(int buttonId, Rectf rect)
    {
        mButtonId = buttonId; 
        mRect = rect;
    }
    
    int getId() { return mButtonId; }

    void setRect(Rectf rect) { mRect = rect; }
    Rectf getRect() { return mRect; }

    virtual void draw() = 0;
    
protected:
    
    void drawTextureRect(Rectf textureRect);
    
    int mButtonId;    
    Rectf mRect;
    
};

// toggle button is either on or off (e.g. show orbits)
class ToggleButton : public Button {
public:
    
    ToggleButton() {}
    ~ToggleButton() {}
    
    void setup(int buttonId, Rectf touchRect, bool on, Rectf onTextureRect, Rectf offTextureRect)
    {
        Button::setup(buttonId, touchRect);
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
class SimpleButton : public Button {
public:
    
    SimpleButton() {}
    ~SimpleButton() {}
    
    void setup(int buttonId, Rectf touchRect, 
               Rectf downTextureRect, Rectf upTextureRect)
    {
        Button::setup(buttonId, touchRect),
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
class TwoStateButton : public Button {
public:
    
    TwoStateButton() {}
    ~TwoStateButton() {}
    
    void setup(int buttonId, Rectf touchRect, bool on, 
               Rectf onDownTextureRect, Rectf onUpTextureRect,
               Rectf offDownTextureRect, Rectf offUpTextureRect)
    {
        Button::setup(buttonId, touchRect),
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


