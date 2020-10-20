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
#include "../include/BrushShader.h"

#include <qt/Shader/String/Util.h>
#include <qt/Shader/String/LogPrint.h>
#include <qt/Shader/Common/SecureData.h>
#include <assert.h>

using namespace gfl::maya::plugins::Shader;

namespace gfl { namespace maya { namespace plugins { namespace Shader {


  GF_ALIGNED(0x80) static const unsigned char  s_BrushVertexShaderCode[] = {
#include "BrushShader.vert.txt"
  };

  GF_ALIGNED(0x80) static const unsigned char  s_BrushPixelShaderCode[] = {
#include "BrushShader.frag.txt"
  };

  /*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  ■
  ■               デフォルト頂点シェーダー
  ■
  ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
  //-----------------------------------------------------------------------------
  /// @brief	コンストラクタ
  //-----------------------------------------------------------------------------
  BrushVertexShader::BrushVertexShader()
  {
    //---- 頂点シェーダオブジェクトの作成 ----//
    m_hProgram = glCreateShader(GL_VERTEX_SHADER);

    Common::SecureData	data = Common::SecureData::DeCode( s_BrushVertexShaderCode, sizeof(s_BrushVertexShaderCode) );
    //const char *vfile = (char*)data.GetData();
    const char* vfile = (char*)s_BrushVertexShaderCode;

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
  BrushPixelShader::BrushPixelShader()
  {
    //---- フラグメントシェーダオブジェクトの作成 ----//
    m_hProgram = glCreateShader(GL_FRAGMENT_SHADER);
    //ソースコードをシェーダオブジェクトに変換
    Common::SecureData	data = Common::SecureData::DeCode( s_BrushPixelShaderCode, sizeof(s_BrushPixelShaderCode) );
    //const char *ffile = (char*)data.GetData();
    const char* ffile = (char*)s_BrushPixelShaderCode;

    glShaderSource(m_hProgram, 1, &ffile,NULL);
    glCompileShader(m_hProgram);//コンパイル

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
  void BrushPixelShader::vSetUpShader() const
  {
    //テクスチャユニット設定
    glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "Texture[0]"), 0 );
    glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "Texture[1]"), 1 );
    glUniform1i( glGetUniformLocation( this->m_hParentShaderObject, "Texture[2]"), 2 );

  }

}}}}
