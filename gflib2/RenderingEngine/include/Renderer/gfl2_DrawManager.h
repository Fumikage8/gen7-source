//=============================================================================
/**
 * @file    gfl2_DrawManager.h
 * @brief   描画マネージャクラス(描画ノード管理、描画順コントロールなどを行う。)
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2016.5.11   ishiguro  重複機能関数の削除
 *  2016.5.10   ishiguro  ワークサイズ取得系関数追加
 *  2016.5.5    ishiguro  デバッグ掲示関数追加
 *
 */
//=============================================================================
#ifndef GFLIB2_RENDERINGENGINE_RENDERER_DRAWMANAGER_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_RENDERER_DRAWMANAGER_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <gfx/include/gfl2_GFGL.h>

#include <RenderingEngine/include/renderer/gfl2_RenderState.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawableNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include <functional>

namespace gfl2 { namespace renderingengine { namespace renderer { 

//==============================================================================
/**
 * @class DrawManager
 * @brief 描画マネージャクラス
 * @date  2014
 */
//==============================================================================
class DrawManager : public gfx::GLMemory
{
public:

	static const u32							EXECUTABLE_LAYER_MAX = 4;//! 一度に実行できるレイヤー数

	struct InitDescription{
		InitDescription( u32 nodeSize = 0, u32 tagSize = 0, u32 envSize = 0 ):
			m_NodeStackSize( nodeSize ),
			m_DrawTagStackSize( tagSize ),
			m_DrawEnvStackSize( envSize )
		{
		}
		u32				m_NodeStackSize;
		u32				m_DrawTagStackSize;
		u32				m_DrawEnvStackSize;
	};

  struct OrderingTableInfo{
    OrderingTableInfo():m_OrderingTable(NULL),m_VisibleTagNum(0){}
	  DrawTag																															**m_OrderingTable;
	  u32																																	m_VisibleTagNum;
  };

#ifdef WIN32
	//static DrawManager* Create( InitDescription desc = InitDescription( 256, 1024 * 8, 16 ) );//128*8*8 = r02の最適化前map01_11_11が８つ分くらい。
	static DrawManager* Create( gfx::IGLAllocator * pAllocator, InitDescription desc = InitDescription( 256, 1024 * 8, 16 ) );//128*8*8 = r02の最適化前map01_11_11が８つ分くらい。
#else
	//static DrawManager* Create( InitDescription desc = InitDescription( 128, 1024, 16 ) );
	static DrawManager* Create( gfx::IGLAllocator* pAllocator, InitDescription desc = InitDescription( 128, 1024, 16 ) );
#endif
	
	virtual ~DrawManager();

	//! @name 描画オブジェクト操作
	//@{
		void AddObject( scenegraph::instance::DrawableNode *pNode );		//!< 描画オブジェクトの追加
		void RemoveObject( scenegraph::instance::DrawableNode *pNode );	//!< 描画オブジェクトの削除
		void RemoveAllObject();																					//!< 描画オブジェクトをすべて削除
    u32 ObjectCnt();                                                //!< 追加された描画オブジェクトの総数

  #if defined(GF_CONFIG_DEBUG) || defined(GF_CONFIG_DEVELOP)
    //-----------------------------------------------------------------------------
    /// @brief    描画オブジェクトコンテナ最大サイズ取得関数
    /// @return   描画オブジェクトコンテナ最大サイズ
    //-----------------------------------------------------------------------------
    u32 GetDrawableNodeContainerMaxSize() const;
  #endif  //  GF_CONFIG_DEBUG || GF_CONFIG_DEVELOP
	//@}

	//! @name DrawEnv操作
	//@{
		void AddDrawEnv( scenegraph::instance::DrawEnvNode *pNode );		//!< DrawEnvの追加
		void RemoveDrawEnv( scenegraph::instance::DrawEnvNode *pNode );	//!< DrawEnvの削除
		void RemoveAllDrawEnv();																				//!< DrawEnvをすべて削除

  #if defined(GF_CONFIG_DEBUG) || defined(GF_CONFIG_DEVELOP)
    //-----------------------------------------------------------------------------
    /// @brief    DrawEnvNodeコンテナ使用サイズ取得関数
    /// @return   DrawEnvNodeコンテナ使用サイズ
    //-----------------------------------------------------------------------------
    u32 GetDrawEnvNodeContainerUsedSize() const;

    //-----------------------------------------------------------------------------
    /// @brief    DrawEnvNodeコンテナ最大サイズ取得関数
    /// @return   DrawEnvNodeコンテナ最大サイズ
    //-----------------------------------------------------------------------------
    u32 GetDrawEnvNodeContainerMaxSize() const;
  #endif  //  GF_CONFIG_DEBUG || GF_CONFIG_DEVELOP
	//@}

	//! @name 描画操作
	//@{
		b32 IsDrawLayerEnable( const scenegraph::resource::DrawLayer::Enum type );
		void DrawLayerEnable( const scenegraph::resource::DrawLayer::Enum type );
		void DrawLayerDisable( const scenegraph::resource::DrawLayer::Enum type );

		void Update();
		void Draw( b32 fUpdate = true );                             //!< シーン全体を描画
	//@}

	//! @name マトリクス操作
	//@{
		inline void SetProjectionMatrix( const math::Matrix44& mat ){ m_Projection = mat; }
		inline const math::Matrix44& GetProjectionMatrix() const { return m_Projection; }

		inline void SetViewMatrix( const math::Matrix34& mat ){ m_View = mat; }
		inline const math::Matrix34& GetViewMatrix() const { return m_View; }
	//@}

  //! @name RenderState変更
	//@{
		inline void SetOverrideShader( const IShaderDriver* pShader ){ m_RenderState.m_pOverrideShader = pShader; }
    void ViewSpaceRenderEnable( b32 enable );
    bool IsViewSpaceRenderEnable() const;
  //@}
	
  //! @name 情報取得
	//@{
    //@ 全描画タグ取得 @note ＯＴ上書き未対応
    inline u32 GetAllTagNum( void ) const;

    //@ 可視描画タグ取得 @note ＯＴ上書きされている場合は、上書きデータのタグ数を返す
    u32 GetVisibleTagNum( void ) const;

    //@ ＯＴ上書き（共有設定）
    void SetOrderingTableInfoOverride( OrderingTableInfo* pOT ){ m_pOTOverride = pOT; }

    //@ ＯＴ取得（Update関数以降に取得すること）
    OrderingTableInfo* GetOrderingTableInfo(){ return &m_OTInfo; }
    const OrderingTableInfo* GetOrderingTableInfo() const { return &m_OTInfo; }

  #if defined(GF_CONFIG_DEBUG) || defined(GF_CONFIG_DEVELOP)
    //-----------------------------------------------------------------------------
    /// @brief       OrderingTableコンテナ最大サイズ取得関数
    /// @return      OrderingTableコンテナ最大サイズ
    /// @attention   DrawManager::Update関数呼出し後に取得できます。
    //-----------------------------------------------------------------------------
    u32 GetOrderingTableMaxSize() const;
  #endif  //  GF_CONFIG_DEBUG || GF_CONFIG_DEVELOP
  //@}

private:
	

protected:
	DrawManager( InitDescription desc );

	void SetDrawEnvState( const DrawTag* pTarget );
	bool SetDrawEnvState_IsCrossAABB( const math::AABB* pAABB, scenegraph::instance::DrawEnvNode* pEnv );
	void SetDrawEnvState_LightPickUp( const DrawTag* pTarget, const math::AABB* pAABB, scenegraph::instance::DrawEnvNode* pEnv );
	void SetDrawEnvState_LightPickUp_PickUp( class PickUpLight& pickUp, const math::AABB* pAABB, scenegraph::instance::DrawEnvNode* pEnv, s32 lightSetNo );
	void SetDrawEnvState_LightPickUp_AddLightNode( class PickUpLight& pickUp );

#if GFL_DEBUG
  //----------------------------------------------------------------------------
  /**
   *   @brief デバッグ掲示関数(ワークサイズの使用状況報告)
   *
   *   @param pMessage         警告メッセージ
   *   @param usedBufferSize   使用しているサイズ
   *   @param maxBufferSize    ワーク最大サイズ
  */
  //-----------------------------------------------------------------------------
  void DebugSizeCheckDump( const c8* pMessage, u32 usedBufferSize, u32 maxBufferSize );
#endif

	InitDescription																											m_Desc;
	gfx::GLFixedSizeContainer<scenegraph::instance::DrawableNode*>			m_DrawableNodeContainer;
	gfx::GLFixedSizeContainer<scenegraph::instance::DrawEnvNode*>				m_DrawEnvNodeContainer;

  OrderingTableInfo                                                   m_OTInfo;
  OrderingTableInfo                                                   *m_pOTOverride;

	RenderState																													m_RenderState;
	math::Matrix44																											m_Projection;   // 透視変換行列
	math::Matrix34																											m_View;					// ビュー変換行列
	u8																																	m_DrawLayerBit;

  //!< @brief ソート比較用関数オブジェクト
  struct RenderElementCompare : ::std::binary_function<DrawTag*, DrawTag*, bool>
  {
    bool operator() (
        const DrawTag* lhs,
        const DrawTag* rhs)
    {
      return lhs->GetRenderKey() < rhs->GetRenderKey();
    }
  };

};

}}}// gfl2::renderingengine::renderer

#endif//  GFLIB2_RENDERINGENGINE_RENDERER_DRAWMANAGER_H_INCLUDED

