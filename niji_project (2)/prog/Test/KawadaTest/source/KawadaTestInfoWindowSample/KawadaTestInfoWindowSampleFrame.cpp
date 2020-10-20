//==============================================================================
/**
 @file    KawadaTestInfoWindowSampleFrame.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.07.01
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <Fade/include/gfl2_FadeManager.h>

#include <str/include/gfl2_MsgData.h>
#include <str/include/gfl2_StrBuf.h>

#include <fs/include/gfl2_Fs.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>
#include <System/include/ArcIdGetter.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>

#include <AppLib/include/Tool/app_tool_MenuWindow.h>
#include <AppLib/include/Tool/app_tool_MenuWindowListener.h>

#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowSheetType.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowSheetTypeListener.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h>

#include <Print/include/WordSet.h>

#include <AppLib/include/Util/app_util_FileAccessor.h>

// nijiのデータのヘッダーファイルのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_miracletrade.h>
#include <niji_conv_header/message/debug/msg_d_kawada.h>

// KawadaTestのインクルード
#include "KawadaTestInfoWindowSampleDataManager.h"
#include "KawadaTestInfoWindowSampleView.h"

#include "KawadaTestInfoWindowSampleFrame.h"




// テスト内容切り替え
#define TEST_KAWADATEST_KAWADATESTINFOWINDOWSAMPLE_BOOKTYPE_KEEPFLAG (0)  // 1のとき先に全ページセットする。0のとき1ページずつセットする。




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
//
//  KawadaTestInfoWindowSampleMenuWindowListener
//
//==============================================================================
class KawadaTestInfoWindowSampleMenuWindowListener : public app::tool::MenuWindowListener
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestInfoWindowSampleMenuWindowListener);

public:
  KawadaTestInfoWindowSampleMenuWindowListener(KawadaTestInfoWindowSampleFrameCell* frameCell)
    : app::tool::MenuWindowListener(),
      m_frameCell(frameCell)
  {}
  virtual ~KawadaTestInfoWindowSampleMenuWindowListener()
  {}

public:
  virtual void SelectMenu( u32 id )
  {
    GFL_PRINT("KawadaTestInfoWindowSampleMenuWindowListener::SelectMenu %d\n", id);

    m_frameCell->ReqMenuWindowClose(id);
  }

private:
  KawadaTestInfoWindowSampleFrameCell* m_frameCell;
};




//==============================================================================
//
//  KawadaTestInfoWindowSampleInfoWindowSheetTypeListener
//
//==============================================================================
class KawadaTestInfoWindowSampleInfoWindowSheetTypeListener : public app::tool::InfoWindowSheetTypeListener
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestInfoWindowSampleInfoWindowSheetTypeListener);

public:
  KawadaTestInfoWindowSampleInfoWindowSheetTypeListener(KawadaTestInfoWindowSampleFrameCell* frameCell)
    : app::tool::InfoWindowSheetTypeListener(),
      m_frameCell(frameCell)
  {}
  virtual ~KawadaTestInfoWindowSampleInfoWindowSheetTypeListener()
  {}

  virtual void OnClose(void)
  {
    GFL_PRINT("KawadaTestInfoWindowSampleInfoWindowSheetTypeListener::OnClose\n");
    
    m_frameCell->ReqInfoWindowSheetTypeClose();
  }

private:
  KawadaTestInfoWindowSampleFrameCell* m_frameCell;
};




//==============================================================================
//
//  KawadaTestInfoWindowSampleInfoWindowBookTypeListener
//
//==============================================================================
class KawadaTestInfoWindowSampleInfoWindowBookTypeListener : public app::tool::InfoWindowBookTypeListener
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestInfoWindowSampleInfoWindowBookTypeListener);

public:
  KawadaTestInfoWindowSampleInfoWindowBookTypeListener(KawadaTestInfoWindowSampleFrameCell* frameCell)
    : app::tool::InfoWindowBookTypeListener(),
      m_frameCell(frameCell)
  {}
  virtual ~KawadaTestInfoWindowSampleInfoWindowBookTypeListener()
  {}

  virtual void OnClose(void)
  {
    GFL_PRINT("KawadaTestInfoWindowSampleInfoWindowBookTypeListener::OnClose\n");
    
    m_frameCell->ReqInfoWindowBookTypeClose();
  }

  virtual void OnChangePage(u32 pageNo)
  {
#if TEST_KAWADATEST_KAWADATESTINFOWINDOWSAMPLE_BOOKTYPE_KEEPFLAG 
    ////////////////////////////////
    // 全ページセット済み
    ////////////////////////////////
    // 全ページセット済みなので、何もしない。
    GFL_PRINT("pageNo=%d\n", pageNo);
#else
    ////////////////////////////////
    // 1ページずつセットするとき
    ////////////////////////////////
    KawadaTestInfoWindowSampleView* l_view = m_frameCell->GetView();
    app::tool::InfoWindowBookType* l_infoWindowBookType = m_frameCell->GetInfoWindowBookType();
    
    gfl2::str::StrBuf* strBuf0 = l_view->GetG2DUtil()->GetTempStrBuf(0);

    print::WordSet*     l_wordSet = m_frameCell->GetWordSet();
    gfl2::str::StrBuf*  l_strBuf[2];
    l_strBuf[0] = m_frameCell->GetStrBuf(0);
    l_strBuf[1] = m_frameCell->GetStrBuf(1);
    gfl2::str::MsgData* l_msgData = m_frameCell->GetMsgData();

    switch(pageNo)
    {
    case 0:
      {
        l_msgData->GetString(MS_LABEL_D_KAWADA_INFO_WINDOW_BOOK_TEXT10_001, *strBuf0);
        l_wordSet->Expand(l_strBuf[0], strBuf0);
        l_msgData->GetString(MS_LABEL_D_KAWADA_INFO_WINDOW_BOOK_BUTTON_000, *strBuf0);
        l_wordSet->Expand(l_strBuf[1], strBuf0);

        l_infoWindowBookType->SetTextContent(
            pageNo,
            l_strBuf[0],
            l_strBuf[1]
        );
        
        l_infoWindowBookType->SetCommandButtonActive(true);
        break;
      }
    case 2:
    case 4:
    case 6:
      {
        l_msgData->GetString(MS_LABEL_D_KAWADA_INFO_WINDOW_BOOK_TEXT10_001, *strBuf0);
        l_wordSet->Expand(l_strBuf[0], strBuf0);
        l_msgData->GetString(MS_LABEL_D_KAWADA_INFO_WINDOW_BOOK_BUTTON_000, *strBuf0);
        l_wordSet->Expand(l_strBuf[1], strBuf0);

        l_infoWindowBookType->SetTextContent(
            pageNo,
            l_strBuf[0],
            l_strBuf[1]
        );

        l_infoWindowBookType->SetCommandButtonActive(false);
        //l_infoWindowBookType->SetCommandButtonActive(true);
        break;
      }
    case 1:
    case 3:
    case 5:
    default:
      {
        l_msgData->GetString(MS_LABEL_D_KAWADA_INFO_WINDOW_BOOK_TEXT6_000, *strBuf0);
        l_wordSet->Expand(l_strBuf[0], strBuf0);
        
        l_infoWindowBookType->SetTextContent(
            pageNo,
            l_strBuf[0],
            NULL
        );
        break;
      }
    }
#endif
  }

  virtual void OnCommand(u32 pageNo)
  {
    app::tool::InfoWindowBookType* l_infoWindowBookType = m_frameCell->GetInfoWindowBookType();
    
    GFL_PRINT("OnCommand pageNo=%d\n", pageNo);
    if(pageNo == 6)
    {
      l_infoWindowBookType->StartClose();
    }
  }

private:
  KawadaTestInfoWindowSampleFrameCell* m_frameCell;
};




//==============================================================================
//
//  KawadaTestInfoWindowSampleFrameListener
//
//==============================================================================
KawadaTestInfoWindowSampleFrameListener::KawadaTestInfoWindowSampleFrameListener(void)
  : applib::frame::AppFrameListener()
{}
KawadaTestInfoWindowSampleFrameListener::~KawadaTestInfoWindowSampleFrameListener()
{}

void KawadaTestInfoWindowSampleFrameListener::EndFuncFinish( applib::frame::CellSuper* /*pEndCell*/ )
{}




//==============================================================================
//
//  KawadaTestInfoWindowSampleFrameCell
//
//==============================================================================

//
// メモ
//
// KawadaTestInfoWindowSampleViewの入力可不可制御はKawadaTestInfoWindowSampleFrameCellで行う。
// フェードはKawadaTestInfoWindowSampleFrameCellで行う。
//

KawadaTestInfoWindowSampleFrameCell::KawadaTestInfoWindowSampleFrameCell(void)
  : applib::frame::CellSuper(),
    m_param(),
    m_fadeColor(0.0f, 0.0f, 0.0f, 255.0f),
    m_seqCount(0),
    m_view(NULL),
    m_viewListener(NULL),
    m_endReq(false),
    m_menuWindow(NULL),
    m_menuWindowListener(NULL),
    m_menuWindowOpenReq(NULL),
    m_menuWindowCloseReq(NULL),
    m_menuWindowSelectId(0),
    m_infoWindowSheetType(NULL),
    m_infoWindowSheetTypeListener(NULL),
    m_touchPosForInfoWindowSheetType(gfl2::math::Vector2(0,0)),
    m_infoWindowSheetTypeOpenReq(NULL),
    m_infoWindowSheetTypeCloseReq(NULL),
    m_infoWindowBookType(NULL),
    m_infoWindowBookTypeListener(NULL),
    m_touchPosForInfoWindowBookType(gfl2::math::Vector2(0,0)),
    m_infoWindowBookTypeOpenReq(NULL),
    m_infoWindowBookTypeCloseReq(NULL),
    m_wordSet(NULL),
    m_msgData(NULL),
    m_fileTestStep(0)
{}
KawadaTestInfoWindowSampleFrameCell::~KawadaTestInfoWindowSampleFrameCell()
{}

applib::frame::Result KawadaTestInfoWindowSampleFrameCell::InitFunc(void)
{
  applib::frame::Result res = applib::frame::RES_CONTINUE;

  // 最初に画面を真っ黒にしておく
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut(gfl2::Fade::DISP_DOUBLE, &m_fadeColor);

  // データの読み込み待ち
  void* res2dDataBuf = m_param.dataManager->GetDataBuf(KawadaTestInfoWindowSampleDataManager::DATA_ID_SIMPLE_LAYOUT_TEST_APPLYT);
  void* msgDataBuf   = m_param.dataManager->GetDataBuf(KawadaTestInfoWindowSampleDataManager::DATA_ID_MY_DEBUG_MESSAGE_DAT);
  if(res2dDataBuf!=NULL && msgDataBuf!=NULL)
  {
    // ビュー
    {
      KawadaTestInfoWindowSampleView::Param param;
      param.appHeap             = m_param.appHeap;
      param.gameManager         = m_param.gameManager;
      param.dataManager         = m_param.dataManager;
      param.frameManager        = m_param.frameManager;
      param.appRenderingManager = m_param.appRenderingManager;
      m_view = GFL_NEW(m_param.appHeap->GetDeviceHeap()) KawadaTestInfoWindowSampleView(param);

      m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) KawadaTestInfoWindowSampleViewListener(this);
      m_view->SetInputListener(m_viewListener);  // KawadaTestInfoWindowSampleViewの親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数

      m_view->SetInputEnabledAllView(false);  // KawadaTestInfoWindowSampleViewの親であるapp::ui::UIViewのメンバ関数
    }

    // MenuWindowビュー
    {
      m_menuWindow = GFL_NEW(m_param.appHeap->GetDeviceHeap()) app::tool::MenuWindow(
          m_param.appHeap,
          m_param.appRenderingManager,
          NULL,
          app::util::G2DUtil::SETUP_LOWER
      );
      m_menuWindowListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) KawadaTestInfoWindowSampleMenuWindowListener(this);
      m_menuWindow->SetListener(m_menuWindowListener);
      m_view->AddSubView(m_menuWindow);

      print::WordSet* wordSet;
      gfl2::str::StrBuf* strBuf0;
      gfl2::str::StrBuf* strBuf1;
      m_view->GetMsgWorkObject(&wordSet, &strBuf0, &strBuf1);
      gfl2::str::MsgData* msgData = m_view->GetMsgData(print::GetMessageArcId(), GARC_message_miracletrade_DAT);

      msgData->GetString(msg_miracletrade_cap_07, *strBuf0);
      wordSet->Expand(strBuf1, strBuf0);
      m_menuWindow->AddItem(*strBuf1);

      msgData->GetString(msg_miracletrade_sel_01, *strBuf0);
      wordSet->Expand(strBuf1, strBuf0);
      m_menuWindow->AddItem(*strBuf1);
    }

    // InfoWindowSheetTypeビュー
    {
      m_infoWindowSheetType = GFL_NEW(m_param.appHeap->GetDeviceHeap()) app::tool::InfoWindowSheetType(
          m_param.appHeap
      );
      //m_infoWindowSheetType->SetBgVisible(true);
      m_infoWindowSheetTypeListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) KawadaTestInfoWindowSampleInfoWindowSheetTypeListener(this);

      m_infoWindowSheetType->SyncCreate(
          m_param.appHeap,
          m_param.appRenderingManager,
          NULL
      );

      m_view->AddSubView(m_infoWindowSheetType);
    }

    // InfoWindowBookTypeビュー
    {
      m_infoWindowBookType = GFL_NEW(m_param.appHeap->GetDeviceHeap()) app::tool::InfoWindowBookType(
          m_param.appHeap
      );
      //m_infoWindowBookType->SetBgVisible(true);
      m_infoWindowBookTypeListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) KawadaTestInfoWindowSampleInfoWindowBookTypeListener(this);

      m_infoWindowBookType->SyncCreate(
          m_param.appHeap,
          m_param.appRenderingManager,
          7,
          NULL
      );

      m_view->AddSubView(m_infoWindowBookType);
    }

    // MsgData
    {
      m_wordSet = GFL_NEW(m_param.appHeap->GetDeviceHeap()) print::WordSet( print::WordSet::DEFAULT_WORD_MAX, print::WordSet::DEFAULT_BUF_LEN, m_param.appHeap->GetDeviceHeap() );
      for(u32 i=0; i<STR_BUF_NUM; ++i)
      {
        m_strBuf[i] = GFL_NEW(m_param.appHeap->GetDeviceHeap()) gfl2::str::StrBuf( STR_BUF_CHAR_NUM_MAX, m_param.appHeap->GetDeviceHeap() );
      }
      m_msgData = GFL_NEW(m_param.appHeap->GetDeviceHeap()) gfl2::str::MsgData(msgDataBuf, m_param.appHeap->GetDeviceHeap());
    }
    
    res = applib::frame::RES_FINISH;
  }

  return res;
}
applib::frame::Result KawadaTestInfoWindowSampleFrameCell::UpdateFunc(void)
{
  enum
  {
    SEQ_WAIT                          = 0,
    SEQ_START_FADE_IN                    ,
    SEQ_WAIT_FADE_IN_END                 ,
    SEQ_WAIT_REQ                         ,
                                      
    SEQ_MENU_WINDOW_TEST_0               ,
    SEQ_MENU_WINDOW_TEST_1               ,
    SEQ_MENU_WINDOW_TEST_2               ,
    SEQ_MENU_WINDOW_TEST_3               ,
    
    SEQ_INFO_WINDOW_SHEET_TYPE_TEST_0    ,
    SEQ_INFO_WINDOW_SHEET_TYPE_TEST_1    ,
    
    SEQ_INFO_WINDOW_BOOK_TYPE_TEST_0     ,
    SEQ_INFO_WINDOW_BOOK_TYPE_TEST_1     ,

    SEQ_START_FADE_OUT                   ,
    SEQ_WAIT_FADE_OUT_END                ,
    SEQ_END                              ,
  };

  applib::frame::Result res = applib::frame::RES_CONTINUE;

  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);

  m_view->UpdateTree();

  switch( this->GetSubSeq() )  // 親であるapplib::frame::CellSuperのメンバ関数
  {
  case SEQ_WAIT:
    {
      ++m_seqCount;
      if(m_seqCount >= 2)  // KawadaTestInfoWindowSampleViewのアニメの初期値が反映されるまで待つ
      {
        this->changeSeq(SEQ_START_FADE_IN);
      }
      break;
    }
  case SEQ_START_FADE_IN:
    {
      pFadeManager->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA);
      this->changeSeq(SEQ_WAIT_FADE_IN_END);
      //breakしない
    }
  case SEQ_WAIT_FADE_IN_END:
    {
      if(
           pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE)
        && m_menuWindow->IsCreate()
      )
      {
        m_view->SetInputEnabledAllView(true);  // KawadaTestInfoWindowSampleViewの親であるapp::ui::UIViewのメンバ関数
        this->changeSeq(SEQ_WAIT_REQ);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_WAIT_REQ:
    {
      if(m_endReq)
      {
        this->changeSeq(SEQ_START_FADE_OUT);
      }
      else if(m_menuWindowOpenReq)
      {
        m_menuWindowOpenReq = false;
        this->changeSeq(SEQ_MENU_WINDOW_TEST_0);
      }
      else if(m_infoWindowSheetTypeOpenReq)
      {
        m_infoWindowSheetTypeOpenReq = false;
        this->changeSeq(SEQ_INFO_WINDOW_SHEET_TYPE_TEST_0);
      }
      else if(m_infoWindowBookTypeOpenReq)
      {
        m_infoWindowBookTypeOpenReq = false;
        this->changeSeq(SEQ_INFO_WINDOW_BOOK_TYPE_TEST_0);
      }
      break;
    }

  case SEQ_MENU_WINDOW_TEST_0:
    {
      m_view->SetInputEnabled(false);  // MenuWindowは親の入力を禁止しないので、自分で禁止する。  // KawadaTestInfoWindowSampleViewの親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数
      m_menuWindow->StartYesNoMenu(0);
      this->changeSeq(SEQ_MENU_WINDOW_TEST_1);
      break;
    }
  case SEQ_MENU_WINDOW_TEST_1:
    {
      if(m_menuWindowCloseReq)
      {
        m_menuWindow->EndMenu(false);
        m_menuWindowCloseReq = false;
        m_view->SetInputEnabled(true);  // 自分で禁止したものを戻す。  // KawadaTestInfoWindowSampleViewの親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数
        this->changeSeq(SEQ_MENU_WINDOW_TEST_2);
        //GFL_SAFE_DELETE(m_menuWindow);  // ここだと1フレ遅れ解放NG
      }
      break;
    }
  case SEQ_MENU_WINDOW_TEST_2:
    {
      //GFL_SAFE_DELETE(m_menuWindow);  // ここだと1フレ遅れ解放OK
      this->changeSeq(SEQ_MENU_WINDOW_TEST_3);
      break;
    }
  case SEQ_MENU_WINDOW_TEST_3:
    {
      //GFL_SAFE_DELETE(m_menuWindow);
      this->changeSeq(SEQ_WAIT_REQ);
      break;
    }

  case SEQ_INFO_WINDOW_SHEET_TYPE_TEST_0:
    {
      gfl2::str::StrBuf* strBuf0 = m_view->GetG2DUtil()->GetTempStrBuf(0);
      //gfl2::str::StrBuf* strBuf1 = m_view->GetG2DUtil()->GetTempStrBuf(0);

      m_msgData->GetString(MS_LABEL_D_KAWADA_INFO_WINDOW_TITLE_000, *strBuf0);
      m_wordSet->Expand(m_strBuf[0], strBuf0);
      
      //m_msgData->GetString(MS_LABEL_D_KAWADA_INFO_WINDOW_TEXT2_000, *strBuf0);
      //m_msgData->GetString(MS_LABEL_D_KAWADA_INFO_WINDOW_TEXT3_000, *strBuf0);
      m_msgData->GetString(MS_LABEL_D_KAWADA_INFO_WINDOW_TEXT4_000, *strBuf0);
      //m_msgData->GetString(MS_LABEL_D_KAWADA_INFO_WINDOW_TEXT5_000, *strBuf0);
      m_wordSet->Expand(m_strBuf[1], strBuf0);

      gfl2::math::Vector3 pos(m_touchPosForInfoWindowSheetType.x-160, 120-m_touchPosForInfoWindowSheetType.y, 0);
      m_infoWindowSheetType->StartOpen(
          m_infoWindowSheetTypeListener,
          pos,
          m_strBuf[0],
          m_strBuf[1]
      );
      this->changeSeq(SEQ_INFO_WINDOW_SHEET_TYPE_TEST_1);
      break;
    }
  case SEQ_INFO_WINDOW_SHEET_TYPE_TEST_1:
    {
      ////////////////////////////////////////////////fileTest
      {
        gfl2::fs::ArcFile::ARCID    arcId = NIJI_ARCID_NUM(ARCID_TITLE);
        gfl2::fs::ArcFile::ARCDATID arcDatId = 0;
        u32 bufNum = 3;  // execUncompFlag=trueのとき3が限界、execUncompFlag=falseのとき8でも大丈夫
        bool execUncompFlag = true;

        if(m_fileTestStep == 0)
        {
          const gfl2::ui::Button* button = m_param.gameManager->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
          if( button->IsTrigger(gfl2::ui::BUTTON_Y) )
          {
            app::util::FileAccessor::FileOpen(
              arcId,
              m_param.appHeap->GetDeviceHeap(),
              true
            );
            for(u32 i=0; i<bufNum; ++i)
            {
              app::util::FileAccessor::FileLoad(
                arcId,
                arcDatId,
                &m_fileTestBuf[i],
                m_param.appHeap->GetDeviceHeap(),
                execUncompFlag,
                128
              );
            }
            app::util::FileAccessor::FileClose(
              arcId,
              m_param.appHeap->GetDeviceHeap()
            );
            m_fileTestStep = 1;
          }
        }
        else if(m_fileTestStep == 1)
        {
          if(
               app::util::FileAccessor::IsFileLoad(&m_fileTestBuf[bufNum-1])
            && app::util::FileAccessor::IsFileClose(arcId)
          )
          {
            for(u32 i=0; i<bufNum; ++i)
            {
              GflHeapSafeFreeMemory(m_fileTestBuf[i]);
            }
            m_fileTestStep = 0;
            GFL_PRINT("fileTest finishes\n");
          }
        }
      }
      ////////////////////////////////////////////////fileTest

      s32 TEST_COUNT = 5*60*30;
      ++m_seqCount;
      if(m_seqCount == TEST_COUNT)
      {
        //m_infoWindowSheetType->StartCloseImmediately();
        m_infoWindowSheetType->CloseManually();
      }

      if(m_infoWindowSheetTypeCloseReq)
      {
        m_infoWindowSheetTypeCloseReq = false;
        this->changeSeq(SEQ_WAIT_REQ);
      }
      break;
    }

  case SEQ_INFO_WINDOW_BOOK_TYPE_TEST_0:
    {
#if TEST_KAWADATEST_KAWADATESTINFOWINDOWSAMPLE_BOOKTYPE_KEEPFLAG 
      ////////////////////////////////
      // 全ページセットするとき
      ////////////////////////////////
      m_infoWindowBookType->ResetContents(7, true);
      
      gfl2::str::StrBuf* strBuf0 = m_view->GetG2DUtil()->GetTempStrBuf(0);

      for(u32 pageNo=0; pageNo<7; ++pageNo)
      {
        u32 strBufIndex0 = pageNo*2;
        u32 strBufIndex1 = strBufIndex0+1;

        switch(pageNo)
        {
        case 0:
          {
            m_msgData->GetString(MS_LABEL_D_KAWADA_INFO_WINDOW_BOOK_TEXT10_001, *strBuf0);
            m_wordSet->Expand(m_strBuf[strBufIndex0], strBuf0);
            m_msgData->GetString(MS_LABEL_D_KAWADA_INFO_WINDOW_BOOK_BUTTON_000, *strBuf0);
            m_wordSet->Expand(m_strBuf[strBufIndex1], strBuf0);
      
            m_infoWindowBookType->SetTextContent(
                pageNo,
                m_strBuf[strBufIndex0],
                m_strBuf[strBufIndex1]
            );
            break;
          }
        case 2:
        case 4:
        case 6:
          {
            m_msgData->GetString(MS_LABEL_D_KAWADA_INFO_WINDOW_BOOK_TEXT10_001, *strBuf0);
            m_wordSet->Expand(m_strBuf[strBufIndex0], strBuf0);
            m_msgData->GetString(MS_LABEL_D_KAWADA_INFO_WINDOW_BOOK_BUTTON_000, *strBuf0);
            m_wordSet->Expand(m_strBuf[strBufIndex1], strBuf0);
      
            m_infoWindowBookType->SetTextContent(
                pageNo,
                m_strBuf[strBufIndex0],
                m_strBuf[strBufIndex1]
            );
            break;
          }
        case 1:
        case 3:
        case 5:
        default:
          {
            m_msgData->GetString(MS_LABEL_D_KAWADA_INFO_WINDOW_BOOK_TEXT6_000, *strBuf0);
            m_wordSet->Expand(m_strBuf[strBufIndex0], strBuf0);
            
            m_infoWindowBookType->SetTextContent(
                pageNo,
                m_strBuf[strBufIndex0],
                NULL
            );
            break;
          }
        }
      }

      gfl2::math::Vector3 pos(m_touchPosForInfoWindowBookType.x-160, 120-m_touchPosForInfoWindowBookType.y, 0);
      m_infoWindowBookType->StartOpen(
          m_infoWindowBookTypeListener,
          pos,
          app::tool::InfoWindowBookType::OPEN_TYPE_NO_ANIM
      );
#else
      ////////////////////////////////
      // 1ページずつセットするとき
      ////////////////////////////////
      gfl2::str::StrBuf* strBuf0 = m_view->GetG2DUtil()->GetTempStrBuf(0);

      m_msgData->GetString(MS_LABEL_D_KAWADA_INFO_WINDOW_BOOK_TEXT10_001, *strBuf0);
      m_wordSet->Expand(m_strBuf[0], strBuf0);
 
      m_msgData->GetString(MS_LABEL_D_KAWADA_INFO_WINDOW_BOOK_BUTTON_000, *strBuf0);
      m_wordSet->Expand(m_strBuf[1], strBuf0);

      m_infoWindowBookType->ResetContents(7);
      
      m_infoWindowBookType->SetTextContent(
          0,
          m_strBuf[0],
          m_strBuf[1]
      );

      gfl2::math::Vector3 pos(m_touchPosForInfoWindowBookType.x-160, 120-m_touchPosForInfoWindowBookType.y, 0);
      m_infoWindowBookType->StartOpen(
          m_infoWindowBookTypeListener,
          pos
      );
#endif
      this->changeSeq(SEQ_INFO_WINDOW_BOOK_TYPE_TEST_1);
      break;
    }
  case SEQ_INFO_WINDOW_BOOK_TYPE_TEST_1:
    {
      s32 TEST_COUNT = 5*60*30;
      ++m_seqCount;
      if(m_seqCount == TEST_COUNT)
      {
        //m_infoWindowBookType->StartCloseImmediately();
        m_infoWindowBookType->CloseManually();
      }

      if(m_infoWindowBookTypeCloseReq)
      {
        m_infoWindowBookTypeCloseReq = false;
        this->changeSeq(SEQ_WAIT_REQ);
      }
      break;
    }

  case SEQ_START_FADE_OUT:
    {
      pFadeManager->RequestDefaultOut(gfl2::Fade::DISP_DOUBLE);
      this->changeSeq(SEQ_WAIT_FADE_OUT_END);
      //breakしない
    }
  case SEQ_WAIT_FADE_OUT_END:
    {
      if(pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE))
      {
        GFL_PRINT("SEQ_WAIT_FADE_OUT_END: IsEnd!=false %d\n", m_view->GetG2DUtil()->GetLayoutWork(0)->IsDrawing());  // wk_0
        this->changeSeq(SEQ_END);
        //breakしない
      }
      else
      {
        GFL_PRINT("SEQ_WAIT_FADE_OUT_END: IsEnd==false %d\n", m_view->GetG2DUtil()->GetLayoutWork(0)->IsDrawing());  // wk_0
        break;
      }
    }
  case SEQ_END:
  default:
    {
      GFL_PRINT("SEQ_END: %d\n", m_view->GetG2DUtil()->GetLayoutWork(0)->IsDrawing());  // wk_0
      res = applib::frame::RES_FINISH;
      break;
    }
  }

  return res;
}
void KawadaTestInfoWindowSampleFrameCell::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{
  m_view->DrawTree(no);
}
applib::frame::Result KawadaTestInfoWindowSampleFrameCell::EndFunc(void)
{
  applib::frame::Result res = applib::frame::RES_CONTINUE;
 
  int seq = this->GetSubSeq();

  switch( seq )  // 親であるapplib::frame::CellSuperのメンバ関数
  {
  case 0:
    {
      m_menuWindow->RemoveSubView();
      m_infoWindowSheetType->RemoveFromSuperView();  // InfoWindowSheetTypeの親であるapp::ui::UIViewのメンバ関数
      m_infoWindowBookType->RemoveFromSuperView();   // InfoWindowBookTypeの親であるapp::ui::UIViewのメンバ関数
      GFL_PRINT("EndFunc: 0 %d\n", m_view->GetG2DUtil()->GetLayoutWork(0)->IsDrawing());  // wk_0
      ++seq;
      //breakしない
    }
  case 1:
    {
      if(
           m_menuWindow->IsModuleFree()
        && m_infoWindowSheetType->EndFunc()
        && m_infoWindowBookType->EndFunc()
      )
      {
        GFL_PRINT("EndFunc: 1 true %d\n", m_view->GetG2DUtil()->GetLayoutWork(0)->IsDrawing());  // wk_0
        
        GFL_SAFE_DELETE(m_menuWindow);
        GFL_SAFE_DELETE(m_menuWindowListener);

        GFL_SAFE_DELETE(m_infoWindowSheetType);
        GFL_SAFE_DELETE(m_infoWindowSheetTypeListener);
        
        GFL_SAFE_DELETE(m_infoWindowBookType);
        GFL_SAFE_DELETE(m_infoWindowBookTypeListener);
      
        GFL_SAFE_DELETE(m_view);
        GFL_SAFE_DELETE(m_viewListener);
   
        GFL_SAFE_DELETE(m_wordSet);
        for(u32 i=0; i<STR_BUF_NUM; ++i)
        {
          GFL_SAFE_DELETE(m_strBuf[i]);
        }
        GFL_SAFE_DELETE(m_msgData);

        // このサンプルでは最後に画面を出しておく
        GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, 0);
      
        ++seq;
        //breakしない
      }
      else
      {
        GFL_PRINT("EndFunc: 1 false %d\n", m_view->GetG2DUtil()->GetLayoutWork(0)->IsDrawing());  // wk_0
        break;
      }
    }
  case 2:
  default:
    {
      res = applib::frame::RES_FINISH;
      break;
    }
  }
  
  this->SetSubSeq(seq);  // 親であるapplib::frame::CellSuperのメンバ関数

  return res;
}

void KawadaTestInfoWindowSampleFrameCell::Setup(const Param& param)
{
  m_param = param;
}

void KawadaTestInfoWindowSampleFrameCell::ReqEnd(void)
{
  m_view->SetInputEnabledAllView(false);  // KawadaTestInfoWindowSampleViewの親であるapp::ui::UIViewのメンバ関数
  m_endReq = true;
}
void KawadaTestInfoWindowSampleFrameCell::ReqMenuWindowOpen(void)
{
  m_menuWindowOpenReq = true;
}
void KawadaTestInfoWindowSampleFrameCell::ReqInfoWindowSheetTypeOpen(const gfl2::math::Vector2& touchPos)
{
  m_touchPosForInfoWindowSheetType = touchPos;
  m_infoWindowSheetTypeOpenReq = true;
}
void KawadaTestInfoWindowSampleFrameCell::ReqInfoWindowBookTypeOpen(const gfl2::math::Vector2& touchPos)
{
  m_touchPosForInfoWindowBookType = touchPos;
  m_infoWindowBookTypeOpenReq = true;
}

void KawadaTestInfoWindowSampleFrameCell::ReqMenuWindowClose(u32 selectId)
{
  m_menuWindowSelectId = selectId;
  m_menuWindowCloseReq = true;
}
void KawadaTestInfoWindowSampleFrameCell::ReqInfoWindowSheetTypeClose(void)
{
  m_infoWindowSheetTypeCloseReq = true;
}
void KawadaTestInfoWindowSampleFrameCell::ReqInfoWindowBookTypeClose(void)
{
  m_infoWindowBookTypeCloseReq = true;
}

void KawadaTestInfoWindowSampleFrameCell::changeSeq(int seq)
{
  this->SetSubSeq(seq);  // 親であるapplib::frame::CellSuperのメンバ関数
  m_seqCount = 0;
}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG

