//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dCommand.cpp
 *	@brief  H3D�R�}���h
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
#include <grp/g3d/gfl_G3dH3dCommand.h>



namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X����
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3D�R�}���h�N���X
//=====================================

H3dCommand::H3dCommand(void)
  : m_dcs(NULL),
    m_ss(NULL),
    m_begin_addr(NULL),
    m_end_addr(NULL)
{
  gfl::std::MemClear(m_dcs_buff, DCS_BUFF_BYTE_SIZE);
  gfl::std::MemClear(m_ss_buff, SS_BUFF_BYTE_SIZE);
}
H3dCommand::~H3dCommand()
{
  if( (m_dcs) && (m_ss) )
  {
    GFL_ASSERT_MSG(0, "EndDraw���Ă΂�Ă��܂���B\n");
  }
}

void H3dCommand::BegineDraw(void)
{
  if( (!m_dcs) && (!m_ss) )
  {
    flushCommandBuffer();

    bit32* command;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, reinterpret_cast<GLint*>(&command));

    AddressType addr = command;
    m_dcs = GFL_NEW_PLACEMENT(m_dcs_buff) nn::gr::CTR::CommandBufferJumpHelper(addr);  // placement new
        // h3d_Draw.h��h3d_DrawImpl.h�̃\�[�X���������A�A�h���X0��n���ď��������Ă����Ȃ��������B
        // ���̏ꍇ�͎g���O��Reset�œK�؂ȃA�h���X��n���Ȃ���΂Ȃ�Ȃ����B

    m_ss = GFL_NEW_PLACEMENT(m_ss_buff) nw::h3d::ShaderState;

    m_begin_addr = addr;
    m_end_addr   = m_begin_addr;  // �S�������`�悵�Ȃ����Ƃ����蓾��̂ŁAm_begin_addr�ŏ��������Ă����B
  }
  else
  {
    GFL_ASSERT_MSG(0, "EndDraw���Ă΂�Ă��܂���B\n");
  }
}

void H3dCommand::EndDraw(void)
{
  if( (m_dcs) && (m_ss) )
  {
    u32 size = m_dcs->GetSize();
    if( size )
    {
      nngxMoveCommandbufferPointer(size);

      u32 first_size = m_dcs->GetSizeToFirstJump();
      if( first_size )
      {
        nngxFlush3DCommandPartially(first_size); // ��؂�R�}���h��ǉ��i�ŏ��̃L�b�N�R�}���h�܂ł̃T�C�Y���w��j
      }

      bit32* command;
      nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, reinterpret_cast<GLint*>(&command));

      AddressType addr = command;
      m_dcs->FinalizeJump(addr);  // �Ō�̃R�}���h�̃T�C�Y���������ނ��߂� Reset ���s���܂��B

      m_end_addr = addr;
    }

    //updateCommandBuffer();

    m_dcs->~CommandBufferJumpHelper();  // �����I�Ƀf�X�g���N�^���Ăяo��  // placement delete�͌Ă΂Ȃ��Ă��������ȁB
    m_ss->~ShaderState();
    m_dcs = NULL;
    m_ss = NULL;
    m_begin_addr = NULL;
    m_end_addr = NULL;
  }
  else
  {
    GFL_ASSERT_MSG(0, "BeginDraw���Ă΂�Ă��܂���B\n");
  }
}

void H3dCommand::flushCommandBuffer(void)
{
  // ���̕`��V�X�e���̃R�}���h���쐬���Ă����ꍇ�A
  // nngxFlush3DCommandNoCacheFlush() �����s���ċ�؂������K�v������܂��B
  // �܂��́AnngxFlush3DCommandPartially() �̈����̃R�}���h�T�C�Y��
  // ���O�̃R�}���h�T�C�Y�����Z����悤�ɂ��Ă��������B
  nngxFlush3DCommandNoCacheFlush();
}

/*
void H3dCommand::updateCommandBuffer(void)
{
  size_t size = (m_end_addr - m_begin_addr) * sizeof(AddressType);  // �����Z��̒l��AddressType�^�̌��Ȃ̂ŁAsizeof�����̂��|���ăo�C�g�T�C�Y�ɂ��Ă��B
  if( size )
  {
    // �`��R�}���h���N�G�X�g�̐����� nngxFlush3DCommandPartially �֐����g���Ă���̂����A
    // nngxFlush3DCommandPartially �֐��̓R�}���h�o�b�t�@�ɑ΂��ăL���b�V���t���b�V�����s���Ă��Ȃ��̂ŁA
    // �����ōs���B

    // @note �R�}���h�o�b�t�@�S�̂ɑ΂��ăL���b�V���t���b�V���������ق����������悢�B
    //       ��������A�f�o�C�X�������S�̂ɑ΂��ăL���b�V���t���b�V���������ق����������悢�B
    //       ���A���������ɂ́A�O���t�B�b�N�X�V�X�e���Ƃ��q�[�v�V�X�e���Ƃ������������ނ��ƂɂȂ邾�낤����A���͂���Ă��Ȃ��B
    //       �ŁA�����ō��g�����R�}���h�o�b�t�@�ɂ����L���b�V���t���b�V�����s�����Ƃɂ��Ă���B
    //       ����ŁAh3d::MaterialData�̗̈�̃L���b�V���t���b�V�����ł��Ă���͂����B
    //       ��
    nngxUpdateBuffer(m_begin_addr, size);
  }
}
*/


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

