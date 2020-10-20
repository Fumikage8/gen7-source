//======================================================================
/**
 * @file    StatusFrame.cpp
 * @date    2015/11/27 16:33:16
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：Frame
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusFrame.h"
#include "StatusUpperDraw.h"
#include "StatusUpperMemoDraw.h"
#include "StatusLowerDraw.h"
#include "StatusLowerBoxMarkDraw.h"
#include "StatusPokemonContainer.h"
#include "StatusDrawerBase.h"

#include <App/Status/include/StatusAppParam.h>
#include <AppLib/include/Tool/AppCommonGrpIconData.h>
#include <AppLib/include/Tool/AppToolTimeLimit.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <Savedata/include/ResortSave.h>
#include <Print/include/WordSet.h>
#include <sound/include/Sound.h>

// gflib2
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>

// poke_3d_lib
#include <model/include/gfl2_BaseCamera.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/field_status.gaix>
#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_pokemon_status.h>

// debug用
#include <GameSys/include/GameData.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  enum {
    MM_SEQ_NONE,
    MM_SEQ_READY,
    MM_SEQ_SETUP,
    MM_SEQ_MAIN,
    MM_SEQ_EXIT,
    MM_SEQ_END,
    MM_SEQ_TIME_OUT,
  };
}


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
StatusFrame::StatusFrame( void )
  : m_pAppHeap(NULL)
  , m_pAppRenderingManager(NULL)
  , m_pWordSet(NULL)
  , m_pWordSetLoader(NULL)
  , m_pCommonIcon(NULL)
  , m_pTrainerIcon(NULL)
  , m_pAppParam(NULL)
  , m_pPokeContainer(NULL)
  , m_pPokeParam(NULL)
  , m_pParentView(NULL)
  , m_pUpperMainDraw(NULL)
  , m_pUpperMemoDraw(NULL)
  , m_pLowerMainDraw(NULL)
  , m_pLowerBoxMarkDraw(NULL)
  , m_pArcReadBuff(NULL)
  , m_LytLoadSeq(0)
  , m_MsgDataCount(0)
  , m_MsgLoadSeq(0)
  , m_WordSetSeq(0)
  , m_CursorCount(0)
  , m_CursorSeq(0)
  , m_CmnIconSeq(0)
  , m_TrainerIconSeq(0)
  , m_EnvSeq(0)
  , m_DispMode(DISP_MODE_STATUS)
  , m_TabMode(TAB_MODE_NUM)
  , m_PokeIndex(0xFFFFFFFF)
  , m_UpdateProhibition(true)
  , m_RibbonNone(true)
  , m_pCameraDrawEnvNode(NULL)
  , m_pCamera(NULL)
  , m_pDrawEnvNode(NULL)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   動作に必要な引数を登録
 *
 * @param   param   セットアップパラメータ
 */
//------------------------------------------------------------------------------
void StatusFrame::Setup( SetupParam& param )
{
  m_pAppParam             = param.appParam;
  m_pAppHeap              = param.heap;
  m_pAppRenderingManager  = param.renderMan;
  m_SetupParam            = param;
}


//------------------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result StatusFrame::InitFunc( void )
{
  // メッセージデータの読み込み
  if( !LoadMsgData() ) return applib::frame::RES_CONTINUE;
  // ワードセットローダーの生成
  if( !CreateWordSetLoader() ) return applib::frame::RES_CONTINUE;
  // メニューカーソルの生成
  if( !CreateMenuCursor() ) return applib::frame::RES_CONTINUE;
  // 共通アイコンの生成
  if( !CreateCommonIcon() ) return applib::frame::RES_CONTINUE;
  // トレーナーアイコンの生成
  if( !CreateTrainerIcon() ) return applib::frame::RES_CONTINUE;
  // カメラの生成
  if( !CreateDrawEnv() ) return applib::frame::RES_CONTINUE;
  // レイアウトデータの読み込み
  if( !LoadLayoutData() ) return applib::frame::RES_CONTINUE;

  // ポケモンコンテナの生成
  CreatePokemonContainer();
  // メイン画面(上)
  CreateUpperMainDraw();
  // メモ画面(上)
  CreateUpperMemoDraw();
  // メイン画面(下)
  CreateLowerMainDraw();
  // ボックスマーク画面(下)
  CreateLowerBoxMarkDraw();

  // 子ビューの追加
  m_pParentView = m_pUpperMainDraw;
  {
    m_pParentView->AddSubView( m_pUpperMemoDraw );
    m_pParentView->AddSubView( m_pLowerMainDraw );
    m_pParentView->AddSubView( m_pLowerBoxMarkDraw );
  }

  SetSubSeq( MM_SEQ_READY );

  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   レイアウトデータの読み込み
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool StatusFrame::LoadLayoutData( void )
{
  enum {
    LYT_SEQ_OPEN,
    LYT_SEQ_LOAD,
    LYT_SEQ_WAIT,
    LYT_SEQ_CLOSE,
    LYT_SEQ_END,
  };

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  u32 arcID = ARCID_FIELD_STATUS;
  u32 dataID = GARC_field_status_field_status_applyt_COMP;

  switch( m_LytLoadSeq )
  {
  case LYT_SEQ_OPEN:
    {
      app::util::FileAccessor::FileOpen( arcID, devHeap );
      m_LytLoadSeq = LYT_SEQ_LOAD;
    } // no break;

  case LYT_SEQ_LOAD:
    {
      if( !app::util::FileAccessor::IsFileOpen( arcID ) ) break;

      app::util::FileAccessor::FileLoad(
        arcID,
        dataID,
        &m_pArcReadBuff,
        devHeap,
        true,
        nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );

      m_LytLoadSeq = LYT_SEQ_WAIT;
    } // no break;

  case LYT_SEQ_WAIT:
    {
      if( !app::util::FileAccessor::IsFileLoad( &m_pArcReadBuff ) ) break;

      app::util::FileAccessor::FileClose( arcID, devHeap );
      m_LytLoadSeq = LYT_SEQ_CLOSE;
    } // no break;

  case LYT_SEQ_CLOSE:
    {
      if( !app::util::FileAccessor::IsFileClose( arcID ) ) break;

      m_LytLoadSeq = LYT_SEQ_END;
    } // no break;

  case LYT_SEQ_END:
    {
      return true;
    } // no break;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージデータの読み込み
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool StatusFrame::LoadMsgData( void )
{
  enum {
    MSG_SEQ_OPEN,
    MSG_SEQ_LOAD,
    MSG_SEQ_WAIT,
    MSG_SEQ_CLOSE,
    MSG_SEQ_INIT,
    MSG_SEQ_END,
  };

  const u32 c_MsgDataList[] = {
    GARC_message_pokemon_status_DAT,
    GARC_message_wazainfo_DAT,
    GARC_message_iteminfo_DAT,
    GARC_message_tokuseiinfo_DAT,
    GARC_message_trainermemo_DAT,
    GARC_message_ribbon_DAT,
  };

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  u32 arcID = print::GetMessageArcId();
  u32 dataID = c_MsgDataList[m_MsgDataCount];

  switch( m_MsgLoadSeq )
  {
  case MSG_SEQ_OPEN:
    {
      app::util::FileAccessor::FileOpen( arcID, devHeap );
      m_MsgLoadSeq = MSG_SEQ_LOAD;
    } // no break;

  case MSG_SEQ_LOAD:
    {
      if( !app::util::FileAccessor::IsFileOpen( arcID ) ) break;

      app::util::FileAccessor::FileLoad(
        arcID,
        dataID,
        &m_pMsgReadBuff[m_MsgDataCount],
        devHeap,
        false,
        4 );

      m_MsgLoadSeq = MSG_SEQ_WAIT;
    } // no break;

  case MSG_SEQ_WAIT:
    {
      if( !app::util::FileAccessor::IsFileLoad( &m_pMsgReadBuff[m_MsgDataCount] ) ) break;

      app::util::FileAccessor::FileClose( arcID, devHeap );
      m_MsgLoadSeq = MSG_SEQ_CLOSE;
    } // no break;

  case MSG_SEQ_CLOSE:
    {
      if( !app::util::FileAccessor::IsFileClose( arcID ) ) break;

      // メッセージデータの生成
      m_pMsgData[m_MsgDataCount] = GFL_NEW(devHeap) gfl2::str::MsgData( m_pMsgReadBuff[m_MsgDataCount], devHeap );
      GFL_ASSERT( m_pMsgData[m_MsgDataCount] );

      m_MsgDataCount++;

      if( m_MsgDataCount == MSG_KIND_NUM )
      {
        m_MsgLoadSeq = MSG_SEQ_INIT;
      }
      else {
        m_MsgLoadSeq = MSG_SEQ_OPEN;
        break;
      }
    } // no break;

  case MSG_SEQ_INIT:
    {
      // ワードセットの生成
      m_pWordSet = GFL_NEW(devHeap) print::WordSet( 16, 64, devHeap );
      GFL_ASSERT( m_pWordSet );

      m_MsgLoadSeq = MSG_SEQ_END;
    } // no break;

  case MSG_SEQ_END:
    {
      return true;
    } // no break;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   ワードセットローダーの生成
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool StatusFrame::CreateWordSetLoader( void )
{
  enum {
    WORD_SEQ_LOAD,
    WORD_SEQ_WAIT,
    WORD_SEQ_END,
  };

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();

  switch( m_WordSetSeq )
  {
  case WORD_SEQ_LOAD:
    {
      u32 id = 0;

      id = PRELOAD_ITEMNAME | PRELOAD_WAZANAME | PRELOAD_TOKUSEI | PRELOAD_SEIKAKU;

      m_pWordSetLoader = GFL_NEW(devHeap) WordSetLoader( devHeap, id );
      m_pWordSetLoader->StartLoad( devHeap );

      m_WordSetSeq++;
    } // no break;

  case WORD_SEQ_WAIT:
    {
      if( !m_pWordSetLoader->WaitLoad() ) break;

      m_WordSetSeq++;
    } // no break;

  case WORD_SEQ_END:
    {
      return true;
    } // no break;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   メニューカーソルの生成
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool StatusFrame::CreateMenuCursor( void )
{
  enum {
    CURSOR_INIT,
    CURSOR_CREATE,
    CURSOR_CHECK,
    CURSOR_END,
  };

  switch( m_CursorSeq )
  {
  case CURSOR_INIT:
    {
      gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();

      m_pMenuCursor[m_CursorCount] = GFL_NEW(devHeap) MenuCursor( m_pAppHeap, m_pAppRenderingManager );
      GFL_ASSERT( m_pMenuCursor );

      m_CursorSeq++;
    } break;

  case CURSOR_CREATE:
    {
      if( m_pMenuCursor[m_CursorCount]->CreateMenuCursor() )
      {
        m_CursorSeq++;
      }
    } break;

  case CURSOR_CHECK:
    {
      m_CursorCount++;

      if( m_CursorCount == CUR_KIND_NUM )
      {
        m_CursorSeq++;
      }
      else {
        m_CursorSeq = CURSOR_INIT;
      }
    } break;

  case CURSOR_END:
    {
      return true;
    } // no break;
  }

  return false;}

//------------------------------------------------------------------------------
/**
 * @brief   共通アイコンの生成
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool StatusFrame::CreateCommonIcon( void )
{
  enum {
    CMN_ICON_SEQ_INIT,
    CMN_ICON_SEQ_READY,
    CMN_ICON_SEQ_END,
  };

  switch( m_CmnIconSeq )
  {
  case CMN_ICON_SEQ_INIT:
    {
      gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();

      m_pCommonIcon = GFL_NEW(devHeap) app::tool::AppCommonGrpIconData();
      {
        GFL_ASSERT( m_pCommonIcon );
        m_pCommonIcon->Initialize( m_pAppHeap );
      }

      m_CmnIconSeq++;
    } // no break;

  case CMN_ICON_SEQ_READY:
    {
      if( !m_pCommonIcon->IsReady() ) break;

      m_CmnIconSeq++;
    } // no break;

  case CMN_ICON_SEQ_END:
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------
/**
   * @brief   トレーナーアイコンの生成（BFCモード専用）
  */
//------------------------------------------------------------------
bool StatusFrame::CreateTrainerIcon( void )
{
  enum {
    TRAINER_ICON_SEQ_INIT,
    TRAINER_ICON_SEQ_READY,
    TRAINER_ICON_SEQ_END,
  };

  /*  BFC専用  */
  if(m_pAppParam->mode != MODE_BFC)
  {
    return true;
  }

  switch( m_TrainerIconSeq )
  {
  case TRAINER_ICON_SEQ_INIT:
    {
      gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();

      m_pTrainerIcon = GFL_NEW(devHeap) app::tool::AppToolTrainerIconRendering();
      {
        GFL_ASSERT( m_pTrainerIcon );
        m_pTrainerIcon->Initialize(
                            m_pAppRenderingManager
                          , m_pAppHeap
                          , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager()
                          , 1
                          , app::util::G2DUtil::SETUP_UPPER
                        );
      }
      ++m_TrainerIconSeq;
    } // no break;

  case TRAINER_ICON_SEQ_READY:
    {
      if( !m_pTrainerIcon->IsReady() ) break;

      m_TrainerIconSeq++;
    } // no break;

  case TRAINER_ICON_SEQ_END:
    {
      return true;
    }
  }

  return false;
  
}


//------------------------------------------------------------------------------
/**
 * @brief   描画環境の生成
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool StatusFrame::CreateDrawEnv( void )
{
  enum {
    ENV_SEQ_CAMERA,
    ENV_SEQ_ENV,
    ENV_SEQ_END,
  };

  switch( m_EnvSeq )
  {
  case ENV_SEQ_CAMERA:
    {
      // カメラを含む環境の生成
      app::util::AppRenderingManager::CreateDefaultCamera( m_pAppHeap, &m_pCameraDrawEnvNode, &m_pCamera );
      m_pCamera->SetAspectRatio( 320.0f / 240.0f );

      // カメラのセット
      m_pAppRenderingManager->SetCamera(
        app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pCamera );

      m_EnvSeq++;
    } // no break;

  case ENV_SEQ_ENV:
    {
      // ライトを含む環境の生成
      m_pDrawEnvNode = app::util::AppRenderingManager::CreateDefaultDrawEnv( m_pAppHeap );

      // デフォルト環境をセット
      m_pAppRenderingManager->AddDrawEnv(
        app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pDrawEnvNode );

      // ライトの設定
      gfl2::renderingengine::scenegraph::instance::LightSetNode* lightSetNode = m_pDrawEnvNode->GetLightSet(0);
      gfl2::renderingengine::scenegraph::instance::LightNode* lightNode = lightSetNode->GetLightNode(0);
      lightNode->SetDirection( gfl2::math::Vector3(-0.129392f, -0.865968f, -0.482898f) );
      lightNode->SetColor( gfl2::math::Vector3(1.0f, 1.0f, 1.0f) );

      m_EnvSeq++;
    } // no break;

  case ENV_SEQ_END:
    {
      return true;
    }
  }

  return false;
}


//------------------------------------------------------------------------------
/**
 * @brief   ポケモンコンテナの生成
 */
//------------------------------------------------------------------------------
void StatusFrame::CreatePokemonContainer( void )
{
  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();

  // ポケモンコンテナ
  m_pPokeContainer = GFL_NEW(devHeap) PokemonContainer( devHeap );
  {
    GFL_ASSERT( m_pPokeContainer );

    // モードに合わせたポケモンのデータを設定
    switch(m_pAppParam->mode)
    {
      case  MODE_PARTY:         /*  fall through  */
      case  MODE_BFC:
      {
        /*  party_dataから  */
        m_pPokeContainer->SetPokeParty( m_pAppParam->party_data.GetPokeParty() );
      }
        break;

      case  MODE_BOX:           /*  fall through  */
      case  MODE_BATTLE_BOX:    /*  fall through  */
      default:
      {
        /*  box_dataから  */
        m_pPokeContainer->SetBoxData( m_pAppParam->box_data, m_pAppParam->box_tray_index );
      }
        break;
    }

    // ポケモンパラメータ
    m_pPokeParam = GFL_NEW(devHeap) pml::pokepara::PokemonParam( devHeap );
    GFL_ASSERT( m_pPokeParam );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メイン画面(上)の生成
 */
//------------------------------------------------------------------------------
void StatusFrame::CreateUpperMainDraw( void )
{
  // メイン画面(上)生成パラメータのセット
  UpperMainDraw::CreateParam createParam;
  {
    createParam.heap            = m_pAppHeap;
    createParam.renderMan       = m_pAppRenderingManager;
    createParam.fadeMan         = m_SetupParam.fadeMan;
    createParam.cmnIcon         = m_pCommonIcon;
    createParam.trainerIconPtr  = m_pTrainerIcon;
  }

  // メイン画面(上)初期化パラメータのセット
  UpperMainDraw::InitParam initParam;
  {
    initParam.appParam       = m_pAppParam;
    initParam.appLytBuff     = m_pArcReadBuff;
    initParam.baseMsgData    = m_pMsgData[MSG_KIND_STATUS];
    initParam.wazaMsgData    = m_pMsgData[MSG_KIND_WAZA_INFO];
    initParam.wordSet        = m_pWordSet;
  }

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();

  // メイン画面(上)
  m_pUpperMainDraw = GFL_NEW(devHeap) UpperMainDraw( createParam );
  {
    GFL_ASSERT(m_pUpperMainDraw);

    m_pUpperMainDraw->SetWordSetLoader( m_pWordSetLoader );
    m_pUpperMainDraw->SetTitleViewUIListener( this );
    m_pUpperMainDraw->Init( initParam );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メモ画面(上)の生成
 */
//------------------------------------------------------------------------------
void StatusFrame::CreateUpperMemoDraw( void )
{
  // メモ画面(上)生成パラメータのセット
  UpperMemoDraw::CreateParam createParam;
  {
    createParam.heap      = m_pAppHeap;
    createParam.renderMan = m_pAppRenderingManager;
  }

  // メモ画面(上)初期化パラメータのセット
  UpperMemoDraw::InitParam initParam;
  {
    initParam.appLytBuff     = m_pArcReadBuff;
    initParam.baseMsgData    = m_pMsgData[MSG_KIND_STATUS];
    initParam.memoMsgData    = m_pMsgData[MSG_KIND_TRAINER_MEMO];
    initParam.ribbonMsgData  = m_pMsgData[MSG_KIND_RIBBON];
    initParam.wordSet        = m_pWordSet;
  }

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();

  // メモ画面(上)
  m_pUpperMemoDraw = GFL_NEW(devHeap) UpperMemoDraw( createParam );
  {
    GFL_ASSERT(m_pUpperMemoDraw);

    m_pUpperMemoDraw->SetWordSetLoader( m_pWordSetLoader );
    m_pUpperMemoDraw->SetTitleViewUIListener( this );
    m_pUpperMemoDraw->Init( initParam );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メイン画面(下)の生成
 */
//------------------------------------------------------------------------------
void StatusFrame::CreateLowerMainDraw( void )
{
  // メイン画面(下)生成パラメータのセット
  LowerMainDraw::CreateParam createParam;
  {
    createParam.heap          = m_pAppHeap;
    createParam.renderMan     = m_pAppRenderingManager;
    createParam.fadeMan       = m_SetupParam.fadeMan;
    createParam.cmnIcon       = m_pCommonIcon;
    createParam.wazaCursor    = m_pMenuCursor[CUR_KIND_WAZA];
    createParam.ribbonCursor  = m_pMenuCursor[CUR_KIND_RIBBON];
  }

  // メイン画面(下)初期化パラメータのセット
  LowerMainDraw::InitParam initParam;
  {
    initParam.appParam       = m_pAppParam;
    initParam.appLytBuff     = m_pArcReadBuff;
    initParam.baseMsgData    = m_pMsgData[MSG_KIND_STATUS];
    initParam.itemMsgData    = m_pMsgData[MSG_KIND_ITEM_INFO];
    initParam.tokuseiMsgData = m_pMsgData[MSG_KIND_TOKUSEI_INFO];
    initParam.wordSet        = m_pWordSet;
    initParam.fileMan        = m_SetupParam.fileMan;
    initParam.devMan         = m_SetupParam.devMan;
  }

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();

  // メイン画面(下)
  m_pLowerMainDraw = GFL_NEW(devHeap) LowerMainDraw( createParam );
  {
    GFL_ASSERT(m_pLowerMainDraw);

    m_pLowerMainDraw->SetWordSetLoader( m_pWordSetLoader );
    m_pLowerMainDraw->SetTitleViewUIListener( this );
    m_pLowerMainDraw->Init( initParam );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボックスマーク変更画面(下)の生成
 */
//------------------------------------------------------------------------------
void StatusFrame::CreateLowerBoxMarkDraw( void )
{
  // ボックスマーク変更画面(下)生成パラメータのセット
  LowerBoxMarkDraw::CreateParam createParam;
  {
    createParam.heap      = m_pAppHeap;
    createParam.renderMan = m_pAppRenderingManager;
  }

  // ボックスマーク変更画面(下)初期化パラメータのセット
  LowerBoxMarkDraw::InitParam initParam;
  {
    initParam.appLytBuff     = m_pArcReadBuff;
    initParam.baseMsgData    = m_pMsgData[MSG_KIND_STATUS];
    initParam.wordSet        = m_pWordSet;
  }

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();

  // ボックスマーク変更画面(下)
  m_pLowerBoxMarkDraw = GFL_NEW(devHeap) LowerBoxMarkDraw( createParam );
  {
    GFL_ASSERT( m_pLowerBoxMarkDraw );

    m_pLowerBoxMarkDraw->SetWordSetLoader( m_pWordSetLoader );
    m_pLowerBoxMarkDraw->SetTitleViewUIListener( this );
    m_pLowerBoxMarkDraw->Init( initParam );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   PROCのメイン更新を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result StatusFrame::UpdateFunc( void )
{
  if( m_pParentView )
  {
    m_pParentView->UpdateTree();
  }

  switch( GetSubSeq() )
  {
  case MM_SEQ_NONE:
    {
      SetSubSeq( MM_SEQ_READY );
    } break;

  case MM_SEQ_READY:
    {
      if( IsReady() )
      {
        // ポケモンパラメータの初期化
        SetPokemonParam( m_pAppParam->poke_index );

        SetSubSeq( MM_SEQ_SETUP );
      }
    } break;

  case MM_SEQ_SETUP:
    {
      // ステータス画面のオープン
      StatusOpen();

      SetSubSeq( MM_SEQ_MAIN );
    } break;

  case MM_SEQ_MAIN:
    {
      if( IsTimeOut() )
      {
        SetSubSeq( MM_SEQ_TIME_OUT );
      }
    } break;

  case MM_SEQ_EXIT:
    {
      if( IsExit() )
      {
        SetSubSeq( MM_SEQ_END );
      }
    } break;

  case MM_SEQ_END:
    {
      return applib::frame::RES_FINISH;
    }
  }

  return applib::frame::RES_CONTINUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   すべての画面の準備が完了したか？
 *
 * @return  "true  = 完了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool StatusFrame::IsReady( void )
{
  if( m_pUpperMainDraw && !m_pUpperMainDraw->IsReady() )
  {
    return false;
  }
  if( m_pLowerMainDraw && !m_pLowerMainDraw->IsReady() )
  {
    return false;
  }
  if( m_pLowerBoxMarkDraw && !m_pLowerBoxMarkDraw->IsReady() )
  {
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   すべての画面の破棄が可能か？
 *
 * @return  "true  = 可"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool StatusFrame::IsExit( void )
{
  if( m_pUpperMainDraw && !m_pUpperMainDraw->IsExit() )
  {
    return false;
  }
  if( m_pLowerMainDraw && !m_pLowerMainDraw->IsExit() )
  {
    return false;
  }
  if( m_pLowerBoxMarkDraw && !m_pLowerBoxMarkDraw->IsExit() )
  {
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   制限時間超過か？
 *
 * @return  "true  = 超過"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool StatusFrame::IsTimeOut( void )
{
  App::Tool::TimeLimit* time_limit = m_pAppParam->time_limit;

  if( time_limit )
  {
    // 制限時間超過
    if( time_limit->Update() )
    {
      // 閉じる
      StatusClose();

      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   開く
 */
//------------------------------------------------------------------------------
void StatusFrame::StatusOpen( void )
{
  FUKUSHIMA_PRINT( "%s\n", __FUNCTION__ );

  if( m_pUpperMainDraw )
  {
    m_pUpperMainDraw->Open();
  }

  if( m_pLowerMainDraw )
  {
    m_pLowerMainDraw->Open();
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------------------
void StatusFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pParentView )
  {
    m_pParentView->DrawTree( no );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------------------
applib::frame::Result StatusFrame::EndFunc( void )
{
  if( m_pLowerBoxMarkDraw )
  {
    if( !m_pLowerBoxMarkDraw->End() ) return applib::frame::RES_CONTINUE;

    m_pLowerBoxMarkDraw->RemoveFromSuperView();

    GFL_SAFE_DELETE( m_pLowerBoxMarkDraw );
  }

  if( m_pLowerMainDraw )
  {
    if( !m_pLowerMainDraw->End() ) return applib::frame::RES_CONTINUE;

    m_pLowerMainDraw->RemoveFromSuperView();

    GFL_SAFE_DELETE( m_pLowerMainDraw );
  }

  if( m_pUpperMemoDraw )
  {
    if( !m_pUpperMemoDraw->End() ) return applib::frame::RES_CONTINUE;

    m_pUpperMemoDraw->RemoveFromSuperView();

    GFL_SAFE_DELETE( m_pUpperMemoDraw );
  }

  if( m_pUpperMainDraw )
  {
    if( !m_pUpperMainDraw->End() ) return applib::frame::RES_CONTINUE;

    GFL_SAFE_DELETE( m_pUpperMainDraw );
  }

  GFL_SAFE_DELETE( m_pPokeParam );
  GFL_SAFE_DELETE( m_pPokeContainer );

  GflHeapSafeFreeMemory( m_pArcReadBuff );

  if( m_pDrawEnvNode )
  {
    m_pAppRenderingManager->RemoveDrawEnv( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pDrawEnvNode );

    GFL_SAFE_DELETE( m_pDrawEnvNode );
  }

  if( m_pCamera )
  {
    m_pAppRenderingManager->UnsetCamera( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY );

    GFL_SAFE_DELETE( m_pCameraDrawEnvNode );
    GFL_SAFE_DELETE( m_pCamera );
  }

  if( m_pCommonIcon )
  {
    m_pCommonIcon->Terminate();
    GFL_SAFE_DELETE( m_pCommonIcon );
  }

  if( m_pTrainerIcon )
  {
    m_pTrainerIcon->Terminate();     /*  内部でフラグを持っているため複数回コールしても良い  */
    if(m_pTrainerIcon->IsDelete())
    {
      GFL_SAFE_DELETE( m_pTrainerIcon );
    }
    else
    {
      return applib::frame::RES_CONTINUE;
    }
  }


  for( u8 i=0; i<CUR_KIND_NUM; ++i )
  {
    if( m_pMenuCursor[i] )
    {
      if( !m_pMenuCursor[i]->DeleteMenuCursor() ) return applib::frame::RES_CONTINUE;

      GFL_SAFE_DELETE( m_pMenuCursor[i] );
    }
  }

  GFL_SAFE_DELETE( m_pWordSetLoader );
  GFL_SAFE_DELETE( m_pWordSet );

  for( u32 i=0; i<MSG_KIND_NUM; ++i )
  {
    GFL_SAFE_DELETE( m_pMsgData[i] );

    GflHeapFreeMemory( m_pMsgReadBuff[i] );
  }

  return applib::frame::RES_FINISH;
}


//------------------------------------------------------------------------------
/**
 * @brief   ポケモンパラメータのセット
 *
 * @param   index   次に表示するインデックス
 */
//------------------------------------------------------------------------------
void StatusFrame::SetPokemonParam( u32 index )
{
  if( m_pPokeContainer )
  {
    // 同じインデックスなら何もしない
    if( m_PokeIndex == index ) return;

    // 前のポケモンの状態を更新
    if( m_PokeIndex != 0xFFFFFFFF )
    {
      // ポケモンパラメータを元のデータに反映
      UpdatePokemonParam();
    }

    // 次に表示するポケモンパラメータを取得
    m_pPokeContainer->GetPokemon( index, m_pPokeParam );
    GFL_ASSERT( m_pPokeParam );

    // 更新禁止か？
    m_UpdateProhibition = IsUpdateProhibition( index );

    { // 各画面へ反映
      if( m_pUpperMainDraw )
      {
        m_pUpperMainDraw->SetPokemonParam( m_pPokeParam, m_pAppParam->battle_fes_trainer_info_ptr[index] );
      }
      if( m_pUpperMemoDraw )
      {
        m_pUpperMemoDraw->SetPokemonParam( m_pPokeParam );
      }
      if( m_pLowerMainDraw )
      {
        pml::PokeParty* party = m_pPokeContainer->GetPokeParty();
        m_pLowerMainDraw->SetPokemonParam( m_pPokeParam, party, index );

        // 更新禁止フラグの更新
        m_pLowerMainDraw->SetUpdateProhibition( m_UpdateProhibition );
        // ポケモンインデックスの変更
        m_pLowerMainDraw->SetPokeIndex( index );
        // 技カーソル位置の初期化
        m_pLowerMainDraw->SetWazaCursorPos( 0, false );
      }
      if( m_pLowerBoxMarkDraw )
      {
        m_pLowerBoxMarkDraw->SetPokemonParam( m_pPokeParam );
      }
    }

    m_PokeIndex = index;
    m_pAppParam->poke_index = index;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンパラメータの反映
 */
//------------------------------------------------------------------------------
void StatusFrame::UpdatePokemonParam( void )
{
  // 更新禁止のため何もしない
  if( IsUpdateProhibition( m_PokeIndex ) ) return;

  if( m_pPokeContainer )
  {
    // ポケモンパラメータを更新
    m_pPokeContainer->UpdatePokemon( m_PokeIndex, *m_pPokeParam );
  }

  // リゾートセーブデータへの反映
  Savedata::ResortSave* resortSave = m_pAppParam->resort_data;
  {
    if( resortSave )
    {      
      if( m_pPokeContainer )    // @fix cov_ctr[10999]
      {
        const pml::PokeParty* party = m_pPokeContainer->GetPokeParty();

        // おんせんリゾート
        if( m_pAppParam->resort_id == Savedata::ResortSave::RESORT_KIND_HOTSPA )
        {
          resortSave->SetPokePartyToHotSpaPokemon( m_pAppParam->resort_group, party, m_pAppHeap->GetDeviceHeap() );
        }
        // うんどうリゾート
        else if( m_pAppParam->resort_id == Savedata::ResortSave::RESORT_KIND_GIM )
        {
          resortSave->SetPokePartyToGimPokemon( m_pAppParam->resort_group, party, m_pAppHeap->GetDeviceHeap() );
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   更新禁止かどうか
 *
 * @param   pokeIndex   ポケモンインデックス
 *
 * @return  "true  = 禁止"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool StatusFrame::IsUpdateProhibition( u32 pokeIndex )
{
  // ステータス画面全体が更新禁止になっているか？
  if( !m_pAppParam->update_prohibition )
  {
    // 個々のポケモンにロックがかかっているかどうか？
    if( !m_pAppParam->poke_lock[pokeIndex] )
    {
      // 更新禁止になっていない
      return false;
    }
  }

  // 更新禁止
  return true;
}





//==============================================================================
/**
 *      ↓　以下がリスナー　↓
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   終了通知
 */
//------------------------------------------------------------------------------
void StatusFrame::EndStatus( void )
{
  SetSubSeq( MM_SEQ_EXIT );
}

//------------------------------------------------------------------------------
/**
 * @brief   キャンセルボタンの押下
 */
//------------------------------------------------------------------------------
void StatusFrame::OnCancelBtn( void )
{
  switch( m_DispMode )
  {
  case DISP_MODE_STATUS:
    {
      // ポケモンパラメータの反映
      UpdatePokemonParam();
      // 閉じる
      StatusClose();
    } break;

  case DISP_MODE_WAZA:
    {
      SetDispMode( DISP_MODE_STATUS );

      // 技説明を閉じる
      CloseWazaExplain();

      // タブをアクティブ
      if( m_pLowerMainDraw )
      {
        m_pLowerMainDraw->ActiveTab();
        // ボタンの設定
        m_pLowerMainDraw->SetButtonEnable(m_pPokeParam);
      }
    } break;

  case DISP_MODE_MEMO:
    {
      SetDispMode( DISP_MODE_STATUS );

      // リボン説明を閉じる
      CloseRibbonExplain();

      // タブをアクティブ
      if( m_pLowerMainDraw )
      {
        m_pLowerMainDraw->ActiveTab();
        // ボタンの設定
        m_pLowerMainDraw->SetButtonEnable(m_pPokeParam);
      }
    } break;

  case DISP_MODE_WAZA_EXCHANGE:
    {
      SetDispMode( DISP_MODE_WAZA );

      if( m_pLowerMainDraw )
      {
        m_pLowerMainDraw->EndWazaExchange();
      }
    } break;

  case DISP_MODE_MSG_WND:
    {
      SetDispMode( DISP_MODE_STATUS );

      if( m_pLowerMainDraw )
      {
        m_pLowerMainDraw->CloseMessageWindow();
      }
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   決定ボタンの押下
 */
//------------------------------------------------------------------------------
void StatusFrame::OnDecideBtn( void )
{
  switch( m_DispMode )
  {
  case DISP_MODE_STATUS:
    {
      if( m_TabMode == TAB_MODE_WAZA )
      {
        // 技説明ウィンドウ等が閉じきっていないので
        if( !IsWazaExplainClosed() ) break;

        if( m_pLowerMainDraw )
        {
          // ボタンの設定
          m_pLowerMainDraw->SetButtonEnable(m_pPokeParam);
          // タブをパッシブ
          m_pLowerMainDraw->PassiveTab();   // @fix NMCat[1519]：PassiveTabをSetButtonEnableよりも後に

          // 技カーソル位置を取得
          u32 cpos = m_pMenuCursor[CUR_KIND_WAZA]->GetCursorPos();
          {
            // 技説明を開く
            OpenWazaExplain( cpos );
          }
        }

        Sound::PlaySE( SEQ_SE_DECIDE1 );
      }
      else if( m_TabMode == TAB_MODE_MEMO )
      {
        if( m_RibbonNone ) break;

        // 先頭のリボンIDを取得
        u32 ribbonID = m_pLowerMainDraw->GetLastRibbonID();
        // リボン説明を開く
        OpenRibbonExplain( ribbonID );

        Sound::PlaySE( SEQ_SE_DECIDE1 );
      }
    } break;

  case DISP_MODE_WAZA:
    {
      if( m_UpdateProhibition ) break;

      if( m_pLowerMainDraw )
      {
        m_pLowerMainDraw->StartWazaExchange();
      }

      SetDispMode( DISP_MODE_WAZA_EXCHANGE );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   閉じる
 */
//------------------------------------------------------------------------------
void StatusFrame::StatusClose( void )
{
  FUKUSHIMA_PRINT( "%s\n", __FUNCTION__ );

  if( m_pUpperMainDraw )
  {
    m_pUpperMainDraw->Close();
  }

  if( m_pLowerMainDraw )
  {
    m_pLowerMainDraw->Close();
  }

  for( u8 i=0; i<CUR_KIND_NUM; ++i )
  {
    m_pMenuCursor[i]->SetVisible( false );
    m_pMenuCursor[i]->UnsetupCursor();
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   画面モードの設定
 *
 * @param   mode    画面モード
 */
//------------------------------------------------------------------------------
void StatusFrame::SetDispMode( u8 mode )
{
  m_DispMode = mode;
}

//------------------------------------------------------------------------------
/**
 * @brief   画面モードの取得
 *
 * @return  画面モード
 */
//------------------------------------------------------------------------------
u8 StatusFrame::GetDispMode( void )
{
  return m_DispMode;
}

//------------------------------------------------------------------------------
/**
 * @brief   タブモードの取得
 *
 * @return  タブモード
 */
//------------------------------------------------------------------------------
u8 StatusFrame::GetTabMode( void )
{
  return m_TabMode;
}

//------------------------------------------------------------------------------
/**
 * @brief   レーダーチャートの表示モード切替
 */
//------------------------------------------------------------------------------
void StatusFrame::ChangeRadarChartMode( void )
{
  if( m_pUpperMainDraw )
  {
    m_pUpperMainDraw->ChangeRadarChartMode();
  }
}




//==============================================================================
// @brief   タブ関連
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   技タブへの変更
 */
//------------------------------------------------------------------------------
bool StatusFrame::ChangeWazaTab( void )
{
  // 多分「DISP_MODE_STATUS」以外では来ないはず。。。
  if( m_DispMode != DISP_MODE_STATUS ) return false;

  if( m_TabMode != TAB_MODE_WAZA )
  {
    m_TabMode = TAB_MODE_WAZA;

    // トレーナーメモを非表示
    if( m_pUpperMemoDraw )
    {
      m_pUpperMemoDraw->Close();
    }
    // レーダーチャートを表示
    if( m_pUpperMainDraw )
    {
      m_pUpperMainDraw->VisibleRadarChart();
    }
    // リボンリストを非表示
    if( m_pLowerMainDraw )
    {
      // 技タブを選択
      m_pLowerMainDraw->SelectWazaTab();
      // リボンを非表示
      m_pLowerMainDraw->CloseRibbonList();
      // ボタンの設定
      m_pLowerMainDraw->SetButtonEnable(m_pPokeParam);
    }

    return true;
  }

   return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   メモタブへの変更
 */
//------------------------------------------------------------------------------
bool StatusFrame::ChangeMemoTab( void )
{
  // 多分「DISP_MODE_STATUS」以外では来ないはず。。。
  if( m_DispMode != DISP_MODE_STATUS ) return false;

  if( m_TabMode != TAB_MODE_MEMO )
  {
    m_TabMode = TAB_MODE_MEMO;

    // レーダーチャートを非表示
    if( m_pUpperMainDraw )
    {
      m_pUpperMainDraw->InvisibleRadarChart();
    }
    // トレーナーメモを表示
    if( m_pUpperMemoDraw )
    {
      m_pUpperMemoDraw->Open();
    }
    // リボンリストを表示
    if( m_pLowerMainDraw )
    {
      // メモタブを選択
      m_pLowerMainDraw->SelectMemoTab();
      // リボンを表示
      m_pLowerMainDraw->OpenRibbonList();
      // ボタンの設定
      m_pLowerMainDraw->SetButtonEnable(m_pPokeParam);
    }

    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   タマゴ用メモタブへの変更
 */
//------------------------------------------------------------------------------
bool StatusFrame::ChangeEggMemoTab( void )
{
  // 多分「DISP_MODE_STATUS」以外では来ないはず。。。
  if( m_DispMode != DISP_MODE_STATUS ) return false;

  if( m_TabMode != TAB_MODE_EGG )
  {
    // レーダーチャートを非表示
    if( m_pUpperMainDraw )
    {
      m_pUpperMainDraw->InvisibleRadarChart();
    }
    // トレーナーメモを表示
    if( m_pUpperMemoDraw )
    {
      m_pUpperMemoDraw->Open();
    }
    // @fix NMCat[1578]
    //// タマゴ用表示
    //if( m_pLowerMainDraw )
    //{
    //  // リボンを非表示
    //  m_pLowerMainDraw->CloseRibbonList();
    //}

    m_TabMode = TAB_MODE_EGG;

    return true;
  }

  return false;
}



//==============================================================================
// @brief   ポケモンパラメータ関連
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   次のポケモンへ
 *
 * @note    十字キー切り替え
 */
//------------------------------------------------------------------------------
void StatusFrame::NextPokemon( void )
{
  FUKUSHIMA_PRINT( "%s\n", __FUNCTION__ );

  if( m_pPokeContainer )
  {
    // 次のインデックスを取得
    u32 next = m_pPokeContainer->GetNextPokeIndex( m_PokeIndex );

    // ポケモンパラメータのセット
    SetPokemonParam( next );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   前のポケモンへ
 *
 * @note    十字キー切り替え
 */
//------------------------------------------------------------------------------
void StatusFrame::PrevPokemon( void )
{
  FUKUSHIMA_PRINT( "%s\n", __FUNCTION__ );

  if( m_pPokeContainer )
  {
    // 前のインデックスを取得
    u32 prev = m_pPokeContainer->GetPrevPokeIndex( m_PokeIndex );

    // ポケモンパラメータのセット
    SetPokemonParam( prev );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   指定したポケモンに変更
 *
 * @note    手持ち表示でボールをタッチした時のみ使用
 */
//------------------------------------------------------------------------------
void StatusFrame::SetPokemon( u32 index )
{
  FUKUSHIMA_PRINT( "%s\n", __FUNCTION__ );

  if( m_PokeIndex != index )
  {
    // ポケモンパラメータのセット
    SetPokemonParam( index );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンパラメータの更新
 */
//------------------------------------------------------------------------------
void StatusFrame::UpdatePokeParam( void )
{
  // ポケモンパラメータを更新
  UpdatePokemonParam();
}



//==============================================================================
// @brief   ボックスマーク変更画面関連
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   ボックスマーク変更画面のオープン
 */
//------------------------------------------------------------------------------
void StatusFrame::OpenBoxMark( bool updateProhibition )
{
  if( m_pLowerMainDraw )
  {
    m_pLowerMainDraw->SetInputEnabled( false );
  }

  if( m_pLowerBoxMarkDraw )
  {
    m_pLowerBoxMarkDraw->Open( updateProhibition );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボックスマーク変更画面のクローズ
 */
//------------------------------------------------------------------------------
void StatusFrame::CloseBoxMark( void )
{
  if( m_pLowerBoxMarkDraw )
  {
    m_pLowerBoxMarkDraw->Close();
  }

  if( m_pLowerMainDraw )
  {
    // ボックスマークの更新
    m_pLowerMainDraw->UpdateBoxMark( m_pPokeParam );

    m_pLowerMainDraw->SetInputEnabled( true );
  }
}



//==============================================================================
// @brief   技説明画面関連
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   技説明画面のオープン
 *
 * @param   index   インデックス
 *
 * @return  "true  = 開いた"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool StatusFrame::OpenWazaExplain( u32 index )
{
  WazaNo wazaNo;
  {
    bool fastmode = m_pPokeParam->StartFastMode();
    {
      wazaNo = m_pPokeParam->GetWazaNo(index);
    }
    m_pPokeParam->EndFastMode( fastmode );
  }

  if( m_pUpperMemoDraw )
  {
    // 上画面の技の説明ウィンドウを開く
    if( m_pUpperMainDraw->OpenWazaExplain( wazaNo ) )
    {
      if( GetDispMode() == DISP_MODE_STATUS )
      {
        u32 msgID = (!m_UpdateProhibition) ? PARAM_WAZA_CHANGE_01 : PARAM_TEXT_01;

        // メッセージウィンドウを開く
        m_pLowerMainDraw->OpenMessageWindow( msgID );

        // 技カーソルの表示
        m_pLowerMainDraw->SetWazaCursorPos( index );

        // 技選択モードへ
        SetDispMode( DISP_MODE_WAZA );
      }
      else if( GetDispMode() == DISP_MODE_WAZA_EXCHANGE )
      {
        // メッセージウィンドウを開く
        m_pLowerMainDraw->OpenMessageWindow( PARAM_WAZA_CHANGE_02 );
      }

      return true;
    }
  }

  return false;

}

//------------------------------------------------------------------------------
/**
 * @brief   技説明画面のクローズ
 */
//------------------------------------------------------------------------------
void StatusFrame::CloseWazaExplain( void )
{
  if( m_pUpperMainDraw )
  {
    m_pUpperMainDraw->CloseWazaExplain();
  }

  // カーソルを非表示
  if( m_pMenuCursor[CUR_KIND_WAZA] )
  {
    m_pMenuCursor[CUR_KIND_WAZA]->SetVisible( false );
  }

  // メッセージウィンドウを閉じる
  if( m_pLowerMainDraw )
  {
    m_pLowerMainDraw->CloseMessageWindow();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   表示する技を変更
 *
 * @param   wazaNo    技No
 */
//------------------------------------------------------------------------------
void StatusFrame::SetWazaExplain( u32 wazaNo )
{
  if( m_pUpperMainDraw )
  {
    m_pUpperMainDraw->SetWazaExplain( wazaNo );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   技説明が閉じきったかどうか
 *
 * @param   wazaNo    技No
 */
//------------------------------------------------------------------------------
bool StatusFrame::IsWazaExplainClosed( void )
{
  if( m_pUpperMainDraw )
  {
    if( !m_pUpperMainDraw->IsClosedWazaExplain() ) return false;
  }

  if( m_pLowerMainDraw )
  {
    if( !m_pLowerMainDraw->IsClosedMessageWindow() ) return false;
  }

  return true;
}



//==============================================================================
// @brief   リボン説明画面関連
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   リボン説明画面のオープン
 *
 * @param   ribbonID    リボンID
 *
 * @return  "true  = 開いた"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool StatusFrame::OpenRibbonExplain( u32 ribbonID )
{
  bool result = false;

  if( m_pUpperMemoDraw )
  {
    // 上画面のリボンの説明ウィンドウを開く
    result = m_pUpperMemoDraw->OpenRibbonExplain( ribbonID );
  }

  // 開く時のみの処理
  if( result )
  {
    // 下画面のリボンのアナログキー入力を有効にする
    if( m_pLowerMainDraw )
    {
      m_pLowerMainDraw->InputRibbonKeyEnabled( true );
    }

    // @fix NMCat[1806]
    {
      // ボタンの設定
      m_pLowerMainDraw->SetButtonEnable(m_pPokeParam);
      // タブをパッシブ
      m_pLowerMainDraw->PassiveTab();   // @fix NMCat[1519]：PassiveTabをSetButtonEnableよりも後に
    }

    SetDispMode( DISP_MODE_MEMO );
  }

  // リボンカーソルを表示
  if( m_pMenuCursor[CUR_KIND_RIBBON] )
  {
    m_pMenuCursor[CUR_KIND_RIBBON]->SetVisible( true );
  }

  return result;
}

//------------------------------------------------------------------------------
/**
 * @brief   リボン説明画面のクローズ
 */
//------------------------------------------------------------------------------
void StatusFrame::CloseRibbonExplain( void )
{
  // 上画面のリボンの説明ウィンドウを閉じる
  if( m_pUpperMemoDraw )
  {
    m_pUpperMemoDraw->CloseRibbonExplain();
  }

  // 下画面のリボンのアナログキー入力を無効にする
  if( m_pLowerMainDraw )
  {
    m_pLowerMainDraw->InputRibbonKeyEnabled( false );
  }

  // カーソルを非表示
  if( m_pMenuCursor[CUR_KIND_RIBBON] )
  {
    m_pMenuCursor[CUR_KIND_RIBBON]->SetVisible( false );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   表示するリボンを変更
 *
 * @param   ribbonID    リボンID
 */
//------------------------------------------------------------------------------
void StatusFrame::SetRibbonExplain( u32 ribbonID )
{
  if( m_pUpperMemoDraw )
  {
    // 説明文をセット
    m_pUpperMemoDraw->SetRibbonExplain( ribbonID );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リボンを未所持かどうか
 *
 * @param   none    true = 未所持、false = それ以外
 */
//------------------------------------------------------------------------------
void StatusFrame::SetRibbonNone( bool none )
{
  m_RibbonNone = none;
}

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)
