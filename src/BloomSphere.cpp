//
//  BloomSphere.cpp
//  Kepler
//
//  Created by Tom Carden on 6/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <vector>
#include "cinder/gl/gl.h"
#include "cinder/CinderMath.h"
#include "cinder/Vector.h"
#include "BloomGl.h" // for Triangle, TODO: make Triangle an inner class or struct
#include "BloomSphere.h"

using namespace ci;
using namespace std;

namespace bloom {

    void BloomSphere::setup( int segments )
    {	
        if (mInited) {
            delete[] mVerts;
            delete[] mNormals;
            delete[] mTexCoords;
        }
        
        mNumVerts		= segments * (segments/2) * 2 * 3;
        mVerts			= new float[ mNumVerts * 3 ];
        mNormals		= new float[ mNumVerts * 3 ];
        mTexCoords		= new float[ mNumVerts * 2 ];
        
        vector<Vec2f> texCoords;
        vector<Triangle> triangles;
        
        const float TWO_PI = 2.0f * M_PI;
        
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
                    triangles.push_back( Triangle( oldv1, oldv2, newv1 ) );
                    triangles.push_back( Triangle( oldv2, newv2, newv1 ) );
                    
                    texCoords.push_back( oldt1 );
                    texCoords.push_back( oldt2 );
                    texCoords.push_back( newt1 );
                    
                    texCoords.push_back( oldt2 );
                    texCoords.push_back( newt2 );
                    texCoords.push_back( newt1 );
                }
            }
        }
        
        
        int index = 0;
        int nIndex = 0;
        for( int i=0; i<triangles.size(); i++ ){
            Triangle t = triangles[i];
            mVerts[index++]		= t.p1.x;
            mVerts[index++]		= t.p1.y;
            mVerts[index++]		= t.p1.z;
            
            mVerts[index++]		= t.p2.x;
            mVerts[index++]		= t.p2.y;
            mVerts[index++]		= t.p2.z;
            
            mVerts[index++]		= t.p3.x;
            mVerts[index++]		= t.p3.y;
            mVerts[index++]		= t.p3.z;
            
            mNormals[nIndex++]	= t.p1.x;
            mNormals[nIndex++]	= t.p1.y;
            mNormals[nIndex++]	= t.p1.z;
            
            mNormals[nIndex++]	= t.p2.x;
            mNormals[nIndex++]	= t.p2.y;
            mNormals[nIndex++]	= t.p2.z;
            
            mNormals[nIndex++]	= t.p3.x;
            mNormals[nIndex++]	= t.p3.y;
            mNormals[nIndex++]	= t.p3.z;
        }
        
        int tIndex = 0;
        for( int i=0; i<texCoords.size(); i++ ){
            mTexCoords[tIndex++]	= texCoords[i].x;
            mTexCoords[tIndex++]	= texCoords[i].y;
        }        
        
        mInited = true;
    }

    void BloomSphere::draw()
    {
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glEnableClientState( GL_NORMAL_ARRAY );
        glVertexPointer( 3, GL_FLOAT, 0, mVerts );
        glTexCoordPointer( 2, GL_FLOAT, 0, mTexCoords );
        glNormalPointer( GL_FLOAT, 0, mNormals );
		glDrawArrays( GL_TRIANGLES, 0, mNumVerts );
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glDisableClientState( GL_NORMAL_ARRAY );        
    }
    
}