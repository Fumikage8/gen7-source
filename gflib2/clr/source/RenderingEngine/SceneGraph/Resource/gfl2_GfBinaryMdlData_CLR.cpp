#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_GfBinaryMdlData_CLR.h>

using System::IntPtr;
using System::Runtime::InteropServices::Marshal;

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace resource {

GfBinaryMdlData::GfBinaryMdlData(array<u8>^ pData)
{
  m_pData = new u8[pData->Length];
  // pin_ptrでmemcpyしたら怒られたのでとりあえずループ回してコピーする
  for (s32 i = 0; i < pData->Length; ++i)
  {
    m_pData[i] = pData[i];
  }
  m_pNative = new gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData();
  m_pNative->SetModelData(reinterpret_cast<c8*>(m_pData));
}

GfBinaryMdlData::!GfBinaryMdlData()
{
  if (m_pNative)
  {
    delete m_pNative;
    m_pNative = 0;
  }

  if (m_pData)
  {
    delete m_pData;
    m_pData = 0;
  }
}

}}}}}
