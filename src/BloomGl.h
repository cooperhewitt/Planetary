//
//  GlExtras.h
//  Kepler
//
//  Created by Robert Hodgin on 4/7/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <map>
#include <vector>
#include "cinder/Rect.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

namespace bloom { namespace gl {

    // hat tip http://craiggiles.wordpress.com/2009/08/03/opengl-es-batch-rendering-on-the-iphone/

    struct VertexData {
        ci::Vec2f vertex;
        ci::Vec2f texture;
    };
    
    struct Batch {
        ci::gl::Texture texture;
        std::vector<VertexData> vertices;
    };
    
    typedef std::map<GLuint, Batch> BatchMap;

    extern BatchMap batchMap;
    
    void beginBatch();
    void batchRect( const ci::gl::Texture &tex, const ci::Rectf &srcRect, const ci::Rectf &dstRect );
    void batchRect( const ci::gl::Texture &tex, const ci::Area &srcArea, const ci::Rectf &dstRect );
    void endBatch();
    
} }
