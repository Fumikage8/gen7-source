//==============================================================================
/**
 @file    ZukanViewRegisterFade.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.02.13
 @brief   図鑑の登録画面に出すフェード層のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_2d.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/register_fade.h>
#include <niji_conv_header/app/zukan/register_fade_pane.h>
#include <niji_conv_header/app/zukan/register_fade_anim_list.h>

// 図鑑のインクルード
#include "ZukanDataManager.h"

#include "ZukanViewRegisterFade.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewRegisterFadeListener
 @brief     図鑑の登録画面に出すフェード層のビューのリスナー
 */
//==============================================================================
class ZukanViewRegisterFadeListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewRegisterFadeListener);

public:
  ZukanViewRegisterFadeListener(ZukanViewRegisterFade* view);
  virtual ~ZukanViewRegisterFadeListener();

public:
  ZukanViewRegisterFade* m_view;
};


//==============================================================================
//
//  ZukanViewRegisterFadeListener
//
//==============================================================================
ZukanViewRegisterFadeListener::ZukanViewRegisterFadeListener(ZukanViewRegisterFade* view)
  : app::ui::UIInputListener(),
    m_view(view)
{}
ZukanViewRegisterFadeListener::~ZukanViewRegisterFadeListener()
{}




//==============================================================================
//
//  ZukanViewRegisterFade
//
//==============================================================================
ZukanViewRegisterFade::ZukanViewRegisterFade(const Param& param)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
    m_param(param),
    m_viewListener(NULL),
    m_dispState(DISP_STATE_OFF),
    m_dispStateCount(0)
{
  // 初期化
  {
    this->setupAnimId();
  }

  // ZukanViewRegisterFadeListener
  {
    m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewRegisterFadeListener(this);
    this->SetInputListener(m_viewListener);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  }

  // レイアウト
  {
    this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ
 
    void* buf = m_param.dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_REGISTER_FADE);
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
          LYTRES_REGISTER_FADE_ZUKAN_FAD_UPP_000_BFLYT,
          LA_REGISTER_FADE_ZUKAN_FAD_UPP_000___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
      };
     
      this->Create2D(  // 親であるapp::ui::UIViewのメンバ
          m_param.appHeap,
          m_param.lytSys,
          1,  // wk_0の1個
          resTbl,
          GFL_NELEMS(resTbl), 
          wkTbl,
          GFL_NELEMS(wkTbl), 
          app::ui::UIView::UNUSE_MESSAGE_DATA
      );
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。データDATA_ID_ZUKAN_RES2Dの読み込みが終わってからフレームをつくるように呼び出し元でしているので。
    }
  }

  // UIView
  {
    //m_param.parentView->AddSubView(this);  // app::ui::UIViewのメンバ
    // 子にしないことにしたのでコメントアウト
  }
}
ZukanViewRegisterFade::~ZukanViewRegisterFade()
{
  // UIView
  //this->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ
  // 子にしないことにしたのでコメントアウト

  // レイアウト
  this->Delete2D();  // 親であるapp::ui::UIViewのメンバ

  // ZukanViewRegisterFadeListener
  GFL_SAFE_DELETE(m_viewListener);
}

void ZukanViewRegisterFade::Update(void)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ

  switch(m_dispState)
  {
  case DISP_STATE_OFF:
    {
      // 何もしない
      break;
    }
  case DISP_STATE_ON_IN:
    {
      bool animEnd = true;
      for(u32 i=0; i<LYT_WK_ID_NUM; ++i)
      {
        if( !pG2DUtil->IsAnimeEnd(i, m_animId[i][ANIM_ID_OUT]) )  // wk_?
        {
          animEnd = false;
        }
      }
      if( animEnd )
      {
        this->changeDispState(DISP_STATE_ON_WORK);
      }
      break;
    }
  case DISP_STATE_ON_WORK:
    {
      // 何もしない
      break;
    }
  case DISP_STATE_ON_OUT:
    {
      bool animEnd = true;
      for(u32 i=0; i<LYT_WK_ID_NUM; ++i)
      {
        if( !pG2DUtil->IsAnimeEnd(i, m_animId[i][ANIM_ID_IN]) )  // wk_?
        {
          animEnd = false;
        }
      }
      if( animEnd )
      {
        this->changeDispState(DISP_STATE_OFF);
      }
      break;
    }
  }

  app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ
}
void ZukanViewRegisterFade::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_UPPER, gfl2::lyt::ORDER_NORMAL, 2 );  // wk_?
}

void ZukanViewRegisterFade::Req(s32 reqId)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  switch(reqId)
  {
  case REQ_ID_IN:
    {
      for(u32 i=0; i<LYT_WK_ID_NUM; ++i)
      {
        pG2DUtil->StartAnime( i, m_animId[i][ANIM_ID_OUT] );  // wk_?
      }
      this->changeDispState(DISP_STATE_ON_IN);
      break;
    }
  case REQ_ID_OUT:
    {
      for(u32 i=0; i<LYT_WK_ID_NUM; ++i)
      {
        pG2DUtil->StartAnime( i, m_animId[i][ANIM_ID_IN] );  // wk_?
      }
      this->changeDispState(DISP_STATE_ON_OUT);
      break;
    }
  }
}

void ZukanViewRegisterFade::changeDispState(DispState dispState)
{
  m_dispState = dispState;
  m_dispStateCount = 0;
}

void ZukanViewRegisterFade::setupAnimId(void)
{
  static const u32 ANIM_ID[LYT_WK_ID_NUM][ANIM_ID_NUM] =
  {
    {
      LA_REGISTER_FADE_ZUKAN_FAD_UPP_000_FADE_IN,
      LA_REGISTER_FADE_ZUKAN_FAD_UPP_000_FADE_OUT,
    },
  };

  if(m_dispState == DISP_STATE_OFF)
  {
    for(u32 j=0; j<LYT_WK_ID_NUM; ++j)
    {
      for(u32 i=0; i<ANIM_ID_NUM; ++i)
      {
        m_animId[j][i] = ANIM_ID[j][i];
      }
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるASSERT。モード変更は画面に表示していないときしか受け付けない。
  }
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

