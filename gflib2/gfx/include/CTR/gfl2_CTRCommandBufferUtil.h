#if !defined(GFLIB2_GFX_CTR_CTRCOMMANDBUFFERUTIL_H_INCLUDED)
#define GFLIB2_GFX_CTR_CTRCOMMANDBUFFERUTIL_H_INCLUDED
#pragma once

#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace gfx { namespace ctr {

class CTRCommandBufferUtil
{
public:
  //! @brief  コマンドバッファを入れ替えて、フロントバッファにジャンプするようにする。
  template<typename ManagerType>
  static void SwapCommandBufferAndJumpToFrontBuffer(ManagerType *commandBufferManager)
  {
    if(commandBufferManager)
    {
      commandBufferManager->SwapCommandBuffer();
      if( commandBufferManager->IsFrontBufferUsed() )
      {
        GFGL::AddCommandJump( commandBufferManager->GetStartPtrOfFrontBuffer(), static_cast<u32>( commandBufferManager->GetSizeOfFrontBuffer() ) );
      }
    }
  }
};

}}}

#endif
