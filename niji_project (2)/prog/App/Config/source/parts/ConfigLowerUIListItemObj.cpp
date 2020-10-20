//=============================================================================
/**
 * @file ConfigLowerUIListItemObj.h
 * @brief コンフィグの下UIリスト項目
 * @author yuto_uchida
 * @date 2015.05.19
 */
//=============================================================================
#include "App/config/source/parts/ConfigLowerUIListItemObj.h"

//  nijiのインクルード
#include "AppLib/include/Ui/UIView.h"

#include "niji_conv_header/app/config/res2d/lyt_anim_list.h"
#include "niji_conv_header/app/config/res2d/lyt_pane.h"

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( config )

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  ConfigLowerUIListItemObj::ConfigLowerUIListItemObj()
  {
    _Clear();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  ConfigLowerUIListItemObj::~ConfigLowerUIListItemObj()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  void ConfigLowerUIListItemObj::Initialize( const INIT_PARAM& rInitParma, app::ui::UIView* pRootView, u32 lytWkId, gfl2::lyt::LytMultiResID* pMultiResId )
  {
    GFL_ASSERT( pRootView );
    GFL_ASSERT( pMultiResId );

    //  初期化パラメータをコピー
    m_initParam = rInitParma;

    m_pRootView   = pRootView;
    m_rootLytWkId = lytWkId;

    gfl2::lyt::LytWk* pLayoutWork = m_pRootView->GetLayoutWork( lytWkId );
    GFL_ASSERT( pLayoutWork );

    gfl2::lyt::LytParts*  pRootPartsPane  = pLayoutWork->GetPartsPane( m_initParam.root_parts_pane_id );
    GFL_ASSERT( pRootPartsPane );

    m_pRootPane = pRootPartsPane;

    //  項目ボタンデータ作成
    {
      m_initParam.item_button_data.picture_pane = m_pRootPane;
      m_initParam.item_button_data.bound_pane   = m_pRootPane;
    }

    //  左の矢印ボタンデータ作成
    {
      gfl2::lyt::LytParts*  pArrowRootPane  = pLayoutWork->GetPartsPane( m_pRootPane, PANENAME_CONFIG_BTN_LOW_000_PANE_CURSOR_L, pMultiResId );
      GFL_ASSERT( pArrowRootPane );

      //  矢印オブジェクトペイン取得
      m_initParam.left_arrow_button_data.picture_pane = pLayoutWork->GetPane( pArrowRootPane, PANENAME_COMMON_CUR_LOW_004_PANE_ICON_CUR, pMultiResId );
      GFL_ASSERT( m_initParam.left_arrow_button_data.picture_pane );

      //  タッチ範囲ペイン取得
      m_initParam.left_arrow_button_data.bound_pane = pLayoutWork->GetBoundingPane( pArrowRootPane, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, pMultiResId );
    }

    //  右の矢印ボタンデータ作成
    {
      gfl2::lyt::LytParts*  pArrowRootPane  = pLayoutWork->GetPartsPane( m_pRootPane, PANENAME_CONFIG_BTN_LOW_000_PANE_CURSOR_R, pMultiResId );
      GFL_ASSERT( pArrowRootPane );

      //  矢印オブジェクトペイン取得
      m_initParam.right_arrow_button_data.picture_pane = pLayoutWork->GetPane( pArrowRootPane, PANENAME_COMMON_CUR_LOW_004_PANE_ICON_CUR, pMultiResId );
      GFL_ASSERT( m_initParam.right_arrow_button_data.picture_pane );

      //  タッチ範囲ペイン取得
      m_initParam.right_arrow_button_data.bound_pane = pLayoutWork->GetBoundingPane( pArrowRootPane, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, pMultiResId );
    }

    //  タイトルテキストボックスペイン取得
    {
      m_pTitleTextBoxPane = pLayoutWork->GetTextBoxPane( pRootPartsPane, PANENAME_CONFIG_BTN_LOW_000_PANE_TEXTBOX_00, pMultiResId );
      GFL_ASSERT( m_pTitleTextBoxPane );
    }

    //  データテキストボックスペイン取得
    {
      m_pDataTextBoxPane = pLayoutWork->GetTextBoxPane( pRootPartsPane, PANENAME_CONFIG_BTN_LOW_000_PANE_TEXTBOX_01, pMultiResId );
      GFL_ASSERT( m_pDataTextBoxPane );
    }

    //  カーソル座標ペイン取得
    {
      m_pCursorPosPane  = pLayoutWork->GetPane( pRootPartsPane, PANENAME_CONFIG_BTN_LOW_000_PANE_NULL_CUR, pMultiResId );
      GFL_ASSERT( m_pCursorPosPane );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  void ConfigLowerUIListItemObj::Terminate()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief   更新
    */
  //------------------------------------------------------------------
  void ConfigLowerUIListItemObj::Update()
  {
    _UpdateArrowButtonObj();
  }

  //------------------------------------------------------------------
  /**
    * @brief   アクティブ設定
    */
  //------------------------------------------------------------------
  void ConfigLowerUIListItemObj::SetActive( const bool bFlg )
  {
    app::util::G2DUtil* pG2DUtil  = m_pRootView->GetG2DUtil();

    if( bFlg == true )
    {
      pG2DUtil->StartAnime( m_rootLytWkId, m_initParam.active_arrow_left_anime_id );
      pG2DUtil->StartAnime( m_rootLytWkId, m_initParam.active_arrow_right_anime_id );
    }
    else
    {
      pG2DUtil->StartAnime( m_rootLytWkId, m_initParam.passive_arrow_left_anime_id );
      pG2DUtil->StartAnime( m_rootLytWkId, m_initParam.passive_arrow_right_anime_id );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   表示設定
    */
  //------------------------------------------------------------------
  void ConfigLowerUIListItemObj::SetViaible( const bool bFlg )
  {
    GFL_ASSERT( m_pRootView );

    app::util::G2DUtil* pG2DUtil  = m_pRootView->GetG2DUtil();
    pG2DUtil->SetPaneVisible( m_pRootPane, bFlg );

    app::tool::ButtonManager* pButtonManager  = m_pRootView->GetButtonManager();
    if( pButtonManager )
    {
      if( bFlg )
      {
        if( 0 < m_dataParam.select_idx )
        {
          pButtonManager->SetButtonVisible( m_initParam.left_arrow_button_data.button_id );
        }

        if( m_dataParam.select_idx < ( m_dataParam.select_num - 1 ) )
        {
          pButtonManager->SetButtonVisible( m_initParam.right_arrow_button_data.button_id );
        }
      }
      else
      {
        pButtonManager->SetButtonInvisible( m_initParam.left_arrow_button_data.button_id );
        pButtonManager->SetButtonInvisible( m_initParam.right_arrow_button_data.button_id );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   項目データ設定
    */
  //------------------------------------------------------------------
  void ConfigLowerUIListItemObj::SetData( const DATA_PARAM& rDataParam )
  {
    GFL_ASSERT( m_pRootView );

    m_dataParam = rDataParam;
    
    //  選択しているテキスト更新
    UpdateText();

    //  矢印ボタンオブジェクト更新
    _UpdateArrowButtonObj();
  }

  //------------------------------------------------------------------
  /**
    * @brief   テキスト更新
    */
  //------------------------------------------------------------------
  void ConfigLowerUIListItemObj::UpdateText()
  {
    //  選択しているテキスト設定
    //  テキスト選択データがない場合は何もしない
    if( 0 < m_dataParam.select_num )
    {
      _SetSelectText( m_dataParam.a_select_text_id[ m_dataParam.select_idx ] );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   項目データリストを移動
    * @param   moveNum : データリストをずらす移動する値
    * @note
    *          SetData()関数であらかじめデータを設定しないと壊れる
    */
  //------------------------------------------------------------------
  const ConfigLowerUIListItemObj::DATA_PARAM* ConfigLowerUIListItemObj::MoveSelectData( const s32 moveNum )
  {
    //  選択項目を移動(s32形にしたのは-の値が入るから)
    s32 nowSelectIdx  = (s32)m_dataParam.select_idx;
    nowSelectIdx += moveNum;

    if( nowSelectIdx < 0 )
    {
      return NULL;
    }
    else if( (s32)m_dataParam.select_num <= nowSelectIdx )
    {
      return NULL;
    }

    //  u32型に変更したのはASSERTチェックはひとつでよいから
    m_dataParam.select_idx = (u32)nowSelectIdx;
    GFL_ASSERT( m_dataParam.select_idx < m_dataParam.select_num );

    //  矢印ボタンオブジェクト更新
    _UpdateArrowButtonObj();

    //  選択テキストを更新
    UpdateText();

    return  &m_dataParam;
  }

  //  矢印ボタンの更新
  void ConfigLowerUIListItemObj::_UpdateArrowButtonObj()
  {
    //  カレント位置がリストの端に到達したらボタンを押せないようにする
    app::tool::ButtonManager* pButtonManager  = m_pRootView->GetButtonManager();

    if( m_dataParam.select_idx <= 0 )
    {
      //  左の矢印を押せないように
      pButtonManager->SetButtonInvisible( m_initParam.left_arrow_button_data.button_id );

      //  右の矢印を押せるように
      pButtonManager->SetButtonVisible( m_initParam.right_arrow_button_data.button_id );
    }
    else if( ( m_dataParam.select_num - 1 ) <= m_dataParam.select_idx )
    {
      //  右の矢印を押せないように
      pButtonManager->SetButtonInvisible( m_initParam.right_arrow_button_data.button_id );

      //  左の矢印を押せるように
      pButtonManager->SetButtonVisible( m_initParam.left_arrow_button_data.button_id );
    }
    else
    {
      //  左右の矢印を押せるように
      pButtonManager->SetButtonVisible( m_initParam.left_arrow_button_data.button_id );
      pButtonManager->SetButtonVisible( m_initParam.right_arrow_button_data.button_id );
    }
  }

  //  選択テキストを指定(かな・漢字の切り替えで呼び出す)
  void ConfigLowerUIListItemObj::_SetSelectText( const u32 textId )
  {
    app::util::G2DUtil* pG2DUtil  = m_pRootView->GetG2DUtil();

    //  項目のテキスト設定
    pG2DUtil->SetTextBoxPaneString( m_pTitleTextBoxPane, m_dataParam.title_text_id );

    //  選択しているテキスト設定
    GFL_ASSERT( m_dataParam.select_idx < m_dataParam.select_num );
    pG2DUtil->SetTextBoxPaneString( m_pDataTextBoxPane, m_dataParam.a_select_text_id[ m_dataParam.select_idx ] );
  }

GFL_NAMESPACE_END( config )
GFL_NAMESPACE_END( app )
