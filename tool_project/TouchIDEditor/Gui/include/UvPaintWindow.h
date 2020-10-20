#if !defined( __UV_PAINT_WINDOW_H__ )
#define	__UV_PAINT_WINDOW_H__

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
#include <string>

#include "UvPaintWindowInfo.h"
#include "UvPaintView.h"

namespace Gui { 

  class UvPaintWindow : QObject {
    Q_OBJECT
  public:
    enum {
      TEX_SIZE = 128,
      DOCK_WIDTH = 16,
      DOCK_HEIGHT = 64
    };

    UvPaintWindow( UvPaintWindowInfo &rInfo );
    ~UvPaintWindow( void );

    inline QDockWidget* GetQDockWidget();
    inline QString GetImageFileName();
    inline GLuint GetTextureId();
    inline UvPaintView* GetUvPaintView() const;

  private:

    void SetImageData( gfl::cmdl::Image *pImage );


    QMainWindow *m_pParent;
    QDockWidget *m_MainDockWidget;

    UvPaintView *m_pUvPaintView;

    QComboBox *m_pRepeatUSetting;
    QComboBox *m_pRepeatVSetting;
    QString m_ImageFileName;

    public slots:
      void LevelChanged(bool);
  };

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  inline QDockWidget* UvPaintWindow::GetQDockWidget()
  {
    return m_MainDockWidget;
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  inline QString UvPaintWindow::GetImageFileName()
  {
    return m_ImageFileName;
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  inline GLuint UvPaintWindow::GetTextureId()
  {
    return m_pUvPaintView->GetTextureId();
  }

  //-----------------------------------------------------------------------------
  /// @brief
  //-----------------------------------------------------------------------------
  inline UvPaintView* UvPaintWindow::GetUvPaintView() const
  {
    return m_pUvPaintView;
  }

}

#endif
