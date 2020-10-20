#include <clr/include/gfx/gfl2_types_CLR.h>

namespace gfl2 { namespace clr { namespace gfx {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
VertexElement::VertexElement()
{
	this->m_pNative = new gfl2::gfx::VertexElement();
}

VertexElement::!VertexElement()
{
	delete this->m_pNative;
}

VertexElement^ VertexElement::VERTEX_ELEMENT_DECL(const u8 stream, const StreamUsage::Enum usage, const u8 usage_index, const VertexType::Enum type, const u8 size )
{
	VertexElement^			vertexElement = gcnew VertexElement();
	*(vertexElement->m_pNative) = gfl2::gfx::VertexElement::VERTEX_ELEMENT_DECL( stream, 
																																							 static_cast<u8>(usage),
																																							 usage_index,
																																							 static_cast<u8>(type),
																																							 size );
	return vertexElement;
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
ConstantBufferElement::ConstantBufferElement()
{
	this->m_pNative = new gfl2::gfx::ConstantBufferElement();
}

ConstantBufferElement::!ConstantBufferElement()
{
	delete this->m_pNative;
}

ConstantBufferElement^ ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_DECL(const ConstantType::Enum type, const u8 size )
{
	ConstantBufferElement^			element = gcnew ConstantBufferElement();
	*(element->m_pNative) = gfl2::gfx::ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_DECL( static_cast<gfl2::gfx::ConstantType>((u32)type), size );
	return element;
}


}}}