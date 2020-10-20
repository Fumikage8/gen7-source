#pragma once
//=================================================================================================
/**
 *	@file	model_rsc_manager.h
 *	@brief
 *	@author	tamada@gamefreak.co.jp
 *	@date	2014.04.24
 */
//=================================================================================================
#include "chara_define.h"
#include "../../System/include/HeapDefine.h"
#include <fs/include/gfl2_FsAsyncFileManager.h>

namespace gfl2{ namespace renderingengine { namespace scenegraph {
	namespace resource { class ResourceNode;		}
	namespace instance { class ModelInstanceNode;	}
}}}


//----------------------------------------------------------------------------------------
//! @brief リソース管理オブジェクトのマネージャー
//----------------------------------------------------------------------------------------
class RscManager
{
public:

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

	//----------------------------------------------------------------------------
	/**
	*	@brief  シングルトンインスタンスの取得
	*    
	*	@return インスタンス
	*/
	//-----------------------------------------------------------------------------
	static RscManager* RscManager::getInstance() 
	{
		static RscManager instance;
		return &instance;
	}

	//-----------------------------------------------------------------------------
	/**
	*  @brief	コンストラクタ
	*/
	//-----------------------------------------------------------------------------
	RscManager(){}

	//-----------------------------------------------------------------------------
	/**
	*  @brief	デストラクタ
	*/
	//-----------------------------------------------------------------------------
	~RscManager(){}

	//-----------------------------------------------------------------------------
	/**
	*  @brief	Setup・Terminate
	*/
	//-----------------------------------------------------------------------------
  void Setup( gfl2::heap::HeapBase * pDevice, gfl2::fs::AsyncFileManager* pAsyncReadMan );
  void Terminate(void);

	//----------------------------------------------------------------------------
	/**
	*	@brief  HIGHモデルの取得関数
	*
	*	@param	id		リソースID
	*	@return モデルリソースノードのインスタンス
	*/
	//-----------------------------------------------------------------------------
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetHighModelResource(chara::RSC_ID id);

  //----------------------------------------------------------------------------
	/**
	*	@brief  LOWモデルの取得関数
	*
	*	@param	id		リソースID
	*	@return モデルリソースノードのインスタンス
	*/
	//-----------------------------------------------------------------------------
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetLowModelResource (chara::RSC_ID id);

	//----------------------------------------------------------------------------
	/**
	*	@brief  モーションの取得関数
	*
	*	@param	id		リソースID
	*	@return モーションリソースノードのインスタンス
	*/
	//-----------------------------------------------------------------------------
	gfl2::renderingengine::scenegraph::resource::ResourceNode* GetMotionResource(chara::RSC_ID id,u32 motionId);

  //----------------------------------------------------------------------------
	/**
	*	@brief  口制御モーションの取得関数
	*
	*	@param	id		リソースID
	*	@return 口制御モーションリソースノードのインスタンス
	*/
	//-----------------------------------------------------------------------------
	gfl2::renderingengine::scenegraph::resource::ResourceNode* GetMouthMotionResource( chara::RSC_ID id );

  //----------------------------------------------------------------------------
	/**
	*	@brief  目パチ制御モーションの取得関数
	*
	*	@param	id		リソースID
	*	@return 口制御モーションリソースノードのインスタンス
	*/
	//-----------------------------------------------------------------------------
	gfl2::renderingengine::scenegraph::resource::ResourceNode* GetEyeMotionResource( chara::RSC_ID id );

  //----------------------------------------------------------------------------
	/**
	*	@brief  モーションコンフィグの取得関数
	*
	*	@param	id		リソースID
	*	@return モーションコンフィグリソースノードのインスタンス
	*/
	//-----------------------------------------------------------------------------
	gfl2::renderingengine::scenegraph::resource::ResourceNode* GetMotionConfigResource(chara::RSC_ID id,u32 motionId);

	//----------------------------------------------------------------------------
	/**
	*	@brief  衝突されるコリジョンの取得関数
	*
	*	@param	id		リソースID
	*	@return コリジョンリソースノードのインスタンス
	*/
	//-----------------------------------------------------------------------------
	gfl2::renderingengine::scenegraph::resource::ResourceNode* GetStaticCollisionResource(chara::RSC_ID id);


  GLResourceAllocator* GetAllocator( void ){ return &m_ResourceAllocator; }

  // モデルリソースノードを取得
  inline gfl2::renderingengine::scenegraph::resource::ResourceNode** GetHighModelResourceNodeTable() { return m_ppModelResourceNodeTable; }

private:

	//----------------------------------------------------------------------------
	/**
	*	@brief  ファイル読み込み
	*	@param  利用するアロケータ
	*	@param  ファイルパス
	*	@return Rawデータ
	*/
	//-----------------------------------------------------------------------------
	void *ReadData( const char* pFilePath );

	GLResourceAllocator m_ResourceAllocator;

  gfl2::heap::HeapBase* m_pDevice;
  gfl2::fs::AsyncFileManager* m_pAsyncReadMan;


  // モデルリソース
  void**                                                      m_ppModelBinaryTable;   
  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_ppModelResourceNodeTable;
  u32                                                         m_ModelDataCount;

  // モーションリソース
  void**                                                      m_ppMotionBinaryTable;   
  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_ppMotionResourceNodeTable;
  u32                                                         m_MotionDataCount;

  static const s32                                            BIN_DATA_CNT = 256;
  c8*                                                         m_pBinaryDataArray[BIN_DATA_CNT];
  s32                                                         m_binaryDataIndex;

  static const s32                                            RESOURCE_NODE_CNT = 256;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pResourceNodeArray[RESOURCE_NODE_CNT];
  s32                                                         m_resourceNodeIndex;
};
