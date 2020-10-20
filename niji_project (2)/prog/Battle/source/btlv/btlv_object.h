//======================================================================
/**
 * @file	btlv_object.h
 * @brief	バトル描画 モデル等の規定クラス
 * @author	ariizumi
 * @data	14/11/13
 */
//======================================================================
#if !defined( __BTLV_OBJECT_H__ )
#define __BTLV_OBJECT_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>

#include <DemoLib/Object/include/Object_GfMdl.h>
#include <DemoLib/Object/include/Object_Effect.h>
#include <DemoLib/Object/include/Object_Chara.h>
#include <DemoLib/Object/include/Object_Cluster.h>

#include "btlv_local_def.h"

GFL_NAMESPACE_BEGIN(btl)

class BattleViewSystem;

typedef DemoLib::Obj::Object_Cluster BtlvObject_Cluster;

//継承用バトルオブジェクト基底
class BtlvObject_Gfmdl:public DemoLib::Obj::Object_GfMdl
{
public:
  //コンストラクタ
  BtlvObject_Gfmdl(BattleViewSystem *btlvCore,DemoLib::Obj::ObjectRenderPipeLine *pipe);
  //デストラクタ
  virtual ~BtlvObject_Gfmdl();

  virtual void Update(int step=1);

  //独自(デバッグ用?)
  BattleViewSystem* GetBattleViewSystem(void){return mBtlvCore;}

  void ReserveCreate(void);
  void ReserveDelete(void);

  int GetType(void){return mType;}
  void SetType(int type){mType = type;}
  int GetKind(void){return mKind;}
  void SetKind(int kind){mKind = kind;}

  void SetAutoRotate(bool flg){mAutoRotate = flg;GetCalcPosition(&mBefPos);}
private:
  BattleViewSystem *mBtlvCore;

  bool mWaitCreate;
  int mDeleteCnt;
  int mType;
  int mKind;
  //自動回転系
  bool mAutoRotate;
  gfl2::math::Vector3 mBefPos;

};
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------

class BtlvObject_Effect:public DemoLib::Obj::Object_Effect
{
public:
  //コンストラクタ
  BtlvObject_Effect(BattleViewSystem *btlvCore);
  //デストラクタ
  virtual ~BtlvObject_Effect();

  virtual void Update(int step=1);

  //独自(デバッグ用?)
  BattleViewSystem* GetBattleViewSystem(void){return mBtlvCore;}

  void ReserveCreate(void);
  void ReserveDelete(void);

  int GetType(void){return mType;}
  void SetType(int type){mType = type;}
  int GetKind(void){return mKind;}
  void SetKind(int kind){mKind = kind;}

  void SetAutoRotate(bool flg){mAutoRotate = flg;GetCalcPosition(&mBefPos);}
private:
  BattleViewSystem *mBtlvCore;

  bool mWaitCreate;
  int mDeleteCnt;
  int mType;
  int mKind;
  //自動回転系
  bool mAutoRotate;
  gfl2::math::Vector3 mBefPos;
};

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
class BtlvObject_Sound:public DemoLib::Obj::Object
{
public:
  BtlvObject_Sound(BattleViewSystem *btlvCore,int idx)
    :DemoLib::Obj::Object()
    ,mBtlvCore(btlvCore)
    ,mIndex(idx)
    ,mIs3DSound(false)
    ,mIsBefUpdate(false)
    {}

  void SetIs3DSound(bool flg){mIs3DSound = flg;}

  void SetSoundId(int id);
  int GetSoundId(void){return mSoundId;}

  void SetTargetGrp(int grp,int type);

protected:
  //IsEnableはチェック済み。
  //毎フレーム呼ばれる
  virtual void UpdateAlways(int step=1);
  //IsUpdateが立っていたら来る関数。
  //IsEnableはチェック済みなので座標更新はこちらで。
  virtual void UpdateCore(int step=1);
  //こっちはUpdate直後の追従処理の前に呼ばれる
  virtual void UpdateFirst(int step=1);

private:
  BattleViewSystem *mBtlvCore;
  int mIndex;
  bool mIs3DSound;
  gfl2::math::Vector3 mBefPos;
  bool mIsBefUpdate;
  int mSoundId;

  int mTargetGrp;
  int mTargetType;

};

GFL_NAMESPACE_END(btl)


#endif