// ============================================================================
/*
 * @file QuickMatchProc.h
 * @brief クイックマッチアプリのプロッククラスです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( QUICKMATCH_PROC_H_INCLUDE )
#define QUICKMATCH_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/QuickMatch/include/QuickMatchAppParam.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)

GFL_NAMESPACE_BEGIN(System)
class QuickMatchApplicationSystem;
class QuickMatchWork;
GFL_NAMESPACE_END(System)


class QuickMatchProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( QuickMatchProc );
public:
  QuickMatchProc();
  QuickMatchProc( NetApp::QuickMatch::APP_PARAM* pAppParam );
  virtual~QuickMatchProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );
    /**
    * @brief	PROCの描画処理(下準備)
    * @param pManager   プロセス管理マネージャー
    * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
    * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
    */
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

private:

  enum
  {
    INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM,
    INIT_SEQUENCE_RESOURCE_LOADING,
  };

  NetApp::QuickMatch::APP_PARAM*                                   m_pAppParam;
  NetApp::QuickMatch::System::QuickMatchWork*                  m_pQuickMatchWork;
  NetApp::QuickMatch::System::QuickMatchApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)

#endif // QUICKMATCH_PROC_H_INCLUDE
