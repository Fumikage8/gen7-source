//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dDynamicTexture.cpp
 *	@brief  H3D�e�N�X�`���R���e���g
 *	@author	Koji Kawada
 *	@date		2012.08.08
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
#include <grp/g3d/gfl_G3dH3dDynamicTexture.h>



namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					�N���X����
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3D�e�N�X�`�����I�����N���X
//=====================================
H3dDynamicTexture::H3dDynamicTexture(void)
{
  // ��`�l���ς���Ă��܂��Ă��Ȃ����m�F����
  STATIC_ASSERT( TEXTURE_FORMAT_RGBA8    == PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_BYTE                    );
  STATIC_ASSERT( TEXTURE_FORMAT_RGB8     == PICA_DATA_TEXTURE_FORMAT_RGB_UNSIGNED_BYTE                     );
  STATIC_ASSERT( TEXTURE_FORMAT_RGBA5551 == PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_SHORT_5_5_5_1           );
  STATIC_ASSERT( TEXTURE_FORMAT_RGB565   == PICA_DATA_TEXTURE_FORMAT_RGB_UNSIGNED_SHORT_5_6_5              );
  STATIC_ASSERT( TEXTURE_FORMAT_RGBA4    == PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_SHORT_4_4_4_4           );
  STATIC_ASSERT( TEXTURE_FORMAT_LA8      == PICA_DATA_TEXTURE_FORMAT_LUMINANCE_ALPHA_UNSIGNED_BYTE         );
  STATIC_ASSERT( TEXTURE_FORMAT_HILO8    == PICA_DATA_TEXTURE_FORMAT_HILO8_DMP_UNSIGNED_BYTE               );
  STATIC_ASSERT( TEXTURE_FORMAT_L8       == PICA_DATA_TEXTURE_FORMAT_LUMINANCE_UNSIGNED_BYTE               );
  STATIC_ASSERT( TEXTURE_FORMAT_A8       == PICA_DATA_TEXTURE_FORMAT_ALPHA_UNSIGNED_BYTE                   );
  STATIC_ASSERT( TEXTURE_FORMAT_LA4      == PICA_DATA_TEXTURE_FORMAT_LUMINANCE_ALPHA_UNSIGNED_BYTE_4_4_DMP );
  STATIC_ASSERT( TEXTURE_FORMAT_L4       == PICA_DATA_TEXTURE_FORMAT_LUMINANCE_UNSIGNED_4BITS_DMP          );
  STATIC_ASSERT( TEXTURE_FORMAT_A4       == PICA_DATA_TEXTURE_FORMAT_ALPHA_UNSIGNED_4BITS_DMP              );
  STATIC_ASSERT( TEXTURE_FORMAT_ETC1     == PICA_DATA_TEXTURE_FORMAT_ETC1_RGB8_NATIVE_DMP                  );
  STATIC_ASSERT( TEXTURE_FORMAT_ETC1A4   == PICA_DATA_TEXTURE_FORMAT_ETC1_ALPHA_RGB8_A4_NATIVE_DMP         );

  // �ȉ���2�͂Ȃ�
  // PICA_DATA_TEXTURE_FORMAT_SHADOW_UNSIGNED_INT                   = 0x0,
  // PICA_DATA_TEXTURE_FORMAT_GAS_DMP_UNSIGNED_SHORT                = 0x0,

  constructorH3dDynamicTexture();
}

H3dDynamicTexture::~H3dDynamicTexture()
{
  Destroy();
}

void H3dDynamicTexture::Create(gfl::heap::NwAllocator* /*heap_allocator*/, gfl::heap::NwAllocator* device_allocator)
{
  // m_cmd_set�̃������m�� & ������
  for(s32 i=0; i<TEX_NO_MAX; ++i)
  {
    m_cmd_set[i].cmd = reinterpret_cast<bit32*>(GflHeapAllocMemoryAlign(
        device_allocator->GetHeapBase(),
        sizeof(bit32)*CmdSet::CMD32_NUM_MAX,
        CmdSet::CMD_ALIGN
    ));
    clearCommand(&(m_cmd_set[i]));
  }

  createTextureContent(device_allocator);
}

void H3dDynamicTexture::Destroy(void)
{
  destroyTextureContent();

  // m_cmd_set�̃������j��
  for(s32 i=0; i<TEX_NO_MAX; ++i)
  {
    if( m_cmd_set[i].cmd )
    {
      GflHeapFreeMemory(m_cmd_set[i].cmd);
    }
  }

  // �R���X�g���N�^�Ɠ���������
  constructorH3dDynamicTexture();
}

b32 H3dDynamicTexture::SetTextureInformation(
    const TextureInformation& info
)
{
  b32 ret = true;
 
  do
  {
    // info�Ɉُ킪�Ȃ����`�F�b�N����
    {
      ret = checkTextureInformation(info);
      if(ret)
      {
        m_info = info;
      }
      else
      {
        break;
      }
    }
 
    // �R�}���h�𐶐�����
    {
      createCommand();
    }
  }
  while(0);

  setTextureContent();

  return ret;
}


void* H3dDynamicTexture::GetPhysicalAddress(void) const
{
  return m_info.physical_address;
}
u16   H3dDynamicTexture::GetWidth(void) const
{
  return m_info.width;
}
u16   H3dDynamicTexture::GetHeight(void) const
{
  return m_info.height;
}
gfl::grp::g3d::TextureFormat H3dDynamicTexture::GetFormat(void) const
{
  return static_cast<gfl::grp::g3d::TextureFormat>(m_info.format);
}
u8    H3dDynamicTexture::GetMipmap(void) const
{
  return m_info.mipmap;
}


void H3dDynamicTexture::initializeTextureInformationForZero(TextureInformation* info)
{
  info->physical_address = NULL;
  // physical_address�� NULL or NULL�łȂ� ���� ���ݒ� or �ݒ�ς� �𔻒肷��̂ŁA���ꂾ������������΂悢�B
}

b32 H3dDynamicTexture::checkTextureInformation(const TextureInformation& info)
{
  b32 ret = true;
  // @note �`�F�b�N�������Ɓ�����Ȃ����͕t�������āI
  if( info.mipmap <= 0 )
  {
    GFL_ASSERT_MSG(0, "mipmap=%d\n", info.mipmap);
    ret = false;
  }
  return ret;
}

void H3dDynamicTexture::initializeCmdSetForZero(CmdSet* cmd_set)
{
  cmd_set->cmd = NULL;
  clearCommand(cmd_set);
}

void H3dDynamicTexture::clearCommand(CmdSet* cmd_set)
{
  if(cmd_set->cmd)
  {
    gfl::std::MemClear(cmd_set->cmd, sizeof(bit32)*CmdSet::CMD32_NUM_MAX);
  }
  cmd_set->cmd_num = 0;
  cmd_set->fnd_cmd = nw::h3d::fnd::Command();
}

void H3dDynamicTexture::constructorH3dDynamicTexture(void)
{
  initializeTextureInformationForZero(&m_info);
  for(s32 i=0; i<TEX_NO_MAX; ++i)
  {
    initializeCmdSetForZero(&(m_cmd_set[i]));
  }
  m_cont = NULL;
}

void H3dDynamicTexture::createCommand(void)
{
/*
  CTR 
  DMPGL2.0�V�X�e��API�d�l��
  2012-03-14
  Ver 3.6
  5.8.16 �e�N�X�`���A�h���X�ݒ背�W�X�^
  �e�N�X�`���������̃A�h���X�͑S�āA�����A�h���X��8�o�C�g�A�h���X�Őݒ肵�܂��B�i�����A�h���X��8�Ŋ������l�ł��B�j
*/

  // ���݂�m_info�̓��e�ŃR�}���h�𐶐�����
  enum
  {
    INDEX_SIZE,
    INDEX_ADDR,
    INDEX_FORMAT,
    INDEX_MAX
  };
  static const u32 REG_TBL[TEX_NO_MAX][INDEX_MAX] =
  {
    { PICA_REG_TEXTURE0_SIZE, PICA_REG_TEXTURE0_ADDR1, PICA_REG_TEXTURE0_FORMAT },  // { 0x82, 0x85, 0x8E }
    { PICA_REG_TEXTURE1_SIZE, PICA_REG_TEXTURE1_ADDR , PICA_REG_TEXTURE1_FORMAT },  // { 0x92, 0x95, 0x96 }
    { PICA_REG_TEXTURE2_SIZE, PICA_REG_TEXTURE2_ADDR , PICA_REG_TEXTURE2_FORMAT },  // { 0x9A, 0x9D, 0x9E }
  };

  for(s32 i=0; i<TEX_NO_MAX; ++i)
  {
    bit32* begin_addr = m_cmd_set[i].cmd;
    bit32* end_addr   = m_cmd_set[i].cmd;
    bit32* cmd = begin_addr;

    // width, height
    *cmd++ = PICA_CMD_DATA_TEXTURE_SIZE(m_info.width, m_info.height);
    *cmd++ = PICA_CMD_HEADER_SINGLE(REG_TBL[i][INDEX_SIZE]);

    // address
    *cmd++ = PICA_CMD_DATA_TEXTURE_ADDR(m_info.physical_address);  // PICA_CMD_DATA_TEXTURE_ADDR����8�Ŋ����Ă���Ă���̂ŁA�n���Ƃ��͂��̂܂܂ł����B
    *cmd++ = PICA_CMD_HEADER_SINGLE(REG_TBL[i][INDEX_ADDR]);

    // format
    *cmd++ = PICA_CMD_DATA_TEXTURE_FORMAT_TYPE(m_info.format);
    *cmd++ = PICA_CMD_HEADER_SINGLE(REG_TBL[i][INDEX_FORMAT]);

    // �`�����l��1���L�b�N
    *cmd++ = 0x1;  // @note �������悤���܂ˁA���v���ȁB
    *cmd++ = PICA_CMD_HEADER_SINGLE(PICA_REG_COMMAND_BUF_KICK_CH1);  // 0x23D

    // ��
    end_addr = cmd;
    m_cmd_set[i].cmd_num = end_addr - begin_addr;  // (bit32*)�^�Ȃ̂�/sizeof(bit32)���Ȃ��Ă����ɂȂ�B
    
    // 32�r�b�g*2��4�Ȃ̂ŁA16�o�C�g�A���C���ɂȂ��Ă���̂ŁA�����s�v�B
    
    // nw::h3d::fnd�̃R�}���h�ɂ���
    m_cmd_set[i].fnd_cmd = nw::h3d::fnd::Command(m_cmd_set[i].cmd, m_cmd_set[i].cmd_num);
  }
}

void H3dDynamicTexture::createTextureContent(gfl::heap::NwAllocator* device_allocator)
{
  m_cont = GFL_NEW(device_allocator->GetHeapBase()) nw::h3d::res::TextureContent;
}
void H3dDynamicTexture::destroyTextureContent(void)
{
  GFL_DELETE m_cont;
  m_cont = NULL;
}
void H3dDynamicTexture::setTextureContent(void)
{
  m_cont->unit0Cmd = m_cmd_set[0].fnd_cmd;
  m_cont->unit1Cmd = m_cmd_set[1].fnd_cmd;
  m_cont->unit2Cmd = m_cmd_set[2].fnd_cmd;
  m_cont->format     = m_info.format;
  m_cont->mipmapSize = m_info.mipmap;
  m_cont->name       = "G3dH3dDynamicTexture";
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

