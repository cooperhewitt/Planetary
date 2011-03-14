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

using namespace ci;
using namespace ci::ipod;
using namespace std;

Data::Data()
{
}

void Data::initArtists()
{
//	mArtists	= getArtists();
	isIniting = true;
	std::thread loaderThread( &Data::backgroundInitArtists, this );	
}

void Data::backgroundInitArtists()
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
	
	pending = getArtists();		
	std::cout << "got " << pending.size() << " artists" << std::endl;
    [autoreleasepool release];	
	isIniting = false;
}

bool Data::update()
{
	if (!isIniting && pending.size() > 0) {
		// TODO: time this, is it OK in one frame?
		mArtists.insert(mArtists.end(),pending.begin(),pending.end());
		pending.clear();
		return true;
	}
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
		for( vector<ci::ipod::PlaylistRef>::iterator it = mArtists.begin(); it != mArtists.end(); ++it ){
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