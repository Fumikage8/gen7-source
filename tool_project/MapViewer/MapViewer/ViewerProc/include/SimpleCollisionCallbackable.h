//*************************************************************************************************
/**
 * @file SimpleCollisionCallbackable.h
 * @brief �R���W�������f���̃R�[���o�b�N�N���X
 * @date 2015.02.10
 * @author saita_kazuki
 */
//*************************************************************************************************

#if !defined( SIMPLECOLLISIONCALLBACKABLE_H_INCLUDED )
#define SIMPLECOLLISIONCALLBACKABLE_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <Collision/include/gfl2_CollisionModel.h>
#include <Math/include/gfl2_Vector3.h>

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )

//=================================================================================================
// �R���W�������C�u�����A��������R�[���o�b�N�N���X
//=================================================================================================
class SimpleCollisionCallbackable_Intersection : public gfl2::collision::ICollisionModelIntersectionCallbackable
{
public:

  static const u32 HIT_COUNT_MAX = 10;  ///< �Փˏ����i�[����ő吔

  ///< �Փˏ��
  struct HIT_DATA
  {
    // @todo �R���W�������f���Ŏg�p����Ă���̂�Vector�Ȃ̂ł�����Vector�BVector3�ɒu�����������BGetNearestIntersection()�����ł�Vector�g�p�B
    gfl2::math::Vector intersection;                                                 ///< ��_
    const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle;  ///< �|���S�����
  };

  // �R���X�g���N�^�A�f�X�g���N�^
  SimpleCollisionCallbackable_Intersection();
  virtual ~SimpleCollisionCallbackable_Intersection();

  /**
   * @brief �����蔻��R�[���o�b�N�֐�
   */
  virtual bool CollisionModelIntersectionCallback( const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, gfl2::math::Vector& intersection);

  /**
   * @brief �w����W�ƍł��߂���_��Ԃ�
   * @param ��ׂ���W
   * @return ��_���W
   */
  const gfl2::math::Vector& GetNearestIntersection( const gfl2::math::Vector& pos) const ;

  /**
   * @brief 1�x�ł�������������Ԃ�
   * @retval true ��������
   * @retval false �������Ă��Ȃ�
   */
  bool IsHit() const;

  /**
   * @brief �Ď擾�\�ȏ�Ԃɂ���
   */
  void Clear( void);

private:

  u32       m_hitCount;                         // ����������
  HIT_DATA  m_hitDataTable[ HIT_COUNT_MAX ];    // �Փˏ��o�b�t�@
};

//=================================================================================================
// �R���W�������C�u�����A��������R�[���o�b�N�N���X
//=================================================================================================
class SimpleCollisionCallbackable_Height : public gfl2::collision::ICollisionModelCallbackable
{
public:

  static const u32 HIT_COUNT_MAX = 10;  ///< �Փˏ����i�[����ő吔

  ///< �Փˏ��
  struct HIT_DATA
  {
    f32   height;
    const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle;
  };

  // �R���X�g���N�^�A�f�X�g���N�^
  SimpleCollisionCallbackable_Height();
  virtual ~SimpleCollisionCallbackable_Height();

  /**
   * @brief �����蔻��R�[���o�b�N�֐�
   */
  virtual bool CollisionModelHeightCallback( const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, f32 height);

  /**
   * @brief �w��̍����Ɉ�ԋ߂��������擾����
   * @note �w�肵�����������Ⴂ�Ƃ���ł����������Ă��Ȃ��ꍇ�͎w�肵���������̂܂܂�Ԃ�
   * @param ��ׂ鍂��
   * @return �Փ˃|���S���̍���
   */
  f32 GetNearestHeight( f32 height) const ;

  /**
   * @brief 1�x�ł�������������Ԃ�
   * @retval true ��������
   * @retval false �������Ă��Ȃ�
   */
  bool IsHit() const;

  /**
   * @brief �Ď擾�\�ȏ�Ԃɂ���
   */
  void Clear( void);

private:

  u32       m_hitCount;                         // ����������
  HIT_DATA  m_hitDataTable[ HIT_COUNT_MAX ];    // �Փˏ��o�b�t�@
};

//=================================================================================================
// �R���W�������C�u�����A���C�J�X�^���R�[���o�b�N�N���X
//=================================================================================================
class customCollisionCallback_Intersection : public gfl2::collision::ICollisionModelIntersectionCallbackable
{
public:

  ///< �Փˏ��
  struct HIT_DATA
  {
    gfl2::math::Vector intersection;
    const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle;
  };

  /**
   * @brief �R���X�g���N�^
   */
  customCollisionCallback_Intersection();

  /**
   * @brief �f�X�g���N�^
   */
  virtual ~customCollisionCallback_Intersection();

  /**
   * @brief �����蔻��R�[���o�b�N�֐�
   */
  virtual bool CollisionModelIntersectionCallback(const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, gfl2::math::Vector& intersection);

  /**
   * @brief ���C�̈�ԋ߂���_���擾����
   * @return �Փ˃f�[�^
   */
  HIT_DATA GetHitData(void) const;

  /**
   * @brief 1�x�ł�������������Ԃ�
   * @retval true ��������
   * @retval false �������Ă��Ȃ�
   */
  bool IsHit(void) const;

private:

  b32       m_isHit;
  HIT_DATA  m_hitData;

};

GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )

#endif // SIMPLECOLLISIONCALLBACKABLE_H_INCLUDED
