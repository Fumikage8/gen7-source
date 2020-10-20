#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_GFGLUtil.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfCollisionMdlData.h>
#include <Windows.h>
#include <shlwapi.h>
 
#pragma comment( lib, "Shlwapi.lib" )

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::scenegraph;

class GLAllocator : public IGLAllocator
{
public:
  GLAllocator():IGLAllocator(){}
  virtual ~GLAllocator(){}
  virtual void*   SystemMemoryMalloc(u32 size, u32 alignment){ return malloc(size); }
  virtual void    SystemMemoryFree  (void* const address    ){ free( address ); }
  virtual void*   LocalMemoryMalloc (u32 size, u32 alignment){ return NULL; }
  virtual void    LocalMemoryFree   (void* const address    ){}
};

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szCmdLine, int sw )
{
  // 初期化
  GLAllocator m_Allocator;
  GFGL::Initialize( &m_Allocator );

  // リソースノードを生成するマネージャ初期化
  scenegraph::resource::ResourceManager::Initialize();

  // カレントディレクトリを取得
  c8 currentPath[512];
  GetCurrentDirectory(sizeof(currentPath),currentPath);

  // コマンドライン引数を取得
  std::string cmdLine( szCmdLine );
  s32 spaceIndexFirst = cmdLine.find_first_of(" ");
  s32 spaceIndexLast  = cmdLine.find_last_of (" ");

  // コマンドライン引数に不備がないかチェック
  if( spaceIndexFirst == std::string::npos
   || spaceIndexLast  == std::string::npos
   || spaceIndexFirst == spaceIndexLast )
  {
    GFL_ASSERT_MSG( false, "引数の数が正しくありません" );
  }

  // 入力パスを取得
  c8 inputPath[512];
  {
    // コマンドライン引数から入力パスを取得
    memset( inputPath, '\0' , sizeof(inputPath) );
    cmdLine.copy(inputPath,spaceIndexFirst,0);

    // 入力パスが相対なら絶対に変換
    if( PathIsRelative(inputPath) )
    {
      c8 temp[512];
      memset( temp, '\0' , sizeof(temp) );
      GetFullPathName( inputPath , sizeof(temp) , temp , NULL );
      memcpy( inputPath, temp , sizeof(temp) );
    }
  }

  // 出力パスを取得
  c8 outputPath[512];
  {
    // コマンドライン引数から出力パスを取得
    memset( outputPath, '\0' , sizeof(outputPath) );
    cmdLine.copy(outputPath , spaceIndexLast-spaceIndexFirst-1,spaceIndexFirst+1);

    // 出力パスが相対なら絶対に変換
    if( PathIsRelative(outputPath) )
    {
      c8 temp[512];
      memset( temp, '\0' , sizeof(temp) );
      GetFullPathName( outputPath , sizeof(temp) , temp , NULL );
      memcpy( outputPath, temp , sizeof(temp) );
    }
  }

  // アトリビュートパスを取得
  c8  attributePath[512];
  {
    // コマンドライン引数からアトリビュートパスを取得
    memset( attributePath, '\0' , sizeof(attributePath) );
    cmdLine.copy(attributePath,cmdLine.size() - (spaceIndexLast+1),spaceIndexLast+1);

    // アトリビュートパスが相対なら絶対に変換
    if( PathIsRelative(attributePath) )
    {
      c8 temp[512];
      memset( temp, '\0' , sizeof(temp) );
      GetFullPathName( attributePath , sizeof(temp) , temp , NULL );
      memcpy( attributePath, temp , sizeof(temp) );
    }
  }

  // コリジョンキャッシュ(gfcol)の作成
  gfl2::renderingengine::scenegraph::resource::GfCollisionMdlData resourceData;
  resourceData.ReadFile( inputPath , outputPath , attributePath );
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pColModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

  // 後始末
  scenegraph::resource::ResourceManager::Terminate();

  return 0;
}
