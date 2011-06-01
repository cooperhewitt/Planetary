/*
 *  Data.cpp
 *  Kepler
 *
 *  Created by Robert Hodgin on 2/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/Text.h"
#include "cinder/Thread.h"
#include "Data.h"
#include "Globals.h"
#include <boost/algorithm/string.hpp>   
#include "CinderFlurry.h"
#include "stdlib.h"
#include <set>

using namespace ci;
using namespace ci::ipod;
using namespace std;
using namespace pollen::flurry;

Data::Data()
{
}

void Data::setup()
{
	mCurrentPlaylist = -1;
	mArtists.clear();
	mPlaylists.clear();
	mFilteredArtists.clear();    
    mNumArtistsPerChar.clear();
	
    if (!isIniting) {
        isIniting = true;
        std::thread artistLoaderThread( &Data::backgroundInit, this );	
    }
}

void Data::backgroundInit()
{
	NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
	
//	float t = app::App::get()->getElapsedSeconds();
//	ci::ipod::PlaylistRef tracks = getAllTracks();
//	std::cout << (app::App::get()->getElapsedSeconds()-t) << " seconds to get all tracks" << std::endl;
//	
//	t = app::App::get()->getElapsedSeconds();
//	int numTracks = tracks->size();
//	map<string,bool> uniqueArtists;
//	for (int i = 0; i < numTracks; i++) {
//		string artist = (*tracks)[i]->getArtist();
//		if (uniqueArtists[artist]) {
//			continue;
//		}
//		else {
//			uniqueArtists[artist] = true;
//			pending.push_back(getArtist((*tracks)[i]->getArtistId()));
//		}
//	}
//	std::cout << (app::App::get()->getElapsedSeconds()-t) << " seconds to derive artists" << std::endl;
	Flurry::getInstrumentation()->startTimeEvent("Music Loading");
	pendingArtists = getArtists();
	std::cout << "got " << pendingArtists.size() << " artists" << std::endl;
	
	
// QUICK FIX FOR GETTING MORE DATA ONTO THE ALPHAWHEEL
	string alphaString	= "ABCDEFGHIJKLMNOPQRSTUVWXYZ#";
	for( int i=0; i<27; i++ ){
		mNumArtistsPerChar[alphaString[i]] = 0;
	}
	float maxCount = 0.0001f;
	for( vector<ci::ipod::PlaylistRef>::iterator it = pendingArtists.begin(); it != pendingArtists.end(); ++it ){
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
	
	Flurry::getInstrumentation()->stopTimeEvent("Music Loading");
	
    std::map<string, string> params;
    params["NumArtists"] = i_to_string(pendingArtists.size());
    Flurry::getInstrumentation()->logEvent("Artists loaded", params);
    
	
	
	Flurry::getInstrumentation()->startTimeEvent("Playlists Loading");
	pendingPlaylists = getPlaylists();
	std::cout << "got " << pendingPlaylists.size() << " playlists" << std::endl;
    params["NumPlaylists"] = i_to_string( pendingPlaylists.size());
    Flurry::getInstrumentation()->logEvent("Playlists loaded", params);
	
	
	
    [autoreleasepool release];	
    
	isIniting = false;
}


bool Data::update()
{
	if (!isIniting && wasIniting) {
		// TODO: time this, is it OK in one frame?
		// TODO: switch state to enum. potential cause of freeze-on-load-screen bug
		mArtists.insert( mArtists.end(), pendingArtists.begin(), pendingArtists.end() );
		pendingArtists.clear();
		
		mPlaylists.insert( mPlaylists.end(), pendingPlaylists.begin(), pendingPlaylists.end() );
		pendingPlaylists.clear();
		
        wasIniting = isIniting;
		return true;
	}
    wasIniting = isIniting;
	return false;
}



void Data::filterArtistsByAlpha( char c )
{
	mFilteredArtists.clear();
	if( c != '#' ){
		
		float startTime = app::App::get()->getElapsedSeconds();
		
		char cLower = static_cast<char> ( tolower( c ) );
		char cUpper = static_cast<char> ( toupper( c ) );
		
		int index = 0;
		for( vector<PlaylistRef>::iterator it = mArtists.begin(); it != mArtists.end(); ++it ){
			string name		= (*it)->getArtistName();
			
			string the		= name.substr( 0, 4 );
			char firstLetter;
			
			if( the == "The " || the == "the " ){
				firstLetter = name[4];
			} else {
				firstLetter = name[0];
			}
		
			if( firstLetter == cUpper || firstLetter == cLower ){
				mFilteredArtists.push_back( index );
			}
			
			index ++;
		}
		
		std::cout << "filtered artists in " << (app::App::get()->getElapsedSeconds()-startTime) << std::endl;
		
	} else {
		int index = 0;
		for( vector<ci::ipod::PlaylistRef>::iterator it = mArtists.begin(); it != mArtists.end(); ++it ){
			string name		= (*it)->getArtistName();
			char firstLetter = name[0];
		
			if( !isalpha( firstLetter ) ){
				std::cout << name << std::endl;
				mFilteredArtists.push_back( index );
			}
			
			index ++;
		}
	}
}


void Data::filterArtistsByPlaylist( PlaylistRef playlist )
{
	
	std::set<uint64_t> artistSet;
	for( Playlist::Iter i = playlist->begin(); i != playlist->end(); i++ ){
		TrackRef track = *i;
		artistSet.insert( track->getArtistId() );
	}
	
	
	mFilteredArtists.clear();
	
	int index = 0;
	for( vector<PlaylistRef>::iterator it = mArtists.begin(); it != mArtists.end(); ++it ){
		uint64_t id				= (*it)->getArtistId();
		
		if (artistSet.find( id ) != artistSet.end()) {
			mFilteredArtists.push_back( index );
		}
//		for( Playlist::Iter i = playlist->begin(); i != playlist->end(); i++ ){
//			TrackRef track = *i;
//			if( track->getArtistId() == id ){
//				mFilteredArtists.push_back( index );
//			}
//		}
		
		index ++;
	}
	
}