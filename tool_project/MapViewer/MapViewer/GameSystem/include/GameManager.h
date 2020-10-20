#ifndef __GAMEMANAGER_H__
#define __GAMEMANAGER_H__
//===================================================================
/**
 * @file    GameManager.h
 * @brief   �Q�[���őS�̂�ʂ��ĊǗ����K�v�ȃN���X�̃C���X�^���X�����N���X
 * @author  k.ohno
 * @date    10.12.22
*/
//===================================================================
#pragma once

#include <base/include/gfl2_Singleton.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <ui/include/gfl2_UI.h>
///#include <ro/include/gfl2_RoManager.h>@saito�s�v

// gfl2
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_types.h>   // for displayNo @todo �A�v���p�ɒ�`������\�������� 150217 ikuta_junya

namespace gfl2 {
  namespace lyt {
    class LayoutRenderPath;  // @todo ������Layout���g����悤�ɂ��Ă���  kawada_koji_20150209
  }
}

#include "GameSystem/include/GameFrameMode.h"
#include "GameSystem/include/GameEventManager.h"

//-----------------------------------------------------------------------------
// �N���X�O���Q�Ɛ錾
//-----------------------------------------------------------------------------
namespace GameSystem {
  class GameProcManager;
  class GameManager;
  class GameData;
}

namespace Field{
  class FieldSystem;
  class Fieldmap;
  class FieldResident;
/**
  namespace FieldScript{
    class FieldScriptSystem;
  }
*///@saito�s�v
}

/**
GFL_NAMESPACE_BEGIN( print );
class GrammaticalAttributeProvider;
GFL_NAMESPACE_END( print );
*/
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
namespace GameSystem {

class GameManager : public gfl2::base::Singleton<GameManager>
{
  GFL_FORBID_COPY_AND_ASSIGN(GameManager); //�R�s�[�R���X�g���N�^�{����֎~

  public:

  /*
     �C�V���l����񋟂��󂯂Ă�����
     ---
     ���i�R�[�h          �FCTR-P-AKJA
     �^�C�g���R�[�h      �F0x00055D01
     ���j�[�NID          �F0x0055D

     ���i�R�[�h          �FCTR-P-AK2A
     �^�C�g���R�[�h      �F0x00055E00
     ���j�[�NID          �F0x0055E
     ---
  */
  // X�̃��j�[�N�h�c���w�� @todo
  // SD�J�[�h�͋��p�Ȃ��߁A�Е��̃��j�[�N�h�c�ł���K�v������B
  static const u32 APLICATION_UNIQUE_ID = 0x0055D; // �A�v���P�[�V�����ŗLID

  static const u8 BUFFER_CLEAR_TARGET_BIT_COLOR = 0x01;
  static const u8 BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL = 0x01 << 1;

  struct BUFFER_CLEAR_SETTING
  {
    gfl2::gfx::Color color;   
    f32 depth;
    u8 stencil;
    u8 clearTarget;    // �N���A���s���Ώۃo�b�t�@�ABUFFER_CLEAR_TARGET_BIT�`�̘_���a
  };


  private:
  gfl2::heap::HeapBase *m_systemHeap;
  gfl2::heap::HeapBase *m_deviceHeap;
  gfl2::heap::HeapBase *m_procCellHeap;

  ///����t���[���Ǘ��p
  GameSystem::FrameMode  m_ReqFrameMode;  // ���샂�[�h���N�G�X�g���
  GameSystem::FrameMode  m_FrameMode;     // ���샂�[�h
  u8         m_FrameCount;    // �t���[���J�E���g 0�̂Ƃ�topFrame 1�̂Ƃ�nextFrame
  s32        m_VSyncCount;    // ���������񐔐���

  // �����_�����O�̃p�����[�^
  f32        m_parallaxDepthRange;  // ���̎��̎����̕␳�l

  ///�����ɏd�v�ȃC���X�^���X�������Ă�������
  GameSystem::GameProcManager*            m_procManager;
  bool                        m_fatalEvent;   ///< FatalEvent����

  GameSystem::GameEventManager*           m_GameEventManager;
  GameData*                               m_GameData;
  Field::Fieldmap                         *m_pFieldmap;
  Field::FieldResident                    *m_pFieldResident;
  ///Field::FieldScript::FieldScriptSystem* m_FieldScriptSystem;@saito�s�v

  ///print::GrammaticalAttributeProvider* m_pGrammaticalAttributeProvider;  ///< ���@�Ή����W���[��@saito�s�v

#if GFL_DEBUG
  bool mDebugKeyDisable;
#endif

	bool mEndRequest;

  public:

  //----------------------------------------------------------------------------
  /**
   *	@brief    �R���X�g���N�^
   *  @param    sHeap �V�X�e���q�[�v
   *  @param    dHeap �f�o�C�X�q�[�v
   */
  //-----------------------------------------------------------------------------
  GameManager(gfl2::heap::HeapBase *sHeap, gfl2::heap::HeapBase *dHeap);
  //-----------------------------------------------------------------------------
  /**
  *     @brief �f�X�g���N�^
  */
  //-----------------------------------------------------------------------------
  ~GameManager();

  //----------------------------------------------------------------------------
  /**
   *	@brief    ������
   */
  //-----------------------------------------------------------------------------
  void Initialize(void);
  //----------------------------------------------------------------------------
  /**
   *	@brief    ���C������
   *  @return   �������I������ꍇfalse ���s��true
   */
  //-----------------------------------------------------------------------------
  bool Process(void);

	void EndRequest()
	{
		mEndRequest = true;
	}

	bool IsEndRequest()
	{
		return mEndRequest;
	}

  //----------------------------------------------------------------------------
  /**
   *	@brief    �J������
   */
  //-----------------------------------------------------------------------------
  void Finalize(void);

  //----------------------------------------------------------------------------
  /**
   *	@brief    ����t���[���ύX���N�G�X�g
   */
  //-----------------------------------------------------------------------------
  void RequestFrameMode( GameSystem::FrameMode mode );

  //----------------------------------------------------------------------------
  /**
   *	@brief    �����_�����O�̃p�����[�^�Ƀf�t�H���g�l��ݒ肷��B
   */
  //-----------------------------------------------------------------------------
  void SetDefaultRenderingParameter( void );
  //----------------------------------------------------------------------------
  /**
   *	@brief    �����_�����O�̃p�����[�^��ݒ肷��B
   *	          �ݒ肵�����܂ł��̒l���g�p���ꑱ���܂��B
   *
   *  @param    parallaxDepthRange  ���̎��̎����̕␳�l
   *	                               (gfl2::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_��
   *                                  gfl2::grp::g3d::StereoCamera��GetDepthRange�֐��̖߂�l��n���ĉ������B)
   */
  //-----------------------------------------------------------------------------
  void SetRenderingParameter( const f32 parallaxDepthRange );
  //----------------------------------------------------------------------------
  /**
   *	@brief    �����_�����O�̃p�����[�^���擾����B
   *
   *  @param[out]    parallaxDepthRange  ���̎��̎����̕␳�l
   *	                                    (gfl2::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_��
   *                                       gfl2::grp::g3d::StereoCamera��GetDepthRange�֐��̖߂�l��n���ĉ������B)
   */
  //-----------------------------------------------------------------------------
  void GetRenderingParameter( f32* parallaxDepthRange ) const;

  // @fix SBTS[2501] : �y�\���F����̃f�����ɗ��̎���ON�ɂ���ƉE�ڗp�f���ɕs�����������z
  //----------------------------------------------------------------------------
  /**
   *	@brief  �E�ڂ������_�����O����邩�H
   *
   *	@retval true  �����_�����O�����
   *	@retval false �����_�����O����Ȃ�
   */
  //-----------------------------------------------------------------------------
  bool IsUpperRightRendering( void ) const;

  /**
   *	@brief  �t�B�[���h�V�X�e�����擾(�Ȃ����Ƃ�����)
   *
   *	@return �t�B�[���h�V�X�e��(NULL�̏ꍇ�Ȃ�)
   */
  Field::FieldSystem* GetFieldSystem( void ){ return m_pFieldSystem; }

  /**
  *	@brief  �t�B�[���h�V�X�e����ݒ�
  *
  * @param  pFieldSystem  �t�B�[���h�V�X�e��
  *
  *	@retun  ����
  */
  void SetFieldSystem( Field::FieldSystem* pFieldSystem ){ m_pFieldSystem = pFieldSystem; }

  public:

  //-----------------------------------------------------------------------------
  /**
   * @brief �t�@�C���ǂݍ��݃}�l�[�W�����擾����
   */
  //-----------------------------------------------------------------------------
  gfl2::fs::AsyncFileManager* GetAsyncFileManager( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief UI �f�o�C�X�}�l�[�W�����擾����
   */
  //-----------------------------------------------------------------------------
  gfl2::ui::DeviceManager* GetUiDeviceManager( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief �v���Z�X�}�l�[�W�����擾����
   */
  //-----------------------------------------------------------------------------
  inline GameSystem::GameProcManager* GetProcManager( void ) const { return m_procManager; }

  //-----------------------------------------------------------------------------
  /**
   * @brief ���ݓ��삵�Ă���v���Z�X�����邩�`�F�b�N
   */
  //-----------------------------------------------------------------------------
  bool IsProcessExists( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief �Q�[���C�x���g�}�l�[�W���̎擾
   */
  //-----------------------------------------------------------------------------
  inline GameSystem::GameEventManager* GetGameEventManager( void ) const { return m_GameEventManager; }

  //-----------------------------------------------------------------------------
  /**
   * @brief ���ݓ��삵�Ă���C�x���g�����邩�`�F�b�N
   */
  //-----------------------------------------------------------------------------
  bool IsEventExists( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief �Q�[���f�[�^���擾
   */
  //-----------------------------------------------------------------------------
  inline GameData* GetGameData( void ) const { return m_GameData; }

  //----------------------------------------------------------------------------
  /**
   *  @brief  �t�B�[���h�}�b�v���[�N�@�̃Z�b�^�[�E�Q�b�^�[
   *  *FieldmapProc���Z�b�g����B
   */
  //-----------------------------------------------------------------------------
  inline void SetFieldmap( Field::Fieldmap* p_fieldmap ) { m_pFieldmap = p_fieldmap; }
  inline Field::Fieldmap* GetFieldmap( void ) const { return m_pFieldmap; }

  //----------------------------------------------------------------------------
  /**
   *  @brief  �t�B�[���hresident
   */
  //-----------------------------------------------------------------------------
  inline void SetFieldResident( Field::FieldResident* p_fieldresi ) { m_pFieldResident = p_fieldresi; }
  inline Field::FieldResident* GetFieldResident( void ) const { return m_pFieldResident; }
  
  //-----------------------------------------------------------------------------
  /**
   * @brief �t�B�[���h�X�N���v�g�V�X�e���̎擾
   */
  //-----------------------------------------------------------------------------
  ///inline Field::FieldScript::FieldScriptSystem* GetFieldScriptSystem( void ) const { return m_FieldScriptSystem; }@saito�s�v

  //--------------------------------------------------------------------------
  /**
   *	@brief  FatalError�C�x���g�N���t���O���Z�b�g
   *
   *  @note   ���̊֐����ĂԂƁA�t�B�[���h�̃C�x���g�`�F�b�N�ŁA
   *           �ŗD���FatalErrorEvent���Ă΂�܂��B
   *           �K���t�B�[���h�̃C�x���g�`�F�b�N���Ă΂�邱�Ƃ�ۏ؂��Ă���g�p�I�I�I
   */
  //--------------------------------------------------------------------------
  void SetFatalErrorEvent( void ){ m_fatalEvent = true; }

  //--------------------------------------------------------------------------
  /**
   *	@brief  FatalError�C�x���g�N���t���O���擾
   *
   *  @retval true:�N������
   *  @retval false:�N�����Ȃ�
   */
  //--------------------------------------------------------------------------
  bool GetFatalErrorEvent( void ) const{ return m_fatalEvent; }


  // �o�b�t�@�N���A�ݒ�

  /**
   *	@brief  �w���ʂ̃N���A�ݒ��ݒ肷��
   *
   *  @param  display   �Ώۉ��
   *  @param  rSetting  �N���A�ݒ�
   *
   *  @return ����
   */
  void SetBufferClearSetting( gfl2::gfx::CtrDisplayNo display, const BUFFER_CLEAR_SETTING& rSetting );

  /**
   *	@brief  �w���ʂ̃N���A�ݒ���擾����
   *
   *  @param  display   �Ώۉ��
   *  @param  pOut      �i�[��
   *
   *  @return ����
   */
  b32 GetBufferClearSetting( gfl2::gfx::CtrDisplayNo display, BUFFER_CLEAR_SETTING* pOut ) const;



  //==========================================================================/
  //�ȉ��Aprivate
  private:

  //�ʐM�v���Z�X
  void NetworkProcess(void);
  // �C�x���g����
  void UpdateEventProcess( void );

  // �v���Z�X�}�l�[�W���̐���
  void CreateProcManager( void ); // �v���Z�X�}�l�[�W���𐶐�����
  void DeleteProcManager( void ); // �v���Z�X�}�l�[�W����j������
  bool UpdateProcManager( void ); // �v���Z�X�}�l�[�W���̍X�V����
  bool NextFrameUpdateProcManager( void );     // �v���Z�X�}�l�[�W���̍X�V����
  void DrawProcManager( void );                // �v���Z�X�}�l�[�W���̕`�揈��

  // �t���[���Ǘ�
  void ClearFrameControl( void );              // �t���[���Ǘ��̏�����
  bool IsFrameControl( GameSystem::FrameMode mode ) const; // ���̃��[�h�`�F�b�N
  bool UpdateFrameControl( void );             // �t���[���R���g���[���X�V
  void ChangeFrameControl( void );             // �t���[���R���g���[���̃��[�h�ύX
  void RequestFrameControl( GameSystem::FrameMode mode );  // �t���[���R���g���[���̃��[�h�ύX���N�G�X�g
  bool DoFrameAction( void );                  // �P�t���[���ōs���A�N�V����

  // �|�[�Y����
  bool isPause( void );

#if GFL_DEBUG
  // UI���O�Đ��̈ꎞ��~�ݒ�
  void DebugUiDeviceSoftwareControlEnabel( bool isEnable );
  // �A�v���P�[�V�����L�[���͖������
  inline bool IsDebugKeyDisable( void ) const { return mDebugKeyDisable; }
#endif

  //! @name  Layout�T���v���̂��߂̍������̏���  @todo ������Layout���g����悤�ɂ��Ă���  kawada_koji_20150209
  //@{
private:
  gfl2::lyt::LayoutRenderPath* m_pLayoutRenderPath;


  Field::FieldSystem* m_pFieldSystem;


  BUFFER_CLEAR_SETTING m_BufferClearSettingTable[ gfl2::gfx::CtrDisplayNo::NumberOf ];   // �o�b�t�@�N���A�ݒ�

public:
  void SetLayoutRenderPath(gfl2::lyt::LayoutRenderPath* pLayoutRenderPath) { m_pLayoutRenderPath = pLayoutRenderPath; }
  gfl2::lyt::LayoutRenderPath* GetLayoutRenderPath(void) const { return m_pLayoutRenderPath; }
  //@}

};

} // namespace GameSystem

/** ---------------------------------------------------------------------
//! @brief  SINGLETON�̑O���Q�Ɛ錾
// ---------------------------------------------------------------------*/
SINGLETON_INSTANCE_DECLARATION( GameSystem::GameManager );



#endif //__GAMEMANAGER_H__
