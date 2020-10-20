/**
 *  GAME FREAK inc.
 *
 *  @file   FieldTraffic_define.h
 *  @brief  往来用定義
 *  @author miyachi_soichi
 *  @date   2015/06/29
 */

#if !defined(__FIELD_TRAFFIC_DEFINE_H__)
#define __FIELD_TRAFFIC_DEFINE_H__
#pragma once

//gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

// 前方宣言
class BaseCollisionScene;
GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(MoveModel)
class FieldMoveModelManager;
GFL_NAMESPACE_END(MoveModel)
GFL_NAMESPACE_BEGIN(Traffic)

enum
{
  FIELD_TRAFFIC_MAX = 10,
};
struct SetupCollision
{
    BaseCollisionScene* pCollisionSceneForTerrain;
    BaseCollisionScene* pCollisionSceneForStaticModel;  /// 
    BaseCollisionScene* pCollisionSceneForTrafficModel; /// 往来
    BaseCollisionScene* pCollisionSceneForEventPos;     /// 上に乗ったら反応
    BaseCollisionScene* pCollisionSceneForEventTalk;    /// 話しかけで反応
    BaseCollisionScene* pCollisionSceneForEventPush;    /// プッシュで反応
    BaseCollisionScene* pCollisionSceneForGround;       /// 地面
}; // struct SetupCollision
struct SetupParam
{
    gfl2::heap::HeapBase              *pHeap;
    MoveModel::FieldMoveModelManager  *pMoveModelManager;
}; // struct SetupParam

// 往来パスデータ構造体
struct TrafficPathData
{
  u32 types;          // 識別子
  u32 pathOffset;     // PathCoreDataへのOffset
  f32 coolTime;       // NPC再出現までの時間
  u32 npcDataOffset;  // NPC出現テーブルへのOffset
  u32 flagwork;       // フラグワークID
  u32 flagwork_num;   // ワーク値
};

//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

// 往来パスデータヘッダー
struct TrafficPathDataHeader
{
  u32 num;
  TrafficPathData data[];
};
// パス生成に必要なもの
struct PathCoreData
{
  u16 num;              // 有効座標数
  s16 curve;           // 曲線フラグ
  s16 loop;            // ループフラグ
  s16 landing;         // 接地フラグ
  f32 position[];      // 座標(x,y,z)
};
//C4200を抑止したい場合
GFL_WARNING_WIN32_POP


// パス中継点演出データ
struct PathPointData
{
  f32 relayProgress;  // 演出挿入位置
  u32 motionID;       // 演出用アニメーション番号
  f32 rotateY;        // 演出方向
  u32 count;          // 演出回数
  f32 startFrame;     // 演出開始フレーム
};
//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止
struct PathPointDataHeader
{
  u32 num;
  PathPointData data[];
};
//C4200を抑止したい場合
GFL_WARNING_WIN32_POP

// NPC生成に必須なもの
struct NpcCoreData
{
  u32 model_id;     // 見た目
  u32 appearance;   // 出現率
  u32 walk_id;      // 移動モーションID
  u32 emote_id;     // リアクションID
  u32 script_id;    // スクリプトID
  u32 romVersion;   // ロムバージョン
  u32 weatherBit;   // 天候ビットフィールド
  u32 timezoneBit;  // 時間帯ビットフィールド
};
//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止
struct NpcCoreDataHeader
{
  u32 num;
  NpcCoreData data[];
};
//C4200を抑止したい場合
GFL_WARNING_WIN32_POP

GFL_NAMESPACE_END(Traffic)
GFL_NAMESPACE_END(Field)

#endif // !defined(__FIELD_TRAFFIC_DEFINE_H__)
