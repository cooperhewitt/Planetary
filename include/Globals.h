/*
 *  Globals.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/20/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "cinder/Color.h"

#pragma once 

static const double TWO_PI			= M_PI * 2.0;

static const double G_DURATION		= 2.5;

// TODO: enum for these?
// TODO: move into State.h?
static const int G_HOME_LEVEL		= 0;
static const int G_ALPHA_LEVEL		= 1;
static const int G_ARTIST_LEVEL		= 2;
static const int G_ALBUM_LEVEL		= 3;
static const int G_TRACK_LEVEL		= 4;
static const int G_NUM_LEVELS		= 5;

static const float G_INIT_CAM_DIST	= 220.0f;
static const int G_NUM_PLANET_TYPES = 5;
static const int G_NUM_PLANET_TYPE_OPTIONS = 3;
static const int G_NUM_CLOUD_TYPES	= 3;

static const float G_MIN_FOV		= 65.0f;
static const float G_MAX_FOV		= 115.0f;

static const int G_RING_LOW_RES		= 250;
static const int G_RING_HIGH_RES	= 500;

static const int G_SKYDOME_RADIUS	= 1000.0f;

static const ci::Color COLOR_BLUE	= ci::Color( 0.1f, 0.2f, 0.5f );
static const ci::Color COLOR_BRIGHT_BLUE = ci::Color( 0.2f, 0.3f, 0.8f );


// TODO: move into State.h
extern float G_ZOOM;
extern bool G_DEBUG;
extern bool G_HELP;

extern bool G_IS_IPAD2;
extern int G_NUM_PARTICLES;
extern int G_NUM_DUSTS;