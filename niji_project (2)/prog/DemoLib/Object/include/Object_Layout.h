//======================================================================
/**
 * @file	Object_Layout.h
 * @brief	オブジェクト Layout
 * @author	Pete
 * @data	15/05/19
 */
//======================================================================
#if !defined( __OBJECT_LAYOUT_H__ )
#define __OBJECT_LAYOUT_H__
#pragma once

#include <fs/include/gfl2_Fs.h>
#include <math/include/gfl2_math.h>
#include <heap/include/gfl2_Heap.h>

#include <DemoLib/Object/include/Object.h>


// UI Base:
#include "AppLib/include/Ui/UIView.h"

// Layout Includes:
#include <Layout/include/gfl2_Layout.h>
#include <AppLib/include/Util/app_util_AppLytAccessor.h>


GFL_NAMESPACE_BEGIN(DemoLib)
  GFL_NAMESPACE_BEGIN(Obj)

class ObjectRenderPipeLine;

class Object_Layout :public Object, public app::ui::UIView
{
public:  

  //コンストラクタ
  Object_Layout(app::util::Heap *heap, app::util::AppRenderingManager *renderManager,int layoutIdx,int numAni);
  //デストラクタ
  virtual ~Object_Layout();

  // Create Layout:
  virtual void LoadLayout(void *buf);
  virtual void Delete(void);

  virtual void DrawLayout(gfl2::gfx::CtrDisplayNo displayNo);

  virtual void AddRenderPath(int opt = 0);
  virtual void RemoveRenderPath(void);
  virtual bool IsAddRenderPath(void){return mIsAddRenderPath;}

  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // Layout System:	  
  void SetAnimation(int index);
  void UpdateAnimation(int step);
  void UpdatePosition(void) {}

  void SetPanePosition(gfl2::math::Vector3 pos,int paneIndex);
  void GetPanePosition(gfl2::math::Vector3 &pos,int paneIndex);

  void SetPaneScale(gfl2::math::Vector3 pos,int paneIndex);
  void GetPaneScale(gfl2::math::Vector3 &pos,int paneIndex);

  void SetPaneAlpha(u8 alpha,int paneIndex);
  void GetPaneAlpha(u8 &alpha,int paneIndex);

  void SetPaneVisible(bool visible,int paneIndex);

protected:
  //IsEnableはチェック済み。
  //毎フレーム呼ばれる
  virtual void UpdateAlways(int step=1);
  //IsUpdateが立っていたら来る関数。
  //IsEnableはチェック済みなので座標更新はこちらで。
  virtual void UpdateCore(int step=1);

  
  ObjectRenderPipeLine *mRenderingPipeLine;

  // UI VIEW:
  app::util::AppRenderingManager * mRenderingManager;
  void *resourceBuf;
  gfl2::str::MsgData * m_pMsgData;
  print::WordSet * m_pWordSet;
  app::util::Heap *mHeap;
    
  bool mIsAddRenderPath;
  int mSceneOpt;

  // Animation:
  int mAniNum;
  int mLayoutIdx;
  int mNumAni;

	u64 tickStart;

};

GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)


#endif //__OBJECT_LAYOUT_H__