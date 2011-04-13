/*
 *  NodeTrack.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "cinder/app/AppBasic.h"
#include "NodeTrack.h"
#include "cinder/Text.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "Globals.h"

using namespace ci;
using namespace ci::ipod;
using namespace std;

NodeTrack::NodeTrack( Node *parent, int index, const Font &font )
	: Node( parent, index, font )
{	
	mIsHighlighted		= true;
	mRadius				*= 10.0f;
    mIsPlaying			= false;
	mHasClouds			= false;
	mIsMostPlayed		= false;
	mIsPopulated		= false;
	mHasAlbumArt		= false;
	
	mTotalOrbitVertices		= 0;
	mPrevTotalOrbitVertices = -1;
    mOrbitVerts				= NULL;
	mOrbitTexCoords			= NULL;
	mOrbitColors			= NULL;
}

void NodeTrack::setData( TrackRef track, PlaylistRef album )
{
	mAlbum			= album;
// TRACK INFORMATION
	mTrack			= track;
	mTrackLength	= (*mAlbum)[mIndex]->getLength();
	mPlayCount		= (*mAlbum)[mIndex]->getPlayCount();
	mStarRating		= (*mAlbum)[mIndex]->getStarRating();

	
	
	string name		= getName();
	char c1			= ' ';
	if( name.length() >= 3 ){
		c1 = name[1];
	}
	
	int c1Int = constrain( int(c1), 32, 127 );
	
	mAsciiPer = ( c1Int - 32 )/( 127.0f - 32 );
	
	
	
	
	//normalize playcount data
	float playCountDelta	= ( mParentNode->mHighestPlayCount - mParentNode->mLowestPlayCount ) + 1.0f;
	float normPlayCount		= ( mPlayCount - mParentNode->mLowestPlayCount )/playCountDelta;

// ALBUM ART
	Surface albumArt		= mTrack->getArtwork( Vec2i( 128, 128 ) );
	if( albumArt ){
		int x				= (int)(normPlayCount*100);
		int y				= c1Int%64;
		int w				= 1;
		int h				= normPlayCount * 64;
		Area a				= Area( x, y, x+w, y+h );
		Surface crop		= albumArt.clone( a );
		mAlbumArt			= gl::Texture( crop );
		mHasAlbumArt		= true;
	}
	
	
	mPlanetTexIndex			= (int)( normPlayCount * ( G_NUM_PLANET_TYPES - 1 ) );
	mCloudTexIndex			= Rand::randInt( G_NUM_CLOUD_TYPES );
   // mPlanetTexIndex *= G_NUM_PLANET_TYPE_OPTIONS + Rand::randInt( G_NUM_PLANET_TYPE_OPTIONS );
	
	if( mPlayCount > 50 ){
		mCloudTexIndex = 2;
	} else if( mPlayCount > 10 ){
		mCloudTexIndex = 1;
	} else {
		mCloudTexIndex = 0;
	}
	
	if( album->size() == 1 ){		// if im the only track, no clouds.
		mHasClouds = false;
	} else if( mPlayCount > 50 ){	// if im one of many tracks, i have clouds if ive been played plenty
		mHasClouds = true;
	}
	
	if( album->size() > 1 ){
		if( mPlayCount == mParentNode->mHighestPlayCount ){
			mIsMostPlayed = true;
		}
	}
	

	mOrbitPath.clear();
	
	mHue				= mAsciiPer;
	mSat				= ( 1.0f - sin( mHue * M_PI ) ) * 0.1f + 0.15f;
	mColor				= Color( CM_HSV, mHue, mSat * 0.5f, 1.0f );
	mGlowColor			= mParentNode->mGlowColor;
	mEclipseColor		= mColor;
	
	mRadius				= math<float>::max( mRadius * pow( normPlayCount + 0.5f, 2.0f ), 0.0003f ) * 0.75;
	mSphere				= Sphere( mPos, mRadius * 7.5f );
	
	mIdealCameraDist	= 0.01f;
	mOrbitPeriod		= mTrackLength;
	mAxialTilt			= Rand::randFloat( 5.0f, 30.0f );
    mAxialVel			= Rand::randFloat( 10.0f, 45.0f );

	mStartRelPos		= Vec3f( cos( mOrbitStartAngle ), sin( mOrbitStartAngle ), 0.0f ) * mOrbitRadius;
	mStartPos			= ( mParentNode->mPos + mStartRelPos ); 
}

void NodeTrack::updateAudioData( double currentPlayheadTime )
{
	if( mIsPlaying ){
		mPercentPlayed		= currentPlayheadTime/mTrackLength;
		mOrbitAngle			= mPercentPlayed * TWO_PI + mOrbitStartAngle;
		
		// TODO: Find a better way to do this without clearing mOrbitPath every frame.
		mOrbitPath.clear();
		
		// Add start position
		mOrbitPath.push_back( Vec3f( cos( mOrbitStartAngle ), sin( mOrbitStartAngle ), 0.0f ) );
		
		// Add middle positions
		int maxNumVecs		= 200;
		int currentNumVecs	= mPercentPlayed * maxNumVecs;
		for( int i=0; i<currentNumVecs; i++ ){
			float per = (float)(i)/(float)(maxNumVecs);
			float angle = mOrbitStartAngle + per * TWO_PI;
			Vec2f pos = Vec2f( cos( angle ), sin( angle ) );
			
			mOrbitPath.push_back( Vec3f( pos.x, pos.y, 0.0f ) );
		}
		
		// Add end position
		mOrbitPath.push_back( Vec3f( cos( mOrbitAngle ), sin( mOrbitAngle ), 0.0f ) );
		
		buildPlayheadProgressVertexArray();
	}
}



void NodeTrack::buildPlayheadProgressVertexArray()
{
	int orbitPathSize	= mOrbitPath.size();
	mTotalOrbitVertices	= orbitPathSize * 2;
	
	if( mTotalOrbitVertices != mPrevTotalOrbitVertices ){
		if (mOrbitVerts != NULL)		delete[] mOrbitVerts;
		if( mOrbitTexCoords != NULL)	delete[] mOrbitTexCoords;
		if( mOrbitColors != NULL)		delete[] mOrbitColors;
		
		mOrbitVerts		= new float[mTotalOrbitVertices*3];
		mOrbitTexCoords	= new float[mTotalOrbitVertices*2];
		mOrbitColors	= new float[mTotalOrbitVertices*4];
		
		mPrevTotalOrbitVertices = mTotalOrbitVertices;
	}
	
	int vIndex		= 0;
	int tIndex		= 0;
	int cIndex		= 0;
	int index		= 0;
	Color col		= COLOR_BRIGHT_BLUE;
	float radius	= mRadius;
	float alpha		= constrain( G_ZOOM - G_ARTIST_LEVEL, 0.0f, 1.0f ) * 0.3f;
	
	for( vector<Vec3f>::iterator it = mOrbitPath.begin(); it != mOrbitPath.end(); ++it )
	{
		Vec3f pos1				= *it * ( mOrbitRadius + radius );
		Vec3f pos2				= *it * ( mOrbitRadius - radius );
		
		mOrbitVerts[vIndex++]	= pos1.x;
		mOrbitVerts[vIndex++]	= pos1.y;
		mOrbitVerts[vIndex++]	= pos1.z;
		
		mOrbitVerts[vIndex++]	= pos2.x;
		mOrbitVerts[vIndex++]	= pos2.y;
		mOrbitVerts[vIndex++]	= pos2.z;
		
		mOrbitTexCoords[tIndex++]	= app::getElapsedSeconds() + index;
		mOrbitTexCoords[tIndex++]	= 0.0f;
		
		mOrbitTexCoords[tIndex++]	= app::getElapsedSeconds() + index;
		mOrbitTexCoords[tIndex++]	= 1.0f;
		
		mOrbitColors[cIndex++]	= col.r;
		mOrbitColors[cIndex++]	= col.g;
		mOrbitColors[cIndex++]	= col.b;
		mOrbitColors[cIndex++]	= alpha;
		
		mOrbitColors[cIndex++]	= col.r;
		mOrbitColors[cIndex++]	= col.g;
		mOrbitColors[cIndex++]	= col.b;
		mOrbitColors[cIndex++]	= alpha;
		
		index ++;
	}
}

/*
void NodeTrack::buildPlayheadProgressVertexArray()
{
	mTotalOrbitVertices	= mOrbitPath.size();
	
	if( mTotalOrbitVertices != mPrevTotalOrbitVertices ){
		if (mOrbitVerts != NULL)	delete[] mOrbitVerts;
		if( mOrbitColors != NULL)	delete[] mOrbitColors;
		
		mOrbitVerts		= new float[mTotalOrbitVertices*3];
		mOrbitColors	= new float[mTotalOrbitVertices*4];
		
		mPrevTotalOrbitVertices = mTotalOrbitVertices;
	}
	
	int vIndex = 0;
	int cIndex = 0;
	Color col  = Color( 0.5f, 0.0f, 0.0f );//0.1f, 0.2f, 0.5f );
	
	float per = constrain( G_ZOOM - G_ARTIST_LEVEL, 0.0f, 1.0f );
	int index = 0;
	
	for( vector<Vec3f>::iterator it = mOrbitPath.begin(); it != mOrbitPath.end(); ++it ){
		Vec3f pos				= *it;
		
		mOrbitVerts[vIndex++]	= pos.x;
		mOrbitVerts[vIndex++]	= pos.y;
		mOrbitVerts[vIndex++]	= pos.z;
		
		mOrbitColors[cIndex++]	= col.r;
		mOrbitColors[cIndex++]	= col.g;
		mOrbitColors[cIndex++]	= col.b;
		
		if( index == 0 ){
			mOrbitColors[cIndex++]	= 0.0f;
		} else {
			mOrbitColors[cIndex++]	= 0.1f * per;
		}
		index ++;
	}
}
*/


void NodeTrack::update( const Matrix44f &mat )
{	
	if( !mIsPlaying ){
		// TODO: THIS IS AWKWARD. This is so the non-playing tracks still orbit
		mPercentPlayed	= ( app::getElapsedSeconds() + mIndex * 50.0f )/mOrbitPeriod;
		mOrbitAngle		= mPercentPlayed * TWO_PI;
		mOrbitStartAngle = mOrbitAngle;
	}
	
	Vec3f prevTransPos  = mat * mPos;    
    
	mRelPos				= Vec3f( cos( mOrbitAngle ), sin( mOrbitAngle ), 0.0f ) * mOrbitRadius;
	mPos				= mParentNode->mPos + mRelPos;
	
	if( mIsPlaying ){
		mStartRelPos		= Vec3f( cos( mOrbitStartAngle ), sin( mOrbitStartAngle ), 0.0f ) * mOrbitRadius;
		mTransStartPos		= mat * ( mParentNode->mPos + mStartRelPos );
	}
	
    float eclipseDist	= 1.0f;
    if( mParentNode->mParentNode->mDistFromCamZAxisPer > 0.0f )
	{
		
		Vec2f p		= mScreenPos;
		float r		= mSphereScreenRadius * 0.45f;
		float rsqrd = r * r;
		
		Vec2f P		= mParentNode->mParentNode->mScreenPos;
		float R		= mParentNode->mParentNode->mSphereScreenRadius * 0.4f;
		float Rsqrd	= R * R;
		float A		= M_PI * Rsqrd;
		
		
		float totalRadius = r + R;
		float c		= p.distance( P );
		if( c < totalRadius && mIsSelected )
		{
			float csqrd = c * c;
			float cos1	= ( Rsqrd + csqrd - rsqrd )/( 2.0f * R * c );
			float CBA	= acos( constrain( cos1, -1.0f, 1.0f ) );
			float CBD	= CBA * 2.0f;
			
			float cos2	= ( rsqrd + csqrd - Rsqrd )/( 2.0f * r * c );
			float CAB	= acos( constrain( cos2, -1.0f, 1.0f ) );
			float CAD	= CAB * 2.0f;
			float intersectingArea = CBA * Rsqrd - 0.5f * Rsqrd * sin( CBD ) + 0.5f * CAD * rsqrd - 0.5f * rsqrd * sin( CAD );
			mEclipseStrength = ( A - intersectingArea ) / A;
			/*
			 std::cout << "================== " << std::endl;
			 std::cout << "c = " << c << std::endl;
			 std::cout << "A = " << A << std::endl;
			 std::cout << "CBA = " << CBA << std::endl;
			 std::cout << "CAB = " << CAB << std::endl;
			 std::cout << "intersectingArea = " << intersectingArea << std::endl;
			 std::cout << "totalRadius = " << totalRadius << std::endl;		
			 std::cout << "mEclipseStrength = " << mEclipseStrength << std::endl;
			 */
		}
	}
	mEclipseColor = ( mColor + Color::white() ) * 0.5f * eclipseDist;
	
	Node::update( mat );

	mTransVel = mTransPos - prevTransPos;	
}

void NodeTrack::drawEclipseGlow()
{
	if( mIsSelected && mDistFromCamZAxisPer > 0.0f ){
        mParentNode->mParentNode->mEclipseStrength = mEclipseStrength * mZoomPer;
	}
}

void NodeTrack::drawPlanet( const vector<gl::Texture> &planets )
{	
	if( mSphereScreenRadius > 2.0f ){
		glEnable( GL_RESCALE_NORMAL );
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glEnableClientState( GL_NORMAL_ARRAY );
		int numVerts;
		if( mIsSelected ){
			glVertexPointer( 3, GL_FLOAT, 0, mSphereVertsHiRes );
			glTexCoordPointer( 2, GL_FLOAT, 0, mSphereTexCoordsHiRes );
			glNormalPointer( GL_FLOAT, 0, mSphereNormalsHiRes );
			numVerts = mTotalVertsHiRes;
		} else {
			glVertexPointer( 3, GL_FLOAT, 0, mSphereVertsLoRes );
			glTexCoordPointer( 2, GL_FLOAT, 0, mSphereTexCoordsLoRes );
			glNormalPointer( GL_FLOAT, 0, mSphereNormalsLoRes );
			numVerts = mTotalVertsLoRes;
		}
		
		gl::disableAlphaBlending();
		
		gl::pushModelView();
		gl::translate( mTransPos );
		gl::scale( Vec3f( mRadius, mRadius, mRadius ) );
		gl::rotate( mMatrix );
		gl::rotate( Vec3f( 0.0f, 0.0f, mAxialTilt ) );
		gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * mAxialVel, 0.0f ) );
		gl::color( mEclipseColor );
		if( mHasAlbumArt ){
			mAlbumArt.enableAndBind();
		} else {
			planets[mPlanetTexIndex].enableAndBind();
		}
		glDrawArrays( GL_TRIANGLES, 0, numVerts );
		gl::popModelView();
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glDisableClientState( GL_NORMAL_ARRAY );
	}
	
}

void NodeTrack::drawClouds( const vector<gl::Texture> &planets, const vector<gl::Texture> &clouds )
{
	if( mSphereScreenRadius > 10.0f ){
		if( mCamDistAlpha > 0.05f ){
			glEnableClientState( GL_VERTEX_ARRAY );
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			glEnableClientState( GL_NORMAL_ARRAY );
			int numVerts;
			if( mIsSelected ){
				glVertexPointer( 3, GL_FLOAT, 0, mSphereVertsHiRes );
				glTexCoordPointer( 2, GL_FLOAT, 0, mSphereTexCoordsHiRes );
				glNormalPointer( GL_FLOAT, 0, mSphereNormalsHiRes );
				numVerts = mTotalVertsHiRes;
			} else {
				glVertexPointer( 3, GL_FLOAT, 0, mSphereVertsLoRes );
				glTexCoordPointer( 2, GL_FLOAT, 0, mSphereTexCoordsLoRes );
				glNormalPointer( GL_FLOAT, 0, mSphereNormalsLoRes );
				numVerts = mTotalVertsLoRes;
			}
			
			gl::disableAlphaBlending();

			gl::pushModelView();
			gl::translate( mTransPos );
			planets[mPlanetTexIndex].enableAndBind();

			glEnable( GL_LIGHTING );
			// LIT CLOUDS
			gl::pushModelView();
			float radius = mRadius + 0.000025f;
			gl::scale( Vec3f( radius, radius, radius ) );
			glEnable( GL_RESCALE_NORMAL );
			gl::rotate( mMatrix );
			gl::rotate( Vec3f( 0.0f, 0.0f, mAxialTilt ) );
			gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * mAxialVel, 0.0f ) );
			gl::enableAdditiveBlending();
			gl::color( ColorA( mEclipseColor, mCamDistAlpha * 0.5f ) );
			glDrawArrays( GL_TRIANGLES, 0, numVerts );
			gl::popModelView();
			gl::popModelView();
			
			glDisableClientState( GL_VERTEX_ARRAY );
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
			glDisableClientState( GL_NORMAL_ARRAY );
		}
	}
}

void NodeTrack::drawOrbitRing( float pinchAlphaPer, GLfloat *ringVertsLowRes, GLfloat *ringVertsHighRes )
{
	float alpha = 0.2f * pinchAlphaPer;
	
	if( mIsMostPlayed )
		gl::color( ColorA( COLOR_BRIGHT_BLUE, alpha ) );
	else
		gl::color( ColorA( COLOR_BLUE, alpha ) );		
	
	gl::pushModelView();
	gl::translate( mParentNode->mTransPos );
	gl::scale( Vec3f( mOrbitRadius, mOrbitRadius, mOrbitRadius ) );
	gl::rotate( mMatrix );
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, ringVertsLowRes );
	glDrawArrays( GL_LINE_STRIP, 0, G_RING_LOW_RES );
	glDisableClientState( GL_VERTEX_ARRAY );
	gl::popModelView();
}

void NodeTrack::drawPlayheadProgress( const gl::Texture &tex )
{
	if( mIsPlaying ){
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glEnableClientState( GL_COLOR_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, mOrbitVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, mOrbitTexCoords );
		glColorPointer( 4, GL_FLOAT, 0, mOrbitColors );
		
		tex.enableAndBind();
		gl::pushModelView();
		gl::translate( mParentNode->mTransPos );
		gl::rotate( mMatrix );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, mTotalOrbitVertices );		
		tex.disable();
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glDisableClientState( GL_COLOR_ARRAY );
		
/*
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.25f ) );
		PolyLine<Vec3f> mLine;
		vector<Vec3f>::iterator it;
		int index = 0;
		for( it = mOrbitPath.begin(); it != mOrbitPath.end(); ++it ){
			if( index > 0 ){
				mLine.push_back( *it );
			}
			index ++;
		}
		gl::draw( mLine );
*/		
		
		gl::color( COLOR_BRIGHT_BLUE );
		Vec3f pos = Vec3f( cos( mOrbitStartAngle ), sin( mOrbitStartAngle ), 0.0f );
		gl::drawLine( pos * ( mOrbitRadius + mRadius * 1.2f ), pos * ( mOrbitRadius - mRadius * 1.2f ) );
//		gl::drawSphere( mStartRelPos, mRadius * 0.5f );
		gl::popModelView();
				
	}
}

string NodeTrack::getName()
{
	string name = mTrack->getTitle();
	if( name.size() < 1 ) name = "Untitled";
	return name;
}

uint64_t NodeTrack::getId()
{
    return mTrack->getItemId();
}
