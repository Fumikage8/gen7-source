#ifndef GFLIB2_RENDERINGENGINE_RENDERER_MATERIALTYPES_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_RENDERER_MATERIALTYPES_H_INCLUDED

#include <types/include/gfl2_Typedef.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DECLARE_ENUM_BEGIN( CLASSNAME )                           \
	class CLASSNAME {                                               \
	public:																                          \
		enum Enum {
#define DECLARE_ENUM_END( CLASSNAME, DEFAULTVALUE )               \
		};																                            \
	public:																                          \
		CLASSNAME(): value_(DEFAULTVALUE) {}                          \
		explicit CLASSNAME( int value ) : value_( Enum( value ) ) {}	\
		CLASSNAME( Enum value ) : value_( value ) {}					        \
		operator Enum () const { return value_; }					            \
	private:															                          \
		Enum value_;													                        \
	};

#ifdef DEBUG_
	#define					down_cast				dynamic_cast
#else
	#define					down_cast				static_cast
#endif

namespace gfl2 { namespace renderingengine { namespace renderer { 
	// [ブレンド/テスト]の項目
	DECLARE_ENUM_BEGIN( BlendMode )
		Blend,
		Separate,
		LogicOp,
		None,
		NumberOf,
	DECLARE_ENUM_END( BlendMode, Blend )

	DECLARE_ENUM_BEGIN( BlendElement )
		Zero,
		One,
		SourceColor,
		OneMinusSourceColor,
		DestinationColor,
		OneMinusDestinationColor,
		SourceAlpha,
		OneMinusSourceAlpha,
		DestinationAlpha,
		OneMinusDestinationAlpha,
		ConstantColor,
		OneMinusConstantColor,
		ConstantAlpha,
		OneMinusConstantAlpha,
		SourceAlphaSaturate,
		NumberOf,
	DECLARE_ENUM_END( BlendElement, Zero )

	DECLARE_ENUM_BEGIN( LogicalOperation )
		CLEAR,//すべてのビット値を 0
		SET,//すべてのビット値を 1	

		COPY,//ソースカラーのビット値	
		NOOP,//デスティネーションカラーのビット値	
					
		COPY_INVERTED,//ソースカラーの反転ビット値	
		INVERT,//デスティネーションカラーの反転ビット値	

		AND,//ソースとデスティネーションカラーの論理積	
		OR,//ソースとデスティネーションカラーの論理和	
		XOR,//ソースとデスティネーションカラーの排他的論理和	

		AND_REVERSE,//ソースとデスティネーションカラーの反転ビット値の論理積	
		OR_REVERSE,//ソースとデスティネーションカラーの反転ビット値の論理和	
					
		AND_INVERTED,//ソースカラーの反転ビット値と、デスティネーションカラーの論理積	
		OR_INVERTED,//ソースカラーの反転ビット値と、デスティネーションカラーの論理和	

		NAND,//ソースとデスティネーションカラーの論理積の反転ビット値	
		NOR,//ソースとデスティネーションカラーの論理和の反転ビット値						
		EQUIV,//ソースとデスティネーションカラーの排他的論理和の反転ビット値	
		NumberOf,
	DECLARE_ENUM_END( LogicalOperation, CLEAR )

}}}

#undef		DECLARE_ENUM_BEGIN
#undef		DECLARE_ENUM_END

#endif