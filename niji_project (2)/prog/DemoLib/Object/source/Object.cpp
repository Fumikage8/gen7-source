//======================================================================
/**
 * @file  ObjectManager.cpp
 * @brief オブジェクトマネージャー
 * @author  ariizumi
 * @data  15/03/16
 */
//======================================================================
#include <math/include/gfl2_EulerRotation.h>
#include <DemoLib/Object/include/Object.h>
#include <DemoLib/Object/include/Object_GfMdl.h>
#include <DemoLib/Object/include/Object_Poke.h>

#if   defined(GF_PLATFORM_CTR)
  #include <nw/ut.h>
#endif

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

Object::Object()
:mPriority(OBJ_DEFAULT_PRIORITY)
,mObjId(-1)
{
  ResetParam();

  mIsUpdate = false;
  mIsEnable = false;
  mIsVisible = false;
  mCreateFrame = -1;
}

Object::~Object()
{
}

void Object::Update(int step)
{
  if( IsEnable() == false )
  {
    return;
  }

  UpdateFirst(step);
  if( mFollowObject && mFollowObject->IsEnable())
  {
    mFollowObject->UpdateAnimationForce();
    gfl2::renderingengine::scenegraph::instance::JointInstanceNode* jointNode;
    jointNode = mFollowObject->GetJointNode(mFollowNodeName);
    if( jointNode )
    {
      const gfl2::math::Matrix34 mtx = jointNode->GetWorldMatrix();

      gfl2::math::Vector3 rotRad = mtx.GetElemRotationZYX_fix();

      if( mIsFollowPos )
      {
        SetPosition(mtx.GetElemPosition());
      }
      if( mIsFollowRot )
      {
        SetRotate(gfl2::math::ConvRadToDeg(rotRad.x),gfl2::math::ConvRadToDeg(rotRad.y),gfl2::math::ConvRadToDeg(rotRad.z));
      }
      if( mIsFollowScale )
      {
        SetScale(mtx.GetElemScaling());
      }
    }
    else
    {
      if( mIsFollowPos )
      {
        gfl2::math::Vector3 pos;
        mFollowObject->GetCalcPosition(&pos);
        SetPosition(pos);
      }
      if( mIsFollowRot )
      {
        gfl2::math::Vector3 rot;
        mFollowObject->GetCalcRotate(&rot);
        SetRotate(rot);
      }
      if( mIsFollowScale )
      {
        gfl2::math::Vector3 scl;
        mFollowObject->GetCalcScale(&scl);
        SetScale(scl);
      }
    }
  }
  else if( mFollowPoke )
  {
    mFollowPoke->UpdateAnimationForce();
    gfl2::math::Vector3 pos;
    const bool ret = mFollowPoke->GetNodePosition( mFollowPokeNode , mFollowPokeNodeIdx , &pos );
    if( ret )
    {
      SetPosition(pos);
    }
    if( mIsFollowRot )
    {
      gfl2::math::Vector3 rot;
      mFollowPoke->GetCalcRotate(&rot);
      SetRotate(rot);
    }
    if( mIsFollowScale )
    {
      gfl2::math::Vector3 scl;
      mFollowPoke->GetCalcScale(&scl);
      SetScale(scl);
    }
  }

  UpdateAlways(step);
  if( IsUpdate() )
  {
    UpdateCore(step);
    ResetFlg();
  }
  UpdateAlwaysAfterCore(step);
}

void Object::SetFollowObject( Object *obj , char *name , bool followPos , bool followRot , bool followScale )
{
  mFollowObject = obj;
  if( mFollowObject )
  {
    if( name )
    {
#if   defined(GF_PLATFORM_CTR)
      nw::ut::strcpy(mFollowNodeName, NODE_STR_LEN, name);
#elif defined(GF_PLATFORM_WIN32)
      strcpy_s(mFollowNodeName, NODE_STR_LEN, name);  // 終端の'\0'を含めた長さを渡す
#endif  // GF_PLATFORM_
    }
    else
    {
#if   defined(GF_PLATFORM_CTR)
    strcpy(mFollowNodeName, "\0");
#elif defined(GF_PLATFORM_WIN32)
    strcpy_s(mFollowNodeName, NODE_STR_LEN, "\0");  // 終端の'\0'を含めた長さを渡す
#endif  // GF_PLATFORM_
    }
    mIsFollowPos = followPos;
    mIsFollowRot = followRot;
    mIsFollowScale = followScale;
  }
  else
  {
#if   defined(GF_PLATFORM_CTR)
    strcpy(mFollowNodeName, "\0");
#elif defined(GF_PLATFORM_WIN32)
    strcpy_s(mFollowNodeName, NODE_STR_LEN, "\0");  // 終端の'\0'を含めた長さを渡す
#endif  // GF_PLATFORM_
    mIsFollowPos = false;
    mIsFollowRot = false;
    mIsFollowScale = false;
  }
}

void Object::SetFollowPoke( Object_Poke *poke , PokeTool::MODEL_NODE node , int nodeIdx , bool followRot , bool followScale )
{
  mFollowPoke = poke;
  mFollowPokeNode = node;
  mFollowPokeNodeIdx = nodeIdx;
  mIsFollowRot = followRot;
  mIsFollowScale = followScale;
}

void Object::GetCalcScale(gfl2::math::Vector3 *scl)
{
  *scl = GetScale().MulPerElem(GetAdjustScale());
  if( GetIsFlip() ) 
  {
    scl->x *= -1;
  }
}


//SRT系の初期化
void Object::ResetParam(void)
{
  mPosition.Set(0,0,0);
  mOffset.Set(0,0,0);
  mRotate.Set(0,0,0);
  mRotateOffset.Set(0,0,0);
  mScale.Set(1,1,1);
  mAdjustScale.Set(1,1,1);

  mRotOrder = gfl2::math::EulerRotation::RotationOrderZYX;

  mFollowObject = NULL;
  mFollowPoke = NULL;
}

GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)