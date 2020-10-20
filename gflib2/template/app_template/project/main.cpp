//
// CTR - Application : CTR2ProjectApp
//

#include <stdio.h>

#ifdef GF_PLATFORM_CTR

#include <nn/os.h>
#include <nn/dbg.h>

extern "C" void nnMain()
{
  NN_LOG("Hello World");
  while(1){}
}
#else  //#ifdef GF_PLATFORM_CTR

#if 0
extern "C" void main()
{
  while(1){}
}

#endif //#ifdef GF_PLATFORM_CTR
