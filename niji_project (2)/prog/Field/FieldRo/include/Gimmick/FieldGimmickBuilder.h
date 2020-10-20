/**
 *  GAME FREAK inc.
 *
 *  @file   FieldGimmickBuilder.h
 *  @brief  フィールドのシーケンスに割り込み独自処理を行うクラス
 *  @author miyachi_soichi
 *  @date   2015.05.28
 */

#if !defined(__FIELD_GIMMICK_BUILDER_H__)
#define __FIELD_GIMMICK_BUILDER_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>

// Field
#include "Field/FieldRo/include/Gimmick/FieldGimmick.h"

GFL_NAMESPACE_BEGIN( Field );
// 前方宣言
class Fieldmap;
class FieldGimmick;
class PlacementDataManager;

class FieldGimmickBuilder
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldGimmickBuilder );
private:
  // 生成禁止としておく
  FieldGimmickBuilder( void ){}
  ~FieldGimmickBuilder( void ){}

public:
  /**
   *  @brief  FieldGimmickを生成する
   *  @param  pHeap       インスタンス生成に使用するヒープ
   *  @param  pFileMgr    ファイルマネージャ
   *  @param  pFieldmap   フィールドマップ
   *  @param  nGimmickID  ID
   *  @param  index       モジュールをロードするスロット番号(enum Field::GimmickAreaIndex)
   *  @param  pPlacementDataManager 配置マネージャー
   */
  static FieldGimmick *CreateGimmick( gfl2::heap::HeapBase *pHeap, gfl2::fs::AsyncFileManager *pFileMgr, Fieldmap *pFieldmap, u32 nGimmickID, u32 index, u32 zoneId, PlacementDataManager* pPlacementDataManager );

}; // class FieldGimmickBuilder

GFL_NAMESPACE_END( Field );

#endif // !defined(__FIELD_GIMMICK_BUILDER_H__)
