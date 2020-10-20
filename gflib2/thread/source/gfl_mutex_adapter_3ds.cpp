//=============================================================================
/**
 * @file	 gfl_mutex_adapter_3ds.cpp
 * @brief	 ミューテックス 3ds
 * @author k.ohno
 * @date	 2015.1.23
 */
//=============================================================================
#if defined( GF_PLATFORM_CTR )

#include <thread/source/gfl_mutex_adapter_3ds.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)


MutexAdapter3ds::MutexAdapter3ds(void)
{
  mMutex.Initialize();
}

MutexAdapter3ds::~MutexAdapter3ds()
{
  mMutex.Finalize();
}

void MutexAdapter3ds::lock(void)
{
  mMutex.Lock();
}


bool MutexAdapter3ds::tryLock(void)
{
  return mMutex.TryLock();
}


void MutexAdapter3ds::unlock(void)
{
  mMutex.Unlock();
}


GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif  // GF_PLATFORM_CTR
