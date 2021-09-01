#pragma once

#include "cinder/Cinder.h"
#include "cinder/Surface.h"
#include "cinder/Function.h"
#include "cinder/cocoa/CinderCocoaTouch.h"

#include <MediaPlayer/MediaPlayer.h>

#include <vector>
#include <string>
#include <ostream>

using std::string;
using std::vector;

namespace cinder { namespace ipod {

class Track {
public:

    Track();
    Track(MPMediaItem *media_item);
    ~Track();

    string   getTitle();
    string   getAlbumTitle();
    string   getArtist();
	string	 getAlbumArtist();

    uint64_t getAlbumId();
    uint64_t getArtistId();
	uint64_t getItemId();

    int      getPlayCount();
    int      getStarRating();
    double   getLength();
//    double   getReleaseDate();  // seconds since 1970
    int      getReleaseYear();  // four digit year, e.g. 2011, 1992
//    int      getReleaseMonth(); // 1-12 (not 0-11 like Java!)
//    int      getReleaseDay();   // 1-31

    Surface getArtwork(const Vec2i &size);

    MPMediaItem* getMediaItem(){
        return m_media_item;
    };

protected:

    MPMediaItem *m_media_item;
    
    uint64_t m_artist_id;
    bool m_artist_id_cached;

    uint64_t m_album_id;
    bool m_album_id_cached;

    uint64_t m_id;
    bool m_id_cached;

};

typedef std::shared_ptr<Track> TrackRef;


class Playlist {
public:

    typedef vector<TrackRef>::iterator Iter;

    Playlist();
    Playlist(MPMediaItemCollection *collection, string playlist_name="Untitled");
    ~Playlist();

    void pushTrack(TrackRef track);
    void pushTrack(Track *track);
    void popLastTrack(){ m_tracks.pop_back(); };

	string getGenre();
    string getAlbumTitle();
    string getArtistName();
	string getAlbumArtistName();
    string getPlaylistName();
    uint64_t getAlbumId();
    uint64_t getArtistId();
    double getTotalLength();

    TrackRef operator[](const int index){ return m_tracks[index]; };
    TrackRef firstTrack(){ return m_tracks.front(); };
    TrackRef lastTrack(){ return m_tracks.back(); };
    Iter   begin(){ return m_tracks.begin(); };
    Iter   end(){ return m_tracks.end(); };
    size_t size(){ return m_tracks.size(); };

    MPMediaItemCollection* getMediaItemCollection();

protected:
    
    vector<TrackRef> m_tracks;
    string m_playlist_name;

    string m_artist_name;
    bool m_artist_name_cached;
    
    uint64_t m_artist_id;
    bool m_artist_id_cached;

};

typedef std::shared_ptr<Playlist> PlaylistRef;

PlaylistRef         getAllTracks();
PlaylistRef         getAlbum(const uint64_t &album_id);
PlaylistRef         getArtist(const uint64_t &artist_id);
vector<PlaylistRef> getAlbums();
vector<PlaylistRef> getAlbumsWithArtist(const string &artist_name);
vector<PlaylistRef> getAlbumsWithArtistId(const uint64_t &artist_id);
PlaylistRef         getAlbumPlaylistWithArtistId(const uint64_t &artist_id);
vector<PlaylistRef> getArtists( std::function<void(float)> progress );
vector<PlaylistRef> getPlaylists( std::function<void(float)> progress );
    
} } // namespace cinder::ipod
