/*
 *  Data.cpp
 *  Kepler
 *
 *  Created by Robert Hodgin on 2/25/11.
 *  Copyright 2013 Smithsonian Institution. All rights reserved.
 *
 */

#include "Data.h"
#include "cinder/Utilities.h" // for toString
#include "CinderFlurry.h"     // for loggin
#include "TaskQueue.h"        // for backgrounding tasks

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
        mArtistProgress = 0.0f;
        mPlaylistProgress = 0.0f;   
        TaskQueue::pushTask( std::bind( std::mem_fun( &Data::backgroundInit ), this ) );
    }
}

void Data::backgroundInit()
{
	Flurry::getInstrumentation()->startTimeEvent("Music Loading");

	mPendingArtists = getArtists( std::bind1st( std::mem_fun(&Data::artistProgress), this ) );
    mPendingPlaylists = getPlaylists( std::bind1st( std::mem_fun(&Data::playlistProgress), this ) );

    map<string, string> params;
    params["NumArtists"]   = toString( mPendingArtists.size() );
    params["NumPlaylists"] = toString( mPendingPlaylists.size() );
	Flurry::getInstrumentation()->stopTimeEvent("Music Loading", params);
	
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
