//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dResource.cpp
 *	@brief  3Dリソース
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK
#include <nn/gx.h>

// NW4C
#include <nw/ut.h>
#include <nw/gfx.h>
#include <nw/anim.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Base.h>
#include <gfl_Heap.h>
#include <gfl_fs.h>
#include <gfl_debug.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dSystem.h>
#include <grp/g3d/gfl_G3dTexture.h>

#include "gfl_G3dErrorCheck.h"
#include <grp/g3d/gfl_G3dResource.h>


#define GFL_GRP_G3D_RESOURCE_PRINT (GFL_DEBUG && (1))  // これが1のとき、確認出力がある。これが0のとき、ない。


namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス定義
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	関数オブジェクト
//=====================================
class UnresolvedReferenceFunctor
{
public:
  UnresolvedReferenceFunctor(void){}
public:
  void operator()(nw::gfx::res::ResTexture refer)
  {
    GFL_PRINT("Texture Name=%s\n", refer.GetName());
  }
  void operator()(nw::gfx::res::ResReferenceShader refer)
  {
    GFL_PRINT("Shader Path=%s, Name=%s\n", refer.GetPath(), refer.GetName());
  }
  void operator()(nw::gfx::res::ResReferenceLookupTable refer)
  {
    GFL_PRINT("Lut Path=%s, TableName=%s\n", refer.GetPath(), refer.GetTableName());
  }
};




//-------------------------------------
///	3Dリソースクラス
//=====================================

//-----------------------------------------------------------------------------
/*
 *  @brief         メンバーアニメーションの要素数で最大の値を得る
 *
 *  @param[in]     array_element_num              animation_resource_pack_array配列の要素数 
 *  @param[in]     animation_resource_pack_array  AnimationResourcePackの配列の先頭へのポインタ
 * 
 *  @retval        s32  animation_resource_pack_arrayのメンバーアニメーションの要素数で最大の値
 *
 *  使用例
 *  gfl::grp::g3d::Resource::AnimationResourcePack  animation_resource_pack_array[]  =
 *  {
 *      { resource0, gfl::grp::g3d::Resource::ANIMATION_RESOURCE_TYPE_SKELETAL, 0 },
 *      { resource0, gfl::grp::g3d::Resource::ANIMATION_RESOURCE_TYPE_SKELETAL, 1 },
 *      { resource1, gfl::grp::g3d::Resource::ANIMATION_RESOURCE_TYPE_SKELETAL, 0 },
 *      { resource1, gfl::grp::g3d::Resource::ANIMATION_RESOURCE_TYPE_SKELETAL, 1 },
 *  };
 *  s32 max_member_anim_set_count = gfl::grp::g3d::Resource::GetMaxMemberAnimSetCount(
 *      sizeof(animation_resource_pack_array)/sizeof(animation_resource_pack_array[0]),
 *      animation_resource_pack_array
 *  );
 */
//-----------------------------------------------------------------------------
s32 Resource::GetMaxMemberAnimSetCount(
    const u32                    array_element_num,
    const AnimationResourcePack* animation_resource_pack_array
)
{
  s32 max_count = 0;
  for( u32 i=0; i<array_element_num; ++i )
  {
    s32 count = 0;
    switch( animation_resource_pack_array[i].animation_resource_type )
    {
    case ANIMATION_RESOURCE_TYPE_SKELETAL:
        count = animation_resource_pack_array[i].resource->GetSkeletalMemberAnimSetCount(animation_resource_pack_array[i].index_in_resource);
        break;
    case ANIMATION_RESOURCE_TYPE_MATERIAL:
        count = animation_resource_pack_array[i].resource->GetMaterialMemberAnimSetCount(animation_resource_pack_array[i].index_in_resource);
        break;
    case ANIMATION_RESOURCE_TYPE_VISIBILITY:
        count = animation_resource_pack_array[i].resource->GetVisibilityMemberAnimSetCount(animation_resource_pack_array[i].index_in_resource);
        break;
    case ANIMATION_RESOURCE_TYPE_CAMERA:
        count = animation_resource_pack_array[i].resource->GetCameraMemberAnimSetCount(animation_resource_pack_array[i].index_in_resource);
        break;
    case ANIMATION_RESOURCE_TYPE_LIGHT:
        count = animation_resource_pack_array[i].resource->GetLightMemberAnimSetCount(animation_resource_pack_array[i].index_in_resource);
        break;
    }
    if( count > max_count ) max_count = count;
  }
  return max_count;
}


//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
 *
 *  @param[in]     allocator    アロケータ
 *  @param[in]     path         グラフィックスリソースファイルのパス名
 *                              (allocatorとpathの両方がNULLでないときはロードする。
 *                               GetBufferの戻り値がNULLのときはロード失敗)
 *  @param[in]     description  リソースの設定内容
 *                              (ロードしたときのみ使用する。
 *                               NULLのときはデフォルト値。
 *                               呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
 *  @param[in]     do_setup     ファイル中のコンテントのセットアップを行う場合はtrue
 *                              (ロードしたときのみ使用する。
 *                               セットアップできているか否かはIsSetupで確認)
 *  @param[in]     common_resource_flag  使用する共通リソースがある場合はビットを立てておく
 */
//-----------------------------------------------------------------------------
Resource::Resource(gfl::heap::NwAllocator* allocator, const char* path, const Description* description, bool do_setup, u32 common_resource_flag)
  : m_nw_res_graphics_file(NULL),
    m_nw_result(),
    m_is_self_setup(false),
    m_is_setup(false),
    m_description(),
    m_buffer(NULL),
    m_own_buffer(false)
{
  GFL_SINGLETON_INSTANCE(System)->GetResourceManager()->AddObject(this);

  if( allocator != NULL && path != NULL )
  {
    Load(allocator, path, description);
    if(do_setup)
    {
      Setup(allocator, NULL, common_resource_flag);
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
 *                 グラフィックスリソースファイルを読み込んだバッファのアタッチ
 *
 *  @param[in]     allocator    アロケータ
 *  @param[in]     buffer       グラフィックスリソースを読み込んだバッファのアドレス
 *                              (GetBufferの戻り値がNULLのときはバッファのアタッチ失敗)
 *  @param[in]     description  リソースの設定内容
 *                              (バッファをアタッチしたときのみ使用する。
 *                               NULLのときはデフォルト値。
 *                               呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
 *  @param[in]     do_setup     ファイル中のコンテントのセットアップを行う場合はtrue
 *                              (バッファをアタッチしたときのみ使用する。
 *                               セットアップできているか否かはIsSetupで確認)
 *  @param[in]     own_buffer   tureのときbufferの管理はこのインスタンスが行うので呼び出し元はbufferを解放してはならない。
 *                              falseのときbufferの管理は呼び出し元で行って下さい。
 *                              (バッファをアタッチしたときのみ使用する)
 *  @param[in]     common_resource_flag  使用する共通リソースがある場合はビットを立てておく
 */
//-----------------------------------------------------------------------------
Resource::Resource(
    gfl::heap::NwAllocator* allocator,
    const void* buffer,
    const Description* description,
    bool do_setup,
    bool own_buffer,
    u32 common_resource_flag )
  : m_nw_res_graphics_file(NULL),
    m_nw_result(),
    m_is_self_setup(false),
    m_is_setup(false),
    m_description(),
    m_buffer(NULL),
    m_own_buffer(false)
{
  GFL_SINGLETON_INSTANCE(System)->GetResourceManager()->AddObject(this);

  if( allocator != NULL && buffer != NULL )
  {
    AttachBuffer(allocator, buffer, description, own_buffer);
    if(do_setup)
    {
      Setup(allocator, NULL, common_resource_flag);
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
 *                 アーカイブファイルからデータを読み込む
 *
 *  @param[in]     allocator    アロケータ
 *  @param[in]     arc_file     アーカイブファイル
 *                              (GetBufferの戻り値がNULLのときはロード失敗)
 *  @param[in]     arc_data_id  アーカイブファイル内のデータID
 *  @param[in]     description  リソースの設定内容
 *                              (読み込んだときのみ使用する。
 *                               NULLのときはデフォルト値。
 *                               呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
 *  @param[in]     do_setup     ファイル中のコンテントのセットアップを行う場合はtrue
 *                              (読み込んだときのみ使用する。
 *                               セットアップできているか否かはIsSetupで確認)
 *  @param[in]     common_resource_flag  使用する共通リソースがある場合はビットを立てておく
 *  @param[in]     is_compress  圧縮データの場合はtrue
 */
//-----------------------------------------------------------------------------
Resource::Resource(
    gfl::heap::NwAllocator* allocator,
    gfl::fs::ArcFile*       arc_file,
    u32                     arc_data_id,
    const Description* description,
    bool do_setup,
    u32 common_resource_flag,
    const b32 is_compress )
  : m_nw_res_graphics_file(NULL),
    m_nw_result(),
    m_is_self_setup(false),
    m_is_setup(false),
    m_description(),
    m_buffer(NULL),
    m_own_buffer(false)
{
  GFL_SINGLETON_INSTANCE(System)->GetResourceManager()->AddObject(this);

  if( allocator != NULL && arc_file != NULL )
  {
    Load(allocator, arc_file, arc_data_id, description, is_compress);
    if(do_setup)
    {
      Setup(allocator, NULL, common_resource_flag);
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         デストラクタ
 */
//-----------------------------------------------------------------------------
Resource::~Resource()
{
  if( m_nw_res_graphics_file )
  {
    nw::ut::SafeCleanup( *m_nw_res_graphics_file );
    GFL_DELETE m_nw_res_graphics_file;
    m_nw_res_graphics_file = NULL;
  }
  if( m_buffer && m_own_buffer )
  {
    GflHeapFreeMemory( const_cast<void*>(m_buffer) );
    m_buffer = NULL;
  }
  
  GFL_SINGLETON_INSTANCE(System)->GetResourceManager()->RemoveObject(this);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         グラフィックスリソースファイルのロード
 *                 ロードだけでファイル中のコンテントのセットアップはしない。
 *
 *  @param[in]     allocator    アロケータ
 *  @param[in]     path         グラフィックスリソースファイルのパス名
 *  @param[in]     description  リソースの設定内容
 *                              (ロードしたときのみ使用する。
 *                               NULLのときはデフォルト値。
 *                               呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
 *
 *  @retval        bool    falseのときはロード失敗。
 *                         (失敗の理由は
 *                          allocatorが不正か、pathが不正か、既にロードした他のデータが存在するか、
 *                          メモリ不足のどれか)
 */
//-----------------------------------------------------------------------------
bool Resource::Load(gfl::heap::NwAllocator* allocator, const char* path, const Description* description)
{
  if( allocator              == NULL ) return false;
  if( path                   == NULL ) return false;
  //if( m_buffer               != NULL ) return false;  m_nw_res_graphics_fileで確認しているのでm_bufferの確認は要らない。
  if( m_nw_res_graphics_file != NULL ) return false;

  gfl::heap::HeapBase* heap_base = allocator->GetHeapBase();
  
  // ファイルロード
  gfl::fs::ReadFile read_file;
  bool open_ret = read_file.Open(path);
  if( !open_ret ) return false;
  u32 size       = read_file.GetSize();
  u32 size_align = read_file.GetSizeAlign();  // テクスチャをロードするには128Byteアライメントを行う必要があるので、
  size_align     = 128;                       // 何をロードするかでアライメントを分ける必要があるかも
  void* buffer = GflHeapAllocMemoryAlign( heap_base, size, size_align );
  s32 read_ret = read_file.Read( buffer, size );  // sizeをsize_alignに合わせたアライメントにしておく必要があるかも
  read_file.Close();
  if( read_ret < 0 )
  {
    GflHeapFreeMemory(buffer);
    return false;
  }

  // ここまで来たら正常に読み込めている
  return AttachBuffer(allocator, buffer, description, true);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アーカイブファイルからデータを読み込む
 *                 読み込むだけでファイル中のコンテントのセットアップはしない。
 *
 *  @param[in]     allocator    アロケータ
 *  @param[in]     arc_file     アーカイブファイル
 *                              (GetBufferの戻り値がNULLのときはロード失敗)
 *  @param[in]     arc_data_id  アーカイブファイル内のデータID
 *  @param[in]     description  リソースの設定内容
 *                              (読み込んだときのみ使用する。
 *                               NULLのときはデフォルト値。
 *                               呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
 *  @param[in]     is_compress  圧縮データの場合はtrue
 * 
 *  @retval        bool    falseのときはロード失敗。
 *                         (失敗の理由は
 *                          allocatorが不正か、pathが不正か、既にロードした他のデータが存在するか、
 *                          メモリ不足のどれか)
 */
//-----------------------------------------------------------------------------
bool Resource::Load(
    gfl::heap::NwAllocator* allocator,
    gfl::fs::ArcFile* arc_file,
    u32 arc_data_id,
    const Description* description,
    const b32 is_compress )
{

  if( allocator              == NULL ) return false;
  if( arc_file               == NULL ) return false;
  //if( m_buffer               != NULL ) return false;  m_nw_res_graphics_fileで確認しているのでm_bufferの確認は要らない。
  if( m_nw_res_graphics_file != NULL ) return false;

  gfl::heap::HeapBase* heap_base = allocator->GetHeapBase();

  // ファイルロード
  size_t size;
  s32    size_align = 128;  // テクスチャをロードするには128Byteアライメントを行う必要があるので、
                            // 何をロードするかでアライメントを分ける必要があるかも
  void*  buffer = NULL;

  if( is_compress )
  {
    // 圧縮データ
    buffer = arc_file->LoadData( arc_data_id, heap_base, size_align, &size );
    if( !buffer )
    {
      return false;
    }
  }
  else
  {
    // 圧縮していないデータ
    size = arc_file->GetDataSize( arc_data_id );
    buffer = GflHeapAllocMemoryAlign( heap_base, size, size_align );
    s32 read_ret = arc_file->LoadData( arc_data_id, buffer );
    if( read_ret < 0 )
    {
      GflHeapFreeMemory(buffer);
      return false;
    }
  }

  // ここまで来たら正常に読み込めている
  return AttachBuffer(allocator, buffer, description, true);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         グラフィックスリソースファイルのアンロード
 *
 *  @retval        bool    falseのときはアンロード失敗。
 *                         (失敗の理由はロードしたデータが存在しない)
 */
//-----------------------------------------------------------------------------
bool Resource::Unload(void)
{
  bool ret = true;
  if( m_nw_res_graphics_file )
  {
    nw::ut::SafeCleanup( *m_nw_res_graphics_file );
    GFL_DELETE m_nw_res_graphics_file;
  }
  else
  {
    ret = false;
  }
  if( m_buffer )
  {
    GflHeapFreeMemory( const_cast<void*>(m_buffer) );
  }
  else
  {
    ret = false;
  }

  m_nw_res_graphics_file = NULL;
  m_nw_result = nw::gfx::Result();
  m_is_self_setup = false;
  m_is_setup = false;
  m_description = Description();
  m_buffer = NULL;
  m_own_buffer = false;

  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         グラフィックスリソースファイルを読み込んだバッファのアタッチ
 *                 バッファのアタッチだけでファイル中のコンテントのセットアップはしない。
 *
 *  @param[in]     allocator    アロケータ
 *  @param[in]     buffer       グラフィックスリソースを読み込んだバッファのアドレス
 *  @param[in]     description  リソースの設定内容
 *                              (バッファをアタッチしたときのみ使用する。
 *                               NULLのときはデフォルト値。
 *                               呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
 *  @param[in]     own_buffer   tureのときbufferの管理はこのインスタンスが行うので呼び出し元はbufferを解放してはならない。
 *                              falseのときbufferの管理は呼び出し元で行って下さい。
 *                              (バッファをアタッチしたときのみ使用する)
 *
 *  @retval        bool    falseのときはバッファのアタッチ失敗。
 *                         (失敗の理由は
 *                          allocatorが不正か、bufferが不正か、既にロードした他のデータが存在するか、
 *                          メモリ不足のどれか)
 */
//-----------------------------------------------------------------------------
bool Resource::AttachBuffer(gfl::heap::NwAllocator* allocator, const void* buffer, const Description* description, bool own_buffer)
{
  if( allocator              == NULL ) return false;
  if( buffer                 == NULL ) return false;
  //if( m_buffer               != NULL ) return false;  m_nw_res_graphics_fileで確認しているのでm_bufferの確認は要らない。
  if( m_nw_res_graphics_file != NULL ) return false;

  gfl::heap::HeapBase* heap_base = allocator->GetHeapBase();
  
  // ここまで来たら正常に読み込めている
  m_nw_res_graphics_file = GFL_NEW(heap_base) nw::gfx::ResGraphicsFile(buffer);
  m_buffer               = buffer;
  m_own_buffer           = own_buffer;

  // 構築
  if( description ) m_description = *description; 

  // ForeachTexture, ForeachIndexStream, ForeachVertexStream で
  // ResGraphicsFile 内の ResTexture, ResIndexStream, ResVertexStream に関数オブジェクトを設定します。
  // 関数オブジェクトは SetLocationFlag でメモリの配置場所を指定します。
  //
  m_nw_res_graphics_file->ForeachTexture(nw::gfx::TextureLocationFlagSetter(m_description.texture_location_flag));
  m_nw_res_graphics_file->ForeachIndexStream(nw::gfx::IndexStreamLocationFlagSetter(m_description.index_stream_location_flag));
  m_nw_res_graphics_file->ForeachVertexStream(nw::gfx::VertexStreamLocationFlagSetter(m_description.vertex_stream_location_flag));

  return true;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         グラフィックスリソースファイルを読み込んだバッファのデタッチ
 *
 *  @retval        bool    falseのときはバッファのデタッチ失敗。
 *                         (失敗の理由はバッファをアタッチしたデータが存在しない)
 */
//-----------------------------------------------------------------------------
bool Resource::DetachBuffer(void)
{
  bool ret = true;
  if( m_nw_res_graphics_file )
  {
    nw::ut::SafeCleanup( *m_nw_res_graphics_file );
    GFL_DELETE m_nw_res_graphics_file;
  }
  else
  {
    ret = false;
  }

  GFL_ASSERT_MSG( !m_own_buffer, "このインスタンスが管理しているバッファをデタッチしようとしています。\n" );
  if( m_buffer )
  {
    m_buffer = NULL;
  }
  else
  {
    ret = false;
  }

  m_nw_res_graphics_file = NULL;
  m_nw_result = nw::gfx::Result();
  m_is_self_setup = false;
  m_is_setup = false;
  m_description = Description();
  m_buffer = NULL;
  m_own_buffer = false;

  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ファイル中のコンテントのセットアップ
 *                 セットアップが完了するまで、other_resourceを変更して何度呼び出してもよい。
 *
 *  @param[in]     allocator         アロケータ
 *  @param[in]     other_resource    セットアップに必要な他のリソース
 *  @param[in]     common_resource_flag  使用する共通リソースがある場合はビットを立てておく
 *
 *  @retval        bool   trueのときはセットアップ完了。
 *                        falseのときはセットアップがまだ完了していない。その理由はGetResultで確認。
 */
//-----------------------------------------------------------------------------
bool Resource::Setup(gfl::heap::NwAllocator* allocator, Resource* other_resource, u32 common_resource_flag)
{
  if( !m_is_setup )
  {
    // リソースのセットアップを行います。リソースのセットアップでは以下のような処理を行います。
    // ・シェーダーやテクスチャの参照解決
    // ・テクスチャや頂点のVRAM転送
    // ・マテリアルのコマンド生成
    // 
    // Setup の戻り値 Result は ResourceResult であり、参照解決エラーなどを返します。
    // エラーコードが返ってきた場合は引数に必要な ResGraphicsFile を与えて再度 Setup することでエラーを解消できます。
    // Setup を行ったリソースは Cleanup を呼び出して内部で確保したメモリなどを破棄する必要があります。
    //
    if(other_resource)
    {
      if( !m_is_self_setup )
      {
        m_nw_result = m_nw_res_graphics_file->Setup(allocator);
        m_is_self_setup = true;
      }
      m_nw_result = m_nw_res_graphics_file->Setup(allocator, *(other_resource->GetNwResGraphicsFile()));
    }
    else
    {
      m_nw_result = m_nw_res_graphics_file->Setup(allocator);
      m_is_self_setup = true;
    }
    if( m_nw_result.IsSuccess() )
    {
      m_is_setup = true;
    }
    else
    {
      if( common_resource_flag )
      {
        do
        {
          Resource* common_resource;
          
          // 共通リソース
          {
            if( common_resource_flag & COMMON_RESOURCE_DEFAULT_SHADER_BIT )
            {
              common_resource = GFL_SINGLETON_INSTANCE(System)->GetCommonDataDefaultShaderResource();
              if(common_resource)
              {
                m_nw_result = m_nw_res_graphics_file->Setup(allocator, *(common_resource->GetNwResGraphicsFile()));
                if( m_nw_result.IsSuccess() )
                {
                  m_is_setup = true;
                  break;
                }
              }
              else
              {
                GFL_ASSERT_MSG(0, "共通リソース0x%X(ビット)がありません。\n", COMMON_RESOURCE_DEFAULT_SHADER_BIT);
              }
            }
            if( common_resource_flag & COMMON_RESOURCE_PARTICLE_DEFAULT_SHADER_BIT )
            {
              common_resource = GFL_SINGLETON_INSTANCE(System)->GetCommonDataParticleDefaultShaderResource();
              if(common_resource)
              {
                m_nw_result = m_nw_res_graphics_file->Setup(allocator, *(common_resource->GetNwResGraphicsFile()));
                if( m_nw_result.IsSuccess() )
                {
                  m_is_setup = true;
                  break;
                }
              }
              else
              {
                GFL_ASSERT_MSG(0, "共通リソース0x%X(ビット)がありません。\n", COMMON_RESOURCE_PARTICLE_DEFAULT_SHADER_BIT);
              }
            }
            if( common_resource_flag & COMMON_RESOURCE_NW_DEFAULT_SHADER_BIT )
            {
              common_resource = GFL_SINGLETON_INSTANCE(System)->GetCommonDataNwDefaultShaderResource();
              if(common_resource)
              {
                m_nw_result = m_nw_res_graphics_file->Setup(allocator, *(common_resource->GetNwResGraphicsFile()));
                if( m_nw_result.IsSuccess() )
                {
                  m_is_setup = true;
                  break;
                }
              }
              else
              {
                GFL_ASSERT_MSG(0, "共通リソース0x%X(ビット)がありません。\n", COMMON_RESOURCE_NW_DEFAULT_SHADER_BIT);
              }
            }
          }

          // Ex共通リソース
          {
            static const u32 ex_common_resource_bit_array[] =
            {
              EX_COMMON_RESOURCE_BIT_00,
              EX_COMMON_RESOURCE_BIT_01,
              EX_COMMON_RESOURCE_BIT_02,
              EX_COMMON_RESOURCE_BIT_03,
              EX_COMMON_RESOURCE_BIT_04,
              EX_COMMON_RESOURCE_BIT_05,
              EX_COMMON_RESOURCE_BIT_06,
              EX_COMMON_RESOURCE_BIT_07,
            };
            for(s32 i=0; i<sizeof(ex_common_resource_bit_array)/sizeof(ex_common_resource_bit_array[0]); ++i)
            {
              if( common_resource_flag & ex_common_resource_bit_array[i] )
              {
                common_resource =  GFL_SINGLETON_INSTANCE(System)->GetExCommonDataExCommonResource(ex_common_resource_bit_array[i]);
                if(common_resource)
                {
                  m_nw_result = m_nw_res_graphics_file->Setup(allocator, *(common_resource->GetNwResGraphicsFile()));
                  if( m_nw_result.IsSuccess() )
                  {
                    m_is_setup = true;
                    break;
                  }
                }
                else
                {
                  GFL_ASSERT_MSG(0, "共通リソース0x%X(ビット)がありません。\n", ex_common_resource_bit_array[i]);
                }
              }
            }
            // do{}while(0);の最後なので、ここでのif(m_is_setup)break;は要らない。
          }

        }
        while(0);
      }
    }
  }
  return m_is_setup;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         グラフィックスリソースファイルのロード
 *                 ファイル中のコンテントのセットアップ
 *                 Loadの後にSetupを行う。
 *
 *  @param[in]     allocator    アロケータ
 *  @param[in]     path         グラフィックスリソースファイルのパス名
 *  @param[in]     description  リソースの設定内容
 *                              (ロードしたときのみ使用する。
 *                               NULLのときはデフォルト値。
 *                               呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
 *  @param[in]     common_resource_flag  使用する共通リソースがある場合はビットを立てておく
 *
 *  @retval        bool   trueのときはロードに成功し、セットアップが完了している。
 *                        falseのときはロードに失敗したかセットアップが完了していない。
 *                        GetBufferの戻り値がNULLのときはロード失敗、NULLでないときはセットアップが完了していない。
 */
//-----------------------------------------------------------------------------
bool Resource::LoadAndSetup(gfl::heap::NwAllocator* allocator, const char* path, const Description* description, u32 common_resource_flag)
{
  bool ret = Load(allocator, path, description);
  if( ret )
  {
    return Setup(allocator, NULL, common_resource_flag);
  }
  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アーカイブファイルからデータを読み込む
 *                 ファイル中のコンテントのセットアップ
 *                 Loadの後にSetupを行う。
 *
 *  @param[in]     allocator    アロケータ
 *  @param[in]     arc_file     アーカイブファイル
 *                              (GetBufferの戻り値がNULLのときはロード失敗)
 *  @param[in]     arc_data_id  アーカイブファイル内のデータID
 *  @param[in]     description  リソースの設定内容
 *                              (読み込んだときのみ使用する。
 *                               NULLのときはデフォルト値。
 *                               呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
 *  @param[in]     common_resource_flag  使用する共通リソースがある場合はビットを立てておく
 *  @param[in]     is_compress  圧縮データの場合はtrue
 *
 *  @retval        bool   trueのときはロードに成功し、セットアップが完了している。
 *                        falseのときはロードに失敗したかセットアップが完了していない。
 *                        GetBufferの戻り値がNULLのときはロード失敗、NULLでないときはセットアップが完了していない。
 */
//-----------------------------------------------------------------------------
bool Resource::LoadAndSetup(
    gfl::heap::NwAllocator* allocator,
    gfl::fs::ArcFile* arc_file,
    u32 arc_data_id,
    const Description* description,
    u32 common_resource_flag,
    const b32 is_compress )
{
  bool ret = Load(allocator, arc_file, arc_data_id, description, is_compress);
  if( ret )
  {
    return Setup(allocator, NULL, common_resource_flag);
  }
  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         グラフィックスリソースファイルのアタッチ
 *                 ファイル中のコンテントのセットアップ
 *                 Attachの後にSetupを行う。
 *
 *  @param[in]     allocator    アロケータ
 *  @param[in]     buffer       グラフィックスリソースを読み込んだバッファのアドレス
 *  @param[in]     description  リソースの設定内容
 *                              (バッファをアタッチしたときのみ使用する。
 *                               NULLのときはデフォルト値。
 *                               呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
 *  @param[in]     own_buffer   tureのときbufferの管理はこのインスタンスが行うので呼び出し元はbufferを解放してはならない。
 *                              falseのときbufferの管理は呼び出し元で行って下さい。
 *                              (バッファをアタッチしたときのみ使用する)
 *  @param[in]     common_resource_flag  使用する共通リソースがある場合はビットを立てておく
 *
 *  @retval        bool   trueのときはバッファのアタッチに成功し、セットアップが完了している。
 *                        falseのときはバッファのアタッチに失敗したかセットアップが完了していない。
 *                        GetBufferの戻り値がNULLのときはバッファのアタッチ失敗、NULLでないときはセットアップが完了していない。
 */
//-----------------------------------------------------------------------------
bool Resource::AttachBufferAndSetup(gfl::heap::NwAllocator* allocator, const void* buffer, const Description* description, bool own_buffer, u32 common_resource_flag)
{
  bool ret = AttachBuffer(allocator, buffer, description, own_buffer);
  if( ret )
  {
    return Setup(allocator, NULL, common_resource_flag);
  }
  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         リソース内の全ての参照オブジェクトを巡回して、未解決の参照オブジェクトの名前を出力する
 *                 GFL_DEBUGが0のときは何もしません。
 */
//-----------------------------------------------------------------------------
void Resource::PrintUnresolvedReference(void) const
{
#if GFL_DEBUG
  //GFL_PRINT("Resource::PrintUnresolvedReference-----------\n");
  if( !m_is_setup )
  {
    int result_description = m_nw_result.GetDescription();
    GFL_PRINT("Result Code=%d, Description=%d\n", m_nw_result.GetCode(), result_description );
  
    // m_nw_res_graphics内にモデルがある場合、それに未解決の参照がある場合、その旨を出力する。  // @note 参照テーブルを使用するマテリアル、ライトも対象に加えたい。
    {
      struct ResultDescriptionPack
      {
        int bit_flag;
        const char* message;
      };
      static const ResultDescriptionPack result_description_pack[] =
      {
        { nw::gfx::RESOURCE_RESULT_OK,                                "Setup が成功したことを表します。" },
        { nw::gfx::RESOURCE_RESULT_NOT_FOUND_TEXTURE,                 "テクスチャ参照解決が失敗したことを表します。" },
                                                                      // このエラーを出すのはgfx_ResTextureMapper.cppとanim_ResAnim.cpp
        { nw::gfx::RESOURCE_RESULT_NOT_FOUND_SHADER,                  "シェーダーの参照解決が失敗したことを表します。" },
        { nw::gfx::RESOURCE_RESULT_NOT_FOUND_LUT,                     "参照テーブルの参照解決が失敗したことを表します。" },
        { nw::gfx::RESOURCE_RESULT_IRRELEVANT_LOCATION_LUT,           "参照テーブルの有効フラグと実際に参照テーブルが一致しないことを表します。" },
        { nw::gfx::RESOURCE_RESULT_IRRELEVANT_LOCATION_SHADER_SYMBOL, "シェーダーシンボルがシェーダーバイナリに存在しないことを表します。" },
        { nw::gfx::RESOURCE_RESULT_IRRELEVANT_TEXTURE_MAPPING_METHOD, "カメラキューブ座標のマッピング方法でキューブテクスチャが存在しないことを表します。" },
      };
      s32 pack_num = sizeof(result_description_pack)/sizeof(result_description_pack[0]);
      for( s32 i=0; i<pack_num; ++i )
      {
        if(result_description & result_description_pack[i].bit_flag) GFL_PRINT("%s\n", result_description_pack[i].message);
      }

      // モデル
      {
        s32 count = m_nw_res_graphics_file->GetModelsCount();
        for( s32 i=0; i<count; ++i )
        {
          nw::gfx::res::ResModel res_model = m_nw_res_graphics_file->GetModels(i);
          ec::ErrorCheck::CheckModelTextureReference(res_model);
          ec::ErrorCheck::CheckModelShaderReference(res_model);
          ec::ErrorCheck::CheckModelLookupTableReference(res_model);
        }
      }
    }

    // m_nw_res_graphics内にテクスチャ、シェーダ、参照テーブルがある場合、それが未解決の参照オブジェクトなら、その旨を出力する。
    {
      m_nw_res_graphics_file->ForeachUnresolvedReferenceTexture(UnresolvedReferenceFunctor());
      m_nw_res_graphics_file->ForeachUnresolvedReferenceShader(UnresolvedReferenceFunctor());
      m_nw_res_graphics_file->ForeachUnresolvedReferenceLut(UnresolvedReferenceFunctor());
    }
  }
  //GFL_PRINT("---------------------------------------------\n");
#endif
}

//-----------------------------------------------------------------------------
/*
 *  @brief         このリソースのメモリサイズを、ヘッダー部だけのメモリサイズにする
 *
 *                 実データ部をVRAMに配置しているのでデバイスメモリにはヘッダー部だけ残しておけばいい、
 *                 だから実データ部をデバイスメモリから削除しよう、というときに使用して下さい。
 *
 *                 Setupが完了した後に呼んで下さい。
 *                 このリソースに含まれる全てのデータをVRAMに配置している場合のみ、使用可能です。
 *                 1つでもFCRAMに配置している場合は使用しないで下さい。
 *
 *                 読み込みをこのリソースクラスで行っているとき(Load系の関数を利用したとき)は、
 *                 読み込みに使用したアロケータを渡して下さい。
 *                 そのアロケータから確保しているバッファのサイズをヘッダー部のサイズにまで縮小します。
 *
 *                 自分でバッファを確保しこのリソースクラスにアタッチしたとき(AttachBuffer系の関数を利用したとき)は、
 *                 そのバッファを確保したアロケータを渡して下さい。
 *                 そのアロケータから確保しているバッファのサイズをヘッダー部のサイズにまで縮小します。
 *
 *  @param[in]     allocator    アロケータ
 */
//-----------------------------------------------------------------------------
void Resource::DeleteImageBlockData( gfl::heap::NwAllocator* allocator )
{
  GFL_ASSERT_MSG(m_is_self_setup, "Setupが完了していません。\n");  //@check リンクされてないリソースがあるかもしれないが出る

  u32 start_addr;
  u32 image_addr;
  u32 header_size;
  u32 file_size;

  start_addr = reinterpret_cast<u32>(m_buffer);
  image_addr = reinterpret_cast<u32>(m_nw_res_graphics_file->GetImageBlockData());

  header_size = image_addr - start_addr;
  file_size = m_nw_res_graphics_file->GetFileSize();

#if GFL_GRP_G3D_RESOURCE_PRINT
  GFL_PRINT( "[DeleteImageBlockData] header_size=0x%x file_size=0x%x\n", header_size, file_size );
#endif

  allocator->GetHeapBase()->ResizeMemory( const_cast<void*>(m_buffer), header_size );
}

//-----------------------------------------------------------------------------
/*
 *  @brief         モデルリソースの要素数を取得する
 * 
 *  @retval        s32    モデルリソースの要素数
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetModelsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetModelsCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         カメラリソースの要素数を取得する
 * 
 *  @retval        s32    カメラリソースの要素数
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetCamerasCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetCamerasCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ライトリソースの要素数を取得する
 * 
 *  @retval        s32    ライトリソースの要素数
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetLightsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetLightsCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         フォグリソースの要素数を取得する
 * 
 *  @retval        s32    フォグリソースの要素数
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetFogsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetFogsCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         シーン環境設定リソースの要素数を取得する
 * 
 *  @retval        s32    シーン環境設定リソースの要素数
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetSceneEnvironmentSettingsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetSceneEnvironmentSettingsCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         スケルタルアニメーションリソースの要素数を取得する
 *
 *  @retval        s32    スケルタルアニメーションリソースの要素数
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetSkeletalAnimsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetSkeletalAnimsCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         マテリアルアニメーションリソースの要素数を取得する
 *
 *  @retval        s32    マテリアルアニメーションリソースの要素数
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetMaterialAnimsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetMaterialAnimsCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ビジビリティアニメーションリソースの要素数を取得する
 *
 *  @retval        s32    ビジビリティアニメーションリソースの要素数
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetVisibilityAnimsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetVisibilityAnimsCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         カメラアニメーションリソースの要素数を取得する
 *
 *  @retval        s32    カメラアニメーションリソースの要素数
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetCameraAnimsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetCameraAnimsCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ライトアニメーションリソースの要素数を取得する
 *
 *  @retval        s32    ライトアニメーションリソースの要素数
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetLightAnimsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetLightAnimsCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         テクスチャリソースの要素数を取得する
 * 
 *  @retval        s32    テクスチャリソースの要素数
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetTexturesCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetTexturesCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         シェーダーリソースの要素数を取得する
 * 
 *  @retval        s32    シェーダーリソースの要素数
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetShadersCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetShadersCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         エミッターリソースの要素数を取得する
 * 
 *  @retval        s32    エミッターリソースの要素数
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetEmittersCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetEmittersCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ルックアップテーブルセットリソースの要素数を取得する
 *
 *  @retval        s32    ルックアップテーブルセットリソースの要素数
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetLutSetsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetLutSetsCount();
  else
    return 0;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         リソースのインデックス番号を取得する
 * 
 *  @param[in]     key  リソースの名前
 * 
 *  @retval        s32    リソースのインデックス番号
 */
//-----------------------------------------------------------------------------
s32 Resource::GetModelsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetModelsIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetCamerasIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetCamerasIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetLightsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetLightsIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetFogsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetFogsIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetSceneEnvironmentSettingsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetSceneEnvironmentSettingsIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetSkeletalAnimsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetSkeletalAnimsIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetMaterialAnimsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetMaterialAnimsIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetVisibilityAnimsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetVisibilityAnimsIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetCameraAnimsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetCameraAnimsIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetLightAnimsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetLightAnimsIndex(key);
    if( i >= 0 ) return i;
  }
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetTexturesIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetTexturesIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetShadersIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetShadersIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetEmittersIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetEmittersIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetLutSetsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetLutSetsIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         メンバーアニメーションの要素数を取得する
 * 
 *  @param[in]     index_in_resource  リソース内におけるインデックス
 * 
 *  @retval        s32    メンバーアニメーションの要素数
 */
//-----------------------------------------------------------------------------
s32 Resource::GetSkeletalMemberAnimSetCount(const s32 index_in_resource) const
{
  if( m_nw_res_graphics_file )
  {
    nw::anim::res::ResAnim res_anim = m_nw_res_graphics_file->GetSkeletalAnims(index_in_resource);
    if( res_anim.IsValid() )
    {
      return res_anim.GetMemberAnimSetCount();
    }
  }
  return 0;
}
s32 Resource::GetMaterialMemberAnimSetCount(const s32 index_in_resource) const
{
  if( m_nw_res_graphics_file )
  {
    nw::anim::res::ResAnim res_anim = m_nw_res_graphics_file->GetMaterialAnims(index_in_resource);
    if( res_anim.IsValid() )
    {
      return res_anim.GetMemberAnimSetCount();
    }
  }
  return 0;
}
s32 Resource::GetVisibilityMemberAnimSetCount(const s32 index_in_resource) const
{
  if( m_nw_res_graphics_file )
  {
    nw::anim::res::ResAnim res_anim = m_nw_res_graphics_file->GetVisibilityAnims(index_in_resource);
    if( res_anim.IsValid() )
    {
      return res_anim.GetMemberAnimSetCount();
    }
  }
  return 0;
}
s32 Resource::GetCameraMemberAnimSetCount(const s32 index_in_resource) const
{
  if( m_nw_res_graphics_file )
  {
    nw::anim::res::ResAnim res_anim = m_nw_res_graphics_file->GetCameraAnims(index_in_resource);
    if( res_anim.IsValid() )
    {
      return res_anim.GetMemberAnimSetCount();
    }
  }
  return 0;
}
s32 Resource::GetLightMemberAnimSetCount(const s32 index_in_resource) const
{
  if( m_nw_res_graphics_file )
  {
    nw::anim::res::ResAnim res_anim = m_nw_res_graphics_file->GetLightAnims(index_in_resource);
    if( res_anim.IsValid() )
    {
      return res_anim.GetMemberAnimSetCount();
    }
  }
  return 0;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションリソースを取得する
 * 
 *  @param[in]     type               アニメーションリソースのタイプ
 *  @param[in]     index_in_resource  リソース内におけるインデックス
 *  @param[out]    res_anim           取得したアニメーションリソースの代入先
 */
//-----------------------------------------------------------------------------
void Resource::GetResAnim(const AnimationResourceType type, const s32 index_in_resource, ResAnim* res_anim) const
{
  if( m_nw_res_graphics_file )
  {  
    nw::anim::ResAnim res;
    
    switch( type )
    {
    case ANIMATION_RESOURCE_TYPE_NONE:
      {
        // 何もしない
      }
      break;
    case ANIMATION_RESOURCE_TYPE_SKELETAL:
      {
        res = m_nw_res_graphics_file->GetSkeletalAnims(index_in_resource);
      }
      break;
    case ANIMATION_RESOURCE_TYPE_MATERIAL:
      {
        res = m_nw_res_graphics_file->GetMaterialAnims(index_in_resource);
      }
      break;
    case ANIMATION_RESOURCE_TYPE_VISIBILITY:
      {
        res = m_nw_res_graphics_file->GetVisibilityAnims(index_in_resource);
      }
      break;
    case ANIMATION_RESOURCE_TYPE_CAMERA:
      {
        res = m_nw_res_graphics_file->GetCameraAnims(index_in_resource);
      }
      break;
    case ANIMATION_RESOURCE_TYPE_LIGHT:
      {
        res = m_nw_res_graphics_file->GetLightAnims(index_in_resource);
      }
      break;
    case ANIMATION_RESOURCE_TYPE_ALL:
    default:
      {
        GFL_ASSERT_MSG( 0, "アニメーションリソースのタイプ%dが不正です。\n", type );
      }
      break;
    }

    res_anim->SetNwResAnim(type, res);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "m_nw_res_graphics_fileがNULLです。\n" );
  }
}
 

#if GFL_DEBUG
//-----------------------------------------------------------------------------
/*
 *  @brief    アニメーションの内容確認
 */
//-----------------------------------------------------------------------------
void Resource::DEBUG_PrintAnimResult(const AnimationResourceType type, const s32 index_in_resource)
{
  GFL_PRINT("Resource::DEBUG_PrintAnimResult\n");

  if( m_nw_res_graphics_file )
  {
    nw::anim::res::ResAnim res_anim;
    switch(type)
    {
    case ANIMATION_RESOURCE_TYPE_SKELETAL:
      res_anim = m_nw_res_graphics_file->GetSkeletalAnims(index_in_resource);  break;
    case ANIMATION_RESOURCE_TYPE_MATERIAL:
      res_anim = m_nw_res_graphics_file->GetMaterialAnims(index_in_resource);  break;
    case ANIMATION_RESOURCE_TYPE_VISIBILITY:
      res_anim = m_nw_res_graphics_file->GetVisibilityAnims(index_in_resource);  break;
    case ANIMATION_RESOURCE_TYPE_CAMERA:
      res_anim = m_nw_res_graphics_file->GetCameraAnims(index_in_resource);  break;
    case ANIMATION_RESOURCE_TYPE_LIGHT:
      res_anim = m_nw_res_graphics_file->GetLightAnims(index_in_resource);  break;
    }

    if( res_anim.IsValid() )
    {
      b32 full_baked_anim = res_anim.IsFullBakedAnim();
      s32 member_anim_set_count = res_anim.GetMemberAnimSetCount();
      f32 frame_size = res_anim.GetFrameSize();
      const char* target_anim_group_name = res_anim.GetTargetAnimGroupName();

      GFL_PRINT("full_baked_anim=%d\n", full_baked_anim);
      GFL_PRINT("member_anim_set_count=%d\n", member_anim_set_count);
      GFL_PRINT("frame_size=%f\n", frame_size);
      if( target_anim_group_name ) GFL_PRINT("target_anim_group_name=%s\n", target_anim_group_name);
      
      for( s32 i=0; i<member_anim_set_count; ++i )
      {
        nw::anim::res::ResMemberAnim res_member_anim = res_anim.GetMemberAnimSet(i);
        
        if( res_member_anim.IsValid() )
        {
          GFL_PRINT("member_anim_set_index[%d]\n", i);

          u32 flags = res_member_anim.GetFlags();
          const char* path = res_member_anim.GetPath();
          u32 primitive_type = res_member_anim.GetPrimitiveType();

          GFL_PRINT("flags=%d\n", flags);
          if( path ) GFL_PRINT("path=%s\n", path);
          GFL_PRINT("primitive_type=%d\n", primitive_type);

          if(    primitive_type != nw::anim::res::ResMemberAnim::PRIMITIVETYPE_BAKED_TRANSFORM
              && primitive_type != nw::anim::res::ResMemberAnim::PRIMITIVETYPE_FULL_BAKED )
          {
            // NintendoWare/CTR/sources/libraries/anim/res/anim_ResAnim.cpp
            // ResMemberAnim::GetPrimitiveSize
            // ResMemberAnim::EvaluateResultForType
            // は
            // nw::anim::res::ResMemberAnim::PrimitiveType
            // によってはNW_ASSERTにひっかかってしまうので
            // ひっかかるPrimitiveTypeでは使わないようにしておく。

            s32 primitive_size = res_member_anim.GetPrimitiveSize();
            
            GFL_PRINT("primitive_size=%d\n", primitive_size);

            static s32 originalValueNone = 0;
 
            void* dstBuf        = NULL;
            bit32 dstFlags      = 0;
            void* originalValue = &originalValueNone;
 
            // NintendoWare/CTR/include/nw/anim/res/anim_ResAnim.h
            // NintendoWare/CTR/sources/libraries/anim/res/anim_ResAnim.cpp
            // を参考にした。
            switch(primitive_type)
            {
            case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_FLOAT:
              {
                GFL_ASSERT_MSG(primitive_size==sizeof(float), "ライブラリの型が変更された可能性があります。\n");  //@check デバッグ用関数内
                static float dstBufFloat;
                dstBuf = &dstBufFloat;
              }
              break;
            case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_INT:
              {
                GFL_ASSERT_MSG(primitive_size==sizeof(s32), "ライブラリの型が変更された可能性があります。\n");  //@check デバッグ用関数内
                static s32 dstBufInt;
                dstBuf = &dstBufInt;
              }
              break;
            case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_BOOL:
              {
                GFL_ASSERT_MSG(primitive_size==sizeof(bool), "ライブラリの型が変更された可能性があります。\n");  //@check デバッグ用関数内
                static bool dstBufBool;
                dstBuf = &dstBufBool;
              }
              break;
            case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_VECTOR2:
              {
                GFL_ASSERT_MSG(primitive_size==sizeof(nw::math::VEC2), "ライブラリの型が変更された可能性があります。\n");  //@check デバッグ用関数内
                static nw::math::VEC2 dstBufVector2;
                dstBuf = &dstBufVector2;
              }
              break;
            case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_VECTOR3:
              {
                GFL_ASSERT_MSG(primitive_size==sizeof(nw::math::VEC3), "ライブラリの型が変更された可能性があります。\n");  //@check デバッグ用関数内
                static nw::math::VEC3 dstBufVector3;
                dstBuf = &dstBufVector3;
              }
              break;
            case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_TRANSFORM:
              {
                GFL_ASSERT_MSG(primitive_size==sizeof(nw::gfx::CalculatedTransform), "ライブラリの型が変更された可能性があります。\n");  //@check デバッグ用関数内
                // この型のみOriginalValueが必要
                static nw::gfx::CalculatedTransform dstBufTransform;
                static nw::math::Transform3         originalValueTransform(gfl::math::VEC3(1.0f,1.0f,1.0f), gfl::math::VEC3(0.0f,0.0f,0.0f), gfl::math::VEC3(0.0f,0.0f,0.0f));
                dstBuf = &dstBufTransform;
                originalValue = &originalValueTransform;
              }
              break;
            case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_RGBA_COLOR:
              {
                GFL_ASSERT_MSG(primitive_size==sizeof(nw::ut::ResFloatColor), "ライブラリの型が変更された可能性があります。\n");  //@check デバッグ用関数内
                static nw::ut::ResFloatColor dstBufRgbaColor;  // NintendoWare/CTR/include/nw/ut/ut_Color.h
                dstBuf = &dstBufRgbaColor;
              }
              break;
            case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_TEXTURE:
              {
                GFL_ASSERT_MSG(primitive_size==sizeof(nw::ut::Offset), "ライブラリの型が変更された可能性があります。\n");  //@check デバッグ用関数内
                static nw::ut::Offset dstBufTexture;  // NintendoWare/CTR/include/nw/ut/ut_ResType.h
                dstBuf = &dstBufTexture;
              }
              break;
            default:
              {
                GFL_ASSERT_MSG(0, "primitive_type=%dは不明なタイプです。\n", primitive_type);
              }
              break;
            }
 
            s32 frame_size_s32 = static_cast<s32>(frame_size);
            for( s32 frame_s32=0; frame_s32<frame_size_s32; ++frame_s32 )
            {
              dstFlags = res_member_anim.EvaluateResultForType(
                  dstBuf,
                  dstFlags,
                  static_cast<f32>(frame_s32),
                  originalValue
              );
 
              switch(primitive_type)
              {
              case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_FLOAT:
                {
                  float* dstBufFloat = reinterpret_cast<float*>(dstBuf);
                  GFL_PRINT("Frame=%3d FLOAT [%d]%f\n",
                      frame_s32,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0,
                      *dstBufFloat
                  );
                }
                break;
              case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_INT:
                {
                  s32* dstBufInt = reinterpret_cast<s32*>(dstBuf);
                  GFL_PRINT("Frame=%3d INT [%d]%d\n",
                      frame_s32,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0,
                      *dstBufInt
                  );
                }
                break;
              case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_BOOL:
                {
                  bool* dstBufBool = reinterpret_cast<bool*>(dstBuf);
                  GFL_PRINT("Frame=%3d BOOL [%d]%d\n",
                      frame_s32,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0,
                      *dstBufBool
                  );
                }
                break;
              case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_VECTOR2:
                {
                  nw::math::VEC2* dstBufVector2 = reinterpret_cast<nw::math::VEC2*>(dstBuf);
                  GFL_PRINT("Frame=%3d VECTOR2 [%d]%f, [%d]%f\n",
                      frame_s32,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0,
                      dstBufVector2->x,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT1,
                      dstBufVector2->y
                  );
                }
                break;
              case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_VECTOR3:
                {
                  nw::math::VEC3* dstBufVector3 = reinterpret_cast<nw::math::VEC3*>(dstBuf);
                  GFL_PRINT("Frame=%3d VECTOR3 [%d]%f, [%d]%f, [%d]%f\n",
                      frame_s32,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0,
                      dstBufVector3->x,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT1,
                      dstBufVector3->y,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT2,
                      dstBufVector3->z
                  );
                }
                break;
              case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_TRANSFORM:
                {
                  nw::gfx::CalculatedTransform* dstBufTransform = reinterpret_cast<nw::gfx::CalculatedTransform*>(dstBuf);
                 
                  gfl::math::VEC3          translate;               dstBufTransform->GetTranslate(&translate);
                  const gfl::math::VEC3&   scale                    = dstBufTransform->Scale();
                  nw::math::VEC3&          direct_scale             = dstBufTransform->DirectScale();  // gfl::math::VEC3&だと
                      // a reference of type "gfl::math::VEC3 &" (not const-qualified) cannot be initialized with a value of type "nn::math::VEC3"
                      // というコンパイルエラーになってしまう。
                  const gfl::math::MTX34&  transform_matrix         = dstBufTransform->TransformMatrix();
                  gfl::math::MTX34&        direct_transform_matrix  = dstBufTransform->DirectTransformMatrix();
                  
                  GFL_PRINT("Frame=%3d TRANSFORM [0x%X] Translate(%f,%f,%f),\n",
                      frame_s32,
                      dstFlags,
                      translate.x, translate.y, translate.z
                  );
                  GFL_PRINT("Scale(%f,%f,%f), DirectScale(%f,%f,%f)\n",
                      scale.x, scale.y, scale.z,
                      direct_scale.x, direct_scale.y, direct_scale.z
                  );
                  GFL_PRINT("TransformMatirx\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n",
                      transform_matrix.f._00, transform_matrix.f._01, transform_matrix.f._02, transform_matrix.f._03,
                      transform_matrix.f._10, transform_matrix.f._11, transform_matrix.f._12, transform_matrix.f._13,
                      transform_matrix.f._20, transform_matrix.f._21, transform_matrix.f._22, transform_matrix.f._23
                  );
                  GFL_PRINT("DirectTransformMatirx\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n",
                      direct_transform_matrix.f._00, direct_transform_matrix.f._01, direct_transform_matrix.f._02, direct_transform_matrix.f._03,
                      direct_transform_matrix.f._10, direct_transform_matrix.f._11, direct_transform_matrix.f._12, direct_transform_matrix.f._13,
                      direct_transform_matrix.f._20, direct_transform_matrix.f._21, direct_transform_matrix.f._22, direct_transform_matrix.f._23
                  );
                }
                break;
              case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_RGBA_COLOR:
                {
                  nw::ut::ResFloatColor* dstBufRgbaColor = reinterpret_cast<nw::ut::ResFloatColor*>(dstBuf);  // NintendoWare/CTR/include/nw/ut/ut_Color.h
                  GFL_PRINT("Frame=%3d RGBA_COLOR [%d]%f, [%d]%f, [%d]%f, [%d]%f\n",
                      frame_s32,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0,
                      dstBufRgbaColor->r,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT1,
                      dstBufRgbaColor->g,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT2,
                      dstBufRgbaColor->b,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT3,
                      dstBufRgbaColor->a
                  );
                }
                break;
              case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_TEXTURE:
                {
                  nw::ut::Offset* dstBufTexture = reinterpret_cast<nw::ut::Offset*>(dstBuf);  // NintendoWare/CTR/include/nw/ut/ut_ResType.h
                  GFL_PRINT("Frame=%3d TEXTURE [%d]%d\n",
                      frame_s32,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0,
                      dstBufTexture->offset
                  );
                }
                break;
              default:
                {
                  GFL_ASSERT_MSG(0, "primitive_type=%dは不明なタイプです。\n", primitive_type);
                }
                break;
              }
            }  // for( s32 frame_s32=0; frame_s32<frame_size_s32; ++frame_s32 )
          }  // if(    primitive_type != 
          else if( primitive_type == nw::anim::res::ResMemberAnim::PRIMITIVETYPE_BAKED_TRANSFORM )
          {
            // NintendoWare/CTR/include/nw/ut/ut_ResUtil.h
            // nw::ut::ResDynamicCast
            // を参考にした。
            nw::anim::res::ResBakedTransformAnim res_baked_transform_anim(res_member_anim.ptr());  
            
            s32 frame_size_s32 = static_cast<s32>(frame_size);
            for( s32 frame_s32=0; frame_s32<frame_size_s32; ++frame_s32 )
            {
              b32 scale_exist = false;
              b32 rotate_exist = false;
              b32 translate_exist = false;

              nw::math::VEC3  scale;
              nw::math::MTX34 rotate;
              nw::math::MTX34 translate;
              bit32 scale_flags = 0;
              bit32 rotate_flags = 0;
              bit32 translate_flags = 0;

              if( res_baked_transform_anim.GetScale() )
              {
                scale_exist = true;
                res_baked_transform_anim.EvaluateScale(&scale, &scale_flags, static_cast<f32>(frame_s32));
              }
              if( res_baked_transform_anim.GetRotate() )
              {
                rotate_exist = true;
                res_baked_transform_anim.EvaluateRotate(&rotate, &rotate_flags, static_cast<f32>(frame_s32));
              }
              if( res_baked_transform_anim.GetTranslate() )
              {
                translate_exist = true;
                res_baked_transform_anim.EvaluateTranslate(&translate, &translate_flags, static_cast<f32>(frame_s32));
              }

              GFL_PRINT("Frame=%3d BAKED_TRANSFORM,\n",
                  frame_s32
              );
              GFL_PRINT("Scale exist=%d [0x%X] (%f,%f,%f)\n",
                  scale_exist,
                  scale_flags,
                  scale.x, scale.y, scale.z
              );
              GFL_PRINT("Rotate exist=%d [0x%X]\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n",
                  rotate_exist,
                  rotate_flags,
                  rotate.f._00, rotate.f._01, rotate.f._02, rotate.f._03,
                  rotate.f._10, rotate.f._11, rotate.f._12, rotate.f._13,
                  rotate.f._20, rotate.f._21, rotate.f._22, rotate.f._23
              );
              GFL_PRINT("Translate exist=%d [0x%X]\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n",
                  translate_exist,
                  translate_flags,
                  translate.f._00, translate.f._01, translate.f._02, translate.f._03,
                  translate.f._10, translate.f._11, translate.f._12, translate.f._13,
                  translate.f._20, translate.f._21, translate.f._22, translate.f._23
              );
            }
          }  // else if( primitive_type != 
          else if( primitive_type == nw::anim::res::ResMemberAnim::PRIMITIVETYPE_FULL_BAKED )
          {
            // NintendoWare/CTR/include/nw/ut/ut_ResUtil.h
            // nw::ut::ResDynamicCast
            // を参考にした。
            nw::anim::res::ResFullBakedAnim res_full_baked_anim(res_member_anim.ptr());
            
            s32 frame_size_s32 = static_cast<s32>(frame_size);
            for( s32 frame_s32=0; frame_s32<frame_size_s32; ++frame_s32 )
            {
              nw::math::MTX34 transform;
              res_full_baked_anim.EvaluateTransform(&transform, static_cast<f32>(frame_s32));

              GFL_PRINT("Frame=%3d FULL_BAKED,\n",
                  frame_s32
              );
              GFL_PRINT("Transform\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n",
                  transform.f._00, transform.f._01, transform.f._02, transform.f._03,
                  transform.f._10, transform.f._11, transform.f._12, transform.f._13,
                  transform.f._20, transform.f._21, transform.f._22, transform.f._23
              );
            }
          }  // else if( primitive_type != 
        }  // if( res_member_anim.IsValid() )
      }  // for( s32 i=0; i<member_anim_set_count; ++i )
    }  // if( res_anim.IsValid() )
  }  // if( m_nw_res_graphics_file )
}

s32 Resource::DEBUG_GetMemberAnimSetIndex(const AnimationResourceType type, const s32 index_in_resource, const char* key) const
{
  GFL_PRINT("Resource::DEBUG_GetMemberAnimSetIndex\n");

  enum
  {
    MEMBER_ANIM_SET_INDEX_NOT_FOUND  = -1  // 指定した名前を持つメンバーアニメーションが存在しない場合のインデックス
  };

  if( m_nw_res_graphics_file )
  {
    nw::anim::res::ResAnim res_anim;
    switch(type)
    {
    case ANIMATION_RESOURCE_TYPE_SKELETAL:
      res_anim = m_nw_res_graphics_file->GetSkeletalAnims(index_in_resource);  break;
    case ANIMATION_RESOURCE_TYPE_MATERIAL:
      res_anim = m_nw_res_graphics_file->GetMaterialAnims(index_in_resource);  break;
    case ANIMATION_RESOURCE_TYPE_VISIBILITY:
      res_anim = m_nw_res_graphics_file->GetVisibilityAnims(index_in_resource);  break;
    case ANIMATION_RESOURCE_TYPE_CAMERA:
      res_anim = m_nw_res_graphics_file->GetCameraAnims(index_in_resource);  break;
    case ANIMATION_RESOURCE_TYPE_LIGHT:
      res_anim = m_nw_res_graphics_file->GetLightAnims(index_in_resource);  break;
    }

    if( res_anim.IsValid() )
    {
      return res_anim.GetMemberAnimSetIndex(key);
    }
  }

  return MEMBER_ANIM_SET_INDEX_NOT_FOUND;
}
#endif


// @note gfl::grp::g3d::Resourceクラスが肥大化するのを防ぐためにも、テクスチャの情報の取得はgfl::grp::g3d::Textureクラスに
// 渡してから行ったほうがいいかもしれない
//-----------------------------------------------------------------------------
/*
 *  @brief         テクスチャの高さ(縦の長さ)を取得する
 * 
 *  @param[in]     index_in_resource  リソース内におけるインデックス
 *  @retval        s32                テクスチャの高さ(縦の長さ)
 */
//-----------------------------------------------------------------------------
s32 Resource::GetTextureHeight(const s32 index_in_resource) const
{
  nw::gfx::res::ResTexture      nw_res_texture       = m_nw_res_graphics_file->GetTextures(index_in_resource);
  nw::gfx::res::ResImageTexture nw_res_image_texture = nw::ut::ResDynamicCast<nw::gfx::res::ResImageTexture>(nw_res_texture);
  return nw_res_image_texture.GetHeight();
}
//-----------------------------------------------------------------------------
/*
 *  @brief         テクスチャの幅(横の長さ)を取得する
 * 
 *  @param[in]     index_in_resource  リソース内におけるインデックス
 *  @retval        s32                テクスチャの幅(横の長さ)
 */
//-----------------------------------------------------------------------------
s32 Resource::GetTextureWidth(const s32 index_in_resource) const
{
  nw::gfx::res::ResTexture      nw_res_texture       = m_nw_res_graphics_file->GetTextures(index_in_resource);
  nw::gfx::res::ResImageTexture nw_res_image_texture = nw::ut::ResDynamicCast<nw::gfx::res::ResImageTexture>(nw_res_texture);
  return nw_res_image_texture.GetWidth();
}
//-----------------------------------------------------------------------------
/*
 *  @brief         テクスチャのフォーマットを取得する
 * 
 *  @param[in]     index_in_resource  リソース内におけるインデックス
 *  @retval        s32                gfl::grp::g3d::Texture::Format
 */
//-----------------------------------------------------------------------------
s32 Resource::GetTextureFormat(const s32 index_in_resource) const
{
  nw::gfx::res::ResTexture      nw_res_texture       = m_nw_res_graphics_file->GetTextures(index_in_resource);
  nw::gfx::res::ResImageTexture nw_res_image_texture = nw::ut::ResDynamicCast<nw::gfx::res::ResImageTexture>(nw_res_texture);
  nw::gfx::res::ResPixelBasedTexture::FormatHW format = static_cast<nw::gfx::res::ResPixelBasedTexture::FormatHW>(nw_res_image_texture.GetFormatHW());
  return (static_cast<gfl::grp::g3d::Texture::Format>(format));
}
//-----------------------------------------------------------------------------
/*
 *  @brief         テクスチャの1ピクセルあたりのビット長を取得する
 * 
 *  @param[in]     index_in_resource  リソース内におけるインデックス
 *  @retval        u32                テクスチャの1ピクセルあたりのビット長
 */
//-----------------------------------------------------------------------------
u32 Resource::GetTextureBitsPerPixel(const s32 index_in_resource) const
{
  nw::gfx::res::ResTexture      nw_res_texture       = m_nw_res_graphics_file->GetTextures(index_in_resource);
  nw::gfx::res::ResImageTexture nw_res_image_texture = nw::ut::ResDynamicCast<nw::gfx::res::ResImageTexture>(nw_res_texture);
  nw::gfx::res::ResPixelBasedImage nw_res_pixel_based_image = nw_res_image_texture.GetImage();
  return nw_res_pixel_based_image.GetBitsPerPixel();
}
//-----------------------------------------------------------------------------
/*
 *  @brief         テクスチャのイメージのアドレスを取得する
 * 
 *  @param[in]     index_in_resource  リソース内におけるインデックス
 *  @retval        void*              テクスチャのイメージのアドレス
 */
//-----------------------------------------------------------------------------
void* Resource::GetTextureImageAddress(const s32 index_in_resource) const
{
  nw::gfx::res::ResTexture      nw_res_texture       = m_nw_res_graphics_file->GetTextures(index_in_resource);
  nw::gfx::res::ResImageTexture nw_res_image_texture = nw::ut::ResDynamicCast<nw::gfx::res::ResImageTexture>(nw_res_texture);
  nw::gfx::res::ResPixelBasedImage nw_res_pixel_based_image = nw_res_image_texture.GetImage();
  return (reinterpret_cast<void*>(nw_res_pixel_based_image.GetImageAddress()));
}

//-----------------------------------------------------------------------------
/*
 *  @brief         リソースの内容を出力する
 */
//-----------------------------------------------------------------------------
void Resource::Dump(void) const
{
#if GFL_DEBUG
  if( m_nw_res_graphics_file )
  {
#define PRINT_COUNT(Item) GFL_PRINT("%s=%d\n", #Item, m_nw_res_graphics_file->Get##Item##Count());
    PRINT_COUNT(Models)
    PRINT_COUNT(Textures)
    PRINT_COUNT(LutSets)
    PRINT_COUNT(Shaders)
    PRINT_COUNT(Cameras)
    PRINT_COUNT(Lights)
    PRINT_COUNT(Fogs)
    PRINT_COUNT(SceneEnvironmentSettings)
    PRINT_COUNT(SkeletalAnims)
    PRINT_COUNT(MaterialAnims)
    PRINT_COUNT(VisibilityAnims)
    PRINT_COUNT(CameraAnims)
    PRINT_COUNT(LightAnims)
    PRINT_COUNT(FogAnims)
    PRINT_COUNT(Emitters)
#undef PRINT_COUNT
  }
#endif  // #if GFL_DEBUG
}

//-----------------------------------------------------------------------------
/*
 *  @brief         リソースからシーンオブジェクトを生成する
 *
 *  @param[in]     allocator             アロケータ
 *  @param[in]     device_allocator      デバイスメモリのアロケータ
 *                                       (allocatorとdevice_allocatorは同じものでも構わない)
 *  @param[in]     resource              シーンオブジェクトのリソース
 *  @param[in]     is_animation_enabled  アニメーションが有効かどうかを指定
 *  @param[in]     buffer_option         バッファの種類
 *  @param[in]     max_anim_objects      AnimBindingが持てるAnimGroupごとのAnimObjectの最大数
 * 
 *  @retval        nw::gfx::SceneObject*     生成したシーンオブジェクト
 *                                           (NULLのときは生成失敗。失敗の理由は
 *                                            allocatorが不正か、device_allocatorが不正か、SceneBuilderのCreateObjectで失敗したかのどれか)
 */
//-----------------------------------------------------------------------------
nw::gfx::SceneObject* Resource::CreateNwSceneObject(
    gfl::heap::NwAllocator*       allocator,
    gfl::heap::NwAllocator*       device_allocator,
    nw::gfx::ResSceneObject       resource,
    bool                          is_animation_enabled,
    bit32                         buffer_option,
    s32                           max_anim_objects
)
{
  if(    allocator         == NULL
      || device_allocator  == NULL )
  {
    return NULL;
  }

  nw::gfx::SceneObject* scene_object = nw::gfx::SceneBuilder()
      .Resource(resource)
      .IsFixedSizeMemory(true)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .BufferOption(buffer_option)
      .SharedMaterialModel(NULL)
      .SharedMeshNodeVisibiltiyModel(NULL)
      .MaxAnimObjectsPerGroup(max_anim_objects)
      .ParticleSetMarginCount(0)
      .IsAnimationEnabled(is_animation_enabled)
      .CreateObject(allocator, device_allocator);

  return scene_object;
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

