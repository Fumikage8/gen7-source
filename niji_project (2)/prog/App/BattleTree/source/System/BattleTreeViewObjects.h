// ============================================================================
/*
 * @file BattleTreeViewObjects.h
 * @brief バトルツリーアプリのViewクラスを持っているクラスです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( BATTLE_TREE_VIEWOBJECTS_H_INCLUDE )
#define BATTLE_TREE_VIEWOBJECTS_H_INCLUDE
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

//  Viewのインクルード
#include "App/BattleTree/source/View/BattleTreeLowerBGView.h"
#include "App/BattleTree/source/View/BattleTreeLowerSelectConfirmView.h"
#include "App/BattleTree/source/View/BattleTreeLowerNPCListView.h"
#include "App/BattleTree/source/View/BattleTreeUpperBGView.h"

//  NetStaticのインクルード
#include "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(System)

//  前方宣言
  class BattleTreeWork;

  class ViewObjects
  {
    GFL_FORBID_COPY_AND_ASSIGN( ViewObjects );
  public:
    ViewObjects();
    virtual~ViewObjects();

    bool InitFunc( App::BattleTree::System::BattleTreeWork* pWork );
    bool EndFunc();

    void InputDisableAll();

    bool IsDrawing();

    App::BattleTree::View::UpperBGView*             GetUpperBGView()              { return m_pUpperBGView; }
    App::BattleTree::View::LowerBGView*             GetLowerBGView()              { return m_pLowerBGView; }
    App::BattleTree::View::LowerSelectConfirmView*  GetLowerSelectConfirmView()   { return m_pLowerSelectConfirmView; }
    App::BattleTree::View::LowerNPCTrainerListView* GetLowerNPCTrainerListView()  { return m_pLowerNPCTrainerListView; }

  private:

    void _Clear();

    App::BattleTree::View::UpperBGView*             m_pUpperBGView;
    App::BattleTree::View::LowerBGView*             m_pLowerBGView;
    App::BattleTree::View::LowerSelectConfirmView*  m_pLowerSelectConfirmView;
    App::BattleTree::View::LowerNPCTrainerListView* m_pLowerNPCTrainerListView;

    u32 m_initSeq, m_endSeq;
  };

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)


#endif // BATTLE_TREE_VIEWOBJECTS_H_INCLUDE
