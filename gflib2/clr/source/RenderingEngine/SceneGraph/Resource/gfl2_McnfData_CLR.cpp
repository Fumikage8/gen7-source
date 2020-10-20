#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_McnfData_CLR.h>
#include <clr/include/util/gfl2_String_CLR.h>

using System::IntPtr;
using System::Runtime::InteropServices::Marshal;

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace resource {

McnfData::McnfData(array<c8>^ pData)
{
	pin_ptr<c8> pSrc = &pData[0];
	m_pNative = new gfl2::renderingengine::scenegraph::resource::McnfData(pSrc);
}

McnfData::McnfData( System::String^ filePath )
{
	std::string str = gfl2::clr::util::String::GetNativeString(filePath);
	m_pNative = new gfl2::renderingengine::scenegraph::resource::McnfData(str);
}

McnfData::!McnfData()
{
	if ( m_pNative)
	{
		delete m_pNative;
		m_pNative = 0;
	}
}

}}}}}
