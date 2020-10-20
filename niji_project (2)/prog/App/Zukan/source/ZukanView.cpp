//==============================================================================
/**
 @file    ZukanView.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.10
 @brief   図鑑のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>

// 図鑑のインクルード
#include "ZukanView.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
//
//  ZukanView
//
//==============================================================================
ZukanView::ZukanView(app::util::Heap* appHeap)
  : app::ui::UIView( appHeap, appHeap->GetDeviceAllocator(), appHeap->GetDeviceAllocator() )
{
  // レイアウト
  this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ
  this->CreateGraphicsSystem( appHeap, false );  // 親であるapp::ui::UIViewのメンバ
}
ZukanView::~ZukanView()
{
  // レイアウト
  this->DeleteGraphicsSystem();  // 親であるapp::ui::UIViewのメンバ
}

void ZukanView::Update(void)
{
  app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ
}
void ZukanView::Draw(gfl2::gfx::CtrDisplayNo /*displayNo*/)
{}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

