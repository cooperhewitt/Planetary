//
//  Slider.cpp
//  Kepler
//
//  Created by Tom Carden on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Slider.h"

// assumes texture is already bound
void Slider::draw()
{
    float thumbProgress = (mRect.x2-mRect.x1) * mValue;
    float midY = (mRect.y1 + mRect.y2) / 2.0f;
    
    float thumbWidth = 28.0f;
    float thumbHeight = 28.0f;
    
    Rectf fgRect(mRect.x1, mRect.y1, mRect.x1 + thumbProgress, mRect.y2);
    Rectf thumbRect(mRect.x1 + thumbProgress - thumbWidth/2.0f, 
                    midY - thumbHeight/2.0, 
                    mRect.x1 + thumbProgress + thumbWidth/2.0f, 
                    midY + thumbHeight/2.0);

    Rectf thumbTexRect = mIsDragging ? mThumbDownTexRect : mThumbUpTexRect;
    
    drawTextureRect(mRect, mBgTexRect);
    drawTextureRect(fgRect, mFgTexRect);
    drawTextureRect(thumbRect, thumbTexRect);
}

// FIXME: are we sure this isn't a Cinder function already?
// ... should it be?
void Slider::drawTextureRect(Rectf rect, Rectf textureRect)
{
    glEnableClientState( GL_VERTEX_ARRAY );
    GLfloat verts[8];
    glVertexPointer( 2, GL_FLOAT, 0, verts );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    GLfloat texCoords[8];
    glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
    int vi = 0;
    int ti = 0;
    verts[vi++] = rect.getX2(); texCoords[ti++] = textureRect.getX2();
    verts[vi++] = rect.getY1(); texCoords[ti++] = textureRect.getY1();
    verts[vi++] = rect.getX1(); texCoords[ti++] = textureRect.getX1();
    verts[vi++] = rect.getY1(); texCoords[ti++] = textureRect.getY1();
    verts[vi++] = rect.getX2(); texCoords[ti++] = textureRect.getX2();
    verts[vi++] = rect.getY2(); texCoords[ti++] = textureRect.getY2();
    verts[vi++] = rect.getX1(); texCoords[ti++] = textureRect.getX1();
    verts[vi++] = rect.getY2(); texCoords[ti++] = textureRect.getY2();
    
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );     
}
