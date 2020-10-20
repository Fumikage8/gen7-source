// ============================================================================
/*
 * @file SuperTrainingLowerItemListView.h
 * @brief すごい特訓アプリの下画面選択リストクラスです。
 * @date 2015.12.07
 * @author uchida_yuto
 */
// ============================================================================
#include "App/SuperTraining/source/View/SuperTrainingLowerItemListView.h"

#include "App/SuperTraining/source/SuperTrainingResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"
#include "AppLib/include/util/app_util_KeyRepeatController.h"

//  リソースのインクルード
#include <arc_index/message.gaix>
#include <niji_conv_header/app/tokkun/res2d/tokkunMenu.h>
#include <niji_conv_header/app/tokkun/res2d/tokkunMenu_anim_list.h>
#include <niji_conv_header/app/tokkun/res2d/tokkunMenu_pane.h>

//  メッセージのインクルード
#include <niji_conv_header/message/msg_tokkun.h>
#include <niji_conv_header/message/msg_itemname.h>

//  サウンドのインクルード
#include <Sound/include/Sound.h>
#include <niji_conv_header/sound/SoundMiddleID.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)
GFL_NAMESPACE_BEGIN(View)

  LowerItemListView::LowerItemListView( App::SuperTraining::System::SuperTrainingWork* pWork ) :
    NetAppLib::System::NetApplicationViewBase(
      pWork,
      App::SuperTraining::SUPER_TRAINING_RESOURCE_ID_LAYOUT,
      LYTRES_TOKKUNMENU_TOKKUN_MIN_LOW_000_BFLYT,
      LA_TOKKUNMENU_TOKKUN_MIN_LOW_000___NUM,
      app::util::G2DUtil::SETUP_LOWER,
      pWork->GetMessageUtility(),
      GARC_message_tokkun_DAT )
  {
    _Clear();

    GFL_ASSERT( pWork );
    m_pWork = pWork;

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    app::util::Heap*  pUseAppHeap = m_pWork->GetAppHeap();

    //  メニューカーソル作成
    {
      m_pMenuCursor = GFL_NEW( pUseAppHeap->GetDeviceHeap() ) NetAppLib::UI::NetAppMenuCursor( m_pWork, GetG2DUtil(), NetAppLib::System::COMMON_RESOURCE_ID_MENU_CURSOR, 0, LAYOUT_WORK_ID );
      m_pMenuCursor->GetAppToolMenuCursor()->StartAnime();
    }

    gfl2::lyt::LytMultiResID& rResourceID = pG2DUtil->GetLayoutResourceID( 0 );
    gfl2::lyt::LytWk* pLayoutWork = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID );
    GFL_ASSERT( pLayoutWork );

    //  ボタン作成
    {
      struct SETUP_BTN_DATA
      {
        gfl2::lyt::LytParts*  pPartsPane;
        u32 seId;
      };

      const SETUP_BTN_DATA  aSetupBtnData[ BUTTON_ID_MAX ]  =
      {
        //  もどるボタン
        { pLayoutWork->GetPartsPane( PANENAME_TOKKUN_MIN_LOW_000_PANE_PARTS_00 ), SEQ_SE_CANCEL1 },

        //  ステータス項目ボタン
        { pLayoutWork->GetPartsPane( PANENAME_TOKKUN_MIN_LOW_000_PANE_MENU_01 ), SEQ_SE_DECIDE1, },
        { pLayoutWork->GetPartsPane( PANENAME_TOKKUN_MIN_LOW_000_PANE_MENU_02 ), SEQ_SE_DECIDE1, },
        { pLayoutWork->GetPartsPane( PANENAME_TOKKUN_MIN_LOW_000_PANE_MENU_03 ), SEQ_SE_DECIDE1, },
        { pLayoutWork->GetPartsPane( PANENAME_TOKKUN_MIN_LOW_000_PANE_MENU_04 ), SEQ_SE_DECIDE1, },
        { pLayoutWork->GetPartsPane( PANENAME_TOKKUN_MIN_LOW_000_PANE_MENU_05 ), SEQ_SE_DECIDE1, },
        { pLayoutWork->GetPartsPane( PANENAME_TOKKUN_MIN_LOW_000_PANE_MENU_06 ), SEQ_SE_DECIDE1, },

        //  とっくん開始ボタン
        { pLayoutWork->GetPartsPane( PANENAME_TOKKUN_MIN_LOW_000_PANE_PARTS_01 ), SEQ_SE_DECIDE1, },
      };

      const app::ui::ButtonInfoEx aButtonInfoList[ BUTTON_ID_MAX ] =
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
        //  Bキャンセル
        {
          BUTTON_ID_CANCEL,
          aSetupBtnData[ BUTTON_ID_CANCEL ].pPartsPane,
          pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_CANCEL ].pPartsPane, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &rResourceID ),
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__PARTS_00_TOUCH,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__PARTS_00_RELEASE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__PARTS_00_CANSEL,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__PARTS_00_TOUCH_RELEASE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__PARTS_00_ACTIVE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__PARTS_00_PASSIVE,
        },

        //      BUTTON_ID_HP,
        {
          BUTTON_ID_HP,
          aSetupBtnData[ BUTTON_ID_HP ].pPartsPane,
          pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_HP ].pPartsPane, PANENAME_TOKKUN_BTN_LOW_000_PANE_BOUND_00, &rResourceID ),
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_01_TOUCH,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_01_RELEASE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_01_CANSEL,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_01_TOUCH_RELEASE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_01_ACTIVE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_01_PASSIVE,
        },

        //      BUTTON_ID_ATK,
        {
          BUTTON_ID_ATK,
          aSetupBtnData[ BUTTON_ID_ATK ].pPartsPane,
          pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_ATK ].pPartsPane, PANENAME_TOKKUN_BTN_LOW_000_PANE_BOUND_00, &rResourceID ),
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_02_TOUCH,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_02_RELEASE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_02_CANSEL,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_02_TOUCH_RELEASE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_02_ACTIVE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_02_PASSIVE,
        },

        //      BUTTON_ID_DEF,
        {
          BUTTON_ID_DEF,
          aSetupBtnData[ BUTTON_ID_DEF ].pPartsPane,
          pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_DEF ].pPartsPane, PANENAME_TOKKUN_BTN_LOW_000_PANE_BOUND_00, &rResourceID ),
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_03_TOUCH,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_03_RELEASE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_03_CANSEL,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_03_TOUCH_RELEASE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_03_ACTIVE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_03_PASSIVE,
        },

        //      BUTTON_ID_SP_ATK,
        {
          BUTTON_ID_SP_ATK,
          aSetupBtnData[ BUTTON_ID_SP_ATK ].pPartsPane,
          pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_SP_ATK ].pPartsPane, PANENAME_TOKKUN_BTN_LOW_000_PANE_BOUND_00, &rResourceID ),
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_04_TOUCH,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_04_RELEASE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_04_CANSEL,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_04_TOUCH_RELEASE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_04_ACTIVE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_04_PASSIVE,
        },

        //      BUTTON_ID_SP_DEF,
        {
          BUTTON_ID_SP_DEF,
          aSetupBtnData[ BUTTON_ID_SP_DEF ].pPartsPane,
          pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_SP_DEF ].pPartsPane, PANENAME_TOKKUN_BTN_LOW_000_PANE_BOUND_00, &rResourceID ),
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_05_TOUCH,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_05_RELEASE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_05_CANSEL,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_05_TOUCH_RELEASE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_05_ACTIVE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_05_PASSIVE,
        },

        //      BUTTON_ID_AGI,
        {
          BUTTON_ID_AGI,
          aSetupBtnData[ BUTTON_ID_AGI ].pPartsPane,
          pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_AGI ].pPartsPane, PANENAME_TOKKUN_BTN_LOW_000_PANE_BOUND_00, &rResourceID ),
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_06_TOUCH,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_06_RELEASE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_06_CANSEL,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_06_TOUCH_RELEASE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_06_ACTIVE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__MENU_06_PASSIVE,
        },

        //  BUTTON_ID_START_TRAINIG
        {
          BUTTON_ID_START_TRAINIG,
          aSetupBtnData[ BUTTON_ID_START_TRAINIG ].pPartsPane,
          pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_START_TRAINIG ].pPartsPane, PANENAME_TOKKUN_BTN_LOW_001_PANE_BOUND_00, &rResourceID ),
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__PARTS_01_TOUCH,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__PARTS_01_RELEASE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__PARTS_01_CANSEL,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__PARTS_01_TOUCH_RELEASE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__PARTS_01_ACTIVE,
          LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__PARTS_01_PASSIVE,
        },
      };

      UIResponder::CreateButtonManager( pUseAppHeap, pLayoutWork, aButtonInfoList, GFL_NELEMS( aButtonInfoList ) );
 
      //  入力コールバック設定
      UIView::SetInputListener( this );

      //  ボタンSEの設定
      {
        app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();

        u32 loopNum = GFL_NELEMS( aSetupBtnData );
        for( u32 i = 0; i < loopNum; ++i )
        {
          pButtonManager->SetButtonSelectSE( i, aSetupBtnData[ i ].seId );
        }
      }

      //  選択アイテム作成
      {
        const SETUP_ITEM_DATA aSetupItemData[ BUTTON_ID_SELECT_TYPE_MAX ]  =
        {
          //  BUTTON_ID_HP,
          {
            tokkun_03_01,
            aButtonInfoList[ BUTTON_ID_HP ]
          },

          //  BUTTON_ID_ATK,
          {
            tokkun_03_02,
            aButtonInfoList[ BUTTON_ID_ATK ]
          },

          //  BUTTON_ID_DEF,
          {
            tokkun_03_03,
            aButtonInfoList[ BUTTON_ID_DEF ]
          },

          //  BUTTON_ID_SP_ATK,
          {
            tokkun_03_04,
            aButtonInfoList[ BUTTON_ID_SP_ATK ]
          },

          //  BUTTON_ID_SP_DEF,
          {
            tokkun_03_05,
            aButtonInfoList[ BUTTON_ID_SP_DEF ]
          },

          //  BUTTON_ID_AGI,
          {
            tokkun_03_06,
            aButtonInfoList[ BUTTON_ID_AGI ]
          },

          //  BUTTON_ID_START_TRAINIG
          {
            tokkun_03_08,
            aButtonInfoList[ BUTTON_ID_START_TRAINIG ]
          }
        };

        u32 loopNum = GFL_NELEMS( m_pSelectItem );
        for( u32 i = 0; i < loopNum; ++i )
        {
          if( aSetupItemData[ i ].buttonInfo.button_id == BUTTON_ID_START_TRAINIG )
          {
            m_pSelectItem[ i ] = GFL_NEW( pUseAppHeap->GetDeviceHeap() ) SelectTrainingItem();
          }
          else
          {
            m_pSelectItem[ i ] = GFL_NEW( pUseAppHeap->GetDeviceHeap() ) SelectCheckItem();
          }

          m_pSelectItem[ i ]->Setup( this, m_pWork, aSetupItemData[ i ], pUseAppHeap->GetDeviceHeap() );
          m_pSelectItem[ i ]->InitFunc();
        }
      }
    }

    //  選択キー操作
    {
      m_menuCursorMax = BUTTON_ID_SELECT_TYPE_MAX;
      m_pSelectItemKeyRepatCtrl = GFL_NEW( pUseAppHeap->GetDeviceHeap() ) app::util::KeyRepeatController( m_menuCursorMax - 1, 0 );
    }

    //  BGループ再生
    {
      pG2DUtil->StartAnime( LAYOUT_WORK_ID, LA_TOKKUNMENU_TOKKUN_MIN_LOW_000__BG_WAVE_GREEN_LOW_00_BG_LOOP_00 );
    }

    //  アイテムインフォセットアップ
    m_itemInfoObject.Setup( this, m_pWork );

    //  初回カーソル位置
    _SetCursorPos( 0 );
  }

  LowerItemListView::~LowerItemListView()
  {
    {
      u32 loopNum = GFL_NELEMS( m_pSelectItem );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_pSelectItem[ i ]->EndFunc();
        GFL_SAFE_DELETE( m_pSelectItem[ i ] );
      }
    }

    m_pMenuCursor->Destroy();
    GFL_SAFE_DELETE( m_pMenuCursor );
    GFL_SAFE_DELETE( m_pSelectItemKeyRepatCtrl );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void LowerItemListView::Update(void)
  {
    if( m_updateSeq == UPDATE_SEQ_IDLE )
    {
    }
    else
    if( m_updateSeq == UPDATE_SEQ_INPUT_ANIME )
    {
      if( UIView::IsButtonAnimation() == false )
      {
        //  アニメを待つ
        //  押したボタンidを返す
        m_pushButtonId  = m_tmpPushButtonId;

        _SetUpdateSeq( UPDATE_SEQ_PAUSE );

        m_tmpPushButtonId = BUTTON_ID_NONE;
      }
    }
    else
    if( m_updateSeq == UPDATE_SEQ_PAUSE )
    {
      
    }

    {
      u32 loopNum = GFL_NELEMS( m_pSelectItem );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_pSelectItem[ i ]->UpdateFunc();
      }
    }

    UIView::Update();
    m_pMenuCursor->Update();
  }

  void LowerItemListView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    app::util::AppRenderingManager* pAppRenderingManager = m_pWork->GetAppRenderingManager();
    pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID );

    if( UIView::IsInputEnabled() == true )
    {
      m_pMenuCursor->Draw( displayNo );
    }
  }

  bool LowerItemListView::IsDrawing(void) const
  {
    bool result = false;
  
    if( UIView::IsDrawing() == true )
    {
      result = true;
    }
    else
    if( m_pMenuCursor->CanDestroy() == false )
    {
      result  = true;
    }

    return result;
  }

  //  開始(Viewを使う前に必ず呼び出し。)
  void LowerItemListView::Start()
  {
    _SetUpdateSeq( UPDATE_SEQ_IDLE );
  }

  //  すごい特訓ボタンにカーソル固定
  void LowerItemListView::LockCursorTrainigButton()
  {
    //  カーソル位置を固定にして移動もしない
    {
      //  チェック項目も移動しない
      const ButtonId  aNonEnableButonId[] =
      {
        BUTTON_ID_HP,
        BUTTON_ID_ATK,
        BUTTON_ID_DEF,
        BUTTON_ID_SP_ATK,
        BUTTON_ID_SP_DEF,
        BUTTON_ID_AGI,
      };

      u32 loopNum = GFL_NELEMS( aNonEnableButonId );
      for( u32 i = 0; i < loopNum; ++i )
      {
        UIView::SetButtonEnable( aNonEnableButonId[ i ], false );
      }
    }

    m_bLockCursor = true;
    m_pSelectItemKeyRepatCtrl->SetMaxPos( 0, 0 );
    _SetCursorButtonId( BUTTON_ID_START_TRAINIG );
  }

  //  チェックボックスの選択の設定を有効にするか
  void LowerItemListView::SetEnableChkBoxSelect( const ButtonId buttonId, const bool bFlg )
  {
    u32 loopNum = GFL_NELEMS( m_pSelectItem );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_pSelectItem[ i ]->GetButtonId() == buttonId )
      {
        m_pSelectItem[ i ]->SetEnableChkBox( bFlg );
      }
    }
  }

  //  チェックボックスの選択が有効化
  bool LowerItemListView::IsEnableChkBoxSelect( const ButtonId buttonId )
  {
    u32 loopNum = GFL_NELEMS( m_pSelectItem );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_pSelectItem[ i ]->GetButtonId() == buttonId )
      {
        return  m_pSelectItem[ i ]->IsChkBox();
      }
    }

    return  false;
  }

  //  選択項目のチェックの切り替え
  bool LowerItemListView::ChkChangeSelect( const ButtonId buttonId )
  {
    u32 loopNum = GFL_NELEMS( m_pSelectItem );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_pSelectItem[ i ]->GetButtonId() == buttonId )
      {
        m_pSelectItem[ i ]->ChangeMark();
        return  true;
      }
    }

    return  false;
  }

  //  選択項目のチェック設定
  void LowerItemListView::SetChkSelect( const ButtonId buttonId, const bool bFlg )
  {
    u32 loopNum = GFL_NELEMS( m_pSelectItem );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_pSelectItem[ i ]->GetButtonId() == buttonId )
      {
        m_pSelectItem[ i ]->SetMark( bFlg );
        return;
      }
    }

    return;
  }

  //  選択項目にチェックがついているか
  bool LowerItemListView::IsChkSelect( const ButtonId buttonId )
  {
    u32 loopNum = GFL_NELEMS( m_pSelectItem );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_pSelectItem[ i ]->GetButtonId() == buttonId )
      {
        return  m_pSelectItem[ i ]->IsMark();
      }
    }

    return false;
  }

  //  とっくん開始ボタンを表示
  void LowerItemListView::SetVisibleTrainingButton( const bool bFlg )
  {
    u32 loopNum = GFL_NELEMS( m_pSelectItem );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_pSelectItem[ i ]->GetButtonId() == BUTTON_ID_START_TRAINIG )
      {
        m_pSelectItem[ i ]->SetVisible( bFlg );

        if( bFlg == true )
        {
          m_menuCursorMax = BUTTON_ID_SELECT_TYPE_MAX;
        }
        else
        {
          //  最後の項目が一つ減る。
          m_menuCursorMax = BUTTON_ID_SELECT_TYPE_MAX - 1;
        }

        m_pSelectItemKeyRepatCtrl->SetMaxPos( m_menuCursorMax - 1, 0 );

        return;
      }
    }
  }

  //  アイテムインフォ表示設定
  void LowerItemListView::SetVisibleItemInfo( const bool bFlg )
  {
    m_itemInfoObject.SetViaible( bFlg );
  }

  //  アイテムインフォの情報設定
  void LowerItemListView::SetItemInfoNumber( const u32 nowNum, const u32 maxNum )
  {
    m_itemInfoObject.SetNowNumber( nowNum );
    m_itemInfoObject.SetMaxNumber( maxNum );
  }

  //  アイテムインフォの情報設定
  void LowerItemListView::SetItemInfoNumberItemLeftPos( const u32 maxNum )
  {
    m_itemInfoObject.SetNowNumber( maxNum );
  }

  //  アイテムインフォの情報に設定している所持数の取得
  u32 LowerItemListView::GetItemInfoNowNumber()
  {
    return  m_itemInfoObject.GetNowNumber();
  }

  //==============================================================================
  /**
   *      ↓　以下がリスナー　↓
   */
  //==============================================================================
  // ペインタッチ時を検知
  // 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
  void LowerItemListView::OnLayoutPaneTouchTriggerEvent( const u32 buttonId )
  {
    _SetCursorButtonId( (ButtonId)buttonId );
  }

  //------------------------------------------------------------------
  /**
    * @brief   ペインタッチ時を検知
    * @param[in] buttonId ボタンid
    * @return 入力処理を続けるかを返す
    */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult LowerItemListView::OnLayoutPaneEvent( const u32 buttonId )
  {
    m_tmpPushButtonId  = (ButtonId)buttonId;
    if( m_tmpPushButtonId == BUTTON_ID_CANCEL )
    {
      _SetUpdateSeq( UPDATE_SEQ_INPUT_ANIME );
    }
    else
    if( m_tmpPushButtonId == BUTTON_ID_START_TRAINIG )
    {
      _SetUpdateSeq( UPDATE_SEQ_INPUT_ANIME );
    }
    else
    {
      //  チェックの選択項目
      _SetUpdateSeq( UPDATE_SEQ_INPUT_ANIME );
    }

    return  app::ui::UIInputListener::DISABLE_ACCESS;
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
  app::ui::UIInputListener::ListenerResult LowerItemListView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    m_pSelectItemKeyRepatCtrl->UpdateInfo( pKey, m_menuCursorPos, 0, NULL );

    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
    {
      UIView::GetButtonManager()->StartSelectedAct( BUTTON_ID_CANCEL );
      return  app::ui::UIInputListener::DISABLE_ACCESS;
    }
    else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      UIView::GetButtonManager()->StartSelectedAct( m_pSelectItem[ m_menuCursorPos ]->GetButtonId() );
      return  app::ui::UIInputListener::DISABLE_ACCESS;
    }
    else
    if( m_bLockCursor == false )
    {
      if( m_pSelectItemKeyRepatCtrl->IsRepeat( gfl2::ui::DIR_UP ) )
      {
        _SetCursorMove( -1 );

        return  app::ui::UIInputListener::DISABLE_ACCESS;
      }
      else
      if( m_pSelectItemKeyRepatCtrl->IsRepeat( gfl2::ui::DIR_DOWN ) )
      {
        _SetCursorMove( 1 );

        return  app::ui::UIInputListener::DISABLE_ACCESS;
      }
    }

    return  app::ui::UIInputListener::ENABLE_ACCESS;
  }

  //  更新シーケンス設定
  void LowerItemListView::_SetUpdateSeq( const UpdateSeq newSeq )
  {
    m_updateSeq = newSeq;

    if( m_updateSeq == UPDATE_SEQ_IDLE )
    {
      m_pushButtonId  = BUTTON_ID_NONE;
      //  ボタンを入力を開始
      UIView::SetInputEnabledAllView( true );
    }
    else
    if( m_updateSeq == UPDATE_SEQ_INPUT_ANIME )
    {
      //  ボタンを入力を停止
      UIView::SetInputEnabledAllView( false );
    }
    else
    if( m_updateSeq == UPDATE_SEQ_PAUSE )
    {
      UIView::SetInputEnabledAllView( false );
    }
  }

  //  チェックマーク選択ボタンか
  bool LowerItemListView::_IsSelectCheckItemButtonId( const ButtonId buttonId )
  {
    static const ButtonId aChkButtonId[]  =
    {
      BUTTON_ID_HP,
      BUTTON_ID_ATK,
      BUTTON_ID_DEF,
      BUTTON_ID_SP_ATK,
      BUTTON_ID_SP_DEF,
      BUTTON_ID_AGI,
    };

    u32 loopNum = GFL_NELEMS( aChkButtonId );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( aChkButtonId[ i ] == buttonId )
      {
        return  true;
      }
    }

    return  false;
  }

  //  カーソル移動
  void LowerItemListView::_SetCursorMove( const s32 move )
  {
    if( move == 0 )
    {
      return;
    }

    s32 newCursorPos  = (s32)m_menuCursorPos + move;

    s32 loopNum = GFL_NELEMS( m_pSelectItem );
    for( s32 i = 0; i < loopNum; ++i )
    {
      if( 0 < move )
      {
        if( loopNum <= newCursorPos )
        {
          newCursorPos  = 0;
        }
      }
      else
      if( move < 0 )
      {
        if( newCursorPos < 0 )
        {
          newCursorPos  = loopNum - 1;
        }
      }

      if( _SetCursorPos( (u32)newCursorPos ) == true )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );
        break;
      }
      else
      {
        if( move < 0 )
        {
          --newCursorPos;
        }
        else
        {
          ++newCursorPos;
        }
      }
    }
  }

  //  カーソル位置設定
  bool LowerItemListView::_SetCursorPos( const u32 pos )
  {
    ::app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLayoutWork = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID );

    bool  bMove = ( m_menuCursorPos != pos );

    m_menuCursorPos = pos;
    GFL_ASSERT( m_menuCursorPos < GFL_NELEMS( m_pSelectItem ) );

    SelectItemBase* pSelectItem = m_pSelectItem[ m_menuCursorPos ];
    if( pSelectItem->IsVisible() == false )
    {
      return  false;
    }

    if( pSelectItem->IsSelectActive() == false )
    {
      return  false;
    }

    gfl2::lyt::LytPane* pCursorPosPane  =  m_pSelectItem[ m_menuCursorPos ]->GetCursorPosPane();
    m_pMenuCursor->GetAppToolMenuCursor()->Put( pLayoutWork, pCursorPosPane );

    //  カーソル位置が変わった
    return  bMove;
  }

  //  カーソル位置設定(ボタンid版)
  void LowerItemListView::_SetCursorButtonId( const ButtonId buttonId )
  {
    u32 loopNum = GFL_NELEMS( m_pSelectItem );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_pSelectItem[ i ]->GetButtonId() == buttonId )
      {
        _SetCursorPos( i );
        return;
      }
    }
  }

//  ここから先は SelectItemBase のクラス定義
  LowerItemListView::SelectItemBase::SelectItemBase()
  {
    _Clear();
  }

  LowerItemListView::SelectItemBase::~SelectItemBase()
  {
  }

  void LowerItemListView::SelectItemBase::Setup( LowerItemListView* pRootView, App::SuperTraining::System::SuperTrainingWork* pWork, const SETUP_ITEM_DATA& rSetup, gfl2::heap::HeapBase* pHeap )
  {
    GFL_ASSERT( pHeap );
    GFL_ASSERT( pRootView );
    GFL_ASSERT( pWork );

    m_pRootView = pRootView;
    m_setupItemData = rSetup;
  }

  void LowerItemListView::SelectItemBase::InitFunc()
  {
  }

  void LowerItemListView::SelectItemBase::EndFunc()
  {
  }

  void LowerItemListView::SelectItemBase::UpdateFunc()
  {
  }

  //  アクティブにするかどうか
  void LowerItemListView::SelectItemBase::SetActive( const bool bActive )
  {
    m_bOldActive  = m_bActive;
    m_bActive = bActive;

    if( m_bOldActive != m_bActive )
    {
      app::util::G2DUtil* pG2DUtil = m_pRootView->GetG2DUtil();

      if( m_bActive == true )
      {
        pG2DUtil->StartAnime( LAYOUT_WORK_ID, m_setupItemData.buttonInfo.active_anime_index );
      }
      else
      {
        pG2DUtil->StartAnime( LAYOUT_WORK_ID, m_setupItemData.buttonInfo.passive_anime_index );
      }
    }
  }

  //  表示設定
  void LowerItemListView::SelectItemBase::SetVisible( const bool bFlg )
  {
    app::util::G2DUtil* pG2DUtil = m_pRootView->GetG2DUtil();
    pG2DUtil->SetPaneVisible( m_setupItemData.buttonInfo.picture_pane, bFlg );
  }

  //  表示しているかどうか
  bool LowerItemListView::SelectItemBase::IsVisible()
  {
    app::util::G2DUtil* pG2DUtil = m_pRootView->GetG2DUtil();
    return  pG2DUtil->GetPaneVisible( m_setupItemData.buttonInfo.picture_pane );
  }

//  ここから先は SelectCheckItem のクラス定義
  LowerItemListView::SelectCheckItem::SelectCheckItem()
  : SelectItemBase()
  {
    _Clear();
  }

  LowerItemListView::SelectCheckItem::~SelectCheckItem()
  {
  }

  void LowerItemListView::SelectCheckItem::Setup( LowerItemListView* pRootView, App::SuperTraining::System::SuperTrainingWork* pWork, const SETUP_ITEM_DATA& rSetup, gfl2::heap::HeapBase* pHeap )
  {
    SelectItemBase::Setup( pRootView, pWork, rSetup, pHeap );

    app::util::G2DUtil* pG2DUtil          = m_pRootView->GetG2DUtil();
    gfl2::lyt::LytWk* pLayoutWork         = m_pRootView->GetLayoutWork( LAYOUT_WORK_ID );
    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    gfl2::lyt::LytParts*  pPartsPane  = (gfl2::lyt::LytParts*)rSetup.buttonInfo.picture_pane;
    //  ペイン取得
    {
      //  チェックマーク
      {
        m_pChkMarkPane  = pLayoutWork->GetPane( pPartsPane, PANENAME_TOKKUN_BTN_LOW_000_PANE_CHECKMARK_CHECK_00, &rMultiResId );
        GFL_ASSERT( m_pChkMarkPane );
      }

      //  チェックボックス
      {
        m_pChkBoxPane  = pLayoutWork->GetPane( pPartsPane, PANENAME_TOKKUN_BTN_LOW_000_PANE_CHECKMARK_BG_00, &rMultiResId );
        GFL_ASSERT( m_pChkBoxPane );
      }

      //  カーソル位置
      {
        m_pCursorPosPane  = pLayoutWork->GetPane( pPartsPane, PANENAME_TOKKUN_BTN_LOW_000_PANE_NULL_CUR, &rMultiResId );
        GFL_ASSERT( m_pCursorPosPane );
      }
    }

    //  固定テキスト設定
    {
      gfl2::lyt::LytTextBox*  pTextPane = pLayoutWork->GetTextBoxPane( pPartsPane, PANENAME_TOKKUN_BTN_LOW_000_PANE_TEXTBOX_00, &rMultiResId );
      GFL_ASSERT( pTextPane );
      pG2DUtil->SetTextBoxPaneString( pTextPane, rSetup.titleStrId );
    }
  }

  //  チェックマーク切り替え
  void LowerItemListView::SelectCheckItem::ChangeMark()
  {
    app::util::G2DUtil* pG2DUtil          = m_pRootView->GetG2DUtil();

    //  チェックボックスをつけるか
    if( pG2DUtil->GetPaneVisible( m_pChkMarkPane ) == false )
    {
      pG2DUtil->SetPaneVisible( m_pChkMarkPane, true );
    }
    else
    {
      pG2DUtil->SetPaneVisible( m_pChkMarkPane, false );
    }
  }

  bool LowerItemListView::SelectCheckItem::IsMark()
  {
    app::util::G2DUtil* pG2DUtil          = m_pRootView->GetG2DUtil();
    return( pG2DUtil->GetPaneVisible( m_pChkMarkPane ) );
  }

  void LowerItemListView::SelectCheckItem::SetMark( const bool bFlg )
  {
    //  チェックボックスは有効になっていないと設定ができない
    if( IsChkBox() == true )
    {
      app::util::G2DUtil* pG2DUtil          = m_pRootView->GetG2DUtil();
      pG2DUtil->SetPaneVisible( m_pChkMarkPane, bFlg );
    }
  }

  //  チェックマークそのもの有効
  void LowerItemListView::SelectCheckItem::SetEnableChkBox( const bool bFlg )
  {
    app::util::G2DUtil* pG2DUtil          = m_pRootView->GetG2DUtil();
    pG2DUtil->SetPaneVisible( m_pChkBoxPane, bFlg );
    if( bFlg == false )
    {
      pG2DUtil->SetPaneVisible( m_pChkMarkPane, false );
    }
  }

  bool LowerItemListView::SelectCheckItem::IsChkBox()
  {
    app::util::G2DUtil* pG2DUtil          = m_pRootView->GetG2DUtil();
    return  pG2DUtil->GetPaneVisible( m_pChkBoxPane);
  }

//  ここから先は SelectTrainingItem のクラス定義
  //  とっくん開始選択アイテム
  LowerItemListView::SelectTrainingItem::SelectTrainingItem()
  {
  }

  LowerItemListView::SelectTrainingItem::~SelectTrainingItem()
  {
  }

  void LowerItemListView::SelectTrainingItem::Setup( LowerItemListView* pRootView, App::SuperTraining::System::SuperTrainingWork* pWork, const SETUP_ITEM_DATA& rSetup, gfl2::heap::HeapBase* pHeap )
  {
    SelectItemBase::Setup( pRootView, pWork, rSetup, pHeap );

    app::util::G2DUtil* pG2DUtil          = m_pRootView->GetG2DUtil();
    gfl2::lyt::LytWk* pLayoutWork         = m_pRootView->GetLayoutWork( LAYOUT_WORK_ID );
    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    gfl2::lyt::LytParts*  pPartsPane  = (gfl2::lyt::LytParts*)rSetup.buttonInfo.picture_pane;

    //  ペイン取得
    {
      //  カーソル位置
      {
        m_pCursorPosPane  = pLayoutWork->GetPane( pPartsPane, PANENAME_TOKKUN_BTN_LOW_001_PANE_NULL_CUR, &rMultiResId );
        GFL_ASSERT( m_pCursorPosPane );
      }
    }

    //  固定テキスト設定
    {
      gfl2::lyt::LytTextBox*  pTextPane = pLayoutWork->GetTextBoxPane( pPartsPane, PANENAME_TOKKUN_BTN_LOW_001_PANE_TEXTBOX_00, &rMultiResId );
      GFL_ASSERT( pTextPane );
      pG2DUtil->SetTextBoxPaneString( pTextPane, rSetup.titleStrId );
    }
  }

//  ここから先は ItemInfoObject のクラス定義
  LowerItemListView::ItemInfoObject::ItemInfoObject()
  {
    _Clear();
  }

  LowerItemListView::ItemInfoObject::~ItemInfoObject()
  {
  }

  void LowerItemListView::ItemInfoObject::Setup( LowerItemListView* pRootView, App::SuperTraining::System::SuperTrainingWork* pWork )
  {
    GFL_ASSERT( pRootView );
    GFL_ASSERT( pWork );

    m_pRootView = pRootView;

    app::util::G2DUtil* pG2DUtil  = m_pRootView->GetG2DUtil();
    gfl2::lyt::LytWk* pLayoutWork = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID );

    m_pItemNameTextPane     = pLayoutWork->GetTextBoxPane( PANENAME_TOKKUN_MIN_LOW_000_PANE_TEXTBOX_03 );
    GFL_ASSERT( m_pItemNameTextPane );

    m_pNowItemNumTextPane   = pLayoutWork->GetTextBoxPane( PANENAME_TOKKUN_MIN_LOW_000_PANE_TEXTBOX_00 );
    GFL_ASSERT( m_pNowItemNumTextPane );

    m_pMaxItemNumTextPane   = pLayoutWork->GetTextBoxPane( PANENAME_TOKKUN_MIN_LOW_000_PANE_TEXTBOX_01 );
    GFL_ASSERT( m_pMaxItemNumTextPane );

    m_pItemNumSlashTextPane = pLayoutWork->GetTextBoxPane( PANENAME_TOKKUN_MIN_LOW_000_PANE_TEXTBOX_02 );
    GFL_ASSERT( m_pItemNumSlashTextPane );

    //  固定テキスト設定
    if( pWork->GetAppParam()->in.useCrown == App::SuperTraining::USE_CROWN_GOLD )
    {
      //  スラッシュ
      pG2DUtil->SetTextBoxPaneString( m_pItemNumSlashTextPane, L"" );
      pG2DUtil->SetTextBoxPaneString( m_pNowItemNumTextPane, L"" );
      pG2DUtil->SetTextBoxPaneString( m_pMaxItemNumTextPane, L"" );

      //  きんのおうかん
      /*
        @attention
          アイテム名の取得がルール違反だが、nijiで問題ないとのことなので、momijiでも変更しない。
          RegisterItemName()を使用するべき。
      */
      {
        gfl2::str::StrBuf&  rStrBuf = pWork->GetMessageUtility()->GetString( GARC_message_itemname_DAT, ITEMNAME_796 );
        pG2DUtil->SetTextBoxPaneString( m_pItemNameTextPane, &rStrBuf );
      }
    }
    else
    {
      //  スラッシュ
      pG2DUtil->SetTextBoxPaneString( m_pItemNumSlashTextPane, tokkun_02_02 );

      //  ぎんのおうかん
      /*
        @attention
          アイテム名の取得がルール違反だが、nijiで問題ないとのことなので、momijiでも変更しない。
          RegisterItemName()を使用するべき。
      */
      {
        gfl2::str::StrBuf&  rStrBuf = pWork->GetMessageUtility()->GetString( GARC_message_itemname_DAT, ITEMNAME_795 );
        pG2DUtil->SetTextBoxPaneString( m_pItemNameTextPane, &rStrBuf );
      }
    }
  }

  void LowerItemListView::ItemInfoObject::SetNowNumber( const u32 num )
  {
    GFL_ASSERT( m_pNowItemNumTextPane );

    m_nowNum  = num;

    app::util::G2DUtil* pG2DUtil  = m_pRootView->GetG2DUtil();
    // @fix GFNMCat[1486] 道具最大は９９９なので数値は３桁
    pG2DUtil->SetTextBoxPaneNumber( m_pNowItemNumTextPane, tokkun_02_01, num, 3, 0 );
  }

  void LowerItemListView::ItemInfoObject::SetMaxNumber( const u32 num )
  {
    GFL_ASSERT( m_pMaxItemNumTextPane );

    app::util::G2DUtil* pG2DUtil  = m_pRootView->GetG2DUtil();
    // @fix GFNMCat[1486] 道具最大は９９９なので数値は３桁
    pG2DUtil->SetTextBoxPaneNumber( m_pMaxItemNumTextPane, tokkun_02_01, num, 3, 0 );
  }

  void LowerItemListView::ItemInfoObject::SetViaible( const bool bFlg )
  {
    GFL_ASSERT( m_pNowItemNumTextPane );
    GFL_ASSERT( m_pMaxItemNumTextPane );
    GFL_ASSERT( m_pItemNumSlashTextPane );
    GFL_ASSERT( m_pItemNameTextPane );

    gfl2::lyt::LytPane* pVisiblePaneArray[] =
    {
      m_pNowItemNumTextPane,
      m_pMaxItemNumTextPane,
      m_pItemNumSlashTextPane,
      m_pItemNameTextPane,
    };

    app::util::G2DUtil* pG2DUtil  = m_pRootView->GetG2DUtil();
    u32 loopNum = GFL_NELEMS( pVisiblePaneArray );
    for( u32 i = 0; i < loopNum; ++i )
    {
      pG2DUtil->SetPaneVisible( pVisiblePaneArray[ i ], bFlg );
    }
  }

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)
