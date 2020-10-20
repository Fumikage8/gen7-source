//=============================================================================
/**
 * @file ConfigUpperUIDraw.h
 * @brief コンフィグの上UI画面
 * @author yuto_uchida
 * @date 2015.05.08
 */
//=============================================================================
#include "App/config/source/parts/ConfigUpperUIDraw.h"

#include "niji_conv_header/app/config/res2d/lyt.h"
#include "niji_conv_header/app/config/res2d/lyt_anim_list.h"
#include "niji_conv_header/app/config/res2d/lyt_pane.h"

#include "AppLib/include/Tool/CommonMessageWindowManipulator.h"

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( config )

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  ConfigUpperUIDraw::ConfigUpperUIDraw( ::app::util::Heap* pUtilHeap ) :
    UIView( pUtilHeap, pUtilHeap->GetDeviceAllocator(), pUtilHeap->GetDeviceAllocator() )
  {
    _Clear();

    m_pUtilHeap = pUtilHeap;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  ConfigUpperUIDraw::~ConfigUpperUIDraw()
  {
    GFL_SAFE_DELETE( m_pCommonMessageWindow );

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  void ConfigUpperUIDraw::Initialize( const INIT_PARAM& param )
  {
    m_initParam = param;

    _Initialize2D( m_initParam.pLytBuff );

    //  メッセージウィンドウ作成
    {
      m_pCommonMessageWindow  = GFL_NEW( m_pUtilHeap->GetDeviceHeap() ) App::Tool::CommonMessageWindowManipulator();

      App::Tool::CommonMessageWindowManipulator::SSetupInfo  setupInfo;
      //  作成に必要なパラメータを設定
      {
        setupInfo.pHeap             = m_pUtilHeap;
        setupInfo.pTargetView       = this;
        setupInfo.layoutWorkID      = LYT_WK_TYPE_SCREEN;
        setupInfo.pHandler          = NULL;

        setupInfo.paneID_WindowParts  = PANENAME_CONFIG_MIN_UPP_000_PANE_MESSAGE;
        setupInfo.paneID_TextBox2     = PANENAME_COMMON_MSG_UPP_000_PANE_TEXTBOX_00;

        /**<  ストリーム表示TextBoxPaneがまとめられているPaneのID、即時表示との切り替えに使う  */
        setupInfo.paneID_NullText1    = PANENAME_COMMON_MSG_UPP_000_PANE_NULL_TEXT1;

        /**<  ストリーム表示TextBoxPane1/2                                      */
        setupInfo.paneID_TextBox1a    = PANENAME_COMMON_MSG_UPP_000_PANE_TEXTBOX_01;

        /**<  ストリーム表示TextBoxPane2/2                                      */
        setupInfo.paneID_TextBox2a    = PANENAME_COMMON_MSG_UPP_000_PANE_TEXTBOX_02;
      }

      m_pCommonMessageWindow->Setup( setupInfo );
    }

    //  背景のBGループ再生
    {
      UIView::GetG2DUtil()->StartAnime( LYT_WK_TYPE_SCREEN, LA_LYT_CONFIG_MIN_UPP_000__BG_BG_LOOP_00 );
    }

    UIView::SetVisible( false );
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  void ConfigUpperUIDraw::Terminate()
  {
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //-------------------------------------------------------------------------
  void ConfigUpperUIDraw::Update()
  {
    _UpdateState();

    UIView::Update();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 描画
    */
  //-------------------------------------------------------------------------
  void ConfigUpperUIDraw::Draw(  gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( pG2DUtil == NULL ) { return; }

    m_pCommonMessageWindow->UpdateWindow();

    for( s32 i = 0; i < LYT_WK_TYPE_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderingManager, displayNo, i );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   ヘルプメッセージ設定
    * @param   textId    : 表示するテキストid
    * @param   msgSpeed  : メッセージスピードタイプ
    */
  //------------------------------------------------------------------
  void ConfigUpperUIDraw::SetHelpMessage( const u32 textId, Savedata::ConfigSave::MSG_SPEED msgSpeed )
  {
    GFL_ASSERT( m_initParam.pMsgData );

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    GFL_ASSERT( pG2DUtil );

    //  テキストスピード設定
    m_pCommonMessageWindow->SetMessageSpeed( msgSpeed );

    //  メッセージを表示
    {
      gfl2::str::StrBuf* pTempbuf = pG2DUtil->GetTempStrBuf( 0 );
      m_initParam.pMsgData->GetString( textId, *pTempbuf );

      if( msgSpeed == Savedata::ConfigSave::MSG_NO_WAIT )
      {
        m_pCommonMessageWindow->ShowMessage( pTempbuf, true );
      }
      else
      {
        m_pCommonMessageWindow->ShowMessage( pTempbuf );
      }
    }

    //  UpdateText()でテキストが変わるので、設定したテキストidを保存しないとだめ
    m_nowHelpTextId = textId;
  }

  //------------------------------------------------------------------
  /**
    * @brief   テキスト更新
    */
  //------------------------------------------------------------------
  void ConfigUpperUIDraw::UpdateText( gfl2::str::MsgData* pMsgData )
  {
    m_initParam.pMsgData  = pMsgData;

    SetHelpMessage( m_nowHelpTextId, Savedata::ConfigSave::MSG_NO_WAIT );
  }

  /**
    * @brief レイアウトワークを生成する
    */
  void ConfigUpperUIDraw::_Initialize2D( void* pAppLytBuff )
  {
    //  2D画面作成
    {
      // アーカイブIDテーブル
      const app::util::G2DUtil::LytResData resTbl[] =
      {
        {  pAppLytBuff, 32, (u32)app::util::G2DUtil::ATTACH_ROOT, },
      };

      //  作成するレイアウトワーク
      const app::util::G2DUtil::LytwkData setupData[ LYT_WK_TYPE_MAX ] =
      {
          //  選択画面
          {
            0,
            LYTRES_LYT_CONFIG_MIN_UPP_000_BFLYT,
            LA_LYT_CONFIG_MIN_UPP_000___NUM,
            pAppLytBuff,
            true,
            app::util::G2DUtil::SETUP_UPPER,
            true
          },
      };

      UIView::Create2D( m_pUtilHeap, NULL, 32, resTbl, GFL_NELEMS(resTbl), setupData, GFL_NELEMS(setupData), m_initParam.pMsgData, m_initParam.pWordSet );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void ConfigUpperUIDraw::_UpdateState()
  {
    if( m_state == STATE_TYPE_IN )
    {
      _SetState( STATE_TYPE_UPDATE );
    }

    if( m_state == STATE_TYPE_UPDATE )
    {
    }

    if( m_state == STATE_TYPE_OUT )
    {
      _SetState( STATE_TYPE_END );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void ConfigUpperUIDraw::_SetState( const StateType newStateType )
  {
    GFL_ASSERT( m_state != newStateType );

    m_state = newStateType;

    if( m_state == STATE_TYPE_NONE )
    {
    }
    else if( m_state == STATE_TYPE_IN )
    {
      UIView::SetVisible( true );
    }
    else if( m_state == STATE_TYPE_UPDATE )
    {
    }
    else if( m_state == STATE_TYPE_OUT )
    {
    }
    else if( m_state ==  STATE_TYPE_END )
    {
      UIView::SetVisible( false );
    }
  }

GFL_NAMESPACE_END( config )
GFL_NAMESPACE_END( app )
