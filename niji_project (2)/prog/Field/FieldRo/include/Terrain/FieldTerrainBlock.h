/**
* @brief  フィールドの地形ブロッククラス
* @file   TerrainBlock.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __FIELDTERRAINBLOCK_H_INCLUDED__ )
#define __FIELDTERRAINBLOCK_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <Collision/include/gfl2_CollisionModel.h>
#include <util/include/gfl2_List.h>

// niji
#include <System/include/nijiAllocator.h>

// poke_3d
#include <model/include/gfl2_BaseModel.h>

// field
#include "Field/FieldRo/include/Terrain/Block/FieldTerrainBlockLodType.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainTypes.h"
#include "Field/FieldRo/include/Terrain/FieldDynamicList.h"

class BaseCollisionScene;   /// コリジョン管理シーン
class StaticActor;

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );

GFL_NAMESPACE_BEGIN(KusaModel);
class FieldKusaModel;
GFL_NAMESPACE_END(KusaModel);

GFL_NAMESPACE_BEGIN(Camera);
class CameraHitObject;
GFL_NAMESPACE_END(Camera);

GFL_NAMESPACE_BEGIN( Terrain );

class TerrainFactory;

GFL_NAMESPACE_BEGIN( Block );
class LodChangeStaticListener;   // 地形ブロックのLOD変更通知を受け取るオブジェクトの基底クラス
GFL_NAMESPACE_END( Block );

typedef Field::DynamicList< Block::LodChangeStaticListener* > StaticLodListenerList;
typedef Field::ListIterator< Block::LodChangeStaticListener* > StaticLodListenerListIterator;

/**
* @brief フィールドの地形管理
*/
class TerrainBlock
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( TerrainBlock );

public:

  struct LodLevel
  {
    enum Value
    {
      HIGH = 0,
      MIDDLE,
      MAX
    };
  };

  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    gfl2::heap::HeapBase*         m_pSystemHeap;
    gfl2::math::Vector3           m_Pos;
    u32                           m_X;
    u32                           m_Z;
    u16                           m_ReplaceNo;
    f32                           m_Size;
    TerrainFactory*               pTerrainFactory;             // 地形生成機能
    BaseCollisionScene*           pCollisionScene;             // 高さコリジョン管理シーン
    BaseCollisionScene**          m_ppWallCollisionSceneTable; // 壁コリジョン管理シーン
    BaseCollisionScene*           m_pCameraEyeCollsionScene;
  };


  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  TerrainBlock( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~TerrainBlock( void );

  /**
  * @brief 初期化処理
  *
  * @param  rInitDesc   初期化設定
  *
  * @return 無し
  */
  void Initialize( const InitializeDescription& rInitDesc );

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
  * @param localAnimationFrame;      Localアニメの現在フレーム
  * @param oneDayAnimationFrame;     1日アニメの現在フレーム
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
  * @brief カメラコリジョン前更新処理
  */
  void UpdateCameraCollisonPrev( void );

  /**
  * @brief カメラコリジョン後更新処理
  */
  void UpdateCameraCollisonAfter( void );

  /**
  * @brief LODが変更中か取得
  *
  * @return true 変更中 false 変更中ではない
  */
  b32 IsLodChanging( void ) const;

  /**
  * @brief LOD変更処理
  *
  * @param  lodLevel 変更したい状態
  *
  * @return 無し
  */
  void ChangeLod( Block::LodType::Value lodLevel );
  Block::LodType::Value GetLod( void ) const { return m_LodType; } 
  Block::LodType::Value GetLodPrev( void ) const { return m_LodTypePrev; } 

  /**
  * @brief LOD変更通知を受け取るリスナーを登録
  * @note  リスナーの寿命は登録側で管理してください
  *
  * @param  pListener  登録するリスナー
  * @param  pListHeap  リスト追加に使用するヒープ( ※最大登録すう分のヒープサイズしか確保されていません、その他の用途には使用しないでください)
  *
  * @return 成否
  */
  b32 RegistListener( Block::LodChangeStaticListener* pListener, gfl2::heap::HeapBase* pHeap );

  /**
  * @brief LOD変更通知を受け取るリスナーを登録削除
  * @note  リスナーの寿命は登録側で管理してください
  *
  * @param  pListener  登録削除するリスナー
  *
  * @return 成否
  */
  b32 UnRegistListener( Block::LodChangeStaticListener* pListener );

  /**
   *  @brief  草モデルを取得する。
   *  @param  index   何段目の草モデルを取得するか。
   *
   *  @retval NULL  なし
   *  @retval ptr   草モデル
   */
  KusaModel::FieldKusaModel* GetKusaModel( u32 index );

  /**
   *  @brief リプレイス番号を取得する
   */
  u32 GetReplaceNo( void ) const { return m_ReplaceNo; }
     
  /**
   *  @brief 現在モデルが表示されているか取得する
   */
  b32 IsVisible( void ) const;

  /**
   *  @brief 現在モデルのDrawTag数を取得
  */
  s32 GetDrawTagNum( void ) const;

  /**
   *  @brief 距離によるmiddleモデルの非表示設定
   */
  void SetMiddleModelInvisibleForDistance( b32 invisible );

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
  const static u32 INVISIBLE_DEBUG_001 = 0x01 << 2;
  void SetDebugInvisibleBit( u32 bitMask, b32 flag );
  static u32* GetEncountGrassModelPriorityDefaultSettingPtr( void );
#endif


private:

  /**
  *  @brief  LOD制御更新
  *
  *  @return 無し
  */
  void UpdateLodControl( void );

  /**
  * @brief LOD変更をリスナーに通知
  *
  * @param  lodType 変更後の状態
  *
  * @return 無し
  */
  void ChangeLodNotificationListener( Block::LodType::Value lodType );

  /**
  * @brief 地形モデルの表示・非表示切り替え
  *
  * @param lodType  対象となるLODモデル
  * @param visible  true 表示 false 非表示
  *
  * @return 無し
  */
  void UpdateVisibleImpl( Block::LodType::Value lodType, b32 visible );

  /**
  * @brief カリングに使用するAABBを取得する
  *
  * @param pOut 格納先
  *
  * @return 無し
  */
  b32 GetCullingAABB( gfl2::math::AABB* pOut ) const;

  /**
  * @brief highモデル削除
  * @note  1フレーム遅れになってない場合は削除しません
  *
  * @param force  trueのとき、1フレーム遅れになってない場合も削除
  *
  * @return true 削除された or 削除済み
  */
  b32 DeleteHighModel( b32 force = false );

  /**
  * @brief 指定モデルのカメラコリジョン(非表示メッシュ)アクターを生成し、シーンに登録する
  *
  * @param pAllocator       アクター生成に使うアロケーター
  * @param pAllocator       アクター生成と生成したアクターのリスト管理に使用するメモリ
  * @param pBaseModel       対象モデル
  * @param pStaticActorList 生成したアクターを保持する動的リスト
  *
  * @return 無し
  */
  void CreateAndResistCollisionCameraEye( System::nijiAllocator* pAllocator, gfl2::heap::HeapBase* pHeap, poke_3d::model::BaseModel* pBaseModel, Field::DynamicList<StaticActor*>* pStaticActorList );

  /**
  * @brief 指定のリストで管理されているカメラコリジョン(非表示メッシュ)アクターを登録解除し、破棄する
  *
  * @param pStaticActorList 指定リスト
  *
  * @return 無し
  */
  void DeleteAndUnResistCollisionCameraEye( Field::DynamicList<StaticActor*>* pStaticActorList );

  /**
  * @brief エンカウント草モデルの描画優先度を設定
  *
  * @param priority 描画の優先度
  * @param force    true 必ず更新
  *
  * @return 無し
  */
  void SetPriorityToEncountGrassModel( u32 priority, b32 force );


  /**
  * @brief エンカウント草モデルのカリング情報を初期化
  *
  * @return 無し
  */
  void SetupEncountGrassCullingInfo( void );

  /**
  * @brief エンカウント草モデルの表示を更新
  *
  * @return 無し
  */
  void UpdateEncountGrassVisible( const gfl2::math::Matrix44* pCullingMatrixTable );

  /**
  * @brief エンカウント草モデルの表示を設定
  *
  * @param lodType  対象となるLODモデル
  * @param visible  true 表示 false 非表示
  *
  * @return 無し
  */
  void SetEncountGrassVisible( Block::LodType::Value lodType, b32 visible );
  
#if PM_DEBUG

  //
  // データチェック
  // 

  // コリジョン透過メッシュに不備があればアサートを出す
  void DebugDataCheckCollisionMesh( poke_3d::model::BaseModel* pBaseModel );

  // コリジョン透過メッシュのマテリアル重複チェック
  void DebugDataCheckCollisionMeshMaterial( poke_3d::model::BaseModel* pBaseModel, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pSrcMesh, gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pSrcMat );
 


#endif

  
  // エンカウント草のカリング制御用情報
  struct ENCOUNT_GRASS_CULLING_INFO
  {
    b32 isOverTerrainModel;                           // true:地形モデルのAABBをはみ出ているならtrue
    b32 isCulling;                                    // カリング結果
  };

  //
  // 制御
  //
  u32                           m_X;
  u32                           m_Z;
  f32                           m_Size;               // ブロックの大きさ(正方形)
  gfl2::math::Vector3           m_Pos;                // ブロックの中心座標
  b32                           m_IsLoading;
  b32                           m_IsHighModel;
  u16                           m_ReplaceNo;
  s32                           m_FactoryUnitNo;     // 生成Unit番号(-1で無効)

  Block::LodType::Value         m_RequestLodType;    // 出したいモデル
  Block::LodType::Value         m_LodType;           // 実際に表示しているモデル
  Block::LodType::Value         m_LodTypePrev;       // 1フレーム前のm_LodTypeの値(trigger取得用)

  u8                            m_InvisibleBit;                  // 0なら表示、それ以外なら非表示
  u8                            m_InvisibleBitForEncountGrass;   // 0なら表示、それ以外なら非表示

  //
  // オブジェクト
  //
  poke_3d::model::BaseModel*                                      m_pModelTable[ Block::LodType::MAX ];

  u32                                                             m_EncountGrassModelPriority;
  ENCOUNT_GRASS_CULLING_INFO*                                     m_pEncountGrassCullingInfoTable;      // エンカウント草のカリング制御用情報
  KusaModel::FieldKusaModel**                                     m_ppEncountGrassModelTable[ Block::LodType::MAX ];
  gfl2::collision::CollisionModel*                                m_pCollisionModel;
  StaticActor*                                                    m_pStaticActor;
  gfl2::collision::CollisionModel**                               m_ppWallCollisionModelTable;
  StaticActor**                                                   m_ppWallStaticActorTable;
  Field::DynamicList<StaticActor*>*                               m_pStaticActorListForCameraEye;
  b32                                                             m_IsHideMeshSystemOff;        // trueならカメラ接触でメッシュの非表示にする機能無効化中

  // リスナーリスト
  StaticLodListenerList*                                          m_pStaticLodListenerList;

  //
  // リソースデータ
  //

  //
  // 外部オブジェクト
  //
  TerrainFactory*               m_pTerrainFactory;             // 地形生成機能
  BaseCollisionScene*           m_pCollisionScene;             // コリジョン管理シーン
  BaseCollisionScene**          m_ppWallCollisionSceneTable;   // 壁コリジョン管理シーン
  BaseCollisionScene*           m_pCameraEyeCollsionScene;
};  // class TerrainBlock


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINBLOCK_H_INCLUDED__