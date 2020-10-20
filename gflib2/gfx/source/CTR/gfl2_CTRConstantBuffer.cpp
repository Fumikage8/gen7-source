#include <gfx/include/CTR/gfl2_CTRTypes.h>
#include <gfx/include/CTR/gfl2_CTRConstantBuffer.h>
#include <gfx/include/gfl2_GFGL.h>

#include <string.h>

namespace gfl2 { namespace gfx { namespace ctr {

CTRConstantBuffer::CTRConstantBuffer(const ConstantBufferElement* pConstantBufferElements, u32 usage, Pool pool, const void* buffer) :
  ConstantBuffer(),
  m_ElementCount(0),
  m_Size(0),
  m_pBuffer(NULL)
{
  GFL_ASSERT( this->GetInstanceAllocator() );
	m_Size = 0;

	u32 elementSize[ConstantType::NumberOf] =
  {
		sizeof(math::Vector4),//Vector4
		sizeof(math::Matrix44),//Matrix44
		sizeof(b32),//Bool
		sizeof(s32),//Int
		sizeof(f32),//Float
		sizeof(b32)*4,//BoolArray
		sizeof(s32)*4,//IntArray
		sizeof(f32)*4//FloatArray
	};

  m_ElementCount = 0;
	while( pConstantBufferElements->Type != 0xFF)
	{
    m_TypeList[m_ElementCount] = pConstantBufferElements->Type;
    m_OffsetList[m_ElementCount] = m_Size;

		//パディングは考慮していない。
		m_Size += elementSize[ pConstantBufferElements->Type ] * pConstantBufferElements->Size;
		++pConstantBufferElements;

    ++m_ElementCount;
	}

  // メモリ確保
  m_pBuffer = this->GetInstanceAllocator()->SystemMemoryMallocAlign(m_Size, 128);

  // メモリコピー
  if (buffer != NULL)
  {
    memcpy(m_pBuffer, buffer, m_Size);
  }
}

CTRConstantBuffer::~CTRConstantBuffer()
{
  // メモリ開放
  if (m_pBuffer != NULL)
  {
    this->GetInstanceAllocator()->SystemMemoryFree(m_pBuffer);
  }
}

void* CTRConstantBuffer::Lock()
{
	return NULL;
}

void CTRConstantBuffer::UnLock()
{
	
}

void CTRConstantBuffer::Update( const void* pData, u32 size )
{
  memcpy(m_pBuffer, pData, size);
}



}}}
