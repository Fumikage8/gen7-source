#ifndef GFLIB2_GRX_BUFFER_H_INCLUDED
#define GFLIB2_GRX_BUFFER_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_types.h>
#include <gfx/include/gfl2_GLMemory.h>

namespace gfl2 { namespace gfx {

template <class Implement_>
class GFGLBase;

class IBuffer : public GLMemory
{
public:
	virtual void* Lock() = 0;
	virtual void UnLock() = 0;
	virtual void Update( const void* pData, u32 size ) = 0;

	virtual ~IBuffer(){}

protected:
	IBuffer():GLMemory(){}
};

class VertexBuffer : public IBuffer
{
public:
	template <class Implement_>
	friend class GFGLBase;

	virtual ~VertexBuffer(){}

	virtual void* Lock(){return NULL;}
	virtual void UnLock(){}
	virtual void Update( const void* pData, u32 size )
  {
    (void)pData;
    (void)size;
  }

protected:
	VertexBuffer() : IBuffer()
	{

	}
};

class IndexBuffer : public IBuffer
{
public:
	template <class Implement_>
	friend class GFGLBase;

	virtual ~IndexBuffer(){}

	virtual void* Lock(){return NULL;}
	virtual void UnLock(){}
	virtual void Update( const void* pData, u32 size )
  {
    (void)pData;
    (void)size;
  }

	u32 GetIndexNum() const { return m_IndexNum; }
	IndexType GetIndexType() const { return m_IndexType; }

protected:
	IndexBuffer() : IBuffer(),
		m_IndexNum( 0 ),
		m_IndexType( IndexType::Index32 )
	{

	}

	u32												m_IndexNum;
	IndexType									m_IndexType;
};

class ConstantBuffer : public IBuffer
{
public:
	template <class Implement_>
	friend class GFGLBase;

	virtual ~ConstantBuffer(){}

	virtual void* Lock(){return NULL;}
	virtual void UnLock(){}
	virtual void Update( const void* pData, u32 size )
  {
    (void)pData;
    (void)size;
  }

protected:
	ConstantBuffer() : IBuffer()
	{

	}
};

}}

#endif
