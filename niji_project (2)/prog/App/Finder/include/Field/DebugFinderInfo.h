//======================================================================
/**
 * @file DebugFinderInfo.h
 * @date 2016/02/23 15:52:25
 * @author miyachi_soichi
 * @brief デバッグ用：ファインダー情報表示
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __DEBUG_FINDER_INFO_H_INCLUDED__
#define __DEBUG_FINDER_INFO_H_INCLUDED__
#pragma once
#if PM_DEBUG
// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Math.h>
#include <gfx/include/gfl2_DrawUtil.h>
// param
#include "Field/FieldRo/include/TrialModel/FieldFinderModel_define.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Finder)

class DebugInfo
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugInfo);

public:
  /**
   *  @brief  スコア用入れ物
   */
  struct DebugPokeScore
  {
    f32 basepoint;
    f32 motion;
    f32 dir;
    f32 screen;
    f32 out;
  };
  /**
   *  @brief  コメント用パラメータ入れ物
   */
  struct DebugCommentParam
  {
    u32 spot_type;
    u32 time_zone;
    u32 weather;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  DebugInfo( gfl2::heap::HeapBase *pHeap );

  /**
   *  @brief  デストラクタ
   */
  ~DebugInfo( void );

  /**
   *  @brief  描画
   */
  void Draw( void );

  //! 矩形の設定
  void SetPokeRect( const gfl2::math::Vector2 &min, const gfl2::math::Vector2 max )
  {
    m_vPokeRect_min = min;
    m_vPokeRect_max = max;
  }
  //! 矩形の色
  void SetPokeRectColor( f32 r, f32 g, f32 b )
  {
    m_vPokeRect_color.Set( r, g, b, 0.4f );
  }
  //! 巡回の設定
  void SetWalkerDataA( Field::TrialModel::FinderPathData *pData ){ m_pWalkerDataA = pData; }
  void SetWalkerDataB( Field::TrialModel::FinderPathData *pData ){ m_pWalkerDataB = pData; }
  //! 待機位置の設定
  void SetStatueData( Field::TrialModel::FinderStatueData *pData ){ m_pStatueData = pData; }
  //! 一方通行の設定
  void SetOnewayData( Field::TrialModel::FinderOnewayData *pData ){ m_pOnewayData = pData; }
  //! スコアの設定
  void SetScorePlace( f32 total, f32 rand, f32 spot )
  {
    m_fScore_Total = total;
    m_fScore_Rand = rand;
    m_fScore_Spot = spot;
  }
  void SetScorePoke( u32 idx, f32 base, f32 motion, f32 dir, f32 screen, f32 out )
  {
    if( idx < Field::TrialModel::FINDER_POKE_MODEL_MAX )
    {
      m_rScore_Poke[idx].basepoint  = base;
      m_rScore_Poke[idx].motion     = motion;
      m_rScore_Poke[idx].dir        = dir;
      m_rScore_Poke[idx].screen     = screen;
      m_rScore_Poke[idx].out        = out;
    }
  }
  void SetCommentParam( u32 spot_type, u32 time_zone, u32 weather )
  {
    m_rComment_Param.spot_type = spot_type;
    m_rComment_Param.time_zone = time_zone;
    m_rComment_Param.weather   = weather;
  }

  //! 描画切替
  void SetEnablePokeRect( bool flag ){ m_EnablePokeRect = flag; }
  void SetEnableWalkerA( bool flag ){ m_EnableWalkerA = flag; }
  void SetEnableWalkerB( bool flag ){ m_EnableWalkerB = flag; }
  void SetEnableStatue( bool flag ){ m_EnableStatue = flag; }
  void SetEnableOneway( bool flag ){ m_EnableOneway = flag; }
  void SetEnableScore( bool flag ){ m_EnableScore = flag; }
  void SetEnableComment( bool flag ){ m_EnableComment = flag; }

  //! 描画取得
  bool IsEnablePokeRect( void ){ return m_EnablePokeRect; }
  bool IsEnableWalkerA( void ){ return m_EnableWalkerA; }
  bool IsEnableWalkerB( void ){ return m_EnableWalkerB; }
  bool IsEnableStatue( void ){ return m_EnableStatue; }
  bool IsEnableOneway( void ){ return m_EnableOneway; }
  bool IsEnableScore( void ){ return m_EnableScore; }
  bool IsEnableComment( void ){ return m_EnableComment; }

private:
  //! ポケモン矩形の描画
  void drawPokeRect( void );
  //! 巡回パスの描画
  void drawWalker( Field::TrialModel::FinderPathData *pData );
  //! 待機座標の描画
  void drawStatue( void );
  //! 一方通行の描画
  void drawOneway( void );
  //! スコアの描画
  void drawScore( void );
  //! コメントParamの描画
  void drawComment( void );

  //! ラインの描画
  void drawLine( const gfl2::math::Vector3 &start, const gfl2::math::Vector3 &end, const gfl2::math::Vector4 &color );

private:
  Field::TrialModel::FinderPathData   *m_pWalkerDataA;
  Field::TrialModel::FinderPathData   *m_pWalkerDataB;
  Field::TrialModel::FinderStatueData *m_pStatueData;
  Field::TrialModel::FinderOnewayData *m_pOnewayData;
  gfl2::gfx::DrawUtil::VertexFormat   *m_pVertex;
  gfl2::math::Vector4                 m_vPokeRect_color;
  gfl2::math::Vector2                 m_vPokeRect_min;
  gfl2::math::Vector2                 m_vPokeRect_max;
  u32                                 m_VertexNum;
  f32                                 m_fScore_Total;
  f32                                 m_fScore_Rand;
  f32                                 m_fScore_Spot;
  DebugPokeScore                      m_rScore_Poke[Field::TrialModel::FINDER_POKE_MODEL_MAX];
  DebugCommentParam                   m_rComment_Param;
  bool                                m_EnablePokeRect;
  bool                                m_EnableWalkerA;
  bool                                m_EnableWalkerB;
  bool                                m_EnableStatue;
  bool                                m_EnableOneway;
  bool                                m_EnableScore;
  bool                                m_EnableComment;

}; // class DebugInfo

GFL_NAMESPACE_END(Finder)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG
#endif // __DEBUG_FINDER_INFO_H_INCLUDED__
