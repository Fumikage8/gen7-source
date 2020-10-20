//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldWorld.h
 *	@brief  ワールド管理クラス
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_WORLD_H__ )
#define __FIELD_WORLD_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <heap/include/gfl2_heap.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <math/include/gfl2_math.h>
#include <math/include/gfl2_Vector.h>
#include <util/include/gfl2_List.h>



//niji
#include "Field/FieldStatic/include/FieldArea.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/FieldWorldDefine.h"

GFL_NAMESPACE_BEGIN(Field);

struct ZoneCollisionData; // ゾーンコリジョンデータ

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
class World
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( World );
public:

  // エリアメモリモード
  enum AreaMemMode
  {
    AREA_MEM_NORMAL,  /// 通常
    AREA_MEM_LOW,     /// バトルモデルを出せるメモリ量
    AREA_MEM_MIDDLE,  /// ショップUIを出せるメモリ量
  };

  // 地形の状態識別子
  struct TerrainMode
  {
    enum Value
    {
      FIX = 0,
      PLAN,
      PLAN_ONLY_WIN32,
      MAX
    };
  };

  // ゾーン情報
  struct ZoneInfo
  {
    u16 m_ZoneID; // ZoneID
    u16 m_AreaID; // ゾーンが所属するAreaID
  };

  // ワールド基礎情報
  struct WorldInfo
  {
    u16   m_Width;      // 幅
    u16   m_Height;     // 高さ
    u32   m_BlockSize;  // ブロックサイズ
    u8    m_TerrainMode;   // 地形リソースの状態、0プランナー設計中マップか？
    u8    m_BlockControl;  // 地形ブロックの制御方式
    u8    pad[2];
  };
  
  struct AreaInfo
  {
    u16 m_AreaID;     ///< エリアID
    u16 m_AreaMemType;  ///< バトルポケモンの表示にAreaHeapを使用するか。(AreaMemMode)
  };

public:
  ///! コンストラクタ・デストラクタ
  World( gfl2::heap::HEAPID heapID );
  World( gfl2::heap::HeapBase* heap );
  ~World();

  /**
   *  @brief  初期化
   *  @param  worldId       ワールドID
   *  @param  p_fileManager ファイルマネージャー
   */
  void Initialize( WorldID worldId, gfl2::fs::AsyncFileManager* p_fileManager );

  /**
   *  @brief  初期化処理
   */
  bool InitializeWait();

  /**
   *  @brief  ワールド情報の破棄
   */
  void Terminate( void );

  /**
   *  @brief 現在のワールドIDを取得
   */
  WorldID GetWorldID() const ;

  /**
   *  @brief  ワールド基礎情報を取得
   */
  const World::WorldInfo& GetWorldInfo( void ) const;

  /**
   *  @brief  ゾーン数を取得する
   */
  u32 GetZoneNum() const;

  /**
   *  @brief  ゾーン情報を取得
   *  @param  idx インデックス
   */
  const World::ZoneInfo& GetZoneInfo( u32 idx ) const;
  const World::ZoneInfo& GetIDZoneInfo( ZoneID zoneID ) const;

  /**
   * @brief ゾーンIDのZoneInfoテーブルインデックスを取得
   */
  u32 GetZoneIDIndex( ZoneID zoneID ) const;
  
  /**
   *  @brief  指定したゾーンのエリア内Indexを取得
   *  @param  zoneID  ゾーンID
   *  @retval エリア内Index
   *
   *  ゾーン毎のデータをエリア単位でパックした場合に
   *  各ゾーンのデータにアクセスするにはエリア内Indexが必要。
   *  ただし、この関数で取得したIndexでエリアデータのBinLinkerにアクセスするには
   *  データのパック時にfield_convert\world\world\converter\GetAreaZoneList.rbが行うのと
   *  同じアルゴリズムでエリア内ゾーンリストを取得し、その順番でパックする必要がある。
  */
  u32 World::GetZoneIndexInArea( ZoneID zoneID ) const;

  
  /**
   *  @brief  エリア数を取得
   */
  u32 GetAreaNum() const;

  /**
   *  @brief  エリアIDリストの取得
   */
  const AreaInfo* GetAreaArray() const ;

  /**
   *  @brief エリアIDのメモリ設定を取得
   */
  b32 IsAreaDisplayBattlePokemonUsingAreaHeap( AreaID areaID ) const;

  /**
   *  @brief エリアIDのメモリ設定を取得
   */
  b32 IsAreaDisplayShopUIUsingAreaHeap( AreaID areaID ) const;

  /**
   *  @brief  ゾーンヒットチェック
   */
  const World::ZoneInfo* IsHitZoneInfo( const gfl2::math::Vector3& pos, b32 is_asseert_check = true ) const;

  /**
  *  @brief  マップジャンプできる？
  *
  *  @return  true できる false 出来ない
  */
  b32 CanMapJump( void ) const;


private:
  void SetupWorldInfo();
  void SetupZoneInfoList();
  void SetupAreaList();

  void CreateZoneAreaList();
  void TerminateZoneAreaList();
  
  const f32 * GetZoneAreaVertexArray( void ) const;
  u32 GetZoneAreaZoneIndexArraySize( u32 zoneIndex ) const;
  u32 * GetZoneAreaZoneIndexArray( u32 zoneIndex );



private:
  static const u32 ZONEAREA_LOCAL_HEAP_SIZE = 0x4000;
  enum
  {
    BIN_IDX_WORLD = 0,
    BIN_IDX_ZONEAREA,
    BIN_IDX_NUM
  };

//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  // ワールドデータヘッダー
  struct WorldDataHeader
  {
    u32 worldDataOffset;
    u32 areaListOffset;
    u32 zoneListOffset;
    u32 endOffset;
    char data[];
  };

  // ゾーンエリアヘッダー
  // GetZoneInfoで帰ってくるゾーンの順番にデータが格納されています。
  struct ZoneAreaHeader
  {
    u32 zoneNum;                  // ゾーン数
    u32 vertexNum;                // 頂点バッファ要素数
    u32 vertexArrayOffset;        // 頂点バッファ開始オフセット
    u32 fileEndOffset;            // ファイルの終端までのオフセット（最終ゾーンのindexArray要素数算出用）
    u32 zoneIndexArrayOffset[];   // 各ゾーンのIndexArray先頭オフセット
    // 以降
    // f32*3 vertexArray[vertexNum] // 頂点バッファの実態
    // u32   zoneIndexArray[]       // ゾーンのindexArray実態 全てtriangle
  };
//C4200を抑止したい場合
GFL_WARNING_WIN32_POP

  WorldID m_WorldId;
  gfl2::heap::HeapBase* m_pHeap;
  gfl2::fs::AsyncFileManager* m_pFileManager;

  void * m_pBuffer;
  gfl2::fs::BinLinkerAccessor m_Accessor;

  WorldDataHeader* m_pWorldData;
  WorldInfo* m_pWorldInfo;
  u32       m_ZoneInfoNum;
  ZoneInfo* m_pZoneInfoList;
  AreaInfo* m_pAreaList;
  u32       m_AreaNum;


  ZoneAreaHeader * m_pZoneAreaData;
  gfl2::heap::HeapBase * m_pZoneAreaLocalHeap;                      // ゾーンエリア生成用ローカルヒープ
  gfl2::util::List<ZoneCollisionData*>* m_pZoneCollisionDataList;   // ゾーン数分のコリジョンリスト 要素数=m_ZoneInfoNum
  gfl2::math::Vector* m_pZoneAreaVertexArray;                       // 頂点バッファ
};

GFL_NAMESPACE_END(Field);

#endif	// __FIELD_WORLD_H__

