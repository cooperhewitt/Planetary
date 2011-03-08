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
	mArtists	= getArtists();
}

// TODO: make the # char select artists with a leading numeral in the name
void Data::filterArtistsByAlpha( char c )
{
	mFilteredArtists.clear();
	if( c != '#' ){
		char cLower = static_cast<char> ( tolower ( c ) );
		
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
		
			if( firstLetter == c || firstLetter == cLower ){
				std::cout << name << std::endl;
				mFilteredArtists.push_back( index );
			}
			
			index ++;
		}
	} else {
		int index = 0;
		for( vector<ci::ipod::PlaylistRef>::iterator it = mArtists.begin(); it != mArtists.end(); ++it ){
			string name		= (*it)->getArtistName();
			char firstLetter = name[0];
		
			if( isdigit( firstLetter ) ){
				std::cout << name << std::endl;
				mFilteredArtists.push_back( index );
			}
			
			index ++;
		}
	}
}