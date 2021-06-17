#include "cinder/app/AppCocoaTouch.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"

#include "CinderIPod.h"
#include "CinderIPodPlayer.h"

using namespace ci;
using namespace ci::app;
using namespace ci::ipod;
using namespace std;


class cinder_ipod_testApp : public AppCocoaTouch {
  public:
    void prepareSettings(Settings *settings);
	void setup();
	void update();
	void draw();

    void touchesBegan(TouchEvent event);

    bool onStateChanged(Player *player);
    bool onTrackChanged(Player *player);

    Player player;

	vector<gl::Texture> tex;
};


void cinder_ipod_testApp::prepareSettings(Settings *settings)
{
    settings->enableMultiTouch(true);
}

void cinder_ipod_testApp::setup()
{
    vector<ipod::PlaylistRef> albums = getAlbums();

    // Load Album Art
	int i = 0;
    for(vector<PlaylistRef>::iterator it = albums.begin(); it != albums.end() && i++ < 64; ++it){
        PlaylistRef &album = *it;

        console() << album->getAlbumTitle() << endl;

        Surface8u art = (*album)[0]->getArtwork(Vec2i(128, 128));
        if(art)
            tex.push_back(gl::Texture(art));
    }

    // Show albums by the first artist
    string first_artist = albums[0]->getArtistName();
    console() << endl << "Albums by " << first_artist << ":" << endl;
    vector<PlaylistRef> artist_albums = ipod::getAlbumsWithArtist(first_artist);
    for(vector<PlaylistRef>::iterator it = artist_albums.begin(); it != artist_albums.end(); ++it){
        PlaylistRef album = *it;
        console() << album->getAlbumTitle() << endl;
    }

    // Show tracks in the first album
    PlaylistRef first_album = albums[0];
    console() << endl << "Tracks in " << first_album->getAlbumTitle() << ":" << endl;
    for(Playlist::Iter it = first_album->begin(); it != first_album->end(); ++it){
        TrackRef track = *it;
		console() << track->getTitle() << endl;
    }

    // Play the first track
    player.registerStateChanged(this, &cinder_ipod_testApp::onStateChanged);
    player.registerTrackChanged(this, &cinder_ipod_testApp::onTrackChanged);
}

void cinder_ipod_testApp::update()
{
}

void cinder_ipod_testApp::draw()
{
    gl::clear(Color(0,0,0));
    gl::setMatricesWindow(getWindowSize(), true);
    float x = 0, y = 0;
    for(vector<gl::Texture>::iterator it = tex.begin(); it != tex.end(); ++it){
    	glPushMatrix();
        glTranslatef(x, y, 0);
        gl::draw(*it);
        glPopMatrix();
        x += it->getWidth();
        if(x > getWindowWidth()){
            x = 0;
            y += it->getHeight();
        }
        if(y > getWindowHeight())
            break;
    }
}


void cinder_ipod_testApp::touchesBegan(TouchEvent event)
{
    if(player.getPlayState() != Player::StatePlaying)
        player.play(getAlbums()[0], 0);
    else
    	player.skipNext();
}


bool cinder_ipod_testApp::onStateChanged(Player *player)
{
    switch(player->getPlayState()){
        case Player::StatePlaying:
            console() << "Playing..." << endl;
            break;
        case Player::StateStopped:
            console() << "Stopped." << endl;
            break;
    }
    return false;
}

bool cinder_ipod_testApp::onTrackChanged(Player *player)
{
    console() << "Now Playing: " << player->getPlayingTrack()->getTitle() << endl;
    return false;
}


CINDER_APP_COCOA_TOUCH( cinder_ipod_testApp, RendererGl )
