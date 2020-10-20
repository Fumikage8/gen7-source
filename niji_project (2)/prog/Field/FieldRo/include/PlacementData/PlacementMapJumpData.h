//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		PlacementMapJumpData.h
 *	@brief  マップジャンプデータ
 *	@author	tomoya takahashi
 *	@date		2015.05.25
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __PLACEMENT_MAPJUMP_DATA_H__ )
#define __PLACEMENT_MAPJUMP_DATA_H__
#pragma once
// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(Field);


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

/**
 *  @brief  マップジャンプのデータ
 */
class PlacementMapJumpData
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( PlacementMapJumpData );


public:

  static const u32 SE_LABEL_NULL = 0xFFFFFFFF;

  struct MapJumpParam
  {
    u32 kick_type;      // 起動タイプ
    u32 is_pos_center;  // イベントの中心にジャンプするか
    u32 se_label;       // SEラベル
  };
 
public:

  /**
   *  @brief  コンストラクタ
   */
  PlacementMapJumpData()
  {
  }

  /**
   *  @brief  デストラクタ
   */
  virtual ~PlacementMapJumpData()
  {
    this->Terminate();
  }

  /**
   *  @brief  初期化
   */
  void Initialize( const void * cpMapJumpParam, u32 mapJumpParamSize );
  

  /**
   *  @brief  破棄
   */
  void Terminate( void );
  

  /**
   *  @brief  マップジャンプタイプの総数を取得
   */
  inline u32 GetMapJumpTypeNum() const { return m_MapJumpParamNum; }

  /**
   *  @brief  マップジャンプタイプごとのパラメータを取得
   */
  const MapJumpParam* GetMapJumpParam( u32 idx ) const;

private:

  const MapJumpParam* m_cpMapJumpParam;
  u32                 m_MapJumpParamNum;
};

GFL_NAMESPACE_END(Field);


#endif	// __PLACEMENT_MAPJUMP_DATA_H__

