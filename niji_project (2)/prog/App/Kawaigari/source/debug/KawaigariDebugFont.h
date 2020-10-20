//======================================================================
/**
 * @file	KawaigariDebugFont.h
 * @brief	Kawaigari Debug Font
 * @author	PETE
 * @data	2016.04.26
 */
//======================================================================

#if PM_DEBUG 

#if !defined __KAWAIGARI_DEBUGFONT_H_INCLUDED__
#define __KAWAIGARI_DEBUGFONT_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <Math/include/gfl2_Matrix.h>
#include <Math/include/gfl2_Quaternion.h>

#include <util/include/gfl2_GLHeapAllocator.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <fs/include/gfl2_Fs.h>

// Text
#include <str/include/gfl2_Font.h>
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

class KawaigariMain;

class KawaigariDebugFont
{

public:
  KawaigariDebugFont(void);
  ~KawaigariDebugFont(void);
  
  bool Finalize(void);
  bool Initialize(void);
  void SetCoreSys(KawaigariMain *core) { coreSys = core; }  
  void Draw(gfl2::gfx::CtrDisplayNo displayNo);  

  gfl2::util::DrawUtilText *TextUtil(void) { return(mDrawUtilText); }

private:
  bool mEnable;
  int mInitSeq;
  KawaigariMain *coreSys;  
  
  // Text Rendering:
  gfl2::heap::NwAllocator *mMemAllocator;
  gfl2::heap::NwAllocator *mDevAllocator;
  System::nijiAllocator *mGLAllocator;
  gfl2::util::UtilTextRenderingPipeLine *mRenderingPipelineText;
  gfl2::str::ResFont *mFontResource;
  gfl2::util::DrawUtilText *mDrawUtilText;
        
};

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // __KAWAIGARI_DEBUGFONT_H_INCLUDED__

#endif // PM_DEBUG
