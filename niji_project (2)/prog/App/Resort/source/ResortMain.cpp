//
// CTR - Ro Module : ResortMain
//

#ifdef GF_PLATFORM_CTR

#include <nn/fnd.h>
#include <nn/os.h>
#include "../include/ResortMain.h"

//
// class definition.
//
class CResortMain
{
public:
	CResortMain()
	{
		NN_LOG("Module: ResortMain (static initializer)\n");
	}
	~CResortMain()
	{
		NN_LOG("Module: ResortMain (static finalizer)\n");
	}
};

CResortMain	s_ResortMain;

//
// function definition.
//
NN_DLL_EXPORT void	ResortMain(void)
{

}

#endif  //#ifdef GF_PLATFORM_CTR


