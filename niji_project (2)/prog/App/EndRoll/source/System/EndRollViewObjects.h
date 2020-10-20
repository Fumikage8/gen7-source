// ============================================================================
/*
 * @file EndRollEndRollViewObjects.h
 * @brief エンドロールアプリのViewクラスを持っているクラスです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( ENDROLL_VIEW_OBJECTS_H_INCLUDE )
#define ENDROLL_VIEW_OBJECTS_H_INCLUDE
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

//  Viewのインクルード
#include "App/EndRoll/source/System/EndRollViewList.h"
#include "App/EndRoll/source/View/EndRollTheEndView.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)
GFL_NAMESPACE_BEGIN(System)

//  前方宣言
  class EndRollWork;

  //  Viewオブジェクト管理
  class EndRollViewObjects
  {
    GFL_FORBID_COPY_AND_ASSIGN( EndRollViewObjects );
  public:
    EndRollViewObjects();
    virtual~EndRollViewObjects();

    bool InitFunc( App::EndRoll::System::EndRollWork* pWork );
    bool EndFunc();

    void InputDisableAll();

    bool IsDrawing();

    //  TheEndView
    App::EndRoll::View::EndRollTheEndView*  GetTheEndUpperView() const { return m_pUpperTheEndView; }

  private:

    void _Clear();

    App::EndRoll::View::EndRollTheEndView*      m_pUpperTheEndView;

    u32 m_initSeq, m_endSeq;
  };

  //  Viewデータアクセサー
  class EndRollViewSystemAccessor
  {
    GFL_FORBID_COPY_AND_ASSIGN( EndRollViewSystemAccessor );

  public:

    EndRollViewSystemAccessor(
      App::EndRoll::System::EndRollViewList* pViewList,
      App::EndRoll::System::EndRollViewObjects* pViewObjects );

    virtual~EndRollViewSystemAccessor();

    App::EndRoll::System::EndRollViewList*    GetViewList() const { return m_pViewList; }
    App::EndRoll::System::EndRollViewObjects*  GetViewObjects() const { return m_pViewObjects; }

  private:
    void _Clear()
    {
      m_pViewList     = NULL;
      m_pViewObjects  = NULL;
    }

    App::EndRoll::System::EndRollViewList*     m_pViewList;
    App::EndRoll::System::EndRollViewObjects*  m_pViewObjects;
  };

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)


#endif // ENDROLL_VIEW_OBJECTS_H_INCLUDE
