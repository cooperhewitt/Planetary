//
//  Buttons.cpp
//  Kepler
//
//  Created by Tom Carden on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "cinder/gl/gl.h"
#include "Buttons.h"

void ToggleButton::draw()
{
    Rectf textureRect = mOn ? mOnTextureRect : mOffTextureRect;
    
    Button::drawTextureRect(textureRect);            
}

void SimpleButton::draw()
{
    Rectf textureRect = mDown ? mDownTextureRect : mUpTextureRect;
    
    Button::drawTextureRect(textureRect);      
}

void TwoStateButton::draw()
{
    Rectf textureRect = mOn ? (mDown ? mOnDownTextureRect : mOnUpTextureRect) : (mDown ? mOffDownTextureRect : mOffUpTextureRect) ;
    
    Button::drawTextureRect(textureRect);    
}

// this is just a utility function, no shame in copying it into YourButton::draw() if you need to
void Button::drawTextureRect(Rectf textureRect)
{
    glEnableClientState( GL_VERTEX_ARRAY );
    GLfloat verts[8];
    glVertexPointer( 2, GL_FLOAT, 0, verts );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    GLfloat texCoords[8];
    glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
    int vi = 0;
    int ti = 0;
    verts[vi++] = mRect.getX2(); texCoords[ti++] = textureRect.getX2();
    verts[vi++] = mRect.getY1(); texCoords[ti++] = textureRect.getY1();
    verts[vi++] = mRect.getX1(); texCoords[ti++] = textureRect.getX1();
    verts[vi++] = mRect.getY1(); texCoords[ti++] = textureRect.getY1();
    verts[vi++] = mRect.getX2(); texCoords[ti++] = textureRect.getX2();
    verts[vi++] = mRect.getY2(); texCoords[ti++] = textureRect.getY2();
    verts[vi++] = mRect.getX1(); texCoords[ti++] = textureRect.getX1();
    verts[vi++] = mRect.getY2(); texCoords[ti++] = textureRect.getY2();
    
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );    
}
