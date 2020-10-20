#ifndef __GFL_LYTSYS_H__
#define __GFL_LYTSYS_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   gfl_LytSys.h
 *  @brief  ���C�A�E�g�V�X�e��
 *  @author tomoya takahashi
 *  @date   2010.10.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn.h>
#include <nn/math.h>

#include <base/gfl_Singleton.h>
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_math.h>
#include <gfl_Fs.h>
#include <str/gfl_Font.h>

#include <grp/g2d/gfl_G2dEnv.h>
#include <grp/g2d/gfl_LytRes.h>

#include <debug/gfl_DebugPrint.h>





namespace gfl {
namespace grp {

//-------------------------------------
/// �O���錾
//=====================================
class GraphicsSystem;
namespace g2d {
class LytSysBase;
class LytWkManager;
class LytWkBase;
class LytWk;
}

namespace g2d {

//-----------------------------------------------------------------------------
/**
 *        �f�o�b�O�o�͐ݒ�
 */
//-----------------------------------------------------------------------------
#define GFL_LYTSYS_DEBUGPRINT_LEVEL1 (gfl::debug::PRINT_FLAG_LEVEL1|gfl::debug::PRINT_FLAG_CATEGORY_G2D)


//*****************************************************************************
/**
 *          ���C�A�E�g�V�X�e���N���X
*/
//*****************************************************************************
//----------------------------------------------------------------------------
/**
 *  @brief  �萔��`
 */
//-----------------------------------------------------------------------------
typedef enum {

  // ���\�[�XID�@�s���l
  LYTSYS_RESID_ERROR = 0xffff,

  // �A�j���[�V�����o�^���s
  LYTSYS_ANIMEINDEX_ERROR = 0xffff,

  // �y�C���q�b�g�`�F�b�N���s
  LYTSYS_PANEINDEX_ERROR = 0xffff,

  // LYTWK�O���[�vID�@ALL
  LYTSYS_LYTWK_GROUPID_ALL = 0xffff,

  // �f�t�H���g�J�����|�W�V����
  LYTSYS_DEFAULT_CAMERA_UPPER_POS_Y = 0,
  LYTSYS_DEFAULT_CAMERA_LOWER_POS_Y = -290,


  ENUM_FORCE_DWORD(LytSysEnum)
} LytSysEnum;


//----------------------------------------------------------------------------
/**
 *  @brief  ���C�A�E�g���\�[�XID�^
 */
//-----------------------------------------------------------------------------
typedef u32 LytResID;

//----------------------------------------------------------------------------
/**
 *  @brief  ���C�A�E�g���\�[�X�@�A�[�J�C�u�C���f�b�N�X�^
 */
//-----------------------------------------------------------------------------
typedef u32 LytArcIndex;

//----------------------------------------------------------------------------
/**
 *  @brief  ���C�A�E�g���\�[�X�@�y�C���A�}�e���A���A�O���[�v�C���f�b�N�X�^
 */
//-----------------------------------------------------------------------------
typedef u32 LytPaneIndex;

//----------------------------------------------------------------------------
/**
 *  @brief  LYTWK�O���[�vID�@�^
 */
//-----------------------------------------------------------------------------
typedef u16 LytWKGroupID;

//----------------------------------------------------------------------------
/**
 *  @brief  �Z�b�g�A�b�v�\���̒�`
 */
//-----------------------------------------------------------------------------
typedef struct {
  u16 lyt_resource_max;   // �Ǘ����\�[�X�ő吔
  u16 lyt_wk_max;         // �Ǘ�LytWk�ő吔
} LytSysSetUp;


//----------------------------------------------------------------------------
/**
 *  @brief  LytWk�Z�b�g�A�b�v�\���̒�`
 */
//-----------------------------------------------------------------------------
typedef struct {
  gfl::math::VEC3   pos;        // �����ʒu
  gfl::math::VEC2   scale;      // �����g��l
  gfl::math::VEC3   rotate;     // ������]�p�x
  u16               priority;   // �\���v���C�I���e�B
  u16               groupID;    // �O���[�vID
} LytWkSetUp;


//-------------------------------------
/// �㉺��ʂɕ`�悷�邽�߂̃f�t�H���g�ݒ�
//�iUserEnv���g�p���Ă��Ȃ��ꍇ�j
//=====================================
extern const LytWkSetUp c_LytWkUpperDefaultSetUp;
extern const LytWkSetUp c_LytWkLowerDefaultSetUp;


//-------------------------------------
/// ���C�A�E�g�R�A�V�X�e��
//=====================================
typedef nw::lyt::Layout   LytCore;

//-------------------------------------
/// �y�C���@����p
//=====================================
typedef nw::lyt::Pane             LytPane;
typedef nw::lyt::PaneLinkList     LytPaneList;  // �����N���X�g

//-------------------------------------
/// �y�C���@��ޕ�
//=====================================
typedef nw::lyt::Picture  LytPicture;   // �摜�̕\�����s���y�C��
typedef nw::lyt::TextBox  LytTextBox;   // ������̕\�����s���y�C��
typedef nw::lyt::Window   LytWindow;    // Window�̕\�����s���y�C��
typedef nw::lyt::Bounding LytBounding;  // BoundingBox�Ƃ��Ďg�p����y�C��

//-------------------------------------
/// �}�e���A�����
//=====================================
typedef nw::lyt::Material LytMaterial;  // �}�e���A��


/// �f�t�H���g�t�H���g�̓o�^�C���f�b�N�X�ő�l
const u32 LYTSYS_DEFAULT_FONT_INDEX_MAX = LYTRES_DEFAULT_FONT_MAX;

//----------------------------------------------------------------------------
/**
 *  @brief  ���C�A�E�g�V�X�e���@�N���X�@�@�V���O���g���N���X
 *
 */
//-----------------------------------------------------------------------------
class LytSys
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(LytSys);


public:


  //----------------------------------------------------------------------------
  /**
   *	@brief  ���C�A�E�g�V�X�e���̏�����
   *	        *�Q�[���J�n���Ɉ�x�����ݒ肵�Ă��������B
   */
  //-----------------------------------------------------------------------------
  static void InitializeSystem( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator );
  //----------------------------------------------------------------------------
  /**
   *	@brief  ���C�A�E�g�V�X�e���̔j��
   */
  //-----------------------------------------------------------------------------
  static void FinalizeSystem( void );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief  Default�t�H���g�ݒ�
   *      �@  ���Q�[���J�n���Ɉ�x�����ݒ肵�Ă��������B
   *
   *  @param  font                  �t�H���g�f�[�^
   *  @param  name                  ���\�[�X���@(��@simple.bcfnt)�@���C�A�E�g���A�N�Z�X���邽�߂̖���
   *  @param  index                 �C���f�b�N�X�i0 �`�@LYTSYS_DEFAULT_FONT_INDEX_MAX)
   *  *�|�C���^��ێ����܂��B
   *  
   */
  //-----------------------------------------------------------------------------
  static void SetDefaultFont( gfl::str::Font* font, const char* name, u32 index = 0 );
  //-----------------------------------------------------------------------------
  /**
   *  @brief  Default�t�H���g����
   *  @param  index                 �C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  static void ResetDefaultFontInfo( u32 index );



  
  //-----------------------------------------------------------------------------
  /**
  *     �R���X�g���N�^�E�f�X�g���N�^
  */
  //-----------------------------------------------------------------------------
  LytSys(const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator);
  LytSys( heap::HeapBase* p_heap );   // �񓯊����������[�h�i�f�t�H���g���\�[�X�̃X���b�h�ǂݍ��݂ɑΉ��j
  ~LytSys();

  
  //----------------------------------------------------------------------------
  /**
   *	@brief  �񓯊��������i�@LytSys�ivoid�j�ŏ����������Ƃ��̂ݕK�v�@�j
   *
   */
  //-----------------------------------------------------------------------------
  void InitializeAsync( const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �񓯊������������܂�
   *	@retval true  �񓯊�������
   *	@retval false �񓯊���������
   */
  //-----------------------------------------------------------------------------
  bool InitializeAsyncWait( void );


  //----------------------------------------------------------------------------
  /**
   *	@brief  ���������������Ă��邩�`�F�b�N
   *	@retval true  ����
   *	@retval false �r��
   */
  //-----------------------------------------------------------------------------
  bool IsInitialized( void ) const;
  

  //----------------------------------------------------------------------------
  /**
   *  @brief    �I�[�g�`��t���O��ON�ɂȂ��Ă���SWk�̃f�[�^�X�V����
   *
   *  @param  lytwkGroupID    Update���s��LYTWK�O���[�vID (�y�C���O���[�vID�Ƃ͈Ⴄ�̂Œ��ӁI�j
   */
  //-----------------------------------------------------------------------------
  void UpdateAll( LytWKGroupID lytwkGroupID = LYTSYS_LYTWK_GROUPID_ALL );

  //----------------------------------------------------------------------------
  /**
   *  @brief    �I�[�g�`��t���O��ON�ɂȂ��Ă���SWk�̕`�揈��
   *
   *  @param    graphics          �O���t�B�b�N�X�V�X�e��
   *  @param    drawtype          �\���ʒ�`
   *  @param    renderTarget      �����_�����O�^�[�Q�b�g�̃t���[���o�b�t�@
   *  @param    rightRenderTarget �E�ڗp�����_�����O�^�[�Q�b�g�@�iDISPLAY_UPPER�̂Ƃ��̂ݎQ�Ɓj
   *  @param    lytwkGroupID      �`����s��LYTWK�O���[�vID(�y�C���O���[�vID�Ƃ͈Ⴄ�̂Œ��ӁI�j
   */
  //-----------------------------------------------------------------------------
  void DrawAll( gfl::grp::GraphicsSystem* graphics, DisplayType drawtype, RenderTarget* renderTarget, RenderTarget* rightRenderTarget = NULL, LytWKGroupID lytwkGroupID = LYTSYS_LYTWK_GROUPID_ALL );

  //----------------------------------------------------------------------------
  /**
   *  @brief  UpperLower�ɑΉ������O���[�o���`����f�[�^
   *
   *  @param  �`��ʁ@UPPER�@LOWER
   *
   *  @return �`����f�[�^
   *
   *   *�ʏ�LytWK�͂���Env���g�p���ĕ`�悳��Ă��܂��B
   *    �������A�X��LytWk�ɕʂ�Env��ݒ肷�邱�Ƃ��\�ł��B
   *    LytWk::SetUserEnv( gfl::grp::g2d::Env& upperEnv, gfl::grp::g2d::Env& lowerEnv );
   */
  //-----------------------------------------------------------------------------
  Env* GetGlobalEnv( DisplayType drawtype );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ���C�A�E�g���\�[�X�̓ǂݍ��݁i���k�f�[�^�Ή��Łj
   *
   *  @param  devAllocator �f�o�C�X�A���P�[�^
   *  @param  arcFile   �A�[�J�C�u�t�@�C��
   *  @param  dataID    �f�[�^ID
   *  @param  comp      ���k�f�[�^�t���O�@true = ���k�f�[�^
   *  @param  useDefaultFont  Default�t�H���g���g�p���邩�H
   *  @param  addTextureNum   �ォ��ǉ�����e�N�X�`����
   *
   *  @retval ���\�[�XID
   *  @retval LTYSYS_RESID_ERROR  ���\�[�X���o�^�ł��Ȃ������B
   */
  //-----------------------------------------------------------------------------
  LytResID LoadLytRes( heap::NwAllocator* devAllocator, fs::ArcFile* arcFile, u32 dataID, bool comp, bool useDefaultFont, u32 addTextureNum );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ���C�A�E�g���\�[�X�̓ǂݍ��݁i�񈳏k�f�[�^�j
   *
   *  @param  devAllocator    �f�o�C�X�A���P�[�^
   *  @param  arcFile         �A�[�J�C�u�t�@�C��
   *  @param  dataID          �f�[�^ID
   *  @param  useDefaultFont  Default�t�H���g���g�p���邩�H
   *  @param  addTextureNum   �ォ��ǉ�����e�N�X�`����
   *
   *  @retval ���\�[�XID
   */
  //-----------------------------------------------------------------------------
  LytResID LoadLytRes( heap::NwAllocator* devAllocator, fs::ArcFile* arcFile, u32 dataID, bool useDefaultFont=true, u32 addTextureNum = 0 );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ���C�A�E�g���\�[�X�̓ǂݍ��݁i���k�f�[�^�j
   *
   *  @param  devAllocator �f�o�C�X�A���P�[�^
   *  @param  arcFile   �A�[�J�C�u�t�@�C��
   *  @param  dataID    �f�[�^ID
   *  @param  useDefaultFont  Default�t�H���g���g�p���邩�H
   *  @param  addTextureNum   �ォ��ǉ�����e�N�X�`����
   *
   *  @retval ���\�[�XID
   *  @retval LTYSYS_RESID_ERROR  ���\�[�X���o�^�ł��Ȃ������B
   */
  //-----------------------------------------------------------------------------
  LytResID LoadLytResComp( heap::NwAllocator* devAllocator, fs::ArcFile* arcFile, u32 dataID, bool useDefaultFont=true, u32 addTextureNum = 0 );


  //----------------------------------------------------------------------------
  /**
   *  @brief  �o�C�i���f�[�^���ړn���B
   *
   *  @param  devAllocator    �f�o�C�X�A���P�[�^
   *  @param  cp_buff          �f�[�^  (�����ł́A�j�����܂���B�j
   *  @param  useDefaultFont  Default�t�H���g���g�p���邩�H
   *  @param  addTextureNum   �ォ��ǉ�����e�N�X�`����
   *
   *  @retval ���\�[�XID
   */
  //-----------------------------------------------------------------------------
  LytResID LoadLytRes( heap::NwAllocator* devAllocator, const void * cp_buff, bool useDefaultFont=true, u32 addTextureNum = 0 );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ���C�A�E�g���\�[�X�̔j��
   *
   *  @param  resID     �j�����郊�\�[�XID
   */
  //-----------------------------------------------------------------------------
  void ReleaseLytRes( LytResID resID );

  //----------------------------------------------------------------------------
  /**
   *  @brief  dstID�̃��\�[�X��srcID�̃e�N�X�`�����\�[�X�����ׂĒǉ�����B
   *
   *  @param  srcID     ���L�e�N�X�`���f�[�^�������Ă��郊�\�[�XID
   *  @param  dstID     ���L�e�N�X�`���f�[�^��ǉ����郊�\�[�XID
   *
   * ���ǉ������e�N�X�`���̖�����LoadLytRes�֐���addTextureNum�ɓn���Ċǂ����B
   * ��dstID�̃��\�[�X�̑O��srcID��j�����Ă͂����܂���B
   *
   */
  //-----------------------------------------------------------------------------
  void RegistTextureRes( LytResID srcID, LytResID dstID, gfl::grp::MemoryArea area = gfl::grp::MEMORY_AREA_FCRAM );


  // �����Ń������z�u���Ȃ������e�N�X�`���́A�f�o�C�X�������ɔz�u����܂��B
  //-----------------------------------------------------------------------------
  /**
   *  @brief  ���\�[�XID���ɂ���e�N�X�`�������ׂă������ɔz�u����
   *
   *  @param  resID   ���\�[�XID
   *  @param  area    �]���惁�����@�iFCRAM/VRAMA/VRAMB�j
   *
   *  2011/05/31 VRAM FCRAM�S�đΉ������悤�ł��B
   */
  //-----------------------------------------------------------------------------
  void TransferAllTexture( LytResID resID, gfl::grp::MemoryArea area );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  ���\�[�XID���ɂ���e�N�X�`�������ׂă������ɔz�u����
   *
   *  @param  resID         ���\�[�XID
   *  @param  texArcIndex   �A�[�J�C�u���̃e�N�X�`���C���f�b�N�X
   *  @param  area          �]���惁�����@�iFCRAM/VRAMA/VRAMB�j
   *  2011/05/31 VRAM FCRAM�S�đΉ������悤�ł��B
   */
  //-----------------------------------------------------------------------------
  void TransferIndexTexture( LytResID resID, LytArcIndex texArcIndex, gfl::grp::MemoryArea area );


  //-----------------------------------------------------------------------------
  /**
   *  @brief  ���\�[�XID�̃��\�[�X���擾����B
   *
   *  @param  resID         ���\�[�XID
   *
   *  @retval ���\�[�XAccessor
   */
  //-----------------------------------------------------------------------------
  LytRes* GetResource( LytResID resID ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  �i�[���Ă��郊�\�[�X�̑������`�F�b�N
   *
   *	@retval ���\�[�X�̑���
   */
  //-----------------------------------------------------------------------------
  u32 GetUseResourceNum( void ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  ���C�A�E�g���[�N����
   *
   *  @param  p_heap            �q�[�v�A���P�[�^
   *  @param  p_device          �f�o�C�X�q�[�v�A���P�[�^
   *  @param  resID             ���C�A�E�g���\�[�XID
   *  @param  lytDataID         resID���̃��C�A�E�g�f�[�^ID
   *  @param  anmResIDTbl       �A�j���[�V�����f�[�^ID�e�[�u��      �A�j���̂Ȃ��ꍇ��NULL
   *  @param  anmTblNum         �A�j���[�V�����f�[�^ID�e�[�u����    �A�j���̂Ȃ��ꍇ��0
   *  @param  anmMax            �o�^�A�j���[�V�����ő吔�i���j
   *  @param  wkdata            ���C�A�E�g���[�N��{���
   *  @param  anmBind           �A�j���[�V�����̃o�C���h���s���B
   *  @param  createPaneTbl     �y�C���e�[�u������
   *
   *  @note
   *  �i���janmMax = �o�^�A�j���[�V�����ő吔�ɂ���
   *        ���L�A�j���[�V�����𗘗p���Ȃ��ꍇ�AanmTblNum�Ɠ��������w�肵�Ă��������B
   *        ���L�A�j���[�V�����𗘗p����ꍇ�AanimTblNum + ���L�A�j���[�V�����̐� ���w�肵�Ă��������B
   *  �A���P�[�^�[�w��ł��g�p����ƁALytWk�֘A�̃q�[�v��S�Ďw�肵���q�[�v���g�p���Ċm�ۂ���悤�ɂȂ�܂��B
   *
   *  @retval ���C�A�E�g���[�N
   *  @retval NULL              �������s
   */
  //-----------------------------------------------------------------------------
  LytWk* CreateLytWk( LytResID resID, LytArcIndex lytDataID, const LytArcIndex* anmDataIDTbl, u32 anmTblNum, u32 anmMax, const LytWkSetUp* setup, b32 anmBind = true, b32 createPaneTbl = false, b32 createPaneTblWithHash = false );
  LytWk* CreateLytWk( gfl::heap::NwAllocator* p_heap, gfl::heap::NwAllocator* p_device, LytResID resID, LytArcIndex lytDataID, const LytArcIndex* anmDataIDTbl, u32 anmTblNum, u32 anmMax, const LytWkSetUp* setup, b32 anmBind = true, b32 createPaneTbl = false, b32 createPaneTblWithHash = false );
  //
  LytWk* BuildLytWk( gfl::heap::NwAllocator* p_heap, gfl::heap::NwAllocator* p_device, LytResID resID, LytArcIndex lytDataID, u32 anmMax, const LytWkSetUp* setup, b32 createPaneTbl=false, b32 createPaneTblWithHash=false );
  void   BuildAnimation( LytWk* wk, LytResID resID, const LytArcIndex* anmDataIDTbl, u32 anmTblNum, b32 anmBind=true, b32 createPaneTblWithHash=false );

  //----------------------------------------------------------------------------
  /**
   * @brief �A�j���[�V��������
   *
   * @param lytWk                  �Ώۃ��C�A�E�g���[�N
   * @param resID                  ���\�[�XID
   * @param anmDataIDTbl           �A�j���[�V�����f�[�^
   * @param snmOffset              �A�j���[�V�����e�[�u���Q�ƃI�t�Z�b�g
   * @param anmTblNum              �A�j���[�V�����e�[�u����
   * @param anmBind                �A�j���[�V�������o�C���h����Ȃ�true���w�肷��
   * @param createPaneTblWithHash  �������Ƀn�b�V���e�[�u���𗘗p����Ȃ�true���w�肷��
   */
  //----------------------------------------------------------------------------
  void CreateAnimation(  LytWk* lytWk, LytResID resID, const LytArcIndex* anmDataIDTbl, u32 anmOffset, u32 anmTblNum, b32 anmBind, b32 createPaneTblWithHash );

  //----------------------------------------------------------------------------
  /**
   * @brief ���C�A�E�g���[�N�����C�A�E�g�}�l�[�W���Ƀ����N������
   *
   * @param lytWk �ǉ��ΏۂƂȂ郌�C�A�E�g���[�N
   */
  //----------------------------------------------------------------------------
  void AddLinkWithLayoutWork( LytWk* lytWk );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ���C�A�E�g���[�N�j��
   *
   *  @param  lytResID  ���C�A�E�g���\�[�XID
   *  @param  anmResID  �A�j���[�V�������\�[�XID
   *  @param  wkdata    ���C�A�E�g���[�N��{���
   */
  //-----------------------------------------------------------------------------
  void RemoveLytWk( LytWk* lytWk );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �������ꂽ���C�A�E�g���[�N���̎擾
   */
  //-----------------------------------------------------------------------------
  u32 GetLytWkNum( void ) const;


  // �g�p�s�\
  LytSysBase* GetLytSysBase(void) const;

private:

  LytSysBase* m_base;
};



//----------------------------------------------------------------------------
/**
 *  @brief  ���C�A�E�g���[�N�@�N���X
 */
//-----------------------------------------------------------------------------
class LytWk
{

  friend class LytSys;
  friend class LytWkManager;

  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(LytWk);


public:

  //----------------------------------------------------------------------------
  /**
   *  @brief    �g�p�\��Ԏ擾
   *
   *  @retval true    �g�p�\
   *  @retval false   �g�p�s�\
   */
  //-----------------------------------------------------------------------------
  bool IsInitialized(void);


  //----------------------------------------------------------------------------
  /**
   *  @brief    ��Update(AutoDrawFlag��false�̂Ƃ��̂ݗL��)
   */
  //-----------------------------------------------------------------------------
  void Update( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief    �ʕ`��(AutoDrawFlag��false�̂Ƃ��̂ݗL��)
   *
   *  @param    graphics    �O���t�B�b�N�X�V�X�e��
   *  @param    drawtype    UPPER�@LOWER
   *  @param    renderTarget �����_�����O�^�[�Q�b�g�̃t���[���o�b�t�@
   *  @param    rightRenderTarget  �E�ڗp�����_�����O�^�[�Q�b�g�@�iDISPLAY_UPPER�̂Ƃ��̂ݎQ�Ɓj
   */
  //-----------------------------------------------------------------------------
  void Draw( gfl::grp::GraphicsSystem* graphics, DisplayType drawtype, RenderTarget* renderTarget, RenderTarget* rightRenderTarget = NULL );


  //----------------------------------------------------------------------------
  /**
   *  @brief  UpperLower�ɑΉ��������[�U�[�`����f�[�^�ݒ�
   *
   *  @param  upperEnv    ���ʗpENV
   *  @param  lowerEnv    ����ʗpENV
   *
   *  *�`���GlobalEnv�ł͂Ȃ�������ENV���g�p����悤�ɂȂ�܂��B
   *  *�߂��ɂ́ASetGlobalEnv()���Ă�ł��������B
   *
   */
  //-----------------------------------------------------------------------------
  void SetUserEnv( const gfl::grp::g2d::Env* upperEnv, const gfl::grp::g2d::Env* lowerEnv );

  //----------------------------------------------------------------------------
  /**
   *  @brief  �O���[�o��ENV���g�p����悤�ɐݒ�
   */
  //-----------------------------------------------------------------------------
  void SetGlobalEnv( void );


  //----------------------------------------------------------------------------
  /**
   *  @brief  LytWkGroupID��ݒ肷��
   *
   *  @param  groupID �O���[�vID
   */
  //-----------------------------------------------------------------------------
  void SetLytWkGroupID( LytWKGroupID groupID );

  //----------------------------------------------------------------------------
  /**
   *  @brief  LytWkGroupID���擾����
   *
   *  @return �O���[�vID
   */
  //-----------------------------------------------------------------------------
  LytWKGroupID GetLytWkGroupID( void ) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief  �`�悷��f�B�X�v���C�^�C�v��ݒ�
   *
   *	@param	  lod     �`�悷��f�B�X�v���C�^�C�v
   *  
   *  lod 
   *    DISPLAY_UPPER     ���ʂɕ\��
   *    DISPLAY_LOWER     ����ʂɕ\��
   *    DISPLAY_BOTH_MAX  ����ʂɕ\��
   *    
   */
  //-----------------------------------------------------------------------------
  void SetDrawDispType( DisplayType lod );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �`�悷��f�B�X�v���C�^�C�v�̎擾
   *
   *	@return �`�悷��f�B�X�v���C�^�C�v
   */
  //-----------------------------------------------------------------------------
  DisplayType GetDrawDispType( void ) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief  ��\���y�C���̃A�j���[�V�����X�V���ȗ�����t���O
   *	�i�f�t�H���gON�j
   */
  //-----------------------------------------------------------------------------
  void SetVisiblePaneNotAnimeOption( bool flag );

  //----------------------------------------------------------------------------
  /**
   *	@brief  ��\���y�C���̃A�j���[�V�����X�V���ȗ�����t���O�`�F�b�N
   *	@retval true  �L��
   *	@retval false ����
   */
  //-----------------------------------------------------------------------------
  bool IsVisiblePaneNotAnimeOption( void ) const;


  //============================================================================
  /**
   *  �y�C������
   */
  //=============================================================================
  //----------------------------------------------------------------------------
  /**
   *  @brief  PaneIndex�̃y�C�����擾
   *
   *  @param  paneIndex GFL_LytSys_ResrouceConv.rb����o�͂����.._pane.h�̃C���f�b�N�X
   *
   *  @retval �y�C��
   *  @retval NULL    Index���s��
   */
  //-----------------------------------------------------------------------------
  LytPane* GetPane( LytPaneIndex paneIndex ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  PaneIndex�̃s�N�`���[�y�C�����擾
   *
   *  @param  paneIndex GFL_LytSys_ResrouceConv.rb����o�͂����.._pane.h�̃C���f�b�N�X
   *
   *  @retval �s�N�`���[�y�C��
   *  @retval NULL Index���s�����APicturePane�łȂ�
   */
  //-----------------------------------------------------------------------------
  LytPicture* GetPicturePane( LytPaneIndex paneIndex ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  PaneIndex�̃e�L�X�g�{�b�N�X�y�C�����擾
   *
   *  @param  paneIndex GFL_LytSys_ResrouceConv.rb����o�͂����.._pane.h�̃C���f�b�N�X
   *
   *  @retval �e�L�X�g�{�b�N�X�y�C��
   *  @retval NULL Index���s�����ATextBoxPane�łȂ�
   */
  //-----------------------------------------------------------------------------
  LytTextBox* GetTextBoxPane( LytPaneIndex paneIndex ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  PeneIndex�̃e�L�X�g�{�b�N�X�y�C���̕�����o�b�t�@�̃��������m��
   *
   *	@param	paneIndex     �y�C���C���f�b�N�X
   *	@param  minLen        �m�ە�����
   */
  //-----------------------------------------------------------------------------
  void AllocTextBoxPaneStringBuffer( LytPaneIndex paneIndex, u16 minLen );

  //----------------------------------------------------------------------------
  /**
   *  @brief  PaneIndex��window�y�C�����擾
   *
   *  @param  paneIndex GFL_LytSys_ResrouceConv.rb����o�͂����.._pane.h�̃C���f�b�N�X
   *
   *  @retval window�y�C��
   *  @retval NULL Index���s����windowPane�łȂ�
   */
  //-----------------------------------------------------------------------------
  LytWindow* GetWindowPane( LytPaneIndex paneIndex ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  PaneIndex�̃o�E���f�B���O�{�b�N�X�y�C�����擾
   *
   *  @param  paneIndex GFL_LytSys_ResrouceConv.rb����o�͂����.._pane.h�̃C���f�b�N�X
   *
   *  @retval �o�E���f�B���O�{�b�N�X�y�C��
   *  @retval NULL Index���s����boundingPane�łȂ�
   */
  //-----------------------------------------------------------------------------
  LytBounding* GetBoundingPane( LytPaneIndex paneIndex ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  PaneIndex�̃}�e���A�����擾
   *
   *  @param  paneIndex GFL_LytSys_ResrouceConv.rb����o�͂����.._pane.h�̃C���f�b�N�X
   *
   *  @retval �}�e���A�����
   *  @retval NULL  Index���s�����}�e���A���ł͂Ȃ�
   */
  //-----------------------------------------------------------------------------
  LytMaterial* GetMaterial( LytPaneIndex paneIndex ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  PaneIndex�̃O���[�v�̃y�C�����X�g���擾
   *
   *  @param  groupIndex  �O���[�v�C���f�b�N�X
   *
   *  @return �y�C�����X�g
   */
  //-----------------------------------------------------------------------------
  LytPaneList* GetPaneList( LytPaneIndex groupIndex ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�b�g�`�F�b�N���\���擾
   *
   *  @retval true  �\
   *  @retval false �s�\    ��x�`�悷��K�v������܂��B
   */
  //-----------------------------------------------------------------------------
  bool IsHitCheckOK(void) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  �SPane�q�b�g�`�F�b�N
   *
   *  @param  �X�N���[�����W�n�ł̂��A���i�^�b�`�p�l�����W���̂܂܁j
   *  @param  boundingOnly  �o�E���f�B���O�y�C���̂݃`�F�b�N����:true
   *
   *  @retval �ŏ��Ƀq�b�g�����y�C��
   *  @retval NULL  �q�b�g���Ȃ�
   *
   *  *�q�b�g�`�F�b�N���s���O�ɕK����xDraw����Ă���K�v������܂��B
   *  �@�܂��A1�O�̕`��ʒu�Ńq�b�g�`�F�b�N���s���܂��B
   *
   *  *��]�ɂ͑Ή����Ă���܂���B
   */
  //-----------------------------------------------------------------------------
  LytPane* GetHitPaneAll( s32 x, s32 y, bool boundingOnly ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  �SPane�q�b�g�`�F�b�N
   *
   *  @param  �X�N���[�����W�n�ł̂��A���i�^�b�`�p�l�����W���̂܂܁j
   *  @param  boundingOnly  �o�E���f�B���O�y�C���̂݃`�F�b�N����:true
   *
   *  @retval �ŏ��Ƀq�b�g�����y�C���̃C���f�b�N�X
   *  @retval LYTSYS_PANEINDEX_ERROR  �q�b�g���Ȃ�
   *
   *  *�q�b�g�`�F�b�N���s���O�ɕK����xDraw����Ă���K�v������܂��B
   *  �@�܂��A1�O�̕`��ʒu�Ńq�b�g�`�F�b�N���s���܂��B
   *
   *  *��]�ɂ͑Ή����Ă���܂���B
   */
  //-----------------------------------------------------------------------------
  LytPaneIndex GetHitPaneIndexAll( s32 x, s32 y, bool boundingOnly ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  GroupPane�q�b�g�`�F�b�N
   *
   *  @param  Group��Index�@GFL_LytSys_ResrouceConv.rb����o�͂����.._pane.h�̃C���f�b�N�X
   *  @param  View���W�n�ł̂��A���i��ʒ��S�����_�j
   *  @param  boundingOnly  �o�E���f�B���O�y�C���̂݃`�F�b�N����:true
   *
   *  @retval �ŏ��Ƀq�b�g�����y�C��
   *  @retval NULL  �q�b�g���Ȃ�
   *
   *  *�q�b�g�`�F�b�N���s���O�ɕK����xDraw����Ă���K�v������܂��B
   *  �@�܂��A1�O�̕`��ʒu�Ńq�b�g�`�F�b�N���s���܂��B
   *
   *  *��]�ɂ͑Ή����Ă���܂���B
   */
  //-----------------------------------------------------------------------------
  LytPane* GetHitPaneGroup( LytPaneIndex groupIndex, s32 x, s32 y, bool boundingOnly ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  GroupPane�q�b�g�`�F�b�N
   *
   *  @param  Group��Index�@GFL_LytSys_ResrouceConv.rb����o�͂����.._pane.h�̃C���f�b�N�X
   *  @param  �X�N���[�����W�n�ł̂��A���i�^�b�`�p�l�����W���̂܂܁j
   *  @param  boundingOnly  �o�E���f�B���O�y�C���̂݃`�F�b�N����:true
   *
   *  @retval �ŏ��Ƀq�b�g�����y�C���̃C���f�b�N�X
   *  @retval LYTSYS_PANEINDEX_ERROR  �q�b�g���Ȃ�
   *
   *  *�q�b�g�`�F�b�N���s���O�ɕK����xDraw����Ă���K�v������܂��B
   *  �@�܂��A1�O�̕`��ʒu�Ńq�b�g�`�F�b�N���s���܂��B
   *
   *  *��]�ɂ͑Ή����Ă���܂���B
   */
  //-----------------------------------------------------------------------------
  LytPaneIndex GetHitPaneIndexGroup( LytPaneIndex groupIndex, s32 x, s32 y, bool boundingOnly ) const;


  //----------------------------------------------------------------------------
  /**
   *  @brief  �P��Pane�Ƃ̃q�b�g�`�F�b�N
   *
   *  @param  �X�N���[�����W�n�ł̂��A���i�^�b�`�p�l�����W���̂܂܁j
   *  @param  �y�C���C���f�b�N�X
   *
   *  @retval true    �q�b�g���Ă���B
   *  @retval false   �q�b�g���Ă��Ȃ��B
   *
   *  *�q�b�g�`�F�b�N���s���O�ɕK����xDraw����Ă���K�v������܂��B
   *  �@�܂��A1�O�̕`��ʒu�Ńq�b�g�`�F�b�N���s���܂��B
   *
   *  *��]�ɂ͑Ή����Ă���܂���B
   */
  //-----------------------------------------------------------------------------
  bool GetHitPane( s32 x, s32 y, LytPaneIndex index ) const;


  //----------------------------------------------------------------------------
  /**
   *  @brief  �}�e���A���̃e�N�X�`���������ւ���B
   *
   *  @param  paneIndex   �}�e���A���̃C���f�b�N�X
   *  @param  mapIndex    �P�}�e���A�����̉��Ԗڂ̃e�N�X�`����ύX���邩�H
   *  @param  cp_res      ���\�[�X���[�N
   *  @param  texArcIndex �e�N�X�`���̃A�[�J�C�u�C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  void SetMaterialImage( LytPaneIndex materialIndex, u32 mapIndex, const LytRes* cp_res, LytArcIndex texArcIndex );



  //-----------------------------------------------------------------------------
  /**
   *  @brief  ���\�[�X�����擾
   *
   *  @param  resID         ���\�[�XID
   *
   *  @retval ���\�[�X
   */
  //-----------------------------------------------------------------------------
  const LytRes* GetResource( void ) const;




  //============================================================================
  /**
   *  ��{����
   */
  //=============================================================================
  //----------------------------------------------------------------------------
  /**
   *  @brief    �I�[�g�`���ON/OFF�ݒ�
   *
   *  @param    flag    �t���O  true:ON   false:OFF
   */
  //-----------------------------------------------------------------------------
  void SetAutoDrawFlag( bool flag );


  //----------------------------------------------------------------------------
  /**
   *  @brief    �I�[�g�`���ON/OFF �擾
   *
   *  @retval true    ON
   *  @retval false   OFF
   */
  //-----------------------------------------------------------------------------
  bool IsAutoDraw( void ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  �����D�揇�ʂ̐ݒ�
   *
   *  @param  pri   �D�揇�ʁi 0 > 0xffff )
   *
   *  *�������A�f�v�X�e�X�g�Ȃ��ŁA�`����s���ꍇ�ɂ́A
   *�@�ォ��`�悳�ꂽ���[�N����O�ɕ`�悳��邽�ߒ��ӁB
   *
   *  UpdateAll��DrawAll���s���ۂ̗D�揇�ʂł��B
   */
  //-----------------------------------------------------------------------------
  void SetPriority( u16 pri );

  //----------------------------------------------------------------------------
  /**
   *  @brief  �����D�揇�ʂ̎擾
   *
   *  @retval �D�揇�ʁi 0 > 0xffff )
   *
   *  *�������A�f�v�X�e�X�g�Ȃ��ŁA�`����s���ꍇ�ɂ́A
   *�@�ォ��`�悳�ꂽ���[�N����O�ɕ`�悳��邽�ߒ��ӁB
   *
   *  UpdateAll��DrawAll���s���ۂ̗D�揇�ʂł��B
   */
  //-----------------------------------------------------------------------------
  u16 GetPriority( void ) const;


  //----------------------------------------------------------------------------
  /**
   *  @brief  ���W�̐ݒ�
   *
   *  @param  pos
   */
  //-----------------------------------------------------------------------------
  void SetTranslate( const gfl::math::VEC3& pos );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ���W�̎擾
   *
   *  @param  p_pos   ���W�i�[��
   */
  //-----------------------------------------------------------------------------
  void GetTranslate( gfl::math::VEC3* p_pos ) const;


  //----------------------------------------------------------------------------
  /**
   *  @brief  �g��̐ݒ�
   *
   *  @param  scale   �X�P�[�����O�l
   */
  //-----------------------------------------------------------------------------
  void SetScale( const gfl::math::VEC2& scale );

  //----------------------------------------------------------------------------
  /**
   *  @brief  �g��̎擾
   *
   *  @param p_scale �X�P�[�����O�l�i�[��
   */
  //-----------------------------------------------------------------------------
  void GetScale( gfl::math::VEC2* p_scale ) const;


  //----------------------------------------------------------------------------
  /**
   *  @brief  ��]�̐ݒ�
   *
   *  @param  rotate   �e���̉�]�l
   */
  //-----------------------------------------------------------------------------
  void SetRotate( const gfl::math::VEC3& rotate );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ��]�̎擾
   *
   *  @param  p_rotate   �e���̉�]�l�i�[��
   */
  //-----------------------------------------------------------------------------
  void GetRotate( gfl::math::VEC3* p_rotate ) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief  �J�����O�t���O�̐ݒ�
   *
   *	@param	flag  �t���O
   *
   *	�J�����O��false�ɂ���Ƃ������Ƃ́A
   *	���A�E�A�����ׂẲ�ʂŃ����_�����O���������s�����Ƃ������Ƃł��̂ŁA
   *	�������ׂɒ��ӂ��Ă��������B
   */
  //-----------------------------------------------------------------------------
  void SetCullingFlag( bool flag );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �J�����O�t���O�̎擾
   *
   *	@retval	true  �J�����O����ON
   *	@retval false �J�����O����OFF
   */
  //-----------------------------------------------------------------------------
  bool IsCullingFlag( void ) const;

  //============================================================================
  /**
   *  �A�j���[�V�����֘A
   */
  //=============================================================================

  //----------------------------------------------------------------------------
  /**
   *  @brief  �A�j���[�V�����̒ǉ��o�^
   *
   *  @param  anmResID  �A�j���[�V�������\�[�XID
   *
   *  @retval �o�^Index
   *  @retval LYTSYS_ANIMEINDEX_ERROR �o�^���s
   */
  //-----------------------------------------------------------------------------
  u32 CreateAnime( LytArcIndex anmDataID );


  //----------------------------------------------------------------------------
  /**
   *  @brief  ���L�A�j���[�V�����̓o�^    LytPaneIndex���w�肵�āA�ʓo�^����^�C�v
   *
   *  @param  startAnimeIndex �o�^�J�n�A�j���[�V�����C���f�b�N�X
   *  @param  anmResID        �A�j���[�V�������\�[�XID
   *  @param  maxNum          �o�^�ő吔�i0���Ƃ��ׂēo�^�j
   *  @param  groupIndex      �o�^�O���[�v�C���f�b�N�X�iLYTSYS_PANEINDEX_ERROR���Ƃ��ׂēo�^�j
   *  @param  paneIndex       �o�^�O���[�v���y�C���C���f�b�N�X�iLYTSYS_PANEINDEX_ERROR���Ƃ��ׂēo�^�j
   *                          �igroupIndex���w�肵�Ȃ��ƁApaneIndex�͌��ʂ�����܂���B�j
   *
   *  @retval �o�^��  (startAnimeIndex����߂�l���̃C���f�b�N�X�ɃA�j���[�V�������ǉ�����܂����B�j
   *
   *  *�A�j���[�V�������[�N���A���Ŋm�ۂł��Ȃ���ASSERT�łƂ܂�܂��B
   *  *�o�^���Ȃǔc�����Ȃ���g�p���Ă��������B
   *
   *  *���L�A�j���[�V�����Ƃ́A
   *  �@anmResID�̃A�j���[�V�����𕡐��̃y�C�������ʂŎg�p���邱�Ƃ��ł��܂��B
   *  LayoutEditor���ŁA�A�j���[�V�������g�p���邱�Ƃ��ł���y�C�����w�肵�Ă����A
   *  ���̊֐��ŃA�j���[�V�������쐬���Ă��������B
   *
   *  ���L�A�j���[�V�����Ƃ������O�ł����A�A�j���[�V�����t���[���̊Ǘ��͓Ɨ����Ă��܂��B
   *  startAnimeIndex+�߂�l���̃C���f�b�N�X���w�肵�ăA�j���[�V�����̃t���[���Ǘ����s���Ă��������B
   *
   *  ����{�ƂȂ��Ă���A�j���[�V�����͒ʏ��CreateAnime�Ő������Ă��������B
   */
  //-----------------------------------------------------------------------------
  u32 CreateShareAnime( u32 startAnimeIndex, LytArcIndex anmDataID, u32 maxNum=0, LytPaneIndex groupIndex=LYTSYS_PANEINDEX_ERROR, LytPaneIndex paneIndex=LYTSYS_PANEINDEX_ERROR );

  //----------------------------------------------------------------------------
  /**
   *  @brief  �A�j���[�V�����̍폜
   *
   *  @param  anmIdx  �폜����A�j���[�V�����C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  void DeleteAnime( u32 anmIdx );

  //----------------------------------------------------------------------------
  /**
   *  @brief    �A�j���[�V������񂪂��邩�`�F�b�N
   *
   *  @param  anmaIdx       �A�j���[�V�����C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  bool IsAnime( u32 anmIdx ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief    �A�j���[�V�����̔��f
   *
   *  @param  anmIdx        �A�j���[�V����IDX
   *  @param  is_auto_unbind   one time �A�j���[�V�����̏ꍇ�A�A�j���[�V����������Aunbind����B
   */
  //-----------------------------------------------------------------------------
  void BindAnime( u32 anmIdx, bool is_auto_unbind = true );

  //-----------------------------------------------------------------------------
  /**
   *  @brief    �A�j���[�V�����̔��f����
   *
   *  @param  anmIdx    �o�^��IDX
   */
  //-----------------------------------------------------------------------------
  void UnBindAnime( u32 anmIdx );

  //-----------------------------------------------------------------------------
  /**
   *  @brief    �S�A�j���[�V�����̔��f����
   */
  //-----------------------------------------------------------------------------
  void UnBindAnimeAll( void );


  //-----------------------------------------------------------------------------
  /**
   *  @brief    �A�j���[�V�������o�C���h����Ă��邩�`�F�b�N
   *
   *  @retval true    �o�C���h����Ă���B
   *  @retval false   �o�C���h����Ă��Ȃ��B
   */
  //-----------------------------------------------------------------------------
  bool IsBindAnime( u32 anmIdx ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief    �I�[�g�A�j���[�V�����ݒ�
   *
   *  @param  anmIdx    �A�j���[�V����IDX
   *  @param  flag      ON/OFF�t���O
   */
  //-----------------------------------------------------------------------------
  void SetAutoAnime( u32 anmIdx, bool flag );

  //-----------------------------------------------------------------------------
  /**
   *  @brief    �I�[�g�A�j���[�V�������`�F�b�N
   *
   *  @param  anmIdx    �A�j���[�V����IDX
   *
   *  @retval true    �I�[�g�A�j��
   *  @retval false   ��I�[�g�A�j��
   */
  //-----------------------------------------------------------------------------
  bool IsAutoAnime( u32 anmIdx ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief    �I�[�g�A�j���[�V���� �X�s�[�h�ݒ�@�i�S�I�[�g�A�j�����ʁj
   *
   *  @param  speed     �A�j���[�V�����X�s�[�h
   */
  //-----------------------------------------------------------------------------
  void SetAutoAnimeSpeed( f32 speed );


  //-----------------------------------------------------------------------------
  /**
   *  @brief    �I�[�g�A�j���[�V���� �X�s�[�h�ݒ�@�i�ʐݒ�j
   *
   *  @param  speed     �A�j���[�V�����X�s�[�h
   */
  //-----------------------------------------------------------------------------
  void SetAutoAnimeSpeed( u32 anmIdx, f32 speed );


  //-----------------------------------------------------------------------------
  /**
   *  @brief    �A�j���[�V�����t���[���ݒ�
   *
   *  @param  anmIdx    �A�j���[�V����IDX
   *  @param  frame     �A�j���[�V�����t���[��
   */
  //-----------------------------------------------------------------------------
  void SetAnimeFrame( u32 anmIdx, f32 frame );

  //-----------------------------------------------------------------------------
  /**
   *  @brief    �A�j���[�V�����t���[���@�J�E���g
   *
   *  @param  anmIdx    �A�j���[�V����IDX
   *  @param  frame     �A�j���[�V�����t���[��
   *
   *  @retval true    �A�j���[�V�����I���iLOOP�ݒ�̂Ƃ��͏��false�j
   *  @retval false   �A�j���[�V�����r��
   */
  //-----------------------------------------------------------------------------
  bool AddAnimeFrame( u32 anmIdx, f32 frame );

  //-----------------------------------------------------------------------------
  /**
   *  @brief    �A�j���[�V�����t���[���擾
   *
   *  @param  anmIdx    �A�j���[�V����IDX
   */
  //-----------------------------------------------------------------------------
  f32 GetAnimeFrame( u32 anmIdx ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief    �A�j���[�V�����ő�t���[���擾
   *
   *  @param  anmIdx    �A�j���[�V����IDX
   */
  //-----------------------------------------------------------------------------
  f32 GetAnimeMaxFrame( u32 anmIdx ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ���[�v�A�j���[�V�������`�F�b�N
   *
   *  @param  anmIdx    �A�j���[�V����IDX
   *
   *  @retval true    ���[�v�A�j���[�V����
   *  @retval false   ���[�v�A�j���[�V��������Ȃ�
   */
  //-----------------------------------------------------------------------------
  bool IsAnimeLoop( u32 anmIdx ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief    �A�j���[�V�����̊����`�F�b�N
   *
   *  @param  anmIdx    �A�j���[�V����IDX
   *
   *  @retval true    ����
   *  @retval false   �r��
   */
  //-----------------------------------------------------------------------------
  bool IsAnimeEnd( u32 anmIdx ) const;






  //----------------------------------------------------------------------------
  /**
   *  @brief  ���C�A�E�g�R�A�V�X�e���@�i�x�[�X���C�u�����̃N���X�j�擾
   *
   *  @return �x�[�X���C�u�����̃N���X
   */
  //-----------------------------------------------------------------------------
  LytCore* GetLayoutCore( void ) const;


	//----------------------------------------------------------------------------
	/**
	 *	@brief    ��\���̃y�C���ɑ΂��čs��v�Z���s�����ǂ�����ݒ肷��
	 *
	 *	@return		none
	 */
	//-----------------------------------------------------------------------------
	void SetInvisiblePaneCalculateMtx( bool flg );

	//----------------------------------------------------------------------------
	/**
	 *	@brief    �R���o�[�g���ɖ��ߍ��܂ꂽ�t�H���g�����擾����B
	 *            ���ۂ͉��L�����[�U�[�f�[�^��g2d_fw�Ƃ��Ė��ߍ��܂ꂽ����
	 *
	 *	@return		f32 ���B0�̎��͎擾���s
	 */
	//-----------------------------------------------------------------------------
	f32 GetFontWidthWhenConvert( LytPaneIndex index );

private:
  //-----------------------------------------------------------------------------
  /**
  *     �ȉ��̊֐��g�p�֎~
  */
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------
  /**
  *     �R���X�g���N�^�E�f�X�g���N�^
  *     ���̃N���X�́A���Ȃ炸LytSys::CreateLytWk���g�p���Đ������邱�ƁB
  */
  //-----------------------------------------------------------------------------
  LytWk(void);
  ~LytWk();
  void Initialize( gfl::heap::NwAllocator* allocator, LytWkManager* p_wkman );
  LytWkBase* GetLytWkBase( void ) const;

private:
  void Finalize( void );

  LytWkManager* m_parent;

  LytWkBase*  m_base;
};




} /* end of namespace g2d */
} /* end of namespace grp */
} /* end of namespace gfl */

#endif // __GFL_LYTSYS_H__
