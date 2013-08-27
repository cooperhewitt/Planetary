//
//  GlExtras.h
//  Kepler
//
//  Created by Robert Hodgin on 4/7/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#pragma once

#include <map>
#include <vector>
#include "cinder/Cinder.h"
#include "cinder/Rect.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include <boost/unordered_map.hpp>

namespace bloom { namespace gl {

	void drawBillboard( const ci::Vec3f &pos, const ci::Vec2f &scale, float rotationDegrees, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	void drawSphericalBillboard( const ci::Vec3f &camEye, const ci::Vec3f &objPos, const ci::Vec2f &scale, float rotationDegrees );
    void drawSphericalRotatedBillboard( const ci::Vec3f &pos, const ci::Vec3f &lookAt, const ci::Vec3f &turnAt, const ci::Vec2f &scale );
	
    // hat tip http://craiggiles.wordpress.com/2009/08/03/opengl-es-batch-rendering-on-the-iphone/

    struct VertexData {
        ci::Vec2f vertex;
        ci::Vec2f texture;
    };
    
    struct Batch {
        ci::gl::Texture texture;
        std::vector<VertexData> vertices;
    };
    
    typedef std::shared_ptr<Batch> BatchRef;
    
    extern boost::unordered_map<GLuint, BatchRef> batchByTex;
    extern std::vector<BatchRef> batches;
    
    void beginBatch();
    void batchRect( const ci::gl::Texture &tex, const ci::Vec2f &pos );            
    void batchRect( const ci::gl::Texture &tex, const ci::Rectf &srcRect, const ci::Rectf &dstRect );
    void batchRect( const ci::gl::Texture &tex, const ci::Area &srcArea, const ci::Rectf &dstRect );
    void endBatch();
    
} }
