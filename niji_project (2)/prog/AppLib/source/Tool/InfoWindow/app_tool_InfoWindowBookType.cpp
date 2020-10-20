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

#include <Print/include/WordSet.h>
#include <Sound/include/Sound.h>

#include "app_tool_InfoWindowDefine.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h"


#define APP_TOOL_INFOWINDOWBOOKTYPE_DEBUG_ENABLE (PM_DEBUG&&(0))  // 必ず0でコミット


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)


//==============================================================================
/**
 @class  InfoWindowBookTypeLocalListener
 @brief  インフォウィンドウの内部リスナー
 */
class InfoWindowBookTypeLocalListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(InfoWindowBookTypeLocalListener);

public:
  InfoWindowBookTypeLocalListener(InfoWindowBookType* view)
    : app::ui::UIInputListener(),
      m_view(view),
      m_inputState(INPUT_STATE_RELEASE)
  {}
  virtual ~InfoWindowBookTypeLocalListener()
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
        if( m_view->GetButtonManager()->GetUpdateState() == app::tool::ButtonManager::UPDATE_STATE_IDLE )  // ボタンマネージャ動作中はタッチさせない
        {
          u16 x = pTouchPanel->GetX();
          u16 y = pTouchPanel->GetY();
          if( pTouchPanel->IsTouchRelease() )
          {
            m_view->ReqTouchRelease(x, y);
          }
          else if( pTouchPanel->IsTouchTrigger() )
          {
            m_view->ReqTouchTrigger(x, y);
          }
          else if( pTouchPanel->IsTouch() )
          {
            m_view->ReqTouchHold(x, y);
          }
        } 
        res = app::ui::UIInputListener::DISABLE_ACCESS;
        break;
      }
    }
    return res;
  }
  /*
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
            Sound::PlaySE( SEQ_SE_DECIDE1 );
          }
          else
          {
            Sound::PlaySE( SEQ_SE_CANCEL1 );
          }
          m_view->ReqClose();
        }
        res = app::ui::UIInputListener::DISABLE_ACCESS;
        break;
      }
    }
    return res;
  }
  */
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId )
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
        u32 button_id = painId;
        switch(button_id)
        {
        case InfoWindowBookType::BUTTON_ID_B:
          {
            m_view->ReqClose();
            break;
          }
        case InfoWindowBookType::BUTTON_ID_A:
          {
            m_view->ReqCommand();
            break;
          }
        case InfoWindowBookType::BUTTON_ID_L:
          {
            m_view->ReqGoToPrevPage();
            break;
          }
        case InfoWindowBookType::BUTTON_ID_R:
          {
            m_view->ReqGoToNextPage();
            break;
          }
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
  void SetInputState(InputState inputState, bool buttonManagerInputDisable=false)  // @fix NMCat[2115]再報告 ページ切り替え中にボタン入力が可能なタイミングがある
                                                                                   //                        →ボタンマネージャへの入力だけ禁止しなければならないときがあるのでbuttonManagerInputDisableを用意
  {
    m_inputState = inputState;
    
    app::tool::ButtonManager* buttonManager = m_view->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数
    if(buttonManagerInputDisable)
    {
      // buttonManagerInputDisableがtrueのときは、ボタンマネージャへの入力を禁止する。
      buttonManager->SetInputEnable( false );
    }
    else
    {
      // buttonManagerInputDisableがfalseのときは、ボタンマネージャへの入力を禁止するか否かはm_inputState次第。
      buttonManager->SetInputEnable( m_inputState == INPUT_STATE_ENABLE );
    }
  }

private:
  InfoWindowBookType* m_view;
  InputState          m_inputState;
};


//==============================================================================
//
//  InfoWindowBookType
//
InfoWindowBookType::InfoWindowBookType(app::util::Heap* appHeap)
  : InfoWindowBase(appHeap),
    m_argPageNumMax(0),
    m_pageNumMax(0),
    m_listener(NULL),
    m_pos(gfl2::math::Vector3(0,0,0)),
    m_contents(NULL),
    m_pageNum(0),
    m_pageNo(0),
    m_keepFlag(false),
    m_commandButtonIsActive(true),
    m_localListener(NULL),
    m_dispState(DISP_STATE_OFF),
    m_dispStateCount(0),
    m_openReq(false),
    m_closeReq(false),
    m_openType(OPEN_TYPE_SCALE_ANIM),
    m_openAnim(0),
    m_wordSet(NULL),
    m_commandPane(NULL),
    m_frameRootPane(NULL),
    m_frameRootFixPos(gfl2::math::Vector3(0,0,0)),
    m_touchPosNum(0),
    m_setupWhenGoToPageByTouchFlag(false),
    m_touchNow(false)
{
  for(s32 i=0; i<FRAME_POS_NUM; ++i)
  {
    m_framePosPane[i] = NULL;
    m_framePosFixPos[i] = gfl2::math::Vector3(0,0,0);
  }

  m_touchTriggerPos.x=0;  m_touchTriggerPos.y=0;
  for(s32 i=0; i<TOUCH_POS_NUM_MAX; ++i)
  {
    m_touchPos[i].x=0;  m_touchPos[i].y=0;
  }
}
InfoWindowBookType::~InfoWindowBookType()
{
  this->destructa();  // 親であるInfoWindowBaseのメンバ関数
}

bool InfoWindowBookType::EndFunc(void)
{
  return InfoWindowBase::EndFunc();
}

void InfoWindowBookType::StartAsyncCreate(
    app::util::Heap* appHeap,
    app::util::AppRenderingManager* appRenderingManager,
    u32 pageNumMax,
    InfoWindowDataManager* dataManager
)
{
  m_argPageNumMax = pageNumMax;
  InfoWindowBase::StartAsyncCreate(appHeap, appRenderingManager, dataManager);
}

void InfoWindowBookType::SyncCreate(
    app::util::Heap* appHeap,
    app::util::AppRenderingManager* appRenderingManager,
    u32 pageNumMax,
    InfoWindowDataManager* dataManager
)
{
  m_argPageNumMax = pageNumMax;
  InfoWindowBase::SyncCreate(appHeap, appRenderingManager, dataManager);
}

bool InfoWindowBookType::IsCreated(void) const
{
  return InfoWindowBase::IsCreated();
}

bool InfoWindowBookType::IsOpen(void) const
{
  return this->IsModal();  // 親であるapp::ui::UIViewのメンバ関数
}

bool InfoWindowBookType::IsDisplay(void) const
{
  return (
       m_dispState == DISP_STATE_ON_OPEN
    || m_dispState == DISP_STATE_ON_WORK
    || m_dispState == DISP_STATE_ON_TOUCH
    || m_dispState == DISP_STATE_ON_GO_TO_CURR_PAGE
    || m_dispState == DISP_STATE_ON_GO_TO_PREV_PAGE
    || m_dispState == DISP_STATE_ON_GO_TO_NEXT_PAGE
  );
}

void InfoWindowBookType::StartCloseImmediately(void)
{
  // クローズアニメはないので、正規のクローズと同じ手順で進めるのが最短クローズとなる。
  this->ReqClose();
}

void InfoWindowBookType::CloseManually(void)
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

void InfoWindowBookType::SetSEOn(bool isOn)
{
  InfoWindowBase::SetSEOn(isOn);
  this->setButtonManagerSEOn();
}

void InfoWindowBookType::SetCommandButtonActive(bool isActive)
{
  m_commandButtonIsActive = isActive;
  this->updateCommandButtonActive(); 
}

void InfoWindowBookType::StartOpen(
    InfoWindowBookTypeListener*   listener,
    const gfl2::math::Vector3&    pos,
    OpenType                      openType
)
{
  if(m_dispState == DISP_STATE_OFF)
  {
    m_listener = listener;
    m_pos      = pos;
    m_openType = openType;
    
    m_openAnim = LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000_IN;
    if(m_openType == OPEN_TYPE_NO_ANIM) m_openAnim = LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000_IN_00;

    GFL_ASSERT(m_pageNum>0);  // 開発中に気付かせるためのASSERT。  // ResetContentsを呼んでページ数設定をしていない。
    if(m_keepFlag)
    {
      GFL_ASSERT(this->areAllPageContentsSet());  // 開発中に気付かせるためのASSERT。  // コンテンツを保持するフラグが立っているのに、前ページが設定されていない。
    }
    m_pageNo = 0;

    // これ以降、他の画面に入力を受け取らせない。
    this->SetModal(true);  // 親であるapp::ui::UIViewのメンバ関数
    m_localListener->SetInputState(InfoWindowBookTypeLocalListener::INPUT_STATE_OCCUPY);
    m_openReq = true;
    m_touchNow = false;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。  // 二重オープンはできない。
  }
}

void InfoWindowBookType::StartClose(void)
{
  if(m_dispState == DISP_STATE_ON_WORK)  // InfoWindowBookTypeは外からクローズを呼べてしまう。開き掛けなどのように何かしている最中のときはクローズできないことにしておく。
  {
    this->ReqClose();
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。  // オープン中でないと閉じれない。
  }
} 

void InfoWindowBookType::ResetContents(u32 pageNum, bool keepFlag)
{
  GFL_ASSERT(pageNum <= m_pageNumMax);  // 開発中に気付かせるためのASSERT。

  for(u32 i=0; i<m_pageNumMax; ++i)
  {
    m_contents[i] = Content();
  }
  m_pageNum = pageNum;
  m_keepFlag = keepFlag;

  // aa/bbのbb
  this->setNumberToPageTextPane(m_frameContentProperty[FRAME_CONTENT_FRONT].pageNumTextPane, m_pageNum);
  this->setNumberToPageTextPane(m_frameContentProperty[FRAME_CONTENT_BACK ].pageNumTextPane, m_pageNum);
}
void InfoWindowBookType::SetTextContent(
    u32                      pageNo,
    const gfl2::str::StrBuf* textStrBuf,
    const gfl2::str::StrBuf* commandStrBuf
)
{
  if(pageNo < m_pageNum)
  {
    m_contents[pageNo].contentType = Content::CONTENT_TYPE_TEXT;
    m_contents[pageNo].contentUnion.textContent.textStrBuf    = textStrBuf;
    m_contents[pageNo].contentUnion.textContent.commandStrBuf = commandStrBuf;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。  // ページ番号がおかしい。
  }
}
void InfoWindowBookType::SetTextureAndTextContent(
    u32                                  pageNo,
    const gfl2::lyt::LytResTextureInfo*  textureInfo,
    const gfl2::str::StrBuf*             textStrBuf,
    const gfl2::str::StrBuf*             commandStrBuf
)
{
  if(pageNo < m_pageNum)
  {
    m_contents[pageNo].contentType = Content::CONTENT_TYPE_TEXTURE_AND_TEXT;
    m_contents[pageNo].contentUnion.textureAndTextContent.textureInfo   = textureInfo;
    m_contents[pageNo].contentUnion.textureAndTextContent.textStrBuf    = textStrBuf;
    m_contents[pageNo].contentUnion.textureAndTextContent.commandStrBuf = commandStrBuf;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。  // ページ番号がおかしい。
  }
}
void InfoWindowBookType::SetQRContent(
    u32                                  pageNo,
    const gfl2::lyt::LytResTextureInfo*  qrTextureInfo,
    const gfl2::str::StrBuf*             commandStrBuf
)
{
  if(pageNo < m_pageNum)
  {
    m_contents[pageNo].contentType = Content::CONTENT_TYPE_QR;
    m_contents[pageNo].contentUnion.qrContent.qrTextureInfo = qrTextureInfo;
    m_contents[pageNo].contentUnion.qrContent.commandStrBuf = commandStrBuf;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。  // ページ番号がおかしい。
  }
}

void InfoWindowBookType::Update(void)
{
#if APP_TOOL_INFOWINDOWBOOKTYPE_DEBUG_ENABLE
  static const char* DISP_STATE_MES[] =
  {
    "OFF",                  
    "WAIT_ONE_FRAME_BEFORE",
    "ON_OPEN",              
    "ON_WORK",              
    "ON_TOUCH",             
    "ON_GO_TO_CURR_PAGE",   
    "ON_GO_TO_PREV_PAGE",   
    "ON_GO_TO_NEXT_PAGE",   
    "WAIT_ONE_FRAME_AFTER", 
  }; 
  GFL_PRINT("0: %s\n", DISP_STATE_MES[m_dispState]);
#endif

  static const f32 MOVE_VELOCITY = 40.0f;//80.0f;80は速過ぎ

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

      pG2DUtil->StartAnime(
          0,  // wk_0
          m_openAnim
      );
      pG2DUtil->StartAnime(
          0,  // wk_0
          LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000_KEEP
      );
     
      // コンテンツがあればオープン前にセットアップできるのでsetupContentを呼んでおく(コンテンツがなくてもOK状態で呼ぶ)。
      this->setupContent(true, FRAME_POS_C);
      this->setupCommonObj();

      // オープン前は前と次のページは非表示にしておく
      m_framePosPane[FRAME_POS_L]->SetVisible(false);
      m_framePosPane[FRAME_POS_R]->SetVisible(false);
    }

    if(m_SEIsOn) Sound::PlaySE( SEQ_SE_INFO );  // インフォウィンドウのオープン音
    
    this->changeDispState(DISP_STATE_WAIT_ONE_FRAME_BEFORE);
    m_openReq = false;
  }

  if(m_closeReq)
  {
    if(m_dispState == DISP_STATE_ON_WORK)  // @fix NMCat[2115] ページ切り替え中にボタン入力が可能なタイミングがある  →ページが落ち着くまでクローズリクエストを待たせておく。
    {
      this->updateCloseReq();
    }
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
              m_openAnim
            )
        )
        {
          m_localListener->SetInputState(InfoWindowBookTypeLocalListener::INPUT_STATE_ENABLE);
          if(m_listener)
          {
            m_listener->OnChangePage(m_pageNo);
          }
          this->setupContent(false, FRAME_POS_C);
          this->setupCommonObj();

          // オープン後は前と次のページは表示しておく
          m_framePosPane[FRAME_POS_L]->SetVisible(true);
          m_framePosPane[FRAME_POS_R]->SetVisible(true);
 
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
      break;
    }
  case DISP_STATE_ON_TOUCH:
    {
      if(m_touchNow)  // @fix NMCat[2115] ページ切り替え中にボタン入力が可能なタイミングがある  →タッチ継続中のときはDISP_STATE_ON_TOUCHの内容を実行する。
      {
        // 移動量
        s32 startX = m_touchTriggerPos.x;
        s32 endX   = m_touchPos[0].x;
       
        // 移動させる
        gfl2::math::Vector3 pos = m_frameRootFixPos;
        pos.x += endX - startX;
        if(pos.x < m_framePosFixPos[FRAME_POS_C].x)
        {
          if(!this->canGoToNextPage())
          {
            pos.x = m_framePosFixPos[FRAME_POS_C].x;
            // 移動できないときはm_touchTriggerPosをセットし直す
            this->setTouchTrigger(m_touchPos[0].x, m_touchPos[0].y);
          }
        }
        else  // if(pos.x >= m_framePosFixPos[FRAME_POS_C].x)
        {
          if(!this->canGoToPrevPage())
          {
            pos.x = m_framePosFixPos[FRAME_POS_C].x;
            // 移動できないときはm_touchTriggerPosをセットし直す
            this->setTouchTrigger(m_touchPos[0].x, m_touchPos[0].y);
          }
        }
        m_frameRootPane->SetTranslate(gfl2::math::ConvertGfVec3ToNwVec3(pos));
       
        // 前次のページをセットアップする
        // 毎フレームセットアップしているが、大した負荷ではないだろうからいいだろう。
        if(pos.x < m_framePosFixPos[FRAME_POS_C].x)
        {
          // 次のページをセットアップ
          if(this->canGoToNextPage())
          {
            this->setupContent(true, FRAME_POS_R);
          }
        }
        else  // if(pos.x >= m_framePosFixPos[FRAME_POS_C].x)
        {
          // 前のページをセットアップ
          if(this->canGoToPrevPage())
          {
            this->setupContent(true, FRAME_POS_L);
          }
        }
      }
      else  // @fix NMCat[2115] ページ切り替え中にボタン入力が可能なタイミングがある  →タッチ終了時はReqTouchReleaseを呼んで状態を変更する。
      {
        this->ReqTouchRelease(0,0);  // (0,0)は使われないのでてきとー
      }
      break;
    }
  case DISP_STATE_ON_GO_TO_CURR_PAGE:
    {
      bool reachFlag = false;
      gfl2::math::Vector3 pos;
      pos = gfl2::math::ConvertNwVec3ToGfVec3( m_frameRootPane->GetTranslate() );
      if(pos.x < m_framePosFixPos[FRAME_POS_C].x)
      {
        pos.x += MOVE_VELOCITY;
        if(pos.x >= m_framePosFixPos[FRAME_POS_C].x) reachFlag = true;
      }
      else  // if(pos.x >= m_framePosFixPos[FRAME_POS_C].x)
      {
        pos.x -= MOVE_VELOCITY;
        if(pos.x <= m_framePosFixPos[FRAME_POS_C].x) reachFlag = true;
      }
      if(reachFlag)
      {
        m_frameRootPane->SetTranslate(gfl2::math::ConvertGfVec3ToNwVec3(m_frameRootFixPos));
        m_localListener->SetInputState(InfoWindowBookTypeLocalListener::INPUT_STATE_ENABLE);
        this->changeDispState(DISP_STATE_ON_WORK);
      }
      else
      {
        m_frameRootPane->SetTranslate(gfl2::math::ConvertGfVec3ToNwVec3(pos));
      }
      break;
    }
  case DISP_STATE_ON_GO_TO_PREV_PAGE:
    {
      gfl2::math::Vector3 pos;
      pos = gfl2::math::ConvertNwVec3ToGfVec3( m_frameRootPane->GetTranslate() );
      pos.x += MOVE_VELOCITY;
      if(pos.x >= m_framePosFixPos[FRAME_POS_R].x)
      {
        m_frameRootPane->SetTranslate(gfl2::math::ConvertGfVec3ToNwVec3(m_frameRootFixPos));
        m_localListener->SetInputState(InfoWindowBookTypeLocalListener::INPUT_STATE_ENABLE);
        if(this->canGoToPrevPage())
        {
          this->swapFrameContent();  // この後でFRAME_POS_Cに対してsetupContentをしているので、スワップはしなくてもいいのでコメントアウトしておく。→下記修正のため必要となりコメントアウトをやめる。
                                     // @fix GFNMCat[1342] 「レギュレーションDL」＞「くわしくみる」にてページ切り替えをすると元のページの内容が一瞬表示される
                                     // FRONTだけを書き換える方法では一瞬書き換え前の状態が見えてしまうので、あらかじめ書いておいたBACK(=新FRONT)と入れ替えることにした。
          --m_pageNo;
          if(m_listener)
          {
            m_listener->OnChangePage(m_pageNo);
          }
          this->setupContent(false, FRAME_POS_C);
          this->setupCommonObj();
        }
        this->changeDispState(DISP_STATE_ON_WORK);

        m_setupWhenGoToPageByTouchFlag = false;  // @fix NMCat[1038] インフォメーションの最初と最後のページをタッチ操作で送ると、操作不能になる
                                                 // ページの移動が終わったので、移動中処理のフラグは落としておく。
      }
      else
      {
        m_frameRootPane->SetTranslate(gfl2::math::ConvertGfVec3ToNwVec3(pos));
      }
      // 移動中に前のページをセットアップ
      if(m_setupWhenGoToPageByTouchFlag)
      {
        if(pos.x >= m_framePosFixPos[FRAME_POS_C].x)
        {
          this->setupContent(true, FRAME_POS_L);
          m_setupWhenGoToPageByTouchFlag = false;
        }
      }
      break;
    }
  case DISP_STATE_ON_GO_TO_NEXT_PAGE:
    {
      gfl2::math::Vector3 pos;
      pos = gfl2::math::ConvertNwVec3ToGfVec3( m_frameRootPane->GetTranslate() );
      pos.x -= MOVE_VELOCITY;
      if(pos.x <= m_framePosFixPos[FRAME_POS_L].x)
      {
        m_frameRootPane->SetTranslate(gfl2::math::ConvertGfVec3ToNwVec3(m_frameRootFixPos));
        m_localListener->SetInputState(InfoWindowBookTypeLocalListener::INPUT_STATE_ENABLE);
        if(this->canGoToNextPage())
        {
          this->swapFrameContent();  // この後でFRAME_POS_Cに対してsetupContentをしているので、スワップはしなくてもいいのでコメントアウトしておく。→下記修正のため必要となりコメントアウトをやめる。
                                     // @fix GFNMCat[1342] 「レギュレーションDL」＞「くわしくみる」にてページ切り替えをすると元のページの内容が一瞬表示される
                                     // FRONTだけを書き換える方法では一瞬書き換え前の状態が見えてしまうので、あらかじめ書いておいたBACK(=新FRONT)と入れ替えることにした。
          ++m_pageNo;
          if(m_listener)
          {
            m_listener->OnChangePage(m_pageNo);
          }
          this->setupContent(false, FRAME_POS_C);
          this->setupCommonObj();
        }
        this->changeDispState(DISP_STATE_ON_WORK);

        m_setupWhenGoToPageByTouchFlag = false;  // @fix NMCat[1038] インフォメーションの最初と最後のページをタッチ操作で送ると、操作不能になる
                                                 // ページの移動が終わったので、移動中処理のフラグは落としておく。
      }
      else
      {
        m_frameRootPane->SetTranslate(gfl2::math::ConvertGfVec3ToNwVec3(pos));
      }
      // 移動中に次のページをセットアップ
      if(m_setupWhenGoToPageByTouchFlag)
      {
        if(pos.x <= m_framePosFixPos[FRAME_POS_C].x)
        {
          this->setupContent(true, FRAME_POS_R);
          m_setupWhenGoToPageByTouchFlag = false;
        }
      }
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

#if APP_TOOL_INFOWINDOWBOOKTYPE_DEBUG_ENABLE
  GFL_PRINT("1: t%d\n", m_touchNow);
#endif

  // 毎フレーム最後に初期化
  m_touchNow = false;
 
#if APP_TOOL_INFOWINDOWBOOKTYPE_DEBUG_ENABLE
  GFL_PRINT("1: %s\n", DISP_STATE_MES[m_dispState]);
#endif
}

void InfoWindowBookType::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  this->Draw(displayNo, gfl2::lyt::ORDER_NORMAL, 0);
}
void InfoWindowBookType::Draw(gfl2::gfx::CtrDisplayNo displayNo, u8 order, u32 layerNo)
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

void InfoWindowBookType::ReqClose(void)
{
  m_localListener->SetInputState(InfoWindowBookTypeLocalListener::INPUT_STATE_OCCUPY);
  m_closeReq = true;
}  

void InfoWindowBookType::ReqCommand(void)
{
  if(m_listener)
  {
    m_listener->OnCommand(m_pageNo);
  }
}  

void InfoWindowBookType::ReqTouchTrigger(u16 x, u16 y)
{
  if(m_dispState == DISP_STATE_ON_WORK)  // @fix NMCat[2115] ページ切り替え中にボタン入力が可能なタイミングがある  →ページが落ち着くまでタッチ入力を受け付けない(m_dispStateを変更できない)ようにしておく。
  {
    this->setTouchTrigger(x, y);
    m_localListener->SetInputState(InfoWindowBookTypeLocalListener::INPUT_STATE_ENABLE, true);  // @fix NMCat[2115]再報告 ページ切り替え中にボタン入力が可能なタイミングがある
                                                                                                //                        →ボタンマネージャへの入力を禁止し、タッチだけしか受け付けないようにしておく
    this->changeDispState(DISP_STATE_ON_TOUCH);
    m_touchNow = true;
  }
}
void InfoWindowBookType::ReqTouchHold(u16 x, u16 y)
{
  if(m_dispState == DISP_STATE_ON_WORK)  // @fix NMCat[2115] ページ切り替え中にボタン入力が可能なタイミングがある  →ページが落ち着いているのにタッチ状態でないときは、タッチ開始にする。
  {
    this->ReqTouchTrigger(x, y);
  }
  else if(m_dispState == DISP_STATE_ON_TOUCH)  // @fix NMCat[2115] ページ切り替え中にボタン入力が可能なタイミングがある  →タッチ状態でなければタッチ入力は無視する。
  {
    for(s32 i=0; i<m_touchPosNum-1; ++i)
    {
      m_touchPos[i+1].x=m_touchPos[i].x;  m_touchPos[i+1].y=m_touchPos[i].y;
    }
    m_touchPos[0].x=x;  m_touchPos[0].y=y;
    if(m_touchPosNum < TOUCH_POS_NUM_MAX)
    {
      ++m_touchPosNum;
    }
    m_touchNow = true;
  }
}
void InfoWindowBookType::ReqTouchRelease(u16 /*x*/, u16 /*y*/)  // 放したときの位置は使わない
{
  if(m_dispState == DISP_STATE_ON_TOUCH)  // @fix NMCat[2115] ページ切り替え中にボタン入力が可能なタイミングがある  →タッチ状態でなければタッチリリースは無視する。
  {
    m_localListener->SetInputState(InfoWindowBookTypeLocalListener::INPUT_STATE_OCCUPY);  // ここから自動移動なるので、入力を受け付けないようにしておく。
    bool goToPrevNextPageFlag = false; 
 
    // 移動方向を速度で判定or位置で判定
    TouchMoveDir touchMoveDir            = this->calcTouchMoveDir();
    FramePos     frameRootPanePosSection = this->calcFrameRootPanePosSection();
    switch(touchMoveDir)
    {
    case TOUCH_MOVE_DIR_LEFT:
      {
        // 速度で判定
        if(frameRootPanePosSection==FRAME_POS_R)
        {
          this->changeDispState(DISP_STATE_ON_GO_TO_CURR_PAGE);
        }
        else
        {
          if(this->canGoToNextPage())
          {
            goToPrevNextPageFlag = true;
            this->changeDispState(DISP_STATE_ON_GO_TO_NEXT_PAGE);
            // 今右側にいるかもしれないので、移動中に次のページをセットアップしなければならない
            m_setupWhenGoToPageByTouchFlag = true;
          }
          else
          {
            this->changeDispState(DISP_STATE_ON_GO_TO_CURR_PAGE);
          }
        }
        break;
      }
    case TOUCH_MOVE_DIR_RIGHT:
      {
        // 速度で判定
        if(frameRootPanePosSection==FRAME_POS_L)
        {
          this->changeDispState(DISP_STATE_ON_GO_TO_CURR_PAGE);
        }
        else
        {
          if(this->canGoToPrevPage())
          {
            goToPrevNextPageFlag = true;
            this->changeDispState(DISP_STATE_ON_GO_TO_PREV_PAGE);
            // 今左側にいるかもしれないので、移動中に前のページをセットアップしなければならない
            m_setupWhenGoToPageByTouchFlag = true;
          }
          else
          {
            this->changeDispState(DISP_STATE_ON_GO_TO_CURR_PAGE);
          }
        }
        break;
      }
    case TOUCH_MOVE_DIR_STOP:
    default:
      {
        // 位置で判定
        switch(frameRootPanePosSection)
        {
        case FRAME_POS_L:
          goToPrevNextPageFlag = true;
          // 今左側に来れているということは、次のページはセットアップ済み
          this->changeDispState(DISP_STATE_ON_GO_TO_NEXT_PAGE);
          m_setupWhenGoToPageByTouchFlag = false;
          break;
        case FRAME_POS_R:
          goToPrevNextPageFlag = true;
          // 今右側に来れているということは、前のページはセットアップ済み
          this->changeDispState(DISP_STATE_ON_GO_TO_PREV_PAGE);
          m_setupWhenGoToPageByTouchFlag = false;
          break;
        case FRAME_POS_C:
        default:
          this->changeDispState(DISP_STATE_ON_GO_TO_CURR_PAGE);
          break;
        }
        break;
      }
    }
 
    if(goToPrevNextPageFlag) 
    {
      if(m_SEIsOn) Sound::PlaySE( SEQ_SE_PAGE );
    }
  }
}

void InfoWindowBookType::ReqGoToPrevPage(void)
{
  if(this->canGoToPrevPage())
  {
    m_localListener->SetInputState(InfoWindowBookTypeLocalListener::INPUT_STATE_OCCUPY);
    this->setupContent(true, FRAME_POS_L);
    this->changeDispState(DISP_STATE_ON_GO_TO_PREV_PAGE);
    m_setupWhenGoToPageByTouchFlag = false;
  }
}  

void InfoWindowBookType::ReqGoToNextPage(void)
{
  if(this->canGoToNextPage())
  {
    m_localListener->SetInputState(InfoWindowBookTypeLocalListener::INPUT_STATE_OCCUPY);
    this->setupContent(true, FRAME_POS_R);
    this->changeDispState(DISP_STATE_ON_GO_TO_NEXT_PAGE);
    m_setupWhenGoToPageByTouchFlag = false;
  }
}  

void InfoWindowBookType::createLayout(void)
{
  // Layout
  gfl2::lyt::LytWk*        lytWk_0  = NULL;
  app::util::G2DUtil*      pG2DUtil = NULL;  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytMultiResID lytRes_0;

  {
    //this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ関数
    //this->CreateGraphicsSystem( m_appHeap, false );  // 親であるapp::ui::UIViewのメンバ関数
    
    void* appLytBuf = m_dataManager->GetDataBuf(InfoWindowDataManager::DATA_ID_BOOK_TYPE);
    
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
        LYTRES_INFORMATION_LRG_INFORMATION_LRG_LOW_000_BFLYT,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000___NUM,
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
   
    lytWk_0  = this->GetLayoutWork( 0 );  // wk_0  // 親であるapp::ui::UIViewのメンバ関数
    pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
    lytRes_0 = pG2DUtil->GetLayoutResourceID( 0 );  // res_0

    gfl2::lyt::LytParts* commandPartsPane = lytWk_0->GetPartsPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_BUTTON_A);

    m_openStartPosPane = lytWk_0->GetPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_NULL_SCALE);
    m_openEndPosPane   = lytWk_0->GetPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_NULL_PIVOT);
    m_bgPane           = lytWk_0->GetPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_BG_BLACK);

    m_commandPane   = lytWk_0->GetTextBoxPane(commandPartsPane, PANENAME_COMMON_BTN_LOW_017_PANE_TEXTBOX_00, &lytRes_0);
    m_frameRootPane = lytWk_0->GetPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_WINDOW_ALL);
    GFL_ASSERT(m_commandPane   != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_frameRootPane != NULL);  // 開発中に気付かせるためのASSERT。
      
    m_frameRootFixPos = gfl2::math::ConvertNwVec3ToGfVec3( m_frameRootPane->GetTranslate() );

    m_framePosPane[FRAME_POS_L] = lytWk_0->GetPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_POS_02);
    m_framePosPane[FRAME_POS_C] = lytWk_0->GetPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_POS_00);
    m_framePosPane[FRAME_POS_R] = lytWk_0->GetPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_POS_01);
    GFL_ASSERT(m_framePosPane[FRAME_POS_L] != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_framePosPane[FRAME_POS_C] != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_framePosPane[FRAME_POS_R] != NULL);  // 開発中に気付かせるためのASSERT。
    
    for(s32 i=0; i<FRAME_POS_NUM; ++i)
    {
      m_framePosFixPos[i] = gfl2::math::ConvertNwVec3ToGfVec3( m_framePosPane[i]->GetTranslate() );
    }

    m_frameContentProperty[FRAME_CONTENT_FRONT].frameTopPane            = lytWk_0->GetPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_WINDOW_00);
    m_frameContentProperty[FRAME_CONTENT_FRONT].contentTopPane          = lytWk_0->GetPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_CONTENT_00);
    m_frameContentProperty[FRAME_CONTENT_FRONT].contentVisiblePane      = lytWk_0->GetPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_VISIBLE_00);
    m_frameContentProperty[FRAME_CONTENT_FRONT].pageNoTextPane          = lytWk_0->GetTextBoxPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_TEXT99_L);
    m_frameContentProperty[FRAME_CONTENT_FRONT].pageNumTextPane         = lytWk_0->GetTextBoxPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_TEXT99_R);
    m_frameContentProperty[FRAME_CONTENT_FRONT].slashTextPane           = lytWk_0->GetTextBoxPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_TEXT_SLASH);
    m_frameContentProperty[FRAME_CONTENT_FRONT].textPane                = lytWk_0->GetTextBoxPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_TEXTBOX_L10);
    m_frameContentProperty[FRAME_CONTENT_FRONT].texturePane             = lytWk_0->GetPicturePane(PANENAME_INFORMATION_LRG_LOW_000_PANE_PICTURE);
    m_frameContentProperty[FRAME_CONTENT_FRONT].qrTexturePane           = lytWk_0->GetPicturePane(PANENAME_INFORMATION_LRG_LOW_000_PANE_QR);
    m_frameContentProperty[FRAME_CONTENT_FRONT].animNoText              = LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000_PATERN_00;
    m_frameContentProperty[FRAME_CONTENT_FRONT].animNoTextureAndText    = LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000_PATERN_01;
    m_frameContentProperty[FRAME_CONTENT_FRONT].animNoQR                = LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000_PATERN_02;                     
    GFL_ASSERT(m_frameContentProperty[FRAME_CONTENT_FRONT].frameTopPane        != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_frameContentProperty[FRAME_CONTENT_FRONT].contentTopPane      != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_frameContentProperty[FRAME_CONTENT_FRONT].contentVisiblePane  != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_frameContentProperty[FRAME_CONTENT_FRONT].pageNoTextPane      != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_frameContentProperty[FRAME_CONTENT_FRONT].pageNumTextPane     != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_frameContentProperty[FRAME_CONTENT_FRONT].slashTextPane       != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_frameContentProperty[FRAME_CONTENT_FRONT].textPane            != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_frameContentProperty[FRAME_CONTENT_FRONT].texturePane         != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_frameContentProperty[FRAME_CONTENT_FRONT].qrTexturePane       != NULL);  // 開発中に気付かせるためのASSERT。
    
    m_frameContentProperty[FRAME_CONTENT_BACK ].frameTopPane            = lytWk_0->GetPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_WINDOW_01);
    m_frameContentProperty[FRAME_CONTENT_BACK ].contentTopPane          = lytWk_0->GetPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_CONTENT_01);
    m_frameContentProperty[FRAME_CONTENT_BACK ].contentVisiblePane      = lytWk_0->GetPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_VISIBLE_01);
    m_frameContentProperty[FRAME_CONTENT_BACK ].pageNoTextPane          = lytWk_0->GetTextBoxPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_TEXT99_L_00);
    m_frameContentProperty[FRAME_CONTENT_BACK ].pageNumTextPane         = lytWk_0->GetTextBoxPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_TEXT99_R_00);
    m_frameContentProperty[FRAME_CONTENT_BACK ].slashTextPane           = lytWk_0->GetTextBoxPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_TEXT_SLASH_00);
    m_frameContentProperty[FRAME_CONTENT_BACK ].textPane                = lytWk_0->GetTextBoxPane(PANENAME_INFORMATION_LRG_LOW_000_PANE_TEXTBOX_L10_01);
    m_frameContentProperty[FRAME_CONTENT_BACK ].texturePane             = lytWk_0->GetPicturePane(PANENAME_INFORMATION_LRG_LOW_000_PANE_PICTURE_01);
    m_frameContentProperty[FRAME_CONTENT_BACK ].qrTexturePane           = lytWk_0->GetPicturePane(PANENAME_INFORMATION_LRG_LOW_000_PANE_QR_01);
    m_frameContentProperty[FRAME_CONTENT_BACK ].animNoText              = LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000_PATERN_03;
    m_frameContentProperty[FRAME_CONTENT_BACK ].animNoTextureAndText    = LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000_PATERN_04;
    m_frameContentProperty[FRAME_CONTENT_BACK ].animNoQR                = LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000_PATERN_05;                     
    GFL_ASSERT(m_frameContentProperty[FRAME_CONTENT_BACK ].frameTopPane        != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_frameContentProperty[FRAME_CONTENT_BACK ].contentTopPane      != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_frameContentProperty[FRAME_CONTENT_BACK ].contentVisiblePane  != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_frameContentProperty[FRAME_CONTENT_BACK ].pageNoTextPane      != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_frameContentProperty[FRAME_CONTENT_BACK ].pageNumTextPane     != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_frameContentProperty[FRAME_CONTENT_BACK ].slashTextPane       != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_frameContentProperty[FRAME_CONTENT_BACK ].textPane            != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_frameContentProperty[FRAME_CONTENT_BACK ].texturePane         != NULL);  // 開発中に気付かせるためのASSERT。
    GFL_ASSERT(m_frameContentProperty[FRAME_CONTENT_BACK ].qrTexturePane       != NULL);  // 開発中に気付かせるためのASSERT。
    
    m_framePosPane[FRAME_POS_L]->SetVisible(false);
    m_framePosPane[FRAME_POS_R]->SetVisible(false);

    // 最初は表示を消しておく
    this->initFrameContent(FRAME_CONTENT_FRONT);
    this->initFrameContent(FRAME_CONTENT_BACK);
  }

  // メッセージ
  {
    m_wordSet = GFL_NEW(m_appHeap->GetDeviceHeap()) print::WordSet( print::WordSet::DEFAULT_WORD_MAX, print::WordSet::DEFAULT_BUF_LEN, m_appHeap->GetDeviceHeap() );

    gfl2::str::StrBuf* strBuf0 = pG2DUtil->GetTempStrBuf(0);
    gfl2::str::StrBuf* strBufSlash = static_cast<gfl2::str::StrBuf*>( m_dataManager->GetDataBuf(InfoWindowDataManager::DATA_ID_STRBUF_SLASH) );
    m_wordSet->Expand(strBuf0, strBufSlash);
    this->SetTextboxPaneMessage(m_frameContentProperty[FRAME_CONTENT_FRONT].slashTextPane, strBuf0);  // 親であるapp::ui::UIViewのメンバ関数
    this->SetTextboxPaneMessage(m_frameContentProperty[FRAME_CONTENT_BACK ].slashTextPane, strBuf0);  // 親であるapp::ui::UIViewのメンバ関数
  }

  // ボタンマネージャ
  {
    struct MyButtonInfo
    {
      u32                                    button_id;
      u32                                    parts_pane_index;
      u32                                    main_pane_index;
      u32                                    bound_pane_index;
      u32                                    touch_anime_index;
      u32                                    release_anime_index;
      u32                                    cancel_anime_index;
      u32                                    key_select_anime_index;
      u32                                    active_anime_index;
      u32                                    passive_anime_index;
      u32                                    bind_key;
      app::tool::ButtonManager::BindKeyMode  bind_key_mode;
      //u32                                    se_id;
    };
    static const MyButtonInfo myButtonInfo[BUTTON_ID_NUM] =
    {
      {
        BUTTON_ID_B,
        PANENAME_INFORMATION_LRG_LOW_000_PANE_BACKBUTTON,
        PANENAME_COMMON_BTN_LOW_000_PANE_MAIN,
        PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__BACKBUTTON_TOUCH,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__BACKBUTTON_RELEASE,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__BACKBUTTON_CANSEL,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__BACKBUTTON_TOUCH_RELEASE,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__BACKBUTTON_ACTIVE,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__BACKBUTTON_PASSIVE,
        gfl2::ui::BUTTON_B,
        app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER//,
        //SEQ_SE_CANCEL1
      }, 
      {
        BUTTON_ID_A,
        PANENAME_INFORMATION_LRG_LOW_000_PANE_BUTTON_A,
        PANENAME_COMMON_BTN_LOW_017_PANE_MAIN,
        PANENAME_COMMON_BTN_LOW_017_PANE_BOUND_00,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__BUTTON_A_TOUCH,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__BUTTON_A_RELEASE,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__BUTTON_A_CANSEL,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__BUTTON_A_TOUCH_RELEASE,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__BUTTON_A_ACTIVE,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__BUTTON_A_PASSIVE,
        gfl2::ui::BUTTON_A,
        app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER//,
        //SEQ_SE_DECIDE1
      },
      {
        BUTTON_ID_L,
        PANENAME_INFORMATION_LRG_LOW_000_PANE_CURSOR_L,
        PANENAME_COMMON_CUR_LOW_004_PANE_ALL,
        PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__CURSOR_L_TOUCH,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__CURSOR_L_RELEASE,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__CURSOR_L_CANSEL,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__CURSOR_L_TOUCH_RELEASE,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__CURSOR_L_ACTIVE,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__CURSOR_L_PASSIVE,      
        gfl2::ui::BUTTON_L | gfl2::ui::BUTTON_LEFT,
        app::tool::ButtonManager::BIND_KEY_MODE_REPEAT//,
        //SEQ_SE_PAGE
      },
      {
        BUTTON_ID_R,
        PANENAME_INFORMATION_LRG_LOW_000_PANE_CURSOR_R,
        PANENAME_COMMON_CUR_LOW_004_PANE_ALL,
        PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__CURSOR_R_TOUCH,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__CURSOR_R_RELEASE,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__CURSOR_R_CANSEL,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__CURSOR_R_TOUCH_RELEASE,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__CURSOR_R_ACTIVE,
        LA_INFORMATION_LRG_INFORMATION_LRG_LOW_000__CURSOR_R_PASSIVE,
        gfl2::ui::BUTTON_R | gfl2::ui::BUTTON_RIGHT,
        app::tool::ButtonManager::BIND_KEY_MODE_REPEAT//,
        //SEQ_SE_PAGE
      },
    };

    app::ui::ButtonInfoEx* buttonInfoEx = GFL_NEW_ARRAY(m_appHeap->GetDeviceHeap()->GetLowerHandle()) app::ui::ButtonInfoEx[BUTTON_ID_NUM];

    for(u32 i=0; i<BUTTON_ID_NUM; ++i)
    {
      gfl2::lyt::LytParts* buttonPartsPane = lytWk_0->GetPartsPane(myButtonInfo[i].parts_pane_index);
      //gfl2::lyt::LytPane*  buttonMainPane  = lytWk_0->GetPane(buttonPartsPane, myButtonInfo[i].main_pane_index , &lytRes_0);
      gfl2::lyt::LytPane*  buttonBoundPane = lytWk_0->GetPane(buttonPartsPane, myButtonInfo[i].bound_pane_index, &lytRes_0);
      
      buttonInfoEx[i].button_id              = myButtonInfo[i].button_id              ;
      //buttonInfoEx[i].picture_pane           = buttonMainPane                         ;
      buttonInfoEx[i].picture_pane           = buttonPartsPane                        ;
      buttonInfoEx[i].bound_pane             = buttonBoundPane                        ;
      buttonInfoEx[i].touch_anime_index      = myButtonInfo[i].touch_anime_index      ;
      buttonInfoEx[i].release_anime_index    = myButtonInfo[i].release_anime_index    ;
      buttonInfoEx[i].cancel_anime_index     = myButtonInfo[i].cancel_anime_index     ;
      buttonInfoEx[i].key_select_anime_index = myButtonInfo[i].key_select_anime_index ;
      buttonInfoEx[i].active_anime_index     = myButtonInfo[i].active_anime_index     ;
      buttonInfoEx[i].passive_anime_index    = myButtonInfo[i].passive_anime_index    ;
    }

    this->CreateButtonManager(m_appHeap, lytWk_0, buttonInfoEx, BUTTON_ID_NUM);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数
    
    app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数
    for(u32 i=0; i<BUTTON_ID_NUM; ++i)
    {
      buttonManager->SetButtonBindKey(myButtonInfo[i].button_id, myButtonInfo[i].bind_key, myButtonInfo[i].bind_key_mode);
          // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバSetStickEmulationModeは
          // デフォルトでtrueなので、十字キーとスティックは同じ動作をする。
      //buttonManager->SetButtonSelectSE(myButtonInfo[i].button_id, myButtonInfo[i].se_id);
    }

    GFL_DELETE_ARRAY(buttonInfoEx);
    
    buttonManager->SetButtonPassive(BUTTON_ID_A);  // 生成直後は必ずパッシブにしておく
  }

  // コンテンツのバッファ
  {
    m_pageNumMax = m_argPageNumMax;
    if(m_pageNumMax > 0)
    {
      m_contents = GFL_NEW_ARRAY(m_appHeap->GetDeviceHeap()) Content[m_pageNumMax];
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。
    }
    m_argPageNumMax = 0;
  }

  // LocalListener
  {
    m_localListener = GFL_NEW(m_appHeap->GetDeviceHeap()) InfoWindowBookTypeLocalListener(this);
    this->SetInputListener(m_localListener);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数

    m_localListener->SetInputState(InfoWindowBookTypeLocalListener::INPUT_STATE_RELEASE);
  }

  // SE
  this->setButtonManagerSEOn();
}

void InfoWindowBookType::destroyLayout(void)
{
  // コンテンツのバッファ
  {
    GFL_SAFE_DELETE_ARRAY(m_contents);
    m_pageNumMax = 0;
  }

  // ボタンマネージャ
  {
    this->ReleaseButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数
  }

  // メッセージ
  {
    GFL_SAFE_DELETE(m_wordSet);
  }

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

void InfoWindowBookType::setupContent(bool noneIsOk, FramePos framePos)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
  if(pG2DUtil)
  {
    // フレームとページを決定する
    s32 frameContent = FRAME_CONTENT_FRONT;
    u32 pageNo = m_pageNo;
    if(framePos != FRAME_POS_C)
    {
      frameContent = FRAME_CONTENT_BACK;
      this->connectFrameContent(framePos, FRAME_CONTENT_BACK);
      if(framePos == FRAME_POS_L)
      {
        if(this->canGoToPrevPage())
        {
          pageNo = m_pageNo -1;
        }
        else
        {
          GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。ページ外。
        }
      }
      else if(framePos == FRAME_POS_R)
      {
        if(this->canGoToNextPage())
        {
          pageNo = m_pageNo +1;
        }
        else
        {
          GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。ページ外。
        }
      }
    }

    // フレームにコンテンツを当てはめていく
    bool isNone = false;
    
    u32 animNo = m_frameContentProperty[frameContent].animNoText;
    switch(m_contents[pageNo].contentType)
    {
    case Content::CONTENT_TYPE_TEXT:
      {
        animNo = m_frameContentProperty[frameContent].animNoText;
        this->SetTextboxPaneMessage(m_frameContentProperty[frameContent].textPane, m_contents[pageNo].contentUnion.textContent.textStrBuf);  // 親であるapp::ui::UIViewのメンバ関数
        break;
      }
    case Content::CONTENT_TYPE_TEXTURE_AND_TEXT:
      {
        animNo = m_frameContentProperty[frameContent].animNoTextureAndText;
        if(m_contents[pageNo].contentUnion.textureAndTextContent.textureInfo)
        {
          m_frameContentProperty[frameContent].texturePane->SetVisible(true);
          app::tool::LytTexReplaceSystem::ReplaceTextureStatic(
              m_contents[pageNo].contentUnion.textureAndTextContent.textureInfo,
              m_frameContentProperty[frameContent].texturePane,
              0,
              0
          );
          nw::lyt::TexSize texSize = m_contents[pageNo].contentUnion.textureAndTextContent.textureInfo->GetSize();
          nw::lyt::Size paneSize(texSize.width, texSize.height);
          m_frameContentProperty[frameContent].texturePane->SetSize(paneSize);
        }
        else
        {
          m_frameContentProperty[frameContent].texturePane->SetVisible(false);
        }
        this->SetTextboxPaneMessage(m_frameContentProperty[frameContent].textPane, m_contents[pageNo].contentUnion.textureAndTextContent.textStrBuf);  // 親であるapp::ui::UIViewのメンバ関数
        break;
      }
    case Content::CONTENT_TYPE_QR:
      {
        animNo = m_frameContentProperty[frameContent].animNoQR;
        if(m_contents[pageNo].contentUnion.qrContent.qrTextureInfo)
        {
          m_frameContentProperty[frameContent].qrTexturePane->SetVisible(true);
          app::tool::LytTexReplaceSystem::ReplaceTextureStatic(
              m_contents[pageNo].contentUnion.qrContent.qrTextureInfo,
              m_frameContentProperty[frameContent].qrTexturePane,
              0,
              0
          );
          nw::lyt::TexSize texSize = m_contents[pageNo].contentUnion.qrContent.qrTextureInfo->GetSize();
          nw::lyt::Size paneSize(texSize.width, texSize.height);
          m_frameContentProperty[frameContent].qrTexturePane->SetSize(paneSize);
        }
        else
        {
          m_frameContentProperty[frameContent].qrTexturePane->SetVisible(false);
        }
        break;
      }
    default:
      {
        if(noneIsOk == false)
        {
          GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
        }
        isNone = true;
        break;
      }
    }

    if(isNone == false)
    {
      m_frameContentProperty[frameContent].contentVisiblePane->SetVisible(true);
      pG2DUtil->StartAnime(
          0,  // wk_0
          animNo
      );
    }
    else
    {
      m_frameContentProperty[frameContent].contentVisiblePane->SetVisible(false);
    }

    // aa/bbのaa
    this->setNumberToPageTextPane(m_frameContentProperty[frameContent].pageNoTextPane, pageNo+1);  // pageNoは0スタート。+1して表示する。

    // 直前に表示していたコンテンツをクリアするなら
    if(!m_keepFlag)
    {
      this->resetContentsExceptCurrPage();
    }
  }
}

void InfoWindowBookType::setNumberToPageTextPane(gfl2::lyt::LytTextBox* textPane, u32 number)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
  if(pG2DUtil)
  {
    gfl2::str::StrBuf* strBuf0      = pG2DUtil->GetTempStrBuf(0);
    gfl2::str::StrBuf* strBufNumber = static_cast<gfl2::str::StrBuf*>( m_dataManager->GetDataBuf(InfoWindowDataManager::DATA_ID_STRBUF_NUMBER) );
    m_wordSet->RegisterNumber(0, static_cast<s32>(number), 2, print::NUM_DISP_SPACE, print::NUM_CODE_DEFAULT);
    m_wordSet->Expand(strBuf0, strBufNumber);
    this->SetTextboxPaneMessage(textPane, strBuf0);  // 親であるapp::ui::UIViewのメンバ関数
  }
}

void InfoWindowBookType::resetContentsExceptCurrPage(void)
{
  for(u32 i=0; i<m_pageNum; ++i)
  {
    if(i != m_pageNo)
    {
      m_contents[i] = Content();
    }
  }
}


void InfoWindowBookType::setupCommonObj(void)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
  if(pG2DUtil)
  {
    // 共通オブジェクトにフレーム外コンテンツを当てはめていく
    bool isCommandButtonOn = false;
    
    switch(m_contents[m_pageNo].contentType)
    {
    case Content::CONTENT_TYPE_TEXT:
      {
        isCommandButtonOn = this->setStringToCommandTextPane(m_contents[m_pageNo].contentUnion.textContent.commandStrBuf);
        break;
      }
    case Content::CONTENT_TYPE_TEXTURE_AND_TEXT:
      {
        isCommandButtonOn = this->setStringToCommandTextPane(m_contents[m_pageNo].contentUnion.textureAndTextContent.commandStrBuf);
        break;
      }
    case Content::CONTENT_TYPE_QR:
      {
        isCommandButtonOn = this->setStringToCommandTextPane(m_contents[m_pageNo].contentUnion.qrContent.commandStrBuf);
        break;
      }
    }

    // ボタンの状態
    this->setButtonInputEnableAndVisible( BUTTON_ID_A, isCommandButtonOn );
    this->setButtonInputEnableAndVisible( BUTTON_ID_L, this->canGoToPrevPage() );
    this->setButtonInputEnableAndVisible( BUTTON_ID_R, this->canGoToNextPage() );

    {
      app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数
      if(buttonManager)
      {
        if( !buttonManager->IsButtonVisible(BUTTON_ID_A) )
        {
          buttonManager->SetButtonPassive(BUTTON_ID_A);  // 非表示にするときは必ずパッシブにしておく
        }
      }
    }

    // ボタンの状態が変わったので、フラグ反映をしておく
    this->updateCommandButtonActive(); 
  }
}

bool InfoWindowBookType::setStringToCommandTextPane(const gfl2::str::StrBuf* commandStrBuf)
{
  if(commandStrBuf != NULL)
  {
    this->SetTextboxPaneMessage(m_commandPane, commandStrBuf);  // 親であるapp::ui::UIViewのメンバ関数
    return true;
  }
  else
  {
    return false;
  }
}

void InfoWindowBookType::setButtonInputEnableAndVisible(u32 buttonId, bool enable)
{
  app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数
 
  if(enable)
  {
    buttonManager->SetButtonInputEnable(buttonId, true);
    buttonManager->SetButtonVisible(buttonId);
  }
  else
  {
    buttonManager->SetButtonInputEnable(buttonId, false);
    buttonManager->SetButtonInvisible(buttonId);
  }
}

void InfoWindowBookType::connectFrameContent(FramePos framePos, FrameContent frameContent)
{
  gfl2::lyt::LytPane* parentFramePosPane = m_frameContentProperty[frameContent].frameTopPane->GetParent();
  parentFramePosPane->RemoveChild(m_frameContentProperty[frameContent].frameTopPane);
  m_framePosPane[framePos]->AppendChild(m_frameContentProperty[frameContent].frameTopPane);
}

void InfoWindowBookType::swapFrameContent(void)
{
  FrameContentProperty temp;

  temp.frameTopPane         = m_frameContentProperty[FRAME_CONTENT_FRONT].frameTopPane        ;
  temp.contentTopPane       = m_frameContentProperty[FRAME_CONTENT_FRONT].contentTopPane      ;
  temp.contentVisiblePane   = m_frameContentProperty[FRAME_CONTENT_FRONT].contentVisiblePane  ;
  temp.pageNoTextPane       = m_frameContentProperty[FRAME_CONTENT_FRONT].pageNoTextPane      ;   
  temp.pageNumTextPane      = m_frameContentProperty[FRAME_CONTENT_FRONT].pageNumTextPane     ;  
  temp.slashTextPane        = m_frameContentProperty[FRAME_CONTENT_FRONT].slashTextPane       ;    
  temp.textPane             = m_frameContentProperty[FRAME_CONTENT_FRONT].textPane            ;
  temp.texturePane          = m_frameContentProperty[FRAME_CONTENT_FRONT].texturePane         ;
  temp.qrTexturePane        = m_frameContentProperty[FRAME_CONTENT_FRONT].qrTexturePane       ;
  temp.animNoText           = m_frameContentProperty[FRAME_CONTENT_FRONT].animNoText          ;
  temp.animNoTextureAndText = m_frameContentProperty[FRAME_CONTENT_FRONT].animNoTextureAndText;
  temp.animNoQR             = m_frameContentProperty[FRAME_CONTENT_FRONT].animNoQR            ;            

  m_frameContentProperty[FRAME_CONTENT_FRONT].frameTopPane         = m_frameContentProperty[FRAME_CONTENT_BACK].frameTopPane        ;
  m_frameContentProperty[FRAME_CONTENT_FRONT].contentTopPane       = m_frameContentProperty[FRAME_CONTENT_BACK].contentTopPane      ;
  m_frameContentProperty[FRAME_CONTENT_FRONT].contentVisiblePane   = m_frameContentProperty[FRAME_CONTENT_BACK].contentVisiblePane  ;
  m_frameContentProperty[FRAME_CONTENT_FRONT].pageNoTextPane       = m_frameContentProperty[FRAME_CONTENT_BACK].pageNoTextPane      ;   
  m_frameContentProperty[FRAME_CONTENT_FRONT].pageNumTextPane      = m_frameContentProperty[FRAME_CONTENT_BACK].pageNumTextPane     ;  
  m_frameContentProperty[FRAME_CONTENT_FRONT].slashTextPane        = m_frameContentProperty[FRAME_CONTENT_BACK].slashTextPane       ;    
  m_frameContentProperty[FRAME_CONTENT_FRONT].textPane             = m_frameContentProperty[FRAME_CONTENT_BACK].textPane            ;
  m_frameContentProperty[FRAME_CONTENT_FRONT].texturePane          = m_frameContentProperty[FRAME_CONTENT_BACK].texturePane         ;
  m_frameContentProperty[FRAME_CONTENT_FRONT].qrTexturePane        = m_frameContentProperty[FRAME_CONTENT_BACK].qrTexturePane       ;
  m_frameContentProperty[FRAME_CONTENT_FRONT].animNoText           = m_frameContentProperty[FRAME_CONTENT_BACK].animNoText          ;
  m_frameContentProperty[FRAME_CONTENT_FRONT].animNoTextureAndText = m_frameContentProperty[FRAME_CONTENT_BACK].animNoTextureAndText;
  m_frameContentProperty[FRAME_CONTENT_FRONT].animNoQR             = m_frameContentProperty[FRAME_CONTENT_BACK].animNoQR            ;            

  m_frameContentProperty[FRAME_CONTENT_BACK].frameTopPane         = temp.frameTopPane        ;
  m_frameContentProperty[FRAME_CONTENT_BACK].contentTopPane       = temp.contentTopPane      ;
  m_frameContentProperty[FRAME_CONTENT_BACK].contentVisiblePane   = temp.contentVisiblePane  ;
  m_frameContentProperty[FRAME_CONTENT_BACK].pageNoTextPane       = temp.pageNoTextPane      ;   
  m_frameContentProperty[FRAME_CONTENT_BACK].pageNumTextPane      = temp.pageNumTextPane     ;  
  m_frameContentProperty[FRAME_CONTENT_BACK].slashTextPane        = temp.slashTextPane       ;    
  m_frameContentProperty[FRAME_CONTENT_BACK].textPane             = temp.textPane            ;
  m_frameContentProperty[FRAME_CONTENT_BACK].texturePane          = temp.texturePane         ;
  m_frameContentProperty[FRAME_CONTENT_BACK].qrTexturePane        = temp.qrTexturePane       ;
  m_frameContentProperty[FRAME_CONTENT_BACK].animNoText           = temp.animNoText          ;
  m_frameContentProperty[FRAME_CONTENT_BACK].animNoTextureAndText = temp.animNoTextureAndText;
  m_frameContentProperty[FRAME_CONTENT_BACK].animNoQR             = temp.animNoQR            ;            

  this->connectFrameContent(FRAME_POS_C, FRAME_CONTENT_FRONT);
  this->connectFrameContent(FRAME_POS_R, FRAME_CONTENT_BACK);  // FRAME_CONTENT_BACKは次のページの配置場所にくっつけておくことにする

  this->initFrameContent(FRAME_CONTENT_BACK);  // FRAME_CONTENT_BACKの表示は消しておく
}

void InfoWindowBookType::initFrameContent(FrameContent frameContent)
{
  // @fix GFNMCat[1342] 「レギュレーションDL」＞「くわしくみる」にてページ切り替えをすると元のページの内容が一瞬表示される
  // FRONTとBACKの入れ替えだけでは、初表示のときに値を設定する前の状態が一瞬見えてしまうので、非表示で初期化する

  // アニメで制御している表示/非表示を、プログラムで全て非表示で初期化しておく。
  m_frameContentProperty[frameContent].textPane->SetVisible(false);
  m_frameContentProperty[frameContent].texturePane->SetVisible(false);
  m_frameContentProperty[frameContent].qrTexturePane->SetVisible(false);
}

void InfoWindowBookType::changeDispState(DispState dispState)
{
  m_dispState = dispState;
  m_dispStateCount = 0;
}

void InfoWindowBookType::updateCloseReq(void)
{
  this->changeDispState(DISP_STATE_WAIT_ONE_FRAME_AFTER);
  m_closeReq = false;
  m_openReq = false;  // オープンリクエストが出ているときにクローズしても問題ないようにしておく。
}
void InfoWindowBookType::updateDispStateWaitOneFrameAfter(void)
{
  this->changeDispState(DISP_STATE_OFF);
  // これ以降、他の画面に入力を解放する。
  this->SetModal(false);  // 親であるapp::ui::UIViewのメンバ関数
  m_localListener->SetInputState(InfoWindowBookTypeLocalListener::INPUT_STATE_RELEASE);
  if(m_listener)
  {
    m_listener->OnClose();
  }

  {
    app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
    app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数
    if(pG2DUtil && buttonManager)
    {
      buttonManager->SetButtonPassive(BUTTON_ID_A);  // クローズしたら必ずパッシブにしておく
    }
  }

  this->initLayoutAfterClose();
}

void InfoWindowBookType::initLayoutAfterClose(void)
{
  // @fix NMCat[2115] ページ切り替え中にボタン入力が可能なタイミングがある  →ページが落ち着く前に強制終了されることもある。
  //      そのような途中終了した後に、生成し直すことなく使い回してもうまくいくように、レイアウトの変更箇所を戻しておく。

  // ペインの位置の初期化
  m_frameRootPane->SetTranslate(gfl2::math::ConvertGfVec3ToNwVec3(m_frameRootFixPos));

  // ペインの親子階層の初期化
  this->connectFrameContent(FRAME_POS_C, FRAME_CONTENT_FRONT);
  this->connectFrameContent(FRAME_POS_R, FRAME_CONTENT_BACK);  // FRAME_CONTENT_BACKは次のページの配置場所にくっつけておくことにする

  // ペインの表示内容の初期化(消しておく)
  this->initFrameContent(FRAME_CONTENT_FRONT);
  this->initFrameContent(FRAME_CONTENT_BACK);
}

bool InfoWindowBookType::areAllPageContentsSet(void) const
{
  bool ret = true;
  if(m_pageNum > 0)
  {
    for(u32 i=0; i<m_pageNum; ++i)
    {
      if(m_contents[i].contentType == Content::CONTENT_TYPE_NONE)
      {
        ret = false;
        break;
      }
    }
  }
  else
  {
    ret = false;  // ページ数が0のときはfalseを返す
  }
  return ret;
}

bool InfoWindowBookType::canGoToPrevPage(void) const
{
  return (m_pageNo > 0);
}
bool InfoWindowBookType::canGoToNextPage(void) const
{
  return (m_pageNum > 0 && m_pageNo < m_pageNum-1);
}

void InfoWindowBookType::setTouchTrigger(u16 x, u16 y)
{
  m_touchPosNum = 0;
  m_touchTriggerPos.x=x;  m_touchTriggerPos.y=y;
  m_touchPos[0].x    =x;  m_touchPos[0].y    =y;
  m_touchPosNum = 1;
}
InfoWindowBookType::TouchMoveDir InfoWindowBookType::calcTouchMoveDir(void) const
{
  TouchMoveDir touchMoveDir = TOUCH_MOVE_DIR_STOP;
  
  if(m_touchPosNum >= 3)
  {
    s32 totalDiff = 0;
    s32 dir = 0;

    s32 startX = m_touchPos[1].x;
    s32 endX   = m_touchPos[0].x;
    s32 diff = endX - startX;
    dir = diff;
    totalDiff += diff;

    startX = m_touchPos[2].x;
    endX   = m_touchPos[1].x;
    diff   = endX - startX;
    if(
         (dir * diff > 0)         // 同じ方向ならOK
      || (dir != 0 && diff == 0)  // 同じ方向でない場合、停止からの移動ならOK
    )    
    {
      totalDiff += diff;

      if(totalDiff < -15)
      {
        touchMoveDir = TOUCH_MOVE_DIR_LEFT;
      }
      else if(totalDiff > 15)
      {
        touchMoveDir = TOUCH_MOVE_DIR_RIGHT;
      }
    }
  }

  return touchMoveDir;
}  
/*
うまく判定できなかったのでボツ。というか、これstartXとendXが逆になってない？
{
  TouchMoveDir touchMoveDir = TOUCH_MOVE_DIR_STOP;
  
  const s32 START_INDEX = 2;
  const s32 END_INDEX   = 7;
  const s32 MIN_COUNT   = 2;
  if(m_touchPosNum > END_INDEX)
  {
    // 手を放した瞬間のブレで逆方向の入力があるかもしれないので、最新の座標は使わない。
    s32 count = 0;  // 総移動量にカウントした数
    s32 totalDiff = 0;  // 総移動量
    s32 dir = 0;  // 方向
    for(s32 i=START_INDEX; i<END_INDEX; ++i)
    {
      s32 startX = m_touchPos[i].x;
      s32 endX   = m_touchPos[i+1].x;
      s32 diff = endX - startX;
      if(i == START_INDEX)  // 最初の移動量で方向を決める
      {
        dir = diff;
        totalDiff += diff;
        ++count;
      }
      else  // 次からは、方向が同じか否かで、加えるか、加えず打ち切るか、を決める。
      {
        if(dir*diff > 0)  // 同じ方向に進んでいるなら、加える。
        {
          totalDiff += diff;
          ++count;
        }
        else  // 移動していない、もしくは、逆方向に進んでいるなら、加えず打ち切る。
        {
          break;
        }
      }
    }

    if(count >= MIN_COUNT)
    {
      // 最新の座標も同じ方向なら、ブレは発生していないので加えていい。
      for(s32 i=START_INDEX; i>0; --i)
      {
        s32 startX = m_touchPos[i-1].x;
        s32 endX   = m_touchPos[i].x;
        s32 diff = endX - startX;
        if(dir*diff > 0)  // 同じ方向に進んでいるなら、加える。
        {
          totalDiff += diff;
          ++count;
        }
        else  // 移動していない、もしくは、逆方向に進んでいるなら、加えず打ち切る。
        {
          break;
        }
      }
    }

    if(count >= MIN_COUNT)
    {
      f32 aveDiff = static_cast<f32>(totalDiff) / count;
      if(aveDiff < -3.0f)
      {
        touchMoveDir = TOUCH_MOVE_DIR_LEFT;
      }
      else if(aveDiff > 3.0f)
      {
        touchMoveDir = TOUCH_MOVE_DIR_RIGHT;
      }
    }
  }
  return touchMoveDir;
}
*/
InfoWindowBookType::FramePos InfoWindowBookType::calcFrameRootPanePosSection(void) const  // m_frameRootPaneが画面の左が中央か右のどのあたりにいるか
{
  FramePos posSection = FRAME_POS_C;
  gfl2::math::Vector3 pos;
  pos = gfl2::math::ConvertNwVec3ToGfVec3( m_frameRootPane->GetTranslate() );
  if(pos.x < -50.0f)
  {
    posSection = FRAME_POS_L;
  }
  else if(pos.x > 50.0f)
  {
    posSection = FRAME_POS_R;
  }
  return posSection;
}

void InfoWindowBookType::setButtonManagerSEOn(void)
{
  static const u32 SE_ID[BUTTON_ID_NUM] =
  {
    SEQ_SE_CANCEL1,
    SEQ_SE_DECIDE1,
    SEQ_SE_PAGE,
    SEQ_SE_PAGE,
  };

  app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数
  if(buttonManager)
  {
    if(m_SEIsOn)
    {
      for(u32 i=0; i<BUTTON_ID_NUM; ++i)
      {
        // iとbutton_idは同じ値
        buttonManager->SetButtonSelectSE(i, SE_ID[i]);
      }
    }
    else
    {
      for(u32 i=0; i<BUTTON_ID_NUM; ++i)
      {
        // iとbutton_idは同じ値
        buttonManager->SetButtonSelectSE(i, app::tool::ButtonManager::SE_NULL);
      }
    }
  }
}

void InfoWindowBookType::updateCommandButtonActive(void)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
  app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数
  if(pG2DUtil && buttonManager)
  {
    // 「表示しているとき＝入力可能なとき」だけアクティブ/パッシブを設定する
    if( buttonManager->IsButtonVisible(BUTTON_ID_A) )
    {
      if(m_commandButtonIsActive)
      {
        buttonManager->SetButtonActive(BUTTON_ID_A);
      }
      else
      {
        buttonManager->SetButtonPassive(BUTTON_ID_A);
      }
    }
  }
}


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

