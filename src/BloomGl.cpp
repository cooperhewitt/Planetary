//
//  GlExtras.cpp
//  Kepler
//
//  Created by Robert Hodgin on 4/7/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "BloomGl.h"

<<<<<<< HEAD

using namespace ci;


void drawBillboard( const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp )
{
	glEnableClientState( GL_VERTEX_ARRAY );
	Vec3f verts[4];
	glVertexPointer( 3, GL_FLOAT, 0, &verts[0].x );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	GLfloat texCoords[8] = { 0, 0, 0, 1, 1, 0, 1, 1 };
	glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
	
	float sinA = math<float>::sin( toRadians( rotationDegrees ) );
	float cosA = math<float>::cos( toRadians( rotationDegrees ) );
	
	verts[0] = pos + bbRight * ( -0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
	verts[1] = pos + bbRight * ( -0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + -0.5f * cosA * scale.y );
	verts[2] = pos + bbRight * ( 0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
	verts[3] = pos + bbRight * ( 0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + -0.5f * cosA * scale.y );
	
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
}



void drawBillboardKepler( const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp )
{
	glEnableClientState( GL_VERTEX_ARRAY );
	Vec3f verts[4];
	glVertexPointer( 3, GL_FLOAT, 0, &verts[0].x );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	GLfloat texCoords[8] = { 0, 0, 0, 1, 1, 0, 1, 1 };
	glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
	
	float sinA = math<float>::sin( toRadians( rotationDegrees ) );
	float cosA = math<float>::cos( toRadians( rotationDegrees ) );
	
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




void drawSphericalBillboardKepler( const Vec3f &camEye, const Vec3f &objPos, const Vec2f &scale, float rotationDegrees )
{	
	Vec3f objToCamProj;
	Vec3f lookAt;
	Vec3f objToCam;
	Vec3f upAux;
	
	float angleCosine;
	
	objToCamProj.x = camEye.x - objPos.x ;
	objToCamProj.y = 0;
	objToCamProj.z = camEye.z - objPos.z ;
	objToCamProj.normalize();
	
	lookAt = Vec3f::zAxis();
	upAux = lookAt.cross( objToCamProj );
	
	angleCosine = lookAt.dot( objToCamProj );
	
	if( (angleCosine < 0.99990 ) && ( angleCosine > -0.9999 ) )
		glRotatef( acos(angleCosine)*180/3.14, upAux.x, upAux.y, upAux.z );	
	
	objToCam = ( camEye - objPos ).normalized();
	
	angleCosine = objToCamProj.dot( objToCam );
	
	if( ( angleCosine < 0.99990 ) && ( angleCosine > -0.9999 ) ){
		if (objToCam[1] < 0)
			glRotatef( acos(angleCosine)*180.0f/M_PI, 1.0f, 0.0f, 0.0f );	
		else
			glRotatef( acos(angleCosine)*180.0f/M_PI,-1.0f, 0.0f, 0.0f );
	}
	
	
	
	
	glEnableClientState( GL_VERTEX_ARRAY );
	Vec3f verts[4];
	glVertexPointer( 3, GL_FLOAT, 0, &verts[0].x );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	GLfloat texCoords[8] = { 0, 0, 0, 1, 1, 0, 1, 1 };
	glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
	
//	float sinA = math<float>::sin( toRadians( rotationDegrees ) );
//	float cosA = math<float>::cos( toRadians( rotationDegrees ) );
	
//	verts[0] = pos + bbRight * ( -0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
//	verts[1] = pos + bbRight * ( -0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + -0.5f * cosA * scale.y );
//	verts[2] = pos + bbRight * ( 0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
//	verts[3] = pos + bbRight * ( 0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + -0.5f * cosA * scale.y );


	verts[0] = Vec3f( -0.5f * scale.x,  0.5f * scale.y, 0.0f );
	verts[1] = Vec3f( -0.5f * scale.x, -0.5f * scale.y, 0.0f );
	verts[2] = Vec3f(  0.5f * scale.x,  0.5f * scale.y, 0.0f );
	verts[3] = Vec3f(  0.5f * scale.x, -0.5f * scale.y, 0.0f );
	
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
}





void drawButton( const ci::Rectf &rect, float u1, float v1, float u2, float v2 )
{
	glEnableClientState( GL_VERTEX_ARRAY );
	GLfloat verts[8];
	glVertexPointer( 2, GL_FLOAT, 0, verts );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	GLfloat texCoords[8];
	glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
	int vi = 0;
	int ti = 0;
	verts[vi++] = rect.getX2(); texCoords[ti++] = u2;
	verts[vi++] = rect.getY1(); texCoords[ti++] = v1;
	verts[vi++] = rect.getX1(); texCoords[ti++] = u1;
	verts[vi++] = rect.getY1(); texCoords[ti++] = v1;
	verts[vi++] = rect.getX2(); texCoords[ti++] = u2;
	verts[vi++] = rect.getY2(); texCoords[ti++] = v2;
	verts[vi++] = rect.getX1(); texCoords[ti++] = u1;
	verts[vi++] = rect.getY2(); texCoords[ti++] = v2;
	
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
}
=======
namespace bloom { namespace gl {
>>>>>>> f4c3fe4288740da69879a45a3b072f13129c46d0

    //BatchMap batchMap;
    
    void beginBatch()
    {
        batchMap.clear();
    }
    
    void batchRect( const ci::gl::Texture &texture, const ci::Rectf &srcRect, const ci::Rectf &dstRect )
    {
        GLuint texId = texture.getId();
        Batch *batch = &batchMap[texId];
        int verts = batch->vertices.size();
        if (verts == 0) {
            batch->texture = texture;
        }
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
        for (BatchMap::iterator it = batchMap.begin(); it != batchMap.end(); ++it) {
            Batch *batch = &it->second;
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