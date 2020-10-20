#include <gfx/include/CTR/gfl2_CTRTypes.h>
#include <gfx/include/CTR/gfl2_CTRIndexBuffer.h>
#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_GFGLUtil.h>

#include <string.h>

namespace gfl2 { namespace gfx { namespace ctr {

CTRIndexBuffer::CTRIndexBuffer(u32 size, u32 usage, Pool pool, IndexType type, const void* buffer) :
  IndexBuffer(),
  m_pBuffer(NULL)
{
  (void)usage;
  (void)pool;

	m_IndexType = type;
	m_IndexNum = size / GFGLUtil::GetIndexStride(type);

  // メモリ確保
  m_pBuffer = gfl2::gfx::GLMemory::GetAllocator()->SystemMemoryMallocAlign(size, 128);
  m_IndexStream.physicalAddr = nngxGetPhysicalAddr(reinterpret_cast<uptr>(m_pBuffer));
  m_IndexStream.drawVtxNum = m_IndexNum;
  m_IndexStream.isUnsignedByte = (type == IndexType::Index8);

  // バッファコピー
  if (buffer != NULL)
  {
    memcpy(m_pBuffer, buffer, size);
  }
}

CTRIndexBuffer::~CTRIndexBuffer()
{
  // メモリ開放
  if (m_pBuffer != NULL)
  {
    gfl2::gfx::GLMemory::GetAllocator()->SystemMemoryFree(m_pBuffer);
  }
}

void* CTRIndexBuffer::Lock()
{
	return NULL;
}

void CTRIndexBuffer::UnLock()
{
	
}

void CTRIndexBuffer::Update( const void* pData, u32 size )
{
  memcpy(m_pBuffer, pData, size);
}



}}}
