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
	mRadiusDest			= mParentNode->mRadiusDest * Rand::randFloat( 0.05f, 0.1f );//0.01f;
	mRadius				= mRadiusDest;
	
	mIsHighlighted		= true;
    mIsPlaying			= false;
	mHasClouds			= false;
	mIsMostPlayed		= false;
	mIsPopulated		= false;
	mHasAlbumArt		= false;
	mHasCreatedAlbumArt = false;
	
	mTotalOrbitVertices		= 0;
	mPrevTotalOrbitVertices = -1;
    mOrbitVerts				= NULL;
	mOrbitTexCoords			= NULL;
	mOrbitColors			= NULL;
	
	mMyTime				= 0.0f;//Rand::randFloat( 250.0 );
	
	mIdealCameraDist	= 0.075f;
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
	mNormPlayCount		= ( mPlayCount - mParentNode->mLowestPlayCount )/playCountDelta;
	
//	Surface albumArt		= mTrack->getArtwork( Vec2i( 128, 128 ) );
//	if( albumArt ){
//		int x				= (int)(mNormPlayCount*100);
//		int y				= c1Int%50;
//		int w				= 1;
//		int h				= mNormPlayCount * 60;
//		Area a				= Area( x, y, x+w, y+h );
//		Surface crop		= albumArt.clone( a );
//		mAlbumArt			= gl::Texture( crop );
//		mHasAlbumArt		= true;
//	}
//	
	
	mPlanetTexIndex			= (int)( mNormPlayCount * ( G_NUM_PLANET_TYPES - 1 ) );
	mCloudTexIndex			= c1Int%G_NUM_CLOUD_TYPES + G_NUM_CLOUD_TYPES;
	
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
	
	mRadius				= mParentNode->mRadiusDest * 0.1f;
	mRadius				= math<float>::max( (mParentNode->mRadiusDest * 0.1f) * pow( mNormPlayCount + 0.5f, 2.0f ), 0.0006f );
	mSphere				= Sphere( mPos, mRadius * 7.5f );
	
	mOrbitPeriod		= mTrackLength;

	setStartAngle();
	
	mAxialTilt			= Rand::randFloat( 5.0f, 30.0f );
    mAxialVel			= Rand::randFloat( 30.0f, 95.0f );

	mStartRelPos		= Vec3f( cos( mOrbitStartAngle ), sin( mOrbitStartAngle ), 0.0f ) * mOrbitRadius;
	mStartPos			= ( mParentNode->mPos + mStartRelPos ); 
}

void NodeTrack::setStartAngle()
{
	mPercentPlayed		= 0.0f;
	float timeOffset	= ( mMyTime )/mOrbitPeriod;
	mOrbitStartAngle	= timeOffset * TWO_PI;
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
		
		mOrbitVerts		= new float[mTotalOrbitVertices*3];
		mOrbitTexCoords	= new float[mTotalOrbitVertices*2];
		
		mPrevTotalOrbitVertices = mTotalOrbitVertices;
	}
	
	int vIndex		= 0;
	int tIndex		= 0;
	int index		= 0;
	float radius	= mRadius;
//	float alpha		= constrain( G_ZOOM - G_ARTIST_LEVEL, 0.0f, 1.0f ) * 0.3f;
	
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
		
		mOrbitTexCoords[tIndex++]	= mMyTime;
		mOrbitTexCoords[tIndex++]	= 0.0f;
		
		mOrbitTexCoords[tIndex++]	= mMyTime;
		mOrbitTexCoords[tIndex++]	= 1.0f;
		
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
	int totalWidth = 128;
	if( !mHasCreatedAlbumArt ){
		Surface albumArt	= mTrack->getArtwork( Vec2i( totalWidth, totalWidth ) );
		if( albumArt ){
			int w			= 24;
			int halfWidth	= w/2;
			int h			= 50;
			int x			= (int)(mNormPlayCount*50);
			int y			= Rand::randInt( totalWidth/2 );
			Area a			= Area( x, y, x+w, y+( mNormPlayCount * h ) );
			Surface crop	= albumArt.clone( a );
			
			Surface::Iter iter = crop.getIter();
			while( iter.line() ) {
				while( iter.pixel() ) {
					if( iter.x() >= halfWidth ){
						int xi = x + iter.x() - halfWidth;
						int yi = y + iter.y();
						ColorA c = albumArt.getPixel( Vec2i( xi, yi ) );
						iter.r() = c.r * 255.0f;
						iter.g() = c.g * 255.0f;
						iter.b() = c.b * 255.0f;
					} else {
						int xi = x + (halfWidth-1) - iter.x();
						int yi = y + iter.y();
						ColorA c = albumArt.getPixel( Vec2i( xi, yi ) );
						iter.r() = c.r * 255.0f;
						iter.g() = c.g * 255.0f;
						iter.b() = c.b * 255.0f;
					}
				}
			}
			
			
			mAlbumArt			= gl::Texture( crop );
			mHasAlbumArt		= true;
		}
		
		mHasCreatedAlbumArt = true;
	}
	
	
	
	mPrevTime		= mCurrentTime;
	mCurrentTime	= (float)app::getElapsedSeconds();
	
	if( !mIsPlaying ){
		mMyTime			+= mCurrentTime - mPrevTime;
	}
	
	// TODO: THIS IS AWKWARD. This is so the non-playing tracks still orbit
	float timeOffset	= mMyTime/mOrbitPeriod;
	mOrbitAngle		= ( mPercentPlayed + timeOffset ) * TWO_PI;
	//mOrbitStartAngle = mOrbitAngle;
	
	Vec3f prevTransPos  = mTransPos;
    // if mTransPos hasn't been set yet, use a guess:
    // FIXME: set mTransPos correctly in the constructor
    if (prevTransPos.length() < 0.0001) prevTransPos = mat * mPos;    
    
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
		if( mIsSelected )
		{
			float csqrd = c * c;
			float cos1	= ( Rsqrd + csqrd - rsqrd )/( 2.0f * R * c );
			float CBA	= acos( constrain( cos1, -1.0f, 1.0f ) );
			float CBD	= CBA * 2.0f;
			
			float cos2	= ( rsqrd + csqrd - Rsqrd )/( 2.0f * r * c );
			float CAB	= acos( constrain( cos2, -1.0f, 1.0f ) );
			float CAD	= CAB * 2.0f;
			float intersectingArea = CBA * Rsqrd - 0.5f * Rsqrd * sin( CBD ) + 0.5f * CAD * rsqrd - 0.5f * rsqrd * sin( CAD );
			mEclipseStrength = 1.0f - ( A - intersectingArea ) / A;

			if( mDistFromCamZAxisPer > 0.0f ){
				if( mEclipseStrength > mParentNode->mParentNode->mEclipseStrength )
					mParentNode->mParentNode->mEclipseStrength = mEclipseStrength;
			}
			
//			std::cout << "================== " << std::endl;
//			std::cout << "c = " << c << std::endl;
//			std::cout << "A = " << A << std::endl;
//			std::cout << "CBA = " << CBA << std::endl;
//			std::cout << "CAB = " << CAB << std::endl;
//			std::cout << "intersectingArea = " << intersectingArea << std::endl;
//			std::cout << "totalRadius = " << totalRadius << std::endl;		
//			std::cout << "mEclipseStrength = " << mEclipseStrength << std::endl;

		}
	}
	mEclipseColor = ( mColor + Color::white() ) * 0.5f * eclipseDist;
	
	Node::update( mat );

	mTransVel = mTransPos - prevTransPos;	
}

void NodeTrack::drawEclipseGlow()
{
	if( mIsSelected && mDistFromCamZAxisPer > 0.0f ){
		if( mEclipseStrength > mParentNode->mParentNode->mEclipseStrength )
			mParentNode->mParentNode->mEclipseStrength = mEclipseStrength;
	}
}

void NodeTrack::drawPlanet( const vector<gl::Texture> &planets )
{	
	if( mSphereScreenRadius > 2.0f && mDistFromCamZAxis < -0.005f ){
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
		gl::scale( Vec3f( mRadius, mRadius, mRadius ) * mDeathPer );
		gl::rotate( mMatrix );
		gl::rotate( Vec3f( 0.0f, 0.0f, mAxialTilt ) );
		gl::rotate( Vec3f( 90.0f, mCurrentTime * mAxialVel, 0.0f ) );
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

void NodeTrack::drawClouds( const vector<gl::Texture> &clouds )
{
	if( mSphereScreenRadius > 2.0f && mDistFromCamZAxis < -0.005f ){
		if( mCamDistAlpha > 0.05f && mIsMostPlayed ){
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

			gl::pushModelView();
			gl::translate( mTransPos );
			clouds[mCloudTexIndex].enableAndBind();

			glEnable( GL_LIGHTING );
			// LIT CLOUDS
			gl::pushModelView();
			float radius = mRadius * mDeathPer + 0.000025f;
			gl::scale( Vec3f( radius, radius, radius ) );
			glEnable( GL_RESCALE_NORMAL );
			gl::rotate( mMatrix );
			gl::rotate( Vec3f( 0.0f, 0.0f, mAxialTilt ) );
			gl::rotate( Vec3f( 90.0f, mCurrentTime * mAxialVel, 0.0f ) );
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
	float newPinchAlphaPer = pinchAlphaPer;
	if( G_ZOOM < G_TRACK_LEVEL - 0.5f ){
		newPinchAlphaPer = pinchAlphaPer;
	} else {
		newPinchAlphaPer = 1.0f;
	}
	
	
	if( mIsMostPlayed )
		gl::color( ColorA( COLOR_BRIGHT_BLUE, 0.2f * mDeathPer ) );
	else
		gl::color( ColorA( COLOR_BLUE, 0.2f * mDeathPer ) );		
	
	gl::pushModelView();
	gl::translate( mParentNode->mTransPos );
	gl::scale( Vec3f( mOrbitRadius, mOrbitRadius, mOrbitRadius ) );
	gl::rotate( mMatrix );
	gl::rotate( Vec3f( 0.0f, 0.0f, toDegrees( mOrbitAngle ) ) );
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, ringVertsLowRes );
	glDrawArrays( GL_LINE_STRIP, 0, G_RING_LOW_RES );
	glDisableClientState( GL_VERTEX_ARRAY );
	gl::popModelView();
}

void NodeTrack::drawAtmosphere( const gl::Texture &tex, const gl::Texture &directionalTex, float pinchAlphaPer )
{
	if( mDistFromCamZAxis < -0.005f ){
		gl::enableAdditiveBlending();

		gl::pushModelView();
		gl::translate( mTransPos );
		Vec2f dir		= mScreenPos - app::getWindowCenter();
		float dirLength = dir.length()/500.0f;
		float angle		= atan2( dir.y, dir.x );
		float stretch	= 1.0f + dirLength * 0.1f;
		gl::enableAdditiveBlending();
		float alpha = 1.0f;
		
//		float alpha = 0.3f * ( 1.0f - dirLength );
//		if( G_ZOOM <= G_ALBUM_LEVEL )
//			alpha = pinchAlphaPer;

		gl::color( ColorA( ( mGlowColor + COLOR_BRIGHT_BLUE ) * 0.5f, alpha ) );
		Vec2f radius = Vec2f( mRadius * stretch, mRadius ) * 2.5f;
		tex.enableAndBind();
		gl::drawBillboard( Vec3f::zero(), radius, -toDegrees( angle ), mBbRight, mBbUp );
		tex.disable();
		gl::popModelView();
	}
}


void NodeTrack::drawPlayheadProgress( float pinchAlphaPer, const gl::Texture &tex )
{
	if( mIsPlaying ){
		float newPinchAlphaPer = pinchAlphaPer;
		if( G_ZOOM < G_TRACK_LEVEL - 0.5f ){
			newPinchAlphaPer = pinchAlphaPer;
		} else {
			newPinchAlphaPer = 1.0f;
		}
		
		gl::color( ColorA( COLOR_BRIGHT_BLUE, 0.4f * newPinchAlphaPer ) );
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, mOrbitVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, mOrbitTexCoords );
		
		tex.enableAndBind();
		gl::pushModelView();
		gl::translate( mParentNode->mTransPos );
		gl::rotate( mMatrix );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, mTotalOrbitVertices );		
		tex.disable();
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		
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
