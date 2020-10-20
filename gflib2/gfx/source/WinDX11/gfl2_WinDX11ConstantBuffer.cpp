#include <gfx/include/WinDX11/gfl2_WinDx11Types.h>
#include <gfx/include/WinDX11/gfl2_WinDX11ConstantBuffer.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace gfx { namespace windx11 {

WinDX11ConstantBuffer::WinDX11ConstantBuffer(const ConstantBufferElement* pConstantBufferElements, u32 usage, Pool pool, const void* buffer) : ConstantBuffer()
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

	u32					size = 0;
	u32					elementSize[ConstantType::NumberOf] = {
		sizeof(math::Vector),//Vector
		sizeof(math::Matrix),//Matrix
		sizeof(b32),//Bool
		sizeof(s32),//Int
		sizeof(f32),//Float
		sizeof(b32)*4,//BoolArray
		sizeof(s32)*4,//IntArray
		sizeof(f32)*4//FloatArray
	};

	while( pConstantBufferElements->Type != 0xFF)
	{
		//パディングは考慮していない。
		size += (elementSize[ pConstantBufferElements->Type ] * pConstantBufferElements->Size);
		++pConstantBufferElements;
	}

	m_BufferDesc.ByteWidth      = size;
	m_BufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER; // 頂点バッファ
	m_BufferDesc.MiscFlags      = 0;
	m_BufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA		subData;
	subData.pSysMem          = buffer;
	subData.SysMemPitch      = 0;
	subData.SysMemSlicePitch = 0;

	hr = GFGL::GetDevice()->CreateBuffer(&m_BufferDesc, buffer ? &subData : NULL, &m_pConstantBuffer);

	if (FAILED(hr))
		assert( 0 );
}

WinDX11ConstantBuffer::~WinDX11ConstantBuffer()
{
	m_pConstantBuffer.reset();
}

void* WinDX11ConstantBuffer::Lock()
{
	HRESULT										hr;
	D3D11_MAPPED_SUBRESOURCE	MappedResource;

	assert( m_BufferDesc.CPUAccessFlags == D3D11_CPU_ACCESS_WRITE );

	hr = GFGL::GetDeviceContext()->Map( m_pConstantBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0,&MappedResource);

	if (FAILED(hr))
		assert( 0 );

	return MappedResource.pData;
}

void WinDX11ConstantBuffer::UnLock()
{
	GFGL::GetDeviceContext()->Unmap(m_pConstantBuffer.get(), 0);
}

void WinDX11ConstantBuffer::Update( const void* pData, u32 size )
{
	assert( m_BufferDesc.ByteWidth == size );

	GFGL::GetDeviceContext()->UpdateSubresource( m_pConstantBuffer.get(), 0, NULL, pData, 0, 0 );
}



}}}
