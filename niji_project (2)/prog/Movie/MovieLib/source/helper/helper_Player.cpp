#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_Player.cpp
 Description: Helper file that manages generic Moflex content playback.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#include <nn.h>
#include <nn/fnd.h>
#include <nn/applet.h>
#include <string.h>

/*---------------------------------------------------------------------------*/

#include "helper_Player.h"
#include "helper_Font.h"
#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*
  Thread function that internally calls the Run method.    
 *---------------------------------------------------------------------------*/

void Player::RunThreadFunc (Player* pPlayer)
{
  for(;;)
    {
        /* The thread will wait while in PLAYER_STOPPED, PLAYER_PAUSED or PLAYER_ERROR state */
        pPlayer->m_RunActivation.Wait();

        /* The Release method has been called, we then have to exit this running thread */
      if (pPlayer->m_EndRunThread == true){
            return;
      }

        /* The thread is in PLAYER_STOPPED, PLAYER_PAUSED or PLAYER_ERROR state, we call its Run loop */
        pPlayer->Run();
    }
}

/*---------------------------------------------------------------------------*
  Gives CPU time to other running thread.
 *---------------------------------------------------------------------------*/

void Player::Yield (void)
{
    nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMicroSeconds(500));
}

/*---------------------------------------------------------------------------*
  Flushes all decoded video frames, internally called by Run and 
  SetPositionByFileOffset methods.
 *---------------------------------------------------------------------------*/

void Player::FlushPendingFrames (void)
{
    /* Finish and remove any pending color conversion process */  
    if (VideoImageRequestPending() == true)
    {
        WaitVideoImage();
        VideoImageRequestEnd();
    }

    for (int i=0; i<2; i++)
    {
        if (m_CurrentVideoStream[i] >= 0)
        {
            int pushedFrames;
            m_pVideoCodec[m_CurrentVideoStream[i]]->GetNbPushedFrame(pushedFrames);
            for (int j=0; j<pushedFrames; j++)
                m_pVideoCodec[m_CurrentVideoStream[i]]->PopFrame();

            m_VideoFramePushIndex[i] = 0;
            m_VideoFramePopIndex[i] = 0;
        }
    }
}

/*---------------------------------------------------------------------------*
  Sets player status.
  Using a critical section to not be called while inside a Run method process.
 *---------------------------------------------------------------------------*/

void Player::SetStatus (PlayerStatus status)
{
    nn::os::CriticalSection::ScopedLock csLock(m_RunSynchro);

    m_Status = status;
}

/*---------------------------------------------------------------------------*
  Compute presentation time of next video image, internally called by 
  VideoImageRequestEnd method.
 *---------------------------------------------------------------------------*/

void Player::SetNextVideoPresentationTime (void)
{
    for(;;)
    {
        if (m_Status == PLAYER_STOPPED)
            return;

        int videoIndex = -1;
        for (int i=0; i<2; i++)
            if (m_CurrentVideoStream[i] >= 0)
            {
                videoIndex = i;
                break;
            }

        if (videoIndex >= 0)
        {
            int neededVideoFrames = GetNbImageFrames(&m_VideoStreamInfo[m_CurrentVideoStream[videoIndex]]);
            int pushedVideoFrames = m_VideoFramePushIndex[videoIndex] - m_VideoFramePopIndex[videoIndex];
            if (pushedVideoFrames >= neededVideoFrames)
            {
                m_NextPresentationTime = m_PtsToPlaybackTime + m_VideoFrameTimeStampArray[videoIndex][m_VideoFramePopIndex[videoIndex]%VIDEO_MAX_QUEUE_SIZE];
                return;
            }
            else
                Yield();
        }
    }
}

/*---------------------------------------------------------------------------*
  Retrieves streams informations from Moflex demuxer, if any available.
  Once done the m_StreamInfoRead boolean is set to true.
 *---------------------------------------------------------------------------*/

void Player::RetrieveStreamInfo (void)
{
    u32 nbStream;
    m_pDemuxer->GetStreamCount(nbStream);
    
    if (nbStream > 0)
    {
        for (int i=0; i<nbStream; i++)
        {
            mw::mo::moflex::StreamInfo stream;
            m_pDemuxer->GetStreamInfo(i,stream);
            if (stream.m_Generic.m_StreamType == mw::mo::moflex::STREAM_TYPE_VIDEO)
            {
                if (m_NbVideoStreams == MAX_VIDEO_STREAM)
                {
                    m_Status = PLAYER_ERROR;
                    m_PlayerError |= MAX_VIDEO_STREAM_EXCEEDED;
                }
                else if (m_NbVideoStreams < MAX_VIDEO_STREAM)
                {
                m_pVideoCodec[m_NbVideoStreams] = NULL;
                m_VideoStreamInfo[m_NbVideoStreams] = stream.m_Video;
                m_VideoStreamIndex[m_NbVideoStreams] = i;
                }
                m_NbVideoStreams++;
            }
            else if (stream.m_Generic.m_StreamType == mw::mo::moflex::STREAM_TYPE_AUDIO)
            {
                if (m_NbAudioStreams == MAX_AUDIO_STREAM)
                {
                    m_Status = PLAYER_ERROR;
                    m_PlayerError |= MAX_AUDIO_STREAM_EXCEEDED;
                }
                else if (m_NbAudioStreams < MAX_AUDIO_STREAM)
                {
                    m_pAudioCodec[m_NbAudioStreams] = NULL;
                    m_AudioStreamInfo[m_NbAudioStreams] = stream.m_Audio;
                    m_AudioStreamIndex[m_NbAudioStreams] = i;
                }
                m_NbAudioStreams++;
            }
            else if (stream.m_Generic.m_StreamType == mw::mo::moflex::STREAM_TYPE_TIMELINE)
            {
                if (m_NbTimelineStreams == MAX_VIDEO_STREAM)
                {
                    m_Status = PLAYER_ERROR;
                    m_PlayerError |= MAX_TIMELINE_STREAM_EXCEEDED;
                }
                else if (m_NbTimelineStreams < MAX_VIDEO_STREAM)
                {
                    m_TimelineStreamInfo[m_NbTimelineStreams] = stream.m_Timeline;
                    m_TimelineStreamIndex[m_NbTimelineStreams] = i;
                }
                m_NbTimelineStreams++;    
            }
            else
            {
                m_Status = PLAYER_ERROR;
                m_PlayerError |= UNKNOWN_STREAM;
            }
        }

        if (m_NbVideoStreams == 0)
            m_PlayerError |= NO_VIDEO_STREAM;
        if (m_NbAudioStreams == 0)
            m_PlayerError |= NO_AUDIO_STREAM;
        if (m_NbTimelineStreams == 0)
            m_PlayerError |= NO_TIMELINE_STREAM;

        m_StreamInfoRead = true;
    }
}

/*---------------------------------------------------------------------------*
  Retrieves timeline streams data from Moflex content, if any available.
 *---------------------------------------------------------------------------*/

void Player::RetrieveTimelineData (void)
{
    u32 nbStream;
    m_pDemuxer->GetStreamCount(nbStream);

    if (nbStream > 0)
    {
        if (m_NbTimelineStreams > 0)
        {
            bool allTimelinesRead = true;

            for (int i=0; i<m_NbTimelineStreams; i++)
            {
                if (m_Timeline[i].m_pEntries == NULL)
                {
                    u32 nbTimelineFrames;
                    m_pDemuxer->GetStreamFrameCount(m_TimelineStreamIndex[i], nbTimelineFrames);
                    if (nbTimelineFrames == 0)
                        allTimelinesRead = false;
                    else
                    {
                        const u8* frameData;
                        u32       frameSize;
                        u32       frameType;
                        s64       frameTimeStamp;

                        mw::mo::moflex::DemuxerReturn demuxerReturn = m_pDemuxer->GetStreamFrameData(m_TimelineStreamIndex[i], frameData, frameSize, frameType, frameTimeStamp);
                        if (demuxerReturn != mw::mo::moflex::DEMUXER_RETURN_OK)
                        {
                          //  NN_ASSERT(demuxerReturn == mw::mo::moflex::DEMUXER_RETURN_OK);
                            m_Status = PLAYER_ERROR;
                            m_PlayerError |= TIMELINE_DEMUXER_ERROR;
                            return;
                        }
                        
                        if (frameSize < sizeof(mw::mo::moflex::TimelineHeader))
                        {
                        //    NN_ASSERT(frameSize >= sizeof(mw::mo::moflex::TimelineHeader));
                            m_Status = PLAYER_ERROR;
                            m_PlayerError |= TIMELINE_INCORRECT_FRAME_SIZE;
                            return;
                        }

                        memcpy(&m_Timeline[i].m_Header,frameData,sizeof(mw::mo::moflex::TimelineHeader));
                        frameData += sizeof(mw::mo::moflex::TimelineHeader);
                        frameSize -= sizeof(mw::mo::moflex::TimelineHeader);
                        
                        m_Timeline[i].m_pEntries = (mw::mo::moflex::TimelineEntry*)m_pAllocator->Alloc(frameSize, 4);
                        if (m_Timeline[i].m_pEntries == NULL)
                        {
                         //   NN_ASSERT(m_Timeline[i].m_pEntries != NULL);
                            m_Status = PLAYER_ERROR;
                            m_PlayerError |= TIMELINE_ENTRIES_ALLOCATION_FAILED;
                            return;
                        }    
                        
                        memcpy(m_Timeline[i].m_pEntries,frameData,frameSize);

                        m_pDemuxer->PopStreamFrame(m_TimelineStreamIndex[i]);

                        /* We perform a sanity check here: if a video stream is stereoscopic interleaved with an odd number 
                           of frames we raise and assertion and return an error. We can't do that test without a timeline 
                           stream associated to a video steam. */
                        for (int j=0; j<m_NbVideoStreams; j++)
                        {
                            if (m_TimelineStreamInfo[i].m_AssociatedStreamIndex == m_VideoStreamIndex[j])
                                if (GetNbImageFrames(&m_VideoStreamInfo[j]) == 2)
                                    if ((m_Timeline[i].m_Header.m_TotalFrames % 2) != 0)
                                    {
                                    //    NN_ASSERT((m_Timeline[i].m_Header.m_TotalFrames % 2) == 0);
                                        m_Status = PLAYER_ERROR;
                                        m_PlayerError |= STEREOSCOPIC_ODD_FRAME_COUNT;
                                        return;
                                    }
                        }
                    }
                }
            }

            if (allTimelinesRead == true)
                m_Status = PLAYER_STOPPED;
        }
        else
            m_Status = PLAYER_STOPPED;
    }
}

/*---------------------------------------------------------------------------*
  Removes decoded video frames that should be presented earlier than the first
  retrieved audio frame. This function doing part of audio/video 
  synchronization job.
 *---------------------------------------------------------------------------*/

void Player::DropEarlyDecodedVideoFrames (void)
{
    if (m_FirstAudioFrameTimeStamp != 0)
    {
        for (int i=0; i<2; i++)
        {
            if (m_CurrentVideoStream[i] >= 0)
            {
                int pushedVideoFrames = m_VideoFramePushIndex[i] - m_VideoFramePopIndex[i];
                while (pushedVideoFrames > 0)
                {
                    s64 videoDisplayDelay = m_VideoFrameTimeStampArray[i][m_VideoFramePopIndex[i]%VIDEO_MAX_QUEUE_SIZE] - m_FirstAudioFrameTimeStamp;
                    if (videoDisplayDelay < 0)
                    {
                        m_pVideoCodec[m_CurrentVideoStream[i]]->PopFrame();
                        m_VideoFramePopIndex[i]++;
                        pushedVideoFrames--;
                    }
                    else
                        break;
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Decodes audio data.
 *---------------------------------------------------------------------------*/

void Player::DecodeAudioData (void)
{
    if (m_NbAudioStreams > 0)
    {
        u32 nbAudioFrames;
        if (m_CurrentAudioStream >= 0)
            m_pDemuxer->GetStreamFrameCount(m_AudioStreamIndex[m_CurrentAudioStream], nbAudioFrames);
        else
            nbAudioFrames = 0;

        while (nbAudioFrames > 0)
        {
            const u8*    frameData;
            u32            frameSize;
            u32            frameType;
            s64            frameTimeStamp;

            m_pDemuxer->GetStreamFrameData(m_AudioStreamIndex[m_CurrentAudioStream], frameData, frameSize, frameType, frameTimeStamp);

            if (m_FirstAudioFrameTimeStamp == 0)
                m_FirstAudioFrameTimeStamp = frameTimeStamp;

            if (m_AudioStreamInfo[m_CurrentAudioStream].m_AudioCodec != mw::mo::moflex::AUDIO_CODEC_PCM && // We don't have PCM data
                m_pAudioCodec[m_CurrentAudioStream] != NULL) // and we have instantiated a codec  for this audio track
            {
                if (m_AudioUnpacked == false)
                {
                    m_pAudioCodec[m_CurrentAudioStream]->UnpackFrame(frameData, frameSize);
                    m_AudioUnpacked = true;
                }

                m_pAudioCodec[m_CurrentAudioStream]->GetFrame((const void*&)frameData, (int &)frameSize);
            }

            if (m_pAudioPresentation->AddAudioData((void*)frameData, frameSize) == false)
                break;

            m_AudioUnpacked = false;
            m_pDemuxer->PopStreamFrame(m_AudioStreamIndex[m_CurrentAudioStream]);

            nbAudioFrames--;
        }
    }
}

/*---------------------------------------------------------------------------*
  Drops all frames of streams that are unused in the playback process.
 *---------------------------------------------------------------------------*/

void Player::DropUnusedStreamsFrames (void)
{
    u32 nbStream;
    m_pDemuxer->GetStreamCount(nbStream);
    
    if (nbStream > 0)
    {
        for (int i=0; i<m_NbAudioStreams; i++)
        {
            if (i != m_CurrentAudioStream)
                 m_pDemuxer->FlushStream(m_AudioStreamIndex[i]);
        }

        for (int i=0; i<m_NbVideoStreams; i++)
        {
            if (i != m_CurrentVideoStream[0] && i != m_CurrentVideoStream[1])
                m_pDemuxer->FlushStream(m_VideoStreamIndex[i]);
        }

        for (int i=0; i<m_NbTimelineStreams; i++)
            m_pDemuxer->FlushStream(m_TimelineStreamIndex[i]);
    }
}

/*---------------------------------------------------------------------------*
  Decodes video data.
  If the player is still in the PLAYER_SEEKING state:
    - it ignores all video frames of type mw::mo::moflex::FRAME_TYPE_PFRAME
      until the first video frame of type mw::mo::moflex::FRAME_TYPE_IFRAME
      is found.
    - it decodes the first mw::mo::moflex::FRAME_TYPE_IFRAME  video frame.
    - if the video is stereoscopic interleaved and if the next decoded video 
      frame is not of the same left/right eye pair, the first decoded video 
      frame is removed from the decoder queue without been displayed (this 
      prevents left/right eye inversion problem at display).
 *---------------------------------------------------------------------------*/

void Player::DecodeVideoData (void)
{
    /*-----------------------------------------------------------------------*/

    u32 nbStream;
    m_pDemuxer->GetStreamCount(nbStream);
    int remainingFrameToDecode[2] = {0,0};

    if (nbStream > 0)
    {
        for (int i=0; i<2; i++)
        {
            if (m_CurrentVideoStream[i] >= 0)
            {
                int pushedVideoFrames;
                m_pVideoCodec[m_CurrentVideoStream[i]]->GetNbPushedFrame(pushedVideoFrames);
                remainingFrameToDecode[i] = m_pVideoCodec[m_CurrentVideoStream[i]]->GetQueueCapacity() - pushedVideoFrames;
            }
        }

        if (remainingFrameToDecode[0] == 0 && remainingFrameToDecode[1] == 0 && m_Status == PLAYER_PREDECODING)
            m_Status = PLAYER_PLAYING;
    }
    
    /*-----------------------------------------------------------------------*/

    for (int i=0; i<2; i++)
    {
        if (m_CurrentVideoStream[i] >= 0)
        {
            u32 nbVideoFrames;
            m_pDemuxer->GetStreamFrameCount(m_VideoStreamIndex[m_CurrentVideoStream[i]], nbVideoFrames);
            if (nbVideoFrames > remainingFrameToDecode[i])
                nbVideoFrames = remainingFrameToDecode[i];
            
            for(int j=0; j<nbVideoFrames; j++)
            {
                const u8*    frameData;
                u32            frameSize;
                u32            frameType;
                s64            frameTimeStamp;

                m_pDemuxer->GetStreamFrameData(m_VideoStreamIndex[m_CurrentVideoStream[i]], frameData, frameSize, frameType, frameTimeStamp);
                
                if (m_Status == PLAYER_SEEKING)
                {
                    if (frameType == mw::mo::moflex::FRAME_TYPE_PFRAME && m_VideoFramePushIndex[i] == m_VideoFramePopIndex[i])
                    {
                        m_pDemuxer->PopStreamFrame(m_VideoStreamIndex[m_CurrentVideoStream[i]]);
                        if (m_CurrentAudioStream >= 0)
                            m_pDemuxer->FlushStream(m_AudioStreamIndex[m_CurrentAudioStream]);
                        continue;
                    }
                }
                
                m_pVideoCodec[m_CurrentVideoStream[i]]->UnpackFrame(frameData);

                m_VideoFrameTimeStampArray[i][m_VideoFramePushIndex[i]%VIDEO_MAX_QUEUE_SIZE] = frameTimeStamp;
                m_pDemuxer->PopStreamFrame(m_VideoStreamIndex[m_CurrentVideoStream[i]]);
                m_VideoFramePushIndex[i]++;

                m_NbDecodedFrame += nbVideoFrames;
                m_DecodedFrameSize += frameSize;

                if (m_Status == PLAYER_SEEKING)
                {
                    if (GetNbImageFrames(&m_VideoStreamInfo[m_CurrentVideoStream[i]]) == 2)
                    {
                        if ((m_VideoFramePushIndex[i] - m_VideoFramePopIndex[i]) == 2)
                        {
                            if (m_VideoFrameTimeStampArray[i][m_VideoFramePopIndex[i]%VIDEO_MAX_QUEUE_SIZE] != 
                                m_VideoFrameTimeStampArray[i][(m_VideoFramePopIndex[i]+1)%VIDEO_MAX_QUEUE_SIZE])
                            {
                                m_pVideoCodec[m_CurrentVideoStream[i]]->PopFrame();
                                m_VideoFramePopIndex[i]++;
                        
                                if (m_CurrentAudioStream >= 0)
                                    m_pDemuxer->FlushStream(m_AudioStreamIndex[m_CurrentAudioStream]);
                            }

                            m_Status = PLAYER_STOPPED;
                            return;
                        }
                    }
                    else
                    {
                        m_Status = PLAYER_STOPPED;
                        return;
                    }
                }
            }
        }
    }

    /*-----------------------------------------------------------------------*/
}

/*---------------------------------------------------------------------------*
  Reads a new Moflex packet if needed.
 *---------------------------------------------------------------------------*/

void Player::ReadNewMoflexData (void)
{
    /*-----------------------------------------------------------------------*/

    bool canReadNextPacket = true;

    if (m_Status == PLAYER_STOPPED)
        canReadNextPacket = false;
    else if (m_Status != PLAYER_OPENING && m_Status != PLAYER_SEEKING)
    {
        bool needToDemuxVideoFrames = false;
        for (int i=0; i<2; i++)
        {
            if (m_CurrentVideoStream[i] >= 0)
            {
                int nbVideoFramesToDecode;
                m_pVideoCodec[m_CurrentVideoStream[i]]->GetNbPushedFrame(nbVideoFramesToDecode);
                nbVideoFramesToDecode = m_pVideoCodec[m_CurrentVideoStream[i]]->GetQueueCapacity() - nbVideoFramesToDecode;
                
                u32 nbDemuxedVideoFrames;
                m_pDemuxer->GetStreamFrameCount(m_VideoStreamIndex[m_CurrentVideoStream[i]], nbDemuxedVideoFrames);
                if (nbDemuxedVideoFrames == 0 && nbVideoFramesToDecode > 0)
                    needToDemuxVideoFrames = true;
            }
        }

        if (needToDemuxVideoFrames == false)
            canReadNextPacket = false;
    }

    /*-----------------------------------------------------------------------*/

    if (canReadNextPacket == true)
    {
        mw::mo::moflex::DemuxerReturn ret = m_pDemuxer->ReadPacket();
        switch(ret)
        {
            case mw::mo::moflex::DEMUXER_RETURN_OK:
            case mw::mo::moflex::DEMUXER_RETURN_BUSY:
            case mw::mo::moflex::DEMUXER_RETURN_SYNCHRONISING:
                break;

            case mw::mo::moflex::DEMUXER_RETURN_END:
                if (m_Status == PLAYER_PLAYING)
                {
                    for (int i=0; i<2; i++)
						if (m_CurrentVideoStream[i] >= 0 && 
							(m_VideoFramePushIndex[i] - m_VideoFramePopIndex[i]) < GetNbImageFrames(&m_VideoStreamInfo[m_CurrentVideoStream[i]]))
							m_Status = PLAYER_END;
                }
                else
                    m_Status = PLAYER_END;
                break;

            default:
                NN_LOG( "ReadPacket error\n");
                NN_ASSERT(ret == mw::mo::moflex::DEMUXER_RETURN_OK || mw::mo::moflex::DEMUXER_RETURN_BUSY ||
                          mw::mo::moflex::DEMUXER_RETURN_SYNCHRONISING || mw::mo::moflex::DEMUXER_RETURN_END);
                m_Status = PLAYER_ERROR;
                break;
        }
    }

    /*-----------------------------------------------------------------------*/
}

/*---------------------------------------------------------------------------*
  Does all the Player processing job like state graph management, 
  frames decoding and synchronisation.
 *---------------------------------------------------------------------------*/

void Player::Run (void)
{
    /*-----------------------------------------------------------------------*/

    nn::os::CriticalSection::ScopedLock csLock(m_RunSynchro);

    /*-----------------------------------------------------------------------*
      Manages PLAYER_ERROR state.

      Clears the player event signal to stall the Run thread.
      Once in this state Player cannot enter in another one.
     *-----------------------------------------------------------------------*/

    if (m_Status == PLAYER_ERROR)
    {
        m_RunActivation.ClearSignal();
        return;
    }

    /*-----------------------------------------------------------------------*
      Manages PLAYER_PAUSED state.

      Next state can be:
        - PLAYER_PREDECODING if ResumePlayback method is called by the user.
     *-----------------------------------------------------------------------*/

    if (m_Status == PLAYER_PAUSED)
    {
        m_RunActivation.ClearSignal();
        return;
    }

    /*-----------------------------------------------------------------------*
      Manages PLAYER_OPENING state.

      Next states can be:
        - PLAYER_STOPPED if opening is successfull.
        - PLAYER_ERROR if opening ended with an error.
     *-----------------------------------------------------------------------*/

    if (m_Status == PLAYER_OPENING)
    {
        if (m_StreamInfoRead == false)
            RetrieveStreamInfo();
        if (m_StreamInfoRead == true)
            RetrieveTimelineData();
        if (m_Status == PLAYER_ERROR)
            return;
    }

    /*-----------------------------------------------------------------------*
      Manages PLAYER_END state.

      Next state is:
        - PLAYER_STOPPING to start end of playback process.
     *-----------------------------------------------------------------------*/

    if (m_Status == PLAYER_END)
        m_Status = PLAYER_STOPPING;

    /*-----------------------------------------------------------------------*
      Manages PLAYER_STOPPING state.

      Next state is:
        - PLAYER_STOPPED.
     *-----------------------------------------------------------------------*/

    if (m_Status == PLAYER_STOPPING)
    {
        m_AudioUnpacked = false;
        m_FirstAudioFrameTimeStamp = 0;
        m_NextPresentationTime = 0;
        m_PauseTime = 0;

        m_Status = PLAYER_STOPPED;
    }

    /*-----------------------------------------------------------------------*
      Manages PLAYER_STOPPED state.

      Next states can be:
        - PLAYER_PREDECODING if StartPlayback is called.
        - PLAYER_SEEKING if SetPositionByFileOffset SetPositionByTimeline
          is called.
     *-----------------------------------------------------------------------*/

    if (m_Status == PLAYER_STOPPED)
    {
        m_RunActivation.ClearSignal();
        return;
    }

    /*-----------------------------------------------------------------------*
      Manages PLAYER_PREDECODING, PLAYER_PLAYING or PLAYER_SEEKING states. 

      Next states of PLAYER_PREDECODING state can be:
        - PLAYER_PLAYING if all pre-decoding is performed.
        - PLAYER_END if end of file is reached while all pre-decoding process 
          is not finished.
        - PLAYER_ERROR if an error is found.

      Next states of PLAYER_PLAYING state can be:
        - PLAYER_END if end of file is reached and all video frames have been
          displayed.
        - PLAYER_STOPPING StopPlayback is called.
        - PLAYER_ERROR if an error is found.

      Next states of PLAYER_SEEKING state can be:
        - PLAYER_END if end of file is reached while seek process is not 
          finished.
        - PLAYER_STOP if seek process is finished.
        - PLAYER_ERROR if an error is found.
     *-----------------------------------------------------------------------*/


    if (m_Status == PLAYER_PREDECODING && m_PauseTime == 0)
        DropEarlyDecodedVideoFrames();
    
    DropUnusedStreamsFrames();

    if( m_Status != PLAYER_SEEKING )
        DecodeAudioData();


    DecodeVideoData();

    /*-----------------------------------------------------------------------*
      Manages Moflex content reading. 
     *-----------------------------------------------------------------------*/

    ReadNewMoflexData();

    /*-----------------------------------------------------------------------*/
}

/*---------------------------------------------------------------------------*
  Constructor
 *---------------------------------------------------------------------------*/

Player::Player() : m_pAllocator(NULL), m_pDemuxer(NULL), m_pCodecInstantiation(NULL), 
                   m_pVideoPresentation(NULL), m_pAudioPresentation(NULL), m_NbVideoStreams(0), m_NbAudioStreams(0)
{
    for (int i=0; i<MAX_VIDEO_STREAM; i++)
    {
        m_pVideoCodec[i] = NULL;
        m_Timeline[i].m_pEntries = NULL;
    }

    for (int i=0; i<MAX_AUDIO_STREAM; i++)
        m_pAudioCodec[i] = NULL;
}

/*---------------------------------------------------------------------------*
  Destructor
 *---------------------------------------------------------------------------*/

Player::~Player()
{
    Release();
}

/*---------------------------------------------------------------------------*
  Initializes a Player instance.
 *---------------------------------------------------------------------------*/

bool Player::Create (gfl2::heap::NwAllocator* pAllocator, 
                     mw::mo::moflex::Demuxer* pDemuxer, 
                     CodecInstantiation* pCodecInstantiation, 
                     VideoPresentationBuffer* pVideoPresentation,
                     AudioPresentationBuffer* pAudioPresentation)
{
    if (pAllocator == NULL || pDemuxer == NULL || pCodecInstantiation == NULL || pVideoPresentation == NULL )
        return false;

    m_pAllocator = pAllocator;
    m_pDemuxer = pDemuxer;
    m_pCodecInstantiation = pCodecInstantiation;
    m_pVideoPresentation = pVideoPresentation;
    m_pAudioPresentation = pAudioPresentation;
    m_Status = PLAYER_OPENING;
    
    m_DecodedFrameSize = 0;
    m_NbDecodedFrame = 0;
    m_PlayerError = 0;

    m_NbAudioStreams = 0;
    m_CurrentAudioStream = -1;
    m_NbVideoStreams = 0;
    m_NbTimelineStreams = 0;
    
    for(int i=0; i<2; i++)
    {
        m_CurrentVideoStream[i] = -1;
        m_VideoFramePushIndex[i] = 0;
        m_VideoFramePopIndex[i] = 0;
    }
        
    m_AudioUnpacked = false;
    m_FirstAudioFrameTimeStamp = 0;
    m_LastPlayedImageTime = 0;
    m_NextPresentationTime = 0;
    m_PauseTime = 0;
    m_TotalVideoFramesDisplayed = 0;

    m_VideoConversion.Initialize();
    m_RunActivation.Initialize(true);
    m_RunSynchro.Initialize();
    m_RunActivation.Signal();
    m_EndRunThread = false;
    m_StreamInfoRead = false;

  {
    {
      gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_AUTO_STACK );
      mpStack = GFL_NEW(systemHeap) gfl2::thread::SimpleStackArea(systemHeap, RUN_THREAD_STACK_SIZE);
    }
    m_RunThread.Start(RunThreadFunc, this, *mpStack, RUN_THREAD_PRIORITY);
  }


    return true;
}

/*---------------------------------------------------------------------------*
  Finalizes a Player instance.
 *---------------------------------------------------------------------------*/

void Player::Release (void)
{
  if (m_pAllocator == NULL)
    return;

  m_EndRunThread = true;

  m_RunActivation.Signal();

  m_RunThread.Join();
  m_RunThread.Finalize();
  m_RunActivation.Finalize();
  m_RunSynchro.Finalize();

  FlushPendingFrames();

    for (int i=0;i<m_NbVideoStreams; i++)
    {
        m_pCodecInstantiation->DeleteVideoCodec(m_pAllocator, m_pVideoCodec[i]);
        m_pVideoCodec[i] = NULL;
    }

    for (int i=0;i<m_NbAudioStreams; i++)
    {
        m_pCodecInstantiation->DeleteAudioCodec(m_pAllocator, m_pAudioCodec[i]);
        m_pAudioCodec[i] = NULL;
    }

    for (int i=0;i<m_NbTimelineStreams; i++)
    {
        if (m_Timeline[i].m_pEntries != NULL)
        {
            m_pAllocator->Free(m_Timeline[i].m_pEntries);
            m_Timeline[i].m_pEntries = NULL;
        }
    }
    
    m_VideoConversion.Finalize();

    m_pAllocator = NULL;
    m_pDemuxer = NULL;
    m_pCodecInstantiation = NULL;
    m_pVideoPresentation = NULL;
    m_pAudioPresentation = NULL;

  GFL_SAFE_DELETE(mpStack);
  
}

/*---------------------------------------------------------------------------*
  Activates/deactivates automatic deblocking.
  If true is given as parameter then dynamic deblocking will be performed on 
  decoded video frames before YUV->RGB color conversion.
  If false is given then deblocking will be disabled.
 *---------------------------------------------------------------------------*/

void Player::EnableDeblocking (bool enable)
{
    m_VideoConversion.EnableDeblocking(enable);
}

/*---------------------------------------------------------------------------*
  Retrieves current player status, internally calls Yield method to give 
  CPU time to Run thread.
 *---------------------------------------------------------------------------*/

PlayerStatus Player::GetStatus (void)
{
    return m_Status;
}

/*---------------------------------------------------------------------------*
  Starts playback, can internally call Yield method to give CPU time to 
  Run thread.
 *---------------------------------------------------------------------------*/

bool Player::StartPlayback (void)
{
    if (m_Status != PLAYER_STOPPED && m_Status != PLAYER_END)
        return false;

    SetStatus(PLAYER_PREDECODING);

    m_RunActivation.Signal();

    /* Wait start of playback or error */  
    for(;;)
    {
        if (m_Status == PLAYER_PLAYING)
            return true;
        else if (m_Status == PLAYER_ERROR || m_Status == PLAYER_STOPPED)
            return false;

        /* This is to give time to the other running threads */
        Yield();
    }
}

/*---------------------------------------------------------------------------*
  Stops playback, decoders video queue are flushed, can internally call Yield 
  method to give CPU time to Run thread.    
 *---------------------------------------------------------------------------*/

bool Player::StopPlayback (void)
{
    /* Finish and remove any pending color conversion process */  
    if (VideoImageRequestPending() == true)
    {
        WaitVideoImage();
        VideoImageRequestEnd();
    }

    /* We can't stop a player that is not playing or in paused state, we return false */  
    if (m_Status != PLAYER_PLAYING && m_Status != PLAYER_PAUSED)
        return false;

    /* If not done yet, instruct the Run loop that is in a separate thread that we want to end the playback */  
    if (m_Status != PLAYER_STOPPED)
    {
        if (m_Status == PLAYER_PAUSED)
            m_RunActivation.Signal(); /* This is to awaken the Run thread that is sleeping when in stopped or paused state */
        SetStatus(PLAYER_STOPPING);
    }
    
    /* Wait end of playback or error */  //任天堂一林さまFix ml-poketech:1357
    while (true)
    {
        {
            nn::os::CriticalSection::ScopedLock csLock(m_RunSynchro);
            if (m_Status == PLAYER_STOPPED || m_Status == PLAYER_END || m_Status == PLAYER_ERROR)
                break;
        }
        /* This is to give time to the other running threads */
        Yield();
    }
  
    for (int i=0; i<2; i++)
    {
        if (m_CurrentVideoStream[i] >= 0)
        {
            int nbframesPerImage = GetNbImageFrames(GetVideoStreamInfo(m_CurrentVideoStream[i]));
            int pushedFrames;
            m_pVideoCodec[m_CurrentVideoStream[i]]->GetNbPushedFrame(pushedFrames);
            pushedFrames = (pushedFrames / nbframesPerImage) * nbframesPerImage;

            for (int j=0; j<pushedFrames; j++)
            {
                m_pVideoCodec[m_CurrentVideoStream[i]]->PopFrame();
                m_VideoFramePopIndex[i]++;
            }
        }
    }
    
    return true;
}

/*---------------------------------------------------------------------------*
  Pauses playback, decoders video queue are kept intact, can internally call 
  Yield method to give CPU time to Run thread.
 *---------------------------------------------------------------------------*/

bool Player::PausePlayback (void)
{
    /* Already paused, we return true */
    if (m_Status == PLAYER_PAUSED)
        return true;

    /* We can't stop a player that is not playing, we return false */  
    if (m_Status != PLAYER_PLAYING)
        return false;

    m_PauseTime = (s64)nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMicroSeconds();

    /* Instruct the Run loop that is in a separate thread that we want to pause the playback */  
    SetStatus(PLAYER_PAUSED);

    return true;
}

/*---------------------------------------------------------------------------*
  Resumes playback, can internally call Yield method to give CPU time to 
  Run thread.    
 *---------------------------------------------------------------------------*/

bool Player::ResumePlayback (void)
{
    if (m_Status != PLAYER_PAUSED)
        return false;

    SetStatus(PLAYER_PREDECODING);

    m_RunActivation.Signal();

    /* Wait start of playback or error */  
    for(;;)
    {
        if (m_Status == PLAYER_PLAYING)
        {
            s64 resumeTime = (s64)nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMicroSeconds();
            s64 pauseTimeElapsed = resumeTime - m_PauseTime;
    
            m_NextPresentationTime += pauseTimeElapsed;
            m_PtsToPlaybackTime += pauseTimeElapsed;

            return true;
        }
        else if (m_Status == PLAYER_ERROR || m_Status == PLAYER_STOPPED)
            return false;

        /* This is to give time to the other running threads */
        Yield();
    }
}

s64 Player::GetPresentedVideoImageTS(void)
{
	if (m_Status != PLAYER_PLAYING)
		return -1;

    int videoIndex = -1;
    for (int i=0; i<2; i++)
       if (m_CurrentVideoStream[i] >= 0)
		{
			videoIndex = i;
			break;
		}

	if (videoIndex == -1)
		return -1;

    return m_VideoFrameTimeStampArray[videoIndex][m_VideoFramePopIndex[videoIndex]%VIDEO_MAX_QUEUE_SIZE];
}

/*---------------------------------------------------------------------------*
  Tells if video image presentation time has elapsed.
 *---------------------------------------------------------------------------*/

s64 Player::VideoPresentationTimeElapsed (void)
{
    if (m_Status != PLAYER_PLAYING)
        return -1;

    if (m_NextPresentationTime == 0)
    {
        int videoIndex = -1;
        for (int i=0; i<2; i++)
            if (m_CurrentVideoStream[i] >= 0)
            {
                videoIndex = i;
                break;
            }

        if (videoIndex == -1)
            return -1;

        s64 videoDisplayDelay;
        if (m_FirstAudioFrameTimeStamp != 0)
            videoDisplayDelay = m_VideoFrameTimeStampArray[videoIndex][m_VideoFramePopIndex[videoIndex]%VIDEO_MAX_QUEUE_SIZE] - m_FirstAudioFrameTimeStamp;
        else
            videoDisplayDelay = 0;

        m_NextPresentationTime = (s64)nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMicroSeconds() + videoDisplayDelay;
        m_PtsToPlaybackTime = m_NextPresentationTime -  m_VideoFrameTimeStampArray[videoIndex][m_VideoFramePopIndex[videoIndex]%VIDEO_MAX_QUEUE_SIZE];
    }

    s64 currentPlaybackTime = (s64)nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMicroSeconds();
	return currentPlaybackTime - m_NextPresentationTime;
}

/*---------------------------------------------------------------------------*
  Start the request of a new video image if all needed video images are 
  decoded, this pushes a batch of color conversions.
  Destination RGB buffers are taken from VideoPresentation object and given to
  ColorConversion object with YUV buffers of decoded video images.
 *---------------------------------------------------------------------------*/

bool Player::VideoImageRequestStart(void)
{
    if (m_Status != PLAYER_PLAYING && m_Status != PLAYER_END)
        return false;

    for (int i=0; i<2; i++)
    {
        if (m_CurrentVideoStream[i] >= 0)
        {
            int pushedFrames;
            m_pVideoCodec[m_CurrentVideoStream[i]]->GetNbPushedFrame(pushedFrames);
            if (pushedFrames < GetNbImageFrames(&m_VideoStreamInfo[m_CurrentVideoStream[i]]))
                return false;            
        }
    }

    for (int i=0; i<2; i++)
    {
        if (m_CurrentVideoStream[i] >= 0)
        {
            int nbImages = GetNbImageFrames(&m_VideoStreamInfo[m_CurrentVideoStream[i]]);
            for (int j=0; j<nbImages; j++)
            {
                int screenIndex = i*2+j;
                void* ptr = m_pVideoPresentation->GetScreenBufferPtr(screenIndex);
                if (ptr != NULL)
                {
                    m_VideoConversion.SetY2rCoefficients(m_pVideoCodec[m_CurrentVideoStream[i]]->GetY2rCoefficient());
                    if (m_VideoConversion.PushConversion(m_pVideoCodec[m_CurrentVideoStream[i]], &m_VideoStreamInfo[m_CurrentVideoStream[i]], ptr, m_pVideoPresentation->GetScreenStride(screenIndex)) == false)
                        return false;
                }
            }
        }
    }

    return true;
}

/*---------------------------------------------------------------------------*
  Ends the video image request if batch of color conversions are all finished, 
  this pops all color conversions.
 *---------------------------------------------------------------------------*/

void Player::VideoImageRequestEnd(void)
{
    int nbImages = m_VideoConversion.GetNbConvertedImages();
    for (int i=0; i<nbImages; i++)
        m_VideoConversion.PopConversion();
    
    int oldTotalFrameDisplayed = m_TotalVideoFramesDisplayed;
    m_LastPlayedImageTime = 0;

    for (int i=0; i<2; i++)
    {
        if (m_CurrentVideoStream[i] >= 0)
        {
            if (m_LastPlayedImageTime == 0)
                m_LastPlayedImageTime = m_VideoFrameTimeStampArray[i][m_VideoFramePopIndex[i]%VIDEO_MAX_QUEUE_SIZE];

            int nbImageFrames = GetNbImageFrames(&m_VideoStreamInfo[m_CurrentVideoStream[i]]);
            m_VideoFramePopIndex[i] += nbImageFrames;
            
            if (oldTotalFrameDisplayed == m_TotalVideoFramesDisplayed)
                m_TotalVideoFramesDisplayed += nbImageFrames;
        }
    }

    SetNextVideoPresentationTime();
}

/*---------------------------------------------------------------------------*
  Skips the video image without doing all color conversion process.
 *---------------------------------------------------------------------------*/

void Player::VideoImageSkip(void)
{
	for (int i=0; i<2; i++)
    {
        if (m_CurrentVideoStream[i] >= 0)
		{
			int pushedFrames;
			m_pVideoCodec[m_CurrentVideoStream[i]]->GetNbPushedFrame(pushedFrames);
			if (pushedFrames < GetNbImageFrames(&m_VideoStreamInfo[m_CurrentVideoStream[i]]) * 2)
				return;
		}
	}

	for (int i=0; i<2; i++)
    {
		if (m_CurrentVideoStream[i] >= 0)
		{
			for (int j=0; j<GetNbImageFrames(&m_VideoStreamInfo[m_CurrentVideoStream[i]]); j++)
				m_pVideoCodec[m_CurrentVideoStream[i]]->PopFrame();
		}
	}
    
    int oldTotalFrameDisplayed = m_TotalVideoFramesDisplayed;
    m_LastPlayedImageTime = 0;

    for (int i=0; i<2; i++)
    {
        if (m_CurrentVideoStream[i] >= 0)
        {
            if (m_LastPlayedImageTime == 0)
                m_LastPlayedImageTime = m_VideoFrameTimeStampArray[i][m_VideoFramePopIndex[i]%VIDEO_MAX_QUEUE_SIZE];

            int nbImageFrames = GetNbImageFrames(&m_VideoStreamInfo[m_CurrentVideoStream[i]]);
            m_VideoFramePopIndex[i] += nbImageFrames;
            
            if (oldTotalFrameDisplayed == m_TotalVideoFramesDisplayed)
                m_TotalVideoFramesDisplayed += nbImageFrames;
        }
    }

    SetNextVideoPresentationTime();
}

/*---------------------------------------------------------------------------*
  Tells if a video image request is pending.
 *---------------------------------------------------------------------------*/

bool Player::VideoImageRequestPending(void)
{
    if (m_VideoConversion.GetNbPendingConversions() > 0)
        return true;

    return false;
}

/*---------------------------------------------------------------------------*
  Tells if a video image is available, meaning that the video image request 
  is finished.
 *---------------------------------------------------------------------------*/

bool Player::VideoImageAvailable(void)
{
    bool conversionRunReturn = m_VideoConversion.Run();
    if (conversionRunReturn == false)
    {
        NN_ASSERT(conversionRunReturn == true);
        m_Status = PLAYER_ERROR;
        return false;
    }

    if (m_VideoConversion.GetNbPendingConversions() == 0 || m_VideoConversion.GetNbPendingConversions() > m_VideoConversion.GetNbConvertedImages())
        return false;
 
    return true;
}

/*---------------------------------------------------------------------------*
  Waits for the availability of video images.
 *---------------------------------------------------------------------------*/

bool Player::WaitVideoImage(void)
{
    bool conversionWaiReturn = m_VideoConversion.WaitConversionDone();
    if (conversionWaiReturn == false)
    {
        NN_ASSERT(conversionWaiReturn == true);
        m_Status = PLAYER_ERROR;
        return false;
    }
 
    return true;
}

/*---------------------------------------------------------------------------*
  Gets the number of video streams in the Moflex content.
 *---------------------------------------------------------------------------*/

int Player::GetNbVideoStream(void)
{
	return m_NbVideoStreams;
}

/*---------------------------------------------------------------------------*
 Gets the video stream index that is associated to the top or bottom screen, 
 return -1 if none.
 *---------------------------------------------------------------------------*/

int Player::GetCurrentVideoStream(PlayerScreen screen)
{
    return m_CurrentVideoStream[screen];
}

/*---------------------------------------------------------------------------*
  Sets the video stream index that will be associated to the top or bottom 
  screen, give -1 in parameter (this is the default) if you want none of them 
  to be associated.
 *---------------------------------------------------------------------------*/

bool Player::SetCurrentVideoStream (PlayerScreen screen, int videoStream)
{
    if (videoStream < -1 || videoStream >= m_NbVideoStreams)
        return false;

    if (videoStream != m_CurrentVideoStream[screen] && m_CurrentVideoStream[screen] >= 0)
    {
        m_pCodecInstantiation->DeleteVideoCodec(m_pAllocator, m_pVideoCodec[m_CurrentVideoStream[screen]]);
        m_pVideoCodec[m_CurrentVideoStream[screen]] = NULL;
        m_CurrentVideoStream[screen] = -1;
    }

    if (videoStream >= 0)
    {
        if (m_pCodecInstantiation->CreateVideoCodec(m_pAllocator, m_VideoStreamInfo[videoStream], m_pVideoCodec[videoStream]) == false)    
        {
            m_PlayerError |= UNKNOWN_VIDEO_CODEC;
            return false;
        }
        m_CurrentVideoStream[screen] = videoStream;
    }

    return true;
}

/*---------------------------------------------------------------------------*
  Gets information on a Moflex video stream.
 *---------------------------------------------------------------------------*/

const mw::mo::moflex::VideoStreamInfo* Player::GetVideoStreamInfo(int videoStream)
{
    if (videoStream < 0 || videoStream >= m_NbVideoStreams)
        return NULL;

    return &m_VideoStreamInfo[videoStream];
}

/*---------------------------------------------------------------------------*
  Gets the number of frames needed to obtain a full video image, given the 
  video stream info. It is 2 if the video stream is stereoscopic in 
  interleaved mode, 1 otherwise.        
 *---------------------------------------------------------------------------*/

int Player::GetNbImageFrames(const mw::mo::moflex::VideoStreamInfo* videoInfo)
{
    if (videoInfo == NULL)
        return 0;

    if (videoInfo->m_Layout == mw::mo::moflex::LAYOUT_INTERLEAVED_LEFT_EYE_FIRST || 
        videoInfo->m_Layout == mw::mo::moflex::LAYOUT_INTERLEAVED_RIGHT_EYE_FIRST)
        return 2;

    return 1;
}

/*---------------------------------------------------------------------------*
  Gets the number of audio streams in the Moflex content
 *---------------------------------------------------------------------------*/

int Player::GetNbAudioStream (void)
{
    return m_NbAudioStreams;
}

/*---------------------------------------------------------------------------*
  Gets the audio stream index that is used for audio playback, 
  returns -1 if none.
 *---------------------------------------------------------------------------*/

int Player::GetCurrentAudioStream (void)
{
    return m_CurrentAudioStream;
}

/*---------------------------------------------------------------------------*
  Sets the audio stream index that will be used for audio playback, give -1 
  in parameter (this is the default) if you want none of them to be played.
  Waits that pending decoded audio is successfully sent to AudioPresention 
  object before switching of audio stream.
  Can internally calls Yield method to give CPU time to Run thread.
 *---------------------------------------------------------------------------*/

bool Player::SetCurrentAudioStream (int audioStream)
{
    if (audioStream < -1 || audioStream >= m_NbAudioStreams)
        return false;

    if (audioStream  == m_CurrentAudioStream)
        return true;

    if (m_pAudioPresentation == NULL)
    {
        m_PlayerError |= UNKNOWN_AUDIO_CODEC;
        return false;
    }

    while (m_AudioUnpacked == true)
    {
        /* This is to give time to the other running threads */
        Yield();
    }

    if (audioStream != m_CurrentAudioStream && m_CurrentAudioStream >= 0)
    {
        m_pCodecInstantiation->DeleteAudioCodec(m_pAllocator, m_pAudioCodec[m_CurrentAudioStream]);
        m_pAudioCodec[m_CurrentAudioStream] = NULL;
        m_CurrentAudioStream = -1;
    }

    if (audioStream >= 0)
    {
        m_pCodecInstantiation->CreateAudioCodec(m_pAllocator, m_AudioStreamInfo[audioStream], m_pAudioCodec[audioStream]);
        m_CurrentAudioStream = audioStream;
    }

    return true;
}

/*---------------------------------------------------------------------------*
  Gets information on a Moflex audio stream.
 *---------------------------------------------------------------------------*/

const mw::mo::moflex::AudioStreamInfo* Player::GetAudioStreamInfo(int audioStream)
{
    if (audioStream < 0 || audioStream >= m_NbAudioStreams)
        return NULL;

    return &m_AudioStreamInfo[audioStream];
}

/*---------------------------------------------------------------------------*
  Gets the number of timeline streams in the Moflex content.
 *---------------------------------------------------------------------------*/

int Player::GetNbTimeline (void)
{
    return m_NbTimelineStreams;
}

/*---------------------------------------------------------------------------*
  Gets information on a Moflex timeline stream.
 *---------------------------------------------------------------------------*/

const mw::mo::moflex::TimelineStreamInfo* Player::GetTimelineStreamInfo (int timelineStream)
{
    if (timelineStream < 0 || timelineStream >= m_NbTimelineStreams)
        return NULL;

    return &m_TimelineStreamInfo[timelineStream];
}

/*---------------------------------------------------------------------------*
  Gets timeline stream index that indexes a given video stream, -1 if none.
 *---------------------------------------------------------------------------*/

int Player::GetTimelineStreamFromVideoStream (int videoStream)
{
    if (videoStream < 0 || videoStream >= m_NbVideoStreams)
        return -1;
    
    int videoStreamIndex = m_VideoStreamIndex[videoStream];

    for (int i=0; i<m_NbTimelineStreams; i++)
    {
        if (videoStreamIndex == m_TimelineStreamInfo[i].m_AssociatedStreamIndex)
            return i;
    }

    return -1;
}

/*---------------------------------------------------------------------------*
  Gets number of entries of a Moflex timeline stream.
 *---------------------------------------------------------------------------*/

u32 Player::GetTimelineNbEntries (int timelineStream)
{
    if (timelineStream < 0 || timelineStream >= m_NbTimelineStreams)
        return 0;

    if (m_Timeline[timelineStream].m_pEntries == NULL)
        return 0;

    return m_Timeline[timelineStream].m_Header.m_NbEntries;
}

/*---------------------------------------------------------------------------*
  Gets the total number of video frames given by a Moflex timeline stream.
 *---------------------------------------------------------------------------*/

u32 Player::GetTimelineTotalFrames (int timelineStream)
{
    if (timelineStream < 0 || timelineStream >= m_NbTimelineStreams)
        return 0;

    if (m_Timeline[timelineStream].m_pEntries == NULL)
        return 0;

    return m_Timeline[timelineStream].m_Header.m_TotalFrames;
}

/*---------------------------------------------------------------------------*
  Gets the duration in microseconds given by a Moflex timeline stream.
 *---------------------------------------------------------------------------*/

s64 Player::GetTimelineDuration (int timelineStream)
{
    if (timelineStream < 0 || timelineStream >= m_NbTimelineStreams)
        return 0;

    if (m_Timeline[timelineStream].m_pEntries == NULL)
        return 0;

    return m_Timeline[timelineStream].m_Header.m_Duration;
}

/*---------------------------------------------------------------------------*
  Gets an entry information given an entry index for a Moflex timeline stream.
 *---------------------------------------------------------------------------*/

mw::mo::moflex::TimelineEntry* Player::GetTimelineEntryByIndex (int timelineStream, u32 index)
{
    if (timelineStream < 0 || timelineStream >= m_NbTimelineStreams)
        return NULL;

    if (m_Timeline[timelineStream].m_pEntries == NULL || index >= m_Timeline[timelineStream].m_Header.m_NbEntries)
        return NULL;

    return &(m_Timeline[timelineStream].m_pEntries[index]);
}

/*---------------------------------------------------------------------------*
  Gets an entry information given a video frame number for a Moflex timeline 
  stream.
  The entry will point to a position on the Moflex content that is before or 
  is on video frame number position.
 *---------------------------------------------------------------------------*/

mw::mo::moflex::TimelineEntry* Player::GetTimelineEntryByFrameNumber (int timelineStream, u32 frameNumber)
{
    if (timelineStream < 0 || timelineStream >= m_NbTimelineStreams)
        return NULL;

    if (m_Timeline[timelineStream].m_pEntries == NULL || frameNumber >= m_Timeline[timelineStream].m_Header.m_TotalFrames)
        return NULL;

    for (u32 i=0; i < m_Timeline[timelineStream].m_Header.m_NbEntries; i++)
    {
        if (m_Timeline[timelineStream].m_pEntries[i].m_FrameNumber <= frameNumber && (i == m_Timeline[timelineStream].m_Header.m_NbEntries-1 || m_Timeline[timelineStream].m_pEntries[i+1].m_FrameNumber > frameNumber))
            return &(m_Timeline[timelineStream].m_pEntries[i]);
    }

    return NULL;
}

/*---------------------------------------------------------------------------*
  Gets an entry information given a timestamp in microseconds for a Moflex 
  timeline stream.
  The entry will point to a position on the Moflex content that is before or 
  on timestamp time.
 *---------------------------------------------------------------------------*/

mw::mo::moflex::TimelineEntry* Player::GetTimelineEntryByTimeStamp (int timelineStream, s64 timeStamp)
{
    if (timelineStream < 0 || timelineStream >= m_NbTimelineStreams)
        return NULL;

    if (m_Timeline[timelineStream].m_pEntries == NULL)
        return NULL;

    for (u32 i=0; i < m_Timeline[timelineStream].m_Header.m_NbEntries; i++)
    {
        if (m_Timeline[timelineStream].m_pEntries[i].m_FrameTimeStamp <= timeStamp && (i == m_Timeline[timelineStream].m_Header.m_NbEntries-1 || m_Timeline[timelineStream].m_pEntries[i+1].m_FrameTimeStamp > timeStamp))
            return &(m_Timeline[timelineStream].m_pEntries[i]);
    }

    return NULL;
}

/*---------------------------------------------------------------------------*
  Seeks into Moflex content to a position specified by the given timeline 
  entry.
  Internally calls SetPositionByFileOffset method.
 *---------------------------------------------------------------------------*/

PlayerStatus Player::SetPositionByTimeline (mw::mo::moflex::TimelineEntry* timelineEntry)
{
    if (timelineEntry == NULL)
    {
        NN_ASSERT(timelineEntry != NULL);
        SetStatus(PLAYER_ERROR);
        return m_Status;
    }
    
    return SetPositionByFileOffset(timelineEntry->m_FrameSyncroBlockOffset);
}

/*---------------------------------------------------------------------------*
  Seeks into Moflex content to a position specified by the given raw file 
  offset.
 *---------------------------------------------------------------------------*/

PlayerStatus Player::SetPositionByFileOffset (s64 fileOffset)
{
    if (m_Status == PLAYER_ERROR)
        return m_Status;

    mw::mo::moflex::DemuxerReturn demuxerReturn = m_pDemuxer->SetContentPosition(fileOffset);
    if (demuxerReturn != mw::mo::moflex::DEMUXER_RETURN_OK)
    {
        NN_ASSERT(demuxerReturn == mw::mo::moflex::DEMUXER_RETURN_OK);
        SetStatus(PLAYER_ERROR);
        return m_Status;
    }

    FlushPendingFrames();

    SetStatus(PLAYER_SEEKING);

    m_RunActivation.Signal();
    
    /* Wait end of seeking process or error */        
    while (m_Status == PLAYER_SEEKING)
    {
        /* This is to give time to the other running threads */
        Yield();
    }

    return m_Status;
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

s64 Player::GetLastPlayedImageTime (void)
{
    return m_LastPlayedImageTime;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void Player::DrawPlaybackInformation (void* screenPtr, u32 screenWidth, u32 screenHeight)
{
    u8* ptr = (u8*)screenPtr;
    int posY = 0;

    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, "________________________________________");
    posY += mw::mo::helper::font::GetFontCharHeight() * 2;

    s64 position;
    m_pDemuxer->GetContentPosition(position);
    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 0, "Play position: %d bytes", (int)position);
    posY += mw::mo::helper::font::GetFontCharHeight();

    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 0, "Total video frames played: %d", m_TotalVideoFramesDisplayed);
    posY += mw::mo::helper::font::GetFontCharHeight();
    if (m_CurrentVideoStream[0] >= 0)
        mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 0, "Playback duration: %d seconds", (m_TotalVideoFramesDisplayed * m_VideoStreamInfo[m_CurrentVideoStream[0]].m_FpsScale) / m_VideoStreamInfo[m_CurrentVideoStream[0]].m_FpsRate);
    else if (m_CurrentVideoStream[1] >= 0)
        mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 0, "Playback duration: %d seconds", (m_TotalVideoFramesDisplayed * m_VideoStreamInfo[m_CurrentVideoStream[1]].m_FpsScale) / m_VideoStreamInfo[m_CurrentVideoStream[1]].m_FpsRate);
    posY += mw::mo::helper::font::GetFontCharHeight();

    u32 nbStream;
    m_pDemuxer->GetStreamCount(nbStream);
    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, "________________________________________");
    posY += mw::mo::helper::font::GetFontCharHeight() * 2;

    s64 size;
    m_pDemuxer->GetContentSize(size);
    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, "File size: %d bytes", (int)size);
    posY += mw::mo::helper::font::GetFontCharHeight();

    if (GetTimelineDuration(0) == 0)
        mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, "Duration: Unknown");
    else
        mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, "Duration: %d seconds", (int)(GetTimelineDuration(0)/1000000));
    posY += mw::mo::helper::font::GetFontCharHeight();

    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, "Number of streams: %d",nbStream);
    posY += mw::mo::helper::font::GetFontCharHeight();

    for (int i=0; i<nbStream; i++)
    {
        mw::mo::moflex::StreamInfo stream;
        m_pDemuxer->GetStreamInfo(i,stream);
        if (stream.m_Generic.m_StreamType == mw::mo::moflex::STREAM_TYPE_VIDEO)
        {
            if ((screenHeight - posY) <  mw::mo::helper::font::GetFontCharHeight() * 5)
            {
                mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, "...");
                break;
            }
            mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, "Stream %d type: video", i);
            posY += mw::mo::helper::font::GetFontCharHeight();
            mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Resolution: %dx%d", stream.m_Video.m_Width, stream.m_Video.m_Height);
            posY += mw::mo::helper::font::GetFontCharHeight();
            mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Framerate: %f fps", (float)stream.m_Video.m_FpsRate/(float)stream.m_Video.m_FpsScale);
            posY += mw::mo::helper::font::GetFontCharHeight();
            switch (stream.m_Video.m_Layout)
            {
                case mw::mo::moflex::LAYOUT_NONE:
                    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Layout: 2D");
                    break;
                case mw::mo::moflex::LAYOUT_INTERLEAVED_LEFT_EYE_FIRST:
                    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Layout: 3D interleaved, left eye first");
                    break;
                case mw::mo::moflex::LAYOUT_INTERLEAVED_RIGHT_EYE_FIRST:
                    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Layout: 3D interleaved, right eye first");
                    break;
                case mw::mo::moflex::LAYOUT_SIDEBYSIDE_LEFT_EYE_FIRST:
                    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Layout: 3D side by side, left eye first");
                    break;
                case mw::mo::moflex::LAYOUT_SIDEBYSIDE_RIGHT_EYE_FIRST:
                    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Layout: 3D side by side, right eye first");
                    break;
                case mw::mo::moflex::LAYOUT_TOPTOBOTTOM_LEFT_EYE_FIRST:
                    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Layout: 3D top to bottom, left eye first");
                    break;
                case mw::mo::moflex::LAYOUT_TOPTOBOTTOM_RIGHT_EYE_FIRST:
                    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Layout: 3D top to bottom, right eye first");
                    break;
            }
            posY += mw::mo::helper::font::GetFontCharHeight();

            switch (stream.m_Video.m_Rotation)
            {
                case mw::mo::moflex::ROTATION_NONE:
                    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Rotation: none");
                    break;
                case mw::mo::moflex::ROTATION_CLOCKWISE_90:
                    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Rotation: 90 degrees clockwise");
                    break;
                case mw::mo::moflex::ROTATION_CLOCKWISE_180:
                    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Rotation: 180ｰ degrees clockwise");
                    break;
                case mw::mo::moflex::ROTATION_CLOCKWISE_270:
                    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Rotation: 270ｰ degrees clockwise");
                    break;
            }
            posY += mw::mo::helper::font::GetFontCharHeight();
        }
        else if (stream.m_Generic.m_StreamType == mw::mo::moflex::STREAM_TYPE_AUDIO)
        {
            if ((screenHeight - posY) <  mw::mo::helper::font::GetFontCharHeight() * 4)
            {
                mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, "...");
                break;
            }
            mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, "Stream %d type: audio", i);
            posY += mw::mo::helper::font::GetFontCharHeight();
            mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Frequency: %d", stream.m_Audio.m_Frequency);
            posY += mw::mo::helper::font::GetFontCharHeight();
            mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Channels: %d", stream.m_Audio.m_Channel);
            posY += mw::mo::helper::font::GetFontCharHeight();
            switch (stream.m_Audio.m_AudioCodec)
            {
                case mw::mo::moflex::AUDIO_CODEC_PCM:
                    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Format: PCM");
                    break;
                case mw::mo::moflex::AUDIO_CODEC_IMAADPCM:
                    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Format: IMAADPCM");
                    break;    
                case mw::mo::moflex::AUDIO_CODEC_DSPADPCM:
                    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Format: DSPADPCM");
                    break;
                case mw::mo::moflex::AUDIO_CODEC_FASTAUDIO:
                    mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Format: FastAudio");
                    break;    
            }
            posY += mw::mo::helper::font::GetFontCharHeight();
        }
        else if (stream.m_Generic.m_StreamType == mw::mo::moflex::STREAM_TYPE_TIMELINE)
        {
            if ((screenHeight - posY) <  mw::mo::helper::font::GetFontCharHeight() * 2)
            {
                mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, "...");
                break;
            }
            mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, "Stream %d type: timeline", i);
            posY += mw::mo::helper::font::GetFontCharHeight();
            mw::mo::helper::font::PrintLineRgb24(ptr, screenWidth, screenHeight, 0, posY, 255, 255, 255, " -Indexed video stream: %d", stream.m_Timeline.m_AssociatedStreamIndex);
            posY += mw::mo::helper::font::GetFontCharHeight();
        }
    }    
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void Player::PrintStatistics (void)
{
    for(int i=0; i<m_NbVideoStreams; i++)
    {
        if (m_pVideoCodec[i] != NULL)
            m_pVideoCodec[i]->PrintStatistics();
    }
    for(int i=0; i<m_NbAudioStreams; i++)
    {
        if (m_pAudioCodec[i] != NULL)    
            m_pAudioCodec[i]->PrintStatistics();
    }

    m_VideoConversion.PrintStatistics();
}

u32 Player::GetError()
{
    return m_PlayerError;
}

void Player::AddError( PlayerError error )
{
    m_PlayerError |= error;
}

u32 Player::PopDecodedFramesCount()
{
    u32 r = m_NbDecodedFrame;
    m_NbDecodedFrame = 0;
    return r;
}

u32 Player::PopDecodedFramesSize()
{
    u32 r = m_DecodedFrameSize;
    m_DecodedFrameSize = 0;
    return r;
}




/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/
#endif// defined(GF_PLATFORM_CTR)
