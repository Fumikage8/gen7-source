#ifndef __SOUNDLOCAL_H__
#define __SOUNDLOCAL_H__

namespace Sound {

  class PMSoundSystem;
  extern PMSoundSystem* s_PMSoundSystem; 

  extern void Initialize_SYSTEM( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* device_heap );
  extern void Initialize_BGM( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* device_heap );
  extern void Initialize_SE( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* device_heap );
  extern void Initialize_VOICE( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* device_heap );
  extern void Initialize_PLAYER( void );

  extern void Finalize_SYSTEM( void );
  extern void Finalize_BGM( void );
  extern void Finalize_SE( void );
  extern void Finalize_VOICE( void );
  extern void Finalize_PLAYER( void );

  extern void Update_SYSTEM( void );
  extern void Update_BGM( void );
  extern void Update_VOICE( void );
  extern void Update_PLAYER( void );

} //namespace Sound

#endif //__SOUNDLOCAL_H__
