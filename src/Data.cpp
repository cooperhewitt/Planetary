/*
 *  Data.cpp
 *  Kepler
 *
 *  Created by Robert Hodgin on 2/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Data.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Thread.h"
#include "cinder/Text.h"
#include "cinder/Utilities.h"
#include "CinderFlurry.h"

using namespace ci;
using namespace ci::ipod;
using namespace std;
using namespace pollen::flurry;

void Data::setup()
{
	mArtists.clear();
	mPlaylists.clear();
    mNumArtistsPerChar.clear();
	
    if (mState != LoadStateLoading) {
        mState = LoadStateLoading;
        std::thread artistLoaderThread( &Data::backgroundInit, this );	
    }
}

void Data::backgroundInit()
{
	NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
	
	Flurry::getInstrumentation()->startTimeEvent("Music Loading");
	mPendingArtists = getArtists();
	cout << "got " << mPendingArtists.size() << " artists" << endl;
	
// QUICK FIX FOR GETTING MORE DATA ONTO THE ALPHAWHEEL
	string alphaString	= "ABCDEFGHIJKLMNOPQRSTUVWXYZ#";
	for( int i=0; i<27; i++ ){
		mNumArtistsPerChar[alphaString[i]] = 0;
	}
	float maxCount = 0.0001f;
	for( vector<ci::ipod::PlaylistRef>::iterator it = mPendingArtists.begin(); it != mPendingArtists.end(); ++it ){
        
		string name		= (*it)->getArtistName();
		string the		= name.substr( 0, 4 );
		char firstLetter;
		
		if( the == "The " || the == "the " ){
			firstLetter = name[4];
		} else {
			firstLetter = name[0];
		}
		
		if( isdigit(firstLetter) ){
			firstLetter = '#';
		} else {
			firstLetter = static_cast<char> ( toupper( firstLetter ) );
		}
		
		mNumArtistsPerChar[firstLetter] += 1.0f;
		
		if( mNumArtistsPerChar[firstLetter] > maxCount ){
			maxCount = mNumArtistsPerChar[firstLetter];
		}
	}
	
	for( int i=0; i<27; i++ ){
		mNormalizedArtistsPerChar[i] = mNumArtistsPerChar[alphaString[i]]/maxCount;
	}

// END ALPHAWHEEL QUICK FIX
	
    map<string, string> params;
    params["NumArtists"] = toString(mPendingArtists.size());
    Flurry::getInstrumentation()->logEvent("Artists loaded", params);
	
	Flurry::getInstrumentation()->startTimeEvent("Playlists Loading");
	mPendingPlaylists = getPlaylists();
	cout << "got " << mPendingPlaylists.size() << " playlists" << endl;
    params.clear();
    params["NumPlaylists"] = toString( mPendingPlaylists.size());
    Flurry::getInstrumentation()->logEvent("Playlists loaded", params);	
		
	Flurry::getInstrumentation()->stopTimeEvent("Music Loading");

    [autoreleasepool release];	
    
    mState = LoadStatePending;
}


void Data::update()
{
	if (mState == LoadStatePending) {

		mArtists.insert( mArtists.end(), mPendingArtists.begin(), mPendingArtists.end() );
		mPendingArtists.clear();
		
		mPlaylists.insert( mPlaylists.end(), mPendingPlaylists.begin(), mPendingPlaylists.end() );
		mPendingPlaylists.clear();
		        
        mState = LoadStateComplete;
	}
}
