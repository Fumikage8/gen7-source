//======================================================================
/**
 * @file	KawaigariDebugFont.C++
 * @brief	Kawaigari Debug Font Code
 * @author	PETE
 * @data	2016.04.26
 */
//======================================================================
#if PM_DEBUG 

#include "../KawaigariMain.h"
#include "System/include/GflUse.h"

// garc FileLoading
#include <arc_def_index/arc_def.h>

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)


KawaigariDebugFont::KawaigariDebugFont(void)
{      
  mRenderingPipelineText = NULL;
  mFontResource = NULL;
  mDrawUtilText = NULL;   
  mEnable = false;  
  mInitSeq = 0;
}

KawaigariDebugFont::~KawaigariDebugFont(void)
{    
}




bool KawaigariDebugFont::Finalize(void)
{
   if(!mEnable) return(true);

  // font
  {
    mDrawUtilText->Terminate();

    GFL_SAFE_DELETE( mDrawUtilText );
    void* pFontBuffer = mFontResource->RemoveResource();
    GflHeapFreeMemory( pFontBuffer );
    GFL_SAFE_DELETE( mFontResource );

    GFL_SAFE_DELETE( mRenderingPipelineText );

    GFL_SAFE_DELETE( mGLAllocator );
    GFL_SAFE_DELETE( mDevAllocator );
    GFL_SAFE_DELETE( mMemAllocator );
  }

  mEnable = false;
  return(true);
}

void KawaigariDebugFont::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  if(!mEnable) return;

  switch( displayNo )
  {
    case gfl2::gfx::CtrDisplayNo::LEFT:			
    {
      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      mRenderingPipelineText->SetRenderTarget( pBackBuffer );
      mRenderingPipelineText->SetDepthStencil( pDepthBuffer );
      mRenderingPipelineText->SetDisplaySize( displayWidth, displayHeight );

      mRenderingPipelineText->Execute();
    }
    break;
  }	

}

bool KawaigariDebugFont::Initialize(void)
{
  if(coreSys==NULL) return(false);

  switch(mInitSeq)
  {
    case 0: // Init Font:
      // font
      {
        gfl2::heap::HeapBase* pMemHeap = coreSys->GetHeap();
        gfl2::heap::HeapBase* pDevHeap = coreSys->GetHeap();

        mMemAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pMemHeap );
        mDevAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pDevHeap );
        mGLAllocator = GFL_NEW(pMemHeap) System::nijiAllocator(pDevHeap);        		

        mRenderingPipelineText = GFL_NEW( coreSys->GetHeap() ) gfl2::util::UtilTextRenderingPipeLine(mGLAllocator, 512, 256 );
        mFontResource = gfl2::str::CreateResFont( "data/app/test/fukushima_test/layout_sample_data/fonts/cbf_std_kujira.bffnt", pMemHeap, pDevHeap, pMemHeap );
        mDrawUtilText = GFL_NEW( pMemHeap ) gfl2::util::DrawUtilText(mMemAllocator, mDevAllocator);
        mDrawUtilText->Initialize( mMemAllocator, mDevAllocator, mRenderingPipelineText->GetUtilTextRenderPath() );
        mDrawUtilText->SetFont( mFontResource );
      }

      mInitSeq++;
      mEnable = true;
      return(true);      

    case 1:
      return(true);
  }

  return(false);
}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // PM_DEBUG

