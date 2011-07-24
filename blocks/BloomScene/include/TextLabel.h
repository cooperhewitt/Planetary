//
//  TextLabel.h
//  Kepler
//
//  Created by Tom Carden on 5/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include <string>
#include "cinder/Font.h"
#include "cinder/Color.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rect.h"
#include "BloomNode.h"

class TextLabel : public BloomNode {
    
public:
    
    TextLabel(const int &id, const ci::Font &font, const ci::Color &color): BloomNode(id), mFont(font), mColor(color) {}
    ~TextLabel() {}
    
    virtual bool touchBegan(ci::app::TouchEvent::Touch touch);
    virtual bool touchEnded(ci::app::TouchEvent::Touch touch);    
    virtual void draw();

    void setText(std::string text);

    void setRect(const ci::Rectf &rect) { mRect = rect; }
    void setRect(const float &x1, const float &y1, const float &x2, const float &y2) { mRect.set(x1,y1,x2,y2); }
    const ci::Rectf& getRect() const { return mRect; }

private:
    
    void updateTexture();
    
    ci::Font mFont;
    ci::Color mColor;
    std::string mText;
    ci::Rectf mRect;

    ci::gl::Texture mTexture;
    
};