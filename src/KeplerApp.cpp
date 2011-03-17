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
#include "AlphaWheel.h"
#include "State.h"
#include "Data.h"
#include "PlayControls.h"
#include "Breadcrumbs.h"
#include "CinderIPod.h"
#include "CinderIPodPlayer.h"
#include "PinchRecognizer.h"
#include "ParticleController.h"
#include <vector>
#include <sstream>

using std::vector;
using namespace ci;
using namespace ci::app;
using namespace std;
using std::stringstream;

float G_ZOOM			= 0;
bool G_DEBUG			= false;
bool G_ACCEL            = false;
bool G_IS_IPAD2			= true;
int G_NUM_PARTICLES		= 250;

GLfloat mat_diffuse[]	= { 1.0, 1.0, 1.0, 1.0 };

float easeInOutQuad( double t, float b, float c, double d );
Vec3f easeInOutQuad( double t, Vec3f b, Vec3f c, double d );

class KeplerApp : public AppCocoaTouch {
  public:
	void			init();
	virtual void	setup();
	void			initTextures();
	virtual void	touchesBegan( TouchEvent event );
	virtual void	touchesMoved( TouchEvent event );
	virtual void	touchesEnded( TouchEvent event );
	virtual void	accelerated( AccelEvent event );
	virtual void	update();
	void			updateArcball();
	void			updateCamera();
	void			updatePlayhead();
	virtual void	draw();
	void			drawLoader();
    void            drawScene();
	void			drawInfoPanel();
	void			setParamsTex();
	bool			onAlphaCharStateChanged( State *state );
	bool			onAlphaCharSelected( AlphaWheel *alphaWheel );
	bool			onWheelClosed( AlphaWheel *alphaWheel );
	bool			onBreadcrumbSelected ( BreadcrumbEvent event );
	bool			onPlayControlsButtonPressed ( PlayControls::PlayButton button );
	bool			onNodeSelected( Node *node );
	void			checkForNodeTouch( const Ray &ray, Matrix44f &mat, const Vec2f &pos );
	bool			onPlayerStateChanged( ipod::Player *player );
    bool			onPlayerTrackChanged( ipod::Player *player );
	
	World			mWorld;
	State			mState;
	AlphaWheel		mAlphaWheel;
	UiLayer			mUiLayer;
	Data			mData;
	
	// ACCELEROMETER
	Matrix44f		mAccelMatrix;
	Matrix44f		mNewAccelMatrix;
	
	// AUDIO
	ipod::Player		mIpodPlayer;
	ipod::PlaylistRef	mCurrentAlbum;
	double				mCurrentTrackPlayheadTime;
	double				mCurrentTrackLength;
//	int					mCurrentTrackId;

	
	// BREADCRUMBS
	Breadcrumbs		mBreadcrumbs;	
	
	// PLAY CONTROLS
	PlayControls	mPlayControls;
	float			mPlayheadPer;	// song percent complete 
	
	// CAMERA PERSP
	CameraPersp		mCam;
	float			mFov, mFovDest;
	Vec3f			mEye, mCenter, mUp;
	Vec3f			mCamVel;
	Vec3f			mCenterDest, mCenterFrom;
	float			mCamDist, mCamDistDest, mCamDistFrom;
	float			mCamDistPinchOffset, mCamDistPinchOffsetDest;
	float			mZoomFrom, mZoomDest;
	Arcball			mArcball;
	Matrix44f		mMatrix;
	Vec3f			mBbRight, mBbUp;
	
	
	// FONTS
	Font			mFont;
	Font			mFontBig;
	Font			mFontSmall;
	
	
	// MULTITOUCH
	Vec2f			mTouchPos;
	Vec2f			mTouchVel;
	bool			mIsDragging;
	bool			onPinchBegan( PinchEvent event );
    bool			onPinchMoved( PinchEvent event );
    bool			onPinchEnded( PinchEvent event );
    vector<Ray>		mPinchRays;
    PinchRecognizer mPinchRecognizer;
	float			mPinchScale;
	float			mTimePinchEnded;
	
	// PARTICLES
    ParticleController mParticleController;
	
	// TEXTURES
	gl::Texture		mLoadingTex;
	gl::Texture		mParamsTex;
	gl::Texture		mAtmosphereTex;
	gl::Texture		mStarTex, mStarAlternateTex;
	gl::Texture		mStarGlowTex;
	gl::Texture		mSkyDome;
	gl::Texture		mDottedTex;
	gl::Texture		mPanelUpTex, mPanelDownTex;
	gl::Texture		mSliderBgTex;
	gl::Texture		mPlayTex, mPauseTex, mForwardTex, mBackwardTex, mDebugTex, mDebugOnTex, mHighlightTex;
	vector<gl::Texture> mButtonsTex;
	vector<gl::Texture> mPanelButtonsTex;
	vector<gl::Texture> mPlanetsTex;
    gl::Texture     mRingsTex;
	vector<gl::Texture> mCloudsTex;
	
	float			mTime;
	
	bool			mIsLoaded;
};

void KeplerApp::setup()
{
	if( G_IS_IPAD2 ){
		G_NUM_PARTICLES = 500;
	}
	
	
	mIsLoaded = false;
	
	Rand::randomize();

	
    // TEXTURES
	initTextures();
    
    
	// INIT ACCELEROMETER
	enableAccelerometer();
	mAccelMatrix		= Matrix44f();
	
	// ARCBALL
	mMatrix	= Quatf();
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( getWindowCenter() );
	mArcball.setRadius( 420 );
	
	// AUDIO
//	mCurrentTrackId         = 1;
//	mCurrentTrackPlayheadTime = 0;
	// TODO: what about? something like this:
	//mCurrentTrack = mIpodPlayer.getCurrentTrack();
	
	// CAMERA PERSP
	mCamDist			= G_INIT_CAM_DIST;
	mCamDistDest		= mCamDist;
	mCamDistPinchOffset	= 1.0f;
	mCamDistPinchOffsetDest = 1.0f;
	mCamDistFrom		= mCamDist;
	mEye				= Vec3f( 0.0f, 0.0f, mCamDist );
	mCenter				= Vec3f::zero();
	mCenterDest			= mCenter;
	mCenterFrom			= mCenter;
	mUp					= Vec3f::yAxis();
	mFov				= 80.0f;
	mFovDest			= 80.0f;
	mCam.setPerspective( mFov, getWindowAspectRatio(), 0.001f, 1000.0f );
	mBbRight			= Vec3f::xAxis();
	mBbUp				= Vec3f::yAxis();
	
	// FONTS
	mFont				= Font( loadResource( "UnitRoundedOT-Medi.otf" ), 13 );
	mFontBig			= Font( loadResource( "UnitRoundedOT-Ultra.otf" ), 256 );
	mFontSmall			= Font( loadResource( "UnitRoundedOT-Ultra.otf" ), 13 );	

	// TOUCH VARS
	mTouchPos			= Vec2f::zero();
	mTouchVel			= Vec2f::zero();
	mIsDragging			= false;
	mTime				= getElapsedSeconds();
	mTimePinchEnded		= 0.0f;
	mPinchRecognizer.init(this);
    mPinchRecognizer.registerBegan( this, &KeplerApp::onPinchBegan );
    mPinchRecognizer.registerMoved( this, &KeplerApp::onPinchMoved );
    mPinchRecognizer.registerEnded( this, &KeplerApp::onPinchEnded );
    
    // PARTICLES
    mParticleController.addParticles( G_NUM_PARTICLES );
	
	// BREADCRUMBS
	mBreadcrumbs.setup( this, mFontSmall );
	mBreadcrumbs.registerBreadcrumbSelected( this, &KeplerApp::onBreadcrumbSelected );
	mBreadcrumbs.setHierarchy(mState.getHierarchy());

	// ALPHA WHEEL
	mAlphaWheel.setup( this );
	mAlphaWheel.registerAlphaCharSelected( this, &KeplerApp::onAlphaCharSelected );
	mAlphaWheel.registerWheelClosed( this, &KeplerApp::onWheelClosed );
	mAlphaWheel.initAlphaTextures( mFontBig );	
	
	// PLAY CONTROLS
	mPlayControls.setup( this, mIpodPlayer.getPlayState() == ipod::Player::StatePlaying );
	mPlayControls.registerButtonPressed( this, &KeplerApp::onPlayControlsButtonPressed );

	// UILAYER
	mUiLayer.setup( this );
		
	// STATE
	mState.registerAlphaCharStateChanged( this, &KeplerApp::onAlphaCharStateChanged );
	mState.registerNodeSelected( this, &KeplerApp::onNodeSelected );
		
	// PLAYER
	mIpodPlayer.registerStateChanged( this, &KeplerApp::onPlayerStateChanged );
    mIpodPlayer.registerTrackChanged( this, &KeplerApp::onPlayerTrackChanged );
}


void KeplerApp::initTextures()
{
	/* THIS DIDNT SEEM TO WORK (OR MAYBE IT JUST DIDNT MAKE MUCH OF A DIFFERENCE)
    gl::Texture::Format format;
	format.enableMipmapping( true );			
	ImageSourceRef img = loadImage( loadResource( "star.png" ) );
	if(img) mStarTex = gl::Texture( img, format );
    */
	
	float t = getElapsedSeconds();
	cout << "before load time = " << t << endl;
	mLoadingTex			= loadImage( loadResource( "loading.jpg" ) );
	mPanelUpTex			= loadImage( loadResource( "panelUp.png" ) );
	mPanelDownTex		= loadImage( loadResource( "panelDown.png" ) );
	mSliderBgTex		= loadImage( loadResource( "sliderBg.png" ) );
	mAtmosphereTex		= loadImage( loadResource( "atmosphere.png" ) );
	mStarTex			= loadImage( loadResource( "star.png" ) );
	mStarAlternateTex	= loadImage( loadResource( "starAlternate.png" ) );
	mStarGlowTex		= loadImage( loadResource( "starGlow.png" ) );
	mSkyDome			= loadImage( loadResource( "skydome.jpg" ) );
	mDottedTex			= loadImage( loadResource( "dotted.png" ) );
	mDottedTex.setWrap( GL_REPEAT, GL_REPEAT );
	mParamsTex			= gl::Texture( 768, 75 );
	mButtonsTex.push_back( gl::Texture( loadImage( loadResource( "play.png" ) ) ) );
	mButtonsTex.push_back( gl::Texture( loadImage( loadResource( "playOn.png" ) ) ) );
	mButtonsTex.push_back( gl::Texture( loadImage( loadResource( "pause.png" ) ) ) );
	mButtonsTex.push_back( gl::Texture( loadImage( loadResource( "pauseOn.png" ) ) ) );
	mButtonsTex.push_back( gl::Texture( loadImage( loadResource( "prev.png" ) ) ) );
	mButtonsTex.push_back( gl::Texture( loadImage( loadResource( "prevOn.png" ) ) ) );
	mButtonsTex.push_back( gl::Texture( loadImage( loadResource( "next.png" ) ) ) );
	mButtonsTex.push_back( gl::Texture( loadImage( loadResource( "nextOn.png" ) ) ) );
	mButtonsTex.push_back( gl::Texture( loadImage( loadResource( "accelOff.png" ) ) ) );
	mButtonsTex.push_back( gl::Texture( loadImage( loadResource( "accelOn.png" ) ) ) );
	mButtonsTex.push_back( gl::Texture( loadImage( loadResource( "debugOff.png" ) ) ) );
	mButtonsTex.push_back( gl::Texture( loadImage( loadResource( "debugOn.png" ) ) ) );
	mPanelButtonsTex.push_back( gl::Texture( loadImage( loadResource( "panelUp.png" ) ) ) );
	mPanelButtonsTex.push_back( gl::Texture( loadImage( loadResource( "panelUpOn.png" ) ) ) );
	mPanelButtonsTex.push_back( gl::Texture( loadImage( loadResource( "panelDown.png" ) ) ) );
	mPanelButtonsTex.push_back( gl::Texture( loadImage( loadResource( "panelDownOn.png" ) ) ) );
    mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "11.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "12.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "13.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "21.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "22.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "23.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "31.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "32.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "33.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "41.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "42.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "43.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "51.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "52.jpg" ) ) ) );
	mPlanetsTex.push_back( gl::Texture( loadImage( loadResource( "53.jpg" ) ) ) );
	mRingsTex           = loadImage( loadResource( "rings.png" ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "clouds1.png" ) ) ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "clouds2.png" ) ) ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "clouds3.png" ) ) ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "clouds4.png" ) ) ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "clouds5.png" ) ) ) );
	t = getElapsedSeconds();
	cout << "after load time = " << t << endl;
	
}


void KeplerApp::init()
{
	static bool inited = false;
	
	if (!inited) {
		// DATA
		mData.initArtists();
	
		// WORLD
		mWorld.setData( &mData );
		//mWorld.initNodes( &mIpodPlayer, mFont );
		
		inited = true;
	}
}

void KeplerApp::touchesBegan( TouchEvent event )
{	
	mIsDragging = false;	
	const vector<TouchEvent::Touch> touches = getActiveTouches();
	float timeSincePinchEnded = getElapsedSeconds() - mTimePinchEnded;
	if( touches.size() == 1 && timeSincePinchEnded > 0.2f ) {
		mTouchPos		= touches.begin()->getPos();
		mTouchVel		= Vec2f::zero();
		mArcball.mouseDown( Vec2f( mTouchPos.x, mTouchPos.y ) );
	}
}

void KeplerApp::touchesMoved( TouchEvent event )
{
	float timeSincePinchEnded = getElapsedSeconds() - mTimePinchEnded;	
	const vector<TouchEvent::Touch> touches = getActiveTouches();
	if( touches.size() == 1 && timeSincePinchEnded > 0.2f ){
		mIsDragging = true;
		Vec2f currentPos = touches.begin()->getPos();
		mTouchVel		= ( currentPos - touches.begin()->getPrevPos() );
		mTouchPos		= currentPos;
		mArcball.mouseDrag( Vec2f( mTouchPos.x, mTouchPos.y ) );
	}
}

void KeplerApp::touchesEnded( TouchEvent event )
{
	float timeSincePinchEnded = getElapsedSeconds() - mTimePinchEnded;	
	const vector<TouchEvent::Touch> touches = event.getTouches();
	if( touches.size() == 1 && timeSincePinchEnded > 0.2f ){
		mTouchPos = touches.begin()->getPos();		
		// if the nav wheel isnt showing and you havent been dragging and your touch is above the uiLayer panel
		if( ! mAlphaWheel.getShowWheel() && ! mIsDragging && mTouchPos.y < mUiLayer.getPanelYPos() ){
			float u			= mTouchPos.x / (float) getWindowWidth();
			float v			= mTouchPos.y / (float) getWindowHeight();
			Ray touchRay	= mCam.generateRay( u, 1.0f - v, mCam.getAspectRatio() );
			checkForNodeTouch( touchRay, mMatrix, mTouchPos );
		}
	}
	if (getActiveTouches().size() != 1) {
		mIsDragging = false;
	}
}

bool KeplerApp::onPinchBegan( PinchEvent event )
{
	mPinchScale = 1.0f;
    mPinchRays = event.getTouchRays( mCam );
	
	mTouchVel	= Vec2f::zero();
	vector<PinchEvent::Touch> touches = event.getTouches();
	Vec2f averageTouchPos;
	for( vector<PinchEvent::Touch>::iterator it = touches.begin(); it != touches.end(); ++it ){
		averageTouchPos += it->mPos;
	}
	averageTouchPos /= touches.size();
	mTouchPos = averageTouchPos;
	
    return false;
}

bool KeplerApp::onPinchMoved( PinchEvent event )
{	
    mPinchRays = event.getTouchRays( mCam );
	
	if( G_ZOOM < G_ARTIST_LEVEL ){
		mFovDest += ( 1.0f - event.getScaleDelta() ) * 100.0f;
		
	} else {
		mCamDistPinchOffsetDest *= ( event.getScaleDelta() - 1.0f ) * -3.5f + 1.0f;
		mCamDistPinchOffsetDest = constrain( mCamDistPinchOffsetDest, 0.35f, 4.5f );
		
		
		// if the pinch will trigger a level change, mess with the FOV to signal the impending change.
		if( mCamDistPinchOffsetDest > 4.1f ){
			mFovDest = 130.0f;//( 1.0f - event.getScaleDelta() ) * 20.0f;
		} else {
			mFovDest = 90.0f;
		}
	}
	
	vector<PinchEvent::Touch> touches = event.getTouches();
	Vec2f averageTouchPos;
	for( vector<PinchEvent::Touch>::iterator it = touches.begin(); it != touches.end(); ++it ){
		averageTouchPos += it->mPos;
	}
	averageTouchPos /= touches.size();
	
	//mTouchThrowVel	= ( averageTouchPos - mTouchPos );
	//mTouchVel		= mTouchThrowVel;
	mTouchPos		= averageTouchPos;
	
	mPinchScale		= event.getScale();
	//cout << "pinch scale: " << mPinchScale << endl;
    return false;
}

bool KeplerApp::onPinchEnded( PinchEvent event )
{
	std::cout << "mCamDistPinchOffset = " << mCamDistPinchOffset << std::endl;
	if( mCamDistPinchOffset > 4.1f ){
		Node *selected = mState.getSelectedNode();
		if( selected ){
			mState.setSelectedNode( selected->mParentNode );
			mFovDest = 90.0f;
			mCamDistPinchOffsetDest = 1.0f;
		}
	}
	mTimePinchEnded = getElapsedSeconds();
	mAlphaWheel.setTimePinchEnded( mTimePinchEnded );	
    mPinchRays.clear();
    return false;
}

void KeplerApp::accelerated( AccelEvent event )
{
	mNewAccelMatrix = event.getMatrix();
	mNewAccelMatrix.invert();
}

bool KeplerApp::onWheelClosed( AlphaWheel *alphaWheel )
{
	std::cout << "wheel closed" << std::endl;
	mFovDest = 100.0f;
	return false;
}

bool KeplerApp::onAlphaCharSelected( AlphaWheel *alphaWheel )
{
	mState.setAlphaChar( alphaWheel->getAlphaChar() );
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
	cout << "node selected!" << endl;
	
	mTime			= getElapsedSeconds();
	mCenterFrom		= mCenter;
	mCamDistFrom	= mCamDist;	
	mZoomFrom		= G_ZOOM;			
	mBreadcrumbs.setHierarchy( mState.getHierarchy() );
	
	if( node && node->mGen > G_ZOOM ){
		node->wasTapped();
	}

	if( node != NULL && node->mGen == G_TRACK_LEVEL ){
		cout << "node selected!" << endl;
		// FIXME: is this a bad OOP thing or is there a cleaner/safer C++ way to handle it?
		NodeTrack* trackNode = (NodeTrack*)node;
		if ( mIpodPlayer.getPlayState() == ipod::Player::StatePlaying ){
			cout << "nothing already playing" << endl;
			ipod::TrackRef playingTrack = mIpodPlayer.getPlayingTrack();
			if ( trackNode->mTrack->getItemId() != playingTrack->getItemId() ) {
				cout << "telling player to play it" << endl;
				mIpodPlayer.play( trackNode->mAlbum, trackNode->mIndex );
			}
			else {
				cout << "already playing it" << endl;				
			}
		}
		else {
			cout << "telling player to play it" << endl;
			mIpodPlayer.play( trackNode->mAlbum, trackNode->mIndex );			
		}
	}
	else {
		cout << "node null or not track level!" << endl;
	}
	
//	if( node ){
//		if( node->mGen == G_ALBUM_LEVEL ){
//			std::cout << "setting currentAlbum = " << node->mAlbum->getAlbumTitle() << std::endl;
//			mCurrentAlbum = node->mAlbum;
//		} else if( node->mGen == G_TRACK_LEVEL ){
//			std::cout << "setting currentTrackId = " << node->mIndex << std::endl;
//			mCurrentTrackId = node->mIndex;
//		}
//	}
	
	return false;
}

bool KeplerApp::onPlayControlsButtonPressed( PlayControls::PlayButton button )
{
	if( button == PlayControls::PREVIOUS_TRACK ){
		mIpodPlayer.skipPrev();
	} else if( button == PlayControls::PLAY_PAUSE ){
		cout << "play/pause pressed" << endl;
		if (mIpodPlayer.getPlayState() == ipod::Player::StatePlaying) {
			cout << "already playing, so asking for pause" << endl;
			mIpodPlayer.pause();
		}
		else {
			cout << "not already playing, so asking for play" << endl;
			mIpodPlayer.play();
		}
	} else if( button == PlayControls::NEXT_TRACK ){
		mIpodPlayer.skipNext();	
	} else if( button == PlayControls::ACCEL ){
		G_ACCEL = !G_ACCEL;
	} else if( button == PlayControls::DBUG ){
		G_DEBUG = !G_DEBUG;
	}
	cout << "play button " << button << " pressed" << endl;
	return false;
}

bool KeplerApp::onBreadcrumbSelected( BreadcrumbEvent event )
{
	int level = event.getLevel();
	if( level == G_HOME_LEVEL ){				// BACK TO HOME
		mAlphaWheel.setShowWheel( !mAlphaWheel.getShowWheel() );
		if( mAlphaWheel.getShowWheel() ) mFovDest = 130.0f;
		mWorld.deselectAllNodes();
		mState.setSelectedNode( NULL );
		mState.setAlphaChar( ' ' );
		mCamDistPinchOffsetDest = 1.0f;
	}
	else if( level == G_ALPHA_LEVEL ){			// BACK TO ALPHA FILTER
		mWorld.deselectAllNodes();
		mData.filterArtistsByAlpha( mState.getAlphaChar() );
		mWorld.filterNodes();
		mState.setSelectedNode(NULL);
		mCamDistPinchOffsetDest = 1.0f;
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

void KeplerApp::checkForNodeTouch( const Ray &ray, Matrix44f &mat, const Vec2f &pos )
{
	vector<Node*> nodes;
	//mWorld.checkForSphereIntersect( nodes, ray, mat );
	mWorld.checkForNameTouch( nodes, pos );

	if( nodes.size() > 0 ){
		Node *nodeWithHighestGen = *nodes.begin();
		int highestGen = nodeWithHighestGen->mGen;
		
		vector<Node*>::iterator it;
		for( it = nodes.begin(); it != nodes.end(); ++it ){
			if( (*it)->mGen > highestGen ){
				highestGen = (*it)->mGen;
				nodeWithHighestGen = *it;
			}
		}
		
		
		if( nodeWithHighestGen ){
			if( highestGen == G_ARTIST_LEVEL ){
				if( ! mState.getSelectedArtistNode() )
					mState.setSelectedNode( nodeWithHighestGen );
			} else {
				mState.setSelectedNode( nodeWithHighestGen );
			}

		}
	}
}

void KeplerApp::update()
{
	if( mData.update() ){
		mWorld.initNodes( &mIpodPlayer, mFont );
		mWorld.initRingVertexArray();
		mIsLoaded = true;
	}
		
	mAccelMatrix	= lerp( mAccelMatrix, mNewAccelMatrix, 0.35f );
	updateArcball();
	mWorld.update( mMatrix );
	//mParticleController.pullToCenter( mState.getPlayingNode() );
    mParticleController.update();
	updateCamera();
	mWorld.updateGraphics( mCam, mBbRight, mBbUp );
	
	mUiLayer.update();
	mAlphaWheel.update( mFov );
	mBreadcrumbs.update();
	mPlayControls.update();
	
	updatePlayhead();
}

void KeplerApp::updateArcball()
{
	if( mTouchVel.length() > 5.0f && !mIsDragging ){
		mArcball.mouseDown( mTouchPos );
		mArcball.mouseDrag( mTouchPos + mTouchVel );
	}
	
	if( G_ACCEL ){
		mMatrix = mAccelMatrix * mArcball.getQuat();
	} else {
		mMatrix = mArcball.getQuat();
	}
	
}


void KeplerApp::updateCamera()
{
	mCamDistPinchOffset -= ( mCamDistPinchOffset - mCamDistPinchOffsetDest ) * 0.4f;
	
	Node* selectedNode = mState.getSelectedNode();
	if( selectedNode ){
		mCamDistDest	= selectedNode->mIdealCameraDist * mCamDistPinchOffset;
		mCenterDest		= mMatrix.transformPointAffine( selectedNode->mPos );
		mZoomDest		= selectedNode->mGen;
		
		mCenterFrom		+= selectedNode->mVel;

	} else {
		mCamDistDest	= G_INIT_CAM_DIST * mCamDistPinchOffset;
		mCenterDest		= mMatrix.transformPointAffine( Vec3f::zero() );

		mZoomDest		= G_HOME_LEVEL;
		if( mState.getAlphaChar() != ' ' ){
			mZoomDest	= G_ALPHA_LEVEL;
		}
	}
	
	// UPDATE FOV
	mFovDest = constrain( mFovDest, G_MIN_FOV, G_MAX_FOV );
	mFov -= ( mFov - mFovDest ) * 0.15f;
	

	if( mFovDest >= 130.0f && ! mAlphaWheel.getShowWheel() && G_ZOOM < G_ARTIST_LEVEL ){
		if (!mAlphaWheel.getShowWheel()) {
			mAlphaWheel.setShowWheel( true );
		}
		//mWorld.deselectAllNodes();
		//mState.setSelectedNode( NULL );
		//mState.setAlphaChar( ' ' );
	} else if( mFovDest < 130.0f ){
		if( mAlphaWheel.getShowWheel() ) {
//			mState.setAlphaChar( mState.getAlphaChar() );
			mAlphaWheel.setShowWheel( false );
		}
	}

	
	double p		= constrain( getElapsedSeconds()-mTime, 0.0, G_DURATION );
	mCenter			= easeInOutQuad( p, mCenterFrom, mCenterDest - mCenterFrom, G_DURATION );
	mCamDist		= easeInOutQuad( p, mCamDistFrom, mCamDistDest - mCamDistFrom, G_DURATION );
	mCamDist		= min( mCamDist, G_INIT_CAM_DIST );
	G_ZOOM			= easeInOutQuad( p, mZoomFrom, mZoomDest - mZoomFrom, G_DURATION );
	
	Vec3f prevEye	= mEye;
	mEye			= Vec3f( mCenter.x, mCenter.y, mCenter.z - mCamDist );
	mCamVel			= mEye - prevEye;
	
	mCam.setPerspective( mFov, getWindowAspectRatio(), 0.0001f, 4000.0f );
	mCam.lookAt( mEye, mCenter, mUp );
	mCam.getBillboardVectors( &mBbRight, &mBbUp );
}

void KeplerApp::updatePlayhead()
{
	if( mIpodPlayer.getPlayState() == ipod::Player::StatePlaying ){
		mCurrentTrackPlayheadTime	= mIpodPlayer.getPlayheadTime();
		mCurrentTrackLength			= mIpodPlayer.getPlayingTrack()->getLength();
	}
}


void KeplerApp::drawLoader()
{
	gl::color( Color( 1.0f, 1.0f, 1.0f ) );
	mLoadingTex.enableAndBind();
	gl::setMatricesWindow( getWindowSize() );
	gl::drawSolidRect( getWindowBounds() );
	mLoadingTex.disable();
	
	gl::enableAdditiveBlending();
	float xCenter = getWindowWidth() * 0.5f;
	float yCenter = getWindowHeight() * 0.5f;

	Rectf bigRect = Rectf( xCenter - 50, yCenter - 50, xCenter + 50, yCenter + 50 );
	mStarGlowTex.enableAndBind();
	gl::drawSolidRect( bigRect );
	mStarGlowTex.disable();
	
	Rectf rect = Rectf( xCenter - 28, yCenter - 28, xCenter + 28, yCenter + 28 );
	mStarTex.enableAndBind();
	gl::drawSolidRect( rect );
	
	float smallOffset	= cos( getElapsedSeconds() * 0.3f + 2.0f ) * 30.0f;
	Rectf smallRect		= Rectf( xCenter - 4.0f + smallOffset, yCenter - 4.0f, xCenter + 4.0f + smallOffset, yCenter + 4.0f );
	//float mediumOffset	= ( getElapsedSeconds() - 3.0f ) * 10.0f;	
	//Rectf mediumRect	= Rectf( xCenter - 25.0f + mediumOffset * 2.5f, yCenter - 25.0f, xCenter + 25.0f + mediumOffset * 2.5f, yCenter + 25.0f );
	gl::color( Color( 0.0f, 0.0f, 0.0f ) );
	gl::disableAlphaBlending();
	gl::enableAlphaBlending();
	gl::drawSolidRect( smallRect );
	//gl::drawSolidRect( mediumRect );
	mStarTex.disable();
	
	gl::disableAlphaBlending();
}

void KeplerApp::draw()
{
	gl::clear( Color( 0, 0, 0 ), true );
	if( !mIsLoaded ){
		drawLoader();
		
		init();
	} else {
		drawScene();
	}
}

void KeplerApp::drawScene()
{
    gl::enableDepthWrite();
    gl::setMatrices( mCam );
    
	// DRAW SKYDOME
    gl::pushModelView();
    gl::rotate( mMatrix );
    gl::color( Color( 1.0f, 1.0f, 1.0f ) );
    mSkyDome.enableAndBind();
    gl::drawSphere( Vec3f::zero(), 990.0f, 16 );
    gl::popModelView();
    
    
    gl::enableAdditiveBlending();
    
    
	// STARS
    mStarTex.enableAndBind();
    mWorld.drawStars();
    mStarTex.disable();
    
    
	// PLANETS
    //Node *albumNode  = mState.getSelectedAlbumNode();
    Node *artistNode = mState.getSelectedArtistNode();
    if( artistNode ){
        gl::enableDepthRead();
        gl::disableAlphaBlending();
        
        glEnable( GL_LIGHTING );
        glEnable( GL_COLOR_MATERIAL );
        glShadeModel( GL_SMOOTH );
        
        glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse );				
        
        // LIGHT FROM ARTIST
        glEnable( GL_LIGHT0 );
        Vec3f lightPos          = artistNode->mTransPos;
        GLfloat artistLight[]	= { lightPos.x, lightPos.y, lightPos.z, 1.0f };
        glLightfv( GL_LIGHT0, GL_POSITION, artistLight );
        glLightfv( GL_LIGHT0, GL_DIFFUSE, ColorA( ( artistNode->mGlowColor + Color::white() * 2.5f ), 1.0f ) );
        
        
        // PLANETS
        mWorld.drawPlanets( mPlanetsTex );
		
        // CLOUDS
        mWorld.drawClouds( mCloudsTex );
        
        // RINGS
        gl::enableAdditiveBlending();
        //			mWorld.drawRings( mRingsTex );
        glDisable( GL_LIGHTING );
        gl::disableDepthRead();
    }
    
    
    // STARGLOWS
    mStarGlowTex.enableAndBind();
    mWorld.drawStarGlows();
	mWorld.drawTouchHighlights();
    mStarGlowTex.disable();
    
    
    gl::disableDepthWrite();
    
    // ORBITS
    gl::enableAdditiveBlending();
    gl::enableDepthRead();
    mWorld.drawOrbitRings( mState.getPlayingNode() );

	// PARTICLES
	//mParticleController.draw( mState.getSelectedArtistNode(), mMatrix );
	if( mState.getSelectedArtistNode() ){
		mStarGlowTex.enableAndBind();
		mParticleController.drawScreenspace( mState.getSelectedArtistNode(), mMatrix, mBbRight, mBbUp );
		mStarGlowTex.disable();
	}
    
    // CONSTELLATION
    mDottedTex.enableAndBind();
    mWorld.drawConstellation( mMatrix );
    mDottedTex.disable();

	gl::disableDepthRead();
	
	// NAMES
    gl::disableDepthWrite();
    glEnable( GL_TEXTURE_2D );
    gl::setMatricesWindow( getWindowSize() );
    gl::enableAdditiveBlending();
    float pinchAlphaOffset = constrain( 1.0f - ( mCamDistPinchOffset - 3.0f ), 0.0f, 1.0f );
    mWorld.drawNames( mCam, pinchAlphaOffset );
    
    
    glDisable( GL_TEXTURE_2D );
    
    
    gl::disableAlphaBlending();
    gl::enableAlphaBlending();
    mUiLayer.draw( mPanelButtonsTex );
    mAlphaWheel.draw();
    mBreadcrumbs.draw();//mUiLayer.getPanelYPos() + 5.0f );
    mPlayControls.draw( mButtonsTex, mSliderBgTex, mFontSmall, mUiLayer.getPanelYPos(), mCurrentTrackPlayheadTime, mCurrentTrackLength );
    mState.draw( mFont );
    
    if( G_DEBUG ) {
        drawInfoPanel();
    }
}



void KeplerApp::drawInfoPanel()
{
	gl::setMatricesWindow( getWindowSize() );
	if( getElapsedFrames() % 30 == 0 ){
		setParamsTex();
	}
	gl::color( Color( 1.0f, 1.0f, 1.0f ) );
	gl::draw( mParamsTex, Vec2f( 23.0f, 25.0f ) );
}


void KeplerApp::setParamsTex()
{
    stringstream s;
	TextLayout layout;	
	layout.setFont( mFontSmall );
	layout.setColor( Color( 1.0f, 0.0f, 0.0f ) );

	int currentLevel = G_HOME_LEVEL;
	if (mState.getSelectedNode()) {
		currentLevel = mState.getSelectedNode()->mGen;
	}
	else if (mState.getAlphaChar() != ' ') {
		currentLevel = G_ALPHA_LEVEL;
	}
	
	
	s.str("");
	s << " CURRENT LEVEL: " << currentLevel;
	layout.addLine( s.str() );
	
	s.str("");
	s << " FPS: " << getAverageFps();
	layout.addLine( s.str() );
	
	s.str("");
	s << " ZOOM LEVEL: " << G_ZOOM;
	layout.addLine( s.str() );
	
	mParamsTex = gl::Texture( layout.render( true, false ) );
}


bool KeplerApp::onPlayerTrackChanged( ipod::Player *player )
{	
	std::cout << "==================================================================" << std::endl;
	console() << "track changed!" << std::endl;

	if (player->getPlayState() == ipod::Player::StatePlaying) {
		
		// get refs to the currently playing things...
		ipod::TrackRef playingTrack = player->getPlayingTrack();
		ipod::PlaylistRef playingArtist = ipod::getArtist(playingTrack->getArtistId());
		ipod::PlaylistRef playingAlbum = ipod::getAlbum(playingTrack->getAlbumId());

		// FIXME: do all this getName() stuff with getId()
		
		console() << "searching all our nodes for the new playing song..." << endl;
		for (int i = 0; i < mWorld.mNodes.size(); i++) {
			Node *artistNode = mWorld.mNodes[i];
			if (artistNode->getName() == playingArtist->getArtistName()) {
				console() << "hey it's an artist we know!" << endl;
				if (!artistNode->mIsSelected) {
					console() << "and it needs to be selected!" << endl;
					mState.setAlphaChar(artistNode->getName());
					mState.setSelectedNode(artistNode);
				}
				for (int j = 0; j < artistNode->mChildNodes.size(); j++) {					
					Node *albumNode = artistNode->mChildNodes[j];
					if (albumNode->getName() == playingAlbum->getAlbumTitle()) {
						console() << "and we know the album!" << endl;
						if (!albumNode->mIsSelected) {
							console() << "and the album needs to be selected" << endl;
							mState.setSelectedNode(albumNode);
						}
						for (int k = 0; k < albumNode->mChildNodes.size(); k++) {
							// FIXME: what's the proper C++ way to do this?
							NodeTrack *trackNode = (NodeTrack*)(albumNode->mChildNodes[k]);
							if (trackNode->mTrack->getItemId() == playingTrack->getItemId()) {
								console() << "and we know the track!" << endl;
								if (!trackNode->mIsSelected) {
									console() << "quick! select it!!!" << endl;
									mState.setSelectedNode(trackNode);
								}
								mState.setPlayingNode(trackNode);
								break;
							}
						}								
						break;
					}
//					else {
//						console() << "new album from current artist, do something clever!" << endl;
//					}
				}
				break;
			}
		}
	}
	else {
		// FIXME: when we pause we'll stop drawing orbits because of this, which is probably the wrong behavior
		mState.setPlayingNode(NULL);
		console() << "trackchanged but nothing's playing" << endl;
	}
	
	
	
	/*
	if( mCurrentAlbum ){
		ipod::TrackRef currentTrack = mCurrentAlbum[mCurrentTrackId];
		
		if( player->getPlayingTrack() != currentTrack ){
			cout << "player album ID = " << player->getPlayingTrack()->getAlbumId() << endl;
			cout << "player artist ID = " << player->getPlayingTrack()->getArtistId() << endl;
			
			if( currentTrack ){
				cout << "global album ID = " << currentTrack->getAlbumId() << endl;
				cout << "global artist ID = " << currentTrack->getArtistId() << endl;
			}
			mState.setSelectedNode( currentAlbumNode->mChildNodes[mCurrentTrackId] );
		}
	}*/
	
    //console() << "Now Playing: " << player->getPlayingTrack()->getTitle() << endl;
    return false;
}

bool KeplerApp::onPlayerStateChanged( ipod::Player *player )
{	
	std::cout << "onPlayerStateChanged()" << std::endl;
    switch( player->getPlayState() ){
        case ipod::Player::StatePlaying:
            console() << "Playing..." << endl;
			mPlayControls.setPlaying(true);
            break;
        case ipod::Player::StateStopped:
            console() << "Stopped." << endl;
			mPlayControls.setPlaying(false);
			break;
        default:
            console() << "Other!" << endl;
			mPlayControls.setPlaying(false);
            break;
    }
    return false;
}


CINDER_APP_COCOA_TOUCH( KeplerApp, RendererGl )
