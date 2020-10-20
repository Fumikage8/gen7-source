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

// RenderingEngine
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

// poke_3d
#include <model/include/gfl2_BaseModel.h>

#include "ViewerProc/include/ReadViewerData.h";

class BaseCollisionScene;   /// コリジョン管理シーン
class StaticActor;

// 前方宣言
GFL_NAMESPACE_BEGIN( gfl2 );
GFL_NAMESPACE_BEGIN( fs );

class BinLinkerAccessor;
///class AsyncFileManager;

GFL_NAMESPACE_END( fs );
GFL_NAMESPACE_END( gfl2 );

GFL_NAMESPACE_BEGIN( Field );
class EnvManager;
class MyRenderingPipeLine;
/**
GFL_NAMESPACE_BEGIN(KusaModel);
class FieldKusaModel;
GFL_NAMESPACE_END(KusaModel);
*/ //@saito_del
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

  // @todo 仮
  class FieldResourceNode : public gfl2::renderingengine::scenegraph::NodeTrait<FieldResourceNode, gfl2::renderingengine::scenegraph::DagNode>
  {
  public:

    friend class ResourceFactory;
    friend class SceneGraphFactory;

    virtual ~FieldResourceNode()
    {

    }

    static gfl2::util::FnvHash GetTypeHash()
    {
      static gfl2::util::FnvHash hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::FieldResourceNode") ) );
      return hash;
    }

    void SetVersionId(u32 id) { m_VersionId = id; }
    u32 GetVersionId() const { return m_VersionId; }

    static FieldResourceNode* CreateNode()
    {
      return new FieldResourceNode();
    }

  protected:
    FieldResourceNode() : m_VersionId( 0 )
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
    ///gfl2::fs::AsyncFileManager*   m_pFileManager;                // ファイル読み込みマネージャー
    gfl2::heap::HeapBase*         m_pSystemHeap;                 // 管理システム生成に使用するヒープ
    gfl2::heap::HeapBase*         m_pHighModelDeviceHeap;        // Highモデルに使用するヒープ(動的)
    gfl2::gfx::IGLAllocator*      m_pAllocator;                  // リソース生成に使用するアロケーター
    u32                           m_HighModleWorkNum;            // highモデル同時生成数
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
  void Initialize( const InitializeDescription& rInitDesc, BaseCollisionScene* pCollisionScene );

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
  *
  * @return true 有効   false 無効
  */
  ///b32 CheckHighModelDataVailed( u32 x, u32 z ) const;

  /**
  * @brief 指定ブロック番号のhighモデルデータの非同期読み込み開始
  *
  * @param  x      x軸番号
  * @param  z      z軸番号
  *
  * @return 生成Unit番号(高速にアクセスできる、使わなくても良い)
  */
  s32 LoadHighModelAsync( u32 x, u32 z );

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

	gfl2::collision::CollisionModel* CreateHeightCollisionModel();
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
  * @param  x      x軸番号
  * @param  z      z軸番号
  *
  * @return 生成したインスタンス(不正な場合はNULLを返す)
  */
  gfl2::collision::CollisionModel* CreateWallCollisionModel( u32 x, u32 z );
  
  /**
  * @brief 指定ブロック番号のMiddleモデルを生成する
  *
  * @param  x      x軸番号
  * @param  z      z軸番号
  *
  * @return 生成したインスタンス(不正な場合はNULLを返す)
  */
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* CreateMiddleModelNode( u32 x, u32 z );

  /**
  * @brief 指定ブロック番号のMiddleモデルを生成する
  *
  * @param  x      x軸番号
  * @param  z      z軸番号
  *
  * @return 生成したインスタンス(不正な場合はNULLを返す)
  */
  poke_3d::model::BaseModel* CreateMiddleModel( u32 x, u32 z );

#if 0
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
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* CreateDistantViewModel( u32 no );

  /**
  * @brief 遠景モデル数を取得 @todo 本来はlayoutが持つ機能
  *
  * @return 遠景モデル数
  */
  u32 GetDistantViewModelNum( void ) const;
#endif //@saito_del
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

	/**
  *	@brief  ファイル読み込み
  *	@return Rawデータ
  */
	///void* ReadData( const char* pFilePath );

	poke_3d::model::BaseModel* CreateHighModel();

#if 0
	//-----------------------------------------------------------------------------
	/*
	*          内部クラス宣言(リソースアロケータ)
	*          @class GLAllocator
	*/
	//-----------------------------------------------------------------------------
	class GLResourceAllocator : public gfl2::gfx::IGLAllocator
	{
	public:
		
		//----------------------------------------------------------------------------
		/**
		*  @brief  コンストラクタ
		*/
		//-----------------------------------------------------------------------------
		GLResourceAllocator():IGLAllocator(),m_pHeap(NULL) {}

		//----------------------------------------------------------------------------
		/**
		*  @brief  デストラクタ
		*/
		//-----------------------------------------------------------------------------
		virtual			~GLResourceAllocator(){}
		
    //----------------------------------------------------------------------------
    /**
    *  @brief  コンストラクタ
    */
    //-----------------------------------------------------------------------------
    void  init(gfl2::heap::HeapBase* pHeap){ m_pHeap = pHeap; }

		//----------------------------------------------------------------------------
		/**
		*  @brief  メインメモリの取得
		*  @param  取得したいメモリサイズ
		*  @param  アライメント
		*  @return メモリ
		*/
		//-----------------------------------------------------------------------------
		virtual void*	SystemMemoryMalloc(u32 size, u32 alignment){ return GflHeapAllocMemoryAlign( m_pHeap , size , alignment ); }

		//----------------------------------------------------------------------------
		/**
		*  @brief  メインメモリの解放
		*  @param  解放したいメモリのポインタ
		*/
		//-----------------------------------------------------------------------------
		virtual void	SystemMemoryFree(void* const address){ return GflHeapFreeMemory( address ); }

		//----------------------------------------------------------------------------
		/**
		*  @brief  ビデオメモリの取得
		*  @param  取得したいメモリサイズ
		*  @param  アライメント
		*  @return メモリ
		*/
		//-----------------------------------------------------------------------------
		virtual void*	LocalMemoryMalloc(u32 size, u32 alignment){ return NULL; }

		//----------------------------------------------------------------------------
		/**
		*  @brief  ビデオメモリの解放
		*  @param  解放したいメモリのポインタ
		*/
		//-----------------------------------------------------------------------------
		virtual void	LocalMemoryFree(void* const address){ }
		
	private:
    
    gfl2::heap::HeapBase *m_pHeap;

	};
#endif
  //
  // 制御
  //
  TerrainLayoutData*    m_pTerrainLayoutData;          // 地形の配置情報
  gfl2::heap::HeapBase* m_pHighModelDeviceHeap;        // Highモデルに使用するヒープ(動的) @todo 要断片化対策


  //
  // オブジェクト
  //
  HighModelUnit*                                              m_pHighModelUnitTable;
  u32                                                         m_HighModelUnitNum;           // highモデル生成Unit数
  u32                                                         m_HighModelUnitRunningNum;    // highモデル生成Unit稼動数
  
  //
  // リソースデータ
  //
  FieldResourceNode*                                          m_pTextureResourceRootNode;
  FieldResourceNode*                                          m_pShaderResourceRootNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pHightModelResourceTable;
  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_ppMiddleModelResourceTable;
  u32                                                         m_MiddleModelResourceNum;
  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_ppDistantViewModelResourceTable;
  u32                                                         m_DistantViewModelResourceNum;

  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pLocalAnimationRootNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pOneDayAnimationRootNode;

	static const s32                                            BIN_DATA_CNT = 10;
	c8*                                                         m_pBinaryDataArray[BIN_DATA_CNT];
	gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pHightModelResource;

	GLResourceAllocator m_ResourceAllocator;

	gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pHeightCollisionModelResource;
	gfl2::collision::CollisionModel*                                m_pCollisionModel;
  StaticActor*                                                    m_pStaticActor;
	BaseCollisionScene*           m_pCollisionScene;             // コリジョン管理シーン

	poke_3d::model::BaseModel *m_pTerrainModel;


  //
  // 外部オブジェクト
  //
  ///gfl2::fs::AsyncFileManager*   m_pFileManager;                // ファイル読み込みマネージャー
  gfl2::gfx::IGLAllocator*      m_pAllocator;                  // リソース生成に使用するアロケーター
  EnvManager*                   m_pEnvManager;                 // 環境マネージャー
  MyRenderingPipeLine*          m_pRenderingPipeLine;          // 描画パイプライン

};  // class TerrainFactory


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINFACTORY_H_INCLUDED__