#ifndef __GFL_G3DMODEL_H__
#define __GFL_G3DMODEL_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dModel.h
 *	@brief  3Dモデル
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


/*
[コンストレインの用語]
バトンを手に持った人間を思い浮かべる。
持っている人間をhuman、humanの手をhand、handに持たれるバトンをbatonと呼ぶことにする。
モデルがhuman、手のボーンがhand、モデルや手のボーンにくっつけられるモデルがbatonとなる。
*/


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>
#include <gfl_Math.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dSceneNode.h>
#include <grp/g3d/gfl_G3dAnimation.h>
#include <grp/g3d/gfl_G3dResAnim.h>
#include <grp/g3d/gfl_G3dAnimationSlot.h>
#include <grp/g3d/gfl_G3dAnimationManager.h>
#include <grp/g3d/gfl_G3dTexture.h>
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dUserData.h>

#include <math/gfl_MathAABB.h>


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					型宣言
 */
//-----------------------------------------------------------------------------
typedef void (*ModelCalculateCallbackFunction)(Model* model, void* work);




//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------


namespace internal {

//-------------------------------------
///	コールバッククラスの前方宣言
//=====================================
class CallbackConstraintOfSkeleton;
class CallbackConstraintOfTransformNode;

}  // namespace internal


//-------------------------------------
///	3Dモデルクラス
//=====================================
class Model : public SceneNode
{
  GFL_FORBID_COPY_AND_ASSIGN(Model);


  // 下記クラスにRenderQueue関連のprivateメンバ関数へのアクセスを許可する
  friend class Scene;
  friend class RenderSystem;
  friend class ShadowSystem;
  friend class SceneNode;


public:
  // マテリアルのバッファの生成オプション
  enum MaterialBufferOption
  {
    MATERIAL_BUFFER_OPTION_NOT_USE,                             // バッファを独自に持たない
    MATERIAL_BUFFER_OPTION_MATERIAL_COLOR,                      // マテリアルカラーのバッファを独自に持つ(マテリアルカラー、コンスタントカラー、その他カラーのバッファ)
    MATERIAL_BUFFER_OPTION_MATERIAL_COLOR_SHADING_PARAMETER,    // シェーディングパラメータのバッファを独自に持つ(MATERIAL_COLOR | フォグのバッファ)
    MATERIAL_BUFFER_OPTION_ANIMATABLE_MATERIAL,                 // アニメーションするマテリアルのバッファを独自に持つ(マテリアルアニメーションのバッファ)
    MATERIAL_BUFFER_OPTION_ALL_MATERIAL,                        // 全てのマテリアルのバッファを独自に持つ
    MATERIAL_BUFFER_OPTION_MAX
  };

  // 設定内容
  struct Description
  {
    MaterialBufferOption  material_buffer_option;

    Model* material_share_source_model;              // マテリアルの共有元のモデル
        // material_share_source_modelがNULLでないとき、material_buffer_optionはMATERIAL_BUFFER_OPTION_NOT_USEでなければならない。
        // gfx_SceneBuilder.h BufferOption, gfx_SkeletalModel.h BufferOption参考。
    Model* mesh_node_visibility_share_source_model;  // メッシュノードビジビリティの共有元のモデル
   
    s32  max_callbacks;             // 管理できるコールバックの最大数

    u32  constraint_human_baton_num_max;  // humanに拘束されるbatonの最大数
    u32  constraint_hand_num_max;         // くっつけられるhandの最大数(このモデルのボーンの個数以下)
    u32  constraint_hand_baton_num_max;   // 1つのhandに拘束されるbatonの最大数
                                          // (合計constraint_hand_num_max*constraint_hand_baton_num_max個のシーンノードをボーンにくっつけられる)

    u32  animation_slot_num;        // アニメーションを割り当てるスロットの個数
    u32  animation_blend_unit_num;  // スロット内でアニメーションのブレンドを行う場合2以上、行わない場合0か1(0でも1でもスロット内に1つユニットをつくります)
    u32  animation_num_in_unit;     // ユニット内で補間ブレンドしながらアニメーションを変更する場合2以上、行わない場合0か1(0でも1でもユニット内に1つつくります)

    u32  calculate_callback_num_max;    // 計算処理にて呼び出されるコールバックの最大数

    Description(void)
      : material_buffer_option(MATERIAL_BUFFER_OPTION_NOT_USE),
        material_share_source_model(NULL),
        mesh_node_visibility_share_source_model(NULL),
        max_callbacks(4),
        constraint_human_baton_num_max(0),
        constraint_hand_num_max(0),
        constraint_hand_baton_num_max(0),
        animation_slot_num(0),
        animation_blend_unit_num(0),
        animation_num_in_unit(0),
        calculate_callback_num_max(1)
    {
      // 何もしない
    }
  };
  static void InitializeDescriptionForDefault(
      Description*  description
  );

#if 0
    // フラグで|(or)で繋ぐのではユーザに親切ではない気がしたので、ビットフィールドで設定してもらうことにした。
    unsigned material_buffer_option_shader_parameter          : 1;  // シェーダーパラメータを利用するならば、１になります。
    unsigned material_buffer_option_shading_parameter         : 1;  // シェーディングパラメータを利用するならば、１になります。
    unsigned material_buffer_option_material_color            : 1;  // マテリアルカラーを利用するならば、１になります。
    unsigned material_buffer_option_rasterization             : 1;  // ラスタライゼーションを利用するならば、１になります。
    unsigned material_buffer_option_texture_coordinator       : 1;  // テクスチャコーディネータを利用するならば、１になります。
    unsigned material_buffer_option_texture_mapper            : 1;  // テクスチャマッパーを利用するならば、１になります。
    unsigned material_buffer_option_procedural_texture_mapper : 1;  // 未実装です。
    unsigned material_buffer_option_fragment_lighting         : 1;  // フラグメントライティングを利用するならば、１になります。
    unsigned material_buffer_option_fragment_lighting_table   : 1;  // フラグメントライティングテーブルを利用するならば、１になります。
    unsigned material_buffer_option_texture_combiner          : 1;  // テクスチャコンバイナを利用するならば、１になります。
    unsigned material_buffer_option_alpha_test                : 1;  // アルファテストを利用するならば、１になります。
    unsigned material_buffer_option_fragment_operation        : 1;  // フラグメントオペレーションを利用するならば、１になります。
    unsigned material_buffer_option_scene_environment         : 1;  // シーン環境を利用するならば、１になります。
    
    // ビットフィールドで1個ずつ指定していたのではユーザに親切ではない気がしたので、よく使うものをenumで用意しておくことにした。
#endif


public:
  // マテリアル用の定数
  enum
  {
    MATERIAL_INDEX_NOT_FOUND    = -1  // 指定した名前を持つマテリアルが存在しない場合のマテリアルのインデックス
  };
  // マテリアルのカリングモード
  enum MaterialCullingMode
  {
    MATERIAL_CULLING_MODE_FRONT     = nw::gfx::res::ResRasterization::CULLINGMODE_FRONT,  // 表面カリング
    MATERIAL_CULLING_MODE_BACK      = nw::gfx::res::ResRasterization::CULLINGMODE_BACK,   // 裏面カリング
    MATERIAL_CULLING_MODE_NONE      = nw::gfx::res::ResRasterization::CULLINGMODE_NONE    // 両面表示
  };
  // マテリアルのテスト関数
  enum MaterialTestFunction  // nw::gfx::res::ResStencilOperation::TestFuncやnw::gfx::res::ResDepthOperation::TestFunc, nw::gfx::res::ResAlphaTest::TestFuncと値を合わせておくこと
  {                          // 値とは参照値やデプスバッファの値のこと
    MATERIAL_TEST_FUNCTION_NEVER,        // すべて不通過です。
    MATERIAL_TEST_FUNCTION_ALWAYS,       // すべて通過です。
    MATERIAL_TEST_FUNCTION_EQUAL,        // 値と等しければ通過します。
    MATERIAL_TEST_FUNCTION_NOTEQUAL,     // 値と等しくなければ通過します。
    MATERIAL_TEST_FUNCTION_LESS,         // 値より小さければ通過します。
    MATERIAL_TEST_FUNCTION_LEQUAL,       // 値以下であれば通過します。
    MATERIAL_TEST_FUNCTION_GREATER,      // 値より大きければ通過します。
    MATERIAL_TEST_FUNCTION_GEQUAL        // 値以上であれば通過します。
  };
  // マテリアルのステンシルテスト後の処理
  enum MaterialStencilOperation
  {
    MATERIAL_STENCIL_OPERATION_KEEP           = nw::gfx::res::ResStencilOperation::STENCILOP_KEEP,           // 現在の値を保持します。
    MATERIAL_STENCIL_OPERATION_ZERO           = nw::gfx::res::ResStencilOperation::STENCILOP_ZERO,           // 0に置換します。 
    MATERIAL_STENCIL_OPERATION_REPLACE        = nw::gfx::res::ResStencilOperation::STENCILOP_REPLACE,        // 参照値に置換します。 
    MATERIAL_STENCIL_OPERATION_INCREASE       = nw::gfx::res::ResStencilOperation::STENCILOP_INCREASE,       // 増加させます。極値処理はクランプです。
    MATERIAL_STENCIL_OPERATION_DECREASE       = nw::gfx::res::ResStencilOperation::STENCILOP_DECREASE,       // 減少させます。極値処理はクランプです。
    MATERIAL_STENCIL_OPERATION_INVERT         = nw::gfx::res::ResStencilOperation::STENCILOP_INVERT,         // ビット単位で反転させます。 
    MATERIAL_STENCIL_OPERATION_INCREASE_WRAP  = nw::gfx::res::ResStencilOperation::STENCILOP_INCREASE_WRAP,  // 増加させます。極値処理は回り込みです。
    MATERIAL_STENCIL_OPERATION_DECREASE_WRAP  = nw::gfx::res::ResStencilOperation::STENCILOP_DECREASE_WRAP   // 減少させます。極値処理は回り込みです。 
  };
  // マテリアルのブレンダーのブレンドモード
#if 1
  enum MaterialBlendMode
  {
    MATERIAL_BLEND_MODE_NOT_USE       ,  // 使用しません。
    MATERIAL_BLEND_MODE_BLEND         ,  // ブレンド処理です。           // MATERIAL_BLEND_MODE_BLENDにしてもMATERIAL_BLEND_MODE_SEPARATE_BLENDが設定されます。
    MATERIAL_BLEND_MODE_SEPARATE_BLEND,  // セパレートブレンド処理です。
    MATERIAL_BLEND_MODE_LOGIC            // 論理演算です。
  };
#else
  /*
  NintendoWare\CTR\include\nw/gfx/res/gfx_ResMaterial.h
  NW_DEPRECATED_FUNCTION(void SetMode(Mode value))
  関数が廃止になったので値を使わないことにする
   */
  enum MaterialBlendMode
  {
    MATERIAL_BLEND_MODE_NOT_USE          = nw::gfx::res::ResBlendOperation::MODE_NOT_USE,         // 使用しません。 
    MATERIAL_BLEND_MODE_BLEND            = nw::gfx::res::ResBlendOperation::MODE_BLEND,           // ブレンド処理です。
    MATERIAL_BLEND_MODE_SEPARATE_BLEND   = nw::gfx::res::ResBlendOperation::MODE_SEPARATE_BLEND,  // セパレートブレンド処理です。
    MATERIAL_BLEND_MODE_LOGIC            = nw::gfx::res::ResBlendOperation::MODE_LOGIC            // 論理演算です。
  };
#endif
  // マテリアルのブレンダーの論理演算の演算方法
  enum MaterialBlendLogicOperation
  {
    MATERIAL_BLEND_LOGIC_OPERATION_CLEAR         = nw::gfx::res::ResBlendOperation::LOGICOP_CLEAR,          // すべてのビット値を 0 にします。 
    MATERIAL_BLEND_LOGIC_OPERATION_AND           = nw::gfx::res::ResBlendOperation::LOGICOP_AND,            // ソースカラーとディスティネーションカラーの論理積をとります。 
    MATERIAL_BLEND_LOGIC_OPERATION_AND_REVERSE   = nw::gfx::res::ResBlendOperation::LOGICOP_AND_REVERSE,    // ソースカラーとディスティネーションカラーの反転ビット値の論理積をとります。 
    MATERIAL_BLEND_LOGIC_OPERATION_COPY          = nw::gfx::res::ResBlendOperation::LOGICOP_COPY,           // ソースカラーのビット値をとります。 
    MATERIAL_BLEND_LOGIC_OPERATION_SET           = nw::gfx::res::ResBlendOperation::LOGICOP_SET,            // すべてのビット値を 1 にします。 
    MATERIAL_BLEND_LOGIC_OPERATION_COPY_INVERTED = nw::gfx::res::ResBlendOperation::LOGICOP_COPY_INVERTED,  // ソースカラーの反転ビット値をとります。 
    MATERIAL_BLEND_LOGIC_OPERATION_NOOP          = nw::gfx::res::ResBlendOperation::LOGICOP_NOOP,           // ディスティネーションカラーのビット値をとります。 
    MATERIAL_BLEND_LOGIC_OPERATION_INVERT        = nw::gfx::res::ResBlendOperation::LOGICOP_INVERT,         // ディスティネーションカラーの反転ビット値をとります。 
    MATERIAL_BLEND_LOGIC_OPERATION_NAND          = nw::gfx::res::ResBlendOperation::LOGICOP_NAND,           // ソースカラーとディスティネーションカラーの論理積の反転ビット値をとります。 
    MATERIAL_BLEND_LOGIC_OPERATION_OR            = nw::gfx::res::ResBlendOperation::LOGICOP_OR,             // ソースカラーとディスティネーションカラーの論理和をとります。 
    MATERIAL_BLEND_LOGIC_OPERATION_NOR           = nw::gfx::res::ResBlendOperation::LOGICOP_NOR,            // ソースカラーとディスティネーションカラーの論理和の反転ビット値をとります。 
    MATERIAL_BLEND_LOGIC_OPERATION_XOR           = nw::gfx::res::ResBlendOperation::LOGICOP_XOR,            // ソースカラーとディスティネーションカラーの排他的論理和をとります。 
    MATERIAL_BLEND_LOGIC_OPERATION_EQUIV         = nw::gfx::res::ResBlendOperation::LOGICOP_EQUIV,          // ソースカラーとディスティネーションカラーの排他的論理和の反転ビット値をとります。
    MATERIAL_BLEND_LOGIC_OPERATION_AND_INVERTED  = nw::gfx::res::ResBlendOperation::LOGICOP_AND_INVERTED,   // ソースカラーの反転ビット値と、ディスティネーションカラーの論理積をとります。 
    MATERIAL_BLEND_LOGIC_OPERATION_OR_REVERSE    = nw::gfx::res::ResBlendOperation::LOGICOP_OR_REVERSE,     // ソースカラーとディスティネーションカラーの反転ビット値の論理和をとります。 
    MATERIAL_BLEND_LOGIC_OPERATION_OR_INVERTED   = nw::gfx::res::ResBlendOperation::LOGICOP_OR_INVERTED     // ソースカラーの反転ビット値と、ディスティネーションカラーの論理和をとります。 
  };
  // マテリアルのブレンダーの入出力の要素
  enum MaterialBlendFactor
  {
    MATERIAL_BLEND_FACTOR_ZERO                     = nw::gfx::res::ResBlendOperation::BLENDFACTOR_ZERO,                      // 指定の成分に「 0 」を利用します。 
    MATERIAL_BLEND_FACTOR_ONE                      = nw::gfx::res::ResBlendOperation::BLENDFACTOR_ONE,                       // 指定の成分に「 1 」を利用します。 
    MATERIAL_BLEND_FACTOR_SRC_COLOR                = nw::gfx::res::ResBlendOperation::BLENDFACTOR_SRC_COLOR,                 // 指定の成分に「ソースカラー」を利用します。 
    MATERIAL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR      = nw::gfx::res::ResBlendOperation::BLENDFACTOR_ONE_MINUS_SRC_COLOR,       // 指定の成分に「1 - ソースカラー」 を利用します。 
    MATERIAL_BLEND_FACTOR_DST_COLOR                = nw::gfx::res::ResBlendOperation::BLENDFACTOR_DST_COLOR,                 // 指定の成分に「デスティネーションカラー」を利用します。 
    MATERIAL_BLEND_FACTOR_ONE_MINUS_DST_COLOR      = nw::gfx::res::ResBlendOperation::BLENDFACTOR_ONE_MINUS_DST_COLOR,       // 指定の成分に「 1 - デスティネーションカラー」を利用します。 
    MATERIAL_BLEND_FACTOR_SRC_ALPHA                = nw::gfx::res::ResBlendOperation::BLENDFACTOR_SRC_ALPHA,                 // 指定の成分に「ソースアルファ」を利用します。 
    MATERIAL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA      = nw::gfx::res::ResBlendOperation::BLENDFACTOR_ONE_MINUS_SRC_ALPHA,       // 指定の成分に「 1 - ソースアルファ」 を利用します。 
    MATERIAL_BLEND_FACTOR_DST_ALPHA                = nw::gfx::res::ResBlendOperation::BLENDFACTOR_DST_ALPHA,                 // 指定の成分に「デスティネーションアルファ」を利用します。 
    MATERIAL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA      = nw::gfx::res::ResBlendOperation::BLENDFACTOR_ONE_MINUS_DST_ALPHA,       // 指定の成分に「 1 - デスティネーションアルファ」を利用します。 
    MATERIAL_BLEND_FACTOR_CONSTANT_COLOR           = nw::gfx::res::ResBlendOperation::BLENDFACTOR_CONSTANT_COLOR,            // 指定の成分に「コンスタントカラー(ブレンドカラー)」を利用します。 
    MATERIAL_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR = nw::gfx::res::ResBlendOperation::BLENDFACTOR_ONE_MINUS_CONSTANT_COLOR,  // 指定の成分に「 1 - コンスタントカラー」を利用します。 
    MATERIAL_BLEND_FACTOR_CONSTANT_ALPHA           = nw::gfx::res::ResBlendOperation::BLENDFACTOR_CONSTANT_ALPHA,            // 指定の成分に「コンスタントアルファ(ブレンドアルファ)」を利用します。 
    MATERIAL_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA = nw::gfx::res::ResBlendOperation::BLENDFACTOR_ONE_MINUS_CONSTANT_ALPHA,  // 指定の成分に「 1 - コンスタントアルファ」を利用します。 
    MATERIAL_BLEND_FACTOR_SRC_ALPHA_SATURATE       = nw::gfx::res::ResBlendOperation::BLENDFACTOR_SRC_ALPHA_SATURATE,        // 指定の成分に「 MIN( ソースアルファ , 1 - デスティネーションアルファ ) 」を利用します。 
  };
  // マテリアルのブレンダーのブレンド式
  enum MaterialBlendEquation
  {
    MATERIAL_BLEND_EQUATION_FUNC_ADD              = nw::gfx::res::ResBlendOperation::BLENDEQ_FUNC_ADD,               // ソースとデスティネーションを加算します。 
    MATERIAL_BLEND_EQUATION_FUNC_SUBTRACT         = nw::gfx::res::ResBlendOperation::BLENDEQ_FUNC_SUBTRACT,          // ソースからデスティネーションを減算します。
    MATERIAL_BLEND_EQUATION_FUNC_REVERSE_SUBTRACT = nw::gfx::res::ResBlendOperation::BLENDEQ_FUNC_REVERSE_SUBTRACT,  // デスティネーションからソースを減算します。
    MATERIAL_BLEND_EQUATION_MIN                   = nw::gfx::res::ResBlendOperation::BLENDEQ_MIN,                    // ソースとデスティネーションのより小さい方です。
    MATERIAL_BLEND_EQUATION_MAX                   = nw::gfx::res::ResBlendOperation::BLENDEQ_MAX                     // ソースとデスティネーションのより大きい方です。
  };
  // マテリアルの描画レイヤー
  enum MaterialTranslucencyKind
  {
    MATERIAL_TRANSLUCENCY_KIND_LAYER0       = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_LAYER0,       // レイヤー 0 ( 不透明メッシュとして描画 ) です。 
    MATERIAL_TRANSLUCENCY_KIND_LAYER1       = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_LAYER1,       // レイヤー 2 ( 半透明メッシュとして描画 ) です。 
    MATERIAL_TRANSLUCENCY_KIND_LAYER2       = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_LAYER2,       // レイヤー 3 ( 減算合成メッシュとして描画 ) です 
    MATERIAL_TRANSLUCENCY_KIND_LAYER3       = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_LAYER3,       // レイヤー 4 ( 加算合成メッシュとして描画 ) です 
    MATERIAL_TRANSLUCENCY_KIND_OPAQUE       = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_OPAQUE,       // TRANSLUCENCY_KIND_LAYER0 と同じです。 
    MATERIAL_TRANSLUCENCY_KIND_TRANSLUCENT  = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_TRANSLUCENT,  // TRANSLUCENCY_KIND_LAYER1 と同じです。 
    MATERIAL_TRANSLUCENCY_KIND_SUBTRACTIVE  = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_SUBTRACTIVE,  // TRANSLUCENCY_KIND_LAYER2 と同じです。 
    MATERIAL_TRANSLUCENCY_KIND_ADDITIVE     = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_ADDITIVE,     // TRANSLUCENCY_KIND_LAYER3 と同じです。 
    MATERIAL_TRANSLUCENCY_KIND_END          = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_END           // 最後の描画レイヤーです。                       
  };
  // マテリアルのテクスチャの繰り返し設定
  enum MaterialTextureWrap
  {
    MATERIAL_TEXTURE_WRAP_CLAMP_TO_EDGE    = nw::gfx::res::ResStandardTextureSampler::WRAP_CLAMP_TO_EDGE,    // テクスチャの端のピクセルを引き伸ばします。 
    MATERIAL_TEXTURE_WRAP_CLAMP_TO_BORDER  = nw::gfx::res::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER,  // テクスチャの端でボーダーカラーを引き伸ばします。
    MATERIAL_TEXTURE_WRAP_REPEAT           = nw::gfx::res::ResStandardTextureSampler::WRAP_REPEAT,           // テクスチャの端で繰り返します。 
    MATERIAL_TEXTURE_WRAP_MIRRORED_REPEAT  = nw::gfx::res::ResStandardTextureSampler::WRAP_MIRRORED_REPEAT   // テクスチャの端で反転しながら繰り返します。 
  };

public:
  // メッシュ用の定数
  enum
  {
    MESH_INDEX_NOT_FOUND    = -1  // 指定した名前を持つメッシュが存在しない場合のメッシュのインデックス
  };

public:
  // ボーン用の定数
  enum
  {
        // BONE_INDEX_NOT_FOUNDもBONE_INDEX_NONEもインデックスがないことを言っているだけなので-1でいい。
    BONE_INDEX_NOT_FOUND  = -1,  // 指定した名前を持つボーンが存在しない場合のボーンのインデックス
    BONE_INDEX_NONE       = -1,  // インデックスなし
    BONE_INDEX_IGNORE     = -2   // 無視していいボーン
  };

public:
  // ユーザデータ用の定数
  enum
  {
    USER_DATA_INDEX_NOT_FOUND    = -1  // 指定した名前を持つユーザデータが存在しない場合のユーザデータのインデックス
  };

public:
#if 0
  //準備中
  // 計算されるかどうか
  enum CalculateState
  {
    CALCULATE_STATE_NO,  // 計算されない
    CALCULATE_STATE_OK   // 計算される
      // 一部計算される状態などが今後追加される予定
  };
#endif
  // 描画されるかどうか
  enum DrawState
  {
    DRAW_STATE_NO,  // 描画されない
    DRAW_STATE_OK   // 描画される
  };

private:
  // 共有タイプ
  enum ShareType
  {
    SHARE_TYPE_NONE,
    SHARE_TYPE_RESOURCE,           // リソースから生成し、共有していない(共有され共有元になったら共有タイプが変わります)
    SHARE_TYPE_SHARE_SOURCE,       // 共有しており、共有元である権利を持っている  // @note NW4Cのデモにマテリアル共有のサンプルあるか？
    SHARE_TYPE_SHARE_DESTINATION   // 共有しており、共有元を参照している
  };

private:
/*
差し替え
差し替えをreplaceと呼ぶことにする。
あるモデルの一部分として別のモデルを表示するような場合に使用する機能。使用例として着せ替えがあげられる。

[用語]
基本モデル  全くカスタマイズをしていないモデル。baseと呼ぶことにする。例：全身。
部分モデル  基本モデルの一部分を差し替えるためのモデル。partと呼ぶことにする。例：頭、ズボン。
追加モデル  他のモデルにくっつけるモデル。addと呼ぶことにする。SceneNode::SetupConstraintでくっつけるモデル。例：イヤリング、腕時計。

[計算の順序]
(1) partモデル専用のアニメ(スケルタルアニメでもマテリアルアニメでも何でもいい)の計算をする。
(2) baseモデルのアニメ(スケルタルアニメのみ)の計算結果を、partモデルに上書きする。
(3) partモデルにはあるがbaseモデルにはないボーンの位置を次のように修正する(スケルタルアニメのみ)。
    baseモデルで先祖にあたるボーンのトランスフォームで、partモデルにだけあるボーンの位置を親から順番に修正する。
*/
  // 差し替え用のボーンのデータ
  struct ReplaceBoneData
  {
    s32 base_index;           // これと同じボーンのbaseでのインデックス
    s32 base_ancestor_index;  // base_indexがgfl::grp::g3d::Model::BONE_INDEX_NONEのときのみ有効な値が入っており、それはbaseのボーンの中で先祖に当たるボーンのインデックス
                              // base_indexが0以上の値もしくはgfl::grp::g3d::Model::BONE_INDEX_IGNOREのとき、base_ancestor_indexには何が入っているか不明
        // (base_index==gfl::grp::g3d::Model::BONE_INDEX_NONE&&base_ancestor_index==gfl::grp::g3d::Model::BONE_INDEX_NONE)のとき、baseのhumanトップに追従する
        
    ReplaceBoneData(void)
      : base_index(gfl::grp::g3d::Model::BONE_INDEX_NONE),
        base_ancestor_index(gfl::grp::g3d::Model::BONE_INDEX_NONE)
    {}
  };

/*
昔はクラス内クラスとしてAnimationSlotを用意していた。
private:
  //-------------------------------------
  ///	アニメーションを割り当てるスロット
  //=====================================
  class AnimationSlot
  {
  };
*/

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 Createは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  Model(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   *                 Destroyを呼びます。
   */
  //-----------------------------------------------------------------------------
  virtual ~Model();
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *
   *  @param[in]     allocator          アロケータ
   *  @param[in]     device_allocator   デバイスメモリのアロケータ
   *  @param[in]     resource           リソース
   *  @param[in]     index_in_resource  リソース内におけるインデックス
   *  @param[in]     description        設定内容
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*           allocator,
      gfl::heap::NwAllocator*           device_allocator,
      Resource*                         resource,
      s32                               index_in_resource,
      Description*                      description  = NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         スケルトンを共有したモデルの生成
   *
   *  @param[in]     allocator                   アロケータ
   *  @param[in]     device_allocator            デバイスメモリのアロケータ
   *  @param[in]     resource                    リソース
   *  @param[in]     index_in_resource           リソース内におけるインデックス
   *  @param[in]     skeleton_share_source_model スケルトンの共有元のモデル
   *  @param[in]     description                 設定内容
   */
  //-----------------------------------------------------------------------------
  virtual void CreateSkeletalModelSharingSkeleton(
      gfl::heap::NwAllocator*           allocator,
      gfl::heap::NwAllocator*           device_allocator,
      Resource*                         resource,
      s32                               index_in_resource,
      Model*                            skeleton_share_source_model,
      Description*                      description  = NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         破棄
   *                 何回呼んでも大丈夫。デストラクタは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         計算処理
   */
  //-----------------------------------------------------------------------------
  virtual void Calculate(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         このクラスが保持しているnw::gfx::Model*を取得
   *
   *  @retval        nw::gfx::Model*    このクラスが保持しているnw::gfx::Model
   */
  //-----------------------------------------------------------------------------
  nw::gfx::Model*  GetNwModel(void) const  {  return nw::ut::DynamicCast<nw::gfx::Model*>( GetNwSceneNode() );  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         座標変換
   */
  //-----------------------------------------------------------------------------
  void SetTransformMatrix(const gfl::math::MTX34& m);
  void SetTransformMatrix(
      f32 m00,
      f32 m01,
      f32 m02,
      f32 m03,
      f32 m10,
      f32 m11,
      f32 m12,
      f32 m13,
      f32 m20,
      f32 m21,
      f32 m22,
      f32 m23
  );
  void GetTransformMatrix(gfl::math::MTX34* m) const;  // 自分のスケールは掛かっておらず、親のスケールは掛かっています。
  gfl::math::MTX34 GetTransformMatrix(void) const;  // 自分のスケールは掛かっておらず、親のスケールは掛かっています。
  //-----------------------------------------------------------------------------
  /**
   *  @brief         平行移動
   */
  //-----------------------------------------------------------------------------
  void SetTranslate(f32 x, f32 y, f32 z);
  void SetTranslate(const gfl::math::VEC3& t);
  void GetTranslate(gfl::math::VEC3* t) const;
  gfl::math::VEC3 GetTranslate(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         回転
   *                 単位はラジアン。
   */
  //-----------------------------------------------------------------------------
  void SetRotateXYZ(f32 x, f32 y, f32 z);
  void SetRotateXYZ(const gfl::math::VEC3& t);
  void GetRotateXYZ(gfl::math::VEC3* t) const;  // 【注意】SetRotateXYZで渡された値を返すだけです。SetRotateXYZ以外で回転を指定していた場合は
  gfl::math::VEC3 GetRotateXYZ(void) const;     //         見た目と合わない値が返ってきます。
  template <typename TMatrix> void SetRotateMatrix(const TMatrix& m)
  {
    nw::gfx::TransformNode* transform_node = GetNwTransformNode();
    transform_node->Transform().SetRotateMatrix(m);
  }
  void SetRotateMatrix(
      f32 m00,
      f32 m01,
      f32 m02,
      f32 m10,
      f32 m11,
      f32 m12,
      f32 m20,
      f32 m21,
      f32 m22
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         スケール
   */
  //-----------------------------------------------------------------------------
  void SetScale(f32 x, f32 y, f32 z);
  void SetScale(const gfl::math::VEC3& t);
  void GetScale(gfl::math::VEC3* t) const;
  gfl::math::VEC3 GetScale(void) const;

#if 0
//  他のモデルに移した後の動作がよくないので、この関数はもっとテストしてからでないと使えない。
  //-----------------------------------------------------------------------------
  /**
   *  @brief         スケルトンの共有元である権利を他のモデルに移す
   *                 thisがスケルトンの共有元であり、
   *                 skeleton_share_new_source_modelがそのスケルトンを共有しているモデルである必要があります。
   *
   *  @param[in]     skeleton_share_new_source_model  新しくスケルトンの共有元の権利を与えられるモデル
   */
  //-----------------------------------------------------------------------------
  void ChangeSkeletonShareSource(Model* skeleton_share_new_source_model);
#endif

  //-----------------------------------------------------------------------------
  /**
   *  @brief         描画順
   *                 プログラマだけが設定できるモデルのレイヤーID
   *                 最初に描画 0 <= u8 <= 127 最後に描画
   *                 最優先される描画順 ModelLayerId > MaterialTranslucencyKind > その他 最劣後される描画順
   *
   *                 **** ModelとParticleにあり全部を一列に並べた統一番号です。デフォルト0000000です。 ****
   */
  //-----------------------------------------------------------------------------
  void SetModelLayerId(u8 model_layer_id);
  u8 GetModelLayerId(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         マテリアル
   */
  //-----------------------------------------------------------------------------
  s32 GetMaterialCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         マテリアルのインデックス番号を取得する
   *
   *  @param[in]     key  マテリアルの名前
   * 
   *  @retval        s32    マテリアルのインデックス番号
   *                        指定した名前を持つマテリアルが存在しない場合、MATERIAL_INDEX_NOT_FOUNDを返す
   */
  //-----------------------------------------------------------------------------
  s32 GetMaterialIndex(const char* key) const;
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         マテリアルカラー
   *                 u8のときは0が黒&透明、255が白&不透明
   *                 f32のときは0.0fが黒&透明、1.0fが白&不透明
   */
  //-----------------------------------------------------------------------------
  // ディフューズカラー
  void SetMaterialColorDiffuse(s32 material_index, f32 r, f32 g, f32 b);
  void SetMaterialColorDiffuse(s32 material_index, f32 r, f32 g, f32 b, f32 a);
  void SetMaterialColorDiffuse(s32 material_index, const gfl::grp::ColorF32& c);
  void SetMaterialColorDiffuse(s32 material_index, u8 r, u8 g, u8 b);
  void SetMaterialColorDiffuse(s32 material_index, u8 r, u8 g, u8 b, u8 a);
  void SetMaterialColorDiffuse(s32 material_index, const gfl::grp::ColorU8& c);
  gfl::grp::ColorF32 GetMaterialColorDiffuse(s32 material_index) const;
  void GetMaterialColorDiffuse(s32 material_index, gfl::grp::ColorF32* c) const;
  void GetMaterialColorDiffuse(s32 material_index, gfl::grp::ColorU8*  c) const;

  // コンスタントカラー
  void SetMaterialColorConstant(s32 material_index, s32 no, f32 r, f32 g, f32 b);
  void SetMaterialColorConstant(s32 material_index, s32 no, f32 r, f32 g, f32 b, f32 a);
  void SetMaterialColorConstant(s32 material_index, s32 no, const gfl::grp::ColorF32& c);
  void SetMaterialColorConstant(s32 material_index, s32 no, u8 r, u8 g, u8 b);
  void SetMaterialColorConstant(s32 material_index, s32 no, u8 r, u8 g, u8 b, u8 a);
  void SetMaterialColorConstant(s32 material_index, s32 no, const gfl::grp::ColorU8& c);
  gfl::grp::ColorF32 GetMaterialColorConstant(s32 material_index, s32 no) const;
  void GetMaterialColorConstant(s32 material_index, s32 no, gfl::grp::ColorF32* c) const;
  void GetMaterialColorConstant(s32 material_index, s32 no, gfl::grp::ColorU8*  c) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         マテリアルが影響を受けるフォグ
   */
  //-----------------------------------------------------------------------------
  void SetMaterialFogEnable(s32 material_index, bool enable);
  bool IsMaterialFogEnable(s32 material_index) const;
  void SetMaterialFogIndex(s32 material_index, s32 index);
  s32  GetMaterialFogIndex(s32 material_index) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         マテリアルが影響を受けるライト
   */
  //-----------------------------------------------------------------------------
  //void SetMaterialFragmentLightEnable(s32 material_index, bool enable);
  //bool IsMaterialFragmentLightEnable(s32 material_index) const;
  //void SetMaterialVertexLightEnable(s32 material_index, bool enable);
  //bool IsMaterialVertexLightEnable(s32 material_index) const;
  //void SetMaterialHemiSphereLightEnable(s32 material_index, bool enable, bool oclusion_enable);
  //bool IsMaterialHemiSphereLightEnable(s32 material_index, bool* oclusion_enable) const;
  void SetMaterialLightSetIndex(s32 material_index, s32 index);
  s32  GetMaterialLightSetIndex(s32 material_index) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         マテリアルのカリングモード
   */
  //-----------------------------------------------------------------------------
  void SetMaterialCullingMode(s32 material_index, MaterialCullingMode mode);
  MaterialCullingMode GetMaterialCullingMode(s32 material_index) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         マテリアルのアルファテスト
   */
  //-----------------------------------------------------------------------------
  bool IsMaterialAlphaTestEnable(s32 material_index) const; 
  void SetMaterialAlphaTestEnable(s32 material_index, bool enable);
  MaterialTestFunction GetMaterialAlphaTestFunction(s32 material_index) const;
  void SetMaterialAlphaTestFunction(s32 material_index, MaterialTestFunction function);
  f32 GetMaterialAlphaTestReference(s32 material_index) const;
  void SetMaterialAlphaTestReference(s32 material_index, f32 reference);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         マテリアルのデプステスト
   */
  //-----------------------------------------------------------------------------
  bool IsMaterialDepthTestEnable(s32 material_index) const;          // DepthOperation IsTestEnabled
  void SetMaterialDepthTestEnable(s32 material_index, bool enable);  // trueのときデプステスト有効
  bool IsMaterialDepthTestMaskEnable(s32 material_index) const;          // DepthOperation IsMaskEnabled
  void SetMaterialDepthTestMaskEnable(s32 material_index, bool enable);  // trueのときデプスマスク有効(trueのときデプスバッファを更新する)
  MaterialTestFunction GetMaterialDepthTestFunction(s32 material_index) const;  // DepthOperation TestFunction
  void SetMaterialDepthTestFunction(s32 material_index, MaterialTestFunction function);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         マテリアルのステンシルテスト
   */
  //-----------------------------------------------------------------------------
  bool IsMaterialStencilTestEnable(s32 material_index) const;
  void SetMaterialStencilTestEnable(s32 material_index, bool enable);
  MaterialTestFunction GetMaterialStencilTestFunction(s32 material_index) const;
  void SetMaterialStencilTestFunction(s32 material_index, MaterialTestFunction function);
  s32 GetMaterialStencilTestReference(s32 material_index) const;
  void SetMaterialStencilTestReference(s32 material_index, s32 value);
  u32 GetMaterialStencilTestMask(s32 material_index) const;
  void SetMaterialStencilTestMask(s32 material_index, u32 value);
  MaterialStencilOperation GetMaterialStencilTestFailOperation(s32 material_index) const;
  void SetMaterialStencilTestFailOperation(s32 material_index, MaterialStencilOperation operation);
  MaterialStencilOperation GetMaterialStencilTestZFailOperation(s32 material_index) const;
  void SetMaterialStencilTestZFailOperation(s32 material_index, MaterialStencilOperation operation);
  MaterialStencilOperation GetMaterialStencilTestPassOperation(s32 material_index) const;
  void SetMaterialStencilTestPassOperation(s32 material_index, MaterialStencilOperation operation);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         マテリアルのブレンダー
   */
  //-----------------------------------------------------------------------------
  MaterialBlendMode GetMaterialBlendMode(s32 material_index) const;
  void SetMaterialBlendMode(s32 material_index, MaterialBlendMode mode);

  void SetMaterialBlendColor(s32 material_index, f32 r, f32 g, f32 b);
  void SetMaterialBlendColor(s32 material_index, f32 r, f32 g, f32 b, f32 a);
  void SetMaterialBlendColor(s32 material_index, const gfl::grp::ColorF32& c);
  void SetMaterialBlendColor(s32 material_index, u8 r, u8 g, u8 b);
  void SetMaterialBlendColor(s32 material_index, u8 r, u8 g, u8 b, u8 a);
  void SetMaterialBlendColor(s32 material_index, const gfl::grp::ColorU8& c);
  gfl::grp::ColorF32 GetMaterialBlendColor(s32 material_index) const;
  void GetMaterialBlendColor(s32 material_index, gfl::grp::ColorF32* c) const;
  void GetMaterialBlendColor(s32 material_index, gfl::grp::ColorU8*  c) const;
  
  MaterialBlendLogicOperation GetMaterialBlendLogicOperation(s32 material_index) const;
  void SetMaterialBlendLogicOperation(s32 material_index, MaterialBlendLogicOperation operation);

  MaterialBlendFactor GetMaterialBlendFunctionSourceRgb(s32 material_index) const;
  void SetMaterialBlendFunctionSourceRgb(s32 material_index, MaterialBlendFactor factor);
  MaterialBlendFactor GetMaterialBlendFunctionSourceAlpha(s32 material_index) const;
  void SetMaterialBlendFunctionSourceAlpha(s32 material_index, MaterialBlendFactor factor);

  MaterialBlendFactor GetMaterialBlendFunctionDestinationRgb(s32 material_index) const;
  void SetMaterialBlendFunctionDestinationRgb(s32 material_index, MaterialBlendFactor factor);
  MaterialBlendFactor GetMaterialBlendFunctionDestinationAlpha(s32 material_index) const;
  void SetMaterialBlendFunctionDestinationAlpha(s32 material_index, MaterialBlendFactor factor);

  MaterialBlendEquation GetMaterialBlendEquationRgb(s32 material_index) const;
  void SetMaterialBlendEquationRgb(s32 material_index, MaterialBlendEquation equation);
  MaterialBlendEquation GetMaterialBlendEquationAlpha(s32 material_index) const;
  void SetMaterialBlendEquationAlpha(s32 material_index, MaterialBlendEquation equation);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         描画順
   *                 透明性の種類(描画レイヤー)
   *                 デザイナがエディタ上で設定できるマテリアルの描画レイヤー
   *                 最初に描画 MATERIAL_TRANSLUCENCY_KIND_LAYER0 <= MaterialTranslucencyKind <= MATERIAL_TRANSLUCENCY_KIND_LAYER3 最後に描画
   *                 最優先される描画順 ModelLayerId > MaterialTranslucencyKind > その他 最劣後される描画順
   */
  //-----------------------------------------------------------------------------
  MaterialTranslucencyKind GetMaterialTranslucencyKind(s32 material_index) const;
  void SetMaterialTranslucencyKind(s32 material_index, MaterialTranslucencyKind kind);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         マテリアルのテクスチャ
   */
  //-----------------------------------------------------------------------------
public:
  // material_index番マテリアルのno番テクスチャ(0<=no<=2)にtexture_resource_packで指すテクスチャを設定する。
  // 元々貼ってあったテクスチャはそのリソースを解放したときに解放されるので、差し替えられてもSetupされたままの状態で残っています。
  void SetMaterialTextureMapperTexture(s32 material_index, s32 no, Resource::TextureResourcePack* texture_resource_pack);
  // material_index番マテリアルのno番テクスチャ(0<=no<=2)にtexture_resourceで指すテクスチャを設定する。
  // 元々貼ってあったテクスチャはそのリソースを解放したときに解放されるので、差し替えられてもSetupされたままの状態で残っています。
  void SetMaterialTextureMapperTexture(s32 material_index, s32 no, Resource* texture_resource, s32 index_in_resource);
  // material_index番マテリアルのno番テクスチャ(0<=no<=2)にtextureで指すテクスチャを設定する。
  // 元々貼ってあったテクスチャはそのリソースを解放したときに解放されるので、差し替えられてもSetupされたままの状態で残っています。
  void SetMaterialTextureMapperTexture(s32 material_index, s32 no, Texture* texture);
private:
  void setMaterialTextureMapperTexture(s32 material_index, s32 no, nw::gfx::ResTexture res_texture);

public:
  // material_index番マテリアルのno番テクスチャ(0<=no<=2)のU方向繰り返し設定を行う。
  void SetMaterialTextureMapperWrapU(s32 material_index, s32 no, MaterialTextureWrap wrap);
  // material_index番マテリアルのno番テクスチャ(0<=no<=2)のU方向繰り返し設定を得る。
  MaterialTextureWrap  GetMaterialTextureMapperWrapU(s32 material_index, s32 no) const;
  // material_index番マテリアルのno番テクスチャ(0<=no<=2)のV方向繰り返し設定を行う。
  void SetMaterialTextureMapperWrapV(s32 material_index, s32 no, MaterialTextureWrap wrap);
  // material_index番マテリアルのno番テクスチャ(0<=no<=2)のV方向繰り返し設定を得る。
  MaterialTextureWrap  GetMaterialTextureMapperWrapV(s32 material_index, s32 no) const;
  // テクスチャの座標は、STは使わず、UVで統一しておく。

  // material_index番マテリアルのno番テクスチャコーディネータ(0<=no<=2)の設定
  // texture_mapper_no番テクスチャマッパーとtexture_coordinator_no番テクスチャコーディネータがあった場合、
  // 「2番テクスチャマッパーなら2番テクスチャコーディネータを使う」というふうに必ずしもなっているわけではない。
  // 「1番テクスチャマッパーと2番テクスチャマッパーが1番テクスチャコーディネータを共有して使う」ということもあり得る。
  // つまりtexture_mapper_no=texture_coordinator_noと必ずなるわけではない、ということに注意して下さい。
  void SetMaterialTextureCoordinatorScaleUV(s32 material_index, s32 no, const gfl::math::VEC2& t);
  void GetMaterialTextureCoordinatorScaleUV(s32 material_index, s32 no, gfl::math::VEC2* t) const;
  
  void SetMaterialTextureCoordinatorRotateUV(s32 material_index, s32 no, const f32 t);  // [radian]
  f32 GetMaterialTextureCoordinatorRotateUV(s32 material_index, s32 no) const;  // [radian]
  
  void SetMaterialTextureCoordinatorTranslateUV(s32 material_index, s32 no, const gfl::math::VEC2& t);
  void GetMaterialTextureCoordinatorTranslateUV(s32 material_index, s32 no, gfl::math::VEC2* t) const;

  void SetMaterialTextureCoordinatorReferenceSceneCameraIndex(s32 material_index, s32 no, s32 scene_camera_index);
  s32  GetMaterialTextureCoordinatorReferenceSceneCameraIndex(s32 material_index, s32 no) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         メッシュ
   */
  //-----------------------------------------------------------------------------
  s32 GetMeshCount() const;
  s32 GetMeshIndex(const char* key) const;
  b32 IsMeshVisible(const s32 mesh_index) const;
  void SetMeshVisible(const s32 mesh_index, const b32 visible);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ユーザデータ
   */
  //-----------------------------------------------------------------------------
  // モデル
  s32 GetModelUserDataCount() const;
  s32 GetModelUserDataIndex(const char* key) const;
  void GetModelUserData(const s32 user_data_index, UserData* user_data) const;
  // マテリアル
  s32 GetMaterialUserDataCount(const s32 material_index) const;
  s32 GetMaterialUserDataIndex(const s32 material_index, const char* key) const;
  void GetMaterialUserData(const s32 material_index, const s32 user_data_index, UserData* user_data) const;
  // メッシュ
  s32 GetMeshUserDataCount(const s32 mesh_index) const;
  s32 GetMeshUserDataIndex(const s32 mesh_index, const char* key) const;
  void GetMeshUserData(const s32 mesh_index, const s32 user_data_index, UserData* user_data) const;
  
  bool GetModelAABB( gfl::math::AABB *p_dest_aabb ) const ;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         計算処理にて呼び出されるコールバック
   */
  //-----------------------------------------------------------------------------
  enum
  {
    CALLBACK_INDEX_NOT_ADD   = -1    // コールバックを追加していないとき
  };
  // 途中でコールバックをなしにする場合は、戻り値を覚えておいて下さい。
  // コールバックを追加できなかったときCALLBACK_INDEX_NOT_ADDを返す。
  s32 AddCalculateCallback(ModelCalculateCallbackFunction function, void* work=NULL);
  // 引数calculate_callback_indexにはAddCalculateCallbackの戻り値を渡して下さい。
  void RemoveCalculateCallback(s32 calculate_callback_index);


#if 0
  //準備中
  //-----------------------------------------------------------------------------
  /**
   *  @brief         計算フラグ
   *                 trueのとき計算される(@note 全部？一部？)
   *                 ただしこれがtrueのときでも、GetTreeFlagがfalseのときは計算されない
   */
  //-----------------------------------------------------------------------------
  void SetCalculateFlag(b32 flag);
  b32  GetCalculateFlag(void) const;
#endif

  //-----------------------------------------------------------------------------
  /**
   *  @brief         描画フラグ
   *                 trueのとき描画される
   *                 ただしこれがtrueのときでも、GetTreeFlagがfalseのときは描画されない
   */
  //-----------------------------------------------------------------------------
  virtual void SetVisible(b32 visible);
  virtual b32  IsVisible(void) const;

#if 0
  //SceneNodeへ移行した
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーンツリーフラグ
   *                 trueのときSceneにAddされていたらシーンツリーに入れられる、falseのときSceneにAddされていてもシーンツリーから外される
   *                 これがtrueであることと、ユーザがSceneにAddしたことは、同じではありません。
   */
  //-----------------------------------------------------------------------------
  void SetTreeFlag(b32 flag);
  b32  GetTreeFlag(void) const;
#endif

#if 0
  //準備中
  //-----------------------------------------------------------------------------
  /**
   *  @brief         計算されるかどうか
   */
  //-----------------------------------------------------------------------------
  CalculateState GetCalculateState(void) const;
#endif

  //-----------------------------------------------------------------------------
  /**
   *  @brief         描画されるかどうか
   */
  //-----------------------------------------------------------------------------
  DrawState GetDrawState(void) const;


  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションを割り当てるスロットが何個用意されているか
   *
   *  @retval        u32    用意されているスロットの個数
   */
  //-----------------------------------------------------------------------------
  u32 GetAnimationSlotNum(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションを割り当てるスロットにアニメーションが割り当てられているか
   *
   *  @param[in]     slot_index    アニメーションを割り当てるスロットのインデックス
   *
   *  @retval        b32    アニメーションが割り当てられていたらtrue
   */
  //-----------------------------------------------------------------------------
  b32 IsAnimationSlotHavingAnimation(u32 slot_index) const;  // havingは英語として気になる


  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションスロットの有効/無効を取得する
   *
   *  @param[in]     slot_index    アニメーションを割り当てるスロットのインデックス
   *
   *  @retval        b32    アニメーションスロットが有効ならtrue
   */
  //-----------------------------------------------------------------------------
  b32  IsAnimationSlotEnable(u32 slot_index) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションスロットの有効/無効を設定する
   *
   *  @param[in]     slot_index    アニメーションを割り当てるスロットのインデックス
   *  @param[in]     enable        アニメーションスロットを有効にするならtrue
   */
  //-----------------------------------------------------------------------------
  void SetAnimationSlotEnable(u32 slot_index, b32 enable);


  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションを変更する
   *                 slot_indexで指定したスロットに対して初めてこの関数を呼んだときは、対象スロットにAnimationを生成する。
   *                 slot_indexで指定したスロットに対して初めてchange_frame>0でこの関数を呼んだときも、対象スロットにAnimationを生成する。
   *
   *  @param[in]     allocator               アロケータ
   *  @param[in]     res_anim                アニメーションリソース(NULLを渡すと全て(スケルタル、マテリアル、ビジビリティ)のアニメーションを解除します)
   *  @param[in]     change_frame            何フレームで変更するか(この間ブレンドされる)
   *  @param[in]     slot_index              アニメーションを割り当てるスロットのインデックス
   *  @param[in]     max_anim_members        アニメーションするメンバの最大数。(res_animがNULLのときは使用されないのでてきとーな値でOK)
   *                                         slot_indexで指定したスロットに対して初めてこの関数を呼んだときは、使用される。
   *                                         slot_indexで指定したスロットに対して初めてchange_frame>0でこの関数を呼んだときも、使用される。
   *                                         Resource::Get???MemberAnimSetCountで得た値を渡すとよい。
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRSTを渡すと、
   *                                         最初にChangeで渡したアニメーションリソースのGetMemberAnimSetCountを使用します。
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERSを渡すと、
   *                                         最初にChangeで渡したアニメーションリソースのアニメーショングループのメンバ数に、
   *                                         即ち、そのアニメーションタイプにおける最大数にします。
   *  @param[in]     force                   強制的にアニメーションを関連付けます。(res_animがNULLのときは使用されないのでてきとーな値でOK)
   */
  //-----------------------------------------------------------------------------
  virtual void ChangeAnimation(
    gfl::heap::NwAllocator* allocator,
    ResAnim*                res_anim,
    u32                     change_frame = 0,
    u32                     slot_index = 0,
    s32                     max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST,
    bool                    force = false
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションを変更する
   *                 slot_indexで指定したスロットに対して初めてこの関数を呼んだときは、対象スロットにAnimationを生成する。
   *                 slot_indexで指定したスロットに対して初めてchange_frame>0でこの関数を呼んだときも、対象スロットにAnimationを生成する。
   *
   *  @param[in]     allocator               アロケータ
   *  @param[in]     resource                リソース(NULLを渡すと全て(スケルタル、マテリアル、ビジビリティ)のアニメーションを解除します)
   *  @param[in]     animation_resource_type アニメーションリソースのタイプ(resourceがNULLのときは使用されないのでてきとーな値でOK)
   *  @param[in]     index_in_resource       リソース内におけるインデックス(resourceがNULLのときは使用されないのでてきとーな値でOK)
   *  @param[in]     change_frame            何フレームで変更するか(この間ブレンドされる)
   *  @param[in]     slot_index              アニメーションを割り当てるスロットのインデックス
   *  @param[in]     max_anim_members        アニメーションするメンバの最大数。(resourceがNULLのときは使用されないのでてきとーな値でOK)
   *                                         slot_indexで指定したスロットに対して初めてこの関数を呼んだときは、使用される。
   *                                         slot_indexで指定したスロットに対して初めてchange_frame>0でこの関数を呼んだときも、使用される。
   *                                         Resource::Get???MemberAnimSetCountで得た値を渡すとよい。
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRSTを渡すと、
   *                                         最初にChangeで渡したアニメーションリソースのGetMemberAnimSetCountを使用します。
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERSを渡すと、
   *                                         最初にChangeで渡したアニメーションリソースのアニメーショングループのメンバ数に、
   *                                         即ち、そのアニメーションタイプにおける最大数にします。
   *  @param[in]     force                   強制的にアニメーションを関連付けます。(resourceがNULLのときは使用されないのでてきとーな値でOK)
   */
  //-----------------------------------------------------------------------------
/**
  そのうちこうするかも。
   *  @param[in]     resource                リソース(NULLを渡すとアニメーションを解除します)
   *  @param[in]     animation_resource_type アニメーションリソースのタイプ(resourceがNULLのときのみANIMATION_RESOURCE_TYPE_ALLを渡してよい。
   *                                         ANIMATION_RESOURCE_TYPE_ALLのとき全て(スケルタル、マテリアル、ビジビリティ)のアニメーションを解除します)
*/
  void ChangeAnimation(
    gfl::heap::NwAllocator*          allocator,
    Resource*                        resource,
    Resource::AnimationResourceType  animation_resource_type,
    s32                              index_in_resource,
    u32                              change_frame = 0,
    u32                              slot_index = 0,
    s32                              max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST,
    bool                             force = false
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         slot_indexのunit_indexに割り当てるアニメーションを変更する
   *
   *  @param[in]     heap_allocator          ヒープアロケータ
   *  @param[in]     device_allocator        デバイスアロケータ
   *  @param[in]     slot_index              スロットのインデックス
   *  @param[in]     unit_index              スロット内につくったユニットのインデックス
   *  @param[in]     res_anim                アニメーションリソース(NULLを渡すと全て(スケルタル、マテリアル、ビジビリティ)のアニメーションを解除します)
   *  @param[in]     resource                リソース(NULLを渡すと全て(スケルタル、マテリアル、ビジビリティ)のアニメーションを解除します)
   *  @param[in]     animation_resource_type アニメーションリソースのタイプ(resourceがNULLのときは使用されないのでてきとーな値でOK)
   *  @param[in]     index_in_resource       リソース内におけるインデックス(resourceがNULLのときは使用されないのでてきとーな値でOK)
   *  @param[in]     change_frame            何フレームで変更するか(この間ブレンドされる)
   *  @param[in]     max_anim_members        アニメーションするメンバの最大数。(res_animがNULLのときは使用されないのでてきとーな値でOK)
   *                                         Resource::Get???MemberAnimSetCountで得た値を渡すとよい。
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRSTを渡すと、
   *                                         最初にChangeで渡したアニメーションリソースのGetMemberAnimSetCountを使用します。
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERSを渡すと、
   *                                         最初にChangeで渡したアニメーションリソースのアニメーショングループのメンバ数に、
   *                                         即ち、そのアニメーションタイプにおける最大数にします。
   *  @param[in]     force                   強制的にアニメーションを関連付けます。(res_animがNULLのときは使用されないのでてきとーな値でOK)
   */
  //-----------------------------------------------------------------------------
  void ChangeUnitAnimation(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    u32                     slot_index,
    u32                     unit_index,
    ResAnim*                res_anim,
    u32                     change_frame,
    s32                     max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST,
    b32                     force = false
  );
  void ChangeUnitAnimation(
    gfl::heap::NwAllocator*          heap_allocator,
    gfl::heap::NwAllocator*          device_allocator,
    u32                              slot_index,
    u32                              unit_index,
    Resource*                        resource,
    Resource::AnimationResourceType  animation_resource_type,
    s32                              index_in_resource,
    u32                              change_frame,
    s32                              max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST,
    b32                              force = false
  );


  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションのフレームを設定する
   *
   *  @param[in]     frame           フレーム
   *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationFrame(f32 frame, u32 slot_index = 0);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションのフレームを取得する
   *
   *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
   * 
   *  @retval        フレーム
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationFrame(u32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの更新フレームを設定する
   *                 0.0fで停止、1.0fで等倍、2.0fで2倍速、-1.0fで逆等倍再生
   *
   *  @param[in]     step_frame      更新フレーム
   *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationStepFrame(f32 step_frame, u32 slot_index = 0);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの更新フレームを取得する
   *
   *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
   * 
   *  @retval        更新フレーム
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationStepFrame(u32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの開始フレームを設定する
   *
   *  @param[in]     frame           フレーム
   *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationStartFrame(f32 frame, u32 slot_index = 0);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの開始フレームを取得する
   *
   *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
   *
   *  @retval        フレーム
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationStartFrame(u32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの終了フレームを設定する
   *
   *  @param[in]     frame           フレーム
   *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationEndFrame(f32 frame, u32 slot_index = 0);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの終了フレームを取得する
   *
   *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
   *
   *  @retval        フレーム
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationEndFrame(u32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの開始フレームと終了フレームを設定する
   *
   *  @param[in]     start_frame     開始フレーム
   *  @param[in]     end_frame       終了フレーム
   *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationStartAndEndFrame(f32 start_frame, f32 end_frame, u32 slot_index = 0);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの現在のフレームが開始フレームか否か判定する
   *
   *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
   *
   *  @retval        bool    現在のフレームが開始フレームの場合tureを返す
   */
  //-----------------------------------------------------------------------------
  virtual bool IsAnimationFrameStartFrame(u32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの現在のフレームが終了フレームか否か判定する
   *
   *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
   *
   *  @retval        bool    現在のフレームが終了フレームの場合tureを返す
   */
  //-----------------------------------------------------------------------------
  virtual bool IsAnimationFrameEndFrame(u32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの現在のフレームが
   *                     step_frame>0のときは終了フレームのときtrueを返す
   *                     step_frame<0のときは開始フレームのときtrueを返す
   *                     step_frame==0のときは開始フレームか終了フレームのときtrueを返し、それ以外のときfalseを返す
   *
   *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
   *
   *  @retval        bool    現在のフレームがstep_frameから見て端フレームとなっている場合tureを返す
   */
  //-----------------------------------------------------------------------------
  virtual bool IsAnimationFrameStepTerminalFrame(u32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションがループ再生か否か取得する
   *
   *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
   *
   *  @retval        bool            ループのときtrue、ループでないときfalseを返す
   */
  //-----------------------------------------------------------------------------
  virtual bool IsAnimationLoop(u32 slot_index = 0) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションにループ再生を設定する
   *
   *  @param[in]     loop            ループのときtrue、ループでないときfalseを設定する
   *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationLoop(bool loop, u32 slot_index = 0);


  //-----------------------------------------------------------------------------
  /**
   *  @brief         フレーム操作
   */
  //-----------------------------------------------------------------------------
  void SetUnitFrame(u32 slot_index, u32 unit_index, f32 frame);
  f32  GetUnitFrame(u32 slot_index, u32 unit_index) const;
  void SetUnitStepFrame(u32 slot_index, u32 unit_index, f32 step_frame);
  f32  GetUnitStepFrame(u32 slot_index, u32 unit_index) const;
  void SetUnitStartFrame(u32 slot_index, u32 unit_index, f32 frame);
  f32  GetUnitStartFrame(u32 slot_index, u32 unit_index) const;
  void SetUnitEndFrame(u32 slot_index, u32 unit_index, f32 frame);
  f32  GetUnitEndFrame(u32 slot_index, u32 unit_index) const;
  void SetUnitStartAndEndFrame(u32 slot_index, u32 unit_index, f32 start_frame, f32 end_frame);
  b32  IsUnitFrameEndFrame(u32 slot_index, u32 unit_index) const;
  b32  IsUnitLoop(u32 slot_index, u32 unit_index) const;
  void SetUnitLoop(u32 slot_index, u32 unit_index, b32 loop);


  //-----------------------------------------------------------------------------
  /**
   *  @brief         slot_index内につくったユニットのアニメーションをブレンドする
   *
   *  @param[in]     heap_allocator          ヒープアロケータ
   *  @param[in]     device_allocator        デバイスアロケータ
   *  @param[in]     slot_index              スロットのインデックス
   *  @param[in]     mode                    AnimationSlot::MODE_NONE                slot_indexのアニメーションをなしにする
   *                                         AnimationSlot::MODE_ANIMATION           ブレンドをやめてanimation_unit_indexで指定したアニメーションを割り当てる
   *                                         AnimationSlot::MODE_BLEND_INTERPOLATOR  補間ブレンド
   *                                         AnimationSlot::MODE_BLEND_ADDER         加算ブレンド
   *  @param[in]     weight0                 AnimationSlot::MODE_BLEND_INTERPOLATORかAnimationSlot::MODE_BLEND_ADDERのときのみ使用。
   *                                         ユニット0のブレンド重み
   *  @param[in]     weight1                 AnimationSlot::MODE_BLEND_INTERPOLATORかAnimationSlot::MODE_BLEND_ADDERのときのみ使用。
   *                                         ユニット1のブレンド重み
   *  @param[in]     interpolator_ignore_no_anim_member   AnimationSlot::MODE_BLEND_INTERPOLATORのときのみ使用。
   *                                                      falseにすると、アニメーションが存在しないメンバはバインド時の値がブレンドされます。
   *                                                      trueにすると、アニメーションが存在しないメンバは重み 0 としてブレンドされます。
   *  @param[in]     interpolator_normalization_enable    AnimationSlot::MODE_BLEND_INTERPOLATORのときのみ使用。
   *                                                      trueにすると、ブレンド重みが合計で 1 になるよう正規化してからブレンドを行います。
   *                                                      falseにすると、設定した重みがそのままブレンドに使用される。
   *  @param[in]     animation_unit_index                 AnimationSlot::MODE_ANIMATIONのときのみ使用。
   *                                                      スロット内のどのユニットにあるアニメーションを割り当てるかを指定する。
   */
  //-----------------------------------------------------------------------------
  void BlendUnitAnimation(
      gfl::heap::NwAllocator*    heap_allocator,
      gfl::heap::NwAllocator*    device_allocator,
      u32                        slot_index,
      AnimationSlot::Mode        mode,
      b32                        interpolator_ignore_no_anim_member = false,
      b32                        interpolator_normalization_enable = true,
      u32                        animation_unit_index = 0
  );
  void BlendUnit2Animation(
      gfl::heap::NwAllocator*    heap_allocator,
      gfl::heap::NwAllocator*    device_allocator,
      u32                        slot_index,
      AnimationSlot::Mode        mode,
      f32                        weight0,
      f32                        weight1,
      b32                        interpolator_ignore_no_anim_member = false,
      b32                        interpolator_normalization_enable = true,
      u32                        animation_unit_index = 0
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ブレンド重み
   */
  //-----------------------------------------------------------------------------
  void SetUnitBlendWeight(u32 slot_index, u32 unit_index, f32 weight);
  f32  GetUnitBlendWeight(u32 slot_index, u32 unit_index);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ResAnim::Typeのアニメーションで変更したポーズをリセットする
   *
   *                 slot_indexスロットに割り当てているアニメーションのResAnim::Typeのポーズをリセットする
   *                 ただし、割り当て中のアニメーションが動かすメンバはリセットされない
   *
   *  @param[in]     type          アニメーションのタイプ
   *  @param[in]     slot_index    スロットのインデックス
   *
   *  アニメーションが割り当てられているときは、そのアニメーションが動かすメンバはリセットされない。
   *  アニメーションが割り当てられていないときは、全てのメンバがリセットされる。
   *
   *  slot_indexスロットにアニメーションが割り当てられていないときは、そのスロットのアニメーションのResAnim::Typeがないため、
   *  何もリセットされないので注意すること。
   *
   *  typeがResAnim::TYPE_MATERIALの場合は、このモデルを生成したときの
   *  Descriptionで指定しているMaterialBufferOptionが
   *  MATERIAL_BUFFER_OPTION_ANIMATABLE_MATERIALか
   *  MATERIAL_BUFFER_OPTION_ALL_MATERIALでなければならない。
   */
  //-----------------------------------------------------------------------------
  void ResetAnimationPose(ResAnim::Type type);
  void ResetAnimationSlotPose(u32 slot_index);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ワールドマトリクスを取得する
   *
   *  @param[out]    matrix          取得したマトリクスを格納して返す
   *
   *  @retval        bool            正常に取得できたらtrue
   */
  //-----------------------------------------------------------------------------
  virtual bool GetWorldMatrix(gfl::math::MTX34* matrix) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ワールドマトリクスを設定する
   *                 SceneのCalculateの後でないと反映されません。
   *
   *  @param[in]     matrix          設定するマトリクス
   *
   *  @retval        bool            正常に設定できたらtrue
   */
  //-----------------------------------------------------------------------------
  virtual bool SetWorldMatrix(const gfl::math::MTX34& matrix);  // ライトやカメラ、ボーンのないモデルなら、これを設定すれば見た目に値が反映される。
                                                                // ボーンのあるモデルだとこれを設定しても見た目に値が反映されているようには見えない。

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ワールドマトリクスの計算の有効フラグ
   *
   *  @param[in]     enable         有効にするときはtrue、無効にするときはfalse
   */
  //-----------------------------------------------------------------------------
  void SetWorldMatrixCalculationEnable(bool enable);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ワールドマトリクスの計算の有効フラグ
   *
   *  @retval        bool            有効のときはtrue、無効のときはfalse
   */
  //-----------------------------------------------------------------------------
  bool IsWorldMatrixCalculationEnable(void) const;


  //-----------------------------------------------------------------------------
  /**
   *  @brief         ボーンの数を取得する
   *
   *  @retval        s32             ボーンの数
   */
  //-----------------------------------------------------------------------------
  virtual s32  GetBonesCount(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ボーンのインデックス番号を取得する
   *
   *  @param[in]     key             ボーンの名前
   *
   *  @retval        s32             ボーンのインデックス番号        
   */
  //-----------------------------------------------------------------------------
  virtual s32  GetBonesIndex(const char* key) const;
   //-----------------------------------------------------------------------------
  /**
   *  @brief         ワールドマトリクスポーズのボーンの数を取得する
   *
   *  @retval        s32             ボーンの数       
   */
  //-----------------------------------------------------------------------------
  virtual s32  GetBonesCountOfWorldMatrixPose(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ワールドマトリクスポーズのマトリクスを取得する
   *
   *  @param[in]     bone_index      ボーンのインデックス
   *  @param[out]    matrix          取得したマトリクスを格納して返す
   *
   *  @retval        bool            正常に取得できたらtrue
   */
  //-----------------------------------------------------------------------------
  virtual bool GetBoneMatrixOfWorldMatrixPose(s32 bone_index, gfl::math::MTX34* matrix) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ワールドマトリクスポーズのマトリクスを設定する
   *
   *  @param[in]     bone_index      ボーンのインデックス
   *  @param[in]     matrix          設定するマトリクス
   *
   *  @retval        bool            正常に設定できたらfalse
   */
  //-----------------------------------------------------------------------------
  virtual bool SetBoneMatrixOfWorldMatrixPose(s32 bone_index, const gfl::math::MTX34& matrix);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ワールドマトリクスポーズのマトリクスのポインタを取得する
   *
   *  @param[in]     bone_index         ボーンのインデックス
   *
   *  @retval        gfl::math::MTX34*  マトリクスのポインタを返す。
   *                                    NULLのときなし。
   *                                    これに値を代入すればワールドマトリクスポーズのマトリクスに反映されます。
   */
  //-----------------------------------------------------------------------------
  virtual gfl::math::MTX34* GetBoneMatrixPointerOfWorldMatrixPose(s32 bone_index) const;

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         差し替え用のボーンデータを作成する
   *
   *  @param[in]     heap_allocator  ヒープメモリのアロケータ
   *  @param[in]     base_model      基本モデル
   */
  //-----------------------------------------------------------------------------
  void CreateReplaceBoneData(gfl::heap::NwAllocator* heap_allocator, Model* base_model);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         差し替え用のボーンデータを破棄する
   */
  //-----------------------------------------------------------------------------
  void DestroyReplaceBoneData(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         差し替え用のボーンの無視設定を行う(親方向)
   *
   *  @param[in]     name   nameのボーンより親のボーンは全て無視設定にする(nameのボーンは有効設定のまま)
   */
  //-----------------------------------------------------------------------------
  void IgnoreParentReplaceBone(const char* name); 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         差し替え用のボーンの無視設定を行う(子方向)
   *
   *  @param[in]     name   nameのボーンより子のボーンは全て無視設定にする(nameのボーンは有効設定のまま)
   */
  //-----------------------------------------------------------------------------
  void IgnoreChildReplaceBone(const char* name); 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         差し替え用のボーンの無視設定を行う(子方向)
   *
   *  @param[in]     name   nameのボーンとそれより子のボーンは全て無視設定にする(nameのボーンも無効設定になる)
   */
  //-----------------------------------------------------------------------------
  void IgnoreSelfChildReplaceBone(const char* name); 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         「baseのボーン」のワールドマトリクスを「partのボーン(このモデルのボーン)」のワールドマトリクスに設定する
   *                 ここで設定した値はSceneのCalculateを呼ぶと上書きされてしまうので、SceneのCalculateの後で呼ぶこと。
   *                 
   *                 // @note ワールドマトリクスに設定する関数なので、humanトップにも設定したほうがいいかもしれないが、
   *                          今はまだ設定していない。
   *                 // @note そもそもワールド、ローカルと分けずに、全部に設定するのがいいのかもしれない。
   *                 // @note この関数にて、コールバックのあるhumanトップやhandボーンに値を設定したら、
   *                          コールバックを呼ぶようにするつもりだが、今はまだ呼べていない。
   *
   *  @param[in]     base_model      基本モデル
   */
  //-----------------------------------------------------------------------------
  void SetWorldReplaceBone(Model* base_model);
  //void SetLocalReplaceBone(Model* base_model);  // @note ローカルマトリクス版だが、まだできていない
                                                  //       NW4Cのスケルトン共有と同じ結果を得るつもりの関数。スケルトンアニメはしているが場所が違うというもの。
  //void SetWorldLocalReplaceBone(Model* base_model);  // @note ワールドマトリクスにもローカルマトリクスに値を設定する版だが、まだできていない
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         res_boneのワールドマトリクスにparent_world_matrixを反映したものを設定する。
   *                 その後、再帰的に子ボーンにその処理を行っていく。
   *                 
   *  @param[in]     skeleton             partのスケルトン
   *  @param[in]     res_bone             partのボーン
   *  @param[in]     parent_world_matrix  res_boneの親のワールドマトリクス
   */
  //-----------------------------------------------------------------------------
  void SetWorldReplaceBoneRecursive(
      nw::gfx::Skeleton*     skeleton,
      nw::gfx::res::ResBone  res_bone,
      const nw::math::MTX34* parent_world_matrix
#if GFL_DEBUG
      , const s32            recursive_count 
#endif
  ); 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         差し替え用のボーンの無視設定を再帰的に行う(子方向)
   *
   *  @param[in]     res_bone   res_boneのボーンを無視設定にし、res_boneより子のボーンも全て無視設定にする(res_boneのボーンも無視設定になる)
   */
  //-----------------------------------------------------------------------------
  void ignoreSelfChildReplaceBoneRecursive(
      nw::gfx::res::ResBone  res_bone
#if GFL_DEBUG
      , const s32            recursive_count 
#endif
  ); 




#if 0
  /*
  batonはどこか1個にしか拘束させることができない。
  handは複数のbatonを持てる。
  humanも複数のbatonを持てる。
  よって、コンストレインの情報はbatonに持たせておくほうがいいので、
  ModelからSceneNodeに移動する。
  ここはコメントアウト。
   */
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストレインのセットアップをする
   *
   *                 既にセットアップされているbaton_scene_nodeをセットアップすると、そのセットアップで上書きされる。
   *                 だから、inheritを変更したいときやhandボーンを変更したいときにもこの関数を呼べばよい。
   *
   *  @param[in]     hand_bone_index     handボーンのインデックス
   *                                     BONE_INDEX_NONEのとき、handボーンではなくhumanトップにつける
   *  @param[in]     hand_bone_key       handボーンの名前
   *                                     NULLのとき、handボーンではなくhumanトップにつける
   *  @param[in]     baton_scene_node    拘束されるbatonシーンノード
   *  @param[in]     inherit_scale       trueのときスケールの追従あり
   *  @param[in]     inherit_rotate      trueのとき回転の追従あり
   *  @param[in]     inherit_translate   trueの平行移動の追従あり
   *
   *  @retval        b32                 正常にセットアップできたらtrue
   */
  //-----------------------------------------------------------------------------
  b32 SetupConstraint(
      const s32 hand_bone_index,
      gfl::grp::g3d::SceneNode* baton_scene_node,
      const b32 inherit_scale     = true,
      const b32 inherit_rotate    = true,
      const b32 inherit_translate = true
  );
  b32 SetupConstraint(
      const char* hand_bone_key,
      gfl::grp::g3d::SceneNode* baton_scene_node,
      const b32 inherit_scale     = true,
      const b32 inherit_rotate    = true,
      const b32 inherit_translate = true
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストレインのクリーンアップをする
   *
   *                 baton_scene_nodeが拘束されなくなる。
   *
   *  @param[in]     hand_bone_index     handボーンのインデックス
   *                                     BONE_INDEX_NONEのとき、handボーンではなくhumanトップにつけられている
   *  @param[in]     hand_bone_key       handボーンの名前
   *                                     NULLのとき、handボーンではなくhumanトップにつけられている
   *  @param[in]     baton_scene_node    拘束されていたbatonシーンノード
   *
   *  @retval        b32                 正常にクリーンアップできたらtrue
   */
  //-----------------------------------------------------------------------------
  b32 CleanupConstraint(
      const s32 hand_bone_index,
      gfl::grp::g3d::SceneNode* baton_scene_node
  );
  b32 CleanupConstraint(
      const char* hand_bone_key,
      gfl::grp::g3d::SceneNode* baton_scene_node
  );
#endif
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーンノードをこれに拘束する
   *                 SceneNode::SetupConstraintから呼ばれる
   *
   *  @param[in]     baton_scene_node    拘束されるbatonシーンノード
   *  @param[in]     hand_scene_index    handボーンのインデックス
   *  @param[in]     inherit_scale       trueのときスケールの追従あり
   *  @param[in]     inherit_rotate      trueのとき回転の追従あり
   *  @param[in]     inherit_translate   trueの平行移動の追従あり
   *
   *  @retval        追加できたらtrueを返す
   *                 元々追加されており新たに追加できないときはGFL_ASSERT_MSGをしてfalseを返す  //@check 検索にひっかからないように確認済の印をつける
   */
  //-----------------------------------------------------------------------------
  b32 AddConstraintBatonSceneNode(
    SceneNode* baton_scene_node,
    const s32  hand_bone_index,
    const b32  inherit_scale,
    const b32  inherit_rotate,
    const b32  inherit_translate
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーンノードの拘束の追従を変更する
   *                 baton_scene_nodeやhand_bone_indexは変更しないで、inheritだけ変更する
   *                 SceneNode::SetupConstraintから呼ばれる
   *
   *  @param[in]     baton_scene_node    拘束しているbatonシーンノード
   *  @param[in]     hand_scene_index    batonをくっつけているhandボーンのインデックス
   *  @param[in]     inherit_scale       trueのときスケールの追従あり
   *  @param[in]     inherit_rotate      trueのとき回転の追従あり
   *  @param[in]     inherit_translate   trueの平行移動の追従あり
   *
   *  @retval        変更できたらtrueを返す
   *                 元々追加されていないときはGFL_ASSERT_MSGをしてfalseを返す  //@check 検索にひっかからないように確認済の印をつける
   */
  //-----------------------------------------------------------------------------
  b32 ChangeConstraintBatonSceneNodeInherit(
    SceneNode* baton_scene_node,
    const s32  hand_bone_index,
    const b32  inherit_scale,
    const b32  inherit_rotate,
    const b32  inherit_translate
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーンノードの拘束を解除する
   *                 SceneNode::SetupConstraintから呼ばれる
   *                 SceneNode::CleanupConstraintから呼ばれる
   *
   *  @param[in]     baton_scene_node    拘束されているbatonシーンノード
   *  @param[in]     hand_scene_index    batonをくっつけているhandボーンのインデックス
   *
   *  @retval        削除できないときはfalseを返す
   *                 元々追加されていないときはGFL_ASSERT_MSGをしてfalseを返す  //@check 検索にひっかからないように確認済の印をつける
   */
  //-----------------------------------------------------------------------------
  b32 RemoveConstraintBatonSceneNode(
    SceneNode* baton_scene_node,
    const s32  hand_bone_index
  );


private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         レンダーキューへ追加、レンダーキューから削除
   */
  //-----------------------------------------------------------------------------
  // UserParameterをRenderQueueには使っていないことが判明したので、
  // SafeDeleteCheck用の使わせてもらう。
  virtual void AddAllRenderQueue(void);  // SafeDeleteCheck用に使っています。
  virtual void RemoveAllRenderQueue(void);  // SafeDeleteCheck用に使っています。
  virtual void AddRenderQueue(s32 render_queue_index);  // SafeDeleteCheck用に使っています。
  virtual void RemoveRenderQueue(s32 render_queue_index);  // SafeDeleteCheck用に使っています。

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ユーザーパラメータのチェック
   */
  //-----------------------------------------------------------------------------
  // このモデルをこのレンダーキューで描画するか
  // UserParameterをRenderQueueには使っていないことが判明したので、
  // SafeDeleteCheck用の使わせてもらう。
  static bool IsUserParameterRenderQueue( const nw::gfx::Model* nw_model, s32 render_queue_index );  // SafeDeleteCheck用に使っています。


private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         本クラスのマテリアルのバッファの生成オプションをNintendoWareのバッファの生成オプションに変更する
   */
  //-----------------------------------------------------------------------------
  static bit32 ConvertMaterialBufferOptionToNwBufferOption(
      const MaterialBufferOption  material_buffer_option
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成時にスケルトン共有をチェックし、適切に処理する
   */
  //-----------------------------------------------------------------------------
  static ShareType CheckSkeletonShareSourceModel(
      Model*               skeleton_share_source_model,
      nw::gfx::Skeleton**  nw_skeleton 
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成時にマテリアル共有をチェックし、適切に処理する
   */
  //-----------------------------------------------------------------------------
  static ShareType CheckMaterialShareSourceModel(
      Model*                material_share_source_model,
      MaterialBufferOption  material_buffer_option,
      nw::gfx::Model**      nw_model
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成時にメッシュノードビジビリティ共有をチェックし、適切に処理する
   */
  //-----------------------------------------------------------------------------
  static ShareType CheckMeshNodeVisibilityShareSourceModel(
      Model*                mesh_node_visibility_share_source_model,
      nw::gfx::Model**      nw_model
  );


private:
  // ユーザーパラメータ(32ビット)
  enum
  {
    // ユーザーパラメータの31～24ビットを、このモデルをどのレンダーキューで描画するかのフラグに使用する
    USER_PARAMETER_RENDER_QUEUE_BIT_NUM    = 8,            // 使用するビット数
    USER_PARAMETER_RENDER_QUEUE_BIT_SHIFT  = 24,           // ビットシフト
    USER_PARAMETER_RENDER_QUEUE_BIT_MASK   = 0xFF000000    // ビットマスク

    // ユーザーパラメータの？～？ビットを、？？？？
    //USER_PARAMeTER_？？？？_BIT_NUM    = ,
    //USER_PARAMeTER_？？？？_BIT_SHIFT  = ,
    //USER_PARAMeTER_？？？？_BIT_MASK   =
  };


private:
  b32             m_tree_flag;  // trueのときSceneにAddされていたらシーンツリーに入れられる、falseのときSceneにAddされていてもシーンツリーから外される

  u32             m_animation_slot_num;
  //AnimationSlot*  m_animation_slot_array;
  u32             m_animation_blend_unit_num;
  u32             m_animation_num_in_unit;
  AnimationManager*  m_animation_manager;

  ShareType    m_skeleton_share_type;
  ShareType    m_material_share_type;
  ShareType    m_mesh_node_visibility_share_type;

  MaterialBufferOption   m_material_buffer_option;

  // 計算処理にて呼び出されるコールバック
  u32                              m_calculate_callback_num_max;
  ModelCalculateCallbackFunction*  m_calculate_callback_function_array;
  void**                           m_calculate_callback_work_array;

  // コンストレイン
  internal::CallbackConstraintOfSkeleton*       m_callback_constraint_of_skeleton;
  internal::CallbackConstraintOfTransformNode*  m_callback_constraint_of_transform_node;

  // 差し替え
  ReplaceBoneData*  m_replace_bone_data_array;  // m_replace_bone_data_array[ボーン数]


#if GFL_DEBUG
  //----------------------------------------------------------------
  //
  // 安全にモデルが解放できたかを確認する機能の有効無効設定
  //
  //----------------------------------------------------------------
private:
  static b32 s_SafeDeleteCheckEnable;  // trueのとき安全解放チェック有効。
  static b32 s_SafeDeleteCheckAssert;  // trueのときASSERTで止める。falseのときPRINTで済ませる。
public: 
  static void SafeDeleteCheckEnable(b32 fEnable){ s_SafeDeleteCheckEnable = fEnable; }
  static b32 IsSafeDeleteCheckEnable(void){ return s_SafeDeleteCheckEnable; }
  static void SafeDeleteCheckAssert(b32 fAssert){ s_SafeDeleteCheckAssert = fAssert; }
  static b32 IsSafeDeleteCheckAssert(void){ return s_SafeDeleteCheckAssert; }

private:
  u32   m_DrawFrameCount;
  char  m_ModelName[32];
private:
  void SetSafeDeleteCheckInfo(const char* pName);
  void SetModelName(const char* pName);
public:
  void SetDrawFrameCount(u32 cnt){ m_DrawFrameCount = cnt; }
  u32 GetDrawFrameCount(void){ return m_DrawFrameCount; }
#endif    


};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DMODEL_H__
