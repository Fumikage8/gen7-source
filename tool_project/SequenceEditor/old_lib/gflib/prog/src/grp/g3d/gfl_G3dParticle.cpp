//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   gfl_G3dParticle.cpp
 *  @brief  3Dパーティクル
 *  @author Koji Kawada
 *  @date   2011.05.02
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
#include <grp/g3d/gfl_G3dSystem.h>
#include <grp/g3d/gfl_G3dParticle.h>


#if GFL_DEBUG
    #define GFL_GRP_G3D_PARTICLE_RECURSIVE_COUNT_MAX (16)  // 再帰呼び出しの回数上限
#endif


namespace gfl {
namespace grp {
namespace g3d {

#if GFL_DEBUG
  b32 Particle::s_SafeDeleteCheckEnable        = false;
  b32 Particle::s_SafeDeleteCheckEnableUserSet = false;
  b32 Particle::s_SafeDeleteCheckForce         = false;
  b32 Particle::s_SafeDeleteCheckAssert        = false;
#endif  

//-----------------------------------------------------------------------------
/**
 *     クラス宣言
 */
//-----------------------------------------------------------------------------
//-------------------------------------
/// 3Dパーティクルクラス
//=====================================

void Particle::InitializeDescriptionForDefault(
    Description*  description
)
{
  description->max_children = 16;
  description->model_max_children = 16;
  description->emitter_max_children = 16;
}
/*
  関数で設定するのはもうやめるカメラのようなタイプのあるものだけ関数設定にする他は直接代入で
 void Particle::InitializeDescription(
    Description*  description,
    s32 max_children,
    s32 model_max_children,
    s32 emitter_max_children
)
{
  description->max_children = max_children;
  description->model_max_children = model_max_children;
  description->emitter_max_children = emitter_max_children;
}*/

Particle::Particle(void)
  : SceneNode(),
    //m_tree_flag(true),
    m_is_setup(false),
    m_scene_node_array(NULL),
    m_model_array(NULL),
    m_emitter_array(NULL)
{
  GFL_SINGLETON_INSTANCE(System)->GetParticleManager()->AddObject(this);
}
Particle::~Particle()
{
  Destroy();
  GFL_SINGLETON_INSTANCE(System)->GetParticleManager()->RemoveObject(this);
}

void Particle::Create(
  gfl::heap::NwAllocator* heap_allocator,
  gfl::heap::NwAllocator* device_allocator,
  Resource*               resource,
  const Description*      description
)
{
  SceneNode::Create();
  
  // description
  Description l_description;
  if( description )
  {
    l_description = *description;
  }
  else
  {
    gfl::grp::g3d::Particle::InitializeDescriptionForDefault(&l_description);
  }

  // リソース
  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();

  // モデルの個数
  s32 model_num = nw_res_graphics_file->GetModelsCount();
  // エミッターの個数
  s32 emitter_num = nw_res_graphics_file->GetEmittersCount();
  // モデルの個数とエミッターの個数の合計
  s32 total_num = model_num+emitter_num;

  // 生成したシーンノードを管理しておく配列
  //m_scene_node_array = GFL_NEW(heap_allocator->GetHeapBase()) nw::ut::MoveArray<nw::gfx::SceneNode*>(l_description.max_children, heap_allocator, nw::ut::ARRAY_WRAPPER);  // l_description.max_childrenと同じかそれ以上の個数が必要なので、l_description.max_childrenを使うのはやめる。
  //m_scene_node_array = GFL_NEW(heap_allocator->GetHeapBase()) nw::ut::MoveArray<nw::gfx::SceneNode*>(total_num, heap_allocator, nw::ut::ARRAY_WRAPPER);  // ギリギリの個数だけ用意
  m_scene_node_array = GFL_NEW(heap_allocator->GetHeapBase()) nw::gfx::SceneNodeArray(total_num, heap_allocator, nw::ut::ARRAY_WRAPPER);  // ギリギリの個数だけ用意
 
  // 生成したパーティクルモデルを管理しておく配列
  //m_model_array = GFL_NEW(heap_allocator->GetHeapBase()) nw::ut::MoveArray<nw::gfx::ParticleModel*>(model_num, heap_allocator, nw::ut::ARRAY_WRAPPER);  // ギリギリの個数だけ用意
  m_model_array = GFL_NEW(heap_allocator->GetHeapBase()) nw::gfx::ParticleModelArray(model_num, heap_allocator, nw::ut::ARRAY_WRAPPER);  // ギリギリの個数だけ用意
  // 生成したパーティクルエミッターを管理しておく配列
  //m_emitter_array = GFL_NEW(heap_allocator->GetHeapBase()) nw::ut::MoveArray<nw::gfx::ParticleEmitter*>(emitter_num, heap_allocator, nw::ut::ARRAY_WRAPPER);  // ギリギリの個数だけ用意
  m_emitter_array = GFL_NEW(heap_allocator->GetHeapBase()) nw::gfx::ParticleEmitterArray(emitter_num, heap_allocator, nw::ut::ARRAY_WRAPPER);  // ギリギリの個数だけ用意

  // モデル生成
  for( s32 i=0; i<model_num; ++i )
  {
    nw::gfx::ResModel res = nw_res_graphics_file->GetModels(i);
    nw::gfx::SceneObject* scene_object = nw::gfx::SceneBuilder()
        .Resource(res)
        .IsFixedSizeMemory(true)
        .MaxChildren(l_description.model_max_children)
        .MaxCallbacks(0)
        .BufferOption(nw::gfx::Model::FLAG_BUFFER_NOT_USE)
        .SharedMaterialModel(NULL)
        .SharedMeshNodeVisibiltiyModel(NULL)
        .MaxAnimObjectsPerGroup(0)  // @note パーティクルのマテリアルアニメーションをやるならここを設定できるようにしなければならない。
                                    // @note アニメーションがあるとき1でいいか？それともAnimGroupそれぞれに複数持つようなことはあるのか？
        .ParticleSetMarginCount(0)
        .IsAnimationEnabled(false)  // @note パーティクルのマテリアルアニメーションをやるならここを設定できるようにしなければならない。
        .CreateObject(heap_allocator, device_allocator);
    
    nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
    b32 push_back = m_scene_node_array->PushBack(scene_node);
    GFL_ASSERT_MSG( push_back, "gfl::grp::g3d::Particle  モデルが確保しておいたシーンノードの要素数%dを越えました。\n", total_num );  //@check 出ないだけ
    
    nw::gfx::ParticleModel* model = nw::ut::DynamicCast<nw::gfx::ParticleModel*>(scene_object);
    push_back = m_model_array->PushBack(model);
    GFL_ASSERT_MSG( push_back, "gfl::grp::g3d::Particle  モデルが確保しておいたモデルの要素数%dを越えました。\n", model_num );  //@check 出ないだけ
  }

  // エミッター生成
  for( s32 i=0; i<emitter_num; ++i )
  {
    nw::gfx::ResParticleEmitter res = nw_res_graphics_file->GetEmitters(i);
    nw::gfx::SceneObject* scene_object = nw::gfx::SceneBuilder()
        .Resource(res)
        .IsFixedSizeMemory(true)
        .MaxChildren(l_description.emitter_max_children)
        .MaxCallbacks(0)
        .BufferOption(nw::gfx::Model::FLAG_BUFFER_NOT_USE)
        .SharedMaterialModel(NULL)
        .SharedMeshNodeVisibiltiyModel(NULL)
        .MaxAnimObjectsPerGroup(0)  // @note パーティクルのマテリアルアニメーションをする場合でもここは0でいいと思うがどうだろう。
        .ParticleSetMarginCount(0)
        .IsAnimationEnabled(false)  // @note パーティクルのマテリアルアニメーションをする場合でもここはfalseでいいと思うがどうだろう。
        .CreateObject(heap_allocator, device_allocator);

    nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
    b32 push_back = m_scene_node_array->PushBack(scene_node);
    GFL_ASSERT_MSG( push_back, "gfl::grp::g3d::Particle  エミッターが確保しておいたシーンノードの要素数%dを越えました。\n", total_num );  //@check 出ないだけ

    nw::gfx::ParticleEmitter* emitter = nw::ut::DynamicCast<nw::gfx::ParticleEmitter*>(scene_object);
    push_back = m_emitter_array->PushBack(emitter);
    GFL_ASSERT_MSG( push_back, "gfl::grp::g3d::Particle  エミッターが確保しておいたエミッターの要素数%dを越えました。\n", emitter_num );  //@check 出ないだけ
  }

  // 階層再構築
  nw::gfx::SceneHelper::ResolveReference(*m_scene_node_array);

  // トップノード生成
  nw::gfx::TransformNode* transform_node = nw::gfx::TransformNode::DynamicBuilder()
    .IsFixedSizeMemory(true)
    .MaxChildren(l_description.max_children)  // @note m_scene_node_arrayのうちで子になるノードの個数を数えておけば、ちょうどピッタリ割り当てることができる。
    .MaxCallbacks(0)
    .Create(device_allocator);

  // トップノードにアタッチ
  {
    NW_FOREACH(nw::gfx::SceneNode* node, *m_scene_node_array)  // @note モデルとエミッターを別々に覚えておくようにしたい。モデル、エミッターどちらもあらかじめDynamicCastしておきたい。
    {
      if(node->GetParent() == NULL)
      {
        b32 attach_child = transform_node->AttachChild(node);
        GFL_ASSERT_MSG( attach_child, "パーティクルのトップノードに子を追加できませんでした。確保しておいた子の個数%d\n", l_description.max_children );  //@check 出ないだけ
      }
    }
  }

  // 親クラスであるSceneNode, Objectの設定
  {
    nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(transform_node);
    SetNwSceneNode(scene_node);
    SetResourceIdAndIndexInResource(resource->GetId(), OBJECT_RESOURCE_ID_PARTICLE);
  }
  
#if GFL_DEBUG      
  if ( model_num )
  {
    nw::gfx::ResModel res = nw_res_graphics_file->GetModels(0);
    this->SetModelName( res.GetName() );//一つ目のモデル名だけでもデバッグ用途に使う。内よりかはマシだろう。
    
    nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
    for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleModel* model = (*i);
      model->SetUserParameter( reinterpret_cast<u32>(this) );
    }
    
  }
#endif
  

  //m_tree_flag = true;
}

#if GFL_DEBUG    
void Particle::SetModelName( const char* pName )
{
  memset( m_ModelName, 0, sizeof(m_ModelName) );
  s32   len = strlen( pName );
  
  if ( len >= sizeof(m_ModelName) )
    len = sizeof(m_ModelName) - 1;//末端null文字キープ
  
  memcpy( m_ModelName, pName, len );
}
#endif

void Particle::Destroy(void)
{
 #if GFL_DEBUG
  if ( s_SafeDeleteCheckEnable )
  {
    u32   cnt = gfl::grp::GraphicsSystem::GetDrawFrameCount();

    if ( ( m_DrawFrameCount + 2 ) > cnt )
    {
      if( s_SafeDeleteCheckAssert )
      {
        GFL_ASSERT_MSG( 0, "パーティクル名%sは描画中に解放されました。 : 最後にコマンドが積まれたフレーム = %d : 破棄フレーム = %d\n\n", m_ModelName, m_DrawFrameCount, cnt );
      }
      else
      {
        GFL_PRINT( "パーティクル名%sは描画中に解放されました。 : 最後にコマンドが積まれたフレーム = %d : 破棄フレーム = %d\n\n", m_ModelName, m_DrawFrameCount, cnt );
      }
    }
  }
#endif
  
  // 独自処理 ここから
  if( m_emitter_array )
  {
    GFL_DELETE m_emitter_array;  // @note m_emitter_arrayに登録されている各シーンノードの解放まで行われていないか確認して！→解放していない
    m_emitter_array = NULL;
  }
  if( m_model_array )
  {
    GFL_DELETE m_model_array;  // @note m_model_arrayに登録されている各シーンノードの解放まで行われていないか確認して！→解放していない
    m_model_array = NULL;
  }
  if( m_scene_node_array )
  {
    GFL_DELETE m_scene_node_array;  // @note m_scene_node_arrayに登録されている各シーンノードの解放まで行われていないか確認して！→解放していない
    m_scene_node_array = NULL;
  }
  //m_tree_flag = true;
  // 独自処理 ここまで
 
  // トップノード破棄
  //SceneNode::Destroy();  // 子がいるので、単品削除ではダメ。
  nw::gfx::SceneNode* scene_node = GetNwSceneNode();
  nw::gfx::SafeDestroyBranch( scene_node );  // 子も一緒に削除。
  // この時点では、ローカル変数scene_nodeにNULLが代入されているだけで、基底クラスのm_nw_scene_nodeにはNULLが代入されていない。
  SetNwSceneNode(NULL);  // 基底クラスのm_nw_scene_nodeにNULLを代入しておく。

  m_is_setup = false;
  
  SceneNode::Destroy();  // もうm_nw_scene_nodeがNULLになっているので、ここで親のDestoryを呼んでm_nw_scene_node破棄以外の処理をさせる。
}

//-----------------------------------------------------------------------------
/*
 *  @brief         平行移動
 */
//-----------------------------------------------------------------------------
void Particle::SetTranslate(f32 x, f32 y, f32 z)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetTranslate( x, y, z );

  {
    nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note 派生クラスでアクセスして、個々の派生クラスで処理するのではなく、SceneNodeで処理したい。
/*
    // 代入前後確認してみる
    GFL_PRINT(
        "[ConstraintNwCalculatedTransform]\n"
        "(%f, %f, %f, %f)\n"
        "(%f, %f, %f, %f)\n"
        "(%f, %f, %f, %f)\n",
        constraint_calculated_transform.TransformMatrix().f._00, constraint_calculated_transform.TransformMatrix().f._01,
            constraint_calculated_transform.TransformMatrix().f._02, constraint_calculated_transform.TransformMatrix().f._03,
        constraint_calculated_transform.TransformMatrix().f._10, constraint_calculated_transform.TransformMatrix().f._11,
            constraint_calculated_transform.TransformMatrix().f._12, constraint_calculated_transform.TransformMatrix().f._13,
        constraint_calculated_transform.TransformMatrix().f._20, constraint_calculated_transform.TransformMatrix().f._21,
            constraint_calculated_transform.TransformMatrix().f._22, constraint_calculated_transform.TransformMatrix().f._23
    );
*/
    constraint_calculated_transform.SetTranslate( x, y, z );
/*
    // 代入前後確認してみる
    GFL_PRINT(
        "[ConstraintNwCalculatedTransform]\n"
        "(%f, %f, %f, %f)\n"
        "(%f, %f, %f, %f)\n"
        "(%f, %f, %f, %f)\n",
        constraint_calculated_transform.TransformMatrix().f._00, constraint_calculated_transform.TransformMatrix().f._01,
            constraint_calculated_transform.TransformMatrix().f._02, constraint_calculated_transform.TransformMatrix().f._03,
        constraint_calculated_transform.TransformMatrix().f._10, constraint_calculated_transform.TransformMatrix().f._11,
            constraint_calculated_transform.TransformMatrix().f._12, constraint_calculated_transform.TransformMatrix().f._13,
        constraint_calculated_transform.TransformMatrix().f._20, constraint_calculated_transform.TransformMatrix().f._21,
            constraint_calculated_transform.TransformMatrix().f._22, constraint_calculated_transform.TransformMatrix().f._23
    );
*/
  }
}
void Particle::SetTranslate(const gfl::math::VEC3& t)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetTranslate( t );

  {
    nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note 派生クラスでアクセスして、個々の派生クラスで処理するのではなく、SceneNodeで処理したい。
    constraint_calculated_transform.SetTranslate( t );
  }
}
void Particle::GetTranslate(gfl::math::VEC3* t) const
{
  {
    const nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note 派生クラスでアクセスして、個々の派生クラスで処理するのではなく、SceneNodeで処理したい。
    constraint_calculated_transform.GetTranslate( t );
    return;
  }

/*
以下はunreachable codeなのでコメントアウト
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().GetTranslate( t );
*/
}
gfl::math::VEC3 Particle::GetTranslate(void) const
{
  {
    const nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note 派生クラスでアクセスして、個々の派生クラスで処理するのではなく、SceneNodeで処理したい。
    return constraint_calculated_transform.GetTranslate();
  }

/*
以下はunreachable codeなのでコメントアウト
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  return transform_node->Transform().GetTranslate();
*/
}

//-----------------------------------------------------------------------------
/*
 *  @brief         回転
 *                 単位はラジアン。
 */
//-----------------------------------------------------------------------------
void Particle::SetRotateXYZ(f32 x, f32 y, f32 z)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetRotateXYZ( x, y, z );

  {
    nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note 派生クラスでアクセスして、個々の派生クラスで処理するのではなく、SceneNodeで処理したい。
    constraint_calculated_transform.SetRotateXYZ( x, y, z );
  }
}
void Particle::SetRotateXYZ(const gfl::math::VEC3& t)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetRotateXYZ( t.x, t.y, t.z );

  {
    nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note 派生クラスでアクセスして、個々の派生クラスで処理するのではなく、SceneNodeで処理したい。
    constraint_calculated_transform.SetRotateXYZ( t.x, t.y, t.z );
  }
}
void Particle::SetRotateMatrix(
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

  {
    nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note 派生クラスでアクセスして、個々の派生クラスで処理するのではなく、SceneNodeで処理したい。
    constraint_calculated_transform.SetRotateMatrix(
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
}

//-----------------------------------------------------------------------------
/*
 *  @brief         スケール
 */
//-----------------------------------------------------------------------------
void Particle::SetScale(f32 x, f32 y, f32 z)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetScale( x, y, z );

  {
    nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note 派生クラスでアクセスして、個々の派生クラスで処理するのではなく、SceneNodeで処理したい。
    constraint_calculated_transform.SetScale( x, y, z );
  } 
}
void Particle::SetScale(const gfl::math::VEC3& t)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetScale( t );

  {
    nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note 派生クラスでアクセスして、個々の派生クラスで処理するのではなく、SceneNodeで処理したい。
    constraint_calculated_transform.SetScale( t );
  } 
}
void Particle::GetScale(gfl::math::VEC3* t) const
{
  {
    const nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note 派生クラスでアクセスして、個々の派生クラスで処理するのではなく、SceneNodeで処理したい。
    *t = constraint_calculated_transform.Scale();
    return;
  } 

/*
以下はunreachable codeなのでコメントアウト
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  *t = transform_node->Transform().Scale();
*/
}
gfl::math::VEC3 Particle::GetScale(void) const
{
  {
    const nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note 派生クラスでアクセスして、個々の派生クラスで処理するのではなく、SceneNodeで処理したい。
    gfl::math::VEC3 s = constraint_calculated_transform.Scale();
    return s;
  } 

/*
以下はunreachable codeなのでコメントアウト
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  gfl::math::VEC3 t = transform_node->Transform().Scale();
  return t;
*/
}

//-----------------------------------------------------------------------------
/*
 *  @brief         描画順を設定する
 *                 プログラマだけが設定できるモデルのレイヤーIDをパーティクルモデルに設定する
 *                 最初に描画 0 <= u8 <= 127 最後に描画
 *                 最優先される描画順 ModelLayerId > MaterialTranslucencyKind > その他 最劣後される描画順
 *
 *                 **** ModelとParticleにあり全部を一列に並べた統一番号です。デフォルト0000000です。 ****
 *
 *  ## パーティクルを構成する全てのものに対して同じ値を設定します ##
 *
 *  @param[in]    model_layer_id    モデルのレイヤーID
 */
//-----------------------------------------------------------------------------
void Particle::SetModelLayerId(u8 model_layer_id)
{
  // u8をずらして使う。最後の1ビットにはModel0、Particle1のデフォルト値が入れてある。
  GFL_ASSERT(model_layer_id <= 127);  //@check 描画順が狂うだけ

  nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
  for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
       i != end;
       ++i )
  {
    nw::gfx::ParticleModel* model = (*i);
    model->SetLayerId((model_layer_id << 1)); 
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         描画順を取得する
 *                 プログラマだけが設定できるモデルのレイヤーIDをパーティクルから取得する
 *                 最初に描画 0 <= u8 <= 127 最後に描画
 *                 最優先される描画順 ModelLayerId > MaterialTranslucencyKind > その他 最劣後される描画順
 *
 *                 **** ModelとParticleにあり全部を一列に並べた統一番号です。デフォルト0000000です。 ****
 *
 *  ## パーティクルを構成する全てのものの中で先頭のものの値を取得します(先頭と先頭以外で異なる値が設定されているとき困ったことになるかもしれません) ##
 *
 *  @retval       u8    モデルのレイヤーID
 */
//-----------------------------------------------------------------------------
u8 Particle::GetModelLayerId(void) const
{
  // u8をずらして使う。最後の1ビットにはModel0、Particle1のデフォルト値が入れてある。
  GFL_ASSERT_MSG(GetModelNum()>0, "ParticleModelNum=0\n");  //@check モデルがないなら0を返すだけ

  u8 model_layer_id = 0;
  nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
  for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
       i != end;
       ++i )
  {
    nw::gfx::ParticleModel* model = (*i);
    model_layer_id = model->GetLayerId();
    break;  // 先頭のものの値を取得して終了
  }
  return (model_layer_id >> 1);
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
void Particle::SetCalculateFlag(b32 flag)
{
  flag;
}
b32  Particle::GetCalculateFlag(void) const
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
void Particle::SetVisible(b32 visible)
{
  NW_FOREACH(nw::gfx::SceneNode* node, *m_scene_node_array)  // @note モデルとエミッターを別々に覚えておくようにしたい。モデル、エミッターどちらもあらかじめDynamicCastしておきたい。
  {
    nw::gfx::ParticleModel* particle_model = nw::ut::DynamicCast<nw::gfx::ParticleModel*>(node);
    if( particle_model )
    {
      particle_model->SetVisible(visible);
    }
  }
}
b32  Particle::IsVisible(void) const
{
  b32 ret = false;
  NW_FOREACH(nw::gfx::SceneNode* node, *m_scene_node_array)  // @note モデルとエミッターを別々に覚えておくようにしたい。モデル、エミッターどちらもあらかじめDynamicCastしておきたい。
  {
    nw::gfx::ParticleModel* particle_model = nw::ut::DynamicCast<nw::gfx::ParticleModel*>(node);
    if( particle_model )
    {
      ret = particle_model->IsVisible();  // 全てのパーティクルモデルに同じ設定をしてあるので、1つだけ確認すれば十分のはず。
      break;
    }
  }
  return ret;
}

#if 0
//-----------------------------------------------------------------------------
/*
 *  @brief         シーンツリーフラグ
 *                 trueのときSceneにAddされていたらシーンツリーに入れられる、falseのときSceneにAddされていてもシーンツリーから外される
 *                 これがtrueであることと、ユーザがSceneにAddしたことは、同じではありません。
 */
//-----------------------------------------------------------------------------
void Particle::SetTreeFlag(b32 flag)
{
  m_tree_flag = flag;
}
b32  Particle::GetTreeFlag(void) const
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
Particle::CalculateState Particle::GetCalculateState(void) const
{
  return CALCULATE_STATE_NO;
}
#endif

//-----------------------------------------------------------------------------
/*
 *  @brief         描画されるかどうか
 */
//-----------------------------------------------------------------------------
Particle::DrawState Particle::GetDrawState(void) const
{
  if( IsVisible() && GetTreeFlag() ) return DRAW_STATE_OK;
  return DRAW_STATE_NO;
}

//-----------------------------------------------------------------------------
/*
 *  @brief  更新フレームを設定する
 *
 *  ## パーティクルを構成する全てのものに対して同じ値を設定します ##
 * 
 *  0.0fで停止、1.0fで等倍、2.0fで2倍速
 *  パーティクルは逆再生はできないので、-1.0fなどの負の値は設定できません。
 */
//-----------------------------------------------------------------------------
void Particle::SetStepFrame(const f32 step_frame)
{
  f32 l_step_frame = step_frame;
  if( l_step_frame < 0.0f )
  {
/*
パーティクルのstep_frameにマイナスを渡すと、
しばらく固まって動かなくなった後で、
gfx_ParticleSet.cpp
SetDefaultValues
        NN_WARNING(limitDefault > -16777216, "limitDefault is overflow");
というwarningが出て、動くようになる。
*/
    GFL_ASSERT_MSG( 0, "パーティクルのステップフレームに負の値%fは設定できません。\n", step_frame );
    l_step_frame = 0.0f;
  }

#if 0
  //再帰は止めた。
  nw::gfx::SceneNode* scene_node = GetNwSceneNode();
  SetStepFrameRecursively(scene_node, l_step_frame
#if GFL_DEBUG
      , 0
#endif 
  );
#endif

  {
    //nw::ut::MoveArray<nw::gfx::ParticleModel*>::iterator end = m_model_array->end();
    nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
    //for( nw::ut::MoveArray<nw::gfx::ParticleModel*>::iterator i = m_model_array->begin();
    for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
         i != end;
         ++i )
    {
      (*i)->ParticleAnimFrameController().SetStepFrame(l_step_frame);  // i->ダメ、i.ダメ、(*i).ダメ
    }
  }
  {
    //nw::ut::MoveArray<nw::gfx::ParticleEmitter*>::iterator end = m_emitter_array->end();
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    //for( nw::ut::MoveArray<nw::gfx::ParticleEmitter*>::iterator i = m_emitter_array->begin();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      (*i)->ParticleAnimFrameController().SetStepFrame(l_step_frame);
    }
  }
}
#if 0
  //再帰は止めた。
void Particle::SetStepFrameRecursively(
    nw::gfx::SceneNode*   nw_scene_node,
    const f32             step_frame
#if GFL_DEBUG
    , const s32           recursive_count 
#endif
)
{
#if GFL_DEBUG
  if( recursive_count >= GFL_GRP_G3D_PARTICLE_RECURSIVE_COUNT_MAX )
  {
    GFL_PRINT("警告: 再帰呼び出しの回数が%d回になりました。\n", recursive_count);
  }
#endif 

  if(nw::ut::IsTypeOf<nw::gfx::ParticleModel>(nw_scene_node))
  {
    nw::gfx::ParticleModel* particle_model =
        nw::ut::DynamicCast<nw::gfx::ParticleModel*>(nw_scene_node);
    particle_model->ParticleAnimFrameController().SetStepFrame(step_frame);
  }
  else if(nw::ut::IsTypeOf<nw::gfx::ParticleEmitter>(nw_scene_node))
  {
    nw::gfx::ParticleEmitter* particle_emitter =
        nw::ut::DynamicCast<nw::gfx::ParticleEmitter*>(nw_scene_node);
    particle_emitter->ParticleAnimFrameController().SetStepFrame(step_frame);
  }

  nw::gfx::SceneNodeChildren::iterator end = nw_scene_node->GetChildEnd();
  for(nw::gfx::SceneNodeChildren::iterator child = nw_scene_node->GetChildBegin();
      child != end;
      ++child)
  {
    SetStepFrameRecursively(*child, step_frame
#if GFL_DEBUG
        , recursive_count +1
#endif 
    );
  }
}
#endif

//-----------------------------------------------------------------------------
/*
 *  @brief  フレームを設定する
 *
 *  ## パーティクルを構成する全てのものに対して同じ値を設定します ##
 * 
 *  @param[in]     frame      フレーム
 */
//-----------------------------------------------------------------------------
void Particle::SetFrame(const f32 frame)
{
  {
    nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
    for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
         i != end;
         ++i )
    {
      (*i)->ParticleAnimFrameController().SetFrame(frame);
    }
  }
  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      (*i)->ParticleAnimFrameController().SetFrame(frame);
    }
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief  フレームを取得する
 *
 *  @param[in]     frame      フレーム
 */
//-----------------------------------------------------------------------------
f32 Particle::GetFrame( void ) const
{
  f32 frame = 0;
  {
    nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
    for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
         i != end;
         ++i )
    {
      if(frame < (*i)->ParticleAnimFrameController().GetFrame() )
      {
        frame = (*i)->ParticleAnimFrameController().GetFrame();
      }
    }
  }
  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      if( frame < (*i)->ParticleAnimFrameController().GetFrame() )
      {
        frame = (*i)->ParticleAnimFrameController().GetFrame();
      }
    }
  }
  
  return frame;
}

//-----------------------------------------------------------------------------
/*
 *  @brief  リセットを行い、初期状態に戻す
 *          GPU処理中はコールできません。
 *          次のどちらかを満たせばGPU処理中ではありません。
 *          (1) シーン登録を解除している。(コマンドバッファがダブルのときはシーン登録を解除してから1フレーム以上経過している)
 *          (2) 終了状態である。(コマンドバッファがダブルのときは終了状態になってから1フレーム以上経過している)
 */
//-----------------------------------------------------------------------------
void Particle::ResetEmitter(void)
{
  // @note 上記注意の(1)、(2)を満たしていないときはASSERTで止めたい。
  // @note エミッターのリセットが安心か調べる

  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      (*i)->Reset();
    }
  }

}

//-----------------------------------------------------------------------------
/*
 *  @brief  リセットを行い、初期状態に戻す
 *          GPU処理中はコールできません。
 *          次のどちらかを満たせばGPU処理中ではありません。
 *          (1) シーン登録を解除している。(コマンドバッファがダブルのときはシーン登録を解除してから1フレーム以上経過している)
 *          (2) 終了状態である。(コマンドバッファがダブルのときは終了状態になってから1フレーム以上経過している)
 */
//-----------------------------------------------------------------------------
void Particle::Reset(void)
{
  // @note 上記注意の(1)、(2)を満たしていないときはASSERTで止めたい。

  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      (*i)->Reset();
    }
  }
  
  {
    nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
    for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleModel* model = (*i);
      model->ForeachParticleSet(nw::gfx::ParticleSetsClear());
      model->ParticleAnimFrameController().SetFrame(0.f);
      // 処理順序の検査のためのヒント情報をリセットします
      model->ResetDebugHint();
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief   放出量を設定する
 *
 *  ## パーティクルを構成する全てのものに対して同じ値を設定します ##
 *
 *  @param[in]     ratio  放出量(0.0fで全く放出されない)
 */
//-----------------------------------------------------------------------------
void Particle::SetEmitterEmissionRatio(f32 ratio)
{
  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleEmitter* emitter = (*i);
      nw::gfx::res::ResParticleEmitterParameter res_particle_emitter_parameter = emitter->GetResParticleEmitterParameterCopy(false);  // @note EmitterとParameterのどちらも同じ設定を持っているがどっちに設定するのが正解か？またParameterのときはtrueかfalseかどっちが正解か？→サンプルにのっとるとこのままでいい。
      res_particle_emitter_parameter.SetEmissionRatio(ratio);
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief   放出量を設定する
 *
 *  @param[in]     name   エミッターの名前
 *  @param[in]     ratio  放出量(0.0fで全く放出されない)
 */
//-----------------------------------------------------------------------------
void Particle::SetEmitterEmissionRatio(const char* name, f32 ratio)
{
  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleEmitter* emitter = (*i);
      if( gfl::std::StringCmp(emitter->GetName(), name)==0 )
      {
        nw::gfx::res::ResParticleEmitterParameter res_particle_emitter_parameter = emitter->GetResParticleEmitterParameterCopy(false);  // @note EmitterとParameterのどちらも同じ設定を持っているがどっちに設定するのが正解か？またParameterのときはtrueかfalseかどっちが正解か？→サンプルにのっとるとこのままでいい。
        res_particle_emitter_parameter.SetEmissionRatio(ratio);
      }
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief   放出量を取得する
 *
 *  @param[in]     name   エミッターの名前
 *
 *  @retval        f32    放出量(0.0fで全く放出されない)
 */
//-----------------------------------------------------------------------------
f32 Particle::GetEmitterEmissionRatio(const char* name) const
{
  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleEmitter* emitter = (*i);
      if( gfl::std::StringCmp(emitter->GetName(), name)==0 )
      {
        nw::gfx::res::ResParticleEmitterParameter res_particle_emitter_parameter = emitter->GetResParticleEmitterParameterCopy(false);  // @note EmitterとParameterのどちらも同じ設定を持っているがどっちに設定するのが正解か？またParameterのときはtrueかfalseかどっちが正解か？→サンプルにのっとるとこのままでいい。
        return res_particle_emitter_parameter.GetEmissionRatio();
      }
    }
  }
  return 0.0f;
}

//-----------------------------------------------------------------------------
/*
 *  @brief   現在のパーティクルの個数を取得する
 *           パーティクルを構成する全てのもので現在存在するものの個数の和を返します
 *
 *  @retval  s32  現在のパーティクルの個数
 */
//-----------------------------------------------------------------------------
s32  Particle::GetParticleCount(void) const
{
  s32 count = 0;

  {
    nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
    for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleModel* model = (*i);

      u32 sets_num = model->GetParticleSetsCount();
      for(u32 j=0; j<sets_num; ++j)
      {
        nw::gfx::ParticleSet* particle_set = model->GetParticleSets(j);
        count += particle_set->GetParticleCollection()->GetCount();
      }
    }
  }

  return count;
}

//-----------------------------------------------------------------------------
/*
 *  @brief   終了状態かどうかを取得する
 *           パーティクルを構成する全てのものが終了状態であればtrueを返します
 *           1つでも終了状態でなければfalseを返します
 *
 *  @retval  b32  終了状態のときtrue
 */
//-----------------------------------------------------------------------------
b32 Particle::IsDone(void) const
{
  {
    nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
    for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleModel* model = (*i);
      if( model->HasParticle() )
      {
        return false;
      }
    }
  }
  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleEmitter* emitter = (*i);
      if( emitter->IsAlive() )
      {
        return false;
      }
    }
  }

  // ここまで来たら終了状態
  return true;
}

//-----------------------------------------------------------------------------
/*
 *  @brief   無限放出を設定する
 *           エミッターからの放出に終わりがあるか否かの設定です。1個1個の粒子の寿命の設定ではありません。
 * 
 *  ## パーティクルを構成する全てのものに対して同じ値を設定します ##
 *
 *  @param[in]  infinity  無限放出にするときtrue
 */
//-----------------------------------------------------------------------------
void Particle::SetEmitterEmissionSpanInfinity(b32 infinity)
{
  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleEmitter* emitter = (*i);
      nw::gfx::res::ResParticleEmitterParameter res_particle_emitter_parameter = emitter->GetResParticleEmitterParameterCopy(false);  // @note EmitterとParameterのどちらも同じ設定を持っているがどっちに設定するのが正解か？またParameterのときはtrueかfalseかどっちが正解か？→サンプルにのっとるとこのままでいい。
      res_particle_emitter_parameter.SetEmissionSpanInfinity(infinity);
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief   無限放出かどうかを取得する
 *           パーティクルを構成する全てのものが無限放出でなければfalseを返します
 *           1つでも無限放出であればtrueを返します
 *
 *  @retval  b32  無限放出のときtrue
 */
//-----------------------------------------------------------------------------
b32 Particle::IsEmitterEmissionSpanInfinity(void) const
{
  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleEmitter* emitter = (*i);
      const nw::gfx::res::ResParticleEmitterParameter res_particle_emitter_parameter = emitter->GetResParticleEmitterParameterCopy(false);  // @note EmitterとParameterのどちらも同じ設定を持っているがどっちに設定するのが正解か？またParameterのときはtrueかfalseかどっちが正解か？→サンプルにのっとるとこのままでいい。
      if( res_particle_emitter_parameter.GetEmissionSpanInfinity() )
      {
        return true;
      }
    }
  }

  // ここまで来たら無限放出ではない
  return false;
}


//-----------------------------------------------------------------------------
/*
 *  @brief   セットアップ
 *
 *  @param[in] particle_context パーティクルコンテキスト
 */
//-----------------------------------------------------------------------------
void Particle::Setup(nw::gfx::ParticleContext* particle_context)
{
  if( !m_is_setup )
  {
    // パーティクルの参照解決・初期化
    nw::gfx::ParticleUtil::SetupParticleObject(m_scene_node_array, particle_context);
    // @note nw::gfx::ParticleUtil::SetupParticleObjectにてparticle_contextは乱数を取得することにしか使われていない。
    //       よって、scene_calculate_groupが変わりparticle_contextが変わったとしても、
    //       nw::gfx::ParticleUtil::SetupParticleObjectを再度やり直す必要はないと思われるので、
    //       m_is_setupフラグを立てて1回しかnw::gfx::ParticleUtil::SetupParticleObjectが行われないようにしておく。
    //       この方針を変更しなくても済むようnw::gfx::ParticleUtil::SetupParticleObjectの中身が書き換わらないことを願う。
    //       そもそも、nw::gfx::ParticleUtil::SetupParticleObjectは何回も呼び出していいものなのだろうか？
    //       何回も呼び出していいものなら、この方針も変更できて安心なのだが・・・
    m_is_setup = true;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief   指定数だけフレームを回す
 *
 *  ## パーティクルを構成する全てのものに対して同じ値を設定します ##
 *
 *  @param[in] times            フレームを回す回数
 *  @param[in] particle_context パーティクルコンテキスト
 */
//-----------------------------------------------------------------------------
void Particle::AddTimes(u32 times, nw::gfx::ParticleContext* particle_context)
{
  b32 particle_enable_swap_buffer = true;
   
  for(u32 k=0; k<times; ++k)
  {
    {
      nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
      for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
           i != end;
           ++i )
      {
        nw::gfx::ParticleModel* model = (*i);
        model->UpdateParticleFrame();
      }
    }
    {
      nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
      for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
           i != end;
           ++i )
      {
        nw::gfx::ParticleEmitter* emitter = (*i);
        emitter->UpdateParticleFrame();
        emitter->Emission(particle_context);
      }
    }

    {
      nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
      for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
           i != end;
           ++i )
      {
        nw::gfx::ParticleModel* model = (*i);
 
        u32 sets_num = model->GetParticleSetsCount();
        for(u32 j=0; j<sets_num; ++j)
        {
          nw::gfx::ParticleSet* particle_set = model->GetParticleSets(j);
          particle_set->UpdateParticles(particle_context, particle_enable_swap_buffer);
        }
      }
    }

    particle_enable_swap_buffer = false;  // @note 下記説明から判断すると、コマンドバッファがダブルでないときはtrueにしておくべきか。→ダブルだからfalseでいい。
   
/*
パーティクルの更新処理を行います。

enableSwapBufferをfalseにすると、内部でバッファのスワップを行いません。通常はtrueにしてください。アプリケーション側でコマンドバッファをダブルにしていて、同一フレームを描画中に複数回UpdateNodeを呼ぶ場合（つまりコマ落ち処理）は、同一フレームの2回目以降の呼び出しではfalseにしてください。

詳しくはParticleDropFrameDemoを参照してください。
*/

/*
VBOダブルバッファ
パーティクルは計算処理、描画処理を並列化できるように内部でVBOを二重化しています。
バッファの切り替えは、ParticleSceneUpdater::UpdateNodeから呼ばれるParticleSet::UpdateParticles内で自動的に行われます。
*/

    // NintendoWare/CTR/sources/libraries/gfx/gfx_ParticleSceneUpdater.cpp 
    // ParticleSceneUpdater::UpdateNode
    // も参考になる。
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief   パーティクルモデルの個数を得る
 *
 *  @retval    パーティクルモデルの個数
 */
//-----------------------------------------------------------------------------
s32 Particle::GetModelNum(void) const
{
  return m_model_array->size();  // sizeは現在の要素数、capacityは保持できる要素の個数、即ちsize<=capacity
}
//-----------------------------------------------------------------------------
/*
 *  @brief   パーティクルモデルを得る
 *
 *  @param[in] index  インデックス
 *  @retval    パーティクルモデル
 */
//-----------------------------------------------------------------------------
nw::gfx::ParticleModel* Particle::GetModel(s32 index) const
{
  nw::gfx::ParticleModel* ret = NULL;
  if( 0<=index && index<GetModelNum() )
  {
    ret = (*m_model_array)[index];
  }
  else
  {
    GFL_ASSERT_MSG(0, "index=%d, size=%d\n", index, GetModelNum());
  }
  return ret;
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl
