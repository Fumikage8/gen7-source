#if defined(GF_PLATFORM_CTR)

#ifndef __MOBICLIPPER_H__
#define __MOBICLIPPER_H__

#pragma once

//#include <gflib.h>
#include "heap/include/gfl2_Heap.h"
#include "heap/include/gfl2_NwAllocator.h"
#include "Layout/include/gfl2_GraphicsType.h"
#include <System/include/SystemEventListener.h>

#include "helper/helper_Player.h"
#include "helper/helper_GlVideoPresentation.h"
#include "helper/helper_GlVideoPresentation.h"
#include "helper/helper_FbVideoPresentation.h"
#include "helper/helper_PcmAudioPresentation.h"
#include "helper/helper_AacAudioPresentation.h"
#include "helper/helper_DisplayDevice.h"
#include "helper/helper_AudioDevice.h"
#include "helper/helper_FsReader.h"
#include "helper/helper_MobiclipCodecInstantiation.h"


namespace gfl2 {
namespace util {
class GLHeapAllocator;
}
}
namespace movie {
namespace renderer {
class MobiclipRenderingPipeLine;
}
}


namespace movie {

//! ムービー再生のための情報を扱う構造体
//! 必要に応じて拡張予定
struct Option
{
	wchar_t* filename;	//!< 再生するMobiclipファイルの場所
	bool need_aac_codec;//!< aacコーデックファイルが必要（未対応）
	float volume;
	int fadein_time;
	int fadeout_time;
	int screen;         //!< ムービー表示先のスクリーン
  bool audioFade;

	Option() : 
	need_aac_codec(false),
	volume(1.0f),
	fadein_time(0),
	fadeout_time(0),
	screen(gfl2::lyt::DISPLAY_UPPER),
  audioFade(true)
	{}
};

//! ムービー再生のメインクラス
//! Initialize後に、Start・Drawを行なってください
class Mobiclipper : public System::SystemEventListener
{
public:
	Mobiclipper();
	virtual ~Mobiclipper(); //@fix KW[1903]

	void Initialize(
					gfl2::heap::NwAllocator* heap_allocator,
					gfl2::heap::NwAllocator* device_allocator,
          gfl2::util::GLHeapAllocator* pGLDeviceAllocator,
          movie::renderer::MobiclipRenderingPipeLine* renderingPipeLine,
					bool enable_sound);

	bool Finalize();
	void Play(const Option& opt);
	bool Stop();
	bool IsPlaying();
	void PlayAgain();	//!< 頭からもう一度再生する
	void RequestEnd(); //!< 終了
	bool Update();
	bool Draw();
  void DrawEnd();
	void DrawCost();
  bool IsUpdate();

	void EnableDeblocking(bool flag);

  virtual void OnSuspendWithSleep(void);
  virtual void OnResumeWithSleep(void);
  virtual void OnSuspendWithHomeMenu( void );
  virtual void OnResumeWithHomeMenu( void );

  virtual bool OnCloseApplication(void);
  
private:
	gfl2::heap::NwAllocator* m_alloc;
	gfl2::heap::NwAllocator* m_device_alloc;
  gfl2::util::GLHeapAllocator* m_pGLDeviceAllocator;
  movie::renderer::MobiclipRenderingPipeLine* m_renderingPipeLine;

	// mobiclip関連2
	Option m_option;

	// mobiclip関連
	nn::fs::FileInputStream m_file;
	mw::mo::helper::GlVideoPresentation m_video;
	mw::mo::helper::PcmAudioPresentation m_audio;
	mw::mo::helper::AacAudioPresentation m_audio_aac;
	mw::mo::helper::Player m_player;
	mw::mo::helper::MobiclipCodecInstantiation m_codec;
	mw::mo::moflex::Demuxer m_demuxer;
	mw::mo::helper::FsReader m_reader;

	u8* dspBufferPtr;

	bool m_need_aac;
	
	int m_fade_timer;

  bool m_enable_sound;

  enum FadeSeq{
  	SEQ_FADEIN_WAIT,
  	SEQ_MAIN,
  	SEQ_FADEOUT_WAIT,
  	SEQ_END,
  };
  FadeSeq m_seq;
  int m_stop_seq;
  int m_finalize_seq;
  s64 m_presentationInterval;

#if PM_DEBUG
  void DebugPrintVideoStreamInfo( const mw::mo::moflex::VideoStreamInfo* info );
#endif
};

} //namespace movie


#endif //__MOBICLIPPER_H__
#endif //defined(GF_PLATFORM_CTR)
