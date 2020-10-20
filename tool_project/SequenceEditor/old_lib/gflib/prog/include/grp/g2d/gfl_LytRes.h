#ifndef __GFL_LYTRES_H__
#define __GFL_LYTRES_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_LytRes.h
 *	@brief  ���C�A�E�g���\�[�X�N���X
 *	@author	tomoya takahashi
 *	@date		2010.10.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn.h>
#include <nw.h>
#include <nw/lyt.h>

#include <gfl_Fs.h>
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_Str.h>

#include <grp/gfl_GraphicsType.h>

namespace gfl {
namespace grp {
namespace g2d {

//-------------------------------------
///	�O���錾
//=====================================
class LytResManager;
class LytSys;
class LytSysBase;
class LytWkBase;
class LytWk;


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
enum
{
  LYTRES_PANENAME_ERROR = 0xffff,  // �y�C�����Ȃ�

  LYTRES_DEFAULT_FONT_MAX = 8,  // �f�t�H���g�t�H���g�o�^�ő吔
};
  
//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          gfl::grp::g2d::LytBinary�\��
 */
//-----------------------------------------------------------------------------
typedef struct _LytBinaryHeader LytBinaryHeader;

//-----------------------------------------------------------------------------
/**
 *          gfl::grp::g2d::LytPaneBinary�\��
 */
//-----------------------------------------------------------------------------
typedef struct _LytPaneBinaryHeader LytPaneBinaryHeader;


//-----------------------------------------------------------------------------
/**
 *    ���\�[�XAccessor
 */
//-----------------------------------------------------------------------------
typedef nw::lyt::ArcResourceAccessor LytResourceAccessor;

//-----------------------------------------------------------------------------
/**
 *    �e�N�X�`�����
 */
//-----------------------------------------------------------------------------
typedef nw::lyt::TextureInfo LytResTextureInfo;
typedef nw::lyt::TexResource LytResTexture;
  
//-----------------------------------------------------------------------------
/**
 *					���C�A�E�g���\�[�X�N���X�錾
*/
//-----------------------------------------------------------------------------
class LytRes
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(LytRes);

  friend class LytResManager;
  friend class LytSys;
  friend class LytSysBase;
  friend class LytWkBase;
  friend class LytWk;
  
public:

  //----------------------------------------------------------------------------
  /**
   *	@brief  ���C�A�E�g���\�[�X���ǂݍ��ݍς݂��`�F�b�N
   *
   *	@retval true    �ǂݍ��ݍς�
   *	@retval false   �ǂݍ��܂�Ă��Ȃ�
   */
  //-----------------------------------------------------------------------------
  bool IsDataIn(void) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  �t�@�C�����擾
   *
   *	@return �t�@�C����
   */
  //-----------------------------------------------------------------------------
  u32 GetFileNum( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  �t�@�C�����擾
   *
   *	@param  lytDataID �f�[�^ID
   *
   *	@return �t�@�C����
   */
  //-----------------------------------------------------------------------------
  const char* GetFileName( u32 lytDataID ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief  �ړI�̃��\�[�X���擾
   *
   *  @param  lytDataID   ���C�A�E�g�f�[�^ID
   * 
   *  @retval ���\�[�X�|�C���^  �Ȃ��ꍇ���i�łł�NULL���߂�B
   */
  //-----------------------------------------------------------------------------
  void* GetResource( u32 lytDataID ) const;

  //-----------------------------------------------------------------------------
  /**
   *   @brief ���\�[�X�u���b�N�̎擾
   *
   *   @param lytDataID   ���C�A�E�g�f�[�^ID
   *
   *   @retval  �A�j���[�V�������\�[�X
   *   @retval  NULL  �f�[�^�����������ꍇ�i���i�ł̂݁j
   */
  //-----------------------------------------------------------------------------
  void* GetAnimeResource( u32 lytDataID ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief  �e�N�X�`�������ׂă������ɔz�u����
   *
   *  @param  area    �]���惁�����@�iFCRAM/VRAMA/VRAMB�j
   */
  //-----------------------------------------------------------------------------
  void TransferAllTexture( gfl::grp::MemoryArea area );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  �e�N�X�`�����������ɔz�u����
   *
   *  @param  texArcIndex   �e�N�X�`�����C�A�E�g�f�[�^ID
   *  @param  area          �]���惁�����@�iFCRAM/VRAMA/VRAMB�j
   */
  //-----------------------------------------------------------------------------
  void TransferIndexTexture( u32 texArcIndex, gfl::grp::MemoryArea area );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  �e�N�X�`�����\�[�X���擾
   *
   *  @param  lytDataID   ���C�A�E�g�f�[�^ID
   *
   *  @retval �e�N�X�`���C���t�H�f�[�^
   */
  //-----------------------------------------------------------------------------
  const LytResTextureInfo GetTexture( u32 lytDataID ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief  �e�N�X�`�����\�[�X���擾
   *
   *  @param  size ���\�[�X�̃T�C�Y���󂯎��ϐ��ւ̃|�C���^�ł��B
   *               �T�C�Y���󂯎��K�v���Ȃ��ꍇ�� NULL ��n���Ă��������B
   *
   *  @retval �e�N�X�`�����\�[�X�̐擪�A�h���X�ւ̃|�C���^
   *
   *  @note   �g�p��
   *          u32 size;
   *          void* buffer = LytRes::GetTextureRes( lytDataID, &size );
   *          nw::lyt::TexResource tex_resource(buffer, *size);
   */
  //-----------------------------------------------------------------------------
  void* GetTextureRes( u32 lytDataID, u32* size ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  ���\�[�X�A�N�Z�T�[�̎擾
   */
  //-----------------------------------------------------------------------------
  LytResourceAccessor* GetResourceAccessor(void) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief  �y�C�����̎擾
   */
  //-----------------------------------------------------------------------------
  const char * GetPaneName( u32 paneEnum )const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  �y�C�����C���f�b�N�X�����擾
   *
   *	@return �y�C�����C���f�b�N�X��
   */
  //-----------------------------------------------------------------------------
  u32 GetPaneNum( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  ���O�̃C���f�b�N�X���擾
   *
   *	@return �C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  u32 GetPaneNameIndex( const char* name ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  Default�t�H���g��ݒ�
   *
   *	@param	font    �t�H���g
   *	@param  name    ����
   *	@param  index   �C���f�b�N�X(0�`LYTRES_DEFAULT_FONT_MAX)
   */
  //-----------------------------------------------------------------------------
  void SetDefaultFont( gfl::str::Font* font, const char* name, u32 index = 0 );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �e�N�X�`����ǉ��o�^
   *
   *	@param	res     ���\�[�X
   */
  //-----------------------------------------------------------------------------
  void RegistTexture( const LytRes* res, gfl::grp::MemoryArea area );


private:
  // �R���X�g���N�^�E�f�X�g���N�^
  LytRes(void);
  ~LytRes();


  //----------------------------------------------------------------------------
  /**
   *	@brief  ���C�A�E�g���\�[�X�̓ǂݍ���
   *
   *  @param  cp_buff          �o�b�t�@�|�C���^
   *	@param  addTextureNum   �ǉ��o�^�@�e�N�X�`����
   */
  //-----------------------------------------------------------------------------
  void Load( heap::NwAllocator* allocator, const void * cp_buff, u32 addTextureNum=0 );

	//----------------------------------------------------------------------------
	/**
	 *	@brief  ���C�A�E�g���\�[�X�̓ǂݍ��݁i���k�f�[�^�Ή��Łj
	 *
	 *	@param  allocator �A���P�[�^
	 *	@param  arcFile   �A�[�J�C�u�t�@�C��
	 *	@param  dataID    �f�[�^ID
	 *	@param	comp			���k�t���O�@true = ���k�f�[�^
	 *	@param  addTextureNum   �ǉ��o�^�@�e�N�X�`����
	 */
	//-----------------------------------------------------------------------------
	void Load( heap::NwAllocator* allocator, fs::ArcFile* arcFile, u32 dataID, bool comp, u32 addTextureNum=0 );

  //----------------------------------------------------------------------------
  /**
   *	@brief  ���C�A�E�g���\�[�X�̓ǂݍ��݁i�񈳏k�f�[�^�j
   *
   *	@param  allocator       �A���P�[�^
   *	@param  arcFile         �A�[�J�C�u�t�@�C��
   *	@param  dataID          �f�[�^ID
   *	@param  addTextureNum   �ǉ��o�^�@�e�N�X�`����
   */
  //-----------------------------------------------------------------------------
  void Load( heap::NwAllocator* allocator, fs::ArcFile* arcFile, u32 dataID, u32 addTextureNum=0 );

	//----------------------------------------------------------------------------
	/**
	 *	@brief  ���C�A�E�g���\�[�X�̓ǂݍ��݁i���k�f�[�^�j
	 *
	 *	@param  allocator �A���P�[�^
	 *	@param  arcFile   �A�[�J�C�u�t�@�C��
	 *	@param  dataID    �f�[�^ID
	 *	@param  addTextureNum   �ǉ��o�^�@�e�N�X�`����
	 */
	//-----------------------------------------------------------------------------
	void LoadComp( heap::NwAllocator* allocator, fs::ArcFile* arcFile, u32 dataID, u32 addTextureNum=0 );

  //----------------------------------------------------------------------------
  /**
   *	@brief  ���C�A�E�g���\�[�X�̔j��
   */
  //-----------------------------------------------------------------------------
  void Release( void );




private:
  void*                           m_memory;
  const LytBinaryHeader*          m_buff;
  LytPaneBinaryHeader*            m_paneData;
  const char*                     m_nameTbl;
  void*                           m_arcData;
  nw::lyt::ArcResourceAccessor*   m_resAccessor;
  
  // Default�t�H���g���
  bool                            m_useDefaultFont[LYTRES_DEFAULT_FONT_MAX];
  nw::lyt::FontKey                m_fontKey[LYTRES_DEFAULT_FONT_MAX];

  // �ǉ��o�^�e�N�X�`���L�[
  u16                             m_textureKeyNum;
  u16                             m_addTextureNum;
  nw::lyt::TextureKey*            m_textureKeyArray;
};



} // g2d
} // grp
} // gfl

#endif // __GFL_LYTRES_H__
