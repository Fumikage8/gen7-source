#if PM_DEBUG
//==============================================================================
/**
 * @file	NetDebug.h
 * @brief	通信デバッグ機能
 * @author	ichiraku_katsuhiko
 * @date	2016/04/29 15:59:28
 */
// =============================================================================
#ifndef __NET_DEBUG_H__
#define __NET_DEBUG_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
// DebugWin
#include <Debug/include/gfl2_DebugWinSystem.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>

// 前方宣言
namespace NetAppLib {
  namespace JoinFesta {
    class JoinFestaPersonalData;
  }
}


GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Debug)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#if defined(GF_PLATFORM_CTR)

// CTR版のみ動作する
class NetDebug
{
  GFL_FORBID_COPY_AND_ASSIGN(NetDebug);
public:

public:
  // コンストラクタ
  NetDebug( gfl2::debug::DebugWinGroup* pParentGroup );
  // デストラクタ
  virtual ~NetDebug();

private:

  // ゲームシンクIDリセット
  static void DebugFunc_ResetGameSyncID( void* userWork, gfl2::debug::DebugWinItem* item );
  // 通信ログ表示
  static void DebugFunc_DispNetLogChange( void* userWork );
  // 通信バージョン変更
  static void DebugFunc_NetVersionChange( void* userWork );

private:
  gfl2::ui::DeviceManager*    m_debugDeviceManager;
  gfl2::ui::Button*           m_pDebugButton;

  gfl2::debug::DebugWinGroup* m_pDataViewGroup;
  gfl2::debug::DebugWinGroup* m_pFlagGroup;

  u8    m_isDispNetLog;

};

#else

// Win版はダミークラス
class NetDebug
{
  GFL_FORBID_COPY_AND_ASSIGN(NetDebug);
public:
  // コンストラクタ
  NetDebug( gfl2::debug::DebugWinGroup* pParentGroup ){}
  // デストラクタ
  virtual ~NetDebug(){}
};

#endif

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(NetLib)

#endif // __NET_DEBUG_H__

#endif // PM_DEBUG
