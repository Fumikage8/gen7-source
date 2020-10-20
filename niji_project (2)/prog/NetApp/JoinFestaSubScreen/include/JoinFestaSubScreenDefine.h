// ============================================================================
/*
 * @file JoinFestaSubScreenDefine.h
 * @brief ジョインフェスタ下画面定義
 * @date 2015.06.10
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_DEFINE_H_INCLUDE )
#define JOINFESTASUBSCREEN_DEFINE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)

/// 定義系
class Define
{
public:
  /**
  * @enum PacketEventType
  *       受信パケットのイベント通知種類
  */
  enum PacketEventType{
    PACKET_EVENT_TYPE_NONE,     //! イベント通知なし
    PACKET_EVENT_TYPE_BATTLE_S, //! バトル（シングル）招待
    PACKET_EVENT_TYPE_BATTLE_D, //! バトル（ダブル）招待
    PACKET_EVENT_TYPE_BATTLE_M, //! バトル（マルチ）招待
    PACKET_EVENT_TYPE_BATTLE_R, //! バトル（ロイヤル）招待
    PACKET_EVENT_TYPE_TRADE,    //! 交換招待

    PACKET_EVENT_TYPE_MAX
  };
};


GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_DEFINE_H_INCLUDE
