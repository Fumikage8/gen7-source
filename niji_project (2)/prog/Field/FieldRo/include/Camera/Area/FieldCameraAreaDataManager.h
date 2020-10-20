/**
* @brief  フィールドカメラ、カメラエリアデータ管理
* @file   FieldCameraAreaDataManager.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERAAREADATAMANAGER_H_INCLUDED__ )
#define __FIELDCAMERAAREADATAMANAGER_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Easing.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <heap/include/gfl2_Heap.h>

// field
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerBase.h"
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetBase.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaTypes.h"


GFL_NAMESPACE_BEGIN( Field );
class ZoneDataLoader;
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );

// 前方宣言
struct BIN_DATA;
struct BIN_AREA_DATA;
class AreaDataAccessor;

struct DataType
{
  // データの種類
  enum Value
  {
    FOLLOW = 0,               // 通常追従型
    FOLLOW_TRANS,             // 2つのパラメータ間を遷移する追従型
    HOLD,
    PATH,

    // @note この並びを変更する場合はバイナリデータのパック順と、SetupDataStartAddrTableのアドレス計算を変更する必要がある
    TYPE_MAX,
    INVALID = 255,
  };
};

struct PointData
{
  f32 offset[3];    /// 座標
  f32 rotation[3];  /// 回転
  f32 fovY;         /// 画角
  f32 distance;     /// 距離
  s32 defaultFlag;  /// デフォルト値を使うか
};

//
// サポート設定、バイナリフォーマット
//
struct BIN_SUPPORT_MOVE_SETTING_DATA
{
  u16 waitFrame;      // 移動開始までの待ち時間
  u16 moveFrame;      // 移動にかける時間
  u32 easeFunc; // 移動に使用するイージング関数
};

struct BIN_SUPPORT_DATA
{
  b32                 zoneDefaultFlag;
  SupportType::Value  type;

  // 共通パラメータ
  b32                         defaultParamFlag;
  f32                         maxValue;
  BIN_SUPPORT_MOVE_SETTING_DATA   outData;
  BIN_SUPPORT_MOVE_SETTING_DATA   inData;

  // 方向サポート用
  SUPPORT_DIRECTION_SETTING_DATA  directionData;
};

//
// 各タイプ別、バイナリフォーマット
//
struct Follow
{
  PointData           pointData;
  BIN_SUPPORT_DATA    supportData;
};

struct FollowTrans
{
  PointData         pointDataA;
  PointData         pointDataB;
  u32               easeFunc;
  BIN_SUPPORT_DATA  supportData;
};

struct Hold
{
  s32 playerFlag;
  f32 offset[3];
  f32 target[3];
  f32 position[3];
  f32 fovY;
  f32 bank;     
};

struct Dummy
{
  f32 dummy;
};

struct CommonData
{
  u32 type;
  u32 interpolationFrame;
  s32 priority;  

  s32 effectiveWork;      // 参照ワーク
  u32 effectiveValue:16;  // 参照ワークがこの値の際にデータが有効化する
  u32 padding:16;
};


struct AreaData
{
  // ランタイム追加部
  b32        isDefault;  // ゾーンデフォルト設定が適用されているならture
  b32        isReplace;  // リプレイスが適用されているならtrue

  // 共通データ部
  CommonData commonData;

  // Type別データ部
  union
  {
    Follow follow;
    FollowTrans followTrans;
    Hold hold;
  };
};



/**
* @brief フィールドカメラの操作、カメラエリア用
*/
class AreaDataManager
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( AreaDataManager );

public:

  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    //const ZoneDataLoader* m_pZoneData; 
    ZoneDataLoader* m_pZoneData; 
    void* m_pZoneDefaultData;
    void* m_pZoneDefaultSupportData;
    void* m_pAreaData;
    gfl2::heap::HeapBase* m_pHeap;
  };



  /**
  * @brief コンストラクタ
  *
  * @param rInitDesc  初期化設定
  *
  * @return 無し
  */ 
  AreaDataManager( const InitializeDescription& rInitDesc );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~AreaDataManager( void );

  /**
  * @brief エリアのデフォルト設定を取得(ゾーンのデフォルト設定が不正だった際のケア用)
  *
  * @param  pOut  格納先
  *
  * @return なし
  */
  void GetDefaultData( AreaData* pOut ) const;

  /**
  * @brief デフォルト設定をゾーンIDから取得
  *
  * @param  zoneId    ゾーンID
  * @param  pOut      格納先
  *
  * @return 成否( ゾーンID範囲外の場合、失敗 )
  */
  b32 GetDefaultDataByZoneId( u32 zoneId, AreaData* pOut ) const;

  /**
  * @brief 指定エリアデータ番号のパラメータを取得
  * @note  前回のエリアデータ番号によってパラメータが変わる事がある(カメラエリア継承機能)
  *
  * @param  pOut            格納先
  * @param  zoneId          ゾーンID
  * @param  areaDataNo      エリアデータ番号
  * @param  prevAreaDataNo  前回のエリアデータ番号( 初めてなら無効値を指定 )
  *
  * @return 成否( データ番号が無効なら失敗 )
  */
  b32 GetData( AreaData* pOut, u32 zoneId, u32 areaDataNo, u32 prevAreaDataNo = AreaDataNo::INVALID ) const;

  /**
  * @brief 指定エリアデータ番号のプライオリティを取得
  * @note  エリアデータ番号が無効な場合は0が返る
  * @note  カメラエリア継承機能でプライオリティが変わる事はありません
  *
  * @param  areaDataNo      エリアデータ番号
  *
  * @return プライオリティ( 0 〜 大きいほど優先 )
  */
  s32 GetPriority( u32 areaDataNo ) const;

  /**
  * @brief 指定エリアデータ番号が有効か取得
  * @note  AreaDataNo::DEFAULTも無効扱いになる
  *
  * @param  areaDataNo      エリアデータ番号
  *
  * @return true 有効 false 無効
  */
  b32 IsAreaDataNoValid( u32 areaDataNo ) const;

#if PM_DEBUG
  //
  // 開発中のデータ作成サポート関連機能
  //

  /**
  * @brief 指定エリアデータ番号のパラメータAがユーザー編集されたものか取得
  * @note  エリアデータ番号が無効な場合はfalseが返る
  *
  * @param  areaDataNo      エリアデータ番号
  *
  * @return true ユーザー編集値 false エリアデフォルト値
  */
  b32 IsParamAUserSet( u32 areaDataNo ) const;

  /**
  * @brief 指定エリアデータ番号のパラメータBがユーザー編集されたものか取得
  * @note  エリアデータ番号が無効な場合はfalseが返る
  *
  * @param  areaDataNo      エリアデータ番号
  *
  * @return true ユーザー編集値 false エリアデフォルト値
  */
  b32 IsParamBUserSet( u32 areaDataNo ) const;

#endif // #if PM_DEBUG


private:

  /**
  * @brief デフォルト設定をデータ番号から取得
  *
  * @param  dataIndex データ番号
  * @param  pOut      格納先
  *
  * @return 成否( データ番号が範囲外の場合、失敗 )
  */
  b32 GetDefaultDataByIndex( u32 dataIndex, AreaData* pOut ) const;

  /**
  * @brief デフォルト設定をデータ番号から取得(カメラサポート)
  *
  * @param  dataIndex データ番号
  * @param  pOut      格納先
  *
  * @return 成否( データ番号が範囲外の場合、失敗 )
  */
  b32 GetDefaultSupportDataByIndex( u32 dataIndex, BIN_SUPPORT_DATA* pOut ) const;

  //
  // 内部制御
  //
  AreaDataAccessor*                 m_pAreaDataAccessor;
  AreaDataAccessor*                 m_pZoneDefaultDataAccessor;
  AreaDataAccessor*                 m_pZoneDefaultSupportDataAccessor;

  //
  // 外部オブジェクト
  //
  ZoneDataLoader*                   m_pZoneData; 
  // const ZoneDataLoader*             m_pZoneData; 
};  // class AreaDataManager


GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERAAREADATAMANAGER_H_INCLUDED__

