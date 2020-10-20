//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dObjectManager.cpp
 *	@brief  3D�I�u�W�F�N�g�}�l�[�W���[
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C

// gflib

// gflib grp g3d
#include <grp/g3d/gfl_G3dObject.h>

#include <grp/g3d/gfl_G3dObjectManager.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�֐���`
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/*
 *  @brief         �I�u�W�F�N�g��ID��ݒ肷��
 *                 class Object�̃t�����h�֐�
 *
 *  @param[in]     object    �I�u�W�F�N�g 
 *  @param[in]     a_id      ID
 */
//-----------------------------------------------------------------------------
void Object_SetId(Object* object, s32 a_id)
{
  object->m_id   = a_id;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �I�u�W�F�N�g�Ɏ�ނ�ݒ肷��
 *                 class Object�̃t�����h�֐�
 *
 *  @param[in]     object    �I�u�W�F�N�g 
 *  @param[in]     a_type    �I�u�W�F�N�g�̎��
 */
//-----------------------------------------------------------------------------
void Object_SetType(Object* object, ObjectType a_type)
{
  object->m_type = a_type;
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �I�u�W�F�N�g�̏����o�͂���
 *  
 *  @param[in]  object  �I�u�W�F�N�g
 */
//-----------------------------------------------------------------------------
void ObjectManager::PrintObjectInformation(Object* object) const
{
#if GFL_DEBUG
  static const char* OBJECT_TYPE_NAME_ARRAY[OBJECT_TYPE_MAX] =
  {
    "OBJECT_TYPE_UNKNOWN",                  
    "OBJECT_TYPE_RESOURCE",                 
    "OBJECT_TYPE_RES_ANIM",                 
    "OBJECT_TYPE_ANIMATION",                
    "OBJECT_TYPE_UNIT_ANIMATION",           
    "OBJECT_TYPE_TEXTURE",                  
    "OBJECT_TYPE_PARTICLE",                 
    "OBJECT_TYPE_SCENE_ENVIRONMENT_SETTING",
    "OBJECT_TYPE_MODEL",                    
    "OBJECT_TYPE_CAMERA",                   
    "OBJECT_TYPE_STEREO_CAMERA",            
    "OBJECT_TYPE_LIGHT",                    
    "OBJECT_TYPE_FOG",                      
    "OBJECT_TYPE_SCENE",                    
    "OBJECT_TYPE_H3D_MODEL",                
  };
  GFL_PRINT("%s\n", OBJECT_TYPE_NAME_ARRAY[m_object_type]);

  switch(m_object_type)
  {
  case OBJECT_TYPE_UNKNOWN:
    {
      break;
    }
  case OBJECT_TYPE_RESOURCE:
    {
      //gfl::grp::g3d::Resource* resource = dynamic_cast<gfl::grp::g3d::Resource*>(object);
      //resource->Dump();
      break;
    }
  case OBJECT_TYPE_RES_ANIM:
    {
      break;
    }
  case OBJECT_TYPE_ANIMATION:
    {
      break;
    }
  case OBJECT_TYPE_UNIT_ANIMATION:
    {
      break;
    }
  case OBJECT_TYPE_TEXTURE:
    {
      break;
    }
  case OBJECT_TYPE_PARTICLE:
    {
      break;
    }
  case OBJECT_TYPE_SCENE_ENVIRONMENT_SETTING:
    {
      break;
    }
  case OBJECT_TYPE_MODEL:
    {
      break;
    }
  case OBJECT_TYPE_CAMERA:
    {
      break;
    }
  case OBJECT_TYPE_STEREO_CAMERA:
    {
      break;
    }
  case OBJECT_TYPE_LIGHT:
    {
      break;
    }
  case OBJECT_TYPE_FOG:
    {
      break;
    }
  case OBJECT_TYPE_SCENE:
    {
      break;
    }
  case OBJECT_TYPE_H3D_MODEL:
    {
      break;
    }
  default:
    {
      GFL_PRINT("�I�u�W�F�N�g�^�C�v���s���ł��BOBJECT_TYPE_UNKNOWN�ł�����܂���B%d\n", m_object_type);
      break;
    }
  }
#endif  // #if GFL_DEBUG
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

