#if !defined( __PAINT_UTIL_H__ )
#define	__PAINT_UTIL_H__

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextBrowser>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <gflib.h>
#include <vector>

#include <qt/Shader/ShaderObject.h>
#include "UvPaintWindowInfo.h"

#define DECLARE_ENUM_BEGIN( CLASSNAME )			\
	class CLASSNAME {													\
	public:																		\
		enum Enum {
#define DECLARE_ENUM_END( CLASSNAME )				\
		};																			\
	};

namespace Gui { 

class PaintUtil{
public:

	//-----------------------------------------------------------------------------
	//! シェーダータイプ
	DECLARE_ENUM_BEGIN( ShaderType )
		MultiTexture = 0,
		FillTexture,
		FillTextureAlpha,
		Brush,
		MultiSamplingTexture,
		NumberOf
	DECLARE_ENUM_END( ShaderType )

	static void Initialize();
	static void Terminate();

	struct FPoint{
		FPoint(){}
		FPoint(float _x, float _y){ x = _x; y = _y; }
		float				x, y;
	};

	struct DrawPointInfo {
		std::vector<FPoint>								pointList;
		s32																lineWidth;
		QColor														qlineColor;
		gfl::core::Vector2								screanSize;
	};

	static void DrawPoints( const DrawPointInfo &rInfo )
	{
		if ( s_Gp )
			s_Gp->drawPoints( rInfo );
	}

	struct DrawBucketInfo{
		QPoint									mousePos;
		QColor									color;
		int											tolerance;
		gfl::core::Vector2			RTSize;
		GLuint									frameBuffer;
		GLuint									renderTexture;
		GLuint									src_attachments;
		GLuint									dest_attachments;
	};

	static void DrawBucket( const DrawBucketInfo &rInfo )
	{
		if ( s_Gp )
			s_Gp->drawBucket( rInfo );
	}

	static void DrawTexture( GLuint textureHandle, GLuint samplingMode = GL_LINEAR, bool fAlpha = false )
	{
		if ( s_Gp )
			s_Gp->drawTexture( textureHandle, samplingMode, fAlpha );
	}

	struct MultiSamplingInfo {
		GLuint								m_TextureHandle;
		GLuint								m_SamplingMode;
		bool									m_fAlpha;
		gfl::core::Vector2		m_TextureSize;
	};

	static void MultiSamplingDrawTexture( const MultiSamplingInfo &rInfo )
	{
		if ( s_Gp )
			s_Gp->multiSamplingDrawTexture( rInfo );
	}

	static void DrawTextures( GLuint textureHandle0, GLuint textureHandle1, GLuint textureHandle2, float blendAlpha  = 1.0f )
	{
		if ( s_Gp )
			s_Gp->drawTextures( textureHandle0, textureHandle1, textureHandle2, blendAlpha );
	}

private:
	PaintUtil();
	~PaintUtil();

	void drawPoints( const DrawPointInfo &rInfo );
	void drawPoint( float x, float y );
	void drawBucket( const DrawBucketInfo &rInfo );
	void drawTexture( GLuint textureHandle, GLuint samplingMode = GL_LINEAR, bool fAlpha = false );
	void multiSamplingDrawTexture( const MultiSamplingInfo &rInfo );
	void drawTextures( GLuint textureHandle0, GLuint textureHandle1, GLuint textureHandle2, float blendAlpha  = 1.0f );

	static PaintUtil				*s_Gp;

	gfl::maya::GLInterface::ShaderObject				*m_pShaderObject[ShaderType::NumberOf];
	gfl::maya::GLInterface::IShader							*m_pVertexShader[ShaderType::NumberOf];
	gfl::maya::GLInterface::IShader							*m_pPixelShader[ShaderType::NumberOf];

	GLuint																			m_hVsScreanSize;
	GLuint																			m_hVsBrushSize;
	GLuint																			m_hVsBrushPos;
	GLuint																			m_hPsBrushColor;
	GLuint																			m_hPsFillAlpha;
	GLuint																			m_hPsMultiSamplingOffSet;
	GLuint																			m_hPsBlendAlpha;

	GLuint																			m_hBrushTexture;

};



}


#undef		DECLARE_ENUM_BEGIN
#undef		DECLARE_ENUM_END

#endif

