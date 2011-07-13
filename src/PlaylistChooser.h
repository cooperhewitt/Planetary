//
//  PlaylistChooser.h
//  Kepler
//
//  Created by Tom Carden on 7/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <vector>
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/Font.h"
#include "cinder/Color.h"
#include "cinder/Camera.h"
#include "CinderIPod.h"
#include "OrientationHelper.h"
#include "Data.h"
#include "World.h"

class PlaylistChooser {

public:    
    
    PlaylistChooser() { mData = NULL; }
    
    void setup( ci::app::AppCocoaTouch *app, const ci::app::Orientation &orientation, const ci::Font &font, const ci::Color &lineColor );
    void update();
    void draw();

    void setInterfaceOrientation( const ci::app::Orientation &orientation );    
    
    void setDataWorldCam(Data *data, World *world, ci::CameraPersp *cam);

private:
    
    Data *mData; // for playlists
    World *mWorld; // for nodes
    ci::CameraPersp *mCam; // for projecting to screen
    
    ci::gl::TextureFontRef mTextureFont;
    ci::Color mLineColor;
    ci::Font mFont;
    
    ci::app::Orientation mInterfaceOrientation;
    ci::Matrix44f mOrientationMatrix;
    ci::Vec2f mInterfaceSize;    
};