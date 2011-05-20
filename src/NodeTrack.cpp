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
#include "cinder/ip/Resize.h"

using namespace ci;
using namespace ci::ipod;
using namespace std;

NodeTrack::NodeTrack( Node *parent, int index, const Font &font )
	: Node( parent, index, font )
{	
	mGen				= G_TRACK_LEVEL;
	mPos				= mParentNode->mPos;
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
	
	mMyTime				= Rand::randFloat( 250.0 );
}

void NodeTrack::setData( TrackRef track, PlaylistRef album )
{
	mAlbum			= album;
// TRACK INFORMATION
	mTrack			= track;
	mTrackLength	= (*mAlbum)[mIndex]->getLength();
	mPlayCount		= (*mAlbum)[mIndex]->getPlayCount();
	mStarRating		= (*mAlbum)[mIndex]->getStarRating();
	if( mStarRating == 0 ) mStarRating = 3;
	
	
	string name		= getName();
	char c1			= ' ';
	if( name.length() >= 3 ){
		c1 = name[1];
	}
	
	int c1Int = constrain( int(c1), 32, 127 );
	
	mAsciiPer = ( c1Int - 32 )/( 127.0f - 32 );
	
	
	
	
	//normalize playcount data
	float playCountDelta	= ( mParentNode->mHighestPlayCount - mParentNode->mLowestPlayCount ) + 1.0f;
	mNormPlayCount			= ( mPlayCount - mParentNode->mLowestPlayCount )/playCountDelta;
	
	mPlanetTexIndex			= constrain( (int)( mNormPlayCount * 8 ), 0, 7 );
	mCloudTexIndex			= c1Int%G_NUM_CLOUD_TYPES + G_NUM_CLOUD_TYPES;
	mPlanetTexIndex			= mStarRating - 1;
	mCloudTexIndex			= mStarRating - 1 + G_NUM_CLOUD_TYPES;

	
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
	mRadius				= math<float>::max( (mParentNode->mRadiusDest * 0.1f) * pow( mNormPlayCount + 0.5f, 2.0f ), 0.0025f );
	mSphere				= Sphere( mPos, mRadius );
	mIdealCameraDist	= mRadius * 14.0f;
	mCloudLayerRadius	= mRadiusDest * 0.1f;
	
	mOrbitPeriod		= mTrackLength;

	setStartAngle();
	
	mAxialTilt			= Rand::randFloat( -5.0f, 20.0f );
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
		mOrbitPath.push_back( Vec3f( cos( mOrbitStartAngle ), 0.0f, sin( mOrbitStartAngle ) ) );
		
		// Add middle positions
		int maxNumVecs		= 500;
		int currentNumVecs	= mPercentPlayed * maxNumVecs;
		for( int i=0; i<currentNumVecs; i++ ){
			float per = (float)(i)/(float)(maxNumVecs);
			float angle = mOrbitStartAngle + per * TWO_PI;
			Vec2f pos = Vec2f( cos( angle ), sin( angle ) );
			
			mOrbitPath.push_back( Vec3f( pos.x, 0.0f, pos.y ) );
		}
		
		// Add end position
		mOrbitPath.push_back( Vec3f( cos( mOrbitAngle ), 0.0f, sin( mOrbitAngle ) ) );
		
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
		float per				= (float)index/(float)orbitPathSize;
		Vec3f pos1				= *it * ( mOrbitRadius + radius );
		Vec3f pos2				= *it * ( mOrbitRadius - radius );
		
		mOrbitVerts[vIndex++]	= pos1.x;
		mOrbitVerts[vIndex++]	= pos1.y;
		mOrbitVerts[vIndex++]	= pos1.z;
		
		mOrbitVerts[vIndex++]	= pos2.x;
		mOrbitVerts[vIndex++]	= pos2.y;
		mOrbitVerts[vIndex++]	= pos2.z;
		
		mOrbitTexCoords[tIndex++]	= per;//mMyTime;
		mOrbitTexCoords[tIndex++]	= 0.0f;
		
		mOrbitTexCoords[tIndex++]	= per;//mMyTime;
		mOrbitTexCoords[tIndex++]	= 1.0f;
		
		index ++;
	}
}


void NodeTrack::update( const Matrix44f &mat, const Surface &surfaces )
{	
//////////////////////
// CREATE MOON TEXTURE
	if( !mHasCreatedAlbumArt ){
		int totalWidth		= 256;
		int halfWidth		= totalWidth/2;
		Surface albumArt	= mTrack->getArtwork( Vec2i( totalWidth, totalWidth ) );
		if( albumArt ){
			int x			= (int)(mNormPlayCount*halfWidth);
			int y			= Rand::randInt( halfWidth );
			
			int w			= (int)(mNormPlayCount*halfWidth);
			int h			= (int)(mNormPlayCount*halfWidth);
			
		// grab a section of the album art
			Area a			= Area( x, y, x+w, y+h );
			Surface crop	= Surface( totalWidth, totalWidth, false );
			ci::ip::resize( albumArt, a, &crop, Area( 0, 0, halfWidth, totalWidth ), FilterSincBlackman() );
			
		// iterate through it to make it a mirror image
			Surface::Iter iter = crop.getIter();
			while( iter.line() ) {
				while( iter.pixel() ) {
					int xi, yi;
					if( iter.x() >= halfWidth ){
						xi = iter.x() - halfWidth;
						yi = iter.y();
					} else {
						xi = (halfWidth-1) - iter.x();
						yi = iter.y();	
					}
					ColorA c = crop.getPixel( Vec2i( xi, yi ) );
					iter.r() = c.r * 255.0f;
					iter.g() = c.g * 255.0f;
					iter.b() = c.b * 255.0f;
				}
			}
			
		// add the planet texture
		// and add the shadow from the cloud layer
			Area planetArea			= Area( 0, totalWidth * mPlanetTexIndex, totalWidth, totalWidth * ( mPlanetTexIndex + 1 ) );
			Surface planetSurface	= surfaces.clone( planetArea );

			iter = planetSurface.getIter();
			while( iter.line() ) {
				while( iter.pixel() ) {
					ColorA albumColor	= crop.getPixel( Vec2i( iter.x(), iter.y() ) );
					ColorA surfaceColor	= planetSurface.getPixel( Vec2i( iter.x(), iter.y() ) );
					float planetVal		= surfaceColor.r;
					float cloudShadow	= ( 1.0f - surfaceColor.g ) * 0.5f + 0.5f;
					float highlight		= surfaceColor.b;
					
					ColorA final		= albumColor * 0.75f + planetVal * 0.25f;// + highlight;
					final *= cloudShadow;

					iter.r() = final.r * 255.0f;// + 25.0f;
					iter.g() = final.g * 255.0f;// + 25.0f;
					iter.b() = final.b * 255.0f;// + 25.0f;
				}
			}
			
			mAlbumArt			= gl::Texture( planetSurface );
			mHasAlbumArt		= true;
		}
		
		mHasCreatedAlbumArt = true;
	}
// END CREATE MOON TEXTURE	
//////////////////////////	
	
	
	
	mPrevTime		= mCurrentTime;
	mCurrentTime	= (float)app::getElapsedSeconds();
	
	if( !mIsPlaying ){
		mMyTime			+= mCurrentTime - mPrevTime;
	}
	
	float timeOffset	= mMyTime/mOrbitPeriod;
	mOrbitAngle		= ( mPercentPlayed + timeOffset ) * TWO_PI;
	
	Vec3f prevTransPos  = mTransPos;
    // if mTransPos hasn't been set yet, use a guess:
    // FIXME: set mTransPos correctly in the constructor
    if (prevTransPos.length() < 0.0001) prevTransPos = mat * mPos;    
    
	mRelPos				= Vec3f( cos( mOrbitAngle ), 0.0f, sin( mOrbitAngle ) ) * mOrbitRadius;
	mPos				= mParentNode->mPos + mRelPos;
	
	if( mIsPlaying ){
		mStartRelPos		= Vec3f( cos( mOrbitStartAngle ), 0.0f, sin( mOrbitStartAngle ) ) * mOrbitRadius;
		mTransStartPos		= mat * ( mParentNode->mPos + mStartRelPos );
	}
	
	
/////////////////////////
// CALCULATE ECLIPSE VARS
    float eclipseDist	= 1.0f;
	if( mParentNode->mParentNode->mDistFromCamZAxisPer > 0.001f && mDistFromCamZAxisPer > 0.001f )
	{
		Vec2f p		= mScreenPos;
		float r		= mSphereScreenRadius;
		float rsqrd = r * r;
		
		Vec2f P		= mParentNode->mParentNode->mScreenPos;
		float R		= mParentNode->mParentNode->mSphereScreenRadius;
		float Rsqrd	= R * R;
		float A		= M_PI * Rsqrd;
		
		float c		= p.distance( P );
		mEclipseDirBasedAlpha = 1.0f - constrain( c, 0.0f, 750.0f )/750.0f;
		if( mEclipseDirBasedAlpha > 0.9f )
			mEclipseDirBasedAlpha = 0.9f - ( mEclipseDirBasedAlpha - 0.9f ) * 9.0f;
		
		if( c < r + R )
		{
			float csqrd = c * c;
			float cos1	= ( Rsqrd + csqrd - rsqrd )/( 2.0f * R * c );
			float CBA	= acos( constrain( cos1, -1.0f, 1.0f ) );
			float CBD	= CBA * 2.0f;
			
			float cos2	= ( rsqrd + csqrd - Rsqrd )/( 2.0f * r * c );
			float CAB	= acos( constrain( cos2, -1.0f, 1.0f ) );
			float CAD	= CAB * 2.0f;
			float intersectingArea = CBA * Rsqrd - 0.5f * Rsqrd * sin( CBD ) + 0.5f * CAD * rsqrd - 0.5f * rsqrd * sin( CAD );
			mEclipseStrength = pow( 1.0f - ( A - intersectingArea ) / A, 2.0f );
			mParentNode->mParentNode->mEclipseStrength += mEclipseStrength;
			//if( mEclipseStrength > mParentNode->mEclipseStrength )
			//	mParentNode->mParentNode->mEclipseStrength = mEclipseStrength;
		}
			
		mEclipseAngle = atan2( P.y - p.y, P.x - p.x );
	}
	mEclipseColor = ( mColor + Color::white() ) * 0.5f * eclipseDist;
// END CALCULATE ECLIPSE VARS
/////////////////////////////
	
	
	Node::update( mat, surfaces );

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
	if( mSphereScreenRadius > 0.5f && mDistFromCamZAxis < -0.005f )
	{
		glEnable( GL_RESCALE_NORMAL );
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glEnableClientState( GL_NORMAL_ARRAY );
		int numVerts;
		if( mDistFromCamZAxisPer < 0.15f ){
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
		float radius = mRadius * mDeathPer;
		gl::scale( Vec3f( radius, radius, radius ) );
		gl::rotate( mMatrix );
		gl::rotate( Vec3f( 0.0f, 0.0f, mAxialTilt ) );
		gl::rotate( Vec3f( 0.0f, mCurrentTime * mAxialVel, 0.0f ) );
		gl::color( mEclipseColor );
		if( mHasAlbumArt ){
			mAlbumArt.enableAndBind();
		} else {
			planets[mPlanetTexIndex].enableAndBind();
		}
		glDrawArrays( GL_TRIANGLES, 0, numVerts );
		gl::popModelView();
		
		/*
		if( mHasAlbumArt && mNormPlayCount > 0.5f && ( mIsSelected || mIsPlaying ) ){
			gl::enableAdditiveBlending();
			gl::pushModelView();
			gl::translate( mTransPos );
			radius = ( mRadius + 0.00004f ) * mDeathPer;
			gl::scale( Vec3f( radius, radius, radius ) );
			gl::rotate( mMatrix );
			gl::rotate( Vec3f( 0.0f, 0.0f, mAxialTilt ) );
			gl::rotate( Vec3f( 0.0f, mCurrentTime * mAxialVel, 0.0f ) );
			gl::color( ColorA( mEclipseColor, 0.5f ) );
			planets[mPlanetTexIndex].enableAndBind();
			glDrawArrays( GL_TRIANGLES, 0, numVerts );
			gl::popModelView();
		}
		 */
		
		
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glDisableClientState( GL_NORMAL_ARRAY );
	}
	
}

void NodeTrack::drawClouds( const vector<gl::Texture> &clouds )
{
	if( mSphereScreenRadius > 2.0f && mDistFromCamZAxisPer > 0.0f ){
		if( mIsMostPlayed ){
			
			glEnableClientState( GL_VERTEX_ARRAY );
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			glEnableClientState( GL_NORMAL_ARRAY );
			int numVerts;
			if( mDistFromCamZAxisPer < 0.5f ){
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
			
			//glEnable( GL_LIGHTING );
			//glEnable( GL_RESCALE_NORMAL );
			
			gl::enableAdditiveBlending();
			gl::pushModelView();
			float radius = mRadius * mDeathPer + mCloudLayerRadius + ( (float)mGen - G_ZOOM ) * 0.001f;
			gl::scale( Vec3f( radius, radius, radius ) );
			
			gl::rotate( mMatrix );
			gl::rotate( Vec3f( 0.0f, 0.0f, mAxialTilt ) );
			gl::rotate( Vec3f( 0.0f, mCurrentTime * mAxialVel, 0.0f ) );
			float alpha = max( 0.7f - mDistFromCamZAxisPer, 0.0f );
			gl::color( ColorA( 1.0f, 1.0f, 1.0f, alpha ) );
			glDrawArrays( GL_TRIANGLES, 0, numVerts );
			gl::popModelView();
			gl::popModelView();
			
			glDisableClientState( GL_VERTEX_ARRAY );
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
			glDisableClientState( GL_NORMAL_ARRAY );
		}
	}
}

void NodeTrack::drawAtmosphere( const gl::Texture &tex, const gl::Texture &directionalTex, float pinchAlphaPer )
{
	if( mDistFromCamZAxis < -0.005f ){
		gl::enableAdditiveBlending();

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
		Vec2f radius = Vec2f( mRadius * stretch, mRadius ) * 2.45f;
		tex.enableAndBind();
		gl::drawBillboard( mTransPos, radius, -toDegrees( angle ), mBbRight, mBbUp );
		tex.disable();
		
		
		gl::color( ColorA( mColor, alpha * mEclipseDirBasedAlpha * mDeathPer ) );
		directionalTex.enableAndBind();
		gl::drawBillboard( mTransPos, radius, -toDegrees( mEclipseAngle ), mBbRight, mBbUp );
		directionalTex.disable();
	}
}



void NodeTrack::drawOrbitRing( float pinchAlphaPer, float camAlpha, const gl::Texture &orbitRingGradient, GLfloat *ringVertsLowRes, GLfloat *ringTexLowRes, GLfloat *ringVertsHighRes, GLfloat *ringTexHighRes )
{	
	float newPinchAlphaPer = pinchAlphaPer;
	if( G_ZOOM < G_TRACK_LEVEL - 0.5f ){
		newPinchAlphaPer = pinchAlphaPer;
	} else {
		newPinchAlphaPer = 1.0f;
	}
	
	gl::color( ColorA( COLOR_BLUE, camAlpha * mDeathPer ) );		
	
	gl::pushModelView();
	gl::translate( mParentNode->mTransPos );
	gl::scale( Vec3f( mOrbitRadius, mOrbitRadius, mOrbitRadius ) );
	gl::rotate( mMatrix );
	gl::rotate( Vec3f( 90.0f, 0.0f, toDegrees( mOrbitAngle ) ) );
	orbitRingGradient.enableAndBind();
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, ringVertsLowRes );
	glTexCoordPointer( 2, GL_FLOAT, 0, ringTexLowRes );
	glDrawArrays( GL_LINE_STRIP, 0, G_RING_LOW_RES );
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	orbitRingGradient.disable();
	gl::popModelView();
}

void NodeTrack::drawPlayheadProgress( float pinchAlphaPer, float camAlpha, const gl::Texture &tex, const gl::Texture &originTex )
{
	if( mIsPlaying ){
//		float newPinchAlphaPer = pinchAlphaPer;
//		if( G_ZOOM < G_TRACK_LEVEL - 0.5f ){
//			newPinchAlphaPer = pinchAlphaPer;
//		} else {
//			newPinchAlphaPer = 1.0f;
//		}
//		
		
		tex.enableAndBind();
		gl::pushModelView();
		gl::translate( mParentNode->mTransPos );
		gl::rotate( mMatrix );
		gl::color( ColorA( mParentNode->mParentNode->mGlowColor, camAlpha ) );
		
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, mOrbitVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, mOrbitTexCoords );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, mTotalOrbitVertices );		
		tex.disable();
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		gl::popModelView();
		
		Vec3f pos = Vec3f( cos( mOrbitStartAngle ), 0.0f, sin( mOrbitStartAngle ) );

		gl::color( ColorA( mParentNode->mParentNode->mGlowColor, 1.0f ) );
		originTex.enableAndBind();
		gl::drawBillboard( mParentNode->mTransPos + ( mMatrix * pos ) * mOrbitRadius, Vec2f( mRadius, mRadius ) * 2.15f, 0.0f, mMatrix * Vec3f::xAxis(), mMatrix * Vec3f::zAxis() );
		originTex.disable();
		
	//	gl::drawLine( pos * ( mOrbitRadius + mRadius * 1.2f ), pos * ( mOrbitRadius - mRadius * 1.2f ) );
		
				
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
