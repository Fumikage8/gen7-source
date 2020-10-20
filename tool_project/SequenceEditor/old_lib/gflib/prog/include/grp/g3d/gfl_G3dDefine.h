#ifndef __GFL_G3DDEFINE_H__
#define __GFL_G3DDEFINE_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dDefine.h
 *	@brief  3D定義
 *	@author	Koji Kawada
 *	@date		2011.03.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>

// gflib grp g3d


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					定数定義
 */
//-----------------------------------------------------------------------------

//-------------------------------------
///	gfx_Config.h
//=====================================
#if defined(NW_GFX_VERTEX_LIGHT_ENABLED)
    #define GFL_GRP_G3D_VERTEX_LIGHT_ENABLE (1)  // 頂点ライト有効
#else
    #define GFL_GRP_G3D_VERTEX_LIGHT_ENABLE (0)  // 頂点ライト無効
#endif


//-------------------------------------
///	deprecated
//
#if 0
/*非推奨
関数の使用を禁止したい際の移行期間中に付けておくとよい。
付けたソースがコンパイルされないとそのメッセージは表示されないので、
付けたら「すぐに」「いつでも」使っている場所に対してメッセージを出すわけ「ではない」。

NintendoWare/CTR/include/nw/config/compiler.h
を参考に作成。

使い方
非推奨の関数にGFL_GRP_G3D_DEPRECATED_FUNCTIONと付ければよい。

例:
gfl_G3dScene.h
GFL_GRP_G3D_DEPRECATED_FUNCTION(bool  AddSceneNode( SceneNode* scene_node ));

gfl::grp::g3d::Sceneクラスのメンバ関数の宣言にだけ付けておけば
c:\home\gflib_cpp\skelton\prog\src/system/launcher.cpp:920:12: Warning:  #1361-D
: function "gfl::grp::g3d::Scene::AddSceneNode" (declared at line 1287 of "c:\ho
me\gflib_cpp\gflib\prog\include\grp/g3d/gfl_G3dScene.h") was declared "deprecate
d"
    s_scene->AddSceneNode(s_StereoCamera);
             ^===============================================  ] 02117 / 02122
というふうにコンパイルでメッセージが表示される。

関数の定義にはGFL_GRP_G3D_DEPRECATED_FUNCTIONは付けられないので宣言にだけ付けておけばよい。

関数の宣言と定義を一緒に行っている場合は
GFL_GRP_G3D_DEPRECATED_FUNCTION(bool  AddSceneNode( SceneNode* scene_node ))
{
}
というふうに付ければよい。

gfl_G3dResAnim.h
  GFL_GRP_G3D_DEPRECATED_FUNCTION( virtual void Create(
      gfl::heap::NwAllocator*           allocator, 
      Resource*                         resource,
      s32                               index_in_resource,
      Type                              type               = TYPE_SKELETAL
  ) );*/
#endif
//
//=====================================
//#define GFL_GRP_G3D_IGNORE_DEPRECATED  // これが定義されているときGFL_GRP_G3D_DEPRECATED_FUNCTIONを無視します。

#if defined( __CC_ARM ) || defined( __ARMCC_VERSION )
  // ARM RealView Compiler.

  #define GFL_GRP_G3D_DEPRECATED_FUNCTION_MSG(MFunc, msg) MFunc __attribute__ ((deprecated))

#elif defined( __CWCC__ ) || defined( __MWERKS__ )
  // Freescale CodeWarrior

  #define GFL_GRP_G3D_DEPRECATED_FUNCTION_MSG(MFunc, msg) MFunc

#elif defined( __GNUC__ )
  // GNU C++

  #define GFL_GRP_G3D_DEPRECATED_FUNCTION_MSG(MFunc, msg) MFunc __attribute__ ((deprecated))

#elif defined( _MSC_VER )
  // Miscrosoft Visual C++

  #define GFL_GRP_G3D_DEPRECATED_FUNCTION_MSG(MFunc, msg) __declspec(deprecated(msg)) MFunc

#else

  #error "gfl::grp::g3d Unknown compiler"
    // #errorディレクティブは、指定されたメッセージを出力し、コンパイル作業を中止します。

#endif

#if !defined(GFL_GRP_G3D_DEPRECATED_FUNCTION)
  #if defined(GFL_GRP_G3D_IGNORE_DEPRECATED)
    #define GFL_GRP_G3D_DEPRECATED_FUNCTION(MFunc) MFunc
  #else
    #define GFL_GRP_G3D_DEPRECATED_FUNCTION(MFunc) GFL_GRP_G3D_DEPRECATED_FUNCTION_MSG(MFunc, "## gfl::grp::g3d Deprecated function ##")
  #endif
#endif



//-------------------------------------
///	h3d
//=====================================
#define GFL_GRP_G3D_H3D_USE (1)  // これが1のときh3dを使用する。これが0のときh3dを使用しない。
#define GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT (GFL_DEBUG&&GFL_GRP_G3D_H3D_USE&&(0))  // これが1のときコマンド数をカウントする

#ifdef BUGFIX_BTS7075_20130802  
#define GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH (GFL_GRP_G3D_H3D_USE&&(1))
#else
#define GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH (GFL_GRP_G3D_H3D_USE&&(0))  
#endif
    // これが1のときDrawMeshにてビルボード計算を行う。
    // これが0のときSubmitViewでしかビルボード計算を行わない
    // (これが0のとき起こり得る不具合：モデルを異なるカメラで複数回描画するときに、
    // 最後にSubmitViewしたカメラの方向をモデルが向いているという不具合)。



//-------------------------------------
///	その他定数
//=====================================
// enumをs32にする定数
enum
{
  ENUMS32MAX = 2100000000
};

// 配列のインデックス
//enum
//{
//  ARRAY_INDEX_NONE = -1  // インデックスなし
//  // 0以上は有効なインデックス
//};




#if GFL_DEBUG
    #define GFL_GRP_G3D_MODEL_RECURSIVE_COUNT_MAX (16)  // 再帰呼び出しの回数上限
#endif




//-------------------------------------
///	3Dシーン用の定数
//=====================================
// シーン計算グループのインデックス
enum
{
  SCENE_CALCULATE_GROUP_INDEX_NONE    = -1  // インデックスなし
  // 0以上は有効なインデックス
};

// シーン描画グループのインデックス
enum
{
  SCENE_DRAW_GROUP_INDEX_NONE         = -1,  // インデックスなし
  SCENE_DRAW_GROUP_INDEX_MULTI        = -2   // 複数のシーン描画グループに所属していることを示す
  // 0以上は有効なインデックス
};

// シーンの初期化処理の内容
enum SceneInitializeProcess
{
  // 必ずSCENE_INITIALIZE_PROCESS_NONEが一番小さな値になるようにして下さい。
  SCENE_INITIALIZE_PROCESS_NONE      = 240,  // 必要なし                // 何故240なんていう大きな数字かというと、
  SCENE_INITIALIZE_PROCESS_ALL,              // 全ての初期化処理を行う  // Scene::Initializeに渡している引数にs32があるから、
  SCENE_INITIALIZE_PROCESS_TRAVERSE,         // トラバースのみ行う      // それと明確に区別できているかを確認するため。

  SCENE_INITIALIZE_PROCESS_ENUMS32MAX = ENUMS32MAX
};




//-------------------------------------
///	3Dカメラ用の定数
//=====================================
// カメラのインデックス  // カメラインデックスはsceneごとに割り振る。scene_calculate_groupごとには割り振らない。
enum
{
  CAMERA_INDEX_NONE    = -1  // インデックスなし
};




//-------------------------------------
///	3D影用の定数
//=====================================
// 影グループのインデックス
enum
{
  SHADOW_GROUP_INDEX_NONE     = -1  // インデックスなし
};




//-------------------------------------
///	テクスチャのフォーマット
//=====================================
enum TextureFormat
{
  TEXTURE_FORMAT_RGBA8    = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA8   ,  //!< 1ピクセルが4バイト、RGBAが各8ビットのフォーマットです。
  TEXTURE_FORMAT_RGB8     = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGB8    ,  //!< 1ピクセルが3バイト、RGBが各8ビットのフォーマットです。
  TEXTURE_FORMAT_RGBA5551 = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA5551,  //!< 1ピクセルが2バイト、RGBが各5ビット、Aが1ビットのフォーマットです。
  TEXTURE_FORMAT_RGB565   = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGB565  ,  //!< 1ピクセルが2バイト、RGBが各5,6,5ビットのフォーマットです。
  TEXTURE_FORMAT_RGBA4    = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA4   ,  //!< 1ピクセルが2バイト、RGBAが各4ビットのフォーマットです。
  TEXTURE_FORMAT_LA8      = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_LA8     ,  //!< 1ピクセルが2バイト、AlphaとLuminanceが各8ビットのフォーマットです 
  TEXTURE_FORMAT_HILO8    = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_HILO8   ,  //!< 1ピクセルが2バイト、X,Yが各8ビットのフォーマットです。
  TEXTURE_FORMAT_L8       = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_L8      ,  //!< 1ピクセルが1バイト、Luminanceが8ビットのフォーマットです。
  TEXTURE_FORMAT_A8       = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_A8      ,  //!< 1ピクセルが1バイト、Alphaが8ビットのフォーマットです。
  TEXTURE_FORMAT_LA4      = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_LA4     ,  //!< 1ピクセルが1バイト、AlphaとLuminanceが各4ビットのフォーマットです 
  TEXTURE_FORMAT_L4       = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_L4      ,  //!< 1ピクセルが4ビット、Luminanceが4ビットのフォーマットです。
  TEXTURE_FORMAT_A4       = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_A4      ,  //!< 1ピクセルが4ビット、Alphaが4ビットのフォーマットです。
  TEXTURE_FORMAT_ETC1     = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_ETC1    ,  //!< 1ピクセルが4ビット相当の圧縮フォーマットです。
  TEXTURE_FORMAT_ETC1A4   = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_ETC1A4  ,  //!< 1ピクセルが8ビット相当の圧縮フォーマットです。                    
};




//-------------------------------------
///	3Dアニメーション用の定数
//=====================================
// アニメーションのmax_anim_membersに何を使うか
enum
{
  ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST          = -1,  // max_anim_membersに最初に渡したアニメーションリソースのGetMemberAnimSetCountを使用する
  ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERS    = -2   // max_anim_membersにmax_membersを使用する
};


// アニメーションタイプ
enum AnimationType
{
  ANIMATION_TYPE_NONE,  // 何も設定されていないときのタイプ
  ANIMATION_TYPE_SKELETAL,
  ANIMATION_TYPE_MATERIAL,
  ANIMATION_TYPE_VISIBILITY,
  ANIMATION_TYPE_LIGHT,
  ANIMATION_TYPE_CAMERA,
  ANIMATION_TYPE_FOG,
  ANIMATION_TYPE_ALL//,  // gfl_G3dResource.hのgfl::grp::g3d::Resource::AnimationResourceTypeにあるので
  //ANIMATION_TYPE_ANY,  // どのアニメーションタイプにもなり得るとき  // 今のところ使っていない、今後使わないかも、ALLと内容かぶるかも。
};


// アニメーションスロット
enum
{
  SLOT_INDEX_NONE  = -1
};


// アニメーション番号
enum
{
  ANIM_NO_NONE  = -1  // アニメーションをはずすときのanim_no
};




//-------------------------------------
///	3Dユーザデータ
//=====================================

// 投影(プロジェクション)に関するユーザデータ  // データの型は整数 DATA_TYPE_INT
#define  GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME  "ProjectionType"    // ユーザデータの名前
#define  GFL_GRP_G3D_H3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME  "$ProjectionType"    // ユーザデータの名前  // h3d版の名前
enum
{
  MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX      = 0,         // タイプ判別に用いることができるデータのインデックス番号
  MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE               =  0,  // タイプがなしのときの値
  MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW        =  1,  // タイプが影投影のときの値
  MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_LIGHT         =  2,   // タイプが光投影のときの値
  MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT  = 11   // タイプが影投影だがプログラムで光投影にしているときの値
  //MATERIAL_USER_DATA_PROJECTION_TYPE_ARG0_INDEX = ,  // 増えたらこんな感じに追加
  //MATERIAL_USER_DATA_PROJECTION_TYPE_ARG0_VALUE_AAA = ,
  //MATERIAL_USER_DATA_PROJECTION_TYPE_ARG0_VALUE_BBB = ,
};

// ???に関するユーザデータ  // データの型は文字列 DATA_TYPE_STRING  // 増えたらこんな感じに追加
//#define  GFL_GRP_G3D_MATERIAL_USER_DATA_???_NAME  "???"    // ユーザデータの名前
//enum
//{
//  MATERIAL_USER_DATA_???_ARG0_INDEX = ,
//  MATERIAL_USER_DATA_???_ARG0_VALUE_AAA = ,
//};




//-------------------------------------
///	便利関数
//=====================================
// ビットフラグにおいて指定したビットが1か0かを調べる関数
// bit_flagにおいてtargetで1になっているビットが全て1のときtrueを返す、どれか1個でも0のときfalseを返す
// targetのビット全てを0にして渡すと必ずtrueが返ってくる
// 使用例
// bit32 bit_flag = 0110;
// bit32 target_a = 0010;
// bit32 target_b = 0100;
// b32 ret = IsBitFlagOn( bit_flag, target_a|target_b );
static inline b32 IsBitFlagOn(const bit32 bit_flag, const bit32 target) { return ( (bit_flag&target) == target ); }

// ビットフラグにおいて指定したビットが1か0かを調べる関数
// target_oneは1ビットしか1にしてはならない
// IsBitFlagOnより比較回数が1回少なく済みます
// 【戻り値注意】戻り値はbit32型のtrue, falseではありません。「真のときの【0以外(1とは限らないしいつも違う値になる)】」か「偽のときの【0】」です。
// 0と比較するとか、if文の条件式に単独で入れておくとか、&&や||で繋ぐ、とかの場合は問題はなく使える。
static inline u32 IsBitFlagOnOne(const bit32 bit_flag, const bit32 target_one) { return ( bit_flag&target_one ); }

// ビットフラグにおいて指定したビットを1にする関数
// bit_flagに対してtargetで1になっているビット全てを1にする、targetで0になっているビットは変更しない
static inline void SetBitFlagOn(bit32* bit_flag, const bit32 target) { (*bit_flag) |= target; }

// ビットフラグにおいて指定したビットを0にする関数
// bit_flagに対してtargetで1になっているビット全てを0にする、targetで0になっているビットは変更しない
static inline void SetBitFlagOff(bit32* bit_flag, const bit32 target) { (*bit_flag) &= (~target); }

// ビットフラグにおいて指定したビットを1か0にする
// bit_flagに対してtargetで1になっているビット全てを1か0にする、targetで0になっているビットは変更しない
static inline void SetBitFlag(bit32* bit_flag, const bit32 target, const b32 is_on)
{ if(is_on) SetBitFlagOn(bit_flag, target); else SetBitFlagOff(bit_flag, target); }




//-------------------------------------
///	置き場所に困った関数
//=====================================

// 実体はgfl_G3dModel.cppにあります。
namespace internal {

extern void MultScale(nw::math::MTX34* dstMatrix, const nw::math::MTX34* srcMatrix, const nw::math::VEC3& scale);
extern void CopyRotate(nw::math::MTX34* dstMatrix, const nw::math::MTX34& srcMatrix);
extern void CopyTranslate(nw::math::MTX34* dstMatrix, const nw::math::MTX34& srcMatrix);

extern void
CalculateWorldMatrixOfSkeleton(
    nw::math::MTX34* transformMatrix,
    nw::math::VEC3* scale,
    const nw::gfx::CalculatedTransform& localTransform,
    const nw::gfx::CalculatedTransform& parentWorldTransform,
    const nw::gfx::CalculatedTransform& parentLocalTransform,
    const bool inheritTransform[ 3 ]
);

extern void
CalculateWorldMatrixOfTransformNode(
    nw::math::MTX34* transformMatrix,
    nw::math::VEC3* scale,
    const nw::gfx::CalculatedTransform& localTransform,
    const nw::gfx::CalculatedTransform& parentWorldTransform,
    const bool inheritTransform[ 3 ]
); 

}  // namespace internal




}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DDEFINE_H__
