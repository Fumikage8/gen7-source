#if PM_DEBUG

//==============================================================================
/**
 * @file    FukushimaTestProc.cpp
 * @author  fukushima_yuya
 * @date    2015.06.19
 * @brief   テスト用プロック
 */
//==============================================================================

// niji
#include "Test/FukushimaTest/include/FukushimaTestProc.h"
#include "Test/FukushimaTest/source/PokeListTest/Field/PokeListTestFieldFrame.h"
#include "Test/FukushimaTest/source/StatusTest/StatusTestFrame.h"
#include "Test/FukushimaTest/source/IconTest/IconTestFrame.h"
#include <Test/FukushimaTest/source/PokeModelTest/PokeModelTestFrame.h>
#include <Test/FukushimaTest/source/DynamicHeaderTest/DynamicHeaderTestFrame.h>

#include <App/NumberInput/include/NumberInputFrame.h>
#include <App/PokeList/include/PokeListIntermediateData.h>
#include <App/StrInput/include/StrInputFrame.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <Debug/StartMenu/include/StartLauncher.h>
#include <GameSys/include/GameData.h>
#include <System/include/HeapDefine.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
static const u32 MANAGER_HEAP_SIZE = 4 * 1024;      //!< 4KB
static const int SIZE_MY_APP = 1 * 1024 * 1024;     //!< 2Mバイト アプリメモリも容量管理のため分けて管理
static const int SIZE_MY_APPDEV = 8 * 1024 * 1024;  //!< 8Mバイト アプリメモリも容量管理のため分けて管理

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
Proc::Proc( void )
  : m_pAppHeap( NULL )
  , m_pAppRenderingManager( NULL )
  , m_pFrameManager( NULL )
  , m_pManagerHeap( NULL )
  , m_pNijiAllocator( NULL )
  , m_CreateFrameIndex( 0 )
  , m_FrameCount( 0 )
  , m_StrInputMode( 0 )
  , m_pPokeListFieldFrame( NULL )
  , m_PokeListAppParam()
  , m_PokeListContext()
  , m_pStatusFrame( NULL )
  , m_StatusAppParam()
  , m_pStrInputFrame( NULL )
  , m_StrInputAppParam()
  , m_pIconTestFrame( NULL )
  , m_pPokeModelTestFrame( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
Proc::~Proc( void )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------------------
gfl2::proc::Result Proc::InitFunc( gfl2::proc::Manager* pManager )
{
  GFL_UNUSED( pManager );

  if( m_pAppRenderingManager == NULL )
  {
    m_pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

    gfl2::heap::HeapBase* pAppHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
    gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

    // マネージャヒープの生成
    m_pManagerHeap = GFL_CREATE_LOCAL_HEAP( pDevHeap, MANAGER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );

    // アプリ用ヒープの生成
    m_pAppHeap = GFL_NEW(m_pManagerHeap) app::util::Heap();
    m_pAppHeap->LocalHeapCreate( pAppHeap, pDevHeap, SIZE_MY_APP,SIZE_MY_APPDEV );

    // NIJIアロケーターの生成
    m_pNijiAllocator = GFL_NEW( pDevHeap ) System::nijiAllocator( pDevHeap );

    // フレームマネージャの生成
    m_pFrameManager = GFL_NEW(m_pManagerHeap) applib::frame::Manager( m_pManagerHeap );
    m_pFrameManager->SetAppFrameListener(this);

    // レンダリングマネージャの生成
    m_pAppRenderingManager = GFL_NEW(m_pAppHeap->GetDeviceHeap()) app::util::AppRenderingManager();
    {
      app::util::AppRenderingManager::Description desc;
      {
        desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
        if( m_CreateFrameIndex != TEST_POKE_MODEL )
        {
          desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
        }
        else {
          desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_POKEMODEL;
        }
      }

      gfl2::fs::AsyncFileManager * afm = m_pGameManager->GetAsyncFileManager();
      m_pAppRenderingManager->StartAsyncCreate( desc, m_pAppHeap, m_pNijiAllocator, afm );
    }
  }

  // Updateを呼ばないと非同期生成が進まない
  if( m_pAppRenderingManager )
  {
    m_pAppRenderingManager->Update();
  }

  if( m_pAppRenderingManager->IsCreated() )
  {
    // フレームの生成
    CreateFrame();

    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCのメイン更新を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------------------
gfl2::proc::Result Proc::UpdateFunc( gfl2::proc::Manager* pManager )
{
  GFL_UNUSED( pManager );

  if( m_pFrameManager->Main() == applib::frame::MAINSTATUS_NULL )
  {
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}
  
//------------------------------------------------------------------------------
/**
 * @brief   PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------------------
void Proc::DrawFunc( gfl2::proc::Manager* pManager,gfl2::gfx::CtrDisplayNo no )
{
  GFL_UNUSED( pManager );

  m_pAppRenderingManager->Update();

  m_pFrameManager->Draw( no );
  m_pAppRenderingManager->Draw( no );
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCのメインを行う関数
 *  1/30フレームの②フレーム目の処理です。
 *  1/60フレーム動作時には、使われません。
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------------------
gfl2::proc::Result Proc::NextFrameMainFunc( gfl2::proc::Manager* pManager )
{
  GFL_UNUSED( pManager );

  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------------------
gfl2::proc::Result Proc::EndFunc( gfl2::proc::Manager* pManager )
{
  // 終了処理が完了するまで待つ
  if( m_pFrameManager->End() == true )
  {
    return gfl2::proc::RES_CONTINUE;
  }
  if( m_FrameCount == 0 )
  {
    m_FrameCount++;
    return gfl2::proc::RES_CONTINUE;
  }
  if( !m_pAppRenderingManager->CanDestroy() )
  {
    return gfl2::proc::RES_CONTINUE;
  }

  GFL_UNUSED( pManager );

  GFL_SAFE_DELETE( m_StrInputAppParam.strBuf );

  GFL_SAFE_DELETE( m_pAppRenderingManager );
  GFL_SAFE_DELETE( m_pFrameManager );
  GFL_SAFE_DELETE( m_pNijiAllocator );
  GFL_SAFE_DELETE( m_pAppHeap );
  gfl2::heap::Manager::DeleteHeap( m_pManagerHeap );

  return gfl2::proc::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief	  EndFuncが完了した直後に呼び出される関数
 */
//------------------------------------------------------------------------------
void Proc::EndFuncFinish( applib::frame::CellSuper* pEndCell )
{
  if( m_pPokeListFieldFrame == pEndCell )
  {
    m_pPokeListFieldFrame = NULL;
  }
  else if( m_pStatusFrame == pEndCell )
  {
    m_pStatusFrame = NULL;
  }
  else if( m_pStrInputFrame == pEndCell )
  {
    m_pStrInputFrame = NULL;

    //if( System::GetLang() != POKEMON_LANG_JAPAN )
    //{
    //  System::SetLang( POKEMON_LANG_JAPAN, m_pAppHeap->GetDeviceHeap() );
    //  System::SetupLangResource( m_pAppHeap->GetDeviceHeap() );
    //}
  }
  else if( m_pIconTestFrame == pEndCell )
  {
    m_pIconTestFrame = NULL;
  }
  else if( m_pPokeModelTestFrame == pEndCell )
  {
    m_pPokeModelTestFrame = NULL;
  }
  else if( m_pNumberInputFrame == pEndCell )
  {
    FUKUSHIMA_PRINT( "InputNum[%ld]\n", m_NumberInputAppParam.inputValue );
    m_pNumberInputFrame = NULL;
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   フレームの生成
 */
//------------------------------------------------------------------------------
void Proc::CreateFrame( void )
{
  gfl2::Fade::FadeManager* fm = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
  gfl2::fs::AsyncFileManager* afm = m_pGameManager->GetAsyncFileManager();
  applib::frame::Manager* frm = this->GetFrameManager();
  GameSys::GameData * gd = m_pGameManager->GetGameData();

  switch( m_CreateFrameIndex )
  {
  case TEST_POKELIST:
    {
      m_PokeListAppParam.mpAppHeap = m_pAppHeap;
      m_PokeListAppParam.mpAppRenderingManager = m_pAppRenderingManager;
      m_PokeListAppParam.mpAsyncFileManager = afm;
      m_PokeListAppParam.mpFadeManager = fm;
      m_PokeListAppParam.mpGameManager = m_pGameManager;

      m_PokeListContext.mode = App::PokeList::MODE_FIELD;
      m_PokeListContext.my_party.SetParameter( gd->GetPlayerParty() );

      m_pPokeListFieldFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, Test::Fukushima::TestPokeListFieldFrame>( frm );
      m_pPokeListFieldFrame->Setup( &m_PokeListAppParam, &m_PokeListContext );
    } break;

  case TEST_STATUS:
    {
      //m_StatusAppParam.mpAppHeap = m_pAppHeap;
      //m_StatusAppParam.mpAppRenderingManager = m_pAppRenderingManager;
      //m_StatusAppParam.mpAsyncFileManager = afm;
      //m_StatusAppParam.mpFadeManager = fm;
      //m_StatusAppParam.mpParty = gd->GetPlayerParty();

      m_StatusAppParam.party_data.SetParameter( gd->GetPlayerParty() );
      m_StatusAppParam.poke_index = 0;

      m_pStatusFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, Test::Fukushima::TestStatusFrame>( frm );
      m_pStatusFrame->Setup( &m_StatusAppParam );
    } break;

  case TEST_STRINPUT:
    {
      {
        m_StrInputAppParam.mpAppRenderingManager  = m_pAppRenderingManager;
        m_StrInputAppParam.mpHeap                 = m_pAppHeap;

        m_StrInputAppParam.mpGameProcManager  = m_pGameManager->GetProcManager();
        m_StrInputAppParam.pSaveMisc          = gd->GetMisc();
        m_StrInputAppParam.cp                 = gd->GetPlayerParty()->GetMemberPointer( 0 );
        m_StrInputAppParam.param              = m_StrInputAppParam.cp->GetMonsNo();
        m_StrInputAppParam.isFieldSubProc     = false;
        m_StrInputAppParam.language           = System::GetLang();

        m_StrInputAppParam.strBuf = GFL_NEW(m_pAppHeap->GetDeviceHeap()) gfl2::str::StrBuf( 64, m_pAppHeap->GetDeviceHeap() );
        m_StrInputAppParam.cp->GetNickName( m_StrInputAppParam.strBuf );

        m_StrInputAppParam.skipInitGraphicMemory = true;

        static const u32 c_StrInputType[] = {
          App::StrInput::TYPE_CHARACTER_NAME,
          App::StrInput::TYPE_POKEMON_NAME,
          App::StrInput::TYPE_BOX_NAME,
          //App::StrInput::TYPE_FREE_WORD_GTS,
          App::StrInput::TYPE_FESCIRCLE_NAME,
          App::StrInput::TYPE_TEAM_NAME,
        };
        m_StrInputAppParam.type = c_StrInputType[m_StrInputMode];
      }

      m_pStrInputFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, App::StrInput::StrInputFrame>( frm );
      m_pStrInputFrame->Setup( &m_StrInputAppParam );
    } break;

  case TEST_ICON:
    {
      //m_pIconTestFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, Test::Fukushima::IconTestFrame>( frm );
      //m_pIconTestFrame->Setup( m_pAppHeap, m_pAppRenderingManager );
      m_pDynamicHeaderTestFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, Test::Fukushima::DynamicHeaderTestFrame>( frm );
      m_pDynamicHeaderTestFrame->Setup( m_pAppHeap, m_pAppRenderingManager );
    } break;

  case TEST_STRINPUT_KOREA:
    {
      //m_StrInputAppParam.mpAppRenderingManager = m_pAppRenderingManager;
      //m_StrInputAppParam.mpHeap = m_pAppHeap;

      //m_StrInputAppParam.mpGameProcManager = m_pGameManager->GetProcManager();
      //m_StrInputAppParam.pSaveMisc = gd->GetMisc();
      //m_StrInputAppParam.cp = gd->GetPlayerParty()->GetMemberPointer( 0 );
      //m_StrInputAppParam.param = m_StrInputAppParam.cp->GetMonsNo();
      //m_StrInputAppParam.isFieldSubProc = false;
      //m_StrInputAppParam.language = POKEMON_LANG_KOREA;
      //System::SetLang( m_StrInputAppParam.language, m_pAppHeap->GetDeviceHeap() );
      //System::SetupLangResource( m_pAppHeap->GetDeviceHeap() );
      //m_StrInputAppParam.skipInitGraphicMemory = true;
      //m_StrInputAppParam.type = App::StrInput::TYPE_BOX_NAME;

      //m_pStrInputFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, App::StrInput::StrInputFrame>( frm );
      //m_pStrInputFrame->Setup( &m_StrInputAppParam );

      m_NumberInputAppParam.callMode  = App::NumberInput::CALL_MODE_ALPHABET;
      m_NumberInputAppParam.callType  = App::NumberInput::CALL_TYPE_BATTLE_RECORDER;

      m_pNumberInputFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, App::NumberInput::NumberInputFrame>( frm );
      m_pNumberInputFrame->Setup( &m_NumberInputAppParam, m_pAppRenderingManager );
      m_pNumberInputFrame->SetGameManager( m_pGameManager );
    } break;

  case TEST_POKE_MODEL:
    {
      m_pPokeModelTestFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, Test::Fukushima::PokeModelTestFrame>( frm );
      m_pPokeModelTestFrame->Setup( m_pAppHeap, m_pAppRenderingManager );
    } break;
  }
}


GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG