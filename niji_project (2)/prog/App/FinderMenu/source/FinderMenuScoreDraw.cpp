//======================================================================
/**
 * @file    FinderMenuScoreDraw.cpp
 * @date    2016/01/14 17:09:12
 * @author  fukushima_yuya
 * @brief   ポケファインダーメニュー：得点表示画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "FinderMenuScoreDraw.h"
#include "FinderMenuDrawListener.h"
#include "FinderMenuDrawerBase.h"

#include <Field/FieldRo/include/TrialModel/FieldFinderModel_define.h>
#include <Field/FieldRo/include/TrialModel/FieldFinderUtil.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameManager.h>
#include <Savedata/include/PokeFinderSave.h>
#include <Savedata/include/MiscSave.h>
#include <Sound/include/Sound.h>
#include <system/include/GflUse.h>

// arc
#include <niji_conv_header/app/finder_menu/finder_menu.h>
#include <niji_conv_header/app/finder_menu/finder_menu_pane.h>
#include <niji_conv_header/app/finder_menu/finder_menu_anim_list.h>
#include <niji_conv_header/app/common/graphic_font/finder_grpfont/res2d/FinderGrpFont.h>
#include <niji_conv_header/message/msg_pokefinder.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  enum Sequence
  {
    SEQ_INIT,
    SEQ_READY,
    SEQ_IDLE,
    SEQ_TUTORIAL,
    SEQ_MAIN,
    SEQ_SAVING,
    SEQ_OUT,
    SEQ_END
  };

  enum {
    MS_BEGIN,
    MS_DIRECT_SCORE,
    MS_MSG_BEGIN,
    MS_MSG_SCORE_EVAL,
    MS_MSG_HIGH_SCORE,
    MS_DIRECT_HIGHSCORE_OUT,
    MS_DIRECT_HIGHSCORE_IN,
    MS_DIRECT_TOTALSCORE,

    MS_DIRECT_VERSION_UP,
    MS_MSG_VERSION_UP,
    MS_MSG_DOWNLOAD,
    MS_MSG_VERSION_DESC,

    MS_MSG_FINAL,

    MS_MSG_NEXT,
    MS_MSG_SUPPORT,

    MS_END,
  };

  // アプリのバージョン
  enum {
    APP_VER_1,
    APP_VER_2,
    APP_VER_3,
    APP_VER_4,
    APP_VER_5,
    APP_VER_NUM,

    APP_VER_FINAL = APP_VER_5,
  };

  static const u8 ORDER_SCORE_DRAW = 128;
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
FinderMenuScoreDraw::FinderMenuScoreDraw( CreateParam& param )
  : app::ui::UIView( param.heap, param.heap->GetDeviceAllocator(), param.heap->GetDeviceAllocator() )
  , m_pHeap( param.heap )
  , m_uiListener( NULL )
  , m_pAppRenderingManager( param.renderMan )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pAppParam(NULL)
  , m_pFinderSave(NULL)
  , m_pGameMan(NULL)
  , m_pDrawerBase( NULL )
  , m_pMsgWindow( NULL )
  , m_Seq( SEQ_INIT )
  , m_MainSeq(MS_BEGIN)
  , m_PhotoScore(0)
  , m_PhotoHighScore(0)
  , m_VersionUpAnim(0)
  , m_StartME(0)
  , m_SeWaitFrame(0)
  , m_pTextNowScore(NULL)
  , m_pTextHighScore(NULL)
  , m_pTextTotalScore(NULL)
{
  m_pGameMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  {
    GameSys::GameData* gd = m_pGameMan->GetGameData();
    {
      m_pFinderSave = gd->GetPokeFinderSave();
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   このビュー専用のUIリスナー設定
 * @param   pListener   UIリスナー
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::SetTitleViewUIListener( FinderMenuDrawListener* pListener )
{
  m_uiListener = pListener;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   appParam      外部設定パラメータ
 * @param   pAppLytBuff   レイアウトバイナリ
 * @param   grpFontBuff   グラフィックフォント
 * @param   msgData       メッセージデータ
 * @param   wordSet       ワードセット
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::Init( APP_PARAM* appParam, void* pAppLytBuff, void* grpFontBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  {
    m_pMsgData  = msgData;
    m_pWordSet  = wordSet;
    m_pAppParam = appParam;
  }

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( m_pHeap, pAppLytBuff, grpFontBuff );
  InitializeText();

  SetInputListener( this );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pHeap         ヒープ
 * @param   pAppLytBuff   レイアウトバイナリデータ
 * @param   grpFontBuff   グラフィックフォント
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff, void* grpFontBuff )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, LYTRES_FINDERGRPFONT_END, app::util::G2DUtil::ATTACH_ROOT },
    { grpFontBuff, 0, app::util::G2DUtil::ATTACH_RESOURCE },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDER_MENU_FINDER_SCORE_LOW_000_BFLYT,
      LA_FINDER_MENU_FINDER_SCORE_LOW_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  // 2Dの生成
  Create2D(
    pHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    m_pMsgData,
    m_pWordSet);

  this->SetVisible( false );
  this->SetInputEnabled( false );

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  // 描画共通処理
  m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, m_pWordSet, m_pMsgData );
  {
    GFL_ASSERT( m_pDrawerBase );

    // ペインの取得
    m_pTextNowScore     = m_pDrawerBase->GetTextBox( PANENAME_FINDER_SCORE_LOW_000_PANE_TEXTSCORE_00 );
    m_pTextHighScore    = m_pDrawerBase->GetTextBox( PANENAME_FINDER_SCORE_LOW_000_PANE_TEXTSCORE_01 );
    m_pTextTotalScore   = m_pDrawerBase->GetTextBox( PANENAME_FINDER_SCORE_LOW_000_PANE_TEXTSCORE_02 );
    m_pTotalScorePlate  = m_pDrawerBase->GetPane( PANENAME_FINDER_SCORE_LOW_000_PANE_SCORE_02 );
    m_pMessageTail      = m_pDrawerBase->GetPane( PANENAME_FINDER_SCORE_LOW_000_PANE_MESSAGETALE );

    gfl2::lyt::LytParts* msgWndParts = m_pDrawerBase->GetParts( PANENAME_FINDER_SCORE_LOW_000_PANE_MESSAGE_L2 );
    {
      m_pMessageWindow = m_pDrawerBase->GetPane( PANENAME_COMMON_MSG_LOW_000_PANE_ALL, msgWndParts );
    }

    // 表示切替
    m_pDrawerBase->SetVisiblePane( m_pTotalScorePlate, false );

    // アニメの再生
    m_pDrawerBase->StartAnim( LA_FINDER_MENU_FINDER_SCORE_LOW_000_ROTOMANIM );
    m_pDrawerBase->StartAnim( LA_FINDER_MENU_FINDER_SCORE_LOW_000_VERSIONUP_CHANGE, false );
  }

  // メッセージウィンドウ
  m_pMsgWindow = GFL_NEW(devHeap) FinderMenuScoreMessageWindow( m_pDrawerBase, m_pAppRenderingManager, this );
  {
    GFL_ASSERT( m_pMsgWindow );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::InitializeText( void )
{
  // 選択した写真の得点
  gfl2::lyt::LytTextBox* nowScore = m_pDrawerBase->GetTextBox( PANENAME_FINDER_SCORE_LOW_000_PANE_TEXTTITLE_00 );
  {
    this->SetTextboxPaneMessage( nowScore, msg_pokefinder_40_04 );
  }
  // 写真の最高得点
  gfl2::lyt::LytTextBox* highScore = m_pDrawerBase->GetTextBox( PANENAME_FINDER_SCORE_LOW_000_PANE_TEXTTITLE_01 );
  {
    this->SetTextboxPaneMessage( highScore, msg_pokefinder_40_05 );
  }
  // 合計得点
  gfl2::lyt::LytTextBox* totalScore = m_pDrawerBase->GetTextBox( PANENAME_FINDER_SCORE_LOW_000_PANE_TEXTTITLE_02 );
  {
    this->SetTextboxPaneMessage( totalScore, msg_pokefinder_40_01 );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool FinderMenuScoreDraw::End( void )
{
  if( m_pMsgWindow )
  {
    if( !m_pMsgWindow->EndFunc() ) return false;

    GFL_SAFE_DELETE( m_pMsgWindow );
  }

  GFL_SAFE_DELETE( m_pDrawerBase );

  Delete2D();
  DeleteGraphicsSystem();

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::Update( void )
{
  app::ui::UIView::Update();

  switch( m_Seq )
  {
  case SEQ_READY:
    {
      if( m_pMsgWindow && !m_pMsgWindow->InitFunc() )
      {
        break;
      }

      m_Seq = SEQ_IDLE;
    } break;

  case SEQ_TUTORIAL:
    {
      if( m_pMsgWindow ) m_pMsgWindow->UpdateFunc();
    } break;

  case SEQ_MAIN:
    {
      if( m_pMsgWindow ) m_pMsgWindow->UpdateFunc();

      Sequence_Main();
    } break;

  case SEQ_OUT:
    {
      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンス
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::Sequence_Main( void )
{
  UpdateMsgTail();

  switch( m_MainSeq )
  {
  case MS_BEGIN:
    {
      m_MainSeq = MS_DIRECT_SCORE;
    } break;

  case MS_DIRECT_SCORE:
    {
      if( m_pMsgWindow ) m_pMsgWindow->Open( WND_KIND_SCORE_BEGIN );      // @fix cov_ctr[11054]
      m_MainSeq = MS_MSG_BEGIN;
    } break;

  case MS_DIRECT_HIGHSCORE_OUT:
    {
      if( m_pDrawerBase->IsAnimEnd( LA_FINDER_MENU_FINDER_SCORE_LOW_000_SCOREREWITE_00 ) )  //!< 最高点のアウトの終了
      {
        // 最高得点のインの演出の開始
        StartHighScoreInDirect();
      }
    } break;

  case MS_DIRECT_HIGHSCORE_IN:
    {
      // 一之瀬さん要望対応：SEの寿命が微妙なので指定フレーム待機に切り替え@miyachi_soichi
      ++ m_SeWaitFrame;
      if( m_SeWaitFrame < 45 ){ break; }

      if( m_pDrawerBase->IsAnimEnd( LA_FINDER_MENU_FINDER_SCORE_LOW_000_SCOREREWITE_01 ) )  //!< 最高点のインの終了
      {
        // 総合得点の演出開始
        StartTotalScoreDirect();
      }
    } break;

  case MS_DIRECT_TOTALSCORE:
    {
      if( Sound::IsSEFinished( SEQ_SE_PF_TOKUTEN ) )  //!< 総合得点の表示の終了
      {
        // バージョンアップのチェック
        if( CheckVersionUp() )
        {
          // バージョンアップ演出の開始
          StartVersionUpDirect();
        }
        else if( GetVersion() == APP_VER_FINAL )
        {
          if( m_pMsgWindow ) m_pMsgWindow->Open( GetWindowKind_FinalComment() );      // @fix cov_ctr[11054]
          m_MainSeq = MS_MSG_FINAL;
        }
        else {
          if( m_pMsgWindow ) m_pMsgWindow->Open( GetWindowKind_Next() );      // @fix cov_ctr[11054]
          m_MainSeq = MS_MSG_NEXT;
        }
      }
    } break;

  case MS_DIRECT_VERSION_UP:
    {
      if( !Sound::IsMEFinished(m_StartME) )               break;
      if( !m_pDrawerBase->IsAnimEnd( m_VersionUpAnim ) )  break;

      // MEの再生を終了し元のBGMを再生する
      Sound::StartBaseBGMFromME();

      if( m_pMsgWindow ) m_pMsgWindow->Open( GetWindowKind_VersionUp() );      // @fix cov_ctr[11054]
      m_MainSeq = MS_MSG_VERSION_UP;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   噴出しの尻尾の更新
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::UpdateMsgTail( void )
{
  u8 alpha = m_pDrawerBase->GetAlpha( m_pMessageWindow );
  {
    m_pDrawerBase->SetAlpha( m_pMessageTail, alpha );
  }
}




//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* g2d = this->GetG2DUtil();
  {
    g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0, ORDER_SCORE_DRAW );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   オープン
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::Open( void )
{
  {
    // 写真の得点
    this->SetTextboxPaneNumber(
      m_pTextNowScore, msg_pokefinder_40_03, m_PhotoScore, 5, 0, print::NUM_DISP_SPACE );

    // 写真の最高得点
    Setup_HighScore( m_PhotoHighScore );

    // 写真の総合得点
    Setup_TotalScore();
  }

  this->SetVisible( true );
  this->SetInputEnabled( true );

  if( !(m_pFinderSave->GetTutorialFlags() & Savedata::PokeFinderSave::FINDER_TUTORIAL_SCORE) )
  {
    m_pMsgWindow->Open( WND_KIND_TUTORIAL_SCORE );
    m_Seq = SEQ_TUTORIAL;
  }
  else {
    m_Seq = SEQ_MAIN;
  }

  m_MainSeq = MS_BEGIN;
}

//------------------------------------------------------------------------------
/**
 * @brief   クローズ
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::Close( void )
{
  this->SetVisible( false );
  this->SetInputEnabled( false );

  m_Seq = SEQ_OUT;
}



//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 *
 * @param   photoIdx    写真のインデックス
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::Setup( u32 photoIdx )
{
  Field::TrialModel::FinderPhotoData* photoData = m_pAppParam->photo_data[photoIdx];
  {
    // 写真の得点
    m_PhotoScore = photoData->data.value;

#if PM_DEBUG
    GFL_PRINT( "------------------------------\n" );
    GFL_PRINT( "Photo Score\n" );
    GFL_PRINT( "Score=[%d]\n\n", photoData->data.value );
    
    for( u32 i=0; i<Field::TrialModel::FINDER_POKE_MODEL_MAX; ++i )
    {
      GFL_PRINT( "model_index=[%d]\n", i );
      GFL_PRINT( "{\n" );
      GFL_PRINT( "  base_point=[%.2f]\n", photoData->data.base_point[i] );
      GFL_PRINT( "  appeal=[%.2f]\n", photoData->data.appeal[i] );
      GFL_PRINT( "  dir=[%.2f]\n", photoData->data.dir[i] );
      GFL_PRINT( "  screen=[%.2f]\n", photoData->data.screen[i] );
      GFL_PRINT( "  focus=[%.2f]\n", photoData->data.focus[i] );
      GFL_PRINT( "  out=[%.2f]\n", photoData->data.out[i] );
      GFL_PRINT( "}\n" );
    }
    
    GFL_PRINT( "place_score=[%d]\n", photoData->data.place_score );
    GFL_PRINT( "rand_score=[%d]\n", photoData->data.rand_score );
    
    GFL_PRINT( "------------------------------\n" );
#endif
  }

  if( m_pFinderSave )
  {
    m_PhotoHighScore = m_pFinderSave->GetHighValue();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   最高点のセットアップ
 *
 * @param   score   点数
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::Setup_HighScore( u32 score )
{
  this->SetTextboxPaneNumber(
    m_pTextHighScore, msg_pokefinder_40_03, score, 5, 0, print::NUM_DISP_SPACE );
}

//------------------------------------------------------------------------------
/**
 * @brief   総合点のセットアップ
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::Setup_TotalScore( void )
{
  // 合計点の更新
  m_pFinderSave->AddTotalValue( m_PhotoScore );

  // 合計点の取得
  u32 totalScore = m_pFinderSave->GetTotalValue();

  this->SetTextboxPaneNumber(
    m_pTextTotalScore, msg_pokefinder_40_02, totalScore, 7, 0, print::NUM_DISP_SPACE );
}




//------------------------------------------------------------------------------
/**
 * @brief   カメラアプリのバージョンを取得
 */
//------------------------------------------------------------------------------
u16 FinderMenuScoreDraw::GetVersion( void )
{
  if( m_pFinderSave )
  {
    return m_pFinderSave->GetCameraVersion();
  }

  GFL_ASSERT(0);
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief   カメラアプリのバージョンアップ処理
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::VersionUp( void )
{
  if( m_pFinderSave )
  {
    u16 version = GetVersion() + 1;

    m_pFinderSave->SetCameraVersion( version );

    // バージョン５になったらスタンプを押す
    if( version == APP_VER_5 )
    {
      Savedata::Misc* misc = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMisc();
      {
        misc->SetTrainerPassStamp( Savedata::STAMP_FLAG_FINDER_VERSION_5 );
      }
    }

    FUKUSHIMA_PRINT( "VersionUp!! [%d] -> [%d]\n", version - 1, version );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   バージョンアップか？
 *
 * @return  "true  = バージョンアップ"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool FinderMenuScoreDraw::CheckVersionUp( void )
{
#if defined(GF_PLATFORM_CTR)
  return Field::TrialModel::FinderUtil::CheckNextLevel( m_pGameMan );
#else
  return false;
#endif
}



//------------------------------------------------------------------------------
/**
 * @brief   最高得点演出の開始(アウト)
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::StartHighScoreOutDirect( void )
{
  // 最高得点をアウトさせる
  m_pDrawerBase->StartAnim( LA_FINDER_MENU_FINDER_SCORE_LOW_000_SCOREREWITE_00 );

  m_MainSeq = MS_DIRECT_HIGHSCORE_OUT;
}

//------------------------------------------------------------------------------
/**
 * @brief   最高得点演出の開始(イン)
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::StartHighScoreInDirect( void )
{
  // 最高得点の更新
  Setup_HighScore( m_PhotoScore );

  // 最高得点のイン演出の開始
  m_pDrawerBase->StartAnim( LA_FINDER_MENU_FINDER_SCORE_LOW_000_SCOREREWITE_01 );

  // 一之瀬さん要望対応：最高得点更新@miyachi_soichi
  Sound::PlaySE( SEQ_SE_SYS_NJ_058 );
  m_SeWaitFrame = 0;

  m_MainSeq = MS_DIRECT_HIGHSCORE_IN;
}

//------------------------------------------------------------------------------
/**
 * @brief   総合得点演出の開始
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::StartTotalScoreDirect( void )
{
  // 総合得点の表示演出の開始
  m_pDrawerBase->SetVisiblePane( m_pTotalScorePlate, true );

  Sound::PlaySE( SEQ_SE_PF_TOKUTEN );

  m_MainSeq = MS_DIRECT_TOTALSCORE;
}

//------------------------------------------------------------------------------
/**
 * @brief   バージョンアップ演出の開始
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::StartVersionUpDirect( void )
{
  // バージョンアップ演出の開始

  static const u32 c_VersionUpAnim[] = {
    LA_FINDER_MENU_FINDER_SCORE_LOW_000_VERSIONUP_NORMAL,   //!< 1 -> 2
    LA_FINDER_MENU_FINDER_SCORE_LOW_000_VERSIONUP_NORMAL,   //!< 2 -> 3
    LA_FINDER_MENU_FINDER_SCORE_LOW_000_VERSIONUP_NORMAL,   //!< 3 -> 4
    LA_FINDER_MENU_FINDER_SCORE_LOW_000_VERSIONUP_FINAL,    //!< 4 -> 5
  };

  u8 version = GetVersion();
  m_VersionUpAnim = c_VersionUpAnim[version];

  // バージョンアップアニメの再生
  m_pDrawerBase->StartAnim( m_VersionUpAnim );
  m_pDrawerBase->SetAnimFrame( LA_FINDER_MENU_FINDER_SCORE_LOW_000_VERSIONUP_CHANGE, static_cast<f32>(version) );

  if( version == (APP_VER_FINAL - 1) )
  {
    // バージョンアップ維持アニメの再生
    m_pDrawerBase->StartAnim( LA_FINDER_MENU_FINDER_SCORE_LOW_000_MAXKEEP );

    m_StartME = STRM_ME_FINDER_UPDATE_F;
  }
  else {
    // バージョンアップ維持アニメの再生
    m_pDrawerBase->StartAnim( LA_FINDER_MENU_FINDER_SCORE_LOW_000_SCOREUPNORMAL_KEEP );

    m_StartME = STRM_ME_FINDER_UPDATE;
  }

  // MEの再生
  Sound::StartME( m_StartME );

  m_MainSeq = MS_DIRECT_VERSION_UP;
}



//------------------------------------------------------------------------------
/**
 * @brief   バージョンアップ達成ウィンドウタイプの取得
 *
 * @return  WindowKind    バージョンアップ達成用
 */
//------------------------------------------------------------------------------
WindowKind FinderMenuScoreDraw::GetWindowKind_VersionUp( void )
{
  switch( GetVersion() )
  {
  case APP_VER_1:
    {
      return WND_KIND_SCORE_VER_UP_1;
    } // no break;

  case APP_VER_2:
    {
      return WND_KIND_SCORE_VER_UP_2;
    } // no break;

  case APP_VER_3:
    {
      return WND_KIND_SCORE_VER_UP_3;
    } // no break;

  case APP_VER_4:
    {
      return WND_KIND_SCORE_VER_UP_4;
    } // no break;
  }

  GFL_ASSERT( 0 );
  return WND_KIND_SCORE_VER_UP_1;
}

//------------------------------------------------------------------------------
/**
 * @brief   新バージョン機能説明ウィンドウタイプの取得
 *
 * @return  WindowKind    新バージョン機能説明用
 */
//------------------------------------------------------------------------------
WindowKind FinderMenuScoreDraw::GetWindowKind_VersionDesc( void )
{
  switch( GetVersion() )
  {
  case APP_VER_2:
    {
      return WND_KIND_SCORE_NEW_VER_1;
    } // no break;

  case APP_VER_3:
    {
      return WND_KIND_SCORE_NEW_VER_2;
    } // no break;

  case APP_VER_4:
    {
      return WND_KIND_SCORE_NEW_VER_3;
    } // no break;

  case APP_VER_5:
    {
      return WND_KIND_SCORE_NEW_VER_4;
    } // no break;
  }

  GFL_ASSERT( 0 );
  return WND_KIND_SCORE_NEW_VER_1;
}

//------------------------------------------------------------------------------
/**
 * @brief   最終コメントウィンドウタイプの取得
 *
 * @return  WindowKind    最終コメント用
 */
//------------------------------------------------------------------------------
WindowKind FinderMenuScoreDraw::GetWindowKind_FinalComment( void )
{
  const WindowKind c_FinalComment[] = {
    WND_KIND_SCORE_FINAL_1,
    WND_KIND_SCORE_FINAL_2,
    WND_KIND_SCORE_FINAL_3,
  };

  u32 rand = System::GflUse::GetPublicRand() % GFL_NELEMS( c_FinalComment );

  return c_FinalComment[rand];
}

//------------------------------------------------------------------------------
/**
 * @brief   次の目標ウィンドウタイプの取得
 *
 * @return  WindowKind    次の目標用
 */
//------------------------------------------------------------------------------
WindowKind FinderMenuScoreDraw::GetWindowKind_Next( void )
{
  switch( GetVersion() )
  {
  case APP_VER_1:
    {
      return WND_KIND_SCORE_NEXT_1;
    } // no break;

  case APP_VER_2:
    {
      return WND_KIND_SCORE_NEXT_2;
    } // no break;

  case APP_VER_3:
    {
      return WND_KIND_SCORE_NEXT_3;
    } // no break;

  case APP_VER_4:
    {
      return WND_KIND_SCORE_NEXT_4;
    } // no break;
  }

  GFL_ASSERT( 0 );
  return WND_KIND_SCORE_NEXT_1;
}

//------------------------------------------------------------------------------
/**
 * @brief   応援ウィンドウタイプの取得
 *
 * @return  WindowKind    応援用
 */
//------------------------------------------------------------------------------
WindowKind FinderMenuScoreDraw::GetWindowKind_Support( void )
{
#if defined(GF_PLATFORM_CTR)
  u32 nextExp = 0;

  // 次の目標までの点数を取得
  Field::TrialModel::FinderUtil::CheckNextLevel( m_pGameMan, &nextExp );

  FUKUSHIMA_PRINT( "nextExp=[%d]\n", nextExp );

  if( nextExp <= 1000 )
  {
    return WND_KIND_SCORE_SUPPORT_1;
  }
  //else if( nextExp <= 10000 )
  else if( nextExp <= 5000 )    // @fix NMCat[2402]
  {
    return WND_KIND_SCORE_SUPPORT_2;
  }
  else {
    return WND_KIND_SCORE_SUPPORT_3;
  }
#else
  return WND_KIND_SCORE_SUPPORT_1;
#endif
}





//------------------------------------------------------------------------------
/**
 * @brief   閉じたことを通知
 */
//------------------------------------------------------------------------------
void FinderMenuScoreDraw::OnClosed( void )
{
  switch( m_pMsgWindow->GetWindowKind() )
  {
  case WND_KIND_TUTORIAL_SCORE:
    {
      m_pFinderSave->SetTutorialFlags( Savedata::PokeFinderSave::FINDER_TUTORIAL_SCORE, true );

      m_Seq = SEQ_MAIN;
    } break;

  case WND_KIND_SCORE_BEGIN:
    {
      if( m_PhotoScore < 500 )
      {
        m_pMsgWindow->Open( WND_KIND_SCORE_PHOTO_EVAL_1 );
      }
      else if( m_PhotoScore < 3000 )
      {
        m_pMsgWindow->Open( WND_KIND_SCORE_PHOTO_EVAL_2 );
      }
      else if( m_PhotoScore < 10000 )
      {
        m_pMsgWindow->Open( WND_KIND_SCORE_PHOTO_EVAL_3 );
      }
      else {
        m_pMsgWindow->Open( WND_KIND_SCORE_PHOTO_EVAL_4 );
      }
      m_MainSeq = MS_MSG_SCORE_EVAL;
    } break;

  case WND_KIND_SCORE_PHOTO_EVAL_1:
  case WND_KIND_SCORE_PHOTO_EVAL_2:
  case WND_KIND_SCORE_PHOTO_EVAL_3:
  case WND_KIND_SCORE_PHOTO_EVAL_4:
    {
      if( m_PhotoScore > m_PhotoHighScore )
      {
        m_pMsgWindow->Open( WND_KIND_SCORE_HIGH_SCORE );
        m_MainSeq = MS_MSG_HIGH_SCORE;
      }
      else {
        // 総合得点の演出開始
        StartTotalScoreDirect();
      }
    } break;

  case WND_KIND_SCORE_HIGH_SCORE:
    {
      // 最高得点をアウトさせる
      StartHighScoreOutDirect();
    } break;

  case WND_KIND_SCORE_VER_UP_1:
  case WND_KIND_SCORE_VER_UP_2:
  case WND_KIND_SCORE_VER_UP_3:
  case WND_KIND_SCORE_VER_UP_4: //@fix GFNMCat[1201]
    {
      m_pMsgWindow->Open( WND_KIND_SCORE_DOWNLOAD );
      m_MainSeq = MS_MSG_DOWNLOAD;
    } break;

  case WND_KIND_SCORE_DOWNLOAD:
    {
      // バージョンアップ処理
      VersionUp();

      m_pMsgWindow->Open( GetWindowKind_VersionDesc() );
      m_MainSeq = MS_MSG_VERSION_DESC;
    } break;

  case WND_KIND_SCORE_NEXT_1:
  case WND_KIND_SCORE_NEXT_2:
  case WND_KIND_SCORE_NEXT_3:
  case WND_KIND_SCORE_NEXT_4: //@fix GFNMCat[1201]
    {
      m_pMsgWindow->Open( GetWindowKind_Support() );
      m_MainSeq = MS_MSG_SUPPORT;
    } break;

  case WND_KIND_SCORE_NEW_VER_1:
  case WND_KIND_SCORE_NEW_VER_2:
  case WND_KIND_SCORE_NEW_VER_3:
  case WND_KIND_SCORE_NEW_VER_4: //@fix GFNMCat[1201]
  case WND_KIND_SCORE_FINAL_1:
  case WND_KIND_SCORE_FINAL_2:
  case WND_KIND_SCORE_FINAL_3:
  case WND_KIND_SCORE_SUPPORT_1:
  case WND_KIND_SCORE_SUPPORT_2:
  case WND_KIND_SCORE_SUPPORT_3:
    {
      m_uiListener->OpenCmdSelect();
      m_MainSeq = MS_END;
    } break;
  }
}





//==============================================================================
// @brief   ポケファインダーメニュー：得点表示画面ウィンドウ
//==============================================================================

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace msgWnd
{
  enum {
    SEQ_INIT,
    SEQ_IDLE,
    SEQ_OPEN,
    SEQ_MSG,
    SEQ_MAIN,
    SEQ_CLOSE,
    SEQ_TERM,
    SEQ_END,
  };

  const struct ScoreMsgParam
  {
    u32   msgID;    //!< メッセージID
    bool  curVis;   //!< カーソル表示
  } c_ScoreMsgList[] = {
    { msg_pokefinder_tutorial_03_01, true },    //!< チュートリアル：得点表示
    { msg_pokefinder_score_01, true },          //!< 開始メッセージ
    { msg_pokefinder_score_19, false },         //!< 評価１
    { msg_pokefinder_score_20, false },         //!< 評価２
    { msg_pokefinder_score_21, false },         //!< 評価３
    { msg_pokefinder_score_22, false },         //!< 評価４
    { msg_pokefinder_score_02, false },         //!< 最高記録更新
    { msg_pokefinder_score_09, true },          //!< バージョンアップ１
    { msg_pokefinder_score_10, true },          //!< バージョンアップ２
    { msg_pokefinder_score_11, true },          //!< バージョンアップ３
    { msg_pokefinder_score_23, true },          //!< バージョンアップ４
    { msg_pokefinder_score_15, false },         //!< データ受信
    { msg_pokefinder_score_16, false },         //!< 新バージョン説明１
    { msg_pokefinder_score_17, false },         //!< 新バージョン説明２
    { msg_pokefinder_score_18, false },         //!< 新バージョン説明３
    { msg_pokefinder_score_24, false },         //!< 新バージョン説明４
    { msg_pokefinder_score_03, true },          //!< 次の目標１
    { msg_pokefinder_score_04, true },          //!< 次の目標２
    { msg_pokefinder_score_05, true },          //!< 次の目標３
    { msg_pokefinder_score_25, true },          //!< 次の目標４
    { msg_pokefinder_score_12, false },         //!< サポート１
    { msg_pokefinder_score_13, false },         //!< サポート２
    { msg_pokefinder_score_14, false },         //!< サポート３
    { msg_pokefinder_score_06, false },         //!< 最終１
    { msg_pokefinder_score_07, false },         //!< 最終２
    { msg_pokefinder_score_08, false },         //!< 最終３
  };
};

//------------------------------------------------------------------------------
/**
 * @brief    コンストラクタ
 */
//------------------------------------------------------------------------------
FinderMenuScoreMessageWindow::FinderMenuScoreMessageWindow( DrawerBase* base, app::util::AppRenderingManager* renderMan, MessageWindowListener* listener )
  : m_pBase(base)
  , m_pMenuWindow(NULL)
  , m_pListener(listener)
  , m_InitSeq(0)
  , m_EndSeq(0)
  , m_MainSeq(msgWnd::SEQ_INIT)
  , m_WndKind(WND_KIND_NUM)
  , m_ActiveAnim(0)
  , m_OpenReqKind(WND_KIND_NUM)
  , m_isOpened(false)
  , m_CursorVisible(false)
  , m_PlaySE(true)
  , m_Result(gfl2::str::StrWin::RES_FINISH)
  , m_pMsgLine_1(NULL)
  , m_pMsgLine_2(NULL)
  , m_pMsgCursor(NULL)
{
  gfl2::lyt::LytParts* msgParts = m_pBase->GetParts( PANENAME_FINDER_SCORE_LOW_000_PANE_MESSAGE_L2 );
  {
    m_pMsgLine_1 = m_pBase->GetTextBox( PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1A, msgParts );
    m_pMsgLine_2 = m_pBase->GetTextBox( PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1B, msgParts );

    gfl2::lyt::LytPane* nullPane = m_pBase->GetPane( PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1, msgParts );
    {
      m_pBase->SetVisiblePane( nullPane, true );
    }

    gfl2::lyt::LytTextBox* textbox = m_pBase->GetTextBox( PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2, msgParts );
    {
      m_pBase->SetVisiblePane( textbox, false );
    }

    m_pMsgCursor = m_pBase->GetPane( PANENAME_COMMON_MSG_LOW_000_PANE_CURSOR_00, msgParts );

    gfl2::lyt::LytPane* allPane = m_pBase->GetPane( PANENAME_COMMON_MSG_LOW_000_PANE_ALL, msgParts );
    {
      m_pBase->SetAlpha( allPane, 0 );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief    初期化
 */
//------------------------------------------------------------------------------
bool FinderMenuScoreMessageWindow::InitFunc( void )
{
  enum {
    INIT_SEQ_MSG_WND,
    INIT_SEQ_WAIT,
    INIT_SEQ_END,
  };

  switch( m_InitSeq )
  {
  case INIT_SEQ_MSG_WND:
    {
      app::util::G2DUtil* g2d = m_pBase->GetG2D();
      {
        g2d->CreateMessageWindow(
          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(),
          0, m_pMsgLine_1, m_pMsgLine_2, m_pBase->GetHeap() );

        g2d->CreateMsgCursor( m_pBase->GetHeap(), m_pMsgCursor );
      }

      m_InitSeq++;
      m_MainSeq = msgWnd::SEQ_IDLE;
    } break;

  case INIT_SEQ_END:
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief    破棄
 */
//------------------------------------------------------------------------------
bool FinderMenuScoreMessageWindow::EndFunc( void )
{
  enum {
    END_SEQ_INIT,
    END_SEQ_WAIT,
    END_SEQ_DELETE,
    END_SEQ_END,
  };

  switch( m_EndSeq )
  {
  case END_SEQ_INIT:
    {
      m_MainSeq = msgWnd::SEQ_TERM;
      m_EndSeq++;
    } break;

  case END_SEQ_WAIT:
    {
      if( m_pMenuWindow )
      {
        if( !m_pMenuWindow->IsDelete() ) break;
      }

      m_EndSeq++;
    } break;

  case END_SEQ_DELETE:
    {
      GFL_SAFE_DELETE( m_pMenuWindow );

      app::util::G2DUtil* g2d = m_pBase->GetG2D();
      {
        g2d->DeleteMessageWindow();
      }

      m_MainSeq = msgWnd::SEQ_END;
      m_EndSeq++;
    } break;

  case END_SEQ_END:
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief    更新
 */
//------------------------------------------------------------------------------
void FinderMenuScoreMessageWindow::UpdateFunc( void )
{
  app::util::G2DUtil* g2d = m_pBase->GetG2D();
  gfl2::str::StrWin::Result result = g2d->PrintMessage();

  switch( m_MainSeq )
  {
  case msgWnd::SEQ_OPEN:
    {
      if( m_pBase->IsAnimEnd( LA_FINDER_MENU_FINDER_SCORE_LOW_000__MESSAGE_L2_IN ) )
      {
        // ロトムクチパクの再生
        m_pBase->StartAnim( LA_FINDER_MENU_FINDER_SCORE_LOW_000_ROTOMSPEAK );

        m_MainSeq = msgWnd::SEQ_MSG;
      }
    } break;

  case msgWnd::SEQ_MSG:
    {
      if( result == gfl2::str::StrWin::RES_DONE )
      {
        // 再生中なら停止させる
        if( !m_pBase->IsAnimEnd( LA_FINDER_MENU_FINDER_SCORE_LOW_000_ROTOMSPEAK ) )
        {
          // ロトムクチパクの停止
          m_pBase->StopAnim( LA_FINDER_MENU_FINDER_SCORE_LOW_000_ROTOMSPEAK, true );
        }

        if( m_CursorVisible )
        {
          // カーソルを表示
          SetVisibleCursor();

          m_PlaySE = false;
        }
      }

      if( result == m_Result )
      {
        if( !m_PlaySE )
        {
          Sound::PlaySE( SEQ_SE_MESSAGE );

          m_PlaySE = true;
        }

        Close();
      }
    } break;

  case msgWnd::SEQ_CLOSE:
    {
      if( m_pBase->IsAnimEnd( LA_FINDER_MENU_FINDER_SCORE_LOW_000__MESSAGE_L2_OUT ) )
      {
        m_MainSeq = msgWnd::SEQ_IDLE;

        if( m_OpenReqKind == WND_KIND_NUM )
        {
          // 入力を有効
          if( m_pListener )
          {
            m_pListener->OnClosed();
            m_pListener->SetButtonEnable( true );
          }

          m_isOpened = false;
        }
        else {
          Open( m_OpenReqKind );
          m_OpenReqKind = WND_KIND_NUM;
        }
      }
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief    開く
 */
//------------------------------------------------------------------------------
void FinderMenuScoreMessageWindow::Open( WindowKind kind )
{
  app::ui::UIView* view = m_pBase->GetUIView();

  if( m_MainSeq == msgWnd::SEQ_IDLE )
  {
    m_WndKind = kind;

    // メッセージの設定
    app::util::G2DUtil* g2d = m_pBase->GetG2D();
    {
      gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );
      {
        g2d->GetString( str, msgWnd::c_ScoreMsgList[kind - WND_KIND_TUTORIAL_SCORE].msgID );

        gfl2::str::StrWin::FinishType finishType;
        {
          m_Result   = gfl2::str::StrWin::RES_FINISH;
          finishType = gfl2::str::StrWin::FINISH_USER;
        }
        g2d->SetMessageFinishMode( finishType, 0 );

        g2d->SetMessage( str );
      }

      // カーソルの非表示
      SetInvisibleCursor();
    }

    //m_pBase->SetVisiblePane( m_pMsgLine_1, true );
    //m_pBase->SetVisiblePane( m_pMsgLine_2, true );

    // カーソルアニメの再生
    m_pBase->StartAnim( LA_FINDER_MENU_FINDER_SCORE_LOW_000__MESSAGE_L2__CURSOR_00_KEEP );

    // インアニメの再生
    m_pBase->StartAnim( LA_FINDER_MENU_FINDER_SCORE_LOW_000__MESSAGE_L2_IN );

    // 入力を無効
    if( m_pListener )
    {
      m_pListener->SetButtonEnable( false );
    }

    m_isOpened = true;
    m_MainSeq  = msgWnd::SEQ_OPEN;
  }
  else {
    m_OpenReqKind = kind;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief    閉じる
 */
//------------------------------------------------------------------------------
void FinderMenuScoreMessageWindow::Close( void )
{
  // アウトアニメ
  m_pBase->StartAnim( LA_FINDER_MENU_FINDER_SCORE_LOW_000__MESSAGE_L2_OUT );

  m_MainSeq = msgWnd::SEQ_CLOSE;
}

//------------------------------------------------------------------------------
/**
 * @brief    開いているか
 */
//------------------------------------------------------------------------------
bool FinderMenuScoreMessageWindow::IsOpened( void ) const
{
  return m_isOpened;
}

//------------------------------------------------------------------------------
/**
 * @brief    ウィンドウの種類を取得
 */
//------------------------------------------------------------------------------
WindowKind FinderMenuScoreMessageWindow::GetWindowKind( void ) const
{
  return m_WndKind;
}


//------------------------------------------------------------------------------
/**
 * @brief   カーソルの表示切り替え
 */
//------------------------------------------------------------------------------
void FinderMenuScoreMessageWindow::SetVisibleCursor( void )
{
  app::util::G2DUtil* g2d = m_pBase->GetG2D();
  {
    // カーソルを非表示
    g2d->SetMsgCursorUserVisible( true, true );
  }

  // 一度だけでいいのでフラグを初期化
  m_CursorVisible = false;
}

void FinderMenuScoreMessageWindow::SetInvisibleCursor( void )
{
  app::util::G2DUtil* g2d = m_pBase->GetG2D();
  {
    // カーソルを非表示
    g2d->SetMsgCursorUserVisible( false, false );
  }

  // メッセージの終端でカーソルを表示するかどうか
  if( msgWnd::c_ScoreMsgList[m_WndKind - WND_KIND_TUTORIAL_SCORE].curVis )
  {
    m_CursorVisible = true;
  }
  else {
    m_CursorVisible = false;
  }
}

GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)
