//
// Copyright (C) PokemonShaderNode
// 
// File: PokemonShaderNodeNode.cpp
//
// Dependency Graph Node: PokemonShaderNode
//
// Author: Maya Plug-in Wizard 2.0
//

#include "qt/Shader/ShaderObject.h"
#include "qt/Shader/CombinerShader.h"

#include "qt/Shader/String/Util.h"
#include "qt/Shader/String/LogPrint.h"
#include "qt/Shader/Common/SecureData.h"
#include <assert.h>

using namespace gfl::maya::plugins::Shader;

namespace gfl { namespace maya { namespace plugins { namespace Shader {


GF_ALIGNED(0x80) static const unsigned char  s_CombinerVertexShaderCode[] = {
	#include "CombinerShader.vert.txt"
};

GF_ALIGNED(0x80) static const unsigned char  s_CombinerPixelShaderCode[] = {
	#include "CombinerShader.frag.txt"
};

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■				デフォルト頂点シェーダー
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//-----------------------------------------------------------------------------
/// @brief	コンストラクタ
//-----------------------------------------------------------------------------
CombinerVertexShader::CombinerVertexShader()
{
	createVertexShader( Connection::PixelShader);
}

//-----------------------------------------------------------------------------
/// @brief	コンストラクタ
//-----------------------------------------------------------------------------
CombinerVertexShader::CombinerVertexShader( Connection::Enum type )
{
	createVertexShader( type );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerVertexShader::createVertexShader( Connection::Enum type )
{
	//---- 頂点シェーダオブジェクトの作成 ----//
	m_hProgram = glCreateShader(GL_VERTEX_SHADER);

	Common::SecureData	data = Common::SecureData::DeCode( s_CombinerVertexShaderCode, sizeof(s_CombinerVertexShaderCode) );
	const char			*vfile = (char*)data.GetData();
	std::string			shaderCodeStr = vfile;

	String::Util::ReplaceStr( shaderCodeStr, "<OutName>", (type == Connection::GeometryShader) ? "vs_out" : "out" );

	vfile = shaderCodeStr.c_str();

	//ソースコードをシェーダオブジェクトに変換
	glShaderSource(m_hProgram, 1, &vfile,NULL);
	glCompileShader(m_hProgram);//コンパイル

	int	compiled;
	glGetShaderiv(m_hProgram, GL_COMPILE_STATUS, &compiled);

	if ( compiled == GL_FALSE )
	{
		assert(0);
	}

	GL_ERR();
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■				デフォルトピクセルシェーダービルダー
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
class CombinerShaderBuilder{
public:
	static std::string BuildPixelShader( Node::CombinerAttribute::AttributeParam *pAttrParam );

private:

	static std::string CreateCombinerRGBFuncCode( int stage, Node::CombinerAttribute::AttributeParam *pAttrParam );
	static std::string CreateCombinerAlphaFuncCode( int stage, Node::CombinerAttribute::AttributeParam *pAttrParam );

	static std::string CreateGetSourceCode( int stage, int sourceType, const Node::CombinerAttribute::TextureCombinerAttribute *pAttr );

	CombinerShaderBuilder(){}
	~CombinerShaderBuilder(){}
};

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
std::string CombinerShaderBuilder::BuildPixelShader( Node::CombinerAttribute::AttributeParam *pAttrParam )
{
	static const std::string	defaultSource = "for( int stageNo = 0; stageNo < cCombinerStageMax; stageNo++ ){ combinerRgbFunc( stageNo );	combinerAlphaFunc( stageNo );	}";
	if ( pAttrParam == NULL )
		return defaultSource;

	std::string								makeCode;

	for( int i = 0; i < Node::CombinerAttribute::CombinerMax; ++i )
	{
		makeCode += CreateCombinerRGBFuncCode( i, pAttrParam );
		makeCode += CreateCombinerAlphaFuncCode( i, pAttrParam );
	}

	if ( 0 )
	{
		static const std::string			buildFile = "C:/Users/ishiguro_masateru/Desktop/buildShader.frag";
		String::LogPrint::Initialize( String::LogPrint::Text, buildFile.c_str() );
		String::LogPrint::PrintStr( makeCode );
		String::LogPrint::Terminate();
	}
	
	return makeCode;
	//return defaultSource;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
std::string CombinerShaderBuilder::CreateGetSourceCode( int stage, int sourceType, const Node::CombinerAttribute::TextureCombinerAttribute *pAttr )
{
	std::string									makeCode;

	static const std::string		inputSourceCode[CombInputSourceType::NumberOf] = {
		"sourceRgba = g_vTexture[0];",
		"sourceRgba = g_vTexture[1];",
		"sourceRgba = g_vTexture[2];",
		"sourceRgba = g_vTexture[3];",
		"sourceRgba = g_vConstnat[<stageNo>];",
		"sourceRgba = g_vVSColor;",
		"sourceRgba = g_vPrimary;",
		"sourceRgba = g_vSecondary;",
		"sourceRgba = g_vAnswer;",
		"sourceRgba = g_vBuffer;"
	};

	static const std::string		sourceOperandCode[OperandType::NumberOf] = {
		"sourceRgba = sourceRgba;",
		"sourceRgba = 1.0 - sourceRgba;",
		"sourceRgba = sourceRgba.wwww;",
		"sourceRgba = 1.0 - sourceRgba.wwww;",
		"sourceRgba = sourceRgba.xxxx;",
		"sourceRgba = 1.0 - sourceRgba.xxxx;",
		"sourceRgba = sourceRgba.yyyy;",
		"sourceRgba = 1.0 - sourceRgba.yyyy;",
		"sourceRgba = sourceRgba.zzzz;",
		"sourceRgba = 1.0 - sourceRgba.zzzz;"
	};

	makeCode += std::string("  ") + inputSourceCode[pAttr->m_InputSource[sourceType]] + std::string("\n");
	makeCode += std::string("  ") + sourceOperandCode[pAttr->m_Operand[sourceType]] + std::string("\n");

	return makeCode;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
std::string CombinerShaderBuilder::CreateCombinerRGBFuncCode( int stage, Node::CombinerAttribute::AttributeParam *pAttrParam )
{
	const Node::CombinerAttribute::TextureCombinerAttribute				*pAttr = &pAttrParam->m_TextureCombinerAttribute[CombinerColorType::RGB][stage];

	if ( stage )
	{
		if ( pAttr->m_MathFormula == MathFormulaType::Replace &&
			pAttr->m_InputSource[InputSourceType::A] == CombInputSourceType::Previous &&
			pAttr->m_Operand[InputSourceType::A] == OperandType::Color &&
			pAttr->m_OutPutBuffer == false )
		{//何もしなくて良い。
			return "";
		}
	}

	std::string									makeCode;

	static const std::string		funcCodes[MathFormulaType::NumberOf] = {
		"g_vAnswer.xyz = sourceA;",
		"g_vAnswer.xyz = sourceA * sourceB;",
		"g_vAnswer.xyz = sourceA + sourceB;",
		"g_vAnswer.xyz = sourceA + sourceB - 0.5;",
		"g_vAnswer.xyz = (sourceA * sourceC) + (sourceB * (1 - sourceC));",
		"g_vAnswer.xyz = sourceA - sourceB;",
		"g_vAnswer.xyz = dot(sourceA, sourceB);",
		"g_vAnswer.xyz = dot(sourceA, sourceB);",
		"g_vAnswer.xyz = (sourceA + sourceB) * sourceC;",
		"g_vAnswer.xyz = (sourceA * sourceB) + sourceC;"
	};


	makeCode += "{\n";

	makeCode += "  vec4	sourceRgba;\n";
	makeCode += "  vec3	sourceA;\n";
	makeCode += "  vec3	sourceB;\n";
	makeCode += "  vec3	sourceC;\n";

	makeCode += CreateGetSourceCode( stage, InputSourceType::A, pAttr );
	makeCode += "  sourceA = sourceRgba.xyz;\n";
	makeCode += CreateGetSourceCode( stage, InputSourceType::B, pAttr );
	makeCode += "  sourceB = sourceRgba.xyz;\n";
	makeCode += CreateGetSourceCode( stage, InputSourceType::C, pAttr );
	makeCode += "  sourceC = sourceRgba.xyz;\n";

	makeCode += "  int		mathFormulaType = cMathFormula[ColorTypeRGB][<stageNo>];\n";

	makeCode += std::string("  ") + funcCodes[pAttr->m_MathFormula] + std::string("\n");

	makeCode += "  g_vAnswer.xyz = g_vAnswer.xyz * cScale[ColorTypeRGB][<stageNo>];\n";

	if ( pAttr->m_OutPutBuffer )
	{
		makeCode += "  g_vBuffer.xyz = g_vAnswer.xyz;\n";
	}
	
	makeCode += "  finalColorRgb.xyz = g_vAnswer.xyz;\n";
	//makeCode += "  finalColorRgb.xyz = vec3(0.0, 1.0, 0.0);\n";
	makeCode += "}\n";

	{
		char buff[5];
		_itoa_s( stage, buff, sizeof(buff), 10 );
		String::Util::ReplaceStr( makeCode, "<stageNo>", buff );
	}

	return makeCode;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
std::string CombinerShaderBuilder::CreateCombinerAlphaFuncCode( int stage, Node::CombinerAttribute::AttributeParam *pAttrParam )
{
	const Node::CombinerAttribute::TextureCombinerAttribute				*pAttr = &pAttrParam->m_TextureCombinerAttribute[CombinerColorType::A][stage];

	if ( stage )
	{
		if ( pAttr->m_MathFormula == MathFormulaType::Replace &&
				 pAttr->m_InputSource[InputSourceType::A] == CombInputSourceType::Previous &&
				 pAttr->m_Operand[InputSourceType::A] == OperandType::Alpha &&
				 pAttr->m_OutPutBuffer == false )
		{//何もしなくて良い。
			return "";
		}
	}

	std::string									makeCode;

	static const std::string		funcCodes[MathFormulaType::NumberOf] = {
		"g_vAnswer.w = sourceA;",
		"g_vAnswer.w = sourceA * sourceB;",
		"g_vAnswer.w = sourceA + sourceB;",
		"g_vAnswer.w = sourceA + sourceB - 0.5;",
		"g_vAnswer.w = (sourceA * sourceC) + (sourceB * (1 - sourceC));",
		"g_vAnswer.w = sourceA - sourceB;",
		"g_vAnswer.w = dot(sourceA, sourceB);",
		"g_vAnswer.w = dot(sourceA, sourceB);",
		"g_vAnswer.w = (sourceA + sourceB) * sourceC;",
		"g_vAnswer.w = (sourceA * sourceB) + sourceC;"
	};

	makeCode += "{\n";
	makeCode += "  vec4	  sourceRgba;\n";
	makeCode += "  float	sourceA;\n";
	makeCode += "  float	sourceB;\n";
	makeCode += "  float	sourceC;\n";
	makeCode += "  int		mathFormulaType = cMathFormula[ColorTypeA][<stageNo>];\n";

	makeCode += CreateGetSourceCode( stage, InputSourceType::A, pAttr );
	makeCode += "  sourceA = sourceRgba.w;\n";
	makeCode += CreateGetSourceCode( stage, InputSourceType::B, pAttr );
	makeCode += "  sourceB = sourceRgba.w;\n";
	makeCode += CreateGetSourceCode( stage, InputSourceType::C, pAttr );
	makeCode += "  sourceC = sourceRgba.w;\n";

	makeCode += std::string("  ") + funcCodes[pAttr->m_MathFormula] + std::string("\n");

	makeCode += "  g_vAnswer.w = g_vAnswer.w * cScale[ColorTypeA][<stageNo>];\n";

	if ( pAttr->m_OutPutBuffer )
	{
		makeCode += "  g_vBuffer.w = g_vAnswer.w;\n";
	}
	
	makeCode += "  finalColorRgb.w = g_vAnswer.w;\n";
	makeCode += "}\n";

	{
		char buff[5];
		_itoa_s( stage, buff, sizeof(buff), 10 );
		String::Util::ReplaceStr( makeCode, "<stageNo>", buff );
	}

	return makeCode;
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■				デフォルトピクセルシェーダー
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//-----------------------------------------------------------------------------
/// @brief	コンストラクタ
//-----------------------------------------------------------------------------
CombinerPixelShader::CombinerPixelShader()
{
	createPixelShader();
}

CombinerPixelShader::CombinerPixelShader( void *pParam )
{
	createPixelShader( pParam );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void CombinerPixelShader::createPixelShader( void *pParam )
{
	Node::CombinerAttribute::AttributeParam *pAttrParam = reinterpret_cast< Node::CombinerAttribute::AttributeParam* >( pParam );

	Common::SecureData	data = Common::SecureData::DeCode( s_CombinerPixelShaderCode, sizeof(s_CombinerPixelShaderCode) );
	std::string		sourceBody = (char*)data.GetData();

	//---- フラグメントシェーダオブジェクトの作成 ----//
	m_hProgram = glCreateShader(GL_FRAGMENT_SHADER);

	//ソースコードをシェーダオブジェクトに変換
	{
		std::string	buildSource = CombinerShaderBuilder::BuildPixelShader( pAttrParam );
		String::Util::ReplaceStr( sourceBody, "<include>", buildSource );
		
		const char *ffile = sourceBody.c_str();

		if ( 0 )
		{
			static const std::string			buildFile = "C:/Users/ishiguro_masateru/Desktop/buildShader.frag";
			String::LogPrint::Initialize( String::LogPrint::Text, buildFile.c_str() );
			String::LogPrint::PrintStr( sourceBody );
			String::LogPrint::Terminate();
		}

		glShaderSource( m_hProgram, 1, &ffile, NULL );
		glCompileShader(m_hProgram);//コンパイル
	}

	int	compiled;
	glGetShaderiv(m_hProgram, GL_COMPILE_STATUS, &compiled);

  if ( compiled == GL_FALSE )
	{
		{
			/* シェーダコンパイルのログバッファ */
			#define MAX_SHADER_LOG_SIZE		(1024*10)
			char s_logBuffer[MAX_SHADER_LOG_SIZE];

			int logSize;
			int length;

			/* ログの長さは、最後のNULL文字も含む */
			glGetShaderiv(m_hProgram, GL_INFO_LOG_LENGTH , &logSize);

			if (logSize > 1)
			{
				glGetShaderInfoLog(m_hProgram, MAX_SHADER_LOG_SIZE, &length, s_logBuffer);
				OutputDebugStringA( s_logBuffer );
				//String::Util::DebugPrint( MString( s_logBuffer ) );
			}
		}

		assert(0);
	}

	GL_ERR();
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerPixelShader::vSetUpShader() const
{
	//テクスチャユニット設定
	glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "Texture[0]"), 0 );
	glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "Texture[1]"), 1 );
	glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "Texture[2]"), 2 );
	glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "CubeTexture[0]"), 3 );
	glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "CubeTexture[1]"), 4 );
	glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "CubeTexture[2]"), 5 );

	glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "LutTexture[0]"), 6 );
	glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "LutTexture[1]"), 7 );
	glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "LutTexture[2]"), 8 );
	glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "LutTexture[3]"), 9 );
	glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "LutTexture[4]"), 10 );
	glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "LutTexture[5]"), 11 );

	glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "HighLightTexture"), 12 );
	glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "PaintedTexture"), 13 );
	glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "PaintTexture"), 14 );
	glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "DepthTexture"), 15 );
}




}}}}
