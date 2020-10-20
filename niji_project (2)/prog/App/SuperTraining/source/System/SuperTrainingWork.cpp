// ============================================================================
/*
 * @file SuperTrainingWork.cpp
 * @brief すごい特訓アプリのワーククラスです。
 * @date 2015.12.04
 * @author uchida_yuto
 */
// ============================================================================
#include "App/SuperTraining/source/System/SuperTrainingWork.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)
GFL_NAMESPACE_BEGIN(System)


  SuperTrainingWork::SuperTrainingWork( APP_PARAM* pAppParam ) :
    ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE )
  {
    _Clear();

    GFL_ASSERT( pAppParam );
    m_pAppParam = pAppParam;

  }


  SuperTrainingWork::~SuperTrainingWork()
  {
  }


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)
