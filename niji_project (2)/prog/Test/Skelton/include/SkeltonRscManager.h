//=================================================================================================
/**
*	@file			model_rsc_manager.h
*	@brief
*	@author		tamada@gamefreak.co.jp
*	@date			2014.04.24
*/
//=================================================================================================
#if defined(GF_PLATFORM_WIN32)
#pragma once

#include "Test/Skelton/include/SkeltonCharaDefine.h"
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <heap/include/gfl2_Heap.h>
#include "System/include/nijiAllocator.h"

typedef const c16 * PATH;

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)


class SkeltonRscObject;

struct HashKey;

class SkeltonRscManager
{
public:
  SkeltonRscManager( gfl2::heap::HeapBase * p_heap, System::nijiAllocator * p_Allocator );
  ~SkeltonRscManager();

  void Update( void );

  bool HasResource( PATH path );
  std::string WStringToString( std::wstring oWString );
  bool LoadModelResource( PATH path, bool is_static = false );
  bool LoadMotionResource( PATH motion_path, bool is_static = false );
  gfl2::renderingengine::scenegraph::resource::ResourceNode * GetResourceNode( PATH path );
  bool UnloadResource( PATH path );


private:
  SkeltonRscObject * searchResource( const HashKey & key );
  SkeltonRscObject * searchEmptyResource( void );

private:
  enum {
    RESOURCE_NUM = 128,
  }; //@fixme

  gfl2::heap::HeapBase * mHeap;
  System::nijiAllocator   *mAllocator;
  SkeltonRscObject * m_rsc;
};

GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)
#endif // GF_PLATFORM_WIN32