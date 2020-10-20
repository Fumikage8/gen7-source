#ifndef GFLIB2_GRX_CTR_CTRVERTEXBUFFER_H_INCLUDED
#define GFLIB2_GRX_CTR_CTRVERTEXBUFFER_H_INCLUDED

#include <gfx/include/gfl2_Buffer.h>
#include <gfx/include/CTR/gfl2_CTRTypes.h>

namespace gfl2 { namespace gfx { namespace ctr {

class CTRGL;

class CTRVertexBuffer : public VertexBuffer
{
public:
	friend class CTRGL;

	CTRVertexBuffer(u32 size, u32 vertex_num, u32 usage, Pool pool, const void* buffer);
	virtual ~CTRVertexBuffer();

	virtual void* Lock();
	virtual void UnLock();
	virtual void Update( const void* pData, u32 size );

	nn::gr::Vertex* GetHandle() { return &m_Vertex; }
  uptr GetPhysicalAddr() { return m_PhysicalAddr; }

private:
	u32					m_VertexNum;
	//GLuint			m_BufferHandle;
  nn::gr::Vertex  m_Vertex;
  void*           m_pBuffer;
  uptr            m_PhysicalAddr;
};

}}}

#endif
