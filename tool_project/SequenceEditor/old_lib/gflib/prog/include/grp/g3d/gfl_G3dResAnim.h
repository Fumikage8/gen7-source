#ifndef __GFL_G3DRESANIM_H__
#define __GFL_G3DRESANIM_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dResAnim.h
 *	@brief  3Dアニメーションリソース
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/anim.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dObject.h>
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dUserData.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3Dアニメーションリソースクラス
//=====================================
class ResAnim : public Object
{
  GFL_FORBID_COPY_AND_ASSIGN(ResAnim);

  // フレンドクラス
  friend class Animation;
  friend class Resource;


public:
  // アニメーションリソースのタイプ
  enum Type  // @note gfl::grp::g3d::AnimationTypeだけにしたいので、いずれ消す。
  {
    TYPE_NONE        = ANIMATION_TYPE_NONE,
    TYPE_SKELETAL    = ANIMATION_TYPE_SKELETAL,
    TYPE_MATERIAL    = ANIMATION_TYPE_MATERIAL,
    TYPE_VISIBILITY  = ANIMATION_TYPE_VISIBILITY,
    TYPE_CAMERA      = ANIMATION_TYPE_CAMERA,
    TYPE_LIGHT       = ANIMATION_TYPE_LIGHT
  };

  // メンバーアニメーションの型
  enum MemberAnimType
  {
    MEMBER_ANIM_TYPE_FLOAT             = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_FLOAT,
    MEMBER_ANIM_TYPE_INT               = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_INT,
    MEMBER_ANIM_TYPE_BOOL              = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_BOOL, 
    MEMBER_ANIM_TYPE_VECTOR2           = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_VECTOR2,  
    MEMBER_ANIM_TYPE_VECTOR3           = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_VECTOR3,   
    MEMBER_ANIM_TYPE_TRANSFORM         = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_TRANSFORM,  
    MEMBER_ANIM_TYPE_RGBA_COLOR        = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_RGBA_COLOR,  
    MEMBER_ANIM_TYPE_TEXTURE           = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_TEXTURE,  
    MEMBER_ANIM_TYPE_BAKED_TRANSFORM   = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_BAKED_TRANSFORM,
    MEMBER_ANIM_TYPE_FULL_BAKED        = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_FULL_BAKED,     
    MEMBER_ANIM_TYPE_INVALID           = -2
  };

  // 専用のResCameraAnimクラスをつくるかどうか迷うもの
  // ResCameraAnimのカメラ行列のアップデートタイプ
  enum ViewUpdaterKind
  {
    VIEW_UPDATER_KIND_NONE     = -1,
    VIEW_UPDATER_KIND_AIM      = nw::anim::res::ResCameraAnimData::VIEW_UPDATER_AIM,
    VIEW_UPDATER_KIND_LOOKAT   = nw::anim::res::ResCameraAnimData::VIEW_UPDATER_LOOKAT,
    VIEW_UPDATER_KIND_ROTATE   = nw::anim::res::ResCameraAnimData::VIEW_UPDATER_ROTATE
  };


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *
   *  @param[in]     allocator          NULL  ←allocatorは使わなくなりました。
   *  @param[in]     resource           リソース
   *  @param[in]     index_in_resource  リソース内におけるインデックス
   *  @param[in]     type               アニメーションリソースのタイプ
   */
  //-----------------------------------------------------------------------------
  // void Resource::GetResAnim(const AnimationResourceType type, const s32 index_in_resource, ResAnim* res_anim) const; を使用して下さい。
  GFL_GRP_G3D_DEPRECATED_FUNCTION( virtual void Create(
      gfl::heap::NwAllocator*           allocator, 
      Resource*                         resource,
      s32                               index_in_resource,
      Type                              type               = TYPE_SKELETAL
  ) );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         破棄
   *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションリソースのタイプを得る
   *
   *  @retval        Type    アニメーションリソースのタイプ
   */
  //-----------------------------------------------------------------------------
  Type GetType(void) const  {  return m_type;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 Createは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  ResAnim(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   *                 Destroyを呼びます。
   */
  //-----------------------------------------------------------------------------
  virtual ~ResAnim();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ResAnimインスタンスを安全にコピーします。
   *
   *                 コピー元は破棄しても大丈夫ですが、コピー元が指していたResourceは破棄しないで下さい。
   *                 ResAnimを継承したクラスのことは考えられていませんので、
   *                 継承したクラスをつくった場合はそれに合わせたCopy関数をつくって下さい。
   *                 オブジェクトIDはコピー元と違ったものになります。
   *
   *  @param[in]  src  コピー元
   */
  //-----------------------------------------------------------------------------
  void Copy(const ResAnim& src);


public:
  //-----------------------------------------------------------------------------
  /*
   *    メンバーアニメーションの情報
   */
  //-----------------------------------------------------------------------------
  //-------------------------------------
  // 共通
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ResAnimの名前を取得する
   *
   *  @retval        const char*    名前
   */
  //-----------------------------------------------------------------------------
  const char* GetName(void) const; 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         フレーム数を取得する
   *
   *  @retval        f32   フレーム数
   */
  //-----------------------------------------------------------------------------
  f32 GetFrameSize(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         メンバーアニメーションの要素数を取得する
   *
   *  @retval        s32    メンバーアニメーションの要素数
   */
  //-----------------------------------------------------------------------------
  s32 GetMemberAnimSetCount(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         メンバーアニメーションの要素のインデックス番号から名前を取得する
   *
   *  @param[in]     member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *
   *  @retval        const char*    メンバーアニメーションの要素の名前
   */
  //-----------------------------------------------------------------------------
  const char* GetMemberAnimSetPath(const s32 member_anim_set_index) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         名前からメンバーアニメーションの要素のインデックス番号を取得する
   *
   *  @param[in]     key    メンバーアニメーションの要素の名前
   *
   *  @retval        s32    メンバーアニメーションの要素のインデックス番号
   */
  //-----------------------------------------------------------------------------
  s32 GetMemberAnimSetIndex(const char* key) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         メンバーアニメーションの型を取得する
   *
   *  @param[in]     member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *
   *  @retval        MemberAnimTypes    メンバーアニメーションの型
   */
  //-----------------------------------------------------------------------------
  MemberAnimType GetMemberAnimType(const s32 member_anim_set_index) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_FLOAT
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     アニメーション結果の値を得る
   *
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[out]   value_valid              正常な値のときtrue
   *
   *  @retval    frameにおけるアニメーション結果の値
   *             正常に取得できなかった場合、戻り値は不定 
   */
  //-----------------------------------------------------------------------------
  f32 GetFloatValue(const s32 member_anim_set_index, const f32 frame, b32* value_valid=NULL) const;
  f32 GetFloatValue(const s32 member_anim_set_index, const s32 frame, b32* value_valid=NULL) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_INT
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     アニメーション結果の値を得る
   *
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[out]   value_valid              正常な値のときtrue 
   *
   *  @retval    frameにおけるアニメーション結果の値
   *             正常に取得できなかった場合、戻り値は不定 
   */
  //-----------------------------------------------------------------------------
  s32 GetIntValue(const s32 member_anim_set_index, const f32 frame, b32* value_valid=NULL) const;
  s32 GetIntValue(const s32 member_anim_set_index, const s32 frame, b32* value_valid=NULL) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_BOOL
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     アニメーション結果の値を得る
   *
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[out]   value_valid              正常な値のときtrue 
   *
   *  @retval    frameにおけるアニメーション結果の値
   *             正常に取得できなかった場合、戻り値は不定 
   */
  //-----------------------------------------------------------------------------
  b32 GetBoolValue(const s32 member_anim_set_index, const f32 frame, b32* value_valid=NULL) const;
  b32 GetBoolValue(const s32 member_anim_set_index, const s32 frame, b32* value_valid=NULL) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_VECTOR2
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     アニメーション結果の値を得る
   *
   *  @param[out]   value                    frameにおけるアニメーション結果の値を返す
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[out]   value_valid              正常な値のときtrue(0はxの判定、1はyの判定)
   *
   *  @retval    正常に取得できたときtrueを返す
   *             trueが返ってきたときでもベクトルの各要素に正常な値があるとは限りませんので、各要素の判定はvalue_validで行って下さい。
   */
  //-----------------------------------------------------------------------------
  b32 GetVector2Value(gfl::math::VEC2* value, const s32 member_anim_set_index, const f32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL) const;
  b32 GetVector2Value(gfl::math::VEC2* value, const s32 member_anim_set_index, const s32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     アニメーション結果の値を得る
   *
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[in]    element                  0ときxの値を取得する、1のときyの値を取得する
   *  @param[out]   value_valid              正常な値のときtrue 
   *
   *  @retval    frameにおけるアニメーション結果の値
   *             正常に取得できなかった場合、戻り値は不定 
   */
  //-----------------------------------------------------------------------------
  f32 GetVector2Value(const s32 member_anim_set_index, const f32 frame, const u32 element, b32* value_valid=NULL) const;
  f32 GetVector2Value(const s32 member_anim_set_index, const s32 frame, const u32 element, b32* value_valid=NULL) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_VECTOR3
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     アニメーション結果の値を得る
   *
   *  @param[out]   value                    frameにおけるアニメーション結果の値を返す
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[out]   value_valid              正常な値のときtrue(0はxの判定、1はyの判定、2はzの判定)
   *
   *  @retval    正常に取得できたときtrueを返す
   *             trueが返ってきたときでもベクトルの各要素に正常な値があるとは限りませんので、各要素の判定はvalue_validで行って下さい。
   */
  //-----------------------------------------------------------------------------
  b32 GetVector3Value(gfl::math::VEC3* value, const s32 member_anim_set_index, const f32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL) const;
  b32 GetVector3Value(gfl::math::VEC3* value, const s32 member_anim_set_index, const s32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     アニメーション結果の値を得る
   *
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[in]    element                  0ときxの値を取得する、1のときyの値を取得する、2のときzの値を取得する
   *  @param[out]   value_valid              正常な値のときtrue 
   *
   *  @retval    frameにおけるアニメーション結果の値
   *             正常に取得できなかった場合、戻り値は不定 
   */
  //-----------------------------------------------------------------------------
  f32 GetVector3Value(const s32 member_anim_set_index, const f32 frame, const u32 element, b32* value_valid=NULL) const;
  f32 GetVector3Value(const s32 member_anim_set_index, const s32 frame, const u32 element, b32* value_valid=NULL) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_TRANSFORM
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     スケールのアニメーション結果の値を得る
   *
   *  @param[out]   value                    frameにおけるアニメーション結果の値を返す
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[out]   value_valid              正常な値のときtrue(0はxの判定、1はyの判定、2はzの判定)
   *
   *  @retval    正常に取得できたときtrueを返す
   *             trueが返ってきたときでもベクトルの各要素に正常な値があるとは限りませんので、各要素の判定はvalue_validで行って下さい。
   */
  //-----------------------------------------------------------------------------
  b32 GetTransformScaleValue(gfl::math::VEC3* value, const s32 member_anim_set_index, const f32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL) const;
  b32 GetTransformScaleValue(gfl::math::VEC3* value, const s32 member_anim_set_index, const s32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     スケールのアニメーション結果の値を得る
   *
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[in]    element                  0ときxの値を取得する、1のときyの値を取得する、2のときzの値を取得する
   *  @param[out]   value_valid              正常な値のときtrue 
   *
   *  @retval    frameにおけるアニメーション結果の値
   *             正常に取得できなかった場合、戻り値は不定 
   */
  //-----------------------------------------------------------------------------
  f32 GetTransformScaleValue(const s32 member_anim_set_index, const f32 frame, const u32 element, b32* value_valid=NULL) const;
  f32 GetTransformScaleValue(const s32 member_anim_set_index, const s32 frame, const u32 element, b32* value_valid=NULL) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     平行移動のアニメーション結果の値を得る
   *
   *  @param[out]   value                    frameにおけるアニメーション結果の値を返す
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[out]   value_valid              正常な値のときtrue(0はxの判定、1はyの判定、2はzの判定)
   *
   *  @retval    正常に取得できたときtrueを返す
   *             trueが返ってきたときでもベクトルの各要素に正常な値があるとは限りませんので、各要素の判定はvalue_validで行って下さい。
   */
  //-----------------------------------------------------------------------------
  b32 GetTransformTranslateValue(gfl::math::VEC3* value, const s32 member_anim_set_index, const f32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL) const;
  b32 GetTransformTranslateValue(gfl::math::VEC3* value, const s32 member_anim_set_index, const s32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     平行移動のアニメーション結果の値を得る
   *
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[in]    element                  0ときxの値を取得する、1のときyの値を取得する、2のときzの値を取得する
   *  @param[out]   value_valid              正常な値のときtrue 
   *
   *  @retval    frameにおけるアニメーション結果の値
   *             正常に取得できなかった場合、戻り値は不定 
   */
  //-----------------------------------------------------------------------------
  f32 GetTransformTranslateValue(const s32 member_anim_set_index, const f32 frame, const u32 element, b32* value_valid=NULL) const;
  f32 GetTransformTranslateValue(const s32 member_anim_set_index, const s32 frame, const u32 element, b32* value_valid=NULL) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     座標変換を表す行列のアニメーション結果の値を得る
   *
   *  @param[out]   value                    frameにおけるアニメーション結果の値を返す
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[out]   value_valid              正常な値のときtrue(0はrotateのxの判定、1はrotateのyの判定、2はrotateのzの判定、
   *                                                            3はtranslateのxの判定、4はtranslateのyの判定、5はtranslateのzの判定)
   *
   *  @retval    正常に取得できたときtrueを返す
   *             falseが返ってきたとき、マトリクスのどの要素に原因があるかはvalue_validで分かります。
   */
  //-----------------------------------------------------------------------------
  b32 GetTransformMatrixValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const f32 frame, \
      b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL, b32* value3_valid=NULL, b32* value4_valid=NULL, b32* value5_valid=NULL) const;
  b32 GetTransformMatrixValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const s32 frame, \
      b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL, b32* value3_valid=NULL, b32* value4_valid=NULL, b32* value5_valid=NULL) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     座標変換を表す行列のアニメーション結果の値を得る
   *
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[in]    element_row              row行 column列
   *  @param[in]    element_column           f._12ならrow=1, column=2
   *  @param[out]   value_valid              正常な値のときtrue(0はrotateのxの判定、1はrotateのyの判定、2はrotateのzの判定、
   *                                                            3はtranslateのxの判定、4はtranslateのyの判定、5はtranslateのzの判定)
   *
   *  @retval    frameにおけるアニメーション結果の値
   *             正常に取得できなかった場合、戻り値は不定
   *             value_validのどれかがfalseのとき、正常に取得できていません。
   */
  //-----------------------------------------------------------------------------
  f32 GetTransformMatrixValue(const s32 member_anim_set_index, const f32 frame, const u32 element_row, const u32 element_column, \
      b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL, b32* value3_valid=NULL, b32* value4_valid=NULL, b32* value5_valid=NULL) const;
  f32 GetTransformMatrixValue(const s32 member_anim_set_index, const s32 frame, const u32 element_row, const u32 element_column, \
      b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL, b32* value3_valid=NULL, b32* value4_valid=NULL, b32* value5_valid=NULL) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_RGBA_COLOR
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     アニメーション結果の値を得る
   *
   *  @param[out]   value                    frameにおけるアニメーション結果の値を返す
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[out]   value_valid              正常な値のときtrue(0はrの判定、1はgの判定、2はbの判定、3はaの判定)
   *
   *  @retval    正常に取得できたときtrueを返す
   */
  //-----------------------------------------------------------------------------
  b32 GetRgbaColorValue(gfl::grp::ColorF32* value, const s32 member_anim_set_index, const f32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL, b32* value3_valid=NULL) const;
  b32 GetRgbaColorValue(gfl::grp::ColorF32* value, const s32 member_anim_set_index, const s32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL, b32* value3_valid=NULL) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     アニメーション結果の値を得る
   *
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[in]    element                  0ときrの値を取得する、1のときgの値を取得する、2のときbの値を取得する、3のときaの値を取得する
   *  @param[out]   value_valid              正常な値のときtrue 
   *
   *  @retval    frameにおけるアニメーション結果の値
   *             正常に取得できなかった場合、戻り値は不定 
   */
  //-----------------------------------------------------------------------------
  f32 GetRgbaColorValue(const s32 member_anim_set_index, const f32 frame, const u32 element, b32* value_valid=NULL) const;
  f32 GetRgbaColorValue(const s32 member_anim_set_index, const s32 frame, const u32 element, b32* value_valid=NULL) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_TEXTURE
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     アニメーション結果の値を得る
   *
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[out]   value_valid              正常な値のときtrue
   *
   *  @retval    frameにおけるアニメーション結果の値
   *             正常に取得できなかった場合、戻り値は不定 
   */
  //-----------------------------------------------------------------------------
  s32 GetTextureValue(const s32 member_anim_set_index, const f32 frame, b32* value_valid=NULL) const;
  s32 GetTextureValue(const s32 member_anim_set_index, const s32 frame, b32* value_valid=NULL) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_BAKED_TRANSFORM
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     スケールのアニメーションが存在するかどうか 
   *
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *
   *  @retval    存在するときtrueを返す
   */
  //-----------------------------------------------------------------------------
  b32 DoesExistBakedTransformScale(const s32 member_anim_set_index) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     回転のアニメーションが存在するかどうか 
   *
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *
   *  @retval    存在するときtrueを返す
   */
  //-----------------------------------------------------------------------------
  b32 DoesExistBakedTransformRotate(const s32 member_anim_set_index) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     平行移動のアニメーションが存在するかどうか 
   *
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *
   *  @retval    存在するときtrueを返す
   */
  //-----------------------------------------------------------------------------
  b32 DoesExistBakedTransformTranslate(const s32 member_anim_set_index) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     スケールのアニメーション結果の値を得る
   *
   *  @param[out]   value                    frameにおけるアニメーション結果の値を返す
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *
   *  @retval    正常に取得できたときtrueを返す
   */
  //-----------------------------------------------------------------------------
  b32 GetBakedTransformScaleValue(gfl::math::VEC3* value, const s32 member_anim_set_index, const f32 frame) const;
  b32 GetBakedTransformScaleValue(gfl::math::VEC3* value, const s32 member_anim_set_index, const s32 frame) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     スケールのアニメーション結果の値を得る
   *
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[in]    element                  0ときxの値を取得する、1のときyの値を取得する、2のときzの値を取得する
   *
   *  @retval    frameにおけるアニメーション結果の値
   *             正常に取得できなかった場合、戻り値は不定 
   */
  //-----------------------------------------------------------------------------
  f32 GetBakedTransformScaleValue(const s32 member_anim_set_index, const f32 frame, const u32 element) const;
  f32 GetBakedTransformScaleValue(const s32 member_anim_set_index, const s32 frame, const u32 element) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     回転を表す行列のアニメーション結果の値を得る
   *
   *  @param[out]   value                    frameにおけるアニメーション結果の値を返す
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *
   *  @retval    正常に取得できたときtrueを返す
   */
  //-----------------------------------------------------------------------------
  b32 GetBakedTransformRotateValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const f32 frame) const;
  b32 GetBakedTransformRotateValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const s32 frame) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     回転を表す行列のアニメーション結果の値を得る
   *
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[in]    element_row              row行 column列
   *  @param[in]    element_column           f._12ならrow=1, column=2
   *
   *  @retval    frameにおけるアニメーション結果の値
   *             正常に取得できなかった場合、戻り値は不定 
   */
  //-----------------------------------------------------------------------------
  f32 GetBakedTransformRotateValue(const s32 member_anim_set_index, const f32 frame, const u32 element_row, const u32 element_column) const;
  f32 GetBakedTransformRotateValue(const s32 member_anim_set_index, const s32 frame, const u32 element_row, const u32 element_column) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     平行移動を表す行列のアニメーション結果の値を得る
   *
   *  @param[out]   value                    frameにおけるアニメーション結果の値を返す
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *
   *  @retval    正常に取得できたときtrueを返す
   */
  //-----------------------------------------------------------------------------
  b32 GetBakedTransformTranslateValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const f32 frame) const;
  b32 GetBakedTransformTranslateValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const s32 frame) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     平行移動を表す行列のアニメーション結果の値を得る
   *
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[in]    element_row              row行 column列
   *  @param[in]    element_column           f._12ならrow=1, column=2
   *
   *  @retval    frameにおけるアニメーション結果の値
   *             正常に取得できなかった場合、戻り値は不定 
   */
  //-----------------------------------------------------------------------------
  f32 GetBakedTransformTranslateValue(const s32 member_anim_set_index, const f32 frame, const u32 element_row, const u32 element_column) const;
  f32 GetBakedTransformTranslateValue(const s32 member_anim_set_index, const s32 frame, const u32 element_row, const u32 element_column) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_FULL_BAKED
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     アニメーション結果の値を得る
   *
   *  @param[out]   value                    frameにおけるアニメーション結果の値を返す
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *
   *  @retval    正常に取得できたときtrueを返す
   */
  //-----------------------------------------------------------------------------
  b32 GetFullBakedValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const f32 frame) const;
  b32 GetFullBakedValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const s32 frame) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     アニメーション結果の値を得る
   *
   *  @param[in]    member_anim_set_index    メンバーアニメーションの要素のインデックス番号
   *  @param[in]    frame                    フレーム
   *  @param[in]    element_row              row行 column列
   *  @param[in]    element_column           f._12ならrow=1, column=2
   *
   *  @retval    frameにおけるアニメーション結果の値
   *             正常に取得できなかった場合、戻り値は不定 
   */
  //-----------------------------------------------------------------------------
  f32 GetFullBakedValue(const s32 member_anim_set_index, const f32 frame, const u32 element_row, const u32 element_column) const;
  f32 GetFullBakedValue(const s32 member_anim_set_index, const s32 frame, const u32 element_row, const u32 element_column) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ユーザデータ
   */
  //-----------------------------------------------------------------------------
  s32 GetUserDataCount() const;
  s32 GetUserDataIndex(const char* key) const;
  void GetUserData(const s32 user_data_index, UserData* user_data) const;


  // 専用のResCameraAnimクラスをつくるかどうか迷うもの
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ResCameraAnimのViewUpdaterKindを取得する
   *
   *  @retval        ViewUpdaterKind
   *                 ResAnimがResCameraAnimでない場合は、VIEW_UPDATER_KIND_NONEを返す
   */
  //-----------------------------------------------------------------------------
  ViewUpdaterKind GetResCameraAnimViewUpdaterKind(void) const;
  
  // フレンドクラスからのみアクセスするprivateメンバ関数
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         このクラスが保持しているnw::anim::ResAnimを取得
   *
   *  @retval        nw::anim::ResAnim    このクラスが保持しているnw::anim::ResAnim
   */
  //-----------------------------------------------------------------------------
  nw::anim::ResAnim GetNwResAnim(void) const {  return m_nw_res_anim;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         このクラスで保持するようnw::anim::ResAnimを設定
   *
   *  @param[in]     type           アニメーションリソースのタイプ
   *  @param[in]     nw_res_anim    アニメーションリソース
   */
  //-----------------------------------------------------------------------------
  void SetNwResAnim(Resource::AnimationResourceType type, nw::anim::ResAnim nw_res_anim);

  
  // フレンドクラスからアクセスしてはならないprivateメンバ
private:
  enum
  {
    // この並び順を使用している箇所があるので、並び順は変えないようにして下さい。
    VALUE_VALID_SCALE_X,
    VALUE_VALID_SCALE_Y,
    VALUE_VALID_SCALE_Z,
    VALUE_VALID_ROTATE_X,
    VALUE_VALID_ROTATE_Y,
    VALUE_VALID_ROTATE_Z,
    VALUE_VALID_TRANSLATE_X,
    VALUE_VALID_TRANSLATE_Y,
    VALUE_VALID_TRANSLATE_Z,
    VALUE_VALID_MAX
  };

  // フレンドクラスからアクセスしてはならないprivateメンバ関数
private:
  //-----------------------------------------------------------------------------
  /*
   *    メンバーアニメーションの情報
   */
  //-----------------------------------------------------------------------------
  s32 GetMemberAnimTypeSize(const s32 member_anim_set_index) const;
  b32 GetTransformValue(nw::gfx::CalculatedTransform* value, const s32 member_anim_set_index, const f32 frame, b32 value_valid_array[VALUE_VALID_MAX]) const;
  void DoesExistBakedTransform(b32* scale_exist, b32* rotate_exist, b32* translate_exist, const s32 member_anim_set_index) const;


private:
  Type               m_type;
  nw::anim::ResAnim  m_nw_res_anim;  // 他のところにあるリソースを覚えておくだけで、生成や破棄はしない。
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DRESANIM_H__
