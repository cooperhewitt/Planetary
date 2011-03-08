#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/Font.h"
#include "cinder/Arcball.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "Globals.h"
#include "Easing.h"
#include "World.h"
#include "UiLayer.h"
#include "State.h"
#include "Data.h"
#include "PlayControls.h"
#include "Breadcrumbs.h"
#include "BreadcrumbEvent.h"
#include "CinderIPod.h"
#include "CinderIPodPlayer.h"
#include <vector>
#include <sstream>

using std::vector;
using namespace ci;
using namespace ci::app;
using namespace std;
using std::stringstream;

//int G_CURRENT_LEVEL		= 0;
float G_ZOOM			= 0;
bool G_DEBUG			= false;
GLfloat mat_ambient[]	= { 0.02, 0.02, 0.05, 1.0 };
GLfloat mat_diffuse[]	= { 1.0, 1.0, 1.0, 1.0 };

float easeInOutQuad( double t, float b, float c, double d );
Vec3f easeInOutQuad( double t, Vec3f b, Vec3f c, double d );

class KeplerApp : public AppCocoaTouch {
  public:
	virtual void	setup();
	virtual void	touchesBegan( TouchEvent event );
	virtual void	touchesMoved( TouchEvent event );
	virtual void	touchesEnded( TouchEvent event );
	virtual void	accelerated( AccelEvent event );
	void			initTextures();
	void			initFonts();
	virtual void	update();
	void			updateArcball();
	void			updateCamera();
	virtual void	draw();
	void			drawInfoPanel();
	void			setParamsTex();
	bool			onAlphaCharStateChanged( State *state );
	bool			onAlphaCharSelected( UiLayer *uiLayer );
	bool			onBreadcrumbSelected ( BreadcrumbEvent event );
	bool			onPlayControlsButtonPressed ( PlayButton button );
	bool			onNodeSelected( Node *node );
	void			checkForNodeTouch( const Ray &ray, Matrix44f &mat );
	bool			onPlayerStateChanged( ipod::Player *player );
    bool			onPlayerTrackChanged( ipod::Player *player );
	
	World			mWorld;
	State			mState;
	UiLayer			mUiLayer;
	Data			mData;
	
	// ACCELEROMETER
	Matrix44f		mAccelMatrix;
	
	// AUDIO
	ipod::Player	mIpodPlayer;
	
	// BREADCRUMBS
	Breadcrumbs		mBreadcrumbs;	
	
	// PLAY CONTROLS
	PlayControls	mPlayControls;
	
	// CAMERA PERSP
	CameraPersp		mCam;
	float			mFov, mFovDest;
	Vec3f			mEye, mCenter, mUp;
	Vec3f			mCamVel;
	Vec3f			mCenterDest, mCenterFrom;
	float			mCamDist, mCamDistDest, mCamDistFrom;
	float			mZoomFrom, mZoomDest;
	Arcball			mArcball;
	Matrix44f		mMatrix;
	Vec3f			mBbRight, mBbUp;
	
	
	// FONTS
	vector<Font>	mFonts;
	
	
	// MULTITOUCH
	Vec2f			mTouchPos;
	Vec2f			mTouchThrowVel;
	Vec2f			mTouchVel;
	bool			mIsDragging;
	
	
	// TEXTURES
	gl::Texture		mParamsTex;
	gl::Texture		mAtmosphereTex;
	gl::Texture		mStarTex;
	gl::Texture		mStarGlowTex;
	gl::Texture		mSkyDome;
	gl::Texture		mDottedTex;
	gl::Texture		mPanelTabTex, mPanelTabDownTex;
	vector<gl::Texture*>	mPlanetsTex;
	
	float			mTime;
};

void KeplerApp::setup()
{
	Rand::randomize();
	
	// INIT ACCELEROMETER
	enableAccelerometer();
	mAccelMatrix		= Matrix44f();
	
	// ARCBALL
	mMatrix	= Quatf();
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( getWindowCenter() );
	mArcball.setRadius( 420 );
	
	
	// CAMERA PERSP
	mCamDist			= G_INIT_CAM_DIST;
	mCamDistDest		= mCamDist;
	mCamDistFrom		= mCamDist;
	mEye				= Vec3f( 0.0f, 0.0f, mCamDist );
	mCenter				= Vec3f::zero();
	mCenterDest			= mCenter;
	mCenterFrom			= mCenter;
	mUp					= Vec3f::yAxis();
	mFov				= 90.0f;
	mFovDest			= 90.0f;
	mCam.setPerspective( mFov, getWindowAspectRatio(), 0.001f, 4000.0f );
	mBbRight			= Vec3f::xAxis();
	mBbUp				= Vec3f::yAxis();
	
	
	// FONTS
	initFonts();
	
	
	// PLAYER
	mIpodPlayer.registerStateChanged( this, &KeplerApp::onPlayerStateChanged );
    mIpodPlayer.registerTrackChanged( this, &KeplerApp::onPlayerTrackChanged );
	
	
	// TOUCH VARS
	mTouchPos			= Vec2f::zero();
	mTouchThrowVel		= Vec2f::zero();
	mTouchVel			= Vec2f::zero();
	mIsDragging			= false;
	mTime				= getElapsedSeconds();
	
	
	// TEXTURES
	initTextures();
	
	// BREADCRUMBS
	mBreadcrumbs.setup( this, mFonts[3] );
	mBreadcrumbs.registerBreadcrumbSelected( this, &KeplerApp::onBreadcrumbSelected );
	mBreadcrumbs.setHierarchy(mState.getHierarchy());
	
	// PLAY CONTROLS
	mPlayControls.setup( this );
	mPlayControls.registerButtonPressed( this, &KeplerApp::onPlayControlsButtonPressed );
	
	// STATE
	mState.registerAlphaCharStateChanged( this, &KeplerApp::onAlphaCharStateChanged );
	mState.registerNodeSelected( this, &KeplerApp::onNodeSelected );
	
	
	// UILAYER
	mUiLayer.setup( this );
	mUiLayer.registerAlphaCharSelected( this, &KeplerApp::onAlphaCharSelected );
	mUiLayer.initAlphaTextures( mFonts[0] );
	
	
	// DATA
	mData.initArtists();
	

	// WORLD
	mWorld.setData( &mData );
	mWorld.initNodes( &mIpodPlayer, mFonts[4] );
}

void KeplerApp::touchesBegan( TouchEvent event )
{	
	mIsDragging = false;
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) 
	{
		mTouchPos		= touchIt->getPos();
		mTouchThrowVel	= Vec2f::zero();
		mIsDragging		= false;
		if( event.getTouches().size() == 1 )
			mArcball.mouseDown( Vec2f( mTouchPos.x, mTouchPos.y ) );
	}
}

void KeplerApp::touchesMoved( TouchEvent event )
{
	mIsDragging = true;
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt )
	{
		mTouchThrowVel	= ( touchIt->getPos() - mTouchPos );
		mTouchVel		= mTouchThrowVel;
		mTouchPos		= touchIt->getPos();
		if( event.getTouches().size() == 1 )
			mArcball.mouseDrag( Vec2f( mTouchPos.x, mTouchPos.y ) );
	}
}

void KeplerApp::touchesEnded( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt )
	{
		mTouchPos = touchIt->getPos();
		if( ! mUiLayer.getShowWheel() && ! mIsDragging ){
			float u			= mTouchPos.x / (float) getWindowWidth();
			float v			= mTouchPos.y / (float) getWindowHeight();
			Ray touchRay	= mCam.generateRay( u, 1.0f - v, mCam.getAspectRatio() );
			checkForNodeTouch( touchRay, mMatrix );
		}
		mIsDragging = false;
	}
}

void KeplerApp::accelerated( AccelEvent event )
{
	mAccelMatrix = event.getMatrix();
	mAccelMatrix.invert();
}

void KeplerApp::initTextures()
{
	mPanelTabTex		= loadImage( loadResource( "panelTab.png" ) );
	mPanelTabDownTex	= loadImage( loadResource( "panelTabDown.png" ) );
	mAtmosphereTex		= loadImage( loadResource( "atmosphere.png" ) );
	mStarTex			= loadImage( loadResource( "star.png" ) );
	mStarGlowTex		= loadImage( loadResource( "starGlow.png" ) );
	mSkyDome			= loadImage( loadResource( "skydome.jpg" ) );
	mDottedTex			= loadImage( loadResource( "dotted.png" ) );
	mDottedTex.setWrap( GL_REPEAT, GL_REPEAT );
	mParamsTex			= gl::Texture( 768, 75 );
	mPlanetsTex.push_back( new gl::Texture( loadImage( loadResource( "pluto.jpg" ) ) ) );
	mPlanetsTex.push_back( new gl::Texture( loadImage( loadResource( "venus.jpg" ) ) ) );
	mPlanetsTex.push_back( new gl::Texture( loadImage( loadResource( "mars.jpg" ) ) ) );
	mPlanetsTex.push_back( new gl::Texture( loadImage( loadResource( "uranus.jpg" ) ) ) );
	mPlanetsTex.push_back( new gl::Texture( loadImage( loadResource( "jupiter.jpg" ) ) ) );
	mPlanetsTex.push_back( new gl::Texture( loadImage( loadResource( "saturn.jpg" ) ) ) );
	mPlanetsTex.push_back( new gl::Texture( loadImage( loadResource( "clouds.png" ) ) ) );
	mPlanetsTex.push_back( new gl::Texture( loadImage( loadResource( "rings.png" ) ) ) );
}

void KeplerApp::initFonts()
{
	mFonts.push_back( Font( loadResource( "UnitRoundedOT-Ultra.otf" ), 256 ) );
	mFonts.push_back( Font( loadResource( "UnitRoundedOT-Ultra.otf" ), 64 ) );
	mFonts.push_back( Font( loadResource( "UnitRoundedOT-Ultra.otf" ), 28 ) );
	mFonts.push_back( Font( loadResource( "UnitRoundedOT-Ultra.otf" ), 16 ) );
	mFonts.push_back( Font( loadResource( "UnitRoundedOT-Ultra.otf" ), 16 ) );
}

bool KeplerApp::onAlphaCharSelected( UiLayer *uiLayer )
{
	mState.setAlphaChar( uiLayer->getAlphaChar() );
	return false;
}

bool KeplerApp::onAlphaCharStateChanged( State *state )
{
	mData.filterArtistsByAlpha( mState.getAlphaChar() );
	mWorld.filterNodes();
	mBreadcrumbs.setHierarchy( mState.getHierarchy() );	
	return false;
}

bool KeplerApp::onNodeSelected( Node *node )
{
	mTime			= getElapsedSeconds();
	mCenterFrom		= mCenter;
	mCamDistFrom	= mCamDist;	
	mZoomFrom		= G_ZOOM;			
	mBreadcrumbs.setHierarchy( mState.getHierarchy() );	
	return false;
}

bool KeplerApp::onPlayControlsButtonPressed( PlayButton button )
{
	cout << "play button " << button << " pressed" << endl;
	return false;
}

bool KeplerApp::onBreadcrumbSelected( BreadcrumbEvent event )
{
	int level = event.getLevel();
	if( level == G_HOME_LEVEL ){				// BACK TO HOME
		mUiLayer.setShowWheel( !mUiLayer.getShowWheel() );
		mWorld.deselectAllNodes();
		mState.setSelectedNode( NULL );
		mState.setAlphaChar( ' ' );
	}
	else if( level == G_ALPHA_LEVEL ){			// BACK TO ALPHA FILTER
		mWorld.deselectAllNodes();
		mData.filterArtistsByAlpha( mState.getAlphaChar() );
		mWorld.filterNodes();
		mState.setSelectedNode(NULL);
	}
	else if( level >= G_ARTIST_LEVEL ){			// BACK TO ARTIST/ALBUM/TRACK
		// get Artist, Album or Track from selectedNode
		Node *current = mState.getSelectedNode();
		while (current != NULL && current->mGen > level) {
			current = current->mParentNode;
		}
		mState.setSelectedNode(current);
	}
	return false;
}

void KeplerApp::checkForNodeTouch( const Ray &ray, Matrix44f &mat )
{
	Node *touchedNode = NULL;
	mWorld.checkForSphereIntersect( touchedNode, ray, mat );
	
	if( touchedNode )
		mState.setSelectedNode(touchedNode);
}

void KeplerApp::update()
{
	updateArcball();
	mWorld.update( mMatrix, mBbRight, mBbUp );
	updateCamera();
	mWorld.updateGraphics( mCam );
	
	mBreadcrumbs.update();
	mPlayControls.update();
}

void KeplerApp::updateArcball()
{
	if( mTouchThrowVel.length() > 10.0f && !mIsDragging ){
		//if( mTouchVel.length() > 1.0f ){
			//mTouchVel *= 0.99f;
			mArcball.mouseDown( mTouchPos );
			mArcball.mouseDrag( mTouchPos + mTouchVel );
		//}
	}
	
	mMatrix = mArcball.getQuat();
}


void KeplerApp::updateCamera()
{
	Node* selectedNode = mState.getSelectedNode();
	if( selectedNode ){
		mCamDistDest	= selectedNode->mIdealCameraDist;
		mCenterDest		= mMatrix.transformPointAffine( selectedNode->mPos );
		mZoomDest		= selectedNode->mGen;
		
		if( selectedNode->mParentNode )
			mCenterFrom		+= selectedNode->mParentNode->mVel;

	} else {
		mCamDistDest	= G_INIT_CAM_DIST;
		mCenterDest		= mMatrix.transformPointAffine( Vec3f::zero() );

		mZoomDest		= G_HOME_LEVEL;
		if( mState.getAlphaChar() != ' ' ){
			mZoomDest	= G_ALPHA_LEVEL;
		}
	}
	
	
	
	// UPDATE FOV
	if( mUiLayer.getShowWheel() ){
		mFovDest = 120.0f;
	} else {
		mFovDest = 100.0f - G_ZOOM * 5.0f;
	}
	mFov -= ( mFov - mFovDest ) * 0.2f;
	
	double p	= constrain( getElapsedSeconds()-mTime, 0.0, G_DURATION );
	mCenter		= easeInOutQuad( p, mCenterFrom, mCenterDest - mCenterFrom, G_DURATION );
	mCamDist	= easeInOutQuad( p, mCamDistFrom, mCamDistDest - mCamDistFrom, G_DURATION );
	G_ZOOM		= easeInOutQuad( p, mZoomFrom, mZoomDest - mZoomFrom, G_DURATION );
	
	Vec3f prevEye		= mEye;
	mEye				= Vec3f( mCenter.x, mCenter.y, mCenter.z - mCamDist );
	mCamVel				= mEye - prevEye;
	
	mCam.setPerspective( mFov, getWindowAspectRatio(), 0.0001f, 4000.0f );
	mCam.lookAt( mEye, mCenter, mUp );
	mCam.getBillboardVectors( &mBbRight, &mBbUp );
}

void KeplerApp::draw()
{
	gl::clear( Color( 0, 0, 0 ), true );
	gl::enableDepthWrite();
	gl::setMatrices( mCam );
	
	// DRAW SKYDOME
	gl::pushModelView();
	gl::rotate( mArcball.getQuat() );
	gl::color( Color( 1.0f, 1.0f, 1.0f ) );
	mSkyDome.enableAndBind();
	gl::drawSphere( Vec3f::zero(), 2000.0f, 64 );
	gl::popModelView();
	
	
	gl::enableAdditiveBlending();
	
	// STARGLOWS
	mStarGlowTex.enableAndBind();
	mWorld.drawStarGlows();
	mStarGlowTex.disable();
	
// STARS
	mStarTex.enableAndBind();
	mWorld.drawStars();
	mStarTex.disable();
	
// CONSTELLATION
	mDottedTex.enableAndBind();
	mWorld.drawConstellation( mMatrix );
	mDottedTex.disable();
			
	if( G_DEBUG ){
		// VISUALIZE THE HIT AREA
		glDisable( GL_TEXTURE_2D );
	//	mWorld.drawSpheres();
	}
	
	
	// PLANETS
	Node *albumNode  = mState.getSelectedAlbumNode();
	Node *artistNode = mState.getSelectedArtistNode();
	if( artistNode ){
		glEnable( GL_LIGHTING );
		glEnable ( GL_COLOR_MATERIAL ) ;
		glShadeModel(GL_SMOOTH);
					
		glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient );
		glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse );
		//glMaterialfv( GL_FRONT, GL_EMISSION, mat_emission );
		
		
		if( albumNode ){
			// LIGHT FROM ALBUM
			glEnable( GL_LIGHT0 );
			Vec3f albumLightPos		= albumNode->mTransPos;
			GLfloat albumLight[]	= { albumLightPos.x, albumLightPos.y, albumLightPos.z, 1.0f };
			Color albumDiffuse		= albumNode->mGlowColor;
			glLightfv( GL_LIGHT0, GL_POSITION, albumLight );
			glLightfv( GL_LIGHT0, GL_DIFFUSE, albumDiffuse );
		}
		
		// LIGHT FROM ARTIST
		glEnable( GL_LIGHT1 );
		Vec3f artistLightPos	= artistNode->mTransPos;
		GLfloat artistLight[]	= { artistLightPos.x, artistLightPos.y, artistLightPos.z, 1.0f };
		Color artistDiffuse		= artistNode->mGlowColor;
		glLightfv( GL_LIGHT1, GL_POSITION, artistLight );
		glLightfv( GL_LIGHT1, GL_DIFFUSE, artistDiffuse );
		
// PLANETS
		gl::enableDepthRead();
		gl::disableAlphaBlending();
		mWorld.drawPlanets( mAccelMatrix, mPlanetsTex );
		
// RINGS
		gl::enableAdditiveBlending();
		mWorld.drawRings( mPlanetsTex[G_RING_TYPE] );
		glDisable( GL_LIGHTING );
	}
	
	
// ATMOSPHERE
	mAtmosphereTex.enableAndBind();
	mWorld.drawAtmospheres();
	mAtmosphereTex.disable();
	
	
// ORBITS
	gl::enableAdditiveBlending();
	gl::enableDepthRead();
	mWorld.drawOrbitalRings();
	gl::disableDepthRead();

// NAMES
	gl::disableDepthWrite();
	glEnable( GL_TEXTURE_2D );
	gl::setMatricesWindow( getWindowSize() );
	gl::enableAdditiveBlending();
	mWorld.drawOrthoNames( mCam );
	
	
	
	
	glDisable( GL_TEXTURE_2D );
	gl::disableAlphaBlending();
	gl::enableAlphaBlending();
	mUiLayer.draw( mPanelTabTex, mPanelTabDownTex );
	mBreadcrumbs.draw( mUiLayer.getPanelYPos() + 5.0f );
	mPlayControls.draw( mUiLayer.getPanelYPos() + mBreadcrumbs.getHeight() + 5.0f );
	mState.draw( mFonts[4] );
	
	//drawInfoPanel();
}



void KeplerApp::drawInfoPanel()
{
	gl::setMatricesWindow( getWindowSize() );
	if( getElapsedFrames() % 30 == 0 ){
		setParamsTex();
	}
	gl::color( Color( 1.0f, 1.0f, 1.0f ) );
	gl::draw( mParamsTex, Vec2f( 0.0f, 0.0f ) );
}


void KeplerApp::setParamsTex()
{
	TextLayout layout;	
	layout.setFont( mFonts[4] );
	layout.setColor( Color( 0.3f, 0.3f, 1.0f ) );

	int currentLevel = G_HOME_LEVEL;
	if (mState.getSelectedNode()) {
		currentLevel = mState.getSelectedNode()->mGen;
	}
	else if (mState.getAlphaChar() != ' ') {
		currentLevel = G_ALPHA_LEVEL;
	}
	
	stringstream s;
	s.str("");
	s << " CURRENT LEVEL: " << currentLevel;
	layout.addLine( s.str() );
	
	s.str("");
	s << " FPS: " << getAverageFps();
	layout.addLine( s.str() );
	
	s.str("");
	s << " ZOOM LEVEL: " << G_ZOOM;
	layout.setColor( Color( 0.0f, 1.0f, 1.0f ) );
	layout.addLine( s.str() );
	
	mParamsTex = gl::Texture( layout.render( true, false ) );
}


bool KeplerApp::onPlayerTrackChanged( ipod::Player *player )
{	
    console() << "Now Playing: " << player->getPlayingTrack()->getTitle() << endl;
	
    return false;
}

bool KeplerApp::onPlayerStateChanged( ipod::Player *player )
{
    switch( player->getPlayState() ){
        case ipod::Player::StatePlaying:
            console() << "Playing..." << endl;
            break;
        case ipod::Player::StateStopped:
            console() << "Stopped." << endl;
            break;
    }
    return false;
}


CINDER_APP_COCOA_TOUCH( KeplerApp, RendererGl )
