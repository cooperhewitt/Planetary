//
//  GlExtras.cpp
//  Kepler
//
//  Created by Robert Hodgin on 4/7/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#include <boost/foreach.hpp>
#include "BloomGl.h"
#include "cinder/Quaternion.h"

using namespace ci;

namespace bloom { namespace gl {
    
//	void drawBillboard( const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp )
//	{
//		glEnableClientState( GL_VERTEX_ARRAY );
//		Vec3f verts[4];
//		glVertexPointer( 3, GL_FLOAT, 0, &verts[0].x );
//		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
//		GLfloat texCoords[8] = { 0, 0, 0, 1, 1, 0, 1, 1 };
//		glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
//		
//		float sinA = math<float>::sin( toRadians( rotationDegrees ) );
//		float cosA = math<float>::cos( toRadians( rotationDegrees ) );
//		
//		verts[0] = pos + bbRight * ( -0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
//		verts[1] = pos + bbRight * ( -0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + -0.5f * cosA * scale.y );
//		verts[2] = pos + bbRight * ( 0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
//		verts[3] = pos + bbRight * ( 0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + -0.5f * cosA * scale.y );
//		
//		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
//		
//		glDisableClientState( GL_VERTEX_ARRAY );
//		glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
//	}



	void drawBillboard( const Vec3f &pos, const Vec2f &scale, float rotInRadians, const Vec3f &bbRight, const Vec3f &bbUp )
	{
		glEnableClientState( GL_VERTEX_ARRAY );
		Vec3f verts[4];
		glVertexPointer( 3, GL_FLOAT, 0, &verts[0].x );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		GLfloat texCoords[8] = { 0, 0, 0, 1, 1, 0, 1, 1 };
		glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
		
		float sinA = math<float>::sin( rotInRadians );
		float cosA = math<float>::cos( rotInRadians );
		
		float scaleXCosA = 0.5f * scale.x * cosA;
		float scaleXSinA = 0.5f * scale.x * sinA;
		float scaleYSinA = 0.5f * scale.y * sinA;
		float scaleYCosA = 0.5f * scale.y * cosA;
		verts[0] = pos + bbRight * ( -scaleXCosA - scaleYSinA ) + bbUp * ( -scaleXSinA + scaleYCosA );
		verts[1] = pos + bbRight * ( -scaleXCosA + scaleYSinA ) + bbUp * ( -scaleXSinA - scaleYCosA );
		verts[2] = pos + bbRight * (  scaleXCosA - scaleYSinA ) + bbUp * (  scaleXSinA + scaleYCosA );
		verts[3] = pos + bbRight * (  scaleXCosA + scaleYSinA ) + bbUp * (  scaleXSinA - scaleYCosA );
		
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
	}


	void drawSphericalBillboard( const Vec3f &camEye, const Vec3f &objPos, const Vec2f &scale, float rotInRadians )
	{	
		glPushMatrix();
		glTranslatef( objPos.x, objPos.y, objPos.z );
		
		Vec3f lookAt = Vec3f::zAxis();
		Vec3f upAux;
		float angleCosine;
		
		Vec3f objToCam = ( camEye - objPos ).normalized();
		Vec3f objToCamProj = Vec3f( objToCam.x, 0.0f, objToCam.z );
		objToCamProj.normalize();
		
		upAux = lookAt.cross( objToCamProj );

// Cylindrical billboarding
		angleCosine = constrain( lookAt.dot( objToCamProj ), -1.0f, 1.0f );
		glRotatef( toDegrees( acos(angleCosine) ), upAux.x, upAux.y, upAux.z );	
		
// Spherical billboarding
		angleCosine = constrain( objToCamProj.dot( objToCam ), -1.0f, 1.0f );
		if( objToCam.y < 0 )	glRotatef( toDegrees( acos(angleCosine) ), 1.0f, 0.0f, 0.0f );	
		else					glRotatef( toDegrees( acos(angleCosine) ),-1.0f, 0.0f, 0.0f );
		
		
		Vec3f verts[4];
		GLfloat texCoords[8] = { 0, 0, 0, 1, 1, 0, 1, 1 };
		
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, &verts[0].x );
		glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
		
		float sinA = math<float>::sin( rotInRadians );
		float cosA = math<float>::cos( rotInRadians );
		
		float scaleXCosA = 0.5f * scale.x * cosA;
		float scaleXSinA = 0.5f * scale.x * sinA;
		float scaleYSinA = 0.5f * scale.y * sinA;
		float scaleYCosA = 0.5f * scale.y * cosA;
		
		verts[0] = Vec3f( ( -scaleXCosA - scaleYSinA ), ( -scaleXSinA + scaleYCosA ), 0.0f );
		verts[1] = Vec3f( ( -scaleXCosA + scaleYSinA ), ( -scaleXSinA - scaleYCosA ), 0.0f );
		verts[2] = Vec3f( (  scaleXCosA - scaleYSinA ), (  scaleXSinA + scaleYCosA ), 0.0f );
		verts[3] = Vec3f( (  scaleXCosA + scaleYSinA ), (  scaleXSinA - scaleYCosA ), 0.0f );

		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
		
		
//		glDisable( GL_TEXTURE_2D );
//		ci::gl::color( Color( 1.0f, 1.0f, 1.0f ) );
//		ci::gl::drawLine( Vec3f::zero(), objToCam );
//		glEnable( GL_TEXTURE_2D );
		
		glPopMatrix();
	}

    void drawSphericalRotatedBillboard( const ci::Vec3f &pos, const ci::Vec3f &lookAt, const ci::Vec3f &turnAt, const ci::Vec2f &scale )
    {
        glPushMatrix();

        // hacked together from three.js's Matrix4.lookAt...
        
		Vec3f z = ( pos - lookAt ).normalized();
        
		if ( z.length() == 0 ) {
			z.z = 1;
		}
        
        Vec3f up = turnAt - pos;
        
		Vec3f x = up.cross(z).normalized();
        
		if ( x.length() == 0 ) {
			z.x += 0.0001;
			x = up.cross(z).normalized();
		}
        
        Vec3f y = z.cross(x).normalized();
    
        float m[16];
        m[ 0] = x.x; m[ 4] = y.x; m[ 8] = z.x; m[12] = pos.x;
        m[ 1] = x.y; m[ 5] = y.y; m[ 9] = z.y; m[13] = pos.y;
        m[ 2] = x.z; m[ 6] = y.z; m[10] = z.z; m[14] = pos.z;
        m[ 3] = 0;   m[ 7] = 0;   m[11] = 0;   m[15] = 1;
            
        glMultMatrixf(m);
        
        ///////////////// and now we just get to draw a square
        // ... might be worth pre-multiplying the verts to avoid the push/mult/pop entirely?
        // ... or setting these up as a VBO since they're always the same
        // ... or batching everything that shares a texture into one billboard array
        
        ci::Vec2f verts[4];
		GLfloat texCoords[8] = { 0, 0, 0, 1, 1, 0, 1, 1 };
		
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glVertexPointer( 2, GL_FLOAT, 0, &verts[0].x );
		glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
				
		verts[0] = ci::Vec2f(-0.5f,-0.5f) * scale;
		verts[1] = ci::Vec2f(-0.5f, 0.5f) * scale;
		verts[2] = ci::Vec2f( 0.5f,-0.5f) * scale;
		verts[3] = ci::Vec2f( 0.5f, 0.5f) * scale;
        
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );	        
        
        glPopMatrix();
    }
    
    /////////////////////////////////////////////////////////

    // FIXME: whither ordered_map?
    boost::unordered_map<GLuint, BatchRef> batchByTex;
    std::vector<BatchRef> batches;
    
    void beginBatch()
    {
        batchByTex.clear();
        batches.clear();
    }
    
    void batchRect( const ci::gl::Texture &texture, const ci::Rectf &srcRect, const ci::Rectf &dstRect )
    {
        GLuint texId = texture.getId();
        boost::unordered_map<GLuint, BatchRef>::iterator iter = batchByTex.find( texId );
        BatchRef batch;
        if (iter != batchByTex.end()) {
            batch = iter->second;
        }
        else {
            batch = BatchRef(new Batch());
            batch->texture = texture;
            batches.push_back(batch);
            batchByTex[texId] = batch;
        }
        int verts = batch->vertices.size();
        batch->vertices.resize(verts + 6);
        batch->vertices[verts].vertex  = ci::Vec2f(dstRect.x1, dstRect.y1);
        batch->vertices[verts].texture = ci::Vec2f(srcRect.x1, srcRect.y1);
        verts++;
        batch->vertices[verts].vertex  = ci::Vec2f(dstRect.x2, dstRect.y1);
        batch->vertices[verts].texture = ci::Vec2f(srcRect.x2, srcRect.y1); 
        verts++;
        batch->vertices[verts].vertex  = ci::Vec2f(dstRect.x2, dstRect.y2);
        batch->vertices[verts].texture = ci::Vec2f(srcRect.x2, srcRect.y2); 
        verts++;
        batch->vertices[verts].vertex  = ci::Vec2f(dstRect.x1, dstRect.y1);
        batch->vertices[verts].texture = ci::Vec2f(srcRect.x1, srcRect.y1); 
        verts++;
        batch->vertices[verts].vertex  = ci::Vec2f(dstRect.x1, dstRect.y2);
        batch->vertices[verts].texture = ci::Vec2f(srcRect.x1, srcRect.y2); 
        verts++;
        batch->vertices[verts].vertex  = ci::Vec2f(dstRect.x2, dstRect.y2);
        batch->vertices[verts].texture = ci::Vec2f(srcRect.x2, srcRect.y2); 
        //verts++;        
    }
    
    void batchRect( const ci::gl::Texture &texture, const ci::Area &srcArea, const ci::Rectf &dstRect )
    {
        batchRect( texture, texture.getAreaTexCoords( srcArea ), dstRect );
    }

    void batchRect( const ci::gl::Texture &texture, const ci::Vec2f &pos )
    {
        batchRect( texture, texture.getCleanBounds(), ci::Rectf(pos.x, pos.y, pos.x + texture.getWidth(), pos.y + texture.getHeight()) );        
    }
    
    void endBatch()
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        BOOST_FOREACH(BatchRef batch, batches) {
            batch->texture.enableAndBind();
            glVertexPointer(2, GL_FLOAT, sizeof(VertexData), &batch->vertices[0].vertex);
            glTexCoordPointer(2, GL_FLOAT, sizeof(VertexData), &batch->vertices[0].texture);
            glDrawArrays(GL_TRIANGLES, 0, batch->vertices.size());
            batch->texture.disable();
        }
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

} }