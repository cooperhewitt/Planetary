//
//  Slider.h
//  Kepler
//
//  Created by Tom Carden on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "BloomNode.h"
#include "cinder/Area.h"
#include "cinder/Rect.h"
#include "cinder/gl/Texture.h"

class Slider : public BloomNode {
public:
    
    Slider(const int &id, 
           const ci::gl::Texture &texture,               
           const ci::Area &bgTexArea, 
           const ci::Area &fgTexArea, 
           const ci::Area &thumbDownTexArea, 
           const ci::Area &thumbUpTexArea): 
        BloomNode(id), 
        mTexture(texture),
        // texture Areas:
        mBgTexArea(bgTexArea),
        mFgTexArea(fgTexArea),
        mThumbDownTexArea(thumbDownTexArea),
        mThumbUpTexArea(thumbUpTexArea),
        // state:
        mValue(0.0f),
        mIsDragging(false) {}

    ~Slider() {}
    
    bool isDragging();
    void setIsDragging(bool isDragging);

    float getValue();
    void setValue(float value);

    virtual bool touchBegan(ci::app::TouchEvent::Touch touch);
    virtual bool touchMoved(ci::app::TouchEvent::Touch touch);
    virtual bool touchEnded(ci::app::TouchEvent::Touch touch);
    virtual void draw();
    
    void setRect(const ci::Rectf &rect) { mRect = rect; }
    void setRect(const float &x1, const float &y1, const float &x2, const float &y2) { mRect.set(x1,y1,x2,y2); }
    const ci::Rectf& getRect() const { return mRect; }
    
protected:

    ci::Area mFgTexArea, mBgTexArea, mThumbDownTexArea, mThumbUpTexArea; // texture coords, fixed
    ci::gl::Texture mTexture;
    ci::Rectf mRect;
    
    float mValue;
    bool mIsDragging;
    
    void updateVerts();    
};