//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		GameClearSaveMessageWindowView.cpp
 *	@brief  ゲームクリアー時のセーブ画面のメッセージウィンドウ
 *	@author	koji_kawada@gamefreak.co.jp
 *	@date		2016.01.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include  <niji_conv_header/app/game_clear_save/game_clear_save_menu_window.h>
#include  <niji_conv_header/app/game_clear_save/game_clear_save_menu_window_anim_list.h>
#include  <niji_conv_header/app/game_clear_save/game_clear_save_menu_window_pane.h>

#include  "AppLib/include/Tool/app_tool_MenuWindow.h"
#include  "AppLib/include/Tool/CommonMessageWindowView.h"


#include  "GameClearSaveMessageWindowView.h"


namespace
{
  enum  {
    LAYOUTWORK_MessageWindow,

    LAYOUTWORK_MAX
  };
}


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(game_clear_save)


void MessageWindowView::Setup(void* pLayoutBuffer)
{
  GFL_ASSERT(!GetG2DUtil());    /*  create2dされていないこと  */
  GFL_ASSERT(pLayoutBuffer);

  app::util::Heap*  pHeap = m_Param.pHeap;


  /*  setupLayoutSystem  */
  {
    const app::util::G2DUtil::LytResData  lytResDataTable[] =
    {
      { pLayoutBuffer, 0, app::util::G2DUtil::ATTACH_NONE },
    };

    const app::util::G2DUtil::LytwkData   layoutWorkTable[LAYOUTWORK_MAX] =
    {
      {
        0,                                          //!< 使用するリソースの番号         : lytResDataTable
        LYTRES_GAME_CLEAR_SAVE_MENU_WINDOW_GAME_CLEAR_SAVE_MENU_WINDOW_UPPER_BFLYT,   //!< レイアウトデータID ( *_BFLYT ) : niji_conv_header/app/gift/Gift.h
        LA_GAME_CLEAR_SAVE_MENU_WINDOW_GAME_CLEAR_SAVE_MENU_WINDOW_UPPER___NUM,       //!< アニメ数                       : niji_conv_header/app/gift/Gift_anim_list.h
        pLayoutBuffer,                              //!< リソースデータ                 : pWork->ResourceManager->ResourceBuffer
        true,                                       //!< 非表示ペインに行列計算を行うか
        app::util::G2DUtil::SETUP_UPPER,            //!< ディスプレイ
        true                                        //!< 表示設定
      },
    };


    /*
      UIView
    */
    SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
    CreateGraphicsSystem( pHeap, false );

    Create2D(
      pHeap,                            /*  app::util::Heap*    */
      NULL,                             /*  gfl2::lyt::LytSys                 (NULLなら内部で新規に生成する。既存のものを使用する場合にはここで指定する)  */
      LAYOUTWORK_MAX,                   /*  u32                               layoutを扱う数（capacity）。内部ワークの数  */
      lytResDataTable,                  /*  app::util::G2DUtil::LytResData*   リソーステーブル        */
      GFL_NELEMS(lytResDataTable),      /*  u32                               リソーステーブル用素数  */
      layoutWorkTable,                  /*  app::util::G2DUtil::LytwkData*    ワークテーブル          */
      GFL_NELEMS(layoutWorkTable),      /*  u32                               ワークテーブル用素数    */
      (gfl2::str::MsgData*)NULL,
      &m_WordSet  // 使わないので必要ないが、ないとapp_util_2dでアサートが出る
    );


    /*  BGpane取得  */
    m_pPictureBG  = GetG2DUtil()->GetLayoutWork(LAYOUTWORK_MessageWindow)->GetPicturePane(PANENAME_GAME_CLEAR_SAVE_MENU_WINDOW_UPPER_PANE_BG_00);
    m_pPictureBG->SetVisible(false);
  }

  /*  汎用メッセージウインドウ  */
  {
    App::Tool::CommonMessageWindowManipulator::SSetupInfo setupInfo;

    setupInfo.pHeap               = pHeap;
    setupInfo.pTargetView         = this;
    setupInfo.layoutWorkID        = LAYOUTWORK_MessageWindow;
    setupInfo.pHandler            = this;

//    setupInfo.animID_TimerIcon    = LA_MENUWINDOW_MENU_MIN_LOW_000__TIMERICON_KEEP;

    setupInfo.animID_TimerIcon      = LA_GAME_CLEAR_SAVE_MENU_WINDOW_GAME_CLEAR_SAVE_MENU_WINDOW_UPPER__MESSAGE__TIMERICON_KEEP;
    setupInfo.animID_TimerIcon_In   = LA_GAME_CLEAR_SAVE_MENU_WINDOW_GAME_CLEAR_SAVE_MENU_WINDOW_UPPER__MESSAGE__TIMERICON_IN;
    setupInfo.animID_TimerIcon_Out  = LA_GAME_CLEAR_SAVE_MENU_WINDOW_GAME_CLEAR_SAVE_MENU_WINDOW_UPPER__MESSAGE__TIMERICON_OUT;







    setupInfo.animID_MsgCursor    = LA_GAME_CLEAR_SAVE_MENU_WINDOW_GAME_CLEAR_SAVE_MENU_WINDOW_UPPER__MESSAGE__CURSOR_00_KEEP;
    setupInfo.paneID_WindowParts  = PANENAME_GAME_CLEAR_SAVE_MENU_WINDOW_UPPER_PANE_MESSAGE;
    setupInfo.paneID_TextBox2     = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2;
    setupInfo.paneID_NullText1    = PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1;
    setupInfo.paneID_TextBox1a    = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1A;
    setupInfo.paneID_TextBox2a    = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1B;


//    setupInfo.paneID_TimerIcon    = PANENAME_MENU_MIN_LOW_000_PANE_TIMERICON;
    setupInfo.paneID_TimerIcon    = PANENAME_COMMON_MSG_LOW_000_PANE_TIMERICON;



    setupInfo.paneID_MsgCursor    = PANENAME_COMMON_MSG_LOW_000_PANE_CURSOR_00;

    m_MessageWindowManipulator.Setup(setupInfo);
  }
}

void MessageWindowView::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame( m_Param.pRenderingManager, displayNo, LAYOUTWORK_MessageWindow );
}


GFL_NAMESPACE_END(game_clear_save)
GFL_NAMESPACE_END(app)

