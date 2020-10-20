//=============================================================================================
/**
 * @file   FadeRenderingManager.cpp
 * @brief  フェード用（エラー画面もここがいいはず）のパイプラインを管理するクラス
           生存期間がゲーム中になるので実装には気をつけ、きちんと開放されるか考えて組みましょう
 * @author k.ohno
 * @date   2015.4.25
 */
//=============================================================================================


#include <AppLib/include/Fade/FadeRenderingManager.h>
#include <layout/include/gfl2_LytSys.h>
#include <system/include/GflUse.h>

GFL_NAMESPACE_BEGIN(AppLib)
GFL_NAMESPACE_BEGIN(Fade)



void FadeRenderingManager::Init(app::util::Heap* pHeap)
{

  mpUpperLeft = GFL_NEW(pHeap->GetSystemHeap()) LayoutRenderingPipeLine( pHeap->GetDeviceHeap(), 512, 256 );
  mpUpperRight = GFL_NEW(pHeap->GetSystemHeap()) LayoutRenderingPipeLine( pHeap->GetDeviceHeap(), 512, 256 );
  mpLower = GFL_NEW(pHeap->GetSystemHeap()) LayoutRenderingPipeLine( pHeap->GetDeviceHeap(), 512, 256 );


}
	
void FadeRenderingManager::End(void)
{
  GFL_DELETE mpLower;
  GFL_DELETE mpUpperRight;
  GFL_DELETE mpUpperLeft;
}





void FadeRenderingManager::AddDrawableLytWk(gfl2::lyt::DisplayType dtype, gfl2::lyt::LytWk* pLytWk)
{
  if(gfl2::lyt::DISPLAY_LOWER != dtype){
    mpUpperLeft->GetLayoutRenderPath()->AddDrawableLytWk(pLytWk,gfl2::lyt::DISPLAY_UPPER) ;
    mpUpperRight->GetLayoutRenderPath()->AddDrawableLytWk(pLytWk,gfl2::lyt::DISPLAY_UPPER_RIGHT) ;
  }
  else{
    mpLower->GetLayoutRenderPath()->AddDrawableLytWk(pLytWk,gfl2::lyt::DISPLAY_LOWER);
  }
}

void FadeRenderingManager::Update()
{
   mpUpperLeft->GetLayoutRenderPath()->ClearDrawableLytWk();
   mpUpperRight->GetLayoutRenderPath()->ClearDrawableLytWk();
   mpLower->GetLayoutRenderPath()->ClearDrawableLytWk();

}

void FadeRenderingManager::PreDraw()
{
}


void FadeRenderingManager::SingleExecute(LayoutRenderingPipeLine* pLayout,gfl2::gfx::DisplayDeviceHandle handle)
{
  gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( handle );
  gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( handle );
  u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( handle );
  u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( handle );
  pLayout->SetRenderTarget( pBackBuffer );
  pLayout->SetDepthStencil( pDepthBuffer );
  pLayout->SetDisplaySize( displayWidth, displayHeight );
  pLayout->Execute();
}



void FadeRenderingManager::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo ){
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      SingleExecute(mpUpperLeft, System::GflUse::m_HandleList[ displayNo ]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      SingleExecute(mpUpperRight, System::GflUse::m_HandleList[ displayNo ]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      SingleExecute(mpLower, System::GflUse::m_HandleList[ displayNo ]);
    }
    break;
  }
}


GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_END(AppLib)
