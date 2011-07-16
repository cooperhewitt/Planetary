/*
 *  Globals.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/20/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once 

#include "cinder/Color.h"

static const double TWO_PI			= M_PI * 2.0;

static const double G_DURATION		= 2.5;

// TODO: enum for these?
// TODO: move into State.h?
static const int G_ALPHA_LEVEL		= 1;
static const int G_ARTIST_LEVEL		= 2;
static const int G_ALBUM_LEVEL		= 3;
static const int G_TRACK_LEVEL		= 4;
static const int G_NUM_LEVELS		= 5;

static const float G_INIT_CAM_DIST	= 250.0f;
static const int G_NUM_PLANET_TYPES = 5;
static const int G_NUM_PLANET_TYPE_OPTIONS = 3;
static const int G_NUM_CLOUD_TYPES	= 5;

static const float G_MIN_FOV		= 55.0f;
static const float G_DEFAULT_FOV	= 60.0f;
static const float G_MAX_FOV		= 90.0f;

static const float G_DEFAULT_ARCBALL_RADIUS	= 500.0f;

static const int G_RING_LOW_RES		= 250;
static const int G_RING_HIGH_RES	= 500;

static const int G_SKYDOME_RADIUS	= 1000.0f;

static const int G_TOTAL_HELP_CALLOUTS = 8;

static const ci::Color GREY				= ci::Color( 0.1f, 0.1f, 0.15f );
static const ci::Color BLUE				= ci::Color( 0.1f, 0.2f, 0.5f );
static const ci::Color BRIGHT_BLUE		= ci::Color( 0.4f, 0.8f, 1.0f );
static const ci::Color BRIGHT_YELLOW	= ci::Color( 1.0f, 0.9f, 0.35f );


// TODO: move into State.h
extern float G_ZOOM;
extern int G_CURRENT_LEVEL;
extern bool G_DEBUG;
extern bool G_SHOW_SETTINGS;
extern bool G_HELP;
extern bool G_ACCEL;
extern bool G_DRAW_RINGS;
extern bool G_DRAW_TEXT;
extern bool G_USE_GYRO;
extern bool G_USE_COMPRESSED;

extern bool G_IS_IPAD2;
extern int G_NUM_PARTICLES;
extern int G_NUM_DUSTS;