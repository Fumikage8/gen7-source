// ============================================================================
/*
 * @file NBRUpperViewPlayerInfoDefault.cpp
 * @brief 通信対戦受付アプリ 上画面：プレイヤー情報（ルール決まった後版）
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/View/SubView/NBRUpperViewPlayerInfoDefault.h"

#include "Savedata/include/MyStatus.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)

//==========================================================================
/**
 *          プレイヤー表示用ペイン定義
 */
//==========================================================================
const NBRUpperViewPlayerInfoDefault::PlayerPane NBRUpperViewPlayerInfoDefault::m_playerPane[ NBRUpperViewPlayerInfoDefault::PLACE_MAX ] = {
  {// 真ん中
    PANENAME_NBR_MIN_UPP_000_PANE_PLAYERDEFAULT_00,
  },
  {// 左
    PANENAME_NBR_MIN_UPP_000_PANE_PLAYERDEFAULT_01,
  },
  {// 右
    PANENAME_NBR_MIN_UPP_000_PANE_PLAYERDEFAULT_02,
  }
};


// コンストラクタ
NBRUpperViewPlayerInfoDefault::NBRUpperViewPlayerInfoDefault( NetAppLib::System::ApplicationWorkBase* pWorkBase,
                                app::util::G2DUtil* pG2dUtil,
                                app::util::G2DUtil::LAYOUT_WORK* pLayoutWork,
                                NetAppLib::Util::NetAppTrainerIconUtility& netAppTrainerIconUtility ) :
  NBRUpperViewBase( pWorkBase, netAppTrainerIconUtility )
{
  // 親ビューのレイアウトを使う
  Create2D( m_pHeap, pG2dUtil->GetLayoutSystem(), 1,
            pLayoutWork, 1,
            pG2dUtil->GetLayoutResourceID(),
            pG2dUtil->GetMsgData(), pG2dUtil->GetWordSet() );

  // 最初は非表示
  SetAllVisible(false);
}


NBRUpperViewPlayerInfoDefault::~NBRUpperViewPlayerInfoDefault()
{
  Delete2D();
}

//--------------------------------------------------------------------------
/**
* @breif  ビュー全体の表示・非表示
* @param  isVisible         true = 表示, false = 非表示
*/
//--------------------------------------------------------------------------
void NBRUpperViewPlayerInfoDefault::SetAllVisible( const bool isVisible )
{
  // 親ペイン
  GetG2DUtil()->SetPaneVisible( 0, PANENAME_NBR_MIN_UPP_000_PANE_PLAYERINFODEFAULT, isVisible );
  ICHI_PRINT("NBRUpperViewPlayerInfoDefault::SetAllVisible %d \n", isVisible );

  // プレイヤーペイン非表示
  if( isVisible == false )
  {
    GetG2DUtil()->SetPaneVisible( 0, m_playerPane[ PLACE_LEFT ].root, false );
    GetG2DUtil()->SetPaneVisible( 0, m_playerPane[ PLACE_CENTER ].root, false );
    GetG2DUtil()->SetPaneVisible( 0, m_playerPane[ PLACE_RIGHT ].root, false );

    {
      gfl2::lyt::LytWk* pLayoutWork = GetG2DUtil()->GetLayoutWork( 0 );

      gfl2::lyt::LytMultiResID layoutMultiResourceID = GetG2DUtil()->GetLayoutResourceID( 0 );

      gfl2::lyt::LytParts* pPartsPaneLeft   = pLayoutWork->GetPartsPane( m_playerPane[ PLACE_LEFT ].root );
      gfl2::lyt::LytParts* pPartsPaneCenter = pLayoutWork->GetPartsPane( m_playerPane[ PLACE_CENTER ].root );
      gfl2::lyt::LytParts* pPartsPaneRight  = pLayoutWork->GetPartsPane( m_playerPane[ PLACE_RIGHT ].root );

      gfl2::lyt::LytPicture* pWindowFramePaneLeft   = pLayoutWork->GetPicturePane( pPartsPaneLeft, PANENAME_COMMON_PLT_003_PANE_WINDOW_PLAYER_FRAME, &layoutMultiResourceID );
      gfl2::lyt::LytPicture* pWindowFramePaneCenter = pLayoutWork->GetPicturePane( pPartsPaneCenter, PANENAME_COMMON_PLT_003_PANE_WINDOW_PLAYER_FRAME, &layoutMultiResourceID );
      gfl2::lyt::LytPicture* pWindowFramePaneRight  = pLayoutWork->GetPicturePane( pPartsPaneRight, PANENAME_COMMON_PLT_003_PANE_WINDOW_PLAYER_FRAME, &layoutMultiResourceID );

      pWindowFramePaneLeft->SetVisible( false );
      pWindowFramePaneCenter->SetVisible( false );
      pWindowFramePaneRight->SetVisible( false );
    }
  }
}

//--------------------------------------------------------------------------
/**
* @breif      プレイヤーの表示
* @param[in]  place       表示位置
* @param[in]  pMyStatus   表示するプレイヤーのマイステータス
*/
//--------------------------------------------------------------------------
void NBRUpperViewPlayerInfoDefault::SetPlayerData( const Place place, Savedata::MyStatus* pMyStatus )
{
  // 一応チェック
  if( place >= PLACE_MAX )
  {
    GFL_ASSERT(0);
    return;
  }

  // 設定するのは一度だけでよい。接続相手が変わらないから。
  if( GetG2DUtil()->GetPaneVisible(0, m_playerPane[ place ].root) == false )
  {
    GetG2DUtil()->SetPaneVisible( 0, m_playerPane[ place ].root, true );

    gfl2::lyt::LytWk* pLayoutWork = GetG2DUtil()->GetLayoutWork( 0 );

    gfl2::lyt::LytMultiResID layoutMultiResourceID = GetG2DUtil()->GetLayoutResourceID( 0 );

    gfl2::lyt::LytParts* pPartsPane = pLayoutWork->GetPartsPane( m_playerPane[ place ].root );

    gfl2::lyt::LytTextBox* pTextPane = pLayoutWork->GetTextBoxPane( pPartsPane, PANENAME_COMMON_PLT_003_PANE_TEXTBOX_00, &layoutMultiResourceID );

    gfl2::lyt::LytPicture* pPicturePane = pLayoutWork->GetPicturePane( pPartsPane, PANENAME_COMMON_PLT_003_PANE_PLAYERPHOTO, &layoutMultiResourceID );

    gfl2::lyt::LytPicture* pWindowFramePane = pLayoutWork->GetPicturePane( pPartsPane, PANENAME_COMMON_PLT_003_PANE_WINDOW_PLAYER_FRAME, &layoutMultiResourceID );

    // プレイヤー名表示
    GetG2DUtil()->SetRegisterPlayerName( 0, pMyStatus );
    GetG2DUtil()->SetTextBoxPaneStringExpand( pTextPane, msg_nbr_cap_01 );

    // トレーナーアイコン
    {
      if( pPicturePane )
      {
        const u32 iconIndex = static_cast<u32>(place);

        app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData;

        iconObjectData.ConvFromMyStatusIconData( pMyStatus->GetIcon() );

        iconObjectData.pReplacePane             = pPicturePane;
        iconObjectData.pSyncVisiblePane         = pLayoutWork->GetPane( m_playerPane[ place ].root );
        iconObjectData.pInLoadVisibleChangePane = pWindowFramePane;
        iconObjectData.pTimerIconRootPane       = NULL;

        m_NetAppTrainerIconUtility.SetIconObjectData( iconIndex, iconObjectData );
      }
    }
  }

}


//--------------------------------------------------------------------------
/**
 * @breif      プレイヤー状態の設定
 * @param[in]  place     表示位置
 * @param[in]  status    プレイヤー状態
 */
//--------------------------------------------------------------------------
void NBRUpperViewPlayerInfoDefault::SetPlayerStatus( const Place place, PlayerStatus status )
{
  // 一応チェック
  if( place >= PLACE_MAX )
  {
    GFL_ASSERT(0);
    return;
  }

  gfl2::lyt::LytWk* pLayoutWork = GetG2DUtil()->GetLayoutWork( 0 );

  gfl2::lyt::LytMultiResID layoutMultiResourceID = GetG2DUtil()->GetLayoutResourceID( 0 );

  gfl2::lyt::LytParts* pPartsPane = pLayoutWork->GetPartsPane( m_playerPane[ place ].root );

  gfl2::lyt::LytTextBox* pTextPane = pLayoutWork->GetTextBoxPane( pPartsPane, PANENAME_COMMON_PLT_003_PANE_TEXTBOX_01, &layoutMultiResourceID );


  if( status == PLAYER_STATUS_SELECTING )
  {
    // 選んでいる
    GetG2DUtil()->SetTextBoxPaneString( pTextPane, msg_nbr_cap_02 );
  }else
  {
    // 完了
    GetG2DUtil()->SetTextBoxPaneString( pTextPane, msg_nbr_cap_03 );
  }
}



GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
