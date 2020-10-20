// ============================================================================
/*
 * @file SuperTrainingWork.h
 * @brief すごい特訓アプリのワーククラスです。
 * @date 2015.12.04
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( SUPER_TRAINIG_WORK_H_INCLUDE )
#define SUPER_TRAINIG_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"

//  アプリのパラメータ
#include "App/SuperTraining/include/SuperTrainingAppParam.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)
GFL_NAMESPACE_BEGIN(System)

  //  前方宣言
  class ViewSystemAccessor;

  class SuperTrainingWork : public NetAppLib::System::ApplicationWorkBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( SuperTrainingWork );

  private:

    static const int  APP_HEAP_SIZE             = 1024 * 1024 * 4;    //  4MB
    static const int  APP_DEVICE_HEAP_SIZE      = 1024 * 1024 * 10;   // 10MB

  public:
    SuperTrainingWork( APP_PARAM* pAppParam );
    virtual~SuperTrainingWork();

    //  Viewシステムの参照の設定・取得
    void SetViewSystemAccessor( ViewSystemAccessor* pAccessor )
    {
      GFL_ASSERT( pAccessor );

      m_pViewSystemAccessor = pAccessor;
    }
    ViewSystemAccessor* GetViewSystemAccessor() { return m_pViewSystemAccessor; }

    //  APPParamを取得
    APP_PARAM*  GetAppParam() const { return m_pAppParam; }

  private:
    void _Clear()
    {
      m_pAppParam           = NULL;
      m_pViewSystemAccessor = NULL;

      m_allockTrainerLoadID    = 0;
    }

    u32                 m_allockTrainerLoadID;
    APP_PARAM*          m_pAppParam;
    ViewSystemAccessor* m_pViewSystemAccessor;
  };



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)

#endif // SUPER_TRAINIGWORK_H_INCLUDE
