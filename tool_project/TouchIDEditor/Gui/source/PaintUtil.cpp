#include "../include/PaintUtil.h"
#include "../include/GLUtil.h"
#include "../include/MainWindow.h"
#include <string>
#include <vector>

#include "../include/MultiTextureShader.h"
#include "../include/FillTextureShader.h"
#include "../include/BrushShader.h"
#include "../include/BucketJob.h"
#include "../include/MultiSamplingTextureShader.h"

using namespace gfl::maya;
using namespace gfl::maya::plugins::Shader;
using namespace gfl::maya::GLInterface;

namespace Gui { 

  static const unsigned char s_BrushTgaCode[] = {
#include "Brush.tga.txt"
  };

  PaintUtil *PaintUtil::s_Gp = NULL;

  /*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  ■
  ■
  ■
  ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void PaintUtil::Initialize()
  {
    if ( s_Gp == NULL )
    {
      s_Gp = new PaintUtil();
    }
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void PaintUtil::Terminate()
  {
    if ( s_Gp )
    {
      delete s_Gp;
      s_Gp = NULL;
    }
  }

  /*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  ■
  ■
  ■
  ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  PaintUtil::PaintUtil()
  {
    for( int i = 0; i < ShaderType::NumberOf; ++i )
    {
      m_pShaderObject[i] = NULL;
      m_pVertexShader[i] = NULL;
      m_pPixelShader[i] = NULL;
    }

    {
      m_pVertexShader[ShaderType::MultiTexture] = DbgNew MultiTextureVertexShader();
      m_pPixelShader[ShaderType::MultiTexture] = DbgNew MultiTexturePixelShader();
      m_pShaderObject[ShaderType::MultiTexture] = ShaderObject::CreateShaderObject( m_pVertexShader[ShaderType::MultiTexture], NULL, m_pPixelShader[ShaderType::MultiTexture] );

      m_pVertexShader[ShaderType::Brush] = DbgNew BrushVertexShader();
      m_pPixelShader[ShaderType::Brush] = DbgNew BrushPixelShader();
      m_pShaderObject[ShaderType::Brush] = ShaderObject::CreateShaderObject( m_pVertexShader[ShaderType::Brush], NULL, m_pPixelShader[ShaderType::Brush] );

      m_pVertexShader[ShaderType::FillTexture] = DbgNew gfl::maya::plugins::Shader::FillTextureVertexShader();
      m_pPixelShader[ShaderType::FillTexture] = DbgNew gfl::maya::plugins::Shader::FillTexturePixelShader();
      m_pShaderObject[ShaderType::FillTexture] = ShaderObject::CreateShaderObject( m_pVertexShader[ShaderType::FillTexture], NULL, m_pPixelShader[ShaderType::FillTexture] );

      m_pVertexShader[ShaderType::MultiSamplingTexture] = DbgNew gfl::maya::plugins::Shader::MultiSamplingTextureVertexShader();
      m_pPixelShader[ShaderType::MultiSamplingTexture] = DbgNew gfl::maya::plugins::Shader::MultiSamplingTexturePixelShader();
      m_pShaderObject[ShaderType::MultiSamplingTexture] = ShaderObject::CreateShaderObject( m_pVertexShader[ShaderType::MultiSamplingTexture], NULL, m_pPixelShader[ShaderType::MultiSamplingTexture] );
    }

    {
      m_hVsScreanSize = m_pShaderObject[ShaderType::Brush]->GetParameterByName("cVsScreanSize");
      m_hVsBrushSize = m_pShaderObject[ShaderType::Brush]->GetParameterByName("cVsBrushSize");
      m_hVsBrushPos = m_pShaderObject[ShaderType::Brush]->GetParameterByName("cVsBrushPos");
      m_hPsBrushColor = m_pShaderObject[ShaderType::Brush]->GetParameterByName("cPsBrushColor");

      m_hPsFillAlpha = m_pShaderObject[ShaderType::FillTexture]->GetParameterByName("cPsFillAlpha");

      m_hPsMultiSamplingOffSet = m_pShaderObject[ShaderType::MultiSamplingTexture]->GetParameterByName("cPsMultiSamplingOffSet");

      m_hPsBlendAlpha = m_pShaderObject[ShaderType::MultiTexture]->GetParameterByName("cPsBlendAlpha");
    }

    {
      TgaFormat *pImage = (TgaFormat*)s_BrushTgaCode;
      char* pImageTop = (char*)&s_BrushTgaCode[ sizeof(TgaFormat) ];

      glGenTextures( 1, &m_hBrushTexture );
      glBindTexture( GL_TEXTURE_2D, m_hBrushTexture );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, pImage->Width, pImage->Hight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImageTop );
      glBindTexture( GL_TEXTURE_2D, 0 );//リセット
    }
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  PaintUtil::~PaintUtil()
  {
    glDeleteTextures( 1, &m_hBrushTexture );

    for( int i = 0; i < ShaderType::NumberOf; ++i )
    {
      SAFE_DELETE( m_pShaderObject[i] );
      SAFE_DELETE( m_pVertexShader[i] );
      SAFE_DELETE( m_pPixelShader[i] );
    }
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void PaintUtil::drawPoints( const DrawPointInfo &rInfo )
  {
    if ( rInfo.pointList.size() == 0 )
    {
      return;
    }

    m_pShaderObject[ShaderType::Brush]->SetShader();

    glEnable( GL_ALPHA_TEST );
    glAlphaFunc( GL_GREATER, 0 );

    {
      glActiveTexture( GL_TEXTURE0 );
      glEnable(GL_TEXTURE_2D);
      glBindTexture( GL_TEXTURE_2D, m_hBrushTexture );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    }

    Math::Vector lineColor( rInfo.qlineColor.red(), rInfo.qlineColor.green(), rInfo.qlineColor.blue() );

    lineColor /= 255.0f;

    m_pShaderObject[ShaderType::Brush]->SetVector( m_hVsScreanSize, Math::Vector(rInfo.screanSize.GFL_VECTOR_X, rInfo.screanSize.GFL_VECTOR_Y, 0, 0) );
    m_pShaderObject[ShaderType::Brush]->SetVector( m_hVsBrushSize, Math::Vector(rInfo.lineWidth, rInfo.lineWidth, 0, 0) );
    m_pShaderObject[ShaderType::Brush]->SetVector( m_hPsBrushColor, lineColor );

    if ( rInfo.pointList.size() == 1 )
    {
      drawPoint( rInfo.pointList[0].x, rInfo.pointList[0].y );
    }
    else
    {
      static int start = 0;

      for( unsigned int i = start; i < rInfo.pointList.size() - 1; ++i )
      {
        Math::Vector from( rInfo.pointList[i].x, rInfo.pointList[i].y, 0.0f, 0.0f );
        Math::Vector to( rInfo.pointList[i + 1].x, rInfo.pointList[i + 1].y, 0.0f, 0.0f );
        Math::Vector line = to - from;
        float len = line.Length3();
        float step = 0.001f / len;

        for( float lerp = 0.0f; lerp < 1.0f; lerp += step )
        {
          Math::Vector pos;

          pos.x = (from.x) + (line.x * lerp);
          pos.y = (from.y) + (line.y * lerp);

          drawPoint( pos.x, pos.y );
        }

        drawPoint( to.x, to.y );
      }
    }

    glUseProgram( 0 );
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void PaintUtil::drawPoint( float x, float y )
  {
    static Math::Vector pos[4] = 
    {
      Math::Vector(-1.0f,-1.0f, 0.0f, 1.0f ),
      Math::Vector( 1.0f,-1.0f, 0.0f, 1.0f ),
      Math::Vector(-1.0f, 1.0f, 0.0f, 1.0f ),
      Math::Vector( 1.0f, 1.0f, 0.0f, 1.0f )
    };

    static Math::Vector uv[4] = 
    {
      Math::Vector( 0.0f, 0.0f, 0.0f, 0.0f ),
      Math::Vector( 1.0f, 0.0f, 0.0f, 0.0f ),
      Math::Vector( 0.0f, 1.0f, 0.0f, 0.0f ),
      Math::Vector( 1.0f, 1.0f, 0.0f, 0.0f )
    };

    m_pShaderObject[ShaderType::Brush]->SetVector( m_hVsBrushPos, Math::Vector(x, y, 0, 0) );

    static u_int index[4] = { 0, 1, 2, 3 };

    glEnableVertexAttribArray( GLInterface::StreamUsage::Position );
    glVertexAttribPointer( GLInterface::StreamUsage::Position, 4, GLInterface::VertexType::Float, GL_FALSE, sizeof(float) * 4, pos );

    glEnableVertexAttribArray( GLInterface::StreamUsage::TexCoord0 );
    glVertexAttribPointer( GLInterface::StreamUsage::TexCoord0, 4, GLInterface::VertexType::Float, GL_FALSE, sizeof(Math::Vector), uv );

    glDrawElements( GL_TRIANGLE_STRIP, sizeof(index) / sizeof(u_int), GL_UNSIGNED_INT, index );
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void PaintUtil::drawBucket( const DrawBucketInfo &rInfo )
  {
    glUseProgram( 0 );
    glShadeModel( GL_FLAT );

    void *pixels = 0;
    void *dest_pixels = 0;
    unsigned int totalSize = rInfo.RTSize.GFL_VECTOR_X * rInfo.RTSize.GFL_VECTOR_Y * 4;
    pixels = DbgNew unsigned char[totalSize];
    dest_pixels = DbgNew unsigned char[totalSize];

    //　フレームバッファをバインド
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, rInfo.frameBuffer );

    //チェック先
    glReadBuffer( rInfo.src_attachments );
    glReadPixels( 0, 0, rInfo.RTSize.GFL_VECTOR_X, rInfo.RTSize.GFL_VECTOR_Y, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

    //書き込み先
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, rInfo.frameBuffer );
    glReadBuffer( rInfo.dest_attachments );
    glReadPixels( 0, 0, rInfo.RTSize.GFL_VECTOR_X, rInfo.RTSize.GFL_VECTOR_Y, GL_RGBA, GL_UNSIGNED_BYTE, dest_pixels );
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

    {
      int x = rInfo.mousePos.x();
      int y = rInfo.mousePos.y();

      x = ::std::max( x, 0 );
      y = ::std::max( y, 0 );
      x = ::std::min( (float)x, rInfo.RTSize.GFL_VECTOR_X );
      y = ::std::min( (float)y, rInfo.RTSize.GFL_VECTOR_Y );

      y = rInfo.RTSize.GFL_VECTOR_Y - y;//イメージが反転している

      int pos = y * rInfo.RTSize.GFL_VECTOR_X;
      pos += x;

      BucketJob::Info info;

      info.m_Color = rInfo.color;
      info.m_ImageSize.Set( rInfo.RTSize );
      info.m_MousePos.setX( x );
      info.m_MousePos.setY( y );
      info.m_pImageData = reinterpret_cast<BucketJob::RGBA32*>( pixels );
      info.m_pDstImageData = reinterpret_cast<BucketJob::RGBA32*>( dest_pixels );
      info.m_Tolerance = rInfo.tolerance;

      BucketJob::DoIt( info );
    }

    //書き込み左記
    glBindTexture( GL_TEXTURE_2D, rInfo.renderTexture );
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, rInfo.RTSize.GFL_VECTOR_X, rInfo.RTSize.GFL_VECTOR_Y, GL_RGBA, GL_UNSIGNED_BYTE, dest_pixels );

    delete[] ( pixels );
    delete[] ( dest_pixels );
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void PaintUtil::drawTexture( GLuint textureHandle, GLuint samplingMode, bool fAlpha )
  {
    using namespace gfl::maya;

    static Math::Vector pos[4] = 
    {
      Math::Vector(-1.0f,-1.0f, 0.0f, 1.0f ),
      Math::Vector( 1.0f,-1.0f, 0.0f, 1.0f ),
      Math::Vector(-1.0f, 1.0f, 0.0f, 1.0f ),
      Math::Vector( 1.0f, 1.0f, 0.0f, 1.0f )
    };

    static Math::Vector uv[4] = 
    {
      Math::Vector( 0.0f, 0.0f, 0.0f, 0.0f ),
      Math::Vector( 1.0f, 0.0f, 0.0f, 0.0f ),
      Math::Vector( 0.0f, 1.0f, 0.0f, 0.0f ),
      Math::Vector( 1.0f, 1.0f, 0.0f, 0.0f )
    };

    static u_int index[4] = { 0, 1, 2, 3 };

    m_pShaderObject[ShaderType::FillTexture]->SetShader();
    glUniform1i( m_hPsFillAlpha, fAlpha );

    {
      glActiveTexture( GL_TEXTURE0 );
      glEnable(GL_TEXTURE_2D);
      glBindTexture( GL_TEXTURE_2D, textureHandle );

      //GL_NEAREST, GL_LINEAR
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, samplingMode);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, samplingMode);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    }

    glEnableVertexAttribArray( GLInterface::StreamUsage::Position );
    glVertexAttribPointer( GLInterface::StreamUsage::Position, 4, GLInterface::VertexType::Float, GL_FALSE, sizeof(float) * 4, pos );

    glEnableVertexAttribArray( GLInterface::StreamUsage::TexCoord0 );
    glVertexAttribPointer( GLInterface::StreamUsage::TexCoord0, 4, GLInterface::VertexType::Float, GL_FALSE, sizeof(Math::Vector), uv );

    glDrawElements( GL_TRIANGLE_STRIP, sizeof(index) / sizeof(u_int), GL_UNSIGNED_INT, index );
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void PaintUtil::multiSamplingDrawTexture( const MultiSamplingInfo &rInfo )
  {
    using namespace gfl::maya;

    static Math::Vector pos[4] = 
    {
      Math::Vector(-1.0f,-1.0f, 0.0f, 1.0f ),
      Math::Vector( 1.0f,-1.0f, 0.0f, 1.0f ),
      Math::Vector(-1.0f, 1.0f, 0.0f, 1.0f ),
      Math::Vector( 1.0f, 1.0f, 0.0f, 1.0f )
    };

    static Math::Vector uv[4] = 
    {
      Math::Vector( 0.0f, 0.0f, 0.0f, 0.0f ),
      Math::Vector( 1.0f, 0.0f, 0.0f, 0.0f ),
      Math::Vector( 0.0f, 1.0f, 0.0f, 0.0f ),
      Math::Vector( 1.0f, 1.0f, 0.0f, 0.0f )
    };

    static u_int index[4] = { 0, 1, 2, 3 };

    Math::Vector offset;

    offset.x = 1.0f / rInfo.m_TextureSize.GFL_VECTOR_X;
    offset.y = 1.0f / rInfo.m_TextureSize.GFL_VECTOR_Y;

    m_pShaderObject[ShaderType::MultiSamplingTexture]->SetShader();
    m_pShaderObject[ShaderType::MultiSamplingTexture]->SetVector( this->m_hPsMultiSamplingOffSet, offset );

    {
      glActiveTexture( GL_TEXTURE0 );
      glEnable(GL_TEXTURE_2D);
      glBindTexture( GL_TEXTURE_2D, rInfo.m_TextureHandle );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, rInfo.m_SamplingMode);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, rInfo.m_SamplingMode);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    }

    glEnableVertexAttribArray( GLInterface::StreamUsage::Position );
    glVertexAttribPointer( GLInterface::StreamUsage::Position, 4, GLInterface::VertexType::Float, GL_FALSE, sizeof(float) * 4, pos );

    glEnableVertexAttribArray( GLInterface::StreamUsage::TexCoord0 );
    glVertexAttribPointer( GLInterface::StreamUsage::TexCoord0, 4, GLInterface::VertexType::Float, GL_FALSE, sizeof(Math::Vector), uv );

    glDrawElements( GL_TRIANGLE_STRIP, sizeof(index) / sizeof(u_int), GL_UNSIGNED_INT, index );
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void PaintUtil::drawTextures( GLuint textureHandle0, GLuint textureHandle1, GLuint textureHandle2, float blendAlpha )
  {
    static Math::Vector pos[4] = 
    {
      Math::Vector(-1.0f,-1.0f, 0.0f, 1.0f ),
      Math::Vector( 1.0f,-1.0f, 0.0f, 1.0f ),
      Math::Vector(-1.0f, 1.0f, 0.0f, 1.0f ),
      Math::Vector( 1.0f, 1.0f, 0.0f, 1.0f )
    };

    static Math::Vector uv[4] = 
    {
      Math::Vector( 0.0f, 0.0f, 0.0f, 0.0f ),
      Math::Vector( 1.0f, 0.0f, 0.0f, 0.0f ),
      Math::Vector( 0.0f, 1.0f, 0.0f, 0.0f ),
      Math::Vector( 1.0f, 1.0f, 0.0f, 0.0f )
    };

    static u_int index[4] = { 0, 1, 2, 3 };
    float value = blendAlpha / 255.0f;

    m_pShaderObject[ShaderType::MultiTexture]->SetShader();
    m_pShaderObject[ShaderType::MultiTexture]->SetVector( this->m_hPsBlendAlpha, Math::Vector( value, value, value, value ) );

    GLuint textureHandles[3] = {textureHandle0, textureHandle1, textureHandle2};

    for( int i = 0; i < 3; ++i )
    {
      glActiveTexture( GL_TEXTURE0 + i );
      glEnable(GL_TEXTURE_2D);
      glBindTexture( GL_TEXTURE_2D, textureHandles[i] );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    }

    glEnableVertexAttribArray( GLInterface::StreamUsage::Position );
    glVertexAttribPointer( GLInterface::StreamUsage::Position, 4, GLInterface::VertexType::Float, GL_FALSE, sizeof(float) * 4, pos );

    glEnableVertexAttribArray( GLInterface::StreamUsage::TexCoord0 );
    glVertexAttribPointer( GLInterface::StreamUsage::TexCoord0, 4, GLInterface::VertexType::Float, GL_FALSE, sizeof(Math::Vector), uv );

    glDrawElements( GL_TRIANGLE_STRIP, sizeof(index) / sizeof(u_int), GL_UNSIGNED_INT, index );
  }

}