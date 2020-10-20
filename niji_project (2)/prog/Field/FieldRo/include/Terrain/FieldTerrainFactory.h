/**
* @brief  フィールドの地形リソースの生成を担うクラス
* @file   FieldTerrainFactory.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __FIELDTERRAINFACTORY_H_INCLUDED__ )
#define __FIELDTERRAINFACTORY_H_INCLUDED__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <Collision/include/gfl2_CollisionModel.h>
#include <System/include/nijiAllocator.h>

// RenderingEngine
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ArrayResourceNode.h>

// poke_3d
#include <model/include/gfl2_BaseModel.h>

// field
#include "Field/FieldRo/include/Terrain/FieldDynamicList.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainTypes.h" // for MemoryMode


// 前方宣言
GFL_NAMESPACE_BEGIN( gfl2 );
GFL_NAMESPACE_BEGIN( fs );

class BinLinkerAccessor;
class AsyncFileManager;

GFL_NAMESPACE_END( fs );
GFL_NAMESPACE_END( gfl2 );

GFL_NAMESPACE_BEGIN( Field );
class EnvManager;
class MyRenderingPipeLine;

GFL_NAMESPACE_BEGIN(KusaModel);
class FieldKusaModel;
GFL_NAMESPACE_END(KusaModel);

GFL_NAMESPACE_BEGIN( Terrain );

// 前方宣言
class TerrainLayoutData;  // フィールド地形の配置設定データ


/**
* @brief フィールドの地形リソースの生成を担うクラス
*/
class TerrainFactory
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( TerrainFactory );

public:

  class HighModelUnit;
  class BgPartsUnit;

  typedef Field::DynamicList< BgPartsUnit* > BgPartsUnitList;
  typedef Field::ListIterator< BgPartsUnit* > BgPartsUnitListIterator;

  // 複数のリソースノードを子に持つためのノード
  class ResourceTopNode : public gfl2::renderingengine::scenegraph::NodeTrait<ResourceTopNode, gfl2::renderingengine::scenegraph::resource::ArrayResourceNode>
  {
  public:

    friend class ResourceFactory;
    friend class SceneGraphFactory;

    virtual ~ResourceTopNode()
    {

    }

    static gfl2::util::FnvHash GetTypeHash()
    {
      static gfl2::util::FnvHash hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::ResourceTopNode") ) );
      return hash;
    }

    void SetVersionId(u32 id) { m_VersionId = id; }
    u32 GetVersionId() const { return m_VersionId; }

    static ResourceTopNode* CreateNode(gfl2::gfx::IGLAllocator* pAllocator)
    {
      gfl2::gfx::GLMemory::SetInstanceAllocator( pAllocator );
      ResourceTopNode* new_node = new ResourceTopNode();
      gfl2::gfx::GLMemory::ClearInstanceAllocator();
      return new_node;
    }

  protected:
    ResourceTopNode() : m_VersionId( 0 )
    {

    }

    u32 m_VersionId;      // 読み込んだリソースのバージョンID
  };


  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    TerrainLayoutData*            m_pTerrainLayoutData;          // 地形の配置情報
    gfl2::fs::BinLinkerAccessor*  m_pAreaResourceDataAccessor;   // 1エリア分の3Dリソースデータ(テクスチャ/シェーダー/アニメーション)
    gfl2::fs::AsyncFileManager*   m_pFileManager;                // ファイル読み込みマネージャー
    gfl2::heap::HeapBase*         m_pSystemHeap;                 // 管理システム生成に使用するヒープ
    gfl2::heap::HeapBase*         m_pHighModelDeviceHeap;        // Highモデル用ヒープの親ヒープ
    gfl2::heap::HeapBase*         m_pDeviceHeap;                 // Highモデル以外のリソースに使用するヒープ(地形初期化以降は使用されない)
    gfl2::gfx::IGLAllocator*      m_pAllocator;                  // Highモデル以外のリソースに使用するアロケーター(地形初期化以降は使用されない)
    u32                           m_HighModleWorkNum;            // highモデル同時生成数
    MemoryMode::Value             m_MemoryMode;                  // ブロックのメモリ形式
    EnvManager*                   m_pEnvManager;                 // 環境マネージャー
    MyRenderingPipeLine*          m_pRenderingPipeLine;          // 描画パイプライン
  };

  /** @brief  生成ユニット状態識別子 */
  struct UnitState
  {
    enum Value
    {
      NOT_EXIST = 0,    // 存在しない
      LOADING,          // ロード中
      LOAD_COMPLETION   // ロード完了
    };
  };

#if PM_DEBUG
  struct DEBUG_INFO
  {
    s32 binaryCompressSize;   // バイナリ(圧縮時)
    s32 binarySize;           // バイナリ(解凍時)
    s32 instanceSize;         // インスタンスサイズ？
    s32 heapSize;             // ヒープ最大
    s32 heapFreeSize;         // ヒープ残り
  };
#endif

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  TerrainFactory( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~TerrainFactory( void );
  
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
  void Update( void );
  
  /**
  * @brief 指定ブロック番号のhighモデルデータが有効かチェックする
  *
  * @param  x      x軸番号
  * @param  z      z軸番号
  * @param  replaceNo 置き換え番号
  *
  * @return true 有効   false 無効
  */
  b32 CheckHighModelDataVailed( u32 x, u32 z, u16 replaceNo = 0 ) const;

  /**
  * @brief 指定ブロック番号のhighモデルデータの非同期読み込み開始
  *
  * @param  x         x軸番号
  * @param  z         z軸番号
  * @param  replaceNo 置き換え番号
  *
  * @return 生成Unit番号(高速にアクセスできる、使わなくても良い)
  */
  s32 LoadHighModelAsync( u32 x, u32 z, u16 replaceNo = 0 );

  /**
  * @brief 指定ブロック番号のhighモデルデータの読み込み状態をチェックする
  *
  * @param  x      x軸番号
  * @param  z      z軸番号
  *
  * @return 生成ユニットの状態
  */
  UnitState::Value CheckHighModelUnitState( u32 x, u32 z ) const;

  /**
  * @brief モデルを破棄する
  *
  * @param  ppModel 対象(削除後、NULLクリアされる)
  *
  * @return 無し
  */
  void DeleteModel( poke_3d::model::BaseModel** ppModel );

  /**
  * @brief 指定ブロック番号のhighモデルデータを破棄する
  *
  * @param  x      x軸番号
  * @param  z      z軸番号
  *
  * @return 無し
  */
  void UnLoadHighModel( u32 x, u32 z );

  /**
  * @brief 稼動していないHighモデル用生成Unit数を取得
  *
  * @return ユニット数
  */
  u32 GetHighModelUnitUnRunnigNum( void ) const;

  /**
  * @brief 指定ブロック番号のhighモデルを生成する
  *
  * @param  x      x軸番号
  * @param  z      z軸番号
  *
  * @return 生成したインスタンス(不正な場合はNULLを返す)
  */
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* CreateHighModelNode( u32 x, u32 z );

  /**
  * @brief 指定ブロック番号のhighモデルを生成する
  *
  * @param  x      x軸番号
  * @param  z      z軸番号
  *
  * @return 生成したインスタンス(不正な場合はNULLを返す)
  */
  poke_3d::model::BaseModel* CreateHighModel( u32 x, u32 z );

  /**
  * @brief 指定ブロック番号の高さコリジョンモデルを生成する
  *
  * @param  x      x軸番号
  * @param  z      z軸番号
  *
  * @return 生成したインスタンス(不正な場合はNULLを返す)
  */
  gfl2::collision::CollisionModel* CreateHeightCollisionModel( u32 x, u32 z );

  /**
  * @brief 指定ブロック番号の壁コリジョンモデルを生成する
  *
  * @param  x                 x軸番号
  * @param  z                 z軸番号
  * @param  ppCollisionModel  生成したインスタンス格納先[ WallIndex::MAX ] NULLが入ることもある
  *
  * @return ロードが行われていない場合はfalseを返す
  */
  b32 CreateWallCollisionModel( u32 x, u32 z, gfl2::collision::CollisionModel** ppCollisionModel );

  /**
  * @brief 指定ブロック番号のMiddleモデルを生成する
  *
  * @param  x      x軸番号
  * @param  z      z軸番号
  *
  * @return 生成したインスタンス(不正な場合はNULLを返す)
  */
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* CreateMiddleModelNode( u32 x, u32 z, u16 replaceNo = 0 );

  /**
  * @brief 指定ブロック番号のMiddleモデルを生成する
  *
  * @param  x      x軸番号
  * @param  z      z軸番号
  *
  * @return 生成したインスタンス(不正な場合はNULLを返す)
  */
  poke_3d::model::BaseModel* CreateMiddleModel( u32 x, u32 z, u16 replaceNo = 0 );


  /**
  * @brief 指定ブロック番号の草モデルを生成する
  *
  * @param  x      x軸番号
  * @param  z      z軸番号
  *
  * @return 生成したインスタンス(不正な場合はNULLを返す)
  */
  KusaModel::FieldKusaModel** CreateEncountGrassModelTable( u32 x, u32 z );

  /**
  * @brief 指定ブロック番号の草モデルを削除する
  *
  * @param  pppModelTable 対象
  *
  * @return 無し
  */
  void DeleteEncountGrassModelTable( KusaModel::FieldKusaModel*** pppModel );

  /**
  * @brief エンカウント草モデルテーブルの要素数
  * @return エンカウント草モデルテーブルの要素数
  */
  u32 GetEncountGrassModelTableNum() const;

  /**
  * @brief 指定番号の遠景モデルを生成する
  *
  * @param  no    番号
  *
  * @return 生成したインスタンス(不正な場合はNULLを返す)
  */
  poke_3d::model::BaseModel* CreateDistantViewModel( u32 no );

  /**
  * @brief 遠景モデルを削除する
  *
  * @param  ppNode 対象
  *
  * @return 無し
  */
  void DeleteDistantViewModel( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode** ppNode );

  /**
  * @brief 遠景モデル数を取得 
  * @note アッパーでは配置情報に持たせたほうが良い
  *
  * @return 遠景モデル数
  */
  u32 GetDistantViewModelNum( void ) const;

  /**
  * @brief ローカルアニメーションのフレーム数を取得
  *
  * @return フレーム数
  */
  u32 GetLocalAnimationFrameSize( void ) const { return m_LocalAnimationFrameSize; }

  /**
  * @brief 1日アニメーションのフレーム数を取得
  *
  * @return フレーム数
  */
  u32 GetOneDayAnimationFrameSize( void ) const { return m_OneDayAnimationFrameSize; }

  /**
  * @brief 生成ユニットが保持するデバイスヒープを取得
  *
  * @param  unitNo ユニット番号
  *
  * @return デバイスヒープ
  */
  gfl2::heap::HeapBase* GetUnitDeviceHeap( s32 unitNo );
  System::nijiAllocator* GetUnitAllocator( s32 unitNo );

  //
  // BGパーツ
  //

  /**
  * @brief 指定番号のBgPartsが有効かチェックする
  *
  * @param  no  番号
  *
  * @return true 有効   false 無効
  */
  b32 CheckBgPartsNoVailed( u32 no ) const;

  /**
  * @brief 指定番号のBgPartの非同期読み込み開始
  *
  * @param  no          番号
  * @param  pDeviceHeap 読み込みに使用するヒープ
  *
  * @return 成否
  */
  b32 LoadBgPartsAsync( u32 no, gfl2::heap::HeapBase* pDeviceHeap, System::nijiAllocator* pDeviceAllocator );

  /**
  * @brief 指定番号のBgPartを破棄
  *        開放チェックは呼び出し側で行ってください
  *
  * @param  no  番号
  *
  * @return 無し
  */
  void UnLoadBgParts( u32 no );

  /**
  * @brief 指定番号のBgPartを生成
  *
  * @param  no  番号
  *
  * @return 生成したインスタンス(不正な場合はNULLを返す)
  */
  poke_3d::model::BaseModel* CreateBgParts( u32 no,  gfl2::heap::HeapBase* pDeviceHeap, System::nijiAllocator* pDeviceAllocator );

  /**
  * @brief 指定番号のBgPartsの読み込み状態をチェックする
  *
  * @param  no  番号
  *
  * @return 生成ユニットの状態
  */
  UnitState::Value CheckBgPartsUnitState( u32 no ) const;


#if PM_DEBUG
  void DebugDumpBlockBinSize( u32 x, u32 z );
  void DebugDumpBlockInstanceSize( u32 x, u32 z );
  void DebugDumpBlockForScript( u32 x, u32 z );
  b32 DebugGetInfo( u32 x, u32 z, DEBUG_INFO* pOut ) const ;
#endif // #if PM_DEBUG

private:

  /**
  * @brief 指定ブロック番号のHighモデル用生成Unitを取得
  *
  * @param  x      x軸番号
  * @param  z      z軸番号
  *
  * @return ユニット(不正な場合はNULLを返す)
  */
  HighModelUnit* GetHighModelUnit( u32 x, u32 z );

  /**
  * @brief 指定ブロック番号のHighモデル用生成Unitを取得(const)
  *
  * @param  x      x軸番号
  * @param  z      z軸番号
  *
  * @return ユニット(不正な場合はNULLを返す)
  */
  const HighModelUnit* GetHighModelUnit( u32 x, u32 z ) const;

  /**
  * @brief 未稼働のHighモデル用生成Unitを取得
  *
  * @return ユニット(未稼働Unitが無い場合はNULLを返す)
  */
  HighModelUnit* GetNotRunningHighModelUnit( void );


  //
  // BGパーツ
  //

  /**
  * @brief 指定データ番号のBgParts生成ユニットを取得する
  *
  * @param  no        データ番号
  *
  * @retval NULL 無い
  * @return BgParts生成ユニット
  */
  const BgPartsUnit* SearchBgPartsUnit( u32 no ) const;

  /**
  * @brief 指定データ番号のBgParts生成ユニットを取得する
  *
  * @param  no        データ番号
  *
  * @retval NULL 無い
  * @return BgParts生成ユニット
  */
  BgPartsUnit* SearchBgPartsUnit( u32 no );

  //
  // 制御
  //
  TerrainLayoutData*    m_pTerrainLayoutData;          // 地形の配置情報

  u32 m_LocalAnimationFrameSize;                       // Localアニメの尺
  u32 m_OneDayAnimationFrameSize;                      // 1日アニメの尺


  //
  // オブジェクト
  //
  HighModelUnit*                                              m_pHighModelUnitTable;
  u32                                                         m_HighModelUnitNum;           // highモデル生成Unit数
  u32                                                         m_HighModelUnitRunningNum;    // highモデル生成Unit稼動数
  
  BgPartsUnitList*                                            m_pBgPartsUnitList;

  //
  // リソースデータ
  //
  ResourceTopNode*                                          m_pTextureShaderResourceRootNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pHightModelResourceTable;
  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_ppMiddleModelResourceTable;
  u32                                                         m_MiddleModelResourceNum;
  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_ppDistantViewModelResourceTable;
  u32                                                         m_DistantViewModelResourceNum;

  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pLocalAnimationRootNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pOneDayAnimationRootNode;

  //
  // 外部オブジェクト
  //
  MyRenderingPipeLine*          m_pRenderingPipeLine;          // 描画パイプライン
  gfl2::fs::AsyncFileManager*   m_pFileManager;                // ファイル読み込みマネージャー
  gfl2::heap::HeapBase*         m_pDeviceHeap;                 // Highモデル以外のリソース生成に使用するヒープ(地形初期化以降は使用されない)
  gfl2::gfx::IGLAllocator*      m_pAllocator;                  // Highモデル以外のリソース生成に使用するアロケーター(地形初期化以降は使用されない)
  EnvManager*                   m_pEnvManager;                 // 環境マネージャー

};  // class TerrainFactory


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINFACTORY_H_INCLUDED__