// ============================================================================
/*
 * @file SuperTrainingViewSystemAccessor.h
 * @brief すごい特訓アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.12.07
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( SUPER_TRAINING_VIEWSYSTEMACCESSOR_H_INCLUDE )
#define SUPER_TRAINING_VIEWSYSTEMACCESSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

//  View関連のインクルード
#include "App/SuperTraining/source/System/SuperTrainingViewObjects.h"
#include "App/SuperTraining/source/System/SuperTrainingViewList.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)
GFL_NAMESPACE_BEGIN(System)

  class ViewSystemAccessor
  {
    GFL_FORBID_COPY_AND_ASSIGN( ViewSystemAccessor );

  public:

    ViewSystemAccessor(
      App::SuperTraining::System::ViewList* pViewList,
      App::SuperTraining::System::ViewObjects* pViewObjects );

    virtual~ViewSystemAccessor();

    App::SuperTraining::System::ViewList*     GetViewList() { return m_pViewList; }
    App::SuperTraining::System::ViewObjects*  GetViewObjects() { return m_pViewObjects; }

  private:

    App::SuperTraining::System::ViewList*     m_pViewList;
    App::SuperTraining::System::ViewObjects*  m_pViewObjects;
  };

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)


#endif // SUPER_TRAINING_VIEWSYSTEMACCESSOR_H_INCLUDE
