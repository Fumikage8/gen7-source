/**
* @brief  フィールドの地形管理を担うクラス
* @file   FieldTerrain.h
* @author ikuta_junya
* @data   2015.02.07
*/

#if !defined( __FIELDTERRAIN_H_INCLUDED__ )
#define __FIELDTERRAIN_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <Collision/include/gfl2_CollisionModel.h>

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )
class MyRenderingPipeLine;
GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );

class FieldTerrainBlock;
class FieldTerrainDistantView;

class FieldResourceNode : public gfl2::renderingengine::scenegraph::NodeTrait<FieldResourceNode, gfl2::renderingengine::scenegraph::DagNode>
{
public:

	friend class ResourceManager;
	friend class SceneGraphManager;
	
    virtual ~FieldResourceNode()
    {

    }

  static gfl2::util::FnvHash GetTypeHash()
	{
		static gfl2::util::FnvHash hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::FieldResourceNode") ) );
		return hash;
	}

#if 0
	//! イベントリスナーインターフェイス
	class IEventListener{
	public:
		inline virtual ~IEventListener(){}

		//更新された時
		virtual void OnUpdate( ResourceNode *pThisNode ) = 0;

		//削除された時
		virtual void OnDelete( ResourceNode *pThisNode ) = 0;

	protected:
		IEventListener(){}
	};

	void SetListener( IEventListener *pEventListener ){ m_pEventListener = pEventListener; }
#endif

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

  // IEventListener					*m_pEventListener;

  u32 m_VersionId;      // 読み込んだリソースのバージョンID
};



/**
* @brief フィールドの地形管理
*/
class FieldTerrain
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldTerrain );


public:

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  FieldTerrain( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~FieldTerrain( void );

  /**
  * @brief 初期化処理
  *
  * @param  pSystemHeap システムヒープ
  * @param  pDeviceHeap デバイスヒープ
  * @param  pAsyncFileManager garc読み込みクラス
  *
  * @return 無し
  */
  void Initialize( gfl2::heap::HeapBase* pSystemHeap, gfl2::heap::HeapBase* pDeviceHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, Test::FieldTest::MyRenderingPipeLine* pRenderingPipeLine );

  /**
  * @brief 終了処理
  *
  * @return 無し
  */
  void Finalize( void );


  void Update( const gfl2::math::Vector3& rPos, const gfl2::math::Matrix44& rViewProjMatrix );
  FieldTerrainBlock* GetBlock( u32 width, u32 height );
  gfl2::collision::CollisionModel* GetCollisionModel( void ){ return m_pCollisionModel; }

private:

  // インスタンス
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*  m_pModelInstanceNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode** m_ppModelInstanceNodeTable;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode** m_ppDistantViewModelInstanceNodeTable;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode** m_ppMiddleModelInstanceNodeTable;

  gfl2::collision::CollisionModel*                                m_pCollisionModel;

  // 地形ブロックの実体
  FieldTerrainBlock*                                              m_pFieldTerrainBlockTable;
  u32                                                             m_BlockCountWidth;
  u32                                                             m_BlockCountHeight;
  s32                                                             m_HighBlockStartX;
  s32                                                             m_HighBlockStartZ;

  // 遠景
  FieldTerrainDistantView*                                        m_pDistantViewTable;


  // バイナリ
  void**                                                          m_ppTextureBinaryTable;
  void**                                                          m_ppShaderBinaryTable;
  void**                                                          m_ppModelBinaryTable;
  void**                                                          m_ppMiddleModelBinaryTable;
  void**                                                          m_ppDistantViewModelBinaryTable;
  void*                                                           m_pCollisionModelBinary;
  
  u32                                                             m_TextureDataCount;
  u32                                                             m_ShaderDataCount;
  u32                                                             m_ModelDataCount;
  u32                                                             m_MiddleModelDataCount;
  u32                                                             m_DistantViewModelDataCount;

  // リソース
  gfl2::renderingengine::scenegraph::resource::ResourceNode**      m_ppTextureResourceNodeTable;
  gfl2::renderingengine::scenegraph::resource::ResourceNode**      m_ppShaderResourceNodeTable;

  FieldResourceNode*                         m_pTextureResourceRootNode;
  FieldResourceNode*                         m_pShaderResourceRootNode;

  gfl2::renderingengine::scenegraph::resource::ResourceNode*       m_pModelResourceRootNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode**      m_ppModelResourceRootNodeTable;
  gfl2::renderingengine::scenegraph::resource::ResourceNode**      m_ppMiddleModelResourceRootNodeTable;
  gfl2::renderingengine::scenegraph::resource::ResourceNode**      m_ppDistantViewModelResourceRootNodeTable;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*       m_pCollisionModeResourceRootNode;




};  // class FieldTerrain


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAIN_H_INCLUDED__