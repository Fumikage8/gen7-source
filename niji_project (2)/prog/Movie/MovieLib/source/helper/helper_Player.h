#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_Player.h
 Description: Helper file that manages Moflex content playback.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_Player_h
#define helper_Player_h

/*---------------------------------------------------------------------------*/
#include "heap/include/gfl2_NwAllocator.h"
#include "heap/include/gfl2_Heap.h"
#include "thread/include/gfl2_Thread.h"

#include <nn/types.h>

/*---------------------------------------------------------------------------*/

#include "mo_GenericVideoDecoder.h"
#include "mo_GenericAudioDecoder.h"
#include "moflex/moflex_Demuxer.h"

/*---------------------------------------------------------------------------*/

#include "helper_VideoConversion.h"
#include "helper_VideoPresentationBuffer.h"
#include "helper_AudioPresentationBuffer.h"
#include "helper_CodecInstantiation.h"

/*---------------------------------------------------------------------------*
  C++ API interface
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

enum PlayerError
{
	NO_VIDEO_STREAM						= 0x00000001,
	MAX_VIDEO_STREAM_EXCEEDED			= 0x00000002,
	NO_AUDIO_STREAM						= 0x00000004,
	MAX_AUDIO_STREAM_EXCEEDED			= 0x00000008,

	NO_TIMELINE_STREAM					= 0x00000010,
	MAX_TIMELINE_STREAM_EXCEEDED		= 0x00000020,

	UNKNOWN_STREAM						= 0x00000040,

	TIMELINE_DEMUXER_ERROR				= 0x00000080,
	TIMELINE_INCORRECT_FRAME_SIZE		= 0x00000100,
	TIMELINE_ENTRIES_ALLOCATION_FAILED	= 0x00000200,

	STEREOSCOPIC_ODD_FRAME_COUNT		= 0x00000400,

	UNKNOWN_VIDEO_CODEC					= 0x00000800,
	UNKNOWN_AUDIO_CODEC					= 0x00001000
};

/*---------------------------------------------------------------------------*
 Values that describe the different states the player can be.
 *---------------------------------------------------------------------------*/

enum PlayerStatus
{
    PLAYER_OPENING,
    PLAYER_STOPPED,
    PLAYER_STOPPING,
    PLAYER_PLAYING,
    PLAYER_SEEKING,
    PLAYER_PREDECODING,
    PLAYER_PAUSED,
    PLAYER_ERROR,
    PLAYER_END,
    PLAYER_STATUS_FORCE_DWORD = 0x7FFFFFFF
};

/*---------------------------------------------------------------------------*
 Values that is used to tell if we want to get/set current video stream of
 either top or bottom screen when calling GetCurrentVideoStream or
 SetCurrentVideoStream Player methods.
 *---------------------------------------------------------------------------*/

enum PlayerScreen
{
    PLAYER_SCREEN_TOP = 0,
    PLAYER_SCREEN_BOTTOM = 1,
    PLAYER_SCREEN_FORCE_DWORD = 0x7FFFFFFF
};

/*---------------------------------------------------------------------------*
 Contants used to configure some Player class properties
 *---------------------------------------------------------------------------*/

/* The thread priority that will call the Run Player method in a loop.
   The priority should be lower than the main thread so that the main 
   thread is not stalled by the Run thread. 
   In this cas it is 17 and main thread default priority is 16. */ 
const int RUN_THREAD_PRIORITY        = 17;

/* The Run thread stack size, 8KB is enough. */ 
const int RUN_THREAD_STACK_SIZE       = 8192;

/* The Number of video frames timestamps that can be enqueued. We want to be
   able to enqueue up to 12 videos frame timestamps, 16 is choosen as it is the 
   closest power of 2 value. */ 
const int VIDEO_MAX_QUEUE_SIZE        = 16;

/* The Maximum number of audio streams contained in a Moflex content that can 
   be managed by the Player class. 8 should enough to manage a Moflex content 
   with localization of all commonly used European languages. */ 
const int MAX_AUDIO_STREAM            = 8;

/* The Maximum number of video streams contained in a Moflex content that can 
   be managed by the Player class. */ 
const int MAX_VIDEO_STREAM            = 8;    




/*---------------------------------------------------------------------------*
 The Player class
 *---------------------------------------------------------------------------*/

class Player
{ 
    /*-----------------------------------------------------------------------*
      Pool of member variables that are privately held by the Player class.
     *-----------------------------------------------------------------------*/

private:

  gfl2::thread::SimpleStackArea* mpStack;
    nn::os::CriticalSection  m_RunSynchro;                // Critical section to prevent concurrent use of Player methods.
    volatile PlayerStatus    m_Status;                    // The current Player status.
    volatile bool            m_EndRunThread;              // Tells if we want the Run thread execution to be ended.
    bool                     m_StreamInfoRead;            // Tells if information on all streams have been read.
    int:                     16;                          // Padding added to avoid compiler warning.
    gfl2::heap::NwAllocator*     m_pAllocator;                // IAllocator used to perform all allocation/deallocation work    
    mw::mo::moflex::Demuxer* m_pDemuxer;                  // Moflex demuxer object pointer used to extract video/audio/timeline frames from a Moflex content
    int                      m_TotalVideoFramesDisplayed; // Total number of frames displayed since creation of the Player class. 
    nn::os::LightEvent       m_RunActivation;             // This event will not be signaled if Player is in PLAYER_STOPPED or PLAYER_PAUSED status,
                                                          // thus preventing the Run thread that is waiting for this event to be signalled to execute.
    nn::os::Thread           m_RunThread;                 // The Run thread object.

    s64 m_VideoFrameTimeStampArray[2][VIDEO_MAX_QUEUE_SIZE]; // Timestamp queues of all decoded video frames, in microseconds.
    s64 m_FirstAudioFrameTimeStamp;                          // Timestamp of first decoded audio frame, in microseconds.
    s64 m_PtsToPlaybackTime;                                 // Value in microseconds to add to timestamps of decoded video frames to obtain their presentation time, 
                                                             // that is the time to display them on the screen.
    s64 m_NextPresentationTime;                              // Value in microseconds of next video frame presentation time, that is the time to display it on the screen.
    s64 m_LastPlayedImageTime;                               // Value in microseconds of the last presented video frame, that is displayed on the screen.
    s64 m_PauseTime;                                         // Time in microseconds at which we put the Player in PLAYER_PAUSED state.
        
    VideoConversion     m_VideoConversion;           // Used for YUV->RGB color conversion and image deblocking.
    CodecInstantiation* m_pCodecInstantiation;       // Used to create and release audio and video decoders, by giving the Codec ID that is stored in 
                                                     // Moflex stream informtion.
    VideoPresentationBuffer*  m_pVideoPresentation;  // Used for video presentation, new RGB images will be given by the VideoConversion object.
    AudioPresentationBuffer*  m_pAudioPresentation;  // Used for audio presentation, new audio data will be given by the current audio decoder object.

    int                                m_NbTimelineStreams;                     // Total number of timeline streams.
    mw::mo::moflex::Timeline           m_Timeline[MAX_VIDEO_STREAM];            // Timeline streams data.
    mw::mo::moflex::TimelineStreamInfo m_TimelineStreamInfo[MAX_VIDEO_STREAM];  // Timeline streams infos.
    int                                m_TimelineStreamIndex[MAX_VIDEO_STREAM]; // Timelines Moflex streams indexes.

    int                                m_NbVideoStreams;                        // Total number of video streams.
    mw::mo::GenericVideoDecoder*       m_pVideoCodec[MAX_VIDEO_STREAM];         // Video streams decoder objects.
    mw::mo::moflex::VideoStreamInfo    m_VideoStreamInfo[MAX_VIDEO_STREAM];     // Video streams infos.
    int                                m_VideoStreamIndex[MAX_VIDEO_STREAM];    // Video Moflex streams indexes.
    int                                m_CurrentVideoStream[2];                 // Selected video streams that will be used for TOP and BOTTOM screen rendering.
    volatile int                       m_VideoFramePushIndex[2];                // Indexes of pushed video frames timestamp for TOP and BOTTOM screen rendering.
    volatile int                       m_VideoFramePopIndex[2];                 // Indexes of popped video frames timestamp for TOP and BOTTOM screen rendering.

    int                                m_NbAudioStreams;                        // Total number of audio streams.
    mw::mo::GenericAudioDecoder*       m_pAudioCodec[MAX_AUDIO_STREAM];         // Audio streams decoder objects.
    mw::mo::moflex::AudioStreamInfo    m_AudioStreamInfo[MAX_AUDIO_STREAM];     // Audio streams infos.
    int                                m_AudioStreamIndex[MAX_AUDIO_STREAM];    // Audio Moflex streams indexes.
    int                                m_CurrentAudioStream;                    // Selected audio stream.
    volatile bool                      m_AudioUnpacked;                         // Tells if audio decoder already unpacked audio data taht has not already been
                                                                                // successfully sent to AudioPresentation object.
    int:                               24;                                      // Padding added to avoid compiler warning.

    u32                                m_PlayerError;

	u32                                m_NbDecodedFrame;
	u32                                m_DecodedFrameSize;
	int:                               32;                                      // Padding added to avoid compiler warning.

    
    /*-----------------------------------------------------------------------*
      Pool of methods that are privately held by the Player class.
     *-----------------------------------------------------------------------*/

    static void RunThreadFunc                (Player* pPlayer);     // Thread function that internally calls the Run method.
    void        FlushPendingFrames           (void);                // Flushes all decoded video frames.
    void        SetStatus                    (PlayerStatus status); // Sets player status.
    void        SetNextVideoPresentationTime (void);                // Compute presentation time of next video image, internally called by VideoImageRequestEnd method.
        
    void        RetrieveStreamInfo           (void);                // Retrieves streams informations from Moflex demuxer, if any available.
    void        RetrieveTimelineData         (void);                // Retrieves timeline streams data from Moflex content, if any available.
    void        DropEarlyDecodedVideoFrames  (void);                // Removes decoded video frames that should be presented earlier than the first retrieved audio frame. 
    void        DecodeAudioData              (void);                // Decodes audio data.
    void        DecodeVideoData              (void);                // Decodes video data.  
    void        DropUnusedStreamsFrames      (void);                // Drops all frames of streams that are unused in the playback process.
    void        ReadNewMoflexData            (void);                // Reads a new Moflex packet if needed.

    void        Run                          (void);                // Does all the Player processing job like state graph management, frames decoding and synchronisation.

    /*-----------------------------------------------------------------------*
      Pool of public methods that can be used by the player application.
     *-----------------------------------------------------------------------*/

public:

    Player();
    ~Player();

    /*-----------------------------------------------------------------------*
      Initialization, de-initialization management.
     *-----------------------------------------------------------------------*/

    bool    Create           (gfl2::heap::NwAllocator* pAllocator, mw::mo::moflex::Demuxer* pDemuxer,
                              CodecInstantiation* pCodecInstantiation,                    
                              VideoPresentationBuffer* pVideoPresentation,
                              AudioPresentationBuffer* pAudioPresentation); // Initializes a Player instance.
    void    Release          (void);                                        // Finalizes a Player instance.
    void    EnableDeblocking (bool enable);                                 // If true, dynamic deblocking will be performed on decoded video frames before YUV->RGB color conversion.
                                                                            // If false, deblocking will be disabled.
    
    /*-----------------------------------------------------------------------*
      Playback management.
     *-----------------------------------------------------------------------*/

    void         Yield          (void); // Gives CPU time to other running threads.
    PlayerStatus GetStatus      (void); // Retrieves current player status, internally calls Yield method to give CPU time to Run thread.
    bool         StartPlayback  (void); // Starts playback, can internally call Yield method to give CPU time to Run thread.
    bool         StopPlayback   (void); // Stops playback, decoders video queue are flushed, can internally call Yield method to give CPU time to Run thread.    
    bool         PausePlayback  (void); // Pauses playback, decoders video queue are kept intact, can internally call Yield method to give CPU time to Run thread.
    bool         ResumePlayback (void); // Resumes playback, can internally call Yield method to give CPU time to Run thread.    

    /*-----------------------------------------------------------------------*
      Presentation management.
     *-----------------------------------------------------------------------*/

    s64  VideoPresentationTimeElapsed (void); // Tells how much time has elapsed in microseconds since video image should have been presented on the screen.
                                              // A negative value means time to present video image is not yet reached.
	                                          // Other values gives how mush we're late in the video image presentation process.
    bool VideoImageRequestStart       (void); // Start the request of a new video image if all needed video images are decoded, this pushes a batch of color conversions.
                                              // Destination RGB buffers are taken from VideoPresentation object and given to ColorConversion object with YUV buffers of decoded video images.
	s64 GetPresentedVideoImageTS	  (void); //Returns the timestamp (in us) of the presented video image

    void VideoImageRequestEnd         (void); // Ends the video image request if batch of color conversions are all finished, this pops all color conversions.
	void VideoImageSkip               (void); // Skips the video image without doing all color conversion process.
    bool VideoImageRequestPending     (void); // Tells if a video image request is pending.
    bool VideoImageAvailable          (void); // Tells if a video image is available, meaning that the video image request is finished.
    bool WaitVideoImage               (void); // Waits for the availability of video images.

    const mw::mo::helper::AudioPresentationBuffer* GetAudioPresentationBuffer (void){return m_pAudioPresentation;} // Returns AudioPresentationBuffer object that were given when calling Create method.
    const mw::mo::helper::VideoPresentationBuffer* GetVideoPresentationBuffer (void){return m_pVideoPresentation;} // Returns VideoPresentationBuffer object that were given when calling Create method.

    /*-----------------------------------------------------------------------*
      Video stream management.
     *-----------------------------------------------------------------------*/

    int                                    GetNbVideoStream      (void);                                             // Gets the number of video streams in the Moflex content.
    int                                    GetCurrentVideoStream (PlayerScreen screen);                              // Gets the video stream index that is associated to the top or bottom screen, return -1 if none.
    bool                                   SetCurrentVideoStream (PlayerScreen screen, int videoStream);             // Sets the video stream index that will be associated to the top or bottom screen, 
                                                                                                                     // give -1 in parameter (this is the default) if you want none of them to be associated.
    const mw::mo::moflex::VideoStreamInfo* GetVideoStreamInfo    (int videoStream);                                  // Gets information on a Moflex video stream.
    int                                    GetNbImageFrames      (const mw::mo::moflex::VideoStreamInfo* videoInfo); // Gets the number of frames needed to obtain a full video image, given the video stream info.
                                                                                                                     // It is 2 if the video stream is stereoscopic in interleaved mode, 1 otherwise.        
    /*-----------------------------------------------------------------------*
      Audio stream management.
     *-----------------------------------------------------------------------*/

    int                                    GetNbAudioStream      (void);            // Gets the number of audio streams in the Moflex content
    int                                    GetCurrentAudioStream (void);            // Gets the audio stream index that is used for audio playback, returns -1 if none.
    bool                                   SetCurrentAudioStream (int audioStream); // Sets the audio stream index that will be used for audio playback, 
                                                                                    // give -1 in parameter (this is the default) if you want none of them to be played.
    const mw::mo::moflex::AudioStreamInfo* GetAudioStreamInfo    (int audioStream); // Gets information on a Moflex audio stream.
    
    /*-----------------------------------------------------------------------*
      Timeline stream management.
     *-----------------------------------------------------------------------*/

    int                                       GetNbTimeline                    (void);                                // Gets the number of timeline streams in the Moflex content.
    const mw::mo::moflex::TimelineStreamInfo* GetTimelineStreamInfo            (int timelineStream);                  // Gets information on a Moflex timeline stream.
    int                                       GetTimelineStreamFromVideoStream (int videoStream);                     // Gets timeline stream index that indexes a given video stream, -1 if none.
    u32                                       GetTimelineNbEntries             (int timelineStream);                  // Gets number of entries of a Moflex timeline stream.
    u32                                       GetTimelineTotalFrames           (int timelineStream);                  // Gets the total number of video frames given by a Moflex timeline stream.
    s64                                       GetTimelineDuration              (int timelineStream);                  // Gets the duration in microseconds given by a Moflex timeline stream.
    mw::mo::moflex::TimelineEntry*            GetTimelineEntryByIndex          (int timelineStream, u32 index);       // Gets an entry information given an entry index for a Moflex timeline stream.
    mw::mo::moflex::TimelineEntry*            GetTimelineEntryByFrameNumber    (int timelineStream, u32 frameNumber); // Gets an entry information given a video frame number for a Moflex timeline stream.
                                                                                                                      // The entry will point to a position on the Moflex content that is before or is on video frame number position.
    mw::mo::moflex::TimelineEntry*            GetTimelineEntryByTimeStamp      (int timelineStream, s64 timeStamp);   // Gets an entry information given a timestamp in microseconds for a Moflex timeline stream.
                                                                                                                      // The entry will point to a position on the Moflex content that is before or on timestamp time.
    /*-----------------------------------------------------------------------*
      Moflex content seeking management.
     *-----------------------------------------------------------------------*/

    PlayerStatus SetPositionByTimeline   (mw::mo::moflex::TimelineEntry* timelineEntry); // Seeks into Moflex content to a position specified by the given timeline entry.
    PlayerStatus SetPositionByFileOffset (s64 fileOffset);                               // Seeks into Moflex content to a position specified by the given raw file offset.

    /*-----------------------------------------------------------------------*
      Retrieval of playback information.
     *-----------------------------------------------------------------------*/

    int  GetTotalVideoFramesDisPlayed (void){return m_TotalVideoFramesDisplayed;}
    s64  GetLastPlayedImageTime       (void);
    void DrawPlaybackInformation      (void* screenPtr, u32 screenWidth, u32 screenHeight);

    /*-----------------------------------------------------------------------*/

    void PrintStatistics (void);

    /*-----------------------------------------------------------------------*/

	u32  GetError();
	u32  PopDecodedFramesCount();
	u32  PopDecodedFramesSize();
	
protected :

	void AddError(PlayerError error);
};

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif  // ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#endif  // ifndef helper_Player_h

/*---------------------------------------------------------------------------*/

#endif// defined(GF_PLATFORM_CTR)
