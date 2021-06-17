#import "CinderIPodPlayerImpl.h"

#include "CinderIPodPlayer.h"

@implementation CinderIPodPlayerImpl

- (id)init:(cinder::ipod::Player*)player
{
    self = [super init];

    m_player = player;
    m_controller = [MPMusicPlayerController iPodMusicPlayer];
    m_library = [MPMediaLibrary defaultMediaLibrary];
    
    m_suppress_events = false;
    
    NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
    
    [nc addObserver: self
           selector: @selector (onStateChanged:)
               name: MPMusicPlayerControllerPlaybackStateDidChangeNotification
             object: m_controller];

    [nc addObserver: self
           selector: @selector (onTrackChanged:)
               name: MPMusicPlayerControllerNowPlayingItemDidChangeNotification
             object: m_controller];

    [m_controller beginGeneratingPlaybackNotifications];
    
    [nc addObserver: self
           selector: @selector (onLibraryChanged:)
               name: MPMediaLibraryDidChangeNotification
             object: m_library];

    [m_library beginGeneratingLibraryChangeNotifications];
    
    return self;
}

- (void)dealloc
{
    [super dealloc];
    // TODO: end generating notifications on m_controller? on library?
    [m_controller dealloc]; // TODO: is this necessary if we aren't the ones allocing iPodMusicPlayer?
}

- (void)onStateChanged:(NSNotification *)notification
{
    if (!m_suppress_events)
        m_cb_state_change.call(m_player);
}

- (void)onTrackChanged:(NSNotification *)notification
{
    if (!m_suppress_events)
        m_cb_track_change.call(m_player);
}

- (void)onLibraryChanged:(NSNotification *)notification
{
    if (!m_suppress_events)
        m_cb_library_change.call(m_player);
}

@end
