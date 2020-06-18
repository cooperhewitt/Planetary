#include "CinderIPod.h"

namespace cinder { namespace ipod {


// TRACK

Track::Track(): m_artist_id_cached(false), m_album_id_cached(false), m_id_cached(false)
{
}
Track::Track(MPMediaItem *media_item): m_artist_id_cached(false), m_album_id_cached(false), m_id_cached(false)
{
    m_media_item = [media_item retain];
}
Track::~Track()
{
}

string Track::getTitle()
{
    return string([[m_media_item valueForProperty: MPMediaItemPropertyTitle] UTF8String]);
}
string Track::getAlbumTitle()
{
    return string([[m_media_item valueForProperty: MPMediaItemPropertyAlbumTitle] UTF8String]);
}
string Track::getArtist()
{
    return string([[m_media_item valueForProperty: MPMediaItemPropertyArtist] UTF8String]);
}
string Track::getAlbumArtist()
{
	return string([[m_media_item valueForProperty: MPMediaItemPropertyAlbumArtist] UTF8String]);
}
uint64_t Track::getAlbumId()
{
    if (!m_album_id_cached) {
        m_album_id = [[m_media_item valueForProperty: MPMediaItemPropertyAlbumPersistentID] longLongValue];
        m_album_id_cached = true;
    }
    return m_album_id;
}
uint64_t Track::getArtistId()
{
    if (!m_artist_id_cached) {
        m_artist_id = [[m_media_item valueForProperty: MPMediaItemPropertyArtistPersistentID] longLongValue];
        m_artist_id_cached = true;
    }
    return m_artist_id;
}
uint64_t Track::getItemId()
{
    if (!m_id_cached) {
        m_id = [[m_media_item valueForProperty: MPMediaItemPropertyPersistentID] longLongValue];
        m_id_cached = true;
    }
    return m_id;
}

int Track::getPlayCount()
{
	return [[m_media_item valueForProperty: MPMediaItemPropertyPlayCount] intValue];
}
	
int Track::getStarRating()
{
	return [[m_media_item valueForProperty: MPMediaItemPropertyRating] intValue];
}
	
//double Track::getReleaseDate()
//{
//    return [[m_media_item valueForProperty: MPMediaItemPropertyReleaseDate] timeIntervalSince1970];
//}
int Track::getReleaseYear()
{
//    NSDate *date = [m_media_item valueForProperty: MPMediaItemPropertyReleaseDate];
//    NSCalendar *gregorian = [[NSCalendar alloc]
//                             initWithCalendarIdentifier:NSGregorianCalendar];    
//    NSDateComponents *components = [gregorian components: NSYearCalendarUnit fromDate: date];
//    NSInteger year = [components year];
//    std::cout << getTitle() << " released " << year << std::endl;
//    [gregorian release];
//    return year;
    // I kiss you http://stackoverflow.com/questions/4862646/get-album-year-for-item-in-ipod-library
    NSNumber *yearNumber = [m_media_item valueForProperty:@"year"];
    if (yearNumber && [yearNumber isKindOfClass:[NSNumber class]])
    {
        int year = [yearNumber intValue];
        if (year != 0)
        {
            return year;
        }
    }    
    return -1;
}
//int Track::getReleaseMonth()
//{
//    NSDate *date = [m_media_item valueForProperty: MPMediaItemPropertyReleaseDate];
//    NSCalendar *gregorian = [[NSCalendar alloc]
//                             initWithCalendarIdentifier:NSGregorianCalendar];    
//    NSDateComponents *components = [gregorian components: NSMonthCalendarUnit fromDate: date];
//    NSInteger year = [components year];
//    [gregorian release];
//    return year;
//}
//int Track::getReleaseDay()
//{
//    NSDate *date = [m_media_item valueForProperty: MPMediaItemPropertyReleaseDate];
//    NSCalendar *gregorian = [[NSCalendar alloc]
//                             initWithCalendarIdentifier:NSGregorianCalendar];    
//    NSDateComponents *components = [gregorian components: NSDayCalendarUnit fromDate: date];
//    NSInteger year = [components year];
//    [gregorian release];
//    return year;
//}
    
double Track::getLength()
{
    return [[m_media_item valueForProperty: MPMediaItemPropertyPlaybackDuration] doubleValue];
}

Surface Track::getArtwork(const Vec2i &size)
{
    MPMediaItemArtwork *artwork = [m_media_item valueForProperty: MPMediaItemPropertyArtwork];
    UIImage *artwork_img = [artwork imageWithSize: CGSizeMake(size.x, size.y)];

    if(artwork_img)
        return cocoa::convertUiImage(artwork_img, true);
    else
        return Surface();
}



// PLAYLIST

Playlist::Playlist(): m_artist_name_cached(false), m_artist_id_cached(false), m_playlist_name("Untitled") { }

Playlist::Playlist(MPMediaItemCollection *media_collection, string playlist_name): m_artist_name_cached(false), m_artist_id_cached(false), m_playlist_name(playlist_name)
{
    NSArray *items = [media_collection items];
    for(MPMediaItem *item in items){
        pushTrack(new Track(item));
    }
}
Playlist::~Playlist()
{
}

void Playlist::pushTrack(TrackRef track)
{
    m_tracks.push_back(track);
}
void Playlist::pushTrack(Track *track)
{
    m_tracks.push_back(TrackRef(track));
}
	
string Playlist::getGenre()
{
	MPMediaItem *item = [getMediaItemCollection() representativeItem];
	return string([[item valueForProperty: MPMediaItemPropertyGenre] UTF8String]);
}

string Playlist::getAlbumTitle()
{
    MPMediaItem *item = [getMediaItemCollection() representativeItem];
    return string([[item valueForProperty: MPMediaItemPropertyAlbumTitle] UTF8String]);
}

string Playlist::getArtistName()
{
    if (!m_artist_name_cached) {
        MPMediaItem *item = [getMediaItemCollection() representativeItem];
        m_artist_name = string([[item valueForProperty: MPMediaItemPropertyArtist] UTF8String]);
        m_artist_name_cached = true;
    }
    return m_artist_name;
}
    	
string Playlist::getAlbumArtistName()
{
	MPMediaItem *item = [getMediaItemCollection() representativeItem];
	return string([[item valueForProperty: MPMediaItemPropertyAlbumArtist] UTF8String]);
}
    
string Playlist::getPlaylistName()
{
    return m_playlist_name;
}

uint64_t Playlist::getAlbumId()
{
    MPMediaItem *item = [getMediaItemCollection() representativeItem];
    return [[item valueForProperty: MPMediaItemPropertyAlbumPersistentID] longLongValue];
}
    
uint64_t Playlist::getArtistId()
{
    if (!m_artist_id_cached) {
        MPMediaItem *item = [getMediaItemCollection() representativeItem];
        m_artist_id = [[item valueForProperty: MPMediaItemPropertyArtistPersistentID] longLongValue];
        m_artist_id_cached = true;
    }
    return m_artist_id;
}
    
double Playlist::getTotalLength()
{
    double length = 0;
    for(Iter it = m_tracks.begin(); it != m_tracks.end(); ++it){
        length += (*it)->getLength();
    }
    return length;
}    
    
MPMediaItemCollection* Playlist::getMediaItemCollection()
{
    NSMutableArray *items = [NSMutableArray array];
    for(Iter it = m_tracks.begin(); it != m_tracks.end(); ++it){
        [items addObject: (*it)->getMediaItem()];
    }
    return [MPMediaItemCollection collectionWithItems:items];
}

// IPOD

PlaylistRef getAllTracks()
{
    MPMediaQuery *query = [MPMediaQuery songsQuery];
    MPMediaItemCollection *tracks = [MPMediaItemCollection collectionWithItems: [query items]];

    return PlaylistRef(new Playlist(tracks));
}

PlaylistRef getAlbum(const uint64_t &album_id)
{
    MPMediaQuery *query = [[MPMediaQuery alloc] init];
    [query addFilterPredicate: [MPMediaPropertyPredicate
           predicateWithValue: [NSNumber numberWithInteger: MPMediaTypeMusic]
                  forProperty: MPMediaItemPropertyMediaType
    ]];
    [query addFilterPredicate: [MPMediaPropertyPredicate
           predicateWithValue: [NSNumber numberWithUnsignedLongLong: album_id]
                  forProperty: MPMediaItemPropertyAlbumPersistentID
    ]];
    MPMediaItemCollection *tracks = [MPMediaItemCollection collectionWithItems: [query items]];

    return PlaylistRef(new Playlist(tracks));
}

vector<PlaylistRef> getAlbums()
{
    MPMediaQuery *query = [MPMediaQuery albumsQuery];

    vector<PlaylistRef> albums;

    NSArray *query_groups = [query collections];
    for(MPMediaItemCollection *group in query_groups){
        PlaylistRef album = PlaylistRef(new Playlist(group));
        albums.push_back(album);
    }

    return albums;
}

vector<PlaylistRef> getAlbumsWithArtist(const string &artist_name)
{
    MPMediaQuery *query = [[MPMediaQuery alloc] init];
    [query addFilterPredicate: [MPMediaPropertyPredicate
           predicateWithValue: [NSNumber numberWithInteger: MPMediaTypeMusic]
                  forProperty: MPMediaItemPropertyMediaType
    ]];
    [query addFilterPredicate: [MPMediaPropertyPredicate
           predicateWithValue: [NSString stringWithUTF8String: artist_name.c_str()]
                  forProperty: MPMediaItemPropertyArtist
    ]];
    [query setGroupingType: MPMediaGroupingAlbum];

    vector<PlaylistRef> albums;

    NSArray *query_groups = [query collections];
    for(MPMediaItemCollection *group in query_groups){
        PlaylistRef album = PlaylistRef(new Playlist(group));
        albums.push_back(album);
    }

    return albums;
}

vector<PlaylistRef> getAlbumsWithArtistId(const uint64_t &artist_id)
{
    MPMediaQuery *query = [[MPMediaQuery alloc] init];
    [query addFilterPredicate: [MPMediaPropertyPredicate
                                predicateWithValue: [NSNumber numberWithInteger: MPMediaTypeMusic]
                                forProperty: MPMediaItemPropertyMediaType
                                ]];
	[query addFilterPredicate: [MPMediaPropertyPredicate
								predicateWithValue: [NSNumber numberWithUnsignedLongLong: artist_id]
								forProperty: MPMediaItemPropertyArtistPersistentID
								]];
    [query setGroupingType: MPMediaGroupingAlbum];
    
    vector<PlaylistRef> albums;
    
    NSArray *query_groups = [query collections];
    for(MPMediaItemCollection *group in query_groups){
        PlaylistRef album = PlaylistRef(new Playlist(group));
        albums.push_back(album);
    }
    
    return albums;
}
  
PlaylistRef getAlbumPlaylistWithArtistId(const uint64_t &artist_id)
{
    MPMediaQuery *query = [[MPMediaQuery alloc] init];
    [query addFilterPredicate: [MPMediaPropertyPredicate
                                predicateWithValue: [NSNumber numberWithInteger: MPMediaTypeMusic]
                                forProperty: MPMediaItemPropertyMediaType
                                ]];
    [query addFilterPredicate: [MPMediaPropertyPredicate
                                predicateWithValue: [NSNumber numberWithUnsignedLongLong: artist_id]
                                forProperty: MPMediaItemPropertyArtistPersistentID
                                ]];
    [query setGroupingType: MPMediaGroupingAlbum];
    
    PlaylistRef album = PlaylistRef(new Playlist());
    
    NSArray *query_groups = [query collections];
    for(MPMediaItemCollection *group in query_groups){
        NSArray *items = [group items];
        for(MPMediaItem *item in items){
            album->pushTrack(new Track(item));
        }
    }
    
    return album;
}    
    
PlaylistRef getArtist(const uint64_t &artist_id)
{
	MPMediaQuery *query = [[MPMediaQuery alloc] init];
	[query addFilterPredicate: [MPMediaPropertyPredicate
								predicateWithValue: [NSNumber numberWithInteger: MPMediaTypeMusic]
								forProperty: MPMediaItemPropertyMediaType
								]];
	[query addFilterPredicate: [MPMediaPropertyPredicate
								predicateWithValue: [NSNumber numberWithUnsignedLongLong: artist_id]
								forProperty: MPMediaItemPropertyArtistPersistentID
								]];
	MPMediaItemCollection *group = [MPMediaItemCollection collectionWithItems: [query items]];
	
	return PlaylistRef(new Playlist(group));
}
	
vector<PlaylistRef> getArtists( std::function<void(float)> progress )
{
    vector<PlaylistRef> artists;
    
    MPMediaQuery *query = [MPMediaQuery artistsQuery];

//	MPMediaQuery *query = [[MPMediaQuery alloc] init];
//	[query addFilterPredicate: [MPMediaPropertyPredicate
//								predicateWithValue: [NSNumber numberWithInteger: MPMediaTypeMusic]
//								forProperty: MPMediaItemPropertyMediaType
//								]];
//	[query setGroupingType: MPMediaGroupingAlbumArtist];    
    
    NSArray *query_groups = [query collections];
    
    int count = [query_groups count];
    
    for(MPMediaItemCollection *group in query_groups){
        PlaylistRef artist = PlaylistRef(new Playlist(group));
        artists.push_back(artist);
        progress( (float)artists.size() / (float)count );
    }

    progress( 1.0f );

    return artists;
}

vector<PlaylistRef> getPlaylists( std::function<void(float)> progress )
{
    MPMediaQuery *query = [MPMediaQuery playlistsQuery];
    
    vector<PlaylistRef> playlists;
    
    // TODO: perhaps filter by the MPMediaPlaylistPropertyPlaylistAttributes property?
//        enum {
//            MPMediaPlaylistAttributeNone    = 0,
//            MPMediaPlaylistAttributeOnTheGo = (1 << 0),
//            MPMediaPlaylistAttributeSmart   = (1 << 1),
//            MPMediaPlaylistAttributeGenius  = (1 << 2)
//        };
//    typedef NSInteger MPMediaPlaylistAttribute;    
    
    NSArray *query_groups = [query collections];
    int count = [query_groups count];
    
    for(MPMediaItemCollection *group in query_groups){
        string name = string([[group valueForProperty: MPMediaPlaylistPropertyName] UTF8String]);
        playlists.push_back(PlaylistRef(new Playlist(group, name)));
        progress( (float)playlists.size() / (float)count );
    }

    progress( 1.0f );
    
    return playlists;
}

} } // namespace cinder::ipod
