#include <gfx/include/CTR/gfl2_CTRTypes.h>
#include <gfx/include/CTR/gfl2_CTRVertexBuffer.h>
#include <gfx/include/gfl2_GFGL.h>

#include <string.h>

namespace gfl2 { namespace gfx { namespace ctr {

CTRVertexBuffer::CTRVertexBuffer(u32 size, u32 vertex_num, u32 usage, Pool pool, const void* buffer) : 
  VertexBuffer(),
  m_VertexNum(0),
  m_pBuffer(NULL),
  m_PhysicalAddr(NULL)
{
	m_VertexNum = vertex_num;

  // メモリ確保
  m_pBuffer = gfl2::gfx::GLMemory::GetAllocator()->SystemMemoryMallocAlign(size * vertex_num, 128);

  // メモリコピー
  if (m_pBuffer != NULL)
  {
    memcpy(m_pBuffer, buffer, size * vertex_num);
  }

  m_PhysicalAddr = nngxGetPhysicalAddr(reinterpret_cast<uptr>(m_pBuffer));
}

CTRVertexBuffer::~CTRVertexBuffer()
{
  // メモリ開放
  if (m_pBuffer != NULL)
  {
    gfl2::gfx::GLMemory::GetAllocator()->SystemMemoryFree(m_pBuffer);
  }
}

void* CTRVertexBuffer::Lock()
{
	return m_pBuffer;
}

void CTRVertexBuffer::UnLock()
{
}

void CTRVertexBuffer::Update( const void* pData, u32 size )
{
  memcpy(m_pBuffer, pData, size);
}


}}}
