//==============================================================================
/**
* @file ToolWidget.cpp
* @brief
* @author kimura_shin
* @data 2012/03/21, 15:51
*/
// =============================================================================

#include "../include/ToolWidget.h"
#include "../include/MainWindow.h"

#define KEY_DEVICE "Device"
#define KEY_LINE "Line"
#define KEY_TOLERANCE "Tolerance"
#define KEY_PENETRATE "Penetrate"

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
ToolWidget::ToolWidget( ) : QGroupBox( "ツール" ), m_Device( TOOL_NOT_SELECT )
{
  QVBoxLayout* v_lay = DbgNew QVBoxLayout;
  QHBoxLayout* h_lay = DbgNew QHBoxLayout;
  const c8 * const icon_tbl[] = {
    ":/point.png",
    ":/brush.png",
    ":/bucket.png",
    ":/syringe.png",
    ":/select.png",
  };

  const c8 * const sc_tbl[] = {
    "v",
    "b",
    "g",
    "i",
    "m"
  };

  const c8 * const tip_tbl[] = {
    "ポイント　モデルを選択します",
    "ブラシ　選択中の色を塗ります",
    "バケツ　区切られた範囲を塗ります",
    "スポイト　クリックしたテクスチャの色と抽出します",
    "範囲選択　ドラッグして範囲選択します",
  };

  for( s32 i = 0; i < TOOL_MAX; ++i )
  {
    IconButton* pb;
    m_pDevice[i] = pb = DbgNew IconButton( icon_tbl[i], i );
    pb->setShortcut( QKeySequence( sc_tbl[i] ) );
    connect( pb, SIGNAL( clicked( ) ), this, SLOT( Clicked( ) ) );
    pb->setToolTip( tip_tbl[i] );
    h_lay->addWidget( pb );
  }

  QSettings* st = MainWindow::GetInstance( )->GetSettings( );
  s32 id = st->value( KEY_DEVICE, TOOL_NOT_SELECT ).toInt( );

  if( TOOL_NOT_SELECT < id )
  {
    m_pDevice[id]->click( );
  }

  v_lay->addLayout( h_lay );

  h_lay = DbgNew QHBoxLayout;
  h_lay->addWidget( DbgNew QLabel( "線：" ) );

  QComboBox* cb = DbgNew QComboBox;
  m_pLine = cb;
  cb->setToolTip( "ブラシの太さを設定します" );
  cb->addItem( "5" );
  cb->addItem( "10" );
  cb->addItem( "15" );
  cb->addItem( "20" );
  cb->addItem( "25" );
  cb->addItem( "30" );
  cb->addItem( "40" );
  cb->addItem( "50" );
  cb->addItem( "60" );
  cb->addItem( "70" );
  cb->addItem( "80" );
  cb->addItem( "90" );
  cb->addItem( "100" );
  cb->setCurrentIndex( st->value( KEY_LINE, 3 ).toInt( ) );
  connect( cb, SIGNAL( currentIndexChanged( int ) ), this, SLOT( LineChanged( int ) ) );
  h_lay->addWidget( cb );

  h_lay->addWidget( DbgNew QLabel( "許容値：" ) );
  cb = DbgNew QComboBox;
  m_pTolerance = cb;
  cb->setToolTip( "範囲選択の誤差許容値を設定します" );

  for( s32 i = 0; i < 0x100; ++i ){
    cb->addItem( QString::number( i ) );
  }

  cb->setCurrentIndex( st->value( KEY_TOLERANCE, 0 ).toInt( ) );
  connect( cb, SIGNAL( currentIndexChanged( int ) ), this, SLOT( ToleranceChanged( int ) ) );
  h_lay->addWidget( cb );
  v_lay->addLayout( h_lay );

  h_lay = DbgNew QHBoxLayout;
  QCheckBox* cbx = DbgNew QCheckBox( "貫通塗り" );
  m_pPenetrate = cbx;
  cbx->setToolTip( "貫通塗りをしますか？" );
  cbx->setChecked( st->value( KEY_PENETRATE, 1 ).toInt( ) );
  connect( cbx, SIGNAL( stateChanged( int ) ), this, SLOT( PenetrateChanged( int ) ) );
  h_lay->addWidget( cbx );
  v_lay->addLayout( h_lay );

  setMaximumSize( 192, 128 );
  setLayout( v_lay );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
ToolWidget::~ToolWidget( ) { }

//------------------------------------------------------------------------------
// clicked
//------------------------------------------------------------------------------
void ToolWidget::Clicked( )
{
  IconButton* pb = ( IconButton* )sender( );
  const s32 id = pb->GetId( );
  static const QPalette off_color( m_pDevice[0]->palette( ) );
  static const QColor on_color( 0x80, 0x00, 0x00, 0x80 );
  bool on_flg = false;

  for( s32 i = 0; i < TOOL_MAX; ++i )
  {
    bool flg = ( i == id && m_Device != i );
    on_flg |= flg;
    m_pDevice[i]->setPalette( flg ? on_color : off_color );
  }

  QSettings* st = MainWindow::GetInstance( )->GetSettings( );

  if( on_flg ){
    st->setValue( KEY_DEVICE, id );
    m_Device = id;
  } 
  else
  {
    st->setValue( KEY_DEVICE, TOOL_NOT_SELECT );
    m_Device = TOOL_NOT_SELECT;
  }
}

//------------------------------------------------------------------------------
// Line changed
//------------------------------------------------------------------------------
void ToolWidget::LineChanged( int val )
{
  MainWindow::GetInstance( )->GetSettings( )->setValue( KEY_LINE, val );
}

//------------------------------------------------------------------------------
// Tolerance changed
//------------------------------------------------------------------------------
void ToolWidget::ToleranceChanged( int val )
{
  MainWindow::GetInstance( )->GetSettings( )->setValue( KEY_TOLERANCE, val );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ToolWidget::SetPenetrate(s32 val)
{
  gfl::cmdl::Model *pModel = MainWindow::GetInstance( )->GetGLWindow( )->GetGLWidget( )->GetModel( GLWidget::MODEL_ID );

  if( pModel )
  {
    gfl::cmdl::Material *pMaterial = pModel->GetMaterial( );

    for( int i = 0; i < pModel->GetMaterialCount( ); ++i )
    {
      pMaterial[i].DrawUvDepthTest( ( val == 0 ) ? true : false );
    }
  }
}

//------------------------------------------------------------------------------
// Penetrate changed
//------------------------------------------------------------------------------
void ToolWidget::PenetrateChanged( int val )
{
  MainWindow::GetInstance( )->GetSettings( )->setValue( KEY_PENETRATE, val );
  SetPenetrate(val);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void ToolWidget::SetPenetrateBySetting( void )
{
  QSettings* st = MainWindow::GetInstance( )->GetSettings( );
  SetPenetrate(st->value( KEY_PENETRATE, 1 ).toInt( ));
}
