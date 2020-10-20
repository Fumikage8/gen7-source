
#ifndef __MOVIEPROC_BASE_H__
#define __MOVIEPROC_BASE_H__
///////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   MovieProc.h
 * @brief  プロセスの基礎クラスです
 * @author ito_isao -->k.ohno
 * @date   2011.11.04
 */
/////////////////////////////////////////////////////////////////////////////////////// 

#pragma once



#if defined(GF_PLATFORM_CTR)


#include "GameSys/include/GameProc.h"
#include "heap/include/gfl2_Heap.h"
//#include "AppLib/include/Util/app_util_heap.h"
#include "IScene.h"
#include "System/include/HeapDefine.h"



// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)
class GLHeapAllocator;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(movie)
GFL_NAMESPACE_BEGIN(renderer)
class MobiclipRenderingPipeLine;
GFL_NAMESPACE_END(renderer)
GFL_NAMESPACE_END(movie)



namespace movie{

enum
{
	RET_NONE,//選択前
	RET_DECIDE,//スキップした。スキップあり時にしかこない
	RET_EXIT, //何もボタンを押さず終了。ループ無し時にしかこない
  RET_UPBX, //↑, B Xをおした時（特定のムービー再生時しかこない
  RET_Y,  //Yをおした時（特定のムービー再生時しかこない
};

class MovieProcBase : public GameSys::GameProc
{
public:
	MovieProcBase()
    : m_heap_alloc(NULL),
	    m_device_alloc(NULL),
      m_gl_device_alloc(NULL),
      m_renderingPipeLine(NULL),
      m_loop(false),
	    m_isDecide(NULL)
	{
	}

	virtual ~MovieProcBase()
	{
	}
  // override
  virtual GameSys::FrameMode GetFrameMode( void ) const { return GameSys::FRAMEMODE_60; }

public:
	virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pProcManager);
	virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pProcManager);
	virtual void DrawFunc(gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo);
	
public:
	virtual void InitLocal(){};
	virtual void EndLocal(){};
	
		// 現在のシーンを以外のものを呼びたいのであれば、上書きする
	virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pProcManager);

public:
	template<class T>
	void CreateScene()
	{
		gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ); 
		m_scene.SetOwner(this);
		m_scene.SetNext(GFL_NEW(heap) T());
	}
	
public:
	gfl2::heap::HeapBase* GetHeap(){return m_heap_alloc->GetHeapBase();}
	gfl2::heap::HeapBase* GetDeviceHeap(){return m_device_alloc->GetHeapBase();}
	gfl2::heap::NwAllocator* GetAllocator(){return m_heap_alloc;}
	gfl2::heap::NwAllocator* GetDeviceAllocator(){return m_device_alloc;}
	gfl2::util::GLHeapAllocator* GetGLDeviceAllocator(){return m_gl_device_alloc;}
//	gfl2::grp::GraphicsSystem* GetGraphicsSystem(){return m_graphics;}
  movie::renderer::MobiclipRenderingPipeLine* GetRenderingPipeLine(void) const { return m_renderingPipeLine; }
  gfl2::ui::TouchPanel* GetTouch(){return m_deviceManager->GetTouchPanel( gfl2::ui::CTR_DeviceManager::TOUCHPANEL_STANDARD );}
	gfl2::ui::Button* GetButton(){return m_button;}
	//gfl2::grp::g2d::LytSys* GetLayout(){return m_layout;}

	void SetLoopFlag(bool l){m_loop = l;}
	bool GetLoopFlag(){return m_loop;}
  
  //RS追加
  void SetDecideFlgPointer( bool *flg ){m_isDecide = flg;}
  void SetNo(int no){ mMovieNo = no; }
  
  void SetReturnCodeBuffer( int * buffer ) { m_returnCode = buffer; }
private:

protected:
	// ヒープ関係
	gfl2::heap::NwAllocator* m_heap_alloc;
	gfl2::heap::NwAllocator* m_device_alloc;
  gfl2::util::GLHeapAllocator* m_gl_device_alloc;

	// UI関係
	gfl2::ui::DeviceManager* m_deviceManager;
	gfl2::ui::Button* m_button;

	// グラフィック関係
//	gfl2::grp::GraphicsSystem* m_graphics;
//	gfl2::grp::g2d::LytSys* m_layout;
  
  movie::renderer::MobiclipRenderingPipeLine*  m_renderingPipeLine;

  int mMovieNo;
  
	// シーン管理
	ParentScene<MovieProcBase> m_scene;

	int m_next;

	bool m_loop;

  //RS追加
  bool *m_isDecide;
  int *m_returnCode;
};


class MovieProc : public MovieProcBase
{
public:
	virtual void InitLocal();
	virtual void EndLocal();
};

}// namespace movie

#else  //defined(GF_PLATFORM_CTR)


namespace movie{

class MovieProc
{
};

  
}// namespace movie

#endif //defined(GF_PLATFORM_CTR)

#endif//__MOVIEPROC_BASE_H__



