//
//  Stats.h
//  Kepler
//
//  Created by Tom Carden on 6/10/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#pragma once

#include "cinder/gl/Texture.h"
#include "cinder/Matrix.h"
#include "cinder/Color.h"
#include "cinder/Font.h"

class Stats {

public:

    void setup(const ci::Font &font, const ci::Color &fpsColor, const ci::Color &color);
    void update(const float &fps, 
                const float &playheadTime, 
                const float &fov,
				const float &camDist,
				const float &pinchPer,
                const float &currentLevel, 
                const float &zoom);
    void draw(const ci::Matrix44f &orientationMatrix);
    
private:

    ci::gl::Texture mParamsTex;
    ci::Color mColor, mFpsColor;
    ci::Font mFont; 
};