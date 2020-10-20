//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl2_EffectRenderPath.h
 *	@brief  エフェクトレンダリングパス
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __GFL2_EFFECT_RENDER_PATH_H__ )
#define __GFL2_EFFECT_RENDER_PATH_H__
#pragma once

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_Surface.h>

#include <util/include/gfl2_List.h>
#include <util/include/gfl2_ListIterator.h>

#include <Effect/include/gfl2_EffectSystem.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(Effect)

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

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
/**
 *  @brief  エフェクト　レンダリングパス
 */
class EffectRenderPath : public renderingengine::renderer::RenderingPipeLine::IRenderPath
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(EffectRenderPath);

public:
  /**
   *  @brief Config
   */
  class Config
  {
  public:
    System* m_pSystem;  //! エフェクトシステム
    util::List<u32>* m_cpGroupIDList; //! レンダリングするグループIDのリスト (中は参照のみ）

    //f32 m_FrameRate;  //! フレームレート

  public:
    Config() :
      m_pSystem(NULL),
      m_cpGroupIDList(NULL)
      //m_FrameRate(1.0f)
    {
    }

    void SetSystem( System* pSystem ) { m_pSystem = pSystem; }
    void SetGroupIDList( util::List<u32>* pList ) { m_cpGroupIDList = pList; }
    
  };

  //カメラ情報上書き用
  struct Camera{
		math::Matrix44		m_Projection;   // 透視変換行列
		math::Matrix34		m_View;					// ビュー変換行列
	};

public:


  /**
   * @brief  コンストラクタ
   */
  EffectRenderPath();
  EffectRenderPath( const Config& config );

  /**
   * @brief  デストラクタ
   */
  virtual ~EffectRenderPath();


  /** 
   * @brief コンフィグ情報を更新
   */
  void SetConfig( const Config& config );

  /** 
   * @brief コンフィグ情報を取得
   */
  const Config& GetConfig( void ) const { return m_Config; }


public:

  /**
   *  @brief  描画実行
   */
	virtual void Execute(const renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext);
  
  /**
   *  @brief  カメラ情報上書き
   */
  virtual const Camera* CameraOverride(){ return NULL; }

private:

  /**
   *  @brief  描画前コマンド生成
   */
	void BeginDraw(const renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext);

  /**
   *  @brief  描画後コマンド生成
   */
	void EndDraw(const renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext);

private:
  Config m_Config;


 };

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(gfl2)

#endif	// __GFL2_EFFECT_RENDER_PATH_H__

