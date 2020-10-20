#include <gfx/include/WinDX11/gfl2_WinDx11Types.h>
#include <gfx/include/WinDX11/gfl2_WinDX11VertexBuffer.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace gfx { namespace windx11 {

WinDX11VertexBuffer::WinDX11VertexBuffer(u32 size, u32 vertex_num, u32 usage, Pool pool, const void* buffer) : VertexBuffer()
{
	HRESULT								hr;

	switch( usage ){
	case Usage::WriteOnly:
		m_BufferDesc.Usage          = D3D11_USAGE_DEFAULT;
		m_BufferDesc.CPUAccessFlags = 0;
		break;
	default:
		m_BufferDesc.Usage          = D3D11_USAGE_DYNAMIC;
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;
	}

	m_BufferDesc.ByteWidth      = size * vertex_num;
	m_BufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER; // 頂点バッファ
	m_BufferDesc.MiscFlags      = 0;
	m_BufferDesc.StructureByteStride = 0;
	m_VertexNum = vertex_num;

	D3D11_SUBRESOURCE_DATA		subData;
	subData.pSysMem          = buffer;
	subData.SysMemPitch      = 0;
	subData.SysMemSlicePitch = 0;

	hr = GFGL::GetDevice()->CreateBuffer(&m_BufferDesc, &subData, &m_pVerBuffer);

	if (FAILED(hr))
		assert( 0 );
}

WinDX11VertexBuffer::~WinDX11VertexBuffer()
{
	m_pVerBuffer.reset();
}

void* WinDX11VertexBuffer::Lock()
{
	HRESULT										hr;
	D3D11_MAPPED_SUBRESOURCE	MappedResource;

	assert( m_BufferDesc.CPUAccessFlags == D3D11_CPU_ACCESS_WRITE );

	hr = GFGL::GetDeviceContext()->Map( m_pVerBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0,&MappedResource);

	if (FAILED(hr))
		assert( 0 );

	return MappedResource.pData;
}

void WinDX11VertexBuffer::UnLock()
{
	GFGL::GetDeviceContext()->Unmap(m_pVerBuffer.get(), 0);
}

void WinDX11VertexBuffer::Update( const void* pData, u32 size )
{
	assert( m_BufferDesc.ByteWidth == size );

	GFGL::GetDeviceContext()->UpdateSubresource( m_pVerBuffer.get(), 0, NULL, pData, 0, 0 );
}



}}}
