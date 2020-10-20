// ============================================================================
/*
 * @file NBRUpperViewVs.cpp
 * @brief 通信対戦受付アプリ 上画面：VS
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/View/SubView/NBRUpperViewVs.h"

#include "Savedata/include/MyStatus.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)


NBRUpperViewVs::NBRUpperViewVs( NetAppLib::System::ApplicationWorkBase* pWorkBase,
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

  // テキスト初期化
  InitializeText();

  // 最初は非表示
  SetAllVisible(false);
}


NBRUpperViewVs::~NBRUpperViewVs()
{
  Delete2D();
}

//--------------------------------------------------------------------------
/**
* @breif  ビュー全体の表示・非表示
* @param  isVisible         true = 表示, false = 非表示
*/
//--------------------------------------------------------------------------
void NBRUpperViewVs::SetAllVisible( const bool isVisible )
{
  // 親ペイン
  GetG2DUtil()->SetPaneVisible( 0, PANENAME_NBR_MIN_UPP_000_PANE_VS, isVisible );
  ICHI_PRINT("NBRUpperViewVs::SetAllVisible %d \n", isVisible );

  if( isVisible == false )
  {
    u32 windowFramePaneId[] = {
      PANENAME_NBR_MIN_UPP_000_PANE_WINDOW_PLAYER_FRAME_00,
      PANENAME_NBR_MIN_UPP_000_PANE_WINDOW_PLAYER_FRAME_01,
      PANENAME_NBR_MIN_UPP_000_PANE_WINDOW_PLAYER_FRAME_02,
      PANENAME_NBR_MIN_UPP_000_PANE_WINDOW_PLAYER_FRAME_03,
    };

    for( u32 i = 0; i < 4; ++i )
    {
      gfl2::lyt::LytPicture* pWindowFramePane = GetG2DUtil()->GetLayoutWork(LAYOUT_WORK_ID_UPPER)->GetPicturePane( windowFramePaneId[i] );
      pWindowFramePane->SetVisible( false );
    }
  }
}

//--------------------------------------------------------------------------
/**
* @breif      プレイヤーの表示
* @param[in]  no          立ち位置Index
* @param[in]  playerData  表示するプレイヤーデータ
*/
//--------------------------------------------------------------------------
void NBRUpperViewVs::SetPlayerData( u8 no, Savedata::MyStatus* pMyStatus )
{
  // レイアウトは左上から右下に向かって0,1,2,3となっている
  u32 paneId[] = {
    PANENAME_NBR_MIN_UPP_000_PANE_TEXTPLAYERNAME_00,
    PANENAME_NBR_MIN_UPP_000_PANE_TEXTPLAYERNAME_01,
    PANENAME_NBR_MIN_UPP_000_PANE_TEXTPLAYERNAME_02,
    PANENAME_NBR_MIN_UPP_000_PANE_TEXTPLAYERNAME_03,
  };
  u32 picturePaneId[] = {
    PANENAME_NBR_MIN_UPP_000_PANE_PLAYERPHOTO_00,
    PANENAME_NBR_MIN_UPP_000_PANE_PLAYERPHOTO_01,
    PANENAME_NBR_MIN_UPP_000_PANE_PLAYERPHOTO_02,
    PANENAME_NBR_MIN_UPP_000_PANE_PLAYERPHOTO_03,
  };
  u32 windowFramePaneId[] = {
    PANENAME_NBR_MIN_UPP_000_PANE_WINDOW_PLAYER_FRAME_00,
    PANENAME_NBR_MIN_UPP_000_PANE_WINDOW_PLAYER_FRAME_01,
    PANENAME_NBR_MIN_UPP_000_PANE_WINDOW_PLAYER_FRAME_02,
    PANENAME_NBR_MIN_UPP_000_PANE_WINDOW_PLAYER_FRAME_03,
  };

  // 立ち位置Indexをレイアウトの並びに合うように変換
  if( no == 1 ){ no = 2; }
  else if( no == 2 ){ no = 1; }

  if( pMyStatus != NULL )
  {
    // プレイヤー名表示
    GetG2DUtil()->SetRegisterPlayerName( 0, pMyStatus );
    GetG2DUtil()->SetTextBoxPaneStringExpand( LAYOUT_WORK_ID_UPPER, paneId[ no ], msg_nbr_cap_01 );
    // トレーナーアイコン
    {
      gfl2::lyt::LytPicture* pPicturePane  = GetG2DUtil()->GetLayoutWork(LAYOUT_WORK_ID_UPPER)->GetPicturePane( picturePaneId[no] );
      gfl2::lyt::LytPicture* pWindowFramePane  = GetG2DUtil()->GetLayoutWork(LAYOUT_WORK_ID_UPPER)->GetPicturePane( windowFramePaneId[no] );
      if( pPicturePane )
      {
        const u32 iconIndex = static_cast<u32>(no);
        pPicturePane->SetVisible( true ); 

        app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData;

        iconObjectData.ConvFromMyStatusIconData( pMyStatus->GetIcon() );

        iconObjectData.pReplacePane             = pPicturePane;
        iconObjectData.pSyncVisiblePane         = NULL;
        iconObjectData.pInLoadVisibleChangePane = pWindowFramePane;
        iconObjectData.pTimerIconRootPane       = NULL;

        m_NetAppTrainerIconUtility.SetIconObjectData( iconIndex, iconObjectData );
      }
    }
  }
  else
  {
    // 破線表示
    GetG2DUtil()->SetTextBoxPaneString( LAYOUT_WORK_ID_UPPER, paneId[ no ], msg_nbr_cap_11 );

    gfl2::lyt::LytPicture* pPicturePane  = GetG2DUtil()->GetLayoutWork(LAYOUT_WORK_ID_UPPER)->GetPicturePane( picturePaneId[no] );
    if( pPicturePane )
    {
      pPicturePane->SetVisible( false ); 
    }

    // @fix GFNMCat[4507]：枠線が残るので非表示にする
    gfl2::lyt::LytPicture* pWindowFramePane  = GetG2DUtil()->GetLayoutWork(LAYOUT_WORK_ID_UPPER)->GetPicturePane( windowFramePaneId[no] );
    if( pWindowFramePane )
    {
      pWindowFramePane->SetVisible( false ); 
    }
  }
}


//--------------------------------------------------------------------------
/**
* @breif  テキストペイン内容初期化
*/
//--------------------------------------------------------------------------
void NBRUpperViewVs::InitializeText()
{
  // 数字１、２、３、４
  SetTextboxPaneMessage( LAYOUT_WORK_ID_UPPER, PANENAME_NBR_MIN_UPP_000_PANE_TEXTPLAYERNUMB_00, msg_nbr_cap_06 );
  SetTextboxPaneMessage( LAYOUT_WORK_ID_UPPER, PANENAME_NBR_MIN_UPP_000_PANE_TEXTPLAYERNUMB_01, msg_nbr_cap_07 );
  SetTextboxPaneMessage( LAYOUT_WORK_ID_UPPER, PANENAME_NBR_MIN_UPP_000_PANE_TEXTPLAYERNUMB_02, msg_nbr_cap_06 );
  SetTextboxPaneMessage( LAYOUT_WORK_ID_UPPER, PANENAME_NBR_MIN_UPP_000_PANE_TEXTPLAYERNUMB_03, msg_nbr_cap_07 );

  GetG2DUtil()->SetPaneVisible( 0, PANENAME_NBR_MIN_UPP_000_PANE_TEXTPLAYERNUMB_00, false );
  GetG2DUtil()->SetPaneVisible( 0, PANENAME_NBR_MIN_UPP_000_PANE_TEXTPLAYERNUMB_01, false );
  GetG2DUtil()->SetPaneVisible( 0, PANENAME_NBR_MIN_UPP_000_PANE_TEXTPLAYERNUMB_02, false );
  GetG2DUtil()->SetPaneVisible( 0, PANENAME_NBR_MIN_UPP_000_PANE_TEXTPLAYERNUMB_03, false );

  SetPlayerData( 0, NULL );
  SetPlayerData( 1, NULL );
  SetPlayerData( 2, NULL );
  SetPlayerData( 3, NULL );
}

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
