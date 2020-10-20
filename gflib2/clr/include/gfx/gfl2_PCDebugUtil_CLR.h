#ifndef GFLIB2_GRX_CLR_PCDEBUGUTIL_CLR_H_INCLUDED
#define GFLIB2_GRX_CLR_PCDEBUGUTIL_CLR_H_INCLUDED

#using <System.Drawing.dll>

#include <clr/include/gfx/gfl2_GFGL_CLR.h>
#include <gfx/include/gfl2_PCDebugUtil.h>

namespace gfl2 { namespace clr { namespace gfx {

  public ref class PCDebugUtil
  {
  public:
    static void PrintScreen(System::String^ fileName, gfl2::clr::gfx::DisplayDeviceHandle^ hDisplay);
    static void PrintScreenAsTga(System::String^ fileName, gfl2::clr::gfx::DisplayDeviceHandle^ hDisplay);
  };

}}}

#endif
