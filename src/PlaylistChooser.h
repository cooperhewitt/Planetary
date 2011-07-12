//
//  PlaylistChooser.h
//  Kepler
//
//  Created by Tom Carden on 7/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <vector>
#include "CinderIPod.h"

class PlaylistChooser {

public:    
    void setup();
    void update();
    void draw();
    
    void setPlaylists(std::vector<ci::ipod::PlaylistRef> playlists);

private:
    
    
    
};