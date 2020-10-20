#ifndef __GFL_G3DH3DMODEL_H__
#define __GFL_G3DH3DMODEL_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dModel.h
 *	@brief  H3Dモデル
 *	@author	Koji Kawada
 *	@date		2012.04.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
/*説明
このH3dModelでは
nw::h3d::res::ModelContent
や各種ステートを扱います。*/
#endif


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_fs.h>

// gflib grp
#include <grp/gfl_GraphicsType.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>



#if GFL_GRP_G3D_H3D_USE

// NW4C
#include <nw/h3d.h>
// gflib grp g3dh3d
#include <grp/g3d/gfl_G3dH3dResource.h>
#include <grp/g3d/gfl_G3dH3dEnvironment.h>
#include <grp/g3d/gfl_G3dH3dCommand.h>
#include <grp/g3d/gfl_G3dH3dResTexture.h>
#include <grp/g3d/gfl_G3dH3dDynamicTexture.h>
#include <grp/g3d/gfl_G3dH3dResShader.h>
#include <grp/g3d/gfl_G3dH3dAnimationManager.h>
#include <grp/g3d/gfl_G3dH3dEx.h>

#include "gfl_G3dH3dModelMemoryManager.h"




namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

// 前方宣言
class Scene;
class H3dUserData;
class H3dModel;

extern bool G3DH3DMODEL_DEBUG_PRINT;


//-------------------------------------
///	H3Dメッシュ関連のイベントを観察するリスナークラス
//=====================================
class H3dMeshListener
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dMeshListener);

  friend class H3dModel;


public:
  // @brief コンストラクタ
  H3dMeshListener(void)
    : next(NULL)
  {}
  // @brief デストラクタ
  virtual ~H3dMeshListener(){}

public:
  // @brief イベントが起きたときに呼ばれる関数
  // @param[in]  model           メッシュが所属しているモデル
  // @param[in]  material_index  メッシュに割り当てられているマテリアルのインデックス
  // @param[in]  command         コマンド
  // @param[in]  env             環境
  virtual void Invoke(H3dModel* model, s32 material_index, H3dCommand* command, H3dEnvironment* env){}


  ///////////////////////////////////////////////////////////////////////////////
  //
  // friendには公開している非公開メンバ
  //
  ///////////////////////////////////////////////////////////////////////////////
private:
  H3dMeshListener* next;  // 次のリスナーのポインタ
};




//-------------------------------------
///	H3Dモデルクラス
//=====================================
class H3dModel : public Object
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dModel);


  friend class H3dAnimationSlot;
  friend class Scene;
  friend class RenderSystem;
  friend struct RenderSceneInternalFunctor;
  friend struct RenderStereoSceneInternalFunctor;


public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 定数
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  enum
  {
    INDEX_NONE          = -1,  // インデックスが見付からないとき
    INDEX_IGNORE        = -2,  // 無視していいボーンという意味で使うインデックス番号

    CONSTANT_COLOR_NUM  = 6    // コンスタントカラーの個数
  };

  // テクスチャコーディネータ  マッピング方法
  enum TextureMappingMethod
  {
    TEXTURE_MAPPING_METHOD_UV_COORDINATE     = nw::h3d::TextureMappingType::UV_COORDINATE,  // テクスチャ座標です。 
    TEXTURE_MAPPING_METHOD_CAMERA_CUBE_ENV   = nw::h3d::TextureMappingType::CUBE_ENV,       // カメラキューブ座標です。 
    TEXTURE_MAPPING_METHOD_CAMERA_SPHERE_ENV = nw::h3d::TextureMappingType::SPHERE_ENV,     // カメラスフィア座標です。 
    TEXTURE_MAPPING_METHOD_PROJECTION        = nw::h3d::TextureMappingType::PROJECTION,     // 投影します。 
    TEXTURE_MAPPING_METHOD_SHADOW            = nw::h3d::TextureMappingType::SHADOW,         // シャドウ用の投影です。 
    TEXTURE_MAPPING_METHOD_SHADOW_CUBE       = nw::h3d::TextureMappingType::SHADOW_CUBE,    // シャドウキューブ用の投影です。
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

  // デスクリプション
  struct Description
  {
    b32 delete_memory_control;  ///< true だと、モデル破棄時のメモリ破棄を１フレーム遅れて実行する。 マテリアルのキャッシュメモリがコマンドバッファに積まれているため。
    s32 maxFragmentLightNum;    ///< シーンで使用するライトの最大数
    
    Description(void);
  };


public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // コンストラクタ / デストラクタ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   */
  //-----------------------------------------------------------------------------
  H3dModel(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~H3dModel();



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 生成 / 破棄
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   */
  //-----------------------------------------------------------------------------
  virtual b32 Create(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      const H3dResource* resource,
      const char* name,
      const Description* description = NULL
  );
  virtual b32 Create(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      const H3dResource* resource,
      s32 index_in_resource,
      const Description* description = NULL
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         破棄
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 設定 / 取得
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
private:  // friendには公開している非公開メンバ
  //-----------------------------------------------------------------------------
  /**
   *  @brief         Sceneへの登録が変わったときに呼び、登録しているSceneを設定する
   *
   *  @param[in]     scene  登録しているScene。Sceneから外したときはNULLを渡す。
   */
  //-----------------------------------------------------------------------------
  inline void SetScene(Scene* scene)
  {
    m_scene = scene;
  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         登録しているSceneを取得する
   *
   *  @retval        登録しているScene。Sceneから外れているときはNULLが返る。
   */
  //-----------------------------------------------------------------------------
  inline Scene* GetScene(void) const
  {
    return m_scene;
  }



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 計算
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //-----------------------------------------------------------------------------
  /**
   *  @brief         計算処理
   */
  //-----------------------------------------------------------------------------
 
#if 0
/*★これは考え方がgfxと合わない

  ///////////////////////////
  // 全部入り
  ///////////////////////////
protected:
  virtual void Calculate(void);
private:
  // アニメーションマネージャ計算
  void calculateAnimationManager(void);

  ////////////////////////////////////////////////////////////////////////////////
  // 【注意】全部入りと個別は1フレームにどちらかしか使わないようにして下さい。
  ////////////////////////////////////////////////////////////////////////////////
  
  ///////////////////////////
  // 個別
  ///////////////////////////
protected:
  // 独自更新
  virtual void CalculateBeforeCulling(void);
  virtual void CalculateAfterCulling(void);

  // ワールドマトリクス更新
  virtual void CalculateWorldMatrix(void);
  // ローカルスケルタルアニメーション更新
  virtual void CalculateLocalSkeletalAnimation(void);
  // ビジビリティアニメーション更新
  virtual void CalculateVisibilityAnimation(void);
  // マテリアルアニメーション更新
  virtual void CalculateMaterialAnimation(void);
  // アニメーションフレーム更新
  virtual void CalculateAnimationFrame(void);
  // ワールドスケルタルアニメーション更新
  virtual void CalculateWorldSkeletalAnimation(void);
*/
#endif


  
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // 派生クラスがつくる関数、g3dでつくった関数
  // gfl::grp::g3d::Model::Calculateに相当する関数
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
protected:
  virtual void Calculate(void);
  ///////////////////////////////////////////////////


  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // 派生クラスがつくらない、Sceneが更新する関数、gfxが勝手にやってくれる関数
  // nw::gfxにおいてシーンツリーに登録しておいたものをアップデートすることに相当する関数
  // CalculateSceneが全部ひとまとめ関数で、それ以外がそれの小分け関数。
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
private:
  virtual void CalculateScene(void);
  
  // 独自更新
  virtual void CalculateBeforeCulling(void);
  virtual void CalculateAfterCulling(void);

// GFBTS[2579] TreeFlagがFalseの時に、アニメーションのフレームが進まない問題を修正
#ifdef BUGFIX_GFBTS2579_20130723
protected:
#endif

  // ワールドマトリクス更新
  virtual void CalculateWorldMatrix(void);
  // ローカルスケルタルアニメーション更新
  virtual void CalculateLocalSkeletalAnimation(void);
  // ビジビリティアニメーション更新
  virtual void CalculateVisibilityAnimation(void);
  // マテリアルアニメーション更新
  virtual void CalculateMaterialAnimation(void);
  // アニメーションフレーム更新
  virtual void CalculateAnimationFrame(void);
  // ワールドスケルタルアニメーション更新
  virtual void CalculateWorldSkeletalAnimation(void);
  ///////////////////////////////////////////////////


  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // このクラス内部でだけ使用する関数
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
private:
  // アニメーションマネージャ計算
  void calculateAnimationManager(void);
  ///////////////////////////////////////////////////



private:  // friendには公開している非公開メンバ
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // SubmitView
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         SubmitView
   */
  //-----------------------------------------------------------------------------
  void SubmitView(
    nw::gfx::RenderQueue::ElementListType* list,
    const nw::gfx::Camera& camera
  );



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 描画
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
private:  // friendには公開している非公開メンバ
  //-----------------------------------------------------------------------------
  /**
   *  @brief         不透明描画
   */
  //-----------------------------------------------------------------------------
  /*void DrawOpa(
      H3dCommand* command,
      H3dEnvironment* env,
      const gfl::math::MTX34* view_mtx,
      bool fragmentAndWBufferDirty
  );*/

  //-----------------------------------------------------------------------------
  /**
   *  @brief         半透明描画
   */
  //-----------------------------------------------------------------------------
  /*void DrawXlu(
      H3dCommand* command,
      H3dEnvironment* env,
      const gfl::math::MTX34* view_mtx,
      bool fragmentAndWBufferDirty
  );*/ 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         メッシュ描画
   */
  //-----------------------------------------------------------------------------
#if 0
  void DrawMesh(
      const nw::h3d::res::MeshData& mesh,
      bool fragmentAndWBufferDirty
  );
#endif

  void DrawMesh(
      H3dCommand* command,
      H3dEnvironment* env,
      const nw::h3d::res::MeshData& mesh,
      bool fragmentAndWBufferDirty
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
      ,
      const gfl::math::MTX34&  view_mtx,
      const gfl::math::MTX34&  inv_view_mtx,
      const u32                inv_exist
#endif
  );



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 描画順
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
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
private:
  void initializeMaterialTranslucencyKind(void);

public:
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



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // ユーザデータ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  /////////////////////////
  // モデル
  /////////////////////////
  s32 GetModelUserDataCount() const;
  // 【注意】ユーザーデータは先頭に $ (ダラー) を付加した名前のメタデータとして保持されます。
  //         $をつけ忘れないように。
  s32 GetModelUserDataIndex(const char* name) const;
  void GetModelUserData(const s32 user_data_index, H3dUserData* user_data) const;
  
  bool GetModelAABB( gfl::math::AABB *p_dest_aabb ) const;
  
  /////////////////////////
  // マテリアル
  /////////////////////////
  s32 GetMaterialUserDataCount(const s32 material_index) const;
  // 【注意】ユーザーデータは先頭に $ (ダラー) を付加した名前のメタデータとして保持されます。
  //         $をつけ忘れないように。
  s32 GetMaterialUserDataIndex(const s32 material_index, const char* name) const;
  void GetMaterialUserData(const s32 material_index, const s32 user_data_index, H3dUserData* user_data) const;
  
  /////////////////////////
  // メッシュ
  /////////////////////////
  s32 GetMeshUserDataCount(const s32 mesh_index) const;
  // 【注意】ユーザーデータは先頭に $ (ダラー) を付加した名前のメタデータとして保持されます。
  //         $をつけ忘れないように。
  s32 GetMeshUserDataIndex(const s32 mesh_index, const char* name) const;
  void GetMeshUserData(const s32 mesh_index, const s32 user_data_index, H3dUserData* user_data) const;

  u32 GetMaterialProjData( const s32 material_index );
  void SetMaterialProjData( const s32 material_index, const u32 value );


private:
  nw::h3d::fnd::PatriciaMap<nw::h3d::res::MetaData>* getModelMetaDataMap(void) const;
  nw::h3d::fnd::PatriciaMap<nw::h3d::res::MetaData>* getMaterialMetaDataMap(const s32 material_index) const;
  nw::h3d::fnd::PatriciaMap<nw::h3d::res::MetaData>* getMeshMetaDataMap(const s32 mesh_index) const;



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 座標変換
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  // スケール
  void SetScale(f32 x, f32 y, f32 z);
  void SetScale(const gfl::math::VEC3& t);
  void GetScale(gfl::math::VEC3* t) const;

  // 回転
  void SetRotateXYZ(const gfl::math::VEC3& t);  // [radian]

  void GetRotateXYZ(gfl::math::VEC3* t) const;  // [radian]  // 【注意】SetRotateXYZで渡された値を返すだけです。SetRotateXYZ以外で回転を指定していた場合は
                                                             //         見た目と合わない値が返ってきます。
 
  template <typename TMatrix> void SetRotateMatrix(const TMatrix& m)
  {
    m_rotate_mtx.f._00 = m.f._00;
    m_rotate_mtx.f._01 = m.f._01;
    m_rotate_mtx.f._02 = m.f._02;
    m_rotate_mtx.f._03 = 0;  // @note m_translate_vecをこっちに統合したい
    m_rotate_mtx.f._10 = m.f._10;
    m_rotate_mtx.f._11 = m.f._11;
    m_rotate_mtx.f._12 = m.f._12;
    m_rotate_mtx.f._13 = 0;
    m_rotate_mtx.f._20 = m.f._20;
    m_rotate_mtx.f._21 = m.f._21;
    m_rotate_mtx.f._22 = m.f._22;
    m_rotate_mtx.f._23 = 0;
    
    SetBitFlagOff(&m_bit_flag, BIT_FLAG_ENABLE_ROTATE_VEC);
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

  // 平行移動
  void SetTranslate(const gfl::math::VEC3& t);
  void GetTranslate(gfl::math::VEC3* t) const;

  virtual bool GetWorldMatrix(gfl::math::MTX34* matrix) const
  {
    *matrix = m_world_mtx;
    return true;
  }
  
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // フラグ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーンツリーフラグ
   *                 trueのときSceneにAddされていたらシーンツリーに入れられる。
   *                 falseのときSceneにAddされていてもシーンツリーから外される。
   *                 これがtrueであることと、ユーザがSceneにAddしたことは、同じではありません。
   *                 falseでシーンツリーから外されているときは、計算も描画もされない。
   */
  //-----------------------------------------------------------------------------
  virtual void SetTreeFlag(b32 flag);
  virtual b32 GetTreeFlag(void) const;
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         描画フラグ
   *                 trueのとき描画される。
   *                 ただしこれがtrueのときでも、GetTreeFlagがfalseのときは描画されない。
   *                 シーンツリーに入れられていてこれがfalseのときは、計算はされるが描画はされない。
   */
  //-----------------------------------------------------------------------------
  virtual void SetVisible(b32 visible);
  virtual b32  IsVisible(void) const;



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // マテリアル
  //
  // u8のときは0が黒&透明、255が白&不透明
  // f32のときは0.0fが黒&透明、1.0fが白&不透明
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  //
  // マテリアル全般
  //
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  s32 GetMaterialCount(void) const;
  s32 GetMaterialIndex(const char* name) const;
  const char* GetMaterialName(s32 material_index) const;

  // マテリアルアニメーションなどで変更フラグが立ったものをデフォルトの立っていない状態に戻す。
  void ResetMaterialState(s32 material_index);
  void ResetAllMaterialState(void);

  // マテリアルのテクスチャ0,1,2のUVのSRTの変更フラグをリセットする
  void ResetMaterialStateForTextureCoordinatorUV(s32 material_index);
  void ResetAllMaterialStateForTextureCoordinatorUV(void);



  ///////////////////////////////////////////
  ///////////////////////////////////////////
  //
  // カラー
  //
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  // アンビエントカラー
  void SetMaterialColorAmbient(s32 material_index, const gfl::grp::ColorF32& c);
  void GetMaterialColorAmbient(s32 material_index, gfl::grp::ColorF32* c) const;

  // ディフューズカラー
  void SetMaterialColorDiffuse(s32 material_index, const gfl::grp::ColorF32& c);
  void GetMaterialColorDiffuse(s32 material_index, gfl::grp::ColorF32* c) const;
  
  // スペキュラカラー
  void GetMaterialColorSpecular0(s32 material_index, gfl::grp::ColorF32* c) const;
  void GetMaterialColorSpecular1(s32 material_index, gfl::grp::ColorF32* c) const;

  // コンスタントカラー
  // noはデザイナーさんツールのコンスタントカラー"番号"
  virtual void SetMaterialColorConstant(s32 material_index, s32 no, const gfl::grp::ColorF32& c);
  virtual void GetMaterialColorConstant(s32 material_index, s32 no, gfl::grp::ColorF32* c) const;


  ///////////////////////////////////////////
  ///////////////////////////////////////////
  //
  // テクスチャマッパー
  //
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  // no  テクスチャマッパー番号(0,1,2,Proc)(テクスチャコーディネータ番号ではない)
  // テクスチャを設定する
  void SetMaterialTextureMapperTexture(s32 material_index, s32 no, const H3dResTexture& res_texture);
  void SetMaterialTextureMapperTexture(s32 material_index, s32 no, const H3dDynamicTexture& dynamic_texture);
  void SetMaterialTextureMapperTexture(s32 material_index, s32 no, const H3dModel& src_model, s32 src_material_index, s32 src_no);

  // @brief  テクスチャの設定を元に戻す
  // @attention VRAMに配置してDelete○○Section(DeleteSomeSectionなど)していると元に戻せないかもしれません。
  void ResetMaterialTextureMapperTexture(s32 material_index, s32 no);
  
  //////////////////////
  // 設定 / 取得
  //////////////////////
  // @retval テクスチャが貼ってあるときtrue、貼ってないときfalse
  b32 IsMaterialTextureMapperTextureValid(s32 material_index, s32 no) const;
  
  // @attention VRAMに配置してDelete○○Section(DeleteSomeSectionなど)していると値が取得できません。
  void* GetMaterialTextureMapperTexturePhysicalAddress(s32 material_index, s32 no) const;
  
  // @attention VRAMに配置してDelete○○Section(DeleteSomeSectionなど)していると値が取得できません。
  u16   GetMaterialTextureMapperTextureWidth(s32 material_index, s32 no) const;
  
  // @attention VRAMに配置してDelete○○Section(DeleteSomeSectionなど)していると値が取得できません。
  u16   GetMaterialTextureMapperTextureHeight(s32 material_index, s32 no) const;
  
  gfl::grp::g3d::TextureFormat GeMaterialTextureMapperTexturetForamat(s32 material_index, s32 no) const;
  
  u8    GetMaterialTextureMapperTextureMipmap(s32 material_index, s32 no) const;

  // @brief  最初に貼ってあったテクスチャの名前を得る
  const char* GetMaterialTextureMapperTextureOriginalName(s32 material_index, s32 no) const;
  // @brief  最初に貼ってあったテクスチャの名前を与え、それがどのマテリアルの何番のテクスチャだったかを得る
  // @param[out]  material_index  マテリアル番号を入れて返す
  // @param[out]  no              テクスチャマッパー番号を入れて返す
  // @retval     trueのとき最初に貼ってあったテクスチャの中にnameという名前を持つものがある。
  //             trueのときはmaterial_indexとnoに有効な値を入れて返す。falseのときは不定。
  b32 GetMaterialTextureMapperTextureOriginalIndex(s32* material_index, s32* no, const char* name) const;

private:
  void initializeModelTextureInfo(s32 material_index, s32 no);
  void* getModelTexturePhysicalAddress(s32 material_index, s32 no) const;
  u16   getModelTextureWidth(s32 material_index, s32 no) const;
  u16   getModelTextureHeight(s32 material_index, s32 no) const;
  gfl::grp::g3d::TextureFormat getModelTextureFormat(s32 material_index, s32 no) const;
  u8    getModelTextureMipmap(s32 material_index, s32 no) const;

  // TEXTURE_CLASS_TYPE_???を得る
  s32 getMaterialTextureClassType(s32 material_index, s32 no) const;

  // SetMaterialTextureMapperTextureのH3dModelを使う版に渡す引数を得る関数
  const nw::h3d::res::TextureContent* getMaterialTextureMapperTextureAsModelTexture(s32 material_index, s32 no) const;
  const H3dResTexture*                getMaterialTextureMapperTextureAsResTexture(s32 material_index, s32 no) const;
  const H3dDynamicTexture*            getMaterialTextureMapperTextureAsDynamicTexture(s32 material_index, s32 no) const;

public:
  // no  テクスチャマッパー番号(0,1,2,Proc)(テクスチャコーディネータ番号ではない)
  // U方向繰り返し設定を行う。
  void SetMaterialTextureMapperWrapU(s32 material_index, s32 no, H3dTextureSamplerExConst::Wrap wrap);
  // U方向繰り返し設定を得る。
  H3dTextureSamplerExConst::Wrap  GetMaterialTextureMapperWrapU(s32 material_index, s32 no) const;
  // V方向繰り返し設定を行う。
  void SetMaterialTextureMapperWrapV(s32 material_index, s32 no, H3dTextureSamplerExConst::Wrap wrap);
  // V方向繰り返し設定を得る。
  H3dTextureSamplerExConst::Wrap  GetMaterialTextureMapperWrapV(s32 material_index, s32 no) const;
  // テクスチャの座標は、STは使わず、UVで統一しておく。

  // no  テクスチャマッパー番号(0,1,2,Proc)(テクスチャコーディネータ番号ではない)
  // テクスチャサンプラータイプ
  void SetMaterialTextureMapperSamplerType(s32 material_index, s32 no, H3dTextureSamplerExConst::SamplerType sampler_type);
  H3dTextureSamplerExConst::SamplerType GetMaterialTextureMapperSamplerType(s32 material_index, s32 no) const;
  //res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_PROJECTION );  // テクスチャサンプラータイプを設定します。 テクスチャサンプラータイプが設定可能なのは０番目のテクスチャマッパーのみです。 １、２番目に設定すると正しく動作しません。

  // no  テクスチャマッパー番号(0,1,2,Proc)(テクスチャコーディネータ番号ではない)
  // 拡大時の補間処理
  void SetMaterialTextureMapperMagFilter(s32 material_index, s32 no, H3dTextureSamplerExConst::MagFilter mag_filter);
  H3dTextureSamplerExConst::MagFilter GetMaterialTextureMapperMagFilter(s32 material_index, s32 no) const;
  // 縮小時の補間処理
  void SetMaterialTextureMapperMinFilter(s32 material_index, s32 no, H3dTextureSamplerExConst::MinFilter min_filter);
  H3dTextureSamplerExConst::MinFilter GetMaterialTextureMapperMinFilter(s32 material_index, s32 no) const;
  
  // ミップマップ設定
  void SetTextureMipMapLodMinLevel( s32 material_index, s32 no, s32 level );
  void SetTextureMipMapLodBias( s32 material_index, s32 no, s32 bias );

  // no  テクスチャマッパー番号(0,1,2,Proc)(テクスチャコーディネータ番号ではない)
  // ボーダーカラー
  void SetMaterialTextureMapperBorderColor(s32 material_index, s32 no, const gfl::grp::ColorF32& c);
  void GetMaterialTextureMapperBorderColor(s32 material_index, s32 no, gfl::grp::ColorF32* c) const;

  
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  //
  // テクスチャコーディネータ
  //
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  // material_index番マテリアルのno番テクスチャコーディネータ(0<=no<=2)の設定
  // texture_mapper_no番テクスチャマッパーとtexture_coordinator_no番テクスチャコーディネータがあった場合、
  // 「2番テクスチャマッパーなら2番テクスチャコーディネータを使う」というふうに必ずしもなっているわけではない。
  // 「1番テクスチャマッパーと2番テクスチャマッパーが1番テクスチャコーディネータを共有して使う」ということもあり得る。
  // つまりtexture_mapper_no=texture_coordinator_noと必ずなるわけではない、ということに注意して下さい。

  // テクスチャ座標
  void SetMaterialTextureCoordinatorScaleUV(s32 material_index, s32 no, const gfl::math::VEC2& t);
  void GetMaterialTextureCoordinatorScaleUV(s32 material_index, s32 no, gfl::math::VEC2* t) const;
  
  void SetMaterialTextureCoordinatorRotateUV(s32 material_index, s32 no, const f32 t);  // [radian]
  f32 GetMaterialTextureCoordinatorRotateUV(s32 material_index, s32 no) const;  // [radian]
  
  void SetMaterialTextureCoordinatorTranslateUV(s32 material_index, s32 no, const gfl::math::VEC2& t);
  void GetMaterialTextureCoordinatorTranslateUV(s32 material_index, s32 no, gfl::math::VEC2* t) const;

  void SetMaterialTextureCoordinatorReferenceSceneCameraIndex(s32 material_index, s32 no, s32 scene_camera_index);
  s32  GetMaterialTextureCoordinatorReferenceSceneCameraIndex(s32 material_index, s32 no) const;

  // マッピング方法
  void SetMaterialTextureCoordinatorMappingMethod(s32 material_index, s32 no, TextureMappingMethod mapping_method);
  TextureMappingMethod GetMaterialTextureCoordinatorMappingMethod(s32 material_index, s32 no) const;

private:
  nw::h3d::fnd::TextureCoord& getMaterialTextureCoordinator(s32 material_index, s32 no);
  const nw::h3d::fnd::TextureCoord& getMaterialTextureCoordinator(s32 material_index, s32 no) const;
  void enableMaterialTextureCoordinatorFlag(s32 material_index, s32 no);








  ///////////////////////////////////////////
  ///////////////////////////////////////////
  // 
  // テクスチャコンバイナ
  // 
  ///////////////////////////////////////////
  ///////////////////////////////////////////
public: 
  // step段目
  void SetMaterialTextureCombinerCombineRgb(s32 material_index, s32 step, H3dTextureCombinerExConst::Combine combine);
  H3dTextureCombinerExConst::Combine  GetMaterialTextureCombinerCombineRgb(s32 material_index, s32 step);

  // step段目
  void SetMaterialTextureCombinerCombineAlpha(s32 material_index, s32 step, H3dTextureCombinerExConst::Combine combine);
  H3dTextureCombinerExConst::Combine  GetMaterialTextureCombinerCombineAlpha(s32 material_index, s32 step);



  // step段目
  void SetMaterialTextureCombinerScaleRgb(s32 material_index, s32 step, H3dTextureCombinerExConst::Scale scale);
  H3dTextureCombinerExConst::Scale  GetMaterialTextureCombinerScaleRgb(s32 material_index, s32 step);
  
  // step段目
  void SetMaterialTextureCombinerScaleAlpha(s32 material_index, s32 step, H3dTextureCombinerExConst::Scale scale);
  H3dTextureCombinerExConst::Scale  GetMaterialTextureCombinerScaleAlpha(s32 material_index, s32 step);



  // step段目
  // no ソースno  A:ソース0  B:ソース1  C:ソース2
  void SetMaterialTextureCombinerOperandRgb(s32 material_index, s32 step, s32 no, H3dTextureCombinerExConst::OperandRgb ope);
  H3dTextureCombinerExConst::OperandRgb  GetMaterialTextureCombinerOperandRgb(s32 material_index, s32 step, s32 no);
  
  // step段目
  // no ソースno  A:ソース0  B:ソース1  C:ソース2
  void SetMaterialTextureCombinerOperandAlpha(s32 material_index, s32 step, s32 no, H3dTextureCombinerExConst::OperandAlpha ope);
  H3dTextureCombinerExConst::OperandAlpha  GetMaterialTextureCombinerOperandAlpha(s32 material_index, s32 step, s32 no);



  // step段目
  // no ソースno  A:ソース0  B:ソース1  C:ソース2
  void SetMaterialTextureCombinerSourceRgb(s32 material_index, s32 step, s32 no, H3dTextureCombinerExConst::Source source);
  H3dTextureCombinerExConst::Source  GetMaterialTextureCombinerSourceRgb(s32 material_index, s32 step, s32 no);
  
  // step段目
  // no ソースno  A:ソース0  B:ソース1  C:ソース2
  void SetMaterialTextureCombinerSourceAlpha(s32 material_index, s32 step, s32 no, H3dTextureCombinerExConst::Source source);
  H3dTextureCombinerExConst::Source  GetMaterialTextureCombinerSourceAlpha(s32 material_index, s32 step, s32 no);


  // step段目
  // noはデザイナーさんツールのコンスタントカラー"番号"
  // デザイナーさんツールのコンスタントカラー"番号"を設定する
  void SetMaterialTextureCombinerConstant(s32 material_index, s32 step, s32 no);
  // デザイナーさんツールのコンスタントカラー"番号"を取得する
  s32 GetMaterialTextureCombinerConstant(s32 material_index, s32 step) const;


  ///////////////////////////////////////////
  ///////////////////////////////////////////
  // 
  // フラグメントシェーダ
  // 
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  
  // step段目は前段の何を使うか(step段目は後段に何を渡すかではないので注意すること)
  void SetMaterialFragmentShaderBufferInputRgb(s32 material_index, s32 step, H3dFragmentShaderExConst::BufferInput input);
  H3dFragmentShaderExConst::BufferInput  GetMaterialFragmentShaderBufferInputRgb(s32 material_index, s32 step);

  // step段目は前段の何を使うか(step段目は後段に何を渡すかではないので注意すること)
  void SetMaterialFragmentShaderBufferInputAlpha(s32 material_index, s32 step, H3dFragmentShaderExConst::BufferInput input);
  H3dFragmentShaderExConst::BufferInput  GetMaterialFragmentShaderBufferInputAlpha(s32 material_index, s32 step);


  ///////////////////////////////////////////
  ///////////////////////////////////////////
  // 
  // シェーダ(頂点シェーダかジオメトリシェーダ)
  // 
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  // シェーダを設定する
  void SetMaterialShader(s32 material_index, const H3dResShader& res_shader);
  // シェーダの設定を元に戻す
  void ResetMaterialShader(s32 material_index);


  ///////////////////////////////////////////
  ///////////////////////////////////////////
  // 
  // ステンシルテスト
  // 
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  // StencilOperation
  b32 IsMaterialStencilTestEnable(s32 material_index) const;
  void SetMaterialStencilTestEnable(s32 material_index, b32 enable);
  H3dStencilOperationExConst::TestFunc GetMaterialStencilTestFunction(s32 material_index) const;
  void SetMaterialStencilTestFunction(s32 material_index, H3dStencilOperationExConst::TestFunc func_func);

  void SetMaterialStencilTestReference(s32 material_index, u8 ref);
  u8 GetMaterialStencilTestReference(s32 material_index) const;

  // @note GetMaterialStencilTestMaskとSetMaterialStencilTestMaskは、MaskとFuncMask間違えていないか自信なしです。
  //       オーサリングツールのステンシルのマスクは使っていないらしいので、どちらがそれに該当するのか検証していません。
  //       →IDエッジで検証したので使って大丈夫です。
  u8 GetMaterialStencilTestMask(s32 material_index) const;  // @note MaskとFuncMask間違えていないか自信なし？
  void SetMaterialStencilTestMask(s32 material_index, u8 mask);  // @note MaskとFuncMask間違えていないか自信なし？

  H3dStencilOperationExConst::StencilOp GetMaterialStencilTestFailOperation(s32 material_index) const;
  void SetMaterialStencilTestFailOperation(s32 material_index, H3dStencilOperationExConst::StencilOp op_fail);
  H3dStencilOperationExConst::StencilOp GetMaterialStencilTestZFailOperation(s32 material_index) const;
  void SetMaterialStencilTestZFailOperation(s32 material_index, H3dStencilOperationExConst::StencilOp op_zfail);
  H3dStencilOperationExConst::StencilOp GetMaterialStencilTestPassOperation(s32 material_index) const;
  void SetMaterialStencilTestPassOperation(s32 material_index, H3dStencilOperationExConst::StencilOp op_zpass);


  ///////////////////////////////////////////
  ///////////////////////////////////////////
  // 
  // アルファテスト
  // 
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  // AlphaTest
  b32 IsMaterialAlphaTestEnable(s32 material_index) const;
  void SetMaterialAlphaTestEnable(s32 material_index, b32 enable);  // trueのときアルファテスト有効
  H3dAlphaTestExConst::TestFunc GetMaterialAlphaTestFunction(s32 material_index) const;
  void SetMaterialAlphaTestFunction(s32 material_index, H3dAlphaTestExConst::TestFunc func);

  void SetMaterialAlphaTestReference(s32 material_index, f32 ref);
  f32 GetMaterialAlphaTestReference(s32 material_index) const;

  void SetMaterialAlphaTestReferenceU8(s32 material_index, u8 ref);
  u8 GetMaterialAlphaTestReferenceU8(s32 material_index) const;


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // デプステスト
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  // DepthTest
  b32 IsMaterialDepthTestEnable(s32 material_index) const;
  void SetMaterialDepthTestEnable(s32 material_index, b32 enable);  // trueのときデプステスト有効
  b32 IsMaterialDepthTestMaskEnable(s32 material_index) const;
  void SetMaterialDepthTestMaskEnable(s32 material_index, b32 enable);  // trueのときデプスマスク有効(trueのときデプスバッファを更新する)
  H3dDepthTestExConst::TestFunc GetMaterialDepthTestFunction(s32 material_index) const;
  void SetMaterialDepthTestFunction(s32 material_index, H3dDepthTestExConst::TestFunc func);


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // ブレンダー
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  
  // @attention 【注意】ブレンドモードによって設定できたりしなかったりするように制御しているので、まず最初にブレンドモードを変更して下さい。
  void SetMaterialBlendMode(s32 material_index, H3dBlendExConst::Mode mode);
  H3dBlendExConst::Mode GetMaterialBlendMode(s32 material_index) const;

  H3dBlendExConst::LogicOperation GetMaterialBlendLogicOperation(s32 material_index) const;
  void SetMaterialBlendLogicOperation(s32 material_index, H3dBlendExConst::LogicOperation operation);

  H3dBlendExConst::Factor GetMaterialBlendFunctionSourceRgb(s32 material_index) const;
  void SetMaterialBlendFunctionSourceRgb(s32 material_index, H3dBlendExConst::Factor factor);
  H3dBlendExConst::Factor GetMaterialBlendFunctionSourceAlpha(s32 material_index) const;
  void SetMaterialBlendFunctionSourceAlpha(s32 material_index, H3dBlendExConst::Factor factor);

  H3dBlendExConst::Factor GetMaterialBlendFunctionDestinationRgb(s32 material_index) const;
  void SetMaterialBlendFunctionDestinationRgb(s32 material_index, H3dBlendExConst::Factor factor);
  H3dBlendExConst::Factor GetMaterialBlendFunctionDestinationAlpha(s32 material_index) const;
  void SetMaterialBlendFunctionDestinationAlpha(s32 material_index, H3dBlendExConst::Factor factor);

  H3dBlendExConst::Equation GetMaterialBlendEquationRgb(s32 material_index) const;
  void SetMaterialBlendEquationRgb(s32 material_index, H3dBlendExConst::Equation equation);
  H3dBlendExConst::Equation GetMaterialBlendEquationAlpha(s32 material_index) const;
  void SetMaterialBlendEquationAlpha(s32 material_index, H3dBlendExConst::Equation equation);


  ///////////////////////////////////////////
  ///////////////////////////////////////////
  // 
  // ラスタライゼーション
  // 
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  // カリングモード(CreativeStudioのマテリアル全般で設定するポリゴンの表裏描画設定のことです)
  void SetMaterialRasterizationCullingMode(s32 material_index, H3dRasterizationExConst::CullingMode cull);
  H3dRasterizationExConst::CullingMode  GetMaterialRasterizationCullingMode(s32 material_index) const;


  ///////////////////////////////////////////
  ///////////////////////////////////////////
  //
  // マテリアルが影響を受けるフォグ
  //
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  void SetMaterialFogEnable(s32 material_index, b32 enable);
  b32 IsMaterialFogEnable(s32 material_index) const;
  void SetMaterialFogIndex(s32 material_index, s32 index);
  s32  GetMaterialFogIndex(s32 material_index) const;


  ///////////////////////////////////////////
  ///////////////////////////////////////////
  //
  // マテリアルが影響を受けるライトセット
  //
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  void SetMaterialLightSetIndex(s32 material_index, s32 index);
  s32  GetMaterialLightSetIndex(s32 material_index) const;


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // ボーンとボーンマトリクス
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  virtual s32 GetBonesCount(void) const;
  virtual s32 GetBonesIndex(const char* key) const;
  virtual const char* GetBonesName(s32 bone_index) const;
private:
  // 使用禁止
  //virtual s32 GetBonesCountOfWorldMatrixPose(void) const;  // GetBonesCountと同じ値が返ってくると思うが、そうでないときはあるのだろうか？
public:
  virtual b32 GetBoneMatrixOfWorldMatrixPose(s32 bone_index, gfl::math::MTX34* matrix) const;
  virtual gfl::math::MTX34* GetBoneMatrixPointerOfWorldMatrixPose(s32 bone_index);
  virtual const gfl::math::MTX34* GetBoneMatrixPointerOfWorldMatrixPose(s32 bone_index) const;
  //virtual gfl::math::MTX34& GetBoneMatrixReferenceOfWorldMatrixPose(s32 bone_index);
  virtual b32 SetBoneMatrixOfWorldMatrixPose(s32 bone_index, const gfl::math::MTX34& matrix);

public:
  // ローカルマトリクス(テストしていないです)(スケールも欲しいときは別途相談して下さい)
  virtual b32 GetBoneMatrixOfLocalMatrixPose(s32 bone_index, gfl::math::MTX34* matrix) const;
  virtual gfl::math::MTX34* GetBoneMatrixPointerOfLocalMatrixPose(s32 bone_index);  // これで取得したマトリクスを変更したときは、変更の度にSetBoneDirtyFlagOfLocalMatrixPose(s32 bone_index)を呼んで下さい。
  virtual void SetBoneDirtyFlagOfLocalMatrixPose(s32 bone_index);
  virtual const gfl::math::MTX34* GetBoneMatrixPointerOfLocalMatrixPose(s32 bone_index) const;
  virtual b32 SetBoneMatrixOfLocalMatrixPose(s32 bone_index, const gfl::math::MTX34& matrix);


  




  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // ボーン差し替え
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  void CreateReplaceBoneData(gfl::heap::NwAllocator* heap_allocator, H3dModel* base_model);
  void DestroyReplaceBoneData(void);
  void SetWorldReplaceBone(H3dModel* base_model);

  void IgnoreParentReplaceBone(const char* name); 
  void IgnoreChildReplaceBone(const char* name); 
  void IgnoreSelfChildReplaceBone(const char* name);
private:
  // @brief  self_indexボーンの先祖にancestor_indexボーンがある場合trueを返し、ない場合falseを返す。ancestor_indexとself_indexが同じときはfalseを返す。
  b32  isAncestorBone(s32 ancestor_index, s32 self_index);

  //-----------------------------------------------------------------------------------------------------------------------------
  // xy_projectのCBoneEffectModelクラスで使用(高速であることを最優先として作っているためエラーチェックは全く行っていない)
  //-----------------------------------------------------------------------------------------------------------------------------
public:
  inline int GetBoneNumberBEM() const { return m_skel_state.GetBoneStateSize(); } ///< 骨数を取得する
  inline const gfl::math::MTX34& GetWorldMatrixBEM() const { return m_cont->worldMtx; } ///< ワールドマトリクスを取得する
  inline int GetBoneIndexByNameBEM(const char* sName) const { return m_cont->GetBoneIdx(sName); } ///< 骨の名前からインデックスを取得する
  inline const char* GetBoneNameByIndexBEM(const int nIndex) const { return m_cont->bones[nIndex].name; } ///< 骨のインデックスから名前を取得する
  inline const gfl::math::MTX34& GetBoneMatrixByIndexBEM(const int nIndex) const { return m_skel_state.GetWorldMtx(nIndex); } ///< 骨のインデックスからマトリクスを取得する
  inline int GetParentBoneIndexByIndexBEM(const int nIndex) const { return m_cont->bones[nIndex].parentIdx; } ///< インデックスから親骨のインデックスを取得する
  inline void SetBoneMatrixByIndexNotTransmitBEM(const int nIndex, const gfl::math::MTX34& inMat) { gfl::math::MTX34Copy(&(m_skel_state.GetWorldMtx(nIndex)), &inMat); } ///< インデックスに対応する骨にマトリクスを設定する(子骨への伝搬は行われない)

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // メッシュ
  // (メッシュノードとは違うので注意)
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  virtual s32 GetMeshCount(void) const;
  virtual s32 GetMeshIndex(const char* name) const;  // メッシュの名前が取れないのでなし
  virtual const char* GetMeshName(s32 mesh_index) const;  // メッシュの名前が取れないのでなし

  //-----------------------------------------------------------------------------
  /**
   *  @brief         メッシュが表示されているか
   *  @param[in]     mesh_index  メッシュのインデックス
   *  @retval        メッシュが表示されているときtrue
   */
  //-----------------------------------------------------------------------------
  virtual b32 IsMeshVisible(const s32 mesh_index) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         メッシュの表示/非表示を切り替える
   *
   *                 メッシュノードビジビリティアニメーションが扱っているフラグと同じフラグを操作するので、
   *                 アニメーションで表示/非表示を切り替えている場合は注意して下さい。
   *
   *  @param[in]     mesh_index  メッシュのインデックス
   *  @param[in]     visible     メッシュを表示するときtrue
   */
  //-----------------------------------------------------------------------------
  virtual void SetMeshVisible(const s32 mesh_index, const b32 visible);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         メッシュのマテリアルを得る
   *
   *  @param[in]     mesh_index  メッシュのインデックス
   *  @retval        マテリアルインデックス
   */
  //-----------------------------------------------------------------------------
  virtual s32 GetMaterialIndexOfMesh(const s32 mesh_index) const;



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // メッシュノード
  // (メッシュとは違うので注意)
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  virtual s32 GetMeshNodeCount(void) const;
  virtual s32 GetMeshNodeDicCount(void) const;  // メッシュノードの名前テーブルの要素数。GetMeshNodeCountの戻り値とは異なることがあるので注意。
  virtual s32 GetMeshNodeIndex(const char* name) const;  // メッシュノードの名前は取れる
  virtual const char* GetMeshNodeName(s32 mesh_node_index) const;  // メッシュノードの名前は取れる
  virtual b32 IsMeshNodeVisible(const s32 mesh_node_index) const;
  virtual void SetMeshNodeVisible(const s32 mesh_node_index, const b32 visible);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // シェーダシンボル
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  b32 CreateShaderExState(
      gfl::heap::NwAllocator* heap_allocator, 
      gfl::heap::NwAllocator* device_allocator, 
      s32 material_index,
      const H3dShaderExState::Description* desc
  );

  // valueはこの関数から戻ったら呼び出し元は破棄してもよい
  void SetShaderSymbolVSFloat(s32 material_index, const char* name, const gfl::math::VEC4& value);  // なるべくindex版を使って下さい。
  void SetShaderSymbolVSFloat(s32 material_index, s32 index, const gfl::math::VEC4& value);  // CreateShaderExStateのDescriptionで渡したshader_symbol_vs_float_name_listのインデックス

  void GetShaderSymbolVSFloat(s32 material_index, const char* name, gfl::math::VEC4* value) const;  // なるべくindex版を使って下さい。
  void GetShaderSymbolVSFloat(s32 material_index, s32 index, gfl::math::VEC4* value) const;  // CreateShaderExStateのDescriptionで渡したshader_symbol_vs_float_name_listのインデックス
  
  s32 GetShaderSymbolIndex(s32 material_index, const char* name) const;
  
  void SetVSBool(s32 material_index, s32 boolIndex, b32 value );

  // 確認用
  void DumpShaderSymbolName(s32 material_index) const;



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // アニメーション
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションマネージャ生成
   */
  //-----------------------------------------------------------------------------
  b32 CreateAnimationManagerAndAnimationSlot(gfl::heap::NwAllocator* heap_allocator, s32 slot_num, const H3dAnimationSlot::Description* slot_desc_array);
  b32 CreateAnimationManager(gfl::heap::NwAllocator* heap_allocator, s32 slot_num);
  virtual b32 CreateAnimationSlot(gfl::heap::NwAllocator* heap_allocator, s32 slot_index, const H3dAnimationSlot::Description* slot_desc);
  virtual b32 IsAnimationSlotCreated(s32 slot_index) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションマネージャ破棄
   */
  //-----------------------------------------------------------------------------
  void DestroyAnimationManager(void);
  void DestroyAnimationSlot(s32 slot_index);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーション変更
   */
  //-----------------------------------------------------------------------------
  virtual H3dAnimationSlot::ChangeAnimationResult ChangeAnimationByResAnim(s32 slot_index, H3dResAnim* res_anim);
  virtual H3dAnimationSlot::ChangeAnimationResult ChangeAnimationByResAnimListIndex(s32 slot_index, s32 list_index);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         今設定されているアニメーションを得る
   *  @param[in]     slot_index  スロット
   *  @retval        アニメーションが設定されていないときはNULLを返す
   *                 H3dResAnimでないアニメーションが設定されているときもNULLを返す
   */
  //-----------------------------------------------------------------------------
  virtual H3dResAnim* GetAnimationAsResAnim(s32 slot_index) const;
  // @note もしアニメーションクラスをResAnim以外にもつくったら、
  // この関数によってアニメの有無判定をしているところに影響が出る。
  // だから、アニメーションクラスを増やしたら、使っている箇所をよく確認すること。

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションのフレームを設定する
   *  @param[in]     frame           フレーム
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationFrame(f32 frame, s32 slot_index = 0);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションのフレームを取得する
   *  @retval        フレーム
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationFrame(s32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの更新フレームを設定する
   *                 0.0fで停止、1.0fで等倍、2.0fで2倍速、-1.0fで逆等倍再生
   *
   *  @param[in]     step_frame      更新フレーム
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationStepFrame(f32 step_frame, s32 slot_index = 0);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの更新フレームを取得する
   *  @retval        更新フレーム
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationStepFrame(s32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの開始フレームを設定する
   *  @param[in]     frame           フレーム
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationStartFrame(f32 start_frame, s32 slot_index = 0);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの開始フレームを取得する
   *  @retval        フレーム
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationStartFrame(s32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの終了フレームを設定する
   *  @param[in]     frame           フレーム
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationEndFrame(f32 end_frame, s32 slot_index = 0);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの終了フレームを取得する
   *  @retval        フレーム
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationEndFrame(s32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの開始フレームと終了フレームを設定する
   *  @param[in]     start_frame     開始フレーム
   *  @param[in]     end_frame       終了フレーム
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationStartAndEndFrame(f32 start_frame, f32 end_frame, s32 slot_index = 0);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの現在のフレームが開始フレームか否か判定する
   *  @retval        bool    現在のフレームが開始フレームの場合tureを返す
   */
  //-----------------------------------------------------------------------------
  virtual b32 IsAnimationFrameStartFrame(s32 slot_index = 0) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの現在のフレームが終了フレームか否か判定する
   *  @retval        bool    現在のフレームが終了フレームの場合tureを返す
   */
  //-----------------------------------------------------------------------------
  virtual b32 IsAnimationFrameEndFrame(s32 slot_index = 0) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの現在のフレームが
   *                     step_frame>0のときは終了フレームのときtrueを返す
   *                     step_frame<0のときは開始フレームのときtrueを返す
   *                     step_frame==0のときは開始フレームか終了フレームのときtrueを返し、それ以外のときfalseを返す
   *
   *  @retval        bool    現在のフレームがstep_frameから見て端フレームとなっている場合tureを返す
   */
  //-----------------------------------------------------------------------------
  virtual b32 IsAnimationFrameStepTerminalFrame(s32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションがループ再生か否か取得する
   *                 今slot_indexに設定されているH3dResAnimのループ状態を取得するだけです。
   *  @retval        bool            ループのときtrue、ループでないときfalseを返す
   */
  //-----------------------------------------------------------------------------
  virtual b32 IsAnimationLoop(s32 slot_index = 0) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションにループ再生を設定する
   *                 今slot_indexに設定されているH3dResAnimのループ状態を変更します。
   *  @param[in]     loop            ループのときtrue、ループでないときfalseを設定する
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationLoop(b32 loop, s32 slot_index = 0);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // コールバック
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  // @brief   メッシュ描画開始時のイベントを観察するリスナーを追加する
  // @retval  追加できたときtrueを返す
  b32 AddPreMeshDrawListener(H3dMeshListener* listener);
  // @brief  メッシュ描画開始時のイベントを観察するリスナーを取り除く
  void RemovePreMeshDrawListener(H3dMeshListener* listener);
  
  // @brief   メッシュ描画終了時のイベントを観察するリスナーを追加する
  // @retval  追加できたときtrueを返す
  b32 AddPostMeshDrawListener(H3dMeshListener* listener);
  // @brief  メッシュ描画終了時のイベントを観察するリスナーを取り除く
  void RemovePostMeshDrawListener(H3dMeshListener* listener);

private:
  b32 addMeshListener(H3dMeshListener*& head, H3dMeshListener* listener);
  void removeMeshListener(H3dMeshListener*& head, H3dMeshListener* listener);
  void callMeshListenerInvoke(H3dMeshListener*& head, s32 material_index, H3dCommand* command, H3dEnvironment* env);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // コマンド発行
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  // @brief  マテリアルコマンドを発行する
  //         コマンドバッファジャンプではなく実際に発行する
  //         コマンドバッファジャンプ版は用意していない
  void MakeMaterialCommand(
      H3dCommand* command,
      H3dEnvironment* env,
      s32 material_index,
      bool fragmentAndWBufferDirty = false
  );
  
  //----------------------------------------------------------------------
  /**
   * @brief MaterialCommandのキャッシュ作成
   * @param device_allocator
   */
  //----------------------------------------------------------------------
  void MakeMaterialCommandCache(gfl::heap::NwAllocator* device_allocator);
  
  //----------------------------------------------------------------------
  /**
   * @brief MaterialCommandのキャッシュ解放。明示的に行う場合は、１フレ遅れ解放に気を付けて。
   */
  //----------------------------------------------------------------------
  void ReleaseMaterialCommandCache();

private:
  // @brief  シェーダコマンドを発行する
  void makeShaderCommand(
      H3dCommand* command,
      H3dEnvironment* env,
      s32 material_index
  );


public:
  const nw::h3d::res::ModelContent* GetNwModelContent(void) const
  {
    return m_cont;
  }  
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // friendには公開している非公開メンバ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
private:
  
  nw::h3d::SkeletonState* GetNwSkeletonState(void) //ここにconstを付けると何故かコンパイルエラーになる
  {
    return &m_skel_state;
  }
  nw::h3d::MaterialStateArray* GetNwMaterialStateArray(void) //ここにconstを付けると何故かコンパイルエラーになる
  {
    return &m_mate_state_array;
  }
  nw::h3d::VisibilityState* GetNwVisibilityState(void) //ここにconstを付けると何故かコンパイルエラーになる
  {
    return &m_visi_state;
  }



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // friendにも公開していない非公開メンバ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
private:
  //-----------------------------------------------------------------------------
  /// @brief メンバ初期化
  //-----------------------------------------------------------------------------
  void initParam();
  
  //-----------------------------------------------------------------------------
  /// @brief メッシュ描画
  //-----------------------------------------------------------------------------
  void drawMesh(
      H3dCommand* command,
      H3dEnvironment* env,
      const nw::h3d::res::MeshData& mesh,
      const nn::math::MTX34& normal
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
      ,
      const gfl::math::MTX34&  view_mtx,
      const gfl::math::MTX34&  inv_view_mtx,
      const u32                inv_exist
#endif
  );

  // シェーダーシンボルのバインドを設定する
  void setShaderBind(s32 material_index, const nw::h3d::res::ShaderContent* shader_content);

  // 座標変換計算
  void calculateTransform(void);

  // スケルタルモデルのメッシュのZ深度デプスを求める
  f32 calculateDepthOfSkeletalModel(
    const nw::h3d::res::MeshData& mesh,
    const nw::gfx::Camera&        camera
  );
  // (スケルタルモデルでない)モデルのメッシュのZ深度デプスを求める
  f32 calculateDepth(
    const nw::h3d::res::MeshData& mesh,
    const nw::gfx::Camera&        camera
  );

  // レンダーキューにモデルのメッシュを積む
  void enqueue(
    nw::gfx::RenderQueue::ElementListType* list,
    const nw::gfx::Camera& camera
  );

  //! @brief フォグのコマンドを生成します。
  //!
  //! 現在設定されているフォグから変更がない場合は、生成する必要はありません。
  //!
  //! 作成したコマンドのフォグ ID 値を返します。
  u16 makeFogCommand(
      nn::gr::CTR::CommandBufferJumpHelper& dcs,
      const nw::h3d::MaterialState& materialState,
      const nw::h3d::SceneState& sceneState,
      u16 currentFogId,
      const H3dEnvironment& env) const;
  
  void SetFragmentAndWBufferDirty( b32 f ){ m_FragmentAndWBufferDirty = f; }

private:
  // シェーダーシンボルのバインド
  struct ShaderBind
  {
    nn::gr::BindSymbolVSFloat worldMtx;
    nn::gr::BindSymbolVSFloat normalMtx;
    nn::gr::BindSymbolVSFloat projMtx;
    nn::gr::BindSymbolVSFloat viewMtx;
    nn::gr::BindSymbolVSFloat univMtx;
    nn::gr::BindSymbolVSInteger lightCt;//@fix 渕野さん要望：バトル背景のライティング対応

    // 頂点ライティング
    nn::gr::BindSymbolVSFloat hslGCol;
    nn::gr::BindSymbolVSFloat hslSCol;
    nn::gr::BindSymbolVSFloat hslSDir;

    nn::gr::BindSymbolVSFloat matAmbi;
    nn::gr::BindSymbolVSFloat matDiff;
  };
  typedef nw::h3d::fnd::DataArray<ShaderBind> ShaderBindArray;

  // ビットフラグ
  enum
  {
    BIT_FLAG_NONE              = 0,
    BIT_FLAG_VISIBLE           = 1 <<  0,
    BIT_FLAG_TREE_FLAG         = 1 <<  1,
    BIT_FLAG_ENABLE_ROTATE_VEC = 1 <<  2,
    BIT_FLAG_MAKE_DRAW_CASH    = 1 <<  3,       ///< 描画時にキャッシュを生成
    BIT_FLAG_DEL_MEM_CONTROL   = 1 <<  4,       ///< 破棄時にメモリマネージャにメモリを登録するか

    BIT_FLAG_DEFAULT           = BIT_FLAG_VISIBLE | BIT_FLAG_TREE_FLAG | BIT_FLAG_ENABLE_ROTATE_VEC
  };

  // テクスチャ情報
  enum
  {
    TEXTURE_CLASS_TYPE_NONE     = 0,  // テクスチャなし
    TEXTURE_CLASS_TYPE_MODEL    = 1,  // モデルをつくったときに入っていたテクスチャ  // モデルをつくったときに入っていたということでModelとしている(ModelTexture)
    TEXTURE_CLASS_TYPE_RES      = 2,  // H3dResTexture
    TEXTURE_CLASS_TYPE_DYNAMIC  = 3,  // H3dDynamicTexture

    TEXTURE_CLASS_TYPE_SHIFT  = 8  // 8ビットずつシフトする
  };
  struct TextureInfo
  {
    u32   texture_class_type;   // TEXTURE_CLASS_TYPE_SHIFTしてTEXTURE_CLASS_TYPE_???を入れておく。|空き|2|1|0|
    const void* ptr[H3dTextureSamplerExConst::TEXTURE_NUM];
        // TEXTURE_CLASS_TYPE_NONEのときNULL
        // TEXTURE_CLASS_TYPE_MODELのときnw::h3d::res::TextureContentへのポインタ
        // TEXTURE_CLASS_TYPE_RESのときH3dResTextureインスタンスへのポインタ
        // TEXTURE_CLASS_TYPE_DYNAMICのときH3dDynamicTextureインスタンスへのポインタ

    TextureInfo(void)
      : texture_class_type(0)
    {
      for(s32 no=0; no<H3dTextureSamplerExConst::TEXTURE_NUM; ++no)
      {
        ptr[no] = NULL;
      }
    }
    // no  テクスチャマッパー番号(0,1,2,Proc)(テクスチャコーディネータ番号ではない)
    void SetTextureClassTypeAndPtr(s32 no, s32 l_type, const void* l_ptr)
    {
      texture_class_type &= (~(0xFF<<(no*TEXTURE_CLASS_TYPE_SHIFT)));  // まず0を入れてから
      texture_class_type |= (l_type<<(no*TEXTURE_CLASS_TYPE_SHIFT));   // 実際の値を入れる
      ptr[no] = l_ptr;
    }
    s32 GetTextureClassType(s32 no) const
    {
      return ( ( texture_class_type >> (no*TEXTURE_CLASS_TYPE_SHIFT) ) & 0xFF );
    }
    const nw::h3d::res::TextureContent* GetPtrAsModel(s32 no) const  // モデルをつくったときに入っていたということでModelとしている(ModelTexture)
    {
      return reinterpret_cast<const nw::h3d::res::TextureContent*>(ptr[no]);
    }
    const H3dResTexture* GetPtrAsRes(s32 no) const
    {
      return reinterpret_cast<const H3dResTexture*>(ptr[no]);
    }
    const H3dDynamicTexture* GetPtrAsDynamic(s32 no) const
    {
      return reinterpret_cast<const H3dDynamicTexture*>(ptr[no]);
    }
  };


private:
  // 差し替え用のボーンのデータ
  struct ReplaceBoneData
  {
    s32 base_index;           // これと同じボーンのbaseでのインデックス
    s32 base_ancestor_index;  // base_indexがgfl::grp::g3d::H3dModel::INDEX_NONEのときのみ有効な値が入っており、それはbaseのボーンの中で先祖に当たるボーンのインデックス
                              // base_indexが0以上の値もしくはgfl::grp::g3d::H3dModel::INDEX_IGNOREのとき、base_ancestor_indexには何が入っているか不明
        // (base_index==gfl::grp::g3d::H3dModel::INDEX_NONE&&base_ancestor_index==gfl::grp::g3d::H3dModel::INDEX_NONE)のとき、baseのhumanトップに追従する
        
    ReplaceBoneData(void)
      : base_index(gfl::grp::g3d::H3dModel::INDEX_NONE),
        base_ancestor_index(gfl::grp::g3d::H3dModel::INDEX_NONE)
    {}
  };

public:
  static void ResetBeforeMaterialSetting()
  {
    s_LastMaterialData.Reset();
    s_GeometryShaderSetUpCheck = false;
  } 
  
  void SetWorldMatrix( const nn::math::MTX34 &mat ){ m_world_mtx = mat; }
  void SetNormalMatrix( const nn::math::MTX34 &mat ){ m_normal_mtx = mat; }
  
  nn::math::MTX34 GetWorldMatrix(){ return m_world_mtx; }
  nn::math::MTX34 GetNormalMatrix(){ return m_normal_mtx; }
  
  b32 IsSkeltalModel()
  {
    return (m_cont->flags & nw::h3d::ModelFlag::IS_SKELETAL_MODEL) ? true : false;
  }
  
  static bool                                 s_DebugFlag;
private:
  struct LastMaterialData{
    void Reset()
    {
      m_Content = NULL;
      m_LightSetId = -1;
      m_FogId = -1;
    }
    const nw::h3d::res::MaterialContent   *m_Content;
    s32                                   m_LightSetId;
    s32                                   m_FogId;
  };
  
  static LastMaterialData     s_LastMaterialData;
  static b32                  s_GeometryShaderSetUpCheck;
  
  const H3dResource*                m_res;         // 他で生成したリソースのポインタを覚えておくだけ
  s32                               m_cont_index;  // m_res内のコンテントのインデックス
  const nw::h3d::res::ModelContent* m_cont;        // m_res内のコンテント

  class MemoryObject : public H3dModelMemoryManager::IMemortObject
  {
    friend class H3dModel;
  public:
    MemoryObject();
    virtual ~MemoryObject();

#if GFL_DEBUG    
    void SetDrawFrameCount( u32 cnt ){ m_DrawFrameCount = cnt; }
    u32 GetDrawFrameCount(){ return m_DrawFrameCount; }
    
    void SetModelName( const char* pName );
    const char* GetModelName(void) const { return m_ModelName; }
    
  private:
    u32                                         m_DrawFrameCount;
    u32                                         m_HeapSize;
    u32                                         m_DeviceSize;
    char                                        m_ModelName[32];
#endif
  private:
    void* m_state_heap_mem;
    void* m_state_device_mem;
    gfl::grp::util::DrawUtil::CacheCommand      *m_MaterialCommandChaches;
    
  };
  
  MemoryObject                 *m_pMemoryObject;
  
#if GFL_DEBUG
private:
  static b32 s_SafeDeleteCheckEnable;
  static b32 s_SafeDeleteCheckEnableUserSet;  // Force終了後にユーザ設定値に戻せるように記憶しておく。
  static b32 s_SafeDeleteCheckForce;          // trueのときs_SafeDeleteCheckEnableをtrueで固定する。
  static b32 s_SafeDeleteCheckAssert;         // trueのときASSERTで止める。falseのときPRINTで済ませる。
  
public:  
  void SetDrawFrameCount( u32 cnt ){ m_pMemoryObject->SetDrawFrameCount(cnt); }
  u32 GetDrawFrameCount(){ return m_pMemoryObject->GetDrawFrameCount(); }

  //----------------------------------------------------------------
  /**
   * @brief 安全にモデルが解放できたかを確認する機能の有効無効設定
   * @param fEnable
   */
  //----------------------------------------------------------------
  static void SafeDeleteCheckEnable( b32 fEnable )
  {
    s_SafeDeleteCheckEnable        = fEnable;
    s_SafeDeleteCheckEnableUserSet = fEnable;
    if(s_SafeDeleteCheckForce) s_SafeDeleteCheckEnable = true;
  }
  static b32 IsSafeDeleteCheckEnable(){ return s_SafeDeleteCheckEnable; }

  static void SafeDeleteCheckForce(b32 fForce)
  {
    s_SafeDeleteCheckForce = fForce;
    if(s_SafeDeleteCheckForce)
    {
      s_SafeDeleteCheckEnable = true;
    }
    else
    {
      s_SafeDeleteCheckEnable = s_SafeDeleteCheckEnableUserSet;
    }
  }
  static b32 IsSafeDeleteCheckForce(void) { return s_SafeDeleteCheckForce; }

  static void SafeDeleteCheckAssert(b32 fAssert){ s_SafeDeleteCheckAssert = fAssert; }
  static b32  IsSafeDeleteCheckAssert(void) { return s_SafeDeleteCheckAssert; }
#endif    


#if GFL_DEBUG
private:
  static b32 s_DeleteMemoryControlEnable;  // falseのとき、外部からdelete_memory_controlをtrueに設定しても、必ずdelete_memory_controlをfalseとする。
  static b32 s_DeleteMemoryControlAssert;   // trueのとき、外部からdelete_memory_controlをtrueに設定していたらASSERTで止める。falseのときPRINTで済ませる。
public:
  static void DeleteMemoryControlEnable(b32 fEnable) { s_DeleteMemoryControlEnable = fEnable; }
  static b32  IsDeleteMemoryControlEnable(void) { return s_DeleteMemoryControlEnable; }
  static void DeleteMemoryControlAssert(b32 fAssert) { s_DeleteMemoryControlAssert = fAssert; }
  static b32  IsDeleteMemoryControlAssert(void) { return s_DeleteMemoryControlAssert; }
#endif    
  
private:  
  nw::h3d::MaterialStateArray  m_mate_state_array;
  nw::h3d::SkeletonState       m_skel_state;
  nw::h3d::VisibilityState     m_visi_state;

  gfl::grp::ColorU8** m_mate_constant_color_array;  // コンスタントカラー。m_mate_constant_color_array[マテリアル数][CONSTANT_COLOR_NUM]となるようにしておく。
                                                    // nw::h3d::MaterialState::SetCombiner"番号"Constantが、コンバイナ"番号"用コンスタントカラーであり、
                                                    // デザイナーさんツールのコンスタントカラー"番号"ではないので、
                                                    // デザイナーさんツールと同じ見方のコンスタントカラー"番号"を用意しておく。
  
  H3dMaterialExState*  m_mate_ex_state_array;

  ShaderBindArray m_shader_bind_array;

  MaterialTranslucencyKind* m_mate_translucency_kind_array;  // マテリアルの描画レイヤー。m_mate_translucency_kind_array[マテリアル数]となるようにしておく。
  const nw::h3d::res::ShaderContent** m_shader_cont_array;  // 現在使っているシェーダ。m_shader_cont_array[マテリアル数]となるようにしておく。
  TextureInfo* m_texture_info_array;  // m_texture_info_array[マテリアル数]となるようにしておく。 
      // @note あんまり細かく分けてメモリを取らずに、マテリアル数で取るって分かっているものは構造体にしてまとめてとってしまいたい。

  nn::math::MTX34 m_world_mtx;
  nn::math::MTX34 m_normal_mtx;

  bit32  m_bit_flag;

  gfl::math::VEC3  m_scale_vec;
  gfl::math::VEC3  m_rotate_vec;  // [radian]
  gfl::math::MTX34 m_rotate_mtx;
  gfl::math::VEC3  m_translate_vec;

  H3dAnimationManager*  m_animation_manager;
  
  Scene* m_scene;  // 所属しているSceneのポインタを覚えておくだけ

  H3dMeshListener* m_pre_mesh_draw_listener_head;
  H3dMeshListener* m_post_mesh_draw_listener_head;

  u8                  m_model_layer_id;  // 0 <= u8 <= 127 (メンバ変数にはこの範囲の数字で持っておく。使うときに1ビットずらす)
  b32                 m_FragmentAndWBufferDirty;

  // ボーン差し替え
  ReplaceBoneData*  m_replace_bone_data_array;  // m_replace_bone_data_array[ボーン数]

  u32* m_mate_proj_array;
  b32           m_fVisible;
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE



#endif // __GFL_G3DH3DMODEL_H__

