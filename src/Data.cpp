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
	for( map< char, float >::iterator it = mNumArtistsPerChar.begin(); it != mNumArtistsPerChar.end(); ++it ){
		it->second = ( it->second/maxCount );
	}
	
	buildVertexArray();
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

void Data::buildVertexArray()
{
	string alphaString	= "ABCDEFGHIJKLMNOPQRSTUVWXYZ#";
	mWheelDataVerts		= new float[ 27*2*6 ]; // LETTERS * 2DIMENSIONS * 2TRIANGLES
	mWheelDataTexCoords = new float[ 27*2*6 ];
	mWheelDataColors	= new float[ 27*4*6 ];	
	
	
	float baseRadius = 195.0f;
	int index = 0;
	int tIndex = 0;
	int cIndex = 0;
	for( int i=0; i<27; i++ ){
		float numArtistsAtChar	= (float)mNumArtistsPerChar[alphaString[i]];
		float per				= (float)i/27.0f;
		float angle				= per * TWO_PI - M_PI_2;
		float angle1			= angle - 0.65f;
		float angle2			= angle + 0.65f;
		float cosAngle			= cos( angle ) * 220.0f;
		float sinAngle			= sin( angle ) * 220.0f;
		float newBaseRadius		= baseRadius + ( numArtistsAtChar * 5.0f );
		float cosAngle1			= cos( angle1 ) * newBaseRadius;
		float sinAngle1			= sin( angle1 ) * newBaseRadius;
		float cosAngle2			= cos( angle2 ) * newBaseRadius;
		float sinAngle2			= sin( angle2 ) * newBaseRadius;
		
		mWheelDataVerts[index++] = cosAngle1;
		mWheelDataVerts[index++] = sinAngle1;
		mWheelDataVerts[index++] = cosAngle2;
		mWheelDataVerts[index++] = sinAngle2;
		mWheelDataVerts[index++] = cosAngle1 + cosAngle;
		mWheelDataVerts[index++] = sinAngle1 + sinAngle;
		
		mWheelDataVerts[index++] = cosAngle2;
		mWheelDataVerts[index++] = sinAngle2;
		mWheelDataVerts[index++] = cosAngle1 + cosAngle;
		mWheelDataVerts[index++] = sinAngle1 + sinAngle;
		mWheelDataVerts[index++] = cosAngle2 + cosAngle;
		mWheelDataVerts[index++] = sinAngle2 + sinAngle;
		
		mWheelDataTexCoords[tIndex++] = 0.0f;
		mWheelDataTexCoords[tIndex++] = 0.0f;
		mWheelDataTexCoords[tIndex++] = 1.0f;
		mWheelDataTexCoords[tIndex++] = 0.0f;
		mWheelDataTexCoords[tIndex++] = 0.0f;
		mWheelDataTexCoords[tIndex++] = 1.0f;
		
		mWheelDataTexCoords[tIndex++] = 1.0f;
		mWheelDataTexCoords[tIndex++] = 0.0f;
		mWheelDataTexCoords[tIndex++] = 0.0f;
		mWheelDataTexCoords[tIndex++] = 1.0f;
		mWheelDataTexCoords[tIndex++] = 1.0f;
		mWheelDataTexCoords[tIndex++] = 1.0f;
		
		for( int c=0; c<6; c++ ){
			mWheelDataColors[cIndex++] = 1.0f;
			mWheelDataColors[cIndex++] = 1.0f;
			mWheelDataColors[cIndex++] = 1.0f;
			mWheelDataColors[cIndex++] = numArtistsAtChar;
		}
	}
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