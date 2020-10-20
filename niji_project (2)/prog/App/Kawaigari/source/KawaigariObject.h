//======================================================================
/**
 * @file	Kawaigari3DObject.h
 * @brief	Kawaigari Objects
 * @author	PETE
 * @data	2015.12.01
 */
//======================================================================

#if !defined __KAWAIGARI_OBJECT_H_INCLUDED__
#define __KAWAIGARI_OBJECT_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <math/include/gfl2_math.h>

#include <DemoLib/Object/include/Object_GfMdl.h>
#include <DemoLib/Object/include/Object_Effect.h>
#include <DemoLib/Object/include/Object_Layout.h>
#include <DemoLib/Object/include/Object_Poke.h>
#include <DemoLib/Object/include/Object_Chara.h>
#include <DemoLib/Object/include/Object_Light.h>

#include "KawaigariMain.h"
#include "KawaigariRenderPipeline.h"

GFL_NAMESPACE_BEGIN( poke_3d )
  GFL_NAMESPACE_BEGIN( model )
    class BaseModel;
  GFL_NAMESPACE_END( model )
GFL_NAMESPACE_END( poke_3d )

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

class KawaigariMain;

class KawaigariObject_Gfmdl:public DemoLib::Obj::Object_GfMdl
{
public:
  //コンストラクタ
  KawaigariObject_Gfmdl(KawaigariMain *core,KawaigariRenderingPipeLine *pipe);
  //デストラクタ
  virtual ~KawaigariObject_Gfmdl();

  virtual void UpdateAlways(int step=1);
  virtual void Update(int step=1);
  
  KawaigariMain* GetKWMain(void){return mCore;}

  void ReserveCreate(void);
  void ReserveDelete(void);

  int GetType(void){return mType;}
  void SetType(int type){mType = type;}
  int GetKind(void){return mKind;}
  void SetKind(int kind){mKind = kind;}

private:
  KawaigariMain *mCore;

  bool mWaitCreate;
  int mDeleteCnt;
  int mType;
  int mKind;
};

class KawaigariObject_Effect:public DemoLib::Obj::Object_Effect
{
public:
  //コンストラクタ
  KawaigariObject_Effect(KawaigariMain *core);
  //デストラクタ
  virtual ~KawaigariObject_Effect();

  virtual void Update(int step=1);

  KawaigariMain* GetKWMain(void){return mCore;}

  void ReserveCreate(void);
  void ReserveDelete(void);

  int GetType(void){return mType;}
  void SetType(int type){mType = type;}
  int GetKind(void){return mKind;}
  void SetKind(int kind){mKind = kind;}

private:
  KawaigariMain *mCore;

  bool mWaitCreate;
  int mDeleteCnt;
  int mType;
  int mKind;

};


  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // __KAWAIGARI_OBJECT_H_INCLUDED__
