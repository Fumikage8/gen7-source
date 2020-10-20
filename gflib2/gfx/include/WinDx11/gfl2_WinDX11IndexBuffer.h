#ifndef GFLIB2_GRX_WINDX11_WINDX11INDEXBUFFER_H_INCLUDED
#define GFLIB2_GRX_WINDX11_WINDX11INDEXBUFFER_H_INCLUDED

#include <gfx/include/gfl2_Buffer.h>
#include <gfx/include/WinDX11/gfl2_WinDX11Types.h>

namespace gfl2 { namespace gfx { namespace windx11 {

class WinDX11GL;

class WinDX11IndexBuffer : public IndexBuffer
{
public:
	friend class WinDX11GL;

	WinDX11IndexBuffer(u32 size, u32 usage, Pool pool, IndexType type, const void* buffer);
	virtual ~WinDX11IndexBuffer();

	virtual void* Lock();
	virtual void UnLock();
	virtual void Update( const void* pData, u32 size );

	ID3D11Buffer* GetBuffer() const { return m_pIndexBuffer.get(); }
  const D3D11_BUFFER_DESC& GetBufferDesc() const { return m_BufferDesc; }

private:
	AutoComPtr<ID3D11Buffer>  m_pIndexBuffer;
	D3D11_BUFFER_DESC					m_BufferDesc;
};

}}}

#endif