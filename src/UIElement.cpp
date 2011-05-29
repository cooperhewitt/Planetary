//
//  UIElement.cpp
//  Kepler
//
//  Created by Tom Carden on 5/29/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "UIElement.h"
#include "cinder/gl/gl.h"

// this is just a utility function, no shame in copying it into YourButton::draw() if you need to
// FIXME: there must be a clean way to do this with Cinder already?
// e.g. draw(Texture, uvRect, screenRect) - no?
void UIElement::drawTextureRect(Rectf textureRect)
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
