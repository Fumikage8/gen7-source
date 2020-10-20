/*
 * File:   main.cpp
 * Author: s-kimura
 *
 * Created on 2010/06/04, 14:30
 */

#include "../Gui/include/_Viewer.h"
#include <rapidxml/include/gfl_CmdlXml.h>

#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;

class MainAllocator : public gfl2::gfx::IGLAllocator
{
public:
  MainAllocator(gfl2::heap::HeapBase *devHeap):IGLAllocator(), mHeap(devHeap){}
  virtual ~MainAllocator(){}

#if GFL_DEBUG
void* _SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum)
{
  return mHeap->AllocMemoryAlign(size,alignment,filename,linenum);
}
#else
void* SystemMemoryMallocAlign(u32 size, u32 alignment)
{
  return GflHeapAllocMemoryAlign( mHeap , size , alignment );
}
#endif

  void SystemMemoryFree(void* const address)
  {
    GflHeapFreeMemory( address );
  }

  void* LocalMemoryMalloc(u32 size, u32 alignment = 4U){
    return NULL;
  }

  void LocalMemoryFree(void* const address)
  {
  }

private:
  gfl2::heap::HeapBase* mHeap;
};

#include "MainHeap.hpp"

#if GFL_DEBUG
int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szCmdLine, int sw )
{
  int argc = 1;
  char* argv[] = {"./TouchIDEditor_.exe"};
#else
int main(int argc, char* argv[])
{
#endif
	if(!gfl::qt::Application::CreateInstance( argc, argv, "X:/delivery/install/TouchIdEditor.exe" )){
		return 0;
	}
     QPixmap pixmap(":/Splash.png");
     QSplashScreen splash(pixmap);
     splash.show();	

     gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEVICE);

     MainAllocator* allocator = new MainAllocator(devHeap);
     gfl2::gfx::GFGL::Initialize(allocator);

     gfl2::gfx::GLMemory::SetNewParameterAllocator(allocator);

     scenegraph::SceneGraphManager::Initialize(allocator);
     scenegraph::resource::ResourceManager::Initialize(allocator);

	// GLUT のプリミティブ機能を使いたいので初期化を呼んでおく
	glutInit( &argc, argv );

	MainWindow* mw = MainWindow::CreateInstance( );

	splash.finish( mw );

	s32 ret = gfl::qt::Application::GetInstance( )->exec( );

	gfl::qt::Application::DestroyInstance( );

    scenegraph::resource::ResourceManager::Terminate();
    scenegraph::SceneGraphManager::Terminate();
    gfl2::gfx::GFGL::Terminate();
    GFL_DELETE(allocator);
	GFL_MAIN_RETURN( ret );
}
