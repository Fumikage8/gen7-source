#if !defined( __BucketJob_H__ )
#define __BucketJob_H__

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

#define DECLARE_ENUM_BEGIN( CLASSNAME )\
class CLASSNAME {\
public:\
  enum Enum {
#define DECLARE_ENUM_END( CLASSNAME )\
};\
};

namespace Gui { 

  class BucketJob {
  public:

    struct RGBA32{
      union{
        struct
        {
          unsigned char data[4];
        };

        struct{
          unsigned char r;
          unsigned char g;
          unsigned char b;
          unsigned char a;
        };

        struct
        {
          unsigned int idata;
        };
      };

      bool CheckRGB( RGBA32 color, int tolerance );
      void SetRGB( RGBA32 color );
    };

    struct Info{
      QPoint m_MousePos;
      QColor m_Color;
      RGBA32 *m_pImageData;
      RGBA32 *m_pDstImageData;
      gfl::core::Vector2 m_ImageSize;
      int m_Tolerance;
    };

    static int GetImagePos( int x, int y );
    static void DoIt( Info &rInfo );

  private:

    static Info s_Info;
    static RGBA32 s_StartColor;
    static RGBA32 s_ReplaceColor;
    static bool *s_fChecked;

    BucketJob();
    ~BucketJob();

    void Draw( int x, int y );

  };
}


#undef DECLARE_ENUM_BEGIN
#undef DECLARE_ENUM_END

#endif
