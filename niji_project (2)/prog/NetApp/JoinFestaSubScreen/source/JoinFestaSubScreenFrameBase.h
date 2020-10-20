// ============================================================================
/*
 * @file JoinFestaSubScreenFrameBase.h
 * @brief ジョインフェスタ下画面のフレームベース
 * @date 2015.09.01
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_FRAME_BASE_H_INCLUDE )
#define JOINFESTASUBSCREEN_FRAME_BASE_H_INCLUDE
#pragma once

#include <util/include/gfl2_std_string.h>
#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/JoinFestaSubScreen\source/ApplicationSystem/JoinFestaSubScreenWork.h"

// ジョインフェスタ下画面定義
#include "NetApp/JoinFestaSubScreen/include/JoinFestaSubScreenDefine.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)

GFL_NAMESPACE_BEGIN(View)
class JoinFestaSubScreenParentView;
GFL_NAMESPACE_END(View)

GFL_NAMESPACE_BEGIN(Frame)


//! ジョインフェスタ下画面フレームベース
class JoinFestaSubScreenFrameBase :
  public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSubScreenFrameBase );
public:
  JoinFestaSubScreenFrameBase(
    ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork,
    View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView );
  virtual~JoinFestaSubScreenFrameBase();

public:

  /**
   *  @brief  受信パケットのイベントチェック更新
   */
  void UpdatePacketEventCheck();

private:
  /**
   *  @brief  受信パケットのイベント取得
   *  @return PacketEventType
   */
  Define::PacketEventType GetPacketEventType();

protected:
  ApplicationSystem::JoinFestaSubScreenWork*  m_pJoinFestaSubScreenWork;
  View::JoinFestaSubScreenParentView*         m_pParentView;
  gfl2::heap::HeapBase*                       m_pHeap;          //! ヒープ
};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_FRAME_BASE_H_INCLUDE
