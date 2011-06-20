//
//  GlExtras.cpp
//  Kepler
//
//  Created by Robert Hodgin on 4/7/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "BloomGl.h"

namespace bloom { namespace gl {

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