//
//  BloomSphere.cpp
//  Kepler
//
//  Created by Tom Carden on 6/12/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#include <vector>
#include "cinder/CinderMath.h"
#include "cinder/Vector.h"
#include "BloomSphere.h"

using namespace ci;
using namespace std;

namespace bloom {

    void BloomSphere::setup( int segments )
    {	
        if (mInited) {
            glDeleteBuffers(1, &mVBO);
        }
        
        mNumVerts = segments * (segments/2) * 2 * 3;
        VertexData *verts = new VertexData[ mNumVerts ];
        
        const float TWO_PI = 2.0f * M_PI;
        
        int vert = 0;
        for( int j = 0; j < segments / 2; j++ ) {
            
            float theta1 = (float)j * TWO_PI / (float)segments - ( M_PI_2 );
            float cosTheta1 = cos( theta1 );
            float sinTheta1 = sin( theta1 );
            
            float theta2 = (float)(j + 1) * TWO_PI / (float)segments - ( M_PI_2 );
            float cosTheta2 = cos( theta2 );
            float sinTheta2 = sin( theta2 );
            
            Vec3f oldv1, oldv2, newv1, newv2;
            Vec2f oldt1, oldt2, newt1, newt2;
            
            for( int i = 0; i <= segments; i++ ) {
                oldv1			= newv1;
                oldv2			= newv2;
                
                oldt1			= newt1;
                oldt2			= newt2;
                
                float invSegs   = 1.0f / (float)segments;
                float theta3	= (float)i * TWO_PI * invSegs;
                float cosTheta3 = cos( theta3 );
                float sinTheta3 = sin( theta3 );
                
                float invI		= (float)i * invSegs;
                float u			= 0.999f - invI;
                float v1		= 0.999f - 2.0f * (float)j * invSegs;
                float v2		= 0.999f - 2.0f * (float)(j+1) * invSegs;
                
                newt1			= Vec2f( u, v1 );
                newt2			= Vec2f( u, v2 );
                
                newv1			= Vec3f( cosTheta1 * cosTheta3, sinTheta1, cosTheta1 * sinTheta3 );			
                newv2			= Vec3f( cosTheta2 * cosTheta3, sinTheta2, cosTheta2 * sinTheta3 );
                
                if( i > 0 ){
                    verts[vert].vertex = oldv1;
                    verts[vert].texture = oldt1;
                    vert++;

                    verts[vert].vertex = oldv2;
                    verts[vert].texture = oldt2;
                    vert++;
                    
                    verts[vert].vertex = newv1;
                    verts[vert].texture = newt1;
                    vert++;

                    verts[vert].vertex = oldv2;
                    verts[vert].texture = oldt2;
                    vert++;
                    
                    verts[vert].vertex = newv2;
                    verts[vert].texture = newt2;
                    vert++;
                    
                    verts[vert].vertex = newv1;
                    verts[vert].texture = newt1;
                    vert++;
                }
            }
        }

        // do VBO (there are more complex ways, let's try this first)
        // (other things to try include VAOs, with glGenVertexArraysOES?)
        glGenBuffers(1, &mVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * mNumVerts, verts, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Leave no VBO bound.        

        delete[] verts;
        
        mInited = true;
    }

    void BloomSphere::draw()
    {
        // here's the old vertex array way...
//      glVertexPointer( 3, GL_FLOAT, sizeof(VertexData), &mVerts[0].vertex );
//      glNormalPointer( GL_FLOAT, sizeof(VertexData), &mVerts[0].vertex );
//      glTexCoordPointer( 2, GL_FLOAT, sizeof(VertexData), &mVerts[0].texture );        
        
        // here's the new vertex buffer way, which needs setting up after mVerts is built in setup
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glVertexPointer( 3, GL_FLOAT, sizeof(VertexData), 0 ); // last arg becomes an offset instead of an address
        glNormalPointer( GL_FLOAT, sizeof(VertexData), 0 );
        glTexCoordPointer( 2, GL_FLOAT, sizeof(VertexData), (GLvoid*)sizeof(Vec3f) );        
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Leave no VBO bound.        

		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glEnableClientState( GL_NORMAL_ARRAY );        
		glDrawArrays( GL_TRIANGLES, 0, mNumVerts );        
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glDisableClientState( GL_NORMAL_ARRAY );
    }
    
}