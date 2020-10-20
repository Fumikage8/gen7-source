//===================================================================
/**
 * @file    CommonApplication.hpp
 * @brief   コモンアプリケーション メインフレームワーク
 * @author  GAME FREAK Inc.
 * @date    15.01.17
*/
//===================================================================
#include "System/include/GflUse.h"
#include <Debug/Launcher/include/LauncherProc.h>


class MyRenderingPipeLine;

namespace Main{

class CommonApplication: public Main::Framework::IApplication
{
public:
  CommonApplication();

  virtual~CommonApplication();

  virtual bool OnInit(void);
  virtual bool OnUpdate(void);
  virtual bool OnTerm(void);
private:

  GameSys::GameManager*												 mpGameManager;

#if PM_DEBUG
  debug::launcher::LauncherProc::CallParam m_pLauncherParam;

  bool DoAutoDebugTest(GameSys::GameManager* pGameManager,  const char* fileName);
#endif  // PM_DEBUG
};

} //namespace Main
