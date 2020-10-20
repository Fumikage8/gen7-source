#ifndef _GF_MAYA_PLUGINS_NODE_CombinerAttribute_H_
#define _GF_MAYA_PLUGINS_NODE_CombinerAttribute_H_

#include "ShaderObject.h"
//#include "MonitoringTexture.h"
//#include "ShaderAttributeTexture.h"
//#include "ShaderNodeBase.h"

#include <map>
#include <string>
#include <algorithm>
#include <QtCore/QString>

#include "CombinerShader.h"
//#include "ShaderAttributeTexture.h"
#include "NativeMath/Math.h"
//#include <Renderer/ShapeModel.h>
//#include <Plugins/Shader/CombinerShader.h>
//#include <Plugins/Util/PluginUtil.h>

//#include <Plugins/Command/ModelExportCmd.h>




#define DECLARE_ENUM_BEGIN( CLASSNAME )		\
	class CLASSNAME {												\
	public:																	\
		enum Enum {
#define DECLARE_ENUM_END( CLASSNAME )			\
		};																		\
	};

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  コンバイナタイプ
DECLARE_ENUM_BEGIN( CombinerColorType )
	RGB	= 0,
	A,
	NumberOf
DECLARE_ENUM_END( CombinerColorType )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  参照テーブルタイプ
DECLARE_ENUM_BEGIN( LookUpTableType )
	Distribution0 = 0,
	Distribution1,
	ReflectionR,
	ReflectionG,
	ReflectionB,
	Fresnel,
	NumberOf
DECLARE_ENUM_END( LookUpTableType )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  入力ソースタイプ
DECLARE_ENUM_BEGIN( InputSourceType )
	A	= 0,
	B,
	C,
	NumberOf
DECLARE_ENUM_END( InputSourceType )


//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ブレンディングタイプ
DECLARE_ENUM_BEGIN( BlendModeType )
	Blend	= 0,
	Separate,
	LogicOp,
	None,
	NumberOf
DECLARE_ENUM_END( BlendModeType )

static inline BlendModeType::Enum GetBlendModeType( QString name )
{
	if ( name == "NotUsed" )
		return BlendModeType::None;
	if ( name == "Blend" )
		return BlendModeType::Blend;
	if ( name == "SeparateBlend" )
		return BlendModeType::Separate;
	if ( name == "Logic" )
		return BlendModeType::LogicOp;

	return BlendModeType::None;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  
DECLARE_ENUM_BEGIN( BlendEquationType )
	Add = 0,
  Subtract,
  ReverseSubtract,
  Min,
  Max,
	NumberOf
DECLARE_ENUM_END( BlendEquationType )

static inline BlendEquationType::Enum GetBlendEquationType( QString name )
{
	if ( name == "FuncAdd" )
		return BlendEquationType::Add;
	if ( name == "FuncSubtract" )
		return BlendEquationType::Subtract;
	if ( name == "FuncReverseSubtract" )
		return BlendEquationType::ReverseSubtract;
	if ( name == "Min" )
		return BlendEquationType::Min;
	if ( name == "Max" )
		return BlendEquationType::Max;

	return BlendEquationType::Add;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ブレンド関数タイプ
DECLARE_ENUM_BEGIN( BlendFuncType )
	Zero = 0,
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
	NumberOf
DECLARE_ENUM_END( BlendFuncType )

static inline BlendFuncType::Enum GetBlendFuncType( QString name )
{
	static const char* pTypeNames[15] = {
		"Zero",
		"One",
		"SourceColor",
		"OneMinusSourceColor",
		"DestinationColor",
		"OneMinusDestinationColor",
		"SourceAlpha",
		"OneMinusSourceAlpha",
		"DestinationAlpha",
		"OneMinusDestinationAlpha",
		"ConstantColor",
		"OneMinusConstantColor",
		"ConstantAlpha",
		"OneMinusConstantAlpha",
		"SourceAlphaSaturate",
	};

	for( int i = 0; i < 15; ++i )
	{
		if ( name == pTypeNames[i] )
			return BlendFuncType::Enum(i);
	}

	return BlendFuncType::Zero;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  
DECLARE_ENUM_BEGIN( LogicOpType )
	CLEAR = 0,//すべてのビット値を 0
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
	NumberOf
DECLARE_ENUM_END( LogicOpType )

static inline LogicOpType::Enum GetLogicOpType( QString name )
{
	static const char* pTypeNames[16] = {
		"Clear",//	全てのビット値を0にします。
		"Set",//	全てのビット値を1にします。

		"Copy",//	ソースカラーのビット値をとります。
		"NoOp",//	ディスティネーションカラーのビット値をとります。
		
		"CopyInverted",//	ソースカラーの反転ビット値をとります。
		"Invert",//	ディスティネーションカラーの反転ビット値をとります。

		"And",//	ソースカラーとディスティネーションカラーの論理積をとります。
		"Or",//	ソースカラーとディスティネーションカラーの論理和をとります。
		"Xor",//	ソースカラーとディスティネーションカラーの排他的論理和をとります。

		"AndReverse",//	ソースカラーと、ディスティネーションカラーの反転ビット値の論理積をとります。
		"OrReverse",//	ソースカラーと、ディスティネーションカラーの反転ビット値の論理和をとります。

		"AndInverted",//	ソースカラーの反転ビット値と、ディスティネーションカラーの論理積をとります。
		"OrInverted"//	ソースカラーの反転ビット値と、ディスティネーションカラーの論理和をとります。 
		
		"Nand",//	ソースカラーとディスティネーションカラーの論理積の反転ビット値をとります。
		"Nor",//	ソースカラーとディスティネーションカラーの論理和の反転ビット値をとります。
		"Equiv",//	ソースカラーとディスティネーションカラーの排他的論理和の反転ビット値をとります。
	};

	for( int i = 0; i < 16; ++i )
	{
		if ( name == pTypeNames[i] )
			return LogicOpType::Enum(i);
	}

	return LogicOpType::CLEAR;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------
static inline int GetDrawLayer( QString name )
{
	static const char* pTypeNames[11] = {
		"ConfigurationType0",//	コンフィグレーションタイプ０です。
		"ConfigurationType1",//	コンフィグレーションタイプ１です。
		"ConfigurationType2",//	コンフィグレーションタイプ２です。
		"ConfigurationType3",//	コンフィグレーションタイプ３です。
		"ConfigurationType4",//	コンフィグレーションタイプ４です。
		"ConfigurationType5",//	コンフィグレーションタイプ５です。
		"ConfigurationType6",//	コンフィグレーションタイプ６です。
		"ConfigurationType7",//	コンフィグレーションタイプ７です。
		"ConfigurationType8",//	コンフィグレーションタイプ8です。
		"ConfigurationType9",//	コンフィグレーションタイプ9です。
		"ConfigurationType10"//	コンフィグレーションタイプ10です。 
	};

	for( int i = 0; i < 11; ++i )
	{
		if ( name == pTypeNames[i] )
			return i;
	}

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  演算タイプ
DECLARE_ENUM_BEGIN( MathFormulaType )
	Replace,
	Modulate,
	Add,
	AddSigned,
	Interpolate,
	Subtract,
	DotProduct3Rgb,
	DotProduct3Rgba,
	AddMult,
	MultAdd,
	NumberOf
DECLARE_ENUM_END( MathFormulaType )

static inline int GetMathFormulaType( QString name )
{
	static const char* pTypeNames[10] = {
		"Replace",//	オペレーション０のみが使用されます。 Result = Arg0
		"Modulate",//	オペレーション０とオペレーション１を乗算します。 Result = Arg0 + Arg1
		"Add",//	オペレーション０とオペレーション１を加算します。 Result = Arg0 + Arg1
		"AddSigned",//	オペレーション０とオペレーション１の加算から0.5を引きます。 Result = Arg0 + Arg1 - 0.5
		"Interpolate",//	オペレーション０とオペレーション１をオペレーション２を用いて補間します。 Result = Arg0 * Arg2 + Arg1 * (1 - Arg2)

		"Subtract",//	オペレーション０からオペレーション１を減算します。 Result = Arg0 - Arg1
		"DotProduct3Rgb",//	オペレーション０とオペレーション１の内積でRGBをそれぞれ置き換えます。 ResultRGB = 4 * ((Arg0r - 0.5) * (Arg1r - 0.5) + (Arg0g - 0.5) * (Arg1g - 0.5) + (Arg0b - 0.5) * (Arg1b - 0.5))
		"DotProduct3Rgba",//	オペレーション０とオペレーション１の内積でRGBAをそれぞれ置き換えます。 ResultRGBA = 4 * ((Arg0r - 0.5) * (Arg1r - 0.5) + (Arg0g - 0.5) * (Arg1g - 0.5) + (Arg0b - 0.5) * (Arg1b - 0.5))
		"AddMult",//	オペレーション０とオペレーション１の加算をオペレーション２と乗算します。 Result = (Arg0 + Arg1) * Arg2
		"MultAdd"//	オペレーション０とオペレーション１の乗算をオペレーション２と加算します。 Result = (Arg0 * Arg1) + Arg2 
	};

	for( int i = 0; i < 10; ++i )
	{
		if ( name == pTypeNames[i] )
			return MathFormulaType::Enum(i);
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
static inline int GetScaleType( QString name )
{
	static const char* pTypeNames[3] = {
		"One",//	等倍スケールモードです。
		"Two",//	２倍スケールモードです。
		"Four",//	４倍スケールモードです。 
	};

	for( int i = 0; i < 3; ++i )
	{
		if ( name == pTypeNames[i] )
			return i;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
static inline int GetConstantType( QString name )
{
	static const char* pTypeNames[11] = {
		"Constant0",//	コンスタントカラーです。
		"Constant1",//	第２のコンスタントカラーです。
		"Constant2",//	第３のコンスタントカラーです。
		"Constant3",//	第４のコンスタントカラーです。
		"Constant4",//	第５のコンスタントカラーです。

		"Constant5",//	第６のコンスタントカラーです。
		"Emission",//	エミッションカラーです。
		"Ambient",//	アンビエントカラーです。
		"Diffuse",//	ディフューズカラーです。
		"Specular0",//	スペキュラーカラーです。

		"Specular1"//	第２のスペキュラーカラーです。 
	};

	for( int i = 0; i < 11; ++i )
	{
		if ( name == pTypeNames[i] )
			return i;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
static inline bool IsOutPutBuffer( QString name )
{
	return (name == "PreviousBuffer") ? false : true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  入力タイプ
DECLARE_ENUM_BEGIN( CombInputSourceType )
	Texture0 = 0,
	Texture1,
	Texture2,
	Texture3,
	Constant,

	PrimaryColor,
	FragmentPrimaryColor,
	FragmentSecondaryColor,
	Previous,
	PreviousBuffer,
	NumberOf
DECLARE_ENUM_END( CombInputSourceType )

//-------------------------------------------------------------------------------------------------------------------------------------------------
static inline int GetCombInputSourceType( QString name )
{
	static const char* pTypeNames[10] = {
		"Texture0",//	テクスチャ０です。
		"Texture1",//	テクスチャ１です。
		"Texture2",//	テクスチャ２です。
		"Texture3",//	テクスチャ３です。 プロシージャルテクスチャになります。
		"Constant",//	コンスタントカラーです。

		"PrimaryColor",//	テクスチャ処理に先立ちテクスチャコンバイナ０に入ってくるフラグメントカラーです。
		"FragmentPrimaryColor",//	フラグメントライトのプライマリーカラーです。 ディフューズ、アンビエント、エミッシブを表します。
		"FragmentSecondaryColor",//	フラグメントライトのセカンダリーカラーです。 スペキュラーを表します。 
		"Previous",//	前のテクスチャコンバイナから入ってくるフラグメントカラーです。
		"PreviousBuffer",//	前のテクスチャコンバイナのバッファから入ってくるカラーです。
	};

	for( int i = 0; i < 10; ++i )
	{
		if ( name == pTypeNames[i] )
			return i;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  入力要素タイプ
DECLARE_ENUM_BEGIN( OperandType )
	Color = 0,
	OneMinusColor,
	Alpha,
	OneMinusAlpha,
	Red,

	OneMinusRed,
	Green,
	OneMinusGreen,
	Blue,
	OneMinusBlue,
	NumberOf
DECLARE_ENUM_END( OperandType )

//-------------------------------------------------------------------------------------------------------------------------------------------------
static inline int GetOperandType( QString name )
{
	static const char* pTypeNames[10] = {
		"Color",//	ソースカラーです。
		"OneMinusColor",//	1-Colorです。
		"Alpha",//	ソースのアルファ値です。
		"OneMinusAlpha",//	1-Alphaです。
		"Red",//	ソースのRed値です。

		"OneMinusRed",//	1-Redです。
		"Green",//	ソースのGreen値です。
		"OneMinusGreen",//	1-Greenです。
		"Blue",//	ソースのBlue値です。
		"OneMinusBlue"//	1-Blueです。 
	};

	for( int i = 0; i < 10; ++i )
	{
		if ( name == pTypeNames[i] )
			return i;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  入力角タイプ
DECLARE_ENUM_BEGIN( LutInputAngleType )
	NH = 0,//法線とハーフベクトル （デフォルト）
	VH,//ビューベクトルとハーフベクトル
	NV,//法線とビューベクトル
	LN,//ライトベクトルと法線
	SP,//ライトベクトルの逆ベクトルとスポットライトの方向ベクトル（RR, RG, RB, FR
	CP,//ハーフベクトルの接平面への投影と接線（RR, RG, RB, FR は不可）
	NumberOf
DECLARE_ENUM_END( LutInputAngleType )

static inline int GetLutInputAngleType( QString name )
{
	static const char* pTypeNames[6] = {
		"CosNormalHalf",//	法線とハーフベクトルのなす角のコサイン値です。
		"CosViewHalf",//	視線ベクトルとハーフベクトルのなす角のコサイン値です。
		"CosNormalView",//	法線と視線ベクトルのなす角のコサイン値です。
		"CosLightNormal",//	ライトベクトルの法線のなす角のコサイン値です。
		"CosLightSpot",//	ライトベクトルとスポット方向ベクトルのなす角のコサイン値です。
		"CosPhi"//	ハーフベクトルの接平面投影ベクトルと接線ベクトルのなす角のコサイン値です。
	};

	for( int i = 0; i < 6; ++i )
	{
		if ( name == pTypeNames[i] )
			return i;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  参照テーブルライティング結果スケールタイプ
DECLARE_ENUM_BEGIN( LutScaleType )
	x0_25,
	x0_5,
	x1_0,
	x2_0,
	x4_0,
	x8_0,
	NumberOf
DECLARE_ENUM_END( LutScaleType )

static inline int GetLutScaleType( QString name )
{
	static const char* pTypeNames[6] = {
		"Quarter",//	0.25倍です。
		"Half",//	0.5倍です。
		"One",//	1倍です。
		"Two",//	2倍です。
		"Four",//	4倍です。
		"Eight"//	8倍です。
	};

	for( int i = 0; i < 6; ++i )
	{
		if ( name == pTypeNames[i] )
			return i;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
static inline bool IsPrimaryFresnelEnable( QString name )
{
	if ( name == "Pri" )		return true;
	if ( name == "PriSec" )	return true;

	return false;
}

static inline bool IsSecondaryFresnelEnable( QString name )
{
	if ( name == "Sec" )		return true;
	if ( name == "PriSec" )	return true;

	return false;
}

static inline unsigned int GetCullMode( QString name )
{
	if ( name == "FrontFace" )	return GL_FRONT;
	if ( name == "BackFace" )		return GL_BACK;
	if ( name == "Always" )			return GL_FRONT;
	if ( name == "Never" )			return GL_NONE;

	return GL_NONE;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  深度テストタイプ
DECLARE_ENUM_BEGIN( DepthTestType )
	LESS = 0,
	LEQUAL,
	EQUAL,
	GEQUAL,
	GREATER,
	NOTEQUAL,
	ALWAYS,
	NEVER,
	NumberOf
DECLARE_ENUM_END( DepthTestType )

static inline DepthTestType::Enum GetDepthTestType( QString name )
{
	if ( name == "Never" )
		return DepthTestType::NEVER;
	if ( name == "Always" )
		return DepthTestType::ALWAYS;
	if ( name == "Less" )
		return DepthTestType::LESS;
	if ( name == "Lequal" )
		return DepthTestType::LEQUAL;
	if ( name == "Equal" )
		return DepthTestType::EQUAL;
	if ( name == "Gequal" )
		return DepthTestType::GEQUAL;
	if ( name == "Greater" )
		return DepthTestType::GREATER;
	if ( name == "Notequal" )
		return DepthTestType::NOTEQUAL;

	return DepthTestType::ALWAYS;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ステンシルテストタイプ
DECLARE_ENUM_BEGIN( StencilTestType )
	LESS = 0,
	LEQUAL,
	EQUAL,
	GEQUAL,
	GREATER,
	NOTEQUAL,
	ALWAYS,
	NEVER,
	NumberOf
DECLARE_ENUM_END( StencilTestType )

static inline StencilTestType::Enum GetStencilTestType( QString name )
{
	if ( name == "Never" )
		return StencilTestType::NEVER;
	if ( name == "Always" )
		return StencilTestType::ALWAYS;
	if ( name == "Less" )
		return StencilTestType::LESS;
	if ( name == "Lequal" )
		return StencilTestType::LEQUAL;
	if ( name == "Equal" )
		return StencilTestType::EQUAL;
	if ( name == "Gequal" )
		return StencilTestType::GEQUAL;
	if ( name == "Greater" )
		return StencilTestType::GREATER;
	if ( name == "Notequal" )
		return StencilTestType::NOTEQUAL;

	return StencilTestType::ALWAYS;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ステンシルオペレーションタイプ
DECLARE_ENUM_BEGIN( StencilOpType )
	KEEP = 0,		//	ステンシル値は現在の値を保持します。
	ZERO,				//	ステンシル値は0で置き換えられます。
	REPLACE,		//	ステンシル値はglStencilFuncのrefで指定した値で置き換えられます。
	INCR,				//	ステンシル値は1加算されます。
	INCR_WRAP,	//	ステンシル値は1加算されます。
	DECR,				//	ステンシル値は1減算されます。
	DECR_WRAP,	//	ステンシル値は1減算されます。
	INVERT,			//	ステンシル値はビット反転します。
	NumberOf
DECLARE_ENUM_END( StencilOpType )

//"Keep" "Zero" "Replace" "Increase" "IncreaseWrap" "Decrease" "DecreaseWrap" "Invert"

static inline StencilOpType::Enum GetStencilOpType( QString name )
{
	if ( name == "Keep" )
		return StencilOpType::KEEP;
	if ( name == "Zero" )
		return StencilOpType::ZERO;
	if ( name == "Replace" )
		return StencilOpType::REPLACE;
	if ( name == "Increase" )
		return StencilOpType::INCR;
	if ( name == "IncreaseWrap" )
		return StencilOpType::INCR_WRAP;
	if ( name == "Decrease" )
		return StencilOpType::DECR;
	if ( name == "DecreaseWrap" )
		return StencilOpType::DECR_WRAP;
	if ( name == "Invert" )
		return StencilOpType::INVERT;

	return StencilOpType::KEEP;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  アルファテストタイプ
DECLARE_ENUM_BEGIN( AlphaTestType )
	LESS = 0,
	LEQUAL,
	EQUAL,
	GEQUAL,
	GREATER,
	NOTEQUAL,
	ALWAYS,
	NEVER,
	NumberOf
DECLARE_ENUM_END( AlphaTestType )

static inline AlphaTestType::Enum GetAlphaTestType( QString name )
{
	if ( name == "Never" )
		return AlphaTestType::NEVER;
	if ( name == "Always" )
		return AlphaTestType::ALWAYS;
	if ( name == "Less" )
		return AlphaTestType::LESS;
	if ( name == "Lequal" )
		return AlphaTestType::LEQUAL;
	if ( name == "Equal" )
		return AlphaTestType::EQUAL;
	if ( name == "Gequal" )
		return AlphaTestType::GEQUAL;
	if ( name == "Greater" )
		return AlphaTestType::GREATER;
	if ( name == "Notequal" )
		return AlphaTestType::NOTEQUAL;

	return AlphaTestType::ALWAYS;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ブレンドタイプ
DECLARE_ENUM_BEGIN( BlendType )
	None = 0,
	Nuki,			//! 抜き設定
	Liner,		//! 線形補完
	Add,			//! 加算
	Sub,			//! 減算
	Mul,			//! 乗算
	Inverse,	//! ネガポジ反転
	NumberOf
DECLARE_ENUM_END( BlendType )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ブレンド関数タイプ
DECLARE_ENUM_BEGIN( BlendFunc )
	SrcAddDst = 0,
	SrcSubDst,
	DstSubSrc,
	Min,
	Max,
	NumberOf
DECLARE_ENUM_END( BlendFunc )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ブレンド関数タイプ
DECLARE_ENUM_BEGIN( BlendElement )
	Zero = 0,
	One,
	CombinerRGB,
	OneSubCombinerRGB,
	FrameBufferRGB,
	OneSubFrameBufferRGB,
	CombinerA,
	OneSubCombinerA,
	FrameBufferA,
	OneSubFrameBufferA,
	BlendRGB,
	OneSubBlendRGB,
	BlendA,
	OneSubBlendA,
	Min,
	NumberOf
DECLARE_ENUM_END( BlendElement )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ポケモン法線タイプ
DECLARE_ENUM_BEGIN( PokeNormalType )
	None = 0,
	Dummy,
	ObjectSpace,
	NumberOf
DECLARE_ENUM_END( PokeNormalType )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ポケモン法線タイプ
DECLARE_ENUM_BEGIN( LutInputAngle )
	NH = 0,//法線とハーフベクトル （デフォルト）
	VH,//ビューベクトルとハーフベクトル
	NV,//法線とビューベクトル
	LN,//ライトベクトルと法線
	NumberOf
DECLARE_ENUM_END( LutInputAngle )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  エッジタイプ(ユーザーアトリビュート)
DECLARE_ENUM_BEGIN( EdgeType )
	Default = 0,
	VColor,
	//Dot,
	None,
	NumberOf
DECLARE_ENUM_END( EdgeType )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  表示レイタータイプ
DECLARE_ENUM_BEGIN( DrawLayer )
	Layer0	= 0,
	Layer1,
	Layer2,
	Layer3,
	NumberOf
DECLARE_ENUM_END( DrawLayer )

namespace gfl { namespace maya { namespace plugins { namespace Node {

class VertexShaderType
{
public:
	enum Enum {
		Default				= 0,			//0
		Poke					= (1<<0),	//1
		PokeNormal		= (1<<1),	//2
		PokeFire			= (1<<2),	//4
		MegaGangar		= (1<<3),	//8
		Contrast			= (1<<4),	//16
		Default1			= (1<<5),	//32
		D01						= (1<<6),	//64
		D01ProjTex		= (1<<7)	//128
	};

	VertexShaderType( void ) : m_Value(Default){}
	VertexShaderType( Enum value ) : m_Value( value ) {}
	operator Enum ( void ) const { return m_Value; }

	bool IsPokemonShader() const;
	bool Is( UInt32 types ) const;

private:
	Enum m_Value;
};

class CombinerAttribute
{
public:	
	static const int										TextureMax = 3;
	static const int										CombinerMax = 6;

	enum{
		ProjectionNone = 0,
		ProjectionDropShadow,
		ProjectionDropLight
	};

	struct LutTableImage{
    unsigned char     lut[256];
		::std::string				imageName;
		::std::string				imagePath;

    LutTableImage()
    {
      memset( lut, 0xFF, sizeof(lut) );
			imagePath = imageName = "";
    }
    
  };

	struct MaterialColor{
		Math::Vector   m_Emission;
		Math::Vector   m_Ambient;
		Math::Vector   m_Diffuse;
		Math::Vector   m_Specular0;
		Math::Vector   m_Specular1;

		GLint					 m_hEmission;
		GLint					 m_hAmbient;
		GLint					 m_hDiffuse;
		GLint				   m_hSpecular0;
		GLint				   m_hSpecular1;
	};

	struct EtcColor{
		Math::Vector   m_BlendColor;
		Math::Vector   m_BufferColor;
		Math::Vector   m_TextureBorderColor[TextureMax];
		GLint				   m_hBufferColor;
	};

	struct TextureCombinerAttribute{
		short   m_MathFormula;
		int		  m_Scale;
		short   m_InputSource[InputSourceType::NumberOf];
		short   m_Operand[InputSourceType::NumberOf];
		bool		m_OutPutBuffer;

		GLint   m_hMathFormula;
		GLint   m_hScale;
		GLint   m_hInputSource[InputSourceType::NumberOf];
		GLint   m_hOperand[InputSourceType::NumberOf];
		GLint		m_hOutPutBuffer;
	};

	struct LookUpTable{
		short						m_LutInputAngle;
		short						m_LutScale;
		GLint						m_hLutInputAngle;
		GLint						m_hLutScale;
		QString					m_LutTableName;

		LookUpTable()
		{
			m_LutInputAngle = m_LutScale = 0;
			m_hLutInputAngle = m_hLutScale = 0;
			m_LutTableName = "";
		}
	};

	struct TextureAttribute{
		short   m_MappingType;
		short   m_UvSetNo;
		float   m_ScaleU;
		float   m_ScaleV;
		float   m_Rotate;
		float   m_TranslateU;
		float   m_TranslateV;
		short   m_RepeatU;
		short   m_RepeatV;
		short   m_MagFilter;
		short   m_MinFilter;

		GLint   m_hMappingType;
		GLint   m_hUvSetNo;
		GLint   m_hScaleU;
		GLint   m_hScaleV;
		GLint   m_hRotate;
		GLint   m_hTranslateU;
		GLint   m_hTranslateV;
	};

	struct BlendFunc{
		short											m_BlendFunc;
		short											m_SrcElement;
		short											m_DstElement;
	};

	struct AttributeParam{
		::std::string									m_UiList;

		int													m_CombinerStageMax;
		GLint												m_hCombinerStageMax;

		GLint												m_PresetType;
		GLint												m_PresetVersion;
		bool												m_PsLightingEnable;
		GLint												m_hVsLightingEnable;
		bool												m_VsLightingEnable;
		short												m_LightSetNo;
		bool												m_FogEnable;
		short												m_FogNo;
		short												m_CullMode;
		
		MaterialColor								m_MaterialColor;													//カラー/マテリアルカラー
		Math::Vector								m_ConsntantColor[CombinerMax];						//カラー/コンスタントカラー
		GLint												m_hConsntantColor[CombinerMax];
		EtcColor										m_EtcColor;																//カラー/その他のカラー

		short												m_BumpMapNo;															//テクスチャ/バンプマッピング/バンプ使用テクスチャ
		short												m_BumpMapType;														//テクスチャ/バンプマッピング/使用方法
		bool												m_ZRecastEnable;													//テクスチャ/バンプマッピング/Ｚ成分の再計算
		GLint												m_hBumpMapNo;
		GLint												m_hBumpMapType;
		GLint												m_hZRecastEnable;
		GLint												m_hTangentNo;

		TextureAttribute						m_TextureAttribute[TextureMax];						//テクスチャ一覧/テクスチャ
	  bool												m_TextureAttributeEnable[TextureMax];
	  GLint												m_hTextureAttributeEnable[TextureMax];
	  GLint												m_hIsTexture2D[TextureMax];

		bool												m_LookUpTableEnable[LookUpTableType::NumberOf];
		GLint												m_hLookUpTableEnable[LookUpTableType::NumberOf];
		LookUpTable									m_LookUpTable[LookUpTableType::NumberOf];	//フラグメント/フラグメントライティング
		bool												m_PrimaryFresnelEnable;										//フラグメント/フラグメントライティング/フレネルをプライマリに適応
		bool												m_SecondaryFresnelEnable;									//フラグメント/フラグメントライティング/フレネルをセカンダリに適応
		bool												m_SpecularColorClamp;											//フラグメント/フラグメントライティング/スペキュラをクランプ
		bool												m_CookTorrance0Enable;										//フラグメント/フラグメントライティング/ジオメトリファクタ０
		bool												m_CookTorrance1Enable;										//フラグメント/フラグメントライティング/ジオメトリファクタ０
		GLint												m_hPrimaryFresnelEnable;
		GLint												m_hSecondaryFresnelEnable;
		GLint												m_hSpecularColorClamp;
		GLint												m_hCookTorrance0Enable;
		GLint												m_hCookTorrance1Enable;

		TextureCombinerAttribute		m_TextureCombinerAttribute[CombinerColorType::NumberOf][CombinerMax];	//フラグメント/テクスチャコンバイナ
		short												m_TextureCombinerConstant[CombinerMax];																//フラグメント/テクスチャコンバイナ/各コンスタントカラー
		GLint												m_hTextureCombinerConstant[CombinerMax];

		short												m_DrawLayer;															//ブレンド/テスト/描画レイヤー
		short												m_AlphaTestType;													//ブレンド/テスト/アルファテスト/満たせば通過
		float												m_AlphaTestRefValue;											//ブレンド/テスト/アルファテスト/参照値
		short												m_DepthTestType;													//ブレンド/テスト/デプステスト/満たせば通過
		bool												m_UpDateDepth;														//ブレンド/テスト/デプステスト/デプスバッファを更新する
		int													m_PolygonOffSet;													//ブレンド/テスト/デプステスト/ポリゴンオフセット


		short												m_StencilTestEnable;											//ブレンド/テスト/ステンシルテスト/
		short												m_StencilTestType;												//ブレンド/テスト/ステンシルテスト/
		short												m_StencilValue;														//ブレンド/テスト/ステンシルテスト/
		short												m_StencilMask;														//ブレンド/テスト/ステンシルテスト/
		short												m_StencilFailOp;													//ブレンド/テスト/ステンシルテスト/
		short												m_StencilZFailOp;													//ブレンド/テスト/ステンシルテスト/
		short												m_StencilZpassOp;													//ブレンド/テスト/ステンシルテスト/


		short												m_BlendMode;															//ブレンド/テスト/ブレンダー/ブレンドモード
		BlendFunc										m_BlendFunc[CombinerColorType::NumberOf];	//ブレンド/テスト/ブレンダー/フレンド式各種
		short												m_LogicalOperation;												//ブレンド/テスト/ブレンダー/論理演算方法

		VertexShaderType						m_VertexShaderType;
		bool												m_IsPokemonShader;//CR様の対応が完了したら削除
		GLint												m_hIsPokemonShader;
		GLint												m_hFsIsPokemonShader;

		bool												m_UseObjectSpaceNormalMap;
		GLint												m_hUseObjectSpaceNormalMap;



		bool												m_UseShadowTexture;
		GLint												m_hUseShadowTexture;

		GLint												m_hHigLightTextureEnable;
		GLint												m_hVsRimParam;
		Math::Vector								m_VsRimParam;
		short												m_PokeNormalType;
		short												m_BlendType;
		bool												m_UpdateShaderParam;

		GLint												m_hVsSceneContrastEnable;
		GLint												m_hVsZoneContrast;
		GLint												m_hVsSceneContrast;
		

    //整理整頓
    void Compaction()
    {
      CheckBumpMapEnable();
      ShiftNullTextureNo();
    }

		void InitTextureCombinerAttribute();

    void CheckBumpMapEnable();
    void ShiftNullTextureNo();
    void ProjectionTextureShift();

		void SetDropShadowCombiner();
		void SetDropLightCombiner();

    void textureShift( short tex0, short tex1, short tex2 );
		
    void Clear();
	};

private:
	CombinerAttribute(){}
	~CombinerAttribute(){}
};

#undef		DECLARE_ENUM_BEGIN
#undef		DECLARE_ENUM_END

}}}}

#endif
