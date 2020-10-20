//======================================================================
/**
 * @file	SequenceObject.h
 * @brief	SEQPLAYER描画 モデル等の規定クラス
 * @author	Pete
 * @data	15/5/12
 */
//======================================================================
#if !defined( __SEQUENCE_OBJECT_H__ )
#define __SEQUENCE_OBJECT_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <math/include/gfl2_math.h>

#include <DemoLib/Object/include/Object_GfMdl.h>
#include <DemoLib/Object/include/Object_Effect.h>
#include <DemoLib/Object/include/Object_Layout.h>
#include <DemoLib/Object/include/Object_Poke.h>
#include <DemoLib/Object/include/Object_Chara.h>
#include <DemoLib/Object/include/Object_Light.h>

#include "DemoLib/SequencePlayer/include/SequencePlayerCore.h"
#include "DemoLib/SequencePlayer/include/SequenceRenderPipeline.h"

GFL_NAMESPACE_BEGIN(DemoLib)
	GFL_NAMESPACE_BEGIN(Sequence)

static const int LIGHTNO_POKEMON_TOP = 4;

class SequenceViewSystem;

//継承用バトルオブジェクト基底
class SequenceObject_Gfmdl:public DemoLib::Obj::Object_GfMdl
{
public:
  //コンストラクタ
  SequenceObject_Gfmdl(SequenceViewSystem *seqCore,SequenceRenderingPipeLine *pipe);
  //デストラクタ
  virtual ~SequenceObject_Gfmdl();

  virtual void UpdateAlways(int step=1);
  virtual void Update(int step=1);

  //独自(デバッグ用?)
  SequenceViewSystem* GetSeqViewSystem(void){return mSeqCore;}

  void ReserveCreate(void);
  void ReserveDelete(void);

  int GetType(void){return mType;}
  void SetType(int type){mType = type;}
  int GetKind(void){return mKind;}
  void SetKind(int kind){mKind = kind;}

  void SetTimeModel(int type) { mTimeModel=true; mTimeType=type; }

private:
  SequenceViewSystem *mSeqCore;

  bool mWaitCreate;
  int mDeleteCnt;
  int mType;
  int mKind;
  bool mTimeModel;
  int mTimeType;
};

class SequenceObject_Poke:public DemoLib::Obj::Object_Poke
{
public:
  //コンストラクタ
  SequenceObject_Poke(SequenceViewSystem *seqCore,SequenceRenderingPipeLine *pipe,int index);
  //デストラクタ
  virtual ~SequenceObject_Poke();

  virtual void Update(int step=1);

  //独自(デバッグ用?)
  SequenceViewSystem* GetSeqViewSystem(void){return mSeqCore;}

  void GetFollowNode( int seqNode , PokeTool::MODEL_NODE *retNode , int *retNodeIdx );
  

  virtual void Create(gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator,bool useShader);
  virtual void RemoveRenderPath(void);

  virtual void UpdateAlways(int step=1);
  virtual void SetVisible(const bool flg);
  virtual void SetShadowVisible(const bool flg);

  void ReserveCreate(void);
  void ReserveDelete(void);

  int GetType(void){return mType;}
  void SetType(int type){mType = type;}
  int GetKind(void){return mKind;}
  void SetKind(int kind){mKind = kind;}

  void SetSimpleParam(int monsNo,int formNo) { mMonsNo = monsNo; mFormNo = formNo;}
  void GetSimpleParam(int &monsNo,int &formNo) { monsNo=mMonsNo; formNo=mFormNo; }

  virtual void SetRotate(const gfl2::math::Vector3 &rot);
  virtual void SetRotateOffset(const gfl2::math::Vector3 rot );


  int GetIndex(void) { return(mIdx); }

private:
  SequenceViewSystem *mSeqCore;

  bool mWaitCreate;
  int mDeleteCnt;
  int mType;
  int mKind;
  int mIdx;

  int mMonsNo,mFormNo;
  bool mIsVisibleShadow;
};

class SequenceObject_Chara:public DemoLib::Obj::Object_Chara
{
public:
  //コンストラクタ
  SequenceObject_Chara(SequenceViewSystem *seqCore,SequenceRenderingPipeLine *pipe,poke_3d::model::CharaModelFactory *charaFactory,poke_3d::model::DressUpModelResourceManager *resourceManager)
    :Object_Chara(pipe,charaFactory,resourceManager)
    ,mSeqCore(seqCore)   
    ,mUpdateEye(false)
  {}
  //デストラクタ
  virtual ~SequenceObject_Chara() {}

  //virtual void Update();
  virtual void UpdateAlways(int step=1);

  //独自(デバッグ用?)
  SequenceViewSystem* GetSeqViewSystem(void){return mSeqCore;}

  void SetUpdateEye(bool b) { mUpdateEye = b; }

private:
  SequenceViewSystem *mSeqCore;
  bool mUpdateEye;

};

class SequenceObject_Layout:public DemoLib::Obj::Object_Layout
{
public:
	//コンストラクタ
	SequenceObject_Layout(SequenceViewSystem *seqCore,int layoutIdx,int numAni);
	//デストラクタ
	virtual ~SequenceObject_Layout();

	virtual void Update(int step=1);

	//独自(デバッグ用?)
	SequenceViewSystem* GetSeqViewSystem(void){return mSeqCore;}

	void ReserveCreate(void);
	void ReserveDelete(void);

	int GetType(void){return mType;}
	void SetType(int type){mType = type;}
	int GetKind(void){return mKind;}
	void SetKind(int kind){mKind = kind;}

  void VSBattleSet(int index);
  void RoyalBattleSet(int index);
  void DendouDemoSet();

private:
	SequenceViewSystem *mSeqCore;

	bool mWaitCreate;
	int mDeleteCnt;
	int mType;
	int mKind;

  gfl2::str::StrBuf *mVSNameBuf[4];

};

class SequenceObject_Light:public DemoLib::Obj::Object_Light
{
public:
  //コンストラクタ
  SequenceObject_Light(SequenceViewSystem *seqCore,SequenceRenderingPipeLine *pipe,bool isTimeLight=false);
  //デストラクタ
  virtual ~SequenceObject_Light();

  virtual void Update(int step=1);

  //独自(デバッグ用?)
  SequenceViewSystem* GetSeqViewSystem(void){return mSeqCore;}

  virtual void UpdateAlways(int step=1);

  void ReserveCreate(void);
  void ReserveDelete(void);

  int GetType(void){return mType;}
  void SetType(int type){mType = type;}
  int GetKind(void){return mKind;}
  void SetKind(int kind){mKind = kind;}

  void SetTimeType(int type) { mTimeType = type; }

private:
  SequenceViewSystem *mSeqCore;

  bool mWaitCreate;
  bool mTimeLight;
  int mDeleteCnt;
  int mType;
  int mTimeType;
  int mKind;  

};

class SequenceObject_Effect:public DemoLib::Obj::Object_Effect
{
public:
  //コンストラクタ
  SequenceObject_Effect(SequenceViewSystem *seqCore);
  //デストラクタ
  virtual ~SequenceObject_Effect();

  virtual void Update(int step=1);

  //独自(デバッグ用?)
  SequenceViewSystem* GetSeqViewSystem(void){return mSeqCore;}

  void ReserveCreate(void);
  void ReserveDelete(void);

  int GetType(void){return mType;}
  void SetType(int type){mType = type;}
  int GetKind(void){return mKind;}
  void SetKind(int kind){mKind = kind;}

private:
  SequenceViewSystem *mSeqCore;

  bool mWaitCreate;
  int mDeleteCnt;
  int mType;
  int mKind;

};


class SequenceObject_Sound:public DemoLib::Obj::Object
{
public:
  SequenceObject_Sound(SequenceViewSystem *seqCore,int idx)
    :DemoLib::Obj::Object()
    ,mSeqCore(seqCore)
    ,mIndex(idx)
    ,mIs3DSound(false)
    ,mIsBefUpdate(false)
  {}

  void SetIs3DSound(bool flg){mIs3DSound = flg;}

  void SetSoundId(int id);
  int GetSoundId(void){return mSoundId;}

protected:
  //IsEnableはチェック済み。
  //毎フレーム呼ばれる
  virtual void UpdateAlways(int step=1);
  //IsUpdateが立っていたら来る関数。
  //IsEnableはチェック済みなので座標更新はこちらで。
  virtual void UpdateCore(int step=1);

private:
  SequenceViewSystem *mSeqCore;
  int mIndex;
  bool mIs3DSound;
  gfl2::math::Vector3 mBefPos;
  bool mIsBefUpdate;
  int mSoundId;

};

	GFL_NAMESPACE_END(Sequence)
GFL_NAMESPACE_END(DemoLib)

#endif //__SEQUENCE_OBJECT_H__
