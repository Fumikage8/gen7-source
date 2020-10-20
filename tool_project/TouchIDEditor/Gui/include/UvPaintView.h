#if !defined( __UV_PAINT_VIEW_H__ )
#define __UV_PAINT_VIEW_H__

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
#include "PaintUtil.h"



#define DECLARE_ENUM_BEGIN( CLASSNAME )\
class CLASSNAME {\
public:\
  enum Enum {
#define DECLARE_ENUM_END( CLASSNAME )\
        };\
};

namespace Gui { 

  class UvPaintView : public QGLWidget {
  public:

    //-----------------------------------------------------------------------------
    //! レンダーターゲットタイプ
    DECLARE_ENUM_BEGIN( RTType )
    Main = 0,
      OpenTexture,
      Painted,
      Paint,
      MainViewPaint,
      Debug,
      NumberOf
      DECLARE_ENUM_END( RTType )

    UvPaintView(UvPaintWindowInfo &rInfo);
    virtual ~UvPaintView( void );

    void SetImageData( gfl::cmdl::Image *pImage );
    void CommenctQComboBox( QComboBox *pQComboBox );

    inline GLuint GetTextureId();
    inline GLuint GetFrameBuffer();

    //木村さん用アクセッサ
    inline gfl::core::Vector2 GetTextureSize() const;
    void CopyImageData( void *pCopyBuffer ) const;
    inline bool IsPainted() const;
    inline bool IsColorMap() const;
    inline void SetColorMapFlag( bool f);

    void SetRenderTarget( RTType::Enum type );

    public slots:
      void MotionChanged( const QString& fname );

  protected :
    /* override QGLWidget:: */ void initializeGL();
    /* override QGLWidget:: */ void resizeGL(int width, int height);
    /* override QGLWidget:: */ void paintGL();

    virtual void mousePressEvent( QMouseEvent *event );
    virtual void mouseMoveEvent( QMouseEvent *event );
    virtual void mouseReleaseEvent( QMouseEvent *event );
    virtual void keyPressEvent( QKeyEvent *event );

  private:
    UvPaintWindowInfo m_UvPaintWindowInfo;

    GLuint m_hBrushTexture;
    GLuint m_hOriginalTexture;
    gfl::cmdl::Image *m_pImageData;
    u16 m_Width;
    u16 m_Height;
    gfl::cmdl::Material::TextureCoordinator* m_pTextureCoordinator;
    gfl::cmdl::Material::TextureMapper* m_pTextureMapper;

    gfl::core::Vector2 m_RTSize;
    GLuint m_FrameBuffer;
    GLuint m_RenderBuffer;
    GLuint m_RenderTexture[RTType::NumberOf];
    int m_RenderRTNo;
    bool m_fRT1Clear;
    bool m_fRT3Clear;
    bool m_fFlush;
    bool m_fIsPainted;
    bool m_fIsColorMap;
    bool m_fMouseReaveFlush;
    HGLRC m_hRC;

    std::vector<PaintUtil::FPoint> m_MousePoints;
    GLuint m_TestID;

    void openTexture( GLuint textureHandle, gfl::cmdl::Material::TextureCoordinator *pTextureCoordinator = NULL, gfl::cmdl::Material::TextureMapper *pTextureMapper = NULL );

    char* readFile( const char* filename, int* p_size, int align = 1 );
  };

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  inline GLuint UvPaintView::GetTextureId()
  {
    return m_RenderTexture[0];
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  inline GLuint UvPaintView::GetFrameBuffer()
  {
    return m_FrameBuffer;
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  inline gfl::core::Vector2 UvPaintView::GetTextureSize() const
  {
    return m_RTSize;
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  inline bool UvPaintView::IsPainted() const
  {
    return m_fIsPainted;
    //return true;
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  inline bool UvPaintView::IsColorMap() const
  {
    return m_fIsColorMap;
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  inline void UvPaintView::SetColorMapFlag( bool f)
  {
    m_fIsColorMap = f;
  }

}


#undef DECLARE_ENUM_BEGIN
#undef DECLARE_ENUM_END

#endif
