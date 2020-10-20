// ============================================================================
/*
 * @file BattleRoyalResultBattleRoyalResultViewObjects.h
 * @brief バトルロイヤル勝敗アプリのViewクラスを持っているクラスです。
 * @date 2015.10.26
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( BATTLE_ROYAL_RESULT_VIEW_OBJECTS_H_INCLUDE )
#define BATTLE_ROYAL_RESULT_VIEW_OBJECTS_H_INCLUDE
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

//  Viewのインクルード
#include "App/BattleRoyalResult/source/System/BattleRoyalResultViewList.h"
#include "App/BattleRoyalResult/source/View/BattleRoyalResultLowerBGView.h"
#include "App/BattleRoyalResult/source/View/BattleRoyalResultUpperRankingView.h"

//  NetStaticのインクルード
#include "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(System)

//  前方宣言
  class BattleRoyalResultWork;

  //  Viewオブジェクト管理
  class BattleRoyalResultViewObjects
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleRoyalResultViewObjects );
  public:
    BattleRoyalResultViewObjects();
    virtual~BattleRoyalResultViewObjects();

    bool InitFunc( App::BattleRoyalResult::System::BattleRoyalResultWork* pWork );
    bool EndFunc();

    void InputDisableAll();

    bool IsDrawing();

    App::BattleRoyalResult::View::BattleRoyalResultLowerBGView*         GetLowerBGView()       { return m_pLowerBGView; }
    App::BattleRoyalResult::View::BattleRoyalResultUpperRankingView*    GetUpperRankingView()  { return m_pUpperRankingView; }

  private:

    void _Clear();

    App::BattleRoyalResult::View::BattleRoyalResultUpperRankingView*  m_pUpperRankingView;
    App::BattleRoyalResult::View::BattleRoyalResultLowerBGView*       m_pLowerBGView;

    u32 m_initSeq, m_endSeq;
  };

  //  Viewデータアクセサー
  class BattleRoyalResultViewSystemAccessor
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleRoyalResultViewSystemAccessor );

  public:

    BattleRoyalResultViewSystemAccessor(
      App::BattleRoyalResult::System::BattleRoyalResultViewList* pViewList,
      App::BattleRoyalResult::System::BattleRoyalResultViewObjects* pViewObjects );

    virtual~BattleRoyalResultViewSystemAccessor();

    App::BattleRoyalResult::System::BattleRoyalResultViewList*    GetViewList() { return m_pViewList; }
    App::BattleRoyalResult::System::BattleRoyalResultViewObjects*  GetViewObjects() { return m_pViewObjects; }

  private:
    void _Clear()
    {
      m_pViewList     = NULL;
      m_pViewObjects  = NULL;
    }

    App::BattleRoyalResult::System::BattleRoyalResultViewList*     m_pViewList;
    App::BattleRoyalResult::System::BattleRoyalResultViewObjects*  m_pViewObjects;
  };

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)


#endif // BATTLE_ROYAL_RESULT_VIEW_OBJECTS_H_INCLUDE
