#if PM_DEBUG
//
// CTR - Ro Module : IslandTestMain
//

#ifdef GF_PLATFORM_CTR

#include <nn/fnd.h>
#include <nn/os.h>
#include "../include/IslandTestMain.h"

//
// class definition.
//
class CIslandTestMain
{
public:
	CIslandTestMain()
	{
		NN_LOG("Module: IslandTestMain (static initializer)\n");
	}
	~CIslandTestMain()
	{
		NN_LOG("Module: IslandTestMain (static finalizer)\n");
	}
};

CIslandTestMain	s_IslandTestMain;

//
// function definition.
//
NN_DLL_EXPORT void	IslandTestMain(void)
{

}

#endif  //#ifdef GF_PLATFORM_CTR

#endif // PM_DEBUG
