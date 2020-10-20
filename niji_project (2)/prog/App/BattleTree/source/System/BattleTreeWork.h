// ============================================================================
/*
 * @file BattleTreeWork.h
 * @brief バトルツリーアプリのワーククラスです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( BATTLE_TREE_WORK_H_INCLUDE )
#define BATTLE_TREE_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h"

//  アプリのパラメータ
#include "App/BattleTree/include/BattleTreeAppParam.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(System)

  //  前方宣言
  class ViewSystemAccessor;

  class BattleTreeWork : public NetAppLib::System::ApplicationWorkBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleTreeWork );

  public:

    //  トレーナーアイコンのロード個数( リストに表示する個数(12)＋確認画面で表示(1) )
    static const u32  TRAINER_LOAD_ICON_MAX          = 12 + 1;

  private:

    static const u32  APP_HEAP_SIZE             = 1024 * 1024 * 4;    //  4MB
    static const u32  APP_DEVICE_HEAP_SIZE      = 1024 * 1024 * 15;   // 15MB


  public:
    BattleTreeWork( App::BattleTree::APP_PARAM* pAppParam );
    virtual~BattleTreeWork();

    //  Viewシステムの参照の設定・取得
    void SetViewSystemAccessor( ViewSystemAccessor* pAccessor )
    {
      GFL_ASSERT( pAccessor );

      m_pViewSystemAccessor = pAccessor;
    }
    ViewSystemAccessor* GetViewSystemAccessor() { return m_pViewSystemAccessor; }

    //  APPParamを取得
    APP_PARAM*  GetAppParam() const { return m_pAppParam; }

    //  トレーナーアイコン貼り付けUtil設定/取得
    void  SetTrainerIconUtilty( NetAppLib::Util::NetAppTrainerIconUtility* pUtiluty ) { m_pNetAppTrainerIconUtility  = pUtiluty; }
    NetAppLib::Util::NetAppTrainerIconUtility*  GetTrainerIconUtilty() const { return m_pNetAppTrainerIconUtility; }

    //  トレーナーIDの割り当て
    u32 GetAllocTrainerLoadID();

  private:
    void _Clear()
    {
      m_pAppParam           = NULL;
      m_pViewSystemAccessor = NULL;
      m_pNetAppTrainerIconUtility = NULL;

      m_allockTrainerLoadID     = 0;
    }

    u32                 m_allockTrainerLoadID;
    APP_PARAM*          m_pAppParam;
    ViewSystemAccessor* m_pViewSystemAccessor;

    NetAppLib::Util::NetAppTrainerIconUtility*            m_pNetAppTrainerIconUtility;
  };

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)

#endif // BattleTreeWORK_H_INCLUDE
