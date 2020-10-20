//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_LytSys.cpp
 *	@brief  ���C�A�E�g�V�X�e��
 *	@author	tomoya takahashi
 *	@date		2010.10.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <nw/lyt.h>

#include <debug/gfl_Assert.h>
#include <debug/gfl_DebugPrint.h>

#include <grp/gfl_GraphicsSystem.h>

#include <grp/g2d/gfl_LytSys.h>
#include <grp/g2d/gfl_LytWkBase.h>
#include <grp/g2d/gfl_LytRes.h>

#include <gfl_Std.h>
#include <gfl_Fs.h>
#include <ro/gfl_RoManager.h>

//-----------------------------------------------------------------------------
/**
*    Singleton�C���X�^���X�̎��̂��`
*/
//-----------------------------------------------------------------------------

namespace gfl {
namespace grp {
namespace g2d {

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
typedef struct
{
  DisplayType     lod;
  Env::CameraType cameraType;
} LytSysLodState;
  
static const LytSysLodState sc_LytSysLodAnmProjIdx[ DISPLAY_BOTH_MAX ] = {
  { DISPLAY_UPPER,    Env::CAMERA_NORMAL },
  { DISPLAY_LOWER,    Env::CAMERA_NORMAL },
  { DISPLAY_UPPER,    Env::CAMERA_RIGHT },
};


//-------------------------------------
///	�㉺��ʂɕ`�悷�邽�߂̃f�t�H���g�ݒ�
//=====================================
const LytWkSetUp c_LytWkUpperDefaultSetUp = 
{
  gfl::math::VEC3(0,LYTSYS_DEFAULT_CAMERA_UPPER_POS_Y,0), 
  gfl::math::VEC2(1,1), 
  gfl::math::VEC3(0,0,0), 
  0,
  0
};

const LytWkSetUp c_LytWkLowerDefaultSetUp = 
{
  gfl::math::VEC3(0,LYTSYS_DEFAULT_CAMERA_LOWER_POS_Y,0), 
  gfl::math::VEC2(1,1), 
  gfl::math::VEC3(0,0,0), 
  0,
  0
};

//-----------------------------------------------------------------------------
/**
 *          LytResManager�N���X�錾
 */
//-----------------------------------------------------------------------------
class LytResManager
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(LytResManager);


public:
  LytResManager( heap::NwAllocator* allocator, u32 lyt_res_max );
  ~LytResManager();

  LytRes* GetClearLytRes( LytResID* idx ) const;
  
  LytRes* GetIndexLytRes( LytResID idx ) const;

  u32 GetUseLytResNum( void ) const;

private:

  // ���C�A�E�g���\�[�X�z��
  LytRes*   m_lytArray;
  u32       m_lytArrayNum;
};

//-----------------------------------------------------------------------------
/**
 *          LytWkManager�N���X�錾
 */
//-----------------------------------------------------------------------------
class LytWkManager
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(LytWkManager);

public:
  LytWkManager( heap::NwAllocator* allocator, u32 wk_max );
  ~LytWkManager();

  LytWk* GetClearWk( void ) const;
  void UpdateAll( LytWKGroupID groupID );
  void DrawAll( DisplayType drawtype, RenderTarget* renderTarget, LytWKGroupID groupID );

  void AddLink( LytWk* wk );
  void DelLink( LytWk* wk );
  void SortPriority( LytWk* wk );

  inline u32 GetCreateWkNum( void ) const { return m_createNum; }
  
private:

  LytWk*  m_array;
  u16     m_arrayNum;
  u16     m_createNum;

  LytWk  m_linkTop;  // �����N���X�g

  bool CheckGroupID( u16 groupID_a, u16 groupID_b ) const;
  
};

//-----------------------------------------------------------------------------
/**
 *					LytSysBase�N���X�錾
*/
//-----------------------------------------------------------------------------
class LytSysBase
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(LytSysBase);


public:
  // �V�X�e��������
  static void InitializeSystem( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator );
  // �V�X�e���j��
  static void FinalizeSystem( void );

  
  // Default�t�H���g�ݒ�
  static void SetDefaultFont( gfl::str::Font* font, const char* name, u32 index );
  // Default�t�H���g�����邩�`�F�b�N
  static bool IsDefaultFont(u32 index);
  // Default�t�H���g���擾
  static gfl::str::Font* GetDefaultFont(u32 index);
  // Default�t�H���g���擾
  static const char* GetDefaultFontName(u32 index);
  // Default�t�H���g����
  static void ResetDefaultFontInfo( u32 index );


  // �R���X�g���N�^�f�X�g���N�^
  LytSysBase( const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator );
  LytSysBase(void);
  ~LytSysBase();

  // �������̗���
  void Initialize( const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator );
  void InitializeAsync( const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator );
  bool InitializeAsyncWait( void );
  inline bool IsInitialized( void ) const { return m_Initialized; }
  
  
  // �����擾����C���^�[�t�F�[�X
  LytWkBaseGlobalData* GetGlobalData(void);
  LytResManager* GetResManager(void) const;
  LytWkManager* GetWkManager(void) const;


  gfl::heap::NwAllocator* GetHeapAllocator(void) const;
  gfl::heap::NwAllocator* GetDeviceAllocator(void) const;
  

private:

  // �����ɉ��ǂݍ��ނ��H
  static const s32 MULTI_LOAD_ROMFILE_NUM = 3;
  
  // �t�H���g���p�o�b�t�@�T�C�Y
  static const u32 FONT_NAME_LENGTH = 64;
  
private:

  bool m_Initialized;
  bool m_InitializeSystem;

  gfl::heap::NwAllocator*         m_allocator;    // �������A���P�[�^
  gfl::heap::NwAllocator*         m_devAllocator;// �f�o�C�X�������A���P�[�^

  
  LytWkBaseGlobalData*   m_globalData;      // �SLytWk���L�f�[�^
  LytResManager*         m_lytResMan;       // ���C�A�E�g���\�[�X�}�l�[�W��
  LytWkManager*          m_wkMan;           // ���[�N�}�l�[�W��

  static gfl::str::Font* m_defaultFont[LYTSYS_DEFAULT_FONT_INDEX_MAX];     // Default�t�H���g
  // Default�t�H���g��
  // dll������̕����񃊃e�����w��ɑΉ����邽�߁A�|�C���^����z��֕ύX�B
  static char            m_defaultFontName[LYTSYS_DEFAULT_FONT_INDEX_MAX][FONT_NAME_LENGTH];

  // �O���[�o�����\�[�X�@�h�����[
  static bool                      m_systemInitialized;
  static nw::lyt::GraphicsResource m_globalResource;
  static nw::lyt::Drawer           m_drawer;
  static gfl::fs::RomFile          m_RomFile;
  static s32                       m_lytSysCount;
};
//���[�U�[�A���P�[�^�[�ݒ�
#define LYTSYSBASE_CHANGE_USE_ALLOCATOR(wk)  \
  nw::os::IAllocator* p_default_allocator_heap = nw::lyt::Layout::GetAllocator();\
  nw::os::IAllocator* p_default_allocator_device = nw::lyt::Layout::GetDeviceMemoryAllocator();\
  nw::lyt::Layout::SetAllocator( (wk)->GetHeapAllocator() );\
  nw::lyt::Layout::SetDeviceMemoryAllocator( (wk)->GetDeviceAllocator() );

// �f�t�H���g�A���P�[�^�[�ɖ߂�
#define LYTSYSBASE_RETURN_ALLOCATOR(wk)  \
  nw::lyt::Layout::SetAllocator( p_default_allocator_heap );\
  nw::lyt::Layout::SetDeviceMemoryAllocator( p_default_allocator_device );



//-----------------------------------------------------------------------------
/**
 *    Default�t�H���g����
 */
//-----------------------------------------------------------------------------
gfl::str::Font* LytSysBase::m_defaultFont[LYTSYS_DEFAULT_FONT_INDEX_MAX]      = {NULL};
char            LytSysBase::m_defaultFontName[LYTSYS_DEFAULT_FONT_INDEX_MAX][LytSysBase::FONT_NAME_LENGTH];// Default�t�H���g��
bool            LytSysBase::m_systemInitialized = false;
nw::lyt::GraphicsResource LytSysBase::m_globalResource;   ///<�O���[�o�����\�[�X
nw::lyt::Drawer           LytSysBase::m_drawer;           ///<�h����[
gfl::fs::RomFile          LytSysBase::m_RomFile;          ///<�����t�@�C���X
s32             LytSysBase::m_lytSysCount = 0;            ///<�����������C�A�E�g�V�X�e���̐�

  
  
//=============================================================================
/**
 *					���C�A�E�g�V�X�e���N���X�@����
*/
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g�V�X�e���̏�����
 *	        *�Q�[���J�n���Ɉ�x�����ݒ肵�Ă��������B
 */
//-----------------------------------------------------------------------------
void LytSys::InitializeSystem( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator )
{
  LytSysBase::InitializeSystem( heapAllocator, deviceAllocator );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g�V�X�e���̔j��
 */
//-----------------------------------------------------------------------------
void LytSys::FinalizeSystem( void )
{
  LytSysBase::FinalizeSystem();
}

//-----------------------------------------------------------------------------
/**
 *  @brief  Default�t�H���g�ݒ�
 *      �@  ���Q�[���J�n���Ɉ�x�����ݒ肵�Ă��������B
 *
 *  @param  font                  �t�H���g�f�[�^
 *  @param  name                  ���\�[�X���@(��@simple.bcfnt)�@���C�A�E�g���A�N�Z�X���邽�߂̖���
 *  @param  index                 �C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void LytSys::SetDefaultFont( gfl::str::Font* font, const char* name, u32 index )
{
  LytSysBase::SetDefaultFont( font, name, index );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  Default�t�H���g����
 *  @param  index                 �C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void LytSys::ResetDefaultFontInfo( u32 index )
{
  LytSysBase::ResetDefaultFontInfo( index );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^
 *	
 *	@param	setup   �Z�b�g�A�b�v���
 */
//-----------------------------------------------------------------------------
LytSys::LytSys( const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator )
{
  gfl::heap::HeapBase* heap = heapAllocator->GetHeapBase();
  
  // LytSysBase����
  m_base = GFL_NEW(heap) LytSysBase( setup, heapAllocator, deviceAllocator );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^
 */
//-----------------------------------------------------------------------------
LytSys::LytSys( heap::HeapBase* p_heap )   // �񓯊����������[�h�i�f�t�H���g���\�[�X�̃X���b�h�ǂݍ��݂ɑΉ��j
{
  // LytSysBase����
  m_base = GFL_NEW(p_heap) LytSysBase();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
LytSys::~LytSys()
{
  GFL_DELETE m_base;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �񓯊��������i�@LytSys�ivoid�j�ŏ����������Ƃ��̂ݕK�v�@�j
 *
 *	@retval true  ��������
 *	@retval false ������
 */
//-----------------------------------------------------------------------------
void LytSys::InitializeAsync( const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator )
{
  m_base->InitializeAsync( setup, heapAllocator, deviceAllocator );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �񓯊������������܂�
 *	@retval true  �񓯊�������
 *	@retval false �񓯊���������
 */
//-----------------------------------------------------------------------------
bool LytSys::InitializeAsyncWait( void )
{
  return m_base->InitializeAsyncWait();
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���������������Ă��邩�`�F�b�N
 *	@retval true  ����
 *	@retval false �r��
 */
//-----------------------------------------------------------------------------
bool LytSys::IsInitialized( void ) const
{
  return m_base->IsInitialized();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �I�[�g�`��t���O��ON�ɂȂ��Ă���SWk�̃f�[�^�X�V����
 *
 *	@param  lytwkGroupID    Update���s�����C�A�E�g���[�N�O���[�vID
 */
//-----------------------------------------------------------------------------
void LytSys::UpdateAll( LytWKGroupID lytwkGroupID )
{
  GFL_ASSERT( m_base->IsInitialized() );
  
  m_base->GetWkManager()->UpdateAll( lytwkGroupID );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �I�[�g�`��t���O��ON�ɂȂ��Ă���SWk�̕`�揈��
 *
 *  @param    graphics          �O���t�B�b�N�X�V�X�e��
 *	@param	  drawtype          �\���ʒ�`
 *	@param    renderTarget      �����_�����O�^�[�Q�b�g�̃t���[���o�b�t�@
 *	@param    rightRenderTarget �E�ڗp�����_�����O�^�[�Q�b�g�@�iDISPLAY_UPPER�̂Ƃ��̂ݎQ�Ɓj
 *	@param    lytwkGroupID      �`����s��LYTWK�O���[�vID(�y�C���O���[�vID�Ƃ͈Ⴄ�̂Œ��ӁI�j
 */
//-----------------------------------------------------------------------------
void LytSys::DrawAll( gfl::grp::GraphicsSystem* graphics, DisplayType drawtype, RenderTarget* renderTarget, RenderTarget* rightRenderTarget, LytWKGroupID lytwkGroupID )
{
  GFL_ASSERT( m_base->IsInitialized() );

  if( graphics->BindCurrentCommandList( renderTarget ) )
  {
    renderTarget->ActivateBuffer();
    m_base->GetWkManager()->DrawAll( drawtype, renderTarget, lytwkGroupID );
  }

  if( (drawtype == DISPLAY_UPPER) && (rightRenderTarget != NULL) ){
    if( graphics->BindCurrentCommandList( rightRenderTarget ) )
    {
      rightRenderTarget->ActivateBuffer();
      m_base->GetWkManager()->DrawAll( DISPLAY_UPPER_RIGHT, rightRenderTarget, lytwkGroupID );
    }
  }
}

/*�R�}���h�L���b�V���g�p��
void LytSys::DrawAll( DisplayType drawtype, FrameBuffer* frameBuffer, FrameBuffer* rightFrameBuffer )
{
  gfl::grp::GraphicsSystem* p_grpSys = GFL_SINGLETON_INSTANCE(GraphicsSystem);
  
  // �Z�[�u
  p_grpSys->StartCommandSave();
  m_base->GetWkManager()->DrawAll( drawtype, frameBuffer );
  p_grpSys->EndCommandSave();


  // �t���[���o�b�t�@�̃o�C���h
  frameBuffer->Bind();
  p_grpSys->ReuseCommandSave();

  if( (drawtype == DISPLAY_UPPER) && (rightFrameBuffer != NULL) ){
    // �t���[���o�b�t�@�̃o�C���h
    rightFrameBuffer->Bind();
    p_grpSys->ReuseCommandSave();
  }
}
*/

//----------------------------------------------------------------------------
/**
 *	@brief  UpperLower�ɑΉ������`����f�[�^
 *
 *	@param	�`��ʁ@UPPER�@LOWER
 *
 *	@return �`����f�[�^
 */
//-----------------------------------------------------------------------------
Env* LytSys::GetGlobalEnv( DisplayType drawtype )
{
  LytWkBaseGlobalData* globalData = m_base->GetGlobalData();
  if( drawtype < DISPLAY_NORMAL_MAX ){
    return &globalData->env[drawtype];
  }
  GFL_ASSERT(0);
  return &globalData->env[0];
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g���\�[�X�̓ǂݍ���
 *
 *	@param  devAllocator �f�o�C�X�A���P�[�^
 *	@param  cp_buff          �f�[�^  (�����ł́A�j�����܂���B�j
 *	@param  useDefaultFont  Default�t�H���g���g�p���邩�H
 *	@param  addTextureNum   �ォ��ǉ�����e�N�X�`����
 *
 *	@retval ���\�[�XID
 *	@retval LTYSYS_RESID_ERROR  ���\�[�X���o�^�ł��Ȃ������B
 */
//-----------------------------------------------------------------------------
LytResID LytSys::LoadLytRes( heap::NwAllocator* devAllocator, const void * cp_buff, bool useDefaultFont, u32 addTextureNum )
{
  LytResManager* lytResMan  = m_base->GetResManager();
  LytRes* res;
  LytResID     idx;

  res = lytResMan->GetClearLytRes( &idx );
  if( res == NULL ){
    return LYTSYS_RESID_ERROR;
  }

  // �������̊m�ې��ݒ�
  LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);
  

  // �ǂݍ��ݏ���
  res->Load( devAllocator, cp_buff, addTextureNum );

  // Default�t�H���g�̐ݒ�
  if( useDefaultFont ){
    for( u32 i=0; i<LYTSYS_DEFAULT_FONT_INDEX_MAX; ++i ){
      if( LytSysBase::IsDefaultFont(i) ){
        res->SetDefaultFont( LytSysBase::GetDefaultFont(i), LytSysBase::GetDefaultFontName(i), i );
      }
    }
  }

  // �������̊m�ې�����Ƃɖ߂�
  LYTSYSBASE_RETURN_ALLOCATOR(m_base);
  
  return idx;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g���\�[�X�̓ǂݍ��݁i���k�f�[�^�Ή��Łj
 *
 *	@param  devAllocator �f�o�C�X�A���P�[�^
 *	@param  arcFile   �A�[�J�C�u�t�@�C��
 *	@param  dataID    �f�[�^ID
 *	@param	comp			���k�f�[�^�t���O�@true = ���k�f�[�^
 *	@param  useDefaultFont  Default�t�H���g���g�p���邩�H
 *	@param  addTextureNum   �ォ��ǉ�����e�N�X�`����
 *
 *	@retval ���\�[�XID
 *	@retval LTYSYS_RESID_ERROR  ���\�[�X���o�^�ł��Ȃ������B
 */
//-----------------------------------------------------------------------------
LytResID LytSys::LoadLytRes( heap::NwAllocator* devAllocator, fs::ArcFile* arcFile, u32 dataID, bool comp, bool useDefaultFont, u32 addTextureNum )
{
	LytResID	resID;

  // �������̊m�ې��ݒ�
  LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);

	if( comp == false ){
		resID = LoadLytRes( devAllocator, arcFile, dataID, useDefaultFont, addTextureNum );
	}else{
		resID = LoadLytResComp( devAllocator, arcFile, dataID, useDefaultFont, addTextureNum );
	}

  // �������̊m�ې�����Ƃɖ߂�
  LYTSYSBASE_RETURN_ALLOCATOR(m_base);

	return resID;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g���\�[�X�̓ǂݍ��݁i�񈳏k�f�[�^�j
 *
 *	@param  devAllocator �f�o�C�X�A���P�[�^
 *	@param  arcFile   �A�[�J�C�u�t�@�C��
 *	@param  dataID    �f�[�^ID
 *	@param  useDefaultFont  Default�t�H���g���g�p���邩�H
 *	@param  addTextureNum   �ォ��ǉ�����e�N�X�`����
 *
 *	@retval ���\�[�XID
 *	@retval LTYSYS_RESID_ERROR  ���\�[�X���o�^�ł��Ȃ������B
 */
//-----------------------------------------------------------------------------
LytResID LytSys::LoadLytRes( heap::NwAllocator* devAllocator, fs::ArcFile* arcFile, u32 dataID, bool useDefaultFont, u32 addTextureNum )
{
  LytResManager* lytResMan  = m_base->GetResManager();
  LytRes* res;
  LytResID     idx;

  res = lytResMan->GetClearLytRes( &idx );
  if( res == NULL ){
    return LYTSYS_RESID_ERROR;
  }

  // �������̊m�ې��ݒ�
  LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);

  // �ǂݍ��ݏ���
  res->Load( devAllocator, arcFile, dataID, addTextureNum );

  // Default�t�H���g�̐ݒ�
  if( useDefaultFont ){
    for( u32 i=0; i<LYTSYS_DEFAULT_FONT_INDEX_MAX; ++i ){
      if( LytSysBase::IsDefaultFont(i) ){
        res->SetDefaultFont( LytSysBase::GetDefaultFont(i), LytSysBase::GetDefaultFontName(i), i );
      }
    }
  }

  // �������̊m�ې�����Ƃɖ߂�
  LYTSYSBASE_RETURN_ALLOCATOR(m_base);
  
  return idx;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g���\�[�X�̓ǂݍ��݁i���k�f�[�^�j
 *
 *	@param  devAllocator �f�o�C�X�A���P�[�^
 *	@param  arcFile   �A�[�J�C�u�t�@�C��
 *	@param  dataID    �f�[�^ID
 *	@param  useDefaultFont  Default�t�H���g���g�p���邩�H
 *	@param  addTextureNum   �ォ��ǉ�����e�N�X�`����
 *
 *	@retval ���\�[�XID
 *	@retval LTYSYS_RESID_ERROR  ���\�[�X���o�^�ł��Ȃ������B
 */
//-----------------------------------------------------------------------------
LytResID LytSys::LoadLytResComp( heap::NwAllocator* devAllocator, fs::ArcFile* arcFile, u32 dataID, bool useDefaultFont, u32 addTextureNum )
{
  LytResManager* lytResMan  = m_base->GetResManager();
  LytRes* res;
  LytResID     idx;

  res = lytResMan->GetClearLytRes( &idx );
  if( res == NULL ){
    return LYTSYS_RESID_ERROR;
  }

  // �������̊m�ې��ݒ�
  LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);

  // �ǂݍ��ݏ���
  res->LoadComp( devAllocator, arcFile, dataID, addTextureNum );

  // Default�t�H���g�̐ݒ�
  if( useDefaultFont ){
    for( u32 i=0; i<LYTSYS_DEFAULT_FONT_INDEX_MAX; ++i ){
      if( LytSysBase::IsDefaultFont(i) ){
        res->SetDefaultFont( LytSysBase::GetDefaultFont(i), LytSysBase::GetDefaultFontName(i), i );
      }
    }
  }

  // �������̊m�ې�����Ƃɖ߂�
  LYTSYSBASE_RETURN_ALLOCATOR(m_base);
  
  return idx;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g���\�[�X�̔j��
 *
 *	@param  resID     �j�����郊�\�[�XID
 */
//-----------------------------------------------------------------------------
void LytSys::ReleaseLytRes( LytResID resID )
{
  LytResManager* lytResMan  = m_base->GetResManager();
  LytRes* res;

  res = lytResMan->GetIndexLytRes( resID );
  if( res ){
    res->Release();
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  dstID�̃��\�[�X��srcID�̃e�N�X�`�����\�[�X�����ׂĒǉ�����B
 *
 *	@param	srcID     ���L�e�N�X�`���f�[�^�������Ă��郊�\�[�XID
 *	@param	dstID     ���L�e�N�X�`���f�[�^��ǉ����郊�\�[�XID
 *
 * ���ǉ������e�N�X�`���̖�����LoadLytRes�֐���addTextureNum�ɓn���Ċǂ����B
 *	
 */
//-----------------------------------------------------------------------------
void LytSys::RegistTextureRes( LytResID srcID, LytResID dstID, gfl::grp::MemoryArea area )
{
  LytResManager* lytResMan  = m_base->GetResManager();
  const LytRes* srcRes;
  LytRes* dstRes;

  // �������̊m�ې��ݒ�
  LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);

  srcRes = lytResMan->GetIndexLytRes( srcID );
  dstRes = lytResMan->GetIndexLytRes( dstID );
  if( dstRes && srcRes ){
    // �e�N�X�`��Resist
    dstRes->RegistTexture( srcRes, area );
  }

  // �������̊m�ې�����Ƃɖ߂�
  LYTSYSBASE_RETURN_ALLOCATOR(m_base);
}


// �����Ń������z�u���Ȃ������e�N�X�`���́A�f�o�C�X�������ɔz�u����܂��B
//-----------------------------------------------------------------------------
/**
 *  @brief  ���\�[�XID���ɂ���e�N�X�`�������ׂă������ɔz�u����
 *  
 *  @param  resID   ���\�[�XID
 *  @param  area    �]���惁�����@�iFCRAM/VRAMA/VRAMB�j
 */
//-----------------------------------------------------------------------------
void LytSys::TransferAllTexture( LytResID resID, gfl::grp::MemoryArea area )
{
  LytResManager* lytResMan  = m_base->GetResManager();
  LytRes* res;

  // �������̊m�ې��ݒ�
  LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);

  res = lytResMan->GetIndexLytRes( resID );
  if( res ){
    res->TransferAllTexture( area );
  }

  // �������̊m�ې�����Ƃɖ߂�
  LYTSYSBASE_RETURN_ALLOCATOR(m_base);
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���\�[�XID���ɂ���e�N�X�`�������ׂă������ɔz�u����
 *  
 *  @param  resID         ���\�[�XID
 *  @param  texArcIndex   �A�[�J�C�u���̃e�N�X�`���C���f�b�N�X
 *  @param  area          �]���惁�����@�iFCRAM/VRAMA/VRAMB�j
 */
//-----------------------------------------------------------------------------
void LytSys::TransferIndexTexture( LytResID resID, LytArcIndex texArcIndex, gfl::grp::MemoryArea area )
{
  LytResManager* lytResMan  = m_base->GetResManager();
  LytRes* res;

  // �������̊m�ې��ݒ�
  LYTSYSBASE_CHANGE_USE_ALLOCATOR(m_base);

  res = lytResMan->GetIndexLytRes( resID );
  if( res ){
    res->TransferIndexTexture( texArcIndex, area );
  }

  // �������̊m�ې�����Ƃɖ߂�
  LYTSYSBASE_RETURN_ALLOCATOR(m_base);
}


//-----------------------------------------------------------------------------
/**
 *  @brief  ���\�[�XID�̃��\�[�X���擾����B
 *  
 *  @param  resID         ���\�[�XID
 *
 *  @retval ���\�[�XAccessor
 */
//-----------------------------------------------------------------------------
LytRes* LytSys::GetResource( LytResID resID ) const
{
  LytResManager* lytResMan  = m_base->GetResManager();
  LytRes* res;

  res = lytResMan->GetIndexLytRes( resID );
  return res;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �i�[���Ă��郊�\�[�X�̑������`�F�b�N
 *
 *	@retval ���\�[�X�̑���
 */
//-----------------------------------------------------------------------------
u32 LytSys::GetUseResourceNum( void ) const
{
  LytResManager* lytResMan = m_base->GetResManager();
  return lytResMan->GetUseLytResNum();
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g���[�N����
 *
 *	@param  resID       ���C�A�E�g���\�[�XID
 *	@param  lytDataID   resID���̃��C�A�E�g�f�[�^ID
 *	@param  anmResIDTbl �A�j���[�V�����f�[�^ID�e�[�u��      �A�j���̂Ȃ��ꍇ��NULL
 *	@param  anmTblNum   �A�j���[�V�����f�[�^ID�e�[�u����    �A�j���̂Ȃ��ꍇ��0
 *	@param  anmMax      �o�^�A�j���[�V�����ő吔
 *	@param  wkdata      ���C�A�E�g���[�N��{���
 *	@param  anmBind     �A�j���[�V�����̃o�C���h���s���B
 *  @param  createPaneTbl     �y�C���e�[�u������
 *
 *  @retval ���C�A�E�g���[�N
 *  @retval NULL              �������s
 */
//-----------------------------------------------------------------------------
LytWk* LytSys::CreateLytWk( LytResID resID, LytArcIndex lytDataID, const LytArcIndex* anmDataIDTbl, u32 anmTblNum, u32 anmMax, const LytWkSetUp* setup, b32 anmBind, b32 createPaneTbl, b32 createPaneTblWithHash )
{
  return CreateLytWk( m_base->GetHeapAllocator(), m_base->GetDeviceAllocator(), resID, lytDataID, anmDataIDTbl, anmTblNum, anmMax, setup, anmBind, createPaneTbl, createPaneTblWithHash );
}

// Allocator�w���
LytWk* LytSys::CreateLytWk( gfl::heap::NwAllocator* p_heap, gfl::heap::NwAllocator* p_device, LytResID resID, LytArcIndex lytDataID, const LytArcIndex* anmDataIDTbl, u32 anmTblNum, u32 anmMax, const LytWkSetUp* setup, b32 anmBind, b32 createPaneTbl, b32 createPaneTblWithHash )
{
  LytWk* wk = BuildLytWk( p_heap, p_device, resID, lytDataID, anmMax, setup, createPaneTbl, createPaneTblWithHash );

  if( wk != NULL ){
    BuildAnimation( wk, resID, anmDataIDTbl, anmTblNum, anmBind, createPaneTblWithHash );
  }
  return wk;
}


//-----------------------------------------------------------------------------
/**
 * @brief ���C�A�E�g���[�N�����i���C�A�E�g�C���X�^���X�̐������j
 */
//-----------------------------------------------------------------------------
LytWk* LytSys::BuildLytWk( gfl::heap::NwAllocator* p_heap, gfl::heap::NwAllocator* p_device, LytResID resID, LytArcIndex lytDataID, u32 anmMax, const LytWkSetUp* setup, b32 createPaneTbl, b32 createPaneTblWithHash )
{
  LytWk*  wk     = m_base->GetWkManager()->GetClearWk();
  LytRes* lytRes = m_base->GetResManager()->GetIndexLytRes( resID );
  GFL_ASSERT_STOP(lytRes);

  if( wk != NULL )
  {
    LytWkBase* base = wk->GetLytWkBase();

    base->Initialize( p_heap, p_device, lytRes, lytDataID, anmMax, m_base->GetGlobalData(), createPaneTbl, createPaneTblWithHash );

    // setup�̓��e�𔽉f
    {
      base->SetTranslate( setup->pos );
      base->SetScale( setup->scale );
      base->SetRotate( setup->rotate );
      base->SetPriority( setup->priority );
      GFL_ASSERT( setup->groupID != LYTSYS_LYTWK_GROUPID_ALL );
      base->SetWkGroupID( setup->groupID );
    }
  }

  return wk;
}


//-----------------------------------------------------------------------------
/**
 * @brief  ���C�A�E�g���[�N�����i�A�j���[�V�����ݒ�̐������j
 */
//-----------------------------------------------------------------------------
void LytSys::BuildAnimation( LytWk* lytWk, LytResID resID, const LytArcIndex* anmDataIDTbl, u32 anmTblNum, b32 anmBind, b32 createPaneTblWithHash )
{
  // �A�j���[�V�����̐ݒ�
  CreateAnimation( lytWk, resID, anmDataIDTbl, 0, anmTblNum, anmBind, createPaneTblWithHash );

  // ���X�g�ɒǉ�
  AddLinkWithLayoutWork( lytWk );
}


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
void LytSys::CreateAnimation(  LytWk* lytWk, LytResID resID, const LytArcIndex* anmDataIDTbl, u32 anmOffset, u32 anmTblNum, b32 anmBind, b32 createPaneTblWithHash )
{
  LytRes*    lytRes = m_base->GetResManager()->GetIndexLytRes( resID );
  LytWkBase* base   = lytWk->GetLytWkBase();
  GFL_ASSERT_STOP(lytRes);
  GFL_ASSERT_STOP(base);

  for( int i=anmOffset; i<anmTblNum; i++ ){
    base->CreateAnime( i, lytRes, anmDataIDTbl[i], anmBind, createPaneTblWithHash );
  }
}


//----------------------------------------------------------------------------
/**
 * @brief ���C�A�E�g���[�N�����C�A�E�g�}�l�[�W���Ƀ����N������
 *
 * @param lytWk �ǉ��ΏۂƂȂ郌�C�A�E�g���[�N
 */
//----------------------------------------------------------------------------
void LytSys::AddLinkWithLayoutWork( LytWk* lytWk )
{
  m_base->GetWkManager()->AddLink( lytWk );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g���[�N�j��
 *
 *	@param  lytResID  ���C�A�E�g���\�[�XID
 *	@param  anmResID  �A�j���[�V�������\�[�XID
 *	@param  wkdata    ���C�A�E�g���[�N��{���
 *
 */
//-----------------------------------------------------------------------------
void LytSys::RemoveLytWk( LytWk* lytWk )
{
  LytWkManager* lytWkMan = m_base->GetWkManager();
  LytResManager* lytResMan = m_base->GetResManager();
  LytWkBase* base = lytWk->GetLytWkBase();
  
  // ���X�g����폜
  lytWkMan->DelLink( lytWk );

  // �A�j���[�V�����̔j��
  {
    int i;
    u32 anime_max = base->GetAnimeMax();
    for( i=0; i<anime_max; i++ ){
      base->DeleteAnime( i );
    }
  }

  // ���C�A�E�g���[�N�j��
  base->Finalize();
}


//----------------------------------------------------------------------------
/**
 *	@brief  �������ꂽ���C�A�E�g���[�N���̎擾
 */
//-----------------------------------------------------------------------------
u32 LytSys::GetLytWkNum( void ) const
{
  LytWkManager* lytWkMan = m_base->GetWkManager();
  return lytWkMan->GetCreateWkNum();
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g�V�X�e���x�[�X�擾
 */
//-----------------------------------------------------------------------------
LytSysBase* LytSys::GetLytSysBase(void) const
{
  return m_base;
}



//=============================================================================
/**
 *					���C�A�E�g�V�X�e�� ���[�N�N���X�@����
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief    �g�p�\��Ԏ擾
 *
 *	@retval true    �g�p�\
 *	@retval false   �g�p�s�\
 */
//-----------------------------------------------------------------------------
bool LytWk::IsInitialized(void)
{
  return m_base->IsInitialized();
}



//----------------------------------------------------------------------------
/**
 *	@brief    ��Update(AutoDrawFlag��false�̂Ƃ��̂ݗL��)
 */
//-----------------------------------------------------------------------------
void LytWk::Update( void )
{
  if( m_base->IsAutoDraw() == false ){
    m_base->Update();
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    �ʕ`��(AutoDrawFlag��false�̂Ƃ��̂ݗL��)
 *
 *	@param	  drawtype    LYTSYS_DRAW_�`
 *	@param    renderTarget �����_�����O�^�[�Q�b�g�̃t���[���o�b�t�@
 *	@param    rightRenderTarget  �E�ڗp�����_�����O�^�[�Q�b�g�@�iDISPLAY_UPPER�̂Ƃ��̂ݎQ�Ɓj
 */
//-----------------------------------------------------------------------------
void LytWk::Draw( gfl::grp::GraphicsSystem* graphics, DisplayType drawtype, RenderTarget* renderTarget, RenderTarget* rightRenderTarget )
{
  if( m_base->IsAutoDraw() == false ){
    // drawtype���s���Ȃ�`�悵�Ȃ�
    if(drawtype >= DISPLAY_BOTH_MAX){  // Coverity[12721] �͈͊O�Q��
      GFL_ASSERT(drawtype < DISPLAY_BOTH_MAX); //@check
      return ;
    }

    if( graphics->BindCurrentCommandList(renderTarget) )
    {
      renderTarget->ActivateBuffer();
      m_base->Draw( sc_LytSysLodAnmProjIdx[drawtype].lod, sc_LytSysLodAnmProjIdx[drawtype].cameraType, *renderTarget );
    }


    if( (drawtype == DISPLAY_UPPER) && (rightRenderTarget != NULL) ){
      if( graphics->BindCurrentCommandList(rightRenderTarget) )
      {
        rightRenderTarget->ActivateBuffer();
        m_base->Draw( sc_LytSysLodAnmProjIdx[DISPLAY_UPPER_RIGHT].lod, sc_LytSysLodAnmProjIdx[DISPLAY_UPPER_RIGHT].cameraType, *rightRenderTarget );
      }
    }


    //nngxFlush3DCommand();
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  UpperLower�ɑΉ��������[�U�[�`����f�[�^�ݒ�
 *
 *	@param	upperEnv    ���ʗpENV
 *	@param	lowerEnv    ����ʗpENV
 *
 *	*�`���GlobalEnv�ł͂Ȃ�������ENV���g�p����悤�ɂȂ�܂��B
 *	*�߂��ɂ́ASetGlobalEnv()���Ă�ł��������B
 *
 */
//-----------------------------------------------------------------------------
void LytWk::SetUserEnv( const gfl::grp::g2d::Env* upperEnv, const gfl::grp::g2d::Env* lowerEnv )
{
  m_base->SetUserEnv( upperEnv, lowerEnv );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �O���[�o��ENV���g�p����悤�ɐݒ�
 */
//-----------------------------------------------------------------------------
void LytWk::SetGlobalEnv( void )
{
  m_base->SetGlobalEnv();
}


//----------------------------------------------------------------------------
/**
 *	@brief  LytWkGroupID��ݒ肷��
 *
 *	@param	groupID �O���[�vID
 */
//-----------------------------------------------------------------------------
void LytWk::SetLytWkGroupID( LytWKGroupID groupID )
{
  GFL_ASSERT( groupID != LYTSYS_LYTWK_GROUPID_ALL );
  m_base->SetWkGroupID( groupID );
}

//----------------------------------------------------------------------------
/**
 *	@brief  LytWkGroupID���擾����
 *
 *	@return �O���[�vID
 */
//-----------------------------------------------------------------------------
LytWKGroupID LytWk::GetLytWkGroupID( void ) const
{
  return m_base->GetWkGroupID();
}


//----------------------------------------------------------------------------
/**
 *	@brief  �`�悷��f�B�X�v���C�^�C�v��ݒ�
 *
 *	@param	  lod     �`�悷��f�B�X�v���C�^�C�v
 */
//-----------------------------------------------------------------------------
void LytWk::SetDrawDispType( DisplayType lod )
{
  m_base->SetDrawDispType( lod );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �`�悷��f�B�X�v���C�^�C�v�̎擾
 *
 *	@return �`�悷��f�B�X�v���C�^�C�v
 */
//-----------------------------------------------------------------------------
DisplayType LytWk::GetDrawDispType( void ) const
{
  return m_base->GetDrawDispType();
}


//----------------------------------------------------------------------------
/**
 *	@brief  ��\���y�C���̃A�j���[�V�����X�V���ȗ�����t���O
 *	�i�f�t�H���gON�j
 */
//-----------------------------------------------------------------------------
void LytWk::SetVisiblePaneNotAnimeOption( bool flag )
{
  m_base->SetVisiblePaneNotAnimeOption( flag );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��\���y�C���̃A�j���[�V�����X�V���ȗ�����t���O�`�F�b�N
 *	@retval true  �L��
 *	@retval false ����
 */
//-----------------------------------------------------------------------------
bool LytWk::IsVisiblePaneNotAnimeOption( void ) const
{
  return m_base->IsVisiblePaneNotAnimeOption();
}




//============================================================================
/**
 *	�y�C������
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  PaneIndex�̃y�C�����擾
 *
 *	@param	paneIndex GFL_LytSys_ResrouceConv.rb����o�͂����.._pane.h�̃C���f�b�N�X
 *
 *	@retval �y�C��
 *	@retval NULL    Index���s��
 */
//-----------------------------------------------------------------------------
LytPane* LytWk::GetPane( LytPaneIndex paneIndex ) const
{
  return m_base->GetPane( paneIndex );
}


//----------------------------------------------------------------------------
/**
 *	@brief  PaneIndex�̃s�N�`���[�y�C�����擾
 *
 *	@param	paneIndex GFL_LytSys_ResrouceConv.rb����o�͂����.._pane.h�̃C���f�b�N�X
 *
 *	@retval �s�N�`���[�y�C��
 *	@retval NULL Index���s�����APicturePane�łȂ�
 */
//-----------------------------------------------------------------------------
LytPicture* LytWk::GetPicturePane( LytPaneIndex paneIndex ) const
{
  return m_base->GetPicturePane( paneIndex );
}

//----------------------------------------------------------------------------
/**
 *	@brief  PaneIndex�̃e�L�X�g�{�b�N�X�y�C�����擾
 *
 *	@param	paneIndex GFL_LytSys_ResrouceConv.rb����o�͂����.._pane.h�̃C���f�b�N�X
 *
 *	@retval �e�L�X�g�{�b�N�X�y�C��
 *	@retval NULL Index���s�����ATextBoxPane�łȂ�
 */
//-----------------------------------------------------------------------------
LytTextBox* LytWk::GetTextBoxPane( LytPaneIndex paneIndex ) const
{
  return m_base->GetTextBoxPane( paneIndex );
}


//----------------------------------------------------------------------------
/**
 *	@brief  PeneIndex�̃e�L�X�g�{�b�N�X�y�C���̕�����o�b�t�@�̃��������m��
 *
 *	@param	paneIndex     �y�C���C���f�b�N�X
 *	@param  minLen        �m�ە�����
 */
//-----------------------------------------------------------------------------
void LytWk::AllocTextBoxPaneStringBuffer( LytPaneIndex paneIndex, u16 minLen )
{
  m_base->AllocTextBoxPaneStringBuffer( paneIndex, minLen );
}

//----------------------------------------------------------------------------
/**
 *	@brief  PaneIndex��window�y�C�����擾
 *
 *	@param	paneIndex GFL_LytSys_ResrouceConv.rb����o�͂����.._pane.h�̃C���f�b�N�X
 *
 *	@retval window�y�C��
 *	@retval NULL Index���s����windowPane�łȂ�
 */
//-----------------------------------------------------------------------------
LytWindow* LytWk::GetWindowPane( LytPaneIndex paneIndex ) const
{
  return m_base->GetWindowPane( paneIndex );
}

//----------------------------------------------------------------------------
/**
 *	@brief  PaneIndex�̃o�E���f�B���O�{�b�N�X�y�C�����擾
 *
 *	@param	paneIndex GFL_LytSys_ResrouceConv.rb����o�͂����.._pane.h�̃C���f�b�N�X
 *
 *	@retval �o�E���f�B���O�{�b�N�X�y�C��
 *	@retval NULL Index���s����boundingPane�łȂ�
 */
//-----------------------------------------------------------------------------
LytBounding* LytWk::GetBoundingPane( LytPaneIndex paneIndex ) const
{
  return m_base->GetBoundingPane( paneIndex );
}

//----------------------------------------------------------------------------
/**
 *	@brief  PaneIndex�̃}�e���A�����擾
 *
 *	@param	paneIndex GFL_LytSys_ResrouceConv.rb����o�͂����.._pane.h�̃C���f�b�N�X
 *
 *	@retval �}�e���A�����
 *	@retval NULL  Index���s�����}�e���A���ł͂Ȃ�
 */
//-----------------------------------------------------------------------------
LytMaterial* LytWk::GetMaterial( LytPaneIndex paneIndex ) const
{
  return m_base->GetMaterial( paneIndex );
}


//----------------------------------------------------------------------------
/**
 *	@brief  PaneIndex�̃O���[�v�̃y�C�����X�g���擾
 *
 *	@param	groupIndex  �O���[�v�C���f�b�N�X
 *
 *	@return �y�C�����X�g
 */
//-----------------------------------------------------------------------------
LytPaneList* LytWk::GetPaneList( LytPaneIndex groupIndex ) const
{
  nw::lyt::Group* group = m_base->GetGroup( groupIndex );
  if( group ){
    return &group->GetPaneList();
  }
  return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �q�b�g�`�F�b�N���\���擾
 *
 *	@retval true  �\
 *	@retval false �s�\    ��x�`�悷��K�v������܂��B
 */
//-----------------------------------------------------------------------------
bool LytWk::IsHitCheckOK(void) const
{
  return m_base->IsHitCheckOK();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �SPane�q�b�g�`�F�b�N
 *
 *	@param	�X�N���[�����W�n�ł̂��A���i�^�b�`�p�l�����W���̂܂܁j
 *	@param  boundingOnly  �o�E���f�B���O�y�C���̂݃`�F�b�N����:true
 *
 *	@retval �ŏ��Ƀq�b�g�����y�C��
 *	@retval NULL  �q�b�g���Ȃ�
 *
 *	*�q�b�g�`�F�b�N���s���O�ɕK����xDraw����Ă���K�v������܂��B
 *	�@�܂��A1�O�̕`��ʒu�Ńq�b�g�`�F�b�N���s���܂��B
 *
 *	*��]�ɂ͑Ή����Ă���܂���B
 */
//-----------------------------------------------------------------------------
LytPane* LytWk::GetHitPaneAll( s32 x, s32 y, bool boundingOnly ) const
{
  // �q�b�g�`�F�b�N���\���`�F�b�N
  if( m_base->IsHitCheckOK() == false ){
    GFL_ASSERT(0);
    return NULL;
  }
  
  LytPane* rootPane = m_base->GetCore()->GetRootPane();
  gfl::math::VEC2 pos = gfl::math::VEC2(static_cast<float>(x-160), static_cast<float>(-y+120));

  
  nw::lyt::PaneList & paneList = rootPane->GetChildList();
  nw::lyt::PaneList::Iterator it      = paneList.GetBeginIter();
  nw::lyt::PaneList::Iterator it_end  = paneList.GetEndIter();

  
  while( it != it_end ){

    // boundingOnly �`�F�b�N
    if( boundingOnly ){
      // Bounding�łȂ���΃`�F�b�N���Ȃ�
      if( nw::ut::DynamicCast<nw::lyt::Bounding*>(&(*it)) == NULL ){
        it ++;
        continue;
      }
    }

    if( nw::lyt::IsContain( &(*it), pos ) ){
      return &(*it);
    }

    it ++;
  }
  
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �SPane�q�b�g�`�F�b�N
 *
 *	@param	�X�N���[�����W�n�ł̂��A���i�^�b�`�p�l�����W���̂܂܁j
 *	@param  boundingOnly  �o�E���f�B���O�y�C���̂݃`�F�b�N����:true
 *
 *	@retval �ŏ��Ƀq�b�g�����y�C���̃C���f�b�N�X
 *	@retval LYTSYS_PANEINDEX_ERROR  �q�b�g���Ȃ�
 *
 *	*�q�b�g�`�F�b�N���s���O�ɕK����xDraw����Ă���K�v������܂��B
 *	�@�܂��A1�O�̕`��ʒu�Ńq�b�g�`�F�b�N���s���܂��B
 *
 *	*��]�ɂ͑Ή����Ă���܂���B
 */
//-----------------------------------------------------------------------------
LytPaneIndex LytWk::GetHitPaneIndexAll( s32 x, s32 y, bool boundingOnly ) const
{
  const LytPane* pane = GetHitPaneAll(x, y, boundingOnly);
  const LytRes* res = m_base->GetLytRes();
  LytPaneIndex ret = LYTSYS_PANEINDEX_ERROR;
  
  if( pane ){
    ret = res->GetPaneNameIndex( pane->GetName() );
    // pene������A�C���f�b�N�X���擾
    if( ret == LYTRES_PANENAME_ERROR ){
      return LYTSYS_PANEINDEX_ERROR;
    }
  } 
  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  GroupPane�q�b�g�`�F�b�N
 *
 *  @param  Group��Index�@GFL_LytSys_ResrouceConv.rb����o�͂����.._pane.h�̃C���f�b�N�X
 *	@param	�X�N���[�����W�n�ł̂��A���i�^�b�`�p�l�����W���̂܂܁j
 *	@param  boundingOnly  �o�E���f�B���O�y�C���̂݃`�F�b�N����:true
 *
 *	@retval �ŏ��Ƀq�b�g�����y�C��
 *	@retval NULL  �q�b�g���Ȃ�
 *
 *	*�q�b�g�`�F�b�N���s���O�ɕK����xDraw����Ă���K�v������܂��B
 *	�@�܂��A1�O�̕`��ʒu�Ńq�b�g�`�F�b�N���s���܂��B
 *
 *	*��]�ɂ͑Ή����Ă���܂���B
 */
//-----------------------------------------------------------------------------
LytPane* LytWk::GetHitPaneGroup( LytPaneIndex groupIndex, s32 x, s32 y, bool boundingOnly ) const
{
  // �q�b�g�`�F�b�N���\���`�F�b�N
  if( m_base->IsHitCheckOK() == false ){
    GFL_ASSERT(0);
    return NULL;
  }


  gfl::math::VEC2 pos = gfl::math::VEC2(static_cast<float>(x-160), static_cast<float>(-y+120));
  nw::lyt::Layout* layout = m_base->GetCore();
  nw::lyt::GroupContainer* groupContainer = layout->GetGroupContainer();
  nw::lyt::Group* group;
  const LytRes* res = m_base->GetLytRes();
  const char* name;

  name = res->GetPaneName( groupIndex );
  if( name == NULL ){
    return NULL;
  }
  
  group = groupContainer->FindGroupByName( name );
  if( group == NULL ){
    return NULL;
  }


  nw::lyt::PaneLinkList & paneList = group->GetPaneList();
  nw::lyt::PaneLinkList::Iterator it      = paneList.GetBeginIter();
  nw::lyt::PaneLinkList::Iterator it_end  = paneList.GetEndIter();

  
  while( it != it_end ){

    // boundingOnly �`�F�b�N
    if( boundingOnly ){
      // Bounding�łȂ���΃`�F�b�N���Ȃ�
      if( nw::ut::DynamicCast<nw::lyt::Bounding*>(it->target) == NULL ){
        it ++;
        continue;
      }
    }

    if( nw::lyt::IsContain( it->target, pos ) ){
      return it->target;
    }

    it ++;
  }
  
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  GroupPane�q�b�g�`�F�b�N
 *
 *  @param  Group��Index�@GFL_LytSys_ResrouceConv.rb����o�͂����.._pane.h�̃C���f�b�N�X
 *	@param	�X�N���[�����W�n�ł̂��A���i�^�b�`�p�l�����W���̂܂܁j
 *	@param  boundingOnly  �o�E���f�B���O�y�C���̂݃`�F�b�N����:true
 *
 *	@retval �ŏ��Ƀq�b�g�����y�C���̃C���f�b�N�X
 *	@retval LYTSYS_PANEINDEX_ERROR  �q�b�g���Ȃ�
 *
 *	*�q�b�g�`�F�b�N���s���O�ɕK����xDraw����Ă���K�v������܂��B
 *	�@�܂��A1�O�̕`��ʒu�Ńq�b�g�`�F�b�N���s���܂��B
 *
 *	*��]�ɂ͑Ή����Ă���܂���B
 */
//-----------------------------------------------------------------------------
LytPaneIndex LytWk::GetHitPaneIndexGroup( LytPaneIndex groupIndex, s32 x, s32 y, bool boundingOnly ) const
{
  const LytPane* pane = GetHitPaneGroup(groupIndex, x, y, boundingOnly);
  const LytRes* res = m_base->GetLytRes();
  LytPaneIndex ret = LYTSYS_PANEINDEX_ERROR;
  
  if( pane ){
    ret = res->GetPaneNameIndex( pane->GetName() );
    // pene������A�C���f�b�N�X���擾
    if( ret == LYTRES_PANENAME_ERROR ){
      return LYTSYS_PANEINDEX_ERROR;
    }
  }
  return ret;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �P��Pane�Ƃ̃q�b�g�`�F�b�N
 *
 *	@param	�X�N���[�����W�n�ł̂��A���i�^�b�`�p�l�����W���̂܂܁j
 *	@param  �y�C���C���f�b�N�X
 *
 *	@retval true    �q�b�g���Ă���B 
 *	@retval false   �q�b�g���Ă��Ȃ��B
 *
 *	*�q�b�g�`�F�b�N���s���O�ɕK����xDraw����Ă���K�v������܂��B
 *	�@�܂��A1�O�̕`��ʒu�Ńq�b�g�`�F�b�N���s���܂��B
 *
 *	*��]�ɂ͑Ή����Ă���܂���B
 */
//-----------------------------------------------------------------------------
bool LytWk::GetHitPane( s32 x, s32 y, LytPaneIndex index ) const
{
  LytPane* p_pane = m_base->GetPane( index );
  gfl::math::VEC2 pos = gfl::math::VEC2(static_cast<float>(x-160), static_cast<float>(-y+120));

  if( nw::lyt::IsContain( p_pane, pos ) ){
    return true;
  }

  return false;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �}�e���A���̃e�N�X�`���������ւ���B
 *
 *	@param	paneIndex   �}�e���A���̃C���f�b�N�X
 *	@param  mapIndex    �P�}�e���A�����̉��Ԗڂ̃e�N�X�`����ύX���邩�H
 *	@param  cp_res      ���\�[�X���[�N
 *	@param  texArcIndex �e�N�X�`���̃A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void LytWk::SetMaterialImage( LytPaneIndex materialIndex, u32 mapIndex, const LytRes* cp_res, LytArcIndex texArcIndex )
{
  LytMaterial* p_material;

  // �}�e���A���擾
  p_material = this->GetMaterial( materialIndex );

  if( p_material ){

    // �e�N�X�`���C���f�b�N�X�����I�[�o�[
    if( mapIndex < p_material->GetTexMapNum() )
    {
      // ���ւ�
      p_material->SetTexMap( mapIndex, cp_res->GetTexture(texArcIndex) );
    }
    else{
      GFL_ASSERT(0);
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���\�[�X�����擾
 *  
 *  @param  resID         ���\�[�XID
 *
 *  @retval ���\�[�X
 */
//-----------------------------------------------------------------------------
const LytRes* LytWk::GetResource( void ) const
{
  return m_base->GetLytRes();
}





//----------------------------------------------------------------------------
/**
 *	@brief  �I�[�g�`��t���O�@�ݒ�@
 *
 *	@param	flag  �t���O�@true:ON�@false:OFF
 */
//-----------------------------------------------------------------------------
void LytWk::SetAutoDrawFlag( bool flag )
{
  m_base->SetAutoDrawFlag(flag);
}

//----------------------------------------------------------------------------
/**
 *	@brief    �I�[�g�`���ON/OFF �擾
 *  
 *	@retval true    ON
 *	@retval false   OFF
 */
//-----------------------------------------------------------------------------
bool LytWk::IsAutoDraw( void ) const
{
  return m_base->IsAutoDraw();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����D�揇�ʂ̐ݒ�
 *
 *	@param	pri   �D�揇�ʁi 0 > 0xffff )
 *
 *	*�������A�f�v�X�e�X�g�Ȃ��ŁA�`����s���ꍇ�ɂ́A
 *�@�ォ��`�悳�ꂽ���[�N����O�ɕ`�悳��邽�ߒ��ӁB
 *
 *	UpdateAll��DrawAll���s���ۂ̗D�揇�ʂł��B
 */
//-----------------------------------------------------------------------------
void LytWk::SetPriority( u16 pri )
{
  m_base->SetPriority( pri );
  m_parent->SortPriority( this );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����D�揇�ʂ̎擾
 *
 *	@retval	�D�揇�ʁi 0 > 0xffff )
 *
 *	*�������A�f�v�X�e�X�g�Ȃ��ŁA�`����s���ꍇ�ɂ́A
 *�@�ォ��`�悳�ꂽ���[�N����O�ɕ`�悳��邽�ߒ��ӁB
 *
 *	UpdateAll��DrawAll���s���ۂ̗D�揇�ʂł��B
 */
//-----------------------------------------------------------------------------
u16 LytWk::GetPriority( void ) const
{
  return m_base->GetPriority();
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���W�̐ݒ�
 *
 *	@param	pos
 */
//-----------------------------------------------------------------------------
void LytWk::SetTranslate( const gfl::math::VEC3& pos )
{
  m_base->SetTranslate( pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���W�̎擾
 *
 *	@param  p_pos   ���W�i�[��
 */
//-----------------------------------------------------------------------------
void LytWk::GetTranslate( gfl::math::VEC3* p_pos ) const
{
  m_base->GetTranslate(p_pos);
}


//----------------------------------------------------------------------------
/**
 *	@brief  �g��̐ݒ�
 *
 *	@param	scale   �X�P�[�����O�l
 */
//-----------------------------------------------------------------------------
void LytWk::SetScale( const gfl::math::VEC2& scale )
{
  m_base->SetScale( scale );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �g��̎擾
 *
 *	@retval scale �X�P�[�����O�l
 */
//-----------------------------------------------------------------------------
void LytWk::GetScale( gfl::math::VEC2* p_scale ) const
{
  m_base->GetScale( p_scale );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ��]�̐ݒ�
 *
 *	@param	rotate   �e���̉�]�l
 */
//-----------------------------------------------------------------------------
void LytWk::SetRotate( const gfl::math::VEC3& rotate )
{
  m_base->SetRotate( rotate );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��]�̎擾
 *
 *	@retval	rotate   �e���̉�]�l
 */
//-----------------------------------------------------------------------------
void LytWk::GetRotate( gfl::math::VEC3* p_rotate ) const
{
  m_base->GetRotate( p_rotate );
}

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
void LytWk::SetCullingFlag( bool flag )
{
  m_base->SetCullingFlag(flag);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����O�t���O�̎擾
 *
 *	@retval	true  �J�����O����ON
 *	@retval false �J�����O����OFF
 */
//-----------------------------------------------------------------------------
bool LytWk::IsCullingFlag( void ) const
{
  return m_base->IsCullingFlag();
}


//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V�����̒ǉ��o�^
 *
 *	@param	anmResID  �A�j���[�V�������\�[�XID
 *
 *	@retval �o�^Index
 *	@retval LYTSYS_ANIMEINDEX_ERROR �o�^���s
 */
//-----------------------------------------------------------------------------
u32 LytWk::CreateAnime( LytArcIndex anmDataID )
{
  const LytRes* lytRes = m_base->GetLytRes();
  u32 anmIdx;
  
  if( lytRes ){
    anmIdx = m_base->GetClearAnimeIndex();
    if( anmIdx != LYTWKBASE_ANIMEIDX_ERROR ){
      m_base->CreateAnime( anmIdx, lytRes, anmDataID );
      return anmIdx;
    }
  }

  return LYTSYS_ANIMEINDEX_ERROR;
}



//----------------------------------------------------------------------------
/**
 *	@brief  ���L�A�j���[�V�����̓o�^    LytPaneIndex���w�肵�āA�ʓo�^����^�C�v
 *
 *  @param  startAnimeIndex �o�^�J�n�A�j���[�V�����C���f�b�N�X
 *	@param	anmResID        �A�j���[�V�������\�[�XID
 *	@param  maxNum          �o�^�ő吔�i0���Ƃ��ׂēo�^�j
 *	@param  groupIndex      �o�^�O���[�v�C���f�b�N�X�iLYTSYS_PANEINDEX_ERROR���Ƃ��ׂēo�^�j
 *	@param  paneIndex       �o�^�O���[�v���y�C���C���f�b�N�X�iLYTSYS_PANEINDEX_ERROR���Ƃ��ׂēo�^�j
 *
 *	@retval �o�^��  (startAnimeIndex����߂�l���̃C���f�b�N�X�ɃA�j���[�V�������ǉ�����܂����B�j
 *
 *	*�A�j���[�V�������[�N���A���Ŋm�ۂł��Ȃ���ASSERT�łƂ܂�܂��B
 *	*�o�^���Ȃǔc�����Ȃ���g�p���Ă��������B
 *
 *	*���L�A�j���[�V�����Ƃ́A
 *	�@anmResID�̃A�j���[�V�����𕡐��̃y�C�������ʂŎg�p���邱�Ƃ��ł��܂��B
 *	LayoutEditor���ŁA�A�j���[�V�������g�p���邱�Ƃ��ł���y�C�����w�肵�Ă����A
 *	���̊֐��ŃA�j���[�V�������쐬���Ă��������B
 *
 *	���L�A�j���[�V�����Ƃ������O�ł����A�A�j���[�V�����t���[���̊Ǘ��͓Ɨ����Ă��܂��B
 *	startAnimeIndex+�߂�l���̃C���f�b�N�X���w�肵�ăA�j���[�V�����̃t���[���Ǘ����s���Ă��������B
 *
 *	����{�ƂȂ��Ă���A�j���[�V�����͒ʏ��CreateAnime�Ő������Ă��������B
 */
//-----------------------------------------------------------------------------
u32 LytWk::CreateShareAnime( u32 startAnimeIndex, LytArcIndex anmDataID, u32 maxNum, LytPaneIndex groupIndex, LytPaneIndex paneIndex )
{
  const LytRes* lytRes = m_base->GetLytRes();
  
  if( lytRes ){
    return m_base->CreateShareAnime( startAnimeIndex, lytRes, anmDataID, maxNum, groupIndex, paneIndex );
  }
  return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V�����̍폜
 *
 *	@param	anmIdx  �폜����A�j���[�V�����C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void LytWk::DeleteAnime( u32 anmIdx )
{
  m_base->DeleteAnime( anmIdx );
}

//----------------------------------------------------------------------------
/**
 *	@brief    �A�j���[�V������񂪂��邩�`�F�b�N
 *
 *  @param  anmaIdx       �A�j���[�V�����C���f�b�N�X
 */
//-----------------------------------------------------------------------------
bool LytWk::IsAnime( u32 anmIdx ) const
{
  return m_base->IsAnime( anmIdx );
}

//-----------------------------------------------------------------------------
/**
 *  @brief    �A�j���[�V�����̔��f
 *
 *  @param  anmIdx    �A�j���[�V����IDX
 *  @param  is_auto_unbind   one time �A�j���[�V�����̏ꍇ�A�A�j���[�V����������Aunbind����B
 */
//-----------------------------------------------------------------------------
void LytWk::BindAnime( u32 anmIdx, bool is_auto_unbind )
{
  m_base->BindAnime( anmIdx, is_auto_unbind );
}


//-----------------------------------------------------------------------------
/**
 *  @brief    �A�j���[�V�����̔��f����
 *
 *  @param  anmIdx    �o�^��IDX
 */
//-----------------------------------------------------------------------------
void LytWk::UnBindAnime( u32 anmIdx )
{
  m_base->UnBindAnime( anmIdx );
}

//-----------------------------------------------------------------------------
/**
 *  @brief    �S�A�j���[�V�����̔��f����
 */
//-----------------------------------------------------------------------------
void LytWk::UnBindAnimeAll( void )
{
  m_base->UnBindAnimeAll();
}


//-----------------------------------------------------------------------------
/**
 *  @brief    �A�j���[�V�������o�C���h����Ă��邩�`�F�b�N
 *
 *  @retval true    �o�C���h����Ă���B
 *  @retval false   �o�C���h����Ă��Ȃ��B
 */
//-----------------------------------------------------------------------------
bool LytWk::IsBindAnime( u32 anmIdx ) const
{
  return m_base->IsBindAnime( anmIdx );
}

//-----------------------------------------------------------------------------
/**
 *  @brief    �I�[�g�A�j���[�V�����ݒ�
 *
 *  @param  anmIdx    �A�j���[�V����IDX
 *  @param  flag      ON/OFF�t���O
 */
//-----------------------------------------------------------------------------
void LytWk::SetAutoAnime( u32 anmIdx, bool flag )
{
  m_base->SetAutoAnime( anmIdx, flag );
}


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
bool LytWk::IsAutoAnime( u32 anmIdx ) const
{
  return m_base->IsAutoAnime( anmIdx );
}

//-----------------------------------------------------------------------------
/**
 *  @brief    �I�[�g�A�j���[�V���� �X�s�[�h�ݒ�@�i�S�I�[�g�A�j�����ʁj
 *
 *  @param  speed     �A�j���[�V�����X�s�[�h
 */
//-----------------------------------------------------------------------------
void LytWk::SetAutoAnimeSpeed( f32 speed )
{
  m_base->SetAutoAnimeSpeed( speed );
}

//-----------------------------------------------------------------------------
/**
 *  @brief    �I�[�g�A�j���[�V���� �X�s�[�h�ݒ�@�i�ʐݒ�j
 *
 *  @param  anmIdx    �A�j���[�V�����C���f�b�N�X
 *  @param  speed     �A�j���[�V�����X�s�[�h
 */
//-----------------------------------------------------------------------------
void LytWk::SetAutoAnimeSpeed( u32 anmIdx, f32 speed )
{
  m_base->SetAutoAnimeSpeed( anmIdx, speed );
}


//-----------------------------------------------------------------------------
/**
 *  @brief    �A�j���[�V�����t���[���ݒ�
 *
 *  @param  anmIdx    �A�j���[�V����IDX
 *  @param  frame     �A�j���[�V�����t���[��
 */
//-----------------------------------------------------------------------------
void LytWk::SetAnimeFrame( u32 anmIdx, f32 frame )
{
  m_base->SetAnimeFrame( anmIdx, frame );
}

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
bool LytWk::AddAnimeFrame( u32 anmIdx, f32 frame )
{
  return m_base->AddAnimeFrame( anmIdx, frame );
}

//-----------------------------------------------------------------------------
/**
 *  @brief    �A�j���[�V�����t���[���擾
 *
 *  @param  anmIdx    �A�j���[�V����IDX
 */
//-----------------------------------------------------------------------------
f32 LytWk::GetAnimeFrame( u32 anmIdx ) const
{
  return m_base->GetAnimeFrame( anmIdx );
}

//-----------------------------------------------------------------------------
/**
 *  @brief    �A�j���[�V�����ő�t���[���擾
 *
 *  @param  anmIdx    �A�j���[�V����IDX
 */
//-----------------------------------------------------------------------------
f32 LytWk::GetAnimeMaxFrame( u32 anmIdx ) const
{
  return m_base->GetAnimeMaxFrame( anmIdx );
}

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
bool LytWk::IsAnimeLoop( u32 anmIdx ) const
{
  return m_base->IsAnimeLoop(anmIdx);
}

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
bool LytWk::IsAnimeEnd( u32 anmIdx ) const
{
  return m_base->IsAnimeEnd( anmIdx );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g�R�A�V�X�e���@�i�x�[�X���C�u�����̃N���X�j�擾
 *
 *	@return �x�[�X���C�u�����̃N���X
 */
//-----------------------------------------------------------------------------
LytCore* LytWk::GetLayoutCore( void ) const
{
  return m_base->GetCore();
}


//----------------------------------------------------------------------------
/**
 *	@brief    ��\���̃y�C���ɑ΂��čs��v�Z���s�����ǂ�����ݒ肷��
 *
 *	@return		none
 */
//-----------------------------------------------------------------------------
void LytWk::SetInvisiblePaneCalculateMtx( bool flg )
{
	m_base->SetInvisiblePaneCalculateMtx(flg);
}

//----------------------------------------------------------------------------
/**
 *	@brief    �R���o�[�g���ɖ��ߍ��܂ꂽ�t�H���g�����擾����B
 *            ���ۂ͉��L�����[�U�[�f�[�^��g2d_fw�Ƃ��Ė��ߍ��܂ꂽ����
 *
 *	@return		f32 ���B0�̎��͎擾���s
 */
//-----------------------------------------------------------------------------
f32 LytWk::GetFontWidthWhenConvert( LytPaneIndex index )
{
  LytTextBox *textPane = GetTextBoxPane(index);
  
  if( textPane )
  {
    const nw::lyt::ExtUserData *extData = textPane->FindExtUserDataByName("g2d_fw");
    if( extData )
    {
      return extData->GetFloatArray()[0];
    }
  }
  return 0;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^ �f�t�H���g�R���X�g���N�^�[
 */
//-----------------------------------------------------------------------------
LytWk::LytWk(void)
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
LytWk::~LytWk()
{
  Finalize();
}

//----------------------------------------------------------------------------
/**
 *	@brief  ������
 *
 *	@param	allocator     �A���P�[�^
 */
//-----------------------------------------------------------------------------
void LytWk::Initialize( gfl::heap::NwAllocator* allocator, LytWkManager* p_wkman )
{
  heap::HeapBase* heap = allocator->GetHeapBase();

  GFL_ASSERT( m_base == NULL );

  m_base = GFL_NEW(heap) LytWkBase();
  m_parent = p_wkman;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j��
 */
//-----------------------------------------------------------------------------
void LytWk::Finalize( void )
{
  if( m_base ){
    // �x�[�X�j��
    GFL_DELETE m_base;
    m_base = NULL;
    m_parent = NULL;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g���[�N�x�[�X�擾
 *
 *	@retval ���C�A�E�g���[�N�x�[�X
 */
//-----------------------------------------------------------------------------
LytWkBase* LytWk::GetLytWkBase( void ) const
{
  return m_base;
}


//=============================================================================
/**
 *					���C�A�E�g�V�X�e�� �x�[�X�N���X�@����
*/
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief  �V�X�e��������
 *
 *	@param	heapAllocator
 *	@param	deviceAllocator 
 */
//-----------------------------------------------------------------------------
void LytSysBase::InitializeSystem( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator )
{
  if(m_systemInitialized == false){
  
    gfl::heap::HeapBase* heap = heapAllocator->GetHeapBase();

    // Laytou���C�u����������
    nw::lyt::Initialize(heapAllocator, deviceAllocator);
    
    {
      const wchar_t* resourcePath = 0;

      m_globalResource.StartSetup();
      
      for (int i = 0;
          (resourcePath = m_globalResource.GetResourcePath(i)) != NULL;
           ++i)
      {
        void * buff;
        s32 result;
        u32 size;

        GFL_PRINT_EX( GFL_LYTSYS_DEBUGPRINT_LEVEL1, "path = %s addr = %d\n", resourcePath, resourcePath );

        m_RomFile.Open( resourcePath, gfl::fs::OPEN_MODE_READ );
        
        GFL_ASSERT( m_RomFile.GetSize() > 0 );

        // �T�C�Y�擾
        size = static_cast<u32>(m_RomFile.GetSize());

        // �o�b�t�@�쐬 ���̃�������lyt���Ǘ�����B
        buff = nw::lyt::Layout::AllocDeviceMemory( size, 128 );
        GFL_ASSERT_STOP(buff);

        result = m_RomFile.Read( buff, size );
        m_RomFile.Close();

        GFL_PRINT_EX( GFL_LYTSYS_DEBUGPRINT_LEVEL1, "readFile size = %d buff = 0x%x result = %d\n", size, buff, result );


        if( result < 0 ){
          // "can not read\n"
          GFL_ASSERT(0);
        }
        m_globalResource.SetResource(i, buff, size, true);
      }
      m_globalResource.FinishSetup();
    }

    // drawer������
    m_drawer.Initialize(m_globalResource);

    nngxUpdateState(NN_GX_STATE_ALL);

  
    // �V�X�e���̏���������
    m_systemInitialized = true;
    m_lytSysCount       = 0;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �V�X�e���j��
 */
//-----------------------------------------------------------------------------
void LytSysBase::FinalizeSystem( void )
{
  if( m_systemInitialized )
  {
    GFL_ASSERT_STOP( m_lytSysCount == 0 );
    
    // drawer�J��
    m_drawer.Finalize();
    m_globalResource.Finalize();
    // Laytou���C�u���� �A���P�[�^�[���N���A
    nw::lyt::Initialize(NULL, NULL);

    m_systemInitialized = false;
    m_lytSysCount       = 0;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  Default�t�H���g �ݒ�
 *
 *  @param  font                  �t�H���g�f�[�^
 *  @param  name                  ���\�[�X���@(��@simple.bcfnt)�@���C�A�E�g���A�N�Z�X���邽�߂̖���
 */
//-----------------------------------------------------------------------------
void LytSysBase::SetDefaultFont( gfl::str::Font* font, const char* name, u32 index )
{
  if( index < LYTSYS_DEFAULT_FONT_INDEX_MAX ){
    m_defaultFont[index]       = font;
    strcpy( m_defaultFontName[index], name);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  Default�t�H���g�����邩�`�F�b�N
 *
 *	@retval true  ����
 *	@retval false �Ȃ�
 */
//-----------------------------------------------------------------------------
bool LytSysBase::IsDefaultFont(u32 index)
{
  if( index < LYTSYS_DEFAULT_FONT_INDEX_MAX ){
    if( m_defaultFont[index] ){
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  Default�t�H���g�擾
 *
 *	@retval Default�t�H���g
 */
//-----------------------------------------------------------------------------
gfl::str::Font* LytSysBase::GetDefaultFont(u32 index)
{
  if( index < LYTSYS_DEFAULT_FONT_INDEX_MAX ){
    return m_defaultFont[index];
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  Default�t�H���g���擾
 *
 *	@return Default�t�H���g��
 */
//-----------------------------------------------------------------------------
const char* LytSysBase::GetDefaultFontName(u32 index)
{
  if( index < LYTSYS_DEFAULT_FONT_INDEX_MAX ){
    return m_defaultFontName[index];
  }
  return NULL; 
}


//----------------------------------------------------------------------------
/**
 *	@brief  �ǉ����ꂽ�t�H���g��������
 *  @param  �t�H���g���w���Ă���index
 */
//-----------------------------------------------------------------------------
void LytSysBase::ResetDefaultFontInfo(u32 index)
{
  if( index < LYTSYS_DEFAULT_FONT_INDEX_MAX ){
    m_defaultFont[index] = NULL;
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^  �t�@�C���񓯊��ǂݍ���Ver
 *
 *	@param	�Z�b�g�A�b�v���
 */
//-----------------------------------------------------------------------------
LytSysBase::LytSysBase() : m_Initialized(false), m_InitializeSystem(false)
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^
 *
 *	@param	�Z�b�g�A�b�v���
 */
//-----------------------------------------------------------------------------
LytSysBase::LytSysBase( const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator ) : m_Initialized(false), m_InitializeSystem(false)
{
  this->Initialize( setup, heapAllocator, deviceAllocator );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g�V�X�e���x�[�X�������@����Ver
 *
 *	@param	setup
 *	@param	heapAllocator
 *	@param	deviceAllocator 
 */
//-----------------------------------------------------------------------------
void LytSysBase::Initialize( const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator )
{
  gfl::heap::HeapBase* heap = heapAllocator->GetHeapBase();
  
  // �������A���P�[�^�[����
  m_allocator = heapAllocator;
  m_devAllocator = deviceAllocator;

  // �����̎g�������S��
  if(!m_systemInitialized){
    InitializeSystem( m_allocator, m_devAllocator );
    m_InitializeSystem = true;
  }
  
  // �}�l�[�W���̐���
  m_lytResMan   = GFL_NEW(heap) LytResManager( heapAllocator, setup->lyt_resource_max );
  m_wkMan       = GFL_NEW(heap) LytWkManager( heapAllocator, setup->lyt_wk_max );


  // �O���[�o���f�[�^����
  m_globalData = GFL_NEW(heapAllocator->GetHeapBase()) LytWkBaseGlobalData();
  m_globalData->p_globalResource  = &m_globalResource;
  m_globalData->p_drawer          = &m_drawer;

  // ����ʃJ�����ݒ菉���l�ݒ�
  {
    m_globalData->env[DISPLAY_LOWER].SetCameraParam( Env::CAMERA_NORMAL, Env::DefaultProjection[DISPLAY_LOWER], Env::DefaultView[DISPLAY_LOWER] );
  }



  // ����������
  m_Initialized = true;
  
  // �������ꂽ���C�A�E�g�V�X�e���̐��𐔂���B
  ++m_lytSysCount;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g�V�X�e���x�[�X�@�񓯊��ǂݍ��݊J�n
 *
 *	@param	setup
 *	@param	heapAllocator
 *	@param	deviceAllocator 
 */
//-----------------------------------------------------------------------------
void LytSysBase::InitializeAsync( const LytSysSetUp* setup, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator )
{
  // �������A���P�[�^�[����
  m_allocator = heapAllocator;
  m_devAllocator = deviceAllocator;

  // �����̎g�������S��
  if(!m_systemInitialized){
    InitializeSystem( m_allocator, m_devAllocator );
    m_InitializeSystem = true;
  }
  
  // �}�l�[�W���̐���
  m_lytResMan   = GFL_NEW(heapAllocator->GetHeapBase()) LytResManager( heapAllocator, setup->lyt_resource_max );
  m_wkMan       = GFL_NEW(heapAllocator->GetHeapBase()) LytWkManager( heapAllocator, setup->lyt_wk_max );


  // �O���[�o���f�[�^����
  m_globalData = GFL_NEW(heapAllocator->GetHeapBase()) LytWkBaseGlobalData();
  
  m_globalData->p_globalResource  = &m_globalResource;
  m_globalData->p_drawer          = &m_drawer;

}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g�V�X�e���x�[�X�@�񓯊��X�V����
 *
 *	@retval true  �񓯊�����������
 *	@retval false �r��
 */
//-----------------------------------------------------------------------------
bool LytSysBase::InitializeAsyncWait( void )
{
  // �����������`�F�b�N
  if(m_Initialized){
    return true;
  }
  
  // ����ʃJ�����ݒ菉���l�ݒ�
  {
    m_globalData->env[DISPLAY_LOWER].SetCameraParam( Env::CAMERA_NORMAL, Env::DefaultProjection[DISPLAY_LOWER], Env::DefaultView[DISPLAY_LOWER] );
  }


  // ����������
  m_Initialized = true;

  // �������ꂽ���C�A�E�g�V�X�e���̐��𐔂���B
  ++m_lytSysCount;

  return true;
}



//----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
LytSysBase::~LytSysBase()
{

  // �悳�ꂽ���C�A�E�g�V�X�e���̐��𐔂���B
  --m_lytSysCount;

  // �O���[�o���f�[�^�j��
  GFL_DELETE m_globalData;
  
  // �}�l�[�W���j��
  GFL_DELETE m_lytResMan;
  GFL_DELETE m_wkMan;     

  // ���̃V�X�e����InitializeSystem�������̂ŁA
  // FinalizeSystem���s���B
  if( m_InitializeSystem ){
    FinalizeSystem();
    m_InitializeSystem = false;
  }

}


//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g���[�N�����L���Ďg�p���郏�[�N�擾
 *
 *	@return ���L���[�N
 */
//-----------------------------------------------------------------------------
LytWkBaseGlobalData* LytSysBase::GetGlobalData(void)
{
  return m_globalData;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���\�[�X�}�l�[�W���擾
 *
 *	@return ���\�[�X�}�l�[�W��
 */
//-----------------------------------------------------------------------------
LytResManager* LytSysBase::GetResManager(void) const
{
  return m_lytResMan;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[�N�}�l�[�W���擾
 *
 *	@return ���[�N�}�l�[�W��
 */
//-----------------------------------------------------------------------------
LytWkManager* LytSysBase::GetWkManager(void) const
{
  return m_wkMan;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �q�[�v�A���P�[�^�̎擾
 *	@return �q�[�v�A���P�[�^
 */
//-----------------------------------------------------------------------------
gfl::heap::NwAllocator* LytSysBase::GetHeapAllocator(void) const
{
  return m_allocator;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�o�C�X�A���P�[�^�擾
 *
 *	@return �f�o�C�X�A���P�[�^
 */
//-----------------------------------------------------------------------------
gfl::heap::NwAllocator* LytSysBase::GetDeviceAllocator(void) const
{
  return m_devAllocator;
}


//=============================================================================
/**
 *    LytWkManager����
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^
 *
 *  @param  allocator   �A���P�[�^
 *	@param	wk_max      ���[�N�ő吔
 */
//-----------------------------------------------------------------------------
LytWkManager::LytWkManager( heap::NwAllocator* allocator, u32 wk_max ) : 
  m_linkTop()
{
  gfl::heap::HeapBase* heap = allocator->GetHeapBase();
  int i;
  
  m_array    = GFL_NEW(heap) LytWk[wk_max];
  m_arrayNum  = wk_max;
  m_createNum = 0;

  // ������
  for( i=0; i<m_arrayNum; i++ ){
    m_array[i].Initialize( allocator, this );
  }

  m_linkTop.Initialize(allocator, this);

  //  �����N���X�g������
  m_linkTop.GetLytWkBase()->SetPrev( &m_linkTop );
  m_linkTop.GetLytWkBase()->SetNext( &m_linkTop );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
LytWkManager::~LytWkManager()
{
  // LytWk�z��j��
  if( m_array ){
    //�j������
    GFL_DELETE_ARRAY m_array;
    m_array = NULL;
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief �N���A���[�N�̎擾
 *
 *  @retval �N���A���[�N  �����Ȃ��ꍇNULL
 */
//-----------------------------------------------------------------------------
LytWk* LytWkManager::GetClearWk( void ) const
{
  for( int i=0; i<m_arrayNum; i++){
    if( m_array[i].IsInitialized() == false ){
      return &m_array[i];
    }
  }

  // �N���A���[�N�Ȃ�
  GFL_ASSERT(0);
  return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �I�[�g�`�悳���S���[�N���X�V����B
 */
//-----------------------------------------------------------------------------
void LytWkManager::UpdateAll( LytWKGroupID groupID )
{
  LytWk* p_now;
  
  // �SLytWk���X�g��LOOP
  p_now = m_linkTop.GetLytWkBase()->GetNext();
  while( p_now != &m_linkTop ){

    // �I�[�g�`�悳��郏�[�N�Ȃ�΁A�X�V
    if( p_now->GetLytWkBase()->IsAutoDraw() ){
      if( CheckGroupID( groupID, p_now->GetLytWkBase()->GetWkGroupID() ) == true ){
        p_now->GetLytWkBase()->Update();
      }
    }

    // ���̃��[�N
    p_now = p_now->GetLytWkBase()->GetNext();
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �I�[�g�`��ݒ肳��Ă���S���[�N��`��
 */
//-----------------------------------------------------------------------------
void LytWkManager::DrawAll( DisplayType drawtype, RenderTarget* renderTarget, LytWKGroupID groupID )
{
  LytWk* p_now;

  // drawtype���s���Ȃ�`�悵�Ȃ�
  if(drawtype >= DISPLAY_BOTH_MAX){
    GFL_ASSERT(drawtype < DISPLAY_BOTH_MAX); //@check
    return ;
  }

  // �SLytWk���X�g��LOOP
  p_now = m_linkTop.GetLytWkBase()->GetNext();
  while( p_now != &m_linkTop ){

    // �I�[�g�`�悳��郏�[�N�Ȃ�΁A�X�V
    if( p_now->GetLytWkBase()->IsAutoDraw() ){
      
      if( CheckGroupID( groupID, p_now->GetLytWkBase()->GetWkGroupID() ) == true ){

        p_now->GetLytWkBase()->Draw( 
            sc_LytSysLodAnmProjIdx[drawtype].lod, sc_LytSysLodAnmProjIdx[drawtype].cameraType, *renderTarget );
        
      }
    }

    // ���̃��[�N
    p_now = p_now->GetLytWkBase()->GetNext();
  }
  //nngxFlush3DCommand();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����N���X�g�Ƀ��[�N��o�^
 *
 *	@param	wk  ���[�N
 */
//-----------------------------------------------------------------------------
void LytWkManager::AddLink( LytWk* wk )
{
  // ���łɃ����N���͂��ĂȂ����`�F�b�N
  if( wk->GetLytWkBase()->GetPrev() != NULL ){
    GFL_ASSERT( wk->GetLytWkBase()->GetPrev() == NULL ); //@check
    return ;
  }
  if( wk->GetLytWkBase()->GetNext() != NULL ){ 
    GFL_ASSERT( wk->GetLytWkBase()->GetNext() == NULL ); //@check
    return ;
  }

  u16 wk_priority = wk->GetPriority();
  u16 next_priority;
    
  // priority 0>0xffff�@�̏��ɓo�^
  // �擪�̏�񂩃`�F�b�N
  if( m_linkTop.GetLytWkBase()->GetNext() == &m_linkTop )
  {
    // �擪�ɓo�^
    m_linkTop.GetLytWkBase()->SetNext( wk );
    m_linkTop.GetLytWkBase()->SetPrev( wk );

    wk->GetLytWkBase()->SetNext( &m_linkTop );
    wk->GetLytWkBase()->SetPrev( &m_linkTop );
  }
  else
  {
    LytWk* next = m_linkTop.GetLytWkBase()->GetNext();
    LytWk* prev = &m_linkTop;

    // ���܂�Priority�`�F�b�N
    while( next != &m_linkTop ){

      next_priority = next->GetPriority();
      if( wk_priority < next_priority ){
        // �����ɐݒ肷��B
        break;
      }

      prev = next;
      next = next->GetLytWkBase()->GetNext();
    }

    wk->GetLytWkBase()->SetNext( next );
    wk->GetLytWkBase()->SetPrev( next->GetLytWkBase()->GetPrev() );
    prev->GetLytWkBase()->SetNext( wk );
    next->GetLytWkBase()->SetPrev( wk );
  }

  // �������J�E���g
  ++m_createNum;
  
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����N���X�g���烏�[�N���폜
 *
 *	@param	wk  ���[�N
 */
//-----------------------------------------------------------------------------
void LytWkManager::DelLink( LytWk* wk )
{
  // �܂��������̏ꍇ�͉��������Ȃ킸�ɏI���
  if( (wk->GetLytWkBase()->GetPrev() == NULL) && (wk->GetLytWkBase()->GetNext() == NULL) ){
    return;
  }

  // ���ł�link����O��Ă��邩�Alink����ꂽ�B
  if( (wk->GetLytWkBase()->GetNext() == NULL) || (wk->GetLytWkBase()->GetPrev() == NULL) ){
    GFL_ASSERT(0); //@check
    return ;
  }

  // �������J�E���g
  --m_createNum;
  
  LytWk* prev = wk->GetLytWkBase()->GetPrev();
  LytWk* next = wk->GetLytWkBase()->GetNext();
  
  prev->GetLytWkBase()->SetNext( next );
  next->GetLytWkBase()->SetPrev( prev );

  wk->GetLytWkBase()->SetNext( NULL );
  wk->GetLytWkBase()->SetPrev( NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief  Priority�ɂ��\�[�g���s���B
 *
 *	@param  wk  �\�[�g���Ȃ����Ăق������[�N
 */
//-----------------------------------------------------------------------------
void LytWkManager::SortPriority( LytWk* wk )
{
  DelLink(wk);
  AddLink(wk);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �O���[�vID�̈�v�`�F�b�N
 *
 *	@param	groupID_a   �O���[�vID  A
 *	@param	groupID_b   �O���[�vID�@B
 *
 *	@retval true    ��v
 *	@retval false   �s��v
 */
//-----------------------------------------------------------------------------
bool LytWkManager::CheckGroupID( u16 groupID_a, u16 groupID_b ) const
{
  if( (groupID_a == LYTSYS_LYTWK_GROUPID_ALL) ){
    return true;
  }
  if( (groupID_b == LYTSYS_LYTWK_GROUPID_ALL) ){
    return true;
  }
  if( groupID_a == groupID_b ){
    return true;
  }
  return false;
}


//=============================================================================
/**
 *          LytResManager�N���X ����
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief    �R���X�g���N�^
 */
//-----------------------------------------------------------------------------
LytResManager::LytResManager( heap::NwAllocator* allocator, u32 lyt_res_max )
{
  heap::HeapBase* heap = allocator->GetHeapBase();
  
  m_lytArray = GFL_NEW_ARRAY(heap) LytRes[lyt_res_max]();
  m_lytArrayNum = lyt_res_max;
}

//----------------------------------------------------------------------------
/**
 *	@brief    �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
LytResManager::~LytResManager()
{
  // �z��j��
  GFL_DELETE_ARRAY m_lytArray;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �g�p���Ă��Ȃ����\�[�X���[�N���擾
 *
 *	@param	idx ���[�N�̃C���f�b�N�X�\��iNULL���j
 *
 *	@return �g�p���Ă��Ȃ����[�N�|�C���^(�Ȃ��Ƃ�NULL)
 */
//-----------------------------------------------------------------------------
LytRes* LytResManager::GetClearLytRes( LytResID* idx  ) const
{
  // ��̃��[�N������
  for( int i=0; i<m_lytArrayNum; i++ ){
    if( m_lytArray[i].IsDataIn() == false ){
      if( idx != NULL ){ *idx = i; }
      return &m_lytArray[i];
    }
  }

  // ��̃��[�N�Ȃ�
  GFL_ASSERT( 0 );

  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  idx�̃��\�[�X���擾
 *
 *	@param	idx   �C���f�b�N�X
 *
 *	@return ���\�[�X
 */
//-----------------------------------------------------------------------------
LytRes* LytResManager::GetIndexLytRes( LytResID idx ) const
{
  GFL_ASSERT( idx < m_lytArrayNum ); //@check
  if(idx >= m_lytArrayNum){ return NULL; }
  return &m_lytArray[idx];
}

//----------------------------------------------------------------------------
/**
 *	@brief  �g�p���̃��\�[�X�̐���Ԃ�
 *
 *	@return
 */
//-----------------------------------------------------------------------------
u32 LytResManager::GetUseLytResNum( void ) const
{
  u32 count = 0;
  // ��̃��[�N������
  for( int i=0; i<m_lytArrayNum; i++ ){
    if( m_lytArray[i].IsDataIn() ){
      ++count;
    }
  }

  return count;
}


} /* end of namespace g2d */
} /* end of namespace grp */
} /* end of namespace gfl */
