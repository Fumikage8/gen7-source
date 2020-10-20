//===================================================================
/**
 * @file    Win32Application.hpp
 * @brief   Win32アプリケーション メインフレームワーク
 * @author  GAME FREAK Inc.
 * @date    15.01.17
*/
//===================================================================

#include "IApplication.h"

#if defined(GF_PLATFORM_WIN32)

class Win32Application: public Main::Framework::IApplication
{
public:
  Win32Application();
  virtual~Win32Application();

  virtual bool OnInit(void);
  virtual bool OnUpdate(void);
  virtual bool OnDraw(void);
  virtual bool OnTerm(void);
};


#endif //defined(GF_PLATFORM_WIN32)
