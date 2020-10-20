#ifndef __GFL_G3DDEFINE_H__
#define __GFL_G3DDEFINE_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dDefine.h
 *	@brief  3D��`
 *	@author	Koji Kawada
 *	@date		2011.03.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>

// gflib grp g3d


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					�萔��`
 */
//-----------------------------------------------------------------------------

//-------------------------------------
///	gfx_Config.h
//=====================================
#if defined(NW_GFX_VERTEX_LIGHT_ENABLED)
    #define GFL_GRP_G3D_VERTEX_LIGHT_ENABLE (1)  // ���_���C�g�L��
#else
    #define GFL_GRP_G3D_VERTEX_LIGHT_ENABLE (0)  // ���_���C�g����
#endif


//-------------------------------------
///	deprecated
//
#if 0
/*�񐄏�
�֐��̎g�p���֎~�������ۂ̈ڍs���Ԓ��ɕt���Ă����Ƃ悢�B
�t�����\�[�X���R���p�C������Ȃ��Ƃ��̃��b�Z�[�W�͕\������Ȃ��̂ŁA
�t������u�����Ɂv�u���ł��v�g���Ă���ꏊ�ɑ΂��ă��b�Z�[�W���o���킯�u�ł͂Ȃ��v�B

NintendoWare/CTR/include/nw/config/compiler.h
���Q�l�ɍ쐬�B

�g����
�񐄏��̊֐���GFL_GRP_G3D_DEPRECATED_FUNCTION�ƕt����΂悢�B

��:
gfl_G3dScene.h
GFL_GRP_G3D_DEPRECATED_FUNCTION(bool  AddSceneNode( SceneNode* scene_node ));

gfl::grp::g3d::Scene�N���X�̃����o�֐��̐錾�ɂ����t���Ă�����
c:\home\gflib_cpp\skelton\prog\src/system/launcher.cpp:920:12: Warning:  #1361-D
: function "gfl::grp::g3d::Scene::AddSceneNode" (declared at line 1287 of "c:\ho
me\gflib_cpp\gflib\prog\include\grp/g3d/gfl_G3dScene.h") was declared "deprecate
d"
    s_scene->AddSceneNode(s_StereoCamera);
             ^===============================================  ] 02117 / 02122
�Ƃ����ӂ��ɃR���p�C���Ń��b�Z�[�W���\�������B

�֐��̒�`�ɂ�GFL_GRP_G3D_DEPRECATED_FUNCTION�͕t�����Ȃ��̂Ő錾�ɂ����t���Ă����΂悢�B

�֐��̐錾�ƒ�`���ꏏ�ɍs���Ă���ꍇ��
GFL_GRP_G3D_DEPRECATED_FUNCTION(bool  AddSceneNode( SceneNode* scene_node ))
{
}
�Ƃ����ӂ��ɕt����΂悢�B

gfl_G3dResAnim.h
  GFL_GRP_G3D_DEPRECATED_FUNCTION( virtual void Create(
      gfl::heap::NwAllocator*           allocator, 
      Resource*                         resource,
      s32                               index_in_resource,
      Type                              type               = TYPE_SKELETAL
  ) );*/
#endif
//
//=====================================
//#define GFL_GRP_G3D_IGNORE_DEPRECATED  // ���ꂪ��`����Ă���Ƃ�GFL_GRP_G3D_DEPRECATED_FUNCTION�𖳎����܂��B

#if defined( __CC_ARM ) || defined( __ARMCC_VERSION )
  // ARM RealView Compiler.

  #define GFL_GRP_G3D_DEPRECATED_FUNCTION_MSG(MFunc, msg) MFunc __attribute__ ((deprecated))

#elif defined( __CWCC__ ) || defined( __MWERKS__ )
  // Freescale CodeWarrior

  #define GFL_GRP_G3D_DEPRECATED_FUNCTION_MSG(MFunc, msg) MFunc

#elif defined( __GNUC__ )
  // GNU C++

  #define GFL_GRP_G3D_DEPRECATED_FUNCTION_MSG(MFunc, msg) MFunc __attribute__ ((deprecated))

#elif defined( _MSC_VER )
  // Miscrosoft Visual C++

  #define GFL_GRP_G3D_DEPRECATED_FUNCTION_MSG(MFunc, msg) __declspec(deprecated(msg)) MFunc

#else

  #error "gfl::grp::g3d Unknown compiler"
    // #error�f�B���N�e�B�u�́A�w�肳�ꂽ���b�Z�[�W���o�͂��A�R���p�C����Ƃ𒆎~���܂��B

#endif

#if !defined(GFL_GRP_G3D_DEPRECATED_FUNCTION)
  #if defined(GFL_GRP_G3D_IGNORE_DEPRECATED)
    #define GFL_GRP_G3D_DEPRECATED_FUNCTION(MFunc) MFunc
  #else
    #define GFL_GRP_G3D_DEPRECATED_FUNCTION(MFunc) GFL_GRP_G3D_DEPRECATED_FUNCTION_MSG(MFunc, "## gfl::grp::g3d Deprecated function ##")
  #endif
#endif



//-------------------------------------
///	h3d
//=====================================
#define GFL_GRP_G3D_H3D_USE (1)  // ���ꂪ1�̂Ƃ�h3d���g�p����B���ꂪ0�̂Ƃ�h3d���g�p���Ȃ��B
#define GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT (GFL_DEBUG&&GFL_GRP_G3D_H3D_USE&&(0))  // ���ꂪ1�̂Ƃ��R�}���h�����J�E���g����

#ifdef BUGFIX_BTS7075_20130802  
#define GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH (GFL_GRP_G3D_H3D_USE&&(1))
#else
#define GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH (GFL_GRP_G3D_H3D_USE&&(0))  
#endif
    // ���ꂪ1�̂Ƃ�DrawMesh�ɂăr���{�[�h�v�Z���s���B
    // ���ꂪ0�̂Ƃ�SubmitView�ł����r���{�[�h�v�Z���s��Ȃ�
    // (���ꂪ0�̂Ƃ��N���蓾��s��F���f�����قȂ�J�����ŕ�����`�悷��Ƃ��ɁA
    // �Ō��SubmitView�����J�����̕��������f���������Ă���Ƃ����s�)�B



//-------------------------------------
///	���̑��萔
//=====================================
// enum��s32�ɂ���萔
enum
{
  ENUMS32MAX = 2100000000
};

// �z��̃C���f�b�N�X
//enum
//{
//  ARRAY_INDEX_NONE = -1  // �C���f�b�N�X�Ȃ�
//  // 0�ȏ�͗L���ȃC���f�b�N�X
//};




#if GFL_DEBUG
    #define GFL_GRP_G3D_MODEL_RECURSIVE_COUNT_MAX (16)  // �ċA�Ăяo���̉񐔏��
#endif




//-------------------------------------
///	3D�V�[���p�̒萔
//=====================================
// �V�[���v�Z�O���[�v�̃C���f�b�N�X
enum
{
  SCENE_CALCULATE_GROUP_INDEX_NONE    = -1  // �C���f�b�N�X�Ȃ�
  // 0�ȏ�͗L���ȃC���f�b�N�X
};

// �V�[���`��O���[�v�̃C���f�b�N�X
enum
{
  SCENE_DRAW_GROUP_INDEX_NONE         = -1,  // �C���f�b�N�X�Ȃ�
  SCENE_DRAW_GROUP_INDEX_MULTI        = -2   // �����̃V�[���`��O���[�v�ɏ������Ă��邱�Ƃ�����
  // 0�ȏ�͗L���ȃC���f�b�N�X
};

// �V�[���̏����������̓��e
enum SceneInitializeProcess
{
  // �K��SCENE_INITIALIZE_PROCESS_NONE����ԏ����Ȓl�ɂȂ�悤�ɂ��ĉ������B
  SCENE_INITIALIZE_PROCESS_NONE      = 240,  // �K�v�Ȃ�                // ����240�Ȃ�Ă����傫�Ȑ������Ƃ����ƁA
  SCENE_INITIALIZE_PROCESS_ALL,              // �S�Ă̏������������s��  // Scene::Initialize�ɓn���Ă��������s32�����邩��A
  SCENE_INITIALIZE_PROCESS_TRAVERSE,         // �g���o�[�X�̂ݍs��      // ����Ɩ��m�ɋ�ʂł��Ă��邩���m�F���邽�߁B

  SCENE_INITIALIZE_PROCESS_ENUMS32MAX = ENUMS32MAX
};




//-------------------------------------
///	3D�J�����p�̒萔
//=====================================
// �J�����̃C���f�b�N�X  // �J�����C���f�b�N�X��scene���ƂɊ���U��Bscene_calculate_group���Ƃɂ͊���U��Ȃ��B
enum
{
  CAMERA_INDEX_NONE    = -1  // �C���f�b�N�X�Ȃ�
};




//-------------------------------------
///	3D�e�p�̒萔
//=====================================
// �e�O���[�v�̃C���f�b�N�X
enum
{
  SHADOW_GROUP_INDEX_NONE     = -1  // �C���f�b�N�X�Ȃ�
};




//-------------------------------------
///	�e�N�X�`���̃t�H�[�}�b�g
//=====================================
enum TextureFormat
{
  TEXTURE_FORMAT_RGBA8    = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA8   ,  //!< 1�s�N�Z����4�o�C�g�ARGBA���e8�r�b�g�̃t�H�[�}�b�g�ł��B
  TEXTURE_FORMAT_RGB8     = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGB8    ,  //!< 1�s�N�Z����3�o�C�g�ARGB���e8�r�b�g�̃t�H�[�}�b�g�ł��B
  TEXTURE_FORMAT_RGBA5551 = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA5551,  //!< 1�s�N�Z����2�o�C�g�ARGB���e5�r�b�g�AA��1�r�b�g�̃t�H�[�}�b�g�ł��B
  TEXTURE_FORMAT_RGB565   = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGB565  ,  //!< 1�s�N�Z����2�o�C�g�ARGB���e5,6,5�r�b�g�̃t�H�[�}�b�g�ł��B
  TEXTURE_FORMAT_RGBA4    = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA4   ,  //!< 1�s�N�Z����2�o�C�g�ARGBA���e4�r�b�g�̃t�H�[�}�b�g�ł��B
  TEXTURE_FORMAT_LA8      = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_LA8     ,  //!< 1�s�N�Z����2�o�C�g�AAlpha��Luminance���e8�r�b�g�̃t�H�[�}�b�g�ł� 
  TEXTURE_FORMAT_HILO8    = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_HILO8   ,  //!< 1�s�N�Z����2�o�C�g�AX,Y���e8�r�b�g�̃t�H�[�}�b�g�ł��B
  TEXTURE_FORMAT_L8       = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_L8      ,  //!< 1�s�N�Z����1�o�C�g�ALuminance��8�r�b�g�̃t�H�[�}�b�g�ł��B
  TEXTURE_FORMAT_A8       = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_A8      ,  //!< 1�s�N�Z����1�o�C�g�AAlpha��8�r�b�g�̃t�H�[�}�b�g�ł��B
  TEXTURE_FORMAT_LA4      = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_LA4     ,  //!< 1�s�N�Z����1�o�C�g�AAlpha��Luminance���e4�r�b�g�̃t�H�[�}�b�g�ł� 
  TEXTURE_FORMAT_L4       = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_L4      ,  //!< 1�s�N�Z����4�r�b�g�ALuminance��4�r�b�g�̃t�H�[�}�b�g�ł��B
  TEXTURE_FORMAT_A4       = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_A4      ,  //!< 1�s�N�Z����4�r�b�g�AAlpha��4�r�b�g�̃t�H�[�}�b�g�ł��B
  TEXTURE_FORMAT_ETC1     = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_ETC1    ,  //!< 1�s�N�Z����4�r�b�g�����̈��k�t�H�[�}�b�g�ł��B
  TEXTURE_FORMAT_ETC1A4   = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_ETC1A4  ,  //!< 1�s�N�Z����8�r�b�g�����̈��k�t�H�[�}�b�g�ł��B                    
};




//-------------------------------------
///	3D�A�j���[�V�����p�̒萔
//=====================================
// �A�j���[�V������max_anim_members�ɉ����g����
enum
{
  ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST          = -1,  // max_anim_members�ɍŏ��ɓn�����A�j���[�V�������\�[�X��GetMemberAnimSetCount���g�p����
  ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERS    = -2   // max_anim_members��max_members���g�p����
};


// �A�j���[�V�����^�C�v
enum AnimationType
{
  ANIMATION_TYPE_NONE,  // �����ݒ肳��Ă��Ȃ��Ƃ��̃^�C�v
  ANIMATION_TYPE_SKELETAL,
  ANIMATION_TYPE_MATERIAL,
  ANIMATION_TYPE_VISIBILITY,
  ANIMATION_TYPE_LIGHT,
  ANIMATION_TYPE_CAMERA,
  ANIMATION_TYPE_FOG,
  ANIMATION_TYPE_ALL//,  // gfl_G3dResource.h��gfl::grp::g3d::Resource::AnimationResourceType�ɂ���̂�
  //ANIMATION_TYPE_ANY,  // �ǂ̃A�j���[�V�����^�C�v�ɂ��Ȃ蓾��Ƃ�  // ���̂Ƃ���g���Ă��Ȃ��A����g��Ȃ������AALL�Ɠ��e���Ԃ邩���B
};


// �A�j���[�V�����X���b�g
enum
{
  SLOT_INDEX_NONE  = -1
};


// �A�j���[�V�����ԍ�
enum
{
  ANIM_NO_NONE  = -1  // �A�j���[�V�������͂����Ƃ���anim_no
};




//-------------------------------------
///	3D���[�U�f�[�^
//=====================================

// ���e(�v���W�F�N�V����)�Ɋւ��郆�[�U�f�[�^  // �f�[�^�̌^�͐��� DATA_TYPE_INT
#define  GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME  "ProjectionType"    // ���[�U�f�[�^�̖��O
#define  GFL_GRP_G3D_H3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME  "$ProjectionType"    // ���[�U�f�[�^�̖��O  // h3d�ł̖��O
enum
{
  MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX      = 0,         // �^�C�v���ʂɗp���邱�Ƃ��ł���f�[�^�̃C���f�b�N�X�ԍ�
  MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE               =  0,  // �^�C�v���Ȃ��̂Ƃ��̒l
  MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW        =  1,  // �^�C�v���e���e�̂Ƃ��̒l
  MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_LIGHT         =  2,   // �^�C�v�������e�̂Ƃ��̒l
  MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT  = 11   // �^�C�v���e���e�����v���O�����Ō����e�ɂ��Ă���Ƃ��̒l
  //MATERIAL_USER_DATA_PROJECTION_TYPE_ARG0_INDEX = ,  // �������炱��Ȋ����ɒǉ�
  //MATERIAL_USER_DATA_PROJECTION_TYPE_ARG0_VALUE_AAA = ,
  //MATERIAL_USER_DATA_PROJECTION_TYPE_ARG0_VALUE_BBB = ,
};

// ???�Ɋւ��郆�[�U�f�[�^  // �f�[�^�̌^�͕����� DATA_TYPE_STRING  // �������炱��Ȋ����ɒǉ�
//#define  GFL_GRP_G3D_MATERIAL_USER_DATA_???_NAME  "???"    // ���[�U�f�[�^�̖��O
//enum
//{
//  MATERIAL_USER_DATA_???_ARG0_INDEX = ,
//  MATERIAL_USER_DATA_???_ARG0_VALUE_AAA = ,
//};




//-------------------------------------
///	�֗��֐�
//=====================================
// �r�b�g�t���O�ɂ����Ďw�肵���r�b�g��1��0���𒲂ׂ�֐�
// bit_flag�ɂ�����target��1�ɂȂ��Ă���r�b�g���S��1�̂Ƃ�true��Ԃ��A�ǂꂩ1�ł�0�̂Ƃ�false��Ԃ�
// target�̃r�b�g�S�Ă�0�ɂ��ēn���ƕK��true���Ԃ��Ă���
// �g�p��
// bit32 bit_flag = 0110;
// bit32 target_a = 0010;
// bit32 target_b = 0100;
// b32 ret = IsBitFlagOn( bit_flag, target_a|target_b );
static inline b32 IsBitFlagOn(const bit32 bit_flag, const bit32 target) { return ( (bit_flag&target) == target ); }

// �r�b�g�t���O�ɂ����Ďw�肵���r�b�g��1��0���𒲂ׂ�֐�
// target_one��1�r�b�g����1�ɂ��Ă͂Ȃ�Ȃ�
// IsBitFlagOn����r�񐔂�1�񏭂Ȃ��ς݂܂�
// �y�߂�l���Ӂz�߂�l��bit32�^��true, false�ł͂���܂���B�u�^�̂Ƃ��́y0�ȊO(1�Ƃ͌���Ȃ��������Ⴄ�l�ɂȂ�)�z�v���u�U�̂Ƃ��́y0�z�v�ł��B
// 0�Ɣ�r����Ƃ��Aif���̏������ɒP�Ƃœ���Ă����Ƃ��A&&��||�Ōq���A�Ƃ��̏ꍇ�͖��͂Ȃ��g����B
static inline u32 IsBitFlagOnOne(const bit32 bit_flag, const bit32 target_one) { return ( bit_flag&target_one ); }

// �r�b�g�t���O�ɂ����Ďw�肵���r�b�g��1�ɂ���֐�
// bit_flag�ɑ΂���target��1�ɂȂ��Ă���r�b�g�S�Ă�1�ɂ���Atarget��0�ɂȂ��Ă���r�b�g�͕ύX���Ȃ�
static inline void SetBitFlagOn(bit32* bit_flag, const bit32 target) { (*bit_flag) |= target; }

// �r�b�g�t���O�ɂ����Ďw�肵���r�b�g��0�ɂ���֐�
// bit_flag�ɑ΂���target��1�ɂȂ��Ă���r�b�g�S�Ă�0�ɂ���Atarget��0�ɂȂ��Ă���r�b�g�͕ύX���Ȃ�
static inline void SetBitFlagOff(bit32* bit_flag, const bit32 target) { (*bit_flag) &= (~target); }

// �r�b�g�t���O�ɂ����Ďw�肵���r�b�g��1��0�ɂ���
// bit_flag�ɑ΂���target��1�ɂȂ��Ă���r�b�g�S�Ă�1��0�ɂ���Atarget��0�ɂȂ��Ă���r�b�g�͕ύX���Ȃ�
static inline void SetBitFlag(bit32* bit_flag, const bit32 target, const b32 is_on)
{ if(is_on) SetBitFlagOn(bit_flag, target); else SetBitFlagOff(bit_flag, target); }




//-------------------------------------
///	�u���ꏊ�ɍ������֐�
//=====================================

// ���̂�gfl_G3dModel.cpp�ɂ���܂��B
namespace internal {

extern void MultScale(nw::math::MTX34* dstMatrix, const nw::math::MTX34* srcMatrix, const nw::math::VEC3& scale);
extern void CopyRotate(nw::math::MTX34* dstMatrix, const nw::math::MTX34& srcMatrix);
extern void CopyTranslate(nw::math::MTX34* dstMatrix, const nw::math::MTX34& srcMatrix);

extern void
CalculateWorldMatrixOfSkeleton(
    nw::math::MTX34* transformMatrix,
    nw::math::VEC3* scale,
    const nw::gfx::CalculatedTransform& localTransform,
    const nw::gfx::CalculatedTransform& parentWorldTransform,
    const nw::gfx::CalculatedTransform& parentLocalTransform,
    const bool inheritTransform[ 3 ]
);

extern void
CalculateWorldMatrixOfTransformNode(
    nw::math::MTX34* transformMatrix,
    nw::math::VEC3* scale,
    const nw::gfx::CalculatedTransform& localTransform,
    const nw::gfx::CalculatedTransform& parentWorldTransform,
    const bool inheritTransform[ 3 ]
); 

}  // namespace internal




}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DDEFINE_H__
