#ifndef GFLIB2_GRX_WINDX11_WINDX11CONSTANTBUFFER_H_INCLUDED
#define GFLIB2_GRX_WINDX11_WINDX11CONSTANTBUFFER_H_INCLUDED

#include <gfx/include/gfl2_Buffer.h>
#include <gfx/include/WinDX11/gfl2_WinDX11Types.h>

namespace gfl2 { namespace gfx { namespace windx11 {

class WinDX11GL;

class WinDX11ConstantBuffer : public ConstantBuffer
{
public:
	friend class WinDX11GL;

	WinDX11ConstantBuffer(const ConstantBufferElement* pConstantBufferElements, u32 usage, Pool pool, const void* buffer);
	virtual ~WinDX11ConstantBuffer();

	virtual void* Lock();
	virtual void UnLock();
	virtual void Update( const void* pData, u32 size );

private:
	AutoComPtr<ID3D11Buffer>		m_pConstantBuffer;
	D3D11_BUFFER_DESC						m_BufferDesc;
};

}}}

#endif