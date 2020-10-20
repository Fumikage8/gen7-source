#ifndef GFLIB2_GRX_CTR_CTRINDEXBUFFER_H_INCLUDED
#define GFLIB2_GRX_CTR_CTRINDEXBUFFER_H_INCLUDED

#include <gfx/include/gfl2_Buffer.h>
#include <gfx/include/CTR/gfl2_CTRTypes.h>

namespace gfl2 { namespace gfx { namespace ctr {

class CTRGL;

class CTRIndexBuffer : public IndexBuffer
{
public:
	friend class CTRGL;

	CTRIndexBuffer(u32 size, u32 usage, Pool pool, IndexType type, const void* buffer);
	virtual ~CTRIndexBuffer();

	virtual void* Lock();
	virtual void UnLock();
	virtual void Update( const void* pData, u32 size );

	//const GLuint GetHandle() const { return m_BufferHandle; }
  nn::gr::Vertex::IndexStream* GetHandle() { return &m_IndexStream; }

private:

	//GLuint		m_BufferHandle;
	
  nn::gr::Vertex::IndexStream m_IndexStream;
  void*           m_pBuffer;
};

}}}

#endif
