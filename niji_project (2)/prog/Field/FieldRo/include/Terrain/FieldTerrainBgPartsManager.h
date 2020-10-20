/**
* @brief  フィールドの地形BGパーツ管理クラス
* @file   FieldTerrainBgPartsManager.h
* @author ikuta_junya
* @data   2015.09.04
*/

#if !defined( __FIELDTERRAINBGPARTSMANAGER_H_INCLUDED__ )
#define __FIELDTERRAINBGPARTSMANAGER_H_INCLUDED__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Matrix44.h>
#include <System/include/nijiAllocator.h>

// field
#include "Field/FieldRo/include/Terrain/FieldDynamicList.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );

class TerrainBgParts;
class TerrainFactory;

typedef Field::DynamicList< TerrainBgParts* > BgPartsList;
typedef Field::ListIterator< TerrainBgParts* > BgPartsListIterator;

/**
* @brief フィールドの地形リソースの生成を担うクラス
*/
class TerrainBgPartsManager
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( TerrainBgPartsManager );

public:

  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    gfl2::heap::HeapBase*         m_pSystemHeap;                 // 管理システム生成に使用するヒープ
    TerrainFactory*               m_pTerrainFactory;             // 地形生成機能
  };

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  TerrainBgPartsManager( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~TerrainBgPartsManager( void );
  
  /**
  * @brief 初期化処理
  *
  * @param  rInitDesc   初期化設定
  *
  * @return 無し
  */
  void Initialize( const InitializeDescription& rInitDesc );

  /**
  * @brief 終了処理
  *
  * @return 無し
  */
  void Terminate( void );

  /**
  * @brief 更新処理(ロード状況を監視、リソースの生成)
  *
  * @return 無し
  */
  void Update( f32 localAnimationFrame, f32 oneDayAnimationFrame, const gfl2::math::Matrix44* pCullingMatrixTable );

  /**
  * @brief フィールド開始用更新処理
  *
  * @return true Setup継続 false 完了
  */
  b32 UpdateStartSetup( void );

  /**
  * @brief フィールド終了用更新処理
  *
  * @return true Setup継続 false 完了
  */
  b32 UpdateEndSetup( void );

  /**
  * @brief 指定データ番号のBgPartsが有効かチェックする
  *
  * @param  no        データ番号
  *
  * @return true 有効   false 無効
  */
  b32 CheckBgPartsNoVailed( u32 no ) const;

  /**
  * @brief 指定データ番号のBgPartsの非同期読み込み開始
  *
  * @param  no                データ番号
  * @param  pDeviceHeap       読み込み及びインスタンス生成に使用するヒープ
  * @param  pDeviceAllocator  インスタンス生成に使用するアロケータ
  *
  * @return 成否( データ番号が不正値、既に存在する場合はfalseを返し何もしない )
  */
  b32 LoadBgPartsAsync( u32 no, gfl2::heap::HeapBase* pDeviceHeap, System::nijiAllocator* pDeviceAllocator );

  /**
  * @brief 全てのBgPartsを破棄する
  *
  * @return 無し
  */
  void UnLoadBgPartsAll( void );
  
  /**
  * @brief 指定データ番号のBgPartsを破棄する
  *
  * @param  no        データ番号
  *
  * @return 成否
  */
  b32 UnLoadBgParts( u32 no );

  /**
  * @brief 管理しているBgParts数を取得する
  *        ロード中、破棄中を含む
  *
  * @return BgParts数
  */
  u32 CountBgParts( void ) const;

  /**
  * @brief BGパーツを読み込んでいる最中か取得する
  *
  * @return true 読み込んでいる最中
  */
  b32 IsLoading( void ) const;


  /**
  * @brief 指定データ番号のBgPartsを表示フラグを設定する
  *
  * @param  no        データ番号
  * @param  flag      true: 表示 false: 非表示
  *
  * @return 無し
  */
  void SetCreateEnable( b32 flag );

  /**
  * @brief 指定データ番号のBgPartsを表示フラグを設定する
  *
  * @param  no        データ番号
  * @param  flag      true: 表示 false: 非表示
  *
  * @return 無し
  */
  void SetVisible( u32 no, b32 flag );

  /**
  * @brief 指定データ番号のBgPartsが既にあるか取得する
  *
  * @param  no        データ番号
  *
  * @return true ある   false ない
  */
  b32 CheckBgPartsExist( u32 no ) const;

private:


  /**
  * @brief 指定データ番号のBgPartsを取得する
  *
  * @param  no        データ番号
  *
  * @retval NULL 無い
  * @return BgParts
  */
  TerrainBgParts* SearchBgParts( u32 no );

  /**
  * @brief 指定データ番号のBgPartsを取得する
  *
  * @param  no        データ番号
  *
  * @retval NULL 無い
  * @return BgParts
  */
  const TerrainBgParts* SearchBgParts( u32 no ) const;

  // 内部制御
  b32            m_CreateEnable;                  // true 生成許可　false 生成不可

  // 内部オブジェクト
  BgPartsList*   m_pBgPartsList;

  // 外部オブジェクト
  TerrainFactory*  m_pTerrainFactory;             // 地形生成機能

};  // class TerrainBgPartsManager


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINBGPARTSMANAGER_H_INCLUDED__