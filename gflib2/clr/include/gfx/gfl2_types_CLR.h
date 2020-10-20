#ifndef GFLIB2_GRX_CLR_TYPES_CLR_H_INCLUDED
#define GFLIB2_GRX_CLR_TYPES_CLR_H_INCLUDED

#using <System.Drawing.dll>

#include <clr/include/gfx/gfl2_enums_CLR.h>
#include <gfx/include/gfl2_types.h>
#include <types/include/gfl2_Typedef.h>

namespace gfl2 { namespace clr { namespace gfx {

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief パイプラインに送られる入力頂点の定義型
	public ref class VertexElement{
	public:

		VertexElement();
		~VertexElement(){this->!VertexElement();}
		!VertexElement();

		static VertexElement^ VERTEX_ELEMENT_DECL(const u8 stream, const StreamUsage::Enum usage, const u8 usage_index, const VertexType::Enum type, const u8 size );
		static VertexElement^ VERTEX_ELEMENT_END()
		{
			return VERTEX_ELEMENT_DECL( 0xFF, StreamUsage::Enum::Position, 0, VertexType::Enum::Float, 4 );
		}

		gfl2::gfx::VertexElement* GetNative(){ return m_pNative; }

	private:

		gfl2::gfx::VertexElement			*m_pNative;
	};

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief コンスタントバッファ要素の定義型
	public ref class ConstantBufferElement {
	public:

		ConstantBufferElement();
		~ConstantBufferElement(){this->!ConstantBufferElement();}
		!ConstantBufferElement();

		// ConstantBufferElement の定義関数
		static ConstantBufferElement^ CONSTANT_BUFFER_ELEMENT_DECL(const ConstantType::Enum type, const u8 size );
	
		// ConstantBufferElement の終端定義用の関数
		static ConstantBufferElement^ CONSTANT_BUFFER_ELEMENT_END(){
			return CONSTANT_BUFFER_ELEMENT_DECL( (ConstantType::Enum)0xFF, 0 );
		}

	gfl2::gfx::ConstantBufferElement* GetNative(){ return m_pNative; }

	private:

		gfl2::gfx::ConstantBufferElement			*m_pNative;
	};


}}}

#undef DECLARE_ENUM_BEGIN
#undef DECLARE_ENUM_END

#endif