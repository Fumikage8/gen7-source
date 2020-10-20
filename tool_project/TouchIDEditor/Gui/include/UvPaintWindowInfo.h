#if !defined( __UV_PAINT_WINDOW_INFO_H__ )
#define	__UV_PAINT_WINDOW_INFO_H__

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

namespace Gui { 

  struct UvPaintWindowInfo
  {
    UvPaintWindowInfo(){}
    UvPaintWindowInfo( QMainWindow *pParent, gfl::cmdl::Model *pModel, gfl::cmdl::Image *pImageData, bool load_id );

    gfl::cmdl::Material::TextureCoordinator* GetTextureCoordinator();
    gfl::cmdl::Material::TextureMapper* GetTextureMapper();

    QMainWindow *m_pParent;
    QGLWidget *m_ShareWidget;
    gfl::cmdl::Model *m_pModel;
    gfl::cmdl::Image *m_pImageData;
    bool m_LoadId;
  };


}

#endif


