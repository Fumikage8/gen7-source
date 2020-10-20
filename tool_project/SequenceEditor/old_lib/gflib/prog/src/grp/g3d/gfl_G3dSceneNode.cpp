//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSceneNode.cpp
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

// gflib grp g3d
#include <grp/g3d/gfl_G3dSceneNode.h>
#include <grp/g3d/gfl_G3dModel.h>
#include <grp/g3d/gfl_G3dSceneCalculateGroup.h>

namespace gfl {
namespace grp {
namespace g3d {


namespace internal {

static const nw::math::Transform3 SCENE_NODE_CONSTRAINT_CALCULATED_TRANSFORM_DEFAULT(
    nw::math::VEC3( 1.0f, 1.0f, 1.0f ),
    nw::math::VEC3( 0.0f, 0.0f, 0.0f ),
    nw::math::VEC3( 0.0f, 0.0f, 0.0f )
);

}  // namespace internal


//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
 *                 Createは呼ばれません。
 */
//-----------------------------------------------------------------------------
SceneNode::SceneNode(void)
  : m_nw_scene_node(NULL),
    m_parent_borrow(NULL),
    m_bit_flag(BIT_FLAG_DEFAULT),
    m_constraint_human_scene_node(NULL),
    m_constraint_hand_bone_index(gfl::grp::g3d::Model::BONE_INDEX_NONE),
    m_rotate(0.0f, 0.0f, 0.0f)
{
  m_constraint_calculated_transform.SetTransform( internal::SCENE_NODE_CONSTRAINT_CALCULATED_TRANSFORM_DEFAULT );
}

//-----------------------------------------------------------------------------
/*
 *  @brief         デストラクタ
 *                 Destroyを呼びます。
 */
//-----------------------------------------------------------------------------
SceneNode::~SceneNode()
{
  Destroy();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         生成
 */
//-----------------------------------------------------------------------------
void SceneNode::Create(void)
{
  m_bit_flag = BIT_FLAG_DEFAULT;

  m_rotate = gfl::math::VEC3(0.0f, 0.0f, 0.0f);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         破棄
 *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
 */
//-----------------------------------------------------------------------------
void SceneNode::Destroy(void)
{
  //GFL_ASSERT_MSG(m_parent_borrow==NULL, "親から外す前に破棄が呼ばれました。\n");  // 親から外れていないときは警告を出すが、何もしない。  //@check
                                                                                    // あまりにこの警告に引っ掛かるので、外してあげることにする。
                                                                                    // SceneのDestroyでも外し忘れを外しているので、
                                                                                    // 親がNULLになっているということはない。
  if( m_parent_borrow )
  {
    m_parent_borrow->RemoveSceneNode(this);
  }


  nw::gfx::SafeDestroy(m_nw_scene_node);  // SafeDestroyはNULLを代入してくれる

  m_parent_borrow = NULL;  // 親から外れていなくても何もしない。

  m_bit_flag = BIT_FLAG_DEFAULT;

  if( m_constraint_human_scene_node )
  {
    CleanupConstraint();
  }

  m_constraint_human_scene_node     = NULL;
  m_constraint_hand_bone_index      = gfl::grp::g3d::Model::BONE_INDEX_NONE;
  m_constraint_calculated_transform.SetTransform( internal::SCENE_NODE_CONSTRAINT_CALCULATED_TRANSFORM_DEFAULT );
  
  m_rotate = gfl::math::VEC3(0.0f, 0.0f, 0.0f);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         シーンツリーフラグ
 *                 trueのときSceneにAddされていたらシーンツリーに入れられる、falseのときSceneにAddされていてもシーンツリーから外される
 *                 これがtrueであることと、ユーザがSceneにAddしたことは、同じではありません。
 */
//-----------------------------------------------------------------------------
void SceneNode::SetTreeFlag(b32 flag)
{
  if( GetTreeFlag() != flag )
  {
    SetBitFlag(&m_bit_flag, BIT_FLAG_TREE_FLAG, flag);
    if( m_parent_borrow )
    {
      m_parent_borrow->RefreshSceneNodeTreeFlag(this);
    }
  }
}
b32  SceneNode::GetTreeFlag(void) const
{
  return ( IsBitFlagOnOne(m_bit_flag, BIT_FLAG_TREE_FLAG) != 0 );  // @note これを【戻り値注意】扱いしてくれるなら
                                                                   // u32型で【0以外(1とは限らないしいつも違う値になる)】か【0】を返すようにしてもいいかも。
}

//-----------------------------------------------------------------------------
/*
 *  @brief         描画フラグ
 *                 trueのとき描画される
 *                 ただしこれがtrueのときでも、GetTreeFlagがfalseのときは描画されない
 */
//-----------------------------------------------------------------------------
void SceneNode::SetVisible(b32 /*visible*/)
{
  // 派生クラスで実装して下さい。
  // @note ModelNodeクラスをつくってそちらに移行したい
}
b32  SceneNode::IsVisible(void) const
{
  // 派生クラスで実装して下さい。
  // @note ModelNodeクラスをつくってそちらに移行したい
  return false;
}

//-----------------------------------------------------------------------------
/*
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
b32 SceneNode::SetupConstraint(
    gfl::grp::g3d::Model* human_scene_node,
    const s32             hand_bone_index,
    const b32             inherit_scale,
    const b32             inherit_rotate,
    const b32             inherit_translate
)
{
  if( m_constraint_human_scene_node )
  {
    // 既に、このシーンノードは拘束されておりbatonになっている
    if(    m_constraint_human_scene_node != human_scene_node
        || m_constraint_hand_bone_index  != hand_bone_index )
    {
      // 違うところにくっつける
      b32 ret_rem = m_constraint_human_scene_node->RemoveConstraintBatonSceneNode(
          this,
          m_constraint_hand_bone_index
      );
      if( ret_rem )
      {
        m_constraint_human_scene_node = NULL;
        m_constraint_hand_bone_index  = gfl::grp::g3d::Model::BONE_INDEX_NONE;

        b32 ret_add = human_scene_node->AddConstraintBatonSceneNode(
            this,
            hand_bone_index,
            inherit_scale,
            inherit_rotate,
            inherit_translate
        );
        if( ret_add )
        {
          m_constraint_human_scene_node = human_scene_node;
          m_constraint_hand_bone_index  = hand_bone_index;
        }
        else
        {
          GFL_ASSERT_MSG( 0, "追加できませんでした。\n" );
        }
        return ret_add;
      }
      else
      {
        GFL_ASSERT_MSG( 0, "付け直すために一旦削除しようとしましたが、削除できませんでした。\n" );
        return ret_rem;
      }
    }
    else
    {
      // 同じところなのでinheritだけ変更する
      b32 ret_cha = human_scene_node->ChangeConstraintBatonSceneNodeInherit(
          this,
          hand_bone_index,
          inherit_scale,
          inherit_rotate,
          inherit_translate
      );
      GFL_ASSERT_MSG( ret_cha, "inheritを変更しようとしましたが、変更できませんでした。\n" );  //@check
      return ret_cha;
    }
  }
  else
  {
    // 初めてこのシーンノードを拘束しbatonにする
    b32 ret_add = human_scene_node->AddConstraintBatonSceneNode(
        this,
        hand_bone_index,
        inherit_scale,
        inherit_rotate,
        inherit_translate
    );
    if( ret_add )
    {
      m_constraint_human_scene_node = human_scene_node;
      m_constraint_hand_bone_index  = hand_bone_index;
    }
    else
    {
      GFL_ASSERT_MSG( 0, "追加できませんでした。\n" );  // @note 元々追加されているとき or 要素数を越えて追加できないとき の2通りのfalseがあるため、
                                                        //       実は前のを消し忘れていて前者のfalseになっているということもあり得る。これを区別したい。
    }
    return ret_add;
  }
}
b32 SceneNode::SetupConstraint(
    gfl::grp::g3d::Model* human_scene_node,
    const char*           hand_bone_key,
    const b32             inherit_scale,
    const b32             inherit_rotate,
    const b32             inherit_translate
)
{
  return SceneNode::SetupConstraint(
    human_scene_node,
    human_scene_node->GetBonesIndex(hand_bone_key),
    inherit_scale,
    inherit_rotate,
    inherit_translate
  );
}

//-----------------------------------------------------------------------------
/*
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
b32 SceneNode::CleanupConstraint(void)
{
  if( m_constraint_human_scene_node )
  {
    // このシーンノードは拘束されておりbatonになっている
    b32 ret_rem = m_constraint_human_scene_node->RemoveConstraintBatonSceneNode(
        this,
        m_constraint_hand_bone_index
    );
    if( ret_rem )
    {
      m_constraint_human_scene_node = NULL;
      m_constraint_hand_bone_index  = gfl::grp::g3d::Model::BONE_INDEX_NONE;
    }
    else
    {
      GFL_ASSERT_MSG( 0, "削除できませんでした。\n" );
    }
    return ret_rem;
  }
  else
  {
    // このシーンノードは拘束されていないのでbatonになっていない
    return false;
  }
}

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

