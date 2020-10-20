#include "qt/Shader/CombinerAttribute.h"

namespace gfl { namespace maya { namespace plugins { namespace Node {

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
bool VertexShaderType::IsPokemonShader() const
{
	switch( this->m_Value ){
	case Poke:
	case PokeNormal:
	case MegaGangar:
		return true;
	default:
		return false;
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
bool VertexShaderType::Is( UInt32 types ) const
{
	return (this->m_Value & types) ? true : false;
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerAttribute::AttributeParam::InitTextureCombinerAttribute()
{
	//コンバイナ初期化
  for( int i = 0; i < CombinerAttribute::CombinerMax; ++i )
  {
	  this->m_TextureCombinerAttribute[CombinerColorType::RGB][i].m_MathFormula = MathFormulaType::Replace;
	  this->m_TextureCombinerAttribute[CombinerColorType::A][i].m_MathFormula = MathFormulaType::Replace;

	  for( int i2 = 0; i2 < InputSourceType::NumberOf; ++i2 )
	  {
		  this->m_TextureCombinerAttribute[CombinerColorType::RGB][i].m_Operand[i2] = OperandType::Color;
		  this->m_TextureCombinerAttribute[CombinerColorType::A][i].m_Operand[i2] = OperandType::Alpha;

		  if ( i )
		  {
			  this->m_TextureCombinerAttribute[CombinerColorType::RGB][i].m_InputSource[i2] = CombInputSourceType::Previous;
			  this->m_TextureCombinerAttribute[CombinerColorType::A][i].m_InputSource[i2] = CombInputSourceType::Previous;
				
		  }
		  else
		  {
			  this->m_TextureCombinerAttribute[CombinerColorType::RGB][i].m_InputSource[i2] = CombInputSourceType::Texture0;
			  this->m_TextureCombinerAttribute[CombinerColorType::A][i].m_InputSource[i2] = CombInputSourceType::Texture0;
		  }
	  }
  }
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerAttribute::AttributeParam::CheckBumpMapEnable()
{
  if ( this->m_TextureAttributeEnable[ this->m_BumpMapNo ] == false )
  {
    this->m_BumpMapNo = -1;//無効に設定
  }
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerAttribute::AttributeParam::ShiftNullTextureNo()
{
  //若い番号のテクスチャが未設定で、それ以降のテクスチャが設定されているとき、番号を詰めて出力する。
  if ( this->m_BumpMapNo >= 0 )
  {//まずは法線マップ番号調整
    int     bumpShift = 0;

    for( int i = 0; i < TextureMax; ++i )
    {
      if ( this->m_TextureAttributeEnable[ i ] == false )
      {
        if ( this->m_BumpMapNo >= i )
        {
          ++bumpShift;
        }
      }
    }

    this->m_BumpMapNo -= bumpShift;
  }

  {
    short       textureShift[TextureMax] = { CombInputSourceType::Texture0, CombInputSourceType::Texture1, CombInputSourceType::Texture2 };

    for( int i = 0; i < TextureMax; ++i )
    {
      if ( this->m_TextureAttributeEnable[ i ] == false )
      {
        for( int i2 = i+1; i2 < TextureMax; ++i2 )
        {
          --textureShift[i2];
        }
      }
    }

    this->textureShift( textureShift[0], textureShift[1], textureShift[2] );
  }
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerAttribute::AttributeParam::ProjectionTextureShift()
{
  //ひとつ上にずらす。０番は投影テクスチャが入る。
  this->textureShift( CombInputSourceType::Texture1, CombInputSourceType::Texture2, CombInputSourceType::Texture2 );

  if ( this->m_BumpMapNo >= 0 )
  {
    ++this->m_BumpMapNo;
  }

	int			texCnt = 0;

	for( int i = 0; i < TextureMax; ++i )
	{
		if ( m_TextureAttributeEnable[i] )
		{
			++texCnt;
		}
	}

	m_TextureAttributeEnable[texCnt] = true;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerAttribute::AttributeParam::SetDropShadowCombiner()
{
	//ランタイムで使用するので０クリア
	m_ConsntantColor[5] = Math::Vector( 0.0f, 0.0f, 0.0f, 0.0f );
	m_TextureCombinerAttribute[CombinerColorType::RGB][3].m_OutPutBuffer = true;
	
	//コンバイナの４段目と５段目の全アトリビュート
	m_TextureCombinerConstant[4] = 5;

	m_TextureCombinerAttribute[CombinerColorType::RGB][4].m_InputSource[InputSourceType::A] = CombInputSourceType::Texture0;
	m_TextureCombinerAttribute[CombinerColorType::RGB][4].m_Operand[InputSourceType::A] = OperandType::OneMinusColor;
	m_TextureCombinerAttribute[CombinerColorType::RGB][4].m_InputSource[InputSourceType::B] = CombInputSourceType::Constant;
	m_TextureCombinerAttribute[CombinerColorType::RGB][4].m_Operand[InputSourceType::B] = OperandType::Color;
	m_TextureCombinerAttribute[CombinerColorType::RGB][4].m_MathFormula = MathFormulaType::Modulate;
	m_TextureCombinerAttribute[CombinerColorType::RGB][4].m_Scale = 0;

	m_TextureCombinerAttribute[CombinerColorType::A][4].m_InputSource[InputSourceType::A] = CombInputSourceType::Previous;
	m_TextureCombinerAttribute[CombinerColorType::A][4].m_Operand[InputSourceType::A] = OperandType::Alpha;
	m_TextureCombinerAttribute[CombinerColorType::A][4].m_MathFormula = MathFormulaType::Replace;
	m_TextureCombinerAttribute[CombinerColorType::A][4].m_Scale = 0;

	m_TextureCombinerAttribute[CombinerColorType::RGB][5].m_InputSource[InputSourceType::A] = CombInputSourceType::PreviousBuffer;
	m_TextureCombinerAttribute[CombinerColorType::RGB][5].m_Operand[InputSourceType::A] = OperandType::Color;
	m_TextureCombinerAttribute[CombinerColorType::RGB][5].m_InputSource[InputSourceType::B] = CombInputSourceType::Previous;
	m_TextureCombinerAttribute[CombinerColorType::RGB][5].m_Operand[InputSourceType::B] = OperandType::OneMinusColor;
	m_TextureCombinerAttribute[CombinerColorType::RGB][5].m_MathFormula = MathFormulaType::Modulate;
	m_TextureCombinerAttribute[CombinerColorType::RGB][5].m_Scale = 0;

	m_TextureCombinerAttribute[CombinerColorType::A][5].m_InputSource[InputSourceType::A] = CombInputSourceType::Previous;
	m_TextureCombinerAttribute[CombinerColorType::A][5].m_Operand[InputSourceType::A] = OperandType::Alpha;
	m_TextureCombinerAttribute[CombinerColorType::A][5].m_MathFormula = MathFormulaType::Replace;
	m_TextureCombinerAttribute[CombinerColorType::A][5].m_Scale = 0;

	m_TextureAttribute[0].m_MappingType = 3;//投影
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerAttribute::AttributeParam::SetDropLightCombiner()
{
	//ランタイムで使用するので０クリア
	m_ConsntantColor[5] = Math::Vector( 0.0f, 0.0f, 0.0f, 0.0f );
	m_TextureCombinerAttribute[CombinerColorType::RGB][3].m_OutPutBuffer = true;
	
	//コンバイナの４段目と５段目の全アトリビュート
	m_TextureCombinerConstant[4] = 5;

	m_TextureCombinerAttribute[CombinerColorType::RGB][4].m_InputSource[InputSourceType::A] = CombInputSourceType::Texture0;
	m_TextureCombinerAttribute[CombinerColorType::RGB][4].m_Operand[InputSourceType::A] = OperandType::Color;
	m_TextureCombinerAttribute[CombinerColorType::RGB][4].m_InputSource[InputSourceType::B] = CombInputSourceType::Constant;
	m_TextureCombinerAttribute[CombinerColorType::RGB][4].m_Operand[InputSourceType::B] = OperandType::Color;
	m_TextureCombinerAttribute[CombinerColorType::RGB][4].m_MathFormula = MathFormulaType::Modulate;
	m_TextureCombinerAttribute[CombinerColorType::RGB][4].m_Scale = 0;

	m_TextureCombinerAttribute[CombinerColorType::A][4].m_InputSource[InputSourceType::A] = CombInputSourceType::Previous;
	m_TextureCombinerAttribute[CombinerColorType::A][4].m_Operand[InputSourceType::A] = OperandType::Alpha;
	m_TextureCombinerAttribute[CombinerColorType::A][4].m_MathFormula = MathFormulaType::Replace;
	m_TextureCombinerAttribute[CombinerColorType::A][4].m_Scale = 0;

	m_TextureCombinerAttribute[CombinerColorType::RGB][5].m_InputSource[InputSourceType::A] = CombInputSourceType::Previous;
	m_TextureCombinerAttribute[CombinerColorType::RGB][5].m_Operand[InputSourceType::A] = OperandType::Color;
	m_TextureCombinerAttribute[CombinerColorType::RGB][5].m_InputSource[InputSourceType::B] = CombInputSourceType::PrimaryColor;
	m_TextureCombinerAttribute[CombinerColorType::RGB][5].m_Operand[InputSourceType::B] = OperandType::Alpha;
	m_TextureCombinerAttribute[CombinerColorType::RGB][5].m_InputSource[InputSourceType::C] = CombInputSourceType::PreviousBuffer;
	m_TextureCombinerAttribute[CombinerColorType::RGB][5].m_Operand[InputSourceType::C] = OperandType::Color;
	m_TextureCombinerAttribute[CombinerColorType::RGB][5].m_MathFormula = MathFormulaType::MultAdd;
	m_TextureCombinerAttribute[CombinerColorType::RGB][5].m_Scale = 0;

	m_TextureCombinerAttribute[CombinerColorType::A][5].m_InputSource[InputSourceType::A] = CombInputSourceType::Previous;
	m_TextureCombinerAttribute[CombinerColorType::A][5].m_Operand[InputSourceType::A] = OperandType::Alpha;
	m_TextureCombinerAttribute[CombinerColorType::A][5].m_MathFormula = MathFormulaType::Replace;
	m_TextureCombinerAttribute[CombinerColorType::A][5].m_Scale = 0;

	m_TextureAttribute[0].m_MappingType = 3;//投影
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerAttribute::AttributeParam::textureShift( short tex0, short tex1, short tex2 )
{
  short       textureShift[TextureMax] = { tex0, tex1, tex2 };

  for( int stageNo = 0; stageNo < CombinerMax; ++stageNo )
  {
    for( int colorType = 0; colorType < CombinerColorType::NumberOf; ++colorType )
    {
      for( int sourceType = 0; sourceType < InputSourceType::NumberOf; ++sourceType )
      {
        short     type = this->m_TextureCombinerAttribute[colorType][stageNo].m_InputSource[sourceType];

        switch( type ){
        case CombInputSourceType::Texture0:
        case CombInputSourceType::Texture1:
        case CombInputSourceType::Texture2:
          {
            short no = type - CombInputSourceType::Texture0;
            this->m_TextureCombinerAttribute[colorType][stageNo].m_InputSource[sourceType] = textureShift[ no ];
          }
          break;
        }
      }
    }
  }

	for( int i = 0; i < TextureMax; ++i )
	{
		short   MappingType = this->m_TextureAttribute[i].m_MappingType;
		short   UvSetNo = this->m_TextureAttribute[i].m_UvSetNo;
		float   ScaleU = this->m_TextureAttribute[i].m_ScaleU;
		float   ScaleV = this->m_TextureAttribute[i].m_ScaleV;
		float   Rotate = this->m_TextureAttribute[i].m_Rotate;
		float   TranslateU = this->m_TextureAttribute[i].m_TranslateU;
		float   TranslateV = this->m_TextureAttribute[i].m_TranslateV;
		short   RepeatU = this->m_TextureAttribute[i].m_RepeatU;
		short   RepeatV = this->m_TextureAttribute[i].m_RepeatV;
		short   MagFilter = this->m_TextureAttribute[i].m_MagFilter;
		short   MinFilter = this->m_TextureAttribute[i].m_MinFilter;

		int			shiftNo = textureShift[i];
		this->m_TextureAttribute[shiftNo].m_MappingType = MappingType;
		this->m_TextureAttribute[shiftNo].m_UvSetNo = UvSetNo;
		this->m_TextureAttribute[shiftNo].m_ScaleU = ScaleU;
		this->m_TextureAttribute[shiftNo].m_ScaleV = ScaleV;
		this->m_TextureAttribute[shiftNo].m_Rotate = Rotate;
		this->m_TextureAttribute[shiftNo].m_TranslateU = TranslateU;
		this->m_TextureAttribute[shiftNo].m_TranslateV = TranslateV;
		this->m_TextureAttribute[shiftNo].m_RepeatU = RepeatU;
		this->m_TextureAttribute[shiftNo].m_RepeatV = RepeatV;
		this->m_TextureAttribute[shiftNo].m_MagFilter = MagFilter;
		this->m_TextureAttribute[shiftNo].m_MinFilter = MinFilter;
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerAttribute::AttributeParam::Clear()
{
  m_PsLightingEnable = false;
  m_VsLightingEnable = false;
  m_LightSetNo = 0;
  m_FogEnable = false;
  m_FogNo = 0;
  m_CullMode = 0;

  memset( &m_MaterialColor, 0, sizeof(MaterialColor) );

  memset( m_ConsntantColor, 0, sizeof(m_ConsntantColor) );
  memset( m_hConsntantColor, 0, sizeof(m_hConsntantColor) );
  memset( &m_EtcColor, 0, sizeof(m_EtcColor) );

  m_BumpMapNo = -1;
  m_BumpMapType = 0;
  m_ZRecastEnable = false;
  m_hBumpMapNo = 0;
  m_hBumpMapType = 0;
  m_hZRecastEnable = 0;

  memset( m_TextureAttribute, 0, sizeof(m_TextureAttribute) );
  //memset( m_ShaderAttributeTexture, 0, sizeof(m_ShaderAttributeTexture) );//std::mapを持っている為安易にゼロクリアできない。
  memset( m_TextureAttributeEnable, 0, sizeof(m_TextureAttributeEnable) );
  memset( m_hTextureAttributeEnable, 0, sizeof(m_hTextureAttributeEnable) );
  memset( m_hIsTexture2D, 0, sizeof(m_hIsTexture2D) );

  memset( m_LookUpTableEnable, 0, sizeof(m_LookUpTableEnable) );
  memset( m_hLookUpTableEnable, 0, sizeof(m_hLookUpTableEnable) );

  m_PrimaryFresnelEnable = false;
  m_SecondaryFresnelEnable = false;
  m_SpecularColorClamp = false;
  m_CookTorrance0Enable = false;
  m_CookTorrance1Enable = false;
  m_hPrimaryFresnelEnable = 0;
  m_hSecondaryFresnelEnable = 0;
  m_hSpecularColorClamp = 0;
  m_hCookTorrance0Enable = 0;
  m_hCookTorrance1Enable = 0;

  memset( m_TextureCombinerAttribute, 0, sizeof(m_TextureCombinerAttribute) );
  memset( m_TextureCombinerConstant, 0, sizeof(m_TextureCombinerConstant) );
  memset( m_hTextureCombinerConstant, 0, sizeof(m_hTextureCombinerConstant) );

  m_DrawLayer = 0;
  m_AlphaTestType = 0;
  m_AlphaTestRefValue = 0.0f;
  m_DepthTestType = 0;
  m_UpDateDepth = false;
  m_PolygonOffSet = 0;
  m_BlendMode = 0;

  memset( m_BlendFunc, 0, sizeof(m_BlendFunc) );
  m_LogicalOperation = 0;
	m_VertexShaderType = VertexShaderType::Default;

	for( int i = 0; i < CombinerAttribute::CombinerMax; ++i )
	{
		for( int type = 0; type < CombinerColorType::NumberOf; ++type )
		{
			this->m_TextureCombinerAttribute[type][i].m_MathFormula = MathFormulaType::Replace;
			this->m_TextureCombinerAttribute[type][i].m_Scale;

			for( int i2 = 0; i2 < InputSourceType::NumberOf; ++i2 )
			{
				this->m_TextureCombinerAttribute[type][i].m_InputSource[i2] = CombInputSourceType::Previous;

				if ( type == CombinerColorType::RGB )
					this->m_TextureCombinerAttribute[type][i].m_Operand[i2] = OperandType::Color;
				else
					this->m_TextureCombinerAttribute[type][i].m_Operand[i2] = OperandType::Alpha;
			}

			this->m_TextureCombinerAttribute[type][i].m_OutPutBuffer = false;
		}

		this->m_TextureCombinerConstant[i];
	}
}

}}}}


