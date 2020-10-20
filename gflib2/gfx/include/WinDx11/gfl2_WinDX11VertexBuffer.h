#ifndef GFLIB2_GRX_WINDX11_WINDX11VERTEXBUFFER_H_INCLUDED
#define GFLIB2_GRX_WINDX11_WINDX11VERTEXBUFFER_H_INCLUDED

#include <gfx/include/gfl2_Buffer.h>
#include <gfx/include/WinDX11/gfl2_WinDX11Types.h>

namespace gfl2 { namespace gfx { namespace windx11 {

class WinDX11GL;

class WinDX11VertexBuffer : public VertexBuffer
{
public:
	friend class WinDX11GL;

	WinDX11VertexBuffer(u32 size, u32 vertex_num, u32 usage, Pool pool, const void* buffer);
	virtual ~WinDX11VertexBuffer();

	virtual void* Lock();
	virtual void UnLock();
	virtual void Update( const void* pData, u32 size );

  ID3D11Buffer* GetBuffer() const { return m_pVerBuffer.get(); }
  const D3D11_BUFFER_DESC& GetBufferDesc() const { return m_BufferDesc; }

private:
	AutoComPtr<ID3D11Buffer>  m_pVerBuffer;
	D3D11_BUFFER_DESC					m_BufferDesc;
	u32												m_VertexNum;
	
};

}}}

#endif