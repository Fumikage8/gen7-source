//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_LytWkBase.cpp
 *	@brief  ���C�A�E�g���[�N�x�[�X
 *	@author	tomoya takahshi
 *	@date		2010.10.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <debug/gfl_Assert.h>
#include <debug/gfl_DebugPrint.h>

#include <grp/g2d/gfl_LytWkBase.h>

#include <grp/g2d/gfl_LytSys.h>
#include <gfl_Math.h>


namespace gfl {
namespace grp {
namespace g2d {


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
  enum{
    CLACTWK_TICK_PRINT_ANIME_CREATE,
    CLACTWK_TICK_PRINT_GET_SHARE_INFO,
    CLACTWK_TICK_PRINT_GET_SHARE_INFO_IN_PANE_NUM,
    CLACTWK_TICK_PRINT_INIT_SHARE,
    CLACTWK_TICK_PRINT_BIND_SHARE,
    CLACTWK_TICK_PRINT_MAX,
  };
  
  GFL_DEBUG_TICK_SETUP(CLACTWK, CLACTWK_TICK_PRINT_MAX );

#if GFL_DEBUG
  
#define CLWKBASE_TICK_DUMP (0)
  
#else

#define CLWKBASE_TICK_DUMP (0)

#endif


//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *    ���[�J���֐�
 */
//-----------------------------------------------------------------------------
//���[�U�[�A���P�[�^�[�ݒ�
#define LYTWKBASE_CHANGE_USER_ALLOCATOR()  \
  nw::os::IAllocator* p_default_allocator_heap = nw::lyt::Layout::GetAllocator();\
  nw::os::IAllocator* p_default_allocator_device = nw::lyt::Layout::GetDeviceMemoryAllocator();\
  nw::lyt::Layout::SetAllocator( m_pUserAllocator );\
  nw::lyt::Layout::SetDeviceMemoryAllocator( m_pUserDeviceAllocator );

// �f�t�H���g�A���P�[�^�[�ɖ߂�
#define LYTWKBASE_RETURN_ALLOCATOR()  \
  nw::lyt::Layout::SetAllocator( p_default_allocator_heap );\
  nw::lyt::Layout::SetDeviceMemoryAllocator( p_default_allocator_device );

  
//----------------------------------------------------------------------------
/**
 *	@brief  �o�C���h�Ώۂ��A�A�j���[�V�����O���[�v�̒��Ɋ܂܂�Ă��邩�`�F�b�N
 *	@retval true  �܂܂�Ă�
 *	@retval false �܂܂�Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
bool IsIncludeAnimationGroupRef(
    const nw::lyt::AnimationGroupRef *const  groupRefs,
    const char* pGroupName,
    u32 bindGroupNum
)
{
  for (u16 grpIdx = 0; grpIdx < bindGroupNum; ++grpIdx)
  {
    // ���L�Ώۂ̃y�C�����o�C���h�w��̃O���[�v�Ɠ������`�F�b�N
    if( gfl::std::StringCmp( pGroupName, groupRefs[grpIdx].GetName() ) == 0 ){
      return true;
    }
  }

  return false;
}


//=============================================================================
/**
 *					LytWkBase����
*/
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^
 */
//-----------------------------------------------------------------------------
LytWkBase::LytWkBase( void ) :
  m_AnimateOpt( nw::lyt::ANIMATEOPT_NOANIMATEINVISIBLE )
{
  GFL_DEBUG_TICK_SET_FLAG(CLACTWK, CLWKBASE_TICK_DUMP );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
LytWkBase::~LytWkBase()
{
  if( m_Initialize ){
    Finalize();
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief    ���C�A�E�g���[�N�@������
 *
 *  @param  heap_allocator        �A���P�[�^�|
 *  @param  device_allocator      �f�o�C�X�A���P�[�^�|
 *	@param  res                   ���\�[�X���
 *	@param  lytDataID             ���C�A�E�g�f�[�^ID
 *	@param  animeNum              �A�j���[�V�����ő�o�^��
 *	@param  globalData            ���L���
 *	@param  createPaneTbl         �y�C���e�[�u������
 *  @param  createPaneTblWithHash ���������p�Ƀn�b�V���L�[�����i����������j
 */
//-----------------------------------------------------------------------------
void LytWkBase::Initialize( heap::NwAllocator* heap_allocator, heap::NwAllocator* device_allocator, const LytRes* res, u32 lytDataID, u32 animeNum,
                            LytWkBaseGlobalData* globalData, b32 createPaneTblFlag, b32 createPaneTblWithHash )
{
  bool result;
  heap::HeapBase* heap = heap_allocator->GetHeapBase();
  heap::HeapBase* device = device_allocator->GetHeapBase();
  void * addr;
  nw::lyt::ArcResourceAccessor* accessor;

  // �g�p����A���P�[�^�[�o�^
  m_pUserAllocator = heap_allocator;
  m_pUserDeviceAllocator = device_allocator;

  // �A���P�[�^���w��̕��ɏ�������
  LYTWKBASE_CHANGE_USER_ALLOCATOR();

  m_globalData = globalData;

  m_LytSys = GFL_NEW(heap) nw::lyt::Layout();
  
  #ifdef GFL_USE_CUSTOM_NW4C
  result = m_LytSys->Build( res->GetResource(lytDataID), res->GetResourceAccessor(), createPaneTblWithHash );
  #else
  result = m_LytSys->Build( res->GetResource(lytDataID), res->GetResourceAccessor() );
  #endif
  GFL_ASSERT_STOP(result);

  m_LytDrawInfo.SetGraphicsResource( m_globalData->p_globalResource );

  if(animeNum>0){
    m_AnimArray = GFL_NEW_ARRAY(heap) LytWkAnime[animeNum];
  }else{
    m_AnimArray = NULL;
  }
  m_AnimMax   = animeNum;
  m_res       = res;

  // �I�[�g�`��ON
  m_AutoDrawFlag = true;

  // GlobalEnv�g�pON
  m_useGlobalEnv = true;

  // �q�b�g�`�F�b�N���\���ǂ���
  m_hitCheckOk = false;

  // ����������
  m_Initialize = true;

  // �J�����OON
  m_CullingFlag = true;

  // �����l�͗���ʕ\��
  m_DisplayType = DISPLAY_BOTH_MAX;

  // �����l�ł̓y�C���e�[�u����NULL
  initializePaneTbl();
  // �y�C���e�[�u������
  if(createPaneTblFlag){ createPaneTbl(heap,createPaneTblWithHash); }

  // �A���P�[�^�����ɖ߂�
  LYTWKBASE_RETURN_ALLOCATOR();
  
}


//----------------------------------------------------------------------------
/**
 *	@brief    ���C�A�E�g���[�N�@�j��
 */
//-----------------------------------------------------------------------------
void LytWkBase::Finalize( void )
{
  int i;
  GFL_ASSERT_STOP(m_LytSys);

  // �y�C���e�[�u���j��
  finalizePaneTbl();

  // �A�j���[�V�����j������
  for( i=0; i<m_AnimMax; i++ ){
    DeleteAnime(i);
  }
  
  if(m_AnimArray){
    GFL_DELETE_ARRAY m_AnimArray;
  }
  m_AnimArray = NULL;
  m_AnimMax   = 0;


  GFL_SAFE_DELETE( m_LytSys );

  m_pUserDeviceAllocator = NULL;
  m_pUserAllocator = NULL;

  // �j������
  m_Initialize = false;
}

  
//----------------------------------------------------------------------------
/**
 *	@brief    ���C�A�E�g���[�N�@�������ς݃`�F�b�N
 *
 *	@retval true    ����������
 *	@retval false   �������O
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsInitialized(void) const
{
  return m_Initialize;
}


//----------------------------------------------------------------------------
/**
 *	@brief    ���C�A�E�g���[�N�@�X�V
 */
//-----------------------------------------------------------------------------
void LytWkBase::Update( void )
{
  // �I�[�g�A�j���[�V�����Ǘ�
  UpdateAutoAnime();
  m_LytSys->Animate( m_AnimateOpt );
}

//----------------------------------------------------------------------------
/**
 *	@brief    ���C�A�E�g���[�N�@�`��
 *
 *	@param  lod           �`���
 *	@param  cameraType    �J�����^�C�v
 *	@param  renderTarget  �����_�[�^�[�Q�b�g
 */
//-----------------------------------------------------------------------------
void LytWkBase::Draw( DisplayType lod, Env::CameraType cameraType, const RenderTarget& renderTarget )
{
  const gfl::grp::g2d::Env* useEnv;

  GFL_ASSERT( lod < DISPLAY_NORMAL_MAX ); //@check

  if( !this->IsDrawDisplayType( lod ) ){
    return ;
  }

  // �g�p������ݒ���擾
  if( m_useGlobalEnv ){
    useEnv = &m_globalData->env[lod];
  }else{
    useEnv = m_UserEnv[lod];
  }

  const Camera& camera = useEnv->GetCamera( cameraType );

  // �J�����ݒ�
  {
    
    m_LytDrawInfo.SetProjectionMtx( camera.GetProjectionMtx() );
    m_LytDrawInfo.SetViewMtx( camera.GetViewMtx() );

    if( lod == DISPLAY_UPPER ){
      const Parallax& parallax = useEnv->GetParallax();
      if( cameraType == Env::CAMERA_NORMAL ){
        m_globalData->p_drawer->SetParallax( -parallax.parallax_half, parallax.dLevel, parallax.addDist );
      }else{
        m_globalData->p_drawer->SetParallax( parallax.parallax_half, parallax.dLevel, parallax.addDist );
      }
    }
    else{
      m_globalData->p_drawer->SetParallax( 0.0f, 0.0f, 0.0f );
    }
  }

  // ���ʂƉ���ʂłP���OK�H
  m_LytSys->CalculateMtx( m_LytDrawInfo );

  // �`��͈͓��`�F�b�N
  if( IsCulling( camera ) ){

    // �`����̏����R�}���h�ݒ�
    useEnv->Add3DCommand( renderTarget );
    
    m_globalData->p_drawer->DrawBegin( m_LytDrawInfo );
    m_globalData->p_drawer->Draw( m_LytSys,  m_LytDrawInfo );
    m_globalData->p_drawer->DrawEnd( m_LytDrawInfo );

    // ��x�`���̓q�b�g�`�F�b�N�\
    m_hitCheckOk = true;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g���[�N�@�O���[�vID�̐ݒ�
 *
 *	@param	groupID   �O���[�vID
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetWkGroupID( u16 groupID )
{
  m_WkGroupID = groupID;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g���[�N  �O���[�vID�̎擾
 *
 *	@return �O���[�vID
 */
//-----------------------------------------------------------------------------
u16 LytWkBase::GetWkGroupID( void ) const
{
  return m_WkGroupID;
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
void LytWkBase::SetUserEnv( const gfl::grp::g2d::Env* upperEnv, const gfl::grp::g2d::Env* lowerEnv )
{
  m_UserEnv[DISPLAY_UPPER] = upperEnv;
  m_UserEnv[DISPLAY_LOWER] = lowerEnv;
  m_useGlobalEnv = false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �O���[�o��ENV���g�p����悤�ɐݒ�
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetGlobalEnv( void )
{
  if( m_useGlobalEnv == false ){
    m_useGlobalEnv = true;
    m_UserEnv[DISPLAY_UPPER] = NULL;
    m_UserEnv[DISPLAY_LOWER] = NULL;
  }
}




//----------------------------------------------------------------------------
/**
 *	@brief  �����N���X�g�@1�O�̃��[�N�ݒ�<LytWk�^>
 *
 *	@param	�P�O�̃��[�N
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetPrev( LytWk* prev )
{
  m_prev = prev;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����N���X�g�@1�O�̃��[�N�擾<LytWk�^>
 *
 *	@retval	�P�O�̃��[�Nreinterpret_cast<LytWk*>�Ƃ��Ďg�p���Ă��������B
 */
//-----------------------------------------------------------------------------
LytWk* LytWkBase::GetPrev( void ) const
{
  return m_prev;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����N���X�g�@1���̃��[�N�ݒ�<LytWk�^>
 *
 *	@param	�P���̃��[�N
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetNext( LytWk* next )
{
  m_next = next;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����N���X�g�@1���̃��[�N�擾<LytWk�^>
 *
 *	@retval	�P���̃��[�Nreinterpret_cast<LytWk*>�Ƃ��Ďg�p���Ă��������B
 */
//-----------------------------------------------------------------------------
LytWk* LytWkBase::GetNext( void ) const
{
  return m_next;
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
nw::lyt::Pane* LytWkBase::GetPane( u32 paneIndex ) const
{
  nw::lyt::Pane* p_pane;

  // �܂��́A�y�C���e�[�u������擾
  p_pane = getPaneTblPane( paneIndex );
  if(p_pane){ return p_pane; }
  
  // ������΁A������A
  const char * paneName = m_res->GetPaneName(paneIndex);
  if( paneName )
  {
    // 4�����ȏ�̃��C�A�E�g������̂ł����͐��K�̖��O�ł̌����K�{
    p_pane = m_LytSys->GetRootPane()->FindPaneByName( paneName, true );

    if( m_ppPaneTbl != NULL ){
      m_ppPaneTbl[paneIndex] = p_pane;
    }
  }
  return p_pane;
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
nw::lyt::Picture* LytWkBase::GetPicturePane( u32 paneIndex ) const
{
  nw::lyt::Pane* pane;
  nw::lyt::Picture* picture;

  pane = GetPane(paneIndex);
  if( pane ){
    picture = nw::ut::DynamicCast<nw::lyt::Picture*>(pane);
    return picture;  
  }
  return NULL;
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
nw::lyt::TextBox* LytWkBase::GetTextBoxPane( u32 paneIndex ) const
{
  nw::lyt::Pane* pane;
  nw::lyt::TextBox* textBox;

  pane = GetPane(paneIndex);
  if( pane ){
    textBox = nw::ut::DynamicCast<nw::lyt::TextBox*>(pane);
    return textBox;  
  }
  return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  PaneIndex�̃e�L�X�g�{�b�N�X�y�C���@�X�g�����O�o�b�t�@�̊m��
 *
 *	@param	paneIndex
 */
//-----------------------------------------------------------------------------
void LytWkBase::AllocTextBoxPaneStringBuffer( u16 paneIndex, u16 minLen ) const
{
  nw::lyt::TextBox* pane = this->GetTextBoxPane( paneIndex );
  
  if( pane ){ // @fix KW[7]

    // �A���P�[�^���w��̕��ɏ�������
    LYTWKBASE_CHANGE_USER_ALLOCATOR();
    
    pane->AllocStringBuffer( minLen );

    // �A���P�[�^�����ɖ߂�
    LYTWKBASE_RETURN_ALLOCATOR();
  }
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
nw::lyt::Window* LytWkBase::GetWindowPane( u32 paneIndex ) const
{
  nw::lyt::Pane* pane;
  nw::lyt::Window* window;

  pane = GetPane(paneIndex);
  if( pane ){
    window = nw::ut::DynamicCast<nw::lyt::Window*>(pane);
    return window;  
  }
  return NULL;
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
nw::lyt::Bounding* LytWkBase::GetBoundingPane( u32 paneIndex ) const
{
  nw::lyt::Pane* pane;
  nw::lyt::Bounding* bounding;

  pane = GetPane(paneIndex);
  if( pane ){
    bounding = nw::ut::DynamicCast<nw::lyt::Bounding*>(pane);
    return bounding;  
  }
  return NULL;
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
nw::lyt::Material* LytWkBase::GetMaterial( u32 paneIndex ) const
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  const char * paneName = m_res->GetPaneName(paneIndex);
  if( paneName ){
    return rootPane->FindMaterialByName( paneName, true );
  }
  return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �q�b�g�`�F�b�N���\���擾
 *
 *	@retval true    �\
 *	@retval false   �s�\
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsHitCheckOK( void ) const
{
  return m_hitCheckOk;
}


//============================================================================
/**
 *	��{����
 */
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief    �I�[�g�`���ON/OFF�ݒ�
 *  
 *	@param	  flag    �t���O  true:ON   false:OFF
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetAutoDrawFlag( bool flag )
{
  m_AutoDrawFlag = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief    �I�[�g�`���ON/OFF �擾
 *  
 *	@retval true    ON
 *	@retval false   OFF
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsAutoDraw( void ) const
{
  return m_AutoDrawFlag;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����D�揇�ʂ̐ݒ�
 *
 *	@param	pri   �D�揇�ʁi 0 > 0xffff )
 *
 *	UpdateAll��DrawAll���s���ۂ̗D�揇�ʂł��B
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetPriority( u16 pri )
{
  m_Priority = pri;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �����D�揇�ʂ̎擾
 *
 *	@retval	�D�揇�ʁi 0 > 0xffff )
 *
 *	UpdateAll��DrawAll���s���ۂ̗D�揇�ʂł��B
 */
//-----------------------------------------------------------------------------
u16 LytWkBase::GetPriority( void ) const
{
  return m_Priority;
}





//----------------------------------------------------------------------------
/**
 *	@brief  ���W�̐ݒ�
 *
 *	@param	pos
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetTranslate( const gfl::math::VEC3& pos )
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  rootPane->SetTranslate( pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���W�̎擾
 *
 *	@param	p_pos ���W�i�[��
 */
//-----------------------------------------------------------------------------
void LytWkBase::GetTranslate( gfl::math::VEC3* p_pos ) const
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  *p_pos = rootPane->GetTranslate();
}


//----------------------------------------------------------------------------
/**
 *	@brief  �g��̐ݒ�
 *
 *	@param	scale   �X�P�[�����O�l
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetScale( const gfl::math::VEC2& scale )
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  rootPane->SetScale( scale );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �g��̎擾
 *
 *	@param p_scale �X�P�[�����O�l�i�[��
 */
//-----------------------------------------------------------------------------
void LytWkBase::GetScale( gfl::math::VEC2* p_scale ) const
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  *p_scale = rootPane->GetScale();
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��]�̐ݒ�
 *
 *	@param	rotate   �e���̉�]�l
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetRotate( const gfl::math::VEC3& rotate )
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  rootPane->SetRotate( rotate );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��]�̎擾
 *
 *	@param	p_rotate   �e���̉�]�l
 */
//-----------------------------------------------------------------------------
void LytWkBase::GetRotate( gfl::math::VEC3* p_rotate ) const
{
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  *p_rotate = rootPane->GetRotate();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����O�t���O�̐ݒ�
 *
 *	@param	flag  �t���O
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetCullingFlag( bool flag )
{
  m_CullingFlag = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����O�t���O�̎擾
 *
 *	@retval	true  �J�����O����ON
 *	@retval false �J�����O����OFF
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsCullingFlag( void ) const
{
  return m_CullingFlag;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �`�悷��f�B�X�v���C�^�C�v��ݒ�
 *
 *	@param	lod   �`�悷��f�B�X�v���C�^�C�v
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetDrawDispType( DisplayType lod )
{
  if( (lod == DISPLAY_UPPER) || (lod == DISPLAY_LOWER) || (lod == DISPLAY_BOTH_MAX) ){
    m_DisplayType = lod;
  }
  else{
    GFL_ASSERT_MSG( 0, "DisplayType Error" );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �`�悷��f�B�X�v���C�^�C�v�̎擾
 *
 *	@return �`�悷��f�B�X�v���C�^�C�v
 */
//-----------------------------------------------------------------------------
DisplayType LytWkBase::GetDrawDispType( void ) const
{
  return m_DisplayType;
}


//============================================================================
/**
 *	�A�j���[�V�����֘A
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �󂢂Ă���A�j���[�V�����C���f�b�N�X���擾
 *	
 *	@retval �󂢂Ă���A�j���[�V�����C���f�b�N�X
 *	@retval LYTWKBASE_ANIMEIDX_ERROR    �󂢂ĂȂ�
 */
//-----------------------------------------------------------------------------
u32 LytWkBase::GetClearAnimeIndex( void ) const
{
  for( int i=0; i<m_AnimMax; i++ ){
    if( m_AnimArray[i].IsInitialized() == false ){
      return i;
    }
  }
  
  return LYTWKBASE_ANIMEIDX_ERROR;
}

//----------------------------------------------------------------------------
/**
 *	@brief    �A�j���[�V�������̐���
 *
 *  @param  allocator     �q�[�v�A���P�[�^
 *  @param  anmaIdx       ������A�j���[�V�����C���f�b�N�X
 *	@param  res           ���\�[�X���
 *	@param  lytDataID     �A�j���[�V�������\�[�X�̃f�[�^ID
 *	@param  is_bind       �o�C���h��ԂŐݒ肷�邩
 */
//-----------------------------------------------------------------------------
void LytWkBase::CreateAnime( u32 anmIdx, const LytRes* res, u32 lytDataID, bool is_bind, bool is_useHash )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return ;
  }
  GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE, "CSA" );

  // �A���P�[�^���w��̕��ɏ�������
  LYTWKBASE_CHANGE_USER_ALLOCATOR();
  
  // ���łɃf�[�^������ꍇ�@�j������
  if( m_AnimArray[anmIdx].IsInitialized() ){
    // �j������
    DeleteAnime( anmIdx );
  }

  // ���C�A�E�g�̃A���P�[�^���g�p���ăA�j���[�V����Transform�𐶐�
  nw::lyt::AnimTransform* animewk = m_LytSys->CreateAnimTransform();

  // ������
  GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_INIT_SHARE, "INIT_SHARE" );
  m_AnimArray[anmIdx].Initialize( m_LytSys, animewk, res, lytDataID, is_useHash, m_LytSys );
  GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_INIT_SHARE );
#if CLWKBASE_TICK_DUMP 
  GFL_PRINT( "CreateAnime Init %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_INIT_SHARE ) );
#endif // CLWKBASE_TICK_DUMP 

  // �o�C���h
  GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_BIND_SHARE, "BIND" );
  m_AnimArray[anmIdx].Bind( m_LytSys, is_bind, is_useHash );
  GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_BIND_SHARE );
#if CLWKBASE_TICK_DUMP 
  GFL_PRINT( "CreateAnime Bind %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_BIND_SHARE ) );
#endif // CLWKBASE_TICK_DUMP 

  // �A���P�[�^�[�����ɖ߂�
  LYTWKBASE_RETURN_ALLOCATOR();

  GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE );
#if CLWKBASE_TICK_DUMP 
  GFL_PRINT( "CreateAnime %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE ) );
#endif // CLWKBASE_TICK_DUMP 
}
  
//----------------------------------------------------------------------------
/**
 *	@brief    ���L�A�j���[�V�����̐���
 *
 *	@param	  anmStartIdx �����J�n�A�j���[�V�����C���f�b�N�X
 *	@param    res         ���\�[�X���
 *	@param    lytDataID   �A�j���[�V�������\�[�X�̃f�[�^ID
 *	@param    maxNum      �o�^�ő吔�i�O�ł���΁A���ׂēo�^�j
 *	@param    maxNum      �o�^�ő吔�i0���Ƃ��ׂēo�^�j
 *	@param    groupIndex  �o�^�O���[�v�C���f�b�N�X�iLYTSYS_PANEINDEX_ERROR���Ƃ��ׂēo�^�j
 *	@param    paneIndex   �o�^�O���[�v���y�C���C���f�b�N�X�iLYTSYS_PANEINDEX_ERROR���Ƃ��ׂēo�^�j
 *
 *	@return �o�^��
 */
//-----------------------------------------------------------------------------
u32 LytWkBase::CreateShareAnime( u32 anmStartIdx, const LytRes* res, u32 lytDataID, u32 maxNum, u32 groupIndex, u32 paneIndex )
{
  u32 shareGroup;
  u32 total_create = 0;
  nw::lyt::Pane* pSrcPane = NULL;
  nw::lyt::Group* pGroup = NULL;
  const char * pGroupName = NULL;
  bool is_pane_end = false;

  GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE, "CSA" );

  // �V�F�A�y�C�����擾
  GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_GET_SHARE_INFO, "SI" );
  void * p_animRes = res->GetAnimeResource( lytDataID );
  nw::lyt::AnimResource animRes( p_animRes );
  shareGroup = animRes.GetAnimationShareInfoNum();
  GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_GET_SHARE_INFO );
#if CLWKBASE_TICK_DUMP 
  GFL_PRINT( "ResShareInfoNum %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_GET_SHARE_INFO ) );
#endif 


  for(int i=0; i<shareGroup; i++){

    // �V�F�A�O���[�v���̃y�C�����擾
    GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_GET_SHARE_INFO_IN_PANE_NUM, "SIIP" );
    bool isAnimeOk = LytWkAnime::GetResShareInfoInPaneNum( m_LytSys, animRes, res, i, &pSrcPane, &pGroup, &pGroupName, groupIndex );
    GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_GET_SHARE_INFO_IN_PANE_NUM );
#if CLWKBASE_TICK_DUMP 
    GFL_PRINT( "GetResShareInfoInPaneNum %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_GET_SHARE_INFO_IN_PANE_NUM ) );
#endif // CLWKBASE_TICK_DUMP 


    if(isAnimeOk)
    {
      is_pane_end = false;

      nw::lyt::PaneLinkList& paneList = pGroup->GetPaneList();

      for (nw::lyt::PaneLinkList::Iterator it = paneList.GetBeginIter(); it != paneList.GetEndIter(); ++it)
      {
        // GroupIndex��PaneIndex�̎w�肪���邩�H
        if( (groupIndex != LYTSYS_PANEINDEX_ERROR) && (paneIndex != LYTSYS_PANEINDEX_ERROR) ){
          if( gfl::std::StringCmp( it->target->GetName(), res->GetPaneName( paneIndex ) ) != 0 ){
            continue;
          }
          is_pane_end = true;
        }

        // �I�[�o�[�`�F�b�N
        if( m_AnimMax > anmStartIdx + total_create ){

          // �A���̈�ł���K�v������B
          if( m_AnimArray[anmStartIdx + total_create].IsInitialized() ){
            GFL_ASSERT(0);
            return total_create;
          }

          // �A���P�[�^���w��̕��ɏ�������
          LYTWKBASE_CHANGE_USER_ALLOCATOR();

          
          // ����
          GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_INIT_SHARE, "IS" );
          bool result = m_AnimArray[ anmStartIdx + total_create ].InitializeShare( m_LytSys, res, p_animRes, i, it, pSrcPane, pGroup );
          GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_INIT_SHARE );
#if CLWKBASE_TICK_DUMP 
          GFL_PRINT( "InitializeShare %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_INIT_SHARE ) );
#endif // CLWKBASE_TICK_DUMP 

          if( result ){
            // �o�C���h
            GFL_DEBUG_TICK_START( CLACTWK, CLACTWK_TICK_PRINT_BIND_SHARE, "IS" );
            m_AnimArray[ anmStartIdx + total_create ].BindShare( m_LytSys, res, pSrcPane );
            GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_BIND_SHARE );
#if CLWKBASE_TICK_DUMP 
            GFL_PRINT( "BindShare %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_BIND_SHARE ) );
#endif // CLWKBASE_TICK_DUMP 
            
            total_create ++;

            // ���������ő�ɂȂ�����I���
            if( (maxNum > 0) && (total_create >= maxNum) ){
              // �A���P�[�^�[�����ɖ߂��B
              LYTWKBASE_RETURN_ALLOCATOR();

              GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE );
#if CLWKBASE_TICK_DUMP 
              GFL_PRINT( "CreateShareAnime %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE ) );
#endif 

              return total_create;
            }
          }
          // �A���P�[�^�[�����ɖ߂��B
          LYTWKBASE_RETURN_ALLOCATOR();
        }else{
          // ���[�N�I�[�o�[
          GFL_ASSERT( 0 );
          return total_create;
        }

        // �w��y�C���@�I��
        if( is_pane_end ){
          break;
        }
      }
    }
  }

  GFL_DEBUG_TICK_END( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE );
#if CLWKBASE_TICK_DUMP 
  GFL_PRINT( "CreateShareAnime %d us\n", GFL_DEBUG_TICK_US_GET( CLACTWK, CLACTWK_TICK_PRINT_ANIME_CREATE ) );
#endif 

  return total_create;
}

//----------------------------------------------------------------------------
/**
 *	@brief    �A�j���[�V�������̔j��
 *
 *  @param  anmaIdx       �j������A�j���[�V�����C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void LytWkBase::DeleteAnime( u32 anmIdx )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return ;
  }
  
  if( m_AnimArray[anmIdx].IsInitialized() ){
    if( m_AnimArray[anmIdx].IsShareAnime() == false ){
      // �o�C���h�̉���
      m_AnimArray[anmIdx].UnBind( m_LytSys );
      // ���C�A�E�g�̃A���P�[�^���g�p���ăA�j���[�V����Transform��j��
      m_LytSys->DeleteAnimTransform( m_AnimArray[anmIdx].GetAnimeWork() );
    }else{
      // �o�C���h�̉���
      m_AnimArray[anmIdx].UnBindShare( m_LytSys );
    }

    // �j��
    m_AnimArray[anmIdx].Finalize();
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    �A�j���[�V�����C���f�b�N�X�ő�l�擾
 *
 *  @retval   �A�j���[�V�����C���f�b�N�X�ő�l
 */
//-----------------------------------------------------------------------------
u32 LytWkBase::GetAnimeMax( void ) const
{
  return m_AnimMax;
}


//----------------------------------------------------------------------------
/**
 *	@brief    �A�j���[�V������񂪂��邩�`�F�b�N
 *
 *  @param  anmaIdx       �A�j���[�V�����C���f�b�N�X
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsAnime( u32 anmIdx ) const
{
  if( anmIdx < m_AnimMax ){
    return m_AnimArray[ anmIdx ].IsInitialized();
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    �A�j���[�V�����̔��f
 *
 *  @param  anmIdx    �A�j���[�V����IDX
 *  @param  is_auto_unbind   one time �A�j���[�V�����̏ꍇ�A�A�j���[�V����������Aunbind����B
 */
//-----------------------------------------------------------------------------
void LytWkBase::BindAnime( u32 anmIdx, bool is_auto_unbind )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return ;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
    m_AnimArray[anmIdx].SetEnable( m_LytSys, true );
    m_AnimArray[anmIdx].SetAutoUnbind(is_auto_unbind);
    this->SetAnimeFrame( anmIdx, 0 );
  }

}

//-----------------------------------------------------------------------------
/**
 *  @brief    �A�j���[�V�����̔��f����
 *
 *  @param  anmIdx    �o�^��IDX
 */
//-----------------------------------------------------------------------------
void LytWkBase::UnBindAnime( u32 anmIdx )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return ;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
    m_AnimArray[anmIdx].SetEnable( m_LytSys, false );
    this->SetAnimeFrame( anmIdx, 0 );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief    �A�j���[�V�����̔��f����
 */
//-----------------------------------------------------------------------------
void LytWkBase::UnBindAnimeAll( void )
{
  u32 i;
  
  for( i=0; i<m_AnimMax; ++i ){
    UnBindAnime(i);
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief    �A�j���[�V�������o�C���h����Ă��邩�`�F�b�N
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsBindAnime( u32 anmIdx ) const
{
  if( anmIdx >= m_AnimMax ){
    return false;
  }
  
  if( m_AnimArray[anmIdx].IsInitialized() ){
    if( m_AnimArray[anmIdx].IsEnable() ){
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    �I�[�g�A�j���[�V�����ݒ�
 *
 *  @param  anmIdx    �A�j���[�V����IDX
 *  @param  flag      ON/OFF�t���O
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetAutoAnime( u32 anmIdx, bool flag )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return ;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
    m_AnimArray[anmIdx].SetAutoAnime( flag );
  }
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
bool LytWkBase::IsAutoAnime( u32 anmIdx ) const
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return false;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
    return m_AnimArray[anmIdx].IsAutoAnime();
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    �I�[�g�A�j���[�V���� �X�s�[�h�ݒ�@�i�S�I�[�g�A�j�����ʁj
 *
 *  @param  speed     �A�j���[�V�����X�s�[�h
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetAutoAnimeSpeed( f32 speed )
{
  // �S�A�j���[�V�����̃I�[�g�A�j���[�V�����X�s�[�h��ݒ�
  for( int i=0; i<m_AnimMax; i++ ){
    if( m_AnimArray[i].IsInitialized() ){
      m_AnimArray[i].SetAutoAnimeSpeed( speed );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief    �I�[�g�A�j���[�V���� �X�s�[�h�ݒ�@�i�ʐݒ�j
 *
 *  @param  anmIdx    �A�j���[�V�����C���f�b�N�X
 *  @param  speed     �A�j���[�V�����X�s�[�h
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetAutoAnimeSpeed( u32 anmIdx, f32 speed )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return ;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
    m_AnimArray[anmIdx].SetAutoAnimeSpeed( speed );
  }
}


//-----------------------------------------------------------------------------
/**
 *  @brief    �A�j���[�V�����t���[���ݒ�
 *
 *  @param  anmIdx    �A�j���[�V����IDX
 *  @param  frame     �A�j���[�V�����t���[��
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetAnimeFrame( u32 anmIdx, f32 frame )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return ;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
    m_AnimArray[anmIdx].GetAnimeWork()->SetFrame( frame );
  }
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
bool LytWkBase::AddAnimeFrame( u32 anmIdx, f32 frame )
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return true;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
    return m_AnimArray[anmIdx].AddAnimeFrame( frame );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    �A�j���[�V�����t���[���擾
 *
 *  @param  anmIdx    �A�j���[�V����IDX
 */
//-----------------------------------------------------------------------------
f32 LytWkBase::GetAnimeFrame( u32 anmIdx ) const
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return 0.0f;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
    return m_AnimArray[anmIdx].GetAnimeWork()->GetFrame();
  }
  return 0.0f;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    �A�j���[�V�����ő�t���[���擾
 *
 *  @param  anmIdx    �A�j���[�V����IDX
 */
//-----------------------------------------------------------------------------
f32 LytWkBase::GetAnimeMaxFrame( u32 anmIdx ) const
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return 0;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
    return m_AnimArray[anmIdx].GetAnimeWork()->GetFrameMax();
  }
  return 0;
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
bool LytWkBase::IsAnimeLoop( u32 anmIdx ) const
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return false;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
    return m_AnimArray[anmIdx].GetAnimeWork()->IsLoopData();
  }
  return false;
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
bool LytWkBase::IsAnimeEnd( u32 anmIdx ) const
{
  if( !(anmIdx < m_AnimMax) )
  {
    GFL_ASSERT(0);
    return true;
  }

  if( m_AnimArray[anmIdx].IsInitialized() ){
    return m_AnimArray[anmIdx].IsAnimeEnd();
  }
  return true;
}



//----------------------------------------------------------------------------
/**
 *	@brief  ��\���y�C���̃A�j���[�V�����X�V���ȗ�����t���O
 *	�i�f�t�H���gON�j
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetVisiblePaneNotAnimeOption( bool flag )
{
  if( flag ){
    m_AnimateOpt = nw::lyt::ANIMATEOPT_NOANIMATEINVISIBLE;
  }
  else{
    m_AnimateOpt = static_cast<nw::lyt::AnimateOpt>(0);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��\���y�C���̃A�j���[�V�����X�V���ȗ�����t���O�`�F�b�N
 *	@retval true  �L��
 *	@retval false ����
 */
//-----------------------------------------------------------------------------
bool LytWkBase::IsVisiblePaneNotAnimeOption( void ) const
{
  if( m_AnimateOpt == 0 ){
    return false;
  }
  return true;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���͈͓��`�F�b�N
 *
 *	@param	camera  �J�������[�N
 *
 *	@retval true    ���͈͓�
 *	@retval false   ���͈͊O
 */
//-----------------------------------------------------------------------------
b32 LytWkBase::IsCulling( const Camera& camera ) const
{
  if( !m_CullingFlag ){
    return true;
  }
  
  nw::ut::Rect rect = m_LytSys->GetLayoutRect();
  nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();
  gfl::math::MTX34 matrix_tmp = rootPane->GetGlobalMtx();
  gfl::math::MTX44 screen_a = camera.GetProjectionMtx();
  gfl::math::MTX34 screen_b;
  gfl::math::MTX34 matrix;
  f32 w;

  
  gfl::math::VEC3 check;
  gfl::math::VEC3 ans;

  //MTX44->MTX34
  screen_b._00 = screen_a._00;
  screen_b._10 = screen_a._10;
  screen_b._20 = screen_a._20;
  screen_b._01 = screen_a._01;
  screen_b._11 = screen_a._11;
  screen_b._21 = screen_a._21;
  screen_b._02 = screen_a._02;
  screen_b._12 = screen_a._12;
  screen_b._22 = screen_a._22;
  screen_b._03 = screen_a._03;
  screen_b._13 = screen_a._13;
  screen_b._23 = screen_a._23;


  // 9�_�`�F�b�N ����1�ł��͈͓��Ȃ�`��B
  for( u32 i = 0; i<9; ++i ){
    GetRectCheckPos( rect, i, &check );

    // �X�N���[�����W�n�܂Ŏ����Ă����B
    gfl::math::VEC3Transform(&check, &matrix_tmp, &check);

    // �X�N���[���s��ւ̕ϊ��ł́A���E�c���u�������B
    ans.x = check.y;
    ans.y = check.x;
    ans.z = check.z;
    gfl::math::VEC3Transform(&ans, &screen_b, &check);
    w = (check.x * screen_a._30) + (check.y * screen_a._31) + (check.z * screen_a._32) + screen_a._33;
    ans.x /= w;
    ans.y /= w;

    if( (gfl::math::Abs(ans.x) <= 1.0f) && (gfl::math::Abs(ans.y) <= 1.0f) ){
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��`�ʒu�����擾
 *
 *	@param	rect    ��`���
 *	@param	p_pos   �ʒu���
 */
//-----------------------------------------------------------------------------
void LytWkBase::GetRectCheckPos( const nw::ut::Rect& rect, u32 idx, gfl::math::VEC3* p_pos )
{
  p_pos->z = 1.0f;
  
  switch( idx ){
  case 0:
    p_pos->x = rect.left;
    p_pos->y = rect.top;
    break;

  case 1:
    p_pos->x = rect.right;
    p_pos->y = rect.top;
    break;

  case 2:
    p_pos->x = rect.left;
    p_pos->y = rect.bottom;
    break;

  case 3:
    p_pos->x = rect.right;
    p_pos->y = rect.bottom;
    break;
    
  case 4:
    p_pos->x = rect.left + ((rect.right - rect.left)/2.0f);
    p_pos->y = rect.top;
    break;

  case 5:
    p_pos->x = rect.left + ((rect.right - rect.left)/2.0f);
    p_pos->y = rect.bottom;
    break;

  case 6:
    p_pos->x = rect.left;
    p_pos->y = rect.top + ((rect.bottom - rect.top)/2.0f);
    break;

  case 7:
    p_pos->x = rect.right;
    p_pos->y = rect.top + ((rect.bottom - rect.top)/2.0f);
    break;

  case 8:
    p_pos->x = rect.left + ((rect.right - rect.left)/2.0f);
    p_pos->y = rect.top + ((rect.bottom - rect.top)/2.0f);
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �y�C���e�[�u��������
 */
//-----------------------------------------------------------------------------
void LytWkBase::initializePaneTbl( void )
{
  m_ppPaneTbl = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �y�C���e�[�u���j��
 */
//-----------------------------------------------------------------------------
void LytWkBase::finalizePaneTbl( void )
{
  GflHeapSafeFreeMemory(m_ppPaneTbl);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �y�C���e�[�u������
 */
//-----------------------------------------------------------------------------
void LytWkBase::createPaneTbl( gfl::heap::HeapBase* p_heap, bool isUseHash )
{
  if( m_ppPaneTbl == NULL ){
    m_ppPaneTbl = static_cast<nw::lyt::Pane**>(GflHeapAllocMemory( p_heap, sizeof(nw::lyt::Pane*) * m_res->GetPaneNum() ));

    nw::lyt::Pane* rootPane = m_LytSys->GetRootPane();

    //@suzuki------------------
    u32 paneNum = m_res->GetPaneNum();

    #ifdef GFL_USE_CUSTOM_NW4C
    if( isUseHash )
    {
      for( u32 i=0; i<paneNum; i++ ){
        m_ppPaneTbl[i] = m_LytSys->FindPaneByNameWithHash( m_res->GetPaneName(i) );
      }
    }
    else
    {
      for( u32 i=0; i<paneNum; i++ ){
        m_ppPaneTbl[i] = rootPane->FindPaneBySimpleName( m_res->GetPaneName(i), true );
      }
    }
    #else
    {
      for( u32 i=0; i<paneNum; i++ ){
        m_ppPaneTbl[i] = rootPane->FindPaneByName( m_res->GetPaneName(i), true );
      }
    }
    #endif


    //@suzuki------------------
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �y�C�����擾
 *
 *	@param	paneIndex   �C���f�b�N�X
 *
 *	@return �y�C��
 */
//-----------------------------------------------------------------------------
nw::lyt::Pane* LytWkBase::getPaneTblPane( u32 paneIndex ) const
{
  if( m_ppPaneTbl == NULL ) {
    return NULL;
  }
  
  if( m_res->GetPaneNum() > paneIndex ){
    return m_ppPaneTbl[ paneIndex ];
  }
  GFL_ASSERT( paneIndex < m_res->GetPaneNum() ); //@check

  return NULL;
}



//-----------------�g�p����-----------
//-----------------------------------------------------------------------------
/**
 *  @brief  �R�A�V�X�e���̎擾
 */
//-----------------------------------------------------------------------------
nw::lyt::Layout* LytWkBase::GetCore(void) const
{
  return m_LytSys;
}

//----------------------------------------------------------------------------
/**
 *	@brief    �`��C���t�H�̎擾
 *
 *	@return �`��C���t�H
 */
//-----------------------------------------------------------------------------
const nw::lyt::DrawInfo* LytWkBase::GetDrawInfo(void) const
{
  return &m_LytDrawInfo;
}

//----------------------------------------------------------------------------
/**
 *	@brief    DrawInfo�ɔ�\���̃y�C���ɑ΂��čs��v�Z���s�����ǂ�����ݒ肷��
 *
 *	@return		none
 */
//-----------------------------------------------------------------------------
void LytWkBase::SetInvisiblePaneCalculateMtx( bool flg )
{
	m_LytDrawInfo.SetInvisiblePaneCalculateMtx( flg );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �`�悷��K�v�����邩�`�F�b�N
 *
 *	@retval true    �K�v������B
 *	@retval false   �K�v���Ȃ��B
 */
//-----------------------------------------------------------------------------
b32 LytWkBase::IsDrawDisplayType( DisplayType lod ) const
{
  GFL_ASSERT( lod < DISPLAY_NORMAL_MAX ); //@check

  // ���ʕ`��
  if( m_DisplayType == DISPLAY_BOTH_MAX ){
    return true;
  }

  if( lod == m_DisplayType ){
    return true;
  }
  return false;
}


//============================================================================
/**
 *	Group����
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *  @brief  Index��Group�̎擾
 *
 *  @retval Group�̃C���X�^���X
 */
//-----------------------------------------------------------------------------
nw::lyt::Group* LytWkBase::GetGroup( u32 paneIndex ) const
{
  nw::lyt::GroupContainer* group_container  = m_LytSys->GetGroupContainer();

  if( group_container == NULL ){
    GFL_ASSERT( group_container ); //@check
    return NULL;
  }
  
  const char * paneName = m_res->GetPaneName(paneIndex);
  if( paneName == NULL ){
    return NULL;
  }

  return group_container->FindGroupByName( paneName );
}

//----------------------------------------------------------------------------
/**
 *	@brief    �Q�ƃ��C�A�E�g���\�[�X�擾
 *
 *	@return   ���C�A�E�g���\�[�X
 */
//-----------------------------------------------------------------------------
const LytRes* LytWkBase::GetLytRes(void) const
{
  return m_res;
}




//============================================================================
/**
 *  �B�������o�֐�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �I�[�g�A�j���[�V�����@�X�V����
 */
//-----------------------------------------------------------------------------
void LytWkBase::UpdateAutoAnime( void )
{
  int i;
  
  for( i=0; i<m_AnimMax; i++ )
  {
    if( m_AnimArray[i].IsInitialized() ){
      if( m_AnimArray[i].IsBindAnime() ){
        if( m_AnimArray[i].IsAutoAnime() ){
          if( m_AnimArray[i].AddAnimeFrame( m_AnimArray[i].GetAutoAnimeSpeed() ) ){
            // ���[�v�I���@���@�I�[�g�A���o�C���h�t���O�������Ă���
            if( m_AnimArray[i].IsAutoUnbind() ){
              // �I�[�g�A���o�C���h����
              this->UnBindAnime(i);
              m_AnimArray[i].SetAutoUnbind(false);
            }
          }
        }
      }
    }
  }
}


//=============================================================================
/**
 *					LytWkAnime����
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^
 */
//-----------------------------------------------------------------------------
LytWkAnime::LytWkAnime( void )
{
  m_Initialize  = false;
  m_AutoAnime   = false;
  m_AutoAnimeSpeed = 1.0f;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
LytWkAnime::~LytWkAnime()
{
  if( m_Initialize ){
    GFL_ASSERT(0);
    Finalize();
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ������
 *
 *	@param  lytwk           NW���C�A�E�g���[�N
 *	@param  animTransform   �ݒ�g�����X�t�H�[��
 *	@param  res             ���\�[�X
 *	@param  lytDataID       ���\�[�X�f�[�^ID
 */
//-----------------------------------------------------------------------------
void LytWkAnime::Initialize( nw::lyt::Layout* lytwk, nw::lyt::AnimTransform* animTransform,
                             const LytRes* res, u32 lytDataID,
                             bool isUsingHash, nw::lyt::Layout* layout )
{
  m_AnimTransform = animTransform;

  // �A�j���[�V�������\�[�X�̃A�N�Z�T
  m_AnimRes.Set( res->GetAnimeResource( lytDataID ) );

  // �����O���[�v��񂪂���̂ł���΃O���[�v�����̃A�j���[�V���������N���ł���悤�ɐ����w�肷��B
  const u16 bindGroupNum = m_AnimRes.GetGroupNum();
  u16 animNum = 0;
  if(bindGroupNum == 0){
    animNum = m_AnimRes.GetResourceBlock()->animContNum; 
  }else{
    const nw::lyt::AnimationGroupRef *const groupRefs = m_AnimRes.GetGroupArray();
    for (int grpIdx = 0; grpIdx < bindGroupNum; ++grpIdx)
    {
      // �O���[�v�ɂ���y�C���S�ĂŕK�v�ɂȂ�A�j���[�V�����̌��𐔂��܂��B
      nw::lyt::Group *const pGroup = lytwk->GetGroupContainer()->FindGroupByName(groupRefs[grpIdx].GetName());
      if (pGroup == NULL)
      {
        // "Group not found
        GFL_ASSERT(0);
        continue;
      }

      #ifdef GFL_USE_CUSTOM_NW4C
      animNum += m_AnimRes.CalcAnimationNum(pGroup, m_AnimRes.IsDescendingBind(),isUsingHash, layout);
      #else
      animNum += m_AnimRes.CalcAnimationNum(pGroup, m_AnimRes.IsDescendingBind());
      #endif
    }
  }

  // �o�C���h����A�j���[�V�������𖾎��I�Ɏw�肵�ă��\�[�X���Z�b�g���܂��B
  m_AnimTransform->SetResource( m_AnimRes.GetResourceBlock(), res->GetResourceAccessor(), animNum );
  
  m_Share       = false;
  m_Initialize  = true;
  m_BindFlag    = false;
  m_AutoAnime   = false;
  m_AutoAnimeSpeed = 1.0f;
  m_Enable      = false;
}





//----------------------------------------------------------------------------
/**
 *	@brief  �������@���L�A�j���[�V�����̏������@���@�����o�C���h
 *
 *	@param  res             ���\�[�X
 *	@param  lytDataID       ���\�[�X�f�[�^ID
 *	@param  shareIdx        �V�F�A�y�C���C���f�b�N�X
 *	@param  shareIdx        �V�F�A�y�C���C���f�b�N�X
 *
 *	@retval true    �o�^����
 *	@retval false   �o�^���s�i�Pane�������j
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::InitializeShare( nw::lyt::Layout* lytwk, const LytRes* res, void * p_animeRes, u32 shareIdx, nw::lyt::PaneLinkList::Iterator it, nw::lyt::Pane* pSrcPane, nw::lyt::Group* pGroup )
{
  m_AnimTransform = NULL;
  // �A�j���[�V�������\�[�X�̃A�N�Z�T
  m_AnimRes.Set( p_animeRes );

  // �Ώ�Pane������
  {
    const u16 animShareInfoNum = m_AnimRes.GetAnimationShareInfoNum();
    const u16 bindGroupNum = m_AnimRes.GetGroupNum();
    
    // �ő�l�`�F�b�N
    if( animShareInfoNum <= shareIdx ){
      GFL_ASSERT(0);
      return false;
    }

    if (pSrcPane == NULL)
    {
      // "Source pane of animation-share is not found"
      GFL_ASSERT(0);
      return false;
    }

    nw::lyt::internal::AnimPaneTree animPaneTree(pSrcPane, m_AnimRes);
  
    
    if (! animPaneTree.IsEnabled()) // ���L���y�C���ɃA�j���[�V����������?
    {
      GFL_ASSERT(0);
      return false;
    }

    // �Ώۂ̃O���[�v�����邩
    if (pGroup == NULL)
    {
      // "Target group of animation-share is not found:"
      GFL_ASSERT(0);
      return false;
    }


    m_Pane = NULL;
    if( it->target != pSrcPane ){
      m_Pane = it->target;
    }
  }

  if( m_Pane == NULL ){
    return false;
  }

  m_Share       = true;
  m_ShareIndex  = shareIdx;
  m_Initialize  = true;
  m_BindFlag    = false;
  m_AutoAnime   = false;
  m_AutoAnimeSpeed = 1.0f;
  m_Enable      = false;

  return true;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �ʏ�o�C���h����
 *
 *	@param	lytwk ���[�N
 */
//-----------------------------------------------------------------------------
void LytWkAnime::Bind( nw::lyt::Layout* lytwk, bool is_enable, bool is_useHash )
{
  if( m_BindFlag ){
    GFL_ASSERT( !m_BindFlag ); //@check
    return ;
  }
  
  // �O���[�v�̗L���Ńo�C���h��ς���
  // �����O���[�v��񂪂���̂ł���΃O���[�v���g�p���ăo�C���h
  const u16 bindGroupNum = m_AnimRes.GetGroupNum();
  if(bindGroupNum == 0)
  {
    #ifdef GFL_USE_CUSTOM_NW4C
    if( is_useHash ){
      lytwk->GetRootPane()->BindAnimationWithHash(m_AnimTransform, lytwk, true, !is_enable);
    }
    else
    #endif
    {
      lytwk->GetRootPane()->BindAnimation(m_AnimTransform, true, !is_enable);
    }
  }
  else{
    const nw::lyt::AnimationGroupRef *const groupRefs = m_AnimRes.GetGroupArray();
    for (int grpIdx = 0; grpIdx < bindGroupNum; ++grpIdx)
    {
      // �O���[�v�ɂ���y�C���S�ĂŕK�v�ɂȂ�A�j���[�V�����̌��𐔂��܂��B
      nw::lyt::Group *const pGroup = lytwk->GetGroupContainer()->FindGroupByName(groupRefs[grpIdx].GetName());
      if (pGroup == NULL)
      {
        // "Group not found
        GFL_ASSERT(0);
        continue;
      }


      #ifdef GFL_USE_CUSTOM_NW4C
      if( is_useHash ){
        nw::lyt::BindAnimationWithHash(pGroup, m_AnimTransform, lytwk, m_AnimRes.IsDescendingBind(), !is_enable);
      }
      else
      #endif
      {
        nw::lyt::BindAnimation(pGroup, m_AnimTransform, m_AnimRes.IsDescendingBind(), !is_enable);
      }
    }
  }

  m_BindFlag = true;
  m_Enable = is_enable;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���L�A�j���[�V�����̃o�C���h����
 *
 *	@param	lytwk ���[�N
 *	@param  res   ���\�[�X�N���X
 */
//-----------------------------------------------------------------------------
void LytWkAnime::BindShare( nw::lyt::Layout* lytwk, const LytRes* res, nw::lyt::Pane * pSrcPane )
{
  if( m_Share == false ){
    GFL_ASSERT(0);
    return ;
  }
  if( m_BindFlag ){  // �o�C���h����
    GFL_ASSERT(0);
    return ;
  }

  if (pSrcPane == NULL)
  {
    // "Source pane of animation-share is not found"
    GFL_ASSERT(0);
    return;
  }

  nw::lyt::internal::AnimPaneTree animPaneTree(pSrcPane, m_AnimRes);
  // srcPaneName�ƈقȂ�y�C���ɑ΂��ăA�j���[�V�������o�C���h
  m_AnimTransform = animPaneTree.Bind(lytwk, m_Pane, res->GetResourceAccessor());
  m_Pane->SetAnimationEnable( m_AnimTransform, false, false );


  m_BindFlag = true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ʏ�A���o�C���h����
 *
 *	@param	lytwk ���[�N
 */
//-----------------------------------------------------------------------------
void LytWkAnime::UnBind( nw::lyt::Layout* lytwk )
{
  if( m_Share == false ){
    if( m_BindFlag ){
      lytwk->UnbindAnimation( m_AnimTransform );
      m_BindFlag = false;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���L�A�j���[�V�����̃A���o�C���h����
 *
 *	@param	lytwk ���[�N
 */
//-----------------------------------------------------------------------------
void LytWkAnime::UnBindShare( nw::lyt::Layout* lytwk )
{
  if( m_BindFlag ){
    if( m_Share && (m_AnimTransform!=NULL) ){
      m_Pane->UnbindAnimation( m_AnimTransform, false );
      lytwk->DeleteAnimTransform( m_AnimTransform );
      m_AnimTransform = NULL;
      m_BindFlag = false;
    }
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V�����̗L���E�����ݒ�
 */
//-----------------------------------------------------------------------------
void LytWkAnime::SetEnable( nw::lyt::Layout* lytwk, bool flag )
{
  if(m_AnimTransform){
    m_Enable = flag;
    if( !m_Share ){
      lytwk->GetRootPane()->SetAnimationEnable( m_AnimTransform, flag ); 
    }else{
      m_Pane->SetAnimationEnable( m_AnimTransform, flag, false ); 
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V�������L�����`�F�b�N
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::IsEnable( void ) const
{
  return m_Enable;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���\�[�X����@Share�O���[�v���擾
 *
 *	@param	res           ���\�[�X
 *	@param	lytDataID     ���\�[�X�f�[�^ID
 *	@param  shareGroupIdx Share�O���[�v�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::GetResShareInfoInPaneNum( const nw::lyt::Layout* lytwk, const nw::lyt::AnimResource& animRes, const LytRes* res, u32 shareGroupIdx, nw::lyt::Pane** ppSrcPane, nw::lyt::Group** ppGroup, const char ** ppGroupName, u32 groupIndex )
{
  const u16 animShareInfoNum = animRes.GetAnimationShareInfoNum();
  
  // �ő�l�`�F�b�N
  if( animShareInfoNum <= shareGroupIdx ){
    GFL_ASSERT(0);
    return false;
  }

  const nw::lyt::AnimationShareInfo *const animShareInfoAry = animRes.GetAnimationShareInfoArray();


  *ppGroupName = animShareInfoAry[shareGroupIdx].GetTargetGroupName();
  // �w��O���[�v�H
  if( groupIndex != LYTSYS_PANEINDEX_ERROR ){
    if( gfl::std::StringCmp( *ppGroupName, res->GetPaneName( groupIndex ) ) != 0 ){
      return false;
    }
  }
  
  // ���̃O���[�v���A�j���[�V�����̊֘A�ɓ����Ă��邩�H
  if( animRes.GetGroupNum() > 0 ) {
    // �����̃O���[�v���֘A�O���[�v�ɓ����Ă��邩�`�F�b�N
    // �֘A�O���[�v�Ɋ܂܂�ĂȂ��ꍇ�͋��L���Ȃ�
    const bool bInclude = IsIncludeAnimationGroupRef(
        animRes.GetGroupArray(),
        *ppGroupName,
        animRes.GetGroupNum() );
    if( !bInclude ){
      return false;
    }
  }

  *ppGroup = lytwk->GetGroupContainer()->FindGroupByName(*ppGroupName);
  if (*ppGroup == NULL)
  {
    // "Target group of animation-share is not found:"
    GFL_ASSERT_MSG(0, "group name %s\n", animShareInfoAry[shareGroupIdx].GetTargetGroupName());
    return false;
  }

  // ���̃V�F�A�y�C�������O���[�v�̃y�C���ő吔�����߂�
  #ifdef GFL_USE_CUSTOM_NW4C
  *ppSrcPane = lytwk->GetRootPane()->FindPaneBySimpleName(animShareInfoAry[shareGroupIdx].GetSrcPaneName());
  #else
  *ppSrcPane = lytwk->GetRootPane()->FindPaneByName(animShareInfoAry[shareGroupIdx].GetSrcPaneName());
  #endif

  if (*ppSrcPane == NULL)
  {
    // "Source pane of animation-share is not found"
    GFL_PRINT( "pane name %s group name %s\n", animShareInfoAry[shareGroupIdx].GetSrcPaneName(), animShareInfoAry[shareGroupIdx].GetTargetGroupName() );
    GFL_ASSERT_MSG(0, "pane name %s\n", animShareInfoAry[shareGroupIdx].GetSrcPaneName());
    return false;
  }

  nw::lyt::internal::AnimPaneTree animPaneTree(*ppSrcPane, animRes);
  if (! animPaneTree.IsEnabled()) // ���L���y�C���ɃA�j���[�V����������?
  {
    // "srcPane not Animation"
    return false;
  }

  return true;
}



//----------------------------------------------------------------------------
/**
 *	@brief  share�O���[�v���ł́A�O���[�vID���擾
 *
 *  @param  lytwk         ���C�A�E�g���[�N
 *	@param	res           ���\�[�X
 *	@param	lytDataID     ���\�[�X�f�[�^ID
 *	@param  shareGroupIdx Share�O���[�v�C���f�b�N�X
 *	@param  cp_panename   �y�C����
 *
 *	@return �O���[�v���̃y�C���C���f�b�N�X
 */
//-----------------------------------------------------------------------------
s32 LytWkAnime::GetResShareGroupInPaneIndex( const nw::lyt::Layout* lytwk, const LytRes* res, const nw::lyt::AnimResource& animRes, u32 shareGroupIdx, const char* cp_panename, nw::lyt::Pane** ppSrcPane, nw::lyt::Group** ppGroup )
{
  const u16 animShareInfoNum = animRes.GetAnimationShareInfoNum();
  
  // �ő�l�`�F�b�N
  if( animShareInfoNum <= shareGroupIdx ){
    GFL_ASSERT(0);
    return -1;
  }

  // ���̃V�F�A�y�C�������O���[�v�̃y�C���ő吔�����߂�
  const nw::lyt::AnimationShareInfo *const animShareInfoAry = animRes.GetAnimationShareInfoArray();
  
  if(*ppSrcPane == NULL)
  {
    #ifdef GFL_USE_CUSTOM_NW4C
    *ppSrcPane = lytwk->GetRootPane()->FindPaneBySimpleName( animShareInfoAry[shareGroupIdx].GetSrcPaneName() );
    #else
    *ppSrcPane = lytwk->GetRootPane()->FindPaneByName( animShareInfoAry[shareGroupIdx].GetSrcPaneName() );
    #endif
  }
  if (*ppSrcPane == NULL)
  {
    // "Source pane of animation-share is not found"
    GFL_ASSERT(0);
    return 0;
  }

  nw::lyt::internal::AnimPaneTree animPaneTree(*ppSrcPane, animRes);
  if (! animPaneTree.IsEnabled()) // ���L���y�C���ɃA�j���[�V����������?
  {
    return 0;
  }

  if(*ppGroup == NULL){
    *ppGroup = lytwk->GetGroupContainer()->FindGroupByName(animShareInfoAry[shareGroupIdx].GetTargetGroupName());
  }
  if (*ppGroup == NULL)
  {
    // "Target group of animation-share is not found:"
    GFL_ASSERT(0);
    return 0;
  }

  nw::lyt::PaneLinkList& paneList = (*ppGroup)->GetPaneList();

  u32 wkcount = 0;
  for (nw::lyt::PaneLinkList::Iterator it = paneList.GetBeginIter(); it != paneList.GetEndIter(); ++it, ++wkcount)
  {
    nw::lyt::Pane* p_pane = it->target;

    if( gfl::std::StringCmp( p_pane->GetName(), cp_panename ) == 0 ){
      return wkcount;
    }
  }

  return -1;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j��
 */
//-----------------------------------------------------------------------------
void LytWkAnime::Finalize( void )
{
  if( m_Initialize ){

    GFL_ASSERT( m_BindFlag == false );

    m_AnimTransform = NULL;
    m_Initialize = false;

    m_Share = false;
    m_Pane = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �������Â݃`�F�b�N
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::IsInitialized( void ) const
{
  return m_Initialize;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V�����t���[����i�߂�
 *
 *	@param	frame �i�߂�t���[����
 *
 *	@retval true      �A�j���[�V�����I��
 *	@retval false     �A�j���[�V�����r��
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::AddAnimeFrame( f32 frame )
{
  bool ret = false;
  if( m_AnimTransform ){
    if( m_Enable ){
      f32 frame_now = m_AnimTransform->GetFrame();
      f32 frame_max = m_AnimTransform->GetFrameMax();
      bool loop = m_AnimTransform->IsLoopData();
      
      //frame��2.0�ŁA�ŏI�t���[��������̏ꍇ
      //�ŏI�t���[�����΂����u�ԂɁ��P��true���Ԃ�A�I�[�g�A���o�C���h�ɂȂ��Ă��܂�����
      //�Ōゾ��frame��1.0f�ɂ���
      if( static_cast<int>(frame) % 2 == 0 //����
          && (static_cast<int>(frame_max)-1) % 2 == 1  //�
          && frame_now + frame > frame_max-1 ){
        frame = 1.0f;
      }

      frame_now += frame;

      if( frame_now < 0 ){
        if( !loop ){
          frame_now = 0;
        }else{
          frame_now += frame_max;
        }
      }else if( frame_max <= frame_now ){

        if( !loop ){  //���P
          frame_now = frame_max-1.0f;
          ret = true;
        }else{
          frame_now -= frame_max;
        }
      }
      m_AnimTransform->SetFrame( frame_now );
    }
  }

  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V�����t���[���̏I�����`�F�b�N
 *
 *	@retval true      �A�j���[�V�����I��
 *	@retval false     �A�j���[�V�����r��
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::IsAnimeEnd( void ) const
{
  if( m_AnimTransform ){
    f32 frame_now = m_AnimTransform->GetFrame();
    f32 frame_max = m_AnimTransform->GetFrameMax();
    bool loop = m_AnimTransform->IsLoopData();

    // UnBind����?
    if( !m_Enable ){
      return true;
    }
    
    if( (frame_now >= (frame_max-1.0f)) && (!loop) ){
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �I�[�g�A�j���[�V�����ݒ�
 *
 *	@param	flag  true  �I�[�g�A�j��ON    false �I�[�g�A�j��
 */
//-----------------------------------------------------------------------------
void LytWkAnime::SetAutoAnime( bool flag )
{
  m_AutoAnime = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �I�[�g�A�j���[�V�����ݒ�@�擾
 *
 *	@retval true    �I�[�g�A�j��ON
 *	@retval false   �I�[�g�A�j��OFF
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::IsAutoAnime( void ) const
{
  return m_AutoAnime;
} 

//----------------------------------------------------------------------------
/**
 *	@brief Lyt�̃A�j���[�V�����N���X���擾
 */
//-----------------------------------------------------------------------------
nw::lyt::AnimTransform* LytWkAnime::GetAnimeWork( void ) const
{
  return m_AnimTransform;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  ���L�A�j���[�V�������`�F�b�N
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::IsShareAnime(void) const
{
  return m_Share;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �o�C���h����Ă��邩�`�F�b�N
 */
//-----------------------------------------------------------------------------
bool LytWkAnime::IsBindAnime( void ) const
{
  return m_BindFlag;
}




} // g2d
} // grp
} // gfl
