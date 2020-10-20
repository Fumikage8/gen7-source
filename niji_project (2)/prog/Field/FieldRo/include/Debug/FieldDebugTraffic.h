/**
 *  GAME FREAK inc.
 *
 *  @file   FieldDebugTraffic.h
 *  @brief  往来デバッグメニュー
 *  @author miyachi_soichi
 *  @date   2015/06/29
 */

#if PM_DEBUG

#if !defined(__FIELD_DEBUG_TRAFFIC_H__)
#define __FIELD_DEBUG_TRAFFIC_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Vector3.h>
#include <heap/include/gfl2_Heap.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>

GFL_NAMESPACE_BEGIN(Field)
// 前方宣言
class TrafficManager;
GFL_NAMESPACE_BEGIN(Debug)

class DebugTraffic
{
public:
  /**
   *  @brief  コンストラクタ
   */
  DebugTraffic( void );

  /**
   *  @brief  デストラクタ
   */
  ~DebugTraffic( void );

  /**
   *  @brief  初期化
   */
  void Initialize( gfl2::heap::HeapBase *pHeap, TrafficManager *pTrafficManager, gfl2::str::MsgData* pMsgData );

  /**
   *  @brief  破棄
   */
  void Terminate( void );

  void CreateDebugMenu( gfl2::debug::DebugWinGroup *root );
  TrafficManager *GetMgr( void ){ return m_pTrafficManager; }
  u32 GetVectorNum( void ){ return m_nVectorNum; }
  gfl2::math::Vector3 GetPosition( u32 idx ){ return m_vPos[ idx ]; }

  void SetVectorNum( u32 num ){ m_nVectorNum = num; }
  void SetPosition( u32 idx, const gfl2::math::Vector3 &pos ){ m_vPos[ idx ] = pos; }

  bool IsCurve( void ) const { return m_bCurve; }
  void SetCurve( bool flag ) { m_bCurve = flag; }

  gfl2::heap::HeapBase *GetHeap( void ){ return m_pHeap; }

  gfl2::str::MsgData* GetMessageData( void ) { return m_pMsgData; }

private:
  gfl2::heap::HeapBase *m_pHeap;
  TrafficManager *m_pTrafficManager;

  gfl2::math::Vector3 m_vPos[ 10 ];
  u32 m_nVectorNum;
  bool m_bCurve;

  gfl2::str::MsgData* m_pMsgData;

}; // class DebugTraffic

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // !defined(__FIELD_DEBUG_TRAFFIC_H__)

#endif // PM_DEBUG
