/*
 *  Globals.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/20/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once 

static const double TWO_PI			= M_PI * 2.0;

static const double G_DURATION				= 3.0;

// TODO: enum for these?
// TODO: move into State.h
static const int G_HOME_LEVEL		= 0;
static const int G_ARTIST_LEVEL		= 1;
static const int G_ALBUM_LEVEL		= 2;
static const int G_TRACK_LEVEL		= 3;
static const int G_NUM_LEVELS		= 4;

static const float G_INIT_CAM_DIST	= 180.0f;

// TODO: move into State.h
extern int	G_CURRENT_LEVEL;