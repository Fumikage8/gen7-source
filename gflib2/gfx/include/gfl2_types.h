#ifndef GFLIB2_GRX_TYPES_H_INCLUDED
#define GFLIB2_GRX_TYPES_H_INCLUDED

#include <gfx/include/gfl2_enums.h>

#include <types/include/gfl2_Typedef.h>
#include <assert.h>

#ifdef DEBUG_
	#define					down_cast				dynamic_cast
#else
	#define					down_cast				static_cast
#endif

#if defined(GF_PLATFORM_WIN32)

	#define GFL2_LITTLE_ENDIAN			//これは適切な場所ができたら移す
	//#define GFL2_BIG_ENDIAN			//これは適切な場所ができたら移す

#endif

namespace gfl2 { namespace gfx {

	//-------------------------------------------------------------------------------------------------------------------------------------------------

	typedef void* DisplayDeviceHandle;   //!< デバイスハンドル

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief パイプラインに送られる入力頂点の定義型
	struct VertexElement {

		u8		Stream;				// ストリーム番号
		u8		Usage;				// ストリームの用途
		u8		UsageIndex;		// ストリームのインデックス
		u8		Type;					// 座標のデータ型
		u8		Size;					// データの要素数
		
		// Vertex Element の定義関数
		inline static VertexElement VERTEX_ELEMENT_DECL(const u8 stream, const u8 usage, const u8 usage_index, const u8 type, const u8 size ){
			VertexElement tmp;
			tmp.Stream = stream;
			tmp.Type   = type;
			tmp.Size   = size;
			tmp.Usage  = usage;
			tmp.UsageIndex  = usage_index;
			return tmp;
		}
	
		// Vertex Element の終端定義用の関数
		inline static const VertexElement& VERTEX_ELEMENT_END(){
			static const VertexElement s_vertex_element = { 0xff };
			return s_vertex_element;
		}
	};

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief コンスタントバッファ要素の定義型
	struct ConstantBufferElement {

		u8		Type;					// 座標のデータ型
		u8		Size;					// データの要素数
		
		// ConstantBufferElement の定義関数
		inline static ConstantBufferElement CONSTANT_BUFFER_ELEMENT_DECL(const u8 type, const u8 size ){
			ConstantBufferElement tmp;
			tmp.Type   = type;
			tmp.Size   = size;
			return tmp;
		}
	
		// ConstantBufferElement の終端定義用の関数
		inline static const ConstantBufferElement& CONSTANT_BUFFER_ELEMENT_END(){
			static const ConstantBufferElement s_constant_element = { 0xff };
			return s_constant_element;
		}
	};

}}

#endif

