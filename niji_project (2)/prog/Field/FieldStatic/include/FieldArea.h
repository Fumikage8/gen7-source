//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldArea.h
 *	@brief  エリアデータ管理
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_AREA_H__ )
#define __FIELD_AREA_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <heap/include/gfl2_heap.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>

// field
#include "Field/FieldRo/include/Terrain/FieldTerrainTypes.h" // for MemoryMode

GFL_NAMESPACE_BEGIN(System);
  class nijiAllocator;
GFL_NAMESPACE_END(System);

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
//! エリアID型
typedef u16 AreaID; 
const AreaID AREAID_ERROR = 0xFFFF;

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class Area
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( Area );
public:
  //-----------------------------------------------------------------------------
  //  定義
  //    下記に追加する場合は、
  //      ①コンバート環境の更新　　R:\home\niji\convert\field_convert\world\area\omake\OMakefile
  //      ②エリア別のメモリ利用制限基準値の追加               AREA_DATA_SIZE_RULE
  //      ③エリア種別リソースのデバッグログ出力文字列の追加   AREA_DATA_NAME
  //    が必要です。 
  //-----------------------------------------------------------------------------
  enum DataIdx
  {
    AREA_DATA_IDX_PLACEMENTDATA = 0,          //! 配置情報
    AREA_DATA_IDX_TERRAIN_RESOURCE,           //! 地形リソース
    AREA_DATA_IDX_TERRAIN_LAYOUT_RESOURCE,    //! 地形配置データリソース
    AREA_DATA_IDX_CHARACTOR_RESOURCE,         //! キャラクターリソース
    AREA_DATA_IDX_STATICMODEL_RESOURCE,       //! 配置モデルリソース
    AREA_DATA_IDX_EFFECT_RESOURCE,            //! エフェクトリソース 
    AREA_DATA_IDX_ENVIRONMENT_RESOURCE,       //! 環境データリソース
    AREA_DATA_IDX_ZONE_SCRIPT_RESOURCE,       //! ゾーンスクリプトリソース
    AREA_DATA_IDX_INIT_SCRIPT_RESOURCE,       //! 初期化スクリプトリソース
    AREA_DATA_IDX_ENCOUNT_DATA,               //! エンカウントデータ
    AREA_DATA_IDX_CAMERA_AREA_DATA,           //! カメラエリアデータ
    AREA_DATA_IDX_NUM,
  };

  // エリアリソーストータルサイズ最大
  //static const u32 AREA_DATA_SIZE_MAX = 0x1200000;  // 当初OKサイズ
  static const u32 AREA_DATA_SIZE_MAX = 0xB80000;     // 今OKサイズ
  static const u32 AREA_DATA_BATTLE_POKEMON_USING_MAX;  // バトルモデル表示にエリアヒープを使用する場合のOKサイズ
  static const u32 AREA_DATA_JOINFESTA_USING_MAX;  // ジョインフェスタ表示にエリアヒープを使用する場合のOKサイズ
  static const u32 AREA_DATA_MIDDLE_USING_MAX;  // ショップUIだけ乗せる場合のメモリ設定

	static const u32 AREA_ARC_OPEN_HEAP_SIZE = 0xC800;	// アーカイブオープンに必要なヒープサイズ
  

#if PM_DEBUG
  // エリア各種リソースの利用制限基準値
  static const u32 AREA_DATA_SIZE_RULE[AREA_DATA_IDX_NUM];

  // エリア種別リソースの名称
  static const char* AREA_DATA_NAME[AREA_DATA_IDX_NUM];
#endif

  enum Mode
  {
    MODE_NORMAL,  ///< 通常
    MODE_BATTLEMODEL,  ///< エリアヒープをポケモンモデル表示に使用
    MODE_JF,      ///< エリアヒープをJFのキャラクター表示に使用
    MODE_MIDDLE,  ///< ショップUIだけ乗せる場合のメモリ設定
    MODE_MAX,
  };

public:
  ///! コンストラクタ・デストラクタ
  Area( gfl2::heap::HeapBase * pArcOpenHeapParentHeap );
  ~Area();

  /**
   *  @biref エリア情報の読み込み開始 
   *  @param  areaID ロードエリアID
   *  @param  p_heapBase ヒープベース
   *  @param  p_fileManager ファイルマネージャ
   *  @param  mode  起動モード
   */
  void Initialize( AreaID areaID, gfl2::heap::HeapBase* p_heapBase, gfl2::heap::HeapBase * p_tmpHeap, gfl2::fs::AsyncFileManager* p_fileManager, Mode mode );
  /**
   *  @brief  初期化処理 
   *  @retval true    初期化完了
   *  @retval false   初期化途中
   */
  bool InitializeWait();

  /**
   *  @brief  エリア情報の破棄
   */
  void Terminate( void );

  /**
   *  @brief アクセサーの取得
   */
  gfl2::fs::BinLinkerAccessor* GetAccessor( DataIdx idx ); 
  
  /*
   *  @brief 自身のエリアIDを取得
   */
  AreaID GetAreaID(){ return m_AreaID; }

  /**
   *  @brief 個別リソースのデータサイズを取得
   */
  u32 GetDataSize(DataIdx idx){ return m_DataSize[idx]; }


  //
  // 静的関数群
  //

  /**
   *  @brief 地形メモリモードを取得
   */
  static Terrain::MemoryMode::Value GetTerrainMemoryMode(AreaID areaID);


#if PM_DEBUG
  // リソースサイズをチェックする。
  void DebugCheckResourceSize();
  u32 DebugGetResourceFreeSize();
#endif // PM_DEBUG


  /**
   *  @brief エリア用のヒープのうち、フィールド用に用意したヒープを返す。
   */
  gfl2::heap::HeapBase * GetFieldExtHeap(){ return m_pFieldExtHeap; };
  System::nijiAllocator * GetFieldExtHeapAllocator(){ return m_pFieldExtHeapAllocator; };

private:
	gfl2::heap::HeapBase * m_pArcFileOpenHeap;	//!	アーカイブファイルを開くためのヒープ

  AreaID m_AreaID;
  gfl2::heap::HeapBase* m_pHeap;
  gfl2::heap::HeapBase* m_pFieldExtHeap;
  System::nijiAllocator * m_pFieldExtHeapAllocator;
 

 

  void* m_pBuffer[AREA_DATA_IDX_NUM];  //! データバッファ
  u32 m_DataSize[AREA_DATA_IDX_NUM];
  gfl2::fs::BinLinkerAccessor m_Accessor[AREA_DATA_IDX_NUM]; //! データアクセサー

  gfl2::fs::AsyncFileManager* m_pFileManager;

  // このエリアで使用してよいメモリサイズ
  u32 m_AreaDataSizeMax;

};

GFL_NAMESPACE_END(Field);

#endif	// __FIELD_AREA_H__

