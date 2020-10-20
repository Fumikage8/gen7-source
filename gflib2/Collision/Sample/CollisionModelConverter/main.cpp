#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_GFGLUtil.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfCollisionMdlData.h>

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

  // コリジョンキャッシュ(gfcol)の作成
  gfl2::renderingengine::scenegraph::resource::GfCollisionMdlData resourceData;
  resourceData.ReadFile( szCmdLine );
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pColModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

  // 後始末
  scenegraph::resource::ResourceManager::Terminate();
  GFGL::Terminate();

  return 0;
}
