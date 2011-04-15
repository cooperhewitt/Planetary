/*
 *  NodeAlbum.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "cinder/app/AppBasic.h"
#include "NodeArtist.h"
#include "NodeAlbum.h"
#include "NodeTrack.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/PolyLine.h"
#include "Globals.h"

using namespace ci;
using namespace ci::ipod;
using namespace std;

NodeAlbum::NodeAlbum( Node *parent, int index, const Font &font )
	: Node( parent, index, font )
{
	mRadiusDest			= mParentNode->mRadiusDest * Rand::randFloat( 0.01f, 0.035f );//0.01f;
	mRadius				= mRadiusDest;
	
	mIsHighlighted		= true;
	mHasAlbumArt		= false;
	mHasCreatedAlbumArt = false;
// NOW SET IN setChildOrbitRadii()
//	mIdealCameraDist	= mRadius * 13.5f;
	mEclipseStrength	= 0.0f;
}


void NodeAlbum::setData( PlaylistRef album )
{
// ALBUM ART IS HANDLED IN UPDATE()
	
	
// ALBUM INFORMATION
	mAlbum				= album;
	mNumTracks			= mAlbum->size();
	mHighestPlayCount	= 0;
	mLowestPlayCount	= 10000;
	for( int i = 0; i < mNumTracks; i++ ){
		float numPlays = (*mAlbum)[i]->getPlayCount();
		if( numPlays < mLowestPlayCount )
			mLowestPlayCount = numPlays;
		if( numPlays > mHighestPlayCount )
			mHighestPlayCount = numPlays;
	}
	
	
	
// ORBIT RADIUS	
	// FIXME: bad c++?
	float numAlbums		= ((NodeArtist*)mParentNode)->mNumAlbums + 2.0f;
	
	float invAlbumPer	= 1.0f/(float)numAlbums;
	float albumNumPer	= (float)mIndex * invAlbumPer;
	
	float minAmt		= mParentNode->mOrbitRadiusMin;
	float maxAmt		= mParentNode->mOrbitRadiusMax;
	float deltaAmt		= maxAmt - minAmt;
	mOrbitRadiusDest	= minAmt + deltaAmt * albumNumPer;// + Rand::randFloat( maxAmt * invAlbumPer * 0.35f );
	
	
// COLORS
	string name		= getName();
	char c1			= ' ';
	char c2			= ' ';
	if( name.length() >= 3 ){
		c1 = name[1];
		c2 = name[2];
	}
	
	int c1Int = constrain( int(c1), 32, 127 );
	int c2Int = constrain( int(c2), 32, 127 );
	
	mAsciiPer = ( c1Int - 32 )/( 127.0f - 32 );
	
	mHue				= mAsciiPer;
	mSat				= ( 1.0f - sin( mHue * M_PI ) ) * 0.1f + 0.15f;
	mColor				= Color( CM_HSV, mHue, mSat * 0.5f, 1.0f );
	mGlowColor			= mParentNode->mGlowColor;
	mEclipseColor       = mColor;


// PHYSICAL PROPERTIES
	mRadius				*= 0.85f;
	mSphere				= Sphere( mPos, mRadius * 7.5f );
	mAxialTilt			= Rand::randFloat( 5.0f );
    mAxialVel			= Rand::randFloat( 10.0f, 45.0f );
	mHasRings			= false;
	if( mNumTracks > 2 ) mHasRings = true;
	
	
// CHILD ORBIT RADIUS CONSTRAINTS
	mOrbitRadiusMin		= mRadius * 3.0f;
	mOrbitRadiusMax		= mRadius * 8.5f;
	

// TEXTURE IDs
    mPlanetTexIndex		= 3 * G_NUM_PLANET_TYPE_OPTIONS + c1Int%6;
	//( mIndex + 6 )%( G_NUM_PLANET_TYPES * G_NUM_PLANET_TYPE_OPTIONS );//(int)( normPlayCount * ( G_NUM_PLANET_TYPES - 1 ) );
	mCloudTexIndex		= c2Int%G_NUM_CLOUD_TYPES;
}


void NodeAlbum::update( const Matrix44f &mat )
{
	if( !mHasCreatedAlbumArt && mChildNodes.size() > 0 ){
		Surface albumArt	= ((NodeTrack*)mChildNodes[0])->mTrack->getArtwork( Vec2i( 256, 256 ) );
		if( albumArt ){
			
			int x				= mAsciiPer * 160 + 20;
			int y				= mAsciiPer * 64;
			Area a				= Area( x, y, x+50, y+128 );
			Surface crop		= albumArt.clone( a );
			
			Surface::Iter iter	= crop.getIter();
			while( iter.line() ) {
				while( iter.pixel() ) {
					if( iter.x() >= 25 ){
						int xi = x + iter.x() - 25;
						int yi = y + iter.y();
						ColorA c = albumArt.getPixel( Vec2i( xi, yi ) );
						iter.r() = c.r * 255.0f;
						iter.g() = c.g * 255.0f;
						iter.b() = c.b * 255.0f;
					} else {
						int xi = x + 24 - iter.x();
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
	
	
	double playbackTime		= app::getElapsedSeconds();
	double percentPlayed	= playbackTime/mOrbitPeriod;
	mOrbitAngle				= percentPlayed * TWO_PI + mOrbitStartAngle;

    Vec3f prevTransPos  = mTransPos;
    // if mTransPos hasn't been set yet, use a guess:
    // FIXME: set mTransPos correctly in the constructor
    if (prevTransPos.length() < 0.0001) prevTransPos = mat * mPos;    
	
	mRelPos		= Vec3f( cos( mOrbitAngle ), sin( mOrbitAngle ), 0.0f ) * mOrbitRadius;
	mPos		= mParentNode->mPos + mRelPos;
    
    float eclipseDist = 1.0f;
    if( mParentNode->mDistFromCamZAxisPer > 0.0f )
	{		
		Vec2f p		= mScreenPos;
		float r		= mSphereScreenRadius * 0.45f;
		float rsqrd = r * r;
		
		Vec2f P		= mParentNode->mScreenPos;
		float R		= mParentNode->mSphereScreenRadius * 0.325f;
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
			std::cout << "r = " << r << std::endl;
			std::cout << "R = " << R << std::endl;
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
	
	mAxialRot = Vec3f( 90.0f, app::getElapsedSeconds() * mAxialVel * 0.75f, mAxialTilt );
	
	mEclipseColor = ( mColor + Color::white() ) * 0.5f * eclipseDist;
    
	Node::update( mat );
	
	mTransVel = mTransPos - prevTransPos;	
}

void NodeAlbum::drawEclipseGlow()
{
	if( mIsSelected && mDistFromCamZAxisPer > 0.0f ){
		mParentNode->mEclipseStrength = mEclipseStrength * mZoomPer;
	}

	Node::drawEclipseGlow();
}

void NodeAlbum::drawOrbitRing( float pinchAlphaPer, GLfloat *ringVertsLowRes, GLfloat *ringVertsHighRes )
{		
	float newPinchAlphaPer = pinchAlphaPer;
	if( G_ZOOM < G_ALBUM_LEVEL - 0.5f ){
		newPinchAlphaPer = pinchAlphaPer;
	} else {
		newPinchAlphaPer = 1.0f;
	}
	
	
	if( mIsPlaying ){
		gl::color( ColorA( COLOR_BRIGHT_BLUE, 0.45f * newPinchAlphaPer ) );
	} else {
		gl::color( ColorA( COLOR_BLUE, 0.2f * newPinchAlphaPer ) );
	}
	
	gl::pushModelView();
	gl::translate( mParentNode->mTransPos );
	gl::scale( Vec3f( mOrbitRadius, mOrbitRadius, mOrbitRadius ) );
	gl::rotate( mMatrix );
	gl::rotate( Vec3f( 0.0f, 0.0f, toDegrees( mOrbitAngle ) ) );
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, ringVertsHighRes );
	glDrawArrays( GL_LINE_STRIP, 0, G_RING_HIGH_RES );
	glDisableClientState( GL_VERTEX_ARRAY );
	gl::popModelView();
	
	
	
	Node::drawOrbitRing( pinchAlphaPer, ringVertsLowRes, ringVertsHighRes );
}


void NodeAlbum::drawPlanet( const vector<gl::Texture> &planets )
{	
	if( mDistFromCamZAxis < -0.02f ){
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
			
			gl::pushModelView();
			gl::translate( mTransPos );
			gl::scale( Vec3f( mRadius, mRadius, mRadius ) );
			gl::rotate( mMatrix );
			gl::rotate( mAxialRot );
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


void NodeAlbum::drawClouds( const vector<gl::Texture> &clouds )
{
	if( mSphereScreenRadius > 5.0f && mDistFromCamZAxis < -0.02f ){
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
			
			gl::enableAdditiveBlending();
			
			gl::pushModelView();
			gl::translate( mTransPos );
				gl::pushModelView();
				float radius = mRadius + 0.0005f;
				gl::scale( Vec3f( radius, radius, radius ) );
				glEnable( GL_RESCALE_NORMAL );
				gl::rotate( mMatrix );
				gl::rotate( mAxialRot );
	// SHADOW CLOUDS
				glDisable( GL_LIGHTING );
				gl::color( ColorA( 0.0f, 0.0f, 0.0f, mCamDistAlpha * 0.5f ) );
				clouds[mCloudTexIndex].enableAndBind();
				glDrawArrays( GL_TRIANGLES, 0, numVerts );
				glEnable( GL_LIGHTING );
				gl::popModelView();
					
	// LIT CLOUDS
				gl::pushModelView();
				radius = mRadius + 0.00075f;
				gl::scale( Vec3f( radius, radius, radius ) );
				glEnable( GL_RESCALE_NORMAL );
				gl::rotate( mMatrix );
				gl::rotate( mAxialRot );
				gl::enableAdditiveBlending();
				gl::color( ColorA( mGlowColor, 1.0f ) );
				glDrawArrays( GL_TRIANGLES, 0, numVerts );
				gl::popModelView();
			gl::popModelView();
			
			glDisableClientState( GL_VERTEX_ARRAY );
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
			glDisableClientState( GL_NORMAL_ARRAY );
	}
}


void NodeAlbum::drawAtmosphere( const gl::Texture &tex, float pinchAlphaPer )
{
	if( mDistFromCamZAxis < -0.02f ){
		if( mIsSelected || mIsPlaying ){

			gl::pushModelView();
			gl::translate( mTransPos );
			Vec2f dir		= mScreenPos - app::getWindowCenter();
			float dirLength = dir.length()/500.0f;
			float angle		= atan2( dir.y, dir.x );
			float stretch	= 1.0f + dirLength * 0.15f;
			gl::enableAdditiveBlending();
			float alpha = 0.8f * ( 1.0f - dirLength );
			if( G_ZOOM <= G_ALBUM_LEVEL )
				alpha = pinchAlphaPer;
			gl::color( ColorA( ( mGlowColor + COLOR_BRIGHT_BLUE ) * 0.5f, alpha ) );
			Vec2f radius = Vec2f( mRadius * stretch, mRadius ) * 2.46f;
			tex.enableAndBind();
			gl::drawBillboard( Vec3f::zero(), radius, -toDegrees( angle ), mBbRight, mBbUp );
			tex.disable();
			gl::popModelView();
		}
	}
}



void NodeAlbum::drawRings( const gl::Texture &tex, GLfloat *planetRingVerts, GLfloat *planetRingTexCoords, float camAlpha )
{
	if( mHasRings && G_ZOOM > G_ARTIST_LEVEL ){
		if( mIsSelected || mIsPlaying ){
			gl::pushModelView();
			gl::translate( mTransPos );
			float c = mRadius * 9.0f;
			gl::scale( Vec3f( c, c, c ) );
			gl::rotate( mMatrix );
			gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * mAxialVel * 0.2f, 0.0f ) );
			
			gl::color( ColorA( mColor, camAlpha ) );
			tex.enableAndBind();
			glEnableClientState( GL_VERTEX_ARRAY );
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			glVertexPointer( 3, GL_FLOAT, 0, planetRingVerts );
			glTexCoordPointer( 2, GL_FLOAT, 0, planetRingTexCoords );
			
			gl::enableAdditiveBlending();
			glDrawArrays( GL_TRIANGLES, 0, 6 );
			
			glDisableClientState( GL_VERTEX_ARRAY );
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
			tex.disable();
			gl::popModelView();
		}
	}
}


void NodeAlbum::select( )
{
	if( !mIsSelected && mChildNodes.size() == 0 ){
		for (int i = 0; i < mNumTracks; i++) {
			TrackRef track		= (*mAlbum)[i];
			string name			= track->getTitle();
			NodeTrack *newNode	= new NodeTrack( this, i, mFont );
			mChildNodes.push_back( newNode );
			newNode->setData( track, mAlbum );
		}
		
		for( vector<Node*>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it ){
			(*it)->setSphereData( mTotalVertsHiRes, mSphereVertsHiRes, mSphereTexCoordsHiRes, mSphereNormalsHiRes,
								  mTotalVertsLoRes, mSphereVertsLoRes, mSphereTexCoordsLoRes, mSphereNormalsLoRes );
		}
		
		setChildOrbitRadii();
	}	
	Node::select();
}

void NodeAlbum::setChildOrbitRadii()
{
	float orbitRadius = mOrbitRadiusMin;
	float orbitOffset;
	for( vector<Node*>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it ){
		orbitOffset = (*it)->mRadius * 2.0f;
		orbitRadius += orbitOffset;
		(*it)->mOrbitRadiusDest = orbitRadius;
		orbitRadius += orbitOffset;
	}
	
	mIdealCameraDist	= orbitRadius * 2.25f;
}

string NodeAlbum::getName()
{
	string name = mAlbum->getAlbumTitle();
	if( name.size() < 1 ) name = "Untitled";
	return name;
}

uint64_t NodeAlbum::getId()
{
    return mAlbum->getAlbumId();
}
