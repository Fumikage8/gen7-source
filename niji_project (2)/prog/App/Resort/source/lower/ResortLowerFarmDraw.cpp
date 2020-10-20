//======================================================================
/**
 * @file    ResortLowerFarmDraw.cpp
 * @date    2015/10/01 20:17:32
 * @author  fukushima_yuya
 * @brief   ポケリゾート：はたけのようす画面(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortLowerFarmDraw.h"

#include <App/Resort/source/ResortDrawerBase.h>
#include <App/Resort/source/ResortDrawListener.h>
#include <App/Resort/source/ResortConst.h>
#include <App/Resort/source/ResortUtility.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <sound/include/Sound.h>

// arc
#include <niji_conv_header/app/resort/resort.h>
#include <niji_conv_header/app/resort/resort_pane.h>
#include <niji_conv_header/app/resort/resort_anim_list.h>
#include <niji_conv_header/message/msg_pokeresort_system.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  enum {
    BTN_ID_CANCEL,
    BTN_ID_DECIDE,
    BTN_ID_NUM,
  };
  
  static const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_CANCEL, NULL, NULL,
      LA_RESORT_RESORT_KINOMI_LOW_000__BACKBUTTON_TOUCH,
      LA_RESORT_RESORT_KINOMI_LOW_000__BACKBUTTON_RELEASE,
      LA_RESORT_RESORT_KINOMI_LOW_000__BACKBUTTON_CANSEL,
      LA_RESORT_RESORT_KINOMI_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_KINOMI_LOW_000__BACKBUTTON_ACTIVE,
      LA_RESORT_RESORT_KINOMI_LOW_000__BACKBUTTON_PASSIVE,
    },
    {
      BTN_ID_DECIDE, NULL, NULL,
      LA_RESORT_RESORT_KINOMI_LOW_000__SELECTBUTTON_TOUCH,
      LA_RESORT_RESORT_KINOMI_LOW_000__SELECTBUTTON_RELEASE,
      LA_RESORT_RESORT_KINOMI_LOW_000__SELECTBUTTON_CANSEL,
      LA_RESORT_RESORT_KINOMI_LOW_000__SELECTBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_KINOMI_LOW_000__SELECTBUTTON_ACTIVE,
      LA_RESORT_RESORT_KINOMI_LOW_000__SELECTBUTTON_PASSIVE,
    },
  };
  
  static const u32 c_ButtonBaseIndex[BTN_ID_NUM] = {
    PANENAME_RESORT_KINOMI_LOW_000_PANE_BACKBUTTON,
    PANENAME_RESORT_KINOMI_LOW_000_PANE_SELECTBUTTON,
  };
  
  static const u32 c_ButtonBoundIndex[BTN_ID_NUM] = {
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_021_PANE_BOUND_00,
  };
}


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   param   生成パラメータ
 */
//------------------------------------------------------------------------------
LowerFarmDraw::LowerFarmDraw( CreateParam& param )
  : LowerBaseDraw( param )
  , m_KinomiIndex(0)
  , m_pItemName(NULL)
  , m_pTimeMsg(NULL)
  , m_pStateMsg(NULL)
  , m_pIconVisPane(NULL)
  , m_pKinomiPic(NULL)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void LowerFarmDraw::Initialize2D( InitParam& param )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { param.lytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_RESORT_RESORT_KINOMI_LOW_000_BFLYT,
      LA_RESORT_RESORT_KINOMI_LOW_000___NUM,
      param.lytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  // 2Dの生成
  Create2D(
    m_pLytHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    param.msgSys,
    param.wordSet );

  this->SetVisible( false );
  this->SetInputEnabled( false );
}

//------------------------------------------------------------------------------
/**
 * @brief   クラスの初期化
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void LowerFarmDraw::InitializeClass( InitParam& param )
{
  gfl2::heap::HeapBase* heap = m_pLytHeap->GetDeviceHeap();
  {
    // 描画共通処理
    m_pDrawerBase = GFL_NEW(heap) DrawerBase( this, m_pLytHeap, 0, param.wordSet, param.msgSys );
    {
      GFL_ASSERT( m_pDrawerBase );
    }
    
    // ボタンマネージャの生成
    CreateButtonManager( c_DefInfo, c_ButtonBaseIndex, c_ButtonBoundIndex, BTN_ID_NUM );

    // アイテムアイコン
    app::tool::ItemIconToolEx::ItemIconToolParam iconParam;
    {
      iconParam.pUIView = this;
      iconParam.pHeap   = m_pIconHeap;
      iconParam.iconMax = 1;
      iconParam.sync    = true;
      iconParam.allocBuffer = true;
    }
    m_pDrawerBase->CreateItemIcon( iconParam );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインの初期化
 */
//------------------------------------------------------------------------------
void LowerFarmDraw::InitializePane( void )
{
  m_pItemName     = m_pDrawerBase->GetTextBox( PANENAME_RESORT_KINOMI_LOW_000_PANE_TEXTBOX_00 );
  m_pTimeMsg      = m_pDrawerBase->GetTextBox( PANENAME_RESORT_KINOMI_LOW_000_PANE_TEXTBOX_01 );
  m_pStateMsg     = m_pDrawerBase->GetTextBox( PANENAME_RESORT_KINOMI_LOW_000_PANE_TEXTBOX_02 );
  m_pIconVisPane  = m_pDrawerBase->GetPane( PANENAME_RESORT_KINOMI_LOW_000_PANE_VISIBLE_KINOMI );
  m_pKinomiPic    = m_pDrawerBase->GetPicture( PANENAME_RESORT_KINOMI_LOW_000_PANE_ITEMICON_00 );
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト
 */
//------------------------------------------------------------------------------
void LowerFarmDraw::InitializeText( void )
{
  // はたけのようす
  gfl2::lyt::LytTextBox* title = m_pDrawerBase->GetTextBox( PANENAME_RESORT_KINOMI_LOW_000_PANE_HEADDER_MAIN );
  {
    this->SetTextboxPaneMessage( title, msg_prsys_low_02_01 );
  }

  // うえる
  gfl2::lyt::LytParts* btnParts = m_pDrawerBase->GetParts( PANENAME_RESORT_KINOMI_LOW_000_PANE_SELECTBUTTON );
  {
    gfl2::lyt::LytTextBox* btnMsg = m_pDrawerBase->GetTextBox( PANENAME_COMMON_BTN_LOW_021_PANE_TEXTBOX_00, btnParts );
    {
      this->SetTextboxPaneMessage( btnMsg, msg_prsys_button_02 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void LowerFarmDraw::Update( void )
{
  app::ui::UIView::Update();

  // 時間の更新
  UpdateTime();
}


//------------------------------------------------------------------------------
/**
 * @brief   時間表示の更新
 */
//------------------------------------------------------------------------------
void LowerFarmDraw::UpdateTime( void )
{
  if( m_pResortSave == NULL ) return;

  if( m_pResortSave->GetFarmNum(m_KinomiIndex) != 0 )
  {
    // --じかん--ふんを表示
    this->SetTextboxPaneMessage( m_pTimeMsg, msg_prsys_common_07 );

    nw::ut::Color8 color = 0x961C00FF;
    {
      m_pTimeMsg->SetTextColor( color, color );
    }
  }
  else {
    TimeParam time;
    {
      m_pResortSave->GetFarmTime( m_KinomiIndex, &time.hour, &time.min, &time.sec );

      //// １分以上ならタイムを表示
      //if( time.GetTotalSec() >= 60 )
      if( time.GetTotalSec() > 60 )
      {
        util::SetTextBoxTime( this, m_pDrawerBase->GetWordSet(), m_pTimeMsg, time );
      }
      // １分未満なら「あとちょっと！」を表示
      else {
        this->SetTextboxPaneMessage( m_pTimeMsg, msg_prsys_temp05 );
      }
    }

    // 色変更
    {
      TimeParam bonusTime;
      {
        m_pResortSave->GetFarmFertilizerTime( &bonusTime.hour, &bonusTime.min, &bonusTime.sec );
      }

      nw::ut::Color8 color = 0x961C00FF;
      {
        if( !bonusTime.IsInvalid() && bonusTime.GetTotalSec() > 0 )
        {
          color = 0x1987E5FF; // シアン：RGBA(25:135:22:255)
        }

        m_pTimeMsg->SetTextColor( color, color );
      }
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   開く
 */
//------------------------------------------------------------------------------
void LowerFarmDraw::Open( void )
{
  // はたけの状態をセットアップ
  SetupFarmState();

  // リザルトの初期化
  this->InitResult();

  this->SetVisible( true );
  this->SetInputEnabled( true );
}

//------------------------------------------------------------------------------
/**
 * @brief   畑の状態を取得
 *
 * @param   time    きのみの生育状態
 *
 * @return  畑の状態
 */
//------------------------------------------------------------------------------
LowerFarmDraw::FarmState LowerFarmDraw::GetFarmState( TimeParam time )
{
  // 生育時間で状態を判定
  if( time.GetTotalSec() > 0 )
  {
    // @fix GFNMCat[5603]：条件を以下から未満に変更
    if( time.hour < 4 )
    {
      return FARM_STATE_TREE;       //!< もう少し
    }
    else if( time.hour < 12 )
    {
      return FARM_STATE_SAPLING;    //!< まだまだ
    }
    else if( time.hour < 18 )
    {
      return FARM_STATE_SPROUT;     //!< まだまだ
    }
    else {
      return FARM_STATE_PLANTED;    //!< 植えたばかり
    }
  }
  
  return FARM_STATE_HARVEST;        //!< 収穫
}

//------------------------------------------------------------------------------
/**
 * @brief   畑の状態をセットアップ
 */
//------------------------------------------------------------------------------
void LowerFarmDraw::SetupFarmState( void )
{
  u16 itemNo = m_pResortSave->GetFarmItemKind( static_cast<int>(m_KinomiIndex) );
  FUKUSHIMA_PRINT( "GetFarmItemKind=[%d]\n", itemNo );

  // アイテム名
  print::WordSet* wordSet = m_pDrawerBase->GetWordSet();
  {
    wordSet->RegisterItemName( 0, itemNo, 1 );
  }
  this->SetTextboxPaneMessage( m_pItemName, msg_prsys_common_05 );

  // 時間
  UpdateTime();

  // アイテムアイコン
  m_pDrawerBase->GetItemIcon()->SetItemIcon( 0, itemNo, m_pKinomiPic );
  m_pDrawerBase->SetVisiblePane( m_pIconVisPane, true );

  // 状態メッセージ
  SetupFarmState_Msg();
}

//------------------------------------------------------------------------------
/**
 * @brief   はたけの状態のセットアップ：状態メッセージ
 */
//------------------------------------------------------------------------------
void LowerFarmDraw::SetupFarmState_Msg( void )
{
  // 畑の状態メッセージ
  static const u32 c_FieldStateMsg[] = {
    msg_prsys_low_02_03,    //!< 植えた
    msg_prsys_low_02_11,    //!< 芽
    msg_prsys_low_02_04,    //!< 若木
    msg_prsys_low_02_05,    //!< 木
    msg_prsys_low_02_12,    //!< 収穫
  };

  // 時間の取得
  TimeParam time;
  {
    m_pResortSave->GetFarmTime( m_KinomiIndex, &time.hour, &time.min, &time.sec );
  }

  // はたけの状態を取得
  FarmState state = GetFarmState( time );
  {
    this->SetTextboxPaneMessage( m_pStateMsg, c_FieldStateMsg[state] );
  }
}





//==============================================================================
/**
 *      app::ui::UIInputListener
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 *
 * @param   paneID
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult LowerFarmDraw::OnLayoutPaneEvent( const u32 paneID )
{
  if( !m_uiListener ) return ENABLE_ACCESS;

  if( paneID == BTN_ID_CANCEL )
  {
    m_Result = RESULT_CANCEL;
  }
  else if( paneID == BTN_ID_DECIDE )
  {
    m_Result = RESULT_DECIDE;
  }

  return ENABLE_ACCESS;
}






//==============================================================================
/**
 *      app::tool::ButtonManager
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ボタンの個別セットアップ
 *
 * @param   btnMan    ボタンマネージャ
 */
//------------------------------------------------------------------------------
void LowerFarmDraw::SetupButton( app::tool::ButtonManager* btnMan )
{
  btnMan->SetButtonInvisible( BTN_ID_DECIDE );

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    if( i == BTN_ID_CANCEL )
    {
      btnMan->SetButtonSelectSE( i, SEQ_SE_CANCEL1 );
    }
    else {
      btnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
    }
  }
}

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)
