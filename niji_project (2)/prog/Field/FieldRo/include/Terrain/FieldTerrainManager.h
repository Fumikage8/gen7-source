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
#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainTypes.h"


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


#if PM_DEBUG
GFL_NAMESPACE_BEGIN( Debug );
class FieldDebugMenu;
GFL_NAMESPACE_END( Debug );
#endif


GFL_NAMESPACE_BEGIN(KusaModel);
class FieldKusaModel;
GFL_NAMESPACE_END(KusaModel);

GFL_NAMESPACE_BEGIN( Terrain );

class TerrainLayoutData;  // フィールド地形の配置設定データ
class TerrainFactory;     // 地形の生成を担うクラス
class TerrainBlock;       // 地形1個
class TerrainDistantView; // 遠景1個
class LocalHeapUtilForFixedSize;
class TerrainBgPartsManager;

GFL_NAMESPACE_BEGIN( Block );
class LodChangeStaticListener;   // 地形ブロックのLOD変更通知を受け取るオブジェクトの基底クラス
class StaticCullingListener;     // フィールド地形ブロックのカリング変更の通知を受ける基底クラス
GFL_NAMESPACE_END( Block );

/**
* @brief フィールドの地形管理
*/
class TerrainManager
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( TerrainManager );

public:

  struct BlockControlMode
  {
    enum Value
    {
      NORMAL = 0,
      SP_ROOM,                 // 特殊な室内用、2×2ブロック単位でロードする
      PLAN,
      FIX,                     // 現在座標から更新しない
      MAX
    };
  };

  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:

    InitializeDescription( void ) :
      m_pSystemHeap( NULL )
      ,m_pDeviceHeap( NULL )
      ,m_pDeviceAllocator( NULL )
      ,pTerrainLayoutData( NULL )
      ,pAreaResourceDataAccessor( NULL )
      ,m_pFileManager( NULL )
      ,m_pCameraEyeCollsionScene( NULL )
      ,m_pEnvManager( NULL )
      ,m_pRenderingPipeLine( NULL )
      ,m_StaticLodListenerMax( 0 )
      ,m_BlockControlMode( BlockControlMode::NORMAL )
      ,m_MemoryMode( MemoryMode::NORMAL )
    {
      for( u32 i = 0; i < WallIndex::MAX; i++ )
      {
        m_pWallCollsionSceneTable[ i ] = NULL;
      }
    }


    gfl2::heap::HeapBase*         m_pSystemHeap;
    gfl2::heap::HeapBase*         m_pDeviceHeap;
    gfl2::gfx::IGLAllocator*      m_pDeviceAllocator;
    TerrainLayoutData*            pTerrainLayoutData;          // 地形の配置情報
    gfl2::fs::BinLinkerAccessor*  pAreaResourceDataAccessor;   // 1エリア分の3Dリソースデータ(テクスチャ/シェーダー/アニメーション)
    gfl2::fs::AsyncFileManager*   m_pFileManager;              // ファイル読み込みマネージャー
    BaseCollisionScene*           m_pWallCollsionSceneTable[ WallIndex::MAX ];     // 壁コリジョン管理シーン
    BaseCollisionScene*           m_pCameraEyeCollsionScene;
    EnvManager*                   m_pEnvManager;               // 環境マネージャー
    MyRenderingPipeLine*          m_pRenderingPipeLine;        // 描画パイプライン
    BlockControlMode::Value       m_BlockControlMode;          // ブロックのコントロール型式
    MemoryMode::Value             m_MemoryMode;                // ブロックのメモリ形式
    u32                           m_StaticLodListenerMax;      // フィールド地形ブロックのLOD変更の通知を受けるリスナーの最大数
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
  * @brief 初期化待ち処理
  *
  * @return true 完了 false 未完了
  */
  b32 InitializeWait( void );

  /**
  * @brief 地形ブロックのコントロール開始
  * @note  フィールド初期化の流れで実行される
  *
  * @return 無し
  */
  void ControlStart( void );

  /**
  * @brief 終了処理
  *
  * @return 無し
  */
  void Terminate( void );

  /**
  * @brief 更新処理
  *
  * @param hour 時間
  * @param minute 分
  * @param second 秒
  *
  * @return 無し
  */
  void Update( s32 hour, s32 minute, s32 second, const gfl2::math::Matrix44* pViewProjMatrix );

  /**
  * @brief カメラコリジョン前更新処理
  */
  void UpdateCameraCollisonPrev( void );

  /**
  * @brief カメラコリジョン後更新処理
  */
  void UpdateCameraCollisonAfter( void );
  
  /**
  * @brief フィールド開始用更新処理
  * @note  事前にSetBlockControlPositionで自機座標を設定してください
  *
  * @return true Setup継続 false 完了
  */
  b32 UpdateStartSetup( void );

  /**
  * @brief 終了開始リクエスト処理
  * @note  終了処理の一番最初に実行される、UpdateWaitTerminate()がfalseを返した後にTerminate()が実行される
  *
  * @return 無し
  */
  void TerminateReq( void );

  /**
  * @brief 終了時可能まで待つ処理
  * @note  Highモデルのロードが終わるまで待つ
  *
  * @return true Wait継続 false 完了
  */
  b32 UpdateWaitTerminate( void );

  /**
  * @brief 地形ブロックのLODが変更中か取得
  *
  * @return true 変更中 false 変更中ではない
  */
  b32 IsBlockLodChanging( void ) const;


  void SetBlockControlPosition( const gfl2::math::Vector3& rPos )
  {
    m_BlockControlPosition = rPos;
  }

  BaseCollisionScene* GetCollsionScene( void )
  {
    return m_pCollsionScene;
  }

  BaseCollisionScene* GetCollisionSceneWall( WallIndex::Value wallIndex )
  {
    return m_pWallCollsionSceneTable[ wallIndex ];
  }

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

  /**
  * @brief 指定座標の地形ブロック番号を取得する(座標が枠外の場合は枠内に含める版)
  * @note  地形ブロック番号が枠内であっても、インスタンスが存在しない場合がある(モデルが一切登録されていない場合)
  *
  * @param  rPos 座標
  * @param  pX   X軸番号格納先
  * @param  pZ   Z軸番号格納先
  *
  * @return true 取得成功 false 失敗(未セットアップ状態で実行した)
  */
  b32 GetBlockIndexClamp( const gfl2::math::Vector3& rPos, s32* pX, s32* pZ ) const ;

  /**
  * @brief 指定座標の地形ブロック番号を取得する
  * @note  地形ブロック番号が枠内であっても、インスタンスが存在しない場合がある(モデルが一切登録されていない場合)
  *
  * @param  rPos 座標
  * @param  pX   X軸番号格納先
  * @param  pZ   Z軸番号格納先
  *
  * @return true 取得成功 false 失敗(未セットアップ状態　or 枠外)
  */
  b32 GetBlockIndex( const gfl2::math::Vector3& rPos, s32* pX, s32* pZ ) const ;

  /**
  * @brief 指定番号の地形ブロックの実体が存在するか取得する
  *
  * @param  x X軸番号
  * @param  z Z軸番号
  *
  * @return true 存在する false 存在しない
  */
  b32 IsBlockInstanceExist( s32 x, s32 z ) const;

  /** 
  * @brief 指定番号の地形ブロックの実体を取得
  *
  * @param  x X軸番号
  * @param  z Z軸番号
  *
  * @return 地形ブロックの実体
  * @retval NULL 存在しない
  */
  const TerrainBlock* GetBlockInstance( s32 x, s32 z ) const;

  /**
  * @brief 配置設定を取得
  *
  * @return 配置設定
  */
  const TerrainLayoutData* GetLayoutData( void ) const { return m_pTerrainLayoutData; }

  /**
   *  @brief  草モデルの取得
   *  @param  x           ブロックX座標
   *  @param  z           ブロックZ座標
   *  @param  floorIndex  フロアインデックス
   */
  KusaModel::FieldKusaModel* GetKusaModel( s32 x, s32 z, s32 floorIndex );
  /**
   *  @brief  草モデルのフロア数を取得
   */
  u32 GetKusaModelFloorNum(void) const;

  /**
  * @brief 地形ブロックの制御点を座標で指定する
  * @note  BlockControlMode::Fixになる
  * @note  変更リクエストの発行まで行う
  * 
  * @param  rPos  座標
  *
  * @return 無し
  */
  void SetExternalBlockControlPointByPosition( const gfl2::math::Vector3& rPos );

  /**
  * @brief 地形ブロックの制御点をブロック番号で指定する
  * @note  BlockControlMode::Fixになる
  * @note  変更リクエストの発行まで行う
  * 
  * @param  x           ブロックX座標
  * @param  z           ブロックZ座標
  *
  * @return 無し
  */
  void SetExternalBlockControlPointByBlockNo( u32 x, u32 z );

  /**
  * @brief 地形ブロックの制御点の座標指定を解除する
  * @note  BlockControlModeが初期値になる
  * @note  変更リクエストの発行まで行う
  * 
  * @return 無し
  */
  void ClearExternalBlockControlPoint( void );

  /**
  * @brief 背景パーツ管理(イベント中にのみ表示する書割のようなモデル)
  * 
  * @return 背景パーツ管理
  */
  TerrainBgPartsManager* GetBgPartsManager( void ) { return m_pBgPartsManager; }

  /**
  * @brief 指定座標の地形ブロックがHighモデルか取得する
  * @note  見た目、コリジョンは地形ブロック境界線からはみでる事を許容しています
  * 
  * @return true Highモデル false Highモデル
  */
  b32 IsHighBlock( const gfl2::math::Vector3& rPos ) const;

  /**
  * @brief 指定座標の地形ブロックがHighモデルか取得する(Trg)
  * @note  見た目、コリジョンは地形ブロック境界線からはみでる事を許容しています
  * 
  * @return true Highモデル false Highモデル
  */
  b32 IsHighBlockTrg( const gfl2::math::Vector3& rPos ) const;

  /**
  * @brief 指定の地形ブロックのリプレイスNoを取得する
  * 
  */
  u32 GetReplaceNo( s32 x, s32 z ) const;

  /**
  * @brief Middleモデルの表示ライン制限を設定
  *
  * @param  line  表示ライン数
  */
  void SetMiddleModelDrawLineLimit( s32 line );

  /**
  * @brief Middleモデルの表示ライン制限をクリア(=全表示)
  */
  void ClearMiddleModelDrawLineLimit( void );

  /**
  * @brief カメラ接触でメッシュの非表示にする機能が有効か取得する
  *
  * @return true 有効   false 無効
  */
  b32 IsHideMeshSystemOn( void ) const { return !m_IsHideMeshSystemOff; }

  /**
  * @brief カメラ接触でメッシュの非表示にする機能を有効化する
  *
  * @param oneTime true:透過遷移を挟さまない false:透過遷移を挟む
  */
  void HideMeshSystemOn( b32 oneTime = true );

  /**
  * @brief カメラ接触でメッシュの非表示にする機能を無効化する
  *
  * @param oneTime true:透過遷移を挟さまない false:透過遷移を挟む
  */
  void HideMeshSystemOff( b32 oneTime = true );


#if PM_DEBUG
  void SetBlockControlSkip( b32 flag  );
  TerrainBlock* GetBlock( s32 x, s32 z );
  void UpdateDrawTagInfo( Debug::FieldDebugMenu* pDebugMenu );

  // デバッグメニュー用
  s32* GetMiddleModelDrawLinePtr( void ){ return &m_MiddleModelDrawLine; }


  //
  // @fix CONNMCat[582]:攻略本用マップ撮影機能、ローカルアニメを特定のフレームで停止する機能
  //

  /**
  * @brief ローカルアニメを指定フレームで一時停止させる
  *
  * @param frame 指定フレーム
  *
  * @return 無し
  */
  void DebugSuspendLocalAnimation( f32 frame );

  /**
  * @brief ローカルアニメの一時停止を解除
  *
  * @return 無し
  */
  void DebugResumeLocalAnimation( void );

  /**
   * @brief 地形生成機能を取得
   *
   * @return 無し
  */
  const TerrainFactory* GetTerrainFactory(void) const { return m_pTerrainFactory; }

#endif

private:

  /**
  *  @brief  アニメーションのフレーム値の更新
  *
  *  @param hour 時間
  *  @param minute 分
  *  @param second 秒
  *
  *  @return 無し
  */
  void UpdateAnimationFrame( s32 hour, s32 minute, s32 second );
  
  /**
  *  @brief  LOD制御更新
  *
  *  @return 無し
  */
  void UpdateLodControl( void );

  /**
  *  @brief  地形ブロック制御点を変更
  *
  *  @param  x           ブロックX座標
  *  @param  z           ブロックZ座標
  *
  *  @return true 実際に変更された  false 現在と同じ設定なので何もしなかった
  */
  b32 ChangeBlockControl( s32 x, s32 z );

  /**
  *  @brief  地形ブロック制御点を計算する(通常モード)
  *
  *  @param  rPos     指定座標
  *  @param  pX       格納先(ブロックX座標)
  *  @param  pZ       格納先(ブロックZ座標)
  *
  *  @return 無し
  */
  void CalcBlockControlPointForNormal( const gfl2::math::Vector3& rPos, s32* pX, s32* pZ ) const;

  /**
  *  @brief  地形ブロック用メモリのモードを取得
  *
  *  @param  memMode            エリアで指定しているメモリモード
  *  @param  highModleWorkNum   Highモデル同時存在数
  *
  *  @return メモリモード
  */
  MemoryMode::Value GetMemoryMode( MemoryMode::Value memMode, u32 highModleWorkNum ) const;


  //
  // 制御
  //
  b32                     m_IsControlStart;           // true 地形ブロックのコントロール開始
  BlockControlMode::Value m_BlockControlMode;         // 現在のブロックコントロールの型式
  BlockControlMode::Value m_BlockControlModeDefault;  // ブロックコントロールの型式、初期設定
  gfl2::math::Vector3   m_BlockControlPosition;     // ブロック制御地点
  s32                   m_HighBlockStartX;
  s32                   m_HighBlockStartZ;
  f32                   m_LocalAnimationFrame;      // Localアニメの現在フレーム
  f32                   m_OneDayAnimationFrame;     // 1日アニメの現在フレーム
  f32                   m_LocalAnimationFrameSize;  // Localアニメの長さ
  f32                   m_OneDayAnimationFrameSize; // 1日アニメの長さ
  s32                   m_MiddleModelDrawLine;      // ミドルモデルの表示制限ライン
  b32                   m_IsHideMeshSystemOff;      // trueならカメラ接触でメッシュの非表示にする機能無効化中

#if PM_DEBUG
  b32                   m_DebugLocalAnimationStop;  // true:ローカルアニメーションの更新を停止
#endif

  //
  // オブジェクト
  //
  TerrainLayoutData*    m_pTerrainLayoutData;       // 配置設定
  TerrainFactory*       m_pTerrainFactory;          // 地形生成機能
  TerrainBlock***       m_ppTerrainBlockTable;      // 地形ブロック[Z軸][X軸]
  TerrainDistantView*   m_pDistantViewTable;        // 遠景[m_DistantViewNum]
  u32                   m_DistantViewNum;           // 遠景オブジェクト数
  TerrainBgPartsManager* m_pBgPartsManager;         // 背景パーツ管理(イベント中にのみ表示する書割のようなモデル)
  BaseCollisionScene*   m_pCollsionScene;           // 高さコリジョン管理シーン
  LocalHeapUtilForFixedSize*  m_pLocalHeapUtilForStaticLodListener; // リスナー管理用ローカルヒープ 

  //
  // 外部オブジェクト
  //
  BaseCollisionScene*   m_pWallCollsionSceneTable[ WallIndex::MAX ]; // 壁コリジョン管理シーン
  EnvManager*           m_pEnvManager;              // 環境マネージャー


};  // class FieldTerrainManager


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINMANGERER_H_INCLUDED__