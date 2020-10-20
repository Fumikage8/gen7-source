//==============================================================================
/**
 @file    ZukanViewRegisterAlolaTop.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の登録の際のアローラ図鑑トップ画面のビュー
 */
//==============================================================================


// gfl2のインクルード
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

#include <Sound/include/Sound.h>

#include <Savedata/include/ZukanSave.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/alola_top.h>
#include <niji_conv_header/app/zukan/alola_top_pane.h>
#include <niji_conv_header/app/zukan/alola_top_anim_list.h>

#include <niji_conv_header/app/common/graphic_font/zukan_grpfont/res2d/ZukanGrpFont.h>

#include <niji_conv_header/message/msg_zukan.h>

// 図鑑登録の呼び出しEventのインクルード
#include <App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanSceneParam.h"
#include "ZukanType.h"
#include "ZukanUtil.h"
#include "ZukanContents.h"
#include "ZukanDataManager.h"
#include "ZukanTool.h"
#include "ZukanPicScrollDataAccessor.h"
#include "ZukanPicScrollDemo.h"
#include "ZukanFrameRegisterAlolaTop.h"

#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
#include "ZukanPrismSheet.h"
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE

#include "ZukanViewRegisterAlolaTop.h"


#define NIJI_APP_ZUKAN_REGISTER_INTRO_FADE_NONE  (0)
#define NIJI_APP_ZUKAN_REGISTER_INTRO_FADE_CROSS (1)
#define NIJI_APP_ZUKAN_REGISTER_INTRO_FADE_ALPHA (2)
#define NIJI_APP_ZUKAN_REGISTER_INTRO_FADE (NIJI_APP_ZUKAN_REGISTER_INTRO_FADE_NONE)  // これが0のときフェードなし、1のときクロスフェード、2のときアルファフェード  // ZukanFrameRegisterIntro.cppにもある  // 1より2のほうがきれいだが0にする。


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewRegisterAlolaTopCommentResultListener
 @brief     コメントのリスナー
 */
//==============================================================================
class ZukanViewRegisterAlolaTopCommentResultListener : public ZukanViewRegisterComment::ResultListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewRegisterAlolaTopCommentResultListener);

public:
  ZukanViewRegisterAlolaTopCommentResultListener(ZukanViewRegisterAlolaTop* view)
    : ZukanViewRegisterComment::ResultListener(),
      m_view(view)
  {}
  virtual ~ZukanViewRegisterAlolaTopCommentResultListener()
  {}

public:
  virtual void Req(s32 /*reqId*/)
  {
    m_view->Req(ZukanViewRegisterAlolaTop::REQ_ID_OUT);
  }

private:
  ZukanViewRegisterAlolaTop* m_view;
};


//==============================================================================
/**
 @class     ZukanViewRegisterAlolaTopListener
 @brief     図鑑の登録の際のアローラ図鑑トップ画面のビューのリスナー
 */
//==============================================================================
class ZukanViewRegisterAlolaTopListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewRegisterAlolaTopListener);

public:
  ZukanViewRegisterAlolaTopListener(ZukanViewRegisterAlolaTop* view);
  virtual ~ZukanViewRegisterAlolaTopListener();

  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

private:
  ZukanViewRegisterAlolaTop* m_view;
};


//==============================================================================
//
//  ZukanViewRegisterAlolaTopListener
//
//==============================================================================
ZukanViewRegisterAlolaTopListener::ZukanViewRegisterAlolaTopListener(ZukanViewRegisterAlolaTop* view)
  : app::ui::UIInputListener(),
    m_view(view)
{}
ZukanViewRegisterAlolaTopListener::~ZukanViewRegisterAlolaTopListener()
{}

app::ui::UIInputListener::ListenerResult ZukanViewRegisterAlolaTopListener::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( pTouchPanel->IsTouchTrigger() )
  {
    //Sound::PlaySE( SEQ_SE_RELEASE );
    m_view->Req(ZukanViewRegisterAlolaTop::REQ_ID_TOUCH_DECIDE);
  }
  return app::ui::UIInputListener::DISABLE_ACCESS;
}
app::ui::UIInputListener::ListenerResult ZukanViewRegisterAlolaTopListener::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( pButton->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    //Sound::PlaySE( SEQ_SE_DECIDE1 );
    m_view->Req(ZukanViewRegisterAlolaTop::REQ_ID_KEY_DECIDE);
  }
  else if( pButton->IsTrigger(gfl2::ui::BUTTON_B) )
  {
    //Sound::PlaySE( SEQ_SE_CANCEL1 );
    m_view->Req(ZukanViewRegisterAlolaTop::REQ_ID_KEY_DECIDE);
  }
  return app::ui::UIInputListener::DISABLE_ACCESS;
}




//==============================================================================
//
//  ZukanViewRegisterAlolaTop
//
//==============================================================================
ZukanViewRegisterAlolaTop::ZukanViewRegisterAlolaTop(const ZukanSceneParam& param)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
    m_param(param),
    m_viewListener(NULL),
    m_waitAnimCount(0),
    m_dispState(DISP_STATE_OFF),
    m_dispStateCount(0),
    m_picScrollDataAccessor(NULL),
    m_picScrollDemo(NULL),
    m_viewComment(NULL),
    m_commentResultListener(NULL),
    m_registerIsBingo(false),
    m_registerLegendLevel(0)
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
    , m_prismSheet(NULL)
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
{
  // ZukanPicScrollDataAccessor
  {
    const void* buf = m_param.dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_PIC_SCROLL_DATA);
    GFL_ASSERT(buf);  // 開発中に気付かせるためのASSERT
    m_picScrollDataAccessor = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanPicScrollDataAccessor;
    m_picScrollDataAccessor->SetupData(buf);
  }

  // ZukanViewRegisterAlolaTopListener
  {
    m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewRegisterAlolaTopListener(this);
    this->SetInputListener(m_viewListener);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  }

  // UIView
  {
    m_param.parentView->AddSubView(this);  // app::ui::UIViewのメンバ
  }
 
  // レイアウト
  {
    this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ
 
    void* buf        = m_param.dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_ALOLA_TOP);
    void* grpFontBuf = m_param.dataManager->GetDataBuf(ZukanDataManager::DATA_ID_ZUKAN_GRPFONT_RES2D);  // グラフィックフォント
    if(buf && grpFontBuf)
    {
      const app::util::G2DUtil::LytResData resTbl[] =
      {
        // res_0
        {
          buf,
          LYTRES_ZUKANGRPFONT_END,  // niji_conv_header/app/common/graphic_font/zukan_grpfont/res2d/ZukanGrpFont.h
          app::util::G2DUtil::ATTACH_ROOT
        },
        // res_1
        {
          grpFontBuf,
          0,
          app::util::G2DUtil::ATTACH_RESOURCE
        },
      };
    
      const app::util::G2DUtil::LytwkData wkTbl_orderType[] =  // 下 < wk_3 < wk_1 < wk_0 < wk_2 < 上
      {
        // wk_0
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_MIN_UPP_000_BFLYT,
          LA_ALOLA_TOP_ZUKAN_MIN_UPP_000___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
        // wk_1
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_MIN_UPP_001_BFLYT,
          LA_ALOLA_TOP_ZUKAN_MIN_UPP_001___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
        // wk_2
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_MIN_UPP_002_BFLYT,
          LA_ALOLA_TOP_ZUKAN_MIN_UPP_002___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
        // wk_3
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_MIN_UPP_003_BFLYT,
          LA_ALOLA_TOP_ZUKAN_MIN_UPP_003___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
        // wk_4
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_ZUKAN_MIN_UPP_004_BFLYT,
          LA_ALOLA_TOP_ZUKAN_MIN_UPP_004___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
        // wk_5
        {
          // 上画面
          0,  // res_0
          LYTRES_ALOLA_TOP_PRISM_SHEET_BFLYT,
          LA_ALOLA_TOP_PRISM_SHEET___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
      };

      this->Create2D(  // 親であるapp::ui::UIViewのメンバ
          m_param.appHeap,
          m_param.lytSys,
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
          6,  // wk_0, wk_1, wk_2, wk_3, wk_4, wk_5の6個
#else  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABL
          5,  // wk_0, wk_1, wk_2, wk_3, wk_4の5個
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
          resTbl,
          GFL_NELEMS(resTbl), 
          wkTbl_orderType,
          GFL_NELEMS(wkTbl_orderType), 
          app::ui::UIView::UNUSE_MESSAGE_DATA
      );
  
      
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
      // キラカード
      {
        m_prismSheet = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanPrismSheet(
          m_param.appHeap,
          this->GetLayoutWork( 5 ),  // wk_5  // 親であるapp::ui::UIViewのメンバ関数
          5  // wk_5
        );
      }
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE

      // ZukanPicScrollDemo
      m_picScrollDemo = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanPicScrollDemo(
          this->GetG2DUtil(),  // 親であるapp::ui::UIViewのメンバ
          this->GetG2DUtil()->GetLayoutResourceID( 0 ),  // res_0
          this->GetLayoutWork( 4 ),  // wk_4  // 親であるapp::ui::UIViewのメンバ関数
          this->GetLayoutWork( 1 ),  // wk_1  // 親であるapp::ui::UIViewのメンバ関数
          this->GetLayoutWork( 3 ),  // wk_3  // 親であるapp::ui::UIViewのメンバ関数
          this->GetLayoutWork( 0 ),  // wk_0  // 親であるapp::ui::UIViewのメンバ関数
          this->GetLayoutWork( 2 ),  // wk_2  // 親であるapp::ui::UIViewのメンバ関数
          4,  // wk_4
          1,  // wk_1  
          3,  // wk_3
          0,  // wk_0  
          2,  // wk_2  
          m_picScrollDataAccessor,
          m_param.contents,
          m_param.pokePicManager,
          m_param.tool->GetPokeExtendData(),
          m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN),
          m_param.tool->GetWordSet(),
          &m_registerIsBingo,
          &m_registerLegendLevel
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
          , m_prismSheet
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
      );

      PokeTool::SimpleParam sp;
      PokeTool::GetSimpleParam(&sp, m_param.param->zukanRegisterEventParam->pp);

      m_picScrollDemo->Setup(sp);

      // インアニメはm_picScrollDemoの準備ができてから行う
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。データDATA_ID_ZUKAN_RES2Dの読み込みが終わってからフレームをつくるように呼び出し元でしているので。
    }
  }

  ZukanUtil::SetPokeModelVisible(m_param.tool, false);  
}
ZukanViewRegisterAlolaTop::~ZukanViewRegisterAlolaTop()
{
  // コメント層のビュー
  this->destroyComment();

#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  // キラカード
  {
    GFL_SAFE_DELETE(m_prismSheet);
  }
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE

  // UIView
  this->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ

  // レイアウト
  this->Delete2D();  // 親であるapp::ui::UIViewのメンバ

  // ZukanViewRegisterAlolaTopListener
  GFL_SAFE_DELETE(m_viewListener);

  // ZukanPicScrollDemo
  GFL_SAFE_DELETE(m_picScrollDemo);
  // ZukanPicScrollDataAccessor
  GFL_SAFE_DELETE(m_picScrollDataAccessor);
}

void ZukanViewRegisterAlolaTop::Update(void)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  //gfl2::lyt::LytWk*        lytWk_0  = this->GetLayoutWork( 0 );  // wk_0  // 親であるapp::ui::UIViewのメンバ関数
  //gfl2::lyt::LytWk*        lytWk_1  = this->GetLayoutWork( 1 );  // wk_1  // 親であるapp::ui::UIViewのメンバ関数
  //gfl2::lyt::LytWk*        lytWk_2  = this->GetLayoutWork( 2 );  // wk_2  // 親であるapp::ui::UIViewのメンバ関数
  //gfl2::lyt::LytWk*        lytWk_3  = this->GetLayoutWork( 3 );  // wk_3  // 親であるapp::ui::UIViewのメンバ関数
  //gfl2::lyt::LytWk*        lytWk_4  = this->GetLayoutWork( 4 );  // wk_4  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytMultiResID lytRes_0 = pG2DUtil->GetLayoutResourceID( 0 );  // res_0

  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
  
  // 表示状態
  switch(m_dispState)
  {
  case DISP_STATE_OFF:
    {
      if(m_picScrollDemo->IsReady())
      {
        // インアニメ
        this->startInSeq();
      }
      break;
    }
  case DISP_STATE_ON_IN:
    {
      if(    pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE)
          && m_param.tool->GetViewRegisterFade()->IsOutFinished() )
      {
        m_picScrollDemo->Start();
        this->changeDispState(DISP_STATE_ON_WORK);
      }
      break;
    }
  case DISP_STATE_ON_WORK:
    {
      break;
    }
  case DISP_STATE_ON_COMMENT_WORK:
    {
      break;
    }
  case DISP_STATE_ON_OUT:
    {
      //++m_waitAnimCount;
      //if( m_waitAnimCount >= 1 )
      if( pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE) )
      {
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
        // キラカード
        {
          if( m_prismSheet->CanDestroy() )
          {
            m_prismSheet->SetVisible(false);
          }
          else
          {
            break;
          }
        }
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
        this->changeDispState(DISP_STATE_WAIT_SAFETY);
      }
      break;
    }
  case DISP_STATE_WAIT_SAFETY:
    {
      m_param.frameCell->Req(ZukanFrameRegisterAlolaTopCell::REQ_ID_END);
      this->changeDispState(DISP_STATE_WAIT_END);
      //breakしない
    }
  case DISP_STATE_WAIT_END:
    {
      break;
    }
  }

  m_picScrollDemo->Update();
  app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  // キラカード
  m_prismSheet->Update();
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
}
void ZukanViewRegisterAlolaTop::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 4, gfl2::lyt::ORDER_NORMAL, 1 );  // wk_4
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 1, gfl2::lyt::ORDER_NORMAL, 1 );  // wk_1
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 3, gfl2::lyt::ORDER_NORMAL, 1 );  // wk_3
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  // キラカード
  {
    pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 5, gfl2::lyt::ORDER_NORMAL, 1 );  // wk_5
  }
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 0, gfl2::lyt::ORDER_NORMAL, 1 );  // wk_0
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 2, gfl2::lyt::ORDER_NORMAL, 1 );  // wk_2
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  // キラカード
  {
    //描画順変更pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 5, gfl2::lyt::ORDER_NORMAL, 1 );  // wk_5
  }
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
}

void ZukanViewRegisterAlolaTop::Req(ReqId reqId, s32 reqParam)
{
  switch(reqId)
  {
  case REQ_ID_TOUCH_DECIDE :
  case REQ_ID_KEY_DECIDE   :
    {
      if( m_picScrollDemo->IsEnd() )
      {
        if(m_dispState == DISP_STATE_ON_WORK)
        {
          Sound::PlaySE( SEQ_SE_DECIDE1 );
          if(m_param.contents->DoesRotomAppear())
          {
            // ロトムがいるときはコメント表示
            this->createComment();
            this->changeDispState(DISP_STATE_ON_COMMENT_WORK);
          }
          else
          {
            // ロトムがいないときは詳細画面へ
            *m_param.nextSceneType = ZukanType::SCENE_TYPE_REGISTER;
            this->startOutSeq();
          }
        }
      }
      break;
    }
  case REQ_ID_OUT   :
    {
      if(m_dispState == DISP_STATE_ON_COMMENT_WORK)
      {
        // 詳細画面へ
        *m_param.nextSceneType = ZukanType::SCENE_TYPE_REGISTER;
        this->startOutSeq();
      }
      break;
    }
  }
}

void ZukanViewRegisterAlolaTop::startInSeq(void)
{
  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
#if NIJI_APP_ZUKAN_REGISTER_INTRO_FADE==NIJI_APP_ZUKAN_REGISTER_INTRO_FADE_CROSS
  pFadeManager->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_CROSS);
#elif NIJI_APP_ZUKAN_REGISTER_INTRO_FADE==NIJI_APP_ZUKAN_REGISTER_INTRO_FADE_ALPHA
  pFadeManager->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA);
#endif
  m_param.tool->GetViewRegisterFade()->Req(ZukanViewRegisterFade::REQ_ID_OUT);

  m_waitAnimCount = 0; 
  this->changeDispState(DISP_STATE_ON_IN);
}
void ZukanViewRegisterAlolaTop::startOutSeq(void)
{
  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
  pFadeManager->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_CROSS, &ZukanType::CROSS_FADE_COLOR_START, &ZukanType::CROSS_FADE_COLOR_END, 1);  // ぱっと消す
  
  m_waitAnimCount = 0; 
  this->changeDispState(DISP_STATE_ON_OUT);
}

void ZukanViewRegisterAlolaTop::changeDispState(DispState dispState)
{
  m_dispState = dispState;
  m_dispStateCount = 0;
}

void ZukanViewRegisterAlolaTop::createComment(void)
{
  if(m_viewComment == NULL)
  {
    m_commentResultListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewRegisterAlolaTopCommentResultListener(this);
  
    ZukanSceneParam commentParam = m_param;
    commentParam.parentView = this;
    m_viewComment = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewRegisterComment(commentParam, m_registerIsBingo, m_registerLegendLevel);

    m_viewComment->SetResultListener(m_commentResultListener);
  }
}
void ZukanViewRegisterAlolaTop::destroyComment(void)
{
  GFL_SAFE_DELETE(m_viewComment);
  GFL_SAFE_DELETE(m_commentResultListener);
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

