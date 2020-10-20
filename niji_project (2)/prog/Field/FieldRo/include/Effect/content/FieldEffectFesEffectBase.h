#if !defined(FIELDEFFECTFESEFFECTBASE_H_INCLUDED)
#define FIELDEFFECTFESEFFECTBASE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   FieldEffectFesEffectBase.h
 * @date   2016/02/09 Tue. 19:03:32
 * @author muto_yu
 * @brief  JoinFesta(FesCircle)エフェクト基底
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <math/include/gfl2_math.h>
#include  <heap/include/gfl2_Heap.h>
#include  <model/include/gfl2_BaseModel.h>
#include  <Effect/include/gfl2_EffectHeap.h>
#include  <Effect/include/gfl2_EffectSystem.h>

#include  "AppLib/include/Util/app_util_Heap.h"
#include  "Field/FieldRo/include/Effect/FieldEffectResource_define.h"
#include  "../FieldEffectBase.h"


// =============================================================================
/**/
//==============================================================================



namespace Field   {
  class MyRenderingPipeLine;
  class FieldRootNode;
namespace Effect  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  JF:エフェクト基底
  *  @date   2016/02/08 Mon. 11:18:15
  */
//==============================================================================
class EffectFesEffectBase
  : public  IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectFesEffectBase);
public:
  EffectFesEffectBase(void);
  virtual ~EffectFesEffectBase();


  struct SetupData
  {
    void*                       pResource;        /**<  EffectType別Archive       */
    gfl2::Effect::GFHeap*       pEffectHeap;      /**<  EffectSystem(eset)用Heap  */
    gfl2::Effect::System*       pEffectSystem;
    gfl2::heap::HeapBase*       pHeapBase;        /**<  エフェクト用一般Heap      */

    FieldRootNode*              pFieldRootNode;
    Field::MyRenderingPipeLine* pFieldPipeLine;
  };

  void  Setup(const SetupData& rSetupData);

  /*  FieldEffectBase I/F  */
//  virtual void  Initialize( void* in_pResource, gfl2::heap::HeapBase* in_pHeap, Field::IField3DObjectNode*  in_pParent );
//  virtual void  Initialize( void* in_pResource, gfl2::heap::HeapBase* in_pHeap, const gfl2::math::Vector3&  in_vPosition );
  virtual bool  Terminate( void );
  virtual void  Update( void );
//  virtual void  Delete( void );
//  virtual bool  IsAnimationLastFrame(void);


protected:
  static const gfl2::math::Vector3    POSITION_ZERO;

  virtual void  OnSetup(void){}
  virtual void  OnUpdate(void){}
  virtual bool  OnTerminate(void){return true;}

  /*  eset  */
  void  StartCore(gfl2::Effect::Handle* pHandle, const Field::Effect::EffectResourceID resourceID, const gfl2::math::Matrix34* pWorldMatrix = NULL, const gfl2::math::Vector3* pOffset = NULL);
  void  StopCore(gfl2::Effect::Handle* pHandle);

  /*  model  */
  //==============================================================================
  /**
    *  @brief  
    *  @date   2016/02/10 Wed. 12:25:15
    */
  //==============================================================================
  class  ModelMotionResource
  {
    GFL_FORBID_COPY_AND_ASSIGN(ModelMotionResource);
  public:
    ModelMotionResource(void)
      : m_pCaller(NULL)
      , m_pResourceNode(NULL)
    {}
    virtual ~ModelMotionResource()
    {Destroy();}

    void  CreateModel(EffectFesEffectBase* pCaller, const u32 binLinkerDataID);
    void  CreateMotion(EffectFesEffectBase* pCaller, const u32 binLinkerDataID);
    void  Destroy(void);

    gfl2::renderingengine::scenegraph::resource::ResourceNode*  GetResourceNode(void) {return m_pResourceNode;}

  protected:
    EffectFesEffectBase*                                        m_pCaller;
    gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pResourceNode;
  };

  //==============================================================================
  /**
    *  @brief  
    *  @date   2016/02/10 Wed. 12:29:49
    */
  //==============================================================================
  class ModelInstance
  {
    GFL_FORBID_COPY_AND_ASSIGN(ModelInstance);
  public:
    ModelInstance(void)
      : m_pCaller(NULL)
      , m_pModelInstanceNode(NULL)
      , m_Model()
    {}
    virtual ~ModelInstance()
    {Destroy();}

    void  Create(EffectFesEffectBase* pCaller, ModelMotionResource& rModelResource);
    void  Update(void);
    void  Destroy(void);

    void  SetMotion(ModelMotionResource* pMotionResource = NULL);
    poke_3d::model::BaseModel&  GetBaseModel(void)  {return m_Model;}

  protected:
    EffectFesEffectBase*                                            m_pCaller;
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pModelInstanceNode;
    poke_3d::model::BaseModel                                       m_Model;
  };


  void  GetViewMatrix(gfl2::math::Matrix34& rOutMatrix);
  void  GetCameraCoordMatrix(gfl2::math::Matrix34& rOutMatrix, const gfl2::math::Vector3& rOffset);   /**<  カメラ座標系に貼り付けるためのマトリクスを取得  */
  void  UpdateToCamCoord(gfl2::Effect::Handle* pHandle, const gfl2::math::Vector3& rOffset);

  /*
    members
  */
  SetupData                     m_SetupData;
  gfl2::util::GLHeapAllocator*  m_pGLHeapAllocator;
};

} /*  namespace Effect  */
} /*  namespace Field   */
#endif  /*  #if !defined(FIELDEFFECTFESEFFECTBASE_H_INCLUDED)  */
