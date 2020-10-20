//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dFog.cpp
 *	@brief  3Dフォグ
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
#include <grp/g3d/gfl_G3dSystem.h>

#include <grp/g3d/gfl_G3dFog.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス定義
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3Dフォグクラス
//=====================================

//-------------------------------------
///	フォグテーブル構造体
//=====================================
void Fog::InitializeFogTableForDefault(
    FogTable*    fog_table
)
{
  for( u32 i=0; i<FOG_TABLE_VALUE_NUM; ++i )
  {
    fog_table->value[i] = 0.0f;
  }
}
 
void Fog::SetupFogTableUsingLookupTable(
    FogTable*    fog_table, 
    Resource*    lookup_table_resource,
    s32          lookup_table_set_index_in_resource,
    s32          lookup_table_index_in_lookup_table_set
)
{
  // 定義
  const u32 FIX12_ONE_U32_u32 = 4096;
  const f32 FIX12_ONE_U32_f32 = 4096.0f;
  const u32 HALF_TABLE_SIZE = nw::gfx::FOG_TABLE_SIZE / 2;
  STATIC_ASSERT(HALF_TABLE_SIZE == 128);  // 128じゃなかったら以下のソースが成り立たないのでコンパイル時にチェック
  STATIC_ASSERT(FOG_TABLE_VALUE_NUM == 128);

  // 変数
  u32 lookup_table_data_value[HALF_TABLE_SIZE];
  u32 index;

  // 参照テーブルを読む
  {
    nw::gfx::ResGraphicsFile* nw_res_graphics_file = lookup_table_resource->GetNwResGraphicsFile();
    nw::gfx::res::ResLookupTableSet nw_res_lookup_table_set = nw_res_graphics_file->GetLutSets(lookup_table_set_index_in_resource);
    nw::gfx::res::ResImageLookupTable nw_res_image_lookup_table = nw_res_lookup_table_set.GetSamplers(lookup_table_index_in_lookup_table_set);

    GFL_ASSERT(nw_res_image_lookup_table.IsAbs());  // 絶対値化フラグが立っているときのアクセス方法しか記述していない。  //@check どんな見た目のフォグになるかは分からないが続けられる。

    const s32 LOOKUP_TABLE_DATA_NUM = 260;
    s32 lookup_table_data_index = 0;
    index = 0;
    for( s32 i=0; i<LOOKUP_TABLE_DATA_NUM; ++i )
    {
      if( i==1 )
      {
        // ヘッダ
      }
      else if ( i>=257 )
      {
        // フッター
      }
      else
      {
        // 有効なデータ
        // テールは256段階で作れれている。
        // フォグテーブルは128段階でよいので、1/2にする
        b32 b_use = false;
        if( lookup_table_data_index % 2 == 0 )  // 0, 2, 4, 6
        {
          b_use = true;
        }

        if( b_use )
        {
          u32 value = nw_res_image_lookup_table.GetCommandCache(i);
          u32 value_up   = ( value & 0xFFF000 ) >> 12;
          u32 value_down = ( value & 0xFFF );
          if( value_down > FIX12_ONE_U32_u32 * 10 )
          {
            GFL_ASSERT_MSG(0, "フォグの参照テーブルの値%dが異常に大きいです。\n", value_down);
            value_down = 0;
          }
          if( value_down > FIX12_ONE_U32_u32 ) value_down = FIX12_ONE_U32_u32;
          if(index < FOG_TABLE_VALUE_NUM)
          {
            fog_table->value[index] = static_cast<f32>(value_down / FIX12_ONE_U32_f32);
          }
          ++index;
        }
        ++lookup_table_data_index;

#if GFL_DEBUG
        static bool s_flag = false;
        
        if(s_flag){
          u32 value = nw_res_image_lookup_table.GetCommandCache(i);
          u32 value_up   = ( value & 0xFFF000 ) >> 12;
          u32 value_down = ( value & 0xFFF ); 
          GFL_PRINT("fog lut GetCommandCache[%4d] %d = %f | %f\n", i, value, static_cast<f32>(value_up / FIX12_ONE_U32_f32), static_cast<f32>(value_down / FIX12_ONE_U32_f32));
        }
#endif // GFL_DEBUG
      }


    }
  }
}


//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
 *                 Createは呼ばれません。
 */
//-----------------------------------------------------------------------------
Fog::Fog(void)
  : SceneNode(),
    m_enable_flag(false),
    m_enable_min_depth(DISABLE_MIN_DEPTH),
    m_enable_max_depth(DISABLE_MAX_DEPTH),
    m_type(TYPE_NONE),
    m_fog_table(NULL),
    m_lookup_table_resource(NULL),
    m_lookup_table_set_index_in_resource(LOOKUP_TABLE_INDEX_NONE),
    m_lookup_table_index_in_lookup_table_set(LOOKUP_TABLE_INDEX_NONE),
    m_intensity(1.0f)
{
  GFL_SINGLETON_INSTANCE(System)->GetFogManager()->AddObject(this);
}
//-----------------------------------------------------------------------------
/*
 *  @brief         デストラクタ
 *                 Destroyを呼びます。
 */
//-----------------------------------------------------------------------------
Fog::~Fog()
{
  Destroy();
  GFL_SINGLETON_INSTANCE(System)->GetFogManager()->RemoveObject(this);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         生成
 *
 *  @param[in]     allocator          アロケータ
 *  @param[in]     device_allocator   デバイスアロケータ
 *  @param[in]     resource           リソース
 *  @param[in]     index_in_resource  リソース内におけるインデックス
 */
//-----------------------------------------------------------------------------
void Fog::Create(
    gfl::heap::NwAllocator*           allocator,
    gfl::heap::NwAllocator*           device_allocator,
    Resource*                         resource,
    s32                               index_in_resource
)
{
  if( resource == NULL )
  {
    GFL_ASSERT_MSG( 0, "リソースが不正のためシーンオブジェクトが生成できませんでした。\n" );
    return;
  }

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::gfx::ResFog res = nw_res_graphics_file->GetFogs(index_in_resource);
  nw::gfx::SceneObject* scene_object = gfl::grp::g3d::Resource::CreateNwSceneObject(
      allocator,
      device_allocator,
      res,
      false,  // アニメーションなし
      nw::gfx::Model::FLAG_BUFFER_NOT_USE,
      0
  );
  GFL_ASSERT_MSG( scene_object, "index_in_resource=%dのシーンオブジェクトが生成できませんでした。\n", index_in_resource );

  nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
  SetNwSceneNode(scene_node);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);

  // 独自処理
  m_enable_flag = true;
  m_enable_min_depth = DISABLE_MIN_DEPTH;
  m_enable_max_depth = DISABLE_MAX_DEPTH;
  m_type = TYPE_NORMAL;
  m_fog_table = NULL;
  m_lookup_table_resource                  = NULL;
  m_lookup_table_set_index_in_resource     = LOOKUP_TABLE_INDEX_NONE;
  m_lookup_table_index_in_lookup_table_set = LOOKUP_TABLE_INDEX_NONE;
  m_intensity                 = 1.0f;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         参照テーブルを使用したフォグの生成
 *
 *  @param[in]     allocator          アロケータ
 *  @param[in]     device_allocator   デバイスアロケータ
 *  @param[in]     resource           リソース
 *  @param[in]     index_in_resource  リソース内におけるインデックス
 *  @param[in]     lookup_table_resource                   参照テーブルのリソース
 *  @param[in]     lookup_table_set_index_in_resource      参照テーブルセットのリソース内におけるインデックス
 *  @param[in]     lookup_table_index_in_lookup_table_set  参照テーブルの参照テーブルセット内におけるインデックス
 */
//-----------------------------------------------------------------------------
void Fog::CreateFogUsingLookupTable(
    gfl::heap::NwAllocator*  allocator,
    gfl::heap::NwAllocator*  device_allocator,
    Resource*                resource,
    s32                      index_in_resource,
    Resource*                lookup_table_resource,
    s32                      lookup_table_set_index_in_resource,
    s32                      lookup_table_index_in_lookup_table_set
)
{
  if( resource == NULL )
  {
    GFL_ASSERT_MSG( 0, "リソースが不正のためシーンオブジェクトが生成できませんでした。\n" );
    return;
  }

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::gfx::ResFog res = nw_res_graphics_file->GetFogs(index_in_resource);
  nw::gfx::SceneObject* scene_object = gfl::grp::g3d::Resource::CreateNwSceneObject(
      allocator,
      device_allocator,
      res,
      false,  // アニメーションなし
      nw::gfx::Model::FLAG_BUFFER_NOT_USE,
      0
  );
  GFL_ASSERT_MSG( scene_object, "index_in_resource=%dのシーンオブジェクトが生成できませんでした。\n", index_in_resource );

  nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
  SetNwSceneNode(scene_node);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);

  // 独自処理
  m_enable_flag = true;
  m_enable_min_depth = DISABLE_MIN_DEPTH;
  m_enable_max_depth = DISABLE_MAX_DEPTH;
  m_type = TYPE_USE_LOOKUP_TABLE_RESOURCE;
  m_lookup_table_resource                  = lookup_table_resource;
  m_lookup_table_set_index_in_resource     = lookup_table_set_index_in_resource;
  m_lookup_table_index_in_lookup_table_set = lookup_table_index_in_lookup_table_set;
  m_intensity                 = 1.0f;

  // 参照テーブルからフォグテーブルを作成
  m_fog_table = GFL_NEW(allocator->GetHeapBase()) FogTable;
  SetupFogTableUsingLookupTable( m_fog_table, m_lookup_table_resource, m_lookup_table_set_index_in_resource, m_lookup_table_index_in_lookup_table_set );
}

//-----------------------------------------------------------------------------
/*
 *  @brief         フォグテーブルを使用したフォグの生成
 *
 *  @param[in]     allocator          アロケータ
 *  @param[in]     device_allocator   デバイスアロケータ
 *  @param[in]     resource           リソース
 *  @param[in]     index_in_resource  リソース内におけるインデックス
 *  @param[in]     fog_table          フォグテーブル(内容をコピーしたものを保持するので、この関数から戻ってきたら、呼び出し元はfog_tableを削除してもよい)
 */
//-----------------------------------------------------------------------------
void Fog::CreateFogUsingFogTable(
    gfl::heap::NwAllocator*  allocator,
    gfl::heap::NwAllocator*  device_allocator,
    Resource*                resource,
    s32                      index_in_resource,
    const FogTable&          fog_table
)
{
  if( resource == NULL )
  {
    GFL_ASSERT_MSG( 0, "リソースが不正のためシーンオブジェクトが生成できませんでした。\n" );
    return;
  }

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::gfx::ResFog res = nw_res_graphics_file->GetFogs(index_in_resource);
  nw::gfx::SceneObject* scene_object = gfl::grp::g3d::Resource::CreateNwSceneObject(
      allocator,
      device_allocator,
      res,
      false,  // アニメーションなし
      nw::gfx::Model::FLAG_BUFFER_NOT_USE,
      0
  );
  GFL_ASSERT_MSG( scene_object, "index_in_resource=%dのシーンオブジェクトが生成できませんでした。\n", index_in_resource );

  nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
  SetNwSceneNode(scene_node);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);

  // 独自処理
  m_enable_flag = true;
  m_enable_min_depth = DISABLE_MIN_DEPTH;
  m_enable_max_depth = DISABLE_MAX_DEPTH;
  m_type = TYPE_CREATE_FOG_TABLE;
  m_fog_table = GFL_NEW(allocator->GetHeapBase()) FogTable;
  *m_fog_table = fog_table;
  m_lookup_table_resource                  = NULL;
  m_lookup_table_set_index_in_resource     = LOOKUP_TABLE_INDEX_NONE;
  m_lookup_table_index_in_lookup_table_set = LOOKUP_TABLE_INDEX_NONE;
  m_intensity                 = 1.0f;
}

void Fog::CreateFogUsingFogTable(
    gfl::heap::NwAllocator*  allocator,
    gfl::heap::NwAllocator*  device_allocator,
    Resource*                resource,
    s32                      index_in_resource
)
{
  if( resource == NULL )
  {
    GFL_ASSERT_MSG( 0, "リソースが不正のためシーンオブジェクトが生成できませんでした。\n" );
    return;
  }

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::gfx::ResFog res = nw_res_graphics_file->GetFogs(index_in_resource);
  nw::gfx::SceneObject* scene_object = gfl::grp::g3d::Resource::CreateNwSceneObject(
      allocator,
      device_allocator,
      res,
      false,  // アニメーションなし
      nw::gfx::Model::FLAG_BUFFER_NOT_USE,
      0
  );
  GFL_ASSERT_MSG( scene_object, "index_in_resource=%dのシーンオブジェクトが生成できませんでした。\n", index_in_resource );

  nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
  SetNwSceneNode(scene_node);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);

  // 独自処理
  m_enable_flag = true;
  m_enable_min_depth = DISABLE_MIN_DEPTH;
  m_enable_max_depth = DISABLE_MAX_DEPTH;
  m_type = TYPE_CREATE_FOG_TABLE;
  m_fog_table = GFL_NEW(allocator->GetHeapBase()) FogTable;
  InitializeFogTableForDefault(m_fog_table); 
  m_lookup_table_resource                  = NULL;
  m_lookup_table_set_index_in_resource     = LOOKUP_TABLE_INDEX_NONE;
  m_lookup_table_index_in_lookup_table_set = LOOKUP_TABLE_INDEX_NONE;
  m_intensity                 = 1.0f;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         破棄
 *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
 */
//-----------------------------------------------------------------------------
void Fog::Destroy(void)
{
  // 独自処理
  m_enable_flag = false;
  m_enable_min_depth = DISABLE_MIN_DEPTH;
  m_enable_max_depth = DISABLE_MAX_DEPTH;
  m_type = TYPE_NONE;
  if( m_fog_table )
  {
    GFL_DELETE m_fog_table;
    m_fog_table = NULL;
  }
  m_lookup_table_resource                  = NULL;
  m_lookup_table_set_index_in_resource     = LOOKUP_TABLE_INDEX_NONE;
  m_lookup_table_index_in_lookup_table_set = LOOKUP_TABLE_INDEX_NONE;
  m_intensity                 = 1.0f;

  SceneNode::Destroy();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         カラー
 *                 u8のときは0が黒&透明、255が白&不透明
 *                 f32のときは0.0fが黒&透明、1.0fが白&不透明
 */
//-----------------------------------------------------------------------------
void Fog::SetColor(f32 r, f32 g, f32 b)
{
  nw::gfx::ResFog res_fog = GetNwResFog();
  res_fog.SetColor(r,g,b);
}
void Fog::SetColor(const gfl::grp::ColorF32& c)  // aはどう使われるか不明
{
  nw::gfx::ResFog res_fog = GetNwResFog();
  res_fog.SetColor(c);
}
void Fog::SetColor(u8 r, u8 g, u8 b)
{
  nw::gfx::ResFog res_fog = GetNwResFog();
  res_fog.SetColor(
    (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX,
    (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX,
    (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX
  );
}
void Fog::SetColor(const gfl::grp::ColorU8& c)   // aはどう使われるか不明
{
  nw::gfx::ResFog res_fog = GetNwResFog();
  res_fog.SetColor(
    (static_cast<f32>(c.r))/gfl::grp::ColorU8::ELEMENT_MAX,
    (static_cast<f32>(c.g))/gfl::grp::ColorU8::ELEMENT_MAX,
    (static_cast<f32>(c.b))/gfl::grp::ColorU8::ELEMENT_MAX,
    (static_cast<f32>(c.a))/gfl::grp::ColorU8::ELEMENT_MAX
  );
}
gfl::grp::ColorF32 Fog::GetColor(void) const    // aはどう使われるか不明
{
  gfl::grp::ColorF32 c(0.0f, 0.0f, 0.0f, 0.0f);
  nw::gfx::ResFog res_fog = GetNwResFog();
  c = res_fog.GetColor();
  return c;
}
void Fog::GetColor(gfl::grp::ColorF32* c) const  // aはどう使われるか不明
{
  nw::gfx::ResFog res_fog = GetNwResFog();
  *c = res_fog.GetColor();
}
void Fog::GetColor(gfl::grp::ColorU8*  c) const  // aはどう使われるか不明
{
  nw::gfx::ResFog res_fog = GetNwResFog();
  gfl::grp::ColorF32 c_f32 = res_fog.GetColor();
  c->Set(
    static_cast<s32>(c_f32.r*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.g*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.b*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.a*gfl::grp::ColorU8::ELEMENT_MAX)
  );
}

//-----------------------------------------------------------------------------
/*
 *  @brief         説明
 */
//-----------------------------------------------------------------------------
void     Fog::SetFunction(Function function)
{
  nw::gfx::ResFogUpdater res_fog_updater = GetNwResFogUpdater();
  res_fog_updater.SetFogUpdaterType( static_cast<nw::gfx::res::ResFogUpdater::FogUpdaterType>(function) );
}
gfl::grp::g3d::Fog::Function Fog::GetFunction(void) const
{
  nw::gfx::ResFogUpdater res_fog_updater = GetNwResFogUpdater();
  return static_cast<Function>( res_fog_updater.GetFogUpdaterType() );
}
void Fog::SetDensity(f32 value)
{
  nw::gfx::ResFogUpdater res_fog_updater = GetNwResFogUpdater();
  res_fog_updater.SetDensity(value); 
}
f32  Fog::GetDensity(void) const
{
  nw::gfx::ResFogUpdater res_fog_updater = GetNwResFogUpdater();
  return res_fog_updater.GetDensity(); 
}
void Fog::SetMinDepth(f32 min)
{
  if( m_enable_flag )
  {
    nw::gfx::ResFogUpdater res_fog_updater = GetNwResFogUpdater();
    res_fog_updater.SetMinFogDepth(min);
  }
  else
  {
    m_enable_min_depth = min;
  }
}
f32  Fog::GetMinDepth(void) const
{
  if( m_enable_flag )
  {
    nw::gfx::ResFogUpdater res_fog_updater = GetNwResFogUpdater();
    return res_fog_updater.GetMinFogDepth(); 
  }
  else
  {
    return m_enable_min_depth;
  }
}
void Fog::SetMaxDepth(f32 max)
{
  if( m_enable_flag )
  {
    nw::gfx::ResFogUpdater res_fog_updater = GetNwResFogUpdater();
    res_fog_updater.SetMaxFogDepth(max);
  }
  else
  {
    m_enable_max_depth = max;
  }
}
f32  Fog::GetMaxDepth(void) const
{
  if( m_enable_flag )
  {
    nw::gfx::ResFogUpdater res_fog_updater = GetNwResFogUpdater();
    return res_fog_updater.GetMaxFogDepth(); 
  }
  else
  {
    return m_enable_max_depth;
  }
}
void Fog::SetDepth(f32 min, f32 max)
{
  if( m_enable_flag )
  {
    nw::gfx::ResFogUpdater res_fog_updater = GetNwResFogUpdater();
    res_fog_updater.SetMinFogDepth(min);
    res_fog_updater.SetMaxFogDepth(max);
  }
  else
  {
    m_enable_min_depth = min;
    m_enable_max_depth = max;
  }
}

void Fog::SetIntensity(const f32 intensity)  // m_typeがTYPE_CREATE_FOG_TABLEかTYPE_USE_LOOKUP_TABLE_RESOURCEのときのみ有効。  // 0.0fで全くフォグが出ない、1.0fで参照テーブルの通りにフォグが出る(0.0f<= <=1.0f)
{
  GFL_ASSERT_MSG( 0.0f<=intensity&&intensity<=1.0f, "intensity=%fは範囲外です。\n", intensity);

  m_intensity = intensity;
  if( m_intensity < 0.0f )      m_intensity = 0.0f;
  else if( m_intensity > 1.0f ) m_intensity = 1.0f;

  // @note フォグの更新を変更があったときしか行わなくするなら、
  //       このm_intensityが変更されたときは忘れずにフォグの更新をするようにしておくこと。
}
f32  Fog::GetIntensity(void) const
{
  return m_intensity;
}

void Fog::SetFogTable(
    const FogTable&          fog_table  // フォグテーブル(内容をコピーしたものを保持するので、この関数から戻ってきたら、呼び出し元はfog_tableを削除してもよい)
)
{
  *m_fog_table = fog_table;  // 配列は、たとえ要素の数が同じであっても、「=」で内容をそっくりコピーすることはできません。memcpyのような関数を使うことが必要です。構造体の場合は、「=」を使って内容をコピーすることができます。
}

//-----------------------------------------------------------------------------
/*
 *  @brief         有効フラグ
 */
//-----------------------------------------------------------------------------
void Fog::SetEnable(bool enable)
{
  b32 old_enable = m_enable_flag;

  if( enable )
  {
    if( !old_enable )
    {
      m_enable_flag = true;
      // この時点でtrueにしておいてSetできるようにする
      this->SetMinDepth(m_enable_min_depth);
      this->SetMaxDepth(m_enable_max_depth);
      m_enable_min_depth = DISABLE_MIN_DEPTH;
      m_enable_max_depth = DISABLE_MAX_DEPTH;
    }
  }
  else
  {
    if( old_enable )
    {
      // この時点ではtrueのままにしておいてSetできるようにする
      m_enable_min_depth = this->GetMinDepth();
      m_enable_max_depth = this->GetMaxDepth();
      this->SetMinDepth(DISABLE_MIN_DEPTH);
      this->SetMaxDepth(DISABLE_MAX_DEPTH);
      m_enable_flag = false;
    }
  }
}

b32 Fog::IsEnable(void) const
{
  return m_enable_flag;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         説明
 */
//-----------------------------------------------------------------------------
#if 0

// gfx_Fog.h/.cpp
// SetupFogSampler関数そのままを試してみてうまくいく。
void Fog::Update(Camera* camera)
{
  nw::gfx::res::ResImageLookupTable fogSampler = GetNwResFog().GetFogSampler();
  nw::gfx::res::ResFogUpdater fogUpdater = GetNwResFogUpdater();
  const gfl::math::MTX44& inverseProjectionMatrix = camera->GetNwCamera()->InverseProjectionMatrix();

  f32 l_Near = camera->GetNearClip();
  f32 l_Far = camera->GetFarClip();
  f32 l_WScale = camera->GetWScale();

  {
    const uint HALF_TABLE_SIZE = nw::gfx::FOG_TABLE_SIZE / 2;
    

    enum
    {
        REG_FOG_TABLE_INDEX = 0xe6,
        REG_FOG_TABLE_PARAM = 0xe8
    };

    const u32 HEADER_FOG_TABLE_INDEX = nw::gfx::internal::MakeCommandHeader(REG_FOG_TABLE_INDEX, 1, false, 0xF);
    const u32 HEADER_FOG_TABLE_PARAM = nw::gfx::internal::MakeCommandHeader(REG_FOG_TABLE_PARAM, 128, false, 0xF);

    fogSampler.SetCommandCache(0, 0);
    fogSampler.SetCommandCache(1, HEADER_FOG_TABLE_INDEX);
    fogSampler.SetCommandCache(3, HEADER_FOG_TABLE_PARAM);

    uint index = 0;
    gfl::math::VEC4 viewPos;
    f32 prevLut = 0.0f;
    
    for (int i = 0; i <= HALF_TABLE_SIZE; ++i)
    {
        if (l_WScale == 0.0f)
        {
            f32 depth = -(f32)i / (f32)( HALF_TABLE_SIZE );
            viewPos.w = inverseProjectionMatrix.f._32 * depth + inverseProjectionMatrix.f._33;
            viewPos.z = -(inverseProjectionMatrix.f._22 * depth + inverseProjectionMatrix.f._23) / viewPos.w;
        }
        else
        {
            viewPos.z = ((f32)i / (f32)HALF_TABLE_SIZE) * (l_Far - l_Near) + l_Near;
        }

        if (viewPos.z <= fogUpdater.GetMinFogDepth())
        {
            viewPos.z = 1.0f;
        }
        else if (viewPos.z > fogUpdater.GetMaxFogDepth())
        {
            viewPos.z = 0.0f;
        }
        else
        {
            if (fogUpdater.GetFogUpdaterType() == nw::gfx::res::ResFogUpdater::FOG_UPDATER_TYPE_LINEAR)
            {
                viewPos.z =
                    (fogUpdater.GetMaxFogDepth() - viewPos.z) / (fogUpdater.GetMaxFogDepth() - fogUpdater.GetMinFogDepth());
            }
            else if (fogUpdater.GetFogUpdaterType() == nw::gfx::res::ResFogUpdater::FOG_UPDATER_TYPE_EXPONENT)
            {
                viewPos.z = 
                    nw::math::FExp(
                    -fogUpdater.GetDensity() *
                    (viewPos.z - fogUpdater.GetMinFogDepth()) / (fogUpdater.GetMaxFogDepth() - fogUpdater.GetMinFogDepth()));
            }
            else if (fogUpdater.GetFogUpdaterType() == nw::gfx::res::ResFogUpdater::FOG_UPDATER_TYPE_EXPONENT_SQUARE)
            {
                f32 viewDepth = fogUpdater.GetDensity() * 
                        (viewPos.z - fogUpdater.GetMinFogDepth()) / (fogUpdater.GetMaxFogDepth() - fogUpdater.GetMinFogDepth());
                viewPos.z =
                    nw::math::FExp( - viewDepth * viewDepth );
            }
            else
            {
                GFL_ASSERT_MSG(false, "Unknown FogUpdater type.");  //@check 検索に引っ掛からないように確認済の印をつけておく。
            }
        }

        if ( i > 0 )
        {
            u32 value = nw::ut::Fixed13::Float32ToFixed13(viewPos.z - prevLut) | (nw::ut::Fixed11::Float32ToFixed11(prevLut) << 13);
            u32 commandIndex = (index == 0) ? 2 : index + 3;
            
            fogSampler.SetCommandCache(commandIndex, value );
            
            ++index;
        }
        
        prevLut = viewPos.z;
    }

  }

}

#elif 0

これは間違い。
フォグが全くかからないならviewPos.z = 1.0f、フォグしか見えないならviewPos.z = 0.0fでなければならない。
これは、参照テーブルの横軸をひっくり返しているのがダメ。
横軸をひっくり返すのではなく、縦軸をひっくり返すのが正解。
void Fog::Update(Camera* camera)
{
  if( !m_lookup_table_resource ) return;




  // 定義
  const f32 FIX12_ONE_U32 = 4096.0f;
  const u32 HALF_TABLE_SIZE = nw::gfx::FOG_TABLE_SIZE / 2;
  STATIC_ASSERT(HALF_TABLE_SIZE == 128);  // 128じゃなかったら以下のソースが成り立たないのでコンパイル時にチェック

  // 変数
  u32 lookup_table_data_value[HALF_TABLE_SIZE];
  u32 index;

  // 参照テーブルを読む
  {
    nw::gfx::ResGraphicsFile* nw_res_graphics_file = m_lookup_table_resource->GetNwResGraphicsFile();
    nw::gfx::res::ResLookupTableSet nw_res_lookup_table_set = nw_res_graphics_file->GetLutSets(m_lookup_table_set_index_in_resource);
    nw::gfx::res::ResImageLookupTable nw_res_image_lookup_table = nw_res_lookup_table_set.GetSamplers(m_lookup_table_index_in_lookup_table_set);

    const s32 LOOKUP_TABLE_DATA_NUM = 259;
    s32 lookup_table_data_index = 0;
    index = 0;
    for( s32 i=0; i<LOOKUP_TABLE_DATA_NUM; ++i )
    {
      if( i==1 || i==131 )
      {
        // ヘッダみたいなものかな
        // 何もしない
      }
      else if( i==129 || i==259 )
      {
        // 区切りにでも使っているものかな
        // 何もしない
      }
      else if ( i>=260 )
      {
        // ちゃんとしたデータではなさそうだ
        // 何もしない
      }
      else
      {
        // 有効なデータ
        
        //if (viewPos.z <= fogUpdater.GetMinFogDepth())
        //{
        //  viewPos.z = 1.0f;
        //}
        //else if (viewPos.z > fogUpdater.GetMaxFogDepth())
        //{
        //  viewPos.z = 0.0f;
        //}
        //に合わせるためにひっくり返す。
       
        b32 b_use = false;
        if( lookup_table_data_index < 8 )
        {
          if( lookup_table_data_index % 2 == 0 )  // 0, 2, 4, 6
          {
            b_use = true;
          }
        }
        else
        {
          if( lookup_table_data_index % 2 == 1 )  // 9, 11, 13, 15, ... , 255
          {
            b_use = true;
          }
        }

        if( b_use )
        {
          u32 value = nw_res_image_lookup_table.GetCommandCache(i);
          u32 value_up   = ( value & 0xFFF000 ) >> 12;
          u32 value_down = ( value & 0xFFF ); 
          lookup_table_data_value[HALF_TABLE_SIZE-1-index] = value_down;
          ++index;
        }
        ++lookup_table_data_index;
      }

#if 0
      {
        u32 value = nw_res_image_lookup_table.GetCommandCache(i);
        u32 value_up   = ( value & 0xFFF000 ) >> 12;
        u32 value_down = ( value & 0xFFF ); 
        GFL_PRINT("fog lut GetCommandCache[%4d] %d = %d | %d\n", i, value, value_up, value_down);
      }
#endif

    }
  }

#if 0
  {
    for(s32 i=0; i<HALF_TABLE_SIZE; ++i)
    {
      GFL_PRINT("fog lut[%4d] %d\n", i, lookup_table_data_value[i]);
    }
  }
#endif

  // 引数
  nw::gfx::res::ResImageLookupTable fogSampler = GetNwResFog().GetFogSampler();
  nw::gfx::res::ResFogUpdater fogUpdater = GetNwResFogUpdater();
  const gfl::math::MTX44& inverseProjectionMatrix = camera->GetNwCamera()->InverseProjectionMatrix();

  // カメラ
  f32 l_Near = camera->GetNearClip();
  f32 l_Far = camera->GetFarClip();
  f32 l_WScale = camera->GetWScale();

  // フォグの参照テーブルに値を設定する
  {
    enum
    {
      REG_FOG_TABLE_INDEX = 0xe6,
      REG_FOG_TABLE_PARAM = 0xe8
    };

    const u32 HEADER_FOG_TABLE_INDEX = nw::gfx::internal::MakeCommandHeader(REG_FOG_TABLE_INDEX, 1, false, 0xF);
    const u32 HEADER_FOG_TABLE_PARAM = nw::gfx::internal::MakeCommandHeader(REG_FOG_TABLE_PARAM, 128, false, 0xF);

    fogSampler.SetCommandCache(0, 0);
    fogSampler.SetCommandCache(1, HEADER_FOG_TABLE_INDEX);
    fogSampler.SetCommandCache(3, HEADER_FOG_TABLE_PARAM);

    index = 0;
    gfl::math::VEC4 viewPos;
    f32 prevLut = 0.0f;
    
    for (s32 i = 0; i <= HALF_TABLE_SIZE; ++i)
    {
      if (l_WScale == 0.0f)
      {
        f32 depth = -(f32)i / (f32)( HALF_TABLE_SIZE );
        viewPos.w = inverseProjectionMatrix.f._32 * depth + inverseProjectionMatrix.f._33;
        viewPos.z = -(inverseProjectionMatrix.f._22 * depth + inverseProjectionMatrix.f._23) / viewPos.w;
      }
      else
      {
        viewPos.z = ((f32)i / (f32)HALF_TABLE_SIZE) * (l_Far - l_Near) + l_Near;
      }

      if (viewPos.z <= fogUpdater.GetMinFogDepth())
      {
        //viewPos.z = 1.0f;
        viewPos.z = lookup_table_data_value[0] / FIX12_ONE_U32;
            // 参照テーブルの開始終了値が開始0終了1になっているとは限らないので、決め打ちの値ではなく参照テーブルの値を使う。
      }
      else if (viewPos.z > fogUpdater.GetMaxFogDepth())
      {
        //viewPos.z = 0.0f;
        viewPos.z = lookup_table_data_value[HALF_TABLE_SIZE-1] / FIX12_ONE_U32;
            // 参照テーブルの開始終了値が開始0終了1になっているとは限らないので、決め打ちの値ではなく参照テーブルの値を使う。
      }
      else
      {
        if( i == 0 )
        {
          viewPos.z = lookup_table_data_value[0] / FIX12_ONE_U32;
        }
        else  // if ( i > 0 )
        {
          viewPos.z = lookup_table_data_value[index] / FIX12_ONE_U32;
        }
      }

      if ( i > 0 )
      {
        u32 value = nw::ut::Fixed13::Float32ToFixed13(viewPos.z - prevLut) | (nw::ut::Fixed11::Float32ToFixed11(prevLut) << 13);
        u32 commandIndex = (index == 0) ? 2 : index + 3;
        
        fogSampler.SetCommandCache(commandIndex, value );
        
        ++index;
      }
      
      prevLut = viewPos.z;
    }
  }

}

#elif 0

FogTableも使えるようにしたので、これは古くなったのでコメントアウト
void Fog::Update(Camera* camera)
{
  if( !m_lookup_table_resource ) return;




  // 定義
  const u32 FIX12_ONE_U32_u32 = 4096;
  const f32 FIX12_ONE_U32_f32 = 4096.0f;
  const u32 HALF_TABLE_SIZE = nw::gfx::FOG_TABLE_SIZE / 2;
  STATIC_ASSERT(HALF_TABLE_SIZE == 128);  // 128じゃなかったら以下のソースが成り立たないのでコンパイル時にチェック

  // 変数
  u32 lookup_table_data_value[HALF_TABLE_SIZE];
  u32 index;

  // 参照テーブルを読む
  {
    nw::gfx::ResGraphicsFile* nw_res_graphics_file = m_lookup_table_resource->GetNwResGraphicsFile();
    nw::gfx::res::ResLookupTableSet nw_res_lookup_table_set = nw_res_graphics_file->GetLutSets(m_lookup_table_set_index_in_resource);
    nw::gfx::res::ResImageLookupTable nw_res_image_lookup_table = nw_res_lookup_table_set.GetSamplers(m_lookup_table_index_in_lookup_table_set);

    const s32 LOOKUP_TABLE_DATA_NUM = 259;
    s32 lookup_table_data_index = 0;
    index = 0;
    for( s32 i=0; i<LOOKUP_TABLE_DATA_NUM; ++i )
    {
      if( i==1 || i==131 )
      {
        // ヘッダみたいなものかな
        // 何もしない
      }
      else if( i==129 || i==259 )
      {
        // 区切りにでも使っているものかな
        // 何もしない
      }
      else if ( i>=260 )
      {
        // ちゃんとしたデータではなさそうだ
        // 何もしない
      }
      else
      {
        // 有効なデータ
        
        //if (viewPos.z <= fogUpdater.GetMinFogDepth())
        //{
        //  viewPos.z = 1.0f;
        //}
        //else if (viewPos.z > fogUpdater.GetMaxFogDepth())
        //{
        //  viewPos.z = 0.0f;
        //}
        //に合わせるために、上下をひっくり返す。
       
        b32 b_use = false;
        if( lookup_table_data_index < 8 )
        {
          if( lookup_table_data_index % 2 == 0 )  // 0, 2, 4, 6
          {
            b_use = true;
          }
        }
        else
        {
          if( lookup_table_data_index % 2 == 1 )  // 9, 11, 13, 15, ... , 255
          {
            b_use = true;
          }
        }

        if( b_use )
        {
          u32 value = nw_res_image_lookup_table.GetCommandCache(i);
          u32 value_up   = ( value & 0xFFF000 ) >> 12;
          u32 value_down = ( value & 0xFFF );
          if( value_down > FIX12_ONE_U32_u32 * 10 )
          {
            GFL_ASSERT_MSG(0, "フォグの参照テーブルの値%dが異常に大きいです。\n", value_down);
            value_down = 0;
          }
          if( value_down > FIX12_ONE_U32_u32 ) value_down = FIX12_ONE_U32_u32;
          lookup_table_data_value[index] = FIX12_ONE_U32_u32-value_down;
          ++index;
        }
        ++lookup_table_data_index;
      }

#if 0
      {
        u32 value = nw_res_image_lookup_table.GetCommandCache(i);
        u32 value_up   = ( value & 0xFFF000 ) >> 12;
        u32 value_down = ( value & 0xFFF ); 
        GFL_PRINT("fog lut GetCommandCache[%4d] %d = %d | %d\n", i, value, value_up, value_down);
      }
#endif

    }
  }

#if 0
  {
    for(s32 i=0; i<HALF_TABLE_SIZE; ++i)
    {
      GFL_PRINT("fog lut[%4d] %d\n", i, lookup_table_data_value[i]);
    }
  }
#endif

  // 引数
  nw::gfx::res::ResImageLookupTable fogSampler = GetNwResFog().GetFogSampler();
  nw::gfx::res::ResFogUpdater fogUpdater = GetNwResFogUpdater();
  const gfl::math::MTX44& inverseProjectionMatrix = camera->GetNwCamera()->InverseProjectionMatrix();

  // カメラ
  f32 l_Near = camera->GetNearClip();
  f32 l_Far = camera->GetFarClip();
  f32 l_WScale = camera->GetWScale();

  // フォグの参照テーブルに値を設定する
  {
    enum
    {
      REG_FOG_TABLE_INDEX = 0xe6,
      REG_FOG_TABLE_PARAM = 0xe8
    };

    const u32 HEADER_FOG_TABLE_INDEX = nw::gfx::internal::MakeCommandHeader(REG_FOG_TABLE_INDEX, 1, false, 0xF);
    const u32 HEADER_FOG_TABLE_PARAM = nw::gfx::internal::MakeCommandHeader(REG_FOG_TABLE_PARAM, 128, false, 0xF);

    fogSampler.SetCommandCache(0, 0);
    fogSampler.SetCommandCache(1, HEADER_FOG_TABLE_INDEX);
    fogSampler.SetCommandCache(3, HEADER_FOG_TABLE_PARAM);

    index = 0;
    gfl::math::VEC4 viewPos;
    f32 prevLut = 0.0f;
    
    for (s32 i = 0; i <= HALF_TABLE_SIZE; ++i)
    {
      if (l_WScale == 0.0f)
      {
        f32 depth = -(f32)i / (f32)( HALF_TABLE_SIZE );
        viewPos.w = inverseProjectionMatrix.f._32 * depth + inverseProjectionMatrix.f._33;
        viewPos.z = -(inverseProjectionMatrix.f._22 * depth + inverseProjectionMatrix.f._23) / viewPos.w;
      }
      else
      {
        viewPos.z = ((f32)i / (f32)HALF_TABLE_SIZE) * (l_Far - l_Near) + l_Near;
      }

      if( !m_enable_flag )
      {
        viewPos.z = 1.0f;
            // ただし、フォグが無効のときは全く出してはいけないので、1.0fにする。
      }
      else
      if (viewPos.z <= fogUpdater.GetMinFogDepth())
      {
        //viewPos.z = 1.0f;
        viewPos.z = lookup_table_data_value[0] / FIX12_ONE_U32_f32;
            // 参照テーブルの開始終了値が開始0終了1になっているとは限らないので、決め打ちの値ではなく参照テーブルの値を使う。
      }
      else if (viewPos.z > fogUpdater.GetMaxFogDepth())
      {
        //viewPos.z = 0.0f;
        viewPos.z = lookup_table_data_value[HALF_TABLE_SIZE-1] / FIX12_ONE_U32_f32;
            // 参照テーブルの開始終了値が開始0終了1になっているとは限らないので、決め打ちの値ではなく参照テーブルの値を使う。
      }
      else
      {
        /*
        これはおかしい。
        if( i == 0 )
        {
          viewPos.z = lookup_table_data_value[0] / FIX12_ONE_U32_f32;
        }
        else  // if ( i > 0 )
        {
          viewPos.z = lookup_table_data_value[index] / FIX12_ONE_U32_f32;
        }
        */
        
        s32 table_i = static_cast<s32>( (viewPos.z - fogUpdater.GetMinFogDepth()) / (fogUpdater.GetMaxFogDepth() - fogUpdater.GetMinFogDepth()) * HALF_TABLE_SIZE );
        if( table_i < 0 ) table_i = 0;
        else if( table_i >= HALF_TABLE_SIZE ) table_i = HALF_TABLE_SIZE -1;
        viewPos.z = lookup_table_data_value[table_i] / FIX12_ONE_U32_f32;  // @note 値を補完すべきか(table_i-1)と(i)で。これでは段々になりそう。
      }

      viewPos.z = 1.0f - ( 1.0f - viewPos.z ) * m_intensity;

      if ( i > 0 )
      {
        u32 value = nw::ut::Fixed13::Float32ToFixed13(viewPos.z - prevLut) | (nw::ut::Fixed11::Float32ToFixed11(prevLut) << 13);
        u32 commandIndex = (index == 0) ? 2 : index + 3;
        
        fogSampler.SetCommandCache(commandIndex, value );
        
        ++index;
      }
      
      prevLut = viewPos.z;
    }
  }

}

#else

//----------------------------------------------------------------------------
/**
 *	@brief  フォグの更新（実際に使われている関数）
 */
//-----------------------------------------------------------------------------
void Fog::Update(Camera* camera)
{
  switch( m_type )
  {
  case TYPE_CREATE_FOG_TABLE:          UpdateUsingFogTable(camera);    break;
  case TYPE_USE_LOOKUP_TABLE_RESOURCE: UpdateUsingLookupTable(camera); break;
  }
}

#endif

//----------------------------------------------------------------------------
/**
 *	@brief  指定されたフォグテーブルを使用しての更新
 */
//-----------------------------------------------------------------------------
void Fog::UpdateUsingFogTable(Camera* camera)
{
  if( !m_fog_table ) return;
  this->updateUsingFogTable( camera, *m_fog_table );
}

//----------------------------------------------------------------------------
/**
 *	@brief  指定された参照テーブルを使用しての更新
 */
//-----------------------------------------------------------------------------
void Fog::UpdateUsingLookupTable(Camera* camera)
{
  if( !m_lookup_table_resource ) return;

  // フォグテーブルを反映
  this->updateUsingFogTable( camera, *m_fog_table );
}


//----------------------------------------------------------------------------
/**
 *	@brief  フォグテーブルを使用したフォグの更新
 */
//-----------------------------------------------------------------------------
void Fog::updateUsingFogTable(Camera* camera, const FogTable& fog_table )
{
  // 定義
  const u32 HALF_TABLE_SIZE = nw::gfx::FOG_TABLE_SIZE / 2;
  STATIC_ASSERT(HALF_TABLE_SIZE == 128);  // 128じゃなかったら以下のソースが成り立たないのでコンパイル時にチェック
  STATIC_ASSERT(FOG_TABLE_VALUE_NUM == 128);

  // 変数
  u32 index;

  // 引数
  nw::gfx::res::ResImageLookupTable fogSampler = GetNwResFog().GetFogSampler();
  nw::gfx::res::ResFogUpdater fogUpdater = GetNwResFogUpdater();
  const gfl::math::MTX44& inverseProjectionMatrix = camera->GetNwCamera()->InverseProjectionMatrix();

  // カメラ
  f32 l_Near = camera->GetNearClip();
  f32 l_Far = camera->GetFarClip();
  f32 l_WScale = camera->GetWScale();

  // フォグの参照テーブルに値を設定する
  {
    enum
    {
      REG_FOG_TABLE_INDEX = 0xe6,
      REG_FOG_TABLE_PARAM = 0xe8
    };

    const u32 HEADER_FOG_TABLE_INDEX = nw::gfx::internal::MakeCommandHeader(REG_FOG_TABLE_INDEX, 1, false, 0xF);
    const u32 HEADER_FOG_TABLE_PARAM = nw::gfx::internal::MakeCommandHeader(REG_FOG_TABLE_PARAM, 128, false, 0xF);

    fogSampler.SetCommandCache(0, 0);
    fogSampler.SetCommandCache(1, HEADER_FOG_TABLE_INDEX);
    fogSampler.SetCommandCache(3, HEADER_FOG_TABLE_PARAM);

    index = 0;
    gfl::math::VEC4 viewPos;
    f32 prevLut = 1.0f;
    
    for (s32 i = 0; i <= HALF_TABLE_SIZE; ++i)
    {
      if (l_WScale == 0.0f)
      {
        f32 depth = -(f32)i / (f32)( HALF_TABLE_SIZE );
        viewPos.w = inverseProjectionMatrix.f._32 * depth + inverseProjectionMatrix.f._33;
        viewPos.z = -(inverseProjectionMatrix.f._22 * depth + inverseProjectionMatrix.f._23) / viewPos.w;
      }
      else
      {
        viewPos.z = ((f32)i / (f32)HALF_TABLE_SIZE) * (l_Far - l_Near) + l_Near;
      }

      if( !m_enable_flag )
      {
        viewPos.z = 0.0f;
      }
      else
      // 参照テーブルの開始終了値が開始0終了1になっているとは限らないので、決め打ちの値ではなく参照テーブルの値を使う。
      if (viewPos.z <= fogUpdater.GetMinFogDepth())
      {
        viewPos.z = fog_table.value[0];
      }
      else if (viewPos.z > fogUpdater.GetMaxFogDepth())
      {
        viewPos.z = fog_table.value[HALF_TABLE_SIZE-1];
      }
      else
      {
        s32 table_i = static_cast<s32>( (viewPos.z - fogUpdater.GetMinFogDepth()) / (fogUpdater.GetMaxFogDepth() - fogUpdater.GetMinFogDepth()) * HALF_TABLE_SIZE );
        if( table_i < 0 ) table_i = 0;
        else if( table_i >= HALF_TABLE_SIZE ) table_i = HALF_TABLE_SIZE -1;
        viewPos.z = fog_table.value[table_i];
      }

      viewPos.z = 1.0f - (viewPos.z * m_intensity);

      if ( i > 0 )
      {
        u32 value = nw::ut::Fixed13::Float32ToFixed13(viewPos.z - prevLut) | (nw::ut::Fixed11::Float32ToFixed11(prevLut) << 13);
        u32 commandIndex = (index == 0) ? 2 : index + 3;

        fogSampler.SetCommandCache(commandIndex, value );

#if GFL_DEBUG
        static bool s_flag = false;
        
        if(s_flag){
          GFL_PRINT( "index %d value %f\n", commandIndex, prevLut );
        }
#endif // GFL_DEBUG
        
        ++index;
      }
      
      prevLut = viewPos.z;
    }
  }
}

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

