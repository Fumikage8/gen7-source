//=============================================================================
/**
 * @brief 
 * @file sound_3D.cpp
 * @author obata
 * @date 2011.10.12
 */
//=============================================================================

#include "Sound/include/Sound.h"
#include "Sound/include/SoundLocal.h"
#include "Sound/include/PMSoundSystem.h"

namespace Sound { 

  void Set3DParam( const gfl2::sound::SoundSystem::Sound3DParam& param )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->Set3DParam( param );
  }

  void Get3DParam( gfl2::sound::SoundSystem::Sound3DParam* buffer ) 
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->Get3DParam( buffer );
  }

  void SetListenerParam( const gfl2::sound::SoundSystem::ListenerParam& param )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->SetListenerParam( param );
  }

  void GetListenerParam( gfl2::sound::SoundSystem::ListenerParam* buffer )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->GetListenerParam( buffer );
  }

  void SetListenerMatrix( const gfl2::math::MTX34& matrix )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->SetListenerMatrix( matrix );
  }

  void SetListenerPos( const gfl2::math::VEC3& pos, const gfl2::math::VEC3& up, const gfl2::math::VEC3& dir )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->SetListenerPos( pos, up, dir );
  }

  void SetListenerVelocity( const gfl2::math::VEC3& velocity )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->SetListenerVelocity( velocity );
  }

  b32 Play3DActor( u8 actor_index, u32 soundID , const nw::snd::SoundStartable::StartInfo* startInfo )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(false);
    return s_PMSoundSystem->Play3DActor( actor_index, soundID , startInfo );
  }

  void Stop3DActor( u8 actor_index, s32 fadeFrames )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->Stop3DActor( actor_index, fadeFrames );
  }

  void Pause3DActor( u8 actor_index, b32 pauseOn, s32 fadeFrames )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->Pause3DActor( actor_index, pauseOn, fadeFrames );
  }

  void Set3DActorVolume( u8 actor_index, f32 volume )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->Set3DActorVolume( actor_index, volume );
  }

  f32 Get3DActorVolume( u8 actor_index ) 
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(0.0f);
    return s_PMSoundSystem->Get3DActorVolume( actor_index );
  }

  void Set3DActorPitch( u8 actor_index, f32 pitch )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->Set3DActorPitch( actor_index, pitch );
  }

  f32 Get3DActorPitch( u8 actor_index ) 
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(0.0f);
    return s_PMSoundSystem->Get3DActorPitch( actor_index );
  }

  void Set3DActorPan( u8 actor_index, f32 pan )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->Set3DActorPan( actor_index, pan );
  }

  f32 Get3DActorPan( u8 actor_index ) 
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(0.0f);
    return s_PMSoundSystem->Get3DActorPan( actor_index );
  }

  void Set3DActorPos( u8 actor_index, const gfl2::math::VEC3& pos )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->Set3DActorPos( actor_index, pos );
  }

  void Get3DActorPos( u8 actor_index, gfl2::math::VEC3* pos ) 
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->Get3DActorPos( actor_index, pos );
  }

  void Set3DActorVelocity( u8 actor_index, const gfl2::math::VEC3& velocity )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->Set3DActorVelocity( actor_index, velocity );
  }

} // namespace Sound


