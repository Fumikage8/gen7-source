//======================================================================
/**
 * @file    StatusLowerDraw.cpp
 * @date    2015/11/27 17:54:05
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：メイン画面(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusLowerDraw.h"
#include "StatusDrawListener.h"
#include "StatusDrawerBase.h"

#include <App/Status/include/StatusAppParam.h>
#include <App/Status/source/lowerParts/StatusPokeModel.h>
#include <App/Status/source/lowerParts/StatusInfoWindow.h>
#include <App/Status/source/lowerParts/StatusWaza.h>
#include <App/Status/source/lowerParts/StatusWazaCursor.h>
#include <App/Status/source/lowerParts/StatusWazaExchange.h>
#include <App/Status/source/lowerParts/StatusMsgWindow.h>

#include <Print/include/SystemFont.h>
#include <Sound/include/Sound.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <niji_conv_header/app/field_status/field_status.h>
#include <niji_conv_header/app/field_status/field_status_pane.h>
#include <niji_conv_header/app/field_status/field_status_anim_list.h>
#include <niji_conv_header/message/msg_pokemon_status.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  enum {
    LAYER_0,
    LAYER_1,
    LAYER_NUM,
  };
}


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
LowerMainDraw::LowerMainDraw( CreateParam& param )
  : app::ui::UIView(param.heap, param.heap->GetDeviceAllocator(), param.heap->GetDeviceAllocator())
  , m_pHeap(param.heap)
  , m_pAppRenderingManager(param.renderMan)
  , m_pFadeManager(param.fadeMan)
  , m_pCommonIcon(param.cmnIcon)
  , m_pCursorWaza(param.wazaCursor)
  , m_pCursorRibbon(param.ribbonCursor)
  , m_uiListener(NULL)
  , m_pPokeParam(NULL)
  , m_pAppParam(NULL)
  , m_pDrawerBase(NULL)
  , m_pLowInfo(NULL)
  , m_pBallPlate(NULL)
  , m_pPokeModel(NULL)
  , m_pInfoWindow(NULL)
  , m_pRibbon(NULL)
  , m_pWaza(NULL)
  , m_pWazaCursor(NULL)
  , m_pWazaExchange(NULL)
  , m_pMsgWindow(NULL)
  , m_Seq(SEQ_INIT)
  , m_PrevTab(TAB_MODE_WAZA)
  , m_PrevEggTab(false)
  , m_UpdateProhibition(true)
  , m_bInputFlag(true)
  , m_bBFCMode(false)
  , m_createNumFont(false)
  , m_pPaneVisibleMark(NULL)
  , m_pPaneVisibleInfo(NULL)
  , m_pPaneVisibleRibbon(NULL)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool LowerMainDraw::End( void )
{
  GFL_SAFE_DELETE( m_pWazaExchange );
  GFL_SAFE_DELETE( m_pWazaCursor );
  GFL_SAFE_DELETE( m_pWaza );
  GFL_SAFE_DELETE( m_pMsgWindow );
  GFL_SAFE_DELETE( m_pRibbon );

  if( m_pInfoWindow )
  {
    if( !m_pInfoWindow->DeleteSequence() ) return false;

    GFL_SAFE_DELETE( m_pInfoWindow );
  }

  if( m_pPokeModel )
  {
    if( !m_pPokeModel->Terminate() ) return false;

    GFL_SAFE_DELETE( m_pPokeModel );
  }

  if( m_pBallPlate )
  {
    if( !m_pBallPlate->Terminate() ) return false;

    GFL_SAFE_DELETE( m_pBallPlate );
  }

  GFL_SAFE_DELETE( m_pLowInfo );
  GFL_SAFE_DELETE( m_pDrawerBase );

  Delete2D();
  DeleteGraphicsSystem();

  if(m_createNumFont)
  {
    print::SystemFont_ResetNumFontInfo(); //@fix GFNMCat[1083]
    print::SystemFont_DeleteNumFont();
  }

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void LowerMainDraw::Init( InitParam& param )
{
  m_pAppParam = param.appParam;

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( param );

  SetInputListener( this );

  m_Seq = SEQ_READY;
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void LowerMainDraw::Initialize2D( InitParam& param )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { param.appLytBuff, 1, static_cast<u32>(app::util::G2DUtil::ATTACH_ROOT) },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    { // MIN_LOW
      0,
      LYTRES_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000_BFLYT,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000___NUM,
      param.appLytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  m_bBFCMode  = param.appParam->IsBFCMode();

  // 2Dの生成
  Create2D(
    m_pHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    param.baseMsgData,
    param.wordSet );

  this->SetInputEnabled( false );

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();
  {
    // 描画共通処理の生成
    m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, param.baseMsgData, param.wordSet );
    {
      GFL_ASSERT(m_pDrawerBase);

      m_pPaneVisibleMark = m_pDrawerBase->GetPane( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_MARK );
      m_pPaneVisibleInfo = m_pDrawerBase->GetPane( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_ABILITY_ITEM );
      m_pPaneVisibleRibbon = m_pDrawerBase->GetPane( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_ALL_RIBBON );

      m_pDrawerBase->StartAnim( LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__BG_WAVE_GREEN_UPP_00_BG_LOOP_00 );
      m_pDrawerBase->StartAnim( LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSPAGEBTN_00_SELECT );
      m_pDrawerBase->StartAnim( LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSPAGEBTN_01_UNSELECT );
    }

    // ボタンマネージャの生成
    CreateButtonManager();

    // ボール
    m_pBallPlate = GFL_NEW(devHeap) BallPlate( m_pDrawerBase );
    {
      GFL_ASSERT( m_pBallPlate );

      // 初期化
      m_pBallPlate->Initialize( this, m_pAppParam->mode );
    }

    // ポケモンモデル
    m_pPokeModel = GFL_NEW(devHeap) PokeModel( m_pDrawerBase, param.fileMan, m_pAppRenderingManager );
    {
      GFL_ASSERT( m_pPokeModel );
    }

    // インフォメーションウィンドウ
    m_pInfoWindow = GFL_NEW(devHeap) InfoWindow( m_pDrawerBase, m_pAppRenderingManager );
    {
      GFL_ASSERT( m_pInfoWindow );

      // 初期化
      m_pInfoWindow->Initialize( param.tokuseiMsgData, param.itemMsgData, this );
    }

    // リボンスクロール
    m_pRibbon = GFL_NEW(devHeap) Ribbon( m_pDrawerBase, m_pCursorRibbon, param.devMan, this );
    {
      GFL_ASSERT( m_pRibbon );

      // 初期化
      m_pRibbon->Initialize( m_uiListener );
    }

    // メッセージウィンドウ
    m_pMsgWindow = GFL_NEW(devHeap) MsgWindow( m_pDrawerBase );
    {
      GFL_ASSERT( m_pMsgWindow );

      // 初期化
      m_pMsgWindow->Initialize();
    }

    // 技
    m_pWaza = GFL_NEW(devHeap) Waza( m_pDrawerBase, m_pCommonIcon );
    {
      GFL_ASSERT( m_pWaza );

      // 初期化
      m_pWaza->Initialize();
    }

    // 技カーソル
    m_pWazaCursor = GFL_NEW(devHeap) WazaCursor( m_pDrawerBase, m_pCursorWaza );
    {
      GFL_ASSERT( m_pWazaCursor );

      // 初期化
      m_pWazaCursor->Initialize();
    }

    // 技カーソル
    m_pWazaExchange = GFL_NEW(devHeap) WazaExchange( m_pDrawerBase );
    {
      GFL_ASSERT( m_pWazaExchange );

      // 初期化パラメータ
      WazaExchange::InitParam initParam;
      {
        initParam.curListener   = m_pWazaCursor;
        initParam.msgListner    = m_pMsgWindow;
        initParam.expListener   = this;
        initParam.uiListener    = m_uiListener;
      };
      // 初期化
      m_pWazaExchange->Initialize( initParam );
    }

    // 情報(下)
    m_pLowInfo = GFL_NEW(devHeap) Information( m_pDrawerBase, m_pCommonIcon );
    {
      GFL_ASSERT( m_pLowInfo );

      // 初期化
      m_pLowInfo->Initialize( this, m_pRibbon );
    }
  }
  
  if(m_bBFCMode)
  {
    InitializeTabForBFC();
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void LowerMainDraw::Update( void )
{
  app::ui::UIView::Update();

  switch( m_Seq )
  {
  case SEQ_READY:
    {
      // 準備シーケンス
      if( !Sequence_Ready() ) break;

      m_Seq = SEQ_IDLE;
    } break;

  case SEQ_IN:
    {
      if( !Sequence_FadeIn() ) break;

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_MAIN:
    {
      // メインシーケンス
      Sequence_Main();
    } break;

  case SEQ_OUT:
    {
      if( m_pCursorWaza )
      {
        m_pCursorWaza->SetVisible( false );
      }
      if( m_pCursorRibbon )
      {
        m_pCursorRibbon->SetVisible( false );
      }

      // ステータス画面の終了へ
      m_uiListener->EndStatus();

      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   準備シーケンス
 *
 * @return  "true  = 準備完了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool LowerMainDraw::Sequence_Ready( void )
{
  if( m_pPokeModel )
  {
    m_pPokeModel->Update();

    if( !m_pPokeModel->Initialize() ) return false;
  }

  if( m_pInfoWindow )
  {
    m_pInfoWindow->Update();

    if( !m_pInfoWindow->IsReady() ) return false;
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   フェードインシーケンス
 *
 * @return  "true  = フェードイン完了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool LowerMainDraw::Sequence_FadeIn( void )
{
  if( !IsOpen() ) return false;

  if( m_pPokeModel )
  {
    m_pPokeModel->Update();

    // ポケモンモデルの準備完了か？
    if( !m_pPokeModel->IsModelReady() ) return false;

    // ポケモンモデルの表示
    m_pPokeModel->SetVisibleModel( true );
  }

  // 入力を有効
  this->SetInputEnabled( true );

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンス
 */
//------------------------------------------------------------------------------
void LowerMainDraw::Sequence_Main( void )
{
  if( m_pCursorWaza )
  {
    m_pCursorWaza->Update();
  }

  if( m_pCursorRibbon )
  {
    m_pCursorRibbon->Update();
  }

  if( m_pBallPlate )
  {
    m_pBallPlate->Update();
  }

  if( m_pRibbon )
  {
    m_pRibbon->Update();
  }

  if( m_pPokeModel )
  {
    m_pPokeModel->Update();
  }

  if( m_pInfoWindow )
  {
    m_pInfoWindow->Update();
  }

  if( m_pWazaExchange )
  {
    m_pWazaExchange->Update();
  }

  // キャンセルボタンの更新
  UpdateCancelBtn();
}


//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void LowerMainDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  if( m_Seq > SEQ_IDLE )
  {
    app::util::G2DUtil* pG2D = this->GetG2DUtil();

    pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, LAYER_0 );

    if( m_Seq < SEQ_OUT )
    {
      if( m_pCursorWaza )
      {
        m_pCursorWaza->Draw( no );
      }
      if( m_pCursorRibbon )
      {
        m_pCursorRibbon->Draw( no );
      }
    }

    if( m_pInfoWindow )
    {
      m_pInfoWindow->Draw( no, gfl2::lyt::ORDER_NORMAL, LAYER_1 );
    }

    if( m_pRibbon )
    {
      m_pRibbon->Draw();
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   オープン
 */
//------------------------------------------------------------------------------
void LowerMainDraw::Open( void )
{
  // 開始位置の設定
  if( m_pBallPlate )
  {
    m_pBallPlate->SetBallIndex( m_pAppParam->poke_index, false );
  }

  // フェードイン
  if( m_pFadeManager )
  {
    m_pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, gfl2::Fade::FADE_DEFAULT_SYNC );
  }

  m_Seq = SEQ_IN;
}

//------------------------------------------------------------------------------
/**
 * @brief   クローズ
 */
//------------------------------------------------------------------------------
void LowerMainDraw::Close( void )
{
  // 入力を無効
  this->SetInputEnabled( false );

  // フェードアウト
  if( m_pFadeManager )
  {
    gfl2::math::Vector4 color( 0, 0, 0, 255 );

    m_pFadeManager->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
  }

  m_Seq = SEQ_OUT;
}

//------------------------------------------------------------------------------
/**
 * @brief   オープン中？
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool LowerMainDraw::IsOpen( void )
{
  if( m_pFadeManager )
  {
    return m_pFadeManager->IsEnd( gfl2::Fade::DISP_LOWER )
      && m_pFadeManager->IsEnd( gfl2::Fade::DISP_UPPER );
  }

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   ポケモンパラメータのセット
 *
 * @param   pp      表示する対象のポケモンパラメータ
 * @param   party   ポケパーティ
 * @param   index   ポケモンインデックス
 */
//------------------------------------------------------------------------------
void LowerMainDraw::SetPokemonParam( pml::pokepara::PokemonParam* pp, pml::PokeParty* party, u32 index )
{
  if( m_pLowInfo )
  {
    m_pLowInfo->Setup( pp, m_pAppParam->poke_lock[index] );
  }

  if( m_pBallPlate )
  {
    m_pBallPlate->Setup( party );
    m_pBallPlate->Setup_GetBall( pp );
  }

  if( m_pPokeModel )
  {
    m_pPokeModel->SetPokeModel( pp );
  }

  if( m_pInfoWindow )
  {
    m_pInfoWindow->Setup( pp );
  }

  if( m_pRibbon )
  {
    m_pRibbon->Setup( pp );
  }

  if( m_pWaza )
  {
    m_pWaza->Setup( pp );
  }

  if( m_pWazaCursor )
  {
    m_pWazaCursor->SetWazaCount( pp );
  }

  if( m_pWazaExchange )
  {
    m_pWazaExchange->Setup( pp );
  }

  // タブの変更処理
  ChangeTab( pp );
  // ボタンの設定
  SetButtonEnable( pp );

  // ポケモンパラメータのセット
  m_pPokeParam = pp;
}


//------------------------------------------------------------------------------
/**
 * @brief   タブのアクティブ
 */
//------------------------------------------------------------------------------
void LowerMainDraw::ActiveTab( void )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

  if(m_bBFCMode)
  {
    /*  BattleFesCircle  */
    InitializeTabForBFC();
  }
  else
  {
    if( btnMan )
    {
      btnMan->SetButtonActive( BTN_ID_TAB_WAZA );
      btnMan->SetButtonActive( BTN_ID_TAB_MEMO );

      // インフォボタンのセット
      SetButtonEnable_Info( btnMan, m_pPokeParam );
      // ボックスマークボタンのセット
      SetButtonEnable_BoxMark( btnMan );
    }

    if( m_pBallPlate )
    {
      m_pBallPlate->SetVisible( true );
    }

    m_pDrawerBase->SetVisiblePane( m_pPaneVisibleMark, true );
    m_pDrawerBase->SetVisiblePane( m_pPaneVisibleInfo, true );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   タブのパッシブ
 */
//------------------------------------------------------------------------------
void LowerMainDraw::PassiveTab( void )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

  if( btnMan )
  {
    btnMan->SetButtonPassive( BTN_ID_TAB_WAZA );
    btnMan->SetButtonPassive( BTN_ID_TAB_MEMO );

    btnMan->SetButtonInputEnable( BTN_ID_BOX_MARK, false );
    btnMan->SetButtonInputEnable( BTN_ID_INFO_TOKUSEI, false );
    btnMan->SetButtonInputEnable( BTN_ID_INFO_ITEM, false );
  }

  if( m_pBallPlate )
  {
    m_pBallPlate->SetVisible( false );
  }

  m_pDrawerBase->SetVisiblePane( m_pPaneVisibleMark, false );
  m_pDrawerBase->SetVisiblePane( m_pPaneVisibleInfo, false );
}


//------------------------------------------------------------------
/**
   * @brief   BattleFesCircle用のTab設定にする
  */
//------------------------------------------------------------------
void LowerMainDraw::InitializeTabForBFC( void )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

  if( btnMan )
  {
    /*  技/メモ切り替え  */
    btnMan->SetButtonPassive( BTN_ID_TAB_WAZA );
    btnMan->SetButtonPassive( BTN_ID_TAB_MEMO );

    /*  ボタン  */
    btnMan->SetButtonInputEnable( BTN_ID_BOX_MARK,      false );  /**<  mark      */
    btnMan->SetButtonInputEnable( BTN_ID_INFO_TOKUSEI,  true  );  /**<  [i]特性   */
    btnMan->SetButtonInputEnable( BTN_ID_INFO_ITEM,     true  );  /**<  [i]道具   */
  }

  /*  visibility  */
  if( m_pBallPlate )
  {
    m_pBallPlate->SetVisible( true );    /**<  ボール  */
  }
  m_pDrawerBase->SetVisiblePane( m_pPaneVisibleMark, false );   /**<  mark  */
  m_pDrawerBase->SetVisiblePane( m_pPaneVisibleInfo, true  );   /**<  info  */
}


//------------------------------------------------------------------------------
/**
 * @brief   技タブを選択
 */
//------------------------------------------------------------------------------
void LowerMainDraw::SelectWazaTab( void )
{
  if( m_PrevTab != TAB_MODE_WAZA )
  {
    // 技タブを選択
    m_pDrawerBase->StartAnim( LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSPAGEBTN_00_SELECT );

    // メモタブを非選択
    m_pDrawerBase->StartAnim( LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSPAGEBTN_01_UNSELECT );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メモタブを選択
 */
//------------------------------------------------------------------------------
void LowerMainDraw::SelectMemoTab( void )
{
  if( m_PrevTab != TAB_MODE_MEMO )
  {
    // 技タブを非選択
    m_pDrawerBase->StartAnim( LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSPAGEBTN_00_UNSELECT );
    // メモタブを選択
    m_pDrawerBase->StartAnim( LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSPAGEBTN_01_SELECT );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   タブの変更処理
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void LowerMainDraw::ChangeTab( const pml::pokepara::PokemonParam* pp )
{
  if( m_uiListener )
  {
    u8 tabMode = m_uiListener->GetTabMode();

    bool fastmode_flag = pp->StartFastMode();
    {
      bool egg = pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG );

      if( egg )
      {
        // タマゴの場合は強制的にタマゴ専用メモタブ
        m_uiListener->ChangeEggMemoTab();
        // リボンを非表示
        m_pDrawerBase->SetVisiblePane( m_pPaneVisibleRibbon, false ); // @fix NMCat[1578]

        // タマゴ用表示
        SetEggMode( true );

        m_PrevEggTab = true;
      }
      else if( tabMode == TAB_MODE_NUM )
      {
        // タマゴじゃなければ初期のタブは技タブ
        m_uiListener->ChangeWazaTab();

        m_PrevTab = TAB_MODE_WAZA;
      }
      // タマゴから別のポケモンに移るとき、前と同じタブに変更する
      else if( m_PrevEggTab )
      {
        // リボンを非表示
        m_pDrawerBase->SetVisiblePane( m_pPaneVisibleRibbon, true ); // @fix NMCat[1578]

        if( m_PrevTab == TAB_MODE_WAZA )
        {
          m_uiListener->ChangeWazaTab();
        }
        else {
          m_uiListener->ChangeMemoTab();
        }

        // タマゴ用表示からの復帰
        SetEggMode( false );
      }
      else {
        m_PrevTab = tabMode;
      }
    }
    pp->EndFastMode( fastmode_flag );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ポケモンインデックスのセット
 *
 * @param   index   ポケモンインデックス
 */
//------------------------------------------------------------------------------
void LowerMainDraw::SetPokeIndex( u32 index )
{
  if( m_pBallPlate )
  {
    m_pBallPlate->SetBallIndex( index, (m_Seq == SEQ_MAIN) );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   タマゴ専用モードの設定
 *
 * @param   flag    タマゴ専用モードフラグ(true = タマゴ専用、false = 通常モード)
 */
//------------------------------------------------------------------------------
void LowerMainDraw::SetEggMode( bool flag )
{
  if( m_pLowInfo )
  {
    // タマゴ専用モードの設定
    m_pLowInfo->SetEggMode( flag );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ボックスマークの更新
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void LowerMainDraw::UpdateBoxMark( const pml::pokepara::PokemonParam* pp )
{
  if( !m_UpdateProhibition )
  {
    if( m_pLowInfo )
    {
      // ボックスマークのセット
      m_pLowInfo->SetBoxMark( pp );
    }
  }

  // リボンリストの入力を有効
  m_pRibbon->SetInputEnable( true );
  // カーソルを非表示
  m_pRibbon->SetVisibleCursor( false );

  SetInputFlagEnable();  // @fix GFNMCat[1576]
}


//------------------------------------------------------------------------------
/**
 * @brief   技カーソルの位置を設定
 *
 * @param   pos       カーソル位置
 * @param   visFlag   表示切替を行うかどうか
 */
//------------------------------------------------------------------------------
void LowerMainDraw::SetWazaCursorPos( u8 pos, bool visFlag )
{
  if( m_pWazaCursor )
  {
    m_pWazaCursor->SetCursorPos( pos, visFlag );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   次の技へ
 */
//------------------------------------------------------------------------------
void LowerMainDraw::NextWazaCursor( void )
{
  if( m_pWazaCursor )
  {
    u8 nowPos = m_pWazaCursor->GetCursorPos();
    u8 curPos = m_pWazaCursor->NextWazaIndex();

    if( nowPos != curPos )
    {
      WazaNo wazaNo = m_pWazaExchange->GetWazaNo( curPos );
      {
        // 表示する技を変更
        m_uiListener->SetWazaExplain( wazaNo );
      }

      // 移動音
      Sound::PlaySE( SEQ_SE_SELECT1 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   前の技へ
 */
//------------------------------------------------------------------------------
void LowerMainDraw::PrevWazaCursor( void )
{
  if( m_pWazaCursor )
  {
    u8 nowPos = m_pWazaCursor->GetCursorPos();
    u8 curPos = m_pWazaCursor->PrevWazaIndex();

    if( nowPos != curPos )
    {
      WazaNo wazaNo = m_pWazaExchange->GetWazaNo( curPos );
      {
        // 表示する技を変更
        m_uiListener->SetWazaExplain( wazaNo );
      }

      // 移動音
      Sound::PlaySE( SEQ_SE_SELECT1 );
    }
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   技入れ替えの開始
 */
//------------------------------------------------------------------------------
void LowerMainDraw::StartWazaExchange( void )
{
  u8 curPos = 0;

  if( m_pWazaCursor )
  {
    curPos = m_pWazaCursor->GetCursorPos();
  }

  if( m_pWazaExchange )
  {
    m_pWazaExchange->SetPlate_Init( curPos );
  }

  if( m_pMsgWindow )
  {
    m_pMsgWindow->Open( PARAM_WAZA_CHANGE_01 );
  }

  if( m_pBallPlate )
  {
    m_pBallPlate->SetVisible( false );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   技入れ替えの終了
 */
//------------------------------------------------------------------------------
void LowerMainDraw::EndWazaExchange( void )
{
  if( m_pBallPlate )
  {
    m_pBallPlate->SetVisible( true );
  }

  if( m_pWazaExchange )
  {
    m_pWazaExchange->End();
  }

  if( m_pWazaExchange )
  {
    m_pMsgWindow->Close();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   キャンセルボタンの更新
 */
//------------------------------------------------------------------------------
// @fix GFNMCat[2100]
void LowerMainDraw::UpdateCancelBtn( void )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    bool isDrag = false;

    if( m_pWazaExchange )   // @fix cov_ctr[10132]
    {
      isDrag = m_pWazaExchange->IsPlateDrag();
    }

    if( btnMan->IsButtonActive( BTN_ID_CANCEL ) )
    {
      if( isDrag )
      {
        btnMan->SetButtonPassive( BTN_ID_CANCEL );
      }
    }
    else {
      if( !isDrag )
      {
        btnMan->SetButtonActive( BTN_ID_CANCEL );
      }
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   リボンリストを表示
 */
//------------------------------------------------------------------------------
void LowerMainDraw::OpenRibbonList( void )
{
  if( m_pRibbon )
  {
    m_pRibbon->Open();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リボンリストを非表示
 */
//------------------------------------------------------------------------------
void LowerMainDraw::CloseRibbonList( void )
{
  if( m_pRibbon )
  {
    m_pRibbon->Close();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リボンリストのアナログキーの入力設定
 *
 * @param   enable    入力フラグ
 */
//------------------------------------------------------------------------------
void LowerMainDraw::InputRibbonKeyEnabled( bool enable )
{
  if( m_pRibbon )
  {
    m_pRibbon->SetInputKeyEnabled( enable );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リボンIDの取得
 *
 * @param   index   インデックス
 *
 * @return  リボンID
 */
//------------------------------------------------------------------------------
u32 LowerMainDraw::GetRibbonID( u32 index ) const
{
  if( m_pRibbon )
  {
    return m_pRibbon->GetRibbonID( index );
  }

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief   最後に選択していたリボンIDの取得
 *
 * @return  リボンID
 */
//------------------------------------------------------------------------------
u32 LowerMainDraw::GetLastRibbonID( void ) const
{
  if( m_pRibbon )
  {
    return m_pRibbon->GetLastRibbonID();
  }

  return 0;
}

  
//------------------------------------------------------------------------------
/**
 * @brief   インフォメーションウィンドウを開く(特性)
 */
//------------------------------------------------------------------------------
void LowerMainDraw::OpenInfoWindow_Tokusei( void )
{
  if( m_pInfoWindow )
  {
    // 特性のインフォメーションウィンドウを開く
    m_pInfoWindow->OpenTokusei();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   インフォメーションウィンドウを開く(アイテム)
 */
//------------------------------------------------------------------------------
void LowerMainDraw::OpenInfoWindow_Item( void )
{
  if( m_pInfoWindow )
  {
    // アイテムのインフォメーションウィンドウを開く
    m_pInfoWindow->OpenItem();
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージウィンドウを開く
 *
 * @param   msgID         メッセージID
 * @param   inputEnalbe   入力フラグを変更するかどうか(true = 変更、false = 変更しない)
 */
//------------------------------------------------------------------------------
void LowerMainDraw::OpenMessageWindow( u32 msgID, bool inputEnable )
{
  if( m_pMsgWindow )
  {
    m_pMsgWindow->Open( msgID, inputEnable );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージウィンドウを閉じる
 */
//------------------------------------------------------------------------------
void LowerMainDraw::CloseMessageWindow( void )
{
  if( m_pMsgWindow )
  {
    m_pMsgWindow->Close();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージウィンドウが閉じきったかどうか
 *
 * @return  "true  == 閉じきった"
 * @return  "false == それ以外"
 */
//------------------------------------------------------------------------------
bool LowerMainDraw::IsClosedMessageWindow( void ) const
{
  if( m_pMsgWindow )
  {
    return m_pMsgWindow->IsClosed();
  }

  return true;
}





//==============================================================================
/**
 *      ↓　以下がリスナー処理　↓
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   リソース生成前に必要なデータを登録。フォントリソースなど
 */
//------------------------------------------------------------------------------
void LowerMainDraw::OnPreLoadResource( void )
{
  if( !print::SystemFont_GetNumFont() )
  {
    print::SystemFont_CreateNumFont( m_pHeap->GetDeviceHeap(), m_pHeap->GetDeviceHeap() );
    print::SystemFont_SetLytSystemNumFont();
    m_createNumFont = true;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインタッチ時通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
void LowerMainDraw::OnLayoutPaneTouchTriggerEvent( const u32 paneId )
{
  if( m_Seq >= SEQ_OUT ) return;

  switch( paneId )
  {
  case BTN_ID_WAZA_00:
  case BTN_ID_WAZA_01:
  case BTN_ID_WAZA_02:
  case BTN_ID_WAZA_03:
    {
      u8 mode = m_uiListener->GetDispMode();

      if( mode == DISP_MODE_WAZA || mode == DISP_MODE_WAZA_EXCHANGE )
      {
        u32 plateIndex = m_pWazaExchange->GetPlateIndex_Now( paneId - BTN_ID_WAZA_00 );

        // 技Noの取得
        WazaNo wazaNo  = m_pWazaExchange->GetWazaNo( plateIndex );

        // 説明する技の変更
        m_uiListener->SetWazaExplain( wazaNo );

        // 技カーソルの設定
        SetWazaCursorPos( plateIndex );
      }
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult LowerMainDraw::OnLayoutPaneEvent( const u32 paneId )
{
  if( m_uiListener == NULL ) return ENABLE_ACCESS;
  if( m_Seq >= SEQ_OUT ) return ENABLE_ACCESS;

  switch( paneId )
  {
  case BTN_ID_CANCEL:
    {
      if( m_uiListener->GetDispMode() == DISP_MODE_STATUS )
      {
        // 入力を無効
        this->SetInputEnabled( false );

        // キャンセルボタンの押下
        m_uiListener->OnCancelBtn();
      }
      else if( m_uiListener->GetDispMode() != DISP_MODE_WAZA_EXCHANGE )
      {
        if( m_uiListener->GetDispMode() == DISP_MODE_MEMO )
        {
          // @fix NMCat[1517]
          m_pRibbon->SetInputEnable( true );
        }

        // キャンセルボタンの押下
        m_uiListener->OnCancelBtn();

        return DISABLE_ACCESS;
      }
      else {
        if( m_pWazaExchange )
        {
          m_pWazaExchange->Reset();
        }
        if( m_pMsgWindow )
        {
          m_pMsgWindow->SetMessageID( PARAM_WAZA_CHANGE_01 );
        }
        m_uiListener->SetDispMode( DISP_MODE_WAZA );
      }
    } break;

  case BTN_ID_TAB_WAZA:
    {
      if( m_uiListener->GetTabMode() != TAB_MODE_EGG )
      {
        // タブの変更(わざ)
        if( m_uiListener->ChangeWazaTab() )
        {
          m_PrevTab = TAB_MODE_WAZA;
        }
      }
    } break;

  case BTN_ID_TAB_MEMO:
    {
      if( m_uiListener->GetTabMode() != TAB_MODE_EGG )
      {
        // タブの変更(メモ)
        if( m_uiListener->ChangeMemoTab() )
        {
          m_PrevTab = TAB_MODE_MEMO;
        }
      }
    } break;

  case BTN_ID_BOX_MARK:
    {
      // ボックスマーク変更画面を開く
      m_uiListener->OpenBoxMark( m_UpdateProhibition );
      //if( !m_UpdateProhibition )
      //{
      //  // ボックスマーク変更画面を開く
      //  m_uiListener->OpenBoxMark();
      //}
      //else {
      //  // メッセージの表示
      //  OpenMessageWindow( PARAM_TEXT_01, true );
      //  // メッセージ表示モードへ変更
      //  m_uiListener->SetDispMode( DISP_MODE_MSG_WND );
      //}
    } break;

  case BTN_ID_INFO_TOKUSEI:
    {
      // インフォメーションウィンドウ(特性)を開く
      OpenInfoWindow_Tokusei();
    } break;

  case BTN_ID_INFO_ITEM:
    {
      // インフォメーションウィンドウ(アイテム)を開く
      OpenInfoWindow_Item();
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
app::ui::UIInputListener::ListenerResult LowerMainDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ) return ENABLE_ACCESS;
  if( m_Seq >= SEQ_OUT )     return ENABLE_ACCESS;

  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

  u8 dispMode = m_uiListener->GetDispMode();

  switch( dispMode )
  {
  case DISP_MODE_STATUS:
    {
      if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
      {
        m_uiListener->OnDecideBtn();
      }
      else if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
      {
        btnMan->StartSelectedAct( BTN_ID_CANCEL );
      }
      else if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
      {
        if( m_uiListener->GetTabMode() == TAB_MODE_WAZA )
        {
          m_uiListener->ChangeRadarChartMode();
        }
      }
      else if( pButton->IsRepeat( gfl2::ui::BUTTON_UP ) )
      {
        if( GetInputFlag() )
        {
          // 前のポケモンへ
          m_uiListener->PrevPokemon();
        }
      }
      else if( pButton->IsRepeat( gfl2::ui::BUTTON_DOWN ) )
      {
        if( GetInputFlag() )
        {
          // 次のポケモンへ
          m_uiListener->NextPokemon();
        }
      }
      else if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) || pButton->IsTrigger( gfl2::ui::BUTTON_L ) )
      {
        if(m_uiListener->GetTabMode() == TAB_MODE_MEMO)
        {
          btnMan->StartSelectedAct( BTN_ID_TAB_WAZA );
        }
      }
      else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) || pButton->IsTrigger( gfl2::ui::BUTTON_R ) )
      {
        if( m_uiListener->GetTabMode() == TAB_MODE_WAZA )
        {
          btnMan->StartSelectedAct( BTN_ID_TAB_MEMO );
        }
      }
    } break;

  case DISP_MODE_WAZA:
  case DISP_MODE_WAZA_EXCHANGE:
    {
      if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
      {
        if( m_UpdateProhibition ) break;

        if( m_pWazaCursor )
        {
          u8 curPos = m_pWazaCursor->GetCursorPos();

          // ボタンアクションの開始
          u32 initPos = m_pWazaExchange->GetPlateIndex_Init( curPos );
          btnMan->StartSelectedAct( BTN_ID_WAZA_00 + initPos );

          // 技入れ替えの開始
          m_pWazaExchange->SetPlate_Now( curPos );  //!< @note ボタンのクリック処理では処理してほしくないのでここで

          if( dispMode == DISP_MODE_WAZA )
          {
            m_uiListener->SetDispMode( DISP_MODE_WAZA_EXCHANGE );
          }
        }
      }
      else if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
      {
        btnMan->StartSelectedAct( BTN_ID_CANCEL );
      }
      else if( pButton->IsRepeat( gfl2::ui::BUTTON_UP ) )
      {
        // 前の技へ
        PrevWazaCursor();
      }
      else if( pButton->IsRepeat( gfl2::ui::BUTTON_DOWN ) )
      {
        // 次の技へ
        NextWazaCursor();
      }
    } break;

  case DISP_MODE_MSG_WND:
    {
      // メッセージウィンドウを閉じる
      CloseMessageWindow();

      m_uiListener->SetDispMode( DISP_MODE_STATUS );
    } break;

  default:
    {
      if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
      {
        m_uiListener->OnDecideBtn();
      }
      else if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
      {
        if( m_pRibbon->GetUpdateState() == app::tool::ScrollPaneManager::STATE_INPUT_WAIT )
        {
          m_pRibbon->SetInputEnable( false );

          btnMan->StartSelectedAct( BTN_ID_CANCEL );
        }
      }
    }
  }

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチパネルイベントの検知
 * @param   pTouchPanel
 * @param   isTouch
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult LowerMainDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( m_uiListener == NULL ) return ENABLE_ACCESS;
  if( m_Seq >= SEQ_OUT ) return ENABLE_ACCESS;

  u8 dispMode = m_uiListener->GetDispMode();

  // ポケモンモデルのタッチアニメの再生
  if( m_pPokeModel )
  {
    m_pPokeModel->CheckTouchModel( pTouchPanel );
  }

  if( dispMode == DISP_MODE_STATUS )
  {
    if( GetInputFlag() )
    {
      if( m_pBallPlate )
      {
        m_pBallPlate->TouchBall( pTouchPanel );
      }
    }

    if( m_pWazaExchange )
    {
      if( !GetInputFlag() )
      {
        return ENABLE_ACCESS;
      }
      if( !m_uiListener->IsWazaExplainClosed() )
      {
        return ENABLE_ACCESS;
      }

      u32 plateIndex = 4;

      if( !m_UpdateProhibition )
      {
        //plateIndex = m_pWazaExchange->SetTouchPanel( pTouchPanel );
        plateIndex = m_pWazaExchange->TouchEvent( pTouchPanel );
      }
      else {
        if( pTouchPanel->IsTouchTrigger() )
        {
          plateIndex = m_pWazaExchange->GetTouchPlateIndex( pTouchPanel );
        }
      }

      if( plateIndex < 4 )
      {
        // タブをパッシブ
        PassiveTab();

        // カーソル位置の設定
        m_pWazaCursor->SetCursorPos( plateIndex, false );

        // 技説明の表示
        m_uiListener->OpenWazaExplain( plateIndex );
      }
    }

    return DISABLE_ACCESS;
  }
  else if( dispMode == DISP_MODE_WAZA || dispMode == DISP_MODE_WAZA_EXCHANGE )
  {
    if( !m_UpdateProhibition )
    {
      if( m_pWazaExchange )
      {
        //m_pWazaExchange->SetTouchPanel( pTouchPanel );
        m_pWazaExchange->TouchEvent( pTouchPanel );

        return DISABLE_ACCESS;
      }
    }
  }
  else if( dispMode == DISP_MODE_MSG_WND )
  {
    // メッセージウィンドウを閉じる
    CloseMessageWindow();

    m_uiListener->SetDispMode( DISP_MODE_STATUS );
  }

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   ペイン選択アニメーションの開始の検知
 * @param   paneId
 */
//------------------------------------------------------------------------------
void LowerMainDraw::OnLayoutPaneStartingAnimeEvent( const u32 paneId )
{
  switch( paneId )
  {
  case BTN_ID_CANCEL:
    {
      u8 dispMode = m_uiListener->GetDispMode();
      {
        if( dispMode == DISP_MODE_STATUS )
        {
          // リボンリストの入力を無効
          m_pRibbon->SetInputEnable( false ); // @fix NMCat[1246]

          SetInputFlagDisable();  // @fix NMCat[235]
        }
        else if( dispMode == DISP_MODE_WAZA )
        {
          app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
          {
            btnMan->SetButtonInputEnable( BTN_ID_WAZA_00, false );
            btnMan->SetButtonInputEnable( BTN_ID_WAZA_01, false );
            btnMan->SetButtonInputEnable( BTN_ID_WAZA_02, false );
            btnMan->SetButtonInputEnable( BTN_ID_WAZA_03, false );
          }
        }
        else if( dispMode == DISP_MODE_MEMO )
        {
          // @fix NMCat[1517]
          InputRibbonKeyEnabled( false );
          // @fix NMCat[1517]
          m_pRibbon->SetInputEnable( false );
        }
      }
    } break;

  case BTN_ID_BOX_MARK:
    {
      // リボンリストの入力を無効
      m_pRibbon->SetInputEnable( false );

      SetInputFlagDisable();  // @fix GFNMCat[1576]
    } break;

  case BTN_ID_INFO_ITEM:
  case BTN_ID_INFO_TOKUSEI:
    {
      SetInputFlagDisable();
    } break;
  }
}





//==============================================================================
/**
 *      ↓　以下が情報(下)リスナー関連　↓
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   道具インフォメーションボタンの入力切替
 *
 * @param   enable    入力フラグ(true = 有効、false = 無効)
 */
//------------------------------------------------------------------------------
void LowerMainDraw::SetInputEnableItemInfoBtn( bool enable )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

  // アイテムのインフォメーションボタンの入力切替
  btnMan->SetButtonInputEnable( BTN_ID_INFO_ITEM, enable );
}





//==============================================================================
/**
 *      ↓　以下がボールプレート(下)リスナー関連　↓
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   タッチされたボールのインデックスを設定
 *
 * @param   index   タッチされたボールのインデックス
 */
//------------------------------------------------------------------------------
void LowerMainDraw::SetTouchedBallIndex( u32 index )
{
  if( m_uiListener )
  {
    // タッチされたポケモンに変更
    m_uiListener->SetPokemon( index );
  }
}





//==============================================================================
/**
 *      ↓　以下が技(下)リスナー関連　↓
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   説明画面の表示を更新
 *
 * @param   wazaNo    技No
 */
//------------------------------------------------------------------------------
void LowerMainDraw::SetWazaExplain( WazaNo wazaNo )
{
  if( m_uiListener )
  {
    m_uiListener->SetWazaExplain( wazaNo );
  }
}





//==============================================================================
/**
 *      ↓　以下がインフォメーションウィンドウリスナー関連　↓
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   閉じ終わった
 */
//------------------------------------------------------------------------------
void LowerMainDraw::OnClose( void )
{
  this->SetInputEnabled( true );

  SetInputFlagEnable();
}





//==============================================================================
/**
 *      ↓　以下がインフォメーションウィンドウリスナー関連　↓
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   タブのパッシブ
 */
//------------------------------------------------------------------------------
void LowerMainDraw::TabPassive( void )
{
  this->PassiveTab();
}

//------------------------------------------------------------------------------
/**
 * @brief   キャンセルボタンの有効設定
 *
 * @param   enable    有効フラグ
 */
//------------------------------------------------------------------------------
void LowerMainDraw::SetCancelBtnEnable( bool enable )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    if( enable )
    {
      btnMan->SetButtonActive( BTN_ID_CANCEL );
    }
    else {
      btnMan->SetButtonPassive( BTN_ID_CANCEL );
    }
  }
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
void LowerMainDraw::CreateButtonManager( void )
{
  const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_CANCEL, NULL, NULL,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BACK_TOUCH,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BACK_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BACK_CANSEL,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BACK_TOUCH_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BACK_ACTIVE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BACK_PASSIVE,
    },
    {
      BTN_ID_TAB_WAZA, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSPAGEBTN_00_ACTIVE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSPAGEBTN_00_PASSIVE,
    },
    {
      BTN_ID_TAB_MEMO, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSPAGEBTN_01_ACTIVE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSPAGEBTN_01_PASSIVE,
    },
    {
      BTN_ID_BOX_MARK, NULL, NULL,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MARKPLT_TOUCH,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MARKPLT_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MARKPLT_CANSEL,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MARKPLT_TOUCH_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MARKPLT_ACTIVE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MARKPLT_PASSIVE,
    },
    {
      BTN_ID_INFO_TOKUSEI, NULL, NULL,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_00_TOUCH,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_00_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_00_CANSEL,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_00_TOUCH_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_00_ACTIVE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_00_PASSIVE,
    },
    {
      BTN_ID_INFO_ITEM, NULL, NULL,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_01_TOUCH,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_01_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_01_CANSEL,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_01_TOUCH_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_01_ACTIVE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_01_PASSIVE,
    },
    {
      BTN_ID_WAZA_00, NULL, NULL,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_00_TOUCH,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_00_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_00_CANSEL,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_00_TOUCH_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_00_ACTIVE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_00_PASSIVE,
    },
    {
      BTN_ID_WAZA_01, NULL, NULL,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_01_TOUCH,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_01_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_01_CANSEL,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_01_TOUCH_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_01_ACTIVE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_01_PASSIVE,
    },
    {
      BTN_ID_WAZA_02, NULL, NULL,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_02_TOUCH,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_02_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_02_CANSEL,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_02_TOUCH_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_02_ACTIVE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_02_PASSIVE,
    },
    {
      BTN_ID_WAZA_03, NULL, NULL,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_03_TOUCH,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_03_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_03_CANSEL,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_03_TOUCH_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_03_ACTIVE,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_03_PASSIVE,
    },
  };
  const gfl2::lyt::LytPaneIndex c_BtnPaneIndex[BTN_ID_NUM][2] = {
    { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTS_BACK,       PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00 },
    { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSPAGEBTN_00,  PANENAME_STATUS_BTN_LOW_001_PANE_BOUND_00 },
    { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSPAGEBTN_01,  PANENAME_STATUS_BTN_LOW_001_PANE_BOUND_00 },
    { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTS_MARKPLT,    PANENAME_COMMON_PLT_LOW_003_PANE_BOUND_00 },
    { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_INFO_00,          PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00 },
    { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_INFO_01,          PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00 },
    { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_00, PANENAME_STATUS_PLT_LOW_000_PANE_BOUND_00 },
    { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_01, PANENAME_STATUS_PLT_LOW_000_PANE_BOUND_00 },
    { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_02, PANENAME_STATUS_PLT_LOW_000_PANE_BOUND_00 },
    { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_03, PANENAME_STATUS_PLT_LOW_000_PANE_BOUND_00 },
  };

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();
  app::ui::ButtonInfoEx* info = GFL_NEW_LOW_ARRAY(devHeap) app::ui::ButtonInfoEx[BTN_ID_NUM];

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    gfl2::lyt::LytParts* parts = m_pDrawerBase->GetParts( c_BtnPaneIndex[i][0] );

    info[i] = c_DefInfo[i];
    info[i].picture_pane = parts;
    info[i].bound_pane   = m_pDrawerBase->GetPane( c_BtnPaneIndex[i][1], parts );
  }

  gfl2::lyt::LytWk* lytwk = m_pDrawerBase->GetLytWk();
  m_pDrawerBase->GetUIView()->CreateButtonManager( m_pHeap, lytwk, info, BTN_ID_NUM );
  {
    app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

    //btnMan->SetButtonBindKey( BTN_ID_CANCEL, gfl2::ui::BUTTON_B );

    for( u32 i=0; i<BTN_ID_NUM; ++i )
    {
      if( i == BTN_ID_INFO_TOKUSEI ) continue;
      if( i == BTN_ID_INFO_ITEM )    continue;

      u32 seid = (i == BTN_ID_CANCEL) ? SEQ_SE_CANCEL1 : SEQ_SE_DECIDE1;

      btnMan->SetButtonSelectSE( i, seid );
    }
  }

  GFL_DELETE_ARRAY info;
}


//------------------------------------------------------------------------------
/**
 * @brief   ボタンの設定
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void LowerMainDraw::SetButtonEnable( const pml::pokepara::PokemonParam* pp )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    bool fastmode = pp->StartFastMode();
    {
      // タブボタン
      SetButtonEnable_Tab( btnMan );
      // 技ボタン
      SetButtonEnable_Waza( btnMan, pp );
      // インフォボタン
      SetButtonEnable_Info( btnMan, pp );
      // ボックスマークボタン
      SetButtonEnable_BoxMark( btnMan );
    }
    pp->EndFastMode( fastmode );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンの設定：タブボタン
 *
 * @param   btnMan    ボタンマネージャ
 */
//------------------------------------------------------------------------------
void LowerMainDraw::SetButtonEnable_Tab( app::tool::ButtonManager* btnMan )
{
  u8 tabMode = m_uiListener->GetTabMode();
  {
    btnMan->SetButtonInputEnable( BTN_ID_TAB_WAZA, (tabMode == TAB_MODE_MEMO) );  //!< メモタブを選択中のみ有効
    btnMan->SetButtonInputEnable( BTN_ID_TAB_MEMO, (tabMode == TAB_MODE_WAZA) );  //!< 技タブを選択中のみ有効
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンの設定：技ボタン
 *
 * @param   btnMan    ボタンマネージャ
 * @param   pp        ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void LowerMainDraw::SetButtonEnable_Waza( app::tool::ButtonManager* btnMan, const pml::pokepara::PokemonParam* pp )
{
  u8 wazaCount = 0;
  {
    // 技の所持数を取得
    wazaCount = pp->GetWazaCount();
  }

  for( u8 i=0; i<4; ++i )
  {
    bool enable = (i<wazaCount);    //!< 基本判定：所持数を超えなければ有効
    {
      // タブが技タブでなければ無効に
      if( m_uiListener->GetTabMode() != TAB_MODE_WAZA )
      {
        enable = false;
      }
    }

    btnMan->SetButtonInputEnable( BTN_ID_WAZA_00+i, enable );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンの設定：インフォボタン
 *
 * @param   btnMan    ボタンマネージャ
 */
//------------------------------------------------------------------------------
void LowerMainDraw::SetButtonEnable_Info( app::tool::ButtonManager* btnMan, const pml::pokepara::PokemonParam* pp )
{
  bool enable = true;
  {
    // タブが技タブでなければ無効に
    if( m_uiListener->GetTabMode() != TAB_MODE_WAZA )
    {
      enable = false;
    }
    // 画面モードが通常以外であれば無効に
    if( m_uiListener->GetDispMode() != DISP_MODE_STATUS )
    {
      enable = false;
    }
  }

  // 特性インフォボタン
  btnMan->SetButtonInputEnable( BTN_ID_INFO_TOKUSEI, enable );

  u16 itemNo = ITEM_DUMMY_DATA;
  {
    // 所持アイテムの取得
    itemNo = pp->GetItem();
  }
  // アイテムインフォボタン
  btnMan->SetButtonInputEnable( BTN_ID_INFO_ITEM, (enable && itemNo != ITEM_DUMMY_DATA) );
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンの設定：ボックスマークボタン
 *
 * @param   btnMan    ボタンマネージャ
 */
//------------------------------------------------------------------------------
void LowerMainDraw::SetButtonEnable_BoxMark( app::tool::ButtonManager* btnMan )
{
  bool enable = true;
  {
    // 画面モードが通常以外であれば無効に
    if( m_uiListener->GetDispMode() != DISP_MODE_STATUS )
    {
      enable = false;
    }
  }

  // ボックスマークボタン
  btnMan->SetButtonInputEnable( BTN_ID_BOX_MARK, enable );
}

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)
