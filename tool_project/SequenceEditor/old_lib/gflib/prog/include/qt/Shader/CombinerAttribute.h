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
//!  �R���o�C�i�^�C�v
DECLARE_ENUM_BEGIN( CombinerColorType )
	RGB	= 0,
	A,
	NumberOf
DECLARE_ENUM_END( CombinerColorType )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  �Q�ƃe�[�u���^�C�v
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
//!  ���̓\�[�X�^�C�v
DECLARE_ENUM_BEGIN( InputSourceType )
	A	= 0,
	B,
	C,
	NumberOf
DECLARE_ENUM_END( InputSourceType )


//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  �u�����f�B���O�^�C�v
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
//!  �u�����h�֐��^�C�v
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
	CLEAR = 0,//���ׂẴr�b�g�l�� 0
	SET,//���ׂẴr�b�g�l�� 1	

	COPY,//�\�[�X�J���[�̃r�b�g�l	
	NOOP,//�f�X�e�B�l�[�V�����J���[�̃r�b�g�l	
	
	COPY_INVERTED,//�\�[�X�J���[�̔��]�r�b�g�l	
	INVERT,//�f�X�e�B�l�[�V�����J���[�̔��]�r�b�g�l	

	AND,//�\�[�X�ƃf�X�e�B�l�[�V�����J���[�̘_����	
	OR,//�\�[�X�ƃf�X�e�B�l�[�V�����J���[�̘_���a	
	XOR,//�\�[�X�ƃf�X�e�B�l�[�V�����J���[�̔r���I�_���a	

	AND_REVERSE,//�\�[�X�ƃf�X�e�B�l�[�V�����J���[�̔��]�r�b�g�l�̘_����	
	OR_REVERSE,//�\�[�X�ƃf�X�e�B�l�[�V�����J���[�̔��]�r�b�g�l�̘_���a	
	
	AND_INVERTED,//�\�[�X�J���[�̔��]�r�b�g�l�ƁA�f�X�e�B�l�[�V�����J���[�̘_����	
	OR_INVERTED,//�\�[�X�J���[�̔��]�r�b�g�l�ƁA�f�X�e�B�l�[�V�����J���[�̘_���a	

	NAND,//�\�[�X�ƃf�X�e�B�l�[�V�����J���[�̘_���ς̔��]�r�b�g�l	
	NOR,//�\�[�X�ƃf�X�e�B�l�[�V�����J���[�̘_���a�̔��]�r�b�g�l						
	EQUIV,//�\�[�X�ƃf�X�e�B�l�[�V�����J���[�̔r���I�_���a�̔��]�r�b�g�l	
	NumberOf
DECLARE_ENUM_END( LogicOpType )

static inline LogicOpType::Enum GetLogicOpType( QString name )
{
	static const char* pTypeNames[16] = {
		"Clear",//	�S�Ẵr�b�g�l��0�ɂ��܂��B
		"Set",//	�S�Ẵr�b�g�l��1�ɂ��܂��B

		"Copy",//	�\�[�X�J���[�̃r�b�g�l���Ƃ�܂��B
		"NoOp",//	�f�B�X�e�B�l�[�V�����J���[�̃r�b�g�l���Ƃ�܂��B
		
		"CopyInverted",//	�\�[�X�J���[�̔��]�r�b�g�l���Ƃ�܂��B
		"Invert",//	�f�B�X�e�B�l�[�V�����J���[�̔��]�r�b�g�l���Ƃ�܂��B

		"And",//	�\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̘_���ς��Ƃ�܂��B
		"Or",//	�\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̘_���a���Ƃ�܂��B
		"Xor",//	�\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̔r���I�_���a���Ƃ�܂��B

		"AndReverse",//	�\�[�X�J���[�ƁA�f�B�X�e�B�l�[�V�����J���[�̔��]�r�b�g�l�̘_���ς��Ƃ�܂��B
		"OrReverse",//	�\�[�X�J���[�ƁA�f�B�X�e�B�l�[�V�����J���[�̔��]�r�b�g�l�̘_���a���Ƃ�܂��B

		"AndInverted",//	�\�[�X�J���[�̔��]�r�b�g�l�ƁA�f�B�X�e�B�l�[�V�����J���[�̘_���ς��Ƃ�܂��B
		"OrInverted"//	�\�[�X�J���[�̔��]�r�b�g�l�ƁA�f�B�X�e�B�l�[�V�����J���[�̘_���a���Ƃ�܂��B 
		
		"Nand",//	�\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̘_���ς̔��]�r�b�g�l���Ƃ�܂��B
		"Nor",//	�\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̘_���a�̔��]�r�b�g�l���Ƃ�܂��B
		"Equiv",//	�\�[�X�J���[�ƃf�B�X�e�B�l�[�V�����J���[�̔r���I�_���a�̔��]�r�b�g�l���Ƃ�܂��B
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
		"ConfigurationType0",//	�R���t�B�O���[�V�����^�C�v�O�ł��B
		"ConfigurationType1",//	�R���t�B�O���[�V�����^�C�v�P�ł��B
		"ConfigurationType2",//	�R���t�B�O���[�V�����^�C�v�Q�ł��B
		"ConfigurationType3",//	�R���t�B�O���[�V�����^�C�v�R�ł��B
		"ConfigurationType4",//	�R���t�B�O���[�V�����^�C�v�S�ł��B
		"ConfigurationType5",//	�R���t�B�O���[�V�����^�C�v�T�ł��B
		"ConfigurationType6",//	�R���t�B�O���[�V�����^�C�v�U�ł��B
		"ConfigurationType7",//	�R���t�B�O���[�V�����^�C�v�V�ł��B
		"ConfigurationType8",//	�R���t�B�O���[�V�����^�C�v8�ł��B
		"ConfigurationType9",//	�R���t�B�O���[�V�����^�C�v9�ł��B
		"ConfigurationType10"//	�R���t�B�O���[�V�����^�C�v10�ł��B 
	};

	for( int i = 0; i < 11; ++i )
	{
		if ( name == pTypeNames[i] )
			return i;
	}

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ���Z�^�C�v
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
		"Replace",//	�I�y���[�V�����O�݂̂��g�p����܂��B Result = Arg0
		"Modulate",//	�I�y���[�V�����O�ƃI�y���[�V�����P����Z���܂��B Result = Arg0 + Arg1
		"Add",//	�I�y���[�V�����O�ƃI�y���[�V�����P�����Z���܂��B Result = Arg0 + Arg1
		"AddSigned",//	�I�y���[�V�����O�ƃI�y���[�V�����P�̉��Z����0.5�������܂��B Result = Arg0 + Arg1 - 0.5
		"Interpolate",//	�I�y���[�V�����O�ƃI�y���[�V�����P���I�y���[�V�����Q��p���ĕ�Ԃ��܂��B Result = Arg0 * Arg2 + Arg1 * (1 - Arg2)

		"Subtract",//	�I�y���[�V�����O����I�y���[�V�����P�����Z���܂��B Result = Arg0 - Arg1
		"DotProduct3Rgb",//	�I�y���[�V�����O�ƃI�y���[�V�����P�̓��ς�RGB�����ꂼ��u�������܂��B ResultRGB = 4 * ((Arg0r - 0.5) * (Arg1r - 0.5) + (Arg0g - 0.5) * (Arg1g - 0.5) + (Arg0b - 0.5) * (Arg1b - 0.5))
		"DotProduct3Rgba",//	�I�y���[�V�����O�ƃI�y���[�V�����P�̓��ς�RGBA�����ꂼ��u�������܂��B ResultRGBA = 4 * ((Arg0r - 0.5) * (Arg1r - 0.5) + (Arg0g - 0.5) * (Arg1g - 0.5) + (Arg0b - 0.5) * (Arg1b - 0.5))
		"AddMult",//	�I�y���[�V�����O�ƃI�y���[�V�����P�̉��Z���I�y���[�V�����Q�Ə�Z���܂��B Result = (Arg0 + Arg1) * Arg2
		"MultAdd"//	�I�y���[�V�����O�ƃI�y���[�V�����P�̏�Z���I�y���[�V�����Q�Ɖ��Z���܂��B Result = (Arg0 * Arg1) + Arg2 
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
		"One",//	���{�X�P�[�����[�h�ł��B
		"Two",//	�Q�{�X�P�[�����[�h�ł��B
		"Four",//	�S�{�X�P�[�����[�h�ł��B 
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
		"Constant0",//	�R���X�^���g�J���[�ł��B
		"Constant1",//	��Q�̃R���X�^���g�J���[�ł��B
		"Constant2",//	��R�̃R���X�^���g�J���[�ł��B
		"Constant3",//	��S�̃R���X�^���g�J���[�ł��B
		"Constant4",//	��T�̃R���X�^���g�J���[�ł��B

		"Constant5",//	��U�̃R���X�^���g�J���[�ł��B
		"Emission",//	�G�~�b�V�����J���[�ł��B
		"Ambient",//	�A���r�G���g�J���[�ł��B
		"Diffuse",//	�f�B�t���[�Y�J���[�ł��B
		"Specular0",//	�X�y�L�����[�J���[�ł��B

		"Specular1"//	��Q�̃X�y�L�����[�J���[�ł��B 
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
//!  ���̓^�C�v
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
		"Texture0",//	�e�N�X�`���O�ł��B
		"Texture1",//	�e�N�X�`���P�ł��B
		"Texture2",//	�e�N�X�`���Q�ł��B
		"Texture3",//	�e�N�X�`���R�ł��B �v���V�[�W�����e�N�X�`���ɂȂ�܂��B
		"Constant",//	�R���X�^���g�J���[�ł��B

		"PrimaryColor",//	�e�N�X�`�������ɐ旧���e�N�X�`���R���o�C�i�O�ɓ����Ă���t���O�����g�J���[�ł��B
		"FragmentPrimaryColor",//	�t���O�����g���C�g�̃v���C�}���[�J���[�ł��B �f�B�t���[�Y�A�A���r�G���g�A�G�~�b�V�u��\���܂��B
		"FragmentSecondaryColor",//	�t���O�����g���C�g�̃Z�J���_���[�J���[�ł��B �X�y�L�����[��\���܂��B 
		"Previous",//	�O�̃e�N�X�`���R���o�C�i��������Ă���t���O�����g�J���[�ł��B
		"PreviousBuffer",//	�O�̃e�N�X�`���R���o�C�i�̃o�b�t�@��������Ă���J���[�ł��B
	};

	for( int i = 0; i < 10; ++i )
	{
		if ( name == pTypeNames[i] )
			return i;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ���͗v�f�^�C�v
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
		"Color",//	�\�[�X�J���[�ł��B
		"OneMinusColor",//	1-Color�ł��B
		"Alpha",//	�\�[�X�̃A���t�@�l�ł��B
		"OneMinusAlpha",//	1-Alpha�ł��B
		"Red",//	�\�[�X��Red�l�ł��B

		"OneMinusRed",//	1-Red�ł��B
		"Green",//	�\�[�X��Green�l�ł��B
		"OneMinusGreen",//	1-Green�ł��B
		"Blue",//	�\�[�X��Blue�l�ł��B
		"OneMinusBlue"//	1-Blue�ł��B 
	};

	for( int i = 0; i < 10; ++i )
	{
		if ( name == pTypeNames[i] )
			return i;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ���͊p�^�C�v
DECLARE_ENUM_BEGIN( LutInputAngleType )
	NH = 0,//�@���ƃn�[�t�x�N�g�� �i�f�t�H���g�j
	VH,//�r���[�x�N�g���ƃn�[�t�x�N�g��
	NV,//�@���ƃr���[�x�N�g��
	LN,//���C�g�x�N�g���Ɩ@��
	SP,//���C�g�x�N�g���̋t�x�N�g���ƃX�|�b�g���C�g�̕����x�N�g���iRR, RG, RB, FR
	CP,//�n�[�t�x�N�g���̐ڕ��ʂւ̓��e�Ɛڐ��iRR, RG, RB, FR �͕s�j
	NumberOf
DECLARE_ENUM_END( LutInputAngleType )

static inline int GetLutInputAngleType( QString name )
{
	static const char* pTypeNames[6] = {
		"CosNormalHalf",//	�@���ƃn�[�t�x�N�g���̂Ȃ��p�̃R�T�C���l�ł��B
		"CosViewHalf",//	�����x�N�g���ƃn�[�t�x�N�g���̂Ȃ��p�̃R�T�C���l�ł��B
		"CosNormalView",//	�@���Ǝ����x�N�g���̂Ȃ��p�̃R�T�C���l�ł��B
		"CosLightNormal",//	���C�g�x�N�g���̖@���̂Ȃ��p�̃R�T�C���l�ł��B
		"CosLightSpot",//	���C�g�x�N�g���ƃX�|�b�g�����x�N�g���̂Ȃ��p�̃R�T�C���l�ł��B
		"CosPhi"//	�n�[�t�x�N�g���̐ڕ��ʓ��e�x�N�g���Ɛڐ��x�N�g���̂Ȃ��p�̃R�T�C���l�ł��B
	};

	for( int i = 0; i < 6; ++i )
	{
		if ( name == pTypeNames[i] )
			return i;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  �Q�ƃe�[�u�����C�e�B���O���ʃX�P�[���^�C�v
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
		"Quarter",//	0.25�{�ł��B
		"Half",//	0.5�{�ł��B
		"One",//	1�{�ł��B
		"Two",//	2�{�ł��B
		"Four",//	4�{�ł��B
		"Eight"//	8�{�ł��B
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
//!  �[�x�e�X�g�^�C�v
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
//!  �X�e���V���e�X�g�^�C�v
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
//!  �X�e���V���I�y���[�V�����^�C�v
DECLARE_ENUM_BEGIN( StencilOpType )
	KEEP = 0,		//	�X�e���V���l�͌��݂̒l��ێ����܂��B
	ZERO,				//	�X�e���V���l��0�Œu���������܂��B
	REPLACE,		//	�X�e���V���l��glStencilFunc��ref�Ŏw�肵���l�Œu���������܂��B
	INCR,				//	�X�e���V���l��1���Z����܂��B
	INCR_WRAP,	//	�X�e���V���l��1���Z����܂��B
	DECR,				//	�X�e���V���l��1���Z����܂��B
	DECR_WRAP,	//	�X�e���V���l��1���Z����܂��B
	INVERT,			//	�X�e���V���l�̓r�b�g���]���܂��B
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
//!  �A���t�@�e�X�g�^�C�v
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
//!  �u�����h�^�C�v
DECLARE_ENUM_BEGIN( BlendType )
	None = 0,
	Nuki,			//! �����ݒ�
	Liner,		//! ���`�⊮
	Add,			//! ���Z
	Sub,			//! ���Z
	Mul,			//! ��Z
	Inverse,	//! �l�K�|�W���]
	NumberOf
DECLARE_ENUM_END( BlendType )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  �u�����h�֐��^�C�v
DECLARE_ENUM_BEGIN( BlendFunc )
	SrcAddDst = 0,
	SrcSubDst,
	DstSubSrc,
	Min,
	Max,
	NumberOf
DECLARE_ENUM_END( BlendFunc )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  �u�����h�֐��^�C�v
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
//!  �|�P�����@���^�C�v
DECLARE_ENUM_BEGIN( PokeNormalType )
	None = 0,
	Dummy,
	ObjectSpace,
	NumberOf
DECLARE_ENUM_END( PokeNormalType )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  �|�P�����@���^�C�v
DECLARE_ENUM_BEGIN( LutInputAngle )
	NH = 0,//�@���ƃn�[�t�x�N�g�� �i�f�t�H���g�j
	VH,//�r���[�x�N�g���ƃn�[�t�x�N�g��
	NV,//�@���ƃr���[�x�N�g��
	LN,//���C�g�x�N�g���Ɩ@��
	NumberOf
DECLARE_ENUM_END( LutInputAngle )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  �G�b�W�^�C�v(���[�U�[�A�g���r���[�g)
DECLARE_ENUM_BEGIN( EdgeType )
	Default = 0,
	VColor,
	//Dot,
	None,
	NumberOf
DECLARE_ENUM_END( EdgeType )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  �\�����C�^�[�^�C�v
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
		
		MaterialColor								m_MaterialColor;													//�J���[/�}�e���A���J���[
		Math::Vector								m_ConsntantColor[CombinerMax];						//�J���[/�R���X�^���g�J���[
		GLint												m_hConsntantColor[CombinerMax];
		EtcColor										m_EtcColor;																//�J���[/���̑��̃J���[

		short												m_BumpMapNo;															//�e�N�X�`��/�o���v�}�b�s���O/�o���v�g�p�e�N�X�`��
		short												m_BumpMapType;														//�e�N�X�`��/�o���v�}�b�s���O/�g�p���@
		bool												m_ZRecastEnable;													//�e�N�X�`��/�o���v�}�b�s���O/�y�����̍Čv�Z
		GLint												m_hBumpMapNo;
		GLint												m_hBumpMapType;
		GLint												m_hZRecastEnable;
		GLint												m_hTangentNo;

		TextureAttribute						m_TextureAttribute[TextureMax];						//�e�N�X�`���ꗗ/�e�N�X�`��
	  bool												m_TextureAttributeEnable[TextureMax];
	  GLint												m_hTextureAttributeEnable[TextureMax];
	  GLint												m_hIsTexture2D[TextureMax];

		bool												m_LookUpTableEnable[LookUpTableType::NumberOf];
		GLint												m_hLookUpTableEnable[LookUpTableType::NumberOf];
		LookUpTable									m_LookUpTable[LookUpTableType::NumberOf];	//�t���O�����g/�t���O�����g���C�e�B���O
		bool												m_PrimaryFresnelEnable;										//�t���O�����g/�t���O�����g���C�e�B���O/�t���l�����v���C�}���ɓK��
		bool												m_SecondaryFresnelEnable;									//�t���O�����g/�t���O�����g���C�e�B���O/�t���l�����Z�J���_���ɓK��
		bool												m_SpecularColorClamp;											//�t���O�����g/�t���O�����g���C�e�B���O/�X�y�L�������N�����v
		bool												m_CookTorrance0Enable;										//�t���O�����g/�t���O�����g���C�e�B���O/�W�I���g���t�@�N�^�O
		bool												m_CookTorrance1Enable;										//�t���O�����g/�t���O�����g���C�e�B���O/�W�I���g���t�@�N�^�O
		GLint												m_hPrimaryFresnelEnable;
		GLint												m_hSecondaryFresnelEnable;
		GLint												m_hSpecularColorClamp;
		GLint												m_hCookTorrance0Enable;
		GLint												m_hCookTorrance1Enable;

		TextureCombinerAttribute		m_TextureCombinerAttribute[CombinerColorType::NumberOf][CombinerMax];	//�t���O�����g/�e�N�X�`���R���o�C�i
		short												m_TextureCombinerConstant[CombinerMax];																//�t���O�����g/�e�N�X�`���R���o�C�i/�e�R���X�^���g�J���[
		GLint												m_hTextureCombinerConstant[CombinerMax];

		short												m_DrawLayer;															//�u�����h/�e�X�g/�`�惌�C���[
		short												m_AlphaTestType;													//�u�����h/�e�X�g/�A���t�@�e�X�g/�������Βʉ�
		float												m_AlphaTestRefValue;											//�u�����h/�e�X�g/�A���t�@�e�X�g/�Q�ƒl
		short												m_DepthTestType;													//�u�����h/�e�X�g/�f�v�X�e�X�g/�������Βʉ�
		bool												m_UpDateDepth;														//�u�����h/�e�X�g/�f�v�X�e�X�g/�f�v�X�o�b�t�@���X�V����
		int													m_PolygonOffSet;													//�u�����h/�e�X�g/�f�v�X�e�X�g/�|���S���I�t�Z�b�g


		short												m_StencilTestEnable;											//�u�����h/�e�X�g/�X�e���V���e�X�g/
		short												m_StencilTestType;												//�u�����h/�e�X�g/�X�e���V���e�X�g/
		short												m_StencilValue;														//�u�����h/�e�X�g/�X�e���V���e�X�g/
		short												m_StencilMask;														//�u�����h/�e�X�g/�X�e���V���e�X�g/
		short												m_StencilFailOp;													//�u�����h/�e�X�g/�X�e���V���e�X�g/
		short												m_StencilZFailOp;													//�u�����h/�e�X�g/�X�e���V���e�X�g/
		short												m_StencilZpassOp;													//�u�����h/�e�X�g/�X�e���V���e�X�g/


		short												m_BlendMode;															//�u�����h/�e�X�g/�u�����_�[/�u�����h���[�h
		BlendFunc										m_BlendFunc[CombinerColorType::NumberOf];	//�u�����h/�e�X�g/�u�����_�[/�t�����h���e��
		short												m_LogicalOperation;												//�u�����h/�e�X�g/�u�����_�[/�_�����Z���@

		VertexShaderType						m_VertexShaderType;
		bool												m_IsPokemonShader;//CR�l�̑Ή�������������폜
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
		

    //��������
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
