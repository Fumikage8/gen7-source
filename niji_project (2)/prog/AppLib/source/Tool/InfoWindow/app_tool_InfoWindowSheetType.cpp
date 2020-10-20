//======================================================================
/**
 * @file app_tool_InfoWindow.cpp
 * @date 2015/07/02 15:49:24
 * @author kawada_koji
 * @brief インフォウィンドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================


// gfl2のインクルード
#include <fs/include/gfl2_Fs.h>
#include <str/include/gfl2_StrSys.h>
#include <str/include/gfl2_StrBuf.h>
#include <thread/include/gfl2_ThreadStatic.h>

// nijiのデータのヘッダーファイルのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/app_information.gaix>
#include <niji_conv_header/app/information/res2d/information_sml.h>
#include <niji_conv_header/app/information/res2d/information_sml_anim_list.h>
#include <niji_conv_header/app/information/res2d/information_sml_pane.h>
#include <niji_conv_header/app/information/res2d/information_lrg.h>
#include <niji_conv_header/app/information/res2d/information_lrg_anim_list.h>
#include <niji_conv_header/app/information/res2d/information_lrg_pane.h>

// nijiのインクルード
#include "AppLib/include/ui/UIInputListener.h"

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>

#include <Sound/include/Sound.h>

#include "app_tool_InfoWindowDefine.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowSheetTypeListener.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowSheetType.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)


//==============================================================================
/**
 @class  InfoWindowSheetTypeLocalListener
 @brief  インフォウィンドウの内部リスナー
 */
class InfoWindowSheetTypeLocalListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(InfoWindowSheetTypeLocalListener);

public:
  InfoWindowSheetTypeLocalListener(InfoWindowSheetType* view)
    : app::ui::UIInputListener(),
      m_view(view),
      m_inputState(INPUT_STATE_RELEASE)
  {}
  virtual ~InfoWindowSheetTypeLocalListener()
  {}

  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
  {
    app::ui::UIInputListener::ListenerResult res = app::ui::UIInputListener::ENABLE_ACCESS;
    switch(m_inputState)
    {
    case INPUT_STATE_OCCUPY:
      {
        res = app::ui::UIInputListener::DISABLE_ACCESS;
        break;
      }
    case INPUT_STATE_ENABLE:
      {
        if( pTouchPanel->IsTouchRelease() )
        { 
          if(m_view->IsSEOn()) Sound::PlaySE( SEQ_SE_CANCEL1 );  // 閉じるときのSEをSEQ_SE_CANCEL1に統一
          m_view->ReqClose();
        }
        res = app::ui::UIInputListener::DISABLE_ACCESS;
        break;
      }
    }
    return res;
  }
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    app::ui::UIInputListener::ListenerResult res = app::ui::UIInputListener::ENABLE_ACCESS;
    switch(m_inputState)
    {
    case INPUT_STATE_OCCUPY:
      {
        res = app::ui::UIInputListener::DISABLE_ACCESS;
        break;
      }
    case INPUT_STATE_ENABLE:
      {
        if( pButton->IsTrigger(gfl2::ui::BUTTON_A) || pButton->IsTrigger(gfl2::ui::BUTTON_B) )
        {
          if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
          {
            if(m_view->IsSEOn()) Sound::PlaySE( SEQ_SE_CANCEL1 );  // 閉じるときのSEをSEQ_SE_CANCEL1に統一
          }
          else
          {
            if(m_view->IsSEOn()) Sound::PlaySE( SEQ_SE_CANCEL1 );  // 閉じるときのSEをSEQ_SE_CANCEL1に統一
          }
          m_view->ReqClose();
        }
        res = app::ui::UIInputListener::DISABLE_ACCESS;
        break;
      }
    }
    return res;
  }

public:
  enum InputState
  {
    INPUT_STATE_RELEASE,  //!< 他のviewに解放中
    INPUT_STATE_OCCUPY,   //!< このviewが占有しているが、入力は受け付けていない
    INPUT_STATE_ENABLE,   //!< このviewが占有していて、入力を受け付けている
  };
  void SetInputState(InputState inputState)
  {
    m_inputState = inputState;
  }

private:
  InfoWindowSheetType* m_view;
  InputState           m_inputState;
};


//==============================================================================
//
//  InfoWindowSheetType
//
InfoWindowSheetType::InfoWindowSheetType(app::util::Heap* appHeap)
  : InfoWindowBase(appHeap),
    m_listener(NULL),
    m_pos(gfl2::math::Vector3(0,0,0)),
    m_titleStrBuf(NULL),
    m_textStrBuf(NULL),
    m_localListener(NULL),
    m_dispState(DISP_STATE_OFF),
    m_dispStateCount(0),
    m_openReq(false),
    m_closeReq(false),
    m_titlePane(NULL),
    m_textPane(NULL),
    m_textLineNum(0)
{}
InfoWindowSheetType::~InfoWindowSheetType()
{
  this->destructa();  // 親であるInfoWindowBaseのメンバ関数
}

bool InfoWindowSheetType::EndFunc(void)
{
  return InfoWindowBase::EndFunc();
}

void InfoWindowSheetType::StartAsyncCreate(
    app::util::Heap* appHeap,
    app::util::AppRenderingManager* appRenderingManager,
    InfoWindowDataManager* dataManager
)
{
  InfoWindowBase::StartAsyncCreate(appHeap, appRenderingManager, dataManager);
}
void InfoWindowSheetType::SyncCreate(
    app::util::Heap* appHeap,
    app::util::AppRenderingManager* appRenderingManager,
    InfoWindowDataManager* dataManager
)
{
  InfoWindowBase::SyncCreate(appHeap, appRenderingManager, dataManager);
}
bool InfoWindowSheetType::IsCreated(void) const
{
  return InfoWindowBase::IsCreated();
}
bool InfoWindowSheetType::IsOpen(void) const
{
  return this->IsModal();  // 親であるapp::ui::UIViewのメンバ関数
}
bool InfoWindowSheetType::IsDisplay(void) const
{
  return (m_dispState == DISP_STATE_ON_OPEN || m_dispState == DISP_STATE_ON_WORK);
}

void InfoWindowSheetType::StartCloseImmediately(void)
{
  // クローズアニメはないので、正規のクローズと同じ手順で進めるのが最短クローズとなる。
  this->ReqClose();
}

void InfoWindowSheetType::CloseManually(void)
{
  // 複数フレームで行うクローズの手順をここで全て行う。
  
  // StartCloseImmediately
  {
    this->ReqClose();
  }

  // Update
  {
    this->updateCloseReq();
    this->updateDispStateWaitOneFrameAfter();
  }
}

void InfoWindowSheetType::StartOpen(
  InfoWindowSheetTypeListener*  listener,
  const gfl2::math::Vector3&    pos,
  const gfl2::str::StrBuf*      titleStrBuf,
  const gfl2::str::StrBuf*      textStrBuf
)
{
  if(m_dispState == DISP_STATE_OFF)
  {
    m_listener    = listener;
    m_pos         = pos;
    m_titleStrBuf = titleStrBuf;
    m_textStrBuf  = textStrBuf;

    this->SetTextboxPaneMessage(m_titlePane, m_titleStrBuf);  // 親であるapp::ui::UIViewのメンバ関数
    this->SetTextboxPaneMessage(m_textPane, m_textStrBuf);  // 親であるapp::ui::UIViewのメンバ関数
    m_textLineNum = gfl2::str::CountLines( m_textStrBuf->GetPtr() );
    GFL_ASSERT( TEXT_LINE_NUM_MIN<=m_textLineNum && m_textLineNum<=TEXT_LINE_NUM_MAX );  // 開発中に気付かせるためのASSERT。

    // これ以降、他の画面に入力を受け取らせない。
    this->SetModal(true);  // 親であるapp::ui::UIViewのメンバ関数
    m_localListener->SetInputState(InfoWindowSheetTypeLocalListener::INPUT_STATE_OCCUPY);
    m_openReq = true;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。  // 二重オープンはできない。
  }
}

void InfoWindowSheetType::Update(void)
{
  ++m_dispStateCount;
  
  InfoWindowBase::Update();

  ////////////////////////////
  // リクエストの処理
  ////////////////////////////
  if( (!m_closeReq) && m_openReq )
  {
    app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
    if(pG2DUtil)
    {
      m_openStartPosPane->SetTranslate( gfl2::math::ConvertGfVec3ToNwVec3(m_pos) );
      m_openEndPosPane->SetTranslate( gfl2::math::ConvertGfVec3ToNwVec3(-m_pos) );

      u32 animNo = LA_INFORMATION_SML_INFORMATION_SML_LOW_000_TEX_L6;
      if(m_textLineNum == TEXT_LINE_NUM_MIN)         animNo = LA_INFORMATION_SML_INFORMATION_SML_LOW_000_TEX_L2;
      else if(m_textLineNum == 2)                    animNo = LA_INFORMATION_SML_INFORMATION_SML_LOW_000_TEX_L3;
      else if(m_textLineNum == 3)                    animNo = LA_INFORMATION_SML_INFORMATION_SML_LOW_000_TEX_L4;
      else if(m_textLineNum == TEXT_LINE_NUM_MAX -1) animNo = LA_INFORMATION_SML_INFORMATION_SML_LOW_000_TEX_L5;

      pG2DUtil->StartAnime(
          0,  // wk_0
          LA_INFORMATION_SML_INFORMATION_SML_LOW_000_IN
      );
      pG2DUtil->StartAnime(
          0,  // wk_0
          LA_INFORMATION_SML_INFORMATION_SML_LOW_000_KEEP
      );
      pG2DUtil->StartAnime(
          0,  // wk_0
          animNo
      );
    }
    
    if(m_SEIsOn) Sound::PlaySE( SEQ_SE_INFO );  // インフォウィンドウのオープン音

    this->changeDispState(DISP_STATE_WAIT_ONE_FRAME_BEFORE);
    m_openReq = false;
  }

  if(m_closeReq)
  {
    this->updateCloseReq();
  }

  ////////////////////////////
  // DispStateの処理
  ////////////////////////////
  switch(m_dispState)
  {
  case DISP_STATE_WAIT_ONE_FRAME_BEFORE:
    {
      // 2回目3回目呼ぶときに前回のアニメの最終フレームが見えてしまうので、1フレーム非表示で待つ。
      if(m_dispStateCount >= 1)  // 最初のオープンのときアニメの1フレーム目見えるか心配だったが、苦情が来てないので大丈夫だと思う。
      {
        this->changeDispState(DISP_STATE_ON_OPEN);
      }
      break;
    }
  case DISP_STATE_ON_OPEN:
    {
      bool breakFlag = true;
      app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
      if(pG2DUtil)
      {
        if(
            pG2DUtil->IsAnimeEnd(
              0,  // wk_0
              LA_INFORMATION_SML_INFORMATION_SML_LOW_000_IN
            )
        )
        {
          m_localListener->SetInputState(InfoWindowSheetTypeLocalListener::INPUT_STATE_ENABLE);
          this->changeDispState(DISP_STATE_ON_WORK);
          breakFlag = false;
        }
      }
      if(breakFlag)
      {
        break;
      }
    }
  case DISP_STATE_ON_WORK:
    {
      // 何もしない
      break;
    }
  case DISP_STATE_WAIT_ONE_FRAME_AFTER:
    {
      // 1フレ遅れ解放を実現するために、ここで1フレーム待つ。だから、呼び出し元は1フレーム待たずに破棄できる。
      if(m_dispStateCount >= 1)
      {
        this->updateDispStateWaitOneFrameAfter();
      }
      break;
    }
  }
}

void InfoWindowSheetType::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  this->Draw(displayNo, gfl2::lyt::ORDER_NORMAL, 0);
}
void InfoWindowSheetType::Draw(gfl2::gfx::CtrDisplayNo displayNo, u8 order, u32 layerNo)
{
  if(this->IsDisplay())
  {
    app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
    if(pG2DUtil)
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_appRenderingManager, displayNo, 0, order, layerNo );  // wk_0
    }
  }
}

void InfoWindowSheetType::ReqClose(void)
{
  m_localListener->SetInputState(InfoWindowSheetTypeLocalListener::INPUT_STATE_OCCUPY);
  m_closeReq = true;
}  

void InfoWindowSheetType::createLayout(void)
{
  // Layout
  {
    //this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ関数
    //this->CreateGraphicsSystem( m_appHeap, false );  // 親であるapp::ui::UIViewのメンバ関数
    
    void* appLytBuf = m_dataManager->GetDataBuf(InfoWindowDataManager::DATA_ID_SHEET_TYPE);
    
    const app::util::G2DUtil::LytResData resTbl[] =
    {
      // res_0
      {
        appLytBuf,
        0,
        static_cast<u32>(app::util::G2DUtil::ATTACH_ROOT)
      },
    };
 
    const app::util::G2DUtil::LytwkData wkTbl[] =
    {
      // wk_0
      {
        // 下画面
        0,  // res_0
        LYTRES_INFORMATION_SML_INFORMATION_SML_LOW_000_BFLYT,
        LA_INFORMATION_SML_INFORMATION_SML_LOW_000___NUM,
        appLytBuf,
        false,
        app::util::G2DUtil::SETUP_LOWER,
        true
      },
    };
 
    this->Create2D(  // 親であるapp::ui::UIViewのメンバ関数
        m_appHeap,
        NULL,
        GFL_NELEMS(wkTbl), 
        resTbl,
        GFL_NELEMS(resTbl), 
        wkTbl,
        GFL_NELEMS(wkTbl), 
        app::ui::UIView::UNUSE_MESSAGE_DATA
    );
   
    gfl2::lyt::LytWk* lytWk = this->GetLayoutWork( 0 );  // wk_0  // 親であるapp::ui::UIViewのメンバ関数
    m_openStartPosPane = lytWk->GetPane(PANENAME_INFORMATION_SML_LOW_000_PANE_NULL_SCALE);
    m_openEndPosPane   = lytWk->GetPane(PANENAME_INFORMATION_SML_LOW_000_PANE_NULL_PIVOT);
    m_bgPane           = lytWk->GetPane(PANENAME_INFORMATION_SML_LOW_000_PANE_BG_BLACK);
    m_titlePane        = lytWk->GetTextBoxPane(PANENAME_INFORMATION_SML_LOW_000_PANE_TEXTBOX_01);
    m_textPane         = lytWk->GetTextBoxPane(PANENAME_INFORMATION_SML_LOW_000_PANE_TEXTBOX_00);
    GFL_ASSERT(m_openStartPosPane != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_openEndPosPane   != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_bgPane           != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_titlePane        != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_textPane         != NULL);  // 開発中に気付かせるためのASSERT。
  }

  // LocalListener
  {
    m_localListener = GFL_NEW(m_appHeap->GetDeviceHeap()) InfoWindowSheetTypeLocalListener(this);
    this->SetInputListener(m_localListener);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数
    
    m_localListener->SetInputState(InfoWindowSheetTypeLocalListener::INPUT_STATE_RELEASE);
  }
}

void InfoWindowSheetType::destroyLayout(void)
{
  // Layout
  {
    this->Delete2D();  // 親であるapp::ui::UIViewのメンバ関数
    this->DeleteGraphicsSystem();  // 親であるapp::ui::UIViewのメンバ関数
  }

  // LocalListener
  {
    this->SetInputListener(NULL);
    GFL_SAFE_DELETE(m_localListener);
  }
}

void InfoWindowSheetType::changeDispState(DispState dispState)
{
  m_dispState = dispState;
  m_dispStateCount = 0;
}

void InfoWindowSheetType::updateCloseReq(void)
{
  this->changeDispState(DISP_STATE_WAIT_ONE_FRAME_AFTER);
  m_closeReq = false;
  m_openReq = false;  // オープンリクエストが出ているときにクローズしても問題ないようにしておく。
}
void InfoWindowSheetType::updateDispStateWaitOneFrameAfter(void)
{
  this->changeDispState(DISP_STATE_OFF);
  // これ以降、他の画面に入力を解放する。
  this->SetModal(false);  // 親であるapp::ui::UIViewのメンバ関数
  m_localListener->SetInputState(InfoWindowSheetTypeLocalListener::INPUT_STATE_RELEASE);
  if(m_listener)
  {
    m_listener->OnClose();
  }
}


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

