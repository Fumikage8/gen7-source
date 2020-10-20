//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dModel.cpp
 *	@brief  3Dモデル
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>
#include <nw/ut.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Math.h>
#include <gfl_debug.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dSystem.h>
#include <grp/g3d/gfl_G3dUserData.h>

#include <grp/g3d/gfl_G3dModel.h>




#define GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD (1)  // メッシュのリソースの取得の仕方を変更する。
                                                  // 0のときnw::gfx::Model::GetResMeshesで取得する。
                                                  // 1のときnw::gfx::res::ResModel::GetMeshesで取得する。

#if 0
/*
@note
メッシュのリソースから何を得たいかで、GetResMeshesを使い分ける必要がありそうだ。
今は
GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD
で全部を切り替えているが、取得するものによって適切な方を使うようにしたい。

C:\home\xy_ctrlib\NintendoWare\CTR\documents\API\nw\gfx\Model\GetResMeshes.html
nw::gfx::Model::GetResMeshes メンバ関数
メッシュのリソースの配列を取得します。
メッシュのリソースはモデルリソースから取得するのではなく、 このメソッドを使って取得するようにしてください。

[質問] メッシュの描画優先度を動的に変更するには？
田中＠NTSC 2010-08-18 19:48:37  
ResModelからResMeshを取得されているのが原因です。
実際の描画ではResModelのResMeshではなく、
nw::gfx::Model::GetResMeshesで取得できるResMeshの配列が使用されています。
その為、こちらを取得するようにして描画優先度を変更してください。 
*/
#endif


namespace gfl {
namespace grp {
namespace g3d {


namespace internal {

// NintendoWare/CTR/tools/Viewer/
// NodeConstraint.h
// NodeConstraint.cpp
// ProtocolEM4C.h  // enum FollowBitType // Flag用ビット定義（=IsContinue|IsFollowScaleのように使用します）  // FollowBitType_None = 0x00,           // 未設定
// ParamSetter.h   // 兄弟ParticleEmitterの設定  // constraint.m_UseOffset[0] = true;

/*---------------------------------------------------------------------------*
  Name:         
  
  Description: 
  
  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MultScale(nw::math::MTX34* dstMatrix, const nw::math::MTX34* srcMatrix, const nw::math::VEC3& scale)
{
    dstMatrix->f._00 = srcMatrix->f._00 * scale.x;
    dstMatrix->f._10 = srcMatrix->f._10 * scale.x;
    dstMatrix->f._20 = srcMatrix->f._20 * scale.x;

    dstMatrix->f._01 = srcMatrix->f._01 * scale.y;
    dstMatrix->f._11 = srcMatrix->f._11 * scale.y;
    dstMatrix->f._21 = srcMatrix->f._21 * scale.y;

    dstMatrix->f._02 = srcMatrix->f._02 * scale.z;
    dstMatrix->f._12 = srcMatrix->f._12 * scale.z;
    dstMatrix->f._22 = srcMatrix->f._22 * scale.z;
}

/*---------------------------------------------------------------------------*
  Name:         
  
  Description: 
  
  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void CopyRotate(nw::math::MTX34* dstMatrix, const nw::math::MTX34& srcMatrix)
{
    dstMatrix->f._00 = srcMatrix.f._00;
    dstMatrix->f._10 = srcMatrix.f._10;
    dstMatrix->f._20 = srcMatrix.f._20;

    dstMatrix->f._01 = srcMatrix.f._01;
    dstMatrix->f._11 = srcMatrix.f._11;
    dstMatrix->f._21 = srcMatrix.f._21;

    dstMatrix->f._02 = srcMatrix.f._02;
    dstMatrix->f._12 = srcMatrix.f._12;
    dstMatrix->f._22 = srcMatrix.f._22;
}

/*---------------------------------------------------------------------------*
  Name:         
  
  Description: 
  
  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void CopyTranslate(nw::math::MTX34* dstMatrix, const nw::math::MTX34& srcMatrix)
{
    dstMatrix->f._03 = srcMatrix.f._03;
    dstMatrix->f._13 = srcMatrix.f._13;
    dstMatrix->f._23 = srcMatrix.f._23;
}

/*---------------------------------------------------------------------------*
  Name:         
  
  Description: 
  
  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void
CalculateWorldMatrixOfSkeleton(
    nw::math::MTX34* transformMatrix,
    nw::math::VEC3* scale,
    const nw::gfx::CalculatedTransform& localTransform,
    const nw::gfx::CalculatedTransform& parentWorldTransform,
    const nw::gfx::CalculatedTransform& parentLocalTransform,
    const bool inheritTransform[ 3 ]
) {
    GFL_ASSERT_STOP_MSG(transformMatrix, "transformMatrixがNULLです。\n" );

    const nw::math::MTX34& parentMatrix = parentWorldTransform.TransformMatrix();  // 座標変換を表す行列を取得します。 自分のスケールは掛かっておらず、親のスケールは掛かっています
    nw::math::VEC3 localTranslate = localTransform.TransformMatrix().GetColumn(3);

    // TR(i) = TR(i-1) * S(i-1) * t(i) * r(i)
    nw::math::MTX34Identity(transformMatrix);

    if (inheritTransform[ 1 ]) CopyRotate(transformMatrix, parentMatrix);
    if (inheritTransform[ 2 ]) CopyTranslate(transformMatrix, parentMatrix);
    if (inheritTransform[ 0 ]) {
        MultScale(transformMatrix, transformMatrix, parentLocalTransform.Scale());  // スケーリング変換ベクトルを取得します
        nw::math::VEC3Mult(scale, &parentWorldTransform.Scale(), &localTransform.Scale());  // スケーリング変換ベクトルを取得します
    } else {
        scale->Set(localTransform.Scale());
    }
    nw::math::MTX34MultTranslate(transformMatrix, transformMatrix, &localTranslate);
    nw::math::MTX33Mult(transformMatrix, transformMatrix, &localTransform.TransformMatrix());
}

void
CalculateWorldMatrixOfTransformNode(
    nw::math::MTX34* transformMatrix,
    nw::math::VEC3* scale,
    const nw::gfx::CalculatedTransform& localTransform,
    const nw::gfx::CalculatedTransform& parentWorldTransform,
    const bool inheritTransform[ 3 ]
) {
    GFL_ASSERT_STOP_MSG(transformMatrix, "transformMatrixがNULLです。\n" );

    const nw::math::MTX34& parentMatrix = parentWorldTransform.TransformMatrix();  // 座標変換を表す行列を取得します。 自分のスケールは掛かっておらず、親のスケールは掛かっています
    nw::math::VEC3 localTranslate = localTransform.TransformMatrix().GetColumn(3);

    // TR(i) = TR(i-1) * S(i-1) * t(i) * r(i)
    nw::math::MTX34Identity(transformMatrix);

    if (inheritTransform[ 1 ]) CopyRotate(transformMatrix, parentMatrix);
    if (inheritTransform[ 2 ]) CopyTranslate(transformMatrix, parentMatrix);
    if (inheritTransform[ 0 ]) {
        nw::math::VEC3Mult(scale, &parentWorldTransform.Scale(), &localTransform.Scale());  // スケーリング変換ベクトルを取得します
    } else {
        scale->Set(localTransform.Scale());
    }
    nw::math::MTX34MultTranslate(transformMatrix, transformMatrix, &localTranslate);
    nw::math::MTX33Mult(transformMatrix, transformMatrix, &localTransform.TransformMatrix());
}


//-------------------------------------
///	コールバッククラスで使用する構造体
//=====================================
struct ConstraintBaton
{
  gfl::grp::g3d::SceneNode*  baton_scene_node;
  b32                        inherit_scale;      // trueのときスケールの追従あり
  b32                        inherit_rotate;     // trueのとき回転の追従あり
  b32                        inherit_translate;  // trueの平行移動の追従あり
  // @note
  // constraint時に呼ばれるコールバックでだけ実行する
  // 差し替えreplace時に呼ばれるコールバックでだけ実行する
  // 両方のコールバックで実行する
  // この指定ができるようにしたい
};
struct ConstraintBatons
{
  u32                         baton_num_max;  // baton_arrayの要素数
  u32                         baton_num;      // baton_arrayで実際に値が入っている個数。
                                              // baton_arrayは常に前に詰めておくので0<= <baton_numだけ扱えばよい。
  ConstraintBaton*            baton_array;

  // 追加できたらtrueを返す
  // 元々追加されており新たに追加できないときはGFL_ASSERT_MSGをしてfalseを返す  //@check 検索にひっかからないように確認済の印を書いておく
  b32 AddBatonSceneNode(
    gfl::grp::g3d::SceneNode* baton_scene_node,
    const b32  inherit_scale,
    const b32  inherit_rotate,
    const b32  inherit_translate
  )
  {
    b32 ret = false;
    b32 exist = false;
    u32 i;
    for( i=0; i<baton_num; ++i )
    {
      ConstraintBaton* baton = &baton_array[i];
      if( baton->baton_scene_node == baton_scene_node )
      {
        exist = true;
        break;
      }
    }
    if( exist )
    {
      GFL_ASSERT_MSG(0, "既に追加されているので新たに追加できません。\n");
      ret = false;
    }
    else
    {
      if(baton_num < baton_num_max)
      {
        ConstraintBaton* baton   = &baton_array[baton_num];
        baton->baton_scene_node  = baton_scene_node;
        baton->inherit_scale     = inherit_scale; 
        baton->inherit_rotate    = inherit_rotate;   
        baton->inherit_translate = inherit_translate;
        ++baton_num;
        ret = true;
      }
      else
      {
        GFL_ASSERT_MSG(0, "baton_arrayの要素数%dを越えたので追加できません。\n", baton_num_max);
        ret = false;
      }
    }
    return ret;
  }

  // 変更できたらtrueを返す
  // 元々追加されていないときはGFL_ASSERT_MSGをしてfalseを返す  //@check 検索にひっかからないように確認済の印を書いておく
  b32 ChangeBatonSceneNodeInherit(
    gfl::grp::g3d::SceneNode* baton_scene_node,
    const b32  inherit_scale,
    const b32  inherit_rotate,
    const b32  inherit_translate
  )
  {
    b32 ret = false;
    b32 exist = false;
    u32 i;
    for( i=0; i<baton_num; ++i )
    {
      ConstraintBaton* baton = &baton_array[i];
      if( baton->baton_scene_node == baton_scene_node )
      {
        exist = true;
        break;
      }
    }
    if( exist )
    {
      ConstraintBaton* baton   = &baton_array[i];
      baton->inherit_scale     = inherit_scale; 
      baton->inherit_rotate    = inherit_rotate;   
      baton->inherit_translate = inherit_translate;
      ret = true;
    }
    else
    {
      GFL_ASSERT_MSG(0, "このシーンノードは追加されていないので追従を変更できません。\n");
      ret = false;
    }
    return ret;
  }

  // 削除できないときはfalseを返す
  // 元々追加されていないときはGFL_ASSERT_MSGをしてfalseを返す  //@check 検索にひっかからないように確認済の印を書いておく
  b32 RemoveBatonSceneNode(
    SceneNode* baton_scene_node
  )
  {
    b32 ret = false;
    b32 exist = false;
    u32 i;
    for( i=0; i<baton_num; ++i )
    {
      ConstraintBaton* baton = &baton_array[i];
      if( baton->baton_scene_node == baton_scene_node )
      {
        exist = true;
        break;
      }
    }
    if( exist )
    {
      for( u32 j=i; j<baton_num-1; ++j )
      {
        baton_array[j] = baton_array[j+1];
      }
      baton_array[baton_num-1].baton_scene_node = NULL;
      --baton_num;
      ret = true;
    }
    else
    {
      GFL_ASSERT_MSG(0, "このシーンノードは追加されていないので削除できません。\n");
      ret = false;
    }
    return ret;
  }
};
struct ConstraintHand
{
  s32               hand_bone_index;
  ConstraintBatons  batons;  // handボーンにくっつけるbaton
};
struct ConstraintHumanOfSkeleton
{
  nw::gfx::Skeleton*        human_skeleton;
  u32                       hand_num_max;  // hand_arrayの要素数
  u32                       hand_num;      // hand_arrayで実際に値が入っている個数。
                                           // hand_arrayは常に前に詰めておくので0<= <hand_numだけ扱えばよい。
  ConstraintHand*           hand_array;

  // 追加できたらtrueを返す
  // 元々追加されており新たに追加できないときはGFL_ASSERT_MSGをしてfalseを返す  //@check 検索にひっかからないように確認済の印を書いておく
  b32 AddBatonSceneNode(
    gfl::grp::g3d::SceneNode* baton_scene_node,
    const s32  hand_bone_index,
    const b32  inherit_scale,
    const b32  inherit_rotate,
    const b32  inherit_translate
  )
  {
    b32 ret = false;
    b32 exist = false;
    u32 h;
    for( h=0; h<hand_num; ++h )
    {
      ConstraintHand* hand = &hand_array[h];
      if( hand->hand_bone_index == hand_bone_index )
      {
        exist = true;
        break;
      }
    }
    if( exist )
    {
      ConstraintHand* hand = &hand_array[h];
      ret = hand->batons.AddBatonSceneNode(
        baton_scene_node,
        inherit_scale,
        inherit_rotate,
        inherit_translate
      );
    }
    else
    {
      if(hand_num < hand_num_max)
      {
        ConstraintHand* hand = &hand_array[hand_num];
        ret = hand->batons.AddBatonSceneNode(
          baton_scene_node,
          inherit_scale,
          inherit_rotate,
          inherit_translate
        );
        if(ret)
        {
          hand->hand_bone_index = hand_bone_index;
          ++hand_num;
        }
      }
      else
      {
        GFL_ASSERT_MSG(0, "hand_arrayの要素数%dを越えたので追加できません。\n", hand_num_max);
        ret = false;
      }
    }
    return ret;
  }

  // 変更できたらtrueを返す
  // 元々追加されていないときはGFL_ASSERT_MSGをしてfalseを返す  //@check 検索にひっかからないように確認済の印を書いておく
  b32 ChangeBatonSceneNodeInherit(
    gfl::grp::g3d::SceneNode* baton_scene_node,
    const s32  hand_bone_index,
    const b32  inherit_scale,
    const b32  inherit_rotate,
    const b32  inherit_translate
  )
  {
    b32 ret = false;
    b32 exist = false;
    u32 h;
    for( h=0; h<hand_num; ++h )
    {
      ConstraintHand* hand = &hand_array[h];
      if( hand->hand_bone_index == hand_bone_index )
      {
        exist = true;
        break;
      }
    }
    if( exist )
    {
      ConstraintHand* hand = &hand_array[h];
      ret = hand->batons.ChangeBatonSceneNodeInherit(
        baton_scene_node,
        inherit_scale,
        inherit_rotate,
        inherit_translate
      );
    }
    else
    {
      GFL_ASSERT_MSG(0, "このシーンノードは追加されていないので追従を変更できません。\n");
      ret = false;
    }
    return ret;
  }

  // 削除できないときはfalseを返す
  // 元々追加されていないときはGFL_ASSERT_MSGをしてfalseを返す  //@check 検索にひっかからないように確認済の印を書いておく
  b32 RemoveBatonSceneNode(
    SceneNode* baton_scene_node,
    const s32  hand_bone_index
  )
  {
    b32 ret = false;
    b32 exist = false;
    u32 h;
    for( h=0; h<hand_num; ++h )
    {
      ConstraintHand* hand = &hand_array[h];
      if( hand->hand_bone_index == hand_bone_index )
      {
        exist = true;
        break;
      }
    }
    if( exist )
    {
      ConstraintHand* hand = &hand_array[h];
      ret = hand->batons.RemoveBatonSceneNode(
        baton_scene_node
      );
      if( ret )
      {
        if( hand->batons.baton_num == 0 )
        {
          // このhandの全batonがなくなったのでhand自体をなくす
          b32 remove_hand = true;
          u32 g;
          for( g=h; g<hand_num-1; ++g )
          {
            {
              // hand_array[g]にhand_array[g+1]をコピーする
              if( hand_array[g].batons.baton_num_max == hand_array[g+1].batons.baton_num_max )
              {
                hand_array[g].hand_bone_index      = hand_array[g+1].hand_bone_index;
                //hand_array[g].batons.baton_num_max == hand_array[g+1].batons.baton_num_max; 変更せずなのでコメントアウト
                hand_array[g].batons.baton_num     = hand_array[g+1].batons.baton_num;
                for( u32 i=0; i<hand_array[g].batons.baton_num; ++i )
                {
                  hand_array[g].batons.baton_array[i] = hand_array[g+1].batons.baton_array[i];
                }
              }
              else
              {
                remove_hand = false;
                break;
              }
            }
          }
          {
            // gは(hand_num-1)とは限らない。gは(hand_num-1)でもいいし、途中でもいい。
            // hand_array[g]はhand_array[g-1]にコピーしてあるので、hand_array[g]を空にする。
            // g=hのときもhand_array[g]を空にするだけなので問題ない。
            hand_array[g].hand_bone_index      = gfl::grp::g3d::Model::BONE_INDEX_NONE;
            //hand_array[g].batons.baton_num_max == ; 変更せずなのでコメントアウト
            for( u32 i=0; i<hand_array[g].batons.baton_num; ++i )
            {
              hand_array[g].batons.baton_array[i].baton_scene_node = NULL;
            }
            hand_array[g].batons.baton_num = 0;
          }
          if( remove_hand )
          {
            --hand_num;
          }
        }
      }
    }
    else
    {
      GFL_ASSERT_MSG(0, "このシーンノードは追加されていないので削除できません。\n");
      ret = false;
    }
    return ret;
  }
};
struct ConstraintHumanOfTransformNode
{
  nw::gfx::TransformNode*  human_transform_node;
  ConstraintBatons         batons;  // humanトップにくっつけるbaton
};

#if 0
/*
NintendoWare/CTR/include/nw/gfx/gfx_SceneNode.h
    typedef ut::Signal2<void, SceneNode*, SceneContext*> UpdateSignal;
NintendoWare/CTR/include/nw/gfx/gfx_TransformNode.h
    typedef ut::Signal2<void, TransformNode*, SceneContext*> CalculateMatrixSignal;
NintendoWare/CTR/include/nw/gfx/gfx_Skeleton.h
    typedef ut::Signal2<void, Skeleton*, int> CalculateMatrixSignal;

NintendoWare/CTR/include/nw/ut/ut_Signal.h
NintendoWare/CTR/include/nw/ut/generated/ut_Signal.hi
    //! "nw/ut/ut_Config.h"の"NW_SIGNAL_MULTI_SLOT_ENABLED"定義が無効の場合、
    //! シグナルに接続できるスロット数は１つになります。
    //! 
    //! "NW_SIGNAL_MULTI_SLOT_ENABLED"定義が無効の場合、
    //! Connect する前に必ず Disconnect するようにしてください。
    //! Disconnect していない場合は、アサートで停止します。

↓
NW_SIGNAL_MULTI_SLOT_ENABLED定義が有効の場合、
1ボーンごとにコールバックを用意し、1スケルトンに複数のコールバックを渡すことができるが、
NW_SIGNAL_MULTI_SLOT_ENABLED定義が無効の場合、
1スケルトンに1コールバックしか渡せない。
今回はNW_SIGNAL_MULTI_SLOT_ENABLED定義が無効の場合に対応できるようなつくりにした。
*/
#endif 


//-------------------------------------
///	コールバッククラス
//=====================================

/*!--------------------------------------------------------------------------*
  @brief        コールバックコンストレインの関数オブジェクトです。

  他のコールバックについては、関数リファレンスの「高度な機能」をご覧ください。
 *---------------------------------------------------------------------------*/
class CallbackConstraintOfSkeleton : public nw::gfx::Skeleton::CalculateMatrixSlot
{
public:
  //! @brief コンストラクタです。
  CallbackConstraintOfSkeleton(
    gfl::heap::NwAllocator*  heap_allocator,
    gfl::heap::NwAllocator*  device_allocator,
    nw::gfx::Skeleton*       human_skeleton,
    u32                      hand_num_max,       // くっつけられるhandの最大数(このモデルのボーンの個数以下)
    u32                      hand_baton_num_max  // 1つのhandに拘束されるbatonの最大数
  )
    : nw::gfx::Skeleton::CalculateMatrixSlot(device_allocator)
  {
    GFL_ASSERT_STOP_MSG( heap_allocator, "heap_allocatorがNULLです。\n" );
    GFL_ASSERT_STOP_MSG( device_allocator, "device_allocatorがNULLです。\n" );
    GFL_ASSERT_STOP_MSG( human_skeleton, "human_skeletonがNULLです。\n" );
    GFL_ASSERT_STOP_MSG( hand_num_max > 0, "hand_num_max=%dは0より大でなければなりません。\n", hand_num_max );
    GFL_ASSERT_STOP_MSG( hand_baton_num_max > 0, "hand_baton_num_max=%dは0より大でなければなりません。\n", hand_baton_num_max );

    m_constraint.human_skeleton = human_skeleton;
    m_constraint.hand_array     = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) ConstraintHand[hand_num_max];
    m_constraint.hand_num_max   = hand_num_max;
    m_constraint.hand_num       = 0;

    for( u32 h=0; h<hand_num_max; ++h )
    {
      m_constraint.hand_array[h].batons.baton_array   = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) ConstraintBaton[hand_baton_num_max];
      m_constraint.hand_array[h].batons.baton_num_max = hand_baton_num_max;
      m_constraint.hand_array[h].batons.baton_num     = 0;
    }

    // 関数オブジェクトをシグナルに接続します。
    m_constraint.human_skeleton->PostCalculateMatrixSignal().Connect(this);
  }

  // デストラクタ
  virtual ~CallbackConstraintOfSkeleton()
  {
    for( u32 h=0; h<m_constraint.hand_num; ++h )
    {
      for( u32 i=0; i<m_constraint.hand_array[h].batons.baton_num; ++i )
      {
        m_constraint.hand_array[h].batons.baton_array[i].baton_scene_node->CleanupConstraint();
      }
    }

    m_constraint.human_skeleton->PostCalculateMatrixSignal().Disconnect(this);

    for( u32 h=0; h<m_constraint.hand_num_max; ++h )
    {
      if( m_constraint.hand_array[h].batons.baton_array )
      {
        GFL_DELETE_ARRAY m_constraint.hand_array[h].batons.baton_array;
        m_constraint.hand_array[h].batons.baton_array = NULL;
      }
    }

    if( m_constraint.hand_array )
    {
      GFL_DELETE_ARRAY m_constraint.hand_array;
      m_constraint.hand_array = NULL;
    }
  }

  //! @brief 拘束されるオブジェクトを追加します。
  bool AddBatonSceneNode(
      gfl::grp::g3d::SceneNode* baton_scene_node,
      const s32  hand_bone_index,
      const b32  inherit_scale,
      const b32  inherit_rotate,
      const b32  inherit_translate
  )
  {
    return m_constraint.AddBatonSceneNode(
      baton_scene_node,
      hand_bone_index,
      inherit_scale,
      inherit_rotate,
      inherit_translate
    );
  }
  // 拘束されるオブジェクトの追従を変更する
  bool ChangeBatonSceneNodeInherit(
      gfl::grp::g3d::SceneNode* baton_scene_node,
      const s32  hand_bone_index,
      const b32  inherit_scale,
      const b32  inherit_rotate,
      const b32  inherit_translate
  )
  {
    return m_constraint.ChangeBatonSceneNodeInherit(
      baton_scene_node,
      hand_bone_index,
      inherit_scale,
      inherit_rotate,
      inherit_translate
    );
  }
  // 拘束されるオブジェクトを削除します。
  bool RemoveBatonSceneNode(
      gfl::grp::g3d::SceneNode* baton_scene_node,
      const s32  hand_bone_index
  )
  {
    return m_constraint.RemoveBatonSceneNode(
      baton_scene_node,
      hand_bone_index
    );
  }

  //! @brief コールバック関数です。
  virtual void Invoke(nw::gfx::Skeleton* skeleton, int boneIdx)
  {
    if( skeleton != m_constraint.human_skeleton )
    {
      return;
    }

    for( u32 h=0; h<m_constraint.hand_num; ++h )
    {
      if( m_constraint.hand_array[h].hand_bone_index == boneIdx )
      {
        for( u32 i=0; i<m_constraint.hand_array[h].batons.baton_num; ++i )
        {
          ConstraintBaton*        baton = &m_constraint.hand_array[h].batons.baton_array[i];
          nw::gfx::TransformNode* baton_nw_transform_node = baton->baton_scene_node->GetNwTransformNode();
      
          bool l_InheritTransform[ 3 ] = { baton->inherit_scale, baton->inherit_rotate, baton->inherit_translate };

          nw::gfx::CalculatedTransform* localPose = skeleton->LocalTransformPose().GetTransform(boneIdx);
          nw::gfx::CalculatedTransform* worldPose = skeleton->WorldTransformPose().GetTransform(boneIdx);

          // オフセット付きワールド変換行列の計算
          CalculateWorldMatrixOfSkeleton(
              &baton_nw_transform_node->Transform().DirectTransformMatrix(),
              &baton_nw_transform_node->Transform().DirectScale(),
              *(baton->baton_scene_node->GetConstraintNwCalculatedTransform()),
              *worldPose,
              *localPose,
              l_InheritTransform
          );
 
          // キャッシュを無効化
          baton_nw_transform_node->Transform().EnableFlags(
              nw::gfx::CalculatedTransform::FLAG_IS_DIRTY
          );
        }
        break;  // 同じボーンはないのでbreak
      }
    }
  }

private:
  ConstraintHumanOfSkeleton  m_constraint;
};

class CallbackConstraintOfTransformNode : public nw::gfx::TransformNode::CalculateMatrixSlot
{
public:
  //! @brief コンストラクタです。
  CallbackConstraintOfTransformNode(
    gfl::heap::NwAllocator*  heap_allocator,
    gfl::heap::NwAllocator*  device_allocator,
    nw::gfx::TransformNode*  human_transform_node,
    u32                      human_baton_num_max  // humanに拘束されるbatonの最大数
  )
    : nw::gfx::TransformNode::CalculateMatrixSlot(device_allocator)
  {
    GFL_ASSERT_STOP_MSG( heap_allocator, "heap_allocatorがNULLです。\n" );
    GFL_ASSERT_STOP_MSG( device_allocator, "device_allocatorがNULLです。\n" );
    GFL_ASSERT_STOP_MSG( human_transform_node, "human_transform_nodeがNULLです。\n" );
    GFL_ASSERT_STOP_MSG( human_baton_num_max > 0, "human_baton_num_max=%dは0より大でなければなりません。\n", human_baton_num_max );

    m_constraint.human_transform_node = human_transform_node;
    m_constraint.batons.baton_array   = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) ConstraintBaton[human_baton_num_max];
    m_constraint.batons.baton_num_max = human_baton_num_max;
    m_constraint.batons.baton_num     = 0;

    // 関数オブジェクトをシグナルに接続します。
    m_constraint.human_transform_node->PostCalculateWorldMatrixSignal().Connect(this);
  }

  // デストラクタ
  virtual ~CallbackConstraintOfTransformNode()
  {
    for( u32 i=0; i<m_constraint.batons.baton_num; ++i )
    {
      m_constraint.batons.baton_array[i].baton_scene_node->CleanupConstraint();
    }
    
    m_constraint.human_transform_node->PostCalculateWorldMatrixSignal().Disconnect(this);

    if( m_constraint.batons.baton_array )
    {
      GFL_DELETE_ARRAY m_constraint.batons.baton_array;
      m_constraint.batons.baton_array = NULL;
    }
  }

  //! @brief 拘束されるオブジェクトを追加します。
  bool AddBatonSceneNode(
      gfl::grp::g3d::SceneNode* baton_scene_node,
      const b32  inherit_scale,
      const b32  inherit_rotate,
      const b32  inherit_translate
  )
  {
    return m_constraint.batons.AddBatonSceneNode(
      baton_scene_node,
      inherit_scale,
      inherit_rotate,
      inherit_translate
    );
  }
  // 拘束されるオブジェクトの追従を変更する
  bool ChangeBatonSceneNodeInherit(
      gfl::grp::g3d::SceneNode* baton_scene_node,
      const b32  inherit_scale,
      const b32  inherit_rotate,
      const b32  inherit_translate
  )
  {
    return m_constraint.batons.ChangeBatonSceneNodeInherit(
      baton_scene_node,
      inherit_scale,
      inherit_rotate,
      inherit_translate
    );
  }
  // 拘束されるオブジェクトを削除します。
  bool RemoveBatonSceneNode(
      gfl::grp::g3d::SceneNode* baton_scene_node
  )
  {
    return m_constraint.batons.RemoveBatonSceneNode(
      baton_scene_node
    );
  }

  //! @brief コールバック関数です。
  virtual void Invoke(nw::gfx::TransformNode* transformNode, nw::gfx::SceneContext* sceneContext)
  {
    GFL_UNUSED(sceneContext);

    if( transformNode != m_constraint.human_transform_node )
    {
      return;
    }

    for( u32 i=0; i<m_constraint.batons.baton_num; ++i )
    {
      ConstraintBaton*        baton = &m_constraint.batons.baton_array[i];
      nw::gfx::TransformNode* baton_nw_transform_node = baton->baton_scene_node->GetNwTransformNode();
      
      bool l_InheritTransform[ 3 ] = { baton->inherit_scale, baton->inherit_rotate, baton->inherit_translate };

      nw::gfx::CalculatedTransform worldPose = transformNode->WorldTransform();

      // オフセット付きワールド変換行列の計算
      CalculateWorldMatrixOfTransformNode(
          &baton_nw_transform_node->Transform().DirectTransformMatrix(),
          &baton_nw_transform_node->Transform().DirectScale(),
          *(baton->baton_scene_node->GetConstraintNwCalculatedTransform()),
          worldPose,
          l_InheritTransform
      );

/*
      GFL_PRINT(
          "[%d] (%f, %f, %f), (%f, %f, %f)\n",
          i,
          baton_nw_transform_node->Transform().DirectTransformMatrix().f._03,
          baton_nw_transform_node->Transform().DirectTransformMatrix().f._13,
          baton_nw_transform_node->Transform().DirectTransformMatrix().f._23,
          worldPose.TransformMatrix().f._03,
          worldPose.TransformMatrix().f._13,
          worldPose.TransformMatrix().f._23
      );
*/

      // キャッシュを無効化
      baton_nw_transform_node->Transform().EnableFlags(
          nw::gfx::CalculatedTransform::FLAG_IS_DIRTY
      );
    }
  }

private:
  ConstraintHumanOfTransformNode  m_constraint;
};


// コールバッククラスの生成
void CreateCallbackConstraint(
    CallbackConstraintOfSkeleton**      callback_constraint_of_skeleton,        // [out]
    CallbackConstraintOfTransformNode** callback_constraint_of_transform_node,  // [out]
    gfl::heap::NwAllocator*  heap_allocator,
    gfl::heap::NwAllocator*  device_allocator,
    nw::gfx::Model*          human_scene_node,
    u32                      human_baton_num_max,  // humanに拘束されるbatonの最大数
    u32                      hand_num_max,         // くっつけられるhandの最大数(human_scene_nodeのボーンの個数以下)
    u32                      hand_baton_num_max    // 1つのhandに拘束されるbatonの最大数
                                                   // (合計hand_num_max*hand_baton_num_max個のシーンノードをボーンにくっつけられる)
)
{
  *callback_constraint_of_skeleton = NULL;
  *callback_constraint_of_transform_node = NULL;
  
  if( hand_num_max > 0 && hand_baton_num_max > 0 )
  {
    nw::gfx::SkeletalModel* skeletal_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(human_scene_node);
    if( skeletal_model ) 
    {
      nw::gfx::Skeleton* skeleton = skeletal_model->GetSkeleton(); 
      CallbackConstraintOfSkeleton* l_callback_constraint_of_skeleton = GFL_NEW(heap_allocator->GetHeapBase()) CallbackConstraintOfSkeleton(
          heap_allocator,
          device_allocator,
          skeleton,
          hand_num_max,
          hand_baton_num_max
      );
      *callback_constraint_of_skeleton = l_callback_constraint_of_skeleton;
    }
  }

  if( human_baton_num_max > 0 )
  {
    CallbackConstraintOfTransformNode* l_callback_constraint_of_transform_node = GFL_NEW(heap_allocator->GetHeapBase()) CallbackConstraintOfTransformNode(
        heap_allocator,
        device_allocator,
        human_scene_node,
        human_baton_num_max
    );
    *callback_constraint_of_transform_node = l_callback_constraint_of_transform_node;
  }
}

// コールバッククラスの破棄
void DestroyCallbackConstraint(
    CallbackConstraintOfSkeleton**      callback_constraint_of_skeleton,       // [in,out]
    CallbackConstraintOfTransformNode** callback_constraint_of_transform_node  // [in,out]
)
{
  if( *callback_constraint_of_transform_node )
  {
    GFL_DELETE (*callback_constraint_of_transform_node);
    *callback_constraint_of_transform_node = NULL;
  }
  if( *callback_constraint_of_skeleton )
  {
    GFL_DELETE (*callback_constraint_of_skeleton);
    *callback_constraint_of_skeleton = NULL;
  }
}


}  // namespace internal


//-----------------------------------------------------------------------------
/**
 *					クラス定義
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3Dモデルクラス
//=====================================

#if GFL_DEBUG
  //----------------------------------------------------------------
  //
  // 安全にモデルが解放できたかを確認する機能の有効無効設定
  //
  //----------------------------------------------------------------
  b32 Model::s_SafeDeleteCheckEnable = false;
  b32 Model::s_SafeDeleteCheckAssert = false;
#endif    


// 設定内容
void Model::InitializeDescriptionForDefault(
    Description*  description
)
{
  description->material_buffer_option = MATERIAL_BUFFER_OPTION_NOT_USE;

  description->material_share_source_model             = NULL;
  description->mesh_node_visibility_share_source_model = NULL;

  description->max_callbacks            = 4;
  
  description->constraint_human_baton_num_max  = 0;
  description->constraint_hand_num_max         = 0;
  description->constraint_hand_baton_num_max   = 0;

  description->animation_slot_num       = 0;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit = 0;

  description->calculate_callback_num_max   = 1;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
 *                 Createは呼ばれません。
 */
//-----------------------------------------------------------------------------
Model::Model(void)
  : SceneNode(),
    //m_tree_flag(true),
    m_animation_slot_num(0),
    //m_animation_slot_array(NULL),
    m_animation_blend_unit_num(0),
    m_animation_num_in_unit(0),
    m_animation_manager(NULL),
    m_skeleton_share_type(SHARE_TYPE_NONE),
    m_material_share_type(SHARE_TYPE_NONE),
    m_mesh_node_visibility_share_type(SHARE_TYPE_NONE),
    m_material_buffer_option(MATERIAL_BUFFER_OPTION_NOT_USE),
    m_calculate_callback_num_max(0),
    m_calculate_callback_function_array(NULL),
    m_calculate_callback_work_array(NULL),
    m_callback_constraint_of_skeleton(NULL),
    m_callback_constraint_of_transform_node(NULL),
    m_replace_bone_data_array(NULL)
{
  STATIC_ASSERT(USER_DATA_INDEX_NOT_FOUND==-1);
  STATIC_ASSERT(BONE_INDEX_NOT_FOUND==-1);

  GFL_SINGLETON_INSTANCE(System)->GetModelManager()->AddObject(this);
}
//-----------------------------------------------------------------------------
/*
 *  @brief         デストラクタ
 *                 Destroyを呼びます。
 */
//-----------------------------------------------------------------------------
Model::~Model()
{
  Destroy();
  GFL_SINGLETON_INSTANCE(System)->GetModelManager()->RemoveObject(this);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         生成
 *
 *  @param[in]     allocator          アロケータ
 *  @param[in]     device_allocator   デバイスメモリのアロケータ
 *  @param[in]     resource           リソース
 *  @param[in]     index_in_resource  リソース内におけるインデックス
 *  @param[in]     description        設定内容
 */
//-----------------------------------------------------------------------------
void Model::Create(
    gfl::heap::NwAllocator*           allocator,
    gfl::heap::NwAllocator*           device_allocator,
    Resource*                         resource,
    s32                               index_in_resource,
    Description*                      description
)
{
  SceneNode::Create();
  
  Description l_description;
  if( description )
  {
    l_description = *description;
  }

  if( resource == NULL )
  {
    GFL_ASSERT_MSG( 0, "リソースが不正のためシーンオブジェクトが生成できませんでした。\n" );
    return;
  }
  resource->PrintUnresolvedReference();

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::gfx::ResModel res = nw_res_graphics_file->GetModels(index_in_resource);

  nw::gfx::Skeleton* skeleton_share_source_skeleton;
  m_skeleton_share_type = CheckSkeletonShareSourceModel(
    NULL,
    &skeleton_share_source_skeleton );

  nw::gfx::Model* material_share_source_nw_model;
  m_material_share_type = CheckMaterialShareSourceModel(
    l_description.material_share_source_model,
    l_description.material_buffer_option,
    &material_share_source_nw_model );

  nw::gfx::Model* mesh_node_visibility_share_source_nw_model;
  m_mesh_node_visibility_share_type = CheckMeshNodeVisibilityShareSourceModel(
    l_description.mesh_node_visibility_share_source_model,
    &mesh_node_visibility_share_source_nw_model );


#define GFL_GRP_G3D_MODEL_MEMORY_DEBUG_PRINT (GFL_DEBUG && (0))

#if GFL_GRP_G3D_MODEL_MEMORY_DEBUG_PRINT
  {
    nw::gfx::SceneBuilder scene_builder;
    scene_builder
        .Resource(res)
        .IsFixedSizeMemory(true)
        .MaxChildren(0)
        .MaxCallbacks(l_description.max_callbacks)
        .BufferOption( ConvertMaterialBufferOptionToNwBufferOption(l_description.material_buffer_option) )
        .SharedMaterialModel(material_share_source_nw_model)
        .SharedMeshNodeVisibiltiyModel(mesh_node_visibility_share_source_nw_model)
        .MaxAnimObjectsPerGroup(l_description.animation_slot_num)
        .ParticleSetMarginCount(0)
        .IsAnimationEnabled( (l_description.animation_slot_num>0)?(true):(false) );  // ここがfalseだとnw::gfx::AnimGroupが取得できない  // これを外部から設定できるようにするか、アニメーションなしモデルクラスをつくるか

            nw::gfx::SkeletalModel::Builder ske_model_builder;
            ske_model_builder
                .IsFixedSizeMemory(true)
                .MaxChildren(0)
                .MaxCallbacks(l_description.max_callbacks)
                .BufferOption( ConvertMaterialBufferOptionToNwBufferOption(l_description.material_buffer_option) )
                .SharedMaterialModel(material_share_source_nw_model)
                .SharedMeshNodeVisibilityModel(mesh_node_visibility_share_source_nw_model)
                .MaxAnimObjectsPerGroup(l_description.animation_slot_num)
                .IsAnimationEnabled( l_description.animation_slot_num>0)?(true):(false) );
   
    GFL_PRINT("[[[[decimal number]]]]\n");
    GFL_PRINT("hep_size=%d, dev_size=%d, alignment=default\n", scene_builder.GetMemorySize(), scene_builder.GetDeviceMemorySize() );
    GFL_PRINT("hep_size=%d, dev_size=%d, alignment=2\n", scene_builder.GetMemorySize(2), scene_builder.GetDeviceMemorySize(2) );
    GFL_PRINT("hep_size=%d, dev_size=%d, alignment=4\n", scene_builder.GetMemorySize(4), scene_builder.GetDeviceMemorySize(4) );
    // alignment=128だと
    // os_Memory.h:413 Panic:NW:Failed assertion alignment <= MAX_ALIGNMENT
    // となる。
    // class MemorySizeCalculator
    //   static const size_t MAX_ALIGNMENT = IAllocator::CACHE_LINE_ALIGNMENT;
    // class IAllocator
    //   static const int CACHE_LINE_ALIGNMENT = 32;
    // 最大32のようだ。
    GFL_PRINT("hep_size=%d, dev_size=%d, alignment=32\n", scene_builder.GetMemorySize(32), scene_builder.GetDeviceMemorySize(32) );
    
   
    // gfx_SkeletalModel.h
    // GetMemorySizeInternal
    {
      nw::gfx::ResSkeletalModel res_ske_model = nw::ut::ResStaticCast<nw::gfx::ResSkeletalModel>(res);
      GFL_PRINT("ske_model_builder  hep_size=%d, dev_size=%d\n", ske_model_builder.GetMemorySize(res_ske_model), ske_model_builder.GetDeviceMemorySize(res_ske_model) );
      
      nw::gfx::ResSkeleton res_ske = res_ske_model.GetSkeleton();
     
#ifdef NW_MOVE_ARRAY_CACHE_LINE_ALIGNMENT_ENABLED
      GFL_PRINT("alignment=%d\n", nw::gfx::Skeleton::TransformPose::TransformArray::MEMORY_ALIGNMENT);
#else
      GFL_PRINT("no alignment\n");
#endif
      
      GFL_PRINT("[+] Skeleton::TransformPose::Transform=%d * GetBonesCount=%d\n", sizeof(nw::gfx::Skeleton::TransformPose::Transform), res_ske.GetBonesCount());
     
      // gfx_StandardSkeleton.h
      // GetMemorySizeInternal
      {
        GFL_PRINT("[+] StandardSkeleton=%d\n", sizeof(nw::gfx::StandardSkeleton));
        GFL_PRINT("[+] Skeleton::TransformPose::Transform=%d * GetBonesCount=%d\n", sizeof(nw::gfx::Skeleton::TransformPose::Transform), res_ske.GetBonesCount());
        GFL_PRINT("[+] math::MTX34=%d * GetBonesCount=%d\n", sizeof(nw::math::MTX34), res_ske.GetBonesCount());
        GFL_PRINT("[+] math::MTX34=%d * GetBonesCount=%d\n", sizeof(nw::math::MTX34), res_ske.GetBonesCount());
     
        // gfx_Skeleton.h
        // GetMemorySizeForConstruct
        {
          GFL_PRINT("maxCallbacks=%d\n", l_description.max_callbacks);
          if( l_description.max_callbacks == 0 )
          {
            // ut/generated/ut_Signal.hi
            // GetMemorySizeForInvalidateSignalInternal
            GFL_PRINT("[+] CalculateMatrixSignal=%d\n", sizeof(nw::gfx::Skeleton::CalculateMatrixSignal));
            GFL_PRINT("[+] CalculateMatrixSignal=%d\n", sizeof(nw::gfx::Skeleton::CalculateMatrixSignal));
          }
          else
          {
            // ut/generated/ut_Signal.hi
            // GetMemorySizeForFixedSizedSignalInternal
#ifdef NW_SIGNAL_MULTI_SLOT_ENABLED
            GFL_PRINT("multi_slot=%d\n", l_description.max_callbacks);
#else
            GFL_PRINT("no multi_slot\n");
#endif
            GFL_PRINT("[+] CalculateMatrixSignal=%d\n", sizeof(nw::gfx::Skeleton::CalculateMatrixSignal));
            GFL_PRINT("[+] CalculateMatrixSignal=%d * maxCallbacks=%d\n", sizeof(nw::gfx::Skeleton::CalculateMatrixSignal), l_description.max_callbacks);
          }
        }
      }
     
      GFL_PRINT("[+] SkeletalModel=%d\n", sizeof(nw::gfx::SkeletalModel));
     
#if 0
//      ここから先どんどん深くなるので、追うのをやめた。

      // gfx_SkeletalModel.h
      // GetMemorySizeForInitialize
      {
              SkeletalModel::GetMemorySizeForInitialize(pSize, resModel, m_Description);
      }
    
    static void GetMemorySizeForInitialize(
        os::MemorySizeCalculator* pSize,
        ResSkeletalModel resModel,
        Description description)
    {
        // SkeletalModel::Initialize
        os::MemorySizeCalculator& size = *pSize;

        Model::GetMemorySizeForInitialize(pSize, resModel, description);

        // SkeletalModel::CreateSkeletalAnimGroup
        if (description.isAnimationEnabled)
        {
            const int animGroupCount = resModel.GetAnimGroupsCount();
            for (int animGroupIdx = 0; animGroupIdx < animGroupCount; ++animGroupIdx)
            {
                anim::ResAnimGroup resAnimGroup = resModel.GetAnimGroups(animGroupIdx);
                const int targetType = resAnimGroup.GetTargetType();
                const bool transformFlag = 
                    (resAnimGroup.GetFlags() & anim::ResAnimGroup::FLAG_IS_CALCULATED_TRANSFORM) != 0;
                if (transformFlag &&
                    targetType == anim::ResGraphicsAnimGroup::TARGET_TYPE_BONE)
                {
                    AnimGroup::Builder()
                        .ResAnimGroup(resAnimGroup)
                        .UseOriginalValue(true)
                        .GetMemorySizeInternal(pSize);
                    break; // 他にスケルタルモデル専用のアニメーショングループがないので
                }
            }
        }
    }

#endif 
    }  
  }
#endif  // #if GFL_GRP_G3D_MODEL_MEMORY_DEBUG_PRINT

#if GFL_GRP_G3D_MODEL_MEMORY_DEBUG_PRINT
gfl::debug::AddPrintFlag( gfl::debug::PRINT_FLAG_LEVEL1|gfl::debug::PRINT_FLAG_CATEGORY_HEAP );
#endif

  nw::gfx::SceneObject* scene_object = nw::gfx::SceneBuilder()
      .Resource(res)
      .IsFixedSizeMemory(true)
      .MaxChildren(0)
      .MaxCallbacks(l_description.max_callbacks)
      .BufferOption( ConvertMaterialBufferOptionToNwBufferOption(l_description.material_buffer_option) )
      .SharedMaterialModel(material_share_source_nw_model)
      .SharedMeshNodeVisibiltiyModel(mesh_node_visibility_share_source_nw_model)
      .MaxAnimObjectsPerGroup(l_description.animation_slot_num)
      .ParticleSetMarginCount(0)
      .IsAnimationEnabled( (l_description.animation_slot_num>0)?(true):(false) )  // ここがfalseだとnw::gfx::AnimGroupが取得できない  // これを外部から設定できるようにするか、アニメーションなしモデルクラスをつくるか
      .CreateObject(allocator, device_allocator);

#if GFL_GRP_G3D_MODEL_MEMORY_DEBUG_PRINT
gfl::debug::ResetPrintFlag();
#endif

  GFL_ASSERT_STOP_MSG( scene_object, "index_in_resource=%dのシーンオブジェクトが生成できませんでした。\n", index_in_resource );  //@fix STOPに変更

  nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
  SetNwSceneNode(scene_node);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);

  // 独自処理 ここから
  m_material_buffer_option = l_description.material_buffer_option;

  AddRenderQueue(0);  // デフォルトで0番のレンダーキューで描画する
  // UserParameterをRenderQueueには使っていないことが判明したので、
  // SafeDeleteCheck用の使わせてもらう。

  //m_tree_flag = true;

#if 0
  m_animation_slot_num = l_description.animation_slot_num;
  if( m_animation_slot_num > 0 )
  {
    m_animation_slot_array = GFL_NEW_ARRAY(allocator->GetHeapBase()) AnimationSlot[m_animation_slot_num];
    for( s32 i=0; i<m_animation_slot_num; ++i )
    {
      m_animation_slot_array[i].SetObjectIndex(i);
    }
  }
#endif
  m_animation_slot_num = l_description.animation_slot_num;
  m_animation_blend_unit_num = l_description.animation_blend_unit_num;
  m_animation_num_in_unit = l_description.animation_num_in_unit;
  //m_animation_manager = GFL_NEW(allocator->GetHeapBase()) AnimationManager;
  //m_animation_manager->Slot_Create(
  //    allocator,
  //    device_allocator,
  //    this,
  //    l_description.animation_slot_num
  //);

  m_calculate_callback_num_max = l_description.calculate_callback_num_max;
  if(m_calculate_callback_num_max > 0)
  {
    m_calculate_callback_function_array = GFL_NEW_ARRAY(allocator->GetHeapBase()) ModelCalculateCallbackFunction[m_calculate_callback_num_max];
    m_calculate_callback_work_array = GFL_NEW_ARRAY(allocator->GetHeapBase()) void*[m_calculate_callback_num_max];
    for( u32 i=0; i<m_calculate_callback_num_max; ++i )
    {
      m_calculate_callback_function_array[i] = NULL;
      m_calculate_callback_work_array[i] = NULL;
    }
  }
  internal::CreateCallbackConstraint(
    &m_callback_constraint_of_skeleton,
    &m_callback_constraint_of_transform_node,
    allocator,
    device_allocator,
    GetNwModel(),
    l_description.constraint_human_baton_num_max,
    l_description.constraint_hand_num_max,
    l_description.constraint_hand_baton_num_max
  );
  // 独自処理 ここまで


#if GFL_DEBUG
  RemoveAllRenderQueue();  // 途中でs_SafeDeleteCheckEnableをONにしたときに対応できるようにNULLにしておく。
  if(s_SafeDeleteCheckEnable)
  {
    SetSafeDeleteCheckInfo(res.GetName());
  }
#endif

}

//-----------------------------------------------------------------------------
/*
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
void Model::CreateSkeletalModelSharingSkeleton(
    gfl::heap::NwAllocator*           allocator,
    gfl::heap::NwAllocator*           device_allocator,
    Resource*                         resource,
    s32                               index_in_resource,
    Model*                            skeleton_share_source_model,
    Description*                      description
)
{
  SceneNode::Create();

  Description l_description;
  if( description )
  {
    l_description = *description;
  }

  if( resource == NULL )
  {
    GFL_ASSERT_MSG( 0, "リソースが不正のためシーンオブジェクトが生成できませんでした。\n" );
    return;
  }
  resource->PrintUnresolvedReference();

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::gfx::ResModel res = nw_res_graphics_file->GetModels(index_in_resource);

  // スケルトン共有チェック
  if( !skeleton_share_source_model )
  {
    GFL_ASSERT_MSG( 0, "スケルトンの共有元のモデルが不正のためモデルが生成できませんでした。\n" );
    return;
  }

  nw::gfx::Skeleton* skeleton_share_source_skeleton;
  m_skeleton_share_type = CheckSkeletonShareSourceModel(
    skeleton_share_source_model,
    &skeleton_share_source_skeleton );

  nw::gfx::Model* material_share_source_nw_model;
  m_material_share_type = CheckMaterialShareSourceModel(
    l_description.material_share_source_model,
    l_description.material_buffer_option,
    &material_share_source_nw_model );

  nw::gfx::Model* mesh_node_visibility_share_source_nw_model;
  m_mesh_node_visibility_share_type = CheckMeshNodeVisibilityShareSourceModel(
    l_description.mesh_node_visibility_share_source_model,
    &mesh_node_visibility_share_source_nw_model );
  
  nw::gfx::SkeletalModel* skeletal_model = nw::gfx::SkeletalModel::Builder()
    .IsFixedSizeMemory(true)
    .MaxChildren(0)
    .MaxCallbacks(l_description.max_callbacks)
    .BufferOption( ConvertMaterialBufferOptionToNwBufferOption(l_description.material_buffer_option) )
    .SharedSkeleton( skeleton_share_source_skeleton )
    .SharedMaterialModel(material_share_source_nw_model)
    .SharedMeshNodeVisibilityModel(mesh_node_visibility_share_source_nw_model)
    .MaxAnimObjectsPerGroup( l_description.animation_slot_num )
    .IsAnimationEnabled( (l_description.animation_slot_num>0)?(true):(false) )  // ここがfalseだとnw::gfx::AnimGroupが取得できない  // これを外部から設定できるようにするか、アニメーションなしモデルクラスをつくるか
    .Create( NULL, res, device_allocator );

  GFL_ASSERT_STOP_MSG( skeletal_model, "index_in_resource=%dのシーンオブジェクトが生成できませんでした。\n", index_in_resource );  //@fix STOPにした

  SetNwSceneNode(skeletal_model);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);

  // 独自処理 ここから
  m_material_buffer_option = l_description.material_buffer_option;

  AddRenderQueue(0);  // デフォルトで0番のレンダーキューで描画する
  // UserParameterをRenderQueueには使っていないことが判明したので、
  // SafeDeleteCheck用の使わせてもらう。

  //m_tree_flag = true;
#if 0
  m_animation_slot_num = l_description.animation_slot_num;
  if( m_animation_slot_num > 0 )
  {
    m_animation_slot_array = GFL_NEW_ARRAY(allocator->GetHeapBase()) AnimationSlot[m_animation_slot_num];
    for( s32 i=0; i<m_animation_slot_num; ++i )
    {
      m_animation_slot_array[i].SetObjectIndex(i);
    }
  }
#endif
  m_animation_slot_num = l_description.animation_slot_num;
  m_animation_blend_unit_num = l_description.animation_blend_unit_num;
  m_animation_num_in_unit = l_description.animation_num_in_unit;
  //m_animation_manager = GFL_NEW(allocator->GetHeapBase()) AnimationManager;
  //m_animation_manager->Slot_Create(
  //    allocator,
  //    device_allocator,
  //    this,
  //    l_description.animation_slot_num
  //);

  m_calculate_callback_num_max = l_description.calculate_callback_num_max;
  if(m_calculate_callback_num_max > 0)
  {
    m_calculate_callback_function_array = GFL_NEW_ARRAY(allocator->GetHeapBase()) ModelCalculateCallbackFunction[m_calculate_callback_num_max];
    m_calculate_callback_work_array = GFL_NEW_ARRAY(allocator->GetHeapBase()) void*[m_calculate_callback_num_max];
    for( u32 i=0; i<m_calculate_callback_num_max; ++i )
    {
      m_calculate_callback_function_array[i] = NULL;
      m_calculate_callback_work_array[i] = NULL;
    }
  }
  internal::CreateCallbackConstraint(
    &m_callback_constraint_of_skeleton,
    &m_callback_constraint_of_transform_node,
    allocator,
    device_allocator,
    GetNwModel(),
    l_description.constraint_human_baton_num_max,
    l_description.constraint_hand_num_max,
    l_description.constraint_hand_baton_num_max
  );
  // 独自処理 ここまで
 

#if GFL_DEBUG
  RemoveAllRenderQueue();  // 途中でs_SafeDeleteCheckEnableをONにしたときに対応できるようにNULLにしておく。
  if(s_SafeDeleteCheckEnable)
  {
    SetSafeDeleteCheckInfo(res.GetName());
  }
#endif

}

//-----------------------------------------------------------------------------
/*
 *  @brief         破棄
 *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
 */
//-----------------------------------------------------------------------------
void Model::Destroy(void)
{
  // 独自処理 ここから
  DestroyReplaceBoneData();
  internal::DestroyCallbackConstraint(
    &m_callback_constraint_of_skeleton,
    &m_callback_constraint_of_transform_node
  );
  if( m_calculate_callback_work_array )
  {
    GFL_DELETE_ARRAY m_calculate_callback_work_array;
    m_calculate_callback_work_array = NULL;
  }
  if( m_calculate_callback_function_array )
  {
    GFL_DELETE_ARRAY m_calculate_callback_function_array;
    m_calculate_callback_function_array = NULL;
  }
#if 0
  if( m_animation_slot_array )
  {
    GFL_DELETE_ARRAY m_animation_slot_array;
    m_animation_slot_array = NULL;
  }
#endif
  if( m_animation_manager )
  {
    GFL_DELETE m_animation_manager;
    m_animation_manager = NULL;
  }

  //m_tree_flag = true;
  m_skeleton_share_type = SHARE_TYPE_NONE;
  m_material_share_type = SHARE_TYPE_NONE;
  m_mesh_node_visibility_share_type = SHARE_TYPE_NONE;
  m_material_buffer_option = MATERIAL_BUFFER_OPTION_NOT_USE;
  m_animation_slot_num = 0;
  m_animation_blend_unit_num = 0;
  m_animation_num_in_unit = 0;
  m_calculate_callback_num_max = 0;
  // 独自処理 ここまで

  SceneNode::Destroy();


#if GFL_DEBUG
  if(s_SafeDeleteCheckEnable)
  {
    u32   cnt = gfl::grp::GraphicsSystem::GetDrawFrameCount();

    if ( ( m_DrawFrameCount + 2 ) > cnt )
    {
      if( s_SafeDeleteCheckAssert )
      {
        GFL_ASSERT_MSG( 0, "gfxモデル%sは描画中に解放されました。 : 最後にコマンドが積まれたフレーム = %d : 破棄フレーム = %d\n\n", m_ModelName, m_DrawFrameCount, cnt );
      }
      else
      {
        GFL_PRINT( "gfxモデル%sは描画中に解放されました。 : 最後にコマンドが積まれたフレーム = %d : 破棄フレーム = %d\n\n", m_ModelName, m_DrawFrameCount, cnt );
      }
    }
  }
#endif

}

//-----------------------------------------------------------------------------
/*
 *  @brief         計算処理
 */
//-----------------------------------------------------------------------------
void Model::Calculate(void)
{
  // 計算処理にて呼び出されるコールバック
  for( u32 i=0; i<m_calculate_callback_num_max; ++i )
  {
    if( m_calculate_callback_function_array[i] )
    {
      m_calculate_callback_function_array[i](this, m_calculate_callback_work_array[i]);
    }
  }

#if 0
  // アニメーションスロット
  for( u32 i=0; i<m_animation_slot_num; i++ )
  {
    m_animation_slot_array[i].Calculate();
  }
#endif
  // アニメーションマネージャ
  if( m_animation_manager )
  {
    m_animation_manager->Calculate();
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         座標変換
 */
//-----------------------------------------------------------------------------
void Model::SetTransformMatrix(const gfl::math::MTX34& m)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetTransformMatrix( m );
}
void Model::SetTransformMatrix(
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
)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetTransformMatrix(
    m00,
    m01,
    m02,
    m03,
    m10,
    m11,
    m12,
    m13,
    m20,
    m21,
    m22,
    m23
  );
}
void Model::GetTransformMatrix(gfl::math::MTX34* m) const  // 自分のスケールは掛かっておらず、親のスケールは掛かっています。
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  *m = transform_node->Transform().TransformMatrix();
}
gfl::math::MTX34 Model::GetTransformMatrix(void) const  // 自分のスケールは掛かっておらず、親のスケールは掛かっています。
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  gfl::math::MTX34 m = transform_node->Transform().TransformMatrix();
  return m;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         平行移動
 */
//-----------------------------------------------------------------------------
void Model::SetTranslate(f32 x, f32 y, f32 z)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetTranslate( x, y, z );
}
void Model::SetTranslate(const gfl::math::VEC3& t)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetTranslate( t );
}
void Model::GetTranslate(gfl::math::VEC3* t) const
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().GetTranslate( t );
}
gfl::math::VEC3 Model::GetTranslate(void) const
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  return transform_node->Transform().GetTranslate();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         回転
 *                 単位はラジアン。
 */
//-----------------------------------------------------------------------------
void Model::SetRotateXYZ(f32 x, f32 y, f32 z)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetRotateXYZ( x, y, z );
}
void Model::SetRotateXYZ(const gfl::math::VEC3& t)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetRotateXYZ( t.x, t.y, t.z );
}
void Model::GetRotateXYZ(gfl::math::VEC3* t) const  // 【注意】SetRotateXYZで渡された値を返すだけです。SetRotateXYZ以外で回転を指定していた場合は
{                                                   //         見た目と合わない値が返ってきます。
  *t = m_rotate;
}
gfl::math::VEC3 Model::GetRotateXYZ(void) const  // 【注意】SetRotateXYZで渡された値を返すだけです。SetRotateXYZ以外で回転を指定していた場合は
{                                                //         見た目と合わない値が返ってきます。
  return m_rotate;
}

void Model::SetRotateMatrix(
    f32 m00,
    f32 m01,
    f32 m02,
    f32 m10,
    f32 m11,
    f32 m12,
    f32 m20,
    f32 m21,
    f32 m22
)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetRotateMatrix(
    m00,
    m01,
    m02,
    m10,
    m11,
    m12,
    m20,
    m21,
    m22
  );
}

//-----------------------------------------------------------------------------
/*
 *  @brief         スケール
 */
//-----------------------------------------------------------------------------
void Model::SetScale(f32 x, f32 y, f32 z)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetScale( x, y, z );
}
void Model::SetScale(const gfl::math::VEC3& t)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetScale( t );
}
void Model::GetScale(gfl::math::VEC3* t) const
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  *t = transform_node->Transform().Scale();
}
gfl::math::VEC3 Model::GetScale(void) const
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  gfl::math::VEC3 t = transform_node->Transform().Scale();
  return t;
}

#if 0
//他のモデルに移した後の動作がよくないので、この関数はもっとテストしてからでないと使えない。
//m_skeleton_type -> m_skeleton_share_type
//SKELETON_TYPE_ -> SHARE_TYPE_
//-----------------------------------------------------------------------------
/*
 *  @brief         スケルトンの共有元である権利を他のモデルに移す
 *                 thisがスケルトンの共有元であり、
 *                 skeleton_share_new_source_modelがそのスケルトンを共有しているモデルである必要があります。
 *
 *  @param[in]     skeleton_share_new_source_model  新しくスケルトンの共有元の権利を与えられるモデル
 */
//-----------------------------------------------------------------------------
void Model::ChangeSkeletonShareSource(Model* skeleton_share_new_source_model)
{
  if(    ( this->m_skeleton_type                            != SKELETON_TYPE_SHARE_SOURCE      )
      || ( skeleton_share_new_source_model->m_skeleton_type != SKELETON_TYPE_SHARE_DESTINATION ) )
  {
    GFL_ASSERT_MSG( 0, "スケルトンの共有設定が異常のため、スケルトンの共有元である権利を他のモデルに移せませんでした。\n" );
    return;
  }

  nw::gfx::SkeletalModel* skeletal_model = \
      nw::ut::DynamicCast<nw::gfx::SkeletalModel*>( this->GetNwSceneNode() );

  nw::gfx::SkeletalModel* skeleton_share_new_source_skeletal_model = \
      nw::ut::DynamicCast<nw::gfx::SkeletalModel*>( skeleton_share_new_source_model->GetNwSceneNode() );

  skeletal_model->SwapSkeleton( skeleton_share_new_source_skeletal_model );

  this->m_skeleton_type                            = SKELETON_TYPE_SHARE_DESTINATION;
  skeleton_share_new_source_model->m_skeleton_type = SKELETON_TYPE_SHARE_SOURCE;
}
#endif

//-----------------------------------------------------------------------------
/*
 *  @brief         描画順
 *                 プログラマだけが設定できるモデルのレイヤーID
 *                 最初に描画 0 <= u8 <= 127 最後に描画
 *                 最優先される描画順 ModelLayerId > MaterialTranslucencyKind > その他 最劣後される描画順
 *
 *                 **** ModelとParticleにあり全部を一列に並べた統一番号です。デフォルト0000000です。 ****
 */
//-----------------------------------------------------------------------------
void Model::SetModelLayerId(u8 model_layer_id)
{
  // u8をずらして使う。最後の1ビットにはModel0、Particle1のデフォルト値が入れてある。
  GFL_ASSERT(model_layer_id <= 127);  //@check 描画順が狂うだけ
  
  nw::gfx::Model* model = GetNwModel();
  model->SetLayerId((model_layer_id << 1));
  model->InvalidateRenderKeyCache();  // これがないと描画順が変わらない
}

/*
[質問] NintendoWare上での描画優先度の指定について

開発環境 : CTR-SDK 0.9.7/NintendoWare 0.7.4

田中＠NTSC 2010-06-03 09:02:17  
>>レイヤID ＞ 透明性 ＞ メッシュ描画優先度 ＞ マテリアルID、深度値
>こちらに関しては確認しておりますのでお待ちください。
こちらに関しては一応これで正しいとのことです。

ただ現状ではマテリアル優先のソートを改良中で一時的に描画優先度をはずしており、
次回以降で0～16段階でソートをできるように修正する予定とのことです
*/

u8 Model::GetModelLayerId(void) const
{
  // u8をずらして使う。最後の1ビットにはModel0、Particle1のデフォルト値が入れてある。
  return ((GetNwModel()->GetLayerId()) >> 1);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         マテリアル
 */
//-----------------------------------------------------------------------------
s32 Model::GetMaterialCount() const
{
  s32 count = 0;
  nw::gfx::Model* model = GetNwModel();
  if( model )
  {
    count = model->GetMaterialCount();
  }
  return count;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         マテリアルのインデックス番号を取得する
 *
 *  @param[in]     key  マテリアルの名前
 * 
 *  @retval        s32    マテリアルのインデックス番号
 *                        指定した名前を持つマテリアルが存在しない場合、MATERIAL_INDEX_NOT_FOUNDを返す
 */
//-----------------------------------------------------------------------------
s32 Model::GetMaterialIndex(const char* key) const
{
  s32              count   = GetMaterialCount();
  nw::gfx::Model*  model   = GetNwModel();
  for( s32 i=0; i<count; ++i )
  {
    nw::gfx::Material*  material = model->GetMaterial(i);
    if( gfl::std::StringCmp( key, material->GetName() ) == 0 ) return i;
  }
  return MATERIAL_INDEX_NOT_FOUND;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         マテリアルカラー
 *                 u8のときは0が黒&透明、255が白&不透明
 *                 f32のときは0.0fが黒&透明、1.0fが白&不透明
 */
//-----------------------------------------------------------------------------
// ディフューズカラー
void Model::SetMaterialColorDiffuse(s32 material_index, f32 r, f32 g, f32 b)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();
 

  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  res_material_color.SetDiffuse(r, g, b); 
  res_material.SetMaterialColorHash(0x0);  // これがないと反映されない


  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
void Model::SetMaterialColorDiffuse(s32 material_index, f32 r, f32 g, f32 b, f32 a)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  res_material_color.SetDiffuse(r, g, b, a); 
  res_material.SetMaterialColorHash(0x0);  // これがないと反映されない


  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
void Model::SetMaterialColorDiffuse(s32 material_index, const gfl::grp::ColorF32& c)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  res_material_color.SetDiffuse(c); 
  res_material.SetMaterialColorHash(0x0);  // これがないと反映されない


  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
void Model::SetMaterialColorDiffuse(s32 material_index, u8 r, u8 g, u8 b)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  res_material_color.SetDiffuse(
      (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX
  ); 
  res_material.SetMaterialColorHash(0x0);  // これがないと反映されない


  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
void Model::SetMaterialColorDiffuse(s32 material_index, u8 r, u8 g, u8 b, u8 a)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  res_material_color.SetDiffuse(
      (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(a))/gfl::grp::ColorU8::ELEMENT_MAX
  ); 
  res_material.SetMaterialColorHash(0x0);  // これがないと反映されない


  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
void Model::SetMaterialColorDiffuse(s32 material_index, const gfl::grp::ColorU8& c)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();

  
  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  res_material_color.SetDiffuse(
      (static_cast<f32>(c.r))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(c.g))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(c.b))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(c.a))/gfl::grp::ColorU8::ELEMENT_MAX
  ); 
  res_material.SetMaterialColorHash(0x0);  // これがないと反映されない


  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
gfl::grp::ColorF32 Model::GetMaterialColorDiffuse(s32 material_index) const
{
  gfl::grp::ColorF32 c(0.0f, 0.0f, 0.0f, 0.0f);

  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  c = res_material_color.GetDiffuse(); 


  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }

  return c;
}

void Model::GetMaterialColorDiffuse(s32 material_index, gfl::grp::ColorF32* c) const
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  *c = res_material_color.GetDiffuse(); 


  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
void Model::GetMaterialColorDiffuse(s32 material_index, gfl::grp::ColorU8*  c) const
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  gfl::grp::ColorF32 c_f32 = res_material_color.GetDiffuse(); 
  c->Set(
    static_cast<s32>(c_f32.r*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.g*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.b*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.a*gfl::grp::ColorU8::ELEMENT_MAX)
  );


  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}

// コンスタントカラー
void Model::SetMaterialColorConstant(s32 material_index, s32 no, f32 r, f32 g, f32 b)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();
 

  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  switch(no)
  {
  case 0: res_material_color.SetConstant0(r, g, b); break;
  case 1: res_material_color.SetConstant1(r, g, b); break;
  case 2: res_material_color.SetConstant2(r, g, b); break;
  case 3: res_material_color.SetConstant3(r, g, b); break;
  case 4: res_material_color.SetConstant4(r, g, b); break;
  case 5: res_material_color.SetConstant5(r, g, b); break;
  default:
    GFL_ASSERT_MSG( 0, "コンスタントカラーの番号が異常です。\n" );
  }
  res_material.SetMaterialColorHash(0x0);  // これがないと反映されない


  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
void Model::SetMaterialColorConstant(s32 material_index, s32 no, f32 r, f32 g, f32 b, f32 a)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();
 

  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  switch(no)
  {
  case 0: res_material_color.SetConstant0(r, g, b, a); break;
  case 1: res_material_color.SetConstant1(r, g, b, a); break;
  case 2: res_material_color.SetConstant2(r, g, b, a); break;
  case 3: res_material_color.SetConstant3(r, g, b, a); break;
  case 4: res_material_color.SetConstant4(r, g, b, a); break;
  case 5: res_material_color.SetConstant5(r, g, b, a); break;
  default:
    GFL_ASSERT_MSG( 0, "コンスタントカラーの番号が異常です。\n" );
  }
  res_material.SetMaterialColorHash(0x0);  // これがないと反映されない


  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
void Model::SetMaterialColorConstant(s32 material_index, s32 no, const gfl::grp::ColorF32& c)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();
 

  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  switch(no)
  {
  case 0: res_material_color.SetConstant0(c); break;
  case 1: res_material_color.SetConstant1(c); break;
  case 2: res_material_color.SetConstant2(c); break;
  case 3: res_material_color.SetConstant3(c); break;
  case 4: res_material_color.SetConstant4(c); break;
  case 5: res_material_color.SetConstant5(c); break;
  default:
    GFL_ASSERT_MSG( 0, "コンスタントカラーの番号が異常です。\n" );
  }
  res_material.SetMaterialColorHash(0x0);  // これがないと反映されない


  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
void Model::SetMaterialColorConstant(s32 material_index, s32 no, u8 r, u8 g, u8 b)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();
 
    f32 fr = (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX;
    f32 fg = (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX;
    f32 fb = (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX;


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  switch(no)
  {
  case 0: res_material_color.SetConstant0(fr, fg, fb); break;
  case 1: res_material_color.SetConstant1(fr, fg, fb); break;
  case 2: res_material_color.SetConstant2(fr, fg, fb); break;
  case 3: res_material_color.SetConstant3(fr, fg, fb); break;
  case 4: res_material_color.SetConstant4(fr, fg, fb); break;
  case 5: res_material_color.SetConstant5(fr, fg, fb); break;
  default:
    GFL_ASSERT_MSG( 0, "コンスタントカラーの番号が異常です。\n" );
  }
  res_material.SetMaterialColorHash(0x0);  // これがないと反映されない


  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
void Model::SetMaterialColorConstant(s32 material_index, s32 no, u8 r, u8 g, u8 b, u8 a)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();
 
    f32 fr = (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX;
    f32 fg = (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX;
    f32 fb = (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX;
    f32 fa = (static_cast<f32>(a))/gfl::grp::ColorU8::ELEMENT_MAX;


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  switch(no)
  {
  case 0: res_material_color.SetConstant0(fr, fg, fb, fa); break;
  case 1: res_material_color.SetConstant1(fr, fg, fb, fa); break;
  case 2: res_material_color.SetConstant2(fr, fg, fb, fa); break;
  case 3: res_material_color.SetConstant3(fr, fg, fb, fa); break;
  case 4: res_material_color.SetConstant4(fr, fg, fb, fa); break;
  case 5: res_material_color.SetConstant5(fr, fg, fb, fa); break;
  default:
    GFL_ASSERT_MSG( 0, "コンスタントカラーの番号が異常です。\n" );
  }
  res_material.SetMaterialColorHash(0x0);  // これがないと反映されない


  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
void Model::SetMaterialColorConstant(s32 material_index, s32 no, const gfl::grp::ColorU8& c)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();

    f32 fr = (static_cast<f32>(c.r))/gfl::grp::ColorU8::ELEMENT_MAX;
    f32 fg = (static_cast<f32>(c.g))/gfl::grp::ColorU8::ELEMENT_MAX;
    f32 fb = (static_cast<f32>(c.b))/gfl::grp::ColorU8::ELEMENT_MAX;
    f32 fa = (static_cast<f32>(c.a))/gfl::grp::ColorU8::ELEMENT_MAX;
 

  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  switch(no)
  {
  case 0: res_material_color.SetConstant0(fr, fg, fb, fa); break;
  case 1: res_material_color.SetConstant1(fr, fg, fb, fa); break;
  case 2: res_material_color.SetConstant2(fr, fg, fb, fa); break;
  case 3: res_material_color.SetConstant3(fr, fg, fb, fa); break;
  case 4: res_material_color.SetConstant4(fr, fg, fb, fa); break;
  case 5: res_material_color.SetConstant5(fr, fg, fb, fa); break;
  default:
    GFL_ASSERT_MSG( 0, "コンスタントカラーの番号が異常です。\n" );
  }
  res_material.SetMaterialColorHash(0x0);  // これがないと反映されない


  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
gfl::grp::ColorF32 Model::GetMaterialColorConstant(s32 material_index, s32 no) const
{
  gfl::grp::ColorF32 c(0.0f, 0.0f, 0.0f, 0.0f);

  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  switch(no)
  {
  case 0: c = res_material_color.GetConstant0(); break;
  case 1: c = res_material_color.GetConstant1(); break;
  case 2: c = res_material_color.GetConstant2(); break;
  case 3: c = res_material_color.GetConstant3(); break;
  case 4: c = res_material_color.GetConstant4(); break;
  case 5: c = res_material_color.GetConstant5(); break;
  default:
    GFL_ASSERT_MSG( 0, "コンスタントカラーの番号が異常です。\n" );
  }


  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }

  return c;
}
void Model::GetMaterialColorConstant(s32 material_index, s32 no, gfl::grp::ColorF32* c) const
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  switch(no)
  {
  case 0: *c = res_material_color.GetConstant0(); break;
  case 1: *c = res_material_color.GetConstant1(); break;
  case 2: *c = res_material_color.GetConstant2(); break;
  case 3: *c = res_material_color.GetConstant3(); break;
  case 4: *c = res_material_color.GetConstant4(); break;
  case 5: *c = res_material_color.GetConstant5(); break;
  default:
    GFL_ASSERT_MSG( 0, "コンスタントカラーの番号が異常です。\n" );
  }


  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
void Model::GetMaterialColorConstant(s32 material_index, s32 no, gfl::grp::ColorU8*  c) const
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  gfl::grp::ColorF32 c_f32;
  switch(no)
  {
  case 0: c_f32 = res_material_color.GetConstant0(); break;
  case 1: c_f32 = res_material_color.GetConstant1(); break;
  case 2: c_f32 = res_material_color.GetConstant2(); break;
  case 3: c_f32 = res_material_color.GetConstant3(); break;
  case 4: c_f32 = res_material_color.GetConstant4(); break;
  case 5: c_f32 = res_material_color.GetConstant5(); break;
  default:
    GFL_ASSERT_MSG( 0, "コンスタントカラーの番号が異常です。\n" );
  }
  c->Set(
    static_cast<s32>(c_f32.r*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.g*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.b*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.a*gfl::grp::ColorU8::ELEMENT_MAX)
  );


  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         マテリアルが影響を受けるフォグ
 */
//-----------------------------------------------------------------------------
void Model::SetMaterialFogEnable(s32 material_index, bool enable)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    //nw::gfx::res::ResMaterial& res_material = material->GetSceneEnvironmentResMaterial();
    nw::gfx::res::ResMaterial  res_material = material->GetShadingParameterResMaterial();  // フォグはこのマテリアル
    u32 flags = res_material.GetFlags();
    u32 fog_bit = nw::gfx::res::ResMaterialData::FLAG_FOG_ENABLED;
    if(enable)
    {
      flags |= fog_bit;
    }
    else
    {
      flags &= ~fog_bit;
    }
    res_material.SetFlags(flags);
    // SceneEnvironmentResMaterial用のSetMaterial*Hash関数がないが反映されているか？
    res_material.SetShadingParameterHash(0x0);  // フォグはこのHash
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
bool Model::IsMaterialFogEnable(s32 material_index) const
{
  bool enable = false;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    //nw::gfx::res::ResMaterial& res_material = material->GetSceneEnvironmentResMaterial();
    nw::gfx::res::ResMaterial res_material = material->GetShadingParameterResMaterial();  // フォグはこのマテリアル
    u32 flags = res_material.GetFlags();
    u32 fog_bit = nw::gfx::res::ResMaterialData::FLAG_FOG_ENABLED;
    flags &= fog_bit;
    if(flags)
    {
      enable = true;
    }
    else
    {
      enable = false;
    }
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return enable;
}
void Model::SetMaterialFogIndex(s32 material_index, s32 index)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    //nw::gfx::res::ResMaterial& res_material = material->GetSceneEnvironmentResMaterial();
    nw::gfx::res::ResMaterial  res_material = material->GetShadingParameterResMaterial();  // フォグはこのマテリアル
    res_material.SetFogIndex(index);
    // SceneEnvironmentResMaterial用のSetMaterial*Hash関数がないが反映されているか？
    res_material.SetShadingParameterHash(0x0);  // フォグはこのHash
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
s32  Model::GetMaterialFogIndex(s32 material_index) const
{
  s32 index = 0;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    //nw::gfx::res::ResMaterial& res_material = material->GetSceneEnvironmentResMaterial();
    nw::gfx::res::ResMaterial  res_material = material->GetShadingParameterResMaterial();  // フォグはこのマテリアル
    index = res_material.GetFogIndex();
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return index;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         マテリアルが影響を受けるライト
 */
//-----------------------------------------------------------------------------
//void SetMaterialFragmentLightEnable(s32 material_index, bool enable);
//bool IsMaterialFragmentLightEnable(s32 material_index) const;
//void SetMaterialVertexLightEnable(s32 material_index, bool enable);
//bool IsMaterialVertexLightEnable(s32 material_index) const;
//void SetMaterialHemiSphereLightEnable(s32 material_index, bool enable, bool oclusion_enable);
//bool IsMaterialHemiSphereLightEnable(s32 material_index, bool* oclusion_enable) const;

void Model::SetMaterialLightSetIndex(s32 material_index, s32 index)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetSceneEnvironmentResMaterial();  // @note ライトセットはこれでいいのか？→gfx_RenderContext.cpp RenderContext::ActivateSceneEnvironmentより、いいと判断できる。
    //nw::gfx::res::ResMaterial& res_material = material->GetFragmentLightingResMaterial();
    //nw::gfx::res::ResMaterial& res_material = material->GetFragmentLightingTableResMaterial();
    res_material.SetLightSetIndex(index);
    // @note SceneEnvironmentResMaterial用のSetMaterial*Hash関数がないが反映されているか？→このインデックスはハッシュに関係ない。
    //res_material.SetFragmentLightingHash(0x0);  // @note この3つのHashを呼んでやっても何か反映されていない気がする→このインデックスはハッシュに関係ない。
    //res_material.SetFragmentLightingTableHash(0x0);  // ResMaterialのGetからして間違っているのかも。
    //res_material.SetFragmentLightingTableParametersHash(0x0);  // そうだとしたら、下記のGet関数のResMaterialも中身も直すこと。
                                                               // @note ActiveLightSetという関数があるが、複数のライトセットを同時にアクティブにできるだろうか。今できているからできるんだろうが。→それはgfx_SceneEnvironment.hでマテリアルごとに描画する際にアクティブなライトセットを切り替えているということ。
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
s32  Model::GetMaterialLightSetIndex(s32 material_index) const
{
  s32 index = 0;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetSceneEnvironmentResMaterial();  // @note ライトセットはこれでいいのか？→gfx_RenderContext.cpp RenderContext::ActivateSceneEnvironmentより、いいと判断できる。
    index = res_material.GetLightSetIndex();
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return index;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         マテリアルのカリングモード
 */
//-----------------------------------------------------------------------------
void Model::SetMaterialCullingMode(s32 material_index, MaterialCullingMode mode)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetRasterizationResMaterial();
    nw::gfx::res::ResRasterization res_rasterization = res_material.GetRasterization();
    res_rasterization.SetCullingMode(static_cast<nw::gfx::res::ResRasterization::CullingMode>(mode));
    res_material.SetRasterizationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
Model::MaterialCullingMode Model::GetMaterialCullingMode(s32 material_index) const
{
  MaterialCullingMode mode = MATERIAL_CULLING_MODE_BACK;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetRasterizationResMaterial();
    nw::gfx::res::ResRasterization res_rasterization = res_material.GetRasterization();
    mode = static_cast<MaterialCullingMode>(res_rasterization.GetCullingMode());
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return mode;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         マテリアルのアルファテスト
 */
//-----------------------------------------------------------------------------
bool Model::IsMaterialAlphaTestEnable(s32 material_index) const
{
  bool enable = false;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetAlphaTestResMaterial();
    nw::gfx::res::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
    nw::gfx::res::ResAlphaTest res_alpha_test = res_fragment_shader.GetAlphaTest();
    
    enable = res_alpha_test.IsTestEnabled();
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return enable;
}
void Model::SetMaterialAlphaTestEnable(s32 material_index, bool enable)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetAlphaTestResMaterial();
    nw::gfx::res::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
    nw::gfx::res::ResAlphaTest res_alpha_test = res_fragment_shader.GetAlphaTest();
   
    res_alpha_test.SetTestEnabled(enable);
    res_material.SetAlphaTestHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
Model::MaterialTestFunction Model::GetMaterialAlphaTestFunction(s32 material_index) const
{
  MaterialTestFunction function = MATERIAL_TEST_FUNCTION_NEVER;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetAlphaTestResMaterial();
    nw::gfx::res::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
    nw::gfx::res::ResAlphaTest res_alpha_test = res_fragment_shader.GetAlphaTest();
    
    function = static_cast<MaterialTestFunction>(res_alpha_test.GetTestFunc());
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return function;
}
void Model::SetMaterialAlphaTestFunction(s32 material_index, MaterialTestFunction function)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetAlphaTestResMaterial();
    nw::gfx::res::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
    nw::gfx::res::ResAlphaTest res_alpha_test = res_fragment_shader.GetAlphaTest();
   
    res_alpha_test.SetTestFunc(static_cast<nw::gfx::res::ResAlphaTest::TestFunc>(function));
    res_material.SetAlphaTestHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
f32 Model::GetMaterialAlphaTestReference(s32 material_index) const
{
  f32 reference = 0.0f;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetAlphaTestResMaterial();
    nw::gfx::res::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
    nw::gfx::res::ResAlphaTest res_alpha_test = res_fragment_shader.GetAlphaTest();
    
    reference = res_alpha_test.GetTestReference();
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return reference;
}
void Model::SetMaterialAlphaTestReference(s32 material_index, f32 reference)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetAlphaTestResMaterial();
    nw::gfx::res::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
    nw::gfx::res::ResAlphaTest res_alpha_test = res_fragment_shader.GetAlphaTest();
   
    res_alpha_test.SetTestReference(reference);
    res_material.SetAlphaTestHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         マテリアルのデプステスト
 */
//-----------------------------------------------------------------------------
bool Model::IsMaterialDepthTestEnable(s32 material_index) const          // DepthOperation IsTestEnabled
{
  bool enable = false;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResDepthOperation res_depth_operation = res_fragment_operation.GetDepthOperation();
    u32 flags = res_depth_operation.GetFlags();
    u32 test_bit = nw::gfx::res::ResDepthOperationData::FLAG_TEST_ENABLED;
    flags &= test_bit;
    if(flags)
    {
      enable = true;
    }
    else
    {
      enable = false;
    }
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return enable;
}
void Model::SetMaterialDepthTestEnable(s32 material_index, bool enable)  // trueのときデプステスト有効
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResDepthOperation res_depth_operation = res_fragment_operation.GetDepthOperation();
    u32 flags = res_depth_operation.GetFlags();
    u32 test_bit = nw::gfx::res::ResDepthOperationData::FLAG_TEST_ENABLED;
    if(enable)
    {
      flags |= test_bit;
    }
    else
    {
      flags &= ~test_bit;
    }
    res_depth_operation.SetFlags(flags);
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
bool Model::IsMaterialDepthTestMaskEnable(s32 material_index) const          // DepthOperation IsMaskEnabled
{
  bool enable = false;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResDepthOperation res_depth_operation = res_fragment_operation.GetDepthOperation();
    u32 flags = res_depth_operation.GetFlags();
    u32 mask_bit = nw::gfx::res::ResDepthOperationData::FLAG_MASK_ENABLED;
    flags &= mask_bit;
    if(flags)
    {
      enable = true;
    }
    else
    {
      enable = false;
    }
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return enable;
}
void Model::SetMaterialDepthTestMaskEnable(s32 material_index, bool enable)  // trueのときデプスマスク有効(trueのときデプスバッファを更新する)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResDepthOperation res_depth_operation = res_fragment_operation.GetDepthOperation();
    u32 flags = res_depth_operation.GetFlags();
    u32 mask_bit = nw::gfx::res::ResDepthOperationData::FLAG_MASK_ENABLED;
    if(enable)
    {
      flags |= mask_bit;
    }
    else
    {
      flags &= ~mask_bit;
    }
    res_depth_operation.SetFlags(flags);
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
Model::MaterialTestFunction Model::GetMaterialDepthTestFunction(s32 material_index) const  // DepthOperation TestFunction
{
  MaterialTestFunction function = MATERIAL_TEST_FUNCTION_NEVER;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResDepthOperation res_depth_operation = res_fragment_operation.GetDepthOperation();
    function = static_cast<MaterialTestFunction>(res_depth_operation.GetTestFunc()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return function;
}
void Model::SetMaterialDepthTestFunction(s32 material_index, MaterialTestFunction function)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResDepthOperation res_depth_operation = res_fragment_operation.GetDepthOperation();
    res_depth_operation.SetTestFunc(static_cast<nw::gfx::res::ResDepthOperation::TestFunc>(function)); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         マテリアルのステンシルテスト
 */
//-----------------------------------------------------------------------------
bool Model::IsMaterialStencilTestEnable(s32 material_index) const
{
  bool enable = false;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    enable = res_stencil_operation.IsTestEnabled(); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return enable;
}
void Model::SetMaterialStencilTestEnable(s32 material_index, bool enable)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    res_stencil_operation.SetTestEnabled(enable); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
Model::MaterialTestFunction Model::GetMaterialStencilTestFunction(s32 material_index) const
{
  MaterialTestFunction function = MATERIAL_TEST_FUNCTION_NEVER;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    function = static_cast<MaterialTestFunction>(res_stencil_operation.GetTestFunction()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return function;
}
void Model::SetMaterialStencilTestFunction(s32 material_index, MaterialTestFunction function)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    res_stencil_operation.SetTestFunction(static_cast<nw::gfx::res::ResStencilOperation::TestFunc>(function)); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
s32 Model::GetMaterialStencilTestReference(s32 material_index) const
{
  s32 value = 0;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    value = res_stencil_operation.GetTestReference(); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return value;
}
void Model::SetMaterialStencilTestReference(s32 material_index, s32 value)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    res_stencil_operation.SetTestReference(value); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
u32 Model::GetMaterialStencilTestMask(s32 material_index) const
{
  u32 value = 0;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    value = res_stencil_operation.GetTestMask(); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return value;
}
void Model::SetMaterialStencilTestMask(s32 material_index, u32 value)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    res_stencil_operation.SetTestMask(value); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
Model::MaterialStencilOperation Model::GetMaterialStencilTestFailOperation(s32 material_index) const
{
  MaterialStencilOperation operation = MATERIAL_STENCIL_OPERATION_KEEP;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    operation = static_cast<MaterialStencilOperation>(res_stencil_operation.GetFailOperation()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return operation;
}
void Model::SetMaterialStencilTestFailOperation(s32 material_index, MaterialStencilOperation operation)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    res_stencil_operation.SetFailOperation(static_cast<nw::gfx::res::ResStencilOperation::StencilOp>(operation)); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
Model::MaterialStencilOperation Model::GetMaterialStencilTestZFailOperation(s32 material_index) const
{
  MaterialStencilOperation operation = MATERIAL_STENCIL_OPERATION_KEEP;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    operation = static_cast<MaterialStencilOperation>(res_stencil_operation.GetZFailOperation()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return operation;
}
void Model::SetMaterialStencilTestZFailOperation(s32 material_index, MaterialStencilOperation operation)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    res_stencil_operation.SetZFailOperation(static_cast<nw::gfx::res::ResStencilOperation::StencilOp>(operation)); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
Model::MaterialStencilOperation Model::GetMaterialStencilTestPassOperation(s32 material_index) const
{
  MaterialStencilOperation operation = MATERIAL_STENCIL_OPERATION_KEEP;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    operation = static_cast<MaterialStencilOperation>(res_stencil_operation.GetPassOperation()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return operation;
}
void Model::SetMaterialStencilTestPassOperation(s32 material_index, MaterialStencilOperation operation)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    res_stencil_operation.SetPassOperation(static_cast<nw::gfx::res::ResStencilOperation::StencilOp>(operation)); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         マテリアルのブレンダー
 */
//-----------------------------------------------------------------------------
#if 1
Model::MaterialBlendMode Model::GetMaterialBlendMode(s32 material_index) const
{
  MaterialBlendMode mode = MATERIAL_BLEND_MODE_NOT_USE;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    nw::gfx::res::ResBlendOperation::Mode nw_mode = res_blend_operation.GetMode();
    
    switch(nw_mode)
    {
    case nw::gfx::res::ResBlendOperation::MODE_SEPARATE_BLEND:
      {
        mode = MATERIAL_BLEND_MODE_SEPARATE_BLEND;
        if(
             GetMaterialBlendFunctionSourceRgb(material_index)        == MATERIAL_BLEND_FACTOR_ONE
          && GetMaterialBlendFunctionSourceAlpha(material_index)      == MATERIAL_BLEND_FACTOR_ONE
          && GetMaterialBlendFunctionDestinationRgb(material_index)   == MATERIAL_BLEND_FACTOR_ZERO
          && GetMaterialBlendFunctionDestinationAlpha(material_index) == MATERIAL_BLEND_FACTOR_ZERO
          && GetMaterialBlendEquationAlpha(material_index)            == MATERIAL_BLEND_EQUATION_FUNC_ADD
        )
        {
          mode = MATERIAL_BLEND_MODE_NOT_USE;
        }
      }
      break;
    case nw::gfx::res::ResBlendOperation::MODE_LOGIC:
      {
        mode = MATERIAL_BLEND_MODE_LOGIC;
      }
      break;
    case nw::gfx::res::ResBlendOperation::MODE_NOT_USE:
      {
          mode = MATERIAL_BLEND_MODE_NOT_USE;
      }
      break;
    case nw::gfx::res::ResBlendOperation::MODE_BLEND:
      {
          mode = MATERIAL_BLEND_MODE_BLEND;
      }
      break;
    default:
      {
        GFL_ASSERT_MSG(0, "ブレンドモードが異常です。%d\n", nw_mode);
      }
      break;
    }
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return mode;
}
void Model::SetMaterialBlendMode(s32 material_index, MaterialBlendMode mode)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    switch(mode) 
    {
    case MATERIAL_BLEND_MODE_NOT_USE:
      {
    // gfx_ResMaterial.h
    //
    //! MODE_NOT_USE 相当のブレンドパラメータを設定するためには、
    //! SetBlendMode(true) でブレンドを有効にし、
    //! SetBlendFuncSrcRgb() と SetBlendFuncSrcAlpha() に BLENDFACTOR_ONE を、
    //! SetBlendFuncDstRgb() と SetBlendFuncDstAlpha() に BLENDFACTOR_ZERO を、
    //! SetBlendEquationRgb() と SetBlendEquationAlpha() に BLENDEQ_FUNC_ADD を設定してください。
        res_blend_operation.SetBlendMode(true);
        SetMaterialBlendFunctionSourceRgb(material_index, MATERIAL_BLEND_FACTOR_ONE);
        SetMaterialBlendFunctionSourceAlpha(material_index, MATERIAL_BLEND_FACTOR_ONE);
        SetMaterialBlendFunctionDestinationRgb(material_index, MATERIAL_BLEND_FACTOR_ZERO);
        SetMaterialBlendFunctionDestinationAlpha(material_index, MATERIAL_BLEND_FACTOR_ZERO);
        SetMaterialBlendEquationAlpha(material_index, MATERIAL_BLEND_EQUATION_FUNC_ADD);
      }
      break;
    case MATERIAL_BLEND_MODE_BLEND:
    case MATERIAL_BLEND_MODE_SEPARATE_BLEND:
      {
        res_blend_operation.SetBlendMode(true);
      }
      break;
    case MATERIAL_BLEND_MODE_LOGIC:
      {
        res_blend_operation.SetBlendMode(false);
      }
      break;
    }
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
#else
Model::MaterialBlendMode Model::GetMaterialBlendMode(s32 material_index) const
{
  MaterialBlendMode mode = MATERIAL_BLEND_MODE_NOT_USE;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    mode = static_cast<MaterialBlendMode>(res_blend_operation.GetMode()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return mode;
}
void Model::SetMaterialBlendMode(s32 material_index, MaterialBlendMode mode)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetMode(static_cast<nw::gfx::res::ResBlendOperation::Mode>(mode)); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
#endif

void Model::SetMaterialBlendColor(s32 material_index, f32 r, f32 g, f32 b)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetBlendColor(r,g,b); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
void Model::SetMaterialBlendColor(s32 material_index, f32 r, f32 g, f32 b, f32 a)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetBlendColor(r,g,b,a); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
void Model::SetMaterialBlendColor(s32 material_index, const gfl::grp::ColorF32& c)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetBlendColor(c); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
void Model::SetMaterialBlendColor(s32 material_index, u8 r, u8 g, u8 b)
{
  SetMaterialBlendColor(
      material_index,
      (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX
  );
}
void Model::SetMaterialBlendColor(s32 material_index, u8 r, u8 g, u8 b, u8 a)
{
  SetMaterialBlendColor(
      material_index,
      (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(a))/gfl::grp::ColorU8::ELEMENT_MAX
  );
}
void Model::SetMaterialBlendColor(s32 material_index, const gfl::grp::ColorU8& c)
{
  SetMaterialBlendColor(
      material_index,
      (static_cast<f32>(c.r))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(c.g))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(c.b))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(c.a))/gfl::grp::ColorU8::ELEMENT_MAX
  );
}
gfl::grp::ColorF32 Model::GetMaterialBlendColor(s32 material_index) const
{
  gfl::grp::ColorF32 c;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    c = res_blend_operation.GetBlendColor(); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return c;
}
void Model::GetMaterialBlendColor(s32 material_index, gfl::grp::ColorF32* c) const
{
  *c = GetMaterialBlendColor(material_index);
}
void Model::GetMaterialBlendColor(s32 material_index, gfl::grp::ColorU8*  c) const
{
  gfl::grp::ColorF32 c_f32 = GetMaterialBlendColor(material_index);
  c->Set(
    static_cast<s32>(c_f32.r*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.g*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.b*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.a*gfl::grp::ColorU8::ELEMENT_MAX)
  );
}

Model::MaterialBlendLogicOperation Model::GetMaterialBlendLogicOperation(s32 material_index) const
{
  MaterialBlendLogicOperation operation = MATERIAL_BLEND_LOGIC_OPERATION_CLEAR;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    operation = static_cast<MaterialBlendLogicOperation>(res_blend_operation.GetLogicOperation()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return operation;
}
void Model::SetMaterialBlendLogicOperation(s32 material_index, MaterialBlendLogicOperation operation)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetLogicOperation(static_cast<nw::gfx::res::ResBlendOperation::LogicOp>(operation)); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}

Model::MaterialBlendFactor Model::GetMaterialBlendFunctionSourceRgb(s32 material_index) const
{
  MaterialBlendFactor factor = MATERIAL_BLEND_FACTOR_ZERO;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    factor = static_cast<MaterialBlendFactor>(res_blend_operation.GetBlendFuncSrcRgb()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return factor;
}
void Model::SetMaterialBlendFunctionSourceRgb(s32 material_index, MaterialBlendFactor factor)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetBlendFuncSrcRgb(static_cast<nw::gfx::res::ResBlendOperation::BlendFactor>(factor)); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
Model::MaterialBlendFactor Model::GetMaterialBlendFunctionSourceAlpha(s32 material_index) const
{
  MaterialBlendFactor factor = MATERIAL_BLEND_FACTOR_ZERO;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    factor = static_cast<MaterialBlendFactor>(res_blend_operation.GetBlendFuncSrcAlpha()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return factor;
}
void Model::SetMaterialBlendFunctionSourceAlpha(s32 material_index, MaterialBlendFactor factor)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetBlendFuncSrcAlpha(static_cast<nw::gfx::res::ResBlendOperation::BlendFactor>(factor)); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}

Model::MaterialBlendFactor Model::GetMaterialBlendFunctionDestinationRgb(s32 material_index) const
{
  MaterialBlendFactor factor = MATERIAL_BLEND_FACTOR_ZERO;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    factor = static_cast<MaterialBlendFactor>(res_blend_operation.GetBlendFuncDstRgb()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return factor;
}
void Model::SetMaterialBlendFunctionDestinationRgb(s32 material_index, MaterialBlendFactor factor)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetBlendFuncDstRgb(static_cast<nw::gfx::res::ResBlendOperation::BlendFactor>(factor)); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
Model::MaterialBlendFactor Model::GetMaterialBlendFunctionDestinationAlpha(s32 material_index) const
{
  MaterialBlendFactor factor = MATERIAL_BLEND_FACTOR_ZERO;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    factor = static_cast<MaterialBlendFactor>(res_blend_operation.GetBlendFuncDstAlpha()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return factor;
}
void Model::SetMaterialBlendFunctionDestinationAlpha(s32 material_index, MaterialBlendFactor factor)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetBlendFuncDstAlpha(static_cast<nw::gfx::res::ResBlendOperation::BlendFactor>(factor)); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}

Model::MaterialBlendEquation Model::GetMaterialBlendEquationRgb(s32 material_index) const
{
  MaterialBlendEquation equation = MATERIAL_BLEND_EQUATION_FUNC_ADD;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    equation = static_cast<MaterialBlendEquation>(res_blend_operation.GetBlendEquationRgb()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return equation;
}
void Model::SetMaterialBlendEquationRgb(s32 material_index, MaterialBlendEquation equation)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetBlendEquationRgb(static_cast<nw::gfx::res::ResBlendOperation::BlendEquation>(equation)); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}
Model::MaterialBlendEquation Model::GetMaterialBlendEquationAlpha(s32 material_index) const
{
  MaterialBlendEquation equation = MATERIAL_BLEND_EQUATION_FUNC_ADD;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    equation = static_cast<MaterialBlendEquation>(res_blend_operation.GetBlendEquationAlpha()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return equation;
}
void Model::SetMaterialBlendEquationAlpha(s32 material_index, MaterialBlendEquation equation)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetBlendEquationAlpha(static_cast<nw::gfx::res::ResBlendOperation::BlendEquation>(equation)); 
    res_material.SetFragmentOperationHash(0x0);  // これがないと反映されない
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         描画順
 *                 透明性の種類(描画レイヤー)
 *                 デザイナがエディタ上で設定できるマテリアルの描画レイヤー
 *                 最初に描画 MATERIAL_TRANSLUCENCY_KIND_LAYER0 <= MaterialTranslucencyKind <= MATERIAL_TRANSLUCENCY_KIND_LAYER3 最後に描画
 *                 最優先される描画順 ModelLayerId > MaterialTranslucencyKind > その他 最劣後される描画順
 */
//-----------------------------------------------------------------------------
Model::MaterialTranslucencyKind Model::GetMaterialTranslucencyKind(s32 material_index) const
{
  MaterialTranslucencyKind kind = MATERIAL_TRANSLUCENCY_KIND_OPAQUE;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    //nw::gfx::res::ResMaterial& res_material = material->GetShaderParameterResMaterial();
    nw::gfx::res::ResMaterial  res_material = material->GetShadingParameterResMaterial();
    kind = static_cast<MaterialTranslucencyKind>(res_material.GetTranslucencyKind()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
  return kind;
}
void Model::SetMaterialTranslucencyKind(s32 material_index, MaterialTranslucencyKind kind)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);

    /*
    { 
      nw::gfx::res::ResMaterial res_material = material->GetOriginal();
      if( res_material.IsValid() )
      {
        res_material.SetTranslucencyKind(static_cast<nw::gfx::res::ResMaterial::TranslucencyKind>(kind));
      }
    }
    */
    /*
    {
      nw::gfx::res::ResMaterial res_material = material->GetBuffer();
      if( res_material.IsValid() )
      {
        res_material.SetTranslucencyKind(static_cast<nw::gfx::res::ResMaterial::TranslucencyKind>(kind));
      }
    }
    */
    /*
    {
      nw::gfx::res::ResMaterial& res_material = material->GetShaderParameterResMaterial();
      if( res_material.IsValid() )
      {
        res_material.SetTranslucencyKind(static_cast<nw::gfx::res::ResMaterial::TranslucencyKind>(kind)); 
        res_material.SetShaderParametersHash(0x0);
      }
    }
    */
    { 
      nw::gfx::res::ResMaterial  res_material = material->GetShadingParameterResMaterial();
      if( res_material.IsValid() )
      {
        res_material.SetTranslucencyKind(static_cast<nw::gfx::res::ResMaterial::TranslucencyKind>(kind)); 
        res_material.SetShadingParameterHash(0x0);
      }
    }
  
    model->InvalidateRenderKeyCache();  // これがないと描画順が変わらない

    /*
    res_material.SetShadingParameterHash               (0x0);
    res_material.SetShaderParametersHash               (0x0);
    res_material.SetTextureCoordinatorsHash            (0x0);
    res_material.SetTextureSamplersHash                (0x0);
    res_material.SetTextureMappersHash                 (0x0);
    res_material.SetMaterialColorHash                  (0x0);
    res_material.SetRasterizationHash                  (0x0);
    res_material.SetFragmentLightingHash               (0x0);
    res_material.SetFragmentLightingTableHash          (0x0); 
    res_material.SetFragmentLightingTableParametersHash(0x0);
    res_material.SetAlphaTestHash                      (0x0);
    res_material.SetTextureCombinersHash               (0x0);
    res_material.SetFragmentOperationHash              (0x0);
    */

/*
[質問][NW4C] nw::gfx::res::ResMaterial::SetTranslucencyKind() について

開発環境 : CTR_SDK 1.0.0、NntendoWare 1.2.33

teabreak 2011-02-11 11:58:19  

御世話になっております。

nw::gfx::res::ResMaterial::SetTranslucencyKind() で描画レイヤーを変更しているのですが、
正しく変更されないケースがあったので原因を調べています。
GetTranslucencyKind() を呼び出している側で
nw::gfx::Material::GetShaderParameterResMaterial()
nw::gfx::Material::GetBuffer()
と参照するマテリアルのリソースがまちまちになっており、
nw::gfx::Model::FLAG_SHADER_PARAMETER が有効になっていない場合は
Original と Buffer のどちらを参照するかわからないことが問題のようです。
Original と Buffer の両方を変更することで正しく変更されたのですが、
このような対応が必要なのでしょうか？
*/

  }
  else
  {
    GFL_ASSERT_MSG( 0, "マテリアルのインデックス番号がマテリアルの数を越えています。\n" );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         マテリアルのテクスチャ
 */
//-----------------------------------------------------------------------------
// material_index番マテリアルのno番テクスチャ(0<=no<=2)にtexture_resource_packで指すテクスチャを設定する。
// 元々貼ってあったテクスチャはそのリソースを解放したときに解放されるので、差し替えられてもSetupされたままの状態で残っています。
void Model::SetMaterialTextureMapperTexture(s32 material_index, s32 no, Resource::TextureResourcePack* texture_resource_pack)
{
  SetMaterialTextureMapperTexture(material_index, no, texture_resource_pack->resource, texture_resource_pack->index_in_resource);
}

// material_index番マテリアルのno番テクスチャ(0<=no<=2)にtexture_resourceで指すテクスチャを設定する。
// 元々貼ってあったテクスチャはそのリソースを解放したときに解放されるので、差し替えられてもSetupされたままの状態で残っています。
void Model::SetMaterialTextureMapperTexture(s32 material_index, s32 no, Resource* texture_resource, s32 index_in_resource)
{
  nw::gfx::ResGraphicsFile* nw_res_graphics_file = texture_resource->GetNwResGraphicsFile();
  nw::gfx::ResTexture       res_texture          = nw_res_graphics_file->GetTextures(index_in_resource);
  setMaterialTextureMapperTexture(material_index, no, res_texture);
}

// material_index番マテリアルのno番テクスチャ(0<=no<=2)にtextureで指すテクスチャを設定する。
// 元々貼ってあったテクスチャはそのリソースを解放したときに解放されるので、差し替えられてもSetupされたままの状態で残っています。
void Model::SetMaterialTextureMapperTexture(s32 material_index, s32 no, Texture* texture)
{
  nw::gfx::ResTexture res_texture = texture->GetNwResTexture();
  setMaterialTextureMapperTexture(material_index, no, res_texture);
}

void Model::setMaterialTextureMapperTexture(s32 material_index, s32 no, nw::gfx::ResTexture res_texture)
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureMapperResMaterial();
  nw::gfx::ResPixelBasedTextureMapper res_texture_mapper = res_material.GetTextureMappers(no);

  // 元々貼ってあったテクスチャはそのままにしておく
  //nw::gfx::ResTexture dummy_texture                      = res_texture_mapper.GetTexture().Dereference();
  //dummy_texture.Cleanup();

#if 0
/*[質問][NW4C] nw::ut::SafeCleanup() の呼び出し後の IsValid() の値について

田中＠NTSC 2011-02-25 09:31:42  
はい。仕様です。
IsValidはあくまでもリソースデータがnullかどうかのチェックです。
対してCleanupはSetupの逆に相当する処理ですので、
IsValidがSetup済みがどうかのチェックには使用することはできません。*/
#endif

  // テクスチャを差し替える
  if( res_texture_mapper.IsValid() )
  {
    // コマンド蓄積
    res_texture_mapper.SetTexture(res_texture);
        // NintendoWare for CTR 関数リファレンスマニュアル
        // テクスチャマッパーにテクスチャを設定します。 テクスチャを設定した後にコマンドの生成を行います。

    // テクスチャを変更した場合はテクスチャマッパーのハッシュを０に設定します。
    res_material.SetTextureMappersHash(0x0);
  }
  else
  {
    //GFL_ASSERT_MSG( 0, "nw::gfx::ResPixelBasedTextureMapperのリソースへのポインタがNULLです。\n" );  // @note 一時的にコメントアウト。コメントアウトはやめて、このASSERTが効くように戻しておいたほうがいい。
  }
}


// material_index番マテリアルのno番テクスチャ(0<=no<=2)のU方向繰り返し設定を行う。
void Model::SetMaterialTextureMapperWrapU(s32 material_index, s32 no, MaterialTextureWrap wrap)
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureMapperResMaterial();
  nw::gfx::ResPixelBasedTextureMapper res_texture_mapper = res_material.GetTextureMappers(no);

  if( res_texture_mapper.IsValid() )
  {
    // テクスチャサンプラー
    nw::gfx::ResTextureSampler res_texure_sampler = res_texture_mapper.GetSampler();
    nw::gfx::ResStandardTextureSampler res_standard_texure_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_texure_sampler);
  
    res_standard_texure_sampler.SetWrapS( static_cast<nw::gfx::res::ResStandardTextureSampler::Wrap>(wrap) );  // コマンド蓄積か?
    // ハッシュ
    res_material.SetTextureSamplersHash(0x0);
  }
  else
  {
    //GFL_ASSERT_MSG( 0, "nw::gfx::ResPixelBasedTextureMapperのリソースへのポインタがNULLです。\n" );  // @note 一時的にコメントアウト。コメントアウトはやめて、このASSERTが効くように戻しておいたほうがいい。
  }
}
// material_index番マテリアルのno番テクスチャ(0<=no<=2)のU方向繰り返し設定を得る。
Model::MaterialTextureWrap  Model::GetMaterialTextureMapperWrapU(s32 material_index, s32 no) const
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureMapperResMaterial();
  nw::gfx::ResPixelBasedTextureMapper res_texture_mapper = res_material.GetTextureMappers(no);

  if( res_texture_mapper.IsValid() )
  {
    // テクスチャサンプラー
    nw::gfx::ResTextureSampler res_texure_sampler = res_texture_mapper.GetSampler();
    nw::gfx::ResStandardTextureSampler res_standard_texure_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_texure_sampler);
  
    return  static_cast<MaterialTextureWrap>( res_standard_texure_sampler.GetWrapS() );
  }
  else
  {
    //GFL_ASSERT_MSG( 0, "nw::gfx::ResPixelBasedTextureMapperのリソースへのポインタがNULLです。\n" );  // @note 一時的にコメントアウト。コメントアウトはやめて、このASSERTが効くように戻しておいたほうがいい。
    return MATERIAL_TEXTURE_WRAP_CLAMP_TO_EDGE;
  }
}
// material_index番マテリアルのno番テクスチャ(0<=no<=2)のV方向繰り返し設定を行う。
void Model::SetMaterialTextureMapperWrapV(s32 material_index, s32 no, MaterialTextureWrap wrap)
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureMapperResMaterial();
  nw::gfx::ResPixelBasedTextureMapper res_texture_mapper = res_material.GetTextureMappers(no);

  if( res_texture_mapper.IsValid() )
  {
    // テクスチャサンプラー
    nw::gfx::ResTextureSampler res_texure_sampler = res_texture_mapper.GetSampler();
    nw::gfx::ResStandardTextureSampler res_standard_texure_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_texure_sampler);
  
    res_standard_texure_sampler.SetWrapT( static_cast<nw::gfx::res::ResStandardTextureSampler::Wrap>(wrap) );  // コマンド蓄積か?
    // ハッシュ
    res_material.SetTextureSamplersHash(0x0);
  }
  else
  {
    //GFL_ASSERT_MSG( 0, "nw::gfx::ResPixelBasedTextureMapperのリソースへのポインタがNULLです。\n" );  // @note 一時的にコメントアウト。コメントアウトはやめて、このASSERTが効くように戻しておいたほうがいい。
  }
}
// material_index番マテリアルのno番テクスチャ(0<=no<=2)のV方向繰り返し設定を得る。
Model::MaterialTextureWrap  Model::GetMaterialTextureMapperWrapV(s32 material_index, s32 no) const
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureMapperResMaterial();
  nw::gfx::ResPixelBasedTextureMapper res_texture_mapper = res_material.GetTextureMappers(no);

  if( res_texture_mapper.IsValid() )
  {
    // テクスチャサンプラー
    nw::gfx::ResTextureSampler res_texure_sampler = res_texture_mapper.GetSampler();
    nw::gfx::ResStandardTextureSampler res_standard_texure_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_texure_sampler);
  
    return  static_cast<MaterialTextureWrap>( res_standard_texure_sampler.GetWrapT() );
  }
  else
  {
    //GFL_ASSERT_MSG( 0, "nw::gfx::ResPixelBasedTextureMapperのリソースへのポインタがNULLです。\n" );  // @note 一時的にコメントアウト。コメントアウトはやめて、このASSERTが効くように戻しておいたほうがいい。
    return MATERIAL_TEXTURE_WRAP_CLAMP_TO_EDGE;
  }
}
// テクスチャの座標は、STは使わず、UVで統一しておく。


// material_index番マテリアルのno番テクスチャコーディネータ(0<=no<=2)の設定
// texture_mapper_no番テクスチャマッパーとtexture_coordinator_no番テクスチャコーディネータがあった場合、
// 「2番テクスチャマッパーなら2番テクスチャコーディネータを使う」というふうに必ずしもなっているわけではない。
// 「1番テクスチャマッパーと2番テクスチャマッパーが1番テクスチャコーディネータを共有して使う」ということもあり得る。
// つまりtexture_mapper_no=texture_coordinator_noと必ずなるわけではない、ということに注意して下さい。
void Model::SetMaterialTextureCoordinatorScaleUV(s32 material_index, s32 no, const gfl::math::VEC2& t)
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureCoordinatorResMaterial();
  nw::gfx::ResTextureCoordinator res_texture_coordinator = res_material.GetTextureCoordinators(no);  // これはIsValid確認がなくても大丈夫だったが、たまたまアクセスした番号のものが用意されていただけかもしれない。

  // コマンド蓄積
  res_texture_coordinator.SetScale(t);
  // ハッシュ
  res_material.SetTextureCoordinatorsHash(0x0);
}
void Model::GetMaterialTextureCoordinatorScaleUV(s32 material_index, s32 no, gfl::math::VEC2* t) const
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureCoordinatorResMaterial();
  nw::gfx::ResTextureCoordinator res_texture_coordinator = res_material.GetTextureCoordinators(no);

  *t = res_texture_coordinator.GetScale();
}

void Model::SetMaterialTextureCoordinatorRotateUV(s32 material_index, s32 no, const f32 t)  // [radian]
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureCoordinatorResMaterial();
  nw::gfx::ResTextureCoordinator res_texture_coordinator = res_material.GetTextureCoordinators(no);  // これはIsValid確認がなくても大丈夫だったが、たまたまアクセスした番号のものが用意されていただけかもしれない。

  // コマンド蓄積
  res_texture_coordinator.SetRotate(t);
  // ハッシュ
  res_material.SetTextureCoordinatorsHash(0x0);
}
f32 Model::GetMaterialTextureCoordinatorRotateUV(s32 material_index, s32 no) const  // [radian]
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureCoordinatorResMaterial();
  nw::gfx::ResTextureCoordinator res_texture_coordinator = res_material.GetTextureCoordinators(no); 

  return res_texture_coordinator.GetRotate();
}

void Model::SetMaterialTextureCoordinatorTranslateUV(s32 material_index, s32 no, const gfl::math::VEC2& t)
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureCoordinatorResMaterial();
  nw::gfx::ResTextureCoordinator res_texture_coordinator = res_material.GetTextureCoordinators(no);  // これはIsValid確認がなくても大丈夫だったが、たまたまアクセスした番号のものが用意されていただけかもしれない。

  // コマンド蓄積
  res_texture_coordinator.SetTranslate(t);
  // ハッシュ
  res_material.SetTextureCoordinatorsHash(0x0);
}
void Model::GetMaterialTextureCoordinatorTranslateUV(s32 material_index, s32 no, gfl::math::VEC2* t) const
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureCoordinatorResMaterial();
  nw::gfx::ResTextureCoordinator res_texture_coordinator = res_material.GetTextureCoordinators(no); 

  *t = res_texture_coordinator.GetTranslate();
}

void Model::SetMaterialTextureCoordinatorReferenceSceneCameraIndex(s32 material_index, s32 no, s32 scene_camera_index)
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureCoordinatorResMaterial();
  nw::gfx::ResTextureCoordinator res_texture_coordinator = res_material.GetTextureCoordinators(no); 

  res_texture_coordinator.SetReferenceCamera(scene_camera_index);  // コマンド蓄積か?
  // ハッシュ
  res_material.SetTextureCoordinatorsHash(0x0);
}
s32  Model::GetMaterialTextureCoordinatorReferenceSceneCameraIndex(s32 material_index, s32 no) const
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureCoordinatorResMaterial();
  nw::gfx::ResTextureCoordinator res_texture_coordinator = res_material.GetTextureCoordinators(no); 

  return res_texture_coordinator.GetReferenceCamera();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         メッシュ
 */
//-----------------------------------------------------------------------------
s32 Model::GetMeshCount() const
{
/*
#if GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==0
*/
  // この方法で正しく取得できる
  nw::gfx::res::ResMeshArray res_mesh_array = GetNwModel()->GetResMeshes();
  return res_mesh_array.size();

/*
#elif GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==1
  // この方法で正しく取得できる
  nw::gfx::res::ResModel res_model = GetNwModel()->GetResModel();
  return res_model.GetMeshesCount();

#endif
*/
}
s32 Model::GetMeshIndex(const char* key) const
{
/*
#if GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==0
*/
/*
  // メッシュに名前を付けたデータがないので、この方法で正しく取得できるかどうか不明
  nw::gfx::res::ResMeshArray res_mesh_array = GetNwModel()->GetResMeshes();
  s32 count = res_mesh_array.size();
  for( s32 i=0; i<count; ++i )
  {
    nw::gfx::res::ResMesh res_mesh = res_mesh_array[i];
    if( gfl::std::StringCmp( key, res_mesh.GetName() ) ==  0 ) return i;
  }
  return MESH_INDEX_NOT_FOUND;
*/

/*
#elif GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==1
  // メッシュに名前を付けたデータがないので、この方法で正しく取得できるかどうか不明
  nw::gfx::res::ResModel res_model = GetNwModel()->GetResModel();
  s32 count = res_model.GetMeshesCount();
  for( s32 i=0; i<count; ++i )
  {
    nw::gfx::res::ResMesh res_mesh = res_model.GetMeshes(i);
    if( gfl::std::StringCmp( key, res_mesh.GetName() ) ==  0 ) return i;
  }
  return MESH_INDEX_NOT_FOUND;

#endif
*/


#if 0
/*[質問][NW4C] メッシュ名を取得するには？
開発環境 : NintendoWare for CTR 2.2.0 
segR 2012-01-18 21:15:15 

田中＠NTSC 2012-01-19 15:39:16  
申し訳ありません。
ResMeshに関しては直接名前が設定されていないようです。

下記のようにして間接的に取得する方法でご対応いただけますでしょうか？
        {
            nw::gfx::Model *pModel = (nw::gfx::Model *)node;
            nw::gfx::ResMeshArray oArray = pModel->GetResModel().GetMeshes();
            for(u32 i = 0; i < oArray.size(); i++){
                s32 nVisIdx = oArray[i].GetMeshNodeVisibilityIndex();
                nw::gfx::ResMeshNodeVisibility oMeshNode = pModel->GetResModel().GetMeshNodeVisibilities(nVisIdx);
                NN_LOG("mesh3 name:%s\n", oMeshNode.GetName());
            }
        }*/
#endif

  {
      nw::gfx::Model *pModel = GetNwModel();
      nw::gfx::ResMeshArray oArray = pModel->GetResModel().GetMeshes();
      for(u32 i = 0; i < oArray.size(); i++){
          s32 nVisIdx = oArray[i].GetMeshNodeVisibilityIndex();
          if(nVisIdx>=0)  // pModel->GetResModel().GetMeshNodeVisibilities(nVisIdx)でAssertion failure at gfx_ResModel.h:155
          {               // NW:Failed assertion 0<= (idx) && static_cast<s32>(idx) < static_cast<s32>(GetMeshNodeVisibilitiesCount())というエラーになったので、値をチェックすることにした。  GAME FREAK inc.  Koji Kawada
            nw::gfx::ResMeshNodeVisibility oMeshNode = pModel->GetResModel().GetMeshNodeVisibilities(nVisIdx);
            if( gfl::std::StringCmp( key, oMeshNode.GetName() ) ==  0 ) return i;
          }
      }
      return MESH_INDEX_NOT_FOUND;
  }
}

b32 Model::IsMeshVisible(const s32 mesh_index) const
{
  nw::gfx::res::ResMeshArray res_mesh_array = GetNwModel()->GetResMeshes();
  s32 count = res_mesh_array.size();
  if( mesh_index < count )
  {
    nw::gfx::res::ResMesh res_mesh = res_mesh_array[mesh_index];
    return res_mesh.IsVisible();
  }
  else
  {
    GFL_ASSERT_MSG(0, "mesh_index=%d, count=%d\n", mesh_index, count);
    return false;
  }
}
void Model::SetMeshVisible(const s32 mesh_index, const b32 visible)
{
  nw::gfx::res::ResMeshArray res_mesh_array = GetNwModel()->GetResMeshes();
  s32 count = res_mesh_array.size();
  if( mesh_index < count )
  {
    nw::gfx::res::ResMesh res_mesh = res_mesh_array[mesh_index];
    res_mesh.SetVisible(visible);
  }
  else
  {
    GFL_ASSERT_MSG(0, "mesh_index=%d, count=%d\n", mesh_index, count);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ユーザデータ
 */
//-----------------------------------------------------------------------------
// モデル
s32 Model::GetModelUserDataCount() const
{
  nw::gfx::res::ResModel res_model = GetNwModel()->GetResModel();
  return res_model.GetUserDataCount();
}
s32 Model::GetModelUserDataIndex(const char* key) const
{
#if 0
/*  NintendoWare/CTR/include/nw/ut/
  ut_ResDictionary.h

    // キー文字列の値で辞書を検索し、インデクスを取得します。
    // 指定のキーが見つからなかった場合には、負の数を返します。
    s32     GetIndex(const char* key) const;
    s32     GetIndex(const ResName key) const
    {
        if ( (! this->IsValid()) || (! key.IsValid()) ) { return -1; }
        return this->GetIndex(key.GetName());
    }*/
#endif

  nw::gfx::res::ResModel res_model = GetNwModel()->GetResModel();
  s32 index = res_model.GetUserDataIndex(key);
  if( index >= 0 ) return index;                      // 見付けた
  else             return USER_DATA_INDEX_NOT_FOUND;  // 見付からなかった
}
void Model::GetModelUserData(const s32 user_data_index, UserData* user_data) const
{
  GFL_ASSERT_MSG(user_data, "取得したユーザデータを格納するuser_dataがNULLです。\n");  //@fix
  if(!user_data) return;
  nw::gfx::res::ResModel res_model = GetNwModel()->GetResModel();
  nw::ut::ResMetaData res_meta_data = res_model.GetUserData(user_data_index);
  user_data->SetNwResMetaData(res_meta_data);
}

// マテリアル
s32 Model::GetMaterialUserDataCount(const s32 material_index) const
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Material*        material     = GetNwModel()->GetMaterial(material_index);
    nw::gfx::res::ResMaterial res_material = material->GetOriginal();
    return res_material.GetUserDataCount(); 
  }
  else
  {
    GFL_ASSERT_MSG(0, "マテリアルのインデックス%d>=%dが不正です。\n", material_index, count);
    return 0;
  }
}
s32 Model::GetMaterialUserDataIndex(const s32 material_index, const char* key) const
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Material*        material     = GetNwModel()->GetMaterial(material_index);
    nw::gfx::res::ResMaterial res_material = material->GetOriginal();
    return res_material.GetUserDataIndex(key);  // @note 不正のときUSER_DATA_INDEX_NOT_FOUNDを返したい。→不正のときは-1なのでUSER_DATA_INDEX_NOT_FOUNDと同じ値。
  }
  else
  {
    GFL_ASSERT_MSG(0, "マテリアルのインデックス%d>=%dが不正です。\n", material_index, count);
    return USER_DATA_INDEX_NOT_FOUND; 
  }
}
void Model::GetMaterialUserData(const s32 material_index, const s32 user_data_index, UserData* user_data) const
{
  GFL_ASSERT_MSG(user_data, "取得したユーザデータを格納するuser_dataがNULLです。\n");  //@fix
  if(!user_data) return;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Material*        material     = GetNwModel()->GetMaterial(material_index);
    nw::gfx::res::ResMaterial res_material = material->GetOriginal();
    nw::ut::ResMetaData res_meta_data = res_material.GetUserData(user_data_index);
    user_data->SetNwResMetaData(res_meta_data);
  }
  else
  {
    GFL_ASSERT_MSG(0, "マテリアルのインデックス%d>=%dが不正です。\n", material_index, count);
  }
}

// メッシュ
s32 Model::GetMeshUserDataCount(const s32 mesh_index) const
{
/*
#if GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==0
*/
  // この方法で正しく取得できる
  s32 count = GetMeshCount();
  if( mesh_index < count )
  {
    nw::gfx::res::ResMeshArray res_mesh_array = GetNwModel()->GetResMeshes();
    nw::gfx::res::ResMesh      res_mesh       = res_mesh_array[mesh_index];
    return res_mesh.GetUserDataCount();
  }
  else
  {
    GFL_ASSERT_MSG(0, "メッシュのインデックス%d>=%dが不正です。\n", mesh_index, count);
    return 0; 
  }

/*
#elif GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==1
  // この方法で正しく取得できる
  s32 count = GetMeshCount();
  if( mesh_index < count )
  {
    nw::gfx::res::ResModel res_model = GetNwModel()->GetResModel();
    nw::gfx::res::ResMesh  res_mesh  = res_model.GetMeshes(mesh_index);
    return res_mesh.GetUserDataCount();
  }
  else
  {
    GFL_ASSERT_MSG(0, "メッシュのインデックス%d>=%dが不正です。\n", mesh_index, count);
    return 0; 
  }

#endif
*/
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Model::GetModelAABB( gfl::math::AABB *p_dest_aabb ) const 
{
  if ( this->GetModelUserDataCount() == 0 )
    return false;
  
  s32   user_data_index = this->GetModelUserDataIndex("BBoxMinMax");

  if ( user_data_index < 0 )
    return false;
  
  UserData    user_data;
  this->GetModelUserData( user_data_index, &user_data );

  p_dest_aabb->SetMinMax(gfl::math::VEC4( user_data.GetFloatValue(0), user_data.GetFloatValue(1), user_data.GetFloatValue(2), 0.0f), 
                         gfl::math::VEC4( user_data.GetFloatValue(3), user_data.GetFloatValue(4), user_data.GetFloatValue(5), 0.0f) );

  return true;
}

s32 Model::GetMeshUserDataIndex(const s32 mesh_index, const char* key) const
{
#if GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==0
  // この方法では正しく取得できない
  s32 count = GetMeshCount();
  if( mesh_index < count )
  {
    nw::gfx::res::ResMeshArray res_mesh_array = GetNwModel()->GetResMeshes();
    nw::gfx::res::ResMesh      res_mesh       = res_mesh_array[mesh_index];
    return res_mesh.GetUserDataIndex(key);  // @note 不正のときUSER_DATA_INDEX_NOT_FOUNDを返したい。→不正のときは-1なのでUSER_DATA_INDEX_NOT_FOUNDと同じ値。
  }
  else
  {
    GFL_ASSERT_MSG(0, "メッシュのインデックス%d>=%dが不正です。\n", mesh_index, count);
    return USER_DATA_INDEX_NOT_FOUND; 
  }

#elif GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==1
  // この方法で正しく取得できる
  s32 count = GetMeshCount();
  if( mesh_index < count )
  {
    nw::gfx::res::ResModel res_model = GetNwModel()->GetResModel();
    nw::gfx::res::ResMesh  res_mesh  = res_model.GetMeshes(mesh_index);
    return res_mesh.GetUserDataIndex(key);  // @note 不正のときUSER_DATA_INDEX_NOT_FOUNDを返したい。→不正のときは-1なのでUSER_DATA_INDEX_NOT_FOUNDと同じ値。
  }
  else
  {
    GFL_ASSERT_MSG(0, "メッシュのインデックス%d>=%dが不正です。\n", mesh_index, count);
    return USER_DATA_INDEX_NOT_FOUND; 
  }

#endif
}
void Model::GetMeshUserData(const s32 mesh_index, const s32 user_data_index, UserData* user_data) const
{
#if GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==0
  // この方法では正しく取得できない
  GFL_ASSERT_MSG(user_data, "取得したユーザデータを格納するuser_dataがNULLです。\n");  //@fix
  if(!user_data) return;
  s32 count = GetMeshCount();
  if( mesh_index < count )
  {
    nw::gfx::res::ResMeshArray res_mesh_array = GetNwModel()->GetResMeshes();
    nw::gfx::res::ResMesh      res_mesh       = res_mesh_array[mesh_index];
    nw::ut::ResMetaData res_meta_data = res_mesh.GetUserData(user_data_index);
    user_data->SetNwResMetaData(res_meta_data);
  }
  else
  {
    GFL_ASSERT_MSG(0, "メッシュのインデックス%d>=%dが不正です。\n", mesh_index, count);
  }

#elif GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==1
  // この方法で正しく取得できる
  GFL_ASSERT_MSG(user_data, "取得したユーザデータを格納するuser_dataがNULLです。\n");  //@fix
  if(!user_data) return;
  s32 count = GetMeshCount();
  if( mesh_index < count )
  {
    nw::gfx::res::ResModel res_model = GetNwModel()->GetResModel();
    nw::gfx::res::ResMesh  res_mesh  = res_model.GetMeshes(mesh_index);
    nw::ut::ResMetaData res_meta_data = res_mesh.GetUserData(user_data_index);
    user_data->SetNwResMetaData(res_meta_data);
  }
  else
  {
    GFL_ASSERT_MSG(0, "メッシュのインデックス%d>=%dが不正です。\n", mesh_index, count);
  }

#endif
}


//-----------------------------------------------------------------------------
/*
 *  @brief         計算処理にて呼び出されるコールバック
 */
//-----------------------------------------------------------------------------
// 途中でコールバックをなしにする場合は、戻り値を覚えておいて下さい。
// コールバックを追加できなかったときCALLBACK_INDEX_NOT_ADDを返す。
s32 Model::AddCalculateCallback(ModelCalculateCallbackFunction function, void* work)
{
  s32 index = CALLBACK_INDEX_NOT_ADD;
  for( u32 i=0; i<m_calculate_callback_num_max; ++i )
  {
    if( !m_calculate_callback_function_array[i] )
    {
      m_calculate_callback_function_array[i] = function;
      m_calculate_callback_work_array[i] = work;
      index = i;
      break;
    }
  }
  GFL_ASSERT_MSG(index>=0, "コールバックが既に%d個あるため、追加できませんでした。\n", m_calculate_callback_num_max);  //@check
  return index;
}
// 引数calculate_callback_indexにはAddCalculateCallbackの戻り値を渡して下さい。
void Model::RemoveCalculateCallback(s32 calculate_callback_index)
{
  if( 0<=calculate_callback_index && calculate_callback_index<m_calculate_callback_num_max )
  {
    m_calculate_callback_function_array[calculate_callback_index] = NULL;
    m_calculate_callback_work_array[calculate_callback_index] = NULL;
  }
  else
  {
    GFL_ASSERT_MSG(0, "calculate_callback_index=%dが不正です。コールバックは最大%d個です。\n", calculate_callback_index, m_calculate_callback_num_max);
  }
}


#if 0
//準備中
//-----------------------------------------------------------------------------
/*
 *  @brief         計算フラグ
 *                 trueのとき計算される(@note 全部？一部？)
 *                 ただしこれがtrueのときでも、GetTreeFlagがfalseのときは計算されない
 */
//-----------------------------------------------------------------------------
void Model::SetCalculateFlag(b32 flag)
{
  flag;
}
b32  Model::GetCalculateFlag(void) const
{
  return true;
}
#endif

//-----------------------------------------------------------------------------
/*
 *  @brief         描画フラグ
 *                 trueのとき描画される
 *                 ただしこれがtrueのときでも、GetTreeFlagがfalseのときは描画されない
 */
//-----------------------------------------------------------------------------
void Model::SetVisible(b32 visible)
{
  nw::gfx::Model* model = GetNwModel();
  model->SetVisible(visible);
}
b32  Model::IsVisible(void) const
{
  nw::gfx::Model* model = GetNwModel();
  return model->IsVisible();
}

#if 0
//SceneNodeへ移行した
//-----------------------------------------------------------------------------
/*
 *  @brief         シーンツリーフラグ
 *                 trueのときSceneにAddされていたらシーンツリーに入れられる、falseのときSceneにAddされていてもシーンツリーから外される
 *                 これがtrueであることと、ユーザがSceneにAddしたことは、同じではありません。
 */
//-----------------------------------------------------------------------------
void Model::SetTreeFlag(b32 flag)
{
  m_tree_flag = flag;
}
b32  Model::GetTreeFlag(void) const
{
  return m_tree_flag;
}
#endif

#if 0
//準備中
//-----------------------------------------------------------------------------
/*
 *  @brief         計算されるかどうか
 */
//-----------------------------------------------------------------------------
Model::CalculateState Model::GetCalculateState(void) const
{
  return CALCULATE_STATE_NO;
}
#endif

//-----------------------------------------------------------------------------
/*
 *  @brief         描画されるかどうか
 */
//-----------------------------------------------------------------------------
Model::DrawState Model::GetDrawState(void) const
{
  if( IsVisible() && GetTreeFlag() ) return DRAW_STATE_OK;
  return DRAW_STATE_NO;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションを割り当てるスロットが何個用意されているか
 *
 *  @retval        u32    用意されているスロットの個数
 */
//-----------------------------------------------------------------------------
u32 Model::GetAnimationSlotNum(void) const
{
  //u32 slot_num = 0;
  //if( m_animation_manager )
  //{
  //  slot_num = m_animation_manager->GetSlotNum();
  //}
  //return slot_num;
  return m_animation_slot_num;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションを割り当てるスロットにアニメーションが割り当てられているか
 *
 *  @param[in]     slot_index    アニメーションを割り当てるスロットのインデックス
 *
 *  @retval        b32    アニメーションが割り当てられていたらtrue
 */
//-----------------------------------------------------------------------------
b32 Model::IsAnimationSlotHavingAnimation(u32 slot_index) const  // havingは英語として気になる
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "スロットのインデックス%dが不正です。\n", slot_index );
    return false;
  }

  return ( m_animation_slot_array[slot_index].GetState() != AnimationSlot::STATE_NONE );
#endif
  if( m_animation_manager )
  {
    if( m_animation_manager->GetSlotAnimationNum(slot_index) > 0 )
    {
      return true;
    }
  }
  return false;
}


//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションスロットの有効/無効を取得する
 *
 *  @param[in]     slot_index    アニメーションを割り当てるスロットのインデックス
 *
 *  @retval        b32    アニメーションスロットが有効ならtrue
 */
//-----------------------------------------------------------------------------
b32  Model::IsAnimationSlotEnable(u32 slot_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->IsSlotEnable(slot_index);
  }
  return false;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションスロットの有効/無効を設定する
 *
 *  @param[in]     slot_index    アニメーションを割り当てるスロットのインデックス
 *  @param[in]     enable        アニメーションスロットを有効にするならtrue
 */
//-----------------------------------------------------------------------------
void Model::SetAnimationSlotEnable(u32 slot_index, b32 enable)
{
  if( m_animation_manager )
  {
    m_animation_manager->SetSlotEnable(slot_index, enable);
  }
}


//-----------------------------------------------------------------------------
/*
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
void Model::ChangeAnimation(
  gfl::heap::NwAllocator* allocator,
  ResAnim*                res_anim,
  u32                     change_frame,
  u32                     slot_index,
  s32                     max_anim_members,
  bool                    force
)
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "スロットのインデックス%dが不正です。\n", slot_index );
    return;
  }

  m_animation_slot_array[slot_index].ChangeAnimation(
      this,
      allocator,
      res_anim,
      change_frame,
      max_anim_members,
      force
  );
#endif
  ChangeUnitAnimation(
      allocator,
      allocator,
      slot_index,
      0,
      res_anim,
      change_frame,
      max_anim_members,
      force
  );
}

//-----------------------------------------------------------------------------
/*
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
#if 0
/*  そのうちこうするかも。
   *  @param[in]     resource                リソース(NULLを渡すとアニメーションを解除します)
   *  @param[in]     animation_resource_type アニメーションリソースのタイプ(resourceがNULLのときのみANIMATION_RESOURCE_TYPE_ALLを渡してよい。
   *                                         ANIMATION_RESOURCE_TYPE_ALLのとき全て(スケルタル、マテリアル、ビジビリティ)のアニメーションを解除します)*/
#endif
void Model::ChangeAnimation(
  gfl::heap::NwAllocator*          allocator,
  Resource*                        resource,
  Resource::AnimationResourceType  animation_resource_type,
  s32                              index_in_resource,
  u32                              change_frame,
  u32                              slot_index,
  s32                              max_anim_members,
  bool                             force
)
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "スロットのインデックス%dが不正です。\n", slot_index );
    return;
  }

  m_animation_slot_array[slot_index].ChangeAnimation(
      this,
      allocator,
      resource,
      animation_resource_type,
      index_in_resource,
      change_frame,
      max_anim_members,
      force
  );
#endif
  ResAnim* res_anim_ptr = NULL;
  ResAnim  res_anim;
  if( resource )
  {
    resource->GetResAnim(animation_resource_type, index_in_resource, &res_anim);
    res_anim_ptr = &res_anim;
  }
  ChangeAnimation(
      allocator,
      res_anim_ptr,
      change_frame,
      slot_index,
      max_anim_members,
      force
  );
}


//-----------------------------------------------------------------------------
/*
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
void Model::ChangeUnitAnimation(
  gfl::heap::NwAllocator* heap_allocator,
  gfl::heap::NwAllocator* device_allocator,
  u32                     slot_index,
  u32                     unit_index,
  ResAnim*                res_anim,
  u32                     change_frame,
  s32                     max_anim_members,
  b32                     force
)
{
  if(m_animation_slot_num == 0 )
  {
    GFL_ASSERT_MSG(0, "NOT Animatable Model\n");
    return;
  }

  if( !m_animation_manager )
  {
    if( heap_allocator )
    {
      m_animation_manager = GFL_NEW(heap_allocator->GetHeapBase()) AnimationManager;
      m_animation_manager->UseOwn_Create(
          heap_allocator,
          device_allocator,
          this,
          m_animation_slot_num,
          m_animation_blend_unit_num,
          m_animation_num_in_unit
      );
    }
    else
    {
      // アニメーション初割り当てなのにメモリがNULLでも、res_animがNULLなら見逃す。
      GFL_ASSERT_MSG(heap_allocator==NULL && res_anim==NULL, "アニメーション初割り当てですが、メモリがNULLのため生成できません。\n");  //@check
    }
  }
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_ChangeAnimation(
      heap_allocator,
      device_allocator,
      slot_index,
      unit_index,
      res_anim,
      change_frame,
      max_anim_members,
      force
    );
  }
}
void Model::ChangeUnitAnimation(
  gfl::heap::NwAllocator*          heap_allocator,
  gfl::heap::NwAllocator*          device_allocator,
  u32                              slot_index,
  u32                              unit_index,
  Resource*                        resource,
  Resource::AnimationResourceType  animation_resource_type,
  s32                              index_in_resource,
  u32                              change_frame,
  s32                              max_anim_members,
  b32                              force
)
{
  ResAnim* res_anim_ptr = NULL;
  ResAnim  res_anim;
  if( resource )
  {
    resource->GetResAnim(animation_resource_type, index_in_resource, &res_anim);
    res_anim_ptr = &res_anim;
  }
  ChangeUnitAnimation(
      heap_allocator,
      device_allocator,
      slot_index,
      unit_index,
      res_anim_ptr,
      change_frame,
      max_anim_members,
      force
  );
}


//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションのフレームを設定する
 *
 *  @param[in]     frame           フレーム
 *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
 */
//-----------------------------------------------------------------------------
void Model::SetAnimationFrame(f32 frame, u32 slot_index)
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "スロットのインデックス%dが不正です。\n", slot_index );
    return;
  }

  m_animation_slot_array[slot_index].SetAnimationFrame(frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetFrame(slot_index, frame);
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションのフレームを取得する
 *
 *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
 * 
 *  @retval        フレーム
 */
//-----------------------------------------------------------------------------
f32 Model::GetAnimationFrame(u32 slot_index) const
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "スロットのインデックス%dが不正です。\n", slot_index );
    return 0.0f;
  }

  return m_animation_slot_array[slot_index].GetAnimationFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->GetFrame(slot_index);
  }
  else
  {
    return 0.0f;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの更新フレームを設定する
 *                 0.0fで停止、1.0fで等倍、2.0fで2倍速、-1.0fで逆等倍再生
 *
 *  @param[in]     step_frame      更新フレーム
 *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
 */
//-----------------------------------------------------------------------------
void Model::SetAnimationStepFrame(f32 step_frame, u32 slot_index)
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "スロットのインデックス%dが不正です。\n", slot_index );
    return;
  }

  m_animation_slot_array[slot_index].SetAnimationStepFrame(step_frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetStepFrame(slot_index, step_frame);
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの更新フレームを取得する
 *
 *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
 * 
 *  @retval        更新フレーム
 */
//-----------------------------------------------------------------------------
f32 Model::GetAnimationStepFrame(u32 slot_index) const
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "スロットのインデックス%dが不正です。\n", slot_index );
    return 0.0f;
  }

  return m_animation_slot_array[slot_index].GetAnimationStepFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->GetStepFrame(slot_index);
  }
  else
  {
    return 0.0f;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの開始フレームを設定する
 *
 *  @param[in]     frame           フレーム
 *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
 */
//-----------------------------------------------------------------------------
void Model::SetAnimationStartFrame(f32 frame, u32 slot_index)
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "スロットのインデックス%dが不正です。\n", slot_index );
    return;
  }

  m_animation_slot_array[slot_index].SetAnimationStartFrame(frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetStartFrame(slot_index, frame);
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの開始フレームを取得する
 *
 *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
 *
 *  @retval        フレーム
 */
//-----------------------------------------------------------------------------
f32 Model::GetAnimationStartFrame(u32 slot_index) const
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "スロットのインデックス%dが不正です。\n", slot_index );
    return 0.0f;
  }

  return m_animation_slot_array[slot_index].GetAnimationStartFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->GetStartFrame(slot_index);
  }
  else
  {
    return 0.0f;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの終了フレームを設定する
 *
 *  @param[in]     frame           フレーム
 *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
 */
//-----------------------------------------------------------------------------
void Model::SetAnimationEndFrame(f32 frame, u32 slot_index)
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "スロットのインデックス%dが不正です。\n", slot_index );
    return;
  }

  m_animation_slot_array[slot_index].SetAnimationEndFrame(frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetEndFrame(slot_index, frame);
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの終了フレームを取得する
 *
 *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
 *
 *  @retval        フレーム
 */
//-----------------------------------------------------------------------------
f32 Model::GetAnimationEndFrame(u32 slot_index) const
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "スロットのインデックス%dが不正です。\n", slot_index );
    return 0.0f;
  }

  return m_animation_slot_array[slot_index].GetAnimationEndFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->GetEndFrame(slot_index);
  }
  else
  {
    return 0.0f;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの開始フレームと終了フレームを設定する
 *
 *  @param[in]     start_frame     開始フレーム
 *  @param[in]     end_frame       終了フレーム
 *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
 */
//-----------------------------------------------------------------------------
void Model::SetAnimationStartAndEndFrame(f32 start_frame, f32 end_frame, u32 slot_index )
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "スロットのインデックス%dが不正です。\n", slot_index );
    return;
  }

  m_animation_slot_array[slot_index].SetAnimationStartAndEndFrame(start_frame, end_frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetStartAndEndFrame(slot_index, start_frame, end_frame);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの現在のフレームが開始フレームか否か判定する
 *
 *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
 *
 *  @retval        bool    現在のフレームが開始フレームの場合tureを返す
 */
//-----------------------------------------------------------------------------
bool Model::IsAnimationFrameStartFrame(u32 slot_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->IsFrameStartFrame(slot_index);
  }
  else
  {
    return false;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの現在のフレームが終了フレームか否か判定する
 *
 *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
 *
 *  @retval        bool    現在のフレームが終了フレームの場合tureを返す
 */
//-----------------------------------------------------------------------------
bool Model::IsAnimationFrameEndFrame(u32 slot_index) const
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "スロットのインデックス%dが不正です。\n", slot_index );
    return 0.0f;
  }

  return m_animation_slot_array[slot_index].IsAnimationFrameEndFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->IsFrameEndFrame(slot_index);
  }
  else
  {
    return false;
  }
}

//-----------------------------------------------------------------------------
/*
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
bool Model::IsAnimationFrameStepTerminalFrame(u32 slot_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->IsFrameStepTerminalFrame(slot_index);
  }
  else
  {
    return false;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションがループ再生か否か取得する
 *
 *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
 *
 *  @retval        bool            ループのときtrue、ループでないときfalseを返す
 */
//-----------------------------------------------------------------------------
bool Model::IsAnimationLoop(u32 slot_index) const
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "スロットのインデックス%dが不正です。\n", slot_index );
    return false;
  }

  return m_animation_slot_array[slot_index].IsAnimationLoop();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->IsLoop(slot_index);
  }
  else
  {
    return false;
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションにループ再生を設定する
 *
 *  @param[in]     loop            ループのときtrue、ループでないときfalseを設定する
 *  @param[in]     slot_index      アニメーションを割り当てたスロットのインデックス
 */
//-----------------------------------------------------------------------------
void Model::SetAnimationLoop(bool loop, u32 slot_index)
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "スロットのインデックス%dが不正です。\n", slot_index );
    return;
  }
  
  m_animation_slot_array[slot_index].SetAnimationLoop(loop);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetLoop(slot_index, loop);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         フレーム操作
 */
//-----------------------------------------------------------------------------
void Model::SetUnitFrame(u32 slot_index, u32 unit_index, f32 frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitFrame(slot_index, unit_index, frame);
  }
}
f32  Model::GetUnitFrame(u32 slot_index, u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_GetUnitFrame(slot_index, unit_index);
  }
  return 0.0f;
}
void Model::SetUnitStepFrame(u32 slot_index, u32 unit_index, f32 step_frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitStepFrame(slot_index, unit_index, step_frame);
  }
}
f32  Model::GetUnitStepFrame(u32 slot_index, u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_GetUnitStepFrame(slot_index, unit_index);
  }
  return 0.0f;
}
void Model::SetUnitStartFrame(u32 slot_index, u32 unit_index, f32 frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitStartFrame(slot_index, unit_index, frame);
  }
}
f32  Model::GetUnitStartFrame(u32 slot_index, u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_GetUnitStartFrame(slot_index, unit_index);
  }
  return 0.0f;
}
void Model::SetUnitEndFrame(u32 slot_index, u32 unit_index, f32 frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitEndFrame(slot_index, unit_index, frame);
  }
}
f32  Model::GetUnitEndFrame(u32 slot_index, u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_GetUnitEndFrame(slot_index, unit_index);
  }
  return 0.0f;
}
void Model::SetUnitStartAndEndFrame(u32 slot_index, u32 unit_index, f32 start_frame, f32 end_frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitStartAndEndFrame(slot_index, unit_index, start_frame, end_frame);
  }
}
b32  Model::IsUnitFrameEndFrame(u32 slot_index, u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_IsUnitFrameEndFrame(slot_index, unit_index);
  }
  return false;
}
b32  Model::IsUnitLoop(u32 slot_index, u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_IsUnitLoop(slot_index, unit_index);
  }
  return false;
}
void Model::SetUnitLoop(u32 slot_index, u32 unit_index, b32 loop)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitLoop(slot_index, unit_index, loop);
  }
}


//-----------------------------------------------------------------------------
/*
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
void Model::BlendUnitAnimation(
    gfl::heap::NwAllocator*    heap_allocator,
    gfl::heap::NwAllocator*    device_allocator,
    u32                        slot_index,
    AnimationSlot::Mode        mode,
    b32                        interpolator_ignore_no_anim_member,
    b32                        interpolator_normalization_enable,
    u32                        animation_unit_index
)
{
  if(m_animation_slot_num == 0 )
  {
    GFL_ASSERT_MSG(0, "NOT Animatable Model\n");
    return;
  }

  if( !m_animation_manager )
  {
    m_animation_manager = GFL_NEW(heap_allocator->GetHeapBase()) AnimationManager;
    m_animation_manager->UseOwn_Create(
        heap_allocator,
        device_allocator,
        this,
        m_animation_slot_num,
        m_animation_blend_unit_num,
        m_animation_num_in_unit
    );
  }
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_BlendAnimation(
      heap_allocator,
      device_allocator,
      slot_index,
      mode,
      interpolator_ignore_no_anim_member,
      interpolator_normalization_enable,
      animation_unit_index
    );
  }
}
void Model::BlendUnit2Animation(
    gfl::heap::NwAllocator*    heap_allocator,
    gfl::heap::NwAllocator*    device_allocator,
    u32                        slot_index,
    AnimationSlot::Mode        mode,
    f32                        weight0,
    f32                        weight1,
    b32                        interpolator_ignore_no_anim_member,
    b32                        interpolator_normalization_enable,
    u32                        animation_unit_index
)
{
  BlendUnitAnimation(
    heap_allocator,
    device_allocator,
    slot_index,
    mode,
    interpolator_ignore_no_anim_member,
    interpolator_normalization_enable,
    animation_unit_index
  );
  {
    if(    mode == AnimationSlot::MODE_BLEND_INTERPOLATOR
        || mode == AnimationSlot::MODE_BLEND_ADDER )
    {
      SetUnitBlendWeight(slot_index, 0, weight0);
      SetUnitBlendWeight(slot_index, 1, weight1);
    }
  }
}


//-----------------------------------------------------------------------------
/*
 *  @brief         ブレンド重み
 */
//-----------------------------------------------------------------------------
void Model::SetUnitBlendWeight(u32 slot_index, u32 unit_index, f32 weight)
{
  if( m_animation_manager )
  {
    m_animation_manager->SetBlendWeight(slot_index, unit_index, weight);
  }
}
f32  Model::GetUnitBlendWeight(u32 slot_index, u32 unit_index)
{
  if( m_animation_manager )
  {
    return m_animation_manager->GetBlendWeight(slot_index, unit_index);
  }
  return 0.0f;
}

//-----------------------------------------------------------------------------
/*
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
void Model::ResetAnimationPose(ResAnim::Type type)
{
  if( m_animation_manager )
  {
    m_animation_manager->Reset(type);
  }
}
void Model::ResetAnimationSlotPose(u32 slot_index)
{
  if( m_animation_manager )
  {
    m_animation_manager->ResetSlot(slot_index);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ワールドマトリクスを取得する
 *
 *  @param[out]    matrix          取得したマトリクスを格納して返す
 *
 *  @retval        bool            正常に取得できたらtrue
 */
//-----------------------------------------------------------------------------
bool Model::GetWorldMatrix(gfl::math::MTX34* matrix) const
{
  nw::gfx::Model* nw_model = GetNwModel();
  if( nw_model )
  {
    const gfl::math::MTX34& mtx = nw_model->WorldMatrix();
    *matrix = mtx;
    return true;
  }
  return false;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ワールドマトリクスを設定する
 *                 SceneのCalculateの後でないと反映されません。
 *
 *  @param[in]    matrix           設定するマトリクス
 *
 *  @retval        bool            正常に設定できたらtrue
 */
//-----------------------------------------------------------------------------
bool Model::SetWorldMatrix(const gfl::math::MTX34& matrix)  // ライトやカメラ、ボーンのないモデルなら、これを設定すれば見た目に値が反映される。
                                                            // ボーンのあるモデルだとこれを設定しても見た目に値が反映されているようには見えない。
{
  nw::gfx::Model* nw_model = GetNwModel();
  if( nw_model )
  {
    gfl::math::MTX34& mtx = nw_model->WorldMatrix();
    mtx = matrix;
    
    // ワールド行列を更新したので、逆行列のキャッシュを無効化します。
    nw_model->InvalidateInverseWorldMatrix();

    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ワールドマトリクスの計算の有効フラグ
 *
 *  @param[in]     enable         有効にするときはtrue、無効にするときはfalse
 */
//-----------------------------------------------------------------------------
void Model::SetWorldMatrixCalculationEnable(bool enable)
{
  nw::gfx::Model* nw_model = GetNwModel();
  if( nw_model )
  {
    if( enable )
    {
      nw_model->Transform().EnableFlags( nw::gfx::CalculatedTransform::FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED );
    }
    else
    {
      nw_model->Transform().DisableFlags( nw::gfx::CalculatedTransform::FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED );
    }
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ワールドマトリクスの計算の有効フラグ
 *
 *  @retval        bool            有効のときはtrue、無効のときはfalse
 */
//-----------------------------------------------------------------------------
bool Model::IsWorldMatrixCalculationEnable(void) const
{
  nw::gfx::Model* nw_model = GetNwModel();
  if( nw_model )
  {
    bool enable = nw_model->Transform().IsEnabledFlags( nw::gfx::CalculatedTransform::FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED );
    return enable; 
  }
  return false;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ボーンの数を取得する
 *
 *  @retval        s32             ボーンの数
 */
//-----------------------------------------------------------------------------
s32  Model::GetBonesCount(void) const
{
  nw::gfx::SkeletalModel* nw_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(GetNwSceneNode());
  return nw_model->GetSkeleton()->GetResSkeleton().GetBonesCount();  // @note 不正のとき0を返したい。
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ボーンのインデックス番号を取得する
 *
 *  @param[in]     key             ボーンの名前
 *
 *  @retval        s32             ボーンのインデックス番号        
 */
//-----------------------------------------------------------------------------
s32  Model::GetBonesIndex(const char* key) const
{
  nw::gfx::SkeletalModel* nw_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(GetNwSceneNode());
  return nw_model->GetSkeleton()->GetResSkeleton().GetBonesIndex(key);  // @note 不正のときBONE_INDEX_NOT_FOUNDを返したい。→不正のときは-1なのでBONE_INDEX_NOT_FOUNDと同じ値。
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ワールドマトリクスポーズのボーンの数を取得する
 *
 *  @retval        s32             ボーンの数       
 */
//-----------------------------------------------------------------------------
s32  Model::GetBonesCountOfWorldMatrixPose(void)
{
  nw::gfx::SkeletalModel* nw_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(GetNwSceneNode());
  nw::gfx::Skeleton::MatrixPose& pose = nw_model->GetSkeleton()->WorldMatrixPose();
  return pose.GetBonesCount();
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ワールドマトリクスポーズのマトリクスを取得する
 *
 *  @param[in]     bone_index      ボーンのインデックス
 *  @param[out]    matrix          取得したマトリクスを格納して返す
 *
 *  @retval        bool            正常に取得できたらtrue
 */
//-----------------------------------------------------------------------------
bool Model::GetBoneMatrixOfWorldMatrixPose(s32 bone_index, gfl::math::MTX34* matrix) const
{
  nw::gfx::SkeletalModel* nw_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(GetNwSceneNode());
  nw::gfx::Skeleton::MatrixPose& pose = nw_model->GetSkeleton()->WorldMatrixPose();
  const gfl::math::MTX34* mtx;
  bool ret = pose.GetMatrix( bone_index, &mtx );
  if( ret )
  {
    *matrix = *mtx;
  }
  return ret; 
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ワールドマトリクスポーズのマトリクスを設定する
 *
 *  @param[in]     bone_index      ボーンのインデックス
 *  @param[in]     matrix          設定するマトリクス
 *
 *  @retval        bool            正常に設定できたらtrue
 */
//-----------------------------------------------------------------------------
bool Model::SetBoneMatrixOfWorldMatrixPose(s32 bone_index, const gfl::math::MTX34& matrix)
{
  nw::gfx::SkeletalModel* nw_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(GetNwSceneNode());
  nw::gfx::Skeleton::MatrixPose& pose = nw_model->GetSkeleton()->WorldMatrixPose();
  gfl::math::MTX34* mtx = NULL;
  bool ret = pose.GetMatrix( bone_index, &mtx );
  if( ret )
  {
    *mtx = matrix;
  }
  return ret; 
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ワールドマトリクスポーズのマトリクスのポインタを取得する
 *
 *  @param[in]     bone_index         ボーンのインデックス
 *
 *  @retval        gfl::math::MTX34*  マトリクスのポインタを返す。
 *                                    NULLのときなし。
 *                                    これに値を代入すればワールドマトリクスポーズのマトリクスに反映されます。
 */
//-----------------------------------------------------------------------------
gfl::math::MTX34* Model::GetBoneMatrixPointerOfWorldMatrixPose(s32 bone_index) const
{
  nw::gfx::SkeletalModel* nw_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(GetNwSceneNode());
  nw::gfx::Skeleton::MatrixPose& pose = nw_model->GetSkeleton()->WorldMatrixPose();
#if 0
  return pose.GetMatrix( bone_index );
  // こちらはbone_indexが不正かどうかのチェックがない関数なので、しょっちゅうASSERTで止まる。
#else
  gfl::math::MTX34* mtx = NULL;
  bool ret = pose.GetMatrix( bone_index, &mtx );
  // ret==falseのときはmtxは変更されないのでそのまま返す。 
  return mtx;
#endif
}


//-----------------------------------------------------------------------------
/*
 *  @brief         差し替え用のボーンデータを作成する
 *
 *  @param[in]     heap_allocator  ヒープメモリのアロケータ
 *  @param[in]     base_model      基本モデル
 */
//-----------------------------------------------------------------------------
void Model::CreateReplaceBoneData(gfl::heap::NwAllocator* heap_allocator, Model* base_model)
{
  s32 bones_count = this->GetBonesCount();

  m_replace_bone_data_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) ReplaceBoneData[bones_count];

  nw::gfx::Model*            nw_model        = this->GetNwModel();
  nw::gfx::SkeletalModel*    skeletal_model  = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(nw_model);
  nw::gfx::Skeleton*         skeleton        = skeletal_model->GetSkeleton();
  nw::gfx::res::ResSkeleton  res_skeleton    = skeleton->GetResSkeleton();
  
  for( s32 b=0; b<bones_count; ++b )
  {
    nw::gfx::res::ResBone res_bone = res_skeleton.GetBones(b);
    const char* bone_name = res_bone.GetName();

    s32 base_index = base_model->GetBonesIndex(bone_name);
    if( base_index >= 0 )
    {
      m_replace_bone_data_array[b].base_index = base_index;
    }
    else
    {
      m_replace_bone_data_array[b].base_index = gfl::grp::g3d::Model::BONE_INDEX_NONE;
      
      // 先祖を探す
      m_replace_bone_data_array[b].base_ancestor_index = gfl::grp::g3d::Model::BONE_INDEX_NONE;  // 初期値代入
      s32 parent_index = res_bone.GetParentBoneIndex();  // 親
      while( parent_index >= 0 )
      {
        if( parent_index < b )
        {
          // 既に先祖が設定されているボーンを参照している
          if( m_replace_bone_data_array[parent_index].base_index >= 0 )
          {
            m_replace_bone_data_array[b].base_ancestor_index = m_replace_bone_data_array[parent_index].base_index;
          }
          else
          {
            m_replace_bone_data_array[b].base_ancestor_index = m_replace_bone_data_array[parent_index].base_ancestor_index;
          }
          break;
        }
        else
        {
          // まだ先祖が設定されていないボーンを参照している
          nw::gfx::res::ResBone res_bone_parent = res_skeleton.GetBones(parent_index);
          const char* bone_name_parent = res_bone_parent.GetName();
   
          s32 base_ancestor_index = base_model->GetBonesIndex(bone_name_parent);
          if( base_ancestor_index >= 0 )
          {
            m_replace_bone_data_array[b].base_ancestor_index = base_ancestor_index;
            break;
          }
          else
          {
            parent_index = res_bone_parent.GetParentBoneIndex();  // 更に親にさかのぼる
          }
        }
      }
    }
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         差し替え用のボーンデータを破棄する
 */
//-----------------------------------------------------------------------------
void Model::DestroyReplaceBoneData(void)
{
  if( m_replace_bone_data_array )
  {
    GFL_DELETE_ARRAY m_replace_bone_data_array;
    m_replace_bone_data_array = NULL;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         差し替え用のボーンの無視設定を行う(親方向)
 *
 *  @param[in]     name   nameのボーンより親のボーンは全て無視設定にする(nameのボーンは有効設定のまま)
 */
//-----------------------------------------------------------------------------
void Model::IgnoreParentReplaceBone(const char* name)
{
  if( m_replace_bone_data_array )
  {
    s32 bones_count = this->GetBonesCount();
 
    nw::gfx::Model*            nw_model        = this->GetNwModel();
    nw::gfx::SkeletalModel*    skeletal_model  = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(nw_model);
    nw::gfx::Skeleton*         skeleton        = skeletal_model->GetSkeleton();
    nw::gfx::res::ResSkeleton  res_skeleton    = skeleton->GetResSkeleton();
    
    for( s32 b=0; b<bones_count; ++b )
    {
      nw::gfx::res::ResBone res_bone = res_skeleton.GetBones(b);
      const char* bone_name = res_bone.GetName();

      if( strcmp( bone_name, name ) == 0 )
      {
        // 親を探す
        s32 parent_index = res_bone.GetParentBoneIndex();  // 親
        while( parent_index >= 0 )
        {
          // 親のボーンを無視設定にする
          m_replace_bone_data_array[parent_index].base_index = gfl::grp::g3d::Model::BONE_INDEX_IGNORE;
          
          // 更に親にさかのぼる
          nw::gfx::res::ResBone res_bone_parent = res_skeleton.GetBones(parent_index);
          parent_index = res_bone_parent.GetParentBoneIndex(); 
        }
        break;
      }
    }
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         差し替え用のボーンの無視設定を行う(子方向)
 *
 *  @param[in]     name   nameのボーンより子のボーンは全て無視設定にする(nameのボーンは有効設定のまま)
 */
//-----------------------------------------------------------------------------
void Model::IgnoreChildReplaceBone(const char* name)
{
  if( m_replace_bone_data_array )
  {
    s32 bones_count = this->GetBonesCount();
 
    nw::gfx::Model*            nw_model        = this->GetNwModel();
    nw::gfx::SkeletalModel*    skeletal_model  = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(nw_model);
    nw::gfx::Skeleton*         skeleton        = skeletal_model->GetSkeleton();
    nw::gfx::res::ResSkeleton  res_skeleton    = skeleton->GetResSkeleton();
    
    for( s32 b=0; b<bones_count; ++b )
    {
      nw::gfx::res::ResBone res_bone = res_skeleton.GetBones(b);
      const char* bone_name = res_bone.GetName();

      if( strcmp( bone_name, name ) == 0 )
      {
        // 子を探す
        nw::gfx::res::ResBone res_bone_child = res_bone.GetChildBone();  // 子
        while( res_bone_child.IsValid() )
        {
          ignoreSelfChildReplaceBoneRecursive(
            res_bone_child
#if GFL_DEBUG
            , 0
#endif        
          );  // 再帰呼び出し

          // 子ボーンの兄弟ボーン
          res_bone_child = res_bone_child.GetNextSibling();
        }
        break;
      }
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         差し替え用のボーンの無視設定を行う(子方向)
 *
 *  @param[in]     name   nameのボーンとそれより子のボーンは全て無視設定にする(nameのボーンも無効設定になる)
 */
//-----------------------------------------------------------------------------
void Model::IgnoreSelfChildReplaceBone(const char* name)
{
  if( m_replace_bone_data_array )
  {
    s32 bones_count = this->GetBonesCount();
 
    nw::gfx::Model*            nw_model        = this->GetNwModel();
    nw::gfx::SkeletalModel*    skeletal_model  = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(nw_model);
    nw::gfx::Skeleton*         skeleton        = skeletal_model->GetSkeleton();
    nw::gfx::res::ResSkeleton  res_skeleton    = skeleton->GetResSkeleton();
    
    for( s32 b=0; b<bones_count; ++b )
    {
      nw::gfx::res::ResBone res_bone = res_skeleton.GetBones(b);
      const char* bone_name = res_bone.GetName();

      if( strcmp( bone_name, name ) == 0 )
      {
        // nameのボーン
        while( res_bone.IsValid() )
        {
          ignoreSelfChildReplaceBoneRecursive(
            res_bone
#if GFL_DEBUG
            , 0
#endif        
          );  // 再帰呼び出し

          // nameのボーンの兄弟ボーン
          res_bone = res_bone.GetNextSibling();
        }
        break;
      }
    }
  }
}

//-----------------------------------------------------------------------------
/*
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
void Model::SetWorldReplaceBone(Model* base_model)
{
  if( m_replace_bone_data_array )
  {
    // 基本モデルのスケルトンで上書きする
    nw::gfx::Model*                   base_nw_model             = base_model->GetNwModel();
    nw::gfx::SkeletalModel*           base_skeletal_model       = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(base_nw_model);
    nw::gfx::Skeleton*                base_skeleton             = base_skeletal_model->GetSkeleton();
    nw::gfx::Skeleton::MatrixPose&    base_world_matrix_pose    = base_skeleton->WorldMatrixPose();
    nw::gfx::Skeleton::TransformPose& base_local_transform_pose = base_skeleton->LocalTransformPose();
    nw::gfx::Skeleton::TransformPose& base_world_transform_pose = base_skeleton->WorldTransformPose();

    {
      nw::gfx::Model*                   nw_model             = this->GetNwModel();
      nw::gfx::SkeletalModel*           skeletal_model       = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(nw_model);
      nw::gfx::Skeleton*                skeleton             = skeletal_model->GetSkeleton();
      nw::gfx::Skeleton::MatrixPose&    world_matrix_pose    = skeleton->WorldMatrixPose();
      nw::gfx::Skeleton::TransformPose& local_transform_pose = skeleton->LocalTransformPose();
      nw::gfx::Skeleton::TransformPose& world_transform_pose = skeleton->WorldTransformPose();
      nw::gfx::res::ResSkeleton         res_skeleton         = skeleton->GetResSkeleton();
   
      s32 bones_count = this->GetBonesCount();
      for( s32 b=0; b<bones_count; ++b )
      {
        if( m_replace_bone_data_array[b].base_index != gfl::grp::g3d::Model::BONE_INDEX_IGNORE )
        {
          nw::gfx::res::ResBone  res_bone              = res_skeleton.GetBones(b);
          s32                    parent_index          = res_bone.GetParentBoneIndex();
 
          nw::math::MTX34*              world_matrix    = world_matrix_pose.GetMatrix(b);
          nw::gfx::CalculatedTransform* local_transform = local_transform_pose.GetTransform(b);
          nw::gfx::CalculatedTransform* world_transform = world_transform_pose.GetTransform(b);
 
          if( m_replace_bone_data_array[b].base_index >= 0 )
          {
            nw::math::MTX34*              base_world_matrix    = base_world_matrix_pose.GetMatrix(m_replace_bone_data_array[b].base_index);
            nw::gfx::CalculatedTransform* base_local_transform = base_local_transform_pose.GetTransform(m_replace_bone_data_array[b].base_index);
            nw::gfx::CalculatedTransform* base_world_transform = base_world_transform_pose.GetTransform(m_replace_bone_data_array[b].base_index);
          
            *world_matrix = *base_world_matrix;
          }
          else  // if( m_replace_bone_data_array[b].base_index == gfl::grp::g3d::Model::BONE_INDEX_NONE )
          {
            if( m_replace_bone_data_array[b].base_ancestor_index >= 0 )
            {
              if( m_replace_bone_data_array[parent_index].base_index >= 0 )
              {
                // 一番親なので、再帰処理する
                nw::math::MTX34* parent_world_matrix = base_world_matrix_pose.GetMatrix(m_replace_bone_data_array[parent_index].base_index);
                SetWorldReplaceBoneRecursive( skeleton, res_bone, parent_world_matrix
#if GFL_DEBUG
                    , 0
#endif 
                ); 
              }
            }
            else if( m_replace_bone_data_array[b].base_ancestor_index == gfl::grp::g3d::Model::BONE_INDEX_NONE )
            {
              // (base_index==gfl::grp::g3d::Model::BONE_INDEX_NONE&&base_ancestor_index==gfl::grp::g3d::Model::BONE_INDEX_NONE)のとき、baseのhumanトップに追従する
              if( parent_index < 0 )
              {
                // 一番親なので、再帰処理する
                nw::math::MTX34 parent_world_matrix;
                base_model->GetWorldMatrix(&parent_world_matrix);
                SetWorldReplaceBoneRecursive( skeleton, res_bone, &parent_world_matrix
#if GFL_DEBUG
                    , 0
#endif 
                ); 
              }
            }
          }
        }
        // このif文が真のときは、
        // 無視していいボーン
        // なので、何もしない
      
      }
    }
  }
}
//void Model::SetLocalReplaceBone(Model* base_model)  // @note ローカルマトリクス版だが、まだできていない
                                                // NW4Cのスケルトン共有と同じ結果を得るつもりの関数。スケルトンアニメはしているが場所が違うというもの。
//void Model::SetWorldLocalReplaceBone(Model* base_model)  // @note ワールドマトリクスにもローカルマトリクスに値を設定する版だが、まだできていない

//-----------------------------------------------------------------------------
/*
 *  @brief         res_boneのワールドマトリクスにparent_world_matrixを反映したものを設定する。
 *                 その後、再帰的に子ボーンにその処理を行っていく。
 *                 
 *  @param[in]     skeleton  partのスケルトン
 *  @param[in]     res_bone  partのボーン
 *  @param[in]     parent_world_matrix  res_boneの親のワールドマトリクス
 */
//-----------------------------------------------------------------------------
void Model::SetWorldReplaceBoneRecursive(
    nw::gfx::Skeleton*     skeleton,
    nw::gfx::res::ResBone  res_bone,
    const nw::math::MTX34* parent_world_matrix
#if GFL_DEBUG
    , const s32            recursive_count 
#endif
)
{
#if GFL_DEBUG
  if( recursive_count >= GFL_GRP_G3D_MODEL_RECURSIVE_COUNT_MAX )
  {
    GFL_PRINT("警告: 再帰呼び出しの回数が%d回になりました。\n", recursive_count);
  }
  // @note NW4Cのスケルトンのアップデート処理を読んでみると、再帰を使っていないように見える。
  // おそらく親の骨のほうが若い番号になっており必ず先に計算されるようにリソースの段階でなっているのだろう。
  // それを信用できるなら、ここの処理も再帰にしなくて済むと思われる。
#endif 

  nw::gfx::Skeleton::MatrixPose&    world_matrix_pose    = skeleton->WorldMatrixPose();
  nw::gfx::Skeleton::TransformPose& local_transform_pose = skeleton->LocalTransformPose();
  nw::gfx::Skeleton::TransformPose& world_transform_pose = skeleton->WorldTransformPose();
  nw::gfx::res::ResSkeleton         res_skeleton         = skeleton->GetResSkeleton();

  s32 b = res_bone.GetIndex();
  nw::math::MTX34*              world_matrix    = world_matrix_pose.GetMatrix(b);
  nw::gfx::CalculatedTransform* local_transform = local_transform_pose.GetTransform(b);
  nw::gfx::CalculatedTransform* world_transform = world_transform_pose.GetTransform(b);

  nw::math::MTX34  local_transform_matrix;
  {
    const nw::math::MTX34& local_t_transform_matrix = local_transform->TransformMatrix();
    const nw::math::VEC3&  local_t_scale            = local_transform->Scale();

    internal::CopyTranslate(&local_transform_matrix, local_t_transform_matrix);
    internal::MultScale(&local_transform_matrix, &local_t_transform_matrix, local_t_scale);
  }

  nw::math::MTX34Mult( world_matrix, parent_world_matrix, &local_transform_matrix );

  // 子ボーン
  nw::gfx::res::ResBone res_bone_child = res_bone.GetChildBone();
  while( res_bone_child.IsValid() )
  {
    if( m_replace_bone_data_array[res_bone_child.GetIndex()].base_index != gfl::grp::g3d::Model::BONE_INDEX_IGNORE )
    {
      SetWorldReplaceBoneRecursive( skeleton, res_bone_child, world_matrix
#if GFL_DEBUG
          , recursive_count +1
#endif        
      );  // 再帰呼び出し
    }
    // このif文が真のときは、
    // 無視していいボーン
    // なので、これより子に対しては何もしない

    // 子ボーンの兄弟ボーン
    res_bone_child = res_bone_child.GetNextSibling();
  }       
}

//-----------------------------------------------------------------------------
/*
 *  @brief         差し替え用のボーンの無視設定を再帰的に行う(子方向)
 *
 *  @param[in]     res_bone   res_boneのボーンを無視設定にし、res_boneより子のボーンも全て無視設定にする(res_boneのボーンも無視設定になる)
 */
//-----------------------------------------------------------------------------
void Model::ignoreSelfChildReplaceBoneRecursive(
    nw::gfx::res::ResBone  res_bone
#if GFL_DEBUG
    , const s32            recursive_count 
#endif
) 
{
#if GFL_DEBUG
  if( recursive_count >= GFL_GRP_G3D_MODEL_RECURSIVE_COUNT_MAX )
  {
    GFL_PRINT("警告: 再帰呼び出しの回数が%d回になりました。\n", recursive_count);
  }
  // @note NW4Cのスケルトンのアップデート処理を読んでみると、再帰を使っていないように見える。
  // おそらく親の骨のほうが若い番号になっており必ず先に計算されるようにリソースの段階でなっているのだろう。
  // それを信用できるなら、ここの処理も再帰にしなくて済むと思われる。
#endif 

  s32 b = res_bone.GetIndex();

  // ボーンを無視設定にする
  m_replace_bone_data_array[b].base_index = gfl::grp::g3d::Model::BONE_INDEX_IGNORE;

  // 子を探す
  nw::gfx::res::ResBone res_bone_child = res_bone.GetChildBone();  // 子
  while( res_bone_child.IsValid() )
  {
    ignoreSelfChildReplaceBoneRecursive(
      res_bone_child
#if GFL_DEBUG
      , recursive_count +1
#endif        
    );  // 再帰呼び出し

    // 子ボーンの兄弟ボーン
    res_bone_child = res_bone_child.GetNextSibling();
  }       
}


//-----------------------------------------------------------------------------
/*
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
b32 Model::AddConstraintBatonSceneNode(
  SceneNode* baton_scene_node,
  const s32  hand_bone_index,
  const b32  inherit_scale,
  const b32  inherit_rotate,
  const b32  inherit_translate
)
{
  if( hand_bone_index >= 0 )
  {
    if( m_callback_constraint_of_skeleton )
    {
      return m_callback_constraint_of_skeleton->AddBatonSceneNode(
          baton_scene_node,
          hand_bone_index,
          inherit_scale,
          inherit_rotate,
          inherit_translate
      );
    }
  }
  else
  {
    if( m_callback_constraint_of_transform_node )
    {
      return m_callback_constraint_of_transform_node->AddBatonSceneNode(
          baton_scene_node,
          inherit_scale,
          inherit_rotate,
          inherit_translate
      );
    }
  }
  return false;
}
//-----------------------------------------------------------------------------
/*
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
b32 Model::ChangeConstraintBatonSceneNodeInherit(
  SceneNode* baton_scene_node,
  const s32  hand_bone_index,
  const b32  inherit_scale,
  const b32  inherit_rotate,
  const b32  inherit_translate
)
{
  if( hand_bone_index >= 0 )
  {
    if( m_callback_constraint_of_skeleton )
    {
      return m_callback_constraint_of_skeleton->ChangeBatonSceneNodeInherit(
          baton_scene_node,
          hand_bone_index,
          inherit_scale,
          inherit_rotate,
          inherit_translate
      );
    }
  }
  else
  {
    if( m_callback_constraint_of_transform_node )
    {
      return m_callback_constraint_of_transform_node->ChangeBatonSceneNodeInherit(
          baton_scene_node,
          inherit_scale,
          inherit_rotate,
          inherit_translate
      );
    }
  }
  return false;
}
//-----------------------------------------------------------------------------
/*
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
b32 Model::RemoveConstraintBatonSceneNode(
  SceneNode* baton_scene_node,
  const s32  hand_bone_index
)
{
  if( hand_bone_index >= 0 )
  {
    if( m_callback_constraint_of_skeleton )
    {
      return m_callback_constraint_of_skeleton->RemoveBatonSceneNode(
          baton_scene_node,
          hand_bone_index
      );
    }
  }
  else
  {
    if( m_callback_constraint_of_transform_node )
    {
      return m_callback_constraint_of_transform_node->RemoveBatonSceneNode(
          baton_scene_node
      );
    }
  }
  return false;
}


//-----------------------------------------------------------------------------
/*
 *  @brief         レンダーキューへ追加、レンダーキューから削除
 */
//-----------------------------------------------------------------------------
// UserParameterをRenderQueueには使っていないことが判明したので、
// SafeDeleteCheck用の使わせてもらう。
void Model::AddAllRenderQueue(void)  // SafeDeleteCheck用に使っています。
{
  nw::gfx::Model* nw_model = GetNwModel();
  u32 user_parameter = nw_model->GetUserParameter<u32>();
  u32 flag = USER_PARAMETER_RENDER_QUEUE_BIT_MASK;
  user_parameter |= flag;
  nw_model->SetUserParameter<u32>(user_parameter);
}
void Model::RemoveAllRenderQueue(void)  // SafeDeleteCheck用に使っています。
{
  nw::gfx::Model* nw_model = GetNwModel();
  u32 user_parameter = nw_model->GetUserParameter<u32>();
  u32 flag = USER_PARAMETER_RENDER_QUEUE_BIT_MASK;
  user_parameter &= ~flag;
  nw_model->SetUserParameter<u32>(user_parameter);
}
void Model::AddRenderQueue(s32 render_queue_index)  // SafeDeleteCheck用に使っています。
{
  nw::gfx::Model* nw_model = GetNwModel();
  u32 user_parameter = nw_model->GetUserParameter<u32>();
  u32 flag = 0x1 << (USER_PARAMETER_RENDER_QUEUE_BIT_SHIFT+render_queue_index);
  user_parameter |= flag;
  nw_model->SetUserParameter<u32>(user_parameter);
}
void Model::RemoveRenderQueue(s32 render_queue_index)  // SafeDeleteCheck用に使っています。
{
  nw::gfx::Model* nw_model = GetNwModel();
  u32 user_parameter = nw_model->GetUserParameter<u32>();
  u32 flag = 0x1 << (USER_PARAMETER_RENDER_QUEUE_BIT_SHIFT+render_queue_index);
  user_parameter &= ~flag;
  nw_model->SetUserParameter<u32>(user_parameter);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ユーザーパラメータのチェック
 */
//-----------------------------------------------------------------------------
// このモデルをこのレンダーキューで描画するか
// UserParameterをRenderQueueには使っていないことが判明したので、
// SafeDeleteCheck用の使わせてもらう。
bool Model::IsUserParameterRenderQueue( const nw::gfx::Model* nw_model, s32 /*render_queue_index*/ )  // SafeDeleteCheck用に使っています。
{
  if( !( nw_model->IsVisible() && nw_model->IsEnabledResults(nw::gfx::SceneNode::FLAG_IS_VISIBLE) ) ) return false;
    // SetVisibleでは表示OFFにできなかったので、gfx_Model.hのIsVisibleModelDefaultFunctorを参考にして判定を追加した。

#if 0
  u32 user_parameter = nw_model->GetUserParameter<u32>();
  u32 flag = 0x1 << (USER_PARAMETER_RENDER_QUEUE_BIT_SHIFT+render_queue_index);
  return ( (user_parameter & flag) != 0 );
#else
  return true;
#endif
}

//-----------------------------------------------------------------------------
/*
 *  @brief         本クラスのマテリアルのバッファの生成オプションをNintendoWareのバッファの生成オプションに変更する
 */
//-----------------------------------------------------------------------------
bit32 Model::ConvertMaterialBufferOptionToNwBufferOption(
    const MaterialBufferOption  material_buffer_option
)
{
  bit32 nw_buffer_option = nw::gfx::Model::FLAG_BUFFER_NOT_USE;
  static const bit32 material_buffer_option_to_nw_buffer_option[MATERIAL_BUFFER_OPTION_MAX] =
  {
    nw::gfx::Model::FLAG_BUFFER_NOT_USE,
    nw::gfx::Model::FLAG_BUFFER_MATERIAL_COLOR,
    nw::gfx::Model::FLAG_BUFFER_MATERIAL_COLOR | nw::gfx::Model::FLAG_BUFFER_SHADING_PARAMETER,
    nw::gfx::Model::MULTI_FLAG_ANIMATABLE_MATERIAL,
    nw::gfx::Model::MULTI_FLAG_BUFFER_MATERIAL,
  };
  if(    0 <= material_buffer_option
      && material_buffer_option < MATERIAL_BUFFER_OPTION_MAX )
  {
    nw_buffer_option = material_buffer_option_to_nw_buffer_option[material_buffer_option];
  }
  return nw_buffer_option;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         生成時にスケルトン共有をチェックし、適切に処理する
 */
//-----------------------------------------------------------------------------
Model::ShareType Model::CheckSkeletonShareSourceModel(
    Model*               skeleton_share_source_model,
    nw::gfx::Skeleton**  nw_skeleton 
)
{
  // チェック
  if( !skeleton_share_source_model )
  {
    *nw_skeleton = NULL;
    return SHARE_TYPE_RESOURCE;
  }

  if(    ( skeleton_share_source_model->m_skeleton_share_type != SHARE_TYPE_RESOURCE     )
      && ( skeleton_share_source_model->m_skeleton_share_type != SHARE_TYPE_SHARE_SOURCE ) )
  {
    GFL_ASSERT_MSG( 0, "スケルトンの共有元のモデルが、スケルトンの共有元になることができません。\n" );
    *nw_skeleton = NULL;
    return SHARE_TYPE_RESOURCE;
  }

  // 反映
  nw::gfx::SkeletalModel* skeleton_share_source_skeletal_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*> \
                                                                 ( skeleton_share_source_model->GetNwSceneNode() );
  nw::gfx::Skeleton*      skeleton_share_source_skeleton       = skeleton_share_source_skeletal_model->GetSkeleton();

  nw::gfx::res::ResSkeleton skeleton_share_source_res_skeleton = skeleton_share_source_skeleton->GetResSkeleton();
  if( skeleton_share_source_model->m_skeleton_share_type == SHARE_TYPE_RESOURCE )
  {
    skeleton_share_source_res_skeleton.EnableFlags(nw::gfx::ResSkeletonData::FLAG_MODEL_COORDINATE);
    skeleton_share_source_model->m_skeleton_share_type = SHARE_TYPE_SHARE_SOURCE;
  }

  *nw_skeleton = skeleton_share_source_skeleton;
  return SHARE_TYPE_SHARE_DESTINATION;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         生成時にマテリアル共有をチェックし、適切に処理する
 */
//-----------------------------------------------------------------------------
Model::ShareType Model::CheckMaterialShareSourceModel(
    Model*                material_share_source_model,
    MaterialBufferOption  material_buffer_option,
    nw::gfx::Model**      nw_model
)
{
  // チェック
  if( !material_share_source_model )
  {
    *nw_model = NULL;
    return SHARE_TYPE_RESOURCE;
  }

  if( material_buffer_option != MATERIAL_BUFFER_OPTION_NOT_USE )
  {
    GFL_ASSERT_MSG( 0, "material_share_source_modelがNULLでないとき、material_buffer_optionはMATERIAL_BUFFER_OPTION_NOT_USEでなければならない。\n" );
    *nw_model = NULL;
    return SHARE_TYPE_RESOURCE;
  }
  if(    ( material_share_source_model->m_material_share_type != SHARE_TYPE_RESOURCE     )
      && ( material_share_source_model->m_material_share_type != SHARE_TYPE_SHARE_SOURCE ) )
  {
    GFL_ASSERT_MSG( 0, "material_share_source_modelがマテリアルの共有元を参照しています。\n" );
    *nw_model = NULL;
    return SHARE_TYPE_RESOURCE;
  }

  // 反映
  nw::gfx::Model* material_share_source_nw_model = material_share_source_model->GetNwModel();

  if( material_share_source_model->m_material_share_type != SHARE_TYPE_RESOURCE )
  {
    material_share_source_model->m_material_share_type = SHARE_TYPE_SHARE_SOURCE;
  }

  *nw_model = material_share_source_nw_model;
  return SHARE_TYPE_SHARE_DESTINATION;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         生成時にメッシュノードビジビリティ共有をチェックし、適切に処理する
 */
//-----------------------------------------------------------------------------
Model::ShareType Model::CheckMeshNodeVisibilityShareSourceModel(
    Model*                mesh_node_visibility_share_source_model,
    nw::gfx::Model**      nw_model
)
{
  // チェック
  if( !mesh_node_visibility_share_source_model )
  {
    *nw_model = NULL;
    return SHARE_TYPE_RESOURCE;
  }

  if(    ( mesh_node_visibility_share_source_model->m_mesh_node_visibility_share_type != SHARE_TYPE_RESOURCE     )
      && ( mesh_node_visibility_share_source_model->m_mesh_node_visibility_share_type != SHARE_TYPE_SHARE_SOURCE ) )
  {
    GFL_ASSERT_MSG( 0, "mesh_node_visibility_share_source_modelがメッシュノードビジビリティの共有元を参照しています。\n" );
    *nw_model = NULL;
    return SHARE_TYPE_RESOURCE;
  }

  // 反映
  nw::gfx::Model* mesh_node_visibility_share_source_nw_model = mesh_node_visibility_share_source_model->GetNwModel();

  if( mesh_node_visibility_share_source_model->m_mesh_node_visibility_share_type != SHARE_TYPE_RESOURCE )
  {
    mesh_node_visibility_share_source_model->m_mesh_node_visibility_share_type = SHARE_TYPE_SHARE_SOURCE;
  }
  
  *nw_model = mesh_node_visibility_share_source_nw_model;
  return SHARE_TYPE_SHARE_DESTINATION;
}


#if GFL_DEBUG
void Model::SetSafeDeleteCheckInfo(const char* pName)
{
  // UserParameterをRenderQueueには使っていないことが判明したので、
  // SafeDeleteCheck用の使わせてもらう。
  nw::gfx::Model* nw_model = GetNwModel();
  nw_model->SetUserParameter<u32>(reinterpret_cast<u32>(this));

  m_DrawFrameCount = 0;
  SetModelName(pName);
}
void Model::SetModelName(const char* pName)
{
  memset( m_ModelName, 0, sizeof(m_ModelName) );
  if(pName)
  {
    s32   len = strlen( pName );
    
    if ( len >= sizeof(m_ModelName) )
      len = sizeof(m_ModelName) - 1;//末端null文字キープ
    
    memcpy( m_ModelName, pName, len );
  }
}
#endif


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

