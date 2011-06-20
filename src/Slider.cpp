//
//  Slider.cpp
//  Kepler
//
//  Created by Tom Carden on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Slider.h"

bool Slider::isDragging() 
{ 
    return mIsDragging; 
}
void Slider::setIsDragging(bool isDragging) 
{
    if (mIsDragging != isDragging) {
        mIsDragging = isDragging; 
        updateVerts(); 
    }
}

float Slider::getValue()
{ 
    return mValue; 
}
void Slider::setValue(float value)
{
    if (mValue != value) {
        mValue = value; 
        updateVerts(); 
    }
}

void Slider::setRect(const Rectf &rect)
{ 
    setRect(rect.x1, rect.y1, rect.x2, rect.y2);
}
void Slider::setRect(const float &x1, const float &y1, const float &x2, const float &y2)
{ 
    mRect.set(x1,y1,x2,y2); 
    updateVerts();    
}

void Slider::setup(int id, 
                   const gl::Texture &texture,               
                   Area bgTexArea, 
                   Area fgTexArea, 
                   Area thumbDownTexArea, 
                   Area thumbUpTexArea)
{
    UIElement::setup(id);
    mTexture = texture;
    // texture Areas:
    mBgTexArea = bgTexArea;
    mFgTexArea = fgTexArea;
    mThumbDownTexArea = thumbDownTexArea;
    mThumbUpTexArea = thumbUpTexArea;
    // state:
    mValue = 0.0f;
    mIsDragging = false;
    // verts:
    mVerts = NULL;
    updateVerts();
}

void Slider::draw()
{
    glVertexPointer( 2, GL_FLOAT, sizeof(VertexData), mVerts );
    glTexCoordPointer( 2, GL_FLOAT, sizeof(VertexData), &mVerts[0].texture );
        
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    mTexture.enableAndBind();
    glDrawArrays( GL_TRIANGLES, 0, mNumVerts );        
    mTexture.disable();
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

void Slider::updateVerts()
{
    if (mVerts == NULL) {
        mNumVerts = 2 * 3 * 3; // 2 triangles, 3 times    
        mVerts = new VertexData[mNumVerts];
    }
    
    // FIXME: is it better to do this with a GL_DYNAMIC_DRAW VBO? (or is this tiny geometry OK?)
        
    const float thumbProgress = (mRect.x2-mRect.x1) * mValue;
    const float midY = (mRect.y1 + mRect.y2) / 2.0f;
    
    const float thumbWidth = 28.0f;
    const float thumbHeight = 28.0f;
    
    Rectf fgRect(mRect.x1, mRect.y1, mRect.x1 + thumbProgress, mRect.y2);
    Rectf thumbRect(mRect.x1 + thumbProgress - thumbWidth/2.0f, 
                    midY - thumbHeight/2.0, 
                    mRect.x1 + thumbProgress + thumbWidth/2.0f, 
                    midY + thumbHeight/2.0);
    
    Area thumbTexArea = mIsDragging ? mThumbDownTexArea : mThumbUpTexArea; 
    
    const float iw = 1.0f / mTexture.getWidth();
    const float ih = 1.0f /mTexture.getHeight();
    
    int vert = 0;
    // gl::draw(mTexture, mBgTexArea, mRect);        
    mVerts[vert].vertex  = Vec2f(mRect.x1, mRect.y1);
    mVerts[vert].texture = Vec2f(mBgTexArea.x1 * iw, mBgTexArea.y1 * ih); 
    vert++;
    mVerts[vert].vertex  = Vec2f(mRect.x2, mRect.y1);
    mVerts[vert].texture = Vec2f(mBgTexArea.x2 * iw, mBgTexArea.y1 * ih); 
    vert++;
    mVerts[vert].vertex  = Vec2f(mRect.x2, mRect.y2);
    mVerts[vert].texture = Vec2f(mBgTexArea.x2 * iw, mBgTexArea.y2 * ih); 
    vert++;
    mVerts[vert].vertex  = Vec2f(mRect.x1, mRect.y1);
    mVerts[vert].texture = Vec2f(mBgTexArea.x1 * iw, mBgTexArea.y1 * ih); 
    vert++;
    mVerts[vert].vertex  = Vec2f(mRect.x1, mRect.y2);
    mVerts[vert].texture = Vec2f(mBgTexArea.x1 * iw, mBgTexArea.y2 * ih); 
    vert++;
    mVerts[vert].vertex  = Vec2f(mRect.x2, mRect.y2);
    mVerts[vert].texture = Vec2f(mBgTexArea.x2 * iw, mBgTexArea.y2 * ih); 
    vert++;
    
    // gl::draw(mTexture, mFgTexArea, fgRect);
    mVerts[vert].vertex  = Vec2f(fgRect.x1, fgRect.y1);
    mVerts[vert].texture = Vec2f(mFgTexArea.x1 * iw, mFgTexArea.y1 * ih); 
    vert++;
    mVerts[vert].vertex  = Vec2f(fgRect.x2, fgRect.y1);
    mVerts[vert].texture = Vec2f(mFgTexArea.x2 * iw, mFgTexArea.y1 * ih); 
    vert++;
    mVerts[vert].vertex  = Vec2f(fgRect.x2, fgRect.y2);
    mVerts[vert].texture = Vec2f(mFgTexArea.x2 * iw, mFgTexArea.y2 * ih); 
    vert++;
    mVerts[vert].vertex  = Vec2f(fgRect.x1, fgRect.y1);
    mVerts[vert].texture = Vec2f(mFgTexArea.x1 * iw, mFgTexArea.y1 * ih); 
    vert++;
    mVerts[vert].vertex  = Vec2f(fgRect.x1, fgRect.y2);
    mVerts[vert].texture = Vec2f(mFgTexArea.x1 * iw, mFgTexArea.y2 * ih); 
    vert++;
    mVerts[vert].vertex  = Vec2f(fgRect.x2, fgRect.y2);
    mVerts[vert].texture = Vec2f(mFgTexArea.x2 * iw, mFgTexArea.y2 * ih); 
    vert++;
    
    // gl::draw(mTexture, thumbTexArea, thumbRect);    
    mVerts[vert].vertex  = Vec2f(thumbRect.x1, thumbRect.y1);
    mVerts[vert].texture = Vec2f(thumbTexArea.x1 * iw, thumbTexArea.y1 * ih); 
    vert++;
    mVerts[vert].vertex  = Vec2f(thumbRect.x2, thumbRect.y1);
    mVerts[vert].texture = Vec2f(thumbTexArea.x2 * iw, thumbTexArea.y1 * ih); 
    vert++;
    mVerts[vert].vertex  = Vec2f(thumbRect.x2, thumbRect.y2);
    mVerts[vert].texture = Vec2f(thumbTexArea.x2 * iw, thumbTexArea.y2 * ih); 
    vert++;
    mVerts[vert].vertex  = Vec2f(thumbRect.x1, thumbRect.y1);
    mVerts[vert].texture = Vec2f(thumbTexArea.x1 * iw, thumbTexArea.y1 * ih); 
    vert++;
    mVerts[vert].vertex  = Vec2f(thumbRect.x1, thumbRect.y2);
    mVerts[vert].texture = Vec2f(thumbTexArea.x1 * iw, thumbTexArea.y2 * ih); 
    vert++;
    mVerts[vert].vertex  = Vec2f(thumbRect.x2, thumbRect.y2);
    mVerts[vert].texture = Vec2f(thumbTexArea.x2 * iw, thumbTexArea.y2 * ih); 
    vert++;    
}

