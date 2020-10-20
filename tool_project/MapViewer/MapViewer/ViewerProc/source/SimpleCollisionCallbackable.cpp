//*************************************************************************************************
/**
 * @file SimpleCollisionCallbackable.cpp
 * @brief �R���W�������f���̃R�[���o�b�N�N���X
 * @date 2015.02.10
 * @author saita_kazuki
 */
//*************************************************************************************************

#include "../include/SimpleCollisionCallbackable.h"
#include <math/include/gfl2_math_control.h>

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )

//=================================================================================================
// �R���W�������C�u�����A��_����R�[���o�b�N�N���X
//=================================================================================================

// �R���X�g���N�^
SimpleCollisionCallbackable_Intersection::SimpleCollisionCallbackable_Intersection() :
  m_hitCount( 0 )
{
  for( u32 i = 0; i < HIT_COUNT_MAX; i++)
  {
    m_hitDataTable[i].intersection = gfl2::math::Vector::GetZero();
    m_hitDataTable[i].pTriangle = NULL;
  }
}

// �f�X�g���N�^
SimpleCollisionCallbackable_Intersection::~SimpleCollisionCallbackable_Intersection()
{
}

// �����蔻��R�[���o�b�N�֐�
bool SimpleCollisionCallbackable_Intersection::CollisionModelIntersectionCallback( const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, gfl2::math::Vector& intersection)
{
  // ��_���L��
  m_hitDataTable[m_hitCount].intersection = intersection;
  m_hitDataTable[m_hitCount].pTriangle = pTriangle;
  m_hitCount++;

  if( m_hitCount >= HIT_COUNT_MAX)
  {
    // �Փˌ��o�I��
    return false;
  }
  else
  {
    // �Փˌ��o���s
    return true;
  }
}

// �w����W�ƍł��߂���_��Ԃ�
const gfl2::math::Vector& SimpleCollisionCallbackable_Intersection::GetNearestIntersection( const gfl2::math::Vector& pos) const
{
  gfl2::math::Vector vec = pos - m_hitDataTable[0].intersection;
  f32 resultDiff = vec.Length3();
  u32 hitIndex = 0;
  for( int i = 1; i < m_hitCount; ++i)
  {
    vec = pos - m_hitDataTable[i].intersection;
    f32 diff = vec.Length3();
    if( diff < resultDiff)
    {
      resultDiff = diff;
      hitIndex = i;
    }
  }

  return m_hitDataTable[hitIndex].intersection;
}

// ����������
bool SimpleCollisionCallbackable_Intersection::IsHit() const
{
  return ( m_hitCount != 0 );
}

// �Ď擾�\�ȏ�Ԃɂ���
void SimpleCollisionCallbackable_Intersection::Clear( void)
{
  m_hitCount = 0;
}

//=================================================================================================
// �R���W�������C�u�����A��������R�[���o�b�N�N���X
//=================================================================================================

// �R���X�g���N�^
SimpleCollisionCallbackable_Height::SimpleCollisionCallbackable_Height() :
  m_hitCount( 0 )
{
  for( u32 i = 0; i < HIT_COUNT_MAX; i++)
  {
    m_hitDataTable[i].height = 0.0f;
    m_hitDataTable[i].pTriangle = NULL;
  }
}

// �f�X�g���N�^
SimpleCollisionCallbackable_Height::~SimpleCollisionCallbackable_Height()
{
}

// �����蔻��R�[���o�b�N�֐� ��_�擾
bool SimpleCollisionCallbackable_Height::CollisionModelHeightCallback( const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, f32 height)
{
  // ��_���L��
  m_hitDataTable[m_hitCount].height = height;
  m_hitDataTable[m_hitCount].pTriangle = pTriangle;
  m_hitCount++;

  if( m_hitCount >= HIT_COUNT_MAX)
  {
    // �Փˌ��o�I��
    return false;
  }
  else
  {
    // �Փˌ��o���s
    return true;
  }
}

// �w��̍����Ɉ�ԋ߂��������擾����
f32 SimpleCollisionCallbackable_Height::GetNearestHeight(f32 height) const
{
  // ��ԋ߂��Փ˃��b�V����T��
  f32 resultDiff = FLT_MAX;
  s32 hitIndex = -1;
  for( u32 i = 0; i < m_hitCount; i++)
  {
    {
      f32 diff = gfl2::math::Abs( m_hitDataTable[i].height - height);
      if( diff < resultDiff)
      {
        resultDiff = diff;
        hitIndex = i;
      }
    }
  }

  if( hitIndex != -1)
  {
    return m_hitDataTable[hitIndex].height;
  }
  return height;
}

// ����������
bool SimpleCollisionCallbackable_Height::IsHit() const
{
  return ( m_hitCount != 0 );
}

// �Ď擾�\�ȏ�Ԃɂ���
void SimpleCollisionCallbackable_Height::Clear( void)
{
  m_hitCount = 0;
}

//=================================================================================================
// �R���W�������C�u�����A���C�J�X�^���R�[���o�b�N�N���X
//=================================================================================================

// �R���X�g���N�^
customCollisionCallback_Intersection::customCollisionCallback_Intersection()
: m_isHit(false)
{
}

// �f�X�g���N�^
customCollisionCallback_Intersection::~customCollisionCallback_Intersection()
{
}

// �����蔻��R�[���o�b�N�֐� ��_�擾
bool customCollisionCallback_Intersection::CollisionModelIntersectionCallback(const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, gfl2::math::Vector& intersection)
{
  if( !m_isHit || m_hitData.intersection.Length() > intersection.Length() )
  {
    m_hitData.intersection  = intersection;
    m_hitData.pTriangle     = pTriangle;
    m_isHit                 = true;
  }
  return true;
}

// ���C�̈�ԋ߂���_���擾����
customCollisionCallback_Intersection::HIT_DATA customCollisionCallback_Intersection::GetHitData(void) const
{
  return m_hitData;
}

// ����������
bool customCollisionCallback_Intersection::IsHit() const
{
  return m_isHit;
}

GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )
