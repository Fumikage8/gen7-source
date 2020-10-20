#include <string.h>
//#include <memory.h>
#include <RenderingEngine/include/SceneGraph/gfl2_NodeTrait.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph {

Inheritance Inheritance::Generate( const Inheritance* basetype, util::FnvHash thistTypeValue )
{
	//! @note デバッグビルド時は、ハッシュの重複チェックをここで行いたい。
	Inheritance inh;

	memset(&inh,0,sizeof(inh));

	inh.m_ThistTypeValue = (thistTypeValue);
	inh.m_pBaseType = basetype;

	return inh;
}

}}}
