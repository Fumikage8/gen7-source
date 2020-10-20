//
// CTR - Ro Module : NijiTemplateRo
//
#if PM_DEBUG

#ifdef GF_PLATFORM_CTR

#include <nn/fnd.h>
#include <nn/os.h>
#include "../include/ThreadTest.h"

//
// class definition.
//
class CNijiTemplateRo
{
public:
	CNijiTemplateRo()
	{
		NN_LOG("Module: CNijiTemplateRo (static initializer)\n");
	}
	~CNijiTemplateRo()
	{
		NN_LOG("Module: CNijiTemplateRo (static finalizer)\n");
	}
};

CNijiTemplateRo	s_NijiTemplateRo;

//
// function definition.
//
NN_DLL_EXPORT void	NijiTemplateRo(void)
{






}

#endif  //#ifdef GF_PLATFORM_CTR


#endif //PM_DEBUG
