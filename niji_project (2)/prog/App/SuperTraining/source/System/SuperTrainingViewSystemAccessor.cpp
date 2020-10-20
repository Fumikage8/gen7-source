// ============================================================================
/*
 * @file SuperTrainingViewSystemAccessor.h
 * @brief すごい特訓アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.12.07
 * @author uchida_yuto
 */
// ============================================================================
#include "App/SuperTraining/source/System/SuperTrainingViewSystemAccessor.h"

//  gflib2のインクルード
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)
GFL_NAMESPACE_BEGIN(System)


  ViewSystemAccessor::ViewSystemAccessor(
    App::SuperTraining::System::ViewList* pViewList,
    App::SuperTraining::System::ViewObjects* pViewObjects )
  {
    GFL_ASSERT( pViewList );
    GFL_ASSERT( pViewObjects );

    m_pViewList     = pViewList;
    m_pViewObjects  = pViewObjects;
  }


  ViewSystemAccessor::~ViewSystemAccessor()
  {
  }

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)
