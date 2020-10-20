//==========================================================================
/**
 * @file   gfl_Ro_Manager.cpp
 * @brief  ���I���W���[���Ǘ��N���X
 * @author Hisanao Suzuki
 */
//==========================================================================
#include <ro/gfl_RoManager.h>


#if GFL_ENABLE_RO


//-----------------------------------------------------------------------------
/**
*    Singleton�C���X�^���X�̎��̂��`
*/
//-----------------------------------------------------------------------------
SINGLETON_INSTANCE_DEFINITION(gfl::ro::RoManager);
template class gfl::base::SingletonAccessor<gfl::ro::RoManager>;




namespace gfl{
namespace ro{


/** ---------------------------------------------------------------------
//! @brief  ���I���W���[���̃��[�h�i�֐��Łj
//! @param  croFileName CRO�t�@�C����
//! @return ���I���W���[���ւ̃|�C���^
// ---------------------------------------------------------------------*/
nn::ro::Module* StartModuleFunc( const char* croFileName, bool is_lower )
{
  RoManager*      roMgr   = GFL_SINGLETON_INSTANCE( RoManager );
  nn::ro::Module* pModule;
  
  if( is_lower ){
    pModule = roMgr->LoadModuleLower( croFileName );
  }else{
    pModule = roMgr->LoadModule( croFileName );
  }

  roMgr->StartModule( pModule );

  return pModule;
}


/** ---------------------------------------------------------------------
//! @brief  �R���X�g���N�^
//! @param  ����
//! @return ����
// ---------------------------------------------------------------------*/
RoManager::RoManager( gfl::heap::HeapBase *modulesHeap ) :
  m_registList(NULL),
  m_modules(NULL),
  m_bssBuffer(NULL),
  m_moduleBuffer(NULL),
  m_heapForModules(modulesHeap),
  m_moduleMax(0),
  m_registNum(0),
  m_isInit(false)
{
  ::std::memset( &m_crsBuffer, 0, sizeof(m_crsBuffer) );
  ::std::memset( &m_crrBuffer, 0, sizeof(m_crrBuffer) );
}


/** ---------------------------------------------------------------------
//! @brief  �f�X�g���N�^
//! @param  ����
//! @return ����
// ---------------------------------------------------------------------*/
RoManager::~RoManager( void )
{
}


/** ---------------------------------------------------------------------
//! @brief  �t�@�C���ǂݍ��ݏ���
//! @param  heap        �q�[�v�Ǘ��N���X�ւ̃|�C���^
//! @param  fileName    �ǂݍ��݂����t�@�C����
//! @param  size        �t�@�C���T�C�Y���i�[�����̃|�C���^
//! @return �ǂݍ��܂ꂽ�t�@�C�����e���i�[�����o�b�t�@���
// ---------------------------------------------------------------------*/
void* RoManager::LoadFile( gfl::heap::HeapBase* heap, const char* fileName, size_t *size )
{
  gfl::fs::ReadFile file;
  bool              open_ret = file.Open(fileName);
  void*             buffer   = NULL;
  size_t            allocSize;

  GFL_ASSERT_STOP( open_ret );

  u32 fileSize = file.GetSize();

  // �m�ۂ��郁������4096�A���C������
  allocSize = nn::math::RoundUp(fileSize, 0x1000);
  buffer    = GflHeapAllocMemoryAlign4096( heap, allocSize );
  GFL_ASSERT_STOP( buffer );

  s32 readSize = file.Read( buffer, fileSize );
  file.Close();

  if( readSize != fileSize )
  {
    GflHeapFreeMemory(buffer);
    buffer = NULL;
    *size  = 0;
  }
  else{
    *size  = readSize;
  }

  return buffer;
}


/** ---------------------------------------------------------------------
//! @brief  ����������
//! @param  heap        �q�[�v�Ǘ��N���X�ւ̃|�C���^
//! @param  crsFileName CRS�t�@�C����
//! @param  crrFileName CRR�t�@�C����
//! @param  moduleMax   �o�^�\���W���[����
//! @return ����
// ---------------------------------------------------------------------*/
void RoManager::Initialize( gfl::heap::HeapBase* heap, const char* crsFileName, const char* crrFileName, u32 moduleMax )
{
  if( !m_isInit )
  {
    size_t crsFileSize = 0;
    size_t crrFileSize = 0;

    m_crsBuffer = LoadFile( heap, crsFileName, &crsFileSize );
    GFL_ASSERT_STOP( m_crsBuffer );
    m_crrBuffer = LoadFile( heap, crrFileName, &crrFileSize );
    GFL_ASSERT_STOP( m_crrBuffer );

    nn::ro::Initialize(m_crsBuffer, crsFileSize);

    m_registList = nn::ro::RegisterList(m_crrBuffer, crrFileSize);
    GFL_ASSERT_STOP( m_registList );

    m_registNum    = 0;
    m_moduleMax    = moduleMax;
    m_modules      = (nn::ro::Module**)GflHeapAllocMemory( heap, moduleMax * sizeof(nn::ro::Module*) );
    m_moduleBuffer = (void**)GflHeapAllocMemory( heap, moduleMax * sizeof(void*) );
    m_bssBuffer    = (bit8**)GflHeapAllocMemory( heap, moduleMax * sizeof(bit8*) );

    GFL_ASSERT_STOP( m_modules );
    GFL_ASSERT_STOP( m_moduleBuffer );
    GFL_ASSERT_STOP( m_bssBuffer );

    m_isInit = true;
  }
}


/** ---------------------------------------------------------------------
//! @brief  ���I���W���[���̃��[�h
//! @param  heap        �q�[�v�Ǘ��N���X�ւ̃|�C���^
//! @param  crsFileName CRS�t�@�C����
//! @param  crrFileName CRR�t�@�C����
//! @param  moduleMax   �o�^�\���W���[����
//! @param  fixLevel    ���W���[��������̃������̉���i�K�B
//! @return ����
// ---------------------------------------------------------------------*/
nn::ro::Module* RoManager::LoadModule( const char* croFileName, nn::ro::FixLevel fixLevel )
{
  if( m_moduleMax <= m_registNum )
  {
    GFL_PRINT( "RoManager::LoadModule() module is Buffer Over!\n" );
    GFL_ASSERT_STOP( false );
    return NULL;
  }

  size_t               croFileSize;
  gfl::heap::HeapBase* heap       = m_heapForModules;
  void*                croDataPtr = LoadFile( heap, croFileName, &croFileSize );
  uptr                 roUseAddr  = NULL;

  GFL_ASSERT_STOP( croDataPtr );

  // CRO �p�̃o�b�t�@���m��
  nn::ro::SizeInfo sizeInfo;
  nn::Result sizeResult = nn::ro::GetSizeInfo(&sizeInfo, croDataPtr);

  GFL_ASSERT_STOP(sizeResult.IsSuccess());

  // BSS�p�o�b�t�@�m��(8�o�C�g�A���C�����g�j
  bit8* bufferPtr = (bit8*)GflHeapAllocMemoryAlign( heap, sizeInfo.bufferSize, 8 );

  // ���W���[�������[�h
  nn::ro::Module* pModule = nn::ro::LoadModule(croDataPtr, croFileSize, bufferPtr, sizeInfo.bufferSize, true, fixLevel);
  GFL_ASSERT_STOP(pModule);

  // �����i�[
  m_modules[m_registNum]      = pModule;
  m_moduleBuffer[m_registNum] = croDataPtr;
  m_bssBuffer[m_registNum]    = bufferPtr;
  m_registNum++;

  GFL_PRINT( "RoManager::LoadModule(%s) [buffSize:0x%x][fileSize:0x%x]\n", croFileName, sizeInfo.bufferSize, croFileSize );

  return pModule;
}

//----------------------------------------------------------------------------
/**
//! @brief  ���I���W���[���̃��[�h  �q�[�v�̉�����m��
//! @param  heap        �q�[�v�Ǘ��N���X�ւ̃|�C���^
//! @param  crsFileName CRS�t�@�C����
//! @param  crrFileName CRR�t�@�C����
//! @param  moduleMax   �o�^�\���W���[����
//! @param  fixLevel    ���W���[��������̃������̉���i�K�B
//! @return ����
 */
//-----------------------------------------------------------------------------
nn::ro::Module* RoManager::LoadModuleLower( const char* croFileName, nn::ro::FixLevel fixLevel )
{
  if( m_moduleMax <= m_registNum )
  {
    GFL_PRINT( "RoManager::LoadModule() module is Buffer Over!\n" );
    GFL_ASSERT_STOP( false );
    return NULL;
  }

  size_t               croFileSize;
  gfl::heap::HeapBase* heap       = m_heapForModules->GetLowerHandle();
  void*                croDataPtr = LoadFile( heap, croFileName, &croFileSize );
  uptr                 roUseAddr  = NULL;

  GFL_ASSERT_STOP( croDataPtr );

  // CRO �p�̃o�b�t�@���m��
  nn::ro::SizeInfo sizeInfo;
  nn::Result sizeResult = nn::ro::GetSizeInfo(&sizeInfo, croDataPtr);

  GFL_ASSERT_STOP(sizeResult.IsSuccess());

  // BSS�p�o�b�t�@�m��(8�o�C�g�A���C�����g�j
  bit8* bufferPtr = (bit8*)GflHeapAllocMemoryAlign( heap, sizeInfo.bufferSize, 8 );

  // ���W���[�������[�h
  nn::ro::Module* pModule = nn::ro::LoadModule(croDataPtr, croFileSize, bufferPtr, sizeInfo.bufferSize, true, fixLevel);
  GFL_ASSERT_STOP(pModule);

  // �����i�[
  m_modules[m_registNum]      = pModule;
  m_moduleBuffer[m_registNum] = croDataPtr;
  m_bssBuffer[m_registNum]    = bufferPtr;
  m_registNum++;

  GFL_PRINT( "RoManager::LoadModule(%s) [buffSize:0x%x][fileSize:0x%x]\n", croFileName, sizeInfo.bufferSize, croFileSize );

  return pModule;
}



/** ---------------------------------------------------------------------
//! @brief  ���I���W���[���̊J�n
//! @param  targetModule �J�n�ΏۂƂȂ铮�I���W���[��
//! @param  isLinkCheck  �V���{�������N����������Ă��邩�������Ƀ`�F�b�N����Ȃ�true���w��
//! @attention ���s�Ɏ��s��������ł���ꍇ��ASSERT�Œ�~������
// ---------------------------------------------------------------------*/
void RoManager::StartModule( nn::ro::Module* targetModule, bool isLinkCheck )
{
  GFL_ASSERT_STOP_MSG( (targetModule != NULL), "RoManager::StartModule() invalid argument[argument is NULL]! \n" );

  s32 index = GetModuleIndex( targetModule );

  GFL_ASSERT_STOP_MSG( (index >= 0), "RoManager::StartModule() invalid argument[argument is not Loaded]! \n" );

  if( isLinkCheck )
  {
    bool isLinked = targetModule->IsAllSymbolResolved();
    GFL_ASSERT_STOP_MSG( isLinked, "RoManager::StartModule() invalid argument[argument is not Linked]! \n" );
  }

  targetModule->DoInitialize();
}


/** ---------------------------------------------------------------------
//! @brief  �o�^����Ă���S�Ă̓��I���W���[���̉��
//! @param  ����
//! @return ����
// ---------------------------------------------------------------------*/
void RoManager::DisposeAllModule( void )
{
  for( u32 i = 0; i < m_registNum; i++ )
  {
    if( m_modules[i] != NULL ){
      m_modules[i]->DoFinalize();
    }
  }
  for( u32 i = 0; i < m_registNum; i++ )
  {
    if( m_modules[i] != NULL )
    {
      m_modules[i]->Unload();
      m_modules[i] = NULL;
    }
    if( m_moduleBuffer[i] != NULL )
    {
      GflHeapFreeMemory( m_moduleBuffer[i] );
      m_moduleBuffer[i] = NULL;
    }
    if( m_bssBuffer[i] != NULL )
    {
      GflHeapFreeMemory( m_bssBuffer[i] );
      m_bssBuffer[i] = NULL;
    }
  }
  m_registNum = 0;
}


/** ---------------------------------------------------------------------
//! @brief  ���I���W���[���̉��
//! @param  targetModule ����ΏۂƂȂ铮�I���W���[��
//! @return ����
// ---------------------------------------------------------------------*/
void RoManager::DisposeModule( nn::ro::Module* targetModule )
{
  s32 index = GetModuleIndex( targetModule );

  if( (index >= 0) && (m_registNum > 0) )
  {
    if( targetModule != NULL )
    {
      GFL_PRINT("Start DoFinalize()\n");
      targetModule->DoFinalize();
      GFL_PRINT("Start Unload(%s)\n", targetModule->GetName() );
      targetModule->Unload();
      GFL_PRINT("Finish Unload\n");

      if( m_moduleBuffer != NULL )
      {
        if( m_moduleBuffer[index] != NULL )
        {
          GflHeapFreeMemory( m_moduleBuffer[index] );
          m_moduleBuffer[index] = NULL;
        }
      }
    }
    if( m_bssBuffer != NULL )
    {
      if( m_bssBuffer[index] != NULL )
      {
        GflHeapFreeMemory( m_bssBuffer[index] );
        m_bssBuffer[index] = NULL;
      }
    }

    // ���X�g���l�߂܂��傤
    for( u32 i = index; i < (m_registNum-1); i++ )
    {
      if( m_bssBuffer != NULL ){
        m_bssBuffer[i]    = m_bssBuffer[i+1];
      }
      if( m_moduleBuffer != NULL ){
        m_moduleBuffer[i] = m_moduleBuffer[i+1];
      }
      if( m_modules != NULL ){
        m_modules[i]      = m_modules[i+1];
      }
    }

    // �O�̂���NULL�ɐݒ�
    if( m_bssBuffer != NULL ){
      m_bssBuffer[m_registNum-1]    = NULL;
    }
    if( m_modules != NULL ){
      m_modules[m_registNum-1]      = NULL;
    }
    if( m_moduleBuffer != NULL ){
      m_moduleBuffer[m_registNum-1] = NULL;
    }
    m_registNum--;
  }
}


/** ---------------------------------------------------------------------
//! @brief  ���I���W���[���̉��
//! @param  targetModule ����ΏۂƂȂ铮�I���W���[��
//! @return �o�^���X�g�̃C���f�b�N�X�l�B0��菬�����l�̏ꍇ�͖����o�B
// ---------------------------------------------------------------------*/
s32 RoManager::GetModuleIndex( nn::ro::Module* targetModule )
{
  for( u32 i = 0; i < m_registNum; i++ )
  {
    if( m_modules[i] == targetModule ){
      return i;
    }
  }

  return -1;
}


/** ---------------------------------------------------------------------
//! @brief  �I������
//! @param  ����
//! @return ����
// ---------------------------------------------------------------------*/
void RoManager::Finalize( void )
{
  if( m_isInit )
  {
    DisposeAllModule();

    if( m_registList != NULL )
    {
      m_registList->Unregister();
      m_registList = NULL;
    }

    if( m_crsBuffer != NULL )
    {
      GflHeapFreeMemory( m_crsBuffer );
      m_crsBuffer = NULL;
    }
    if( m_crrBuffer != NULL )
    {
      GflHeapFreeMemory( m_crrBuffer );
      m_crrBuffer = NULL;
    }
    if( m_modules != NULL )
    {
      GflHeapFreeMemory( m_modules );
      m_modules = NULL;
    }
    if( m_moduleBuffer != NULL )
    {
      GflHeapFreeMemory( m_moduleBuffer );
      m_moduleBuffer = NULL;
    }
    if( m_bssBuffer != NULL )
    {
      GflHeapFreeMemory( m_bssBuffer );
      m_bssBuffer = NULL;
    }
    nn::ro::Finalize();

    m_isInit = false;
  }
}


} // namespace ro
} // namespace gfl




#endif // GFL_ENABLE_RO

