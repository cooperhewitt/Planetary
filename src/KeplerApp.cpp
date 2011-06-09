#include <sys/sysctl.h>
#include <vector>
#include <sstream>
#import <CoreMotion/CoreMotion.h>
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
#include "OrientationHelper.h"
#include "Globals.h"
#include "Easing.h"
#include "World.h"
#include "HelpLayer.h"
#include "UiLayer.h"
#include "NotificationOverlay.h"
#include "AlphaWheel.h"
#include "State.h"
#include "Data.h"
#include "PlayControls.h"
#include "CinderIPod.h"
#include "CinderIPodPlayer.h"
#include "PinchRecognizer.h"
#include "ParticleController.h"
#include "LoadingScreen.h"
#include "NodeArtist.h"
#include "PlaylistFilter.h"
#include "LetterFilter.h"
#include "CinderFlurry.h"

using std::vector;
using namespace ci;
using namespace ci::app;
using namespace std;
using std::stringstream;
using namespace pollen::flurry;

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

GLfloat mat_diffuse[]	= { 1.0, 1.0, 1.0, 1.0 };

float easeInOutQuad( double t, float b, float c, double d );
Vec3f easeInOutQuad( double t, Vec3f b, Vec3f c, double d );



class KeplerApp : public AppCocoaTouch {
  public:
    
	virtual void	setup();
    void            prepareSettings(Settings *settings);
    void            remainingSetup();
	void			initLoadingTextures();
	void			initTextures();
	void			initGalaxyVertexArray();
	void			initDarkMatterVertexArray();
	virtual void	touchesBegan( TouchEvent event );
	virtual void	touchesMoved( TouchEvent event );
	virtual void	touchesEnded( TouchEvent event );
	bool            orientationChanged( OrientationEvent event );
    void            setInterfaceOrientation( const Orientation &orientation );
	virtual void	update();
	void			updateGyro();
	void			updateArcball();
	void			updateCamera();
	virtual void	draw();
	void			drawNoArtists();
    void            drawScene();
	void			drawInfoPanel();
	void			setParamsTex();
	virtual void    shutdown();
	bool			onAlphaCharStateChanged( State *state );
	bool			onPlaylistStateChanged( State *state );
	bool			onAlphaCharSelected( AlphaWheel *alphaWheel );
	bool			onWheelToggled( AlphaWheel *alphaWheel );
	bool			onPlayControlsButtonPressed ( PlayControls::ButtonId button );
	bool			onPlayControlsPlayheadMoved ( float amount );
	bool			onSelectedNodeChanged( Node *node );
	void			checkForNodeTouch( const Ray &ray, Matrix44f &mat, const Vec2f &pos );
	bool			onPlayerStateChanged( ipod::Player *player );
    bool			onPlayerTrackChanged( ipod::Player *player );
    bool			onPlayerLibraryChanged( ipod::Player *player );
    void            updateIsPlaying();
    Node*           getPlayingTrackNode( ipod::TrackRef playingTrack, Node* albumNode );
    Node*           getPlayingAlbumNode( ipod::TrackRef playingTrack, Node* artistNode );
    Node*           getPlayingArtistNode( ipod::TrackRef playingTrack );
	
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
    
    std::map<Orientation, std::map<Orientation, int> > mRotationSteps;

	Quatf			mGyroQuat;
	// Objective C (FIXME: move Gyro stuff to something that looks like OrientationHelper)
    CMMotionManager *motionManager;
    CMAttitude *referenceAttitude;

// AUDIO
	ipod::Player		mIpodPlayer;
	ipod::PlaylistRef	mCurrentAlbum;
	int					mPlaylistIndex; // FIXME: move this into State
	
// PLAY CONTROLS
	PlayControls	mPlayControls;
	float			mScaleSlider;
	float			mSpeedSlider;
	
// CAMERA PERSP
	CameraPersp		mCam;
	float			mFov, mFovDest;
	Vec3f			mEye, mCenter, mUp;
	Vec3f			mCamVel;
	Vec3f			mCenterDest, mCenterFrom;
	Vec3f			mCenterOffset;		// if pinch threshold is exceeded, look towards parent?
	Vec3f			mCamNormal;
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
	Matrix44f		mMatrix;
	Vec3f			mBbRight, mBbUp;
	float			mCamRingAlpha; // 1.0 = camera is viewing rings side-on
								   // 0.0 = camera is viewing rings from above or below
	float			mFadeInAlphaToArtist;
	float			mFadeInArtistToAlbum;
	float			mFadeInAlbumToTrack;
	float			mFadeOverFullZoomDuration;
	
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
	
	// TEXTURES
//    TextureLoader   mTextureLoader;
	gl::Texture		mParamsTex;
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
	
// GALAXY
	GLfloat			*mGalaxyVerts;
	GLfloat			*mGalaxyTexCoords;
	GLfloat			*mDarkMatterVerts;
	GLfloat			*mDarkMatterTexCoords;
	int				mDarkMatterCylinderRes;
	float			mLightMatterBaseRadius;
	float			mDarkMatterBaseRadius;
	
	float			mTime;
	bool			mHasNoArtists;
	bool			mDataIsLoaded;
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
    
    // http://stackoverflow.com/questions/448162/determine-device-iphone-ipod-touch-with-iphone-sdk/1561920#1561920
    // http://www.clintharris.net/2009/iphone-model-via-sysctlbyname/
    size_t size;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);  
    char *machine = new char[size];
    sysctlbyname("hw.machine", machine, &size, NULL, 0);    
    G_IS_IPAD2 = (strcmp("iPad1,1",machine) != 0);
    console() << "G_IS_IPAD2: " << G_IS_IPAD2 << endl;
    delete[] machine;

	if( G_IS_IPAD2 ){
		G_NUM_PARTICLES = 30;
		G_NUM_DUSTS = 2500;
		motionManager = [[CMMotionManager alloc] init];
		[motionManager startDeviceMotionUpdates];
		
		CMDeviceMotion *dm = motionManager.deviceMotion;
		referenceAttitude = [dm.attitude retain];
	}
	
	mOrientationHelper.setup();
    mOrientationHelper.registerOrientationChanged( this, &KeplerApp::orientationChanged );    
    setInterfaceOrientation( mOrientationHelper.getInterfaceOrientation() );
    
    mRemainingSetupCalled = false;

    // TODO: surely we can store this somewhere else? in OrientationHelper perhaps.
    std::map<Orientation,int> pSteps;
    pSteps[LANDSCAPE_LEFT_ORIENTATION] = 1;
    pSteps[LANDSCAPE_RIGHT_ORIENTATION] = -1;
    pSteps[UPSIDE_DOWN_PORTRAIT_ORIENTATION] = 2;
    std::map<Orientation,int> llSteps;
    llSteps[PORTRAIT_ORIENTATION] = -1;
    llSteps[LANDSCAPE_RIGHT_ORIENTATION] = 2;
    llSteps[UPSIDE_DOWN_PORTRAIT_ORIENTATION] = 1;
    std::map<Orientation,int> lrSteps;
    lrSteps[PORTRAIT_ORIENTATION] = 1;
    lrSteps[LANDSCAPE_LEFT_ORIENTATION] = 2;
    lrSteps[UPSIDE_DOWN_PORTRAIT_ORIENTATION] = -1;
    std::map<Orientation,int> upSteps;
    upSteps[PORTRAIT_ORIENTATION] = 2;
    upSteps[LANDSCAPE_LEFT_ORIENTATION] = -1;
    upSteps[LANDSCAPE_RIGHT_ORIENTATION] = 1;
    mRotationSteps[PORTRAIT_ORIENTATION] = pSteps;
    mRotationSteps[LANDSCAPE_LEFT_ORIENTATION] = llSteps;
    mRotationSteps[LANDSCAPE_RIGHT_ORIENTATION] = lrSteps;
    mRotationSteps[UPSIDE_DOWN_PORTRAIT_ORIENTATION] = upSteps;
    
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

    mDataIsLoaded	= false;
	mLoadingScreen.setEnabled( true );
	G_DRAW_RINGS	= true;
	G_DRAW_TEXT		= true;
	//Rand::randomize();

    // TEXTURES
    initTextures();
	
	
	// ARCBALL
	mMatrix	= Quatf();
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
	mCamNormal			= Vec3f::zero();
	mCenterDest			= mCenter;
	mCenterFrom			= mCenter;
	mCenterOffset		= mCenter;
    // FIXME: let's put this setup stuff back in setup()
    // this was overriding the (correct) value which is now always set by setInterfaceOrientation
//	mUp					= Vec3f::yAxis();
	mFov				= G_DEFAULT_FOV;
	mFovDest			= G_DEFAULT_FOV;
	mCam.setPerspective( mFov, getWindowAspectRatio(), 0.0001f, 1200.0f );
	mBbRight			= Vec3f::xAxis();
	mBbUp				= Vec3f::yAxis();
	mFadeInAlphaToArtist = 0.0f;
	mFadeInArtistToAlbum = 0.0f;
	mFadeInAlbumToTrack = 0.0f;
	mFadeOverFullZoomDuration = 0.0f;
	
	
// FONTS
	mFontLarge			= Font( loadResource( "AauxPro-Black.ttf" ), 26 );
	mFont				= Font( loadResource( "UnitRoundedOT-Medi.otf" ), 14 );
	mFontBig			= Font( loadResource( "AauxPro-Black.ttf"), 24 );
	mFontMediSmall		= Font( loadResource( "UnitRoundedOT-Medi.otf" ), 13 );
	mFontMediTiny		= Font( loadResource( "UnitRoundedOT-Medi.otf" ), 11 );
	
	
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
	mIpodPlayer.registerStateChanged( this, &KeplerApp::onPlayerStateChanged );
    mIpodPlayer.registerTrackChanged( this, &KeplerApp::onPlayerTrackChanged );
    mIpodPlayer.registerLibraryChanged( this, &KeplerApp::onPlayerLibraryChanged );
	mPlaylistIndex = 0;
	
    // DATA
    mData.setup(); // NB:- is asynchronous, see update() for what happens when it's done

    // WORLD
    mWorld.setup( &mData );
	
	mHasNoArtists = false;
	
	mDarkMatterCylinderRes = 48;
	initGalaxyVertexArray();
	initDarkMatterVertexArray();
	mLightMatterBaseRadius = G_INIT_CAM_DIST * 0.75f;
	mDarkMatterBaseRadius = G_INIT_CAM_DIST * 0.86f;

    Flurry::getInstrumentation()->stopTimeEvent("Remaining Setup");

    //console() << "setupEnd: " << getElapsedSeconds() << std::endl;
}

void KeplerApp::initLoadingTextures()
{
    //float t = getElapsedSeconds();
    //console() << "initLoadingTextures, begin: " << t << std::endl;
    // only add textures here if they are *required* for LoadingScreen
    // otherwise add them to initTextures
	
	mStarGlowTex	= loadImage( loadResource( "starGlow.png" ) );
	
//    mTextureLoader.requestTexture( "star.png",     mStarTex );
//    mTextureLoader.requestTexture( "starGlow.png", mStarGlowTex );
    //console() << "initLoadingTextures, duration: " << getElapsedSeconds() - t << std::endl;
}

void KeplerApp::initTextures()
{
    Flurry::getInstrumentation()->startTimeEvent("Load Textures");    
//	float t = getElapsedSeconds();
//	console() << "initTextures start time = " << t << endl;
    mStarTex			= loadImage( loadResource( "star.png" ) );
	mStarCoreTex		= loadImage( loadResource( "starCore.png" ) );
	mEclipseGlowTex		= loadImage( loadResource( "eclipseGlow.png" ) );
	mEclipseShadowTex	= loadImage( loadResource( "eclipseShadow.png" ) );
	mLensFlareTex		= loadImage( loadResource( "lensFlare.png" ) );
	mParticleTex		= loadImage( loadResource( "particle.png" ) );
	mSkyDome			= loadImage( loadResource( "skydome.png" ) );
	mGalaxyDome			= loadImage( loadResource( "skydome.jpg" ) );
	mDottedTex			= loadImage( loadResource( "dotted.png" ) );
	mDottedTex.setWrap( GL_REPEAT, GL_REPEAT );
	mRingsTex           = loadImage( loadResource( "rings.png" ) );
    mPlayheadProgressTex = loadImage( loadResource( "playheadProgress.png" ) );
	mPlayheadProgressTex.setWrap( GL_REPEAT, GL_REPEAT );
	mParamsTex			= gl::Texture( 768, 75 );    
	mUiButtonsTex		= loadImage( loadResource( "uiButtons.png" ) );
	mUiBigButtonsTex	= loadImage( loadResource( "uiBigButtons.png" ) );
	mUiSmallButtonsTex	= loadImage( loadResource( "uiSmallButtons.png" ) );
	mOverlayIconsTex	= loadImage( loadResource( "overlayIcons.png" ) );
    mAtmosphereTex		= loadImage( loadResource( "atmosphere.png" ) );
	mAtmosphereDirectionalTex = loadImage( loadResource( "atmosphereDirectional.png" ) );
	mAtmosphereSunTex	= loadImage( loadResource( "atmosphereSun.png" ) );
	mGalaxyTex			= loadImage( loadResource( "galaxy.jpg" ) );
	mDarkMatterTex		= loadImage( loadResource( "darkMatter.png" ) );
	mOrbitRingGradientTex = loadImage( loadResource( "orbitRingGradient.png" ) );
	mTrackOriginTex		= loadImage( loadResource( "origin.png" ) );
    
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "planetClouds1.png" ) ) ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "planetClouds2.png" ) ) ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "planetClouds3.png" ) ) ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "planetClouds4.png" ) ) ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "planetClouds5.png" ) ) ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "moonClouds1.png" ) ) ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "moonClouds2.png" ) ) ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "moonClouds3.png" ) ) ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "moonClouds4.png" ) ) ) );
	mCloudsTex.push_back( gl::Texture( loadImage( loadResource( "moonClouds5.png" ) ) ) );
	
	mHighResSurfaces	= Surface( loadImage( loadResource( "surfacesHighRes.png" ) ) );
	mLowResSurfaces		= Surface( loadImage( loadResource( "surfacesLowRes.png" ) ) );
	
    mNoAlbumArtSurface = Surface( loadImage( loadResource( "noAlbumArt.png" ) ) );
	//console() << "initTextures duration = " << (getElapsedSeconds()-t) << endl;
    Flurry::getInstrumentation()->stopTimeEvent("Load Textures");    
}

void KeplerApp::initGalaxyVertexArray()
{
	std::cout << "initializing Galaxy Vertex Array" << std::endl;
	mGalaxyVerts		= new float[18];
	mGalaxyTexCoords	= new float[12];
	int i	= 0;
	int t	= 0;
	float w	= 400.0f;
	Vec3f corner;
	
	
	corner			= Vec3f( -w, 0.0f, -w );
	mGalaxyVerts[i++]			= corner.x;
	mGalaxyVerts[i++]			= corner.y;
	mGalaxyVerts[i++]			= corner.z;
	mGalaxyTexCoords[t++]		= 0.0f;
	mGalaxyTexCoords[t++]		= 0.0f;
	
	corner			= Vec3f( w, 0.0f, -w );
	mGalaxyVerts[i++]			= corner.x;
	mGalaxyVerts[i++]			= corner.y;
	mGalaxyVerts[i++]			= corner.z;
	mGalaxyTexCoords[t++]		= 1.0f;
	mGalaxyTexCoords[t++]		= 0.0f;
	
	corner			= Vec3f( w, 0.0f, w );	
	mGalaxyVerts[i++]			= corner.x;
	mGalaxyVerts[i++]			= corner.y;
	mGalaxyVerts[i++]			= corner.z;
	mGalaxyTexCoords[t++]		= 1.0f;
	mGalaxyTexCoords[t++]		= 1.0f;
	
	corner			= Vec3f( -w, 0.0f, -w );
	mGalaxyVerts[i++]			= corner.x;
	mGalaxyVerts[i++]			= corner.y;
	mGalaxyVerts[i++]			= corner.z;
	mGalaxyTexCoords[t++]		= 0.0f;
	mGalaxyTexCoords[t++]		= 0.0f;
	
	corner			= Vec3f( w, 0.0f, w );	
	mGalaxyVerts[i++]			= corner.x;
	mGalaxyVerts[i++]			= corner.y;
	mGalaxyVerts[i++]			= corner.z;
	mGalaxyTexCoords[t++]		= 1.0f;
	mGalaxyTexCoords[t++]		= 1.0f;
	
	corner			= Vec3f( -w, 0.0f, w );	
	mGalaxyVerts[i++]			= corner.x;
	mGalaxyVerts[i++]			= corner.y;
	mGalaxyVerts[i++]			= corner.z;
	mGalaxyTexCoords[t++]		= 0.0f;
	mGalaxyTexCoords[t++]		= 1.0f;
}


void KeplerApp::initDarkMatterVertexArray()
{
	std::cout << "initializing Dark Matter Vertex Array" << std::endl;
	mDarkMatterVerts		= new float[ mDarkMatterCylinderRes * 6 * 3 ]; // cylinderRes * two-triangles * 3d
	mDarkMatterTexCoords	= new float[ mDarkMatterCylinderRes * 6 * 2 ]; // cylinderRes * two-triangles * 2d
	
	int i	= 0;
	int t	= 0;
	
	for( int x=0; x<mDarkMatterCylinderRes; x++ ){
		float per1		= (float)x/(float)mDarkMatterCylinderRes;
		float per2		= (float)(x+1)/(float)mDarkMatterCylinderRes;
		float angle1	= per1 * TWO_PI;
		float angle2	= per2 * TWO_PI;
		
		float sa1 = sin( angle1 );
		float ca1 = cos( angle1 );
		float sa2 = sin( angle2 );
		float ca2 = cos( angle2 );
		
		float h = 0.5f;
		Vec3f v1 = Vec3f( ca1, -h, sa1 );
		Vec3f v2 = Vec3f( ca2, -h, sa2 );
		Vec3f v3 = Vec3f( ca1,  h, sa1 );
		Vec3f v4 = Vec3f( ca2,  h, sa2 );
		
		mDarkMatterVerts[i++]		= v1.x;
		mDarkMatterVerts[i++]		= v1.y;
		mDarkMatterVerts[i++]		= v1.z;
		mDarkMatterTexCoords[t++]	= per1;
		mDarkMatterTexCoords[t++]	= 0.0f;
		
		mDarkMatterVerts[i++]		= v2.x;
		mDarkMatterVerts[i++]		= v2.y;
		mDarkMatterVerts[i++]		= v2.z;
		mDarkMatterTexCoords[t++]	= per2;
		mDarkMatterTexCoords[t++]	= 0.0f;
		
		mDarkMatterVerts[i++]		= v3.x;
		mDarkMatterVerts[i++]		= v3.y;
		mDarkMatterVerts[i++]		= v3.z;
		mDarkMatterTexCoords[t++]	= per1;
		mDarkMatterTexCoords[t++]	= 1.0f;
		
		mDarkMatterVerts[i++]		= v2.x;
		mDarkMatterVerts[i++]		= v2.y;
		mDarkMatterVerts[i++]		= v2.z;
		mDarkMatterTexCoords[t++]	= per2;
		mDarkMatterTexCoords[t++]	= 0.0f;
		
		mDarkMatterVerts[i++]		= v4.x;
		mDarkMatterVerts[i++]		= v4.y;
		mDarkMatterVerts[i++]		= v4.z;
		mDarkMatterTexCoords[t++]	= per2;
		mDarkMatterTexCoords[t++]	= 1.0f;
		
		mDarkMatterVerts[i++]		= v3.x;
		mDarkMatterVerts[i++]		= v3.y;
		mDarkMatterVerts[i++]		= v3.z;
		mDarkMatterTexCoords[t++]	= per1;
		mDarkMatterTexCoords[t++]	= 1.0f;
	}
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
                checkForNodeTouch( touchRay, mMatrix, mTouchPos );
            }
        }
	}
	if (getActiveTouches().size() != 1) {
        if (mIsDragging) {
            Flurry::getInstrumentation()->logEvent("Camera Moved");
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
    Flurry::getInstrumentation()->logEvent("Pinch Ended");

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

    std::map<string, string> params;
    params["Device Orientation"] = getOrientationString(event.getDeviceOrientation());
    Flurry::getInstrumentation()->logEvent("Orientation Changed", params);    
    
	if( ! G_USE_GYRO ){
		// Look over there!
		// heinous trickery follows...
		Orientation prevOrientation = event.getPrevInterfaceOrientation();
		if (mInterfaceOrientation != prevOrientation) {
			if( mTouchVel.length() > 2.0f && !mIsDragging ){        
				int steps = mRotationSteps[prevOrientation][mInterfaceOrientation];
				mTouchVel.rotate( (float)steps * M_PI/2.0f );
			}
		}
		// ... end heinous trickery
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
    if (mDataIsLoaded) {
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
	if( mAlphaWheel.getShowWheel() ){
		mFovDest = G_MAX_FOV;
	} else {
		G_HELP = false;
		mFovDest = G_DEFAULT_FOV;
	}
    
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
	mData.setFilter( LetterFilter(mState.getAlphaChar()) );
    mState.setFilterMode( State::FilterModeAlphaChar );

    std::map<string, string> parameters;
    parameters["Letter"] = ""+mState.getAlphaChar();
    parameters["Count"] = ""+mData.mFilteredArtists.size();
    Flurry::getInstrumentation()->logEvent("Letter Selected" , parameters);

	mWorld.filterNodes();
    
    mState.setSelectedNode( NULL );
    
	return false;
}

bool KeplerApp::onPlaylistStateChanged( State *state )
{
	std::cout << "playlist changed" << std::endl;
	mData.setFilter( PlaylistFilter(mState.getPlaylist()) );
    mState.setFilterMode( State::FilterModePlaylist );
    
	mPlayControls.setPlaylist( mState.getPlaylistName() );
	
    // FIXME: enable this 
//    std::map<string, string> parameters;
//    parameters["Playlist"] = ""+mState.getPlaylist();
//    Flurry::getInstrumentation()->logEvent("Playlist Selected" , parameters);
	
	mWorld.filterNodes();
    mState.setSelectedNode( NULL );
    
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
            // TODO: which track? album? artist?
            Flurry::getInstrumentation()->logEvent("Track Selected");
            // FIXME: is this a bad OOP thing or is there a cleaner/safer C++ way to handle it?
            NodeTrack* trackNode = (NodeTrack*)node;
            if ( mIpodPlayer.hasPlayingTrack() ){
				trackNode->setStartAngle();
                ipod::TrackRef playingTrack = mIpodPlayer.getPlayingTrack();
                if ( trackNode->getId() != playingTrack->getItemId() ) {
                    mIpodPlayer.play( trackNode->mAlbum, trackNode->mIndex );
                }
            }
            else {
                mIpodPlayer.play( trackNode->mAlbum, trackNode->mIndex );			
            }
        } 
        else if (node->mGen == G_ARTIST_LEVEL) {
            Flurry::getInstrumentation()->logEvent("Artist Selected");
        } 
        else if (node->mGen == G_ALBUM_LEVEL) {
            Flurry::getInstrumentation()->logEvent("Album Selected");
        }
	}

    updateIsPlaying();

	return false;
}

bool KeplerApp::onPlayControlsPlayheadMoved( float dragPer )
{
	if ( mIpodPlayer.hasPlayingTrack() ) {
        ipod::TrackRef playingTrack = mIpodPlayer.getPlayingTrack();
        double trackLength = playingTrack->getLength();
        if( getElapsedFrames() % 3 == 0 ){
            // FIXME: make the playhead update to finger position when touched
            // rather than making the playhead TIME update to finger position and then lagging to update the playhead position
            // subtle but important :)
            mIpodPlayer.setPlayheadTime( trackLength * dragPer );
        }
    }
    return false;
}

bool KeplerApp::onPlayControlsButtonPressed( PlayControls::ButtonId button )
{
    switch( button ) {
        
        case PlayControls::PREV_TRACK:
            Flurry::getInstrumentation()->logEvent("Previous Track Button Selected");            
            mIpodPlayer.skipPrev();
            break;
        
        case PlayControls::PLAY_PAUSE:
            Flurry::getInstrumentation()->logEvent("Play/Pause Button Selected");            
            if (mIpodPlayer.getPlayState() == ipod::Player::StatePlaying) {
                mIpodPlayer.pause();
				mNotificationOverlay.show( mOverlayIconsTex, Area( 0.0f, 128.0f, 128.0f, 256.0f ), "PAUSED" );
            }
            else {
                mIpodPlayer.play();
				mNotificationOverlay.show( mOverlayIconsTex, Area( 0.0f, 0.0f, 128.0f, 128.0f ), "PLAY" );
            }
            break;
        
        case PlayControls::NEXT_TRACK:
            Flurry::getInstrumentation()->logEvent("Next Track Button Selected");            
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
				
            Flurry::getInstrumentation()->logEvent("Shuffle Button Selected");    
            break;
			
		case PlayControls::REPEAT:
			if( mIpodPlayer.getRepeatMode() != ipod::Player::RepeatModeAll ){
				mIpodPlayer.setRepeatMode( ipod::Player::RepeatModeNone );
				mNotificationOverlay.show( mOverlayIconsTex, Area( 256.0f, 128.0f, 384.0f, 256.0f ), "REPEAT NONE" );
			} else {
				mIpodPlayer.setRepeatMode( ipod::Player::RepeatModeAll );
				mNotificationOverlay.show( mOverlayIconsTex, Area( 256.0f, 0.0f, 384.0f, 128.0f ), "REPEAT ALL" );
			}
			
            Flurry::getInstrumentation()->logEvent("Repeat Button Selected");   
            break;
        
        case PlayControls::HELP:
			std::cout << "HELP!!!! IN PLAYCONTROLS!!!!" << std::endl;
			if( G_SHOW_SETTINGS ){
				Flurry::getInstrumentation()->logEvent("Help Button Selected");            
				G_HELP = !G_HELP;
				if( G_HELP && !mAlphaWheel.getShowWheel() )
					mAlphaWheel.setShowWheel( true );
			}
            break;
        
        case PlayControls::DRAW_RINGS:
			if( G_SHOW_SETTINGS ){
				Flurry::getInstrumentation()->logEvent("Draw Rings Button Selected");            
				G_DRAW_RINGS = !G_DRAW_RINGS;
				if( G_DRAW_RINGS )	mNotificationOverlay.show( mOverlayIconsTex, Area( 512.0f, 0.0f, 640.0f, 128.0f ), "ORBIT LINES" );
				else				mNotificationOverlay.show( mOverlayIconsTex, Area( 512.0f, 128.0f, 640.0f, 256.0f ), "ORBIT LINES" );
			}
            break;
        
        case PlayControls::DRAW_TEXT:
			if( G_SHOW_SETTINGS ){
				Flurry::getInstrumentation()->logEvent("Draw Text Button Selected");            
				G_DRAW_TEXT = !G_DRAW_TEXT;
				if( G_DRAW_TEXT )	mNotificationOverlay.show( mOverlayIconsTex, Area( 640.0f, 0.0f, 768.0f, 128.0f ), "TEXT LABELS" );
				else				mNotificationOverlay.show( mOverlayIconsTex, Area( 640.0f, 128.0f, 768.0f, 256.0f ), "TEXT LABELS" );
			}
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
				Flurry::getInstrumentation()->logEvent("Use Gyro Button Selected");            
				G_USE_GYRO = !G_USE_GYRO;
				if( G_USE_GYRO )	mNotificationOverlay.show( mOverlayIconsTex, Area( 384.0f, 0.0f, 512.0f, 128.0f ), "GYROSCOPE" );
				else				mNotificationOverlay.show( mOverlayIconsTex, Area( 384.0f, 128.0f, 512.0f, 256.0f ), "GYROSCOPE" );
			}
            break;
			
		case PlayControls::GOTO_GALAXY:
            Flurry::getInstrumentation()->logEvent("Galaxy Button Selected");
			mState.setSelectedNode( NULL );
            break;
			
        case PlayControls::GOTO_CURRENT_TRACK:
            Flurry::getInstrumentation()->logEvent("Current Track Button Selected");            
            // pretend the track just got changed again, this will select it:
            onPlayerTrackChanged( &mIpodPlayer );
            break;
			
		case PlayControls::SETTINGS:
            Flurry::getInstrumentation()->logEvent("Settings Button Selected");            
            G_SHOW_SETTINGS = !G_SHOW_SETTINGS;
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
            break;
			
		case PlayControls::NEXT_PLAYLIST:
			mPlaylistIndex ++;
			mPlaylistIndex = constrain( mPlaylistIndex, 0, (int)mData.mPlaylists.size() - 1 );
            mState.setPlaylist( mData.mPlaylists[ mPlaylistIndex ] );
            break;
			
		case PlayControls::PREV_PLAYLIST:
            mPlaylistIndex --;
			mPlaylistIndex = constrain( mPlaylistIndex, 0, (int)mData.mPlaylists.size() - 1 );
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
 
void KeplerApp::checkForNodeTouch( const Ray &ray, Matrix44f &mat, const Vec2f &pos )
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
                    Flurry::getInstrumentation()->logEvent("Artist Node Touched");                    
					mState.setSelectedNode( nodeWithHighestGen );
                }
//                else {
//                    console() << "ignoring artist node selection" << std::endl;                                        
//                }
			} 
            else if ( highestGen == G_TRACK_LEVEL ){
                Flurry::getInstrumentation()->logEvent("Track Node Touched");
				if( nodeWithHighestGen != mState.getSelectedNode() ) {                    
                    mState.setSelectedNode( nodeWithHighestGen );
                }
                else {
                    // if this is already the selected node, just toggle the play state
                    if (mIpodPlayer.getPlayState() == ipod::Player::StatePlaying) {
                        mIpodPlayer.pause();
                    }
                    else if (mIpodPlayer.getPlayState() == ipod::Player::StatePaused) {
                        mIpodPlayer.play();
                    }                    
                }
                
            }
            else {
                Flurry::getInstrumentation()->logEvent("Album Node Touched");
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
	if( mData.update() ){
		mWorld.initNodes( &mIpodPlayer, mFont, mFontMediTiny, mHighResSurfaces, mLowResSurfaces, mNoAlbumArtSurface );
		mDataIsLoaded = true;
		mLoadingScreen.setEnabled( false );
		mUiLayer.setIsPanelOpen( true );
        onSelectedNodeChanged( NULL );
        // and then make sure we know about the current track if there is one
        if ( mIpodPlayer.getPlayState() == ipod::Player::StatePlaying ) {
            Flurry::getInstrumentation()->logEvent("Startup with Track Playing");                        
            onPlayerTrackChanged( &mIpodPlayer );
        } else {
            Flurry::getInstrumentation()->logEvent("Startup without Track Playing");
			mAlphaWheel.setShowWheel( true );
		}
	}
    //mTextureLoader.update();
    
    if ( mRemainingSetupCalled )
	{
		if( G_IS_IPAD2 && G_USE_GYRO ) {
			updateGyro();
        }

        updateArcball();

        // for mPlayControls and mWorld.mPlayingTrackNode
		double currentTrackPlayheadTime = mIpodPlayer.getPlayheadTime();
        // TODO: cache this when playing track changes
		double currentTrackLength = mIpodPlayer.getPlayingTrack()->getLength();

		if( mWorld.mPlayingTrackNode && G_ZOOM > G_ARTIST_LEVEL ){
			mWorld.mPlayingTrackNode->updateAudioData( currentTrackPlayheadTime );
		}
		
		mScaleSlider = 0.25f + mPlayControls.getParamSlider1Value() * 2.0f;
		mSpeedSlider = mPlayControls.getParamSlider2Value() * 0.075f;
        mWorld.update( mMatrix, mScaleSlider, mSpeedSlider );
		
        updateCamera();
        mWorld.updateGraphics( mCam, mBbRight, mBbUp );

		Matrix44f inverseMatrix = mMatrix.inverted();
        Vec3f invBbRight		= inverseMatrix * mBbRight;
        Vec3f invBbUp			= inverseMatrix * mBbUp;        
        
        if( mDataIsLoaded ){
            mWorld.buildStarsVertexArray( invBbRight, invBbUp, mFadeInAlphaToArtist * 0.3f );
            mWorld.buildStarGlowsVertexArray( invBbRight, invBbUp, mFadeInAlphaToArtist );
        }
		
		Node *selectedArtistNode = mState.getSelectedArtistNode();
		if( selectedArtistNode ){
			mParticleController.update( mMatrix.inverted() * mCenter, selectedArtistNode->mRadius * 0.15f, invBbRight, invBbUp );
			float per = selectedArtistNode->mEclipseStrength * 0.5f + 0.25f;
			mParticleController.buildParticleVertexArray( mPlayControls.getParamSlider1Value() * 5.0f, 
														  selectedArtistNode->mColor, 
														  ( sin( per * M_PI ) * sin( per * 0.25f ) * 0.75f ) + 0.25f, 
														  mMatrix );
			mParticleController.buildDustVertexArray( mPlayControls.getParamSlider1Value(), selectedArtistNode, mPinchAlphaPer, ( 1.0f - mCamRingAlpha ) * 0.15f * mFadeInArtistToAlbum );
		}
		
		mNotificationOverlay.update();
		
        mUiLayer.setShowSettings( G_SHOW_SETTINGS );
        mUiLayer.update();
        
		mHelpLayer.update();
		mAlphaWheel.update( mFov );
        
        mPlayControls.update();
        mPlayControls.setAlphaWheelVisible( mAlphaWheel.getShowWheel() );
        mPlayControls.setShowSettings( G_SHOW_SETTINGS );
        mPlayControls.setOrbitsVisible( G_DRAW_RINGS );
        mPlayControls.setLabelsVisible( G_DRAW_TEXT );
        mPlayControls.setHelpVisible( G_HELP );

		mPlayControls.setDebugVisible( G_DEBUG );	
		if( G_IS_IPAD2 ) mPlayControls.setGyroVisible( G_USE_GYRO );
		
		if( mIpodPlayer.getShuffleMode() == ipod::Player::ShuffleModeOff ) mPlayControls.setShuffleVisible( false );
		else mPlayControls.setShuffleVisible( true );
		
		if( mIpodPlayer.getRepeatMode() == ipod::Player::RepeatModeNone ) mPlayControls.setRepeatVisible( false );
		else mPlayControls.setRepeatVisible( true );
		

        mPlayControls.setElapsedSeconds( (int)currentTrackPlayheadTime );
        mPlayControls.setRemainingSeconds( -(int)(currentTrackLength - currentTrackPlayheadTime) );
        mPlayControls.setPlayheadProgress( currentTrackPlayheadTime / currentTrackLength );
    }
    else {
        // make sure we've drawn the loading screen first
        if (getElapsedFrames() > 1) {
            remainingSetup();
        }        
    }
    
    mNotificationOverlay.update();
}


void KeplerApp::updateGyro()
{
	CMQuaternion quat;
	
    CMDeviceMotion *deviceMotion = motionManager.deviceMotion;		
    CMAttitude *attitude = deviceMotion.attitude;
    
    // If we have a reference attitude, multiply attitude by its inverse
    // After this call, attitude will contain the rotation from referenceAttitude
    // to the current orientation instead of from the fixed reference frame to the
    // current orientation
	/*
    if (referenceAttitude != nil) {
        [attitude multiplyByInverseOfAttitude:referenceAttitude];
    }
	*/

	quat		= attitude.quaternion;
	mGyroQuat	= Quatf( quat.w, quat.x, -quat.y, quat.z );
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
	
	
//	if( G_IS_IPAD2 ){
//		mMatrix = mArcball.getQuat() * mGyroQuat;
//	} else {
//		mMatrix = mArcball.getQuat();
//	}
    mMatrix.setToIdentity();
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
			Vec3f dirToParent = selectedNode->mParentNode->mTransPos - selectedNode->mTransPos;
			mCenterOffset -= ( mCenterOffset - ( dirToParent * ( mPinchPer - mPinchPerThresh ) * 2.5f ) ) * 0.2f;
			
		} else {
			mCenterOffset -= ( mCenterOffset - Vec3f::zero() ) * 0.2f;
		}
		mCenterDest		= selectedNode->mTransPos;
		mZoomDest		= selectedNode->mGen;
		mCenterFrom		+= selectedNode->mTransVel;
		
	} else {
		mCamDistDest	= G_INIT_CAM_DIST * cameraDistMulti;
		mCenterDest		= mMatrix.transformPointAffine( Vec3f::zero() );

		mZoomDest		= G_HOME_LEVEL;
		if( mState.getAlphaChar() != ' ' ){
			mZoomDest	= G_ALPHA_LEVEL;
		}
		
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
	mFadeOverFullZoomDuration = p/duration;
	
    // apply the Arcball to the camera eye/up vectors
    // (instead of to the whole scene)
    // FIXME: mMatrix can go away
    // FIXME: mBbRight and mBbUp should be correct now, no need to invert
    // FIXME: arcball interaction is probably broken under orientation
    // FIXME: no need to calculate mTransPos for Nodes etc.
    // FIXME: gyro should be applied here as well
    // FIXME: sorting uses world.z but it should be dist-from-cam.z
    Quatf q = mArcball.getQuat();
    q.w *= -1.0;
	
	Vec3f prevEye	= mEye;
    Vec3f camOffset = q * Vec3f( 0, 0, mCamDist);
    mEye = mCenter - camOffset;
	//mEye			= Vec3f( mCenter.x, mCenter.y, mCenter.z - mCamDist );//- sin( mCamDistAnim ) * distToTravel * 0.25f );
	mCamVel			= mEye - prevEye;
	
	//Vec3f mRotatedUp = mUp;
    //mRotatedUp.rotateZ( -mPinchRotation );

	mCam.setPerspective( mFov, getWindowAspectRatio(), 0.001f, 2000.0f );
	mCam.lookAt( (mEye - mCenterOffset), mCenter, q * mUp );
	mCam.getBillboardVectors( &mBbRight, &mBbUp );
	mCamNormal = mEye - mCenter;
	mCamNormal.normalize();
}

void KeplerApp::draw()
{
	gl::clear( Color( 0, 0, 0 ), true );
	if( !mDataIsLoaded ){
		mLoadingScreen.draw( mStarGlowTex );
	} else if( mData.mArtists.size() == 0 ){
		if( !mHasNoArtists ) mNoArtistsTex = gl::Texture( loadImage( loadResource( "noArtists.png" ) ) );
		
		mHasNoArtists = true;
		drawNoArtists();
	} else {
		drawScene();
	}
    mNotificationOverlay.draw();
}

void KeplerApp::drawNoArtists()
{
	gl::setMatricesWindow( getWindowSize() );    
	
    gl::pushModelView();
    gl::multModelView( mOrientationMatrix );
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
	Node *artistNode	= mState.getSelectedArtistNode();
	if( artistNode ){
		unsortedNodes.push_back( artistNode );
	}
	vector<Node*> sortedNodes = mWorld.sortNodes( unsortedNodes );	
	
	
	// For doing galaxy-axis fades
	Vec3f transEye = mMatrix.inverted() * mEye;
	float zoomOff = 1.0f - mFadeInAlphaToArtist;//constrain( ( G_ARTIST_LEVEL - G_ZOOM ), 0.0f, 1.0f );
	float camGalaxyAlpha = constrain( abs( transEye.y ) * 0.004f, 0.0f, 1.0f );
	float alpha, radius;
	float invAlpha = pow( 1.0f - camGalaxyAlpha, 2.5f ) * zoomOff;
	
    gl::enableDepthWrite();
    gl::setMatrices( mCam );
	gl::pushModelView();
    gl::rotate( mMatrix );
	
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
    mSkyDome.enableAndBind();
    gl::drawSphere( Vec3f::zero(), G_SKYDOME_RADIUS, 24 );
	
	
	
	gl::enableAdditiveBlending();
	
	
	
// LIGHTMATTER
	if( invAlpha > 0.01f ){
		gl::color( ColorA( BRIGHT_BLUE, invAlpha * 2.0f ) );

		radius = mLightMatterBaseRadius * 0.9f;
		gl::pushModelView();
			mGalaxyDome.enableAndBind();
			glEnableClientState( GL_VERTEX_ARRAY );
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			glVertexPointer( 3, GL_FLOAT, 0, mDarkMatterVerts );
			glTexCoordPointer( 2, GL_FLOAT, 0, mDarkMatterTexCoords );
			
			gl::scale( Vec3f( radius, radius * 0.69f, radius ) );
			gl::rotate( Vec3f( 0.0f, getElapsedSeconds() * -2.0f, 0.0f ) );
			glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
		
			gl::scale( Vec3f( 1.25, 1.15f, 1.25f ) );
			gl::rotate( Vec3f( 0.0f, getElapsedSeconds() * -0.5f, 0.0f ) );
			glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );

			glDisableClientState( GL_VERTEX_ARRAY );
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
			mGalaxyDome.disable();
		gl::popModelView();
	}
	
	
	gl::enableAdditiveBlending();
	
	
	
// GALAXY SPIRAL PLANES
	alpha = ( 1.25f - camGalaxyAlpha ) * zoomOff;//sqrt(camGalaxyAlpha) * zoomOff;
	if( alpha > 0.01f ){
		mGalaxyTex.enableAndBind();
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, mGalaxyVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, mGalaxyTexCoords );
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, alpha ) );
		
		gl::translate( Vec3f( 0.0f, 2.5f, 0.0f ) );
		gl::rotate( Vec3f( 0.0f, getElapsedSeconds() * -4.0f, 0.0f ) );
		  glDrawArrays( GL_TRIANGLES, 0, 6 );
		
		gl::translate( Vec3f( 0.0f, -5.0f, 0.0f ) );
		gl::rotate( Vec3f( 0.0f, getElapsedSeconds() * -2.0f, 0.0f ) );
		  glDrawArrays( GL_TRIANGLES, 0, 6 );
		
		gl::translate( Vec3f( 0.0f, 2.5f, 0.0f ) );
		gl::scale( Vec3f( 0.5f, 0.5f, 0.5f ) );
		gl::rotate( Vec3f( 0.0f, getElapsedSeconds() * -15.0f, 0.0f ) );
		  glDrawArrays( GL_TRIANGLES, 0, 6 );
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		mGalaxyTex.disable();
	}
	
    gl::popModelView();
	
	
	
// CENTER OF GALAXY
	if( invAlpha > 0.01f ){
		mStarGlowTex.enableAndBind();
		gl::color( ColorA( BLUE, invAlpha ) );
		gl::drawBillboard( Vec3f::zero(), Vec2f( 300.0f, 300.0f ), getElapsedSeconds() * 10.0f, mBbRight, mBbUp );
		gl::color( ColorA( BRIGHT_BLUE, invAlpha * 1.5f ) );
		gl::drawBillboard( Vec3f::zero(), Vec2f( 200.0f, 200.0f ), -getElapsedSeconds() * 7.0f, mBbRight, mBbUp );
		mStarGlowTex.disable();
	}
	
	

	
	gl::enableAdditiveBlending();
	
	
// STARS
	mStarTex.enableAndBind();
	mWorld.drawStarsVertexArray( mMatrix );
	mStarTex.disable();
	
	
// STARGLOWS bloom (TOUCH HIGHLIGHTS)
	mEclipseGlowTex.enableAndBind();
	mWorld.drawTouchHighlights( mFadeInArtistToAlbum );
	mEclipseGlowTex.disable();
	
	
// STARGLOWS bloom
	mStarGlowTex.enableAndBind();
	mWorld.drawStarGlowsVertexArray( mMatrix );
	mStarGlowTex.disable();
	
	
	// SHADOWS
	//gl::setMatricesWindow( getWindowSize() );
//	for( int i = 0; i < sortedNodes.size(); i++ ){
//		if( sortedNodes[i]->mGen >= G_ALBUM_LEVEL ){
//			gl::color( Color( 1.0f, 1.0f, 1.0f ) );
//			sortedNodes[i]->findShadows( mEclipseShadowTex, pow( mCamRingAlpha, 2.0f ) );
//		}
//	}
	//gl::setMatrices( mCam );
	

	if( artistNode ){ // defined at top of method
		Vec2f interfaceSize = getWindowSize();
		if ( isLandscapeOrientation( mInterfaceOrientation ) ) {
			interfaceSize = interfaceSize.yx(); // swizzle it!
		}
		
		//float zoomOffset = constrain( 1.0f - ( G_ALBUM_LEVEL - G_ZOOM ), 0.0f, 1.0f );
		mCamRingAlpha = constrain( abs( transEye.y - artistNode->mPos.y ), 0.0f, 1.0f ); // WAS 0.6f

		glCullFace( GL_BACK );
		glEnable( GL_CULL_FACE );
		glEnable( GL_COLOR_MATERIAL );
		glEnable( GL_RESCALE_NORMAL );
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, ColorA( 0.0f, 0.0f, 0.0f, 1.0f ) );
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, ColorA( Color::white(), 1.0f ) );
		
	
		
		for( int i = 0; i < sortedNodes.size(); i++ ){
			if( sortedNodes[i]->mGen == G_ALBUM_LEVEL ){
				gl::enableAlphaBlending();
				glDisable( GL_CULL_FACE );
				glEnableClientState( GL_VERTEX_ARRAY );
				glEnableClientState( GL_TEXTURE_COORD_ARRAY );
				mEclipseShadowTex.enableAndBind();
				sortedNodes[i]->findShadows( pow( mCamRingAlpha, 1.2f ) );
				glDisableClientState( GL_VERTEX_ARRAY );
				glDisableClientState( GL_TEXTURE_COORD_ARRAY );
				glEnable( GL_CULL_FACE );
				
				gl::enableDepthWrite();
			}
			
			gl::enableDepthRead();
			glEnable( GL_LIGHTING );
			
			// LIGHT FROM ARTIST
			glEnable( GL_LIGHT0 );
			glEnable( GL_LIGHT1 );
			Vec3f lightPos          = artistNode->mTransPos;
			GLfloat artistLight[]	= { lightPos.x, lightPos.y, lightPos.z, 1.0f };
			glLightfv( GL_LIGHT0, GL_POSITION, artistLight );
			glLightfv( GL_LIGHT0, GL_DIFFUSE, ColorA( artistNode->mColor, 1.0f ) );
			glLightfv( GL_LIGHT1, GL_POSITION, artistLight );
			glLightfv( GL_LIGHT1, GL_DIFFUSE, ColorA( BRIGHT_BLUE, 1.0f ) );
			gl::disableAlphaBlending();
			gl::enableAlphaBlending();
//			if( i==0 ){
//				artistNode->drawStarCore( mStarCoreTex );	
//				glDisable( GL_LIGHTING );
//				gl::disableDepthRead();
//				gl::enableAdditiveBlending();
//				artistNode->drawAtmosphere( mAtmosphereTex, mAtmosphereDirectionalTex, mPinchAlphaPer );
//				glEnable( GL_LIGHTING );
//				gl::enableDepthRead();
//				gl::enableAlphaBlending();
//			}
			
			
			sortedNodes[i]->drawPlanet( mStarCoreTex );
			sortedNodes[i]->drawClouds( mCloudsTex );

			glDisable( GL_LIGHTING );
			gl::disableDepthRead();
			
			if( sortedNodes[i]->mGen == G_ARTIST_LEVEL ){
				gl::enableAlphaBlending();
				sortedNodes[i]->drawAtmosphere( interfaceSize * 0.5f, mAtmosphereSunTex, mAtmosphereDirectionalTex, mPinchAlphaPer );	
			} else {
				gl::enableAdditiveBlending();
				sortedNodes[i]->drawAtmosphere( interfaceSize * 0.5f, mAtmosphereTex, mAtmosphereDirectionalTex, mPinchAlphaPer );
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
        mWorld.drawOrbitRings( mPinchAlphaPer, sqrt( mCamRingAlpha ), mOrbitRingGradientTex );
	}
	
// PARTICLES
	if( mState.getSelectedArtistNode() ){
		mParticleTex.enableAndBind();
		mParticleController.drawParticleVertexArray( mState.getSelectedArtistNode(), mMatrix );
		mParticleTex.disable();
	}
	
	
// RINGS
	if( artistNode ){
		//alpha = pow( mCamRingAlpha, 2.0f );
		mWorld.drawRings( mRingsTex, mCamRingAlpha * 0.5f );
	}
	
	
// DUSTS
	if( mState.getSelectedArtistNode() ){
		gl::enableAdditiveBlending();
		mParticleController.drawDustVertexArray( mState.getSelectedArtistNode(), mMatrix );
	}

	
	
// PLAYHEAD PROGRESS
	if( mWorld.mPlayingTrackNode && G_ZOOM > G_ARTIST_LEVEL ){
		gl::enableAdditiveBlending();
		
		float pauseAlpha = 1.0f;
		if( mIpodPlayer.getPlayState() == ipod::Player::StatePaused ){
			pauseAlpha = sin(getElapsedSeconds() * M_PI * 2.0f ) * 0.25f + 0.75f;
		}
		if( G_DRAW_RINGS )
			mWorld.mPlayingTrackNode->drawPlayheadProgress( mPinchAlphaPer, mCamRingAlpha, pauseAlpha, mPlayheadProgressTex, mTrackOriginTex );
	}
	
	
// CONSTELLATION
	if( mData.mFilteredArtists.size() > 1 && G_DRAW_RINGS ){
		gl::enableAdditiveBlending();
		mDottedTex.enableAndBind();
		mWorld.drawConstellation( mMatrix );
		mDottedTex.disable();
	}
	
	
	gl::enableAlphaBlending();
	
	
	
// DARKMATTER //////////////////////////////////////////////////////////////////////////////////////////
	if( invAlpha > 0.01f ){
		glEnable( GL_CULL_FACE ); 
		glCullFace( GL_FRONT ); 
		alpha = pow( 1.0f - camGalaxyAlpha, 8.0f ) * zoomOff;
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, invAlpha ) );
		radius = mDarkMatterBaseRadius * 0.85f;
		gl::pushModelView();
		gl::rotate( mMatrix );
		mDarkMatterTex.enableAndBind();
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, mDarkMatterVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, mDarkMatterTexCoords );
		
		gl::rotate( Vec3f( 0.0f, getElapsedSeconds() * -2.0f, 0.0f ) );
		gl::scale( Vec3f( radius, radius * 0.5f, radius ) );
			glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
		
		gl::rotate( Vec3f( 0.0f, getElapsedSeconds() * -0.5f, 0.0f ) );
		gl::scale( Vec3f( 1.3f, 1.3f, 1.3f ) );
			glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );

		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		mDarkMatterTex.disable();
		gl::popModelView();
		glDisable( GL_CULL_FACE ); 
	}
	
	
	
	gl::disableDepthRead();
	gl::disableDepthWrite();
	glEnable( GL_TEXTURE_2D );
	gl::setMatricesWindow( getWindowSize() );
	gl::enableAdditiveBlending();
	

	
// NAMES
	if( G_DRAW_TEXT ){
		mWorld.drawNames( mCam, mPinchAlphaPer, getAngleForOrientation(mInterfaceOrientation) );
	}
	
	
// LENSFLARE?!?!  SURELY YOU MUST BE MAD.
	if( artistNode && artistNode->mDistFromCamZAxis > 0.0f ){
		int numFlares  = 5;
		float radii[7] = { 0.8f, 1.2f, 4.5f, 8.0f, 6.0f };
		float dists[7] = { 0.8f, 1.0f, 1.5f, 1.70f, 2.0f };
		
		float distFromCenter = artistNode->mScreenPos.distance( getWindowCenter() );
		float distPer = constrain( 1.0f - distFromCenter/400.0f, 0.0f, 1.0f );
		float alpha = distPer * 0.2f * sin( distPer * M_PI );
		
		gl::enableAdditiveBlending();
		mLensFlareTex.enableAndBind();
		
		Vec2f flarePos = getWindowCenter() - artistNode->mScreenPos;
		float flareDist = flarePos.length();
		Vec2f flarePosNorm = flarePos.normalized();
		
		for( int i=0; i<numFlares; i++ ){
			gl::color( ColorA( BRIGHT_BLUE, alpha ) );
			flarePos = getWindowCenter() + flarePosNorm * dists[i] * flareDist;
			float flareRadius = artistNode->mSphereScreenRadius * radii[i] * distPer;
			gl::drawSolidRect( Rectf( flarePos.x - flareRadius, flarePos.y - flareRadius, flarePos.x + flareRadius, flarePos.y + flareRadius ) );
		}
		

		mLensFlareTex.disable();
		
		
		glDisable( GL_TEXTURE_2D );
		Vec2f dirToCenter = ( artistNode->mScreenPos - getWindowCenter() );
		float distToCenter = dirToCenter.length();
		dirToCenter.normalize();
		float left		= sin( constrain( dirToCenter.x * (float)M_PI, 0.0f, (float)M_PI ) ) * 0.5f;
		float right		= sin( constrain( ( 1.0f - dirToCenter.x ) * (float)M_PI, 0.0f, (float)M_PI ) ) * 0.5f;
		float top		= sin( constrain( dirToCenter.y * (float)M_PI, 0.0f, (float)M_PI ) ) * 0.5f;
		float bottom	= sin( constrain( ( 1.0f - dirToCenter.y ) * (float)M_PI, 0.0f, (float)M_PI ) ) * 0.5f;
		
		gl::color( ColorA( artistNode->mGlowColor, top ) );
		gl::drawLine( Vec2f( 1.0f, 1.0f ), Vec2f( getWindowWidth(), 1.0f ) );
		
		gl::color( ColorA( artistNode->mGlowColor, bottom ) );
		gl::drawLine( Vec2f( 1.0f, getWindowHeight() ), Vec2f( getWindowWidth(), getWindowHeight() ) );
		
		gl::color( ColorA( artistNode->mGlowColor, left ) );
		gl::drawLine( Vec2f( 1.0f, 1.0f ), Vec2f( 1.0f, getWindowHeight() ) );

		gl::color( ColorA( artistNode->mGlowColor, right ) );
		gl::drawLine( Vec2f( getWindowWidth(), 1.0f ), Vec2f( getWindowWidth(), getWindowHeight() ) );
		glEnable( GL_TEXTURE_2D );

	}

	
// PINCH FINGER POSITIONS
	if( mIsPinching ){
		float radius = mPinchHighlightRadius;
		float alpha = 1.0f;

		if( mPinchPer > mPinchPerThresh ){
			mStarGlowTex.enableAndBind();
			alpha = 0.2f;
			//c = BRIGHT_BLUE;
		} else {
			mStarGlowTex.enableAndBind();
		}
					  
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

	
	glDisable( GL_TEXTURE_2D );
	
	
//	if( G_DEBUG ){
//		
//		// HIT AREA VISUALIZER
//		for (int i = 0; i < mWorld.mNodes.size(); i++) {
//			Node* artistNode = mWorld.mNodes[i];
//			if (artistNode->mIsHighlighted) {
//				gl::color(ColorA(0.0f,0.0f,1.0f,0.25f));
//				if( artistNode->mDistFromCamZAxisPer > 0.0f ){
//					if( G_DRAW_TEXT && artistNode->mIsHighlighted ) gl::drawSolidRect(artistNode->mHitArea);
//					gl::drawSolidRect(artistNode->mSphereHitArea);       
//				}
//				
//				for (int j = 0; j < artistNode->mChildNodes.size(); j++) {					
//					Node* albumNode = artistNode->mChildNodes[j];
//					if (albumNode->mIsHighlighted) {
//						gl::color(ColorA(0.0f,1.0f,0.0f,0.25f));
//						if( G_DRAW_TEXT ) gl::drawSolidRect(albumNode->mHitArea);
//						gl::drawSolidRect(albumNode->mSphereHitArea);
//						
//						for (int k = 0; k < albumNode->mChildNodes.size(); k++) {
//							Node *trackNode = albumNode->mChildNodes[k];
//							if (trackNode->mIsHighlighted) {
//								gl::color(ColorA(1.0f,0.0f,0.0f,0.25f));
//								if( G_DRAW_TEXT ) gl::drawSolidRect(trackNode->mHitArea);
//								gl::drawSolidRect(trackNode->mSphereHitArea);
//							}
//						}            
//					}
//				}
//			}
//		}
//	}
    

   
	

// SHADOWS
	
//	for( int i = 0; i < sortedNodes.size(); i++ ){
//		if( sortedNodes[i]->mGen == G_ALBUM_LEVEL ){
//			gl::color( Color( 1.0f, 1.0f, 1.0f ) );
//			sortedNodes[i]->findShadows( mEclipseShadowTex );
//		}
//	}

	
	
	gl::disableAlphaBlending();
    gl::enableAlphaBlending();
	
// EVERYTHING ELSE	
	mAlphaWheel.draw( mData.mNormalizedArtistsPerChar );
	mHelpLayer.draw( mUiButtonsTex, mUiLayer.getPanelYPos() );
    mUiLayer.draw( mUiButtonsTex );
    mPlayControls.draw( mUiLayer.getPanelYPos() );
	
	mNotificationOverlay.draw();
	
	
	if( G_DEBUG ){
		gl::enableAdditiveBlending();
		drawInfoPanel();
	}
}


void KeplerApp::drawInfoPanel()
{
	gl::setMatricesWindow( getWindowSize() );
    gl::pushModelView();
    gl::multModelView( mOrientationMatrix );
	if( getElapsedFrames() % 30 == 0 ){
		setParamsTex();
	}
	gl::color( Color( 1.0f, 1.0f, 1.0f ) );
	gl::draw( mParamsTex, Vec2f( 23.0f, 25.0f ) );
    gl::popModelView();
}


void KeplerApp::setParamsTex()
{
    stringstream s;
	TextLayout layout;	
	layout.setFont( mFont );
	layout.setColor( BRIGHT_BLUE );

	s.str("");
	s << "FPS: " << getAverageFps();
	layout.addLine( s.str() );
	
	layout.setColor( BLUE );
	s.str("");
	s << "PLAYHEAD TIME: " << mIpodPlayer.getPlayheadTime();
	layout.addLine( s.str() );
	
	s.str("");
	s << "FOV: " << mFov;
	layout.addLine( s.str() );
	
	s.str("");
	s << "CURRENT LEVEL: " << G_CURRENT_LEVEL;
	layout.addLine( s.str() );
	
	s.str("");
	s << "ZOOM LEVEL: " << G_ZOOM;
	layout.addLine( s.str() );
	
	mParamsTex = gl::Texture( layout.render( true, false ) );
}

bool KeplerApp::onPlayerLibraryChanged( ipod::Player *player )
{	
//	console() << "/////////////////////" << std::endl;
//	console() << "onPlayerLibraryChanged!" << std::endl;

    Flurry::getInstrumentation()->logEvent("Player Library Changed");

    mDataIsLoaded = false;
	mLoadingScreen.setEnabled( true );
    mState.setup();    
    mData.setup();
	mWorld.setup( &mData );
    
    return false;
}

bool KeplerApp::onPlayerTrackChanged( ipod::Player *player )
{	
    // TODO: does Flurry care about this?
    
    mPlayControls.setLastTrackChangeTime( getElapsedSeconds() );
    
//	console() << "==================================================================" << std::endl;
//	console() << "onPlayerTrackChanged!" << std::endl;

    Flurry::getInstrumentation()->logEvent("Player Track Changed");

	if (mIpodPlayer.hasPlayingTrack()) {
        
		ipod::TrackRef playingTrack = mIpodPlayer.getPlayingTrack();

        mPlayControls.setCurrentTrack(" " + playingTrack->getArtist() 
                                      + " • " + playingTrack->getAlbumTitle() 
                                      + " • " + playingTrack->getTitle() + " ");
                
        Node *selectedNode = mState.getSelectedNode();
        if (!(selectedNode != NULL && selectedNode->getId() == playingTrack->getItemId())) {
        
            Node* artistNode = getPlayingArtistNode( playingTrack );
            if (artistNode != NULL) {

                // make doubly-sure we're focused on the correct letter
                mState.setAlphaChar( artistNode->getName() );

                artistNode->select();

                Node* albumNode = getPlayingAlbumNode( playingTrack, artistNode );
                if (albumNode != NULL) {

                    albumNode->select();

                    // TODO: only select the track automatically if we're already at track level
//                    if ( selectedNode && selectedNode->mGen == G_TRACK_LEVEL ) {
                        // TODO: let's not do this if the current playing album and artist don't match
                        //       the transition is too jarring/annoying
                        //       better to use this opportunity to update info about the currently playing track
                        Node* trackNode = getPlayingTrackNode( playingTrack, albumNode );
						NodeTrack* tn = (NodeTrack*)trackNode;
                        if (trackNode != NULL) {
							
							tn->setStartAngle();
                            if (!trackNode->mIsSelected) {
                                //console() << "    selecting track node" << std::endl;
                                // this one gets selected in World
                                mState.setSelectedNode(trackNode);
                            }
//                            else {
//                                console() << "    track node already selected" << std::endl;                            
//                            }
                        }
                        else {
                            // TODO: log this in Flurry, with track details and current state details
                            console() << "  track changed to a track we don't have a track node for - ideally this should never happen" << std::endl;
                        }
                    
//                    }
//                    else {
//                        console() << "    not selecting track node because it's too whooshy" << std::endl;                                                    
//                        // make sure updateIsPlaying is correct though...
//                        updateIsPlaying();
//                    }
                    
                }
                else {
                    // TODO: log this in Flurry, with track details and current state details
                    console() << "  track changed to a track we don't have an album node for - ideally this should never happen" << std::endl;
                }                
            }
            else {
                // TODO: log this in Flurry, with track details and current state details
                console() << "  track changed to a track we don't have an artist node for - ideally this should never happen" << std::endl;
            }
        }
//        else {
//			// For when you have tapped "fly to current track" 
//			onSelectedNodeChanged( selectedNode );
//            console() << "    track changed but we've already selected the node for that track" << std::endl;
//        }
        
	}
	else {
//		console() << "    trackchanged but nothing's playing" << endl;

        mPlayControls.setCurrentTrack("");
		// TOM: I put this next line in. Is this how to go to album level view when last track ends?
		mState.setSelectedNode( mState.getSelectedAlbumNode() );
        // FIXME: disable play button and zoom-to-current-track button

	}

    // TODO: profile with Flurry start/stopTimedEvent?
    
//    console() << "onPlayerTrackChanged done in " << (getElapsedSeconds() - mElapsedSecondsSinceTrackChange) << " seconds" << std::endl;
//	console() << "==================================================================" << std::endl;

    return false;
}

bool KeplerApp::onPlayerStateChanged( ipod::Player *player )
{	
    std::map<string, string> params;
    params["State"] = player->getPlayState();
    Flurry::getInstrumentation()->logEvent("Player State Changed", params);
    
    if ( mState.getSelectedNode() == NULL ) {
        onPlayerTrackChanged( player );
    }
    mPlayControls.setPlaying(player->getPlayState() == ipod::Player::StatePlaying);
    updateIsPlaying();    
    return false;
}

void KeplerApp::updateIsPlaying()
{
    // update mIsPlaying state for all nodes...
    if ( mIpodPlayer.hasPlayingTrack() ){
        ipod::TrackRef track = mIpodPlayer.getPlayingTrack();
        mWorld.setIsPlaying( track->getArtistId(), track->getAlbumId(), track->getItemId() );
    }
    else {
        // this should be OK to do since the above will happen if something is queued and paused
        mWorld.setIsPlaying( 0, 0, 0 );
    }
}


Node* KeplerApp::getPlayingTrackNode( ipod::TrackRef playingTrack, Node* albumNode )
{
    if (albumNode != NULL) {
        for (int k = 0; k < albumNode->mChildNodes.size(); k++) {
            // FIXME: what's the proper C++ way to do this cast?
            NodeTrack *trackNode = (NodeTrack*)(albumNode->mChildNodes[k]);
            if (trackNode->getId() == playingTrack->getItemId()) {
                return trackNode;
            }
        }
    }
    return NULL;
}

Node* KeplerApp::getPlayingAlbumNode( ipod::TrackRef playingTrack, Node* artistNode )
{
    if (artistNode != NULL) {
        uint64_t albumId = playingTrack->getAlbumId();
        for (int j = 0; j < artistNode->mChildNodes.size(); j++) {					
            Node* albumNode = artistNode->mChildNodes[j];
            if (albumNode->getId() == albumId) {
                return albumNode;
            }
        }
    }
    return NULL;
}

Node* KeplerApp::getPlayingArtistNode( ipod::TrackRef playingTrack )
{
    uint64_t artistId = playingTrack->getArtistId();    
    for (int i = 0; i < mWorld.mNodes.size(); i++) {
        Node* artistNode = mWorld.mNodes[i];
        if (artistNode->getId() == artistId) {
            return artistNode;
        }
    }    
    return NULL;
}

void KeplerApp::shutdown()
{
    [motionManager stopDeviceMotionUpdates];
    [motionManager release];
    [referenceAttitude release];
}


CINDER_APP_COCOA_TOUCH( KeplerApp, RendererGl )
