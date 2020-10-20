// ============================================================================
/*
 * @file BattleRoyalResultWork.h
 * @brief バトルロイヤルの結果アプリのワーククラスです。
 * @date 2015.10.23
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( BATTLE_ROYAL_RESULT_WORK_H_INCLUDE )
#define BATTLE_ROYAL_RESULT_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"

//  アプリパラメータ
#include "App/BattleRoyalResult/include/BattleRoyalResultAppParam.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)

  class NetAppTrainerIconUtility;

GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(System)

  //  前方宣言
  class BattleRoyalResultViewSystemAccessor;

  class BattleRoyalResultWork : public NetAppLib::System::ApplicationWorkBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleRoyalResultWork );

  private:

    static const int  APP_HEAP_SIZE             = 1024 * 1024 * 4;    //  4MB
    static const int  APP_DEVICE_HEAP_SIZE      = 1024 * 1024 * 16;   // 16MB

  public:
    BattleRoyalResultWork( App::BattleRoyalResult::APP_PARAM* pAppParam );
    virtual~BattleRoyalResultWork();

    void SetViewSystemAccessor( BattleRoyalResultViewSystemAccessor* pAccessor )
    {
      GFL_ASSERT( pAccessor );
      m_pViewSystemAcceeor  = pAccessor;
    }

    //  Viewアクセスの取得・設定
    BattleRoyalResultViewSystemAccessor*  GetViewSystemAccessor() const { return m_pViewSystemAcceeor; }

    //  アプリパラメータ取得
    App::BattleRoyalResult::APP_PARAM*  GetAppParam();

    //  トレーナーアイコン取得・設定
    NetAppLib::Util::NetAppTrainerIconUtility*  GetTrainerIconUtil() const { return m_pTrainerIconUtil; }
    void SetTrainerIconUtil( NetAppLib::Util::NetAppTrainerIconUtility* pUtil )
    {
      GFL_ASSERT( pUtil );
      m_pTrainerIconUtil  = pUtil;
    }

  private:
    void _Clear()
    {
      m_pViewSystemAcceeor  = NULL;
      m_pAppParam           = NULL;
      m_pTrainerIconUtil    = NULL;
    }
    
    BattleRoyalResultViewSystemAccessor*  m_pViewSystemAcceeor;
    App::BattleRoyalResult::APP_PARAM*    m_pAppParam;

    NetAppLib::Util::NetAppTrainerIconUtility*  m_pTrainerIconUtil;
  };

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)

#endif // BATTLE_ROYAL_RESULTWORK_H_INCLUDE
