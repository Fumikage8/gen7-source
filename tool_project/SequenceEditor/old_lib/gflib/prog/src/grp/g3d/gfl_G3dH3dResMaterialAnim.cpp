//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dResMaterialAnim.cpp
 *	@brief  H3D�A�j���[�V�����R���e���g
 *	@author	Koji Kawada
 *	@date		2012.04.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_fs.h>

// gflib grp
#include <grp/gfl_GraphicsType.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>



#if GFL_GRP_G3D_H3D_USE

// NW4C
#include <nw/h3d.h>
// gflib grp g3dh3d
#include <grp/g3d/gfl_G3dH3dResMaterialAnim.h>



namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					�N���X����
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3D�}�e���A���A�j���[�V�����R���e���g�N���X
//=====================================
H3dResMaterialAnim::H3dResMaterialAnim(void)
  : H3dResAnim(),
    m_texpat_buff(NULL)
{}

H3dResMaterialAnim::~H3dResMaterialAnim()
{
  Destroy();
}

b32 H3dResMaterialAnim::SetupTexturePatternAnimTexture(H3dResource* other_resource)
{
  b32 ret = false;

  if( m_texpat_buff )
  {
    // �e�N�X�`���p�^�[���A�j���[�V����������ꍇ

    // ���ɐݒ肳��Ă���o�C���h���͍폜����܂���B ������ Bind() �����s�����ꍇ�A�܂��o�C���h����Ă��Ȃ��e�N�X�`���̂݃o�C���h�����݂܂��B
    const nw::h3d::res::MaterialAnimContent* mat_anim_cont = GetNwMaterialAnimContent();
    nw::h3d::Result result = m_texpat_bind_state.Bind(mat_anim_cont, other_resource->GetNwResourceBinary());
 
    // �o�C���h����
    ////////{
    ////////  // ����͍�������œn����other_resource�Ɋւ��Ă̌��ʂł���A�S�e�N�X�`�����o�C���h����Ă��邩�̌��ʂł͂Ȃ��̂ŁA�����ł͎g���Ȃ��B
    ////////  s32 res_desc = result.GetDescription();
    ////////  if( res_desc == nw::h3d::Result::RESULT_OK ) ret = true;
    ////////}
    {
      ret = true;
      for(u32 i=0; i<mat_anim_cont->GetTextureListCount(); ++i)
      {
        if( !(m_texpat_bind_state.GetTexture(i)) )
        {
          ret = false;
          break;
        }
      }
    }
  }
  else
  {
    // �e�N�X�`���p�^�[���A�j���[�V�������Ȃ��ꍇ
    // �Z�b�g���Ȃ��Ă������i�Ȃ̂ŁAtrue��Ԃ�
    ret = true;
  }
  
  return ret;
}

b32 H3dResMaterialAnim::createInternal(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const H3dResource* resource, const char* name)
{
  s32 index = resource->GetMaterialAnimContentIndex(name);
  return createInternal(heap_allocator, device_allocator, resource, index);
}

b32 H3dResMaterialAnim::createInternal(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* /*device_allocator*/, const H3dResource* resource, s32 index_in_resource)
{
  // resource���o���Ă���
  m_cont_index = index_in_resource;
  m_cont = resource->GetNwMaterialAnimContent(index_in_resource);

  {
    u32 num = getTexturePatternAnimTextureNum();  // ���̊֐���m_cont�̐ݒ肪�I����Ă��Ȃ��Ǝg���Ȃ��̂ŁA�ݒ菇�ɒ��ӂ��邱�ƁB
    if( num > 0 )
    {
      m_texpat_buff = GflHeapAllocMemoryAlign(
          heap_allocator->GetHeapBase(),
          nw::h3d::TexturePatternAnimBindState::CalcSize(num), 4);
      m_texpat_bind_state.Init(m_texpat_buff, num);
    }
  }

  return true;
}

void H3dResMaterialAnim::destroyInternal(void)
{
  if(m_texpat_buff) GflHeapFreeMemory(m_texpat_buff);

  // �R���X�g���N�^�Ɠ���������
  m_cont_index = H3dResource::INDEX_NONE;
  m_cont = NULL;
  m_texpat_buff = NULL;
}

u32 H3dResMaterialAnim::getTexturePatternAnimTextureNum(void) const
{
  u32 ret = 0;
  const nw::h3d::res::MaterialAnimContent* mat_cont = GetNwMaterialAnimContent();
  if( mat_cont->HasTexturePatternAnim() )
  {
    ret = mat_cont->GetTextureListCount();
    GFL_ASSERT_MSG(ret>0, "HasTexturePatternAnim��true�Ȃ̂�GetTextureListCount��0�ł��B\n");  //@check
  }
  else
  {
    GFL_ASSERT_MSG(mat_cont->GetTextureListCount()==0, "HasTexturePatternAnim��false�Ȃ̂�GetTextureListCount��0����B\n");  //@check
  }
  return ret;
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

