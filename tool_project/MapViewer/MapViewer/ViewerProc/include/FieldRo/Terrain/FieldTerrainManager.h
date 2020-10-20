/**
* @brief  フィールドの地形管理を担うクラス
* @file   FieldTerrainManager.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __FIELDTERRAINMANGERER_H_INCLUDED__ )
#define __FIELDTERRAINMANGERER_H_INCLUDED__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

// field
#include "ViewerProc/include/FieldRo/FieldRootNode.h"


// 前方宣言
GFL_NAMESPACE_BEGIN( gfl2 );
GFL_NAMESPACE_BEGIN( fs );

class BinLinkerAccessor;
class AsyncFileManager;

GFL_NAMESPACE_END( fs );
GFL_NAMESPACE_END( gfl2 );

class BaseCollisionScene; // コリジョンシーン

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
class EnvManager;
class MyRenderingPipeLine;
GFL_NAMESPACE_BEGIN( Terrain );

class TerrainLayoutData;  // フィールド地形の配置設定データ
class TerrainFactory;     // 地形の生成を担うクラス
class TerrainBlock;       // 地形1個
class TerrainDistantView; // 遠景1個
#if 0
GFL_NAMESPACE_BEGIN( Block );
class LodChangeStaticListener;   // 地形ブロックのLOD変更通知を受け取るオブジェクトの基底クラス
GFL_NAMESPACE_END( Block );
#endif //@saito_del

/**
* @brief フィールドの地形管理
*/
class TerrainManager
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( TerrainManager );

public:

  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    gfl2::heap::HeapBase*         m_pSystemHeap;
    gfl2::heap::HeapBase*         m_pDeviceHeap;
    gfl2::gfx::IGLAllocator*      m_pDeviceAllocator;
    TerrainLayoutData*            pTerrainLayoutData;          // 地形の配置情報
    gfl2::fs::BinLinkerAccessor*  pAreaResourceDataAccessor;   // 1エリア分の3Dリソースデータ(テクスチャ/シェーダー/アニメーション)
    gfl2::fs::AsyncFileManager*   m_pFileManager;              // ファイル読み込みマネージャー
    BaseCollisionScene*           m_pWallCollsionScene;        // 壁コリジョン管理シーン
    EnvManager*                   m_pEnvManager;               // 環境マネージャー
    MyRenderingPipeLine*          m_pRenderingPipeLine;        // 描画パイプライン

  };

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  TerrainManager( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~TerrainManager( void );
  
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
  * @brief 更新処理
  *
  * @return 無し
  */
  void Update( void );

#if 0
  void SetBlockControlPosition( const gfl2::math::Vector3& rPos )
  {
    m_BlockControlPosition = rPos;
  }
#endif //@saito_del
  BaseCollisionScene* GetCollsionScene( void )
  {
    return m_pCollsionScene;
  }
#if 0
  /**
  * @brief 地形ブロックのLOD変更通知を受け取るリスナーを登録
  * @note  リスナーの寿命は登録側で管理してください
  * @note  今は1ブロックあたり10個しかリスナーをもてないが、今後固定長ではなく動的的確保にする可能性がある
  *
  * @param  pListener  登録するリスナー
  *
  * @return 成否
  */
  b32 RegistBlockListener( Block::LodChangeStaticListener* pListener );

  /**
  * @brief 地形ブロックのLOD変更通知を受け取るリスナーを登録削除
  * @note  リスナーの寿命は登録側で管理してください
  *
  * @param  pListener  登録削除するリスナー
  *
  * @return 成否
  */
  b32 UnRegistBlockListener( Block::LodChangeStaticListener* pListener );
  
  
  b32 GetBlockIndex( const gfl2::math::Vector3& rPos, s32* pX, s32* pZ ) const ;


  b32 IsBlockInstanceExist( s32 x, s32 z ) const;
#endif //@saito_del
  /**
  * @brief 配置設定を取得
  *
  * @return 配置設定
  */
  const TerrainLayoutData* GetLayoutData( void ) const { return m_pTerrainLayoutData; }


private:




  //
  // 制御
  //
  gfl2::math::Vector3   m_BlockControlPosition;     // ブロック制御地点
  s32                   m_HighBlockStartX;
  s32                   m_HighBlockStartZ;

  //
  // オブジェクト
  //
  TerrainLayoutData*    m_pTerrainLayoutData;       // 配置設定
  TerrainFactory*       m_pTerrainFactory;          // 地形生成機能
  TerrainBlock***       m_ppTerrainBlockTable;      // 地形ブロック[Z軸][X軸]
  TerrainDistantView*   m_pDistantViewTable;        // 遠景[m_DistantViewNum]
  u32                   m_DistantViewNum;           // 遠景オブジェクト数
  BaseCollisionScene*   m_pCollsionScene;           // 高さコリジョン管理シーン


  //
  // 外部オブジェクト
  //
  BaseCollisionScene*   m_pWallCollsionScene;       // 壁コリジョン管理シーン
  EnvManager*           m_pEnvManager;              // 環境マネージャー


};  // class FieldTerrainManager


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINMANGERER_H_INCLUDED__
