//==============================================================================
/**
 @file    ZukanViewRegisterComment.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の登録画面に出すコメント層のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>

#include <str/include/gfl2_StrBuf.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <Sound/include/Sound.h>

// 図鑑登録の呼び出しEventのインクルード
#include <App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h>

// 図鑑登録チェッカーのインクルード
#include <PokeTool/include/PokeToolPokeGet.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/register_comment.h>
#include <niji_conv_header/app/zukan/register_comment_pane.h>
#include <niji_conv_header/app/zukan/register_comment_anim_list.h>

#include <niji_conv_header/message/msg_zukan.h>
#include <niji_conv_header/message/msg_zukan_hyouka.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanDataManager.h"
#include "ZukanUtil.h"
#include "ZukanTool.h"
#include "ZukanContents.h"
#include "ZukanFrameRegister.h"

#include "ZukanViewRegisterComment.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewRegisterCommentListener
 @brief     図鑑の登録画面に出すコメント層のビューのリスナー
 */
//==============================================================================
class ZukanViewRegisterCommentListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewRegisterCommentListener);

public:
  ZukanViewRegisterCommentListener(ZukanViewRegisterComment* view);
  virtual ~ZukanViewRegisterCommentListener();

  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

public:
  ZukanViewRegisterComment* m_view;
};


//==============================================================================
//
//  ZukanViewRegisterCommentListener
//
//==============================================================================
ZukanViewRegisterCommentListener::ZukanViewRegisterCommentListener(ZukanViewRegisterComment* view)
  : app::ui::UIInputListener(),
    m_view(view)
{}
ZukanViewRegisterCommentListener::~ZukanViewRegisterCommentListener()
{}

app::ui::UIInputListener::ListenerResult ZukanViewRegisterCommentListener::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( pTouchPanel->IsTouchTrigger() )
  {
    m_view->ReqEnd();
  }
  return app::ui::UIInputListener::DISABLE_ACCESS;
}
app::ui::UIInputListener::ListenerResult ZukanViewRegisterCommentListener::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( pButton->IsTrigger(gfl2::ui::BUTTON_A) || pButton->IsTrigger(gfl2::ui::BUTTON_B) )
  {
    m_view->ReqEnd();
  }
  return app::ui::UIInputListener::DISABLE_ACCESS;
}




//==============================================================================
//
//  ZukanViewRegisterComment
//
//==============================================================================
ZukanViewRegisterComment::ZukanViewRegisterComment(const ZukanSceneParam& param, bool registerIsBingo, u32 registerLegendLevel)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
    m_param(param),
    m_resultListener(NULL),
    m_viewListener(NULL),
    m_mesCursorPane(NULL),
    m_mesCursorVisible(false),
    m_strBuf(NULL),
    m_msgNum(0),
    m_msgNo(0),
    m_dispState(DISP_STATE_OFF),
    m_dispStateCount(0),
    m_rotomVoice(NULL),
    m_registerIsBingo(registerIsBingo),
    m_registerLegendLevel(registerLegendLevel)
{
  // 初期化
  {
    for(u32 i=0; i<2; ++i)
    {
      m_mesWinTextPane[i] = NULL;
    }
    for(u32 i=0; i<MSG_NUM_MAX; ++i)
    {
      m_msgIdList[i] = 0;
    }
    for(u32 i=0; i<ZukanType::INFO_TYPE_NUM; ++i)
    {
      m_achievementTopPane[i]   = NULL;
      m_achievementZukanPane[i] = NULL;
      m_achievementRatePane[i]  = NULL;
    }
  }

  // str
  {
    m_strBuf = GFL_NEW(m_param.appHeap->GetDeviceHeap()) gfl2::str::StrBuf( STR_BUF_CHAR_NUM_MAX, m_param.appHeap->GetDeviceHeap() );
  }

  // ZukanViewRegisterCommentListener
  {
    m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewRegisterCommentListener(this);
    this->SetInputListener(m_viewListener);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  }

  // UIView
  {
    m_param.parentView->AddSubView(this);  // app::ui::UIViewのメンバ
  }

  // レイアウト
  {
    this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ
 
    void* buf = m_param.dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_REGISTER_COMMENT);
    if(buf)
    {
      const app::util::G2DUtil::LytResData resTbl[] =
      {
        // res_0
        {
          buf,
          0,
          app::util::G2DUtil::ATTACH_ROOT
        },
      };
     
      const app::util::G2DUtil::LytwkData wkTbl[] =
      {
        // wk_0
        {
          // 上画面
          0,  // res_0
          LYTRES_REGISTER_COMMENT_ZUKAN_CMP_UPP_000_BFLYT,
          LA_REGISTER_COMMENT_ZUKAN_CMP_UPP_000___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
      };
     
      this->Create2D(  // 親であるapp::ui::UIViewのメンバ
          m_param.appHeap,
          m_param.lytSys,
          1,  // wk_0の1個だけ 
          resTbl,
          GFL_NELEMS(resTbl), 
          wkTbl,
          GFL_NELEMS(wkTbl), 
          app::ui::UIView::UNUSE_MESSAGE_DATA
      );
     
      // ページ内容生成
      this->createPage();

      this->setupMsgIdList();

      // インアニメ
      this->startInSeq();
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。データDATA_ID_ZUKAN_RES2Dの読み込みが終わってからフレームをつくるように呼び出し元でしているので。
    }
  }

  // ロトムボイス
  m_rotomVoice = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanRotomVoice();
  m_rotomVoice->Start(true, 0);
}
ZukanViewRegisterComment::~ZukanViewRegisterComment()
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  pG2DUtil->DeleteMsgCursor();
  pG2DUtil->DeleteMessageWindow();
  
  // ロトムボイス
  GFL_SAFE_DELETE(m_rotomVoice);

  // UIView
  this->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ

  // レイアウト
  this->Delete2D();  // 親であるapp::ui::UIViewのメンバ

  // ZukanViewRegisterCommentListener
  GFL_SAFE_DELETE(m_viewListener);

  // str
  GFL_SAFE_DELETE(m_strBuf);
}

void ZukanViewRegisterComment::Update(void)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ

  switch(m_dispState)
  {
  case DISP_STATE_ON_IN:
    {
      if( pG2DUtil->IsAnimeEnd(0, LA_REGISTER_COMMENT_ZUKAN_CMP_UPP_000_IN) )  // wk_0
      {
        pG2DUtil->CreateMessageWindow(
            GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(),
            0,  // wk_0
            m_mesWinTextPane[0],
            m_mesWinTextPane[1],
            m_param.appHeap
        );
        pG2DUtil->CreateMsgCursor(
            m_param.appHeap,
            m_mesCursorPane
        );
        pG2DUtil->StartAnime( 0, LA_REGISTER_COMMENT_ZUKAN_CMP_UPP_000__CURSOR_00_KEEP );  // wk_0  // メッセージ送りのカーソル
 
        /*
        {
          ZukanTool::ExpandStrBufParam expandStrBufParam;
          expandStrBufParam.dstStrBuf        = m_strBuf;
          expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
          expandStrBufParam.srcMsgStrId      = msg_zkn_17_01;
          m_param.tool->ExpandStrBuf(expandStrBufParam);
        }
        */

        {
          ZukanTool::ExpandStrBufParam expandStrBufParam;
          expandStrBufParam.dstStrBuf        = m_strBuf;
          expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN_HYOUKA;
          expandStrBufParam.srcMsgStrId      = m_msgIdList[m_msgNo];
          m_param.tool->ExpandStrBuf(expandStrBufParam);
        }
        ++m_msgNo;
 
        pG2DUtil->SetMessage( m_strBuf );
        //pG2DUtil->SetMessageFinishMode( gfl2::str::StrWin::FINISH_NONE, 0 );
        pG2DUtil->SetMessageFinishMode( gfl2::str::StrWin::FINISH_USER, 0 );
        m_mesCursorVisible = false;
        pG2DUtil->SetMsgCursorUserVisible(false, false);

        // 増えたところをアニメさせる演出
        {
          const ZukanContents::PokeData* pokeData = m_param.contents->GetCurrDispPokeData();

          if(pokeData->alolaNo   != 0) pG2DUtil->StartAnime( 0, LA_REGISTER_COMMENT_ZUKAN_CMP_UPP_000_MATH_00 );  // wk_0
          if(pokeData->island1No != 0) pG2DUtil->StartAnime( 0, LA_REGISTER_COMMENT_ZUKAN_CMP_UPP_000_MATH_01 );  // wk_0
          if(pokeData->island2No != 0) pG2DUtil->StartAnime( 0, LA_REGISTER_COMMENT_ZUKAN_CMP_UPP_000_MATH_02 );  // wk_0
          if(pokeData->island3No != 0) pG2DUtil->StartAnime( 0, LA_REGISTER_COMMENT_ZUKAN_CMP_UPP_000_MATH_03 );  // wk_0
          if(pokeData->island4No != 0) pG2DUtil->StartAnime( 0, LA_REGISTER_COMMENT_ZUKAN_CMP_UPP_000_MATH_04 );  // wk_0
        }

        this->changeDispState(DISP_STATE_ON_WORK);
      }
      break;
    }
  case DISP_STATE_ON_WORK:
    {
      gfl2::str::StrWin::Result mesResult = pG2DUtil->PrintMessage();
      if( mesResult == gfl2::str::StrWin::RES_DONE )
      {
        if(m_mesCursorVisible==false && m_msgNo<m_msgNum)
        {
          pG2DUtil->SetMsgCursorUserVisible(true, true);
          m_mesCursorVisible = true;
        }
      }
      //if( mesResult == gfl2::str::StrWin::RES_DONE )  // gfl2::str::StrWin::FinishTypeがFINISH_NONEのとき
      if( mesResult == gfl2::str::StrWin::RES_FINISH )  // gfl2::str::StrWin::FinishTypeがFINISH_USERのとき
      {
        pG2DUtil->SetMsgCursorUserVisible(true, false);
        Sound::PlaySE(SEQ_SE_MESSAGE);

        // ウィンドウ消さずに2回目のメッセージか、消して2回目のメッセージか？
        if(m_msgNo < m_msgNum)
        {
          pG2DUtil->DeleteMsgCursor();
          pG2DUtil->DeleteMessageWindow();
          
          // 次のメッセージ
          pG2DUtil->CreateMessageWindow(
              GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(),
              0,  // wk_0
              m_mesWinTextPane[0],
              m_mesWinTextPane[1],
              m_param.appHeap
          );
          pG2DUtil->CreateMsgCursor(
              m_param.appHeap,
              m_mesCursorPane
          );
          pG2DUtil->StartAnime( 0, LA_REGISTER_COMMENT_ZUKAN_CMP_UPP_000__CURSOR_00_KEEP );  // wk_0  // メッセージ送りのカーソル
 
          {
            ZukanTool::ExpandStrBufParam expandStrBufParam;
            expandStrBufParam.dstStrBuf        = m_strBuf;
            expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN_HYOUKA;
            expandStrBufParam.srcMsgStrId      = m_msgIdList[m_msgNo];
            m_param.tool->ExpandStrBuf(expandStrBufParam);
          }
          ++m_msgNo;
          
          pG2DUtil->SetMessage( m_strBuf );
          pG2DUtil->SetMessageFinishMode( gfl2::str::StrWin::FINISH_USER, 0 );
          m_mesCursorVisible = false;
          pG2DUtil->SetMsgCursorUserVisible(false, false);
        }
        else
        {
          this->changeDispState(DISP_STATE_ON_WAIT_END);
        }
      } 
      break;
    }
  case DISP_STATE_ON_WAIT_END:
    {
      this->ReqEnd();
      break;
    }
  }

  app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ
  m_rotomVoice->Update();
}
void ZukanViewRegisterComment::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 0, gfl2::lyt::ORDER_NORMAL, 1 );  // wk_0
}

void ZukanViewRegisterComment::ReqEnd(void)
{
  if(m_dispState == DISP_STATE_ON_WAIT_END)
  {
    m_resultListener->Req(0);
  }
}

void ZukanViewRegisterComment::createPage(void)
{
  app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  gfl2::lyt::LytWk*        lytWk_0  = this->GetLayoutWork( 0 );  // wk_0  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytMultiResID lytRes_0 = pG2DUtil->GetLayoutResourceID( 0 );  // res_0

  //const ZukanContents::PokeData* pokeData = m_param.contents->GetCurrDispPokeData();

  // テキスト
  {
    static const u32 ZUKAN_PANE_ID[ZukanType::INFO_TYPE_NUM] =
    {
      PANENAME_ZUKAN_CMP_UPP_000_PANE_TEXTBOX_00,
      PANENAME_ZUKAN_CMP_UPP_000_PANE_TEXTBOX_02,
      PANENAME_ZUKAN_CMP_UPP_000_PANE_TEXTBOX_04,
      PANENAME_ZUKAN_CMP_UPP_000_PANE_TEXTBOX_06,
      PANENAME_ZUKAN_CMP_UPP_000_PANE_TEXTBOX_08,
    };
    static const u32 RATE_PANE_ID[ZukanType::INFO_TYPE_NUM] =
    {
      PANENAME_ZUKAN_CMP_UPP_000_PANE_TEXTBOX_01,
      PANENAME_ZUKAN_CMP_UPP_000_PANE_TEXTBOX_03,
      PANENAME_ZUKAN_CMP_UPP_000_PANE_TEXTBOX_05,
      PANENAME_ZUKAN_CMP_UPP_000_PANE_TEXTBOX_07,
      PANENAME_ZUKAN_CMP_UPP_000_PANE_TEXTBOX_09,
    };

    static const u32 ZUKAN_MSG_ID[ZukanType::INFO_TYPE_NUM] =
    {
      msg_zkn_02_01,
      msg_zkn_02_02,
      msg_zkn_02_03,
      msg_zkn_02_04,
      msg_zkn_02_05,
    };
 
    pG2DUtil->SetMessageData( m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN), m_param.tool->GetWordSet() );
    {
      for(u32 i=0; i<ZukanType::INFO_TYPE_NUM; ++i)
      {
        gfl2::lyt::LytTextBox* zukanTextPane = lytWk_0->GetTextBoxPane(ZUKAN_PANE_ID[i]);
        gfl2::lyt::LytTextBox* rateTextPane  = lytWk_0->GetTextBoxPane(RATE_PANE_ID[i]);

        m_achievementZukanPane[i] = zukanTextPane;
        m_achievementRatePane[i]  = rateTextPane;
          
        pG2DUtil->SetTextBoxPaneString(zukanTextPane, ZUKAN_MSG_ID[i]);
        pG2DUtil->SetTextBoxPaneNumber(rateTextPane, msg_zkn_01_04, m_param.contents->GetAchievementRate(static_cast<ZukanType::InfoType>(i)), 3, 0, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU);

        // 増えたところを青文字にする演出はなくなったのでコメントアウト
        //nw::ut::Color8 upColor(17, 191, 246, 255);
        //if(pokeData->alolaNo   != 0) rateTextPane->SetTextColor(upColor, upColor);
        //if(pokeData->island1No != 0) rateTextPane->SetTextColor(upColor, upColor);
        //if(pokeData->island2No != 0) rateTextPane->SetTextColor(upColor, upColor);
        //if(pokeData->island3No != 0) rateTextPane->SetTextColor(upColor, upColor);
        //if(pokeData->island4No != 0) rateTextPane->SetTextColor(upColor, upColor);
      }
    }
    pG2DUtil->SetMessageData(NULL, NULL);
  }

  // メッセージウィンドウ
  {
    m_mesWinTextPane[0] = lytWk_0->GetTextBoxPane(PANENAME_ZUKAN_CMP_UPP_000_PANE_TEXTBOX_10);
    m_mesWinTextPane[1] = lytWk_0->GetTextBoxPane(PANENAME_ZUKAN_CMP_UPP_000_PANE_TEXTBOX_11);
    m_mesCursorPane = lytWk_0->GetPane(PANENAME_ZUKAN_CMP_UPP_000_PANE_CURSOR_00);
    
    // メッセージウィンドウ用のテキストペインにグラフィッカーさんが入れておいたダミー文字列が
    // 最初にちらっと見えてしまうので、空文字列をセットして消しておく。
    m_strBuf->Clear(); 
    for(u32 i=0; i<2; ++i)
    {
      pG2DUtil->SetTextBoxPaneString( m_mesWinTextPane[i], m_strBuf );
    }
  }

  // 達成率ペインの表示/非表示
  {
    static const u32 ACHIEVEMENT_TOP_PANE_ID[ZukanType::INFO_TYPE_NUM] =
    {
      PANENAME_ZUKAN_CMP_UPP_000_PANE_NULL_00,
      PANENAME_ZUKAN_CMP_UPP_000_PANE_NULL_01,
      PANENAME_ZUKAN_CMP_UPP_000_PANE_NULL_02,
      PANENAME_ZUKAN_CMP_UPP_000_PANE_NULL_03,
      PANENAME_ZUKAN_CMP_UPP_000_PANE_NULL_04,
    };
 
    for(u32 i=0; i<ZukanType::INFO_TYPE_NUM; ++i)
    {
      m_achievementTopPane[i] = lytWk_0->GetPane(ACHIEVEMENT_TOP_PANE_ID[i]);
      if( !m_param.contents->IsOpened(static_cast<ZukanType::InfoType>(i)) )
      {
        m_achievementZukanPane[i]->SetVisible(false);
        m_achievementRatePane[i]->SetVisible(false);
      }
    }
  }
}

void ZukanViewRegisterComment::startInSeq(void)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  pG2DUtil->StartAnime(
      0,  // wk_0
      LA_REGISTER_COMMENT_ZUKAN_CMP_UPP_000_IN
  );
  pG2DUtil->StartAnime(
      0,  // wk_0
      LA_REGISTER_COMMENT_ZUKAN_CMP_UPP_000_ROTOM_LOOP
  );

  this->changeDispState(DISP_STATE_ON_IN);
}

void ZukanViewRegisterComment::changeDispState(DispState dispState)
{
  m_dispState = dispState;
  m_dispStateCount = 0;
}

void ZukanViewRegisterComment::setupMsgIdList(void)
{
  Savedata::ZukanData* svZukanData = m_param.gameManager->GetGameData()->GetZukanData();

  static const u32 HYOUKA_LEVEL_NUM = 7;
  static const u32 HYOUKA_NUM_MIN[ZukanType::INFO_TYPE_NUM][HYOUKA_LEVEL_NUM] =  // 0はナシ
  {
    /**
      @fix MMcat[247]   図鑑のほめる演出の発生がnijiと同じ数になっている
        登録時評価のテーブルが未対応だったためmomiji用に修正
    */
#if 0
    { 30, 90, 150, 210, 270, 285, 300 },  // 図鑑完成に含めないポケモンの匹数は数えないように、伝説除き匹数をゲットできるようにしておく。
    { 12, 36,  60,  84, 108, 120,   0 },
    { 13, 39,  65,  91, 117, 130,   0 },
    { 13, 39,  65,  91, 117, 130,   0 },
    { 10, 30,  50,  70,  90, 100,   0 },
#else
  // 図鑑完成に含めないポケモンの匹数は数えないように、伝説除き匹数をゲットできるようにしておく。
  #define POKEDEX_MAXCOUNT_ALOLA  (400)   /**<  アローラ図鑑最大  */
  #define POKEDEX_MAXCOUNT_ISLE1  (150)   /**<  島1図鑑最大       */
  #define POKEDEX_MAXCOUNT_ISLE2  (160)   /**<  島2図鑑最大       */
  #define POKEDEX_MAXCOUNT_ISLE3  (160)   /**<  島3図鑑最大       */
  #define POKEDEX_MAXCOUNT_ISLE4  (130)   /**<  島4図鑑最大       */
  #define PERCENT_VALUE(_percentage, _value)  ((_value) * (_percentage) / 100)
  #define POKEDEX_PRAISE_TABLE_ENTRY(_per1, _per2, _per3, _per4, _per5, _per6, _per7, _max)   {PERCENT_VALUE(_per1, _max), PERCENT_VALUE(_per2, _max), PERCENT_VALUE(_per3, _max), PERCENT_VALUE(_per4, _max), PERCENT_VALUE(_per5, _max), PERCENT_VALUE(_per6, _max), PERCENT_VALUE(_per7, _max), }

  POKEDEX_PRAISE_TABLE_ENTRY(10, 30, 50, 70, 90,  95, 100, POKEDEX_MAXCOUNT_ALOLA),   /**<  アローラ図鑑評価テーブル  */
  POKEDEX_PRAISE_TABLE_ENTRY(10, 30, 50, 70, 90, 100,   0, POKEDEX_MAXCOUNT_ISLE1),   /**<  島1図鑑評価テーブル       */
  POKEDEX_PRAISE_TABLE_ENTRY(10, 30, 50, 70, 90, 100,   0, POKEDEX_MAXCOUNT_ISLE2),   /**<  島2図鑑評価テーブル       */
  POKEDEX_PRAISE_TABLE_ENTRY(10, 30, 50, 70, 90, 100,   0, POKEDEX_MAXCOUNT_ISLE3),   /**<  島3図鑑評価テーブル       */
  POKEDEX_PRAISE_TABLE_ENTRY(10, 30, 50, 70, 90, 100,   0, POKEDEX_MAXCOUNT_ISLE4),   /**<  島4図鑑評価テーブル       */

  #undef  POKEDEX_PRAISE_TABLE_ENTRY
  #undef  PERCENT_VALUE
  #undef  POKEDEX_MAXCOUNT_ISLE4
  #undef  POKEDEX_MAXCOUNT_ISLE3
  #undef  POKEDEX_MAXCOUNT_ISLE2
  #undef  POKEDEX_MAXCOUNT_ISLE1
  #undef  POKEDEX_MAXCOUNT_ALOLA
#endif
  };
  static const u32 HYOUKA_MSG_ID[ZukanType::INFO_TYPE_NUM][HYOUKA_LEVEL_NUM] =
  {
    { msg_pokeget_hyouka_10_01, msg_pokeget_hyouka_10_02, msg_pokeget_hyouka_10_03, msg_pokeget_hyouka_10_04, msg_pokeget_hyouka_10_05, msg_pokeget_hyouka_10_06, msg_pokeget_hyouka_10_07 },
    { msg_pokeget_hyouka_02_01, msg_pokeget_hyouka_02_02, msg_pokeget_hyouka_02_03, msg_pokeget_hyouka_02_04, msg_pokeget_hyouka_02_05, msg_pokeget_hyouka_02_06,                        0 },
    { msg_pokeget_hyouka_03_01, msg_pokeget_hyouka_03_02, msg_pokeget_hyouka_03_03, msg_pokeget_hyouka_03_04, msg_pokeget_hyouka_03_05, msg_pokeget_hyouka_03_06,                        0 },
    { msg_pokeget_hyouka_04_01, msg_pokeget_hyouka_04_02, msg_pokeget_hyouka_04_03, msg_pokeget_hyouka_04_04, msg_pokeget_hyouka_04_05, msg_pokeget_hyouka_04_06,                        0 },
    { msg_pokeget_hyouka_05_01, msg_pokeget_hyouka_05_02, msg_pokeget_hyouka_05_03, msg_pokeget_hyouka_05_04, msg_pokeget_hyouka_05_05, msg_pokeget_hyouka_05_06,                        0 },
  };


  // [0]
  // 初めてロトムが話すとき
  if(svZukanData->GetZukanOSVersion() == Savedata::ZukanData::ZUKAN_OS_VERSION_00)
  {
    m_msgIdList[m_msgNum++] = msg_pokeget_hyouka_01_01;
    svZukanData->SetZukanOSVersion(Savedata::ZukanData::ZUKAN_OS_VERSION_01);
  }

  // [1]
  // ビンゴ状態になったとき
  if(m_registerIsBingo)
  {
    m_msgIdList[m_msgNum++] = msg_pokeget_hyouka_01_02;
  }
  
  // [2]
  // 準伝 or 伝説 or UB
  if(m_registerLegendLevel == ZukanType::REGISTER_LEGEND_LEVEL_QUASI_LEGEND)
  {
    // 準伝説のポケモンが登録されたとき
    m_msgIdList[m_msgNum++] = msg_pokeget_hyouka_01_06;  // 準伝説（タイプ：ヌル、シルヴァディなど）
  }
  else if(m_registerLegendLevel == ZukanType::REGISTER_LEGEND_LEVEL_LEGEND)
  {
    // 伝説のポケモンが登録されたとき
    m_msgIdList[m_msgNum++] = msg_pokeget_hyouka_01_04;  // 伝説
  }
  else if(m_registerLegendLevel == ZukanType::REGISTER_LEGEND_LEVEL_UB)
  {
    // UBポケモンが登録されたとき
    m_msgIdList[m_msgNum++] = msg_pokeget_hyouka_01_05;  // UB
  }
  else if(m_registerLegendLevel == ZukanType::REGISTER_LEGEND_LEVEL_TIKI)  // @fix NMCat[1107] タイプ：ヌル、シルヴァディの図鑑表示についての質問  →準伝説とティキを分けたので、ティキがUBのような扱いになった。
  {
    // ティキが登録されたとき
    m_msgIdList[m_msgNum++] = msg_pokeget_hyouka_01_03;  // 土地神(カプ・○○)(ティキ)
  }
  else if(m_registerLegendLevel == ZukanType::REGISTER_LEGEND_LEVEL_MABOROSHI)  // 幻  // @fix GFNMCat[3983] まぼろしポケモン用の評価コメントが表示されない  →まぼろしを伝説と分ける。
  {
    // 幻ポケモンが登録されたとき
    m_msgIdList[m_msgNum++] = msg_pokeget_hyouka_01_07;  // まぼろし
  }

  // [3,4,5,6]
  // 各島図鑑「○％」
  {
    MonsNo monsNo = m_param.param->zukanRegisterEventParam->pp->GetMonsNo();
    const PokeTool::ExtendData* pokeExtendData = m_param.tool->GetPokeExtendData();

    for(u32 i=ZukanType::INFO_TYPE_ISLAND_START; i<=ZukanType::INFO_TYPE_ISLAND_END; ++i)
    {
      if( m_param.contents->IsOpened(static_cast<ZukanType::InfoType>(i)) )  // @fix NMCat[3766] オープンしていない島図鑑の褒めるメッセージが表示される  →オープンした島しかメッセージを表示しない
      {
        PokeTool::ExtendData::LocalArea localArea = static_cast<PokeTool::ExtendData::LocalArea>(i-ZukanType::INFO_TYPE_ISLAND_START + PokeTool::ExtendData::LOCAL_AREA_B);  // 島１エリアから
        u32 localNo = pokeExtendData->GetLocalNumber(monsNo, localArea);
        if(localNo != PokeTool::ExtendData::LOCAL_NUMBER_NONE)  // @fix NMCat[3487] ポケモン図鑑登録時のほめる演出が何度も発生する  →今着目しているポケモンが出現する島に関してのみ評価する
        {
          for(u32 level=HYOUKA_LEVEL_NUM; level>0; --level)
          {
            if( HYOUKA_NUM_MIN[i][level -1] > 0 )
            {
              if( m_param.contents->GetGetNum(static_cast<ZukanType::InfoType>(i)) == HYOUKA_NUM_MIN[i][level -1] )
              {
                m_msgIdList[m_msgNum++] = HYOUKA_MSG_ID[i][level -1];
                break;
              }
            }
          }
        }
      }
    }
  }

  // [7]
  // アローラ図鑑「○％」
  for(u32 level=HYOUKA_LEVEL_NUM; level>0; --level)
  {
    u32 i = ZukanType::INFO_TYPE_ALOLA;
    {
      if( HYOUKA_NUM_MIN[i][level -1] > 0 )
      {
        if( m_param.contents->GetGetNum(static_cast<ZukanType::InfoType>(i)) == HYOUKA_NUM_MIN[i][level -1] )
        {
          m_msgIdList[m_msgNum++] = HYOUKA_MSG_ID[i][level -1];
          break;
        }
      }
    }
  }

  // [8]
  // お決まりメッセージ
  if(m_msgNum == 0)  // @fix TNMCat[61] ジガルデを図鑑登録した際のテキストに違和感がある  →お決まりメッセージは他のメッセージを表示していないときしか出さないことになった。
  {
    switch(m_param.param->zukanRegisterEventParam->callerType)
    {
    case PokeTool::PokeGetUtil::CallerType::HOKAKU:  // 捕獲したとき
      {
        m_msgIdList[m_msgNum++] = msg_pokeget_hyouka_00_01;
        break;
      }
    case PokeTool::PokeGetUtil::CallerType::SHINKA:  // 進化したとき
    case PokeTool::PokeGetUtil::CallerType::SHINKA_NET:
      {
        m_msgIdList[m_msgNum++] = msg_pokeget_hyouka_00_02;
        break;
      }
    case PokeTool::PokeGetUtil::CallerType::TAMAGO_FUKA:  // タマゴから生まれたとき
      {
        m_msgIdList[m_msgNum++] = msg_pokeget_hyouka_00_03;
        break;
      }
    default:
      {
        m_msgIdList[m_msgNum++] = msg_pokeget_hyouka_00_04;  // それ以外（イベントや交換などで入手）
        break;
      }
    }
  }

  GFL_ASSERT(m_msgNum <= MSG_NUM_MAX);  // 開発中に気付かせるためのASSERT
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

