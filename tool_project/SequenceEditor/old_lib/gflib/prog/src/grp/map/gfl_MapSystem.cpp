//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_MapSystem.cpp
 *	@brief  �}�b�v�Ǘ��V�X�e��
 *	@author	tomoya takahashi
 *	@date		2011.01.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include <grp/map/gfl_MapSystem.h>

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
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					����
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^
 */
//-----------------------------------------------------------------------------
System::System( void ) : 
  m_Initialize(false),
  m_CreateModel(false),
  m_Visible(true),
  m_Trans(gfl::math::VEC3(0,0,0)),
  m_pArcFile(NULL),
  m_ArcFileOriginal(false),
  m_DataID(0),
  m_pMapData(NULL),
  m_MapDataHeapSize(0),
  m_FileType(0),
  m_pFileControl(NULL),
  m_cpFileControls(NULL),
  m_VramAddrTex(0),
  m_VramAddrPltt(0),
  m_MapTexVramSize(0),
  m_DivLoadSize(0),
  m_LoadStatus(LOAD_STATE_IDLING)
{
  this->ReleaseGlobalTexResource();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^  + ������
 */
//-----------------------------------------------------------------------------
System::System( const SetUpParam& param, gfl::grp::g3d::Scene* p_scene, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap ) : 
  m_Initialize(false),
  m_CreateModel(false),
  m_Visible(true),
  m_Trans(gfl::math::VEC3(0,0,0)),
  m_pArcFile(NULL),
  m_ArcFileOriginal(false),
  m_DataID(0),
  m_pMapData(NULL),
  m_MapDataHeapSize(0),
  m_FileType(0),
  m_pFileControl(NULL),
  m_cpFileControls(NULL),
  m_VramAddrTex(0),
  m_VramAddrPltt(0),
  m_MapTexVramSize(0),
  m_ModelDatas(),
  m_DivLoadSize(0),
  m_LoadStatus(LOAD_STATE_IDLING)
{
  // ���[�N�N���A
  this->ReleaseGlobalTexResource();

  // ����������
  Initialize( param, p_scene, p_sysHeap, p_deviceHeap );
}




//----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
System::~System()
{
  Finalize();
}

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
void System::Initialize( const SetUpParam& param, gfl::grp::g3d::Scene* p_scene, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap )
{
  // ���d�������`�F�b�N
  if( m_Initialize != false ){
    GFL_ASSERT(0);
    Finalize();
  }

  // ���f�����[�h�̕ۑ�
  m_ModelMode = param.modelMode;
  
  // �V�[���V�X�e��������
  m_pScene = p_scene;

  // �t�@�C���R���g���[���ۑ�
  m_cpFileControls = param.cpFileControls;

  // �}�b�v�o�b�t�@������
  InitializeMapBuffer( param, p_sysHeap, p_deviceHeap );

  // VRAM�A�h���X������
  InitializeVramAddr( param, p_sysHeap, p_deviceHeap );

  // ���\�[�X�o�b�t�@������
  InitializeModelBuffer( param, p_sysHeap, p_deviceHeap );

  // �����ǂݍ��ݏ�����
  InitializeDivLoad( param, p_sysHeap, p_deviceHeap );

  // ����������
  m_Initialize = true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j��
 */
//-----------------------------------------------------------------------------
void System::Finalize( void )
{
  if( m_Initialize ){
    m_Initialize = false;

    {
      u32 count = 0;
      while( (ResetLoadData() == false) && (count < 10000) ){
        nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMicroSeconds( 10000 ) );
        ++count;
      }
      GFL_ASSERT_STOP( count < 10000 );
    }

    //�A�[�J�C�u�t�@�C��
    ReleaseArc();

    // �����ǂݍ��ݔj��
    FinalizeDivLoad();

    // ���\�[�X�o�b�t�@�j��
    FinalizeModelBuffer();

    // VRAM�A�h���X�j��
    FinalizeVramAddr();

    // �}�b�v�o�b�t�@�j��
    FinalizeMapBuffer();

    m_pScene = NULL;

    Clear();
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X�V����
 */
//-----------------------------------------------------------------------------
void System::Update( void )
{
  // �ǂݍ��ݏ���
  UpdateLoad();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �`��X�V����
 */
//-----------------------------------------------------------------------------
void System::UpdateDraw( gfl::grp::g3d::Camera* p_camera )
{
  GFL_UNUSED(p_camera);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ǂݍ��݃��N�G�X�g
 *
 *	@param	dataID        �f�[�^ID
 *	@param	divLoadFlag   �����ǂݍ��݂��邩�B�@�itrue=�����@false=�ꊇ�j
 *	@param  true  ���N�G�X�g����
 *	@param  false ���N�G�X�g���s
 */
//-----------------------------------------------------------------------------
bool System::ReqLoadData( u32 dataID, b32 divLoadFlag )
{
  u32 loadSize;
  GFL_ASSERT_STOP( m_pArcFile );
  if( !m_pArcFile->WaitSetupForFastMode() ){
    return false;
  }
  if( m_pArcFile->WaitAsyncRead() ){
    return false;
  }
  
  if( divLoadFlag ){
    loadSize = m_DivLoadSize;
  }else{
    loadSize = m_pArcFile->GetDataSize( dataID );
  }

  m_DataID = dataID;
  m_DivLoad.StartFileLoad( m_pArcFile, m_DataID, loadSize );

  m_LoadStatus = LOAD_STATE_START;

  return true;
}

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
bool System::ReqLoadCompData( u32 dataID, b32 divLoadFlag, gfl::heap::HeapBase* p_tmp_buff )
{
  u32 loadSize;
  GFL_ASSERT_STOP( m_pArcFile );
  if( !m_pArcFile->WaitSetupForFastMode() ){
    return false;
  }
  if( m_pArcFile->WaitAsyncRead() ){
    return false;
  }
  
  if( divLoadFlag ){
    loadSize = m_DivLoadSize;
  }else{
    loadSize = m_pArcFile->GetDataSize( dataID );
  }

  m_DataID = dataID;
  m_DivLoad.StartCompFileLoad( m_pArcFile, m_DataID, loadSize, p_tmp_buff );

  m_LoadStatus = LOAD_STATE_START;

  return true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ǂݍ��݃��Z�b�g
 */
//-----------------------------------------------------------------------------
bool System::ResetLoadData( void )
{
  bool result = false;
  
  // �ǂݍ��ݏ��̔j��
  DeleteModel();
  
  result = m_DivLoad.ResetFileLoad();
  if( result ){
    m_LoadStatus = LOAD_STATE_IDLING;
  }
  return result;
}

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
gfl::grp::map::System::LoadStatus System::GetLoadStatus( void ) const
{
  return m_LoadStatus;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ǂݍ���ł����f�[�^���擾
 *
 *	@return �ǂݍ���ł����f�[�^(gflib_cpp/tools/binlinker.rb���g�p���Đ�������B�j
 */
//-----------------------------------------------------------------------------
const gfl::base::BinLinkerAccessor& System::GetLoadMemoryPointer(void) const
{
  return m_LinkData;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ǂݍ���ł����f�[�^ID���擾
 *
 *	@return �f�[�^ID
 */
//-----------------------------------------------------------------------------
u32 System::GetLoadDataID( void ) const
{
  return m_DataID;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �A�[�J�C�u�f�[�^�ݒ�
 *
 *	@param	  cp_path   �A�[�J�C�u�f�[�^�̃p�X
 *  @param    p_heap    �q�[�v
 */
//-----------------------------------------------------------------------------
void System::ResistArc( const wchar_t* cp_path, gfl::heap::HeapBase* p_heap )
{
  if( m_pArcFile ){
    GFL_ASSERT(0);
    ReleaseArc();
  }
  m_pArcFile = GFL_NEW(p_heap) gfl::fs::ArcFile(p_heap, cp_path, 0);
  m_pArcFile->StartSetupForFastMode( p_heap, p_heap );
  m_ArcFileOriginal = true;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �A�[�J�C�u�f�[�^�ݒ�  ID�o�[�W����
 *
 *	@param	  arc_id    �A�[�J�C�u�f�[�^��ID
 *  @param    p_heap    �q�[�v
 *  @param    file_on_pc  �p�\�R������̓ǂݍ��݂̏ꍇtrue, ROM�̏ꍇfalse
 */
//-----------------------------------------------------------------------------
void System::ResistArc( gfl::fs::ArcFile::ARCID arc_id, gfl::heap::HeapBase* p_heap, bool file_on_pc )
{
  if( m_pArcFile ){
    GFL_ASSERT(0);
    ReleaseArc();
  }
  u32 init_flag = 0;
  if( file_on_pc ) {
    init_flag |= gfl::fs::ArcFile::HIO;
  }
  m_pArcFile = GFL_NEW(p_heap) gfl::fs::ArcFile(p_heap, arc_id, init_flag);
  m_pArcFile->StartSetupForFastMode( p_heap, p_heap );
  m_ArcFileOriginal = true;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �A�[�J�C�u�f�[�^�ݒ�
 *
 *	@param	 p_arc 
 */
//-----------------------------------------------------------------------------
void System::ResistArc( gfl::fs::ArcFile* p_arc )
{
  if( m_pArcFile ){
    GFL_ASSERT(0);
    ReleaseArc();
  }
  m_pArcFile = p_arc;
  m_ArcFileOriginal = false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�[�J�C�u�ݒ芮���҂�
 *
 *  @retval true    ����
 *  @retval false   �r���i�A�[�J�C�u�̃t�@�[�X�g���[�h�Z�b�g�A�b�v���j
 */
//-----------------------------------------------------------------------------
bool System::IsRegistArcFinished( void ) const
{
  if( m_pArcFile ){
    if( m_pArcFile->WaitSetupForFastMode() ){
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�[�J�C�u�f�[�^�j��
 */
//-----------------------------------------------------------------------------
void System::ReleaseArc( void )
{
  if(m_pArcFile){
    if( m_ArcFileOriginal ){
      GFL_DELETE m_pArcFile;
    }
    m_pArcFile = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�[�J�C�u�f�[�^�擾
 *
 */
//-----------------------------------------------------------------------------
gfl::fs::ArcFile* System::GetArc( void ) const
{
  return m_pArcFile;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �O���[�o���e�N�X�`�����\�[�X�@�ݒ�
 *  
 *	@param	  p_globalTex   �O���[�o���e�N�X�`��
 *	@param    index         �O���[�o���e�N�X�`���̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void System::ResistGlobalTexResource( gfl::grp::g3d::Resource* p_globalTex, u32 index )
{
  // ���f�����[�h�̕s��v�`�F�b�N
  if( !this->isModelMode( MODEL_DEFAULT ) ){GFL_ASSERT_STOP(0);}
  
  if(index < GLOBAL_RESOURCE_MAX){
    m_ModelDatas.def.m_pGlobalTexRes[index] = p_globalTex;
  }else{
    GFL_ASSERT(index < GLOBAL_RESOURCE_MAX); //@check
  }
}

#if GFL_GRP_G3D_H3D_USE
void System::ResistGlobalTexResource( gfl::grp::g3d::H3dResource* p_globalTex, u32 index )
{
  // ���f�����[�h�̕s��v�`�F�b�N
  if( !this->isModelMode( MODEL_H3D ) ){GFL_ASSERT_STOP(0);}
  
  if(index < GLOBAL_RESOURCE_MAX){
    m_ModelDatas.h3d.m_pGlobalTexRes[index] = p_globalTex;
  }else{
    GFL_ASSERT(index < GLOBAL_RESOURCE_MAX); //@check
  }
}
#endif // GFL_GRP_G3D_H3D_USE

//----------------------------------------------------------------------------
/**
 *	@brief  �O���[�o���e�N�X�`�����\�[�X�@�N���A
 */
//-----------------------------------------------------------------------------
void System::ReleaseGlobalTexResource(void)
{
  if( m_ModelMode == MODEL_DEFAULT ){
    for( u32 i=0; i<GLOBAL_RESOURCE_MAX; ++i ){
      m_ModelDatas.def.m_pGlobalTexRes[i] = NULL;
    }
  }
  else{
#if GFL_GRP_G3D_H3D_USE
    for( u32 i=0; i<GLOBAL_RESOURCE_MAX; ++i ){
      m_ModelDatas.h3d.m_pGlobalTexRes[i] = NULL;
    }
#endif // GFL_GRP_G3D_H3D_USE
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �n�`�\����ON�EOFF
 *
 *	@param	flag    true:ON false:OFF
 */
//-----------------------------------------------------------------------------
void System::SetVisible( b32 flag )
{
  m_Visible = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �n�`�\����ON�EOFF�@�`�F�b�N
 *
 *	@retval true      ON
 *	@retval false     OFF
 */
//-----------------------------------------------------------------------------
b32 System::IsVisible( void ) const
{
  return m_Visible;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �n�`�\���ʒu�̐ݒ�
 *
 *	@param	  pos   �ʒu
 */
//-----------------------------------------------------------------------------
void System::SetTrans( const gfl::math::VEC3& pos )
{
  m_Trans = pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �n�`�\���ʒu�̎擾
 *
 *	@return �ʒu
 */
//-----------------------------------------------------------------------------
const gfl::math::VEC3& System::GetTrans( void ) const
{
  return m_Trans;
}

 
//----------------------------------------------------------------------------
/**
 *	@brief  �A�g���r���[�g�擾
 *  
 *	@param	  p_attrInfo    �A�g���r���[�g�C���t�H
 *	@param    cp_pos        �RD���W
 *	@param    cp_way        �n�ʕ����iDefault�́A(0.0f,-1.0f,0.0f)
 *	@param    map_width     �}�b�v�u���b�N��
 */
//-----------------------------------------------------------------------------
void System::GetAttr( gfl::grp::map::AttrInfo* p_attrInfo, const void* cp_pos, const gfl::math::VEC3* cp_way, f32 map_width )
{
  // �A�g���r���[�g�C���t�H�̏�����
  FileControl::ClearAttrInfo( p_attrInfo );
  
  // �A�g���r���[�g�̎擾
  if( m_pFileControl ){
    m_pFileControl->GetAttr( this, p_attrInfo, cp_pos, cp_way, map_width );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �n�`���f������
 *  
 *  @param  p_allocator       �A���P�[�^�i�f�o�C�X�������j
 *  @param  p_model_memory    ���f�����\�[�X�o�b�t�@
 *  @param  p_tex_memory      �e�N�X�`�����\�[�X�o�b�t�@
 *  @param  cp_desc           ���\�[�X�W�J�ݒ�
 */
//-----------------------------------------------------------------------------
void System::CreateModel( gfl::heap::NwAllocator* p_allocator, void* p_model_memory, void* p_tex_memory, const gfl::grp::g3d::Resource::Description* cp_res_desc, const gfl::grp::g3d::Model::Description* cp_model_desc, u32 common_resource_flag )
{
  bool result;

  // ���f�����[�h�̕s��v�`�F�b�N
  if( !this->isModelMode( MODEL_DEFAULT ) ){GFL_ASSERT_STOP(0);}

  
  // ���d�o�^�Ď�
  if( m_ModelDatas.def.m_pMdlRes->IsSetup() ){
    GFL_ASSERT(0);
    DeleteModel();
  }

  // ���f��
  m_ModelDatas.def.m_pMdlRes->AttachBuffer( p_allocator, p_model_memory, cp_res_desc, false );
  if( p_tex_memory ){
    m_ModelDatas.def.m_pTexRes->AttachBufferAndSetup( p_allocator, p_tex_memory, cp_res_desc, false );
    // ���f�����\�[�X�ɁA�e�N�X�`�����\�[�X��o�^
    m_ModelDatas.def.m_pMdlRes->Setup( p_allocator, m_ModelDatas.def.m_pTexRes );
  }


  // �O���[�o���e�N�X�`���̃Z�b�g�A�b�v
  for( u32 i=0; i<GLOBAL_RESOURCE_MAX; ++i ){
    if( m_ModelDatas.def.m_pGlobalTexRes[i] ){
      m_ModelDatas.def.m_pMdlRes->Setup( p_allocator, m_ModelDatas.def.m_pGlobalTexRes[i] );
    }
  }
  
  // ���\�[�X�ݒ�
  result = m_ModelDatas.def.m_pMdlRes->Setup( p_allocator, NULL, common_resource_flag );
  if( !result ){
    int nw_result = m_ModelDatas.def.m_pMdlRes->GetResult();
    GFL_PRINT( "ModelResource SetUp Fail %d\n", nw_result );
    m_ModelDatas.def.m_pMdlRes->PrintUnresolvedReference();
    GFL_ASSERT_STOP_MSG( result, "ModelResource SetUp Fail %d\n", nw_result );
  }

  // ���f���̍\�z ���\�[�X�̂O�Ԃɂ��邱�ƁI
  m_ModelDatas.def.m_pModel->Create( p_allocator, p_allocator, m_ModelDatas.def.m_pMdlRes, 0, const_cast<gfl::grp::g3d::Model::Description*>(cp_model_desc) );


  // ���f����������
  m_CreateModel = true;

  // ���f���Ɉʒu��o�^
  m_ModelDatas.def.m_pModel->SetTranslate( m_Trans );

  // �V�[���ɓo�^
  m_pScene->AddSceneNode( m_ModelDatas.def.m_pModel );
}
#if GFL_GRP_G3D_H3D_USE
// H3D��
void System::CreateH3dModel( gfl::heap::NwAllocator* p_allocator, void* p_model_memory, void* p_tex_memory, /* const gfl::grp::g3d::H3dResource::Description* cp_res_desc,  const gfl::grp::g3d::H3dModel::Description* cp_model_desc, */ u32 common_resource_flag )
{
  bool result;

  // ���f�����[�h�̕s��v�`�F�b�N
  if( !this->isModelMode( MODEL_H3D ) ){GFL_ASSERT_STOP(0);}
  
  // ���d�o�^�Ď�
  if( m_ModelDatas.h3d.m_pMdlRes->IsSetup() )
  {
    GFL_ASSERT(0);
    DeleteModel();
  }

  // ���f��
  m_ModelDatas.h3d.m_pMdlRes->AttachBuffer( p_model_memory/*, cp_res_desc, */ );
  if( p_tex_memory ){
    m_ModelDatas.h3d.m_pTexRes->AttachBuffer( p_tex_memory );
    m_ModelDatas.h3d.m_pTexRes->Setup( p_allocator, p_allocator/*, cp_res_desc, */ );
    // ���f�����\�[�X�ɁA�e�N�X�`�����\�[�X��o�^
    m_ModelDatas.h3d.m_pMdlRes->Setup( p_allocator, p_allocator, m_ModelDatas.h3d.m_pTexRes );
  }


  // �O���[�o���e�N�X�`���̃Z�b�g�A�b�v
  for( u32 i=0; i<GLOBAL_RESOURCE_MAX; ++i ){
    if( m_ModelDatas.h3d.m_pGlobalTexRes[i] ){
      m_ModelDatas.h3d.m_pMdlRes->Setup( p_allocator, p_allocator, m_ModelDatas.h3d.m_pGlobalTexRes[i] );
    }
  }
  
  // ���\�[�X�ݒ�
  result = m_ModelDatas.h3d.m_pMdlRes->Setup( p_allocator, p_allocator, NULL, common_resource_flag );
  if( !result ){
    GFL_ASSERT_STOP_MSG( result, "ModelResource SetUp Fail\n" );
  }

  // ���f���̍\�z ���\�[�X�̂O�Ԃɂ��邱�ƁI
  m_ModelDatas.h3d.m_pModel->Create( p_allocator, p_allocator, m_ModelDatas.h3d.m_pMdlRes, 0 /*,const_cast<gfl::grp::g3d::Model::Description*>(cp_model_desc)*/ );


  // ���f����������
  m_CreateModel = true;

  // ���f���Ɉʒu��o�^
  m_ModelDatas.h3d.m_pModel->SetTranslate( m_Trans );

  // �V�[���ɓo�^
  m_pScene->AddH3dModelToCalc( m_ModelDatas.h3d.m_pModel, 0 );
  m_pScene->AddH3dModelToDraw( m_ModelDatas.h3d.m_pModel, 0 );
}
#endif // GFL_GRP_G3D_H3D_USE



//----------------------------------------------------------------------------
/**
 *	@brief  �n�`���f���j��
 */
//-----------------------------------------------------------------------------
void System::DeleteModel( void )
{
  b32 result;

  if( m_CreateModel ){

    if( m_ModelMode == MODEL_DEFAULT ){
      // �V�[������j��
      m_pScene->RemoveSceneNode( m_ModelDatas.def.m_pModel );
    
      // ���f�����\�[�X�j��
      m_ModelDatas.def.m_pModel->Destroy();

      // ���\�[�X�̔j��
      if( m_ModelDatas.def.m_pTexRes->IsSetup() ){
        result = m_ModelDatas.def.m_pTexRes->DetachBuffer();
        GFL_ASSERT_STOP( result );
      }
      result = m_ModelDatas.def.m_pMdlRes->DetachBuffer();
      GFL_ASSERT_STOP( result );
    }
    else {

#if GFL_GRP_G3D_H3D_USE
      // �V�[������j��
      m_pScene->RemoveH3dModelFromCalcAndAllDraw( m_ModelDatas.h3d.m_pModel );
    
      // ���f�����\�[�X�j��
      m_ModelDatas.h3d.m_pModel->Destroy();

      // ���\�[�X�̔j��
      if( m_ModelDatas.h3d.m_pTexRes->IsSetup() )
      {
        m_ModelDatas.h3d.m_pTexRes->DetachBuffer();
      }
      m_ModelDatas.h3d.m_pMdlRes->DetachBuffer();
#endif // GFL_GRP_G3D_H3D_USE

    }

    // ���f���j���@�˗�
    m_CreateModel = false;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �n�`���f���̎擾
 *
 *	@retval ���f���I�u�W�F�N�g
 */
//-----------------------------------------------------------------------------
gfl::grp::g3d::Model* System::GetModel( void ) const
{
  // ���f�����[�h�̕s��v�`�F�b�N
  if( !this->isModelMode( MODEL_DEFAULT ) ){GFL_ASSERT_STOP(0);}
  return m_ModelDatas.def.m_pModel;
}
#if GFL_GRP_G3D_H3D_USE
gfl::grp::g3d::H3dModel* System::GetH3dModel( void ) const
{
  // ���f�����[�h�̕s��v�`�F�b�N
  if( !this->isModelMode( MODEL_H3D ) ){GFL_ASSERT_STOP(0);}
  return m_ModelDatas.h3d.m_pModel;
}
#endif // GFL_GRP_G3D_H3D_USE

//----------------------------------------------------------------------------
/**
 *	@brief  �n�`���f���̃��f�����\�[�X�擾
 *
 *	@retval ���f�����\�[�X
 */
//-----------------------------------------------------------------------------
const gfl::grp::g3d::Resource* System::GetResourceModel( void ) const
{
  // ���f�����[�h�̕s��v�`�F�b�N
  if( !this->isModelMode( MODEL_DEFAULT ) ){GFL_ASSERT_STOP(0);}
  return m_ModelDatas.def.m_pMdlRes;
}
#if GFL_GRP_G3D_H3D_USE
const gfl::grp::g3d::H3dResource* System::GetH3dResourceModel( void ) const
{
  // ���f�����[�h�̕s��v�`�F�b�N
  if( !this->isModelMode( MODEL_H3D ) ){GFL_ASSERT_STOP(0);}
  return m_ModelDatas.h3d.m_pMdlRes;
}
#endif // GFL_GRP_G3D_H3D_USE

//----------------------------------------------------------------------------
/**
 *	@brief  �n�`���f���̃e�N�X�`�����\�[�X�擾
 *
 *	@retval �e�N�X�`�����\�[�X
 */
//-----------------------------------------------------------------------------
const gfl::grp::g3d::Resource* System::GetResourceTexture( void ) const
{
  // ���f�����[�h�̕s��v�`�F�b�N
  if( !this->isModelMode( MODEL_DEFAULT ) ){GFL_ASSERT_STOP(0);}
  return m_ModelDatas.def.m_pTexRes;
}
#if GFL_GRP_G3D_H3D_USE
const gfl::grp::g3d::H3dResource* System::GetH3dResourceTexture( void ) const
{
  // ���f�����[�h�̕s��v�`�F�b�N
  if( !this->isModelMode( MODEL_H3D ) ){GFL_ASSERT_STOP(0);}
  return m_ModelDatas.h3d.m_pTexRes;
}
#endif // GFL_GRP_G3D_H3D_USE


//----------------------------------------------------------------------------
/**
 *	@brief  �ǂݍ��񂾃}�b�v�u���b�N�̃T�C�Y���擾
 */
//-----------------------------------------------------------------------------
s32 System::GetLoadFileSize( void ) const
{
  return m_DivLoad.GetLoadFileSize();
}

//-----------------------------------------------------------------------------
/**
 *    protected �����o�֐�
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  ���[�N�̃N���A����
 */
//-----------------------------------------------------------------------------
void System::Clear( void )
{
  GFL_ASSERT( m_Initialize == false );

  m_Initialize      = false;
  m_ModelMode       = MODEL_DEFAULT;
  m_pScene          = NULL;
  m_Visible          = true;
  m_Trans           = gfl::math::VEC3(0,0,0);
  m_pArcFile        = NULL;
  m_ArcFileOriginal = NULL;
  m_DataID          = 0;
  m_pMapData        = NULL;
  m_MapDataHeapSize = 0;
  m_FileType        = 0;
  m_pFileControl    = NULL;
  m_cpFileControls  = NULL;
  m_VramAddrTex     = 0;
  m_VramAddrPltt    = 0;
  this->ReleaseGlobalTexResource();
  m_DivLoadSize     = 0;
  m_LoadStatus      = LOAD_STATE_IDLING;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�v�o�b�t�@�@������
 *
 *	@param	param         �p�����[�^
 *	@param  p_sysHeap     �V�X�e���������p�q�[�v
 *	@param  p_deviceHeap  �f�o�C�X�������p�q�[�v
 */
//-----------------------------------------------------------------------------
void System::InitializeMapBuffer( const SetUpParam& param, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap )
{
  GFL_UNUSED( p_sysHeap );
  
  m_MapDataHeapSize = param.mapDataHeapSize;
  m_pMapData        = GflHeapAllocMemoryAlign( p_deviceHeap, m_MapDataHeapSize, 128 );
  m_LinkData.Initialize( m_pMapData );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�v�o�b�t�@�@�j��
 */
//-----------------------------------------------------------------------------
void System::FinalizeMapBuffer( void )
{
  GflHeapFreeMemory( m_pMapData );
  m_pMapData        = NULL;
  m_MapDataHeapSize = 0;
  m_LinkData.Initialize( NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���f���o�b�t�@�@������
 *
 *	@param	param         �p�����[�^
 *	@param  p_sysHeap     �V�X�e���������p�q�[�v
 *	@param  p_deviceHeap  �f�o�C�X�������p�q�[�v
 */
//-----------------------------------------------------------------------------
void System::InitializeModelBuffer( const SetUpParam& param, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap )
{
  GFL_UNUSED( param );
  GFL_UNUSED( p_deviceHeap );
  if( m_ModelMode == MODEL_DEFAULT ){
    m_ModelDatas.def.m_pTexRes = GFL_NEW(p_sysHeap) gfl::grp::g3d::Resource();
    m_ModelDatas.def.m_pMdlRes = GFL_NEW(p_sysHeap) gfl::grp::g3d::Resource();
    m_ModelDatas.def.m_pModel  = GFL_NEW(p_sysHeap) gfl::grp::g3d::Model();
  }
  else{
#if GFL_GRP_G3D_H3D_USE
    m_ModelDatas.h3d.m_pTexRes = GFL_NEW(p_sysHeap) gfl::grp::g3d::H3dResource();
    m_ModelDatas.h3d.m_pMdlRes = GFL_NEW(p_sysHeap) gfl::grp::g3d::H3dResource();
    m_ModelDatas.h3d.m_pModel  = GFL_NEW(p_sysHeap) gfl::grp::g3d::H3dModel();
#endif // GFL_GRP_G3D_H3D_USE
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���f���o�b�t�@ �j��
 */
//-----------------------------------------------------------------------------
void System::FinalizeModelBuffer( void )
{
  DeleteModel();
  
  if( m_ModelMode == MODEL_DEFAULT ){
    if(m_ModelDatas.def.m_pModel){
      GFL_DELETE m_ModelDatas.def.m_pModel;
      GFL_DELETE m_ModelDatas.def.m_pMdlRes;
      GFL_DELETE m_ModelDatas.def.m_pTexRes; 

      m_ModelDatas.def.m_pModel  = NULL;
      m_ModelDatas.def.m_pMdlRes = NULL;
      m_ModelDatas.def.m_pTexRes = NULL;
    }
  }
  else{
#if GFL_GRP_G3D_H3D_USE
    if(m_ModelDatas.h3d.m_pModel){
      GFL_DELETE m_ModelDatas.h3d.m_pModel;
      GFL_DELETE m_ModelDatas.h3d.m_pMdlRes;
      GFL_DELETE m_ModelDatas.h3d.m_pTexRes; 

      m_ModelDatas.h3d.m_pModel  = NULL;
      m_ModelDatas.h3d.m_pMdlRes = NULL;
      m_ModelDatas.h3d.m_pTexRes = NULL;
    }
#endif // GFL_GRP_G3D_H3D_USE
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���f���f�[�^�pVRAM�m�ۏ���
 *
 *	@param	param         �p�����[�^
 *	@param  p_sysHeap     �V�X�e���������p�q�[�v
 *	@param  p_deviceHeap  �f�o�C�X�������p�q�[�v
 */
//-----------------------------------------------------------------------------
void System::InitializeVramAddr( const SetUpParam& param, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap )
{
  // �n�[�h�ɂ���āA�ς��B
  GFL_UNUSED(p_sysHeap);
  GFL_UNUSED(p_deviceHeap);

  // Default�̃T�|�[�g�͂Ȃ�
  GFL_ASSERT(param.texVramSize == 0); //@check
  GFL_ASSERT(param.plttVramSize == 0); //@check
}

//----------------------------------------------------------------------------
/**
 *	@brief  VRAM�m�ۗ̈�̃N���A
 */
//-----------------------------------------------------------------------------
void System::FinalizeVramAddr( void )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  �������[�h�V�X�e���̏�����
 *
 *	@param	param         �p�����[�^
 *	@param  p_sysHeap     �V�X�e���q�[�v
 *	@param  p_deviceHeap  �f�o�C�X�q�[�v
 */
//-----------------------------------------------------------------------------
void System::InitializeDivLoad( const SetUpParam& param, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap )
{
  GFL_UNUSED( param );
  GFL_UNUSED( p_sysHeap );
  GFL_UNUSED( p_deviceHeap );
  
  m_DivLoadSize = param.divLoadSize;
  m_DivLoad.Initialize( m_pMapData, m_MapDataHeapSize, p_sysHeap, param.divMode, param.threadPri );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �������[�h�V�X�e���̔j��
 */
//-----------------------------------------------------------------------------
void System::FinalizeDivLoad( void )
{
  m_DivLoad.Finalize();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����ǂݍ��݂̏�ԑJ��
 */
//-----------------------------------------------------------------------------
void System::UpdateLoad( void )
{
  // �ǂݍ��݊����`�F�b�N
  if( (m_LoadStatus == LOAD_STATE_END) || 
      (m_LoadStatus == LOAD_STATE_IDLING) ){
    return ;
  }
  
  switch( m_LoadStatus ){

  case LOAD_STATE_START:             // �ǂݍ��݊J�n
    m_LoadStatus  = LOAD_STATE_LOAD;
    // break through
    
  case LOAD_STATE_LOAD:              // �ǂݍ��ݒ�
    if( m_DivLoad.ContinueFileLoad() != false ){
      // SetUp��
      m_LoadStatus  = LOAD_STATE_SETUP;

      // �V�O�l�`������t�@�C���R���g���[���擾
      m_pFileControl = m_cpFileControls->GetFileControl( m_LinkData.GetSignature() );
    }
    break;

  case LOAD_STATE_SETUP:             // �Z�b�g�A�b�v��
    
    if( m_pFileControl ){
      if( m_pFileControl->SetUp( this ) != false ){
        // �Z�b�g�A�b�v����
        m_LoadStatus = LOAD_STATE_END;
      }
    }else{
      m_LoadStatus = LOAD_STATE_END;
    }
    break;

  
  case LOAD_STATE_IDLING:        // �ҋ@��
  case LOAD_STATE_END:           // �����i�f�[�^�������Ă����ԁj
  default:
    break;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���f�����[�h�̈�v�`�F�b�N
 */
//-----------------------------------------------------------------------------
bool System::isModelMode( ModelMode mode ) const 
{
  if( mode == m_ModelMode ){
    return true;
  }
  return false;
}





} // namespace map
} // namespace grp
} // namespace gfl
