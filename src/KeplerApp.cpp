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
#include "AlphaWheel.h"
#include "State.h"
#include "Data.h"
#include "PlayControls.h"
//#include "Breadcrumbs.h"
#include "CinderIPod.h"
#include "CinderIPodPlayer.h"
#include "PinchRecognizer.h"
#include "ParticleController.h"
#include "LoadingScreen.h"
#include "NodeArtist.h"

#include "CinderFlurry.h"
//#include "TextureLoader.h"
#include <sys/sysctl.h>
#include <vector>
#include <sstream>
#import <CoreMotion/CoreMotion.h>


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
	void			updatePlayhead();
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
//	bool			onBreadcrumbSelected ( BreadcrumbEvent event );
	bool			onPlayControlsButtonPressed ( PlayControls::PlayButton button );
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
	
	//NodeTrack*		mCurrentTrackNode;
	
    LoadingScreen   mLoadingScreen;
	World			mWorld;
	State			mState;
	AlphaWheel		mAlphaWheel;
	UiLayer			mUiLayer;
	HelpLayer		mHelpLayer;
	Data			mData;

// ORIENTATION
    OrientationHelper mOrientationHelper;    
    Orientation     mInterfaceOrientation;
    Matrix44f       mOrientationMatrix;
    Matrix44f       mInverseOrientationMatrix;    
	Quatf			mGyroQuat;
    std::map<Orientation, std::map<Orientation, int> > mRotationSteps;
	// Objective C
    CMMotionManager *motionManager;
    CMAttitude *referenceAttitude;

	
	
// AUDIO
	ipod::Player		mIpodPlayer;
	ipod::PlaylistRef	mCurrentAlbum;
	double				mCurrentTrackPlayheadTime;
	double				mCurrentTrackLength;
	float				mElapsedSecondsSinceTrackChange;
	int					mPlaylistIndex;
	
// BREADCRUMBS
//	Breadcrumbs		mBreadcrumbs;	
	
// PLAY CONTROLS
	PlayControls	mPlayControls;
	float			mPlayheadPer;	// song percent complete
	
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
	
	
	
// FONTS
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
	gl::Texture		mStarTex, mStarGlowTex, mEclipseGlowTex;
	gl::Texture		mSkyDome, mGalaxyDome;
	gl::Texture		mDottedTex;
	gl::Texture		mPlayheadProgressTex;
    gl::Texture     mRingsTex;
	gl::Texture		mUiButtonsTex;
	gl::Texture		mCurrentTrackTex;
    gl::Texture		mAtmosphereTex, mAtmosphereDirectionalTex;
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
		G_NUM_PARTICLES = 40;
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
    
	mElapsedSecondsSinceTrackChange = 0.0f;

	
    // TEXTURES
    initTextures();
	
	
	// ARCBALL
	mMatrix	= Quatf();
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( getWindowCenter() );
	mArcball.setRadius( G_DEFAULT_ARCBALL_RADIUS );
	mArcball.setQuat( Quatf( 1.0, 0.0f, 0.0f ) );
	
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
	mFont				= Font( loadResource( "UnitRoundedOT-Medi.otf" ), 14 );
	mFontBig			= Font( loadResource( "UnitRoundedOT-Ultra.otf"), 256 );
	mFontMediSmall		= Font( loadResource( "UnitRoundedOT-Medi.otf" ), 12 );
	mFontMediTiny		= Font( loadResource( "UnitRoundedOT-Medi.otf" ), 10 );
	
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
    
	// BREADCRUMBS
//	mBreadcrumbs.setup( this, mFontMediSmall, mOrientationHelper.getInterfaceOrientation() );
//	mBreadcrumbs.registerBreadcrumbSelected( this, &KeplerApp::onBreadcrumbSelected );
//	mBreadcrumbs.setHierarchy(mState.getHierarchy());

	// PLAY CONTROLS
	mPlayControls.setup( this, mIpodPlayer.getPlayState() == ipod::Player::StatePlaying, mOrientationHelper.getInterfaceOrientation() );
	mPlayControls.registerButtonPressed( this, &KeplerApp::onPlayControlsButtonPressed );
	mPlayControls.registerPlayheadMoved( this, &KeplerApp::onPlayControlsPlayheadMoved );

	// UILAYER
	mUiLayer.setup( this, mOrientationHelper.getInterfaceOrientation() );

	// HELP LAYER
	mHelpLayer.setup( this, mOrientationHelper.getInterfaceOrientation() );
	mHelpLayer.initHelpTextures( mFontMediSmall );
	
    // ALPHA WHEEL
	mAlphaWheel.setup( this, mOrientationHelper.getInterfaceOrientation() );
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
	mEclipseGlowTex		= loadImage( loadResource( "eclipseGlow.png" ) );
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
    mAtmosphereTex		= loadImage( loadResource( "atmosphere.png" ) );
	mAtmosphereDirectionalTex = loadImage( loadResource( "atmosphereDirectional.png" ) );
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
        Vec3f worldTouchPos = mInverseOrientationMatrix * Vec3f(mTouchPos,0);
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
                Vec3f worldTouchPos = mInverseOrientationMatrix * Vec3f(mTouchPos,0);
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
	
	Node *selectedNode = mState.getSelectedNode();
	int currentLevel = 0;
	if( selectedNode ){
		currentLevel = selectedNode->mGen;
	}
	
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
//    bool belowBreadcrumbs = worldPos.y > mBreadcrumbs.getHeight();
    bool notTab = !mUiLayer.getPanelTabRect().contains(worldPos);
    bool valid = aboveUI && notTab;// && belowBreadcrumbs;
    return valid;
}


bool KeplerApp::orientationChanged( OrientationEvent event )
{
    Orientation orientation = event.getInterfaceOrientation();
    mLoadingScreen.setInterfaceOrientation(orientation);
    if (mDataIsLoaded) {
        mPlayControls.setInterfaceOrientation(orientation);
        mHelpLayer.setInterfaceOrientation(orientation);
        mUiLayer.setInterfaceOrientation(orientation);
        mAlphaWheel.setInterfaceOrientation(orientation);
//        mBreadcrumbs.setInterfaceOrientation(orientation);    
    }
    setInterfaceOrientation(orientation);

    std::map<string, string> params;
    params["Device Orientation"] = getOrientationString(event.getDeviceOrientation());
    Flurry::getInstrumentation()->logEvent("Orientation Changed", params);    
    
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
    
    return false;
}

void KeplerApp::setInterfaceOrientation( const Orientation &orientation )
{
    mInterfaceOrientation = orientation;
    mOrientationMatrix = getOrientationMatrix44( mInterfaceOrientation, getWindowSize() );
    mInverseOrientationMatrix = mOrientationMatrix.inverted();
    mUp = getUpVectorForOrientation( mInterfaceOrientation );    
}

bool KeplerApp::onWheelToggled( AlphaWheel *alphaWheel )
{
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
	return false;
}

bool KeplerApp::onAlphaCharStateChanged( State *state )
{
	mData.filterArtistsByAlpha( mState.getAlphaChar() );

    std::map<string, string> parameters;
    parameters["Letter"] = ""+mState.getAlphaChar();
    parameters["Count"] = ""+mData.mFilteredArtists.size();
    Flurry::getInstrumentation()->logEvent("Letter Selected" , parameters);

	//console() << "Letter " << mState.getAlphaChar() << " has " << mData.mFilteredArtists.size() << " artists" << std::endl;
	
	mWorld.filterNodes();
//	mBreadcrumbs.setHierarchy( mState.getHierarchy() );	
    
    mState.setSelectedNode( NULL );
    
	return false;
}

bool KeplerApp::onPlaylistStateChanged( State *state )
{
	std::cout << "playlist changed" << std::endl;
	mData.filterArtistsByPlaylist( mState.getPlaylist() );
	mPlayControls.createPlaylistTexture( mState.getPlaylistName(), mFontMediSmall );
	
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

bool KeplerApp::onPlayControlsButtonPressed( PlayControls::PlayButton button )
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
            }
            else {
                mIpodPlayer.play();
            }
            break;
        
        case PlayControls::NEXT_TRACK:
            Flurry::getInstrumentation()->logEvent("Next Track Button Selected");            
            mIpodPlayer.skipNext();	
            break;
        
        case PlayControls::HELP:
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
			}
            break;
        
        case PlayControls::DRAW_TEXT:
			if( G_SHOW_SETTINGS ){
				Flurry::getInstrumentation()->logEvent("Draw Text Button Selected");            
				G_DRAW_TEXT = !G_DRAW_TEXT;
			}
            break;
        
		case PlayControls::USE_GYRO:
			if( G_SHOW_SETTINGS ){
				Flurry::getInstrumentation()->logEvent("Use Gyro Button Selected");            
				G_USE_GYRO = !G_USE_GYRO;
			}
            break;
			
		case PlayControls::GOTO_GALAXY:
            Flurry::getInstrumentation()->logEvent("Galaxy Button Selected");            
			mWorld.deselectAllNodes();
			mState.setSelectedNode( NULL );
			mState.setAlphaChar( ' ' );
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
            // TODO: Flurry log?
            break;
        
		case PlayControls::DEBUG_FEATURE:
			G_DEBUG = !G_DEBUG;
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
		if( G_IS_IPAD2 && G_USE_GYRO )
			updateGyro();
		updateArcball();
		updatePlayhead();
		if( mWorld.mPlayingTrackNode && G_ZOOM > G_ARTIST_LEVEL ){
			mWorld.mPlayingTrackNode->updateAudioData( mCurrentTrackPlayheadTime );
		}
		
        mWorld.update( mMatrix );
		
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
			mParticleController.update( mMatrix.inverted() * mCenter, selectedArtistNode->mRadius * 0.16f, invBbRight, invBbUp );
			float per = selectedArtistNode->mEclipseStrength * 0.5f + 0.25f;
			mParticleController.buildParticleVertexArray( selectedArtistNode->mGlowColor, sin( per * M_PI ) * sin( per * 0.25f ) );
			mParticleController.buildDustVertexArray( selectedArtistNode, mPinchAlphaPer, ( 1.0f - mCamRingAlpha ) * 0.05f * mFadeInArtistToAlbum );
		}
		/*else {
			mParticleController.update( 100.0f, invBbRight, invBbUp );
			mParticleController.buildParticleVertexArray();
			mParticleController.buildDustVertexArray( NULL, mPinchAlphaPer, sqrt( 1.0f - mCamRingAlpha ) );
		}*/
		
        mUiLayer.update();
		mHelpLayer.update();
		mAlphaWheel.update( mFov );
//        mBreadcrumbs.update();
        mPlayControls.update();
    }
    else {
        // make sure we've drawn the loading screen first
        if (getElapsedFrames() > 1) {
            remainingSetup();
        }        
    }
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
        Vec3f downPos = mInverseOrientationMatrix * ( Vec3f(mTouchPos,0) );
        mArcball.mouseDown( Vec2i(downPos.x, downPos.y) );
		
        Vec3f dragPos = mInverseOrientationMatrix * ( Vec3f(mTouchPos + mTouchVel,0) );
        mArcball.mouseDrag( Vec2i(dragPos.x, dragPos.y) );        
	}
	
	
	if( G_IS_IPAD2 ){
		mMatrix = mArcball.getQuat() * mGyroQuat;
	} else {
		mMatrix = mArcball.getQuat();
	}
}


void KeplerApp::updateCamera()
{	
	mPinchTotal -= ( mPinchTotal - mPinchTotalDest ) * 0.4f;
	mPinchPer = ( mPinchTotal - mPinchScaleMin )/( mPinchScaleMax - mPinchScaleMin );
	mPinchHighlightRadius -= ( mPinchHighlightRadius - 200.0f ) * 0.4f;
	
	float cameraDistMulti = mPinchPer * 2.0f + 0.5f;
	
	if( mPinchPer > mPinchPerThresh ){
		if( ! mIsPastPinchThresh ){
			mPinchHighlightRadius = 650.0f;
			mIsPastPinchThresh = true;
		}
		
		
		
		mPinchAlphaPer -= ( mPinchAlphaPer ) * 0.1f;
		
		if( G_CURRENT_LEVEL == G_TRACK_LEVEL ){
			mFovDest = 70.0f;
		} else if( G_CURRENT_LEVEL == G_ALBUM_LEVEL ){
			mFovDest = 85.0f;
		} else if( G_CURRENT_LEVEL == G_ARTIST_LEVEL ){
			mFovDest = 85.0f;
		} else {
			mFovDest = G_DEFAULT_FOV;
		}
		mFov -= ( mFov - mFovDest ) * 0.2f;
		
	} else {
		if( mIsPastPinchThresh ){
			mPinchHighlightRadius = 125.0f;
			mIsPastPinchThresh = false;
		}
		
		mPinchAlphaPer -= ( mPinchAlphaPer - 1.0f ) * 0.1f;
		mFovDest = G_DEFAULT_FOV;
		
		mFov -= ( mFov - mFovDest ) * 0.04f;
	}
	
	
	int currentLevel = 0;
	Node* selectedNode = mState.getSelectedNode();
	if( selectedNode ){
		currentLevel	= selectedNode->mGen;
		mCamDistDest	= selectedNode->mIdealCameraDist * cameraDistMulti;
		
		
		
		if( selectedNode->mParentNode && mPinchPer > mPinchPerThresh ){
			Vec3f dirToParent = selectedNode->mParentNode->mTransPos - selectedNode->mTransPos;
			mCenterOffset -= ( mCenterOffset - ( dirToParent * ( mPinchPer - mPinchPerThresh ) * 2.5f ) ) * 0.1f;
			
		} else {
			mCenterOffset -= ( mCenterOffset - Vec3f::zero() ) * 0.05f;
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
	
	
	if( mPinchPer > mPinchPerThresh && ! mAlphaWheel.getShowWheel() && currentLevel <= G_ALPHA_LEVEL ){
		mAlphaWheel.setShowWheel( true ); 
		
	} else if( mPinchPer <= mPinchPerThresh && mAlphaWheel.getShowWheel() ){
		mAlphaWheel.setShowWheel( false ); 
	}
	

//	float distToTravel = mState.getDistBetweenNodes();
//	double duration = 3.0f;
//	if( distToTravel < 1.0f )		duration = 2.0;
//	else if( distToTravel < 5.0f )	duration = 2.75f;
	double duration = 2.0f;
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
	
	
	Vec3f prevEye	= mEye;
	mEye			= Vec3f( mCenter.x, mCenter.y, mCenter.z - mCamDist );//- sin( mCamDistAnim ) * distToTravel * 0.25f );
	mCamVel			= mEye - prevEye;

	mCam.setPerspective( mFov, getWindowAspectRatio(), 0.001f, 2000.0f );
	mCam.lookAt( mEye - mCenterOffset, mCenter, mUp );
	mCam.getBillboardVectors( &mBbRight, &mBbUp );
	mCamNormal = mEye - mCenter;
	mCamNormal.normalize();
}

void KeplerApp::updatePlayhead()
{
	//if( mIpodPlayer.getPlayState() == ipod::Player::StatePlaying ){
		mCurrentTrackPlayheadTime	= mIpodPlayer.getPlayheadTime();
        // TODO: cache this when playing track changes
		mCurrentTrackLength			= mIpodPlayer.getPlayingTrack()->getLength();
	//}
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
	vector<Node*> sortedNodes = mWorld.getDepthSortedNodes( G_ALBUM_LEVEL, G_TRACK_LEVEL );
	Node *artistNode	= mState.getSelectedArtistNode();
	
	
	// For doing galaxy-axis fades
	Vec3f transEye = mMatrix.inverted() * mEye;
	float zoomOff = 1.0f - mFadeInAlphaToArtist;//constrain( ( G_ARTIST_LEVEL - G_ZOOM ), 0.0f, 1.0f );
	float camGalaxyAlpha = constrain( abs( transEye.y ) * 0.004f, 0.0f, 1.0f );
	float alpha, radius;
	float invAlpha = pow( 1.0f - camGalaxyAlpha, 2.5f ) * zoomOff;
	
    gl::enableDepthWrite();
    gl::setMatrices( mCam );
	
// SKYDOME
    gl::pushModelView();
    gl::rotate( mMatrix );
	Color c = Color( CM_HSV, mPinchPer * 0.2f + 0.475f, 1.0f - mPinchPer * 0.5f, 1.0f );
	if( mIsPastPinchThresh )
		c = Color( CM_HSV, mPinchPer * 0.3f + 0.7f, 1.0f - mPinchPer * 0.5f, 1.0f );
    gl::color( c * pow( 1.0f - zoomOff, 3.0f ) );
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
	
	
//// ECLIPSEGLOWS OVERLAY
//	mEclipseGlowTex.enableAndBind();
//	mWorld.drawEclipseGlows();
//	mEclipseGlowTex.disable();


// STARGLOWS occluded
//	mEclipseGlowTex.enableAndBind();
//	mWorld.drawStarGlowsVertexArray( mMatrix );
//	mEclipseGlowTex.disable();



// STARGLOWS bloom (TOUCH HIGHLIGHTS)
	mEclipseGlowTex.enableAndBind();
	mWorld.drawTouchHighlights( mFadeInArtistToAlbum );
	mEclipseGlowTex.disable();
	
	
	if( artistNode ){ // defined at top of method
		//float zoomOffset = constrain( 1.0f - ( G_ALBUM_LEVEL - G_ZOOM ), 0.0f, 1.0f );
		mCamRingAlpha = constrain( abs( transEye.y - artistNode->mPos.y ), 0.0f, 1.0f ); // WAS 0.6f

		glCullFace( GL_BACK );
		glEnable( GL_CULL_FACE );
		glEnable( GL_COLOR_MATERIAL );
		glEnable( GL_RESCALE_NORMAL );
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, ColorA( 0.1f, 0.025f, 0.01f, 1.0f ) );
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, ColorA( Color::white(), 1.0f ) );
//		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, ColorA( Color::white(), 1.0f ) );
//		glMaterialf(  GL_FRONT_AND_BACK, GL_SHININESS, 20.0f );
		
		for( int i = 0; i < sortedNodes.size(); i++ ){
			gl::enableDepthRead();
			glEnable( GL_LIGHTING );
			
			// LIGHT FROM ARTIST
			glEnable( GL_LIGHT0 );
			Vec3f lightPos          = artistNode->mTransPos;
			GLfloat artistLight[]	= { lightPos.x, lightPos.y, lightPos.z, 1.0f };
			glLightfv( GL_LIGHT0, GL_POSITION, artistLight );
			glLightfv( GL_LIGHT0, GL_DIFFUSE, ColorA( artistNode->mColor, 1.0f ) );
//			glLightfv( GL_LIGHT0, GL_SPECULAR, ColorA( artistNode->mGlowColor, 1.0f ) );

	
			gl::enableAlphaBlending();
			//glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, ColorA( 0.4f, 0.1f, 0.0f, 1.0f ) );
			sortedNodes[i]->drawPlanet();

			//glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, ColorA( 0.0f, 0.0f, 0.0f, 1.0f ) );
			sortedNodes[i]->drawClouds( mCloudsTex );
			
			gl::disableAlphaBlending();
			glDisable( GL_LIGHTING );
			gl::disableDepthRead();
			
			if( sortedNodes[i]->mGen == G_ALBUM_LEVEL ){
				sortedNodes[i]->drawAtmosphere( mAtmosphereTex, mAtmosphereDirectionalTex, mPinchAlphaPer );	
			}
			
			if( sortedNodes[i]->mGen == G_TRACK_LEVEL ){//&& sortedNodes[i]->isMostPlayed() ){
				sortedNodes[i]->drawAtmosphere( mAtmosphereTex, mAtmosphereDirectionalTex, mPinchAlphaPer );
			}
		}
		glDisable( GL_CULL_FACE );
		glDisable( GL_RESCALE_NORMAL );
	}
	
	
	glDisable( GL_LIGHTING );
	gl::enableAdditiveBlending();
	gl::disableDepthRead();
	
// STARGLOWS bloom
	mStarGlowTex.enableAndBind();
	mWorld.drawStarGlowsVertexArray( mMatrix );
	mStarGlowTex.disable();
	
	
    
	gl::enableDepthRead();	
	gl::disableDepthWrite();
	gl::enableAdditiveBlending();
	
	
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
		if( G_DRAW_RINGS )
			mWorld.mPlayingTrackNode->drawPlayheadProgress( mPinchAlphaPer, mCamRingAlpha, mPlayheadProgressTex, mTrackOriginTex );
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
	
	
	
    
    gl::disableAlphaBlending();
    gl::enableAlphaBlending();
	
	// EVERYTHING ELSE
	
	mAlphaWheel.draw( mData.mWheelDataVerts, mData.mWheelDataTexCoords, mData.mWheelDataColors );
	mHelpLayer.draw( mUiButtonsTex, mUiLayer.getPanelYPos() );
    mUiLayer.draw( mUiButtonsTex );
//    mBreadcrumbs.draw( mUiButtonsTex, mUiLayer.getPanelYPos() );
    mPlayControls.draw( mInterfaceOrientation, mUiButtonsTex, mCurrentTrackTex, mFontMediSmall, mUiLayer.getPanelYPos(), mCurrentTrackPlayheadTime, mCurrentTrackLength, mElapsedSecondsSinceTrackChange );
	gl::disableAlphaBlending();
	if( G_DEBUG ) drawInfoPanel();
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
	layout.setFont( mFontMediSmall );
	layout.setColor( Color( 1.0f, 0.0f, 0.0f ) );

	s.str("");
	s << "FPS: " << getAverageFps();
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
    
    mElapsedSecondsSinceTrackChange = getElapsedSeconds();

    Flurry::getInstrumentation()->logEvent("Player Track Changed");

	if (mIpodPlayer.hasPlayingTrack()) {
        
		ipod::TrackRef playingTrack = mIpodPlayer.getPlayingTrack();

	// Create current track text texture
		TextLayout layout;
		layout.setFont( mFontMediSmall );
		layout.setColor( Color::white() );			
		layout.addLine( " " + playingTrack->getArtist() + " • " + playingTrack->getAlbumTitle() + " • " + playingTrack->getTitle() + " " );
		bool PREMULT = false;
		mCurrentTrackTex = gl::Texture( layout.render( true, PREMULT ) );
		 
        
        Node *selectedNode = mState.getSelectedNode();
        if (!(selectedNode != NULL && selectedNode->getId() == playingTrack->getItemId())) {
        
            Node* artistNode = getPlayingArtistNode( playingTrack );
            if (artistNode != NULL) {

                // ensure that breadcrumbs are consistent
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
		mCurrentTrackTex.reset();
		// TOM: I put this next line in. Is this how to go to album level view when last track ends?
		mState.setSelectedNode( mState.getSelectedAlbumNode() );
        // FIXME: disable zoom-to-current-track button
	}

    updatePlayhead();
    
    // TODO: profile with Flurry start/stopTimedEvent?
    
//    console() << "onPlayerTrackChanged done in " << (getElapsedSeconds() - mElapsedSecondsSinceTrackChange) << " seconds" << std::endl;
//	console() << "==================================================================" << std::endl;

    return false;
}

bool KeplerApp::onPlayerStateChanged( ipod::Player *player )
{	
    // TODO: a tidy getStringForPlayerState function?
    std::map<string, string> params;
    switch(player->getPlayState()) {
        case ipod::Player::StateStopped:
            params["State"] = "Stopped";
            break;
        case ipod::Player::StatePlaying:
            params["State"] = "Playing";
            break;
        case ipod::Player::StatePaused:
            params["State"] = "Paused";
            break;
        case ipod::Player::StateInterrupted:
            params["State"] = "Interrupted";
            break;
        case ipod::Player::StateSeekingForward:
            params["State"] = "Seeking Forward";
            break;
        case ipod::Player::StateSeekingBackward:
            params["State"] = "Seeking Backward";
            break;
    }
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
