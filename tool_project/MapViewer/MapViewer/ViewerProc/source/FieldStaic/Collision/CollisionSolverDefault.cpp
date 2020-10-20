//=================================================================================================
/**
 *  @file   CollisionSolverDefault.h
 *  @brief  �f�t�H���g�\���o�[
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include <Collision/include/gfl2_CollisionModel.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CollisionMeshResourceNode.h>
#include <math/include/gfl2_math_control.h>

#include "ViewerProc/include/FieldStatic/Collision/BaseShape.h"
#include "ViewerProc/include/FieldStatic/Collision/CollisionSolverDefault.h"
#include "ViewerProc/include/FieldStatic/Collision/StaticActor.h"
#include "ViewerProc/include/FieldStatic/Collision/DynamicActor.h"

//----------------------------------------------------------------------------------------
//! @brief �\���o�[�x�[�X
//----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
*  @brief  �R���X�g���N�^
*/
//-----------------------------------------------------------------------------
CollisionSolverDefault::CollisionSolverDefault()
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  �f�X�g���N�^
*/
//-----------------------------------------------------------------------------
CollisionSolverDefault::~CollisionSolverDefault()
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  �c���[�Ԃ̏Փ˂�����
*
*  @param  pRootStaticActorRoot  �Փ˂����c���[
*  @param  pRootStaticActorRoot  �Փ˂���c���[
*/
//-----------------------------------------------------------------------------
void CollisionSolverDefault::Solved(gfl2::renderingengine::scenegraph::instance::InstanceNode* pRootStaticActorRoot,
                                    gfl2::renderingengine::scenegraph::instance::InstanceNode* pRootDynamicActorRoot)
{
  // ������
  for( u32 i=0 ; i<pRootStaticActorRoot->GetChildCount() ; i++ )
  {
    StaticActor* pStaticActor = pRootStaticActorRoot->GetChild(i)->SafeCast<StaticActor>();
    if( pStaticActor )
    {
      pStaticActor->InitCollisionResult();
    }
  }
  for( u32 i=0 ; i<pRootDynamicActorRoot->GetChildCount() ; i++ )
  {
    DynamicActor* pDynamicActor = pRootDynamicActorRoot->GetChild(i)->SafeCast<DynamicActor>();
    if( pDynamicActor )
    {
      pDynamicActor->InitCollisionResult();
    }
  }

  // ����
  for( u32 i=0 ; i<pRootStaticActorRoot->GetChildCount() ; i++ )
  {
    StaticActor* pStaticActor = pRootStaticActorRoot->GetChild(i)->SafeCast<StaticActor>();
    if( !pStaticActor )
    {
      continue;
    }

    for( u32 i=0 ; i<pRootDynamicActorRoot->GetChildCount() ; i++ )
    {
      DynamicActor* pDynamicActor = pRootDynamicActorRoot->GetChild(i)->SafeCast<DynamicActor>();
      if( !pDynamicActor )
      {
        continue;
      }

      b32 isHit = false;
      gfl2::math::Vector3 adjustVec(0.0f,0.0f,0.0f);

      // dynamic sphere vs static mesh
      if( pDynamicActor->GetShapeType() == BaseActor::SPHERE 
       && pStaticActor->GetShapeType()  == BaseActor::MESH  )
      {
        isHit = TestSphereMesh( pDynamicActor,
                                static_cast<SphereShape*>(pDynamicActor->GetShape()),
                                pStaticActor,
                                static_cast<MeshShape*>  (pStaticActor->GetShape()),
                                &adjustVec );
      }
      // dynamic cylinder vs static cylinder
      else if( pDynamicActor->GetShapeType() == BaseActor::CYLINDER 
            && pStaticActor->GetShapeType()  == BaseActor::CYLINDER )
      {
        isHit = TestCylinderCylinder( pDynamicActor,
                                      static_cast<CylinderShape*>(pDynamicActor->GetShape()),
                                      pStaticActor,
                                      static_cast<CylinderShape*>(pStaticActor->GetShape()),
                                      &adjustVec );
      }
      // dynamic cylinder vs static Box
      else if( pDynamicActor->GetShapeType() == BaseActor::CYLINDER 
            && pStaticActor->GetShapeType()  == BaseActor::BOX )
      {
        isHit = TestCylinderBox(pDynamicActor,
                                static_cast<CylinderShape*>(pDynamicActor->GetShape()),
                                pStaticActor,
                                static_cast<BoxShape*>(pStaticActor->GetShape()),
                                &adjustVec );
      }
      // dynamic cylinder vs static Line
      else if( pDynamicActor->GetShapeType() == BaseActor::CYLINDER 
            && pStaticActor->GetShapeType()  == BaseActor::LINE )
      {
        isHit = TestCylinderLine(pDynamicActor,
                                 static_cast<CylinderShape*>(pDynamicActor->GetShape()),
                                 pStaticActor,
                                 static_cast<LineShape*>(pStaticActor->GetShape()),
                                 &adjustVec );
      }

      // �Փ˂��Ă���
      if( isHit )
      {
        // �_�C�i�~�b�N�A�N�^�[�̋󂢂Ă���Փˌ��ʃ��[�N���擾
        BaseActor::CollisionResult* pDynamicActorResult = NULL;
        for( u32 i=0 ; i<BaseActor::MAX_COLLISION_RESULT_CNT ; i++ )
        {
          if( pDynamicActor->GetCollisionResult(i)->isFree() )
          {
            pDynamicActorResult = pDynamicActor->GetCollisionResult(i);
            break;
          }
        }

        // �X�^�e�B�b�N�A�N�^�[�̋󂢂Ă���Փˌ��ʃ��[�N���擾
        BaseActor::CollisionResult* pSaticActorResult = NULL;
        for( u32 i=0 ; i<BaseActor::MAX_COLLISION_RESULT_CNT ; i++ )
        {
          if( pStaticActor->GetCollisionResult(i)->isFree() )
          {
            pSaticActorResult = pStaticActor->GetCollisionResult(i);
            break;
          }
        }

        // ��������������擾
        if( pDynamicActorResult )
        {
          pDynamicActorResult->pTargetActor = pStaticActor;
        }
        if( pSaticActorResult )
        {
          pSaticActorResult->pTargetActor   = pDynamicActor;
        }

        // �A�N�^�[�̍��W�X�V
        gfl2::math::Vector3 newDynamicActorPos( adjustVec + pDynamicActor->GetPosition() );
        pDynamicActor->SetPosition( newDynamicActorPos );
      }
    }
  }
}

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
int CollisionSolverDefault::TestSphereMesh(BaseActor*           pDynamicActor,
                                           SphereShape*         pDynamicActorSphere,
                                           BaseActor*           pStaticActor,
                                           MeshShape*           pStaticActorMesh,
                                           gfl2::math::Vector3* pAdjustVec)
{
  // �f�[�^�s���`�F�b�N
  if( !pDynamicActor
   || !pDynamicActorSphere
   || !pStaticActorMesh
   || !pStaticActorMesh
   || !pStaticActorMesh->m_pCollisionModel
   || !pStaticActorMesh->m_pCollisionModel->GetCollisionMeshNode() )
  {
    return 0;
  }

  // �R���W�����p�W�I���g���̎擾
  gfl2::renderingengine::scenegraph::resource::CollisionMeshResourceNode* pColMeshNode = pStaticActorMesh->m_pCollisionModel->GetCollisionMeshNode();

  // �S�g���C�A���O���ƏՓˌ��o
  gfl2::math::Vector3 tempClosestPt;
  f32                 minDefLength  = -1.0f;
  u32                 triangleCount = pColMeshNode->GetCollisionTriangleCount();
  for (u32 i = 0; i<triangleCount ; ++i)
  {
    // �Փ˂��Ă��邩�H
    const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle = pColMeshNode->GetCollisionTriangle(i);
    const gfl2::math::Vector3 trianglePos0( pTriangle->m_Positions[0].x , pTriangle->m_Positions[0].y , pTriangle->m_Positions[0].z );
    const gfl2::math::Vector3 trianglePos1( pTriangle->m_Positions[1].x , pTriangle->m_Positions[1].y , pTriangle->m_Positions[1].z );
    const gfl2::math::Vector3 trianglePos2( pTriangle->m_Positions[2].x , pTriangle->m_Positions[2].y , pTriangle->m_Positions[2].z );

    int hit = TestSphereTriangle( pDynamicActorSphere->m_r,
                                  pDynamicActorSphere->m_pos + pDynamicActor->GetPosition(),
                                  gfl2::math::Vector3( trianglePos0 + pStaticActor->GetPosition() ),
                                  gfl2::math::Vector3( trianglePos1 + pStaticActor->GetPosition() ),
                                  gfl2::math::Vector3( trianglePos2 + pStaticActor->GetPosition() ),
                                  &tempClosestPt );

    if( !hit )
    {
      continue;
    }

    // ���b�V���S�̂ł̍ŋߐړ_�����
    gfl2::math::Vector3 def       = tempClosestPt - ( pDynamicActorSphere->m_pos + pDynamicActor->GetPosition() );
    f32                 defLength = def.Length();
    if( minDefLength < 0.0f || defLength < minDefLength )
    {
      // �ŋߐړ_�̋������X�V
      minDefLength = defLength;

      // �����Ԃ��x�N�g���̎擾
      gfl2::math::Vector3 adjustVec    = def.Normalize() * -1.0f;
      f32                 adjustVecLen = pDynamicActorSphere->m_r - defLength;
      *pAdjustVec = adjustVec * (adjustVecLen+MARJIN);
    }
  }

  if( minDefLength < 0.0f )
  {
    return 0;
  }
  return 1;
}

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
int CollisionSolverDefault::TestSphereTriangle( const f32                  r,
                                                const gfl2::math::Vector3& pos,
                                                const gfl2::math::Vector3& a,
                                                const gfl2::math::Vector3& b,
                                                const gfl2::math::Vector3& c,
                                                gfl2::math::Vector3*       pClosestPt )
{
  // ���̒��S�ɑ΂���ŋߐړ_�ł���O�p�`ABC��ɂ���_P��������
  *pClosestPt = ClosestPtPointTriangle( pos , a,b,c );

  // ���ƎO�p�`����������̂́A���̒��S����_P�܂ł�(��������)������(��������)���̔��a�����������ꍇ
  gfl2::math::Vector3 v = *pClosestPt - pos;
  return v.Dot(v) <= r * r;
}

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
int CollisionSolverDefault::TestCylinderCylinder(BaseActor*           pDynamicActor,
                                                 CylinderShape*       pDynamicActorCylinder,
                                                 BaseActor*           pStaticActor,
                                                 CylinderShape*       pStaticActorCylinder,
                                                 gfl2::math::Vector3* pAdjustVec )
{
  // DynamicActor�̍��W���擾
  gfl2::math::Vector3 dynamicActorPos( pDynamicActor->GetPosition() );

  // StaticActor�̍��W���擾
  gfl2::math::Vector3 staticActorPos( pStaticActor->GetPosition() );

  // ��̃X�t�B�A�Ԃ̋������擾
  gfl2::math::Vector3 worldDynamicActorSphere( dynamicActorPos + pDynamicActorCylinder->m_pos ); worldDynamicActorSphere.y = 0.0f;
  gfl2::math::Vector3 worldStaticActorSphere ( staticActorPos  +  pStaticActorCylinder->m_pos ); worldStaticActorSphere.y  = 0.0f;
  gfl2::math::Vector3 vecFromStaticActorToDynamicActor = worldDynamicActorSphere - worldStaticActorSphere;
  f32                 lenFromStaticActorToDynamicActor = vecFromStaticActorToDynamicActor.Length();

  // �Փ˔���
  if( lenFromStaticActorToDynamicActor < pDynamicActorCylinder->m_r + pStaticActorCylinder->m_r )
  {
    // �����Ԃ��x�N�g���̍쐬
    f32 osikaeshi = (pDynamicActorCylinder->m_r + pStaticActorCylinder->m_r) - lenFromStaticActorToDynamicActor;
    gfl2::math::Vector3 normalVecFromStaticActorToDynamicActor = vecFromStaticActorToDynamicActor.Normalize();
    *pAdjustVec = normalVecFromStaticActorToDynamicActor * (osikaeshi + MARJIN);

    return true;
  }
  return false;
}

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
int CollisionSolverDefault::TestCylinderBox(BaseActor*           pDynamicActor,
                                            CylinderShape*       pDynamicActorCylinder,
                                            BaseActor*           pStaticActor,
                                            BoxShape*            pStaticActorBox,
                                            gfl2::math::Vector3* pAdjustVec )
{
  // DynamicActor�̍��W���擾
  gfl2::math::Vector3 dynamicActorPos( pDynamicActor->GetPosition() );

  // StaticActor�̍��W���擾
  gfl2::math::Vector3 staticActorPos( pStaticActor->GetPosition() );

  // ���O����
  gfl2::math::Vector3 worldDynamicActorSphere( dynamicActorPos + pDynamicActorCylinder->m_pos ); worldDynamicActorSphere.y = 0.0f;
  gfl2::math::Vector3 worldStaticActorBox    ( staticActorPos  +       pStaticActorBox->m_pos );     worldStaticActorBox.y = 0.0f;
  f32 dotX = pStaticActorBox->m_u0.Dot( worldStaticActorBox - worldDynamicActorSphere );
  f32 dotY = pStaticActorBox->m_u1.Dot( worldStaticActorBox - worldDynamicActorSphere );

  // �Փ˔���
  if( gfl2::math::Abs(dotX) > pStaticActorBox->m_e0 + pDynamicActorCylinder->m_r 
   || gfl2::math::Abs(dotY) > pStaticActorBox->m_e1 + pDynamicActorCylinder->m_r )
  {
    return false;
  }

  // X���ɒ��s����ӂɋ߂��̂��AY���ɒ��s����ӂɋ߂��̂�����
  f32 defX = (pStaticActorBox->m_e0 + pDynamicActorCylinder->m_r) - gfl2::math::Abs(dotX);
  f32 defY = (pStaticActorBox->m_e1 + pDynamicActorCylinder->m_r) - gfl2::math::Abs(dotY);

  // X���ɒ��s����ӂɋ߂�
  if( defX < defY )
  {
    // X���ɒ��s����ӂ̍��ӂɓ������Ă���
    if( dotX>0.0f )
    {
      *pAdjustVec = gfl2::math::Vector3(-1.0f*defX*pStaticActorBox->m_u0.x,
                                        -1.0f*defX*pStaticActorBox->m_u0.y,
                                        -1.0f*defX*pStaticActorBox->m_u0.z );
    }
    // X���ɒ��s����ӂ̉E�ӂɓ������Ă���
    else
    {
      *pAdjustVec = gfl2::math::Vector3(defX*pStaticActorBox->m_u0.x,
                                        defX*pStaticActorBox->m_u0.y,
                                        defX*pStaticActorBox->m_u0.z );
    }
  }
  // Y���ɒ��s����ӂɋ߂�
  else
  {
    // Y���ɒ��s����ӂ̏�ӂɓ������Ă���
    if( dotY>0.0f )
    {
      *pAdjustVec =  gfl2::math::Vector3(-1.0f*defY*pStaticActorBox->m_u1.x,
                                         -1.0f*defY*pStaticActorBox->m_u1.y,
                                         -1.0f*defY*pStaticActorBox->m_u1.z );
    }
    // Y���ɒ��s����ӂ̉��ӂɓ������Ă���
    else
    {
      *pAdjustVec =  gfl2::math::Vector3(defY*pStaticActorBox->m_u1.x,
                                         defY*pStaticActorBox->m_u1.y,
                                         defY*pStaticActorBox->m_u1.z );
    }
  }

  return true;
}

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
int CollisionSolverDefault::TestCylinderLine    ( BaseActor*           pDynamicActor,
                                                  CylinderShape*       pDynamicActorCylinder,
                                                  BaseActor*           pStaticActor,
                                                  LineShape*           pStaticActorLine,
                                                  gfl2::math::Vector3* pAdjustVec)
{
  // �ŋߐړ_�̎擾
  gfl2::math::Vector3 worldDynamicActorSphere( pDynamicActor->GetPosition() + pDynamicActorCylinder->m_pos  ); worldDynamicActorSphere.y = 0.0f;
  gfl2::math::Vector3 worldStaticActorLine0  (  pStaticActor->GetPosition() +      pStaticActorLine->m_pos0 ); worldStaticActorLine0.y   = 0.0f;
  gfl2::math::Vector3 worldStaticActorLine1  (  pStaticActor->GetPosition() +      pStaticActorLine->m_pos1 ); worldStaticActorLine1.y   = 0.0f;
  gfl2::math::Vector3 closestPt              ( ClosestPtPointSegment( worldDynamicActorSphere , worldStaticActorLine0 , worldStaticActorLine1 ) );

  // �Փ˔���
  gfl2::math::Vector3 def( worldDynamicActorSphere - closestPt );
  if( def.Length() > pDynamicActorCylinder->m_r )
  {
    return false;
  }

  // �����Ԃ��x�N�g���̍쐬
  f32 osikaeshi = pDynamicActorCylinder->m_r - def.Length();
  gfl2::math::Vector3 normal = def.Normalize();
  *pAdjustVec = normal * (osikaeshi + MARJIN);

  return true;
}

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
                                                                  const gfl2::math::Vector3& b )
{
  // �p�����[�^������Ă���ʒu d(t) = a + t*(b-a)�̌v�Z�ɂ��ab��c���ˉe
  gfl2::math::Vector3 ab = b-a;
  gfl2::math::Vector3 ac = p-a;
  f32 t = ac.Dot(ab) / ab.Dot(ab);

  // �����̊O���ɂ���ꍇ�At(�]����d)���ŋߐړ_�܂ŃN�����v
  if( t<0.0f ) t = 0.0f;
  if( t>1.0f ) t = 1.0f;

  // �N�����v����Ă���t����̎ˉe����Ă���ʒu���v�Z
  gfl2::math::Vector3 d(a.x + t * ab.x,
                        a.y + t * ab.y,
                        a.z + t * ab.z);
  return d;
}

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
gfl2::math::Vector3 CollisionSolverDefault::ClosestPtPointTriangle(const gfl2::math::Vector3& p,
                                                                   const gfl2::math::Vector3& a,
                                                                   const gfl2::math::Vector3& b,
                                                                   const gfl2::math::Vector3& c)
{
  // P��A�̊O���̒��_�̈�̒��ɂ��邩�ǂ����`�F�b�N
  gfl2::math::Vector3 ab = b-a;
  gfl2::math::Vector3 ac = c-a;
  gfl2::math::Vector3 ap = p-a;
  f32                 d1 = ab.Dot(ap);
  f32                 d2 = ac.Dot(ap);
  if( d1 <= 0.0f && d2 <= 0.0f )
  {
    return a; // �d�S���W(1,0,0)
  }

  // P��B�̊O���̒��_�̈�̒��ɂ��邩�ǂ����`�F�b�N
  gfl2::math::Vector3 bp = p-b;
  f32                 d3 = ab.Dot(bp);
  f32                 d4 = ac.Dot(bp);
  if( d3 >= 0.0f && d4 <= d3 )
  {
    return b;  // �d�S���W(0,1,0)
  }

  // P��AB�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����P��AB��ɑ΂���ˉe��Ԃ�
  f32 vc = d1*d4 - d3*d2;
  if( vc <= 0.0f && d1 > 0.0f && d3 <= 0.0f )
  {
    f32 v = d1 / (d1-d3);
    //return a + v * ab; // �d�S���W(1-v,v,0)
    gfl2::math::Vector3 temp0( v*ab.x, v*ab.y, v*ab.z ); // �d�S���W(1-v,v,0)
    gfl2::math::Vector3 temp1( a+temp0 );                // �d�S���W(1-v,v,0)
    return temp1;
  }

  // P��C�̊O���̒��_�̈�̒��ɂ��邩�ǂ����`�F�b�N
  gfl2::math::Vector3 cp = p-c;
  f32 d5 = ab.Dot(cp);
  f32 d6 = ac.Dot(cp);
  if( d6 >= 0.0f && d5 <= d6 )
  {
    return c; // �d�S���W(0,0,1)
  }

  // P��AC�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����P��AC��ɑ΂���ˉe��Ԃ�
  f32 vb = d5*d2 - d1*d6;
  if( vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
  {
    f32 w = d2 / (d2-d6);
    //return a + w * ac; // �d�S���W(1-w,0,0)
    gfl2::math::Vector3 temp0( w*ac.x, w*ac.y, w*ac.z ); // �d�S���W(1-w,0,0)
    gfl2::math::Vector3 temp1( a+temp0 );                // �d�S���W(1-w,0,0)
    return temp1;
  }

  // P��BC�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����P��BC��ɑ΂���ˉe��Ԃ�
  f32 va = d3*d6 - d5*d4;
  if( va <= 0.0f && (d4-d3) >= 0.0f && (d5-d6) >= 0.0f )
  {
    f32 w = (d4-d3) / ((d4-d3) + (d5-d6));
    //return b + w * (c-b); // �d�S���W(0,1-w,w)
    gfl2::math::Vector3 temp0( c-b );      // �d�S���W(1-v,v,0)
    gfl2::math::Vector3 temp1( w*temp0.x,  w*temp0.y,  w*temp0.z); // �d�S���W(1-v,v,0)
    gfl2::math::Vector3 temp2( b+temp1 );  // �d�S���W(1-v,v,0)
    return temp2;
  }

  // P�͖ʗ̈�̒��ɂ���BQ�����̏d�S���W(u,v,w)��p���Čv�Z
  f32 denom = 1.0f / (va+vb+vc);
  f32 v = vb * denom;
  f32 w = vc * denom;
  return a + ab * v + ac * w;
}

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
gfl2::math::Vector3 CollisionSolverDefault::ClosestPtPointRect(const gfl2::math::Vector3& p,
                                                               const gfl2::math::Vector3& rectC,
                                                               const gfl2::math::Vector3& rectU0,
                                                               const gfl2::math::Vector3& rectU1,
                                                               const f32                  rectE0,
                                                               const f32                  rectE1)
{
  gfl2::math::Vector3 d = p - rectC;

  // �{�b�N�X�̒��S�ɂ����錋�ʂ���J�n�A��������i�K�I�ɐi�߂�
  gfl2::math::Vector3 closestPt( rectC );

  // �e�����`�̎��ɑ΂���
  for( u32 i=0 ; i<2 ; i++ )
  {
    gfl2::math::Vector3 rectU( i==0 ? rectU0 : rectU1 );
    f32                 rectE( i==0 ? rectE0 : rectE1 );

    // ...d�����̎��Ɏˉe����
    // �����`�̒��S����d�̎��ɉ����������𓾂�
    f32 dist = d.Dot(rectU);

    // �����`�͈̔͂����������傫���ꍇ�A�����`�܂ŃN�����v
    if( dist >  rectE )
    {
      dist =  rectE;
    }
    if( dist < -rectE )
    {
      dist = -rectE;
    }

    // ���[���h���W�𓾂邽�߂ɂ��̋����������ɉ����Đi�߂�
    closestPt += gfl2::math::Vector3( dist*rectU.x , dist*rectU.y, dist*rectU.z);
  }

  return closestPt;
}