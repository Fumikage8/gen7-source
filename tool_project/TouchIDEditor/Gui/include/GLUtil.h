#if !defined( __GL_UTIL_H__ )
#define	__GL_UTIL_H__

#include <gflib.h>

namespace Gui { 

#define GL_ERR()\
  {\
  switch( glGetError() )\
  {\
  case GL_NO_ERROR:\
  break;\
  case GL_INVALID_ENUM:\
  GFL_ASSERT_MSG( FALSE, "GL_INVALID_ENUM" );\
  break;\
  case GL_INVALID_VALUE:\
  GFL_ASSERT_MSG( FALSE, "GL_INVALID_VALUE" );\
  break;\
  case GL_INVALID_OPERATION:\
  GFL_ASSERT_MSG( FALSE, "GL_INVALID_OPERATION" );\
  break;\
  case GL_STACK_OVERFLOW:\
  GFL_ASSERT_MSG( FALSE, "GL_STACK_OVERFLOW" );\
  break;\
  case GL_STACK_UNDERFLOW:\
  GFL_ASSERT_MSG( FALSE, "GL_STACK_UNDERFLOW" );\
  break;\
  case GL_OUT_OF_MEMORY:\
  GFL_ASSERT_MSG( FALSE, "GL_OUT_OF_MEMORY" );\
  break;\
  case GL_TABLE_TOO_LARGE:\
  GFL_ASSERT_MSG( FALSE, "GL_TABLE_TOO_LARGE" );\
  break;\
  default:\
  GFL_ASSERT_MSG( FALSE, (char*)::gluErrorString( glGetError() ) );\
  break;\
}\
}\

  //-----------------------------------------------------------------------------
  /**
  * @struct TgaFormat
  * @brief TgaFormat情報
  */
# pragma pack (1)//ちょっと反則
  struct TgaFormat
  {
    unsigned char No_ID; /* 0=イメージＩＤ無し*/
    unsigned char CMap_Type; /* 0=カラーマップ無し */
    unsigned char Image_Type; /* 2=無圧縮RGBタイプ 10=圧縮RGBタイプ */
    unsigned char CMap_Spec[5]; /* スキップされる*/

    unsigned short Ox; /* 画面 左上 X 0固定 */
    unsigned short Oy; /* 画面 左上 Y 0固定 */

    unsigned short Width; /* 幅 (dot)*/
    unsigned short Hight; /* 高さ (dot)*/

    unsigned char Pixel; /* Pixel Size. 32=32bit */
    unsigned char IDesc_Type; /* 8=TGA32 アトリビュート */
  };
# pragma pack ()

#define RGBA32(r, g, b, a) (a << 24) | (b << 16) | (g << 8) | (r << 0)

}

#endif

