#include <gfx/include/WinDX11/gfl2_WinDx11Types.h>
#include <gfx/include/WinDX11/gfl2_WinDX11IndexBuffer.h>
#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_GFGLUtil.h>

namespace gfl2 { namespace gfx { namespace windx11 {

WinDX11IndexBuffer::WinDX11IndexBuffer(u32 size, u32 usage, Pool pool, IndexType type, const void* buffer) : IndexBuffer()
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

	m_BufferDesc.ByteWidth      = size;
	m_BufferDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER; // 頂点バッファ
	m_BufferDesc.MiscFlags      = 0;
	m_BufferDesc.StructureByteStride = 0;
	m_IndexNum = size / GFGLUtil::GetIndexStride(type);
	m_IndexType = type;

	D3D11_SUBRESOURCE_DATA		subData;
	subData.pSysMem          = buffer;
	subData.SysMemPitch      = 0;
	subData.SysMemSlicePitch = 0;

	hr = GFGL::GetDevice()->CreateBuffer(&m_BufferDesc, &subData, &m_pIndexBuffer);

	if (FAILED(hr))
		assert( 0 );
}

WinDX11IndexBuffer::~WinDX11IndexBuffer()
{
	m_pIndexBuffer.reset();
}

void* WinDX11IndexBuffer::Lock()
{
	HRESULT										hr;
	D3D11_MAPPED_SUBRESOURCE	MappedResource;

  assert( m_BufferDesc.CPUAccessFlags == D3D11_CPU_ACCESS_WRITE );

	hr = GFGL::GetDeviceContext()->Map( m_pIndexBuffer.get(), 0, D3D11_MAP_READ, 0,&MappedResource);

	if (FAILED(hr))
		assert( 0 );

	return MappedResource.pData;
}

void WinDX11IndexBuffer::UnLock()
{
	GFGL::GetDeviceContext()->Unmap(m_pIndexBuffer.get(), 0);
}

void WinDX11IndexBuffer::Update( const void* pData, u32 size )
{
	assert( m_BufferDesc.ByteWidth == size );

	GFGL::GetDeviceContext()->UpdateSubresource( m_pIndexBuffer.get(), 0, NULL, pData, 0, 0 );
}



}}}
