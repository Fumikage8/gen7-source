#ifndef GFLIB2_GRX_CLR_BUFFER_CLR_H_INCLUDED
#define GFLIB2_GRX_CLR_BUFFER_CLR_H_INCLUDED

#using <System.Drawing.dll>

#include <gfx/include/gfl2_Buffer.h>

namespace gfl2 { namespace clr { namespace gfx {

interface class IBuffer
{
public:

	void Update( array<u8>^ pData, u32 size );
};

public ref class VertexBuffer : public IBuffer
{
public:

	VertexBuffer( gfl2::gfx::VertexBuffer *pNative ){ m_pNative = pNative; }
	~VertexBuffer(){this->!VertexBuffer();}
	!VertexBuffer();

	virtual void Update( array<u8>^ pData, u32 size );

	gfl2::gfx::VertexBuffer* GetNative(){ return m_pNative; }

private:

	gfl2::gfx::VertexBuffer			*m_pNative;
};

public ref class IndexBuffer : public IBuffer
{
public:

	IndexBuffer( gfl2::gfx::IndexBuffer *pNative ){ m_pNative = pNative; }
	~IndexBuffer(){this->!IndexBuffer();}
	!IndexBuffer();

	virtual void Update( array<u8>^ pData, u32 size );

	gfl2::gfx::IndexBuffer* GetNative(){ return m_pNative; }

private:

	gfl2::gfx::IndexBuffer			*m_pNative;
};


public ref class ConstantBuffer : public IBuffer
{
public:

	ConstantBuffer( gfl2::gfx::ConstantBuffer *pNative ){ m_pNative = pNative; }
	~ConstantBuffer(){this->!ConstantBuffer();}
	!ConstantBuffer();

	virtual void Update( array<u8>^ pData, u32 size );

	gfl2::gfx::ConstantBuffer* GetNative(){ return m_pNative; }

private:

	gfl2::gfx::ConstantBuffer			*m_pNative;
};


}}}

#endif