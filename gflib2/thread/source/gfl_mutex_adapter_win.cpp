//=============================================================================
/**
 * @file    gfl_mutex_adapter_win.cpp
 * @brief	 ミューテックス std実装
 * @author  ohno_katsumi
 * @date    2014.05.13
 */
//=============================================================================


#include <thread/source/gfl_mutex_adapter_win.h>

#if defined(GF_PLATFORM_WIN32)


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)


MutexAdapterWin::MutexAdapterWin(void)
{
}

void MutexAdapterWin::lock(void)
{
  mMutex.lock();
}


bool MutexAdapterWin::tryLock(void)
{
  return mMutex.try_lock();
}


void MutexAdapterWin::unlock(void)
{
  mMutex.unlock();
}

GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif //GFL_PRATFORM_WIN32
