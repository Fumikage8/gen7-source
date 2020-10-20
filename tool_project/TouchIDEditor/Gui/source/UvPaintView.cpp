#include "../include/UvPaintView.h"
#include "../include/GLUtil.h"
#include "../include/MainWindow.h"
#include <string>
#include <vector>

#include "../include/MultiTextureShader.h"
#include "../include/BrushShader.h"
#include "../include/BucketJob.h"
#include "../include/PaintUtil.h"

using namespace gfl::maya;
using namespace gfl::maya::plugins::Shader;
using namespace gfl::maya::GLInterface;

namespace Gui { 

  static const unsigned char  s_BrushTgaCode[] = {
#include "Brush.tga.txt"
  };

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  UvPaintView::UvPaintView( UvPaintWindowInfo &rInfo ) : 
    QGLWidget( rInfo.m_ShareWidget, rInfo.m_ShareWidget )
  {
    makeCurrent();

    bool f = isSharing();

    m_UvPaintWindowInfo = rInfo; //コピー

    SetImageData( rInfo.m_pImageData );

    setMinimumSize(16,64);
    resize(256,256);

    m_pTextureCoordinator = m_UvPaintWindowInfo.GetTextureCoordinator();
    m_pTextureMapper = m_UvPaintWindowInfo.GetTextureMapper();

    {
      //IDモデルにテクスチャを設定
      gfl::cmdl::Material* pMaterial = rInfo.m_pModel->GetMaterial();

      for( int i = 0; i < rInfo.m_pModel->GetMaterialCount(); ++i )
      {
        gfl::cmdl::Material::TextureCoordinator *pTextureCoordinator = pMaterial[i].GetTextureCoordinator();
        gfl::cmdl::Material::TextureMapper *pTextureMapper = pMaterial[i].GetTextureMapper();

        for ( int i2 = 0; i2 < pMaterial[i].GetTextureMapperCount(); ++i2 )
        {
          QString imageName = pTextureMapper[i2].GetFileName();
          QByteArray imageNameBytes = imageName.toAscii();
          std::string imageNameStr = imageNameBytes.data();

          {
            if ( imageName == m_pImageData->GetFileName() )
            {
              m_TestID = *pTextureMapper[i2].GetTextureId();
              break;
            }
          }
        }
      }
    }

    setFocusPolicy( Qt::StrongFocus );
    m_RenderRTNo = 0;
    m_fRT1Clear = false;
    m_fIsPainted = false;
    m_fIsColorMap = false;
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  UvPaintView::~UvPaintView()
  {
    makeCurrent();

    {
      if ( m_FrameBuffer )
      {
        glDeleteFramebuffersEXT( 1, &m_FrameBuffer );
        m_FrameBuffer = 0;
      }

      if ( m_RenderBuffer )
      {
        glDeleteRenderbuffersEXT( 1, &m_RenderBuffer );
        m_RenderBuffer = 0;
      }

      glDeleteTextures( RTType::NumberOf, m_RenderTexture );	
    }

    glDeleteTextures( 1, &m_hOriginalTexture );
    glDeleteTextures( 1, &m_hBrushTexture );
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void UvPaintView::CommenctQComboBox( QComboBox *pQComboBox )
  {
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void UvPaintView::SetImageData( gfl::cmdl::Image* pImageData )
  {
    m_pImageData = pImageData;
    m_Width = pImageData->GetWidth();
    m_Height = pImageData->GetHeight();
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void UvPaintView::MotionChanged( const QString& fname )
  {
    QByteArray	nameBytes = fname.toAscii();
    std::string	nameStr = nameBytes.data();

    nameStr = "";
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void UvPaintView::initializeGL()
  {
    makeCurrent();

    qglClearColor( QColor(0, 0, 255) );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    {
      TgaFormat *pImage = (TgaFormat*)s_BrushTgaCode;
      char* pImageTop = (char*)&s_BrushTgaCode[ sizeof(TgaFormat) ];

      glGenTextures( 1, &m_hBrushTexture );
      glBindTexture( GL_TEXTURE_2D, m_hBrushTexture );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, pImage->Width, pImage->Hight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImageTop );
      glBindTexture( GL_TEXTURE_2D, 0 ); //リセット
    }

    if ( 0 )
    {
      std::string starPath = "C:/home/xy_tools/DCCTools/Maya/Sample/pm0078_00_Body1.tga";
      int size;
      char* pFileData = readFile( starPath.c_str(), &size );
      TgaFormat *pImage = (TgaFormat*)pFileData;

      char* pImageTop = (char*)&pFileData[ sizeof(TgaFormat) ];

      glGenTextures( 1, &m_hOriginalTexture );
      glBindTexture( GL_TEXTURE_2D, m_hOriginalTexture );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, pImage->Width, pImage->Hight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImageTop );
      glBindTexture( GL_TEXTURE_2D, 0 ); //リセット

      delete pFileData;
    }
    else
    {
      //リソースはここでつくらないといけないみたい。コンテキスト関係か？
      glGenTextures( 1, &m_hOriginalTexture );
      glBindTexture( GL_TEXTURE_2D, m_hOriginalTexture );
      {
        c8* pImage = m_pImageData->GetImage();
        int width = m_pImageData->GetWidth();
        int height = m_pImageData->GetHeight();
        s32 type = m_pImageData->GetRGB_Type();
        s32 o_type = m_pImageData->GetOrderType();
        glTexImage2D( GL_TEXTURE_2D, 0, type, width, height, 0, type, o_type, pImage );
      }
      glBindTexture( GL_TEXTURE_2D, 0 ); //リセット
    }

    {
      m_RTSize.Set(512, 512);

      // テクスチャ4枚生成
      glGenTextures( RTType::NumberOf, m_RenderTexture );

      // 4枚分の設定をしておく
      glEnable( GL_TEXTURE_2D );

      for( int i=0; i < RTType::NumberOf; i++ )
      {
        glBindTexture( GL_TEXTURE_2D, m_RenderTexture[i] );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_RTSize.GFL_VECTOR_X, m_RTSize.GFL_VECTOR_Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
        glBindTexture( GL_TEXTURE_2D, 0 );
      }

      glDisable( GL_TEXTURE_2D );

      // レンダーバッファの設定
      glGenRenderbuffersEXT( 1, &m_RenderBuffer );
      glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_RenderBuffer );
      glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, m_RTSize.GFL_VECTOR_X, m_RTSize.GFL_VECTOR_Y );

      // フレームバッファの設定
      glGenFramebuffersEXT( 1, &m_FrameBuffer );
      glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_FrameBuffer );

      for( int i=0; i < RTType::NumberOf; i++ )
      {
        glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, m_RenderTexture[i], 0 );
      }

      glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_RenderBuffer );
      glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

      m_fRT3Clear = false;
      m_fFlush = true; //初期化フラッシュ
      m_fMouseReaveFlush = false;
    }

    glUseProgram( 0 );
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void UvPaintView::resizeGL(int width, int height)
  {
    makeCurrent();

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float x = width / height;

    glFrustum( -x, x, -1.0, 1.0, 1.0, 1000.0 );
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void UvPaintView::mousePressEvent( QMouseEvent *event )
  {
    makeCurrent();

    MainWindow *pMainWindow = dynamic_cast<MainWindow*>( m_UvPaintWindowInfo.m_pParent );
    QColor qlineColor = pMainWindow->GetIdWidget()->GetIdColor();
    s32 deviceType = pMainWindow->GetToolWidget()->GetDevice();
    s32 tolerance = pMainWindow->GetToolWidget()->GetTolerance();

    if ( deviceType == ToolWidget::TOOL_BUCKET )
    {
      PaintUtil::FPoint pos;

      pos.x = static_cast<float>(event->pos().x()) / static_cast<float>(this->size().width());
      pos.y = static_cast<float>(event->pos().y()) / static_cast<float>(this->size().height());

      pos.x *= this->m_RTSize.GFL_VECTOR_X;
      pos.y *= this->m_RTSize.GFL_VECTOR_Y;

      PaintUtil::DrawBucketInfo info;

      info.mousePos = QPoint(pos.x, pos.y);
      info.color = qlineColor;
      info.tolerance = tolerance;
      info.RTSize.Set( this->m_RTSize );
      info.frameBuffer = this->m_FrameBuffer;
      info.renderTexture = this->m_RenderTexture[2];
      info.src_attachments = GL_COLOR_ATTACHMENT0_EXT;
      info.dest_attachments = GL_COLOR_ATTACHMENT2_EXT;

      PaintUtil::DrawBucket( info );
      m_fIsPainted = true;
    }
    else
    {
      mouseMoveEvent( event );
    }
    repaint();
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void UvPaintView::mouseMoveEvent( QMouseEvent *event )
  {
    MainWindow *pMainWindow = dynamic_cast<MainWindow*>( m_UvPaintWindowInfo.m_pParent );
    QColor qlineColor = pMainWindow->GetIdWidget()->GetIdColor();
    s32 deviceType = pMainWindow->GetToolWidget()->GetDevice();

    makeCurrent();

    if( event->buttons( ) & Qt::LeftButton )
    { // 書き込み
      QPoint mousePos = event->pos();
      int test = this->size().width();
      test = this->size().height();

      PaintUtil::FPoint pos;

      pos.x = static_cast<float>(mousePos.x()) / static_cast<float>(this->size().width());
      pos.y = static_cast<float>(mousePos.y()) / static_cast<float>(this->size().height());

      pos.x *= 2.0f;
      pos.x -= 1.0f;

      pos.y *= 2.0f;
      pos.y -= 1.0f;
      pos.y *= -1.0f;

      m_MousePoints.push_back( pos );

      m_fIsPainted = true;
      repaint();
    }

    if ( m_MousePoints.size() > 5 )
    {
      //これ以上溜め込まない。一度フラッシュ。
      m_fFlush = true;
    }
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void UvPaintView::mouseReleaseEvent( QMouseEvent *event )
  {
    m_fFlush = true;
    m_fMouseReaveFlush = true;
    repaint();
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void UvPaintView::keyPressEvent( QKeyEvent * event )
  {
    makeCurrent();

    switch( event->key() )
    {
    case Qt::Key_F1:
      m_RenderRTNo = RTType::Main;
      break;
    case Qt::Key_F2:
      m_RenderRTNo = RTType::OpenTexture;
      break;
    case Qt::Key_F3:
      m_RenderRTNo = RTType::Painted;
      break;
    case Qt::Key_F4:
      m_RenderRTNo = RTType::Paint;
      break;
    case Qt::Key_F5:
      m_RenderRTNo = RTType::MainViewPaint;
      break;
    }
    repaint();
  }

  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  /// @brief
  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  void UvPaintView::paintGL()
  {
    static const GLuint attachments[4] = 
    {
      GL_COLOR_ATTACHMENT0_EXT,
      GL_COLOR_ATTACHMENT1_EXT,
      GL_COLOR_ATTACHMENT2_EXT,
      GL_COLOR_ATTACHMENT3_EXT,
    }; 

    makeCurrent();
    glUseProgram( 0 );

    {
      glPushAttrib(GL_ALL_ATTRIB_BITS);

      //　フレームバッファをバインド
      glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_FrameBuffer );

      //　ビューポート設定
      glViewport( 0, 0, m_RTSize.GFL_VECTOR_X, m_RTSize.GFL_VECTOR_Y );

      glDisable( GL_DEPTH_TEST );
      glDisable( GL_CULL_FACE );
      glDisable( GL_BLEND );

      {
        //現在書き込み中描画
        glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + RTType::Paint );
        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        MainWindow *pMainWindow = dynamic_cast<MainWindow*>( m_UvPaintWindowInfo.m_pParent );
        s32 deviceType = pMainWindow->GetToolWidget()->GetDevice();

        switch(deviceType)
        {
        case ToolWidget::TOOL_POINT:

        case ToolWidget::TOOL_BRUSH:
          {
            MainWindow *pMainWindow = dynamic_cast<MainWindow*>( m_UvPaintWindowInfo.m_pParent );
            s32 lineWidth = pMainWindow->GetToolWidget()->GetLine();
            QColor qlineColor = pMainWindow->GetIdWidget()->GetIdColor();

            PaintUtil::DrawPointInfo info;
            info.pointList = m_MousePoints;
            info.lineWidth = lineWidth;
            info.qlineColor = qlineColor;
            info.screanSize.GFL_VECTOR_X = this->size().width();
            info.screanSize.GFL_VECTOR_Y = this->size().height();

            PaintUtil::DrawPoints( info );
          }
          break;

        default:
          break;
        }
      }

      if ( m_fRT3Clear == false )
      {
        //一緒にクリア
        glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + RTType::MainViewPaint );
        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
      }

      {
        glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + RTType::Painted );

        if ( m_fRT3Clear == false )
        {
          glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
          glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
          m_fRT3Clear = true;
        }

        if ( m_fFlush )
        {
          //書き込み蓄積

          //蓄積前
          {
            MainWindow *pMainWindow = dynamic_cast<MainWindow*>( m_UvPaintWindowInfo.m_pParent );
            s32 deviceType = pMainWindow->GetToolWidget()->GetDevice();

            switch(deviceType)
            {
            case ToolWidget::TOOL_POINT:
            case ToolWidget::TOOL_BRUSH:
              {
                MainWindow *pMainWindow = dynamic_cast<MainWindow*>( m_UvPaintWindowInfo.m_pParent );
                s32 lineWidth = pMainWindow->GetToolWidget()->GetLine();
                QColor qlineColor = pMainWindow->GetIdWidget()->GetIdColor();

                PaintUtil::DrawPointInfo			info;
                info.pointList = m_MousePoints;
                info.lineWidth = lineWidth;
                info.qlineColor = qlineColor;
                info.screanSize.GFL_VECTOR_X = this->size().width();
                info.screanSize.GFL_VECTOR_Y = this->size().height();

                PaintUtil::DrawPoints( info );
                //drawPoints();
              }
              break;

            default:
              break;
            }
          }
          //蓄積後

          if ( m_fMouseReaveFlush == false && m_MousePoints.size() )
          {
            PaintUtil::FPoint		lastPos = m_MousePoints[ m_MousePoints.size() - 1 ];
            m_MousePoints.clear();//蓄積したのでログ削除
            m_MousePoints.push_back(lastPos);
          }
          else
          {
            m_MousePoints.clear();//蓄積したのでログ削除
          }

          m_fFlush = false;
          m_fMouseReaveFlush = false;
        }

        {
          //３Ｄビューからの書き込み
          glEnable( GL_ALPHA_TEST );
          glAlphaFunc( GL_GREATER, 0 );

          PaintUtil::MultiSamplingInfo info;

          info.m_TextureHandle = m_RenderTexture[RTType::MainViewPaint];
          info.m_SamplingMode = GL_NEAREST;
          info.m_fAlpha = false;
          info.m_TextureSize.Set( this->m_RTSize );

          PaintUtil::MultiSamplingDrawTexture( info );

          {
            //書き込んだのでクリア
            glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + RTType::MainViewPaint );
            glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
          }
        }
      }

      if ( m_fRT1Clear == false )
      {
        //テクスチャを展開してレンダリング
        glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + RTType::OpenTexture );

        //　FBOのバッファをクリア
        glClearColor( 0, 0, 0, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glUseProgram( 0 );

        if(m_UvPaintWindowInfo.m_LoadId)
        {
          openTexture( m_hOriginalTexture, 0, 0 );
        }
        else
        {
          openTexture( m_hOriginalTexture, m_pTextureCoordinator, m_pTextureMapper );
        }

        m_fRT1Clear = true;
      }

      {
        //合成
        MainWindow *pMainWindow = dynamic_cast<MainWindow*>( m_UvPaintWindowInfo.m_pParent );
        float modelAlpha = pMainWindow->GetDisplayWidget()->GetAlpha( GLWidget::MODEL_ID );

        glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + RTType::Main );
        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        PaintUtil::DrawTextures(m_RenderTexture[RTType::OpenTexture], m_RenderTexture[RTType::Painted], m_RenderTexture[RTType::Paint], modelAlpha );
        glUseProgram( 0 );
      }

      //　フレームバッファのアンバインド
      glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

      glPopAttrib();
    }

    glClearColor( 1, 1, 1, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    PaintUtil::DrawTexture( m_RenderTexture[m_RenderRTNo], GL_NEAREST );
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void UvPaintView::openTexture( GLuint textureHandle, gfl::cmdl::Material::TextureCoordinator *pTextureCoordinator, gfl::cmdl::Material::TextureMapper *pTextureMapper )
  {
    gfl::core::Vector4 pos[4] = 
    {
      gfl::core::Vector4(-1.0f,-1.0f, 0.0f, 1.0f ),
      gfl::core::Vector4( 1.0f,-1.0f, 0.0f, 1.0f ),
      gfl::core::Vector4(-1.0f, 1.0f, 0.0f, 1.0f ),
      gfl::core::Vector4( 1.0f, 1.0f, 0.0f, 1.0f )
    };

    gfl::core::Vector2 uv[4] = 
    {
      gfl::core::Vector2( 0.0f, 0.0f ),
      gfl::core::Vector2( 1.0f, 0.0f ),
      gfl::core::Vector2( 0.0f, 1.0f ),
      gfl::core::Vector2( 1.0f, 1.0f )
    };

    if ( pTextureCoordinator )
    {
      f32 scaleS = pTextureCoordinator->GetScaleS();
      f32 scaleT = pTextureCoordinator->GetScaleT();
      f32 transS = pTextureCoordinator->GetTranslateS();
      f32 transT = pTextureCoordinator->GetTranslateT();
      f32 rotate = pTextureCoordinator->GetRotate();

      for( int i = 0; i < 4; ++i )
      {
        gfl::core::Vector2 rotuv;

        //Mayaはテクスチャが移動するので逆スクロール
        uv[i].GFL_VECTOR_X -= transS;
        uv[i].GFL_VECTOR_Y -= transT;

        rotuv.GFL_VECTOR_X = uv[i].GFL_VECTOR_X * cos( rotate ) - uv[i].GFL_VECTOR_Y * sin( rotate );
        rotuv.GFL_VECTOR_Y = uv[i].GFL_VECTOR_X * sin( rotate ) + uv[i].GFL_VECTOR_Y * cos( rotate );
        uv[i].Set( rotuv );

        uv[i].GFL_VECTOR_X *= scaleS;
        uv[i].GFL_VECTOR_Y *= scaleT;
      }
    }

    static u_int index[4] = { 0, 1, 2, 3 };
    s32 wrapU = GL_CLAMP_TO_EDGE;
    s32 wrapV = GL_CLAMP_TO_EDGE;

    if ( pTextureMapper )
    {
      wrapU = pTextureMapper->GetWrapS();
      wrapV = pTextureMapper->GetWrapT();
    }

    {
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
    }

    {
      glEnable(GL_TEXTURE_2D);
      glBindTexture( GL_TEXTURE_2D, textureHandle );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapU);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapV);

      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    }

    {
      glEnableClientState( GL_VERTEX_ARRAY );
      glVertexPointer ( 4, GL_FLOAT, 0, pos );

      /* 頂点テクスチャ座標 */
      glEnableClientState( GL_TEXTURE_COORD_ARRAY );
      glTexCoordPointer ( 2, GL_FLOAT, 0, uv );

      glDrawElements( GL_TRIANGLE_STRIP, sizeof(index) / sizeof(u_int), GL_UNSIGNED_INT, index );
    }
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  char* UvPaintView::readFile( const char* filename, int* p_size, int align )
  {
    FILE *hFile;
    int file_size;
    char *read_buff;

    hFile = fopen( filename, "rb" );

    if ( hFile == NULL )
    {
      char	mess[256];	
      memset( mess, 0, 256 );
      sprintf( mess, "%s can not file open.\n", filename );
      printf( mess );
      return NULL;
    }

    fseek( hFile, 0, SEEK_END );
    file_size = ftell( hFile );
    *p_size = file_size;
    fseek( hFile, 0, SEEK_SET );

    file_size += align; //必ずNULLが入るようにしたい。
    size_t buf_size = file_size + ((-file_size) & (align-1));

    {
      char	mess[256];	
      memset( mess, 0, 256 );
      sprintf( mess, "%d %d %d\n", buf_size, file_size, align );
    }

    read_buff = DbgNew char[buf_size];
    memset( read_buff, 0, buf_size );
    fread( read_buff, file_size, 1, hFile );
    fclose( hFile );


    return( read_buff );
  }

  //------------------------------------------------------------------------------
  // 
  //------------------------------------------------------------------------------
  void UvPaintView::CopyImageData( void *pCopyBuffer ) const
  {
    static const GLuint attachments[4] = 
    {
      GL_COLOR_ATTACHMENT0_EXT,
      GL_COLOR_ATTACHMENT1_EXT,
      GL_COLOR_ATTACHMENT2_EXT,
      GL_COLOR_ATTACHMENT3_EXT,
    }; 

    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_FrameBuffer );
    glReadBuffer( attachments[0] );
    glReadPixels( 0, 0, m_RTSize.GFL_VECTOR_X, m_RTSize.GFL_VECTOR_Y, GL_RGBA, GL_UNSIGNED_BYTE, pCopyBuffer );
  }

  //------------------------------------------------------------------------------
  // 
  //------------------------------------------------------------------------------
  void UvPaintView::SetRenderTarget( RTType::Enum type )
  {
    //　フレームバッファをバインド
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_FrameBuffer );

    //現在書き込み中描画
    glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + type );
  }

}