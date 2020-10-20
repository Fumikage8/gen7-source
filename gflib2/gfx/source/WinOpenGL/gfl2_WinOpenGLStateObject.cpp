#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLStateObject.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  WinOpenGLBlendStateObjectクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
WinOpenGLBlendStateObject::WinOpenGLBlendStateObject() : BlendStateObject()
{
	m_FixData = m_StateData;
}

WinOpenGLBlendStateObject::~WinOpenGLBlendStateObject()
{
	
}

void WinOpenGLBlendStateObject::UpdateState()
{
	m_FixData = m_StateData;
}

void WinOpenGLBlendStateObject::Bind( const WinOpenGLSurface** ppSurface, u32 max )
{
	if ( m_FixData.m_AlphaToCoverageEnable )
		glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	else
		glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

	if ( m_FixData.m_AlphaTestEnable )
	{
		glEnable( GL_ALPHA_TEST );
		glAlphaFunc( GL_GEQUAL, m_FixData.m_AlphaRef );
	}
	else
	{
		glDisable( GL_ALPHA_TEST );
	}

	if ( m_FixData.m_BlendEnable )
	{
		glEnable( GL_BLEND );
		glDisable( GL_COLOR_LOGIC_OP );

		{
			GLenum		modeRGB = s_openglBlendOp[ m_FixData.m_ColorBlendOp ];
			GLenum		modeAlpha = s_openglBlendOp[ m_FixData.m_AlphaBlendOp ];

			glBlendEquationSeparate( modeRGB, modeAlpha );
		}

		{
			GLenum		srcRGB = s_openglBlendFunc[ m_FixData.m_SrcColorBlendFunc ];
			GLenum		dstRGB = s_openglBlendFunc[ m_FixData.m_DestColorBlendFunc ];
			GLenum		srcAlpha = s_openglBlendFunc[ m_FixData.m_SrcAlphaBlendFunc ];
			GLenum		dstAlpha = s_openglBlendFunc[ m_FixData.m_DestAlphaBlendFunc ];

			glBlendFuncSeparate( srcRGB, dstRGB, srcAlpha, dstAlpha );
		}
	}
	else
	{
		glDisable( GL_BLEND );
		glDisable( GL_COLOR_LOGIC_OP );
	}

	for( u32 i = 0; i < max; ++i )
	{
		if ( ppSurface[i] )
		{
			glColorMaski( i,m_FixData.m_RenderTargetWriteMask[i] & COLOR_WRITE_ENABLE_RED,
											m_FixData.m_RenderTargetWriteMask[i] & COLOR_WRITE_ENABLE_GREEN,
											m_FixData.m_RenderTargetWriteMask[i] & COLOR_WRITE_ENABLE_BLUE,
											m_FixData.m_RenderTargetWriteMask[i] & COLOR_WRITE_ENABLE_ALPHA );
		}
	}
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  WinOpenGLRasterizerStateObjectクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
WinOpenGLRasterizerStateObject::WinOpenGLRasterizerStateObject() : RasterizerStateObject()
{
	m_FixData = m_StateData;
}

WinOpenGLRasterizerStateObject::~WinOpenGLRasterizerStateObject()
{
	
}

void WinOpenGLRasterizerStateObject::UpdateState()
{
	m_FixData = m_StateData;
}

void WinOpenGLRasterizerStateObject::Bind()
{
	if ( m_StateData.m_MultisampleEnable )
	{
		glEnable( GL_MULTISAMPLE );
	}
	else
	{
		glDisable( GL_MULTISAMPLE );
	}

	if ( m_FixData.m_DepthBias )
	{
		glEnable( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset( 1.0f, static_cast<float>( m_FixData.m_DepthBias ) );
	}
	else
	{
		glDisable( GL_POLYGON_OFFSET_FILL );
	}

	{
		glPolygonMode( GL_FRONT_AND_BACK, s_openglFillMode[m_FixData.m_FillMode] );
	}

	{
    // GFではCWが表, CCWが裏
		if ( m_FixData.m_CullMode == CullMode::None )
		{
			glDisable( GL_CULL_FACE );
		}
		else
		{
			glEnable( GL_CULL_FACE );
			glCullFace( s_openglCullMode[m_FixData.m_CullMode] );
		}
	}
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  WinOpenGLDepthStencilStateObjectクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
WinOpenGLDepthStencilStateObject::WinOpenGLDepthStencilStateObject() : DepthStencilStateObject()
{
	m_FixData = m_StateData;
}

WinOpenGLDepthStencilStateObject::~WinOpenGLDepthStencilStateObject()
{
	
}

void WinOpenGLDepthStencilStateObject::UpdateState()
{
	m_FixData = m_StateData;
}

void WinOpenGLDepthStencilStateObject::Bind()
{
	if ( m_FixData.m_DepthTestEnable )
	{
		glEnable( GL_DEPTH_TEST );
	}
	else
	{
		glDisable( GL_DEPTH_TEST );
	}


	static const GLint func[] = { GL_LESS, GL_LEQUAL, GL_EQUAL, GL_GEQUAL, GL_GREATER, GL_NOTEQUAL, GL_ALWAYS, GL_NEVER };
	static const GLint stencil_op[] = { GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_INCR_WRAP, GL_DECR, GL_DECR_WRAP, GL_INVERT };

	glDepthFunc( func[ m_FixData.m_DepthFunc ] );
	glDepthMask( m_FixData.m_DepthWriteEnable ? GL_TRUE : GL_FALSE );

	if ( m_FixData.m_StencilTestEnable )
	{
		glEnable( GL_STENCIL_TEST );
		glStencilMask( m_FixData.m_SetStencilWriteMask );

		for( int face = 0; face < PolygonFace::NumberOf; ++face )
		{
			int			setFace = face;
			GLuint	glFace = (face == PolygonFace::CCW) ? GL_FRONT : GL_BACK;

			if ( m_FixData.m_TwoSideStencilTestEnable == false && face == PolygonFace::CW )
			{
				setFace = PolygonFace::CCW;
			}

			glStencilFuncSeparate(glFace,
														s_openglCompareFunc[ m_FixData.m_StencilFunc[ setFace ] ],
														m_FixData.m_StencilRef,
														m_FixData.m_StencilMask);

			glStencilOpSeparate( glFace,
				s_openglStencilOp[ m_FixData.m_StencilFailOp[setFace] ],
				s_openglStencilOp[ m_FixData.m_StencilDepthFailOp[setFace] ],
				s_openglStencilOp[ m_FixData.m_StencilPassOp[setFace] ]);
		}
	}
	else
	{
		glDisable( GL_STENCIL_TEST );
	}
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  WinOpenGLDepthStencilStateObjectクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
WinOpenGLSamplerStateObject::WinOpenGLSamplerStateObject() : SamplerStateObject()
{
	m_FixData = m_StateData;

  // サンプラーオブジェクト作成
  glGenSamplers(1, &m_SamplerHandle);
}

WinOpenGLSamplerStateObject::~WinOpenGLSamplerStateObject()
{
  // サンプラーオブジェクト削除
  glDeleteSamplers(1, &m_SamplerHandle);
}

void WinOpenGLSamplerStateObject::UpdateState()
{
	m_FixData = m_StateData;
}

void WinOpenGLSamplerStateObject::Bind( u32 texNo, const WinOpenGLTexture *pTexture ) const
{
	if ( pTexture )
	{
		s32						mipLevel = pTexture->GetTextureDesc().mipLevels;
		Format				format = pTexture->GetTextureDesc().format;

    // texNoのテクスチャユニットにサンプラーをバインド
		glBindSampler( texNo, m_SamplerHandle );

		if ( (mipLevel > 1) && (m_FixData.m_MipmapLodBias != 0.0f) )
		{
      glSamplerParameteri( m_SamplerHandle, GL_TEXTURE_MIN_FILTER, s_openglTextureFilter[m_FixData.m_MinFilter][m_FixData.m_MipFilter] );
			glSamplerParameterf( m_SamplerHandle,GL_TEXTURE_LOD_BIAS, m_FixData.m_MipmapLodBias );
		}
		else
		{
			glSamplerParameteri( m_SamplerHandle, GL_TEXTURE_MIN_FILTER, (m_FixData.m_MinFilter == TextureFilter::Point) ? GL_NEAREST : GL_LINEAR );
			glSamplerParameterf( m_SamplerHandle,GL_TEXTURE_LOD_BIAS, 0.0 );
		}
    glSamplerParameteri( m_SamplerHandle, GL_TEXTURE_MAG_FILTER, (m_FixData.m_MagFilter == TextureFilter::Point) ? GL_NEAREST : GL_LINEAR );

		glSamplerParameteri( m_SamplerHandle, GL_TEXTURE_WRAP_S, s_openglTextureAddress[m_FixData.m_WrapS] );
		glSamplerParameteri( m_SamplerHandle, GL_TEXTURE_WRAP_T, s_openglTextureAddress[m_FixData.m_WrapT] );
		glSamplerParameteri( m_SamplerHandle, GL_TEXTURE_WRAP_R, s_openglTextureAddress[m_FixData.m_WrapR] );
		glSamplerParameterfv( m_SamplerHandle, GL_TEXTURE_BORDER_COLOR, &m_FixData.m_BorderColor.x );

		if ( format == Format::D24S8 )
		{
			//glSamplerParameteri ( m_SamplerHandle, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_STENCIL_INDEX );
		}

		//ＧＬは、MipLevel調整をＡＰＩでなく、シェーダーサンプラ命令で行う
	}
}


}}}
