//===================================================================
/**
 * @file    CommonApplication.hpp
 * @brief   コモンアプリケーション メインフレームワーク
 * @author  GAME FREAK Inc.
 * @date    15.01.17
*/
//===================================================================
#include "System/include/GflvUse.h"
#include "GameSystem/include/GameManager.h"
///#include <Debug/Launcher/include/LauncherProc.h>
/**
namespace Fieldtest
{
  class MyRenderingPipeLine;//@saito
}
*///@saito_del
namespace Field{
  class ViewerProc;
}

namespace Main{

class CommonApplication: public Main::Framework::IApplication
{
public:
  CommonApplication();

  virtual~CommonApplication();

  virtual bool OnInit(void);
  virtual bool OnUpdate(void);
  virtual bool OnDraw(void);
  virtual bool OnTerm(void);
  virtual void EndRequest(void);

private:

  GameSystem::GameManager*	 mpGameManager;
  gfl2::heap::NwAllocator * m_pMemAllocator;
  gfl2::heap::NwAllocator * m_pDevAllocator;

#if PM_DEBUG
  ///debug::launcher::LauncherProc::CallParam m_pLauncherParam;@saito不要
#endif  // PM_DEBUG

};

} //namespace Main
