#include "../include/UvPaintWindow.h"
#include "../include/GLUtil.h"
#include <string>
#include <vector>

namespace Gui { 

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  UvPaintWindow::UvPaintWindow( UvPaintWindowInfo &rInfo )
  {
    m_pParent = rInfo.m_pParent;

    m_ImageFileName = rInfo.m_pImageData->GetFileName();

    QByteArray imageNameBytes = m_ImageFileName.toAscii();
    std::string imageNameStr = imageNameBytes.data();

    // オブジェクト
    m_MainDockWidget = DbgNew QDockWidget( imageNameStr.c_str() );

    m_MainDockWidget->setObjectName( imageNameStr.c_str() ); // ウィンドウ設定を保存するときの固有名
    m_MainDockWidget->setAllowedAreas( Qt::TopDockWidgetArea|Qt::BottomDockWidgetArea );
    m_MainDockWidget->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );

    QVBoxLayout* m_pVLayout = DbgNew QVBoxLayout;

    QCheckBox* m_pQCheckBox = DbgNew QCheckBox( "書き込み有効" );
    m_pQCheckBox->setCheckState( Qt::Checked );
    m_pVLayout->addWidget( m_pQCheckBox );

    m_pUvPaintView = DbgNew Gui::UvPaintView( rInfo );
    m_pVLayout->addWidget( m_pUvPaintView );

    QWidget* m_pPanelWidget = DbgNew QWidget;
    m_pPanelWidget->setLayout( m_pVLayout );
    m_MainDockWidget->setWidget( m_pPanelWidget );

    LevelChanged(false);
    connect( m_MainDockWidget, SIGNAL( topLevelChanged(bool)), this, SLOT( LevelChanged(bool) ) );
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  UvPaintWindow::~UvPaintWindow( void )
  {
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  void UvPaintWindow::SetImageData( gfl::cmdl::Image *pImage )
  {
    m_pUvPaintView->SetImageData( pImage );
  }

  //------------------------------------------------------------------------------
  // 
  //------------------------------------------------------------------------------
  void UvPaintWindow::LevelChanged(bool flag)
  {
    s32 tex;
    tex = flag? 2048 : 128;

    QSize size(tex,tex);
    m_pUvPaintView->setMaximumSize( size );

    if(!flag)
    {
      m_pUvPaintView->setMinimumSize( size );
    }

    size.setWidth(tex+DOCK_WIDTH);
    size.setHeight(tex+DOCK_HEIGHT);
    m_MainDockWidget->setMaximumSize( size );
  }
}
