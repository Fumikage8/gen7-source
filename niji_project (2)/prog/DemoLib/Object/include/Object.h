//======================================================================
/**
 * @file	Object.h
 * @brief	オブジェクト基底
 * @author	ariizumi
 * @data	15/03/16
 */
//======================================================================
#if !defined( __OBJECT_H__ )
#define __OBJECT_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>

#include "PokeTool/include/PokeModel.h"
GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

//オブジェクトの優先度。小さいほうが高い
//同数だった場合先登録のほうが早い
static const int OBJ_DEFAULT_PRIORITY = 64;

class Object_GfMdl;
class Object_Poke;

class Object
{
public:
  //コンストラクタ
  Object();
  //デストラクタ
  virtual ~Object();

  //更新管理系
  //作成は個々に任せるが、更新・開放は同じフローでいけるので共通化できるようにしておく。
  //更新処理。Updateフラグが立っていたらUpdateCoreを呼ぶので座標更新系はそっちで。
  virtual void Update(int step=1);

  //座標の設定・取得
  virtual const gfl2::math::Vector3& GetPosition(void)const{return mPosition;};
  virtual void SetPosition(const gfl2::math::Vector3 pos ){mPosition = pos;SetIsUpdate();};
  virtual void SetPosition(const f32 x,const f32 y,const f32 z){mPosition.Set(x,y,z);SetIsUpdate();};

  virtual const gfl2::math::Vector3& GetOffset(void)const{return mOffset;};
  virtual void SetOffset(const gfl2::math::Vector3 pos ){mOffset = pos;SetIsUpdate();};
  virtual void SetOffset(const f32 x,const f32 y,const f32 z){mOffset.Set(x,y,z);SetIsUpdate();};

  virtual const void GetCalcPosition(gfl2::math::Vector3 *pos){*pos = GetPosition() + GetOffset();}

  virtual const gfl2::math::Vector3& GetScale(void)const{return mScale;};
  virtual void SetScale(const f32 scale ){mScale.Set(scale,scale,scale);SetIsUpdate();};
  virtual void SetScale(const gfl2::math::Vector3 scale ){mScale = scale;SetIsUpdate();};
  virtual void SetScale(const f32 sx,const f32 sy,const f32 sz ){mScale.Set(sx,sy,sz);SetIsUpdate();};
  virtual void MultScale(const f32 scale ){mScale = mScale*scale;}
  virtual void MultScale(const gfl2::math::Vector3 scale ){mScale = mScale.MulPerElem(scale);}
  virtual void MultScale(const f32 sx,const f32 sy,const f32 sz ){mScale = mScale.MulPerElem(gfl2::math::Vector3(sx,sy,sz));}

  virtual const gfl2::math::Vector3& GetAdjustScale(void){return mAdjustScale;}
  virtual void SetAdjustScale(const f32 scale , bool autoAdjust = true ){mAdjustScale.Set(scale,scale,scale);SetIsUpdate();};
  virtual void SetAdjustScale(const gfl2::math::Vector3 scale , bool autoAdjust = true ){mAdjustScale = scale;SetIsUpdate();};

  virtual void GetCalcScale(gfl2::math::Vector3 *scl);

  //RotateはDeg（0〜360)で扱っています。
  virtual const gfl2::math::Vector3& GetRotate(void)const{return mRotate;};
  virtual f32  GetRotateY(void)const{return mRotate.y;};
  virtual void SetRotate(const f32 rx,const f32 ry,const f32 rz ){mRotate.Set(rx,ry,rz);SetIsUpdate();};
  virtual void SetRotate(const gfl2::math::Vector3 &rot){mRotate = rot;SetIsUpdate();};
  virtual void SetRotateY(const f32 ry ){mRotate.y = ry;SetIsUpdate();};

  virtual const gfl2::math::Vector3& GetRotateOffset(void){return mRotateOffset;}
  virtual void SetRotateOffset(const gfl2::math::Vector3 rot ){mRotateOffset = rot;SetIsUpdate();};
  virtual void SetRotateOffsetY( f32 ry ){mRotateOffset.y = ry;SetIsUpdate();};

  virtual const void GetCalcRotate(gfl2::math::Vector3 *rot){*rot = GetRotate() + GetRotateOffset();}

  virtual void SetRotationOrder(gfl2::math::EulerRotation::RotationOrder order){mRotOrder = order;}
  virtual gfl2::math::EulerRotation::RotationOrder GetRotationOrder(void)const{return mRotOrder;}

  virtual void SetAlpha(const int alpha){};

  virtual bool GetVisible(void)const{return mIsVisible;}
  virtual void SetVisible(const bool flg){mIsVisible = flg;}

  virtual bool GetIsFlip(void)const{return mIsFlip;}
  virtual void SetIsFlip(const bool flg){mIsFlip = flg;}

  virtual bool IsUpdate(void)const{return mIsUpdate;}
  virtual void SetIsUpdate(const bool flg = true){mIsUpdate=flg;}
  virtual void ResetFlg(void){mIsUpdate = false;}
  //その他
  virtual bool IsEnable(void) const{return mIsEnable;}
  //SRT系の初期化
  virtual void ResetParam(void);  

  //管理系
  void SetPriority(int pri){mPriority = pri;}
  int GetPriority(void){return mPriority;}

  //OBJID シーケンスのグループIDと連携目的
  int GetObjId(void){return mObjId;}
  void SetObjId(int id){mObjId = id;}

  // Create Frame (Used for Animation frame Calculation)
  void SetCreateFrame(int n) { mCreateFrame = n; }
  int GetCreateFrame(void) { return(mCreateFrame); }

  // DrawLayout (Needed for Layout Objects)
  virtual void DrawLayout(gfl2::gfx::CtrDisplayNo displayNo) { }

  //追従系
  virtual void SetFollowObject( Object *obj , char *name = NULL , bool followPos = false , bool followRot = false , bool followScale = false );
  virtual void SetFollowPoke( Object_Poke *poke , PokeTool::MODEL_NODE node = PokeTool::MODEL_NODE_MAX , int nodeIdx = 0 , bool followRot = false , bool followScale = false );

  virtual void UpdateAnimationForce(void){}
  virtual gfl2::renderingengine::scenegraph::instance::JointInstanceNode* GetJointNode( char *name ){return NULL;}

#if PM_DEBUG
  virtual gfl2::math::Vector3* GetPositionPointer(void){return &mPosition;};
  virtual gfl2::math::Vector3* GetOffsetPointer(void){return &mOffset;};
  virtual gfl2::math::Vector3* GetScalePointer(void){return &mScale;};
  virtual gfl2::math::Vector3* GetAdjustScalePointer(void){return &mAdjustScale;}
  virtual gfl2::math::Vector3* GetRotatePointer(void){return &mRotate;}
  virtual gfl2::math::Vector3* GetRotateOffsetPointer(void){return &mRotateOffset;}
  virtual bool* GetVisiblePointer(void){return &mIsVisible;}
#endif //PM_DEBUG


protected:

  //IsEnableはチェック済み。
  //毎フレーム呼ばれる
  virtual void UpdateAlways(int step=1){};
  //こっちはUpdateCoreの後に呼ばれる。座標反映後の更新はこちらで。
  virtual void UpdateAlwaysAfterCore(int step=1){};
  //こっちはUpdate直後の追従処理の前に呼ばれる
  virtual void UpdateFirst(int step=1){};
  //IsUpdateが立っていたら来る関数。
  //IsEnableはチェック済みなので座標更新はこちらで。
  virtual void UpdateCore(int step=1){};

  //その他
  virtual void SetIsEnable(const bool flg){mIsEnable = flg;}
  
  gfl2::math::Vector3 mPosition;
  gfl2::math::Vector3 mOffset;  //追従のオフセットでも使用
  gfl2::math::Vector3 mRotate;
  gfl2::math::Vector3 mRotateOffset;//追従のオフセットでも使用
  gfl2::math::Vector3 mScale;
  gfl2::math::Vector3 mAdjustScale;
  //回転軸設定
  gfl2::math::EulerRotation::RotationOrder mRotOrder;
  bool            mIsUpdate;
  bool            mIsEnable;
  bool            mIsVisible;
  bool            mIsFlip;

  //追従用
  static const int NODE_STR_LEN = 32;
  Object *mFollowObject;
  char mFollowNodeName[NODE_STR_LEN];
  bool mIsFollowPos;
  bool mIsFollowRot;
  bool mIsFollowScale;

  //ポケモン追従用
  Object_Poke *mFollowPoke;
  PokeTool::MODEL_NODE mFollowPokeNode;
  int mFollowPokeNodeIdx;


  //管理系
  int mPriority;
  int mObjId;
  int mCreateFrame;
};

GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)


#endif //__OBJECT_H__