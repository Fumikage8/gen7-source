#ifndef GFLIB2_GRX_WINDX11_WINDX11QUERYOBJECT_H_INCLUDED
#define GFLIB2_GRX_WINDX11_WINDX11QUERYOBJECT_H_INCLUDED

#include <gfx/include/gfl2_QueryObject.h>
#include <util/include/gfl2_AutoComPtr.h>
#include <gfx/include/WinDX11/gfl2_WinDX11Types.h>

namespace gfl2 { namespace gfx { namespace windx11 {

class WinDX11QueryObject : public QueryObject
{
public:

	WinDX11QueryObject();
	virtual ~WinDX11QueryObject();

	inline AutoComPtr<ID3D11Predicate>& GetPredicate(){ return m_pPredicate; };

private:
	AutoComPtr<ID3D11Predicate>					m_pPredicate;
};

}}}

#endif