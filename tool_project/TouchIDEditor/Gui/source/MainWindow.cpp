
#include "../include/MainWindow.h"
#include "../include/ToolWidget.h"
#include "../include/IdWidget.h"
#include "../include/UvPaintView.h"
#include "../include/PaintUtil.h"
#include "../include/ResourceConv.h"

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

#include "TouchIDEditor/HeapDefine.h"

GFL_SINGLETON_SUBSTANCE( MainWindow )

#define KEY_LOAD_DIR "LoadDir"

  // -----------------------------------------------------------------------------
  // Construct
  // -----------------------------------------------------------------------------
  MainWindow::MainWindow( gfl::heap::HeapBase* )
{
  gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEVICE);
  allocator = new GLAllocator(devHeap);

  gfl2::gfx::GLMemory::SetNewParameterAllocator(allocator);

  SetInstance( this );
  setWindowIcon( QIcon( ":/app.png" ) );
  setWindowFlags( windowFlags( ) & ~Qt::WindowMaximizeButtonHint );

  m_pUndoStack = DbgNew QUndoStack( this );
  m_pSettings = DbgNew QSettings( "GameFreak", "TouchIdEditor" );

  CreateAction( );
  CreateMenu( );

  m_pGLWindow = DbgNew GLWindow;

  setCentralWidget( m_pGLWindow );

  QDockWidget* dw = DbgNew QDockWidget;
  QVBoxLayout* v_lay = DbgNew QVBoxLayout;

  m_pToolWidget = DbgNew ToolWidget;
  v_lay->addWidget( m_pToolWidget );
  m_pIdWidget = DbgNew IdWidget;
  v_lay->addWidget( m_pIdWidget );
  m_pDisplayWidget = DbgNew DisplayWidget;
  v_lay->addWidget( m_pDisplayWidget );
  m_pMotionWidget = DbgNew MotionWidget;
  v_lay->addWidget( m_pMotionWidget );

  QWidget* panel = DbgNew QWidget;
  panel->setLayout( v_lay );
  dw->setWidget( panel );
  dw->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
  addDockWidget( Qt::RightDockWidgetArea, dw );

  dw = DbgNew QDockWidget;
  m_pTextureWidget = DbgNew TextureWidget;
  dw->setWidget( m_pTextureWidget );
  addDockWidget( Qt::LeftDockWidgetArea, dw );

  m_pDockWindow = DbgNew QMainWindow(this);
  m_pDockWindow->setWindowFlags( m_pDockWindow->windowFlags() & ~(Qt::WindowCloseButtonHint|Qt::WindowMaximizeButtonHint) );

  Gui::PaintUtil::Initialize( );

  show( );
}

// ---------------------------------------------------------------------------
// Destruct
// ---------------------------------------------------------------------------
MainWindow::~MainWindow( void )
{
  Gui::PaintUtil::Terminate( );

  for( unsigned int i = 0; i < m_pUvPaintWindowList.size( ); ++i ){
    delete m_pUvPaintWindowList[i];
  }

  while( !m_ReloadImage.isEmpty( ) ){
    delete m_ReloadImage.takeFirst( );
  }

  delete allocator;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MainWindow::CreateAction( )
{
  // ファイル
  m_pLoadGfmdlFile = DbgNew QAction( "モデル gfmdl をロード", this );
  m_pLoadGfmdlFile->setShortcut( QKeySequence::Open );
  connect( m_pLoadGfmdlFile, SIGNAL( triggered( ) ), this, SLOT( LoadGfmdl( ) ) );

  CreateRecentFileAction( );

  m_pSaveIdFile = DbgNew QAction( "ID テクスチャを保存", this );
  m_pSaveIdFile->setShortcut( QKeySequence::Save );
  connect( m_pSaveIdFile, SIGNAL( triggered( ) ), this, SLOT( SaveId( ) ) );

  // 編集
  m_pUndo = m_pUndoStack->createUndoAction( this, "やり直し" );
  m_pUndo->setShortcut( QKeySequence::Undo );
  m_pRedo = m_pUndoStack->createRedoAction( this, "やり直しのやり直し" );
  m_pRedo->setShortcut( QKeySequence::Redo );

  // ヘルプ
  m_pHelpXml = DbgNew QAction( "TouchId.xml の説明", this );
  connect( m_pHelpXml, SIGNAL( triggered( ) ), this, SLOT( ShowHelpXml( ) ) );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MainWindow::CreateMenu( )
{
  m_pFileMenu = menuBar( )->addMenu( "ファイル" );
  m_pFileMenu->addAction( m_pLoadGfmdlFile );

  CreateRecentFileMenu( m_pFileMenu );

  m_pFileMenu->addAction( m_pSaveIdFile );

  m_pActionMenu = menuBar( )->addMenu( "編集" );
  m_pActionMenu->addAction( m_pUndo );
  m_pActionMenu->addAction( m_pRedo );

  m_pHelpMenu = menuBar( )->addMenu( "ヘルプ" );
  m_pHelpMenu->addAction( m_pHelpXml );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool MainWindow::LoadFile( const QString& fname )
{
  m_LastLoadFileName = fname;
  // 別スレッドで gui を作成出来無いので仕方ない
  QDialog mesdia( this );
  QVBoxLayout* vlay = DbgNew QVBoxLayout;
  QLabel* lb = DbgNew QLabel( QString( "Loading... %1" ).arg( fname ) );
  vlay->addWidget( lb );
  mesdia.setLayout( vlay );
  mesdia.show( );
  mesdia.repaint( );

  GLWidget* wid = GetGLWindow( )->GetGLWidget( );

#ifdef WAIT_LOAD
  while( wid->GetRenderState( ) )
  {
    gfl::core::Time::SleepMilliSecond( 100 );
  }
  wid->SetRenderState( GLWidget::RENDER_WAIT );
#endif

  for( s32 i = 0; i < GLWidget::MODEL_MAX; ++i )
  {
    gfl::cmdl::Model* model = ( ( GLWidget* )m_pGLWindow->_pGLWidget )->GetModel( i );

    if( model )
    {
      model->Clear( );
    }
  }

  for( unsigned int i = 0; i < m_pUvPaintWindowList.size( ); ++i ){
    delete m_pUvPaintWindowList[i];
  }

  m_pUvPaintWindowList.clear( );

  gfl::cmdl::Model* model = DbgNew gfl::cmdl::Model;
  gfl::cmdl::Model* id_model = DbgNew gfl::cmdl::Model;

  m_IdLoaded = false;

  // モーション読み込み
  QString rootdir(fname.section('/', 0, -2));
  QDir::setCurrent(rootdir);
  gfl::core::File::s_RootDir = rootdir + '/';

  // Gfmdl読み込み
  gfl2::renderingengine::scenegraph::resource::GfMdlData modelResourceData;
  modelResourceData.ReadFile(fname.toStdString());

  auto pModelResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(allocator, &modelResourceData);

  // 読み込み失敗したら無視
  if (pModelResourceRootNode == NULL)
  {
    return false;
  }

  auto pModelInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(allocator, pModelResourceRootNode);

  wid->Clear( );

  ResourceConv::ToModel(model, pModelInstanceNode);
  wid->SetModel( model );
  wid->SetModelInstanceNode(pModelInstanceNode);

  ResourceConv::ToModel(id_model, pModelInstanceNode);
  wid->SetModel( id_model, GLWidget::MODEL_ID );

  m_pTextureWidget->LoadTexture( model, id_model );

  m_pMotionWidget->SetMotionList( );

  QGridLayout* g_lay = DbgNew QGridLayout;

  if( 1 )
  {
    //UvPaintWindow作成
    QList<gfl::cmdl::Image*>* pImageList = model->GetImageData( );

    int num = pImageList->size( );

    s32 lcnt = pImageList->size( );

    for( int i = 0; i < pImageList->size( ); ++i )
    {
      gfl::cmdl::Image *pImage = ( gfl::cmdl::Image* )( pImageList->at( i ) );

      QString imageName = "Textures/" + pImage->GetFileName();
      QByteArray imageNameBytes = imageName.toAscii( );
      std::string imageNameStr = imageNameBytes.data( );

      imageName.replace( ".tga", "Id.tga" );
      bool reload;

      if( QFile::exists( imageName ) )
      {
        gfl::cmdl::Image* pReloadImage = DbgNew gfl::cmdl::Image;
        *pReloadImage = *pImage;
        pReloadImage->Load( imageName );

        // Swap R & B
        const s32 ssize = pReloadImage->GetWidth( ) * pReloadImage->GetHeight( );
        u32* simg = ( u32* )pReloadImage->GetImage( );

        for( s32 j = 0; j < ssize; ++j, ++simg )
        {
          u32 rgb = *simg;
          *simg = ( ( rgb & 0xff ) << 16 ) | ( ( rgb & 0xff0000 ) >> 16 ) | ( rgb & 0xff00ff00 );
        }

        pReloadImage->SetRGB_Type( GL_RGBA );
        pReloadImage->SetOrderType( GL_UNSIGNED_BYTE );
        m_ReloadImage.append( pReloadImage );
        pImage = pReloadImage;
        reload = true;
        m_IdLoaded = true;
      } 
      else
      {
        reload = false;
      }

      Gui::UvPaintWindowInfo info( this, model, pImage, reload );
      info.m_ShareWidget = m_pGLWindow->GetGLWidget( );

      Gui::UvPaintWindow *pUvPaintWindow = DbgNew Gui::UvPaintWindow( info );

      g_lay->addWidget( pUvPaintWindow->GetQDockWidget( ), i >> 3, i & 7 );
      m_pUvPaintWindowList.push_back( pUvPaintWindow );
    }

    // 全部設定させてから表示させる
    QWidget* w = DbgNew QWidget;
    w->setLayout( g_lay );
    w->show( );

    m_pDockWindow->setCentralWidget( w );
    m_pDockWindow->show( );

    for( int i = 0; i < pImageList->size( ); ++i )
    {
      m_pUvPaintWindowList.at( i )->GetQDockWidget( )->show( );
    }

    {
      //IDモデルにテクスチャを設定
      gfl::cmdl::Material* pMaterial = id_model->GetMaterial( );

      for( int i = 0; i < id_model->GetMaterialCount( ); ++i )
      {
        gfl::cmdl::Material::TextureCoordinator *pTextureCoordinator = pMaterial[i].GetTextureCoordinator( );
        gfl::cmdl::Material::TextureMapper *pTextureMapper = pMaterial[i].GetTextureMapper( );

        if( pTextureCoordinator )
        {
          for( int i2 = 0; i2 < pMaterial[i].GetTextureCoordinatorCount( ); ++i2 )
          {
            pTextureCoordinator[i2].SetScaleS( 1.0f );
            pTextureCoordinator[i2].SetScaleT( 1.0f );
            pTextureCoordinator[i2].SetTranslateS( 0.0f );
            pTextureCoordinator[i2].SetTranslateT( 0.0f );
          }
        }

        if( pTextureMapper )
        {
          for( int i2 = 0; i2 < pMaterial[i].GetTextureMapperCount( ); ++i2 )
          {
            QString imageName = pTextureMapper[i2].GetFileName( );
            QByteArray imageNameBytes = imageName.toAscii( );
            std::string imageNameStr = imageNameBytes.data( );

            pTextureMapper[i2].SetWrapS( GL_REPEAT );
            pTextureMapper[i2].SetWrapT( GL_REPEAT );

            for( unsigned int i3 = 0; i3 < m_pUvPaintWindowList.size( ); ++i3 )
            {
              if( imageName == m_pUvPaintWindowList[i3]->GetImageFileName( ) )
              {
                *pTextureMapper[i2].GetTextureId( ) = m_pUvPaintWindowList[i3]->GetTextureId( );

                if( i2 == 0 )
                {
                  //カラーマップである。
                  m_pUvPaintWindowList[i3]->GetUvPaintView( )->SetColorMapFlag( true );
                }
                break;
              }
            }
          }
        }
      }
    }
  }

  m_pDockWindow->showMinimized( );
  m_pIdWidget->CheckId( fname );
  GetToolWidget( )->SetPenetrateBySetting( );
  adjustSize( );
  wid->m_ShowAllStep = 1;

#ifdef WAIT_LOAD
  wid->SetRenderState( GLWidget::RENDER_NONE );
#endif

  SetCurrentFile( fname );

  if( !m_IdLoaded )
  {
    startTimer( 1000 );
  }

  return true;
}


//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MainWindow::LoadGfmdl( )
{
  QString fname = QFileDialog::getOpenFileName( this, ".gfmdl ファイルを選択してください。", m_pSettings->value( KEY_LOAD_DIR ).toString( ), "Gfmdl (*.gfmdl)" );

  if( !fname.isEmpty( ) )
  {
    m_pSettings->setValue( KEY_LOAD_DIR, fname.section( '/', 0, -2 ) );
    LoadFile( fname );
  }
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MainWindow::SaveId( )
{
  SaveIdExec( );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MainWindow::SaveIdExec( bool mes_on )
{
  gfl::cmdl::Model* model = m_pGLWindow->GetGLWidget( )->GetModel( GLWidget::MODEL_ID );
  QString savefname, used_color;
  QMap<u32, s32> col_map;

  if( model )
  {
    std::vector<Gui::UvPaintWindow*> list = GetUvPaintWindowList( );
    QMap<QString, bool> chkmap;

    s32 org_alpha = m_pDisplayWidget->GetAlpha( GLWidget::MODEL_ID );

    if( 0xff != org_alpha )
    {
      QMessageBox::information( this, "WatchOut!", "右ペインにある [表示]->[IDα] スライダーバーを右端に設定してください。" );
      return;
    }

    for( u32 i = 0; i < list.size( ); ++i )
    {
      if( list[i]->GetUvPaintView( )->IsColorMap( ) )
      {
        QString fname( list[i]->GetImageFileName( ) );

        if( chkmap.contains( fname ) )
        {
          continue;
        }

        chkmap.insert( fname, true );
        fname.replace( ".tga", "Id.tga" );

        gfl::core::Vector2 textureSize = list[i]->GetUvPaintView( )->GetTextureSize( );

        s32 w = ( s32 )textureSize.GetX( );
        s32 h = ( s32 )textureSize.GetY( );

        const s32 size = w*h;

        u32 *copy_buffer, *copy_buffer_org;
        copy_buffer = copy_buffer_org = DbgNew u32 [size];
        list[i]->GetUvPaintView( )->CopyImageData( copy_buffer );

        u32 *save_buffer, *save_buffer_org;
        save_buffer = save_buffer_org = DbgNew u32 [size];

        for( s32 j = 0; j < size; ++j )
        {
          u32 rgb = *copy_buffer++;

          if( !col_map.value( rgb ) && m_pIdWidget->m_IdMap.value( rgb ) )
          {
            col_map.insert( rgb, 1 );
          }

          *save_buffer++ = ( ( rgb & 0xff ) << 16 ) | ( ( rgb & 0xff0000 ) >> 16 ) | ( rgb & 0xff00ff00 );
        }

        gfl::qt::Tga::Save("Textures/" + fname, ( c8* )save_buffer_org, w, h, 32, gfl::qt::Tga::IT_FULL_COLOR );

        // RGB->L8
        u8 *l8, *l8org;
        l8 = l8org = DbgNew u8 [size];

        copy_buffer = copy_buffer_org;
        //				u32 bef=0;
        for( s32 j = 0; j < size; ++j ){
          u32 c = ( *copy_buffer++ ) | 0xff000000;
          *l8++ = m_pIdWidget->m_IdMap.value( c );
        }

        gfl::qt::Ctex::ToCtex(l8org, w, h);

        fname = "ctex/" + fname.replace("Id.tga", ".ctex");

        QDir qdir = QDir();
        qdir.cdUp();
        fname = qdir.absoluteFilePath(fname);

        gfl::xml::Document doc;
        doc.Load(fname);
        gfl::xml::Node* node = doc.RootNode();

        if(node)
        {
          node = node->FirstNode("GraphicsContentCtr")->FirstNode("Textures")->FirstNode("ImageTextureCtr");

          node->SetAttribute("Width", w);
          node->SetAttribute("Height", h);
          node->SetAttribute("Format", "L8");

          node = node->FirstNode("Images");
          node->remove_first_node();
          
          node = node->AddNode("PixelBasedImageCtr");
          u32 enc_size;
          c8* enc = gfl::core::Utility::EncodeBase64(reinterpret_cast<c8*>(l8org), size, &enc_size);
          node->SetValue(enc, enc_size);

          fname.replace(".ctex", "Id.ctex");
          doc.Save(fname);
          delete enc;
        }

        delete [] copy_buffer_org;
        delete [] save_buffer_org;
        delete [] l8org;

        savefname.append( fname + "\n" );
      }
    }

    QString buf;
    QMap<QString, QComboBox*>* lst = m_pTextureWidget->GetMaterialTreatment( );
    QStringList ky( lst->keys( ) );

    const c8 * const mt_tbl[] = {
#define SET_MATERIAL_TREATMENT(name,txt) #name,
#include "../include/TextureWidgetDef.h"
    };

    for( s32 j = 0; j < ky.size( ); ++j )
    {
      buf.append( ky.at( j ) + " " + QString( mt_tbl[lst->value( ky.at( j ) )->currentIndex( )] ) + "\n" );
    }

    gfl::qt::File::Save( "Material.txt", buf.toAscii( ) );

    QList<u32> col_lst( col_map.keys( ) );

    for( s32 j = 0; j < col_lst.size( ); ++j )
    {
      used_color.append( m_pIdWidget->m_IdMapString.value( col_lst.at( j ) ) + "\n" );
    }
  }

  if( mes_on && !savefname.isEmpty( ) )
  {
    QMessageBox::information( this, "Saved", "IDテクスチャーが保存されました。\n" + savefname );

    if( !used_color.isEmpty( ) )
    {
      QMessageBox::information( this, "Saved", "＊使用されたID色＊\n" + used_color );
    }
  }
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MainWindow::CheckId( )
{
  if( m_IdLoaded ){
    return;
  }

  // 別スレッドで gui を作成出来無いので仕方ない
  QDialog mesdia( this );
  QVBoxLayout* vlay = DbgNew QVBoxLayout;
  QLabel* lb = DbgNew QLabel( QString( "元モデルにID色が使われていないかチェックしています…" ) );
  vlay->addWidget( lb );
  mesdia.setLayout( vlay );
  mesdia.show( );
  mesdia.repaint( );

  SaveIdExec( false );

  // カレントディレクトリを設定
  QDir dir( QDir::currentPath( ) + "/Textures" );
  dir.setFilter( QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks );

  // モーションファイルを取得
  QFileInfoList f_lst = dir.entryInfoList( QStringList( "*Id.tga" ) );
  QMap<u32, s32> col_map;

  for( s32 i = 0; i < f_lst.size( ); ++i )
  {
    QFileInfo fi = f_lst.at( i );
    gfl::qt::Tga tga;
    if( tga.Load( fi.filePath( ) ) )
    {
      u8* c = ( u8* )tga.GetImage( );
      for( s32 j = 0; j < tga.GetHeight( ); ++j )
      {
        for( s32 k = 0; k < tga.GetWidth( ); ++k )
        {
          u32 rgba = ( c[0] << 16 ) | ( c[1] << 8 ) | ( c[2] ) | ( c[3] << 24 );

          if( m_pIdWidget->m_IdMap.value( rgba ) )
          {
            c[0] = c[1] = c[2] = 0x00;
            c[3] = 0xff;

            if( !col_map.value( rgba ) )
            {
              col_map.insert( rgba, 1 );
            }
          }

          c += 4; // rgba size
        }
      }

      tga.Save(fi.filePath( ));
    }
  }

  if( col_map.size( ) )
  {
    QMessageBox::information( this, "Checked", "元モデルテクスチャーにID色があったので自動リロードします" );
    LoadFile( m_LastLoadFileName );
  } 
  else
  {
    QMessageBox::information( this, "Checked", "元モデルテクスチャーにID色はありません" );
  }
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MainWindow::timerEvent( QTimerEvent* event )
{
  CheckId( );
  killTimer( event->timerId( ) );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MainWindow::ShowHelpXml( )
{
  m_pHelpDialog = new QDialog( this );

  QVBoxLayout* lay = new QVBoxLayout;
  QTextBrowser* tb = new QTextBrowser;
  tb->setHtml( gfl::qt::File::Load( "help.html" ) );
  lay->addWidget( tb );
  m_pHelpDialog->setLayout( lay );

  m_pHelpDialog->resize( 640, 480 );
  m_pHelpDialog->show( );
}
