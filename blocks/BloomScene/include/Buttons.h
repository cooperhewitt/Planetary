//
//  Buttons.h
//  Kepler
//
//  Created by Tom Carden on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/gl/Texture.h"
#include "BloomNode.h"

// toggle button is either on or off (e.g. show orbits)
class ToggleButton : public BloomNode {
public:
    
    ToggleButton( const int &buttonId, 
                  const bool &on, 
                  const ci::gl::Texture &texture,
                  const ci::Area &onTextureArea, 
                  const ci::Area &offTextureArea ):
        BloomNode(buttonId),
        mOn(on),
        mTexture(texture),
        mOnTextureArea(onTextureArea),
        mOffTextureArea(offTextureArea) {}

    ~ToggleButton() {}
    
    virtual bool touchBegan(ci::app::TouchEvent::Touch touch);
    virtual bool touchEnded(ci::app::TouchEvent::Touch touch);    
    virtual void draw();
    
    bool isOn() { return mOn; }
    void setOn(bool on) { mOn = on; }

    void setRect(const ci::Rectf &rect) { mRect = rect; }
    void setRect(const float &x1, const float &y1, const float &x2, const float &y2) { mRect.set(x1,y1,x2,y2); }
    const ci::Rectf& getRect() const { return mRect; }
    
protected:
    
    bool mOn;
    ci::Area mOnTextureArea, mOffTextureArea;
    ci::gl::Texture mTexture;
    ci::Rectf mRect;
};

// simple button is either being pressed or not (e.g. next, prev)
class SimpleButton : public BloomNode {
public:
    
    SimpleButton( const int &buttonId, 
                 const ci::gl::Texture &texture,
                 const ci::Area &downTextureArea, 
                 const ci::Area &upTextureArea ):
        BloomNode(buttonId),
        mTexture(texture),
        mDownTextureArea(downTextureArea),
        mUpTextureArea(upTextureArea),
        mDownCount(0) {}
    
    ~SimpleButton() {}
    
    virtual bool touchBegan(ci::app::TouchEvent::Touch touch);
    virtual bool touchEnded(ci::app::TouchEvent::Touch touch);    
    virtual void draw();
    
    void setRect(const ci::Rectf &rect) { mRect = rect; }
    void setRect(const float &x1, const float &y1, const float &x2, const float &y2) { mRect.set(x1,y1,x2,y2); }
    const ci::Rectf& getRect() const { return mRect; }
    
protected:
    
    int mDownCount;
    ci::Area mUpTextureArea, mDownTextureArea;
    ci::gl::Texture mTexture;
    ci::Rectf mRect;
    
};

// two-state button is either being pressed or not and has two possible states (e.g. play/pause)
class TwoStateButton : public BloomNode {
public:
    
    TwoStateButton( const int &buttonId, 
                    const bool &on,
                    const ci::gl::Texture &texture,
                    const ci::Area &offDownTextureArea, 
                    const ci::Area &offUpTextureArea,
                    const ci::Area &onDownTextureArea, 
                    const ci::Area &onUpTextureArea ):
        BloomNode(buttonId),
        mOn(on),
        mTexture(texture),
        mDownCount(0),
        mOnDownTextureArea(onDownTextureArea),
        mOnUpTextureArea(onUpTextureArea),
        mOffDownTextureArea(offDownTextureArea),
        mOffUpTextureArea(offUpTextureArea) {}
    
    ~TwoStateButton() {}
        
    bool isOn() { return mOn; }
    void setOn(bool on) { mOn = on; }
    
    virtual bool touchBegan(ci::app::TouchEvent::Touch touch);
    virtual bool touchEnded(ci::app::TouchEvent::Touch touch);    
    virtual void draw();

    void setRect(const ci::Rectf &rect) { mRect = rect; }
    void setRect(const float &x1, const float &y1, const float &x2, const float &y2) { mRect.set(x1,y1,x2,y2); }
    const ci::Rectf& getRect() const { return mRect; }

protected:
    
    int mDownCount;
    bool mOn;
    ci::Area mOnUpTextureArea, mOnDownTextureArea, mOffUpTextureArea, mOffDownTextureArea;
    ci::gl::Texture mTexture;
    ci::Rectf mRect;
    
};

// three-state button has three possible states (e.g. play/pause)
// FIXME: just use an array of texture areas and make it a MultiStateButton
class ThreeStateButton : public BloomNode {
public:
    
    ThreeStateButton( const int &buttonId, 
                      const int &state,
                      const ci::gl::Texture &texture,
                      const ci::Area &firstTextureArea, 
                      const ci::Area &secondTextureArea,
                      const ci::Area &thirdTextureArea ):
        BloomNode(buttonId),
        mState(state),
        mTexture(texture),
        mFirstTextureArea(firstTextureArea),
        mSecondTextureArea(secondTextureArea),
        mThirdTextureArea(thirdTextureArea) {}
    
    ~ThreeStateButton() {}
    
    int getState() const { return mState; }
    void setState(const int &state) { mState = state; }
    
    virtual bool touchBegan(ci::app::TouchEvent::Touch touch);
    virtual bool touchEnded(ci::app::TouchEvent::Touch touch);    
    virtual void draw();
    
    void setRect(const ci::Rectf &rect) { mRect = rect; }
    void setRect(const float &x1, const float &y1, const float &x2, const float &y2) { mRect.set(x1,y1,x2,y2); }
    const ci::Rectf& getRect() const { return mRect; }
    
protected:
    
    int mState;
    ci::Area mFirstTextureArea, mSecondTextureArea, mThirdTextureArea;
    ci::gl::Texture mTexture;
    ci::Rectf mRect;
    
};
