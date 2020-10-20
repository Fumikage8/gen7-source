#ifndef __GFL_MAPSYSTEM_H__
#define __GFL_MAPSYSTEM_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_MapSystem.h
 *	@brief  �}�b�v�Ǘ��V�X�e��
 *	@author	tomoya takahashi
 *	@date		2011.01.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once


#include <gfl_Common.h>
#include <gfl_Heap.h>
#include <gfl_Math.h>
#include <gfl_Fs.h>
#include <grp/g3d/gfl_G3dScene.h>
#include <grp/g3d/gfl_G3dResource.h>
#include <gfl_Base.h>

#include <grp/map/gfl_MapFileControl.h>
#include <grp/map/gfl_MapDivLoad.h>


namespace gfl{
namespace grp{
namespace map{


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *    ���N���X
 */
//-----------------------------------------------------------------------------
class System
{
  GFL_FORBID_COPY_AND_ASSIGN( System );

public:

  enum ModelMode{
    MODEL_DEFAULT,    ///< gfl::g3d::Model�N���X���g�p
    MODEL_H3D,        ///< gfl::g3d::H3dModel�N���X���g�p
  };
  
  //-------------------------------------
  ///	���f�[�^
  //=====================================
  struct SetUpParam{
    u32 mapDataHeapSize;                  // �}�b�v�f�[�^�o�b�t�@ByteSize
    u32 texVramSize;                      // �e�N�X�`��VRAM ByteSize�@�iVRAM�ɔz�u���Ȃ����O�j
    u32 plttVramSize;                      // �e�N�X�`��VRAM ByteSize�@�iVRAM�ɔz�u���Ȃ����O�j
    const FileControlArray* cpFileControls;  // �t�@�C���R���g���[���z��
    u32 divLoadSize;                      // �����ǂݍ��݁@1��̓ǂݍ���ByteSize
    DivLoad::Mode divMode;                // �����ǂݍ��݃��[�h
    gfl::base::Thread::Priority threadPri;// �X���b�h�D�揇��
    ModelMode     modelMode;
  };

  enum LoadStatus{
    LOAD_STATE_IDLING = 0,        // �ҋ@��
    LOAD_STATE_START,             // �ǂݍ��݊J�n
    LOAD_STATE_LOAD,              // �ǂݍ��ݒ�
    LOAD_STATE_SETUP,             // �Z�b�g�A�b�v��
    LOAD_STATE_END,               // �����i�f�[�^�������Ă����ԁj

    ENUM_FORCE_DWORD(LoadStatus)
  };


  //-------------------------------------
  ///	�o�^�\�@�O���[�o���e�N�X�`�����\�[�X��
  //=====================================
  static const u32 GLOBAL_RESOURCE_MAX = 8;


public:

  //----------------------------------------------------------------------------
  /**
   *	@brief  �R���X�g���N�^
   */
  //-----------------------------------------------------------------------------
  System( void );
    
  //----------------------------------------------------------------------------
  /**
   *	@brief  �R���X�g���N�^�@�i���������݁j
   *
   *	@param	param         ���f�[�^
   *	@param  p_scene       �V�[���V�X�e��
   *	@param  p_sysHeap     �V�X�e������Ɏg�p����q�[�v
   *	@param  p_deviceHeap  �f�o�C�X�q�[�v
   */
  //-----------------------------------------------------------------------------
  System( const SetUpParam& param, gfl::grp::g3d::Scene* p_scene, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  virtual ~System();
  

  //----------------------------------------------------------------------------
  /**
   *	@brief  ������
   *
   *	@param	param         ���f�[�^
   *	@param  p_scene       �V�[���V�X�e��
   *	@param  p_sysHeap     �V�X�e������Ɏg�p����q�[�v
   *	@param  p_deviceHeap  �f�o�C�X�q�[�v
   */
  //-----------------------------------------------------------------------------
  virtual void Initialize( const SetUpParam& param, gfl::grp::g3d::Scene* p_scene, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �j��
   */
  //-----------------------------------------------------------------------------
  virtual void Finalize( void );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  �X�V����
   */
  //-----------------------------------------------------------------------------
  virtual void Update( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �`��X�V����
   *
   *	Lod������ȂǁB
   */
  //-----------------------------------------------------------------------------
  virtual void UpdateDraw( gfl::grp::g3d::Camera* p_camera );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �ǂݍ��݃��N�G�X�g
   *
   *	@param	dataID        �f�[�^ID
   *	@param	divLoadFlag   �����ǂݍ��݂��邩�B�@�itrue=�����@false=�ꊇ�j
   *
   *	@param  true  ���N�G�X�g����
   *	@param  false ���N�G�X�g���s
   */
  //-----------------------------------------------------------------------------
  virtual bool ReqLoadData( u32 dataID, b32 divLoadFlag );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �ǂݍ��݃��N�G�X�g  ���k�f�[�^�̓ǂݍ��݃��N�G�X�g
   *
   *	@param	dataID        �f�[�^ID
   *	@param	divLoadFlag   �����ǂݍ��݂��邩�B�@�itrue=�����@false=�ꊇ�j
   *	@param  p_tmp_heap    �e���|�����m�ۗp�̃o�b�t�@
   *
   *	@param  true  ���N�G�X�g����
   *	@param  false ���N�G�X�g���s
   */
  //-----------------------------------------------------------------------------
  virtual bool ReqLoadCompData( u32 dataID, b32 divLoadFlag, gfl::heap::HeapBase* p_tmp_buff );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �ǂݍ��݃��Z�b�g
   */
  //-----------------------------------------------------------------------------
  virtual bool ResetLoadData( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �ǂݍ��ݏ�Ԃ̎擾
   *
   *	@retval  LOAD_STATE_IDLING = 0,        // �ҋ@��
   *	@retval  LOAD_STATE_START,             // �ǂݍ��݊J�n
   *	@retval  LOAD_STATE_LOAD,              // �ǂݍ��ݒ�
   *	@retval  LOAD_STATE_SETUP,             // �Z�b�g�A�b�v��
   *	@retval  LOAD_STATE_END,               // ����
   */
  //-----------------------------------------------------------------------------
  gfl::grp::map::System::LoadStatus GetLoadStatus( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  �ǂݍ���ł����f�[�^���擾
   *
   *	@return �ǂݍ���ł����f�[�^(gflib_cpp/tools/binlinker.rb���g�p���Đ�������B�j
   */
  //-----------------------------------------------------------------------------
  const gfl::base::BinLinkerAccessor& GetLoadMemoryPointer(void) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  �ǂݍ���ł����f�[�^ID���擾
   *
   *	@return �f�[�^ID
   */
  //-----------------------------------------------------------------------------
  u32 GetLoadDataID( void ) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief  �A�[�J�C�u�f�[�^�ݒ�
   *
   *	@param	  cp_path   �A�[�J�C�u�f�[�^�̃p�X
   *  @param    p_heap    �q�[�v
   */
  //-----------------------------------------------------------------------------
  void ResistArc( const wchar_t* cp_path, gfl::heap::HeapBase* p_heap );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �A�[�J�C�u�f�[�^�ݒ�  ID�o�[�W����
   *
   *	@param	  arc_id      �A�[�J�C�u�f�[�^��ID
   *  @param    p_heap      �q�[�v
   *  @param    file_on_pc  �p�\�R������̓ǂݍ��݂̏ꍇtrue, ROM�̏ꍇfalse
   */
  //-----------------------------------------------------------------------------
  void ResistArc( gfl::fs::ArcFile::ARCID arc_id, gfl::heap::HeapBase* p_heap, bool file_on_pc = false  );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �A�[�J�C�u�f�[�^�ݒ�
   *
   *	@param	 p_arc 
   */
  //-----------------------------------------------------------------------------
  void ResistArc( gfl::fs::ArcFile* p_arc );


  //----------------------------------------------------------------------------
  /**
   *	@brief  �A�[�J�C�u�ݒ芮���҂�
   *
   *  @retval true    ����
   *  @retval false   �r���i�A�[�J�C�u�̃t�@�[�X�g���[�h�Z�b�g�A�b�v���j
   */
  //-----------------------------------------------------------------------------
  bool IsRegistArcFinished( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  �A�[�J�C�u�f�[�^�j��
   */
  //-----------------------------------------------------------------------------
  void ReleaseArc( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �A�[�J�C�u�f�[�^�擾
   *
   */
  //-----------------------------------------------------------------------------
  gfl::fs::ArcFile* GetArc( void ) const;
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  �O���[�o���e�N�X�`�����\�[�X�@�ݒ�
   *  
   *	@param	  p_globalTex   �O���[�o���e�N�X�`��
   *	@param    index         �o�^�C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  void ResistGlobalTexResource( gfl::grp::g3d::Resource* p_globalTex, u32 index = 0 );
#if GFL_GRP_G3D_H3D_USE
  void ResistGlobalTexResource( gfl::grp::g3d::H3dResource* p_globalTex, u32 index = 0 );
#endif // GFL_GRP_G3D_H3D_USE

  //----------------------------------------------------------------------------
  /**
   *	@brief  �O���[�o���e�N�X�`�����\�[�X�@�N���A
   */
  //-----------------------------------------------------------------------------
  void ReleaseGlobalTexResource(void);
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  �n�`�\����ON�EOFF
   *
   *	@param	flag    true:ON false:OFF
   */
  //-----------------------------------------------------------------------------
  void SetVisible( b32 flag );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �n�`�\����ON�EOFF�@�`�F�b�N
   *
   *	@retval true      ON
   *	@retval false     OFF
   */
  //-----------------------------------------------------------------------------
  b32 IsVisible( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  �n�`�\���ʒu�̐ݒ�
   *
   *	@param	  pos   �ʒu
   */
  //-----------------------------------------------------------------------------
  void SetTrans( const gfl::math::VEC3& pos );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �n�`�\���ʒu�̎擾
   *
   *	@return �ʒu
   */
  //-----------------------------------------------------------------------------
  const gfl::math::VEC3& GetTrans( void ) const;

   
  //----------------------------------------------------------------------------
  /**
   *	@brief  �A�g���r���[�g�擾
   *  
   *	@param	  p_attrInfo    �A�g���r���[�g�C���t�H
   *	@param    cp_pos        ���W������킷���[�N
   *	@param    cp_way        �n�ʕ����iDefault�́A(0.0f,-1.0f,0.0f)
   *	@param    map_width     �}�b�v�u���b�N��
   */
  //-----------------------------------------------------------------------------
  virtual void GetAttr( gfl::grp::map::AttrInfo* p_attrInfo, const void* cp_pos, const gfl::math::VEC3* cp_way, f32 map_width );


  //----------------------------------------------------------------------------
  /**
   *	@brief  �n�`���f������
   *  
   *  @param  p_allocator          �A���P�[�^�i�f�o�C�X�������j
   *  @param  p_model_memory       ���f�����\�[�X�o�b�t�@
   *  @param  p_tex_memory         �e�N�X�`�����\�[�X�o�b�t�@
   *  @param  cp_res_desc           ���\�[�X�W�J�ݒ�
   *  @param  cp_model_desc       �@���f���W�J�ݒ�
   *  @param  common_resource_flag �g�p���鋤�ʃ��\�[�X������ꍇ�̓r�b�g�𗧂ĂĂ���
   */
  //-----------------------------------------------------------------------------
  virtual void CreateModel( gfl::heap::NwAllocator* p_allocator, void* p_model_memory, void* p_tex_memory, const gfl::grp::g3d::Resource::Description* cp_res_desc = NULL, const gfl::grp::g3d::Model::Description* cp_model_desc = NULL, u32 common_resource_flag = grp::g3d::Resource::COMMON_RESOURCE_NONE );

#if GFL_GRP_G3D_H3D_USE
  virtual void CreateH3dModel( gfl::heap::NwAllocator* p_allocator, void* p_model_memory, void* p_tex_memory, /* const gfl::grp::g3d::H3dResource::Description* cp_res_desc = NULL, const gfl::grp::g3d::H3dModel::Description* cp_model_desc = NULL, */ u32 common_resource_flag = grp::g3d::H3dResource::COMMON_RESOURCE_NONE );
#endif // GFL_GRP_G3D_H3D_USE


  //----------------------------------------------------------------------------
  /**
   *	@brief  �n�`���f���j��
   */
  //-----------------------------------------------------------------------------
  virtual void DeleteModel( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �n�`���f���̎擾
   *
   *	@retval ���f���I�u�W�F�N�g
   */
  //-----------------------------------------------------------------------------
  gfl::grp::g3d::Model* GetModel( void ) const;
#if GFL_GRP_G3D_H3D_USE
  gfl::grp::g3d::H3dModel* GetH3dModel( void ) const;
#endif // GFL_GRP_G3D_H3D_USE

  //----------------------------------------------------------------------------
  /**
   *	@brief  �n�`���f���̃��f�����\�[�X�擾
   *
   *	@retval ���f�����\�[�X
   */
  //-----------------------------------------------------------------------------
  const gfl::grp::g3d::Resource* GetResourceModel( void ) const;
#if GFL_GRP_G3D_H3D_USE
  const gfl::grp::g3d::H3dResource* GetH3dResourceModel( void ) const;
#endif // GFL_GRP_G3D_H3D_USE

  //----------------------------------------------------------------------------
  /**
   *	@brief  �n�`���f���̃e�N�X�`�����\�[�X�擾
   *
   *	@retval �e�N�X�`�����\�[�X
   */
  //-----------------------------------------------------------------------------
  const gfl::grp::g3d::Resource* GetResourceTexture( void ) const;
#if GFL_GRP_G3D_H3D_USE
  const gfl::grp::g3d::H3dResource* GetH3dResourceTexture( void ) const;
#endif // GFL_GRP_G3D_H3D_USE


  //----------------------------------------------------------------------------
  /**
   *	@brief  �ǂݍ��񂾃}�b�v�u���b�N�̃T�C�Y���擾
   */
  //-----------------------------------------------------------------------------
  s32 GetLoadFileSize( void ) const;

protected:

  //----------------------------------------------------------------------------
  /**
   *	@brief  ���[�N�̃N���A����  ���ׂẴ����o�̒l�����������܂��B
   */
  //-----------------------------------------------------------------------------
  virtual void Clear( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �}�b�v�o�b�t�@�@������
   *
   *	@param	param         �p�����[�^
   *	@param  p_sysHeap     �V�X�e���������p�q�[�v
   *	@param  p_deviceHeap  �f�o�C�X�������p�q�[�v
   */
  //-----------------------------------------------------------------------------
  virtual void InitializeMapBuffer( const SetUpParam& param, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap );
  //----------------------------------------------------------------------------
  /**
   *	@brief  �}�b�v�o�b�t�@�@�j��
   */
  //-----------------------------------------------------------------------------
  virtual void FinalizeMapBuffer( void );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ���f���o�b�t�@�@������
   *
   *	@param	param         �p�����[�^
   *	@param  p_sysHeap     �V�X�e���������p�q�[�v
   *	@param  p_deviceHeap  �f�o�C�X�������p�q�[�v
   */
  //-----------------------------------------------------------------------------
  virtual void InitializeModelBuffer( const SetUpParam& param, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap );
  //----------------------------------------------------------------------------
  /**
   *	@brief  ���f���o�b�t�@ �j��
   */
  //-----------------------------------------------------------------------------
  virtual void FinalizeModelBuffer( void );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ���f���f�[�^�pVRAM�m�ۏ���
   *
   *	@param	param         �p�����[�^
   *	@param  p_sysHeap     �V�X�e���������p�q�[�v
   *	@param  p_deviceHeap  �f�o�C�X�������p�q�[�v
   */
  //-----------------------------------------------------------------------------
  virtual void InitializeVramAddr( const SetUpParam& param, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap );
  //----------------------------------------------------------------------------
  /**
   *	@brief  VRAM�m�ۗ̈�̃N���A
   */
  //-----------------------------------------------------------------------------
  virtual void FinalizeVramAddr( void );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  �������[�h�V�X�e���̏�����
   *
   *	@param	param         �p�����[�^
   *	@param  p_sysHeap     �V�X�e���q�[�v
   *	@param  p_deviceHeap  �f�o�C�X�q�[�v
   */
  //-----------------------------------------------------------------------------
  virtual void InitializeDivLoad( const SetUpParam& param, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap );
  //----------------------------------------------------------------------------
  /**
   *	@brief  �������[�h�V�X�e���̔j��
   */
  //-----------------------------------------------------------------------------
  virtual void FinalizeDivLoad( void );

  
  //----------------------------------------------------------------------------
  /**
   *	@brief  �����ǂݍ��݂̏�ԑJ��
   */
  //-----------------------------------------------------------------------------
  virtual void UpdateLoad( void );


  //----------------------------------------------------------------------------
  /**
   *	@brief  ���f�����[�h�̈�v�`�F�b�N
   */
  //-----------------------------------------------------------------------------
  bool isModelMode( ModelMode mode ) const ;

protected:
  /// g3d::Model���g�p����ꍇ
  struct DefaultModelDatas
  {
    gfl::grp::g3d::Model* m_pModel;         // ���f��
    gfl::grp::g3d::Resource* m_pMdlRes;     // ���f�����\�[�X
    gfl::grp::g3d::Resource* m_pTexRes;     // ���f���e�N�X�`�����\�[�X
    gfl::grp::g3d::Resource* m_pGlobalTexRes[GLOBAL_RESOURCE_MAX]; // ���L�e�N�X�`�����\�[�X
  };
  

#if GFL_GRP_G3D_H3D_USE
  /// g3d::H3dModel���g�p����ꍇ
  struct H3dModelDatas
  {
    gfl::grp::g3d::H3dModel* m_pModel;         // ���f��
    gfl::grp::g3d::H3dResource* m_pMdlRes;     // ���f�����\�[�X
    gfl::grp::g3d::H3dResource* m_pTexRes;     // ���f���e�N�X�`�����\�[�X
    gfl::grp::g3d::H3dResource* m_pGlobalTexRes[GLOBAL_RESOURCE_MAX]; // ���L�e�N�X�`�����\�[�X
  };
#endif // GFL_GRP_G3D_H3D_USE

  /// Model���g�p����ꍇ
  union ModelDatas
  {
    DefaultModelDatas def;
#if GFL_GRP_G3D_H3D_USE
    H3dModelDatas     h3d;
#endif // GFL_GRP_G3D_H3D_USE
  };

protected:

  // �������@�L��
  b8 m_Initialize;
  b8 m_CreateModel;

  // ���f�����[�h
  ModelMode   m_ModelMode;    ///< ���̒l�����āAModelDatas�̒��g���g��������B

  // �\��ON�EOFF
  b8 m_Visible;
  

  // �`��o�^�V�[���V�X�e��
  gfl::grp::g3d::Scene* m_pScene;


  // �\���ʒu
  gfl::math::VEC3 m_Trans;

  // �f�[�^�ǂݍ��݃A�[�J�C�u�t�@�C��
  gfl::fs::ArcFile* m_pArcFile;
  bool m_ArcFileOriginal;
  // �A�[�J�C�u�f�[�^ID
  u32 m_DataID;

  // �}�b�v���ǂݍ��݃o�b�t�@
  void* m_pMapData;         // �o�b�t�@�|�C���^
  u32 m_MapDataHeapSize;    // �o�b�t�@�T�C�Y
  
  // �p�b�N�o�C�i���f�[�^Accessor
  gfl::base::BinLinkerAccessor m_LinkData;

  // �t�B�[���h�R���g���[��
  u32 m_FileType;               // �t�@�C�����ʁ@�^�C�v
  FileControl*  m_pFileControl;  // ���̃t�@�C���^�C�v�ɑΉ������t�@�C���R���g���[��
  const FileControlArray* m_cpFileControls; // �t�B�[���h�R���g���[���f�[�^
  

  // VRAM�̈�A�h���X
  uptr m_VramAddrTex;     // �e�N�X�`���@VRAM�A�h���X
  uptr m_VramAddrPltt;    // �p���b�g�@�@VRAM�A�h���X
  u32 m_MapTexVramSize;   // �e�N�X�`��VRAM�T�C�Y

  // �n�`���f�����
  ModelDatas      m_ModelDatas;

  // �����ǂݍ��݊Ǘ�
  DivLoad m_DivLoad;
  u32 m_DivLoadSize;        // �����ǂݍ��݁@�P��̓ǂݍ��݃T�C�Y
  LoadStatus  m_LoadStatus; // �ǂݍ��ݏ��
};
 


} // namespace map
} // namespace grp
} // namespace gfl
#endif // __GFL_MAPSYSTEM_H__
