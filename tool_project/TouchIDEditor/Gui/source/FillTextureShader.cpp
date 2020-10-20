//
// Copyright (C) PokemonShaderNode
// 
// File: PokemonShaderNodeNode.cpp
//
// Dependency Graph Node: PokemonShaderNode
//
// Author: Maya Plug-in Wizard 2.0
//

#include <qt/Shader/ShaderObject.h>
#include "../include/FillTextureShader.h"

#include <qt/Shader/String/Util.h>
#include <qt/Shader/String/LogPrint.h>
#include <qt/Shader/Common/SecureData.h>
#include <assert.h>

using namespace gfl::maya::plugins::Shader;

namespace gfl { namespace maya { namespace plugins { namespace Shader {


  GF_ALIGNED(0x80) static const unsigned char s_FillTextureVertexShaderCode[] = {
#include "FillTextureShader.vert.txt"
  };

  GF_ALIGNED(0x80) static const unsigned char s_FillTexturePixelShaderCode[] = {
#include "FillTextureShader.frag.txt"
  };

  /*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  ■
  ■               デフォルト頂点シェーダー
  ■
  ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
  //-----------------------------------------------------------------------------
  /// @brief	コンストラクタ
  //-----------------------------------------------------------------------------
  FillTextureVertexShader::FillTextureVertexShader()
  {
    //---- 頂点シェーダオブジェクトの作成 ----//
    m_hProgram = glCreateShader(GL_VERTEX_SHADER);

    Common::SecureData	data = Common::SecureData::DeCode( s_FillTextureVertexShaderCode, sizeof(s_FillTextureVertexShaderCode) );
    const char* vfile = (char*)s_FillTextureVertexShaderCode;

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
  ■               デフォルトピクセルシェーダー
  ■
  ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
  //-----------------------------------------------------------------------------
  /// @brief	コンストラクタ
  //-----------------------------------------------------------------------------
  FillTexturePixelShader::FillTexturePixelShader()
  {
    //---- フラグメントシェーダオブジェクトの作成 ----//
    m_hProgram = glCreateShader(GL_FRAGMENT_SHADER);
    //ソースコードをシェーダオブジェクトに変換
    Common::SecureData	data = Common::SecureData::DeCode( s_FillTexturePixelShaderCode, sizeof(s_FillTexturePixelShaderCode) );
    const char* ffile = (char*)s_FillTexturePixelShaderCode;

    glShaderSource(m_hProgram, 1, &ffile,NULL);
    glCompileShader(m_hProgram);//コンパイル

    int	compiled;
    glGetShaderiv(m_hProgram, GL_COMPILE_STATUS, &compiled);

    if ( compiled == GL_FALSE )
    {
      {
        /* シェーダコンパイルのログバッファ */
        const int max_shader_log_size = 1024 * 10;
        char s_logBuffer[max_shader_log_size];

        int logSize;
        int length;

        /* ログの長さは、最後のNULL文字も含む */
        glGetShaderiv(m_hProgram, GL_INFO_LOG_LENGTH , &logSize);

        if (logSize > 1)
        {
          glGetShaderInfoLog(m_hProgram, max_shader_log_size, &length, s_logBuffer);
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
  void FillTexturePixelShader::vSetUpShader() const
  {
    //テクスチャユニット設定
    glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "Texture[0]"), 0 );
    glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "Texture[1]"), 1 );
    glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "Texture[2]"), 2 );

  }

}}}}
