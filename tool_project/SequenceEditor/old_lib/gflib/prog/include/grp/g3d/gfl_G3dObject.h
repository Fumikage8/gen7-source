#ifndef __GFL_G3DOBJECT_H__
#define __GFL_G3DOBJECT_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dObject.h
 *	@brief  3D�I�u�W�F�N�g
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/ut.h>

// gflib
#include <gfl_Macros.h>


namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�I�u�W�F�N�g��ID
//=====================================
enum
{
  OBJECT_ID_NOT_MANAGED       = -1  // ObjectManager�ɊǗ�����Ă��Ȃ����Ƃ�����ID�̒l�B
  // 0�ȏ��ObjectManager�ɊǗ�����Ă���A�I�u�W�F�N�g���g��ID�ƂȂ�B
};

//-------------------------------------
///	���̃I�u�W�F�N�g�����������\�[�X��ID
//=====================================
enum
{
  OBJECT_RESOURCE_ID_NOT_USE    = -1,  // ���\�[�X���g���Ă��Ȃ����Ƃ�����ID�̒l�B
  OBJECT_RESOURCE_ID_PARTICLE   = -2   // �p�[�e�B�N���̂��߃��\�[�X���̕����̃f�[�^����I�u�W�F�N�g�����������Ƃ�����ID�̒l�B
  // 0�ȏ�̓��\�[�X���g���Ă���A���̃��\�[�X��ID�ƂȂ�B
};

//-------------------------------------
///	���\�[�X���ɂ����邱�̃I�u�W�F�N�g�̃C���f�b�N�X
//=====================================
enum
{
  OBJECT_INDEX_IN_RESOURCE_NOT_USE    = -1  // ���\�[�X���g���Ă��Ȃ����Ƃ������C���f�b�N�X�̒l�B
  // 0�ȏ�̓��\�[�X���g���Ă���A���̃��\�[�X���ł̃C���f�b�N�X�ƂȂ�B
};

//-------------------------------------
///	���[�U���C�ӂŕt������ID
//=====================================
enum
{
  OBJECT_USER_ID_NOT_USE    = -1,  // ���[�U���C�ӂŕt������ID��t���Ă��Ȃ����Ƃ�����ID�̒l�B
  OBJECT_USER_ID_ANYTHING   = -2   // ���[�U���C�ӂŕt������ID�͉��ł������B���[�U���C�ӂŕt������ID���w�肹���Ɍ�������ꍇ�ɗp����ID�̒l�B
  // 0�ȏ�̓��[�U���C�ӂŕt������ID��t���Ă���A���̃I�u�W�F�N�g�Ƀ��[�U���t����ID�ƂȂ�B
};

//-------------------------------------
///	�I�u�W�F�N�g�̏��L��
//=====================================
typedef enum
{
  OBJECT_OWNER_SCENE,  // Scene���Ǘ����Ă���I�u�W�F�N�g�BScene��������s���B
  OBJECT_OWNER_USER    // ���[�U���Ǘ����Ă���I�u�W�F�N�g�B���[�U��������s���B
}
ObjectOwner;

//-------------------------------------
///	�I�u�W�F�N�g�̏��L�҂�ID
//=====================================
enum
{
  OBJECT_OWNER_ID_USER    = -1  // ���[�U���Ǘ����Ă���I�u�W�F�N�g�̏ꍇ(OBJECT_OWNER_USER)��ID�̒l�B
  // 0�ȏ�̓��[�U�ȊO���Ǘ����Ă���A���̊Ǘ����Ă�����̂�ID�ƂȂ�B(��FScene���Ǘ����Ă���ꍇ(OBJECT_OWNER_SCENE)�Ȃ�Scene��ID)
};

//-------------------------------------
///	�I�u�W�F�N�g�̎��
//=====================================
typedef enum
{
  // ���Ԃ���ёւ���\��������܂��̂ŁA
  // OBJECT_TYPE_MODEL<=type&&type<=OBJECT_TYPE_FOG
  // �Ƃ����g�����͂��Ȃ��ŉ������B
  OBJECT_TYPE_UNKNOWN,                    //  0
  
  OBJECT_TYPE_RESOURCE,                   //  1

  OBJECT_TYPE_RES_ANIM,                   //  2

  OBJECT_TYPE_ANIMATION,                  //  3
  OBJECT_TYPE_UNIT_ANIMATION,             //  4

  OBJECT_TYPE_TEXTURE,                    //  5

  OBJECT_TYPE_PARTICLE,                   //  6

  OBJECT_TYPE_SCENE_ENVIRONMENT_SETTING,  //  7

  OBJECT_TYPE_MODEL,                      //  8
  OBJECT_TYPE_CAMERA,                     //  9
  OBJECT_TYPE_STEREO_CAMERA,              // 10
  OBJECT_TYPE_LIGHT,                      // 11
  OBJECT_TYPE_FOG,                        // 12

  OBJECT_TYPE_SCENE,                      // 13

  OBJECT_TYPE_H3D_MODEL,                  // 14

  OBJECT_TYPE_MAX
}
ObjectType;


//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3D�I�u�W�F�N�g�N���X
//=====================================
class Object
{
  GFL_FORBID_COPY_AND_ASSIGN(Object);

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   */
  //-----------------------------------------------------------------------------
  Object(void)
    : m_id(OBJECT_ID_NOT_MANAGED),
      m_resource_id(OBJECT_RESOURCE_ID_NOT_USE),
      m_index_in_resource(OBJECT_INDEX_IN_RESOURCE_NOT_USE),
      m_user_id(OBJECT_USER_ID_NOT_USE),
      m_owner(OBJECT_OWNER_USER),
      m_owner_id(OBJECT_OWNER_ID_USER),
      m_type(OBJECT_TYPE_UNKNOWN)
  {}
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  virtual ~Object() {}

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �v�Z����
   */
  //-----------------------------------------------------------------------------
  virtual void Calculate(void) {}

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ID�𓾂�
   *
   *  @retval        s32   ID(0�ȏ�)
   */
  //-----------------------------------------------------------------------------
  s32 GetId(void) const {  return m_id;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���̃I�u�W�F�N�g�����������\�[�X��ID�𓾂�
   *
   *  @retval        s32   ���̃I�u�W�F�N�g�����������\�[�X��ID(0�ȏ�)
   */
  //-----------------------------------------------------------------------------
  s32 GetResourceId(void) const {  return m_resource_id;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���\�[�X���ɂ����邱�̃I�u�W�F�N�g�̃C���f�b�N�X�𓾂�
   *
   *  @retval        s32   ���\�[�X���ɂ����邱�̃I�u�W�F�N�g�̃C���f�b�N�X(0�ȏ�)
   */
  //-----------------------------------------------------------------------------
  s32 GetIndexInResource(void) const {  return m_index_in_resource;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���\�[�X���ɂ����邱�̃I�u�W�F�N�g�̃C���f�b�N�X�𓾂�
   *  @brief         ���[�U���C�ӂŕt������ID�𓾂�
   *
   *  @retval        s32   ���[�U���C�ӂŕt������ID(0�ȏ�)
   */
  //-----------------------------------------------------------------------------
  s32 GetUserId(void) const {  return m_user_id;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���̃I�u�W�F�N�g�����������\�[�X��ID�𓾂�
   *                 ���\�[�X���ɂ����邱�̃I�u�W�F�N�g�̃C���f�b�N�X�𓾂�
   *
   *  @param[out]    a_resource_id         ���̃I�u�W�F�N�g�����������\�[�X��ID(0�ȏ�)���i�[����
   *  @param[out]    a_index_in_resource   ���\�[�X���ɂ����邱�̃I�u�W�F�N�g�̃C���f�b�N�X(0�ȏ�)���i�[����
   */
  //-----------------------------------------------------------------------------
  void GetResourceIdAndIndexInResource(s32& a_resource_id, s32& a_index_in_resource ) const
  {
    a_resource_id       = GetResourceId();
    a_index_in_resource = GetIndexInResource();
  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���L�҂𓾂�
   *
   *  @retval        ObjectOwner   �I�u�W�F�N�g�̏��L��
   */
  //-----------------------------------------------------------------------------
  ObjectOwner GetOwner(void) const {  return m_owner;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���L�҂�ID�𓾂�
   *
   *  @retval        s32   ���L�҂�ID(0�ȏ�)
   */
  //-----------------------------------------------------------------------------
  s32 GetOwnerId(void) const {  return m_owner_id;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���L�҂𓾂�
   *                 ���L�҂�ID�𓾂�
   *
   *  @param[out]    a_owner      �I�u�W�F�N�g�̏��L�҂��i�[����
   *  @param[out]    a_owner_id   ���L�҂�ID(0�ȏ�)���i�[����
   */
  //-----------------------------------------------------------------------------
  void GetOwnerAndOwnerId(ObjectOwner& a_owner, s32& a_owner_id) const
  {
    a_owner     = GetOwner();
    a_owner_id  = GetOwnerId();
  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ��ނ𓾂�
   *
   *  @retval        ObjectType   �I�u�W�F�N�g�̎��
   */
  //-----------------------------------------------------------------------------
  ObjectType GetObjectType(void) const {  return m_type;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���[�U���C�ӂŕt������ID��ݒ肷��
   *                 ���[�U���Ǘ����Ă���I�u�W�F�N�g�łȂ��ꍇ�́A���[�U���C�ӂŕt������ID��ݒ�ł��Ȃ��B
   *
   *  @param[in]     a_user_id  ���[�U���C�ӂŕt������ID(0�ȏサ���ݒ肳�������Ȃ��̂�u16�ɂ���)
   * 
   *  @retval        bool   ���[�U���C�ӂŕt������ID��ݒ�ł�����true��Ԃ�
   */
  //-----------------------------------------------------------------------------
  bool SetUserId(u16 a_user_id)
  {
    if( m_owner == OBJECT_OWNER_USER )
    {
      m_user_id = a_user_id;
      return true;
    }
    return false;
  }

protected:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���̃I�u�W�F�N�g�����������\�[�X��ID��ݒ肷��
   *                 ���\�[�X���ɂ����邱�̃I�u�W�F�N�g�̃C���f�b�N�X��ݒ肷��
   *
   *  @param[in]     a_resource_id         ���̃I�u�W�F�N�g�����������\�[�X��ID
   *  @param[in]     a_index_in_resource   ���\�[�X���ɂ����邱�̃I�u�W�F�N�g�̃C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  void SetResourceIdAndIndexInResource(s32 a_resource_id, s32 a_index_in_resource)
  {
    m_resource_id       = a_resource_id;
    m_index_in_resource = a_index_in_resource;
  }

private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �I�u�W�F�N�g��ID��ݒ肷��
   *                 class Object�̃t�����h�֐�
   *
   *  @param[in]     object    �I�u�W�F�N�g 
   *  @param[in]     a_id      ID
   */
  //-----------------------------------------------------------------------------
  friend void Object_SetId(Object* object, s32 a_id);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �I�u�W�F�N�g�Ɏ�ނ�ݒ肷��
   *                 class Object�̃t�����h�֐�
   *
   *  @param[in]     object    �I�u�W�F�N�g 
   *  @param[in]     a_type    �I�u�W�F�N�g�̎��
   */
  //-----------------------------------------------------------------------------
  friend void Object_SetType(Object* object, ObjectType a_type);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���L�҂�ݒ肷��
   *                 ���L�҂�ID��ݒ肷��
   *                 class Object�̃t�����h�֐�
   *
   *  @param[in]     object       �I�u�W�F�N�g 
   *  @param[in]     a_owner      �I�u�W�F�N�g�̏��L��
   *  @param[in]     a_owner_id   ���L�҂�ID
   */
  //-----------------------------------------------------------------------------
  friend void Object_SetOwnerAndOwnerId(Object* object, ObjectOwner a_owner, s32 a_owner_id);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���[�U���C�ӂŕt������ID��ݒ肷��(�����I)
   *                 ���[�U���Ǘ����Ă��Ȃ��I�u�W�F�N�g�ł��A���[�U���C�ӂŕt������ID��ݒ�ł���B
   *
   *  @param[in]     object     �I�u�W�F�N�g 
   *  @param[in]     a_user_id  ���[�U���C�ӂŕt������ID
   */
  //-----------------------------------------------------------------------------
  friend void Object_SetUserIdForcedly(Object* object, s32 a_user_id);

private:
  s32           m_id;                 // OBJECT_ID                 // ID(0�ȏ�)
  s32           m_resource_id;        // OBJECT_RESOURCE_ID        // ���̃I�u�W�F�N�g�����������\�[�X��ID(0�ȏ�)
  s32           m_index_in_resource;  // OBJECT_INDEX_IN_RESOURCE  // ���\�[�X���ɂ����邱�̃I�u�W�F�N�g�̃C���f�b�N�X(0�ȏ�)
  s32           m_user_id;            // OBJECT_USER_ID            // ���[�U���C�ӂŕt������ID(0�ȏ�)
  ObjectOwner   m_owner;              // OBJECT_OWNER
  s32           m_owner_id;           // OBJECT_OWNER_ID           //	���L�҂�ID(0�ȏ�)
  ObjectType    m_type;               // OBJECT_TYPE
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DOBJECT_H__
