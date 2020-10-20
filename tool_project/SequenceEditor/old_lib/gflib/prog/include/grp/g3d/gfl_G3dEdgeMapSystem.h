#ifndef __GFL_G3DEDGEMAPSYSTEM_H__
#define __GFL_G3DEDGEMAPSYSTEM_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dEdgeMapSystem.h
 *	@brief  3D影システム
 *	@author	Masateru Ishiguro
 *	@date		2011.04.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// CTR_SDK

// NW4C
#include <nw/gfx.h>
#include <nw/gfx/gfx_Model.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

//  gflib grp
#include <grp/gfl_RenderTarget.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dModel.h>
#include <grp/g3d/gfl_G3dCamera.h>
#include <grp/g3d/gfl_G3dShadowGroup.h>
#include <grp/g3d/gfl_G3dH3dModel.h>

#include <base/gfl_KeyContainer.h>

#define DECLARE_ENUM_BEGIN( CLASSNAME )	\
	class CLASSNAME {											\
	public:																\
		enum Enum {
#define DECLARE_ENUM_END( CLASSNAME )		\
		};                                  \
	};

namespace gfl {
namespace grp {
namespace g3d {
  
//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ユーザーアトリビュートEdgeMapAlphaMask
DECLARE_ENUM_BEGIN( EdgeMapAlphaMask )
  None = 0,
  Texture0,
  Texture1,
  Texture2,
  NumberOf
DECLARE_ENUM_END( EdgeMapAlphaMask )  

  
enum{
  DEFAULT_EDGE_MODEL_VCOL_A = 0,
  ALPHA_EDGE_MODEL_VCOL_A,
  DISABLE_NORMAL_EDGE_MODEL_VCOL_A,
  ALPHA_DISABLE_NORMAL_EDGE_MODEL_VCOL_A,
  DISABLE_NORMAL_EDGE_DISABLE_MODEL_VCOL_A,
  ALPHA_DISABLE_NORMAL_EDGE_DISABLE_MODEL_VCOL_A,
  EDGE_MODEL_MAX
};

DECLARE_ENUM_BEGIN( CommandType )
  Upper = 0,                      // 上画面　or 左画面
  Lower,                          // 下画面
  UpperRight,                     // 右画面
  None,                           // 対応する画面なし。 
  NumberOf
DECLARE_ENUM_END( CommandType )  
  
DECLARE_ENUM_BEGIN( EdgeType )
  DefaultEdgeTypeID = 0,
  VColorEdgeTypeID,
  None,                       // 透過（描画しない）
  Erace,                      // 抜き
  OwnColor,                   // 自前で塗る（のでマテリアル切り替えを行わない）
  NumberOf
DECLARE_ENUM_END( CommandType )    

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
  
//-------------------------------------
///	プリレンダー用スロット
//=====================================  
class EdgeMapPreRenderCallbackSlot : public nw::gfx::Model::RenderSlot, public gfl::grp::g3d::H3dMeshListener
{
public:
    
    EdgeMapPreRenderCallbackSlot() : nw::gfx::Model::RenderSlot(NULL),
      m_MorphingEnable( false )
    {

    }
    virtual ~EdgeMapPreRenderCallbackSlot(){}
    
    //----------------------------------------------------------------------
    /**
     * @brief nw::gfx::Model用コールバック関数
     * @param model           シェーダー適応対象
     * @param mesh            シェーダー適応対象
     * @param renderContext   現在のRenderContext
     */
    //----------------------------------------------------------------------
    virtual void Invoke(nw::gfx::Model* model, nw::gfx::ResMesh mesh, nw::gfx::RenderContext* renderContext);
    
    //----------------------------------------------------------------------
    /**
     * @brief gfl::grp::g3d::H3dModel用コールバック関数
     * @param model             シェーダー適応対象
     * @param material_index    現在のマテリアルインデックス
     * @param command           コマンドの積み先
     * @param env               現在のH3dEnvironment
     */
    //----------------------------------------------------------------------
    virtual void Invoke(gfl::grp::g3d::H3dModel* model, s32 material_index, gfl::grp::g3d::H3dCommand* command, gfl::grp::g3d::H3dEnvironment* env);
    
    //----------------------------------------------------------------------
    /**
     * @brief エッジマップモデル設定関数（初期化時に設定されます。）
     * @param id            モデルインデックス
     * @param p_model       モデルデータポインタ
     */
    //----------------------------------------------------------------------
    void SetEdgeModel( s32 id, gfl::grp::g3d::Model* p_model ){ m_pGfxEdgeModel[id] = p_model; }
    
    //----------------------------------------------------------------------
    /**
     * @brief エッジマップH3dモデル設定関数（初期化時に設定されます。）
     * @param id            モデルインデックス
     * @param p_model       モデルデータポインタ
     */
    //----------------------------------------------------------------------
    void SetEdgeModel( s32 id, gfl::grp::g3d::H3dModel* p_model ){ m_pH3dEdgeModel[id] = p_model; }
    
    //----------------------------------------------------------------------
    /**
     * @brief コールバック有効無効設定関数
     * 
     * 　　　　エッジマップに登録されたモデルはこのリスナーがぶら下がるが、
     * 　　　　実際にエッジマップに描く時でない場合は、falseが渡される。
     * 
     * @param f     有効無効フラグ
     */
    //----------------------------------------------------------------------
    void RenderCallbackEnable( b32 f ){ m_fEnable = f; }
    
    //----------------------------------------------------------------------
    /**
     * @brief モーフィング対応シェーダーを使うか
     * 
     * 　　　　　kujiraでは、結局モーフィングシェーダーは実装されませんでした。
     * 　　　　　のでこの関数は使用されていません。
     * 
     * @param f   有効無効フラグ
     */
    //----------------------------------------------------------------------
    void SetMorphingEnable( b32 f ){ m_MorphingEnable = f; }
    
private:    
    gfl::grp::g3d::Model*       m_pGfxEdgeModel[EDGE_MODEL_MAX];    //!< マテリアル差し替えようGfxモデル
    gfl::grp::g3d::H3dModel*    m_pH3dEdgeModel[EDGE_MODEL_MAX];    //!< マテリアル差し替えようH3dモデル
    b32                         m_fEnable;                          //!< Invoke有効フラグ
    b32                         m_MorphingEnable;                   //!< モーフィング有効フラグ
};

//-------------------------------------
///	ポストレンダー用スロット
//=====================================
class EdgeMapPostRenderCallbackSlot : public nw::gfx::Model::RenderSlot, public gfl::grp::g3d::H3dMeshListener
{
public:
    EdgeMapPostRenderCallbackSlot() : nw::gfx::Model::RenderSlot(NULL){}
    
    virtual ~EdgeMapPostRenderCallbackSlot(){}
    
    //----------------------------------------------------------------------
    /**
     * @brief nw::gfx::Model用コールバック関数
     * @param model           シェーダー適応対象
     * @param mesh            シェーダー適応対象
     * @param renderContext   現在のRenderContext
     */
    //----------------------------------------------------------------------
    virtual void Invoke(nw::gfx::Model* model, nw::gfx::ResMesh mesh, nw::gfx::RenderContext* renderContext);
    
    //----------------------------------------------------------------------
    /**
     * @brief gfl::grp::g3d::H3dModel用コールバック関数
     * @param model             シェーダー適応対象
     * @param material_index    現在のマテリアルインデックス
     * @param command           コマンドの積み先
     * @param env               現在のH3dEnvironment
     */
    //----------------------------------------------------------------------
    virtual void Invoke(gfl::grp::g3d::H3dModel* model, s32 material_index, gfl::grp::g3d::H3dCommand* command, gfl::grp::g3d::H3dEnvironment* env);
    
    //----------------------------------------------------------------------
    /**
     * @brief コールバック有効無効設定関数
     * 
     * 　　　　エッジマップに登録されたモデルはこのリスナーがぶら下がるが、
     * 　　　　実際にエッジマップに描く時でない場合は、falseが渡される。
     * 
     * @param f     有効無効フラグ
     */
    //----------------------------------------------------------------------
    void RenderCallbackEnable( b32 f ){ m_fEnable = f; }
    
    b32                         m_fEnable;      //!< Invoke有効フラグ   @note priveteにしたほうが良い。
};

//-------------------------------------
///	エッジシステムクラス
//=====================================
class G3DEdgeMapSystem
{
  GFL_FORBID_COPY_AND_ASSIGN(G3DEdgeMapSystem);
  friend class Scene;
  
public:
  /**
   * @brief ＩＤエッジが有効かのユーザータグ名取得
   * @return タグ名
   */
  static const char* GetIDEdgeEnableTagName();
  
  /**
   * @brief エッジＩＤ値のユーザータグ名取得
   * @return タグ名
   */
  static const char* GetEdgeIDTagName();
  
  /**
   * @brief モーフィング有効無効設定関数
   * @param fEnable     有効無効フラグ
   */
  void MorphingEnable( f32 fEnable );
  
  /**
   * @brief コールバック有効無効関数
   * 
   * 　　　　エッジマップに登録されたモデルはこのリスナーがぶら下がるが、
   * 　　　　実際にエッジマップに描く時でない場合は、falseが渡される。
   * 
   * @param f   有効無効フラグ
   */
  void RenderCallbackEnable( b32 f );
  
public:
  
  static const s32          EDGE_MAP_WIDTH = 512;     //!< エッジマップ描画先レンダーターゲットの幅
  static const s32          EDGE_MAP_HEIGHT = 256;    //!< エッジマップ描画先レンダーターゲットの高さ
  
  G3DEdgeMapSystem();
  ~G3DEdgeMapSystem();
  
  /**
   * @brief エッジシステムワーク確保
   * @param heap_allocator      メモリ確保先
   * @param device_allocator    メモリ確保先
   * @param p_desc              レンダーターゲット設定内容
   */
  void CreateWork(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, gfl::grp::RenderTarget::Description* p_desc, gfl::grp::RenderTarget::Description* p_low_desc, b32 f_delete_memory_control );
  
  /**
   * @brief マテリアル切り替え用モデル作成関数
   * @param heap_allocator            メモリ確保先
   * @param device_allocator          メモリ確保先
   */
  void createGfxModel(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);
  
  /**
   * @brief マテリアル切り替え用モデル作成関数
   * @param heap_allocator            メモリ確保先
   * @param device_allocator          メモリ確保先
   * @param f_delete_memory_control   解放時コントロール
   */
  void createH3dModel(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, b32 f_delete_memory_control);

  /**
   * @brief エッジワーク破棄
   */
  void DestroyWork(void);

  /**
   * @brief 描画グループ設定
   * @param scene_draw_group_index    描画グループ
   */
  void SetGroupIndex( s32 scene_draw_group_index ){ m_GroupIndex = scene_draw_group_index; }

  /**
   * @brief   描画グループインデックス取得
   * @return  描画グループインデックス
   */
  s32 GetGroupIndex(){ return m_GroupIndex; }

  /**
   * @brief   レンダーターゲット取得
   * @return  レンダーターゲット
   */
  gfl::grp::RenderTarget* GetEdgeRenderTarget( s32 type ) const
  {
    switch( type ){
    case DISPLAY_UPPER:                          // 上画面　or 左画面
      return m_render_target[CommandType::Upper];
    case DISPLAY_LOWER:                          // 下画面
      return m_render_target[CommandType::Lower];
    case DISPLAY_UPPER_RIGHT:                    // 右画面
      return m_render_target[CommandType::UpperRight];
    }

    return m_render_target[CommandType::None];// 対応する画面なし。 
  }
  
  /**
   * @brief マテリアル差し替えリスナー取得
   * @return リスナーインスタンス
   */
  EdgeMapPreRenderCallbackSlot* GetPreRenderCallbackSlot(){ return &m_PreRenderCallbackSlot; }
  
  /**
   * @brief マテリアル差し替えリスナー取得
   * @return リスナーインスタンス
   */
  EdgeMapPostRenderCallbackSlot* GetPostRenderCallbackSlot(){ return &m_PostRenderCallbackSlot; }
  
  DECLARE_ENUM_BEGIN( CommonResourceType )
    NormalTexture = 0,
    NormalToUvShader,
    NumberOf
  DECLARE_ENUM_END( CommonResourceType )  

  s32                       m_GroupIndex;                                         //!< 描画フループ
  gfl::grp::g3d::Resource   m_GfxCommonResource[CommonResourceType::NumberOf];    //!< マテリアル差し替えようモデルリソース
  gfl::grp::g3d::Resource   m_GfxResource[EDGE_MODEL_MAX];                        //!< マテリアル差し替えようモデルリソース
  gfl::grp::g3d::Model      m_GfxEdgeModel[EDGE_MODEL_MAX];                       //!< マテリアル差し替えようモデル
  
  gfl::grp::g3d::H3dResource   m_H3dCommonResource[CommonResourceType::NumberOf];             //!< マテリアル差し替えようモデルリソース
  b8                           m_H3dCommonResourceReleaseFlag[CommonResourceType::NumberOf];  //!< マテリアル差し替えようモデルリソース解放フラグ
  gfl::grp::g3d::H3dResource   m_H3dResource[EDGE_MODEL_MAX];                                 //!< マテリアル差し替えようモデルリソース
  b8                           m_H3dResourceReleaseFlag[EDGE_MODEL_MAX];                      //!< マテリアル差し替えようモデルリソース解放フラグ
  gfl::grp::g3d::H3dModel      m_H3dEdgeModel[EDGE_MODEL_MAX];                                //!< マテリアル差し替えようモデル
  void                         *m_pH3dCommonData[ CommonResourceType::NumberOf ];             //!< マテリアル差し替えようモデルリソース
  void                         *m_pH3dResourceData[ EDGE_MODEL_MAX ];                         //!< マテリアル差し替えようモデルリソース
  
  gfl::grp::RenderTarget*         m_render_target[CommandType::NumberOf];                     //!< エッジマップ描画先
  EdgeMapPreRenderCallbackSlot    m_PreRenderCallbackSlot;                                    //!< 描画前コールバックリスナー
  EdgeMapPostRenderCallbackSlot   m_PostRenderCallbackSlot;                                   //!< 描画後コールバックリスナー
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#undef		DECLARE_ENUM_BEGIN
#undef		DECLARE_ENUM_END

#endif // __GFL_G3DEDGEMAPSYSTEM_H__
