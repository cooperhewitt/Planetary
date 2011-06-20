//
//  Buttons.h
//  Kepler
//
//  Created by Tom Carden on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/gl/Texture.h"
#include "UIElement.h"

using namespace ci;

// toggle button is either on or off (e.g. show orbits)
class ToggleButton : public UIElement {
public:
    
    ToggleButton() {}
    ~ToggleButton() {}
    
    void setup(int buttonId, 
               bool on, 
               const gl::Texture &texture,
               Area onTextureArea, 
               Area offTextureArea)
    {
        UIElement::setup(buttonId);
        mOn = on;
        mTexture = texture;
        mOnTextureArea = onTextureArea;
        mOffTextureArea = offTextureArea; 
    }
    
    virtual void draw();
    
    bool isOn() { return mOn; }
    void setOn(bool on) { mOn = on; }
    
protected:
    
    bool mOn;
    Area mOnTextureArea, mOffTextureArea;
    gl::Texture mTexture;
};

// simple button is either being pressed or not (e.g. next, prev)
class SimpleButton : public UIElement {
public:
    
    SimpleButton() {}
    ~SimpleButton() {}
    
    void setup(int buttonId, 
               const gl::Texture &texture,
               Area downTextureArea, 
               Area upTextureArea)
    {
        UIElement::setup(buttonId),
        mTexture = texture;
        mDown = false;
        mDownTextureArea = downTextureArea;
        mUpTextureArea = upTextureArea;
    }

    bool isDown() { return mDown; }
    void setDown(bool down) { mDown = down; }
    
    virtual void draw();
    
protected:
    
    bool mDown;
    Area mUpTextureArea, mDownTextureArea;
    gl::Texture mTexture;
    
};

// two-state button is either being pressed or not and has two possible states (e.g. play/pause)
class TwoStateButton : public UIElement {
public:
    
    TwoStateButton() {}
    ~TwoStateButton() {}
    
    void setup(int buttonId, 
               bool on, 
               const gl::Texture &texture,               
               Area offDownTextureArea, Area offUpTextureArea,
               Area onDownTextureArea, Area onUpTextureArea)
    {
        UIElement::setup(buttonId),
        mOn = on;
        mTexture = texture;
        mDown = false;
        mOnDownTextureArea = onDownTextureArea;
        mOnUpTextureArea = onUpTextureArea;
        mOffDownTextureArea = offDownTextureArea;
        mOffUpTextureArea = offUpTextureArea;
    }
    
    bool isDown() { return mDown; }
    void setDown(bool down) { mDown = down; }
    bool isOn() { return mOn; }
    void setOn(bool on) { mOn = on; }
    
    virtual void draw();
    
protected:
    
    bool mDown, mOn;
    Area mOnUpTextureArea, mOnDownTextureArea, mOffUpTextureArea, mOffDownTextureArea;
    gl::Texture mTexture;
    
};
