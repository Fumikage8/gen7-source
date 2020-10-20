#include <gfx/include/WinDX11/gfl2_WinDx11Types.h>
#include <gfx/include/WinDX11/gfl2_WinDX11QueryObject.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace gfx { namespace windx11 {

WinDX11QueryObject::WinDX11QueryObject() : QueryObject()
{
	D3D11_QUERY_DESC qdesc;

  qdesc.MiscFlags = D3D11_QUERY_MISC_PREDICATEHINT;
  qdesc.Query = D3D11_QUERY_OCCLUSION_PREDICATE;

	HRESULT			hr = GFGL::GetDevice()->CreatePredicate( &qdesc, &m_pPredicate );

	if (FAILED(hr))
		assert(0);
}

WinDX11QueryObject::~WinDX11QueryObject()
{
	m_pPredicate.reset();
}

}}}
