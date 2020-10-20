//======================================================================
/**
 *  GAME FREAK inc.
 *
 *  @author miyuki iwasawa 
 *
 */
//====================================================================== 

#ifndef __H_HIO_SOUND_EDITOR_H__
#define __H_HIO_SOUND_EDITOR_H__

//#include <nn/os.h>
//#include <nn/fs.h>

#include <nw/types.h>

#include <nw/ut.h>
#include <nw/snd.h>
#include <nw/snd/snd_Config.h>
#include <nw/snd/edit/sndedit_SoundEdit.h>

#include <Sound/include/Sound.h>

#if PM_DEBUG
  #ifdef NW_SND_CONFIG_ENABLE_DEV
     ///#ifdef GFL_USE_HIO_SOUND_EDIT
     #ifdef GF_PLATFORM_CTR
       #define GF_HIO_SOUND_EDITOR_ENABLE
     #endif //GF_PLATFORM_CTR
     ///#endif
  #endif
#endif

#ifdef GF_HIO_SOUND_EDITOR_ENABLE

namespace Sound {

  class HioSoundEditor
  {
    public:

      HioSoundEditor( gfl2::heap::HeapBase* p_heap );
      ~HioSoundEditor();

      void  Initialize( gfl2::heap::HeapBase* p_heap );
      void  Update( void );
      void  Release( void );
  
    private:
      //gfl2::heap::HeapBase* m_pHeap;
      nw::snd::edit::SoundEdit m_SoundEdit;
      bool m_IsConnected;
    
      void* m_pMemoryForSoundEdit;
      void* m_pMemoryForHioWork;
      void* m_pMemoryForStringBlock;
  };


} // namespace Sound


#endif  //#ifdef GF_HIO_SOUND_EDITOR_ENABLE

#endif  //#ifdef __H_HIO_SOUND_EDITOR_H__
