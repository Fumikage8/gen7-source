// ============================================================================
/*
 * @file NBRFrameBase.h
 * @brief 通信対戦受付アプリ　フレームベース
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_FRAME_BASE_H_INCLUDE )
#define NBR_FRAME_BASE_H_INCLUDE
#pragma once

#include <util/include/gfl2_std_string.h>
#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerViewMessageMenu.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerViewMessageMenuListener.h"

// タイムアウト計測
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
class NBRNet;

GFL_NAMESPACE_BEGIN(View)
class NBRParentView;
GFL_NAMESPACE_END(View)

GFL_NAMESPACE_BEGIN(Frame)


class NBRFrameBase :
  public NetAppLib::System::NetAppFrameBase,
  public View::NBRLowerViewMessageMenuListener
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRFrameBase );

private:
  // デフォルトタイムアウト値
  static const u32 DEFAULT_TIMEOUT = NetAppLib::Util::NetAppTimeout::DEFAULT_TIMEOUT_MILLI_SECOND;

public:
  // コンストラクタ
  NBRFrameBase( ApplicationSystem::NBRWork* pNBRWork, View::NBRParentView* pParentView, NBRNet* pNet );
  // デストラクタ
  virtual~NBRFrameBase();

  /**
   * @brief タイムアウト計測開始
   * @param[in] timeout  タイムアウト値（ms)
   */
  void TimeOutStart( const u32 timeout = DEFAULT_TIMEOUT );

  /**
   * @brief タイムアウトチェック
   * @retun タイムアウトならtrue
   */
  bool IsTimeOut();

  /**
   * @brief シンプル版上画面の表示
   */
  void SetupUpperViewPlayerInfoSimple();

  /**
   * @brief 通常版上画面の表示
   */
  void SetupUpperViewPlayerInfoDefault();

  /**
   * @brief VS上画面の表示
   */
  void SetupUpperViewVs();

protected:
  ApplicationSystem::NBRWork*               m_pNBRWork;       //! NBRワーク
  View::NBRParentView*                      m_pParentView;    //! 親ビュー
  NBRNet*                                   m_pNet;           //! 通信管理クラス
  gfl2::heap::HeapBase*                     m_pHeap;          //! ヒープ
  NetAppLib::Util::NetAppTimeout            m_timeout;        //! タイムアウトクラス
  View::NBRLowerViewMessageMenu*            m_pMesageMenuView;//! メッセージメニュー
};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_FRAME_BASE_H_INCLUDE
