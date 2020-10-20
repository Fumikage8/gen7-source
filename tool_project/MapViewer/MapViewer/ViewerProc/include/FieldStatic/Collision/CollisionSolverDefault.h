#pragma once
//=================================================================================================
/**
 *  @file   CollisionSolverDefault.h
 *  @brief  �f�t�H���g�\���o�[
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include <math/include/gfl2_Vector3.h>
#include "ViewerProc/include/FieldStatic/Collision/BaseSolver.h"

class SphereShape;
class MeshShape;
class CylinderShape;
class BoxShape;
class LineShape;

//----------------------------------------------------------------------------------------
//! @brief �\���o�[�x�[�X
//----------------------------------------------------------------------------------------
class CollisionSolverDefault : public BaseSolver
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �R���X�g���N�^
  */
  //-----------------------------------------------------------------------------
  CollisionSolverDefault();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �f�X�g���N�^
  */
  //-----------------------------------------------------------------------------
  virtual ~CollisionSolverDefault();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �c���[�Ԃ̏Փ˂�����
  *
  *  @param  pRootStaticActorRoot  �Փ˂����c���[
  *  @param  pRootStaticActorRoot  �Փ˂���c���[
  */
  //-----------------------------------------------------------------------------
  virtual void Solved(gfl2::renderingengine::scenegraph::instance::InstanceNode* pRootStaticActorRoot,
                      gfl2::renderingengine::scenegraph::instance::InstanceNode* pRootDynamicActorRoot);

private:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  DynamicSphere Vs StaticMesh
  *
  *  @param  pDynamicActor        �_�C�i�~�b�N�A�N�^�[
  *  @param  pDynamicActorSphere  �_�C�i�~�b�N�A�N�^�[�����X�t�B�A
  *  @param  pStaticActor         �X�^�e�B�b�N�A�N�^�[
  *  @param  pStaticActorMesh     �X�^�e�B�b�N�A�N�^�[�������b�V��
  *  @param  pAdjustVec           �_�C�i�~�b�N�A�N�^�[�ւ̕␳��
  *  @return 1�Ȃ�Փ˂��Ă���A0�Ȃ�Փ˂��Ă��Ȃ�
  */
  //-----------------------------------------------------------------------------
  int TestSphereMesh( BaseActor*           pDynamicActor,
                      SphereShape*         pDynamicActorSphere,
                      BaseActor*           pStaticActor,
                      MeshShape*           pStaticActorMesh,
                      gfl2::math::Vector3* pAdjustVec);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  Spehere Vs Triangle
  *
  *  @param  r          �X�t�B�A�̔��a
  *  @param  pos        �X�t�B�A�̒��S�ʒu
  *  @param  a          �g���C�A���O���̍��W1
  *  @param  b          �g���C�A���O���̍��W2
  *  @param  c          �g���C�A���O���̍��W3
  *  @param  pClosestPt �ŋߐړ_
  *  @return 1�Ȃ�Փ˂��Ă���A0�Ȃ�Փ˂��Ă��Ȃ�
  */
  //-----------------------------------------------------------------------------
  int TestSphereTriangle( const f32                  r,
                          const gfl2::math::Vector3& pos,
                          const gfl2::math::Vector3& a,
                          const gfl2::math::Vector3& b,
                          const gfl2::math::Vector3& c,
                          gfl2::math::Vector3*       pClosestPt );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  DynamicCylinder Vs StaticCylinder
  *
  *  @param  pDynamicActor         �_�C�i�~�b�N�A�N�^�[
  *  @param  pDynamicActorCylinder �_�C�i�~�b�N�A�N�^�[�����V�����_�[
  *  @param  pStaticActor          �X�^�e�B�b�N�A�N�^�[
  *  @param  pStaticActorCylinder  �X�^�e�B�b�N�A�N�^�[�����V�����_�[
  *  @param  pAdjustVec            �_�C�i�~�b�N�A�N�^�[�ւ̕␳��
  *  @return 1�Ȃ�Փ˂��Ă���A0�Ȃ�Փ˂��Ă��Ȃ�
  */
  //-----------------------------------------------------------------------------
  int TestCylinderCylinder( BaseActor*           pDynamicActor,
                            CylinderShape*       pDynamicActorCylinder,
                            BaseActor*           pStaticActor,
                            CylinderShape*       pStaticActorCylinder,
                            gfl2::math::Vector3* pAdjustVec);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  DynamicCylinder Vs StaticBox
  *
  *  @param  pDynamicActor         �_�C�i�~�b�N�A�N�^�[
  *  @param  pDynamicActorCylinder �_�C�i�~�b�N�A�N�^�[�����V�����_�[
  *  @param  pStaticActor          �X�^�e�B�b�N�A�N�^�[
  *  @param  pStaticActorBox       �X�^�e�B�b�N�A�N�^�[�����{�b�N�X
  *  @param  pAdjustVec            �_�C�i�~�b�N�A�N�^�[�ւ̕␳��
  *  @return 1�Ȃ�Փ˂��Ă���A0�Ȃ�Փ˂��Ă��Ȃ�
  */
  //-----------------------------------------------------------------------------
  int TestCylinderBox     ( BaseActor*           pDynamicActor,
                            CylinderShape*       pDynamicActorCylinder,
                            BaseActor*           pStaticActor,
                            BoxShape*            pStaticActorBox,
                            gfl2::math::Vector3* pAdjustVec);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  DynamicCylinder Vs StaticLine
  *
  *  @param  pDynamicActor         �_�C�i�~�b�N�A�N�^�[
  *  @param  pDynamicActorCylinder �_�C�i�~�b�N�A�N�^�[�����V�����_�[
  *  @param  pStaticActor          �X�^�e�B�b�N�A�N�^�[
  *  @param  pStaticActorLine      �X�^�e�B�b�N�A�N�^�[�������C��
  *  @param  pAdjustVec            �_�C�i�~�b�N�A�N�^�[�ւ̕␳��
  *  @return 1�Ȃ�Փ˂��Ă���A0�Ȃ�Փ˂��Ă��Ȃ�
  */
  //-----------------------------------------------------------------------------
  int TestCylinderLine    ( BaseActor*           pDynamicActor,
                            CylinderShape*       pDynamicActorCylinder,
                            BaseActor*           pStaticActor,
                            LineShape*           pStaticActorLine,
                            gfl2::math::Vector3* pAdjustVec);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �w��_�����Line�̍ŋߐړ_���Ƃ�
  *
  *  @param  p          �w��_
  *  @param  a          ���C���̐�[
  *  @param  b          ���C���̏I�[
  *  @return �ŋߐړ_
  */
  //-----------------------------------------------------------------------------
  gfl2::math::Vector3 CollisionSolverDefault::ClosestPtPointSegment(const gfl2::math::Vector3& p,
                                                                    const gfl2::math::Vector3& a,
                                                                    const gfl2::math::Vector3& b );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �w��_�����Triangle�̍ŋߐړ_���Ƃ�
  *
  *  @param  p          �w��_
  *  @param  a          �g���C�A���O���̍��W1
  *  @param  b          �g���C�A���O���̍��W2
  *  @param  c          �g���C�A���O���̍��W3
  *  @return �ŋߐړ_
  */
  //-----------------------------------------------------------------------------
  gfl2::math::Vector3 ClosestPtPointTriangle( const gfl2::math::Vector3& p,
                                              const gfl2::math::Vector3& a,
                                              const gfl2::math::Vector3& b,
                                              const gfl2::math::Vector3& c);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  �w��_�����Rect�̍ŋߐړ_���Ƃ�
  *
  *  @param  p          �w��_
  *  @param  rectC      �����`�̒��S�_
  *  @param  rectU0     �����`�̃��[�J����X����\���P�ʃx�N�g��
  *  @param  rectU1     �����`�̃��[�J����Y����\���P�ʃx�N�g��
  *  @param  rectE0     X���ɂ����������`�̒����̔���
  *  @param  rectE1     Y���ɂ����������`�̒����̔���
  *  @return �ŋߐړ_
  */
  //-----------------------------------------------------------------------------
  gfl2::math::Vector3 ClosestPtPointRect( const gfl2::math::Vector3& p,
                                          const gfl2::math::Vector3& RectC,
                                          const gfl2::math::Vector3& RectU0,
                                          const gfl2::math::Vector3& RectU1,
                                          const f32                  RectE0,
                                          const f32                  RectE1);

};
