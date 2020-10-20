//==============================================================================
/**
 @file    ZukanViewBg.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の背景のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <Sound/include/Sound.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/bg.h>
#include <niji_conv_header/app/zukan/bg_pane.h>
#include <niji_conv_header/app/zukan/bg_anim_list.h>


// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanDataManager.h"

#include "ZukanViewBg.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewBgListener
 @brief     図鑑の背景のビューのリスナー
 */
//==============================================================================
class ZukanViewBgListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewBgListener);

public:
  ZukanViewBgListener(ZukanViewBg* view);
  virtual ~ZukanViewBgListener();

public:
  ZukanViewBg* m_view;
};


//==============================================================================
//
//  ZukanViewBgListener
//
//==============================================================================
ZukanViewBgListener::ZukanViewBgListener(ZukanViewBg* view)
  : app::ui::UIInputListener(),
    m_view(view)
{}
ZukanViewBgListener::~ZukanViewBgListener()
{}




//==============================================================================
//
//  ZukanViewBg
//
//==============================================================================
ZukanViewBg::ZukanViewBg(const Param& param)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
    m_param(param),
    m_viewListener(NULL)
{
  // ZukanViewBgListener
  {
    m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewBgListener(this);
    this->SetInputListener(m_viewListener);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  }

  // レイアウト
  {
    this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ
 
    void* buf = m_param.dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_BG);
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
          LYTRES_BG_ZUKAN_BGR_UPP_000_BFLYT,
          LA_BG_ZUKAN_BGR_UPP_000___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },

        // wk_1
        {
          // 下画面
          0,  // res_0
          LYTRES_BG_ZUKAN_BGR_LOW_000_BFLYT,
          LA_BG_ZUKAN_BGR_LOW_000___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_LOWER,
          true
        },
      };
     
      this->Create2D(  // 親であるapp::ui::UIViewのメンバ
          m_param.appHeap,
          m_param.lytSys,
          2,  // wk_0, wk_1の2個だけ 
          resTbl,
          GFL_NELEMS(resTbl), 
          wkTbl,
          GFL_NELEMS(wkTbl), 
          app::ui::UIView::UNUSE_MESSAGE_DATA
      );
     
      app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
      pG2DUtil->StartAnime(
          0,  // wk_0
          LA_BG_ZUKAN_BGR_UPP_000_LOOP
      );
      pG2DUtil->StartAnime(
          1,  // wk_1
          LA_BG_ZUKAN_BGR_LOW_000_LOOP
      );
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。データDATA_ID_ZUKAN_RES2Dの読み込みが終わってからフレームをつくるように呼び出し元でしているので。
    }
  }

  // UIView
  {
    m_param.parentView->AddSubView(this);  // app::ui::UIViewのメンバ
  }
}
ZukanViewBg::~ZukanViewBg()
{
  // UIView
  this->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ

  // レイアウト
  this->Delete2D();  // 親であるapp::ui::UIViewのメンバ

  // ZukanViewBgListener
  GFL_SAFE_DELETE(m_viewListener);
}

void ZukanViewBg::Update(void)
{
  app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ
}
void ZukanViewBg::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 0, gfl2::lyt::ORDER_NORMAL, 0 );  // wk_0
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 1, gfl2::lyt::ORDER_NORMAL, 0 );  // wk_1
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

