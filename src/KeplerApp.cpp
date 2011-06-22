#include <vector>

#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/Font.h"
#include "cinder/Arcball.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"

#include "CinderIPod.h"
#include "CinderIPodPlayer.h"
#include "CinderFlurry.h"

#include "OrientationHelper.h"
#include "GyroHelper.h"
#include "Device.h"

#include "Globals.h"

#include "State.h"
#include "Data.h"
#include "PlaylistFilter.h"
#include "LetterFilter.h"

#include "World.h"
#include "NodeArtist.h"
#include "Galaxy.h"
#include "BloomSphere.h"

#include "LoadingScreen.h"
#include "UiLayer.h"
#include "PlayControls.h"
#include "HelpLayer.h"
#include "NotificationOverlay.h"
#include "Stats.h"
#include "AlphaWheel.h"
#include "PinchRecognizer.h"
#include "ParticleController.h"

#include "Easing.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace pollen::flurry;
using namespace bloom;

float G_ZOOM			= 0;
int G_CURRENT_LEVEL		= 0;
bool G_ACCEL			= true;
bool G_DEBUG			= false;
bool G_SHOW_SETTINGS	= false;
bool G_HELP             = false;
bool G_DRAW_RINGS		= false;
bool G_DRAW_TEXT		= false;
bool G_USE_GYRO			= false;
bool G_IS_IPAD2			= false;
int G_NUM_PARTICLES		= 25;
int G_NUM_DUSTS			= 250;

class KeplerApp : public AppCocoaTouch {
  public:
    
	virtual void	setup();
    void            prepareSettings(Settings *settings);
    void            remainingSetup();
	void			initLoadingTextures();
	void			initTextures();

	virtual void	touchesBegan( TouchEvent event );
	virtual void	touchesMoved( TouchEvent event );
	virtual void	touchesEnded( TouchEvent event );

	bool            orientationChanged( OrientationEvent event );
    void            setInterfaceOrientation( const Orientation &orientation );
	
    virtual void	update();
	void			updateArcball();
	void			updateCamera();

	virtual void	draw();
	void			drawNoArtists();
    void            drawScene();

    // convenience methods for Flurry
    void            logEvent(const string &event);
    void            logEvent(const string &event, const map<string,string> &params);
    
	bool			onAlphaCharStateChanged( State *state );
	bool			onPlaylistStateChanged( State *state );
	bool			onAlphaCharSelected( AlphaWheel *alphaWheel );
	bool			onWheelToggled( AlphaWheel *alphaWheel );

	bool			onPlayControlsButtonPressed ( PlayControls::ButtonId button );
	bool			onPlayControlsPlayheadMoved ( float amount );
	
    bool			onSelectedNodeChanged( Node *node );

	void			checkForNodeTouch( const Ray &ray, const Vec2f &pos );
	
    bool			onPlayerStateChanged( ipod::Player *player );
    bool			onPlayerTrackChanged( ipod::Player *player );
    bool			onPlayerLibraryChanged( ipod::Player *player );
	
// UI BITS:
    LoadingScreen       mLoadingScreen;
	AlphaWheel          mAlphaWheel;
	UiLayer             mUiLayer;
	HelpLayer		    mHelpLayer;
    NotificationOverlay mNotificationOverlay;
    
// 3D BITS:
	World               mWorld;
    
// DATA/TRUTH BITS:
	State               mState;
	Data                mData;

// ORIENTATION
    OrientationHelper mOrientationHelper;    
    Orientation       mInterfaceOrientation;
    Matrix44f         mOrientationMatrix;
    Matrix44f         mInverseOrientationMatrix;    
    
// GYRO
    GyroHelper        mGyroHelper;
    
// AUDIO
	ipod::Player		mIpodPlayer;
    ipod::TrackRef      mPlayingTrack;
	int					mPlaylistIndex; // FIXME: move this into State
    double              mCurrentTrackLength; // cached by onPlayerTrackChanged
    double              mCurrentTrackPlayheadTime;
    ipod::Player::State mCurrentPlayState;
	
// PLAY CONTROLS
	PlayControls	mPlayControls;
	
// CAMERA PERSP
	CameraPersp		mCam;
	float			mFov, mFovDest;
	Vec3f			mEye, mCenter, mUp;
	Vec3f			mCenterDest, mCenterFrom;
	Vec3f			mCenterOffset;		// if pinch threshold is exceeded, look towards parent?
	float			mCamDist, mCamDistDest, mCamDistFrom;
	float			mCamDistAnim;
	float			mPinchPerInit;
	float			mPinchPer;			// 0.0 (max pinched) to 1.0 (max spread)
	float			mPinchTotalDest;	// accumulation of pinch deltas
	float			mPinchTotal;		// eases to pinchTotalDest
	float			mPinchTotalInit;	// reset value of pinchTotalDest
	float			mPinchScaleMax, mPinchScaleMin;
	float			mPinchPerThresh;
	float			mPinchPerFrom, mPinchTotalFrom;	// used to decelerate values post new node selection
	float			mPinchHighlightRadius;
	float			mPinchRotation;
	bool			mIsPastPinchThresh;
	
	float			mZoomFrom, mZoomDest;
	Arcball			mArcball;
	float			mCamRingAlpha; // 1.0 = camera is viewing rings side-on
								   // 0.0 = camera is viewing rings from above or below
	float			mFadeInAlphaToArtist;
	float			mFadeInArtistToAlbum;
	float			mFadeInAlbumToTrack;
	
	float			mAlphaWheelRadius;
	
// FONTS
	Font			mFontLarge;
	Font			mFont;
	Font			mFontBig;
	Font			mFontMediSmall;
	Font			mFontMediTiny;
	
// MULTITOUCH
	Vec2f			mTouchPos;
	Vec2f			mTouchVel;
	bool			mIsDragging;
    bool            mIsTouching; // set true in touchesBegan if there's one touch and it's in-world
	bool			onPinchBegan( PinchEvent event );
    bool			onPinchMoved( PinchEvent event );
    bool			onPinchEnded( PinchEvent event );
    bool            keepTouchForPinching( TouchEvent::Touch touch );
    bool            positionTouchesWorld( Vec2f pos );
    vector<Ray>		mPinchRays;
	vector<Vec2f>	mPinchPositions;
    PinchRecognizer mPinchRecognizer;
	float			mTimePinchEnded;
	float			mPinchAlphaPer;
	bool			mIsPinching;
	
// PARTICLES
    ParticleController mParticleController;

// STATS
    Stats mStats;
    
	// TEXTURES
	gl::Texture		mStarTex, mStarGlowTex, mStarCoreTex, mEclipseGlowTex, mLensFlareTex;
	gl::Texture		mEclipseShadowTex;
	gl::Texture		mSkyDome, mGalaxyDome;
	gl::Texture		mDottedTex;
	gl::Texture		mPlayheadProgressTex;
    gl::Texture     mRingsTex;
	gl::Texture		mUiButtonsTex, mUiBigButtonsTex, mUiSmallButtonsTex, mOverlayIconsTex;
    gl::Texture		mAtmosphereTex, mAtmosphereDirectionalTex, mAtmosphereSunTex;
	gl::Texture		mNoArtistsTex;
	gl::Texture		mParticleTex;
	gl::Texture		mGalaxyTex;
	gl::Texture		mDarkMatterTex;
	gl::Texture		mOrbitRingGradientTex;
	gl::Texture		mTrackOriginTex;
	vector<gl::Texture> mCloudsTex;
	
	Surface			mHighResSurfaces;
	Surface			mLowResSurfaces;
	Surface			mNoAlbumArtSurface;
	
// SKYDOME
    BloomSphere mSkySphere;
    
// GALAXY
    Galaxy mGalaxy;
	
	float			mTime;
    bool            mRemainingSetupCalled; // setup() is short and fast, remainingSetup() is slow
};

void KeplerApp::prepareSettings(Settings *settings)
{
#ifdef DEBUG
    // "Kepler" ID:
    Flurry::getInstrumentation()->init("DZ7HPD6FE1GGADVNJ3EX");
#else
    // "Planetary" ID:
    Flurry::getInstrumentation()->init("7FY9M7BIVCFVJRGNSD1E");
#endif
}

void KeplerApp::setup()
{
	Flurry::getInstrumentation()->startTimeEvent("Setup");
    
    //console() << "setupStart: " << getElapsedSeconds() << std::endl;
    
    G_IS_IPAD2 = bloom::isIpad2();
    console() << "G_IS_IPAD2: " << G_IS_IPAD2 << endl;

	if( G_IS_IPAD2 ){
		G_NUM_PARTICLES = 30;
		G_NUM_DUSTS = 2500;
        mGyroHelper.setup();
	}
	
	mOrientationHelper.setup();
    mOrientationHelper.registerOrientationChanged( this, &KeplerApp::orientationChanged );    
    setInterfaceOrientation( mOrientationHelper.getInterfaceOrientation() );
    
    mRemainingSetupCalled = false;

    // !!! this has to be set up before any other UI things so it can consume touch events
    mLoadingScreen.setup( this, mOrientationHelper.getInterfaceOrientation() );
    
    initLoadingTextures();

    Flurry::getInstrumentation()->stopTimeEvent("Setup");
}

void KeplerApp::remainingSetup()
{
    if (mRemainingSetupCalled) return;

    Flurry::getInstrumentation()->startTimeEvent("Remaining Setup");

    mRemainingSetupCalled = true;

	mLoadingScreen.setEnabled( true );
    
	G_DRAW_RINGS	= true;
	G_DRAW_TEXT		= true;
	//Rand::randomize();

    // TEXTURES
    initTextures();
	
	// ARCBALL
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( getWindowCenter() );
	mArcball.setRadius( G_DEFAULT_ARCBALL_RADIUS );
	mArcball.setQuat( Quatf( -0.2, 0.0f, -0.3f ) );
	
	// CAMERA PERSP
	mCamDist			= G_INIT_CAM_DIST;
	mCamDistDest		= mCamDist;
	mPinchPerInit		= 0.2f;
	mPinchPer			= mPinchPerInit;
	mPinchScaleMax		= 3.0f;
	mPinchScaleMin		= 0.5f;
	mPinchTotalInit		= ( mPinchScaleMax - mPinchScaleMin ) * mPinchPerInit + mPinchScaleMin;
	mPinchTotal			= mPinchTotalInit;
	mPinchTotalDest		= mPinchTotal;
	mPinchPerThresh		= 0.65f;
	mPinchPerFrom		= mPinchPer;
	mPinchTotalFrom		= mPinchTotal;
	mIsPinching			= false;
	mPinchHighlightRadius = 50.0f;
	mPinchRotation		= 0.0f;
	mIsPastPinchThresh	= false;
	
	mCamDistFrom		= mCamDist;
	mCamDistAnim		= 0.0f;
	mEye				= Vec3f( 0.0f, 0.0f, mCamDist );
	mCenter				= Vec3f::zero();
	mCenterDest			= mCenter;
	mCenterFrom			= mCenter;
	mCenterOffset		= mCenter;
    // FIXME: let's put this setup stuff back in setup()
    // this was overriding the (correct) value which is now always set by setInterfaceOrientation
//	mUp					= Vec3f::yAxis();
	mFov				= G_DEFAULT_FOV;
	mFovDest			= G_DEFAULT_FOV;
	mCam.setPerspective( mFov, getWindowAspectRatio(), 0.0001f, 1200.0f );
	mFadeInAlphaToArtist = 0.0f;
	mFadeInArtistToAlbum = 0.0f;
	mFadeInAlbumToTrack = 0.0f;
	
	
// FONTS
    // NB:- to would-be optimizers:
    //      loadResource is fairly fast (~7ms for 5 fonts)
    //      Font(...) is a bit slower, ~250ms for these fonts (measured a few times)
	mFontLarge			= Font( loadResource( "AauxPro-Black.ttf" ), 26 );
	mFont				= Font( loadResource( "UnitRoundedOT-Medi.otf" ), 14 );
	mFontBig			= Font( loadResource( "AauxPro-Black.ttf"), 24 );
	mFontMediSmall		= Font( loadResource( "UnitRoundedOT-Medi.otf" ), 13 );
	mFontMediTiny		= Font( loadResource( "UnitRoundedOT-Medi.otf" ), 11 );

// STATS
    mStats.setup( mFont, BRIGHT_BLUE, BLUE );
	
// NOTIFICATION OVERLAY
	mNotificationOverlay.setup( this, mOrientationHelper.getInterfaceOrientation(), mFontLarge );
	
	
// TOUCH VARS
	mTouchPos			= getWindowCenter();
	mTouchVel			= Vec2f(2.1f, 0.3f );
	mIsDragging			= false;
    mIsTouching         = false;
	mTime				= getElapsedSeconds();
	mTimePinchEnded		= 0.0f;
	mPinchRecognizer.init(this);
    mPinchRecognizer.registerBegan( this, &KeplerApp::onPinchBegan );
    mPinchRecognizer.registerMoved( this, &KeplerApp::onPinchMoved );
    mPinchRecognizer.registerEnded( this, &KeplerApp::onPinchEnded );
    mPinchRecognizer.setKeepTouchCallback( this, &KeplerApp::keepTouchForPinching );
	mPinchAlphaPer		= 1.0f;
    
    // PARTICLES
    mParticleController.addParticles( G_NUM_PARTICLES );
	mParticleController.addDusts( G_NUM_DUSTS );
	
    // NB:- order of UI init is important to register callbacks in correct order
    
	// PLAY CONTROLS
	mPlayControls.setup( this, mOrientationHelper.getInterfaceOrientation(), &mIpodPlayer, mFontMediSmall, mFontMediTiny, mUiButtonsTex, mUiBigButtonsTex, mUiSmallButtonsTex );
	mPlayControls.registerButtonPressed( this, &KeplerApp::onPlayControlsButtonPressed );
	mPlayControls.registerPlayheadMoved( this, &KeplerApp::onPlayControlsPlayheadMoved );

	// UILAYER
	mUiLayer.setup( this, mOrientationHelper.getInterfaceOrientation(), G_SHOW_SETTINGS );

	// HELP LAYER
	mHelpLayer.setup( this, mOrientationHelper.getInterfaceOrientation() );
	mHelpLayer.initHelpTextures( mFontMediSmall );
	
    // ALPHA WHEEL
	mAlphaWheelRadius = 300.0f;
	mAlphaWheel.setup( this, mOrientationHelper.getInterfaceOrientation(), mAlphaWheelRadius );
	mAlphaWheel.registerAlphaCharSelected( this, &KeplerApp::onAlphaCharSelected );
	mAlphaWheel.registerWheelToggled( this, &KeplerApp::onWheelToggled );
	mAlphaWheel.initAlphaTextures( mFontBig );	
	
	// STATE
	mState.registerAlphaCharStateChanged( this, &KeplerApp::onAlphaCharStateChanged );
	mState.registerNodeSelected( this, &KeplerApp::onSelectedNodeChanged );
	mState.registerPlaylistStateChanged( this, &KeplerApp::onPlaylistStateChanged );
	
	// PLAYER
    mCurrentPlayState = mIpodPlayer.getPlayState();
    // FIXME: init mCurrentTrackLength here?
	mIpodPlayer.registerStateChanged( this, &KeplerApp::onPlayerStateChanged );
    mIpodPlayer.registerTrackChanged( this, &KeplerApp::onPlayerTrackChanged );
    mIpodPlayer.registerLibraryChanged( this, &KeplerApp::onPlayerLibraryChanged );
	mPlaylistIndex = 0;
	
    // DATA
    mData.setup(); // NB:- is asynchronous, see update() for what happens when it's done

    // WORLD
    mWorld.setup();
	
    // SKY
    mSkySphere.setup(24);
    
    // GALAXY (TODO: Move to World?)
    mGalaxy.setup(G_INIT_CAM_DIST, BRIGHT_BLUE, BLUE, mGalaxyDome, mGalaxyTex, mDarkMatterTex, mStarGlowTex);

    // Make sure initial PlayControl settings are correct:
    mPlayControls.setAlphaWheelVisible( mAlphaWheel.getShowWheel() );
    mPlayControls.setShowSettings( G_SHOW_SETTINGS );
    mPlayControls.setOrbitsVisible( G_DRAW_RINGS );
    mPlayControls.setLabelsVisible( G_DRAW_TEXT );
    mPlayControls.setHelpVisible( G_HELP );
    mPlayControls.setDebugVisible( G_DEBUG );	    
    mPlayControls.setShuffleVisible( mIpodPlayer.getShuffleMode() != ipod::Player::ShuffleModeOff );
    mPlayControls.setRepeatVisible( mIpodPlayer.getRepeatMode() != ipod::Player::RepeatModeNone );    
    if( G_IS_IPAD2 ) {
        mPlayControls.setGyroVisible( G_USE_GYRO );
    }    
    
    Flurry::getInstrumentation()->stopTimeEvent("Remaining Setup");

    //console() << "setupEnd: " << getElapsedSeconds() << std::endl;
}

void KeplerApp::initLoadingTextures()
{
    gl::Texture::Format fmt;
    fmt.enableMipmapping( true );
    fmt.setMinFilter( GL_LINEAR_MIPMAP_LINEAR );    
    fmt.setMagFilter( GL_LINEAR ); // TODO: experiment with GL_NEAREST where appropriate
	mStarGlowTex = gl::Texture( loadImage( loadResource( "starGlow.png" ) ), fmt);
}

void KeplerApp::initTextures()
{
    Flurry::getInstrumentation()->startTimeEvent("Load Textures");    
    
    gl::Texture::Format mipFmt;
    mipFmt.enableMipmapping( true );
    mipFmt.setMinFilter( GL_LINEAR_MIPMAP_LINEAR );    
    mipFmt.setMagFilter( GL_LINEAR ); // TODO: experiment with GL_NEAREST where appropriate

    gl::Texture::Format repeatMipFmt;
    repeatMipFmt.enableMipmapping( true );
    repeatMipFmt.setMinFilter( GL_LINEAR_MIPMAP_LINEAR );    
    repeatMipFmt.setMagFilter( GL_LINEAR ); // TODO: experiment with GL_NEAREST where appropriate
    repeatMipFmt.setWrap( GL_REPEAT, GL_REPEAT );
    
    mStarTex                  = gl::Texture( loadImage( loadResource( "star.png" ) ), mipFmt );
	mStarCoreTex              = gl::Texture( loadImage( loadResource( "starCore.png" ) ), mipFmt );
	mEclipseGlowTex           = gl::Texture( loadImage( loadResource( "eclipseGlow.png" ) ), mipFmt );
	mEclipseShadowTex         = gl::Texture( loadImage( loadResource( "eclipseShadow.png" ) ), mipFmt );
	mLensFlareTex             = gl::Texture( loadImage( loadResource( "lensFlare.png" ) ), mipFmt );
	mParticleTex              = gl::Texture( loadImage( loadResource( "particle.png" ) ), mipFmt );
	mSkyDome                  = gl::Texture( loadImage( loadResource( "skydome.png" ) ), mipFmt );
	mGalaxyDome               = gl::Texture( loadImage( loadResource( "skydome.jpg" ) ), mipFmt );
	mDottedTex                = gl::Texture( loadImage( loadResource( "dotted.png" ) ), repeatMipFmt );
	mRingsTex                 = gl::Texture( loadImage( loadResource( "rings.png" ) ) /*, fmt */ );
    mPlayheadProgressTex      = gl::Texture( loadImage( loadResource( "playheadProgress.png" ) ), repeatMipFmt );
	mUiButtonsTex             = gl::Texture( loadImage( loadResource( "uiButtons.png" ) )/*, fmt*/ );
	mUiBigButtonsTex          = gl::Texture( loadImage( loadResource( "uiBigButtons.png" ) )/*, fmt*/ );
	mUiSmallButtonsTex        = gl::Texture( loadImage( loadResource( "uiSmallButtons.png" ) )/*, fmt*/ );
	mOverlayIconsTex          = gl::Texture( loadImage( loadResource( "overlayIcons.png" ) )/*, fmt*/ );
    mAtmosphereTex            = gl::Texture( loadImage( loadResource( "atmosphere.png" ) ), mipFmt );
	mAtmosphereDirectionalTex = gl::Texture( loadImage( loadResource( "atmosphereDirectional.png" ) ), mipFmt );
	mAtmosphereSunTex         = gl::Texture( loadImage( loadResource( "atmosphereSun.png" ) ), mipFmt );
	mGalaxyTex                = gl::Texture( loadImage( loadResource( "galaxy.jpg" ) ), mipFmt );
	mDarkMatterTex            = gl::Texture( loadImage( loadResource( "darkMatter.png" ) )/*, fmt*/ );
	mOrbitRingGradientTex     = gl::Texture( loadImage( loadResource( "orbitRingGradient.png" ) ), mipFmt );
	mTrackOriginTex           = gl::Texture( loadImage( loadResource( "origin.png" ) ), mipFmt );

	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "planetClouds1.png" ) ), mipFmt ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "planetClouds2.png" ) ), mipFmt ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "planetClouds3.png" ) ), mipFmt ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "planetClouds4.png" ) ), mipFmt ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "planetClouds5.png" ) ), mipFmt ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "moonClouds1.png" ) ), mipFmt ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "moonClouds2.png" ) ), mipFmt ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "moonClouds3.png" ) ), mipFmt ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "moonClouds4.png" ) ), mipFmt ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "moonClouds5.png" ) ), mipFmt ) );
	
	mHighResSurfaces	= Surface( loadImage( loadResource( "surfacesHighRes.png" ) ) );
	mLowResSurfaces		= Surface( loadImage( loadResource( "surfacesLowRes.png" ) ) );
    mNoAlbumArtSurface = Surface( loadImage( loadResource( "noAlbumArt.png" ) ) );
    
    Flurry::getInstrumentation()->stopTimeEvent("Load Textures");    
}

void KeplerApp::touchesBegan( TouchEvent event )
{	
    if (!mRemainingSetupCalled) return;
	mIsDragging = false;
	const vector<TouchEvent::Touch> touches = getActiveTouches();
	float timeSincePinchEnded = getElapsedSeconds() - mTimePinchEnded;
	if( touches.size() == 1 && timeSincePinchEnded > 0.2f && keepTouchForPinching(*touches.begin()) ) {
        mIsTouching = true;
        mTouchPos		= touches.begin()->getPos();
        mTouchVel		= Vec2f::zero();
		Vec3f worldTouchPos;
		if( G_USE_GYRO ) worldTouchPos = Vec3f(mTouchPos,0);
		else			 worldTouchPos = mInverseOrientationMatrix * Vec3f(mTouchPos,0);
        mArcball.mouseDown( Vec2i(worldTouchPos.x, worldTouchPos.y) );
	}
    else {
        mIsTouching = false;
    }
}

void KeplerApp::touchesMoved( TouchEvent event )
{
    if (!mRemainingSetupCalled) return;

    if ( mIsTouching ) {
        float timeSincePinchEnded = getElapsedSeconds() - mTimePinchEnded;	
        const vector<TouchEvent::Touch> touches = getActiveTouches();
        if( touches.size() == 1 && timeSincePinchEnded > 0.2f ){
            Vec2f currentPos = touches.begin()->getPos();
            Vec2f prevPos = touches.begin()->getPrevPos();        
            if (positionTouchesWorld(currentPos) && positionTouchesWorld(prevPos)) {
                mIsDragging = true;
                mTouchVel		= currentPos - prevPos;
                mTouchPos		= currentPos;
                Vec3f worldTouchPos;
				if( G_USE_GYRO ) worldTouchPos = Vec3f(mTouchPos,0);
				else			 worldTouchPos = mInverseOrientationMatrix * Vec3f(mTouchPos,0);
                mArcball.mouseDrag( Vec2i( worldTouchPos.x, worldTouchPos.y ) );
            }
        }
    }
}

void KeplerApp::touchesEnded( TouchEvent event )
{
    if (!mRemainingSetupCalled) return;    
	float timeSincePinchEnded = getElapsedSeconds() - mTimePinchEnded;	
	const vector<TouchEvent::Touch> touches = event.getTouches();
	if( touches.size() == 1 && timeSincePinchEnded > 0.2f ){        
        Vec2f currentPos = touches.begin()->getPos();
        Vec2f prevPos = touches.begin()->getPrevPos();   
        if (positionTouchesWorld(currentPos) && positionTouchesWorld(prevPos)) {
            mTouchPos = currentPos;
			
            // if the nav wheel isnt showing and you havent been dragging
			// and your touch is above the uiLayer panel and the Help panel isnt showing
            if( ! mAlphaWheel.getShowWheel() && ! mIsDragging && !G_HELP ){
                float u			= mTouchPos.x / (float) getWindowWidth();
                float v			= mTouchPos.y / (float) getWindowHeight();
                Ray touchRay	= mCam.generateRay( u, 1.0f - v, mCam.getAspectRatio() );
                checkForNodeTouch( touchRay, mTouchPos );
            }
        }
	}
	if (getActiveTouches().size() != 1) {
        if (mIsDragging) {
            logEvent("Camera Moved");
        }
		mIsDragging = false;
        mIsTouching = false;
	}
}

bool KeplerApp::onPinchBegan( PinchEvent event )
{
	mIsPinching = true;
    mPinchRays = event.getTouchRays( mCam );
	mPinchPositions.clear();
	
	mTouchVel	= Vec2f::zero();
	vector<PinchEvent::Touch> touches = event.getTouches();
	Vec2f averageTouchPos = Vec2f::zero();
	
	for( vector<PinchEvent::Touch>::iterator it = touches.begin(); it != touches.end(); ++it ){
		averageTouchPos += it->mPos;
		mPinchPositions.push_back( it->mPos );
	}
	averageTouchPos /= touches.size();
	mTouchPos = averageTouchPos;
	
// using pinch to control arcball is weird because of the pop
// from one finger to two fingers. disabled until a fix is found.
//	Vec3f worldTouchPos = mInverseOrientationMatrix * Vec3f(mTouchPos,0);
//	mArcball.mouseDrag( Vec2i( worldTouchPos.x, worldTouchPos.y ) );
	
    return false;
}

bool KeplerApp::onPinchMoved( PinchEvent event )
{	
    mPinchRays = event.getTouchRays( mCam );
	mPinchPositions.clear();
	
	mPinchTotalDest *= ( 1.0f + ( 1.0f - event.getScaleDelta() ) * 0.65f * mPinchScaleMax );
	mPinchTotalDest = constrain( mPinchTotalDest, mPinchScaleMin, mPinchScaleMax );
	
	vector<PinchEvent::Touch> touches = event.getTouches();
	Vec2f averageTouchPos = Vec2f::zero();
	for( vector<PinchEvent::Touch>::iterator it = touches.begin(); it != touches.end(); ++it ){
		averageTouchPos += it->mPos;
		mPinchPositions.push_back( it->mPos );
	}
	averageTouchPos /= touches.size();
// using pinch to control arcball is weird because of the pop
// from one finger to two fingers. disabled until a fix is found.
//	Vec3f worldTouchPos = mInverseOrientationMatrix * Vec3f(mTouchPos,0);
//	mArcball.mouseDrag( Vec2i( worldTouchPos.x, worldTouchPos.y ) );
	
	//mTouchThrowVel	= ( averageTouchPos - mTouchPos );
	//mTouchVel		= mTouchThrowVel;
	mTouchPos		= averageTouchPos;
	mPinchRotation	+= event.getRotationDelta();

    return false;
}

bool KeplerApp::onPinchEnded( PinchEvent event )
{
    logEvent("Pinch Ended");

	if( mPinchPer > mPinchPerThresh ){
		Node *selected = mState.getSelectedNode();
		if( selected ){
            console() << "backing out using pinch!" << std::endl;
			mState.setSelectedNode( selected->mParentNode );
		}
	}
	
	mTimePinchEnded = getElapsedSeconds();
	mAlphaWheel.setTimePinchEnded( mTimePinchEnded );
    mPinchRays.clear();
	mIsPinching = false;
    return false;
}

bool KeplerApp::keepTouchForPinching( TouchEvent::Touch touch )
{
    return positionTouchesWorld(touch.getPos());
}

bool KeplerApp::positionTouchesWorld( Vec2f screenPos )
{
    Vec2f worldPos = (mInverseOrientationMatrix * Vec3f(screenPos,0)).xy();
    bool aboveUI = worldPos.y < mUiLayer.getPanelYPos();
    bool notTab = !mUiLayer.getPanelTabRect().contains(worldPos);
    return aboveUI && notTab;
}


bool KeplerApp::orientationChanged( OrientationEvent event )
{
    Orientation orientation = event.getInterfaceOrientation();
    setInterfaceOrientation(orientation);

    Orientation prevOrientation = event.getPrevInterfaceOrientation();
    
	if( ! G_USE_GYRO ){
		// Look over there!
		// heinous trickery follows...
		if (mInterfaceOrientation != prevOrientation) {
			if( mTouchVel.length() > 2.0f && !mIsDragging ){        
				int steps = getRotationSteps(prevOrientation,mInterfaceOrientation);
				mTouchVel.rotate( (float)steps * M_PI/2.0f );
			}
		}
		// ... end heinous trickery
	}

    if (prevOrientation != orientation) {
        std::map<string, string> params;
        params["Device Orientation"] = getOrientationString(event.getDeviceOrientation());
        logEvent("Orientation Changed", params);    
    }

    return false;
}

void KeplerApp::setInterfaceOrientation( const Orientation &orientation )
{
    mInterfaceOrientation = orientation;

    mOrientationMatrix = getOrientationMatrix44( mInterfaceOrientation, getWindowSize() );
    mInverseOrientationMatrix = mOrientationMatrix.inverted();
    
    if( ! G_USE_GYRO )  mUp = getUpVectorForOrientation( mInterfaceOrientation );
	else				mUp = Vec3f::yAxis();

    mLoadingScreen.setInterfaceOrientation(orientation);
    if (mData.getState() == Data::LoadStateComplete) {
        mPlayControls.setInterfaceOrientation(orientation);
        mHelpLayer.setInterfaceOrientation(orientation);
        mUiLayer.setInterfaceOrientation(orientation);
        mAlphaWheel.setInterfaceOrientation(orientation);
        mNotificationOverlay.setInterfaceOrientation(orientation);
    }
}

bool KeplerApp::onWheelToggled( AlphaWheel *alphaWheel )
{
	std::cout << "Wheel Toggled" << std::endl;
    
    const bool showWheel = mAlphaWheel.getShowWheel();
    
	if( showWheel ){
		mFovDest = G_MAX_FOV;
	} else {
		G_HELP = false; 
        
		mFovDest = G_DEFAULT_FOV;
	}
    
    mPlayControls.setAlphaWheelVisible( showWheel );
    
	return false;
}

bool KeplerApp::onAlphaCharSelected( AlphaWheel *alphaWheel )
{
	mState.setAlphaChar( alphaWheel->getAlphaChar() );
	
	stringstream s;
	s.str("");
	s << "FILTERING ARTISTS BY '";
	s << alphaWheel->getAlphaChar();
	s << "'";

	mNotificationOverlay.show( mOverlayIconsTex, Area( 768.0f, 0.0f, 896.0f, 128.0f ), s.str() );
	return false;
}

bool KeplerApp::onAlphaCharStateChanged( State *state )
{
    if (mState.getAlphaChar() != ' ') {
        mWorld.setFilter( LetterFilter(mState.getAlphaChar()) );
        mState.setFilterMode( State::FilterModeAlphaChar );

        mState.setSelectedNode( NULL );
        mPlayControls.setPlaylist( "" ); // FIXME: should be setPlaylistActive(true/false)

        std::map<string, string> params;
        params["Letter"] = toString( mState.getAlphaChar() );
        params["Count"] = toString( mWorld.getNumFilteredNodes() );
        logEvent("Letter Selected" , params);
    }    
	return false;
}

bool KeplerApp::onPlaylistStateChanged( State *state )
{
	std::cout << "playlist changed" << std::endl;

    mState.setAlphaChar( ' ' );
    mWorld.setFilter( PlaylistFilter(mState.getPlaylist()) );
    mState.setFilterMode( State::FilterModePlaylist ); // TODO: make this part of Filter?
    mState.setSelectedNode( NULL );
    
	mPlayControls.setPlaylist( mState.getPlaylist()->getPlaylistName() );
	
    std::map<string, string> parameters;
    parameters["Playlist"] = mState.getPlaylist()->getPlaylistName();
    parameters["Count"] = toString( mWorld.getNumFilteredNodes() );    
    logEvent("Playlist Selected" , parameters);	
    
	return false;
}

bool KeplerApp::onSelectedNodeChanged( Node *node )
{
	mTime			= getElapsedSeconds();
	mCenterFrom		= mCenter;
	mCamDistFrom	= mCamDist;	
	mPinchPerFrom	= mPinchPer;
	mPinchTotalFrom	= mPinchTotal;
	mZoomFrom		= G_ZOOM;
	
	mPinchPer		= mPinchPerInit;
	mPinchTotalDest = mPinchTotalInit;

	if( node && node->mGen > G_ZOOM ){
		node->wasTapped();
	}

	if( node != NULL ) {
        if (node->mGen == G_TRACK_LEVEL) {
            // this is probably a bad OOP thing, maybe there's a virtual method on Node to do this?
            NodeTrack* trackNode = dynamic_cast<NodeTrack*>(node);
            if (trackNode) {
                if ( mIpodPlayer.hasPlayingTrack() ){
                    if ( trackNode->getId() != mPlayingTrack->getItemId() ) {
                        mIpodPlayer.play( trackNode->mAlbum, trackNode->mIndex );
                    }
                }
                else {
                    mIpodPlayer.play( trackNode->mAlbum, trackNode->mIndex );			
                }
            }
            else {
                // FIXME: log error?
            }
            logEvent("Track Selected");            
        } 
        else if (node->mGen == G_ARTIST_LEVEL) {
            logEvent("Artist Selected");
        } 
        else if (node->mGen == G_ALBUM_LEVEL) {
            logEvent("Album Selected");
        }
	}
    else {
        logEvent("Selection Cleared");        
    }

	return false;
}

bool KeplerApp::onPlayControlsPlayheadMoved( float dragPer )
{
    // every third frame, because setPlayheadTime is slow
	if ( mIpodPlayer.hasPlayingTrack() ) {
        mIpodPlayer.setPlayheadTime( mCurrentTrackLength * dragPer );
    }
    return false;
}

bool KeplerApp::onPlayControlsButtonPressed( PlayControls::ButtonId button )
{
    switch( button ) {
        
        case PlayControls::PREV_TRACK:
            logEvent("Previous Track Button Selected");            
            mIpodPlayer.skipPrev();
            break;
        
        case PlayControls::PLAY_PAUSE:
            logEvent("Play/Pause Button Selected");            
            if (mCurrentPlayState == ipod::Player::StatePlaying) {
                mIpodPlayer.pause();
				mNotificationOverlay.show( mOverlayIconsTex, Area( 0.0f, 128.0f, 128.0f, 256.0f ), "PAUSED" );
            }
            else {
                mIpodPlayer.play();
				mNotificationOverlay.show( mOverlayIconsTex, Area( 0.0f, 0.0f, 128.0f, 128.0f ), "PLAY" );
            }
            break;
        
        case PlayControls::NEXT_TRACK:
            logEvent("Next Track Button Selected");            
            mIpodPlayer.skipNext();	
            break;
			
		case PlayControls::SHUFFLE:
			if( mIpodPlayer.getShuffleMode() != ipod::Player::ShuffleModeOff ){
				mIpodPlayer.setShuffleMode( ipod::Player::ShuffleModeOff );
				mNotificationOverlay.show( mOverlayIconsTex, Area( 128.0f, 128.0f, 256.0f, 256.0f ), "SHUFFLE OFF" );
			} else {
				mIpodPlayer.setShuffleMode( ipod::Player::ShuffleModeSongs );
				mNotificationOverlay.show( mOverlayIconsTex, Area( 128.0f, 0.0f, 256.0f, 128.0f ), "SHUFFLE ON" );
			}
            mPlayControls.setShuffleVisible( mIpodPlayer.getShuffleMode() != ipod::Player::ShuffleModeOff );
            logEvent("Shuffle Button Selected");    
            break;
			
		case PlayControls::REPEAT:
			if( mIpodPlayer.getRepeatMode() != ipod::Player::RepeatModeAll ){
				mIpodPlayer.setRepeatMode( ipod::Player::RepeatModeNone );
				mNotificationOverlay.show( mOverlayIconsTex, Area( 256.0f, 128.0f, 384.0f, 256.0f ), "REPEAT NONE" );
			} else {
				mIpodPlayer.setRepeatMode( ipod::Player::RepeatModeAll );
				mNotificationOverlay.show( mOverlayIconsTex, Area( 256.0f, 0.0f, 384.0f, 128.0f ), "REPEAT ALL" );
			}
			mPlayControls.setRepeatVisible( mIpodPlayer.getRepeatMode() != ipod::Player::RepeatModeNone );    
            logEvent("Repeat Button Selected");   
            break;
        
        case PlayControls::HELP:
			std::cout << "HELP!!!! IN PLAYCONTROLS!!!!" << std::endl;
			if( G_SHOW_SETTINGS ){
				logEvent("Help Button Selected");            
				G_HELP = !G_HELP;
				if( G_HELP && !mAlphaWheel.getShowWheel() )
					mAlphaWheel.setShowWheel( true );
			}
            mPlayControls.setHelpVisible( G_HELP );
            break;
        
        case PlayControls::DRAW_RINGS:
			if( G_SHOW_SETTINGS ){
				logEvent("Draw Rings Button Selected");            
				G_DRAW_RINGS = !G_DRAW_RINGS;
				if( G_DRAW_RINGS )	mNotificationOverlay.show( mOverlayIconsTex, Area( 512.0f, 0.0f, 640.0f, 128.0f ), "ORBIT LINES" );
				else				mNotificationOverlay.show( mOverlayIconsTex, Area( 512.0f, 128.0f, 640.0f, 256.0f ), "ORBIT LINES" );
			}
            mPlayControls.setOrbitsVisible( G_DRAW_RINGS );            
            break;
        
        case PlayControls::DRAW_TEXT:
			if( G_SHOW_SETTINGS ){
				logEvent("Draw Text Button Selected");            
				G_DRAW_TEXT = !G_DRAW_TEXT;
				if( G_DRAW_TEXT )	mNotificationOverlay.show( mOverlayIconsTex, Area( 640.0f, 0.0f, 768.0f, 128.0f ), "TEXT LABELS" );
				else				mNotificationOverlay.show( mOverlayIconsTex, Area( 640.0f, 128.0f, 768.0f, 256.0f ), "TEXT LABELS" );
			}
            mPlayControls.setLabelsVisible( G_DRAW_TEXT );
            break;
        
		case PlayControls::USE_GYRO:
//			{
//				std::cout << "Starting tile render" << std::endl;
//				gl::TileRender tr( getWindowWidth() * 3, getWindowHeight() * 3, 11, 131 );
//				std::cout << "Tile Renderer initialized" << std::endl;
//				tr.setMatrices( mCam );
//				std::cout << "Matrices set" << std::endl;
//				while( tr.nextTile() ) {
//					std::cout << "While..." << std::endl;
//					draw();
//					std::cout << "Scene drawn" << std::endl;
//				}
//				std::cout << "done while." << std::endl;
//				writeImage( getHomeDirectory() + "tileRenderOutput.png", tr.getSurface() );
//				std::cout << "Image written" << std::endl;
//			}
			
			if( G_SHOW_SETTINGS ){
				logEvent("Use Gyro Button Selected");            
				G_USE_GYRO = !G_USE_GYRO;
				if( G_USE_GYRO )	mNotificationOverlay.show( mOverlayIconsTex, Area( 384.0f, 0.0f, 512.0f, 128.0f ), "GYROSCOPE" );
				else				mNotificationOverlay.show( mOverlayIconsTex, Area( 384.0f, 128.0f, 512.0f, 256.0f ), "GYROSCOPE" );
			}
            mPlayControls.setGyroVisible( G_USE_GYRO );
            break;
			
		case PlayControls::GOTO_GALAXY:
            logEvent("Galaxy Button Selected");
			mState.setSelectedNode( NULL );
            break;
			
        case PlayControls::GOTO_CURRENT_TRACK:
            logEvent("Current Track Button Selected");            
            // pretend the track just got changed again, this will select it:
            onPlayerTrackChanged( &mIpodPlayer );
            break;
			
		case PlayControls::SETTINGS:
            logEvent("Settings Button Selected");            
            G_SHOW_SETTINGS = !G_SHOW_SETTINGS;
            mPlayControls.setShowSettings( G_SHOW_SETTINGS );            
            break;

        case PlayControls::SLIDER:
        case PlayControls::PARAMSLIDER1:
        case PlayControls::PARAMSLIDER2:
            // TODO: Flurry log?
            break;
        
		case PlayControls::DEBUG_FEATURE:
			G_DEBUG = !G_DEBUG;
			if( G_DEBUG )	mNotificationOverlay.show( mOverlayIconsTex, Area( 768.0f, 0.0f, 896.0f, 128.0f ), "DEBUG MODE" );
			else			mNotificationOverlay.show( mOverlayIconsTex, Area( 768.0f, 128.0f, 896.0f, 256.0f ), "DEBUG MODE" );
            mPlayControls.setDebugVisible( G_DEBUG );
            break;
			
		case PlayControls::NEXT_PLAYLIST:
			mPlaylistIndex++;
            if (mPlaylistIndex > mData.mPlaylists.size() - 1) {
                mPlaylistIndex = 0;
            }
            mState.setPlaylist( mData.mPlaylists[ mPlaylistIndex ] );
            break;
			
		case PlayControls::PREV_PLAYLIST:
            mPlaylistIndex--;
            if (mPlaylistIndex < 0) {
                mPlaylistIndex = mData.mPlaylists.size() -1;
            }
            mState.setPlaylist( mData.mPlaylists[ mPlaylistIndex ] );
            break;	
			
		case PlayControls::SHOW_WHEEL:
			mAlphaWheel.setShowWheel( !mAlphaWheel.getShowWheel() );
            break;	
			
        case PlayControls::NO_BUTTON:
            //console() << "unknown button pressed!" << std::endl;
            break;

	} // switch

	return false;
}
 
void KeplerApp::checkForNodeTouch( const Ray &ray, const Vec2f &pos )
{
	vector<Node*> nodes;
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
				if( ! mState.getSelectedArtistNode() ) {
                    console() << "setting artist node selection" << std::endl;                                        
                    logEvent("Artist Node Touched");                    
					mState.setSelectedNode( nodeWithHighestGen );
                }
//                else {
//                    console() << "ignoring artist node selection" << std::endl;                                        
//                }
			} 
            else if ( highestGen == G_TRACK_LEVEL ){
                logEvent("Track Node Touched");
				if( nodeWithHighestGen != mState.getSelectedNode() ) {                    
                    mState.setSelectedNode( nodeWithHighestGen );
                }
                else {
                    // if this is already the selected node, just toggle the play state
                    if (mCurrentPlayState == ipod::Player::StatePlaying) {
                        mIpodPlayer.pause();
                    }
                    else if (mCurrentPlayState == ipod::Player::StatePaused) {
                        mIpodPlayer.play();
                    }                    
                }
                
            }
            else {
                logEvent("Album Node Touched");
                //console() << "setting node selection" << std::endl;
                mState.setSelectedNode( nodeWithHighestGen );
			}
		}
        else {
            // TODO: shouldn't be possible... confirm!
        }        
	}
//    else {
//        console() << "no nodes hit by touches" << std::endl;
//    }
}

void KeplerApp::update()
{
    if (mData.getState() == Data::LoadStatePending) {
        mData.update(); // processes pending nodes
		mWorld.initNodes( mData.mArtists, mFont, mFontMediTiny, mHighResSurfaces, mLowResSurfaces, mNoAlbumArtSurface );
		mLoadingScreen.setEnabled( false );
		mUiLayer.setIsPanelOpen( true );
        onSelectedNodeChanged( NULL );
        // and then make sure we know about the current track if there is one
        if ( mCurrentPlayState == ipod::Player::StatePlaying ) {
            logEvent("Startup with Track Playing");                        
            onPlayerTrackChanged( &mIpodPlayer );
        } else {
            logEvent("Startup without Track Playing");
			mAlphaWheel.setShowWheel( true );
		}
	}
    
    if ( mRemainingSetupCalled )
	{
		if( G_IS_IPAD2 && G_USE_GYRO ) {
			mGyroHelper.update();
        }

        updateArcball();

        const int elapsedFrames = getElapsedFrames();
        
        // fake playhead time if we're dragging (so it looks really snappy)
        if (mPlayControls.playheadIsDragging()) {
            mCurrentTrackPlayheadTime = mCurrentTrackLength * mPlayControls.getPlayheadValue();
        }
        else if (elapsedFrames % 30) {
            mCurrentTrackPlayheadTime = mIpodPlayer.getPlayheadTime();
        }

		if( mWorld.mPlayingTrackNode && G_ZOOM > G_ARTIST_LEVEL ){
			mWorld.mPlayingTrackNode->updateAudioData( mCurrentTrackPlayheadTime );
		}
		
		const float scaleSlider = mPlayControls.getParamSlider1Value();
		const float speedSlider = mPlayControls.getParamSlider2Value();
        mWorld.update( 0.25f + scaleSlider * 2.0f, speedSlider * 0.075f );
		
        updateCamera();
        
        Vec3f bbRight, bbUp;
        mCam.getBillboardVectors( &bbRight, &bbUp );        
        
        mWorld.updateGraphics( mCam, bbRight, bbUp, mFadeInAlphaToArtist );

        mGalaxy.update( mEye, mFadeInAlphaToArtist, getElapsedSeconds(), bbRight, bbUp );
		
		Node *selectedArtistNode = mState.getSelectedArtistNode();
		if( selectedArtistNode ){
			mParticleController.update( mCenter, selectedArtistNode->mRadius * 0.15f, bbRight, bbUp );
			float per = selectedArtistNode->mEclipseStrength * 0.5f + 0.25f;
			mParticleController.buildParticleVertexArray( scaleSlider * 5.0f, 
														  selectedArtistNode->mColor, 
														  ( sin( per * M_PI ) * sin( per * 0.25f ) * 0.75f ) + 0.25f );
			mParticleController.buildDustVertexArray( scaleSlider, selectedArtistNode, mPinchAlphaPer, ( 1.0f - mCamRingAlpha ) * 0.15f * mFadeInArtistToAlbum );
		}
		
		mNotificationOverlay.update();
		
        mUiLayer.setShowSettings( G_SHOW_SETTINGS );
        mUiLayer.update();
        
		mHelpLayer.update();
		mAlphaWheel.update( mFov );
        
        mPlayControls.update();

        if (elapsedFrames % 30 == 0) {
            mPlayControls.setElapsedSeconds( (int)mCurrentTrackPlayheadTime );
            mPlayControls.setRemainingSeconds( -(int)(mCurrentTrackLength - mCurrentTrackPlayheadTime) );
        }
        if (!mPlayControls.playheadIsDragging()) {
            mPlayControls.setPlayheadProgress( mCurrentTrackPlayheadTime / mCurrentTrackLength );
        }
                
        if( G_DEBUG && elapsedFrames % 30 == 0 ){
            mStats.update(getAverageFps(), mCurrentTrackPlayheadTime, mFov, G_CURRENT_LEVEL, G_ZOOM);
        }
        
    }
    else {
        // make sure we've drawn the loading screen first
        if (getElapsedFrames() > 1) {
            remainingSetup();
        }        
    }
    
    mNotificationOverlay.update();
}

void KeplerApp::updateArcball()
{	
	if( mTouchVel.length() > 2.0f && !mIsDragging ){
        Vec3f downPos;
		if( G_USE_GYRO )	downPos = ( Vec3f(mTouchPos,0) );
		else				downPos = mInverseOrientationMatrix * ( Vec3f(mTouchPos,0) );
        mArcball.mouseDown( Vec2i(downPos.x, downPos.y) );
		
		Vec3f dragPos;
		if( G_USE_GYRO )	dragPos = ( Vec3f(mTouchPos + mTouchVel,0) );
		else				dragPos = mInverseOrientationMatrix * ( Vec3f(mTouchPos + mTouchVel,0) );
        mArcball.mouseDrag( Vec2i(dragPos.x, dragPos.y) );        
	}	
}


void KeplerApp::updateCamera()
{	
	mPinchTotal -= ( mPinchTotal - mPinchTotalDest ) * 0.4f;
	mPinchPer = ( mPinchTotal - mPinchScaleMin )/( mPinchScaleMax - mPinchScaleMin );
	mPinchHighlightRadius -= ( mPinchHighlightRadius - 200.0f ) * 0.4f;
	
	float cameraDistMulti = mPinchPer * 2.0f + 0.5f;
	
// IF THE PINCH IS PAST THE POP THRESHOLD...
	if( mPinchPer > mPinchPerThresh ){
		if( ! mIsPastPinchThresh ) mPinchHighlightRadius = 650.0f;
		mPinchAlphaPer -= ( mPinchAlphaPer ) * 0.1f;
		mIsPastPinchThresh = true;
		
		if( G_CURRENT_LEVEL == G_TRACK_LEVEL )			mFovDest = 70.0f;
		else if( G_CURRENT_LEVEL == G_ALBUM_LEVEL )		mFovDest = 85.0f;
		else if( G_CURRENT_LEVEL == G_ARTIST_LEVEL )	mFovDest = 85.0f;
		else											mFovDest = 95.0f;
		
		mFov -= ( mFov - mFovDest ) * 0.2f;
	
// OTHERWISE...
	} else {
		if( mIsPastPinchThresh ) mPinchHighlightRadius = 125.0f;
		mPinchAlphaPer -= ( mPinchAlphaPer - 1.0f ) * 0.1f;
		mIsPastPinchThresh = false;
		
		mFovDest = G_DEFAULT_FOV;
		mFov -= ( mFov - mFovDest ) * 0.2f;//075f;
	}
    
	
	Node* selectedNode = mState.getSelectedNode();
	if( selectedNode ){
		mCamDistDest	= selectedNode->mIdealCameraDist * cameraDistMulti;
		
		if( selectedNode->mParentNode && mPinchPer > mPinchPerThresh ){
			Vec3f dirToParent = selectedNode->mParentNode->mPos - selectedNode->mPos;
			mCenterOffset -= ( mCenterOffset - ( dirToParent * ( mPinchPer - mPinchPerThresh ) * 2.5f ) ) * 0.2f;
			
		} else {
			mCenterOffset -= ( mCenterOffset - Vec3f::zero() ) * 0.2f;
		}
		mCenterDest		= selectedNode->mPos;
		mZoomDest		= selectedNode->mGen;
		mCenterFrom		+= selectedNode->mVel;
		
	} else {
		mCamDistDest	= G_INIT_CAM_DIST * cameraDistMulti;
		mCenterDest		= Vec3f::zero();
        
		if( mState.getFilterMode() == State::FilterModeUndefined ){
            mZoomDest = G_HOME_LEVEL;
        } else {
			mZoomDest = G_ALPHA_LEVEL;
		}
        //		mZoomDest		= G_HOME_LEVEL;
        //		if( mState.getAlphaChar() != ' ' ){
        //			mZoomDest	= G_ALPHA_LEVEL;
        //		}        
		
		mCenterOffset -= ( mCenterOffset - Vec3f::zero() ) * 0.05f;
	}
	
	G_CURRENT_LEVEL = mZoomDest;
	
	
	if( mIsPinching && G_CURRENT_LEVEL <= G_ALPHA_LEVEL ){
		if( mPinchPer > mPinchPerThresh && ! mAlphaWheel.getShowWheel() ){
			mAlphaWheel.setShowWheel( true ); 
			std::cout << "updateCamera opened alphawheel" << std::endl;
			
		} else if( mPinchPer <= mPinchPerThresh && mAlphaWheel.getShowWheel() ){
			mAlphaWheel.setShowWheel( false ); 
			std::cout << "updateCamera closed alphawheel" << std::endl;
		}
	}
	
	
	

	float distToTravel = mState.getDistBetweenNodes();
	double duration = 3.0f;
	if( distToTravel < 1.0f )		duration = 2.0;
	else if( distToTravel < 5.0f )	duration = 2.75f;
//	double duration = 2.0f;
	double p		= constrain( getElapsedSeconds()-mTime, 0.0, duration );
	
	mCenter			= easeInOutCubic( p, mCenterFrom, (mCenterDest + mCenterOffset) - mCenterFrom, duration );
//	mCenter			= easeInOutCubic( p, mCenterFrom, mCenterDest - mCenterFrom, duration );
	mCamDist		= easeInOutCubic( p, mCamDistFrom, mCamDistDest - mCamDistFrom, duration );
	mCamDist		= min( mCamDist, G_INIT_CAM_DIST );
	mCamDistAnim	= easeInOutCubic( p, 0.0f, M_PI, duration );
	//mPinchPer		= easeInOutCubic( p, mPinchPerFrom, 0.5f, duration );
	//mPinchTotalDest	= easeInOutCubic( p, mPinchTotalFrom, mPinchTotalInit, duration );
	G_ZOOM			= easeInOutCubic( p, mZoomFrom, mZoomDest - mZoomFrom, duration );

	
	mFadeInAlphaToArtist	= constrain( G_ZOOM - G_ALPHA_LEVEL, 0.0f, 1.0f );
	mFadeInArtistToAlbum	= constrain( G_ZOOM - G_ARTIST_LEVEL, 0.0f, 1.0f );
	mFadeInAlbumToTrack		= constrain( G_ZOOM - G_ALBUM_LEVEL, 0.0f, 1.0f );
	
    // apply the Arcball to the camera eye/up vectors
    // (instead of to the whole scene)
    Quatf q = mArcball.getQuat();
    q.w *= -1.0; // reverse the angle, keep the axis
	
    // TODO/FIXME/ROBERT: Robert test this?
	if( G_IS_IPAD2 && G_USE_GYRO ){
		q = mGyroHelper.getQuat();
	}	
    
    Vec3f camOffset = q * Vec3f( 0, 0, mCamDist);
    mEye = mCenter - camOffset;

	mCam.setPerspective( mFov, getWindowAspectRatio(), 0.001f, 2000.0f );
	mCam.lookAt( mEye - mCenterOffset, mCenter, q * mUp );
}

void KeplerApp::draw()
{
	gl::clear( Color( 0, 0, 0 ), true );
	if( mData.getState() != Data::LoadStateComplete ){
		mLoadingScreen.draw( mStarGlowTex );
	} else if( mData.mArtists.size() == 0 ){
		drawNoArtists();
	} else {
		drawScene();
	}
    mNotificationOverlay.draw();
    
    const GLenum discards[]  = {GL_DEPTH_ATTACHMENT_OES};
//    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glDiscardFramebufferEXT(GL_FRAMEBUFFER_OES,1,discards);
}

void KeplerApp::drawNoArtists()
{
    if( !mNoArtistsTex ) {
        mNoArtistsTex = gl::Texture( loadImage( loadResource( "noArtists.png" ) ) );
    }
    
	gl::setMatricesWindow( getWindowSize() );    
	
    glPushMatrix();
    glMultMatrixf( mOrientationMatrix );
	Vec2f interfaceSize = getWindowSize();
	if( isLandscapeOrientation( mInterfaceOrientation ) ){
		interfaceSize = interfaceSize.yx();
	}
    Vec2f center = interfaceSize * 0.5f;
    gl::color( Color::white() );
	
	mNoArtistsTex.enableAndBind();
	Vec2f v1( center - mNoArtistsTex.getSize() * 0.5f );
	Vec2f v2( v1 + mNoArtistsTex.getSize() );
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	gl::drawSolidRect( Rectf( v1, v2 ) );
	mNoArtistsTex.disable();
}



void KeplerApp::drawScene()
{	
	vector<Node*> unsortedNodes = mWorld.getUnsortedNodes( G_ALBUM_LEVEL, G_TRACK_LEVEL );
	Node *artistNode = mState.getSelectedArtistNode();
	if( artistNode ){
		unsortedNodes.push_back( artistNode );
	}
	vector<Node*> sortedNodes = mWorld.sortNodes( unsortedNodes );	
		
    gl::enableDepthWrite();
    gl::setMatrices( mCam );
    
// SKYDOME
    Color c = Color( CM_HSV, mPinchPer * 0.2f + 0.475f, 1.0f - mPinchPer * 0.5f, 1.0f );
    if( mIsPastPinchThresh )
        c = Color( CM_HSV, mPinchPer * 0.3f + 0.7f, 1.0f, 1.0f );
    
    if( artistNode && artistNode->mDistFromCamZAxis > 0.0f ){
        float distToCenter = ( getWindowCenter() - artistNode->mScreenPos ).length();
        gl::color( lerp( ( artistNode->mGlowColor + BRIGHT_BLUE ) * 0.5f, BRIGHT_BLUE, min( distToCenter / 300.0f, 1.0f ) ) );
    } else {
        gl::color( BRIGHT_BLUE );
    }
  //  gl::color( c * pow( 1.0f - zoomOff, 3.0f ) );
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    mSkyDome.enableAndBind();
    glPushMatrix();
    gl::scale( Vec3f(G_SKYDOME_RADIUS,G_SKYDOME_RADIUS,G_SKYDOME_RADIUS) );
    mSkySphere.draw();
    glPopMatrix();
    mSkyDome.disable();
    glDisable(GL_CULL_FACE);
    
// GALAXY
    mGalaxy.drawLightMatter();	
    mGalaxy.drawSpiralPlanes();    
	mGalaxy.drawCenter();	
	
// STARS
	gl::enableAdditiveBlending();
	mStarTex.enableAndBind();
	mWorld.drawStarsVertexArray();
	mStarTex.disable();
	
// STARGLOWS bloom (TOUCH HIGHLIGHTS)
	mEclipseGlowTex.enableAndBind();
	mWorld.drawTouchHighlights( mFadeInArtistToAlbum );
	mEclipseGlowTex.disable();
	
// STARGLOWS bloom
	mStarGlowTex.enableAndBind();
	mWorld.drawStarGlowsVertexArray();
	mStarGlowTex.disable();

	if( artistNode ){ // defined at top of method
		Vec2f interfaceSize = getWindowSize();
//		if ( isLandscapeOrientation( mInterfaceOrientation ) ) {
//			interfaceSize = interfaceSize.yx(); // swizzle it!
//		}
		
		//float zoomOffset = constrain( 1.0f - ( G_ALBUM_LEVEL - G_ZOOM ), 0.0f, 1.0f );
		mCamRingAlpha = constrain( abs( mEye.y - artistNode->mPos.y ), 0.0f, 1.0f ); // WAS 0.6f

		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );
		glEnable( GL_COLOR_MATERIAL );
		glEnable( GL_RESCALE_NORMAL );
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, ColorA( 0.0f, 0.0f, 0.0f, 1.0f ) );
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, ColorA( Color::white(), 1.0f ) );

        // LIGHT FROM ARTIST
        glEnable( GL_LIGHT0 );
        glEnable( GL_LIGHT1 );
        Vec3f lightPos          = artistNode->mPos;
        GLfloat artistLight[]	= { lightPos.x, lightPos.y, lightPos.z, 1.0f };
        glLightfv( GL_LIGHT0, GL_POSITION, artistLight );
        glLightfv( GL_LIGHT0, GL_DIFFUSE, ColorA( artistNode->mColor, 1.0f ) );
        glLightfv( GL_LIGHT1, GL_POSITION, artistLight );
        glLightfv( GL_LIGHT1, GL_DIFFUSE, ColorA( BRIGHT_BLUE, 1.0f ) );
        
		for( int i = 0; i < sortedNodes.size(); i++ ){
            
			if( (G_IS_IPAD2 || G_DEBUG) && sortedNodes[i]->mGen == G_ALBUM_LEVEL ){
				gl::enableAlphaBlending();
				glDisable( GL_CULL_FACE );
				mEclipseShadowTex.enableAndBind();
				sortedNodes[i]->findShadows( pow( mCamRingAlpha, 1.2f ) );
				glEnable( GL_CULL_FACE );

				//gl::enableDepthWrite();
			}
			
			gl::enableDepthRead();
			glEnable( GL_LIGHTING );

			gl::disableAlphaBlending(); // dings additive blending            
			gl::enableAlphaBlending();  // restores alpha blending
            			
			sortedNodes[i]->drawPlanet( mStarCoreTex ); // star core tex for artistars, planets do their own thing
			sortedNodes[i]->drawClouds( mCloudsTex );

			glDisable( GL_LIGHTING );
			gl::disableDepthRead();
			
			if( sortedNodes[i]->mGen == G_ARTIST_LEVEL ){
				//gl::enableAlphaBlending();
				//sortedNodes[i]->drawAtmosphere( mEye - mCenterOffset, interfaceSize * 0.5f, mAtmosphereSunTex, mAtmosphereDirectionalTex, mPinchAlphaPer );	
			} else {
				gl::enableAdditiveBlending();
				sortedNodes[i]->drawAtmosphere( mEye - mCenterOffset, interfaceSize * 0.5f, mAtmosphereTex, mAtmosphereDirectionalTex, mPinchAlphaPer );
			}
			
		}
        
		glDisable( GL_CULL_FACE );
		glDisable( GL_RESCALE_NORMAL );
		
		gl::enableAdditiveBlending();
		artistNode->drawExtraGlow( mStarGlowTex );
	}

	glDisable( GL_LIGHTING );
    
	gl::enableDepthRead();	
	gl::disableDepthWrite();
	
	
// ORBITS
	if( G_DRAW_RINGS ){
        mOrbitRingGradientTex.enableAndBind();
        mWorld.drawOrbitRings( mPinchAlphaPer, sqrt( mCamRingAlpha ) );
        mOrbitRingGradientTex.disable();
	}
	
// PARTICLES
	if( artistNode ){
		mParticleTex.enableAndBind();
		mParticleController.drawParticleVertexArray( artistNode );
		mParticleTex.disable();
	}
	
// RINGS
	if( artistNode ){
		//alpha = pow( mCamRingAlpha, 2.0f );
        // TODO: consider only doing this on planets in our sorted loop, above
		mWorld.drawRings( mRingsTex, mCamRingAlpha * 0.5f );
	}
	
// FOR dust, playhead progress, constellations, etc.
    gl::enableAlphaBlending();    
    gl::enableAdditiveBlending();    
	
// DUSTS
	if( artistNode && (G_IS_IPAD2 || G_DEBUG) ){
		mParticleController.drawDustVertexArray( artistNode );
	}
	
// PLAYHEAD PROGRESS
	if( G_DRAW_RINGS && mWorld.mPlayingTrackNode && G_ZOOM > G_ARTIST_LEVEL ){
        const bool paused = mCurrentPlayState == ipod::Player::StatePaused;
        const float pauseAlpha = paused ? sin(getElapsedSeconds() * M_PI * 2.0f ) * 0.25f + 0.75f : 1.0f;
        mWorld.mPlayingTrackNode->drawPlayheadProgress( mPinchAlphaPer, mCamRingAlpha, pauseAlpha, mPlayheadProgressTex, mTrackOriginTex );
	}
	
// CONSTELLATION
	if( mWorld.getNumFilteredNodes() > 1 && G_DRAW_RINGS ){
		mDottedTex.enableAndBind();
		mWorld.drawConstellation();
		mDottedTex.disable();
	}
	
// GALAXY DARK MATTER:
    if (G_IS_IPAD2 || G_DEBUG) {
		gl::enableAlphaBlending();    
        mGalaxy.drawDarkMatter();
    }
	
// NAMES
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::setMatricesWindow( getWindowSize() ); 
    gl::enableAdditiveBlending();
	
    if( G_DRAW_TEXT ){
		mWorld.drawNames( mCam, mPinchAlphaPer, getAngleForOrientation(mInterfaceOrientation) );
	}
		
// LENSFLARE?!?!  SURELY YOU MUST BE MAD.
	if( (G_IS_IPAD2 || G_DEBUG) && artistNode && artistNode->mDistFromCamZAxis > 0.0f ){
		int numFlares  = 5;
		float radii[7] = { 0.8f, 1.2f, 4.5f, 8.0f, 6.0f };
		float dists[7] = { 0.8f, 1.0f, 1.5f, 1.70f, 2.0f };
		
		float distFromCenter = artistNode->mScreenPos.distance( getWindowCenter() );
		float distPer = constrain( 1.0f - distFromCenter/400.0f, 0.0f, 1.0f );
		float alpha = distPer * 0.2f * sin( distPer * M_PI );

        gl::enableAlphaBlending();    
        gl::enableAdditiveBlending();		
		mLensFlareTex.enableAndBind();
		
		Vec2f flarePos = getWindowCenter() - artistNode->mScreenPos;
		float flareDist = flarePos.length();
		Vec2f flarePosNorm = flarePos.normalized();
		
		for( int i=0; i<numFlares; i++ ){
			gl::color( ColorA( BRIGHT_BLUE, alpha ) );
			flarePos = getWindowCenter() + flarePosNorm * dists[i] * dists[i] * flareDist;
			float flareRadius = artistNode->mSphereScreenRadius * radii[i] * distPer;
			gl::drawSolidRect( Rectf( flarePos.x - flareRadius, flarePos.y - flareRadius, flarePos.x + flareRadius, flarePos.y + flareRadius ) );
		}
		
		mLensFlareTex.disable();
	}

	
// PINCH FINGER POSITIONS
	if( mIsPinching ){

		float radius = mPinchHighlightRadius;
		float alpha = mPinchPer > mPinchPerThresh ? 0.2f : 1.0f;
        mStarGlowTex.enableAndBind();					  
		gl::color( ColorA( c, max( mPinchPer - mPinchPerInit, 0.0f ) * alpha ) );
		
		for( vector<Vec2f>::iterator it = mPinchPositions.begin(); it != mPinchPositions.end(); ++it ){
			gl::drawSolidRect( Rectf( it->x - radius, it->y - radius, it->x + radius, it->y + radius ) );
		}
	} else if( mIsTouching ){
		float radius = 100.0f;
		float alpha = 0.5f;
		mStarGlowTex.enableAndBind();
		gl::color( ColorA( BLUE, alpha ) );
		
		gl::drawSolidRect( Rectf( mTouchPos.x - radius, mTouchPos.y - radius, mTouchPos.x + radius, mTouchPos.y + radius ) );
	}

//    if (G_DEBUG) {
//        mWorld.drawHitAreas();
//    }

	gl::disableAlphaBlending(); // stops additive blending
    gl::enableAlphaBlending();  // reinstates normal alpha blending
	
// EVERYTHING ELSE	
	mAlphaWheel.draw( mData.mNormalizedArtistsPerChar );
	mHelpLayer.draw( mUiButtonsTex, mUiLayer.getPanelYPos() );
    mUiLayer.draw( mUiButtonsTex );
    mPlayControls.draw( mUiLayer.getPanelYPos() );

    gl::enableAdditiveBlending();    
	mNotificationOverlay.draw();
	
	if( G_DEBUG ){
        //gl::setMatricesWindow( getWindowSize() );
        mStats.draw( mOrientationMatrix );
	}
}

bool KeplerApp::onPlayerLibraryChanged( ipod::Player *player )
{	
    // RESET:
	mLoadingScreen.setEnabled( true );
    mState.setup();    
    mData.setup();
	mWorld.setup();
    logEvent("Player Library Changed");
    return false;
}

bool KeplerApp::onPlayerTrackChanged( ipod::Player *player )
{	
    mPlayControls.setLastTrackChangeTime( getElapsedSeconds() );
    
	if (mIpodPlayer.hasPlayingTrack()) {
        
        ipod::TrackRef newTrack = mIpodPlayer.getPlayingTrack();

        uint64_t trackId = newTrack->getItemId();
        
        if (mPlayingTrack && trackId == mPlayingTrack->getItemId() && mWorld.mPlayingTrackNode && mWorld.mPlayingTrackNode->getId() == trackId) {
            //std::cout << "skipping needless onPlayerTrackChange" << std::endl;
            return false;
        }

        mPlayingTrack = newTrack;

        mCurrentTrackLength = mPlayingTrack->getLength();
        
        string artistName = mPlayingTrack->getArtist();
        
        mPlayControls.setCurrentTrack( " " + artistName 
                                      + " • " + mPlayingTrack->getAlbumTitle() 
                                      + " • " + mPlayingTrack->getTitle() + " " );

        // make doubly-sure we're focused on the correct letter
        // FIXME: only if the filter mode is alpha
        mState.setAlphaChar( artistName );
        
        uint64_t artistId = mPlayingTrack->getArtistId();
        uint64_t albumId = mPlayingTrack->getAlbumId();
        
        // first be sure to create nodes for artist, album and track:
        mWorld.selectHierarchy( artistId, albumId, trackId );

        // and finally tell other things that care about the current selection
        Node* currentSelection = mState.getSelectedNode();
        if (currentSelection == NULL || currentSelection->getId() != trackId) {
            mState.setSelectedNode( mWorld.getTrackNodeById( artistId, albumId, trackId ) );
        }

        // then sync the mIsPlaying state for all nodes...
        mWorld.updateIsPlaying( artistId, albumId, trackId );
	}
	else {
        
        mPlayingTrack.reset();
        
        mCurrentTrackLength = 0;
        
        mPlayControls.setCurrentTrack("");
		// go to album level view when the last track ends:
		mState.setSelectedNode( mState.getSelectedAlbumNode() );
        // FIXME: disable play button and zoom-to-current-track button
        
        // this should be OK to do since the above will happen if something is queued and paused
        mWorld.updateIsPlaying( 0, 0, 0 );        
	}
        
    logEvent("Player Track Changed");
    
    return false;
}

bool KeplerApp::onPlayerStateChanged( ipod::Player *player )
{	
    mCurrentPlayState = mIpodPlayer.getPlayState();

    mPlayControls.setPlaying(mCurrentPlayState == ipod::Player::StatePlaying);
    
    if ( mState.getSelectedNode() == NULL ) {
        onPlayerTrackChanged( player );
    }
    else {
        // update mIsPlaying state for all nodes...
        if ( mIpodPlayer.hasPlayingTrack() ){
            mWorld.updateIsPlaying( mPlayingTrack->getArtistId(), mPlayingTrack->getAlbumId(), mPlayingTrack->getItemId() );
        }
        else {
            // this should be OK to do since the above will happen if something is queued and paused
            mWorld.updateIsPlaying( 0, 0, 0 );
        }
    }
    
    std::map<string, string> params;
    params["State"] = mIpodPlayer.getPlayStateString();
    logEvent("Player State Changed", params);
    
    return false;
}

void KeplerApp::logEvent(const string &event)
{
    if (G_DEBUG) std::cout << "logging: " << event << std::endl;
    Flurry::getInstrumentation()->logEvent(event);
}
void KeplerApp::logEvent(const string &event, const map<string,string> &params)
{
    if (G_DEBUG) std::cout << "logging: " << event << " with params..." << std::endl;
    Flurry::getInstrumentation()->logEvent(event, params);
}

CINDER_APP_COCOA_TOUCH( KeplerApp, RendererGl )
