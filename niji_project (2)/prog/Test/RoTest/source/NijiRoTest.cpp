//
// CTR - Ro Module : NijiTestRo
//
#if PM_DEBUG

#if defined(GF_PLATFORM_CTR)

#include <nn/fnd.h>
#include <nn/os.h>

#endif  //defined(GF_PLATFORM_CTR)

#include "../include/NijiRoTest.h"

//
// class definition.
//
class CNijiTestRo
{
public:
	CNijiTestRo()
	{
		//NN_LOG("Module: CNijiTemplateRo (static initializer)\n");
	}
	~CNijiTestRo()
	{
		//NN_LOG("Module: CNijiTemplateRo (static finalizer)\n");
	}
};

CNijiTestRo	s_CNijiTestRo;

//
// function definition.
//
#if defined(GF_PLATFORM_CTR)
NN_DLL_EXPORT void	NijiTestRo(void)
#elif defined(GF_PLATFORM_WIN32)
void	NijiTestRo(void)
#endif
{

}

#endif // PM_DEBUG
