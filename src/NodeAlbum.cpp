/*
 *  NodeAlbum.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/21/11.
 *  Copyright 2013 Smithsonian Institution. All rights reserved.
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
#include "cinder/ip/Resize.h"
#include "BloomGl.h"

using namespace ci;
using namespace ci::ipod;
using namespace std;

NodeAlbum::NodeAlbum( Node *parent, int index, const Font &font, const Font &smallFont, const Surface &hiResSurfaces, const Surface &loResSurfaces, const Surface &noAlbumArt )
	: Node( parent, index, font, smallFont, hiResSurfaces, loResSurfaces, noAlbumArt )
{
	mGen				= G_ALBUM_LEVEL;
	mPos				= mParentNode->mPos;
	
	mIsHighlighted		= true;
	mIsBlockedBySun		= false;
	mBlockedBySunPer	= 1.0f;
	mHasAlbumArt		= false;
// NOW SET IN setChildOrbitRadii()
//	mIdealCameraDist	= mRadius * 13.5f;
	mEclipseStrength	= 0.0f;
	mClosenessFadeAlpha	= 1.0f;	
    
    mShadowVerts = NULL;
    mShadowTexCoords = NULL;
}

void NodeAlbum::setData( PlaylistRef album )
{	
	
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

    mId = mAlbum->getAlbumId();

// ORBIT RADIUS	
	// FIXME: bad c++?
	float numAlbums		= ((NodeArtist*)mParentNode)->getNumAlbums() + 2.0f;
	
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
	mHasRings			= false;
	if( mNumTracks > 2 ) mHasRings = true;
	mTotalLength		= mAlbum->getTotalLength();
//	std::cout<<"NodeAlbum total length = " << mTotalLength<<std::endl;
	mReleaseYear		= (*mAlbum)[0]->getReleaseYear();
	
	mRadiusInit			= mParentNode->mRadiusDest * constrain( mTotalLength * 0.00004f, 0.01f, 0.06f );//Rand::randFloat( 0.01f, 0.035f );
	mRadius				= mRadiusInit;
	mCloudLayerRadius	= mRadius * 0.015f;
	
	mSphere				= Sphere( mPos, mRadiusInit );
	mAxialTilt			= Rand::randFloat( 5.0f );
    mAxialVel			= Rand::randFloat( 10.0f, 45.0f );
	mAxialRot			= Vec3f( 0.0f, Rand::randFloat( 150.0f ), mAxialTilt );
	
// CHILD ORBIT RADIUS CONSTRAINTS
	mOrbitRadiusMin		= mRadiusInit * 3.0f;
	mOrbitRadiusMax		= mRadiusInit * 8.5f;
	

// TEXTURE IDs
    mPlanetTexIndex		= c1Int%G_NUM_CLOUD_TYPES;//3 * G_NUM_PLANET_TYPE_OPTIONS + c1Int%6;
	mCloudTexIndex		= c2Int%G_NUM_CLOUD_TYPES;
	
	
// CREATE PLANET TEXTURE
	int totalWidth		= 128;
	if( G_IS_IPAD2 ) totalWidth = 256;
    
	int halfWidth		= totalWidth/2;
	int border			= 10;
	mAlbumArtSurface	= (*mAlbum)[0]->getArtwork( Vec2i( totalWidth, totalWidth ) );
	
	bool hasAlbumArt = true;
	if( !mAlbumArtSurface ){
		hasAlbumArt = false;
		mAlbumArtSurface = mNoAlbumArtSurface;
	}
	
	int x			= (int)( mAsciiPer*halfWidth );
	int y			= (int)( mAsciiPer*border );
	
	int w			= (int)( halfWidth );
	int h			= (int)( totalWidth - border*2 );
	
	if( !hasAlbumArt ){
		w = 1;
		h = h/8;
	}
	
	// grab a section of the album art
	Surface crop	= Surface( totalWidth, totalWidth, false );
	Surface crop2	= Surface( totalWidth, totalWidth, false );
	ci::ip::resize( mAlbumArtSurface, Area( x, y, x+w, y+h ), &crop, Area( 0, 0, halfWidth, totalWidth ), FilterCubic() );
	
	// make it a mirror image
	Surface::Iter iter = crop2.getIter();
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
	//	
	//	// fix the polar pinching
	//	Surface::Iter iter2 = crop.getIter();
	//	while( iter2.line() ) {
	//		float cosTheta = cos( M_PI * ( iter2.y() - (float)( totalWidth - 1 )/2.0f ) / (float)( totalWidth - 1 ) );
	//		
	//		while( iter2.pixel() ) {
	//			float phi	= TWO_PI * ( iter2.x() - halfWidth ) / (double)totalWidth;
	//			float phi2	= phi * cosTheta;
	//			int i2 = phi2 * totalWidth/TWO_PI + halfWidth;
	//			
	//			if( i2 < 0 || i2 > totalWidth-1 ){
	//				// this should never happen
	//				iter2.r() = 255.0f;
	//				iter2.g() = 0.0f;
	//				iter2.b() = 0.0f;
	//			} else {
	//				ColorA c = crop2.getPixel( Vec2i( i2, iter2.y() ) );
	//				iter2.r() = c.r * 255.0f;
	//				iter2.g() = c.g * 255.0f;
	//				iter2.b() = c.b * 255.0f;
	//			}
	//		}
	//	}
	
	// add the planet texture
	// and add the shadow from the cloud layer
	Area planetArea			= Area( 0, totalWidth * mPlanetTexIndex, totalWidth, totalWidth * ( mPlanetTexIndex + 1 ) );
	Surface planetSurface;
	if( G_IS_IPAD2 ) planetSurface = mHighResSurfaces.clone( planetArea );
	else			 planetSurface = mLowResSurfaces.clone( planetArea );
	
	iter = planetSurface.getIter();
	while( iter.line() ) {
		while( iter.pixel() ) {
			ColorA albumColor	= crop2.getPixel( Vec2i( iter.x(), iter.y() ) );
			ColorA surfaceColor	= planetSurface.getPixel( Vec2i( iter.x(), iter.y() ) );
			float planetVal		= surfaceColor.r;
			float cloudShadow	= surfaceColor.g * 0.5f + 0.5f;
			//float brightness	= surfaceColor.b;
			
			ColorA final		= albumColor;// + planetVal * 0.25f;
			final *= cloudShadow * planetVal;
			
			iter.r() = constrain( final.r * 255.0f - 0.0f, 0.0f, 255.0f );// + 25.0f;
			iter.g() = constrain( final.g * 255.0f - 0.0f, 0.0f, 255.0f );// + 25.0f;
			iter.b() = constrain( final.b * 255.0f - 0.0f, 0.0f, 255.0f );// + 25.0f;
		}
	}
	
    gl::Texture::Format fmt;
    fmt.enableMipmapping( true );
    fmt.setMinFilter( GL_LINEAR_MIPMAP_LINEAR );
	
	mAlbumArtTex		= gl::Texture( planetSurface, fmt );
	mHasAlbumArt		= true;

}


void NodeAlbum::update( float param1, float param2 )
{
	mRadiusDest		= mRadiusInit * param1;
	mRadius			-= ( mRadius - mRadiusDest ) * 0.2f;
	mSphere			= Sphere( mPos, mRadius );
	
	//double playbackTime		= app::getElapsedSeconds();
	//double percentPlayed	= playbackTime/mOrbitPeriod;
	mOrbitAngle	+= param2 * mAxialVel * 0.05f;
	mAxialRot.y -= mAxialVel * ( param2 * 10.0f );
		
    Vec3f prevPos  = mPos;
	
	mRelPos		= Vec3f( cos( mOrbitAngle ), 0.0f, sin( mOrbitAngle ) ) * mOrbitRadius;
	mPos		= mParentNode->mPos + mRelPos;
	
	
	
	/////////////////////////
	// CALCULATE ECLIPSE VARS
    if( mParentNode->mDistFromCamZAxis > 0.0f && mDistFromCamZAxis > 0.0f && mIsHighlighted ) //&& ( mIsSelected || mIsPlaying )
	{		
		Vec2f p		= mScreenPos;
		float r		= mSphereScreenRadius;
		float rsqrd = r * r;
		
		Vec2f P		= mParentNode->mScreenPos;
		float R		= mParentNode->mSphereScreenRadius * 0.85f;
		float Rsqrd	= R * R;
		float A		= M_PI * Rsqrd;
		
		float c		= p.distance( P );
		mEclipseDirBasedAlpha = 1.0f - constrain( c, 0.0f, 1500.0f )/1500.0f;
		if( mEclipseDirBasedAlpha > 0.9f )
			mEclipseDirBasedAlpha = 0.9f - ( mEclipseDirBasedAlpha - 0.9f ) * 9.0f;
		
		
		if( c < r + R ){
			float csqrd = c * c;
			float cos1	= ( Rsqrd + csqrd - rsqrd )/( 2.0f * R * c );
			float CBA	= acos( constrain( cos1, -1.0f, 1.0f ) );
			float CBD	= CBA * 2.0f;
			
			float cos2	= ( rsqrd + csqrd - Rsqrd )/( 2.0f * r * c );
			float CAB	= acos( constrain( cos2, -1.0f, 1.0f ) );
			float CAD	= CAB * 2.0f;
			float intersectingArea = CBA * Rsqrd - 0.5f * Rsqrd * sin( CBD ) + 0.5f * CAD * rsqrd - 0.5f * rsqrd * sin( CAD );
			mEclipseStrength = pow( 1.0f - ( A - intersectingArea ) / A, 2.0f );
			
			if( mDistFromCamZAxisPer > 0.0f ){
				if( mEclipseStrength > mParentNode->mEclipseStrength )
					mParentNode->mEclipseStrength = mEclipseStrength;
			}
		}
		
		mEclipseAngle = atan2( P.y - p.y, P.x - p.x );
		
		// if the album is further away from the camera than the sun,
		// check to see if it is behind the sun.
		float blockThresh = 1.0f;
		if( mDistFromCamZAxis > mParentNode->mDistFromCamZAxis ){
			if( c < R * ( blockThresh * 2.0f ) && c >= R * blockThresh ){
				mBlockedBySunPer = ( ( c - R )/(R*blockThresh) ) * 0.5f + 0.5f;
			} else if( c < R * blockThresh ){
				mBlockedBySunPer = 0.5f;
			} else {
				mBlockedBySunPer = 1.0f;
			}
		} else {
			mBlockedBySunPer = 1.0f;
		}
    } else {
		mBlockedBySunPer = 1.0f;
	}

	mEclipseColor = ( mColor + Color::white() ) * 0.5f * ( 1.0f - mEclipseStrength * 0.5f );
	// END CALCULATE ECLIPSE VARS
	/////////////////////////////
	
	
	
	mCloudLayerRadius	= mRadius * 0.005f + mDistFromCamZAxisPer * 0.005;
	
	Node::update( param1, param2 );
	
	mVel = mPos - prevPos;	
}

void NodeAlbum::drawEclipseGlow()
{
	Node::drawEclipseGlow();
}

void NodeAlbum::drawPlanet( const gl::Texture &tex )
{	
	if( mDistFromCamZAxis > mRadius ){
        
		glPushMatrix();
		gl::translate( mPos );
		gl::scale( Vec3f( mRadius, mRadius, mRadius ) * mDeathPer );
		gl::rotate( mAxialRot );
		
		mAlbumArtTex.enableAndBind();
		
		
		if( mIsHighlighted ){
			const float eclipseAmt = ( 1.0f - mEclipseStrength ) * 0.5f + 0.5f;
			gl::color( ColorA( eclipseAmt, eclipseAmt, eclipseAmt, mClosenessFadeAlpha * mBlockedBySunPer ) );
			gl::enableAlphaBlending();
		} else {
			gl::color( ColorA( BLUE, mClosenessFadeAlpha * mBlockedBySunPer ) );
			gl::enableAdditiveBlending();
		}
		
		
		// when the planet goes offscreen, the screenradius becomes huge. 
		// so if the screen radius is greater than 600, assume it is offscreen and just render a lo-res version
		// consider frustum culling?
		if( mSphereScreenRadius < 800.0f ){
			if( mSphereScreenRadius > 50.0f ){
                mHiSphere->draw();
			} else if( mSphereScreenRadius > 30.0f ){
                mMdSphere->draw();
			} else if( mSphereScreenRadius > 10.0f ){
                mLoSphere->draw();
			} else {
                mTySphere->draw();
			}
		} else {
            mLoSphere->draw();
		}
        
        mAlbumArtTex.disable();
        
		glPopMatrix();
		
	}
}


void NodeAlbum::drawClouds( const vector<gl::Texture> &clouds )
{
	if( mSphereScreenRadius > 5.0f && mDistFromCamZAxis > mRadius ){		
        
        // FIXME: by drawing this sphere twice, below, we bind and unbind the same vertex array - could be optimized? (maybe lodSphere->bind(), lodSphere->drawArrays(), lodSphere->unbind()?)
        BloomSphere *lodSphere = NULL;
        
		// when the planet goes offscreen, the screenradius becomes huge. 
		// so if the screen radius is greater than 500, assume it is offscreen and just render a lo-res version
		// consider frustum culling?
		if( mSphereScreenRadius < 800.0f ){
			if( mSphereScreenRadius > 50.0f ){
                lodSphere = mHiSphere;
			} else if( mSphereScreenRadius > 30.0f ){
                lodSphere = mMdSphere;
			} else if( mSphereScreenRadius > 10.0f ){
                lodSphere = mLoSphere;
			} else {
                lodSphere = mTySphere;
			}
		} else {
            lodSphere = mLoSphere;
		}
		
		
		
        
		glPushMatrix();
		gl::translate( mPos );
		
		clouds[mCloudTexIndex].enableAndBind();
        
        const float radius = mRadius * mDeathPer + mCloudLayerRadius;
        const float alpha = constrain( ( 5.0f - mDistFromCamZAxis ) * 0.2f, 0.0f, 0.334f ) * mClosenessFadeAlpha;        

        gl::scale( Vec3f( radius, radius, radius ) );
        gl::rotate( mAxialRot * Vec3f( 1.0f, 0.75f, 1.0f ) + Vec3f( 0.0f, 0.5f, 0.0f ) ); 
		
		// SHADOW CLOUDS
		if( mIsHighlighted ){
			gl::enableAlphaBlending();
			if( G_IS_IPAD2 || G_DEBUG ){
				gl::color( ColorA( 0.0f, 0.0f, 0.0f, alpha ) );
				lodSphere->draw();
			}
			const float eclipseAmt = ( 1.0f - mEclipseStrength ) * 0.5f + 0.5f;
			gl::color( ColorA( eclipseAmt, eclipseAmt, eclipseAmt, alpha * 2.0f ) );
		} else {
			gl::color( ColorA( BLUE, alpha * 2.0f ) );
		}
        
        // LIT CLOUDS
		gl::enableAdditiveBlending();
		const float radius2 = (mRadius * mDeathPer + mCloudLayerRadius*1.5f) / radius;
		gl::scale( Vec3f( radius2, radius2, radius2 ) );
		lodSphere->draw();
        
        clouds[mCloudTexIndex].disable();
        
		glPopMatrix();
		
	}
}


void NodeAlbum::drawAtmosphere( const Vec3f &camEye, const Vec2f &center, const gl::Texture &tex, const gl::Texture &directionalTex, float pinchAlphaPer, float scaleSliderOffset )
{
	if( mClosenessFadeAlpha > 0.0f && mDistFromCamZAxis > mRadius ){		
		float alpha = ( 1.0f - mScreenDistToCenterPer * 0.75f ) + mEclipseStrength;
		alpha *= mDeathPer * mClosenessFadeAlpha * ( mBlockedBySunPer - 0.5f ) * 2.0f;
		Vec2f radius( mRadius, mRadius );
		radius *= ( 2.42f + scaleSliderOffset + max( ( mSphereScreenRadius - 160.0f ) * 0.001f, 0.0f ) ) * mDeathPer;
		

		if( mIsHighlighted ){
			gl::color( ColorA( BRIGHT_BLUE, ( 1.0f + mEclipseStrength * 2.0f ) * mClosenessFadeAlpha ) );
			tex.enableAndBind();
			bloom::gl::drawSphericalBillboard( camEye, mPos, radius, 0.0f );
			tex.disable();
			gl::color( ColorA( mColor, alpha * mEclipseDirBasedAlpha ) );
		} else {
			gl::color( ColorA( BRIGHT_BLUE, alpha * mEclipseDirBasedAlpha ) );
		}
		
		
		directionalTex.enableAndBind();
		bloom::gl::drawSphericalRotatedBillboard( mPos, camEye, mParentNode->mPos, radius );        
		directionalTex.disable();
	}
}


void NodeAlbum::drawOrbitRing( float pinchAlphaPer, float camAlpha, const OrbitRing &orbitRing, float fadeInAlphaToArtist, float fadeInArtistToAlbum )
{		
	float newPinchAlphaPer = pinchAlphaPer;
	if( G_ZOOM < G_ALBUM_LEVEL - 0.5f ){
		newPinchAlphaPer = pinchAlphaPer;
	} else {
		newPinchAlphaPer = 1.0f;
	}
	
	if( mIsPlaying ){
		gl::color( ColorA( BRIGHT_BLUE, 0.5f * camAlpha * fadeInAlphaToArtist ) );
	} else {
		if( mIsHighlighted ){
			gl::color( ColorA( BLUE, 0.5f * camAlpha * fadeInAlphaToArtist ) );
		} else {
			gl::color( ColorA( BLUE, 0.5f * camAlpha * fadeInAlphaToArtist * 0.3f ) );
		}
	}
	
	glPushMatrix();
	gl::translate( mParentNode->mPos );
	gl::scale( Vec3f( mOrbitRadius, mOrbitRadius, mOrbitRadius ) );
	gl::rotate( Vec3f( 90.0f, 0.0f, toDegrees( mOrbitAngle ) ) );	
    orbitRing.drawHighRes();
	glPopMatrix();
	
	Node::drawOrbitRing( pinchAlphaPer, camAlpha, orbitRing, fadeInAlphaToArtist, fadeInArtistToAlbum );
}



void NodeAlbum::drawRings( const gl::Texture &tex, const PlanetRing &planetRing, float camAlpha )
{
	if( mHasRings && G_ZOOM > G_ARTIST_LEVEL ){
		if( mIsSelected || mIsPlaying ){
			gl::enableAdditiveBlending();
			
			glPushMatrix();
			gl::translate( mPos );
			
            float c = 0.5f * mIdealCameraDist;
			gl::scale( Vec3f( c, c, c ) );
			gl::rotate( Vec3f( 0.0f, app::getElapsedSeconds() * mAxialVel * 0.2f, 0.0f ) );
			
			float zoomPer = constrain( 1.0f - ( mGen - G_ZOOM ), 0.0f, 1.0f );
			gl::color( ColorA( mColor, camAlpha * zoomPer ) );
			
            tex.enableAndBind();
            planetRing.draw();
			tex.disable();
            
			glPopMatrix();
		}
	}
}


void NodeAlbum::select()
{
	if( !mIsSelected ){
		if( mChildNodes.size() == 0 ){
			for (int i = 0; i < mNumTracks; i++) {
				TrackRef track		= (*mAlbum)[i];
				string name			= track->getTitle();
				NodeTrack *newNode	= new NodeTrack( this, i, mFont, mSmallFont, mHighResSurfaces, mLowResSurfaces, mNoAlbumArtSurface );
				mChildNodes.push_back( newNode );
				newNode->setData( track, mAlbum, mAlbumArtSurface );
			}
			
			for( vector<Node*>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it ){
				(*it)->setSphereData( mHiSphere, mMdSphere, mLoSphere, mTySphere );
			}
			
			setChildOrbitRadii();
			
			
		} else {
			for( vector<Node*>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it ){
				(*it)->setIsDying( false );
			}
		}
	}	
	Node::select();
}

void NodeAlbum::findShadows( float camAlpha )
{	
	if( mIsHighlighted ){
		Vec3f P0, P1, P2, P4;
		Vec3f P3a, P3b;
		Vec3f P5a, P5b, P6a, P6b;
		Vec3f outerTanADir, outerTanBDir, innerTanADir, innerTanBDir;
		
		float r0, r1, r0Inner, rTotal;
		float d, dMid, dMidSqrd;
		
		// Positions	
		P0				= mParentNode->mPos;
		P1				= mPos;
		P4				= ( P0 + P1 )*0.5f;
		
		// Radii
		r0				= mParentNode->mRadius * 0.175f;
		r1				= mRadius * 1.05f;
		rTotal			= r0 + r1;
		r0Inner			= abs( r0 - r1 );
		
		d				= P0.distance( P1 );
		dMid			= d * 0.5f;
		dMidSqrd		= dMid * dMid;
		
		float newRTotal		= r0Inner + dMid;
		float newRDelta		= abs( dMid - r0Inner );
		
		if( dMid > newRTotal ){
			// std::cout << "not intersecting" << std::endl;
		} else if( dMid < newRDelta ){
			// std::cout << "contained" << std::endl;
		} else if( dMid == 0 ){
			// std::cout << "concentric" << std::endl;
		} else {
			float a = ( dMidSqrd - r0Inner * r0Inner + dMidSqrd ) / d;
			P2 = P4 + a * ( ( P0 - P4 ) / dMid );
			
			float h = sqrt( dMidSqrd - a * a ) * 0.5f;
			
			Vec3f p = ( P1 - P0 )/dMid;
			
			P3a = P2 + h * Vec3f( -p.z, p.y, p.x );
			P3b = P2 - h * Vec3f( -p.z, p.y, p.x );
			
			
			Vec3f P3aDirNorm = P3a - P0;
			P3aDirNorm.normalize();
			
			Vec3f P3bDirNorm = P3b - P0;
			P3bDirNorm.normalize();
			
			P5a = P3a + P3aDirNorm * r1;
			P5b = P3b + P3bDirNorm * r1;
			P6a = P1 + P3aDirNorm * r1; 
			P6b = P1 + P3bDirNorm * r1;
			
			float amt = r0 * 3.0f;
			outerTanADir = ( P6a - P5a ) * amt;
			outerTanBDir = ( P6b - P5b ) * amt;
			innerTanADir = ( P6a - P5b ) * amt;
			innerTanBDir = ( P6b - P5a ) * amt;
			
			Vec3f P7a = P6a + outerTanBDir;
			Vec3f P7b = P6b + outerTanADir;
			
			float distOfShadow = max( 1.0f - r0, 0.01f );
			P7a = P6a + ( P7a - P6a ).normalized() * distOfShadow;
			P7b = P6b + ( P7b - P6b ).normalized() * distOfShadow;
			
			glEnable( GL_TEXTURE_2D );
			buildShadowVertexArray( P6a, P6b, P7a, P7b );
			
			float alpha = camAlpha * mDeathPer;
			gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.2f * alpha ) );
			
			glVertexPointer( 3, GL_FLOAT, 0, mShadowVerts );
			glTexCoordPointer( 2, GL_FLOAT, 0, mShadowTexCoords );
			
			glEnableClientState( GL_VERTEX_ARRAY );
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			glDrawArrays( GL_TRIANGLES, 0, 12 ); // dont forget to change the vert count in buildShadowVertexArray VVV
			glDisableClientState( GL_VERTEX_ARRAY );
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );        
		}
		
		/*
		 if( G_DEBUG ){
		 glDisable( GL_TEXTURE_2D );
		 
		 gl::enableAlphaBlending();
		 gl::color( ColorA( mGlowColor, 0.4f ) );
		 gl::drawLine( P0, P1 );
		 
		 glPushMatrix();
		 gl::translate( P0 );
		 gl::rotate( mMatrix );
		 gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
		 gl::drawStrokedCircle( Vec2f::zero(), r0, 50 );
		 glPopMatrix();
		 
		 glPushMatrix();
		 gl::translate( P0 );
		 gl::rotate( mMatrix );
		 gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
		 gl::drawStrokedCircle( Vec2f::zero(), r0Inner, 50 );
		 glPopMatrix();
		 
		 glPushMatrix();
		 gl::translate( P1 );
		 gl::rotate( mMatrix );
		 gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
		 gl::drawStrokedCircle( Vec2f::zero(), r1, 25 );
		 glPopMatrix();
		 
		 glPushMatrix();
		 gl::translate( P2 );
		 gl::rotate( mMatrix );
		 gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
		 gl::drawStrokedCircle( Vec2f::zero(), 0.01f, 16 );
		 glPopMatrix();
		 
		 
		 
		 glPushMatrix();
		 gl::translate( P3a );
		 //gl::rotate( mMatrix );
		 //gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
		 gl::drawStrokedCircle( Vec2f::zero(), 0.01f, 16 );
		 glPopMatrix();
		 
		 glPushMatrix();
		 gl::translate( P3b );
		 //gl::rotate( mMatrix );
		 //gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
		 gl::drawStrokedCircle( Vec2f::zero(), 0.01f, 16 );
		 glPopMatrix();
		 
		 glPushMatrix();
		 gl::translate( P5a );
		 gl::drawStrokedCircle( Vec2f::zero(), 0.01f, 16 );
		 glPopMatrix();
		 
		 glPushMatrix();
		 gl::translate( P5b );
		 gl::drawStrokedCircle( Vec2f::zero(), 0.01f, 16 );
		 glPopMatrix();
		 
		 glPushMatrix();
		 gl::translate( P6a );
		 gl::drawStrokedCircle( Vec2f::zero(), 0.01f, 16 );
		 glPopMatrix();
		 
		 glPushMatrix();
		 gl::translate( P6b );
		 gl::drawStrokedCircle( Vec2f::zero(), 0.01f, 16 );
		 glPopMatrix();
		 
		 
		 gl::drawLine( P6a, ( P6a + mMatrix * outerTanBDir ) );
		 gl::drawLine( P6b, ( P6b + mMatrix * outerTanBDir ) );
		 gl::drawLine( P6a, ( P6a + mMatrix * innerTanBDir ) );
		 gl::drawLine( P6b, ( P6b + mMatrix * innerTanBDir ) );
		 
		 gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.4f ) );	
		 glPushMatrix();
		 gl::translate( P4 );
		 gl::rotate( mMatrix );
		 gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
		 gl::drawStrokedCircle( Vec2f::zero(), dMid, 50 );
		 glPopMatrix();
		 
		 glEnable( GL_TEXTURE_2D );
		 }
		 */
		
		for( vector<Node*>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it ){
			(*it)->findShadows( camAlpha );
		}
	}
}



void NodeAlbum::buildShadowVertexArray( Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4 )
{
    if( mShadowVerts != NULL )		delete[] mShadowVerts;
    if( mShadowTexCoords != NULL )  delete[] mShadowTexCoords;
    
	int numVerts		= 12;			// dont forget to change the vert count in findShadows ^^^
	mShadowVerts		= new float[ numVerts * 3 ]; // x, y
	mShadowTexCoords	= new float[ numVerts * 2 ]; // u, v
	int i = 0;
	int t = 0;
	
	Vec3f v1 = ( p1 + p2 ) * 0.5f;	// midpoint between base vertices
	Vec3f v2 = ( p3 + p4 ) * 0.5f;	// midpoint between end vertices
	
	mShadowVerts[i++]	= p1.x;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p1.y;		mShadowTexCoords[t++]	= 0.2f;
	mShadowVerts[i++]	= p1.z;
	mShadowVerts[i++]	= v2.x;		mShadowTexCoords[t++]	= 0.5f;
	mShadowVerts[i++]	= v2.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= v2.z;
	mShadowVerts[i++]	= p3.x;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p3.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= p3.z;
    
	// umbra 
	mShadowVerts[i++]	= p1.x;		mShadowTexCoords[t++]	= 0.5f;
	mShadowVerts[i++]	= p1.y;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p1.z;
	mShadowVerts[i++]	= v1.x;		mShadowTexCoords[t++]	= 0.75f;
	mShadowVerts[i++]	= v1.y;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= v1.z;
	mShadowVerts[i++]	= v2.x;		mShadowTexCoords[t++]	= 0.75f;
	mShadowVerts[i++]	= v2.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= v2.z;
	
	// umbra 
	mShadowVerts[i++]	= v1.x;		mShadowTexCoords[t++]	= 0.75f;
	mShadowVerts[i++]	= v1.y;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= v1.z;
	mShadowVerts[i++]	= p2.x;		mShadowTexCoords[t++]	= 0.5f;
	mShadowVerts[i++]	= p2.y;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p2.z;
	mShadowVerts[i++]	= v2.x;		mShadowTexCoords[t++]	= 0.75f;
	mShadowVerts[i++]	= v2.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= v2.z;
	
	mShadowVerts[i++]	= p2.x;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p2.y;		mShadowTexCoords[t++]	= 0.2f;
	mShadowVerts[i++]	= p2.z;
	mShadowVerts[i++]	= p4.x;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p4.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= p4.z;
	mShadowVerts[i++]	= v2.x;		mShadowTexCoords[t++]	= 0.5f;
	mShadowVerts[i++]	= v2.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= v2.z;	
	
}

//void NodeAlbum::findShadows( float camAlpha )
//{	
//    mShadow.setup( this, mParentNode, camAlpha );
//	
//	for( vector<Node*>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it ){
//		(*it)->findShadows( camAlpha );
//	}
//}

void NodeAlbum::setChildOrbitRadii()
{
	float orbitRadius = mOrbitRadiusMin;
	float orbitOffset;
	for( vector<Node*>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it ){
		orbitOffset = (*it)->mRadiusInit * 2.0f;
		orbitRadius += orbitOffset;
		(*it)->mOrbitRadiusDest = orbitRadius;
		orbitRadius += orbitOffset;
	}
	
	mIdealCameraDist = orbitRadius * 2.0f;
}

float NodeAlbum::getReleaseYear()
{
	return mReleaseYear;
}

string NodeAlbum::getName()
{
	string name = mAlbum->getAlbumTitle();
	if( name.size() < 1 ) name = "Untitled";
	return name;
}

uint64_t NodeAlbum::getId()
{
    return mId;
}
