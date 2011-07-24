//
//  TextureRect.h
//  Kepler
//
//  Created by Tom Carden on 7/18/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/gl/Texture.h"
#include "cinder/Area.h"
#include "cinder/Rect.h"
#include "BloomNode.h"

class TextureRect : public BloomNode
{
public:
    
    TextureRect( const ci::gl::Texture &texture, const ci::Area &area ): mTexture(texture), mArea(area) {}
    
    virtual void draw();
    
    void setRect(const ci::Rectf &rect) { mRect = rect; }
    void setRect(const float &x1, const float &y1, const float &x2, const float &y2) { mRect.set(x1,y1,x2,y2); }
    const ci::Rectf& getRect() const { return mRect; }

private:
    
    ci::gl::Texture mTexture;
    ci::Area mArea;
    ci::Rectf mRect;
};