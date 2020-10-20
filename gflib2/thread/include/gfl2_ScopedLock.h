#ifndef __GFL2_SCOPEDLOCK_H__
#define __GFL2_SCOPEDLOCK_H__
//=============================================================================
/**
 * @file	 gfl2_Scopedlock.h
 * @brief	 スコープドロック置き換え
 * @author k.ohno
 * @date	 January 21, 2015
 */
//=============================================================================
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>


/*
    3DSではクリティカルセクションが推奨されていますので、
    ＰＣでも同じクラス名で扱えるように宣言しなおしています。
*/


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)





// 3DS
//src/savedata/AccessPointSaveData.cpp:146:  nn::os::CriticalSection::ScopedLock lock(m_criticalSection); 
//
// nn::os::CriticalSection  m_criticalSection


//PC
//std::lock_guard<std::mutex> lock(g_i_mutex);  std::lock_guard<std::mutex> lock(print_mtx_);
//std::mutex print_mtx_;


//共通

//  gfl2::thread::CriticalSection m_criticalSection;
//  gfl2::thread::ScopedLock lock(m_criticalSection);



#if defined(GF_PLATFORM_WIN32)

class CriticalSection : public ::std::recursive_mutex
{
public:
  void Initialize(void){};
  void Finalize(void){};
};
//typedef std::mutex CriticalSection;
typedef ::std::lock_guard<::std::recursive_mutex> ScopedLock;
#elif defined(GFL_PLATFORM_3DS)
typedef nn::os::CriticalSection CriticalSection;
typedef nn::os::CriticalSection::ScopedLock ScopedLock;
#else
#endif




GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif// __GFL2_SCOPEDLOCK_H__
