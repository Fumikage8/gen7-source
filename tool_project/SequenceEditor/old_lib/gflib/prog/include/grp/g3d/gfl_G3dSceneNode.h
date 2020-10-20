#ifndef __GFL_G3DSCENENODE_H__
#define __GFL_G3DSCENENODE_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSceneNode.h
 *	@brief  3Dシーンノード
 *	@author	Koji Kawada
 *	@date		2010.10.12
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
#include <grp/g3d/gfl_G3dObject.h>
#include <grp/g3d/gfl_G3dSceneCalculateGroup.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	クラスの前方宣言
//=====================================
class Model;

namespace internal {

class CallbackConstraintOfSkeleton;       // これがないと、SceneNodeクラス内のfriendクラスとして書くことができない
class CallbackConstraintOfTransformNode;

}  // namespace internal


//-------------------------------------
///	3Dシーンノードクラス
//=====================================
class SceneNode : public Object
{
  GFL_FORBID_COPY_AND_ASSIGN(SceneNode);


  friend class Scene;
  friend class SceneSystem;
  //friend class Model::AnimationSlot;  // こういう表記ではomakeが通らない
  friend class AnimationSlot;
  friend class internal::CallbackConstraintOfSkeleton;       // friend class CallbackConstraintOfSkeleton;ではomakeが通らない
  friend class internal::CallbackConstraintOfTransformNode;


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   */
  //-----------------------------------------------------------------------------
  virtual void Create(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         破棄
   *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーンツリーフラグ
   *                 trueのときSceneにAddされていたらシーンツリーに入れられる、falseのときSceneにAddされていてもシーンツリーから外される
   *                 これがtrueであることと、ユーザがSceneにAddしたことは、同じではありません。
   */
  //-----------------------------------------------------------------------------
  virtual void SetTreeFlag(b32 flag);
  virtual b32  GetTreeFlag(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         描画フラグ
   *                 trueのとき描画される
   *                 ただしこれがtrueのときでも、GetTreeFlagがfalseのときは描画されない
   */
  //-----------------------------------------------------------------------------
  virtual void SetVisible(b32 visible);
  virtual b32  IsVisible(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーンにAddされていたらtrueを返す(シーンツリーフラグがどうなっているかは調べていないので、Addされていてもシーンツリーには入っていないかもしれない)
   *                 このシーンノードの親がSceneCalculateGroupでなくても、親の親の・・・親の親がSceneCalculateGroupだったら、Addされていると判定する
   */
  //-----------------------------------------------------------------------------
  b32 IsAddedToScene(void) const { return (m_parent_borrow != NULL); }


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストレインのセットアップをする
   *                 このシーンノードは拘束されbatonになる
   *
   *                 既にこのシーンノードがbatonとしてセットアップされているときにこの関数を呼ぶと、新しいセットアップで上書きされる。
   *                 だから、初めてのセットアップ時だけでなく、inheritを変更したいときやhandボーンを変更したいときにもこの関数を呼んでよい。
   *
   *  @param[in]     human_scene_node    humanシーンノード  // @note 今はModelに限定していますが、LightやCameraにも拘束できるようにするかも。
   *  @param[in]     hand_bone_index     handボーンのインデックス
   *                                     gfl::grp::g3d::Model::BONE_INDEX_NONEのとき、handボーンではなくhumanトップにつける
   *  @param[in]     hand_bone_key       handボーンの名前
   *                                     NULLのとき、handボーンではなくhumanトップにつける
   *  @param[in]     inherit_scale       trueのときスケールの追従あり
   *  @param[in]     inherit_rotate      trueのとき回転の追従あり
   *  @param[in]     inherit_translate   trueの平行移動の追従あり
   *
   *  @retval        b32                 正常にセットアップできたらtrue
   *
   *  @note 拘束したときに、拘束する前のトランスフォームを変更しないか変更するかを選べるようにしたい。
   *  選べないなら、どちらかに統一しておくこと！
   */
  //-----------------------------------------------------------------------------
  b32 SetupConstraint(
      gfl::grp::g3d::Model* human_scene_node,
      const s32             hand_bone_index,
      const b32             inherit_scale     = true,
      const b32             inherit_rotate    = true,
      const b32             inherit_translate = true
  );
  b32 SetupConstraint(
      gfl::grp::g3d::Model* human_scene_node,
      const char*           hand_bone_key,
      const b32             inherit_scale     = true,
      const b32             inherit_rotate    = true,
      const b32             inherit_translate = true
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストレインのクリーンアップをする
   *                 このシーンノードの拘束を解きbatonでなくする
   *
   *  @retval        b32                 正常にクリーンアップできたらtrue
   *                                     元々コンストレインされていないときはfalseを返す。
   *
   *  @note 拘束を解くときに、拘束する時のトランスフォームを変更しないか変更するかを選べるようにしたい。
   *  選べないなら、どちらかに統一しておくこと！
   */
  //-----------------------------------------------------------------------------
  b32 CleanupConstraint(void);


protected:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         このクラスが保持しているnw::gfx::SceneNode*を取得
   *
   *  @retval        nw::gfx::SceneNode*    このクラスが保持しているnw::gfx::SceneNode
   */
  //-----------------------------------------------------------------------------
  virtual nw::gfx::SceneNode* GetNwSceneNode(void) const  {  return m_nw_scene_node;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         このクラスが保持しているnw::gfx::SceneNode*をnw::gfx::TransformNode*にキャストして取得
   *
   *  @retval        nw::gfx::TransformNode*    このクラスが保持しているnw::gfx::SceneNode
   */
  //-----------------------------------------------------------------------------
  virtual nw::gfx::TransformNode* GetNwTransformNode(void) const
  {
    return nw::ut::DynamicCast<nw::gfx::TransformNode*>( m_nw_scene_node );
  }


protected:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 Createは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  SceneNode(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   *                 Destroyを呼びます。
   */
  //-----------------------------------------------------------------------------
  virtual ~SceneNode();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         このクラスで保持するようnw::gfx::SceneNode*を設定
   *
   *  @param[in]     nw_scene_node    シーンノード
   */
  //-----------------------------------------------------------------------------
  void SetNwSceneNode(nw::gfx::SceneNode* nw_scene_node)  {  m_nw_scene_node = nw_scene_node;  }


protected:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストレインで上書きされる前の値
   *                 派生クラスでm_constraint_calculated_transformを書き換えるためのアクセス関数
   *
   *                 @note 派生クラスでアクセスして、個々の派生クラスで処理するのではなく、SceneNodeで処理したい。
   */
  //-----------------------------------------------------------------------------
  nw::gfx::CalculatedTransform& ConstraintNwCalculatedTransform(void)
  {
    return m_constraint_calculated_transform;
  }
  const nw::gfx::CalculatedTransform& ConstraintNwCalculatedTransform(void) const  // @note 派生クラスのconst関数から呼ぶための関数だが、
                                                                                   // 個々の派生クラスで処理するのではなく、SceneNodeで処理するように
                                                                                   // なったら不要になるので、削除する予定の関数。
  {
    return m_constraint_calculated_transform;
  }
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストレインで上書きされる前の値
   *                 friendクラスでm_constraint_calculated_transformを見るためのアクセス関数
   */
  //-----------------------------------------------------------------------------
  const nw::gfx::CalculatedTransform* GetConstraintNwCalculatedTransform(void) const
  {
    return &m_constraint_calculated_transform;
  }


private:
  // 親を設定する
  void SetParent(SceneCalculateGroup* parent)
  {
    // NULLを設定するときは、UnsetParentを呼ぶこと。
    GFL_ASSERT_MSG(parent, "parent=NULL\n");  //@check
    
    if( !m_parent_borrow )
    {
      m_parent_borrow = parent;
    }
    else
    {
      GFL_ASSERT_MSG(0, "既に親0x%pがいます。\n", m_parent_borrow);
    }
  }
  void UnsetParent(void)
  {
    // NULLを設定する
    GFL_ASSERT_MSG(m_parent_borrow, "m_parent_borrow=NULL\n");  //@check

    m_parent_borrow = NULL;
  }
  // GetParentはなし(なしの理由は特にないが、安易に使って欲しくないので)
  

public:
  //テスト用に緊急用意しただけb32 IsParentExist(void) const { return (m_parent_borrow != NULL); }


private:
  // シーンノード(とその派生クラス)を制御するビットフラグ
  enum
  {
    // なし
    BIT_FLAG_NONE         = 0,  // 何もしない

    // 個別
    BIT_FLAG_TREE_FLAG    = 0x1 <<  0,  // シーンツリーフラグ
                                        // 1のときSceneにAddされていたらシーンツリーに入れられる、
                                        // 0のときSceneにAddされていてもシーンツリーから外される。

    // 複合
    BIT_FLAG_DEFAULT      = BIT_FLAG_TREE_FLAG   // デフォルト
  };

private:
  nw::gfx::SceneNode* m_nw_scene_node;

  // 親
  SceneCalculateGroup*          m_parent_borrow;  // 他のところで生成したものを借りて覚えておく  // @note ModelやLightが親になることもあるので、親と成り得るものの型をつくりたい。  // @note ここを変えたらIsAddedToSceneも変更すること

  // ビットフラグ
  bit32                         m_bit_flag;

  // コンストレイン
  gfl::grp::g3d::Model*         m_constraint_human_scene_node;      // NULLでないとき、このシーンノードは拘束されbatonになっている。
  s32                           m_constraint_hand_bone_index;
  nw::gfx::CalculatedTransform  m_constraint_calculated_transform;  // コンストレインでユーザが設定した値が上書きされてしまうので、上書きされる前の値をとっておく。

protected:
  gfl::math::VEC3  m_rotate;  // XYZの順番に回転する。単位はラジアン。
                              // 【注意】専用のSetRotateXYZ関数以外で回転を行っている場合は、見た目と異なる値が入っている。
                              // @note SceneNodeではなくTransNodeとかをつくってそちらに持たせたい。
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSCENENODE_H__
