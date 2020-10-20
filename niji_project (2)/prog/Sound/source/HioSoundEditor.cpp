//======================================================================
/**
 *  GAME FREAK inc.
 *
 *  @author miyuki iwasawa 
 *
 */
//====================================================================== 
#include "Sound/include/HioSoundEditor.h"

#include "Sound/include/Sound.h"
#include "Sound/include/PMSoundSystem.h"
#include <System/include/HeapDefine.h>
#include <debug/include/gfl2_DebugPrint.h>

#ifdef GF_HIO_SOUND_EDITOR_ENABLE

namespace Sound {

HioSoundEditor::HioSoundEditor( gfl2::heap::HeapBase* p_heap )
{
  this->Initialize(p_heap);
}

HioSoundEditor::~HioSoundEditor()
{
  this->Release();
}

void HioSoundEditor::Initialize( gfl2::heap::HeapBase* p_heap )
{
  SOUND_UPDATE_CRITICALSECTION;

  //gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG ); //ちょっと間借り
  Sound::PMSoundSystem* pm_sound_system = Sound::GetPMSoundSystem();
  gfl2::sound::SoundSystem* gfl_sound_system = pm_sound_system->GetSoundSystem();
  //nw::snd::SoundArchive* sound_archive = dynamic_cast<nw::snd::RomSoundArchive*>(gfl_sound_system->GetSoundArchive());//↓のように変更
  nw::snd::RomSoundArchive* sound_archive = gfl_sound_system->GetRomSoundArchive();
  nw::snd::SoundArchivePlayer* sound_archive_player = gfl_sound_system->GetSoundArchivePlayer();
  
  //HIOの初期化
  {
    m_pMemoryForHioWork = GflHeapAllocMemoryAlign( p_heap, nn::hio::WORKMEMORY_SIZE, 4 );
    nn::hio::Initialize( m_pMemoryForHioWork );
  }
  // STRING ブロックのロード
  {
    u32 stringBlockSize = sound_archive->GetLabelStringDataSize();
    m_pMemoryForStringBlock = GflHeapAllocMemoryAlign( p_heap, stringBlockSize, 4 );
    if ( ! sound_archive->LoadLabelStringData( m_pMemoryForStringBlock, stringBlockSize ) )
    {
      GFL_ASSERT( 0 );
    }
  }
  // サウンドエディットの初期化
  {
    nw::snd::edit::SoundEdit::Option option;

    u32 memorySize = m_SoundEdit.GetRequiredMemorySize(option);
    m_pMemoryForSoundEdit = GflHeapAllocMemoryAlign( p_heap, memorySize, 4 );

    nw::snd::edit::SoundEdit::InitializeArg args;
    args.buffer             = m_pMemoryForSoundEdit;
    args.bufferSize         = memorySize;
    args.soundArchive       = sound_archive;
    args.soundArchivePlayer = sound_archive_player;

    m_SoundEdit.Initialize(args, option);
    m_SoundEdit.Start();
  }
}

void HioSoundEditor::Release( void )
{
  SOUND_UPDATE_CRITICALSECTION;

  //サウンドエディットの終了
  {
    m_SoundEdit.Stop();
    m_SoundEdit.Finalize();
    GflHeapFreeMemory( m_pMemoryForSoundEdit );
  }
  GflHeapFreeMemory( m_pMemoryForStringBlock );
  //HIOの終了
  {
    nn::hio::Finalize();
    GflHeapFreeMemory(m_pMemoryForHioWork);
  }
}

void HioSoundEditor::Update( void )
{
  SOUND_UPDATE_CRITICALSECTION;

  m_SoundEdit.Update();

  if (m_IsConnected != m_SoundEdit.IsConnected())
  {  
    if(m_IsConnected)
    {
      m_IsConnected = false;
      GFL_PRINT("disconnected.\n");
    }
    else
    {
      m_IsConnected = true;
      GFL_PRINT("connected.\n");
    }
  }
}

} // namespace Sound

#endif  //#ifdef __H_HIO_SOUND_EDITOR_H__

