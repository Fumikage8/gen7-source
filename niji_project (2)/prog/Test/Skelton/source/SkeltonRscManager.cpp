//=================================================================================================
/**
*	@file			SkeltonModel_rsc_manager.cpp
*	@brief
*	@author		tamada@gamefreak.co.jp
*	@date			2014.04.24
*/
//=================================================================================================

#if defined(GF_PLATFORM_WIN32)
#include "Test/Skelton/include/SkeltonRscManager.h"
#include "Test/Skelton/include/SkeltonCharaDefine.h"

#include <math/include/gfl2_Matrix.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
//#include <util/include/FnvHash.h>

#include "Shlwapi.h"  // for PathFileExists

//=================================================================================================
//=================================================================================================

#ifdef UNICODE
typedef c16 PATHSTRCODE;
#else
typedef c8 PATHSTRCODE;
#endif

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)

  void CopyPath( PATHSTRCODE * dst, const PATHSTRCODE * src )
{
  while ( *src )
  {
    *dst = *src;
    dst ++ ; src ++;
  }
  *dst = '\0';
}

bool ComparePath( const PATHSTRCODE * path1, const PATHSTRCODE * path2 )
{
  do
  {
    if (*path1 != *path2 ) return false;
    if (*path1 == '\0' ) break;
    ++path1;
    ++path2;
  }while( true );
  return true;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
struct HashKey
{
  static const u32 FNV_1_PRIME_32 = 0x01000193;

  u32		value;
  inline HashKey() { value = 0; };
  explicit inline HashKey(u32 val) : value(val) {};

  inline bool operator==(const HashKey &val) const
  {
    return val.value == value;
  }

  explicit inline HashKey(const PATHSTRCODE *str,u32 startVal = FNV_1_PRIME_32)
  {
    value = startVal;
    while (*str)
    {
      value *= FNV_1_PRIME_32;
      value^= (u32)*str++;
    }
  }
};

template <size_t N>
inline u32 HashCalcStatic( const PATHSTRCODE (&str)[N] )
{
  return str[N - 1] + HashCalcStatic( reinterpret_cast<const PATHSTRCODE (&)[N - 1]>(str) );
}

template <>
inline u32 HashCalcStatic<1>( const PATHSTRCODE (&str)[1] )
{
  return str[0];
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class SkeltonRscObject
{
public:
  enum { PATH_MAX_LENGTH = 256 };
  enum Type {
    TYPE_NONE,
    TYPE_MODEL,
    TYPE_MOTION,
  };

  Type m_type;
  bool m_exists;
  bool m_is_static;
  u16 m_count;
  c16 m_path[ PATH_MAX_LENGTH ];
  HashKey m_hash;
  gfl2::renderingengine::scenegraph::resource::IResourceData * m_raw_data;
  gfl2::renderingengine::scenegraph::resource::ResourceNode * m_node;

  SkeltonRscObject() :
    m_type(TYPE_NONE),
    m_exists(false),
    m_node(NULL)
  {
  }

  ~SkeltonRscObject()
  {
    Clear();
  }

  void Clear( void )
  {
    m_type = TYPE_NONE;
    m_exists = false;
    m_count = 0;
    m_path[0] = '\0';
    //m_hash. ; //@todo 何で初期化するのが正しいのか？
  }


};


char* loadFile(const wchar_t* filename)
{
  OutputDebugString(L"SkeltonModel::loadFile ");
  OutputDebugString( filename );
  OutputDebugString(L"\n");

  char* srcBuffer;
  {
    // ファイルオープン
    HANDLE file = CreateFile( filename, 
      GENERIC_READ,
      FILE_SHARE_READ,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_READONLY,
      NULL);
    if(file == INVALID_HANDLE_VALUE){ return NULL; }

    // 読み込み用バッファ作成
    DWORD fileSize = GetFileSize(file, NULL);
    srcBuffer = new char[fileSize + 1];
    srcBuffer[fileSize] = 0x00; // 終端コード

    // 読み込み
    DWORD readSize;
    ReadFile(file, srcBuffer, fileSize, &readSize, NULL);
    CloseHandle(file);
    if(readSize != fileSize){
      delete[] srcBuffer;
      return NULL;
    }
  }
  OutputDebugString(L"Load Success.\n");
  return srcBuffer;
}

//=================================================================================================
//=================================================================================================
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
SkeltonRscManager::SkeltonRscManager( gfl2::heap::HeapBase * p_heap, System::nijiAllocator * p_Allocator ) :
  mHeap(p_heap),
  mAllocator(p_Allocator),
  m_rsc( GFL_NEW(p_heap) SkeltonRscObject[RESOURCE_NUM] )
{
  chara::CheckModelResource(); //debug用途
}

SkeltonRscManager::~SkeltonRscManager()
{
  GFL_DELETE_ARRAY m_rsc;
}

void SkeltonRscManager::Update()
{
  // 非同期読み込み処理をここに実装？
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
SkeltonRscObject * SkeltonRscManager::searchResource( const HashKey & key )
{

  for ( int i = 0; i < RESOURCE_NUM; ++ i )
  {
    if ( m_rsc[i].m_exists && m_rsc[i].m_hash == key )
    {
      return &m_rsc[i];
    }
  }
  return NULL;
}

SkeltonRscObject * SkeltonRscManager::searchEmptyResource( void )
{

  for ( int i = 0; i < RESOURCE_NUM; ++ i )
  {
    if ( m_rsc[i].m_exists == false )
    {
      return &m_rsc[i];
    }
  }
  return NULL;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool SkeltonRscManager::HasResource( PATH path )
{
  HashKey key(reinterpret_cast<const c16*>(path));
  SkeltonRscObject * rsc = searchResource( key );
  if ( rsc != NULL )
  {
    assert( ComparePath( rsc->m_path, path ) == true );
    return true;
  }
  return false;
  //  return ( searchResource( key ) != NULL );
}


bool SkeltonRscManager::LoadModelResource( PATH path, bool is_static )
{
  HashKey key(reinterpret_cast<const c16*>(path));
  SkeltonRscObject * p_rsc = searchResource( key );
  if ( p_rsc )
  {
    ++ p_rsc->m_count;
    return true; //読み込み済み
  }
  p_rsc = searchEmptyResource();
  if ( !p_rsc )
  {
    return false;
  }

  FILE *fp;
  if ( _wfopen_s( &fp, path, L"r" ) != 0 )
  {
    return false;
  }
  else
  {
    fclose(fp);
  }

  gfl2::gfx::GLMemory::SetInstanceAllocator( mAllocator);
  p_rsc->m_raw_data = new gfl2::renderingengine::scenegraph::resource::GfMdlData( path );
  gfl2::gfx::GLMemory::ClearInstanceAllocator();
  p_rsc->m_node     = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mAllocator, p_rsc->m_raw_data );
  p_rsc->m_type     = SkeltonRscObject::TYPE_MODEL;
  p_rsc->m_exists   = true;
  p_rsc->m_hash     = key;
  p_rsc->m_count    = 1;
  CopyPath( p_rsc->m_path, path );
  return true;
}

bool SkeltonRscManager::LoadMotionResource( PATH motion_path, bool is_static )
{
  HashKey key(reinterpret_cast<const c16*>(motion_path));
  SkeltonRscObject * p_rsc = searchResource( key );
  if ( p_rsc )
  {
    ++ p_rsc->m_count;
    return true; //読み込み済み
  }
  p_rsc = searchEmptyResource();
  if ( !p_rsc )
  {
    return false;
  }

  FILE *fp;
  if ( _wfopen_s( &fp, motion_path, L"r" ) != 0 )
  {
    return false;
  }
  else
  {
    fclose(fp);
  }

  //c8 * p_file = loadFile( motion_path );
  //if ( p_file == NULL ) return false;
  gfl2::gfx::GLMemory::SetInstanceAllocator( mAllocator);
  p_rsc->m_raw_data = new gfl2::renderingengine::scenegraph::resource::GfMotData( gfl2::renderingengine::scenegraph::resource::ParserUtil::UTF16toUTF8(motion_path).c_str() );
  gfl2::gfx::GLMemory::ClearInstanceAllocator();
  p_rsc->m_node = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mAllocator, p_rsc->m_raw_data );
  p_rsc->m_type     = SkeltonRscObject::TYPE_MOTION;
  p_rsc->m_exists   = true;
  p_rsc->m_hash     = key;
  p_rsc->m_count    = 1;
  CopyPath( p_rsc->m_path, motion_path );
  //delete p_file;
  return true;
}

gfl2::renderingengine::scenegraph::resource::ResourceNode * SkeltonRscManager::GetResourceNode( PATH path )
{
  HashKey key(reinterpret_cast<const c16*>(path));
  SkeltonRscObject * p_rsc = searchResource( key );

  if ( !p_rsc )
  {
    return NULL;
  }
  {
    return p_rsc->m_node;
  }
}

bool SkeltonRscManager::UnloadResource( PATH path )
{
  if( path == NULL )
  {
    return false;
  }

  OutputDebugString(L"SkeltonRscManager::UnloadResource ");
  OutputDebugString( path );
  OutputDebugString(L"\n");
  HashKey key(reinterpret_cast<const c16*>(path));
  SkeltonRscObject * p_rsc = searchResource( key );
  if ( !p_rsc )
  {
    return false;
  }
  else
  {
    OutputDebugString(L" decriment counter\n" );
    -- p_rsc->m_count;
    if ( p_rsc->m_count == 0 && p_rsc->m_is_static == false )
    {
      OutputDebugString(L" resource clear\n" );
      delete p_rsc->m_node;
      delete p_rsc->m_raw_data;
      p_rsc->Clear();
    }
  }
  return true;
}


//=================================================================================================
//=================================================================================================
namespace chara
{
  //-------------------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------------------
  const ModelResource * GetModelResource( RSC_ID id )
  {
    static const wchar_t * const heroAnimeTable[] = {
      L"R:\\home\\momiji\\exec\\data_win32\\skelton\\resource\\chara\\tr0001\\tr0001_00_fi01_wait01.gfmot",
      L"R:\\home\\momiji\\exec\\data_win32\\skelton\\resource\\chara\\tr0001\\tr0001_00_fi20_walk01.gfmot",
      L"R:\\home\\momiji\\exec\\data_win32\\skelton\\resource\\chara\\tr0001\\tr0001_00_fi21_run01.gfmot",
      L"",
      L"",
      L"",
      L"",
      L"",
      L"",
      L"",
      L"",
      L"R:\\home\\momiji\\exec\\data_win32\\skelton\\resource\\chara\\tr0001\\tr0001_00_guts01.gfmot",
      L"R:\\home\\momiji\\exec\\data_win32\\skelton\\resource\\chara\\tr0001\\tr0001_00_fi20_walk02.gfmot",
    };

    static const ModelResource hero =
    {
      L"R:\\home\\momiji\\exec\\data_win32\\skelton\\resource\\chara\\tr0001\\tr0001_00_fi.gfmdl",
      heroAnimeTable,
      GFL_NELEMS(heroAnimeTable),
      1.0f  //FLD_PLAYER_SIZE_RATE		//1.0f		//adjustHeight
    };


    static const ModelResource Npc00 =
    {
      L"R:\\home\\momiji\\exec\\data_win32\\skelton\\resource\\chara\\tr0001\\tr0001_00_fi.gfmdl",
      heroAnimeTable,
      GFL_NELEMS(heroAnimeTable),
      1.0f  //FLD_PLAYER_SIZE_RATE		//1.0f		//adjustHeight
    };

    static const ModelResource Npc01 =
    {
      L"R:\\home\\momiji\\exec\\data_win32\\skelton\\resource\\chara\\tr0001\\tr0001_00_fi.gfmdl",
      heroAnimeTable,
      GFL_NELEMS(heroAnimeTable),
      1.0f	//FLD_PLAYER_SIZE_RATE		//1.0f		//adjustHeight
    };

    static const ModelResource Npc02 =
    {
      L"R:\\home\\momiji\\exec\\data_win32\\skelton\\resource\\chara\\tr0001\\tr0001_00_fi.gfmdl",
      heroAnimeTable,
      GFL_NELEMS(heroAnimeTable),
      1.0f  //FLD_PLAYER_SIZE_RATE		//1.0f		//adjustHeight
    };

    static const ModelResource Npc03 =
    {
      L"R:\\home\\momiji\\exec\\data_win32\\skelton\\resource\\chara\\tr0001\\tr0001_00_fi.gfmdl",
      heroAnimeTable,
      GFL_NELEMS(heroAnimeTable),
      1.0f  //FLD_PLAYER_SIZE_RATE		//1.0f		//adjustHeight
    };


    static const ModelResource Test =
    {
      L"R:\\home\\momiji\\exec\\data_win32\\skelton\\resource\\test\\tr0001_01.gfmdl",
      NULL,
      0,
      1.0f		//1.48f		//adjustHeight 0.4m
    };

    static const ModelResource * const resourceTable[] = {
      &hero,
      &Npc00,
      &Npc01,
      &Npc02,
      &Npc03,
      &Npc00,
      &Npc00,
      &Npc00,
    };

    if ( id < RSCID_NUM ) return resourceTable[id];

    return &hero;
  }

  static void CheckModelResource( void )
  {
    for (int id = RSCID_PLAYER_MALE; id < RSCID_NUM; id ++ )
    {
      const ModelResource * p_resource = GetModelResource( static_cast<RSC_ID>(id) );
      if ( !PathFileExists(p_resource->model_file_name ) )
      {
        GFL_ASSERT(L"model_file not exists!!!");
      }
      for (int i = 0; i < p_resource->anime_max; ++ i )
      {
        if ( ComparePath(p_resource->anime_file_table[i],L"") == true )
        {
          continue;
        }
        if ( !PathFileExists( p_resource->anime_file_table[i] ) )
        {
          OutputDebugString(L"anime_file not exists!!!\n");
          GFL_ASSERT( 0 );
        }
      }
    }
  }

  const c16 * const GetMotionName( u32 motion_id )
  {
    switch( motion_id )
    {
    case MOTION_ID_WAIT:
      return L"MOTION_ID_WAIT";
    case MOTION_ID_WALK:
      return L"MOTION_ID_WALK";
    case MOTION_ID_RUN:
      return L"MOTION_ID_RUN";
    case MOTION_ID_WAIT_WALK:
      return L"MOTION_ID_WAIT_WALK";
    case MOTION_ID_WALK_WAIT:
      return L"MOTION_ID_WALK_WAIT";
    case MOTION_ID_WAIT_RUN:
      return L"MOTION_ID_WAIT_RUN";
    case MOTION_ID_RUN_WAIT:
      return L"MOTION_ID_RUN_WAIT";
    case MOTION_ID_WALK_100F:	//@fixme 暫定
      return L"MOTION_ID_WALK_100F";
    case MOTION_ID_RUN_100F:	//@fixme 暫定
      return L"MOTION_ID_RUN_100F";
    default:
      return L"MOTION_ID_UNKNOWN";
    }
  }


}; // end of namespace chara

GFL_NAMESPACE_END(Skelton)
  GFL_NAMESPACE_END(Test)
#endif // GF_PLATFORM_WIN32
