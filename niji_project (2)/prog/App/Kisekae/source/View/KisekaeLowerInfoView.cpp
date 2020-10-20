// ============================================================================
/*
 * @file kisekaseLowerInfoView.cpp
 * @brief きせかえアプリの下画面を表示するクラスです。
 * @date  2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Kisekae/source/View/KisekaeLowerInfoView.h"

#include "App/Kisekae/source/KisekaeResourceID.h"

#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"
#include "AppLib/include/Tool/CommonMessageObject.h"

//  リソースのインクルード
#include <arc_index/message.gaix>
#include <niji_conv_header/app/shop_kisekae/res2d/ShopKisekaeLytData.h>
#include <niji_conv_header/app/shop_kisekae/res2d/ShopKisekaeLytData_anim_list.h>
#include <niji_conv_header/app/shop_kisekae/res2d/ShopKisekaeLytData_pane.h>

#include <niji_conv_header/message/msg_kisekae.h>
#include <niji_conv_header/sound/SoundMiddleID.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(View)

  LowerInfoView::LowerInfoView( App::Kisekae::System::Work* pWork ) :
    NetAppLib::System::NetApplicationViewBase(
      pWork,
      App::Kisekae::RESOURCE_ID_LAYOUT,
      LYTRES_SHOPKISEKAELYTDATA_SHOPKISEKAE_MSG_LOW_000_BFLYT,
      LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_MSG_LOW_000___NUM,
      app::util::G2DUtil::SETUP_LOWER,
      pWork->GetMessageUtility(),
      GARC_message_kisekae_DAT )
  {
    _Clear();

    GFL_ASSERT( pWork );
    m_pWork = pWork;

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    app::util::Heap*  pUseAppHeap = m_pWork->GetAppHeap();

    gfl2::lyt::LytMultiResID& rResourceID = pG2DUtil->GetLayoutResourceID( 0 );
    gfl2::lyt::LytWk* pLayoutWork = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
    GFL_ASSERT( pLayoutWork );

    //  ボタン作成
    {
      struct SETUP_BTN_DATA
      {
        gfl2::lyt::LytParts*  pPartsPane;
        u32 seId;
      };

      const SETUP_BTN_DATA aSetupBtnData[ BUTTON_ID_MAX ]  =
      {
        { pLayoutWork->GetPartsPane( PANENAME_SHOPKISEKAE_MSG_LOW_000_PANE_BACKBUTTON ), SEQ_SE_CANCEL1 },
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
        //  Bキャンセル
        {
          BUTTON_ID_CANCEL,
          aSetupBtnData[ BUTTON_ID_CANCEL ].pPartsPane,
          pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_CANCEL ].pPartsPane, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &rResourceID ),
          LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_MSG_LOW_000__BACKBUTTON_TOUCH,
          LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_MSG_LOW_000__BACKBUTTON_RELEASE,
          LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_MSG_LOW_000__BACKBUTTON_CANSEL,
          LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_MSG_LOW_000__BACKBUTTON_TOUCH_RELEASE,
          LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_MSG_LOW_000__BACKBUTTON_ACTIVE,
          LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_MSG_LOW_000__BACKBUTTON_PASSIVE
        }
      };

      UIResponder::CreateButtonManager( pUseAppHeap, pLayoutWork, aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );
 
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
    }

    //  枠のウィンドウ
    {
      m_pLowerCommonMessageObject = GFL_NEW( pUseAppHeap->GetDeviceHeap() ) app::tool::CommonMessageObject();

      app::util::GetPaneHelper  paneHelper( pLayoutWork, &rResourceID );
      paneHelper.Push( PANENAME_SHOPKISEKAE_MSG_LOW_000_PANE_MESSAGE_L10 );

      app::tool::CommonMessageObject::INIT_PARAM  initParam;
      {
        initParam.lytId         = LAYOUT_WORK_ID_LOWER;
        initParam.pG2DUtil      = pG2DUtil;
        initParam.inAnimeId     = LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_MSG_LOW_000__MESSAGE_L10_IN;
        initParam.outAnimeId    = LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_MSG_LOW_000__MESSAGE_L10_OUT;
        initParam.pWindowPane   = paneHelper.Peek();
        initParam.showWaitFrame = 0;

        //  テキストペイン設定
        {
          initParam.pTextBoxPaneArray[ 0 ]  = paneHelper.GetTextBoxPane( PANENAME_COMMON_MSG_LOW_006_PANE_TEXTBOX_00 );
          initParam.textBoxPaneNum  = 1;
        }
      }

      m_pLowerCommonMessageObject->InitFunc( initParam, pUseAppHeap->GetDeviceHeap() );
    }
  }

  LowerInfoView::~LowerInfoView()
  {
    m_pLowerCommonMessageObject->EndFunc();
    GFL_SAFE_DELETE( m_pLowerCommonMessageObject );
  }

  bool LowerInfoView::IsDrawing(void) const
  {
    bool result = false;
  
    if( UIView::IsDrawing() )
    {
      result = true;
    }

    return result;
  }

  void LowerInfoView::Update( void )
  {
    if( m_state == STATE_IDLE )
    {
    }

    if( m_state == STATE_INPUT_EVENT_WAIT )
    {
      if( UIView::IsButtonAnimation() == false )
      {
        _SetState( STATE_INPUT_STOP );
      }
    }

    if( m_state == STATE_INPUT_STOP )
    {
    }

    m_pLowerCommonMessageObject->Update();

    NetAppLib::System::NetApplicationViewBase::Update();
  }

  void LowerInfoView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    app::util::AppRenderingManager* pAppRenderingManager = m_pWork->GetAppRenderingManager();
    pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  }

  //  アイドリングを開始
  void LowerInfoView::StartIdling()
  {
    _SetState( STATE_IDLE );
  }

  //------------------------------------------------------------------
  /**
    * @brief   ペインタッチ時を検知
    * @param[in] buttonId ボタンid
    * @return 入力処理を続けるかを返す
    */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult LowerInfoView::OnLayoutPaneEvent( const u32 id )
  {
    //  idle状態でないと入力は受け付けない
    if( m_state != STATE_IDLE )
    {
      return  ENABLE_ACCESS;
    }

    m_tmpPushId = (ButtonId)id;

    _SetState( STATE_INPUT_EVENT_WAIT );

    return  DISABLE_ACCESS;
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
  app::ui::UIInputListener::ListenerResult LowerInfoView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    //  idle状態でないと入力は受け付けない
    if( m_state != STATE_IDLE )
    {
      return  ENABLE_ACCESS;
    }

    //  キャンセルボタン
    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) == true )
    {
      UIView::GetButtonManager()->StartSelectedAct( BUTTON_ID_CANCEL );
      return  DISABLE_ACCESS;
    }

    if( m_pDelegae != NULL )
    {
      if( m_pDelegae->LowerInfoViewDelegata_OnKeyAction( pButton, pKey, pStick ) == true )
      {
        return  DISABLE_ACCESS;
      }
    }

    return  ENABLE_ACCESS;
  }

  void LowerInfoView::_SetState( const State newState )
  {
    m_state = newState;

    if( m_state == STATE_IDLE )
    {
      m_pushId  = m_tmpPushId = BUTTON_ID_NONE;

      //  初期メッセージを設定
      {
        gfl2::str::StrBuf*  pStrBufArray[] =
        {
          &m_pWork->GetMessageUtility()->GetString( GARC_message_kisekae_DAT, kisekae_04_01 ),
        };

        m_pLowerCommonMessageObject->ShowMessageTextPane( pStrBufArray, GFL_NELEMS( pStrBufArray ) );
      }

      UIView::SetInputEnabledAllView( true );
    }
    else
    if( m_state == STATE_INPUT_EVENT_WAIT )
    {
      UIView::SetInputEnabledAllView( false );
    }
    else
    if( m_state == STATE_INPUT_STOP )
    {
      m_pushId  = m_tmpPushId;
      m_tmpPushId = BUTTON_ID_NONE;
    }
  }

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)
