// ============================================================================
/*
 * @file BattleRoyalResultLowerBGView.cpp
 * @brief バトルロイヤル勝敗アプリの背景下画面を表示するクラス
 * @date 2015.10.26
 * @author uchida_yuto
 */
// ============================================================================
#include "App/BattleRoyalResult/source/View/BattleRoyalResultLowerBGView.h"
#include "App/BattleRoyalResult/source/BattleRoyalResultResourceID.h"
#include "App/BattleRoyalResult/source/System/BattleRoyalResultWork.h"

#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"

//  リソースのインクルード
#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_battleroyal_result.h>
#include <niji_conv_header/app/battle_royal/res2d/BattleRoyalResultLyt.h>
#include <niji_conv_header/app/battle_royal/res2d/BattleRoyalResultLyt_anim_list.h>
#include <niji_conv_header/app/battle_royal/res2d/BattleRoyalResultLyt_pane.h>

#include <niji_conv_header/sound/SoundMiddleID.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(View)

  BattleRoyalResultLowerBGView::BattleRoyalResultLowerBGView( App::BattleRoyalResult::System::BattleRoyalResultWork* pWork ) :
    NetAppLib::System::NetApplicationViewBase(
      pWork,
      App::BattleRoyalResult::BATTLE_ROYAL_RESULT_RESOURCE_ID_LAYOUT,
      LYTRES_BATTLEROYALRESULTLYT_BRL_MIN_LOW_000_BFLYT,
      LA_BATTLEROYALRESULTLYT_BRL_MIN_LOW_000___NUM,
      app::util::G2DUtil::SETUP_LOWER,
      pWork->GetMessageUtility(),
      GARC_message_battleroyal_result_DAT )
  {
    _Clear();

    GFL_ASSERT( pWork );
    m_pWork = pWork;

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    gfl2::lyt::LytMultiResID& rResourceID = pG2DUtil->GetLayoutResourceID( 0 );
    gfl2::lyt::LytWk* pLayoutWork = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
    GFL_ASSERT( pLayoutWork );

    //  ＢＧループアニメ再生
    pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, LA_BATTLEROYALRESULTLYT_BRL_MIN_LOW_000__BG_BG_LOOP_00 );

    //  ボタン関連の設定
    {
      gfl2::lyt::LytParts*  pNextBtnPartsPane  = pLayoutWork->GetPartsPane( PANENAME_BRL_MIN_LOW_000_PANE_NEXT_BUTTON );
      GFL_ASSERT( pNextBtnPartsPane );

      const ::app::ui::ButtonInfoEx aButtonPaneIdxList[ BUTTON_ID_MAX ] =
      {
      /*
        app::tool::ButtonManager::ButtonId button_id;
        gfl2::lyt::LytPane* picture_pane;
        gfl2::lyt::LytPane* bound_pane;
        u32                 touch_anime_index;        //!< タッチ時のアニメ
        u32                 release_anime_index;      //!< タッチ決定時のアニメ
        u32                 cancel_anime_index;       //!< タッチ状態からのキャンセルアニメ
        u32                 key_select_anime_index;   //!< キー決定時のアニメ ( touch => release の連続アニメ )
        u32                 active_anime_index;       //!< アクティブアニメ
        u32                 passive_anime_index;      //!< パッシブアニメ
      */
        //  もどるボタン
        {
          BUTTON_ID_NEXT,
          pNextBtnPartsPane,
          pLayoutWork->GetBoundingPane( pNextBtnPartsPane, PANENAME_COMMON_BTN_LOW_018_PANE_BOUND_00, &rResourceID ),
          LA_BATTLEROYALRESULTLYT_BRL_MIN_LOW_000__NEXT_BUTTON_TOUCH,
          LA_BATTLEROYALRESULTLYT_BRL_MIN_LOW_000__NEXT_BUTTON_RELEASE,
          LA_BATTLEROYALRESULTLYT_BRL_MIN_LOW_000__NEXT_BUTTON_CANSEL,
          LA_BATTLEROYALRESULTLYT_BRL_MIN_LOW_000__NEXT_BUTTON_TOUCH_RELEASE,
          LA_BATTLEROYALRESULTLYT_BRL_MIN_LOW_000__NEXT_BUTTON_ACTIVE,
          LA_BATTLEROYALRESULTLYT_BRL_MIN_LOW_000__NEXT_BUTTON_PASSIVE,
        },
      };

      //  テキスト設定
      {
        gfl2::lyt::LytTextBox*  pTextBoxPane  = pLayoutWork->GetTextBoxPane( pNextBtnPartsPane, PANENAME_COMMON_BTN_LOW_018_PANE_TEXTBOX_00, &rResourceID );
        GFL_ASSERT( pTextBoxPane );

        pG2DUtil->SetTextBoxPaneString( pTextBoxPane, battle_r_result_02_01 );
      }

      UIResponder::CreateButtonManager( m_pWork->GetAppHeap(), pLayoutWork, aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );
 
      //  入力コールバック設定
      UIView::SetInputListener( this );

      //  ボタンSEの設定
      {
        app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();
        pButtonManager->SetButtonSelectSE( BUTTON_ID_NEXT, SEQ_SE_DECIDE1 );
      }
    }
  }

  BattleRoyalResultLowerBGView::~BattleRoyalResultLowerBGView()
  {
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void BattleRoyalResultLowerBGView::Update(void)
  {
    if( m_state == STATE_TYPE_IN_NEXT_BUTTON )
    {
      app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
      if( pG2DUtil->IsAnimeEnd( LAYOUT_WORK_ID_LOWER, LA_BATTLEROYALRESULTLYT_BRL_MIN_LOW_000_IN ) == true )
      {
        UIView::SetInputEnabled( true );
        _SetState( STATE_TYPE_IDLE );
      }
    }

     NetAppLib::System::NetApplicationViewBase::Update();
  }

  void BattleRoyalResultLowerBGView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    app::util::AppRenderingManager* pAppRenderingManager = m_pWork->GetAppRenderingManager();
    pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  }

  //  もどるボタン登場
  void BattleRoyalResultLowerBGView::InEventBackButton()
  {
    _SetState( STATE_TYPE_IN_NEXT_BUTTON );
  }

  //------------------------------------------------------------------
  /**
    * @brief   ペインタッチ時を検知
    * @param[in] buttonId ボタンid
    * @return 入力処理を続けるかを返す
    */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult BattleRoyalResultLowerBGView::OnLayoutPaneEvent( const u32 id )
  {
    if( id == BUTTON_ID_NEXT )
    {
      m_pushButtonId  = (ButtonId)id;
      return  DISABLE_ACCESS;
    }

    return  ENABLE_ACCESS;
  }

  //------------------------------------------------------------------
  /**
    * @brief   キー入力通知用リスナ
    * @param[in] pButton  ボタン入力の情報
    * @param[in] pKey     キー入力の情報
    * @param[in] pStick   スティック入力の情報
    * @return 入力処理を続けるかを返す
    */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult BattleRoyalResultLowerBGView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( UIView::IsButtonAnimation() == true )
    {
      return  DISABLE_ACCESS;
    }

    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == true )
    {
      UIView::GetButtonManager()->StartSelectedAct( BUTTON_ID_NEXT );
      return  DISABLE_ACCESS;
    }

    return  ENABLE_ACCESS;
  }

  void BattleRoyalResultLowerBGView::_SetState( const StateType newState )
  {
    m_state = newState;

    if( m_state == STATE_TYPE_IN_NEXT_BUTTON )
    {
      app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
      pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, LA_BATTLEROYALRESULTLYT_BRL_MIN_LOW_000_IN );
    }
  }

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)
