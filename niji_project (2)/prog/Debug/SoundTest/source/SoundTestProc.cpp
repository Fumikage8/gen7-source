//======================================================================
/**
 * @file SoundTestProc.cpp
 * @date 2015/10/15 17:43:20
 * @author saito_nozomu
 * @brief サウンドテスト用プロセス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_DrawUtilText.h>
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>
#include <fade/include/gfl2_FadeManager.h>
#include <Layout/include/gfl2_LytSys.h>

#include <System/include/HeapDefine.h>
#include <System/include/GflUse.h>
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/DllProc.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_AppLytAccessor.h>
#include "Sound/include/Sound.h"
#include "Sound/include/HioSoundEditor.h"

#include  "Print/include/wordset.h"
#include <arc_def_index/arc_def.h>
#include <arc_index/debug_battle.gaix>
#include <arc_index/debug_message.gaix>

//通信用
#ifdef  GF_PLATFORM_CTR
#include <NetStatic/NetLib/include/NijiNetworkSystem.h>
#include <NetStatic/NetLib/include/Wifi/WifiConnectRunner.h>
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#include <gflnet2/include/nex/gflnet2_NexMatchMakingClient.h>
#endif

#include "Debug/SoundTest/include/SoundTestProc.h"
///#include "Debug/SoundTest/include/SoundConfig.h"
#include "Debug/SoundTest/include/UpdaterBase.h"
#include "Debug/SoundTest/include/UpdaterBGM.h"
#include "Debug/SoundTest/include/UpdaterME.h"
#include "Debug/SoundTest/include/UpdaterSE.h"
#include "Debug/SoundTest/include/UpdaterPV.h"
#include "Debug/SoundTest/include/UpdaterPVZukan.h"
#include "Debug/SoundTest/include/UpdaterPika.h"

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(sound)

#if defined(GF_PLATFORM_CTR)
const char* SOUND_FILE_PATH = "rom:/data/sound/debug/out.txt";
const char* VOICE_FILE_PATH = "rom:/data/sound/debug/out2.txt";
#elif defined(GF_PLATFORM_WIN32)
const char* SOUND_FILE_PATH = "R:/home/momiji/exec/data_ctr/data/sound/debug/out.txt";//L"rom:/sounddebug/out.txt";
const char* VOICE_FILE_PATH = "R:/home/momiji/exec/data_ctr/data/sound/debug/out2.txt";//L"rom:/sounddebug/out2.txt";
#endif

//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
SoundTestProc::SoundTestProc( void )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
SoundTestProc::~SoundTestProc( )
{
}

//-----------------------------------------------------------------------------
/**
  * @brief   PROCの初期化を行う関数
  * @param   プロセス管理マネージャー
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
  */
//-----------------------------------------------------------------------------
gfl2::proc::Result SoundTestProc::InitFunc( gfl2::proc::Manager* pManager )
{
  for (int i=0; i<PAGE_MAX;i++)
  {
    m_pUpdaterList[i] = NULL;
  }

  // アロケータ
  gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ); 
	gfl2::heap::HeapBase* dev_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

	m_heap_alloc = GFL_NEW(heap) gfl2::heap::NwAllocator(heap);
	m_device_alloc  = GFL_NEW(dev_heap) gfl2::heap::NwAllocator(dev_heap);
  m_pDeviceHeapAllocator = GFL_NEW( dev_heap ) System::nijiAllocator( dev_heap, System::nijiAllocator::APPLICATION );

  // レンダリングパイプラインの生成
	m_pRenderingPipeLine  = GFL_NEW( pManager->GetHeap() ) gfl2::util::UtilTextRenderingPipeLine( m_pDeviceHeapAllocator, 512, 256 );
	// DrawUtilTextの生成
	m_pDrawUtilText = GFL_NEW(heap) gfl2::util::DrawUtilText(m_heap_alloc, m_device_alloc);
	m_pDrawUtilText->Initialize( m_heap_alloc, m_device_alloc, m_pRenderingPipeLine->GetUtilTextRenderPath() );
	m_pDrawUtilText->SetFont( print::SystemFont_GetFont() );

  const gfl2::lyt::LytSysSetUp setup = {
	  GFL_SINGLETON_INSTANCE(print::SystemFont)->GetRectDrawerShaderRes(),
    10, // lyt_resource_max
    10  // lyt_wk_max
  };
  m_pLytSys = GFL_NEW( heap ) gfl2::lyt::LytSys( &setup, m_heap_alloc, m_device_alloc ); 
  

  this->Enter();

  return gfl2::proc::RES_FINISH;
}

//---------------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @param   プロセス管理マネージャー
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//---------------------------------------------------------------------------
gfl2::proc::Result SoundTestProc::EndFunc(gfl2::proc::Manager* pManager)
{
  if( m_pLytSys )
  {
    GFL_DELETE  m_pLytSys;
    m_pLytSys = NULL;
  }

	if( m_pRenderingPipeLine != NULL )
	{
		m_pRenderingPipeLine->GetUtilTextRenderPath()->ClearDrawableText();
	}

	// DrawUtilTextの解放
	m_pDrawUtilText->Terminate();
	GFL_SAFE_DELETE(m_pDrawUtilText);
#ifdef GF_HIO_SOUND_EDITOR_ENABLE
  //サウンドエディット機能の解放
  GFL_DELETE  m_pSoundEditor;
#endif //GF_HIO_SOUND_EDITOR_ENABLE
  for(int i = 0; i < PAGE_MAX; ++i)
	{
    if (m_pUpdaterList[i] != NULL)
    {
      GFL_DELETE m_pUpdaterList[i];
    }
	}

  // string 削除
	GFL_DELETE m_str;
	GFL_DELETE m_str_search;
	
	// メッセージデータを削除
  GFL_DELETE m_msg_debug;
	GFL_DELETE m_msg_sound;
	GFL_DELETE m_msg_voice;
  GFL_DELETE m_pWordSet;
	// 作成しておいたサウンドヒープを削除
	GFL_DELETE m_sound_heap;

  GFL_DELETE m_pRenderingPipeLine;

  GFL_DELETE m_heap_alloc;
	GFL_DELETE m_device_alloc;
  GFL_DELETE m_pDeviceHeapAllocator;

  return gfl2::proc::RES_FINISH;
}

//---------------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @param   プロセス管理マネージャー
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//---------------------------------------------------------------------------
gfl2::proc::Result SoundTestProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  this->Update();
  return gfl2::proc::RES_CONTINUE;
  ///return gfl2::proc::RES_FINISH;
}

//---------------------------------------------------------------------------
/**
 * @brief	PROCの描画処理
 * @param pManager   プロセス管理マネージャー
 * @param displayNo  描画対象画面
 * 登録したPROCによる、フレームバッファへの描画を実行します。
 * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
 */
//---------------------------------------------------------------------------
void SoundTestProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{

  switch( displayNo )
  {
    // 左画面描画
  case gfl2::gfx::CtrDisplayNo::LEFT:
    {
      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
      m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      m_pRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

      m_pRenderingPipeLine->Execute();
      break;
    } // case gfl2::gfx::CtrDisplayNo::LEFT

    // 右画面描画(立体視時のみ)
  case gfl2::gfx::CtrDisplayNo::RIGHT:
    {
      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
      m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      m_pRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

      m_pRenderingPipeLine->Execute();
      break;
    } // case gfl2::gfx::CtrDisplayNo::RIGHT

    // 下画面描画
  case gfl2::gfx::CtrDisplayNo::DOWN:
    {
      m_pDrawUtilText->ClearDrawText();
      m_pUpdater->DrawBottom(m_pDrawUtilText);
        
      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
      m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      m_pRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

      m_pRenderingPipeLine->Execute();
      break;
    } // case gfl2::gfx::CtrDisplayNo::DOWN

  } // switch( displayNo )
}


//---------------------------------------------------------------------------
/**
 * @brief	更新関数
 * @return bool false 終了 true 継続
 */
//---------------------------------------------------------------------------
bool SoundTestProc::Update(void)
{

#ifdef GF_HIO_SOUND_EDITOR_ENABLE
  m_pSoundEditor->Update();
#endif //GF_HIO_SOUND_EDITOR_ENABLE

  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
#if 0
	// メニューに戻る
	if(pButton->IsTrigger(gfl2::ui::BUTTON_START))
	{
		return false;
	}
#endif
	// 読み直し
	if(pButton->IsTrigger(gfl2::ui::BUTTON_SELECT))
	{
	}

  if ( m_pUpdater->CanPageChange() )
  {
    // カテゴリ切り替え
	  if(pButton->IsTrigger(gfl2::ui::BUTTON_R))
	  {
		  ++m_page;
		  if(m_page == PAGE_MAX){m_page = PAGE_MIN;}

      m_pUpdater = m_pUpdaterList[m_page];
      m_pUpdater->ChangePageFunc();

      NOZOMU_PRINT("CATEGORY: %d\n",m_page);
	  }
	
    if(pButton->IsTrigger(gfl2::ui::BUTTON_L))
	  {
		  --m_page;
		  if(m_page < PAGE_MIN){m_page = PAGE_MAX-1;}

      m_pUpdater = m_pUpdaterList[m_page];
      m_pUpdater->ChangePageFunc();

      NOZOMU_PRINT("CATEGORY: %d\n",m_page);
	  }
  }

  {
    Page page = (Page)m_page;
    m_pUpdater->UpdateInput(page);
    m_page = page;
  }

  m_pDrawUtilText->ClearDrawText();
  m_pUpdater->DrawTop(m_pDrawUtilText);

  return true;
}

//---------------------------------------------------------------------------
/**
 * @brief	プロセス開始時の初期化処理
 */
//---------------------------------------------------------------------------
void SoundTestProc::Enter(void)
{
	m_container.Initialize(m_heap_alloc->GetHeapBase());
	m_container.Load(m_heap_alloc->GetHeapBase(), SOUND_FILE_PATH, VOICE_FILE_PATH);
  /*
	m_playing = -1;
	m_playing_bgm = -1;
	m_playing_bgm_name = -1;
	m_search = -1;
	*/
	m_page = PAGE_BGM;

  m_pWordSet = GFL_NEW(m_heap_alloc->GetHeapBase()) print::WordSet(print::WordSet::DEFAULT_WORD_MAX, 90, m_heap_alloc->GetHeapBase());

	// メッセージデータの読み込み
	m_msg_sound = GFL_NEW(m_heap_alloc->GetHeapBase())
					gfl2::str::MsgData(
						print::GetMessageArcId_Debug(), 
						GARC_debug_message_d_sound_DAT,
						m_heap_alloc->GetHeapBase(),
						gfl2::str::MsgData::LOAD_FULL
					);
	m_msg_voice = GFL_NEW(m_heap_alloc->GetHeapBase())
					gfl2::str::MsgData(
						print::GetMessageArcId_Debug(), 
						GARC_debug_message_d_voice_DAT,
						m_heap_alloc->GetHeapBase(),
						gfl2::str::MsgData::LOAD_FULL
					);
  m_msg_debug = GFL_NEW(m_heap_alloc->GetHeapBase())
					gfl2::str::MsgData(
						print::GetMessageArcId_Debug(), 
						GARC_debug_message_debug_menu_DAT,
						m_heap_alloc->GetHeapBase(),
						gfl2::str::MsgData::LOAD_FULL
					);

	// メッセージ表示用のバッファを確保
	m_str = GFL_NEW(m_heap_alloc->GetHeapBase()) gfl2::str::StrBuf(STRLEN_LABEL_BUFFER, m_heap_alloc->GetHeapBase());
	m_str_search = GFL_NEW(m_heap_alloc->GetHeapBase()) gfl2::str::StrBuf(STRLEN_LABEL_BUFFER, m_heap_alloc->GetHeapBase());
	
  m_sound_heap = NULL;

  UpdaterBase::SetupParam param;
  param.m_DevHeap = m_heap_alloc->GetHeapBase();
  param.m_pLytSys = m_pLytSys;
  param.m_pStr = m_str;
  param.m_pSearchStr = m_str_search;
  param.m_pMsg = m_msg_sound;
  param.m_pDebugMsg = m_msg_debug;
  param.m_pWordSet = m_pWordSet;
  
  param.m_SoundIteratorBegin = m_container.Begin(CATEGORY_BGM);
  param.m_SoundIteratorEnd = m_container.End(CATEGORY_BGM);
  param.m_Page = PAGE_BGM;
  m_pUpdaterList[PAGE_BGM] = GFL_NEW(m_heap_alloc->GetHeapBase()) UpdaterBGM(param);
  param.m_SoundIteratorBegin = m_container.Begin(CATEGORY_ME);
  param.m_SoundIteratorEnd = m_container.End(CATEGORY_ME);
  param.m_Page = PAGE_ME;
  m_pUpdaterList[PAGE_ME] = GFL_NEW(m_heap_alloc->GetHeapBase()) UpdaterME(param);
  param.m_SoundIteratorBegin = m_container.Begin(CATEGORY_SE);
  param.m_SoundIteratorEnd = m_container.End(CATEGORY_SE);
  param.m_Page = PAGE_SE;
  m_pUpdaterList[PAGE_SE] = GFL_NEW(m_heap_alloc->GetHeapBase()) UpdaterSE(param);
  param.m_SoundIteratorBegin = m_container.Begin(CATEGORY_PV);
  param.m_SoundIteratorEnd = m_container.End(CATEGORY_PV);
  param.m_Page = PAGE_PV;
  param.m_pMsg = m_msg_voice;
  m_pUpdaterList[PAGE_PV] = GFL_NEW(m_heap_alloc->GetHeapBase()) UpdaterPV(param, &m_container);
  param.m_SoundIteratorBegin = m_container.Begin(CATEGORY_PV);
  param.m_SoundIteratorEnd = m_container.End(CATEGORY_PV);
  param.m_Page = PAGE_PV_ZUKAN;
  param.m_pMsg = m_msg_voice;
  m_pUpdaterList[PAGE_PV_ZUKAN] = GFL_NEW(m_heap_alloc->GetHeapBase()) UpdaterPVZukan(param, &m_container);
  param.m_SoundIteratorBegin = m_container.Begin(CATEGORY_PIKA);
  param.m_SoundIteratorEnd = m_container.End(CATEGORY_PIKA);
  param.m_Page = PAGE_PIKA;
  param.m_pMsg = m_msg_voice;
  m_pUpdaterList[PAGE_PIKA] = GFL_NEW(m_heap_alloc->GetHeapBase()) UpdaterPika(param);
  
  m_pUpdater = m_pUpdaterList[PAGE_BGM];
  
  m_pUpdater->SetDispInfo();

#ifdef GF_HIO_SOUND_EDITOR_ENABLE
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ); //ちょっと間借り
  //サウンドエディット機能の初期化
  m_pSoundEditor = GFL_NEW(m_heap_alloc->GetHeapBase()) Sound::HioSoundEditor(p_heap);
#endif //GF_HIO_SOUND_EDITOR_ENABLE
}

GFL_NAMESPACE_END(sound)
GFL_NAMESPACE_END(debug)

#endif //PM_DEBUG