#include "gfl_UtilPixelShader.h"

#include <nw/demo.h>
#include <nn/math.h>
#include <math.h>

#include <nn/os.h>
#include <nn/gx.h>
#include <nn/math.h>
#include <nn/fnd.h>
#include <nn/fs.h>
#include <nn/os/os_Memory.h>
#include <string.h>
#include <debug/gfl_Assert.h>
#include <grp/gfl_GraphicsType.h>

namespace gfl { namespace grp { namespace util {

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■				頂点カラーシェーダー
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void VertexColorPixelShader::vSetLinkProgram( GLuint program_object_id )
{
	GFL_GRPGL_ERROR_CHECK();
	this->m_ParentProgramObjectID = program_object_id;

	// Fragment uniform
	m_UniformLocations[PS_ALPHA_TEST] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_FragOperation.enableAlphaTest");
	m_UniformLocations[PS_ALPHA_REF] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_FragOperation.alphaRefValue");
	m_UniformLocations[PS_ALPHA_FUNC] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_FragOperation.alphaTestFunc");

	GFL_GRPGL_ERROR_CHECK();

	// テクスチャコンバイナ設定
	m_UniformLocations[PS_TEXTURE0_SAMPLER_TYPE] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_Texture[0].samplerType");
	GFL_GRPGL_ERROR_CHECK();

	m_UniformLocations[PS_TEXENV0_SRC_RGB] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].srcRgb");
	m_UniformLocations[PS_TEXENV0_SRC_ALPHA] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].srcAlpha");
	m_UniformLocations[PS_TEXENV0_COMBINE_RGB] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].combineRgb");
	m_UniformLocations[PS_TEXENV0_COMBINE_ALPHA] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].combineAlpha");
	m_UniformLocations[PS_TEXENV0_OPERAND_RGB] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].operandRgb");
	m_UniformLocations[PS_TEXENV0_OPERAND_ALPHA] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].operandAlpha");
  m_UniformLocations[PS_TEXENV0_SRC_CONST] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].constRgba");
	GFL_GRPGL_ERROR_CHECK();
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
GLuint VertexColorPixelShader::vGetObjectID() const
{
	return GL_DMP_FRAGMENT_SHADER_DMP;
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void VertexColorPixelShader::vSetUpShader() const
{
#if 0
	// Fragment uniform
	glUniform1i(m_UniformLocations[PS_ALPHA_TEST], GL_TRUE);
	glUniform1f(m_UniformLocations[PS_ALPHA_REF], 0.1f);
	glUniform1i(m_UniformLocations[PS_ALPHA_FUNC], GL_GREATER);

  glUniform3i(m_UniformLocations[PS_TEXENV0_SRC_RGB], GL_PRIMARY_COLOR, GL_PRIMARY_COLOR, GL_PRIMARY_COLOR);
  glUniform3i(m_UniformLocations[PS_TEXENV0_SRC_ALPHA], GL_PRIMARY_COLOR, GL_PRIMARY_COLOR, GL_PRIMARY_COLOR);

	glUniform1i(m_UniformLocations[PS_TEXENV0_COMBINE_RGB], GL_REPLACE);
	glUniform1i(m_UniformLocations[PS_TEXENV0_COMBINE_ALPHA], GL_REPLACE);
	glUniform3i(m_UniformLocations[PS_TEXENV0_OPERAND_RGB], GL_SRC_COLOR, GL_SRC_COLOR, GL_SRC_COLOR);
	glUniform3i(m_UniformLocations[PS_TEXENV0_OPERAND_ALPHA], GL_SRC_ALPHA, GL_SRC_ALPHA, GL_SRC_ALPHA);

#else
  {
    /*static const u32 commandBuffer[] = {
      0x00000000, 0x000f00c0,//(0x00c0)src*設定
      0x00000000, 0x000f00c8,//(0x00c8)src*設定
      0x00000000, 0x000f00d0,//(0x00d0)src*設定
      0x00000000, 0x000f00d8,//(0x00d8)src*設定
      0x00000000, 0x000f00f0,//(0x00f0)src*設定
      0x00000000, 0x000f00f8,//(0x00f8)src*設定
      0x00001a61, 0x000f0104,//(0x0104)アルファテスト設定レジスタ
    };*/
    
    static u32 commandBuffer[] = 
    {
      // 0xc0 + 0
      // srcRgb, srcAlpha
      // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].srcRgb"), GL_PRIMARY_COLOR, GL_PRIMARY_COLOR, GL_PRIMARY_COLOR);
      // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].srcAlpha"), GL_PRIMARY_COLOR, GL_PRIMARY_COLOR, GL_PRIMARY_COLOR);
      PICA_CMD_DATA_TEX_ENV_SRC(PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR, 
                                PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR,                                       
                                PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR),

      // 0xc0
      // テクスチャコンバイナ0の先頭レジスタ
      PICA_CMD_HEADER_BURSTSEQ(PICA_REG_TEX_ENV5, 0x5),

      // operandRgb, operandAlpha                        
      // 0xc0 + 1
      // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].operandRgb"), GL_SRC_COLOR, GL_SRC_COLOR, GL_SRC_COLOR);
      // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].operandAlpha"), GL_SRC_ALPHA, GL_SRC_ALPHA, GL_SRC_ALPHA);        
      PICA_CMD_DATA_TEX_ENV_OPERAND(PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR,
                                    PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA),

      // combineRgb, combineAlpha
      // 0xc0 + 2            
      // glUniform1i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].combineRgb"), GL_REPLACE);
      // glUniform1i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].combineAlpha"), GL_REPLACE);
      PICA_CMD_DATA_TEX_ENV_COMBINE(PICA_DATA_TEX_ENV_COMBINE_REPLACE, PICA_DATA_TEX_ENV_COMBINE_REPLACE),

      // constRgba
      // 0xc0 + 3
      // glUniform4f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].constRgba"), 0.0f, 0.0f, 0.0f, 0.0f);
      PICA_CMD_DATA_TEX_ENV_CONST(0x0, 0x0, 0x0, 0x0),

      // scaleRgb, scaleAlpha
      // 0xc0 + 4            
      // glUniform1f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].scaleRgb"), 1.0f);        
      // glUniform1f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].scaleAlpha"), 1.0f);
      PICA_CMD_DATA_TEX_ENV_SCALE(PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1),
      
      0x00001a61, 0x000f0104 //(0x0104)アルファテスト設定レジスタ
    };
            
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
  }
#endif
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■				テクスチャマップシェーダー
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void TextureMapPixelShader::vSetLinkProgram( GLuint program_object_id )
{
	GFL_GRPGL_ERROR_CHECK();
	this->m_ParentProgramObjectID = program_object_id;

	// Fragment uniform
	m_UniformLocations[PS_ALPHA_TEST] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_FragOperation.enableAlphaTest");
	m_UniformLocations[PS_ALPHA_REF] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_FragOperation.alphaRefValue");
	m_UniformLocations[PS_ALPHA_FUNC] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_FragOperation.alphaTestFunc");

	GFL_GRPGL_ERROR_CHECK();

	// テクスチャコンバイナ設定
	m_UniformLocations[PS_TEXTURE0_SAMPLER_TYPE] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_Texture[0].samplerType");
	GFL_GRPGL_ERROR_CHECK();

	m_UniformLocations[PS_TEXENV0_SRC_RGB] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].srcRgb");
	m_UniformLocations[PS_TEXENV0_SRC_ALPHA] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].srcAlpha");
	m_UniformLocations[PS_TEXENV0_COMBINE_RGB] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].combineRgb");
	m_UniformLocations[PS_TEXENV0_COMBINE_ALPHA] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].combineAlpha");
	m_UniformLocations[PS_TEXENV0_OPERAND_RGB] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].operandRgb");
	m_UniformLocations[PS_TEXENV0_OPERAND_ALPHA] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].operandAlpha");
	GFL_GRPGL_ERROR_CHECK();
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
GLuint TextureMapPixelShader::vGetObjectID() const
{
	return GL_DMP_FRAGMENT_SHADER_DMP;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void TextureMapPixelShader::vSetUpShader() const
{
#if 0
	// Fragment uniform
	glUniform1i(m_UniformLocations[PS_ALPHA_TEST], GL_TRUE);
	glUniform1f(m_UniformLocations[PS_ALPHA_REF], 0.1f);
	glUniform1i(m_UniformLocations[PS_ALPHA_FUNC], GL_GREATER);

	// テクスチャコンバイナ設定
	glUniform1i(m_UniformLocations[PS_TEXTURE0_SAMPLER_TYPE], GL_TEXTURE_2D);

	glUniform3i(m_UniformLocations[PS_TEXENV0_SRC_RGB], GL_PRIMARY_COLOR, GL_TEXTURE0, GL_TEXTURE0);
	glUniform3i(m_UniformLocations[PS_TEXENV0_SRC_ALPHA], GL_PRIMARY_COLOR, GL_TEXTURE0, GL_TEXTURE0);
	glUniform1i(m_UniformLocations[PS_TEXENV0_COMBINE_RGB], GL_MODULATE);
	glUniform1i(m_UniformLocations[PS_TEXENV0_COMBINE_ALPHA], GL_MODULATE);
	glUniform3i(m_UniformLocations[PS_TEXENV0_OPERAND_RGB], GL_SRC_COLOR, GL_SRC_COLOR, GL_SRC_COLOR);
	glUniform3i(m_UniformLocations[PS_TEXENV0_OPERAND_ALPHA], GL_SRC_ALPHA, GL_SRC_ALPHA, GL_SRC_ALPHA);
#else

  {
    /*static const u32 commandBuffer[] = {
      0x03300330, 0x000f00c0,//(0x00c0)src*設定
      0x00010001, 0x000f00c2,//(0x00c2)combine*設定
      0x00001a61, 0x000f0104 //(0x0104)アルファテスト設定レジスタ
    };*/
    
    static u32 commandBuffer[] = 
    {
      // 0xc0 + 0
      // srcRgb, srcAlpha
      // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].srcRgb"), GL_PRIMARY_COLOR, GL_PRIMARY_COLOR, GL_PRIMARY_COLOR);
      // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].srcAlpha"), GL_PRIMARY_COLOR, GL_PRIMARY_COLOR, GL_PRIMARY_COLOR);
      PICA_CMD_DATA_TEX_ENV_SRC(PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR,
                                PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR),

      // 0xc0
      // テクスチャコンバイナ0の先頭レジスタ
      PICA_CMD_HEADER_BURSTSEQ(PICA_REG_TEX_ENV5, 0x5),

      // operandRgb, operandAlpha                        
      // 0xc0 + 1
      // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].operandRgb"), GL_SRC_COLOR, GL_SRC_COLOR, GL_SRC_COLOR);
      // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].operandAlpha"), GL_SRC_ALPHA, GL_SRC_ALPHA, GL_SRC_ALPHA);        
      PICA_CMD_DATA_TEX_ENV_OPERAND(PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR,
                                    PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA),

      // combineRgb, combineAlpha
      // 0xc0 + 2            
      // glUniform1i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].combineRgb"), GL_MODULATE);
      // glUniform1i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].combineAlpha"), GL_MODULATE);
      PICA_CMD_DATA_TEX_ENV_COMBINE(PICA_DATA_TEX_ENV_COMBINE_MODULATE, PICA_DATA_TEX_ENV_COMBINE_MODULATE),

      // constRgba
      // 0xc0 + 3
      // glUniform4f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].constRgba"), 0.0f, 0.0f, 0.0f, 0.0f);
      PICA_CMD_DATA_TEX_ENV_CONST(0x0, 0x0, 0x0, 0x0),

      // scaleRgb, scaleAlpha
      // 0xc0 + 4            
      // glUniform1f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].scaleRgb"), 1.0f);        
      // glUniform1f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].scaleAlpha"), 1.0f);
      PICA_CMD_DATA_TEX_ENV_SCALE(PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1),
      
      0x00001a61, 0x000f0104 //(0x0104)アルファテスト設定レジスタ
    };
    
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
  }
  
#endif
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■				テクスチャマップシェーダー
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void RgbTextureMapPixelShader::vSetLinkProgram( GLuint program_object_id )
{
	GFL_GRPGL_ERROR_CHECK();
	this->m_ParentProgramObjectID = program_object_id;

	// Fragment uniform
	m_UniformLocations[PS_ALPHA_TEST] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_FragOperation.enableAlphaTest");
	m_UniformLocations[PS_ALPHA_REF] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_FragOperation.alphaRefValue");
	m_UniformLocations[PS_ALPHA_FUNC] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_FragOperation.alphaTestFunc");

	GFL_GRPGL_ERROR_CHECK();

	// テクスチャコンバイナ設定
	m_UniformLocations[PS_TEXTURE0_SAMPLER_TYPE] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_Texture[0].samplerType");
	GFL_GRPGL_ERROR_CHECK();

	m_UniformLocations[PS_TEXENV0_SRC_RGB] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].srcRgb");
	m_UniformLocations[PS_TEXENV0_SRC_ALPHA] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].srcAlpha");
	m_UniformLocations[PS_TEXENV0_COMBINE_RGB] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].combineRgb");
	m_UniformLocations[PS_TEXENV0_COMBINE_ALPHA] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].combineAlpha");
	m_UniformLocations[PS_TEXENV0_OPERAND_RGB] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].operandRgb");
	m_UniformLocations[PS_TEXENV0_OPERAND_ALPHA] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].operandAlpha");
	GFL_GRPGL_ERROR_CHECK();
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
GLuint RgbTextureMapPixelShader::vGetObjectID() const
{
	return GL_DMP_FRAGMENT_SHADER_DMP;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void RgbTextureMapPixelShader::vSetUpShader() const
{
  static u32 commandBuffer[] = 
  {
    // 0xc0 + 0
    // srcRgb, srcAlpha
    // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].srcRgb"), GL_PRIMARY_COLOR, GL_PRIMARY_COLOR, GL_PRIMARY_COLOR);
    // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].srcAlpha"), GL_PRIMARY_COLOR, GL_PRIMARY_COLOR, GL_PRIMARY_COLOR);
    PICA_CMD_DATA_TEX_ENV_SRC(PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR,
                              PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR),

    // 0xc0
    // テクスチャコンバイナ0の先頭レジスタ
    PICA_CMD_HEADER_BURSTSEQ(PICA_REG_TEX_ENV5, 0x5),

    // operandRgb, operandAlpha                        
    // 0xc0 + 1
    // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].operandRgb"), GL_SRC_COLOR, GL_SRC_COLOR, GL_SRC_COLOR);
    // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].operandAlpha"), GL_SRC_ALPHA, GL_SRC_ALPHA, GL_SRC_ALPHA);        
    PICA_CMD_DATA_TEX_ENV_OPERAND(PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR,
                                  PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA),

    // combineRgb, combineAlpha
    // 0xc0 + 2            
    // glUniform1i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].combineRgb"), GL_MODULATE);
    // glUniform1i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].combineAlpha"), GL_REPLACE);
    PICA_CMD_DATA_TEX_ENV_COMBINE(PICA_DATA_TEX_ENV_COMBINE_MODULATE, PICA_DATA_TEX_ENV_COMBINE_REPLACE),

    // constRgba
    // 0xc0 + 3
    // glUniform4f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].constRgba"), 0.0f, 0.0f, 0.0f, 0.0f);
    PICA_CMD_DATA_TEX_ENV_CONST(0x0, 0x0, 0x0, 0x0),

    // scaleRgb, scaleAlpha
    // 0xc0 + 4            
    // glUniform1f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].scaleRgb"), 1.0f);        
    // glUniform1f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].scaleAlpha"), 1.0f);
    PICA_CMD_DATA_TEX_ENV_SCALE(PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1),

    0x00001a61, 0x000f0104 //(0x0104)アルファテスト設定レジスタ
  };

  nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);

}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■				被写界深度用テクスチャマップシェーダー
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void FrontDofTextureMapPixelShader::vSetLinkProgram( GLuint program_object_id )
{
	GFL_GRPGL_ERROR_CHECK();
	this->m_ParentProgramObjectID = program_object_id;

	// Fragment uniform
	m_UniformLocations[PS_ALPHA_TEST] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_FragOperation.enableAlphaTest");
	m_UniformLocations[PS_ALPHA_REF] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_FragOperation.alphaRefValue");
	m_UniformLocations[PS_ALPHA_FUNC] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_FragOperation.alphaTestFunc");

	GFL_GRPGL_ERROR_CHECK();

	// テクスチャコンバイナ設定
	m_UniformLocations[PS_TEXTURE0_SAMPLER_TYPE] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_Texture[0].samplerType");
	GFL_GRPGL_ERROR_CHECK();

	m_UniformLocations[PS_TEXENV0_SRC_RGB] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].srcRgb");
	m_UniformLocations[PS_TEXENV0_SRC_ALPHA] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].srcAlpha");
	m_UniformLocations[PS_TEXENV0_COMBINE_RGB] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].combineRgb");
	m_UniformLocations[PS_TEXENV0_COMBINE_ALPHA] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].combineAlpha");
	m_UniformLocations[PS_TEXENV0_OPERAND_RGB] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].operandRgb");
	m_UniformLocations[PS_TEXENV0_OPERAND_ALPHA] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].operandAlpha");
	GFL_GRPGL_ERROR_CHECK();
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
GLuint FrontDofTextureMapPixelShader::vGetObjectID() const
{
	return GL_DMP_FRAGMENT_SHADER_DMP;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void FrontDofTextureMapPixelShader::vSetUpShader() const
{
  u8              color = 0x80;
  u32             scale = PICA_DATA_TEX_ENV_SCALE_1;
  static s32      fTest = 0;
  
  if ( fTest == 1 )
    scale = PICA_DATA_TEX_ENV_SCALE_2;
  if ( fTest == 2 )
    scale = PICA_DATA_TEX_ENV_SCALE_4;
  
  static u32 commandBuffer[] = 
  {
    // 0xc0 + 0
    // srcRgb, srcAlpha
    // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].srcRgb"), GL_TEXTURE0, GL_PRIMARY_COLOR, GL_PRIMARY_COLOR);
    // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].srcAlpha"), GL_TEXTURE0, GL_PRIMARY_COLOR, GL_PRIMARY_COLOR);
    PICA_CMD_DATA_TEX_ENV_SRC(PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0, PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE1, PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
                              PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0, PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE1, PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT),

    // 0xc0
    // テクスチャコンバイナ0の先頭レジスタ
    PICA_CMD_HEADER_BURSTSEQ(PICA_REG_TEX_ENV4, 0x5), 

    // operandRgb, operandAlpha                        
    // 0xc0 + 1
    // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].operandRgb"), GL_SRC_COLOR, GL_SRC_COLOR, GL_SRC_COLOR);
    // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].operandAlpha"), GL_SRC_ALPHA, GL_SRC_ALPHA, GL_SRC_ALPHA);        
    PICA_CMD_DATA_TEX_ENV_OPERAND(PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR,
                                  PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA),

    // combineRgb, combineAlpha
    // 0xc0 + 2            
    // glUniform1i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].combineRgb"), GL_INTERPOLATE);
    // glUniform1i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].combineAlpha"), GL_INTERPOLATE);
    PICA_CMD_DATA_TEX_ENV_COMBINE(PICA_DATA_TEX_ENV_COMBINE_INTERPOLATE, PICA_DATA_TEX_ENV_COMBINE_INTERPOLATE),

    // constRgba
    // 0xc0 + 3
    // glUniform4f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].constRgba"), 0.5f, 0.5f, 0.5f, 0.5f);
    PICA_CMD_DATA_TEX_ENV_CONST(color, color, color, color),

    // scaleRgb, scaleAlpha
    // 0xc0 + 4            
    // glUniform1f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].scaleRgb"), 1.0f);        
    // glUniform1f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].scaleAlpha"), 1.0f);
    PICA_CMD_DATA_TEX_ENV_SCALE(PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1),
    
    //------------------------------------------------
    
    // 0xc0 + 0
    // srcRgb, srcAlpha
    // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].srcRgb"), prev, tex2, const);
    // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].srcAlpha"), prev, tex2, const);
    PICA_CMD_DATA_TEX_ENV_SRC(PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS, PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE2, PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
                              PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS, PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE2, PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT),

    // 0xc0
    // テクスチャコンバイナ0の先頭レジスタ
    PICA_CMD_HEADER_BURSTSEQ(PICA_REG_TEX_ENV5, 0x5), 

    // operandRgb, operandAlpha                        
    // 0xc0 + 1
    // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].operandRgb"), GL_SRC_COLOR, GL_SRC_COLOR, GL_SRC_COLOR);
    // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].operandAlpha"), GL_SRC_ALPHA, GL_SRC_ALPHA, GL_SRC_ALPHA);        
    PICA_CMD_DATA_TEX_ENV_OPERAND(PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR,
                                  PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA),

    // combineRgb, combineAlpha
    // 0xc0 + 2            
    // glUniform1i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].combineRgb"), GL_MODULATE);
    // glUniform1i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].combineAlpha"), GL_MODULATE);
    PICA_CMD_DATA_TEX_ENV_COMBINE(PICA_DATA_TEX_ENV_COMBINE_INTERPOLATE, PICA_DATA_TEX_ENV_COMBINE_INTERPOLATE),

    // constRgba
    // 0xc0 + 3
    // glUniform4f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].constRgba"), 0.5f, 0.5f, 0.5f, 0.5f);
    PICA_CMD_DATA_TEX_ENV_CONST(color, color, color, color),

    // scaleRgb, scaleAlpha
    // 0xc0 + 4            
    // glUniform1f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].scaleRgb"), 1.0f);        
    // glUniform1f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].scaleAlpha"), 1.0f);
    PICA_CMD_DATA_TEX_ENV_SCALE(PICA_DATA_TEX_ENV_SCALE_1, scale),

    0x00001a61, 0x000f0104 //(0x0104)アルファテスト設定レジスタ
  };

  nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■				被写界深度用テクスチャマップシェーダー
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void BackDofTextureMapPixelShader::vSetLinkProgram( GLuint program_object_id )
{
	GFL_GRPGL_ERROR_CHECK();
	this->m_ParentProgramObjectID = program_object_id;

	// Fragment uniform
	m_UniformLocations[PS_ALPHA_TEST] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_FragOperation.enableAlphaTest");
	m_UniformLocations[PS_ALPHA_REF] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_FragOperation.alphaRefValue");
	m_UniformLocations[PS_ALPHA_FUNC] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_FragOperation.alphaTestFunc");

	GFL_GRPGL_ERROR_CHECK();

	// テクスチャコンバイナ設定
	m_UniformLocations[PS_TEXTURE0_SAMPLER_TYPE] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_Texture[0].samplerType");
	GFL_GRPGL_ERROR_CHECK();

	m_UniformLocations[PS_TEXENV0_SRC_RGB] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].srcRgb");
	m_UniformLocations[PS_TEXENV0_SRC_ALPHA] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].srcAlpha");
	m_UniformLocations[PS_TEXENV0_COMBINE_RGB] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].combineRgb");
	m_UniformLocations[PS_TEXENV0_COMBINE_ALPHA] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].combineAlpha");
	m_UniformLocations[PS_TEXENV0_OPERAND_RGB] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].operandRgb");
	m_UniformLocations[PS_TEXENV0_OPERAND_ALPHA] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].operandAlpha");
	GFL_GRPGL_ERROR_CHECK();
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
GLuint BackDofTextureMapPixelShader::vGetObjectID() const
{
	return GL_DMP_FRAGMENT_SHADER_DMP;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void BackDofTextureMapPixelShader::vSetUpShader() const
{
  
  static u32 commandBuffer[] = 
  {
    // 0xc0 + 0
    // srcRgb, srcAlpha
    // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].srcRgb"), GL_TEXTURE0, GL_PRIMARY_COLOR, GL_PRIMARY_COLOR);
    // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].srcAlpha"), GL_TEXTURE0, GL_PRIMARY_COLOR, GL_PRIMARY_COLOR);
    PICA_CMD_DATA_TEX_ENV_SRC(PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR,
                              PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR),

    // 0xc0
    // テクスチャコンバイナ0の先頭レジスタ
    PICA_CMD_HEADER_BURSTSEQ(PICA_REG_TEX_ENV5, 0x5),

    // operandRgb, operandAlpha                        
    // 0xc0 + 1
    // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].operandRgb"), GL_SRC_COLOR, GL_SRC_COLOR, GL_SRC_COLOR);
    // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].operandAlpha"), GL_SRC_ALPHA, GL_SRC_ALPHA, GL_SRC_ALPHA);        
    PICA_CMD_DATA_TEX_ENV_OPERAND(PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR, PICA_DATA_OPE_RGB_SRC_COLOR,
                                  PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA),

    // combineRgb, combineAlpha
    // 0xc0 + 2            
    // glUniform1i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].combineRgb"), GL_MODULATE);
    // glUniform1i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].combineAlpha"), GL_MODULATE);
    PICA_CMD_DATA_TEX_ENV_COMBINE(PICA_DATA_TEX_ENV_COMBINE_MODULATE, PICA_DATA_TEX_ENV_COMBINE_MODULATE),

    // constRgba
    // 0xc0 + 3
    // glUniform4f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].constRgba"), 0.0f, 0.0f, 0.0f, 0.0f);
    PICA_CMD_DATA_TEX_ENV_CONST(0x0, 0x0, 0x0, 0x0),

    // scaleRgb, scaleAlpha
    // 0xc0 + 4            
    // glUniform1f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].scaleRgb"), 1.0f);        
    // glUniform1f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].scaleAlpha"), 1.0f);
    PICA_CMD_DATA_TEX_ENV_SCALE(PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1),

    0x00001a61, 0x000f0104 //(0x0104)アルファテスト設定レジスタ
  };

  nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■				デバッグシェーダー
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DebugPixelShader::vSetLinkProgram( GLuint program_object_id )
{
	GFL_GRPGL_ERROR_CHECK();
	this->m_ParentProgramObjectID = program_object_id;

	// Fragment uniform
	m_UniformLocations[PS_ALPHA_TEST] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_FragOperation.enableAlphaTest");
	m_UniformLocations[PS_ALPHA_REF] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_FragOperation.alphaRefValue");
	m_UniformLocations[PS_ALPHA_FUNC] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_FragOperation.alphaTestFunc");

	GFL_GRPGL_ERROR_CHECK();

	// テクスチャコンバイナ設定
	m_UniformLocations[PS_TEXTURE0_SAMPLER_TYPE] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_Texture[0].samplerType");
	GFL_GRPGL_ERROR_CHECK();

	m_UniformLocations[PS_TEXENV0_SRC_RGB] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].srcRgb");
	m_UniformLocations[PS_TEXENV0_SRC_ALPHA] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].srcAlpha");
	m_UniformLocations[PS_TEXENV0_COMBINE_RGB] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].combineRgb");
	m_UniformLocations[PS_TEXENV0_COMBINE_ALPHA] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].combineAlpha");
	m_UniformLocations[PS_TEXENV0_OPERAND_RGB] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].operandRgb");
	m_UniformLocations[PS_TEXENV0_OPERAND_ALPHA] = glGetUniformLocation(m_ParentProgramObjectID, "dmp_TexEnv[0].operandAlpha");
	GFL_GRPGL_ERROR_CHECK();
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
GLuint DebugPixelShader::vGetObjectID() const
{
	return GL_DMP_FRAGMENT_SHADER_DMP;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DebugPixelShader::vSetUpShader() const
{ 
  {
    static u32 commandBuffer[] = 
    {
      // 0xc0 + 0
      // srcRgb, srcAlpha
      // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].srcRgb"), GL_TEXTURE0, GL_PRIMARY_COLOR, GL_PRIMARY_COLOR);
      // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].srcAlpha"), GL_TEXTURE0, GL_PRIMARY_COLOR, GL_PRIMARY_COLOR);
      PICA_CMD_DATA_TEX_ENV_SRC(PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR,
                                PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR, PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR),

      // 0xc0
      // テクスチャコンバイナ0の先頭レジスタ
      PICA_CMD_HEADER_BURSTSEQ(PICA_REG_TEX_ENV5, 0x5),

      // operandRgb, operandAlpha                        
      // 0xc0 + 1
      // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].operandRgb"), GL_SRC_COLOR, GL_SRC_COLOR, GL_SRC_COLOR);
      // glUniform3i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].operandAlpha"), GL_SRC_ALPHA, GL_SRC_ALPHA, GL_SRC_ALPHA);        
      PICA_CMD_DATA_TEX_ENV_OPERAND(PICA_DATA_OPE_RGB_SRC_ALPHA, PICA_DATA_OPE_RGB_SRC_ALPHA, PICA_DATA_OPE_RGB_SRC_ALPHA,
                                    PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA, PICA_DATA_OPE_ALPHA_SRC_ALPHA),

      // combineRgb, combineAlpha
      // 0xc0 + 2            
      // glUniform1i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].combineRgb"), GL_MODULATE);
      // glUniform1i(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].combineAlpha"), GL_MODULATE);
      PICA_CMD_DATA_TEX_ENV_COMBINE(PICA_DATA_TEX_ENV_COMBINE_REPLACE, PICA_DATA_TEX_ENV_COMBINE_REPLACE),

      // constRgba
      // 0xc0 + 3
      // glUniform4f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].constRgba"), 0.0f, 0.0f, 0.0f, 0.0f);
      PICA_CMD_DATA_TEX_ENV_CONST(0x0, 0x0, 0x0, 0x0),

      // scaleRgb, scaleAlpha
      // 0xc0 + 4            
      // glUniform1f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].scaleRgb"), 1.0f);        
      // glUniform1f(glGetUniformLocation(s_PgID, "dmp_TexEnv[0].scaleAlpha"), 1.0f);
      PICA_CMD_DATA_TEX_ENV_SCALE(PICA_DATA_TEX_ENV_SCALE_1, PICA_DATA_TEX_ENV_SCALE_1),

      0x00001a61, 0x000f0104 //(0x0104)アルファテスト設定レジスタ
    };

    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
  }
}

}}}


