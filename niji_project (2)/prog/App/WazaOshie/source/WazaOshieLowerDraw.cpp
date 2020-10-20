//======================================================================
/**
 * @file    WazaOshieLowerDraw.cpp
 * @date    2015/07/01 15:45:28
 * @author  fukushima_yuya
 * @brief   技教え・技忘れ下画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "WazaOshieConst.h"
#include "WazaOshieDrawerBase.h"
#include "WazaOshieDrawListener.h"
#include "WazaOshieInfoWindow.h"
#include "WazaOshieLowerExplain.h"
#include "WazaOshieLowerDraw.h"
#include "WazaOshieLowerStatus.h"

#include <App/PokeList/include/PokeListIntermediateData.h>
#include <AppLib/include/Tool/AppCommonGrpIconData.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_KeyRepeatController.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <Print/include/WordSet.h>
#include <Print/include/SystemFont.h>
#include <sound/include/Sound.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>
#include <str/include/gfl2_Str.h>

// arc
#include <niji_conv_header/app/common_status/common_status.h>
#include <niji_conv_header/app/common_status/common_status_pane.h>
#include <niji_conv_header/app/common_status/common_status_anim_list.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  enum Sequence
  {
    SEQ_INIT,
    SEQ_READY,
    SEQ_IN,
    SEQ_MAIN,
    SEQ_OUT,
    SEQ_END_READY,
    SEQ_END
  };
}


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
WazaOshieLowerDraw::WazaOshieLowerDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager, MenuCursor* pCursor )
  : app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap( pHeap )
  , m_uiListener( NULL )
  , m_pAppRenderingManager( pAppRenderingManager )
  , m_pAsyncFileManager( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager() )
  , m_pFadeManager( NULL )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pCommonIcon( NULL )
  , m_pPartyData( NULL )
  , m_CreateNumFont( true )
  , m_pAppParam( NULL )
  , m_Seq( SEQ_INIT )
  , m_DispMode( 0 )
  , m_SelectPlate( 0 )
  , m_pStatus( NULL )
  , m_pExplain( NULL )
  , m_pCursor( pCursor )
  , m_pInfoWindow( NULL )
  , m_pPokeParam( NULL )
  , m_WazaCount( 1 )
{
  for( u8 i=0; i<DISP_MODE_NUM; ++i )
  {
    m_pKeyController[i] = NULL;
    m_CursorPos[i]      = 0;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void WazaOshieLowerDraw::Init( InitParam& param )
{
  {
    m_pAppParam     = param.appParam;
    m_pMsgData      = param.msgData;
    m_pWordSet      = param.wordSet;
    m_pFadeManager  = param.fadeMan;
    m_pCommonIcon   = param.cmnIcon;
    m_pPartyData    = param.partyData;
    m_CreateNumFont = param.createNumFont;
  }

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( param.arcBuff );
  InitializeText();

  SetInputListener( this );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool WazaOshieLowerDraw::End( void )
{
  GFL_SAFE_DELETE( m_pInfoWindow );

  for( u32 i=0; i<DISP_MODE_NUM; ++i )
  {
    GFL_SAFE_DELETE( m_pKeyController[i] );
  }

  GFL_SAFE_DELETE( m_pExplain );
  GFL_SAFE_DELETE( m_pStatus );

  if( m_pDrawerBase[RES_ID_STATUS] )
  {
    app::tool::PokeIconToolEx* pokeIcon = m_pDrawerBase[RES_ID_STATUS]->GetPokeIcon();

    if( pokeIcon && !pokeIcon->EndFunc() ) return false;

    m_pDrawerBase[RES_ID_STATUS]->DeletePokeIcon();
  }

  if( m_pCursor )
  {
    m_pCursor->UnsetupCursor();
  }

  for( u32 i=0; i<RES_ID_NUM; ++i )
  {
    GFL_SAFE_DELETE( m_pDrawerBase[i] );
  }

  Delete2D();
  DeleteGraphicsSystem();
  
  if( m_CreateNumFont )
  {
    print::SystemFont_ResetNumFontInfo(); //@fix GFNMCat[1083]
    print::SystemFont_DeleteNumFont();
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void WazaOshieLowerDraw::Update( void )
{
  app::ui::UIView::Update();

  if( m_pStatus ) m_pStatus->Update();
  if( m_pCursor ) m_pCursor->Update();

  // シーケンス処理
  switch( m_Seq )
  {
  case SEQ_READY:
    {
      if( m_pInfoWindow && !m_pInfoWindow->IsReady() ) break;

      // ステータス画面を開く
      WazaOshieOpen();
    } break;

  case SEQ_IN:
    {
      if( !IsEndInOutAnim( true ) ) break;
      
      this->SetInputEnabled( true );

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_OUT:
    {
      if( !IsEndInOutAnim(false) ) break;

      m_Seq = SEQ_END_READY;
    } break;

  case SEQ_END_READY:
    {
      if( m_pInfoWindow && !m_pInfoWindow->DeleteSequence() ) break;

      if( m_uiListener ) m_uiListener->EndWazaOshie();

      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void WazaOshieLowerDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
 
  if( m_Seq > SEQ_READY )
  {
    pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
    pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 1 );

    if( m_pCursor ) m_pCursor->Draw( no, (gfl2::lyt::ORDER_NORMAL + 1) );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 */
//-----------------------------------------------------------------------------
bool WazaOshieLowerDraw::IsExit( void )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief   ステータス画面のオープン
 */
//-----------------------------------------------------------------------------
void WazaOshieLowerDraw::WazaOshieOpen( void )
{
  // フェードイン
  if( m_pFadeManager ) m_pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, gfl2::Fade::FADE_DEFAULT_SYNC );

  // カーソルの表示
  if( m_pCursor )
  {
    m_pCursor->SetVisible( true );
  }

  m_Seq = SEQ_IN;
}

//-----------------------------------------------------------------------------
/**
 * @brief   ステータス画面のクローズ
 */
//-----------------------------------------------------------------------------
void WazaOshieLowerDraw::WazaOshieClose( void )
{
  // 入力を無効
  this->SetInputEnabled( false );

  // フェードアウト
  gfl2::math::Vector4 color( 0, 0, 0, 255 );
  if( m_pFadeManager ) m_pFadeManager->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );

  m_Seq = SEQ_OUT;
}

//-----------------------------------------------------------------------------
/**
 * @brief   イン・アウトアニメが終了したかどうか
 * @param   in    インアニメかどうか
 * @return  true = 終了、false = 継続
 */
//-----------------------------------------------------------------------------
bool WazaOshieLowerDraw::IsEndInOutAnim( bool in )
{
  if( m_pFadeManager )
  {
    return m_pFadeManager->IsEnd( gfl2::Fade::DISP_LOWER )
      && m_pFadeManager->IsEnd( gfl2::Fade::DISP_UPPER );
  }

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief   このビュー専用のUIリスナー設定
 * @param   pListener   UIリスナー
 */
//-----------------------------------------------------------------------------
void WazaOshieLowerDraw::SetTitleViewUIListener( WazaOshieDrawListener* pListener )
{
  m_uiListener = pListener;
}



//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pHeap         ヒープ
 * @param   pAppLytBuff   レイアウトバイナリデータ
 */
//------------------------------------------------------------------------------
void WazaOshieLowerDraw::Initialize2D( void* pAppLytBuff )
{
  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000_BFLYT,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
    {
      0,
      LYTRES_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000_BFLYT,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  SetWordSetLoader( m_pWordSetLoader );

  // 2Dの生成
  Create2D(
    m_pHeap,
    NULL,
    2,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    m_pMsgData,
    m_pWordSet );

  this->SetInputEnabled( false );

  {
    m_pDrawerBase[RES_ID_STATUS] = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, RES_ID_STATUS, m_pWordSet, m_pMsgData );
    {
      GFL_ASSERT( m_pDrawerBase[RES_ID_STATUS] );
      
      // BGアニメの再生
      m_pDrawerBase[RES_ID_STATUS]->StartAnim( LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__BG_00_BG_LOOP_00 );

      // ポケモンアイコン
      app::tool::PokeIconToolEx::PokeIconToolParam param;
      {
        param.pHeap   = m_pHeap;
        param.pUIView = this;
        param.iconMax = 1;
        param.sync    = true;
      }
      m_pDrawerBase[RES_ID_STATUS]->CreatePokeIcon( param );
    }

    m_pDrawerBase[RES_ID_EXPLAIN] = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, RES_ID_EXPLAIN, m_pWordSet, m_pMsgData );
    {
      GFL_ASSERT( m_pDrawerBase[RES_ID_EXPLAIN] );

      // BGアニメの再生
      m_pDrawerBase[RES_ID_EXPLAIN]->StartAnim( LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000_BG_KEEP );
    }

    // ボタンマネージャの生成
    CreateButtonManager();
    SetButtonEnable( DISP_MODE_STATUS );
  }

  {
    u8 mode = m_pAppParam->mode;

    u8 pokeIndex = m_pAppParam->select_poke_index;
    App::PokeList::IntermediateData* data = m_pPartyData;
    bool moudoku = data->IsSickMoudoku( pokeIndex );

    m_pPokeParam = data->GetPoke( pokeIndex );
    const pml::pokepara::PokemonParam* pp = m_pPokeParam;

    m_pStatus = GFL_NEW(devHeap) WazaOshieStatus( m_pDrawerBase[RES_ID_STATUS], mode );
    GFL_ASSERT( m_pStatus );
    m_pStatus->Setup( m_pCommonIcon, moudoku, pp, m_pAppParam->waza_no );

    m_pExplain = GFL_NEW(devHeap) WazaOshieExplain( m_pDrawerBase[RES_ID_EXPLAIN], m_pCommonIcon );
    GFL_ASSERT( m_pExplain );

    bool fastmode_flag = pp->StartFastMode();
    m_WazaCount = (mode == MODE_WAZA_OBOE) ? 5 : pp->GetWazaCount();  //!< 技覚えの場合、この画面に来るときは必ず５つになっている。
    pp->EndFastMode( fastmode_flag );

    const u32 CUR_POS_PANE_MAX = 7;

    // カーソル
    {
      static const u32 c_CursorPosIndex[CUR_POS_PANE_MAX][3] = {
        { PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_00, PANENAME_STATUS_PLT_LOW_000_PANE_NULL_CUR, RES_ID_STATUS },
        { PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_01, PANENAME_STATUS_PLT_LOW_000_PANE_NULL_CUR, RES_ID_STATUS },
        { PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_02, PANENAME_STATUS_PLT_LOW_000_PANE_NULL_CUR, RES_ID_STATUS },
        { PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_03, PANENAME_STATUS_PLT_LOW_000_PANE_NULL_CUR, RES_ID_STATUS },
        { PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_04, PANENAME_STATUS_PLT_LOW_000_PANE_NULL_CUR, RES_ID_STATUS },
        { PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_PARTSYN_00,       PANENAME_COMMON_BTN_LOW_005_PANE_NULL_CUR, RES_ID_EXPLAIN },
        { PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_PARTSYN_01,       PANENAME_COMMON_BTN_LOW_005_PANE_NULL_CUR, RES_ID_EXPLAIN },
      };

      gfl2::lyt::LytPane* curPosPane[CUR_POS_PANE_MAX];

      for( u8 i=0; i<CUR_POS_PANE_MAX; ++i )
      {
        u32 partsIndex = c_CursorPosIndex[i][0];
        u32 paneIndex  = c_CursorPosIndex[i][1];
        u32 drawIndex  = c_CursorPosIndex[i][2];

        gfl2::lyt::LytParts* parts = m_pDrawerBase[drawIndex]->GetParts( partsIndex );
        curPosPane[i] = m_pDrawerBase[drawIndex]->GetPane( paneIndex, parts );
      }

      // セットアップ
      m_pCursor->SetupCursor( this->GetG2DUtil(), curPosPane, CUR_POS_PANE_MAX, gfl2::lyt::DISPLAY_LOWER );
      m_pCursor->SetCursorPos( m_pDrawerBase[RES_ID_STATUS]->GetLytWk(), 0 );
    }

    //
    {
      m_pKeyController[DISP_MODE_STATUS] = GFL_NEW(devHeap) app::util::KeyRepeatController( m_WazaCount-1, 0 );
      GFL_ASSERT( m_pKeyController[DISP_MODE_STATUS] );

      u32 btnNum = 2;
      m_pKeyController[DISP_MODE_EXPLAIN] = GFL_NEW(devHeap) app::util::KeyRepeatController( btnNum-1, 0 );
      GFL_ASSERT( m_pKeyController[DISP_MODE_EXPLAIN] );
    }

    m_pInfoWindow = GFL_NEW(devHeap) WazaOshieInfoWindow( m_pDrawerBase[RES_ID_STATUS], m_pAppRenderingManager, this );
    GFL_ASSERT( m_pInfoWindow );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void WazaOshieLowerDraw::InitializeText( void )
{

}

//------------------------------------------------------------------------------
/**
 * @brief   画面モードの変更
 * @param   dispMode    画面モード
 */
//------------------------------------------------------------------------------
void WazaOshieLowerDraw::SetDispMode( u8 dispMode )
{
  // カーソルのモードを変更
  if( m_pCursor )
  {
    if( dispMode == DISP_MODE_STATUS )
    {
      m_CursorPos[DISP_MODE_STATUS] = m_SelectPlate;
      m_pCursor->SetCursorPos( m_pDrawerBase[RES_ID_STATUS]->GetLytWk(), m_CursorPos[DISP_MODE_STATUS] );
    }
    else {
      m_CursorPos[DISP_MODE_EXPLAIN] = 0;
      m_pCursor->SetCursorPos( m_pDrawerBase[RES_ID_EXPLAIN]->GetLytWk(), m_CursorPos[DISP_MODE_EXPLAIN] + 5 );
    }
  }

  // ボタンの入力有効設定を変更
  SetButtonEnable( dispMode );

  m_DispMode = dispMode;
}

//------------------------------------------------------------------------------
/**
 * @brief   技説明画面のセットアップ
 * @param   index   技プレートのインデックス
 */
//------------------------------------------------------------------------------
void WazaOshieLowerDraw::SetupWazaExplain( u32 index )
{
  WazaOshieExplain::SetupParam param;

  if( index == 4 )
  {
    param.wazaNo = m_pAppParam->waza_no;
    param.nowPP  = pml::wazadata::GetMaxPP( param.wazaNo, 0 );
    param.maxPP  = param.nowPP;
    m_pExplain->Setup( MODE_WAZA_OBOE, param );
  }
  else {
    // セットアップパラメータのセット
    m_pStatus->SetExplainSetupParam( index, &param );
    m_pExplain->Setup( MODE_WAZA_WASURE, param );
  }

  m_SelectPlate = index;
}

//------------------------------------------------------------------------------
/**
 * @brief   次の技へ
 */
//------------------------------------------------------------------------------
void WazaOshieLowerDraw::SetupWazaNext( void )
{
  u32 plateMax = (m_pAppParam->mode == MODE_WAZA_OBOE) ? 5 : m_WazaCount;

  u32 next = (m_SelectPlate + 1) % plateMax;

  SetupWazaExplain( next );
}

//------------------------------------------------------------------------------
/**
 * @brief   前の技へ
 */
//------------------------------------------------------------------------------
void WazaOshieLowerDraw::SetupWazaPrev( void )
{
  u32 plateMax = (m_pAppParam->mode == MODE_WAZA_OBOE) ? 5 : m_WazaCount;

  u32 prev = (m_SelectPlate == 0) ? (plateMax - 1) : (m_SelectPlate - 1);

  SetupWazaExplain( prev );
}







//==============================================================================
/**
 *      ↓　以下がリスナー処理　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult WazaOshieLowerDraw::OnLayoutPaneEvent( const u32 paneId )
{
  GFL_UNUSED( paneId );

  if( m_uiListener == NULL ) return ENABLE_ACCESS;

  switch( paneId )
  {
  case BTN_ID_BACK:
    {
      m_pAppParam->end_mode = END_MODE_CANCEL;

      WazaOshieClose();
    } break;

  case BTN_ID_WAZA_0:
  case BTN_ID_WAZA_1:
  case BTN_ID_WAZA_2:
  case BTN_ID_WAZA_3:
    {
      u32 index = paneId - BTN_ID_WAZA_0;

      m_pCursor->SetCursorPos( m_pDrawerBase[RES_ID_STATUS]->GetLytWk(), index );

      // 画面モードの変更
      SetDispMode( DISP_MODE_EXPLAIN );

      // 技選択画面(技を忘れる)
      SetupWazaExplain( index );
    } break;

  case BTN_ID_WAZA_4:
    {
      u32 index = paneId - BTN_ID_WAZA_0;

      m_pCursor->SetCursorPos( m_pDrawerBase[RES_ID_STATUS]->GetLytWk(), index );

      // 画面モードの変更
      SetDispMode( DISP_MODE_EXPLAIN );

      // 技選択画面(覚えるのを諦める)
      SetupWazaExplain( index );
    } break;

  case BTN_ID_INFO_0:
    {
      if( !m_pInfoWindow ) break;

      //m_pCursor->SetCursorPos( m_pDrawerBase[RES_ID_STATUS]->GetLytWk(), 0 );
      m_pCursor->SetVisible( false );

      TokuseiNo tokusei = m_pStatus->GetActTokuseiNo();
      m_pInfoWindow->OpenTokusei( tokusei );
    } break;

  case BTN_ID_INFO_1:
    {
      if( !m_pInfoWindow ) break;

      //m_pCursor->SetCursorPos( m_pDrawerBase[RES_ID_STATUS]->GetLytWk(), 1 );
      m_pCursor->SetVisible( false );

      u16 itemNo = m_pStatus->GetActItemNo();
      m_pInfoWindow->OpenItem( itemNo );
    } break;

  case BTN_ID_DSC_DECIDE:
    {
      // 決定された技IDを設定
      m_pAppParam->waza_no = m_pExplain->GetWazaNo();
      m_pAppParam->select_index = static_cast<u8>(m_SelectPlate);
      m_pAppParam->end_mode = END_MODE_DECIDE;

      // 技覚え・技忘れを閉じる
      WazaOshieClose();
    } break;

  case BTN_ID_DSC_CANCEL:
    {
      // 技説明画面を非表示
      m_pExplain->SetVisible( false );

      // 画面モードの変更
      SetDispMode( DISP_MODE_STATUS );
    } break;

  case BTN_ID_DSC_ARROW_L:
    {
      SetupWazaPrev();
    } break;

  case BTN_ID_DSC_ARROW_R:
    {
      SetupWazaNext();
    } break;
  }

  return ENABLE_ACCESS;
}


//------------------------------------------------------------------------------
/**
 * @brief   キー入力通知用リスナ
 * @param   pKey
 * @param   pStick
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult WazaOshieLowerDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ) return ENABLE_ACCESS;

  app::tool::ButtonManager* bm = app::ui::UIResponder::GetButtonManager();

  // 技切り替え
  if( m_DispMode == DISP_MODE_STATUS )
  {
    {
      app::util::KeyRepeatController* keyRepeat = m_pKeyController[DISP_MODE_STATUS];
      u8 curPos = m_CursorPos[DISP_MODE_STATUS];
      u8 btnNum = (m_pAppParam->mode == MODE_WAZA_OBOE) ? 5 : m_WazaCount;

      keyRepeat->UpdateInfo( pKey, curPos, 0 );

      if( keyRepeat->IsRepeat( gfl2::ui::DIR_UP ) )
      {
        curPos = (curPos + btnNum - 1) % btnNum;
      }
      else if( keyRepeat->IsRepeat( gfl2::ui::DIR_DOWN ) )
      {
        curPos = (curPos + 1) % btnNum;
      }

      if( m_CursorPos[DISP_MODE_STATUS] != curPos )
      {
        m_pCursor->SetCursorPos( m_pDrawerBase[RES_ID_STATUS]->GetLytWk(), curPos );

        m_CursorPos[DISP_MODE_STATUS] = curPos;

        Sound::PlaySE( SEQ_SE_SELECT1 );
      }
    }

    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      u32 pos = m_CursorPos[DISP_MODE_STATUS] + BTN_ID_WAZA_0;
      bm->StartSelectedAct( pos );
    }
  }
  else if( m_DispMode == DISP_MODE_EXPLAIN )
  {
    {
      app::util::KeyRepeatController* keyRepeat = m_pKeyController[DISP_MODE_EXPLAIN];
      u8 curPos = m_CursorPos[DISP_MODE_EXPLAIN];

      keyRepeat->UpdateInfo( pKey, curPos, 0 );

      if( keyRepeat->IsRepeat( gfl2::ui::DIR_UP ) )
      {
        curPos = (curPos + 2 - 1) % 2;
      }
      else if( keyRepeat->IsRepeat( gfl2::ui::DIR_DOWN ) )
      {
        curPos = (curPos + 1) % 2;
      }

      if( m_CursorPos[DISP_MODE_EXPLAIN] != curPos )
      {
        m_pCursor->SetCursorPos( m_pDrawerBase[RES_ID_EXPLAIN]->GetLytWk(), curPos + 5 );

        m_CursorPos[DISP_MODE_EXPLAIN] = curPos;

        Sound::PlaySE( SEQ_SE_SELECT1 );
      }
    }

    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      bm->StartSelectedAct( m_CursorPos[DISP_MODE_EXPLAIN]+ BTN_ID_DSC_DECIDE );
    }
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_L ) || pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
    {
      bm->StartSelectedAct( BTN_ID_DSC_ARROW_L );
    }
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_R ) || pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
    {
      bm->StartSelectedAct( BTN_ID_DSC_ARROW_R );
    }
  }

  return ENABLE_ACCESS;
}


//------------------------------------------------------------------------------
/**
 * @brief   リソース生成前に必要なデータを登録。フォントリソースなど
 */
//------------------------------------------------------------------------------
void WazaOshieLowerDraw::OnPreLoadResource( void )
{
  if( m_CreateNumFont )
  {
    print::SystemFont_CreateNumFont( m_pHeap->GetDeviceHeap(), m_pHeap->GetDeviceHeap() );
    print::SystemFont_SetLytSystemNumFont();
  }
}




//==============================================================================
/**
 *      ↓　以下がWazaOshieInfoWindowListener関連　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   閉じ終わった
 */
//------------------------------------------------------------------------------
void WazaOshieLowerDraw::OnClose( void )
{
  // カーソルを表示
  m_pCursor->SetVisible( true );

  // 入力を有効
  this->SetInputEnabled( true );
}






//==============================================================================
/**
 *      ↓　以下がボタンマネージャ関連　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 */
//------------------------------------------------------------------------------
void WazaOshieLowerDraw::CreateButtonManager( void )
{
  const app::ui::ButtonInfoLytWk c_DefInfo[] = {
    {
      BTN_ID_BACK, NULL, NULL, NULL,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTS_BACK_TOUCH,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTS_BACK_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTS_BACK_CANSEL,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTS_BACK_TOUCH_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTS_BACK_ACTIVE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTS_BACK_PASSIVE,
    },
    {
      BTN_ID_WAZA_0, NULL, NULL, NULL,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_00_TOUCH,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_00_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_00_CANSEL,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_00_TOUCH_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_00_ACTIVE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_00_PASSIVE,
    },
    {
      BTN_ID_WAZA_1, NULL, NULL, NULL,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_01_TOUCH,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_01_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_01_CANSEL,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_01_TOUCH_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_01_ACTIVE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_01_PASSIVE,
    },
    {
      BTN_ID_WAZA_2, NULL, NULL, NULL,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_02_TOUCH,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_02_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_02_CANSEL,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_02_TOUCH_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_02_ACTIVE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_02_PASSIVE,
    },
    {
      BTN_ID_WAZA_3, NULL, NULL, NULL,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_03_TOUCH,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_03_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_03_CANSEL,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_03_TOUCH_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_03_ACTIVE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_03_PASSIVE,
    },
    {
      BTN_ID_WAZA_4, NULL, NULL, NULL,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_04_TOUCH,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_04_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_04_CANSEL,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_04_TOUCH_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_04_ACTIVE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__PARTSSKILLPLT_04_PASSIVE,
    },
    {
      BTN_ID_INFO_0, NULL, NULL, NULL,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__INFO_00_TOUCH,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__INFO_00_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__INFO_00_CANSEL,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__INFO_00_TOUCH_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__INFO_00_ACTIVE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__INFO_00_PASSIVE,
    },
    {
      BTN_ID_INFO_1, NULL, NULL, NULL,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__INFO_01_TOUCH,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__INFO_01_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__INFO_01_CANSEL,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__INFO_01_TOUCH_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__INFO_01_ACTIVE,
      LA_COMMON_STATUS_COMMONSTATUS_SKL_LOW_000__INFO_01_PASSIVE,
    },
    {
      BTN_ID_DSC_DECIDE, NULL, NULL, NULL,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSYN_00_TOUCH,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSYN_00_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSYN_00_CANSEL,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSYN_00_TOUCH_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSYN_00_ACTIVE,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSYN_00_PASSIVE,
    },
    {
      BTN_ID_DSC_CANCEL, NULL, NULL, NULL,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSYN_01_TOUCH,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSYN_01_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSYN_01_CANSEL,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSYN_01_TOUCH_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSYN_01_ACTIVE,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSYN_01_PASSIVE,
    },
    {
      BTN_ID_DSC_ARROW_L, NULL, NULL, NULL,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSCUR_L_TOUCH,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSCUR_L_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSCUR_L_CANSEL,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSCUR_L_TOUCH_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSCUR_L_ACTIVE,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSCUR_L_PASSIVE,
    },
    {
      BTN_ID_DSC_ARROW_R, NULL, NULL, NULL,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSCUR_R_TOUCH,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSCUR_R_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSCUR_R_CANSEL,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSCUR_R_TOUCH_RELEASE,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSCUR_R_ACTIVE,
      LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTSCUR_R_PASSIVE,
    },
  };

  const gfl2::lyt::LytPaneIndex c_ButtonPaneIndex[BTN_ID_NUM][3] = {
    { RES_ID_STATUS, PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTS_BACK, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00 },
    { RES_ID_STATUS, PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_00, PANENAME_STATUS_PLT_LOW_000_PANE_BOUND_00 },
    { RES_ID_STATUS, PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_01, PANENAME_STATUS_PLT_LOW_000_PANE_BOUND_00 },
    { RES_ID_STATUS, PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_02, PANENAME_STATUS_PLT_LOW_000_PANE_BOUND_00 },
    { RES_ID_STATUS, PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_03, PANENAME_STATUS_PLT_LOW_000_PANE_BOUND_00 },
    { RES_ID_STATUS, PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_PARTSSKILLPLT_04, PANENAME_STATUS_PLT_LOW_000_PANE_BOUND_00 },
    { RES_ID_STATUS, PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_INFO_00, PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00 },
    { RES_ID_STATUS, PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_INFO_01, PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00 },
    { RES_ID_EXPLAIN, PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_PARTSYN_00, PANENAME_COMMON_BTN_LOW_005_PANE_BOUND_00 },
    { RES_ID_EXPLAIN, PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_PARTSYN_01, PANENAME_COMMON_BTN_LOW_005_PANE_BOUND_00 },
    { RES_ID_EXPLAIN, PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_PARTSCUR_L, PANENAME_COMMON_CUR_LOW_002_PANE_BOUND_00 },
    { RES_ID_EXPLAIN, PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_PARTSCUR_R, PANENAME_COMMON_CUR_LOW_002_PANE_BOUND_00 },
  };

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();
  app::ui::ButtonInfoLytWk* info = GFL_NEW_LOW_ARRAY(devHeap) app::ui::ButtonInfoLytWk[BTN_ID_NUM];

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    const gfl2::lyt::LytPaneIndex* pArray = c_ButtonPaneIndex[i];
    DrawerBase* pBase = m_pDrawerBase[pArray[0]];

    gfl2::lyt::LytParts* pParts = pBase->GetParts( pArray[1] );

    info[i] = c_DefInfo[i];
    info[i].picture_pane = pParts;
    info[i].bound_pane = pBase->GetPane( pArray[2], pParts );
    info[i].lyt_wk = pBase->GetLytWk();
  }

  app::ui::UIResponder::CreateButtonManager( m_pHeap, info, BTN_ID_NUM );

  app::tool::ButtonManager* pBtnMan = app::ui::UIResponder::GetButtonManager();
  {
    pBtnMan->SetButtonBindKey( BTN_ID_BACK, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
    pBtnMan->SetButtonBindKey( BTN_ID_DSC_CANCEL, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );

    for( u8 i=0; i<BTN_ID_NUM; ++i )
    {
      if( i == BTN_ID_BACK || i == BTN_ID_DSC_CANCEL )
      {
        pBtnMan->SetButtonSelectSE( i, SEQ_SE_CANCEL1 );
      }
      else {
        pBtnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
      }
    }
  }

  GFL_DELETE_ARRAY info;
}


//------------------------------------------------------------------------------
/**
 * @brief   ボタンのセット
 * @param   dispMode  技選択画面 or 技説明画面
 */
//------------------------------------------------------------------------------
void WazaOshieLowerDraw::SetButtonEnable( u8 dispMode )
{
  app::tool::ButtonManager* bm = app::ui::UIResponder::GetButtonManager();

  // ボタンの入力設定
  bool enable = (dispMode == DISP_MODE_STATUS);

  for( u32 i=0; i<BTN_ID_SKL_NUM; ++i )
  {
    bm->SetButtonInputEnable( i, enable );
  }

  for( u32 i=BTN_ID_DSC_DECIDE; i<BTN_ID_NUM; ++i )
  {
    bm->SetButtonInputEnable( i, !enable );
  }

  // ボタンの表示切替
  if( enable )  //!< @fix NMCat[245]
  {
    u32 mode = m_pAppParam->mode;

    if( mode == MODE_WAZA_OBOE )
    {
      bm->SetButtonInvisible( BTN_ID_INFO_0 );
      bm->SetButtonInvisible( BTN_ID_INFO_1 );
      bm->SetButtonInputEnable( BTN_ID_INFO_0, false );
      bm->SetButtonInputEnable( BTN_ID_INFO_1, false );

      bm->SetButtonVisible( BTN_ID_WAZA_4 );
      bm->SetButtonInputEnable( BTN_ID_WAZA_4, true );
    }
    else if( mode == MODE_WAZA_WASURE )
    {
      bm->SetButtonVisible( BTN_ID_INFO_0 );
      bm->SetButtonVisible( BTN_ID_INFO_1 );
      bm->SetButtonInputEnable( BTN_ID_INFO_0, true );
      bm->SetButtonInputEnable( BTN_ID_INFO_1, true );

      bm->SetButtonInvisible( BTN_ID_WAZA_4 );
      bm->SetButtonInputEnable( BTN_ID_WAZA_4, false );
    }
  }
}



GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )
