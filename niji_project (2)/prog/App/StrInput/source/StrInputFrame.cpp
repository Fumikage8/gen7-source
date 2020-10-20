//==============================================================================
/**
 * @file    StrInputFrame.cpp
 * @brief   Frame関連
 * @author  fukushima_yuya
 * @date    2015.04.30
 * @note    sango_project app strinput.hから移植
 */
//==============================================================================

// niji
#include "App/StrInput/include/StrInputFrame.h"
#include "App/StrInput/source/StrInputDraw.h"

#include <GameSys/include/GameData.h>
#include <System/include/HeapDefine.h>

// gflib2
#include <ngc/include/gfl2_NGWordChecker.h>


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace  app::strinput
 * @brief      文字入力画面
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( StrInput )


//==============================================================================
//  定数定義
//==============================================================================
#define  STRINPUT_SYS_HEAP_SIZE    ( 0x10300 )    //!< システムヒープサイズ

//------------------------------------------------------------------------------
/**
 * @brief    コンストラクタ
 */
 //------------------------------------------------------------------------------
StrInputFrame::StrInputFrame( void )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
//------------------------------------------------------------------------------
StrInputFrame::~StrInputFrame( void )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   外部設定パラメータのセット
 *
 * @param   prm    外部設定パラメータ
 */
//------------------------------------------------------------------------------
void StrInputFrame::Setup( APP_PARAM* prm )
{
  CreateHeap( prm );
  m_pNGWordChecker = GFL_NEW( m_pHeap ) gfl2::ngc::NGWordChecker();
  m_pNGWordChecker->Initialize( m_pHeap );
  m_pMain  = GFL_NEW( m_pHeap ) MainSystem( prm );
  m_pParam = prm;
}


//------------------------------------------------------------------------------
/**
 * @brief   Proc初期化処理
 *
 * @return  applib::frame::Result
 */
//------------------------------------------------------------------------------
applib::frame::Result StrInputFrame::InitFunc( void )
{
  //StartTime();

  GameSys::GameProcManager * gpm = m_pParam->mpGameProcManager;
  m_pMain->SetGameManager( gpm );

  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   Proc終了処理
 *
 * @return  applib::frame::Result
 */
//------------------------------------------------------------------------------
applib::frame::Result StrInputFrame::EndFunc( void )
{
#if PM_DEBUG
  if( TYPE_CHARACTER_NAME_DEBUG == m_pParam->type)
  {
    if( m_pParam->changeFlag )
    {
      GameSys::GameManager* game_manager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
      GameSys::GameData*    game_data    = game_manager->GetGameData();
      Savedata::MyStatus*   my_status    = game_data->GetPlayerStatus();
      Savedata::FieldMenu*  field_menu   = game_data->GetFieldMenu();
      // 名前とニックネームを設定する
      my_status->SetNameString(m_pParam->strBuf);
      field_menu->SetNickNameString(m_pParam->strBuf);
      GFL_DELETE m_pParam;
    } 
  }
#endif

  //EndTime();

  GFL_DELETE  m_pMain;

  m_pNGWordChecker->Finalize();
  GFL_DELETE  m_pNGWordChecker;

  DeleteHeap();

  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   Procアップデート処理
 *
 * @return  gfl2::proc::Result
 */
//------------------------------------------------------------------------------
applib::frame::Result StrInputFrame::UpdateFunc( void )
{
  if( m_pMain->Update() == true )
  {
    return applib::frame::RES_CONTINUE;
  }
  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   Proc描画処理
 *
 * @param   no    ディスプレイNo
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void StrInputFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  m_pMain->Draw( no );
}

//------------------------------------------------------------------------------
/**
 * @brief   システムヒープ作成
 *
 * @param   prm   外部設定パラメータ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void StrInputFrame::CreateHeap(APP_PARAM * prm)
{
  gfl2::heap::HEAPID heapId = (prm->isFieldSubProc) ? HEAPID_EVENT_DEVICE : HEAPID_APP_DEVICE;

  m_pHeap = GFL_CREATE_HEAP(
            gfl2::heap::Manager::GetHeapByHeapId( heapId ),
            HEAPID_STRINPUT_SYS, STRINPUT_SYS_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
}

//------------------------------------------------------------------------------
/**
 * @brief   システムヒープ削除
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void StrInputFrame::DeleteHeap( void )
{
  gfl2::heap::Manager::DeleteHeap( m_pHeap );
}

//------------------------------------------------------------------------------
/**
 * @brief   FrameModeの取得
 *
 * @param   none
 *
 * @return  FrameMode
 *
 * @note  フレームモードを切り替えるときは、戻り値を変えてください。
 */
//------------------------------------------------------------------------------
GameSys::FrameMode StrInputFrame::GetFrameMode( void ) const
{
  return (m_pParam->isFieldSubProc) ? GameSys::FRAMEMODE_30 : GameSys::FRAMEMODE_60;
}


//------------------------------------------------------------------------------
/**
 * @brief   開始時間設定
 *
 * @param   none
 *
 * @return  none
 *
 *  ポケモン名入力用
 */
//------------------------------------------------------------------------------
void StrInputFrame::StartTime( void )
{
  // いらない
  if( m_pParam->type != TYPE_POKEMON_NAME )
  {
    return;
  }

#ifdef GF_PLATFORM_CTR
  m_startTick = nn::os::Tick::GetSystemCurrent();
#endif
}

//------------------------------------------------------------------------------
/**
 * @brief   終了時間設定
 *
 * @param   none
 *
 * @return  none
 *
 *  ポケモン名入力用
 */
//------------------------------------------------------------------------------
void StrInputFrame::EndTime( void )
{
  // いらない
  if( m_pParam->type != TYPE_POKEMON_NAME )
  {
    return;
  }

  // キャンセル
  if( m_pParam->changeFlag == false || m_pParam->isInputCancel == true )
  {
    return;
  }

  // 入力文字列
  //const gfl2::str::STRCODE * str;
  //if( m_pParam->strCode != NULL )
  //{
  //  str = m_pParam->strCode;
  //}
  //else if( m_pParam->strBuf != NULL )
  //{
  //  str = m_pParam->strBuf->GetPtr();
  //}
  //else {
  //  str = NULL;
  //  GFL_ASSERT( 0 );
  //  return;
  //}

  // ポケモン
  if( m_pParam->cp == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

#ifdef GF_PLATFORM_CTR
  // 時間（秒）
  nn::os::Tick now = nn::os::Tick::GetSystemCurrent();
  nn::fnd::TimeSpan ss = m_startTick.ToTimeSpan();
  nn::fnd::TimeSpan es = now.ToTimeSpan();
  u32 sec = static_cast<u32>( (es.GetNanoSeconds()-ss.GetNanoSeconds()) / 1000000000 );

  // TVデータの型がu16なので補正
  if( sec > 0xffff )
  {
    sec = 0xffff;
  }
#endif
  // 使用しない
  //MonsNo mons  = m_pParam->cp->GetMonsNo();
  //pml::Sex sex = m_pParam->cp->GetSex();
  //netapp::util::SangoTVNaviProgramUtility::CreateTVProgramData_01( mons, str, sec, sex );
}


GFL_NAMESPACE_END( StrInput )
GFL_NAMESPACE_END( App )
