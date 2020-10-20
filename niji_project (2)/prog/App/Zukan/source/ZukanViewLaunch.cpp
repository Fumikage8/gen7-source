//==============================================================================
/**
 @file    ZukanViewLaunch.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の起動画面のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <str/include/gfl2_StrBuf.h>
#include <Layout/include/gfl2_Layout.h>
#include <Fade/include/gfl2_FadeManager.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Tool/app_tool_MenuCursor.h>
#include <AppLib/include/Tool/app_tool_ButtonManager.h>
#include <AppLib/include/Tool/AppToolRotomFriendship.h>

#include <Sound/include/Sound.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/launch.h>
#include <niji_conv_header/app/zukan/launch_pane.h>
#include <niji_conv_header/app/zukan/launch_anim_list.h>

#include <niji_conv_header/app/common/graphic_font/zukan_grpfont/res2d/ZukanGrpFont.h>

#include <niji_conv_header/app/rotom_fade/rotom_fade.h>
#include <niji_conv_header/app/rotom_fade/rotom_fade_pane.h>
#include <niji_conv_header/app/rotom_fade/rotom_fade_anim_list.h>

#include <niji_conv_header/message/msg_zukan.h>
#include <niji_conv_header/message/msg_zukan_hyouka.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanDefine.h"
#include "ZukanType.h"
#include "ZukanDataManager.h"
#include "ZukanTool.h"
#include "ZukanUtil.h"
#include "ZukanContents.h"
#include "ZukanFrameLaunch.h"

#include "./ZukanViewLaunch.h"




#define APP_ZUKAN_GRPFONT_TEST_VER_NONE    (0)    // グラフィックフォント差し替えを行なわないので、地のレイアウトにグラフィックフォントを含んでおくこと
#define APP_ZUKAN_GRPFONT_TEST_VER_AUTO    (1)    // グラフィックフォントを地のレイアウトに含んではならない
#define APP_ZUKAN_GRPFONT_TEST_VER_MANUAL  (2)    // グラフィックフォントを地のレイアウトに含んでもいい(含まないほうが無駄はない)

#define APP_ZUKAN_GRPFONT_TEST_VER (APP_ZUKAN_GRPFONT_TEST_VER_AUTO)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewLaunchListener
 @brief     図鑑の起動画面のビューのリスナー
 */
//==============================================================================
class ZukanViewLaunchListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewLaunchListener);

public:
  ZukanViewLaunchListener(ZukanViewLaunch* view);
  virtual ~ZukanViewLaunchListener();

  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  virtual void OnLayoutPaneTouchTriggerEvent( const u32 paneId );

public:
  enum InputState
  {
    INPUT_STATE_RELEASE,  //!< 他のviewに解放中
    INPUT_STATE_OCCUPY,   //!< このviewが占有しているが、入力は受け付けていない
    INPUT_STATE_ENABLE,   //!< このviewが占有していて、入力を受け付けている
  };
  void SetInputState(InputState inputState);

public:
  ZukanViewLaunch* m_view;
  InputState       m_inputState;
};


//==============================================================================
//
//  ZukanViewLaunchListener
//
//==============================================================================
ZukanViewLaunchListener::ZukanViewLaunchListener(ZukanViewLaunch* view)
  : app::ui::UIInputListener(),
    m_view(view),
    m_inputState(INPUT_STATE_RELEASE)
{}
ZukanViewLaunchListener::~ZukanViewLaunchListener()
{}

app::ui::UIInputListener::ListenerResult ZukanViewLaunchListener::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
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
      if( m_view->GetButtonManager()->GetUpdateState() == app::tool::ButtonManager::UPDATE_STATE_IDLE )  // ボタンマネージャ動作中はキー入力させない
      {
        if( pButton->IsTrigger(gfl2::ui::BUTTON_A) )
        {
          m_view->Req(ZukanViewLaunch::REQ_ID_KEY_DECIDE);
        }
        else if( pButton->IsTrigger(gfl2::ui::BUTTON_UP) )
        {
          m_view->Req(ZukanViewLaunch::REQ_ID_KEY_TRIGGER_UP);
        }
        else if( pButton->IsTrigger(gfl2::ui::BUTTON_DOWN) )
        {
          m_view->Req(ZukanViewLaunch::REQ_ID_KEY_TRIGGER_DOWN);
        }
        else if( pButton->IsRepeat(gfl2::ui::BUTTON_UP) )
        {
          m_view->Req(ZukanViewLaunch::REQ_ID_KEY_REPEAT_UP);
        }
        else if( pButton->IsRepeat(gfl2::ui::BUTTON_DOWN) )
        {
          m_view->Req(ZukanViewLaunch::REQ_ID_KEY_REPEAT_DOWN);
        }
      }
      res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  }
  return res;
}
app::ui::UIInputListener::ListenerResult ZukanViewLaunchListener::OnLayoutPaneEvent( const u32 painId )
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
      m_view->Req(ZukanViewLaunch::REQ_ID_BUTTON_DECIDE, button_id);
      res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  }
  return res;
}
void ZukanViewLaunchListener::OnLayoutPaneTouchTriggerEvent( const u32 paneId )
{
  switch(m_inputState)
  {
  case INPUT_STATE_ENABLE:
    {
      u32 button_id = paneId;
      m_view->Req(ZukanViewLaunch::REQ_ID_BUTTON_TOUCH, button_id);
      break;
    }
  }
}

void ZukanViewLaunchListener::SetInputState(InputState inputState)
{
  m_inputState = inputState;
  
  app::tool::ButtonManager* buttonManager = m_view->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  buttonManager->SetInputEnable( m_inputState == INPUT_STATE_ENABLE );
}




//==============================================================================
//
//  ZukanViewLaunch
//
//==============================================================================
ZukanViewLaunch::ZukanViewLaunch(const ZukanSceneParam& param)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
    m_param(param),
    m_modeIsAdmire(false),
    m_viewListener(NULL),
    m_cursorIndex(*m_param.launchSceneCursorIndex),
    m_fadeOutFlag(false),
    m_mesWinPane(NULL),
    m_mesCursorPane(NULL),
    m_strBuf(NULL),
    m_bgmChanged(false),
    m_rotomFadeFlag(false),
    m_rotomVoice(NULL),
    m_nextInfoType(ZukanType::INFO_TYPE_NUM),
    m_dispState(DISP_STATE_OFF),
    m_dispStateCount(0)
{
  // 初期化
  {
    for(u32 i=0; i<LYT_WK_ID_NUM; ++i)
    {
      m_waitAnimIndex[i] = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
    }

    for(u32 i=0; i<BUTTON_ID_NUM; ++i)
    {
      m_buttonPaneInfo[i] = ZukanType::ButtonPaneInfo();
    }

    for(s32 i=0; i<UPPER_LOOP_ANIM_NUM; ++i)
    {
      m_upperLoopAnimIndex[i] = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
    }

    for(u32 i=0; i<2; ++i)
    {
      m_mesWinTextPane[i] = NULL;
    }
  }

  // str
  {
    m_strBuf = GFL_NEW(m_param.appHeap->GetDeviceHeap()) gfl2::str::StrBuf( STR_BUF_CHAR_NUM_MAX, m_param.appHeap->GetDeviceHeap() );
  }

  // ZukanViewLaunchListener
  {
    m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewLaunchListener(this);
    this->SetInputListener(m_viewListener);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  }

  // UIView
  {
    m_param.parentView->AddSubView(this);  // app::ui::UIViewのメンバ
  }

  // レイアウト
  {
    this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ
 
    void* buf          = m_param.dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_LAUNCH);
    void* rotomFadeBuf = m_param.dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_ROTOM_FADE);
    void* grpFontBuf   = m_param.dataManager->GetDataBuf(ZukanDataManager::DATA_ID_ZUKAN_GRPFONT_RES2D);  // グラフィックフォント
    if(buf && rotomFadeBuf && grpFontBuf)
    {
      const app::util::G2DUtil::LytResData resTbl[] =
      {
        // res_0
        {
          buf,
#if APP_ZUKAN_GRPFONT_TEST_VER==APP_ZUKAN_GRPFONT_TEST_VER_AUTO
          LYTRES_ZUKANGRPFONT_END,  // niji_conv_header/app/common/graphic_font/zukan_grpfont/res2d/ZukanGrpFont.h
#else
          0,
#endif
          app::util::G2DUtil::ATTACH_ROOT
        },
#if APP_ZUKAN_GRPFONT_TEST_VER==APP_ZUKAN_GRPFONT_TEST_VER_AUTO
        // res_1
        {
          grpFontBuf,
          0,
          app::util::G2DUtil::ATTACH_RESOURCE
        },
#elif APP_ZUKAN_GRPFONT_TEST_VER==APP_ZUKAN_GRPFONT_TEST_VER_MANUAL
        // res_1
        {
          grpFontBuf,
          0,
          app::util::G2DUtil::ATTACH_ROOT
        },
#endif
        // res_2
        {
          rotomFadeBuf,
          0,
          app::util::G2DUtil::ATTACH_NONE
        },
      };
     
      const app::util::G2DUtil::LytwkData wkTbl[] =
      {
        // wk_0
        {
          // 下画面
          0,  // res_0
          LYTRES_LAUNCH_ZUKAN_MIN_LOW_001_BFLYT,
          LA_LAUNCH_ZUKAN_MIN_LOW_001___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_LOWER,
          true
        },
        // wk_1
        {
          // 上画面
          0,  // res_0
          LYTRES_LAUNCH_ZUKAN_STR_UPP_000_BFLYT,
          LA_LAUNCH_ZUKAN_STR_UPP_000___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
        // wk_2
        {
          // 下画面
          2,  // res_2
          LYTRES_ROTOM_FADE_ROTOM_FADE_LOW_000_BFLYT,
          LA_ROTOM_FADE_ROTOM_FADE_LOW_000___NUM,
          rotomFadeBuf,
          true,
          app::util::G2DUtil::SETUP_LOWER,
          true
        },
        // wk_3
        {
          // 上画面
          2,  // res_2
          LYTRES_ROTOM_FADE_ROTOM_FADE_UPP_000_BFLYT,
          LA_ROTOM_FADE_ROTOM_FADE_UPP_000___NUM,
          rotomFadeBuf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
      };
     
      this->Create2D(  // 親であるapp::ui::UIViewのメンバ
          m_param.appHeap,
          m_param.lytSys,
          4,  // wk_0, wk_1, wk_2, wk_3の4個
          resTbl,
          GFL_NELEMS(resTbl), 
          wkTbl,
          GFL_NELEMS(wkTbl), 
          app::ui::UIView::UNUSE_MESSAGE_DATA
      );
      
      app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ

#if APP_ZUKAN_GRPFONT_TEST_VER==APP_ZUKAN_GRPFONT_TEST_VER_MANUAL
      // グラフィックフォント(手動差し替え)
      {
        gfl2::lyt::LytWk*        lytWk_1     = this->GetLayoutWork( LYT_WK_ID_UPPER );  // wk_1  // 親であるapp::ui::UIViewのメンバ関数
        gfl2::lyt::LytSys*       lytSys      = pG2DUtil->GetLayoutSystem();
        gfl2::lyt::LytMultiResID lytResID_1  = pG2DUtil->GetLayoutResourceID( 1 );  // res_1
        gfl2::lyt::LytRes*       lytRes_1    = lytSys->GetResource( &lytResID_1 );

        const gfl2::lyt::LytResTextureInfo* texInfo = lytRes_1->GetTexture( LYTRES_ZUKANGRPFONT_PICTUREBOOK_TOP_TITLE_J_BFLIM, lytResID_1.GetMultiID() );
        
        gfl2::lyt::LytPicture* picPane = lytWk_1->GetPicturePane(PANENAME_ZUKAN_STR_UPP_000_PANE_LOGO_00);  // 使っているペインその１
        app::tool::LytTexReplaceSystem::ReplaceTextureStatic( texInfo, picPane, 0, 0 );
        
        picPane = lytWk_1->GetPicturePane(PANENAME_ZUKAN_STR_UPP_000_PANE_LOGO_01);  // 使っているペインその２
        app::tool::LytTexReplaceSystem::ReplaceTextureStatic( texInfo, picPane, 0, 0 );
      }
#endif

      // ボタンマネージャ
      this->createButtonManager();

      // メニューカーソルレイアウト生成
      m_param.tool->CreateMenuCursorLayoutWork(pG2DUtil, gfl2::lyt::DISPLAY_LOWER, app::tool::MenuCursor::SIZE_SMALL);

      // ページ内容生成
      this->createPage();
      this->setupButtonOpen();

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

  ZukanUtil::SetPokeModelVisible(m_param.tool, false);
}
ZukanViewLaunch::~ZukanViewLaunch()
{
  // ロトムボイス
  GFL_SAFE_DELETE(m_rotomVoice);
  
  // UIView
  this->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ

  // メニューカーソルレイアウト破棄
  //m_param.tool->DestroyMenuCursorLayoutWork();

  // ボタンマネージャ
  this->destroyButtonManager();

  // レイアウト
  this->Delete2D();  // 親であるapp::ui::UIViewのメンバ

  // ZukanViewLaunchListener
  GFL_SAFE_DELETE(m_viewListener);

  // str
  GFL_SAFE_DELETE(m_strBuf);
}

void ZukanViewLaunch::Update(void)
{
  app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();

  if(m_modeIsAdmire)
  {
    this->updateForAdmire();
  }
  else
  {
    this->updateForLaunch();
  }

  app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ
  menuCursor->Update();
}
void ZukanViewLaunch::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_LOWER, gfl2::lyt::ORDER_NORMAL, 0 );  // wk_0
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_UPPER, gfl2::lyt::ORDER_NORMAL, 0 );  // wk_1
 
  if(m_rotomFadeFlag)
  {
    pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_ROTOM_LOWER, gfl2::lyt::ORDER_NORMAL, 0 );  // wk_2
    pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_ROTOM_UPPER, gfl2::lyt::ORDER_NORMAL, 0 );  // wk_3
  }

  app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();
  menuCursor->Draw( m_param.appRenderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER );
}

void ZukanViewLaunch::Req(ReqId reqId, s32 reqParam)
{
  switch(reqId)
  {
  case REQ_ID_KEY_DECIDE:
    {
      app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
      buttonManager->StartSelectedAct(m_cursorIndex);
      break;
    }
  case REQ_ID_KEY_TRIGGER_UP:
    this->setCursorIndexAndPutMenuCursor(this->getUpCursorIndex());
    break;
  case REQ_ID_KEY_TRIGGER_DOWN:
    this->setCursorIndexAndPutMenuCursor(this->getDownCursorIndex());
    break;
  case REQ_ID_KEY_REPEAT_UP:
    if(!this->isCursorIndexAtTop())
    {
      this->setCursorIndexAndPutMenuCursor(this->getUpCursorIndex());
    }
    break;
  case REQ_ID_KEY_REPEAT_DOWN:
    if(!this->isCursorIndexAtBottom())
    {
      this->setCursorIndexAndPutMenuCursor(this->getDownCursorIndex());
    }
    break;
  case REQ_ID_BUTTON_DECIDE:
    {
      if(m_modeIsAdmire)
      {
        if( BUTTON_ID_LIST_ELEM_ZUKAN_START<=reqParam && reqParam<=BUTTON_ID_LIST_ELEM_ZUKAN_END )
        {
          this->startAdmireResultSeq(reqParam); 
        }
        else if(reqParam == BUTTON_ID_BACK)
        {
          this->startAdmireOutSeq();            
        }
      }
      else
      {
        if( BUTTON_ID_LIST_ELEM_ZUKAN_START<=reqParam && reqParam<=BUTTON_ID_LIST_ELEM_ZUKAN_END )
        {
          ZukanType::InfoType infoType = static_cast<ZukanType::InfoType>( reqParam - BUTTON_ID_LIST_ELEM_ZUKAN_START );
          if( m_param.contents->DoesPokeExist(infoType) )  // 1匹もいないときは、いけなくしておくのでパッシブにもするか。そしたら評価のときのパッシブの使い方と競合するので注意すること！
          {
            if(reqParam == BUTTON_ID_ALOLA_TOP)
            {
              this->setParamWhenOutSeq( infoType, ZukanType::SCENE_TYPE_ALOLA_TOP );
            }
            else
            {
              this->setParamWhenOutSeq( infoType, ZukanType::SCENE_TYPE_ISLAND_TOP );
            }
            this->startOutSeq();
          }
        }

        if(reqParam == BUTTON_ID_ADMIRE)
        {
          this->startAdmireInSeq();
        }
        else if(reqParam == BUTTON_ID_BACK)
        {
          this->setParamWhenOutSeq( ZukanType::INFO_TYPE_NUM, ZukanType::SCENE_TYPE_END );  // InfoTypeを設定しなくてもいいときなのでINFO_TYPE_NUMを渡すことにする。
          this->startOutSeq();
        }
      }
    }
    break;
  case REQ_ID_BUTTON_TOUCH:
    {
      switch(reqParam)
      {
      case BUTTON_ID_ALOLA_TOP:   this->setCursorIndexAndPutMenuCursor(BUTTON_ID_ALOLA_TOP  );  break;
      case BUTTON_ID_ISLAND1_TOP: this->setCursorIndexAndPutMenuCursor(BUTTON_ID_ISLAND1_TOP);  break;
      case BUTTON_ID_ISLAND2_TOP: this->setCursorIndexAndPutMenuCursor(BUTTON_ID_ISLAND2_TOP);  break;
      case BUTTON_ID_ISLAND3_TOP: this->setCursorIndexAndPutMenuCursor(BUTTON_ID_ISLAND3_TOP);  break;
      case BUTTON_ID_ISLAND4_TOP: this->setCursorIndexAndPutMenuCursor(BUTTON_ID_ISLAND4_TOP);  break;
      case BUTTON_ID_ADMIRE:      this->setCursorIndexAndPutMenuCursor(BUTTON_ID_ADMIRE     );  break;
      case BUTTON_ID_BACK:                                                                      break;
      }
    }
    break;
  }
}

void ZukanViewLaunch::setCursorIndexAndPutMenuCursor(s32 cursorIndex, bool playSEFlag, bool notSetIfSameIndexFlag)
{
  bool setFlag = false;
  if( !notSetIfSameIndexFlag )
  {
    setFlag = true;
  }
  else if( notSetIfSameIndexFlag && (m_cursorIndex != cursorIndex) )
  {
    setFlag = true;
  }

  if(setFlag)
  {
    m_cursorIndex = cursorIndex;
 
    // カーソル移動音
    if(playSEFlag) Sound::PlaySE( SEQ_SE_SELECT1 );
 
    // カーソルの表示位置を変更
    app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
    app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();
    //menuCursor->SetPos( m_buttonPaneInfo[m_cursorIndex].partsPane->GetTranslate() + m_buttonPaneInfo[m_cursorIndex].cursorPane->GetTranslate() );
    gfl2::math::VEC3 pos = pG2DUtil->GetPanePos(m_buttonPaneInfo[m_cursorIndex].cursorPane);
    menuCursor->SetPos( &pos );
    menuCursor->SetSize( m_buttonPaneInfo[m_cursorIndex].cursorPane );
  }
}
s32 ZukanViewLaunch::getUpCursorIndex(void)
{
  // 非表示のボタンは飛ばす
  app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  
  //s32 buttonIdListElemEnd = BUTTON_ID_LIST_ELEM_END;
  s32 buttonIdListElemNum = BUTTON_ID_LIST_ELEM_NUM;
  if(m_modeIsAdmire)
  {
    //buttonIdListElemEnd = BUTTON_ID_LIST_ELEM_END_FOR_ADMIRE;
    buttonIdListElemNum = BUTTON_ID_LIST_ELEM_NUM_FOR_ADMIRE;
  }
 
  s32 cursorIndex = m_cursorIndex;
  for(s32 i=0; i<buttonIdListElemNum; ++i)
  {
    cursorIndex = (m_cursorIndex -i-1 + buttonIdListElemNum) % buttonIdListElemNum;

    if( ZukanUtil::IsButtonVisible(buttonManager, cursorIndex) )
    {
      break;
    }
  }
  return cursorIndex;
}
s32 ZukanViewLaunch::getDownCursorIndex(void)
{
  // 非表示のボタンは飛ばす
  app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
 
  //s32 buttonIdListElemEnd = BUTTON_ID_LIST_ELEM_END;
  s32 buttonIdListElemNum = BUTTON_ID_LIST_ELEM_NUM;
  if(m_modeIsAdmire)
  {
    //buttonIdListElemEnd = BUTTON_ID_LIST_ELEM_END_FOR_ADMIRE;
    buttonIdListElemNum = BUTTON_ID_LIST_ELEM_NUM_FOR_ADMIRE;
  }
 
  s32 cursorIndex = m_cursorIndex;
  for(s32 i=0; i<buttonIdListElemNum; ++i)
  {
    cursorIndex = (m_cursorIndex +i+1) % buttonIdListElemNum;

    if( ZukanUtil::IsButtonVisible(buttonManager, cursorIndex) )
    {
      break;
    }
  }
  return cursorIndex;
}
bool ZukanViewLaunch::isCursorIndexAtTop(void)
{
  // ボタンの先頭は非表示にはならない
  return (m_cursorIndex == BUTTON_ID_LIST_ELEM_START);
}
bool ZukanViewLaunch::isCursorIndexAtBottom(void)
{
  // ボタンの後ろは非表示にはなることがある
  app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ

  s32 buttonIdListElemEnd = BUTTON_ID_LIST_ELEM_START;
  s32 buttonIdListElemEndTemp = BUTTON_ID_LIST_ELEM_END;
  if(m_modeIsAdmire)
  {
    buttonIdListElemEndTemp = BUTTON_ID_LIST_ELEM_END_FOR_ADMIRE;
  }
  for(s32 i=buttonIdListElemEndTemp; i>=BUTTON_ID_LIST_ELEM_START; --i)
  {
    if( ZukanUtil::IsButtonVisible(buttonManager, i) )
    {
      buttonIdListElemEnd = i;
      break;
    }
  }

  return (m_cursorIndex == buttonIdListElemEnd);
}

void ZukanViewLaunch::createButtonManager(void)
{
  ZukanUtil::CreateButtonManager(
      m_param.appHeap,
      this,
      0,  // res_0
      LYT_WK_ID_LOWER,  // wk_0
      BUTTON_ID_NUM,
      BUTTON_CONST_INFO,
      NULL,
      m_buttonPaneInfo
  );
}
void ZukanViewLaunch::destroyButtonManager(void)
{
  ZukanUtil::DestroyButtonManager(this);
}

void ZukanViewLaunch::startInSeq(void)
{
  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  
  switch(m_param.prevSceneType)
  {
  case ZukanType::SCENE_TYPE_ALOLA_TOP:
  case ZukanType::SCENE_TYPE_ISLAND_TOP:
    m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_LAUNCH_ZUKAN_MIN_LOW_001_BACK_PLATE;
    m_waitAnimIndex[LYT_WK_ID_UPPER] = LA_LAUNCH_ZUKAN_STR_UPP_000_BACK;

    // 図鑑起動アニメ
    ZukanUtil::StartLytWaitAnim(pG2DUtil, m_waitAnimIndex, LYT_WK_ID_NUM);
    // 図鑑起動アニメの音→なくなりました
    //Sound::PlaySE(なし);

    if(m_param.prevSceneType == ZukanType::SCENE_TYPE_ALOLA_TOP)
    {
      if( ZukanType::IsSubjectPaneListPosInfoReset(m_param.contents->GetPaneListPosInfoOfSearchSubject()) )
      {
        pFadeManager->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_CROSS);
      }
    }

    this->changeDispState(DISP_STATE_ON_FADE_IN);
    break;
  case ZukanType::SCENE_TYPE_START_ZUKAN:
  default:
    m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_LAUNCH_ZUKAN_MIN_LOW_001_IN_PLATE;
    
    if(m_param.contents->DoesRotomAppear())
    {
      // ロトム有のときだけ、ロトムの起動アニメを再生する
      m_waitAnimIndex[LYT_WK_ID_UPPER] = LA_LAUNCH_ZUKAN_STR_UPP_000_IN;
      pFadeManager->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, 1);  // ぱっと出す
    
      // 図鑑起動アニメ
      ZukanUtil::BindLytWaitAnim(pG2DUtil, m_waitAnimIndex, LYT_WK_ID_NUM, 0);  // ロトムの起動アニメと同時に再生するので、ここではまだ再生しない。
      
      m_rotomFadeFlag = true;
      ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_ROTOM_LOWER, LA_ROTOM_FADE_ROTOM_FADE_LOW_000_FADE_00, 0);
      ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_ROTOM_UPPER, LA_ROTOM_FADE_ROTOM_FADE_UPP_000_FADE_00, 0);
      
      this->changeDispState(DISP_STATE_ON_FADE_IN_ROTOM_IN);
    }
    else
    {
      // ロトム無のときは、ウェイトアニメを再生する
      pG2DUtil->StartAnime( LYT_WK_ID_UPPER, LA_LAUNCH_ZUKAN_STR_UPP_000_TITLE_WAIT );
      pFadeManager->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA);
    
      // 図鑑起動アニメ
      ZukanUtil::StartLytWaitAnim(pG2DUtil, m_waitAnimIndex, LYT_WK_ID_NUM);  // ここで再生する
      
      this->changeDispState(DISP_STATE_ON_FADE_IN);
    }
    break;
  }

  // ループアニメを開始する
  for(s32 i=0; i<UPPER_LOOP_ANIM_NUM; ++i)
  {
    pG2DUtil->StartAnime( LYT_WK_ID_UPPER, m_upperLoopAnimIndex[i] );
  }

  // ロトム有無
  if(!m_param.contents->DoesRotomAppear())
  {
    // ロトム無のときだけ、ロトムを消すアニメを再生しておく
    pG2DUtil->StartAnime( LYT_WK_ID_UPPER, LA_LAUNCH_ZUKAN_STR_UPP_000_TOP_ROTOM_OFF );
  }

  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewLaunchListener::INPUT_STATE_OCCUPY);

#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
  m_param.debugInfo->SetCurrTime(ZukanDebugInfo::TIME_PHASE_ZUKAN_LAUNCH_FADE_IN_START);
  m_param.debugInfo->Print();
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
}
void ZukanViewLaunch::startOutSeq(void)
{
  switch(*m_param.nextSceneType)
  {
  case ZukanType::SCENE_TYPE_ALOLA_TOP:
  case ZukanType::SCENE_TYPE_ISLAND_TOP:
    m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_LAUNCH_ZUKAN_MIN_LOW_001_NEXT_PLATE;
    m_waitAnimIndex[LYT_WK_ID_UPPER] = LA_LAUNCH_ZUKAN_STR_UPP_000_NEXT;
    break;
  case ZukanType::SCENE_TYPE_END:
  default:
    m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_LAUNCH_ZUKAN_MIN_LOW_001_RETURN_PLATE;
    m_waitAnimIndex[LYT_WK_ID_UPPER] = LA_LAUNCH_ZUKAN_STR_UPP_000_RETURN;
    m_fadeOutFlag = true;
    break;
  }
 
  app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();
  menuCursor->SetVisible(false);

  // @fix GFNMCat[4468] アローラ図鑑への画面遷移アニメが、遷移フローによって異なります
  // 図鑑評価アニメがバインドされたままだと、アウトアニメがうまく再生されないので、(バインドされているいないに関わらず)図鑑評価アニメをアンバインドする。
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  pG2DUtil->StopAnime(LYT_WK_ID_LOWER, LA_LAUNCH_ZUKAN_MIN_LOW_001_EVALUATION);
  pG2DUtil->StopAnime(LYT_WK_ID_UPPER, LA_LAUNCH_ZUKAN_STR_UPP_000_ROTOM);

  ZukanUtil::StartLytWaitAnim(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM);  // 親であるapp::ui::UIViewのメンバ
  this->changeDispState(DISP_STATE_ON_OUT);
 
  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewLaunchListener::INPUT_STATE_OCCUPY);
}

void ZukanViewLaunch::startAdmireInSeq(void)
{
  this->changeMode(true);
  m_mesWinPane->SetVisible(true);
  m_cursorIndex = 0;
  app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  buttonManager->SetButtonPassive(BUTTON_ID_ADMIRE);       
  
  m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_LAUNCH_ZUKAN_MIN_LOW_001_EVALUATION;
  m_waitAnimIndex[LYT_WK_ID_UPPER] = LA_LAUNCH_ZUKAN_STR_UPP_000_ROTOM;
  
  app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();
  menuCursor->SetVisible(false);
 
  ZukanUtil::StartLytWaitAnim(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM);  // 親であるapp::ui::UIViewのメンバ
  this->changeDispState(DISP_STATE_ADMIRE_ON_IN);
  
  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewLaunchListener::INPUT_STATE_OCCUPY);
}
void ZukanViewLaunch::startAdmireOutSeq(void)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  pG2DUtil->DeleteMsgCursor();
  pG2DUtil->DeleteMessageWindow();  // 開始は「インアニメ→update(ここで開始)」、終了は「アウトアニメ(ここで終了)→update」となっているのが、対応がうまくないので時間があればリファクタリングしたいところ。
  app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  buttonManager->SetButtonActive(BUTTON_ID_ADMIRE);       
  
  pG2DUtil->StopAnime( LYT_WK_ID_UPPER, LA_LAUNCH_ZUKAN_STR_UPP_000_ROTOM_LOOP );

  ZukanUtil::BindLytAnim(this->GetG2DUtil(), LYT_WK_ID_LOWER, LA_LAUNCH_ZUKAN_MIN_LOW_001_EVALUATION, 0);
  ZukanUtil::BindLytAnim(this->GetG2DUtil(), LYT_WK_ID_UPPER, LA_LAUNCH_ZUKAN_STR_UPP_000_ROTOM, 0);
  
  app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();
  menuCursor->SetVisible(false);
 
  ZukanUtil::StartLytWaitAnim(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM);  // 親であるapp::ui::UIViewのメンバ
  this->changeDispState(DISP_STATE_ADMIRE_ON_OUT);
  
  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewLaunchListener::INPUT_STATE_OCCUPY);
        
  m_rotomVoice->Stop();
}

void ZukanViewLaunch::startAdmireResultSeq(s32 buttonId)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ

  app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  for(s32 i=BUTTON_ID_LIST_ELEM_ZUKAN_START; i<=BUTTON_ID_LIST_ELEM_ZUKAN_END; ++i)
  {
    if( ZukanUtil::IsButtonVisible(buttonManager, i) )
    {
      if( i != buttonId ) buttonManager->SetButtonPassive(i);       
    }
    else
    {
      s32 plateIndex = i - BUTTON_ID_LIST_ELEM_ISLAND_START;
      if(plateIndex >= 0)
      {
        pG2DUtil->StartAnime( LYT_WK_ID_LOWER, m_offPlate[plateIndex].passiveAnim );
      }
      else
      {
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
      }
    }
  }
  buttonManager->SetButtonPassive(BUTTON_ID_BACK);

  ZukanType::InfoType infoType = static_cast<ZukanType::InfoType>( buttonId - BUTTON_ID_LIST_ELEM_ZUKAN_START );
  u32 admireResultMsgStrID = 0;
  u32 admireResultMEID     = 0;
  this->getAdmireResultId(infoType, &admireResultMsgStrID, &admireResultMEID);

  {
    ZukanTool::ExpandStrBufParam expandStrBufParam;
    expandStrBufParam.dstStrBuf        = m_strBuf;
    expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN_HYOUKA;
    expandStrBufParam.srcMsgStrId      = admireResultMsgStrID;
    m_param.tool->ExpandStrBuf(expandStrBufParam);
  }

  pG2DUtil->SetMessage( m_strBuf );
  pG2DUtil->SetMessageFinishMode( gfl2::str::StrWin::FINISH_USER, 0 );
  this->changeDispState(DISP_STATE_ADMIRE_ON_RESULT_MES);

  app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();
  menuCursor->SetVisible(false);

  Sound::StartME(admireResultMEID);
  m_bgmChanged = true;
        
  m_rotomVoice->Stop();

  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewLaunchListener::INPUT_STATE_OCCUPY);
}

void ZukanViewLaunch::getAdmireResultId(ZukanType::InfoType infoType, u32* admireResultMsgStrID, u32* admireResultMEID) const
{
  static const u32 ALOLA_LEVEL_MIN_NUM[] =
  {         //  niji
    0,
    55,     //   40,
    110,    //   80,
    165,    //  120,
    220,    //  160,
    280,    //  210,
    340,    //  260,
    400,    //  300  // 図鑑完成に含めないポケモンも匹数を数えないように。伝説除き匹数をゲットできるようにする。ただし、表示上は伝説の匹数も含めておく。
  };
  static const u32  ALOLA_LEVEL_ELEMS = GFL_NELEMS(ALOLA_LEVEL_MIN_NUM);
  
  /*  島1  */
  static const u32 ISLAND1_LEVEL_MIN_NUM[] =
  {
    0,
    35,     //   30,
    70,     //   60,
    110,    //   90,
    150,    //  120
  };
  static const u32  ISLAND1_LEVEL_ELEMS = GFL_NELEMS(ISLAND1_LEVEL_MIN_NUM);

  /*  島2  */
  static const u32 ISLAND2_LEVEL_MIN_NUM[] =
  {
    0,
    40,     //   35,
    80,     //   70,
    120,    //  105,
    160,    //  130
  };
  static const u32  ISLAND2_LEVEL_ELEMS = GFL_NELEMS(ISLAND2_LEVEL_MIN_NUM);

  /*  島3  */
  static const u32 ISLAND3_LEVEL_MIN_NUM[] =
  {
    0,
    40,     //   35,
    80,     //   70,
    120,    //  105,
    160,    //  130
  };
  static const u32  ISLAND3_LEVEL_ELEMS = GFL_NELEMS(ISLAND3_LEVEL_MIN_NUM);

  /*  島4  */
  static const u32 ISLAND4_LEVEL_MIN_NUM[] =
  {
      0,
     35,    //   25,
     70,    //   50,
    105,    //   75,
    130,    //  100
  };
  static const u32  ISLAND4_LEVEL_ELEMS = GFL_NELEMS(ISLAND4_LEVEL_MIN_NUM);

  static const u32 ADMIRE_RESULT_ME_ID[] =
  {
    STRM_ME_HYOUKA01,
    STRM_ME_HYOUKA02,  // ビギナーランク 
    STRM_ME_HYOUKA03,  // ノービスランク 
    STRM_ME_HYOUKA04,  // ノーマルランク 
    STRM_ME_HYOUKA05,  // スーパーランク 
    STRM_ME_HYOUKA06,  // ハイパーランク 
    STRM_ME_HYOUKA07,  // エリートランク 
    STRM_ME_HYOUKA08,  // コンプリート
  };

  /*
    図鑑評価メッセージ
  */
  /*  alola  */
  static const u32  ALOLA_MESSAGE_TABLE[Savedata::FieldMenu::ROTOM_RANK_MAX][ALOLA_LEVEL_ELEMS] =
  {
    /*  →図鑑レベル  */
    /*  ↓新密度  */
    {msg_hyouka_00_01,   msg_hyouka_00_02,   msg_hyouka_00_03,   msg_hyouka_00_04,   msg_hyouka_00_05,   msg_hyouka_00_06,   msg_hyouka_00_07,   msg_hyouka_00_08,},
    {msg_hyouka_00_01_b, msg_hyouka_00_02_b, msg_hyouka_00_03_b, msg_hyouka_00_04_b, msg_hyouka_00_05_b, msg_hyouka_00_06_b, msg_hyouka_00_07_b, msg_hyouka_00_08_b,},
    {msg_hyouka_00_01_c, msg_hyouka_00_02_c, msg_hyouka_00_03_c, msg_hyouka_00_04_c, msg_hyouka_00_05_c, msg_hyouka_00_06_c, msg_hyouka_00_07_c, msg_hyouka_00_08_c,},
    {msg_hyouka_00_01_d, msg_hyouka_00_02_d, msg_hyouka_00_03_d, msg_hyouka_00_04_d, msg_hyouka_00_05_d, msg_hyouka_00_06_d, msg_hyouka_00_07_d, msg_hyouka_00_08_d,},
  };
  /*  島1  */
  static const u32  ISLAND1_MESSAGE_TABLE[Savedata::FieldMenu::ROTOM_RANK_MAX][ALOLA_LEVEL_ELEMS] =
  {
    /*  →図鑑レベル  */
    /*  ↓新密度  */
    {msg_hyouka_01_01,   msg_hyouka_01_02,   msg_hyouka_01_03,   msg_hyouka_01_04,   msg_hyouka_01_05},
    {msg_hyouka_01_01_b, msg_hyouka_01_02_b, msg_hyouka_01_03_b, msg_hyouka_01_04_b, msg_hyouka_01_05_b},
    {msg_hyouka_01_01_c, msg_hyouka_01_02_c, msg_hyouka_01_03_c, msg_hyouka_01_04_c, msg_hyouka_01_05_c},
    {msg_hyouka_01_01_d, msg_hyouka_01_02_d, msg_hyouka_01_03_d, msg_hyouka_01_04_d, msg_hyouka_01_05_d},
  };

  /*  島2  */
  static const u32  ISLAND2_MESSAGE_TABLE[Savedata::FieldMenu::ROTOM_RANK_MAX][ALOLA_LEVEL_ELEMS] =
  {
    /*  →図鑑レベル  */
    /*  ↓新密度  */
    {msg_hyouka_02_01,   msg_hyouka_02_02,   msg_hyouka_02_03,   msg_hyouka_02_04,   msg_hyouka_02_05},
    {msg_hyouka_02_01_b, msg_hyouka_02_02_b, msg_hyouka_02_03_b, msg_hyouka_02_04_b, msg_hyouka_02_05_b},
    {msg_hyouka_02_01_c, msg_hyouka_02_02_c, msg_hyouka_02_03_c, msg_hyouka_02_04_c, msg_hyouka_02_05_c},
    {msg_hyouka_02_01_d, msg_hyouka_02_02_d, msg_hyouka_02_03_d, msg_hyouka_02_04_d, msg_hyouka_02_05_d},
  };

  /*  島3  */
  static const u32  ISLAND3_MESSAGE_TABLE[Savedata::FieldMenu::ROTOM_RANK_MAX][ALOLA_LEVEL_ELEMS] =
  {
    /*  →図鑑レベル  */
    /*  ↓新密度  */
    {msg_hyouka_03_01,   msg_hyouka_03_02,   msg_hyouka_03_03,   msg_hyouka_03_04,   msg_hyouka_03_05},
    {msg_hyouka_03_01_b, msg_hyouka_03_02_b, msg_hyouka_03_03_b, msg_hyouka_03_04_b, msg_hyouka_03_05_b},
    {msg_hyouka_03_01_c, msg_hyouka_03_02_c, msg_hyouka_03_03_c, msg_hyouka_03_04_c, msg_hyouka_03_05_c},
    {msg_hyouka_03_01_d, msg_hyouka_03_02_d, msg_hyouka_03_03_d, msg_hyouka_03_04_d, msg_hyouka_03_05_d},
  };

  /*  島4  */
  static const u32  ISLAND4_MESSAGE_TABLE[Savedata::FieldMenu::ROTOM_RANK_MAX][ALOLA_LEVEL_ELEMS] =
  {
    /*  →図鑑レベル  */
    /*  ↓新密度  */
    {msg_hyouka_04_01,   msg_hyouka_04_02,   msg_hyouka_04_03,   msg_hyouka_04_04,   msg_hyouka_04_05},
    {msg_hyouka_04_01_b, msg_hyouka_04_02_b, msg_hyouka_04_03_b, msg_hyouka_04_04_b, msg_hyouka_04_05_b},
    {msg_hyouka_04_01_c, msg_hyouka_04_02_c, msg_hyouka_04_03_c, msg_hyouka_04_04_c, msg_hyouka_04_05_c},
    {msg_hyouka_04_01_d, msg_hyouka_04_02_d, msg_hyouka_04_03_d, msg_hyouka_04_04_d, msg_hyouka_04_05_d},
  };

  STATIC_ASSERT(Savedata::FieldMenu::ROTOM_RANK_MAX == 4);    /**< ロトム親密度の数が変わったらこちらも対応する必要がある   */

  const u32*                            levelMinNumArray  = ALOLA_LEVEL_MIN_NUM;
  u32                                   levelMinNumSize   = 0;
  const u32*                            pMessageTable     = ALOLA_MESSAGE_TABLE[0];
  GameSys::GameManager*                 pGameManager      = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  GameSys::GameData*                    pGameData         = pGameManager->GetGameData();
  const Field::EventWork*               pEventWork        = pGameData->GetEventWork();
  const Savedata::FieldMenu::RotomRank  rotomRank         = pGameData->GetFieldMenu()->GetRotomRank(App::Tool::RotomFriendship::GetDataIndex(pEventWork));
  const u32                             getNum            = m_param.contents->GetGetNum(infoType); /**<  捕獲数  */


  switch(infoType)
  {
  case ZukanType::INFO_TYPE_ALOLA  : levelMinNumArray = ALOLA_LEVEL_MIN_NUM;   levelMinNumSize = ALOLA_LEVEL_ELEMS;   pMessageTable = ALOLA_MESSAGE_TABLE[rotomRank];   break;
  case ZukanType::INFO_TYPE_ISLAND1: levelMinNumArray = ISLAND1_LEVEL_MIN_NUM; levelMinNumSize = ISLAND1_LEVEL_ELEMS; pMessageTable = ISLAND1_MESSAGE_TABLE[rotomRank]; break;
  case ZukanType::INFO_TYPE_ISLAND2: levelMinNumArray = ISLAND2_LEVEL_MIN_NUM; levelMinNumSize = ISLAND2_LEVEL_ELEMS; pMessageTable = ISLAND2_MESSAGE_TABLE[rotomRank]; break;
  case ZukanType::INFO_TYPE_ISLAND3: levelMinNumArray = ISLAND3_LEVEL_MIN_NUM; levelMinNumSize = ISLAND3_LEVEL_ELEMS; pMessageTable = ISLAND3_MESSAGE_TABLE[rotomRank]; break;
  case ZukanType::INFO_TYPE_ISLAND4: levelMinNumArray = ISLAND4_LEVEL_MIN_NUM; levelMinNumSize = ISLAND4_LEVEL_ELEMS; pMessageTable = ISLAND4_MESSAGE_TABLE[rotomRank]; break;
  }

  u32 level = 0;  // 0始まり
  u32 i = levelMinNumSize;
  while(i > 0)
  {
    if(getNum >= levelMinNumArray[i-1])
    {
      level = i-1;
      break;
    }
    --i;
  }

  GFL_PRINT("rotom msg: type[%d], rotomRank[%d], dexLv[%d]\n", infoType, rotomRank, level);

  


  *admireResultMsgStrID = pMessageTable[level];
  *admireResultMEID     = ADMIRE_RESULT_ME_ID[level];
}





void ZukanViewLaunch::setParamWhenOutSeq(ZukanType::InfoType infoType, ZukanType::SceneType nextSceneType)
{
  m_nextInfoType = infoType;
   
  *m_param.nextSceneType = nextSceneType;
  *m_param.launchSceneCursorIndex = m_cursorIndex;
}

void ZukanViewLaunch::changeDispState(DispState dispState)
{
  m_dispState = dispState;
  m_dispStateCount = 0;
}

void ZukanViewLaunch::createPage(void)
{
  app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  gfl2::lyt::LytWk*        lytWk_0  = this->GetLayoutWork( LYT_WK_ID_LOWER );  // wk_0  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytWk*        lytWk_1  = this->GetLayoutWork( LYT_WK_ID_UPPER );  // wk_1  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytMultiResID lytRes_0 = pG2DUtil->GetLayoutResourceID( 0 );  // res_0

  ////////////////
  ////////////////
  // 上画面
  ////////////////
  ////////////////
  {
    gfl2::lyt::LytPane* digit3Pane = lytWk_1->GetPane(PANENAME_ZUKAN_STR_UPP_000_PANE_DIGIT_3_TOP);
    gfl2::lyt::LytPane* digit2Pane = lytWk_1->GetPane(PANENAME_ZUKAN_STR_UPP_000_PANE_DIGIT_2_TOP);
    gfl2::lyt::LytPane* digit1Pane = lytWk_1->GetPane(PANENAME_ZUKAN_STR_UPP_000_PANE_DIGIT_1_TOP);

    u32 achievementRate = m_param.contents->GetAchievementRate(ZukanType::INFO_TYPE_ALOLA);
    if(achievementRate < 10)
    {
      digit3Pane->SetVisible(false);
      digit2Pane->SetVisible(false);
      digit1Pane->SetVisible(true);
      ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_UPPER, LA_LAUNCH_ZUKAN_STR_UPP_000__NUM_05_NUMBER, achievementRate);
    }
    else if(achievementRate < 100)
    {
      digit3Pane->SetVisible(false);
      digit2Pane->SetVisible(true);
      digit1Pane->SetVisible(false);
      ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_UPPER, LA_LAUNCH_ZUKAN_STR_UPP_000__NUM_04_NUMBER, achievementRate/10);
      ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_UPPER, LA_LAUNCH_ZUKAN_STR_UPP_000__NUM_03_NUMBER, achievementRate%10);
    }
    else
    {
      digit3Pane->SetVisible(true);
      digit2Pane->SetVisible(false);
      digit1Pane->SetVisible(false);
      ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_UPPER, LA_LAUNCH_ZUKAN_STR_UPP_000__NUM_02_NUMBER, achievementRate/100);
      achievementRate = achievementRate - (achievementRate/100) * 100;
      ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_UPPER, LA_LAUNCH_ZUKAN_STR_UPP_000__NUM_01_NUMBER, achievementRate/10);
      ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_UPPER, LA_LAUNCH_ZUKAN_STR_UPP_000__NUM_00_NUMBER, achievementRate%10);
    }

    // メッセージウィンドウ
    {
      gfl2::lyt::LytParts* mesWinPartsPane = lytWk_1->GetPartsPane(PANENAME_ZUKAN_STR_UPP_000_PANE_MESSAGE_WINDOW);
      m_mesWinPane = mesWinPartsPane;
      m_mesWinTextPane[0] = lytWk_1->GetTextBoxPane(mesWinPartsPane, PANENAME_ZUKAN_MSG_UPP_000_PANE_TEXTBOX_01, &lytRes_0);
      m_mesWinTextPane[1] = lytWk_1->GetTextBoxPane(mesWinPartsPane, PANENAME_ZUKAN_MSG_UPP_000_PANE_TEXTBOX_02, &lytRes_0);
      m_mesCursorPane = lytWk_1->GetPane(mesWinPartsPane, PANENAME_ZUKAN_MSG_UPP_000_PANE_CURSOR_00, &lytRes_0);
      
      // メッセージウィンドウ用のテキストペインにグラフィッカーさんが入れておいたダミー文字列が
      // 最初にちらっと見えてしまうので、空文字列をセットして消しておく。
      m_strBuf->Clear(); 
      for(u32 i=0; i<2; ++i)
      {
        pG2DUtil->SetTextBoxPaneString( m_mesWinTextPane[i], m_strBuf );
      }

      gfl2::lyt::LytPane* twoLinePane = lytWk_1->GetPane(mesWinPartsPane, PANENAME_ZUKAN_MSG_UPP_000_PANE_NULL_TEXT1, &lytRes_0);
      twoLinePane->SetVisible(true);
    }

    {
      // ループアニメ
      static const u32 UPPER_LOOP_ANIM_INDEX[UPPER_LOOP_ANIM_NUM] =
      {
        LA_LAUNCH_ZUKAN_STR_UPP_000_NEW_LOOP,
      };
      for(s32 i=0; i<UPPER_LOOP_ANIM_NUM; ++i)
      {
        m_upperLoopAnimIndex[i] = UPPER_LOOP_ANIM_INDEX[i];
      }

      // 色替えアニメ
      if(m_param.contents->IsGetCompleted(ZukanType::INFO_TYPE_ALOLA))
      {
        pG2DUtil->StartAnime( LYT_WK_ID_UPPER, LA_LAUNCH_ZUKAN_STR_UPP_000_COMP_COLOR_01 );
      }
      else
      {
        pG2DUtil->StartAnime( LYT_WK_ID_UPPER, LA_LAUNCH_ZUKAN_STR_UPP_000_COMP_COLOR_00 );
      }
    }
  }

  ////////////////
  ////////////////
  // 下画面
  ////////////////
  ////////////////
  {
    static const u32 ZUKAN_MES[] =
    {
      msg_zkn_02_01,
      msg_zkn_02_02,
      msg_zkn_02_03,
      msg_zkn_02_04,
      msg_zkn_02_05,
    };
    static const u32 OTHER_MES[] =
    {
      msg_zkn_02_07,
    };

    static const ZukanType::InfoType ZUKAN_INFO_TYPE[] =
    {
      ZukanType::INFO_TYPE_ALOLA,
      ZukanType::INFO_TYPE_ISLAND1,
      ZukanType::INFO_TYPE_ISLAND2,
      ZukanType::INFO_TYPE_ISLAND3,
      ZukanType::INFO_TYPE_ISLAND4,
    };

    u32 zukanMesIndex = 0;
    u32 otherMesIndex = 0;
    pG2DUtil->SetMessageData( m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN), m_param.tool->GetWordSet() );
    {
      for(u32 i=0; i<BUTTON_ID_NUM; ++i)
      {
        if( BUTTON_ID_LIST_ELEM_ZUKAN_START <= i && i <= BUTTON_ID_LIST_ELEM_ZUKAN_END )
        {
          gfl2::lyt::LytTextBox* textBox0 = lytWk_0->GetTextBoxPane(m_buttonPaneInfo[i].partsPane, PANENAME_ZUKAN_BTN_LOW_001_PANE_TEXTBOX_00, &lytRes_0);
          gfl2::lyt::LytTextBox* textBox1 = lytWk_0->GetTextBoxPane(m_buttonPaneInfo[i].partsPane, PANENAME_ZUKAN_BTN_LOW_001_PANE_TEXTBOX_01, &lytRes_0);
          gfl2::lyt::LytTextBox* textBox2 = lytWk_0->GetTextBoxPane(m_buttonPaneInfo[i].partsPane, PANENAME_ZUKAN_BTN_LOW_001_PANE_TEXTBOX_02, &lytRes_0);
          pG2DUtil->SetTextBoxPaneString(textBox0, ZUKAN_MES[zukanMesIndex]);
          pG2DUtil->SetTextBoxPaneNumber(textBox1, msg_zkn_01_02, m_param.contents->GetRealGetNum(ZUKAN_INFO_TYPE[zukanMesIndex]), 3, 0, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU);
          pG2DUtil->SetTextBoxPaneNumber(textBox2, msg_zkn_01_02, m_param.contents->GetRealSeeNum(ZUKAN_INFO_TYPE[zukanMesIndex]), 3, 0, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU);
          if(m_param.contents->IsGetCompleted(ZUKAN_INFO_TYPE[zukanMesIndex]))
          {
            ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, BUTTON_CONST_INFO[i].other3_anime_index, 0);
          }
          else if(m_param.contents->IsSeeCompleted(ZUKAN_INFO_TYPE[zukanMesIndex]))
          {
            ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, BUTTON_CONST_INFO[i].other2_anime_index, 0);
          }
          else
          {
            ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, BUTTON_CONST_INFO[i].other1_anime_index, 0);
          }
          ++zukanMesIndex;
        }
        else if( BUTTON_ID_LIST_ELEM_OTHER_START <= i && i <= BUTTON_ID_LIST_ELEM_OTHER_END )
        {
          gfl2::lyt::LytTextBox* textBox0 = lytWk_0->GetTextBoxPane(m_buttonPaneInfo[i].partsPane, PANENAME_ZUKAN_BTN_LOW_002_PANE_TEXTBOX_00, &lytRes_0);
          pG2DUtil->SetTextBoxPaneString(textBox0, OTHER_MES[otherMesIndex]);
          ++otherMesIndex;
        }
      }
    }
    pG2DUtil->SetMessageData(NULL, NULL);
  }

  // OffPlate
  {
    static const u32 OFF_PLATE_PANE_INDEX[BUTTON_ID_LIST_ELEM_ISLAND_NUM] =
    {
      PANENAME_ZUKAN_MIN_LOW_001_PANE_B_PLATE_01,
      PANENAME_ZUKAN_MIN_LOW_001_PANE_B_PLATE_02,
      PANENAME_ZUKAN_MIN_LOW_001_PANE_B_PLATE_03,
      PANENAME_ZUKAN_MIN_LOW_001_PANE_B_PLATE_04,
    };
    static const u32 OFF_PLATE_ACTIVE_ANIM_INDEX[BUTTON_ID_LIST_ELEM_ISLAND_NUM] =
    {
      LA_LAUNCH_ZUKAN_MIN_LOW_001__B_PLATE_01_ACTIVE,
      LA_LAUNCH_ZUKAN_MIN_LOW_001__B_PLATE_02_ACTIVE,
      LA_LAUNCH_ZUKAN_MIN_LOW_001__B_PLATE_03_ACTIVE,
      LA_LAUNCH_ZUKAN_MIN_LOW_001__B_PLATE_04_ACTIVE,
    };
    static const u32 OFF_PLATE_PASSIVE_ANIM_INDEX[BUTTON_ID_LIST_ELEM_ISLAND_NUM] =
    {
      LA_LAUNCH_ZUKAN_MIN_LOW_001__B_PLATE_01_PASSIVE,
      LA_LAUNCH_ZUKAN_MIN_LOW_001__B_PLATE_02_PASSIVE,
      LA_LAUNCH_ZUKAN_MIN_LOW_001__B_PLATE_03_PASSIVE,
      LA_LAUNCH_ZUKAN_MIN_LOW_001__B_PLATE_04_PASSIVE,
    };

    for(s32 i=0; i<BUTTON_ID_LIST_ELEM_ISLAND_NUM; ++i)
    {
      m_offPlate[i].pane        = lytWk_0->GetPane(OFF_PLATE_PANE_INDEX[i]);
      m_offPlate[i].activeAnim  = OFF_PLATE_ACTIVE_ANIM_INDEX[i];
      m_offPlate[i].passiveAnim = OFF_PLATE_PASSIVE_ANIM_INDEX[i];

      m_offPlate[i].pane->SetVisible(false);
    }
  }
}

void ZukanViewLaunch::setupButtonOpen(void)
{
  app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  
  // ADMIREボタン
  if(!m_param.contents->DoesRotomAppear())
  {
    ZukanUtil::SetButtonVisible(buttonManager, BUTTON_ID_ADMIRE, false);
  }
  
  // ISLANDボタン
  for(s32 i=ZukanType::INFO_TYPE_ISLAND_END; i>=ZukanType::INFO_TYPE_ISLAND_START; --i)
  {
    if(m_param.contents->IsOpened(static_cast<ZukanType::InfoType>(i)))
    {
      break;
    }
    else
    {
      ZukanUtil::SetButtonVisible(buttonManager, BUTTON_ID_LIST_ELEM_ZUKAN_START + i, false);
      m_offPlate[i - ZukanType::INFO_TYPE_ISLAND_START].pane->SetVisible(true);
    }
  }
}

void ZukanViewLaunch::changeMode(bool modeIsAdmire)
{
  m_modeIsAdmire = modeIsAdmire;
  if(m_modeIsAdmire)
  {
    this->changeDispState(DISP_STATE_ADMIRE_OFF);
  }
  else
  {
    this->changeDispState(DISP_STATE_ON_IN_FROM_ADMIRE);
  }
}
void ZukanViewLaunch::updateForLaunch(void)
{
  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();

  // 表示状態
  switch(m_dispState)
  {
  case DISP_STATE_OFF:
    {
      break;
    }
  case DISP_STATE_ON_FADE_IN:
    {
      if(pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE))
      {
        this->changeDispState(DISP_STATE_ON_IN0);
        break;  // 直下のステートにいくわけではないのでbreakする
      }
      else
      {
        break;
      }
    }
  case DISP_STATE_ON_FADE_IN_ROTOM_IN:
    {
      if(pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE))
      {
        // ロトムフェードアニメと図鑑起動アニメを同時に再生する
        // ロトムフェードアニメをしなくてもいいときは、図鑑起動アニメだけ再生する
       
        if(m_rotomFadeFlag)
        {
          // ロトムフェードアニメ
          pG2DUtil->StartAnime(LYT_WK_ID_ROTOM_LOWER, LA_ROTOM_FADE_ROTOM_FADE_LOW_000_FADE_00);
          pG2DUtil->StartAnime(LYT_WK_ID_ROTOM_UPPER, LA_ROTOM_FADE_ROTOM_FADE_UPP_000_FADE_00);
          // ロトムフェードアニメのＳＥ
          Sound::PlaySE( SEQ_SE_SYS_NJ_028 );  // 起動のＳＥ
          Sound::PlaySE( SEQ_SE_SYS_NJ_031 );  // ロトムのボイス
        }
       
        // 図鑑起動アニメ
        ZukanUtil::StartLytWaitAnim(pG2DUtil, m_waitAnimIndex, LYT_WK_ID_NUM);
        // 図鑑起動アニメの音→なくなりました
        switch(m_param.prevSceneType)
        {
        case ZukanType::SCENE_TYPE_ALOLA_TOP:
        case ZukanType::SCENE_TYPE_ISLAND_TOP:
          //Sound::PlaySE(なし);
          break;
        case ZukanType::SCENE_TYPE_START_ZUKAN:
        default:
          //Sound::PlaySE(なし);
          break;
        }
       
        this->changeDispState(DISP_STATE_ON_ROTOM_IN);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case DISP_STATE_ON_ROTOM_IN:
    {
      if(m_rotomFadeFlag)
      {
        if(
               pG2DUtil->IsAnimeEnd(LYT_WK_ID_ROTOM_LOWER, LA_ROTOM_FADE_ROTOM_FADE_LOW_000_FADE_00)
            && pG2DUtil->IsAnimeEnd(LYT_WK_ID_ROTOM_UPPER, LA_ROTOM_FADE_ROTOM_FADE_UPP_000_FADE_00)
        )
        {
          this->changeDispState(DISP_STATE_ON_IN0);
          //breakしない
        }
        else
        {
          break;
        }
      }
      else
      {
        this->changeDispState(DISP_STATE_ON_IN0);
        //breakしない
      }
    }
  case DISP_STATE_ON_IN0:
    {
      if( ZukanUtil::IsLytWaitAnimEnd(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM) )
      {
        this->changeDispState(DISP_STATE_ON_IN1);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case DISP_STATE_ON_IN1:
    {
      //if( (*(m_param.setupState)) == ZukanType::SETUP_STATE_ALL_FINISHED )  // この画面から出るときに待つことにした
      {
        menuCursor->SetVisible(true);
        this->setCursorIndexAndPutMenuCursor(m_cursorIndex, false, false);
        this->changeDispState(DISP_STATE_ON_WORK);
        m_viewListener->SetInputState(ZukanViewLaunchListener::INPUT_STATE_ENABLE);
      }
      break;
    }
  case DISP_STATE_ON_IN_FROM_ADMIRE:
    {
      pG2DUtil->StopAnime( LYT_WK_ID_UPPER, LA_LAUNCH_ZUKAN_STR_UPP_000_ROTOM );
      for(s32 i=0; i<UPPER_LOOP_ANIM_NUM; ++i)
      {
        pG2DUtil->StartAnime( LYT_WK_ID_UPPER, m_upperLoopAnimIndex[i] );
      }

      menuCursor->SetVisible(true);
      this->setCursorIndexAndPutMenuCursor(m_cursorIndex, false, false);
      this->changeDispState(DISP_STATE_ON_WORK);
      m_viewListener->SetInputState(ZukanViewLaunchListener::INPUT_STATE_ENABLE);
      break;
    }
  case DISP_STATE_ON_WORK:
    {
      break;
    }
  case DISP_STATE_ON_OUT:
    {
      if( ZukanUtil::IsLytWaitAnimEnd(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM) )
      {
        if(m_fadeOutFlag)
        {
          pFadeManager->RequestDefaultOut(gfl2::Fade::DISP_DOUBLE);
          this->changeDispState(DISP_STATE_ON_FADE_OUT);
        }
        else
        {
          if(*m_param.nextSceneType == ZukanType::SCENE_TYPE_ALOLA_TOP)
          {
            pFadeManager->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_CROSS, &ZukanType::CROSS_FADE_COLOR_START, &ZukanType::CROSS_FADE_COLOR_END, 1);  // ぱっと消す
            this->changeDispState(DISP_STATE_ON_FADE_OUT);
          }
          else
          {
            this->changeDispState(DISP_STATE_WAIT_SAFETY);
          }
        }
      }
      break;
    }
  case DISP_STATE_ON_FADE_OUT:
    {
      if(pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE))
      {
        this->changeDispState(DISP_STATE_WAIT_SAFETY);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case DISP_STATE_WAIT_SAFETY:
    {
      // メニューカーソルレイアウト破棄
      if( m_param.tool->DestroyMenuCursorLayoutWork() )
      {
        if( (*(m_param.setupState)) == ZukanType::SETUP_STATE_ALL_FINISHED )
        {
          // データの準備ができてからコンテンツのセットアップを行う
          if(m_nextInfoType < ZukanType::INFO_TYPE_NUM)
          {
            // Contents
            m_param.contents->SetInfoType(m_nextInfoType);
            m_param.contents->SetupData(m_nextInfoType);
          } 

          m_param.frameCell->Req(ZukanFrameLaunchCell::REQ_ID_END);
          this->changeDispState(DISP_STATE_WAIT_END);
        }
      }
      break;
    }
  case DISP_STATE_WAIT_END:
    {
      break;
    }
  }
}
void ZukanViewLaunch::updateForAdmire(void)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  app::tool::MenuCursor* menuCursor = m_param.tool->GetMenuCursor();

  // 表示状態
  switch(m_dispState)
  {
  case DISP_STATE_ADMIRE_OFF:
    {
      break;
    }
  case DISP_STATE_ADMIRE_ON_IN:
    {
      if( ZukanUtil::IsLytWaitAnimEnd(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM) )
      {
        pG2DUtil->StartAnime( LYT_WK_ID_UPPER, LA_LAUNCH_ZUKAN_STR_UPP_000_ROTOM_LOOP );
        //for(s32 i=0; i<UPPER_LOOP_ANIM_NUM; ++i)  // @fix GFNMCat[5185] 「図鑑」において、「図鑑評価」画面へ遷移すると上画面背景のアニメーションが停止する  →アニメ停止をコメントアウトし、停止しないようにした。
        //{
        //  pG2DUtil->StopAnime( LYT_WK_ID_UPPER, m_upperLoopAnimIndex[i] );
        //}

        pG2DUtil->CreateMessageWindow(
            GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(),
            LYT_WK_ID_UPPER,
            m_mesWinTextPane[0],
            m_mesWinTextPane[1],
            m_param.appHeap
        );
        pG2DUtil->CreateMsgCursor(
            m_param.appHeap,
            m_mesCursorPane
        );
        pG2DUtil->StartAnime( LYT_WK_ID_UPPER, LA_LAUNCH_ZUKAN_STR_UPP_000__MESSAGE_WINDOW__CURSOR_00_KEEP );  // メッセージ送りのカーソル
        
        m_rotomVoice->Start(true, 0);
        this->changeDispState(DISP_STATE_ADMIRE_ON_SELECT_START);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case DISP_STATE_ADMIRE_ON_SELECT_START:
    {
      {
        ZukanTool::ExpandStrBufParam expandStrBufParam;
        expandStrBufParam.dstStrBuf        = m_strBuf;
        expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
        expandStrBufParam.srcMsgStrId      = msg_zkn_13_01;
        expandStrBufParam.srcRegBufType[0] = ZukanTool::REG_BUF_TYPE_PLAYER_NAME;
        expandStrBufParam.srcRegBufType[1] = ZukanTool::REG_BUF_TYPE_PLAYER_NICKNAME;
        m_param.tool->ExpandStrBuf(expandStrBufParam);
      }
      pG2DUtil->SetMessage( m_strBuf );
      pG2DUtil->SetMessageFinishMode( gfl2::str::StrWin::FINISH_NONE, 0 );

      this->changeDispState(DISP_STATE_ADMIRE_ON_SELECT_MES);
      break;
    }
  case DISP_STATE_ADMIRE_ON_SELECT_MES:
    {
      gfl2::str::StrWin::Result mesResult = pG2DUtil->PrintMessage();
      if( mesResult == gfl2::str::StrWin::RES_DONE )  // gfl2::str::StrWin::FinishTypeがFINISH_NONEのとき
      {
        menuCursor->SetVisible(true);
        this->setCursorIndexAndPutMenuCursor(m_cursorIndex, false, false);
        m_viewListener->SetInputState(ZukanViewLaunchListener::INPUT_STATE_ENABLE);

        this->changeDispState(DISP_STATE_ADMIRE_ON_SELECT_WAIT);
      }
      break;
    }
  case DISP_STATE_ADMIRE_ON_SELECT_WAIT:
    {
      break;
    }
  case DISP_STATE_ADMIRE_ON_RESULT_MES:
    {
      bool isNext = false;

      if( m_bgmChanged )
      {
        ZukanType::InfoType infoType = static_cast<ZukanType::InfoType>( m_cursorIndex - BUTTON_ID_LIST_ELEM_ZUKAN_START );
        u32 admireResultMsgStrID = 0;
        u32 admireResultMEID     = 0;
        this->getAdmireResultId(infoType, &admireResultMsgStrID, &admireResultMEID);

        if( Sound::IsMEFinished(admireResultMEID) )
        {
          Sound::StartBaseBGMFromME();
          m_bgmChanged = false;
  
          //m_rotomVoice->Start(false, 0);  // MEと被るのでナシにする。コメントアウトした。
        }
      }

      gfl2::str::StrWin::Result mesResult = pG2DUtil->PrintMessage();
      if( mesResult == gfl2::str::StrWin::RES_FINISH )  // gfl2::str::StrWin::FinishTypeがFINISH_USERのとき
      {
        if( m_bgmChanged == false )
        {
          isNext = true;
        }
      }

      if(isNext)
      {
        Sound::PlaySE(SEQ_SE_MESSAGE);  // メッセージが切り替わるので音がないと寂しい

        app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
        for(s32 i=BUTTON_ID_LIST_ELEM_ZUKAN_START; i<=BUTTON_ID_LIST_ELEM_ZUKAN_END; ++i)
        {
          if( ZukanUtil::IsButtonVisible(buttonManager, i) )
          {
            if( i != m_cursorIndex ) buttonManager->SetButtonActive(i);
          }
          else
          {
            s32 plateIndex = i - BUTTON_ID_LIST_ELEM_ISLAND_START;
            if(plateIndex >= 0)
            {
              pG2DUtil->StartAnime( LYT_WK_ID_LOWER, m_offPlate[plateIndex].activeAnim );
            }
            else
            {
              GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
            }
          }
        }
        buttonManager->SetButtonActive(BUTTON_ID_BACK);

        this->changeDispState(DISP_STATE_ADMIRE_ON_SELECT_START);
      }
      break;
    }
  case DISP_STATE_ADMIRE_ON_OUT:
    {
      if( ZukanUtil::IsLytWaitAnimEnd(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM) )
      {
        m_mesWinPane->SetVisible(false);
        this->changeMode(false);
      }
      break;
    }
  }
  
  m_rotomVoice->Update();
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

