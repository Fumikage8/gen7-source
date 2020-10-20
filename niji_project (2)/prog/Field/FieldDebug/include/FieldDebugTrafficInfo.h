//======================================================================
/**
 * @file FieldDebugTrafficInfo.h
 * @date 2016/03/07 12:59:52
 * @author miyachi_soichi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_DEBUG_TRAFFICINFO_H_INCLUDED__
#define __FIELD_DEBUG_TRAFFICINFO_H_INCLUDED__
#pragma once
#if PM_DEBUG
// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Math.h>
#include <gfx/include/gfl2_DrawUtil.h>

// 前方宣言
GFL_NAMESPACE_BEGIN(Field)
class TrafficManager;
GFL_NAMESPACE_END(Field)

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Traffic)

class DebugInfo
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugInfo);

public:
  /**
   *  @brief  初期化パラメータ
   */
  struct Desc
  {
    gfl2::heap::HeapBase  *pHeap;
    Field::TrafficManager *pTrafficManager;
    
  };
  
public:
  /**
   *  @brief  コンストラクタ
   */
  DebugInfo( const Desc &desc );
  
  /**
   *  @brief  デストラクタ
   */
  ~DebugInfo( void );
  
  /**
   *  @brief  描画
   */
  void Draw( void );
  
  //! 描画切替
  void SetEnableMasterDraw( bool flag ){ m_bMasterDraw = flag; }
  
  //! 描画取得
  bool IsEnableMasterDraw( void ){ return m_bMasterDraw; }
  
private:
  /**
   *  @brief  パス描画
   */
  void drawPathAll( void );
  void drawPathSingle( u32 idx );
  
private:
  Field::TrafficManager             *m_pTrafficManager;   //!< 往来管理
  gfl2::gfx::DrawUtil::VertexFormat *m_pVertex;           //!< 描画用頂点
  u32                               m_VertexNum;          //!< 頂点数
  bool                              m_bMasterDraw;        //!< 描画マスター管理
  
}; // class DebugInfo

GFL_NAMESPACE_END(Traffic)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG
#endif // __FIELD_DEBUG_TRAFFICINFO_H_INCLUDED__
