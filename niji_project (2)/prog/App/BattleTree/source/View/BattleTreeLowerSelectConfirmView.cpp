// ============================================================================
/*
 * @file LowerSelectConfirmView.h
 * @brief バトルツリーアプリの下画面選択確認表示するクラスです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#include "App/BattleTree/source/View/BattleTreeLowerSelectConfirmView.h"
#include "App/BattleTree/source/BattleTreeResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"
#include "AppLib/include/Tool/CommonMessageObject.h"
#include "AppLib/include/Util/app_util_KeyRepeatController.h"

//  リソースのインクルード
#include <arc_index/message.gaix>
#include <niji_conv_header/app/battletree/res2d/battletreeLyt.h>
#include <niji_conv_header/app/battletree/res2d/battletreeLyt_anim_list.h>
#include <niji_conv_header/app/battletree/res2d/battletreeLyt_pane.h>

//  メッセージのインクルード
#include <niji_conv_header/message/msg_battletrainer_select.h>

//  サウンドのインクルード
#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(View)

  LowerSelectConfirmView::LowerSelectConfirmView( App::BattleTree::System::BattleTreeWork* pWork ) :
    NetAppLib::System::NetApplicationViewBase(
      pWork,
      App::BattleTree::BATTLE_TREE_RESOURCE_ID_LAYOUT,
      LYTRES_BATTLETREELYT_BATTLETREE_MIN_LOW_002_BFLYT,
      LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002___NUM,
      app::util::G2DUtil::SETUP_LOWER,
      pWork->GetMessageUtility(),
      GARC_message_battletrainer_select_DAT )
  {
    _Clear();

    GFL_ASSERT( pWork );
    m_pWork = pWork;

    gfl2::heap::HeapBase* pDevHeap = m_pWork->GetAppHeap()->GetDeviceHeap();

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    gfl2::lyt::LytWk* pLayoutWork = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID );
    GFL_ASSERT( pLayoutWork );

    gfl2::lyt::LytMultiResID& rMultiResID   = pG2DUtil->GetLayoutResourceID( 0 );

    //  メニューカーソル作成
    {
      m_pMenuCursor     = GFL_NEW( pDevHeap ) NetAppLib::UI::NetAppMenuCursor( m_pWork, GetG2DUtil(), NetAppLib::System::COMMON_RESOURCE_ID_MENU_CURSOR, 0, LAYOUT_WORK_ID );
      m_pKeyRepeatCtrl  = GFL_NEW( pDevHeap ) app::util::KeyRepeatController( GFL_NELEMS( m_aMenuCursorPosData ) - 1, 0 );
    }

    //  ボタン作成
    {
      struct SETUP_BTN_DATA
      {
        gfl2::lyt::LytParts*  pPartsPane;
        u32 seId;
      };

      //  リストの順序が変わるとここを並べなおさないといけない
      const SETUP_BTN_DATA aSetupBtnData[ BUTTON_ID_MAX ]  =
      {
        { pLayoutWork->GetPartsPane( PANENAME_BATTLETREE_MIN_LOW_002_PANE_PARTS_11 ),   SEQ_SE_DECIDE1 },
        { pLayoutWork->GetPartsPane( PANENAME_BATTLETREE_MIN_LOW_002_PANE_PARTS_10 ),   SEQ_SE_DECIDE1 },
        { pLayoutWork->GetPartsPane( PANENAME_BATTLETREE_MIN_LOW_002_PANE_BACKBUTTON ), SEQ_SE_CANCEL1 },
      };

      const app::ui::ButtonInfoEx aButtonPaneIdxList[ BUTTON_ID_MAX ] =
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
        //  no
        {
          BUTTON_ID_NO,
          aSetupBtnData[ BUTTON_ID_NO ].pPartsPane,
          pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_NO ].pPartsPane, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00, &rMultiResID ),
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__PARTS_11_TOUCH,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__PARTS_11_RELEASE,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__PARTS_11_CANSEL,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__PARTS_11_TOUCH_RELEASE,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__PARTS_11_ACTIVE,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__PARTS_11_PASSIVE,
        },

        //  yes
        {
          BUTTON_ID_YES,
          aSetupBtnData[ BUTTON_ID_YES ].pPartsPane,
          pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_YES ].pPartsPane, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00, &rMultiResID ),
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__PARTS_10_TOUCH,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__PARTS_10_RELEASE,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__PARTS_10_CANSEL,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__PARTS_10_TOUCH_RELEASE,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__PARTS_10_ACTIVE,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__PARTS_10_PASSIVE,
        },

        //  Bキャンセル
        {
          BUTTON_ID_CANCEL,
          aSetupBtnData[ BUTTON_ID_CANCEL ].pPartsPane,
          pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_CANCEL ].pPartsPane, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &rMultiResID ),
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__BACKBUTTON_TOUCH,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__BACKBUTTON_RELEASE,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__BACKBUTTON_CANSEL,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__BACKBUTTON_TOUCH_RELEASE,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__BACKBUTTON_ACTIVE,
          LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__BACKBUTTON_PASSIVE
        }
      };

      UIResponder::CreateButtonManager( m_pWork->GetAppHeap(), pLayoutWork, aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );
 
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

      //  カーソルのボタンデータ作成
      {
        const MENU_CURSOR_POS_DATA  aSettingMenuCursorPosData[ MENU_CURSOR_POS_MAX ] =
        {
          {
            pLayoutWork->GetPane( aSetupBtnData[ BUTTON_ID_YES ].pPartsPane, PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR, &rMultiResID ),
            BUTTON_ID_YES
          },

          {
            pLayoutWork->GetPane( aSetupBtnData[ BUTTON_ID_NO ].pPartsPane, PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR, &rMultiResID ),
            BUTTON_ID_NO
          },
        };

        GFL_ASSERT( sizeof( aSettingMenuCursorPosData ) == sizeof( m_aMenuCursorPosData ) );
        gfl2::std::MemCopy( aSettingMenuCursorPosData, m_aMenuCursorPosData, sizeof( aSettingMenuCursorPosData ) );

        //  固定テキスト設定
        {
          gfl2::lyt::LytTextBox*  pLytTextBox = NULL;
          //  yes
          {
            pLytTextBox = pLayoutWork->GetTextBoxPane( aSetupBtnData[ BUTTON_ID_YES ].pPartsPane, PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00, &rMultiResID );
            pG2DUtil->SetTextBoxPaneString( pLytTextBox, battle_trainer_select_yes );
          }
          
          //  no
          {
            pLytTextBox = pLayoutWork->GetTextBoxPane( aSetupBtnData[ BUTTON_ID_NO ].pPartsPane, PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00, &rMultiResID );
            pG2DUtil->SetTextBoxPaneString( pLytTextBox, battle_trainer_select_no );
          }
        }
      }
    }

    //  ウィンドウ作成
    {
      m_pLowerCommonMessageObject = GFL_NEW( pDevHeap ) app::tool::CommonMessageObject();

      app::util::GetPaneHelper  paneHelper( pLayoutWork, &rMultiResID );
      paneHelper.Push( PANENAME_BATTLETREE_MIN_LOW_002_PANE_PARTS_12 );

      app::tool::CommonMessageObject::INIT_PARAM  initParam;
      {
        initParam.lytId         = LAYOUT_WORK_ID;
        initParam.pG2DUtil      = pG2DUtil;
        initParam.inAnimeId     = LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__PARTS_12_IN;
        initParam.outAnimeId    = LA_BATTLETREELYT_BATTLETREE_MIN_LOW_002__PARTS_12_OUT;
        initParam.pWindowPane   = paneHelper.Peek();
        initParam.showWaitFrame = 0;

        //  テキストペイン設定
        {
          initParam.pTextBoxPaneArray[ 0 ]  = paneHelper.GetTextBoxPane( PANENAME_COMMON_MSG_LOW_001_PANE_TEXTBOX_00 );
          initParam.textBoxPaneNum  = 1;
        }
      }

      m_pLowerCommonMessageObject->InitFunc( initParam, pDevHeap );
      {
        gfl2::str::StrBuf* ppStrBuf[ 1 ] = { NULL };
        ppStrBuf[ 0 ] = &m_pWork->GetMessageUtility()->GetString( GARC_message_battletrainer_select_DAT, battle_trainer_select_01_03 );
        m_pLowerCommonMessageObject->ShowMessageTextPane( ppStrBuf, 1 );
      }
    }

    //  ペイン取得
    {
      app::tool::AppToolTrainerIconRendering::OutputDefaultIconObjectData( 1, &m_trainerIconObjData );
      m_trainerIconObjData.pReplacePane    = pLayoutWork->GetPicturePane( PANENAME_BATTLETREE_MIN_LOW_002_PANE_PLAYERPHOTO );

      m_pTrainerNameTextBox = pLayoutWork->GetTextBoxPane( PANENAME_BATTLETREE_MIN_LOW_002_PANE_TEXTBOX_00 );
    }

    //  アイコンロードidを取得
    m_trainerLoadIconId = m_pWork->GetAllocTrainerLoadID();
  }

  LowerSelectConfirmView::~LowerSelectConfirmView()
  {
    m_pMenuCursor->Destroy();

    m_pLowerCommonMessageObject->EndFunc();

    GFL_SAFE_DELETE( m_pLowerCommonMessageObject );
    GFL_SAFE_DELETE( m_pKeyRepeatCtrl );
    GFL_SAFE_DELETE( m_pMenuCursor );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void LowerSelectConfirmView::Update(void)
  {
    if( m_updateSeq == UPDATE_SEQ_IDLE )
    {
    }
    else
    if( m_updateSeq == UPDATE_SEQ_BTN_ANIM_WAIT )
    {
      if( UIView::IsButtonAnimation() == false )
      {
        _SetUpdateSeq( UPDATE_SEQ_PAUSE );
      }
    }
    else
    if( m_updateSeq == UPDATE_SEQ_PAUSE )
    {

    }

    m_pLowerCommonMessageObject->Update();

    UIView::Update();
    m_pMenuCursor->Update();
  }

  void LowerSelectConfirmView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    app::util::AppRenderingManager* pAppRenderingManager = m_pWork->GetAppRenderingManager();
    pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID );

    if( UIView::IsInputEnabled() == true )
    {
      m_pMenuCursor->Draw( displayNo );
    }
  }

  bool LowerSelectConfirmView::IsDrawing(void) const
  {
    bool result = false;
  
    if( UIView::IsDrawing() == true )
    {
      result = true;
    }

    return result;
  }

  //------------------------------------------------------------------
  /**
    * @brief   処理開始
    * @note    ポーズ状態から復帰する
    */
  //------------------------------------------------------------------
  void LowerSelectConfirmView::Start( const App::BattleTree::NPC_SELECT_ITEM* pItem )
  {
    GFL_ASSERT( pItem );

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    //  トレーナー名
    pG2DUtil->SetTextBoxPaneString( m_pTrainerNameTextBox, pItem->aName );
    //  アイコン
    {
      m_trainerIconObjData.trainerId  = pItem->type;
      m_pWork->GetTrainerIconUtilty()->SetIconObjectData( m_trainerLoadIconId, m_trainerIconObjData, false );
    }

    _SetUpdateSeq( UPDATE_SEQ_IDLE );
  }

  //  押したボタンid
  LowerSelectConfirmView::ButtonId  LowerSelectConfirmView::GetPushId()
  {
    if( m_updateSeq == UPDATE_SEQ_PAUSE )
    {
      return m_pushId;
    }

    return  BUTTON_ID_NONE;
  }

  //------------------------------------------------------------------
  /**
    * @brief   ペインタッチ時を検知
    * @param[in] buttonId ボタンid
    * @return 入力処理を続けるかを返す
    */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult LowerSelectConfirmView::OnLayoutPaneEvent( const u32 id )
  {
    m_pushId  = (ButtonId)id;
    _SetUpdateSeq( UPDATE_SEQ_BTN_ANIM_WAIT );

    return  DISABLE_ACCESS;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインタッチ時を検知
  * @note 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
  */
  //--------------------------------------------------------------------------------------------
  void LowerSelectConfirmView::OnLayoutPaneTouchTriggerEvent( const u32 buttonId )
  {
    _SetCursorButtonId( (ButtonId)buttonId );
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
  app::ui::UIInputListener::ListenerResult LowerSelectConfirmView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
     m_pKeyRepeatCtrl->UpdateInfo( pKey, m_menuCursorPos, 0 );

    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
    {
      UIView::GetButtonManager()->StartSelectedAct( BUTTON_ID_CANCEL );

      return  DISABLE_ACCESS;
    }
    else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
    {
      _PushEntry();
    }
    else
    {
      if( m_pKeyRepeatCtrl->IsRepeat( gfl2::ui::DIR_UP ) )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );

        _SetCursorMove( -1 );
        return  DISABLE_ACCESS;
      }
      else
      if( m_pKeyRepeatCtrl->IsRepeat( gfl2::ui::DIR_DOWN ) )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );

        _SetCursorMove( 1 );
        return  DISABLE_ACCESS;
      }
    }

    return  ENABLE_ACCESS;
  }

  //  更新シーケンス設定
  void  LowerSelectConfirmView::_SetUpdateSeq( const UpdateSeq newSeq )
  {
    m_updateSeq = newSeq;

    if( m_updateSeq == UPDATE_SEQ_IDLE )
    {
      _SetCursorPos( 0 );

      UIView::SetInputEnabledAllView( true );
    }
    else
    if( m_updateSeq == UPDATE_SEQ_BTN_ANIM_WAIT )
    {
      UIView::SetInputEnabledAllView( false );
    }
    else
    if( m_updateSeq == UPDATE_SEQ_PAUSE )
    {
      UIView::SetInputEnabledAllView( false );
    }
  }

  //  カーソル移動
  void LowerSelectConfirmView::_SetCursorMove( const s32 move )
  {
    s32 maxNum = GFL_NELEMS( m_aMenuCursorPosData );
    s32 newCursorPos  = (s32)m_menuCursorPos + move;
    if( newCursorPos < 0 )
    {
      newCursorPos = maxNum - 1;
    }
    else
    if( maxNum <= newCursorPos )
    {
      newCursorPos = 0;
    }

    _SetCursorPos( (u32)newCursorPos );
  }

  //  カーソル位置設定
  void LowerSelectConfirmView::_SetCursorPos( const u32 pos )
  {
    ::app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLayoutWork = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID );

    m_menuCursorPos = pos;
    GFL_ASSERT( m_menuCursorPos < GFL_NELEMS( m_aMenuCursorPosData ) );
    m_pMenuCursor->GetAppToolMenuCursor()->Put( pLayoutWork, m_aMenuCursorPosData[ m_menuCursorPos ].pPosPane );
  }

  //  カーソル位置設定(ボタンid版)
  void LowerSelectConfirmView::_SetCursorButtonId( const ButtonId buttonId )
  {
    u32 loopNum = GFL_NELEMS( m_aMenuCursorPosData );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_aMenuCursorPosData[ i ].buttonId == buttonId )
      {
        _SetCursorPos( i );
        return;
      }
    }
  }

  //  決定
  void LowerSelectConfirmView::_PushEntry()
  {
    UIView::GetButtonManager()->StartSelectedAct( m_aMenuCursorPosData[ m_menuCursorPos ].buttonId );
  }

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)
