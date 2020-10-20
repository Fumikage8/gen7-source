//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_LytRes.cpp
 *	@brief  ���C�A�E�g���\�[�X�N���X
 *	@author	tomoya takahashi
 *	@date		2010.10.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <debug/gfl_Assert.h>
#include <debug/gfl_DebugPrint.h>

#include <grp/g2d/gfl_LytRes.h>

#include <gfl_Math.h>
#include <gfl_Std.h>


namespace gfl {
namespace grp {
namespace g2d {

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

//-----------------------------------------------------------------------------
/**
 *          gfl::grp::g2d::LytBinary�\��
 */
//-----------------------------------------------------------------------------
static const u32 LYT_BINARY_PATH_MAX = 64;  //�p�X���e�[�u���̃T�C�Y
struct _LytBinaryHeader {
  u32   filenum;
  char  nametbl[];
};

//-----------------------------------------------------------------------------
/**
 *          gfl::grp::g2d::LytPaneBinary�\��
 */
//-----------------------------------------------------------------------------
static const u32 LYT_PANE_BINARY_NAME_SHIFT_BIT = 5;                                 // �y�C�����Z�o�p�r�b�g��
static const u32 LYT_PANE_BINARY_NAME_STR_MAX = (1<<LYT_PANE_BINARY_NAME_SHIFT_BIT); // �y�C����������ő吔


struct _LytPaneBinaryHeader 
{
  u32   panenum;
  char  nametbl[];
};



//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^
 */
//-----------------------------------------------------------------------------
LytRes::LytRes( void )
{
  m_memory        = NULL;
  m_buff          = NULL;
  m_resAccessor   = NULL;
  for( u32 i=0; i<LYTRES_DEFAULT_FONT_MAX; ++i ){
    m_useDefaultFont[i]= false;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
LytRes::~LytRes()
{
  if( m_buff ){
    Release();
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g���\�[�X�̓ǂݍ���
 *
 *	@param  allocator �A���P�[�^
 *  @param  p_buff          �o�b�t�@�|�C���^
 *	@param  addTextureNum   �ǉ��o�^�@�e�N�X�`����
 */
//-----------------------------------------------------------------------------
void LytRes::Load( heap::NwAllocator* allocator, const void * cp_buff, u32 addTextureNum )
{
  heap::HeapBase* heap = allocator->GetHeapBase();
  s32 result;
  uptr addr;
  
  // �g�p����o�b�t�@��ݒ�
  m_buff = reinterpret_cast<const LytBinaryHeader*>(cp_buff);

  // �f�[�^��͂��s���A�����񕔂ƃA�[�J�C�u�f�[�^���ɕ�����B
  {
    //�@�܂��́A�t�@�C�������X�g
    m_nameTbl = m_buff->nametbl;
    addr = 4 + (m_buff->filenum * LYT_BINARY_PATH_MAX);

    // ���̎��Ƀy�C�������X�g
    m_paneData = reinterpret_cast<LytPaneBinaryHeader*>( reinterpret_cast<uptr>(m_buff) + addr );
    addr += (4 + (m_paneData->panenum * LYT_PANE_BINARY_NAME_STR_MAX));

    
    addr = math::RoundUp(addr, 128);  // �f�[�^��128byte�A���C�����g����Ă���B

    GFL_PRINT( "panenum %d addr = 0x%x\n", m_paneData->panenum, addr );
    m_arcData = reinterpret_cast<void*>( reinterpret_cast<uptr>(m_buff) + addr );
  }
  

  // ���\�[�X�A�N�Z�T�[����
  m_resAccessor = GFL_NEW(heap) nw::lyt::ArcResourceAccessor();
  result = m_resAccessor->Attach( m_arcData, "." );
  GFL_ASSERT_STOP( result );


  // �ǉ��o�^�e�N�X�`���L�[
  m_textureKeyNum = addTextureNum;
  if( m_textureKeyNum ){
    m_textureKeyArray = reinterpret_cast<nw::lyt::TextureKey*>GflHeapAllocMemory( heap, sizeof(nw::lyt::TextureKey) * m_textureKeyNum );
    m_addTextureNum   = 0;
  }
}


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
void LytRes::Load( heap::NwAllocator* allocator, fs::ArcFile* arcFile, u32 dataID, bool comp, u32 addTextureNum )
{
	if( comp == false ){
		Load( allocator, arcFile, dataID, addTextureNum );
	}else{
		LoadComp( allocator, arcFile, dataID, addTextureNum );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g���\�[�X�̓ǂݍ��݁i�񈳏k�f�[�^�j
 *
 *	@param  allocator �A���P�[�^
 *	@param  arcFile   �A�[�J�C�u�t�@�C��
 *	@param  dataID    �f�[�^ID
 *	@param  addTextureNum   �ǉ��o�^�@�e�N�X�`����
 */
//-----------------------------------------------------------------------------
void LytRes::Load( heap::NwAllocator* allocator, fs::ArcFile* arcFile, u32 dataID, u32 addTextureNum )
{
  u32 size;
  s32 result;
  heap::HeapBase* heap = allocator->GetHeapBase();

  size = arcFile->GetDataSize( dataID );

  m_memory = GflHeapAllocMemoryAlign( heap, size, 128 );

  result = arcFile->LoadData( dataID, m_memory );
  GFL_ASSERT_STOP( result >= 0 );

  // �g�p����o�b�t�@��ݒ�
  Load( allocator, m_memory, addTextureNum );
}

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
void LytRes::LoadComp( heap::NwAllocator* allocator, fs::ArcFile* arcFile, u32 dataID, u32 addTextureNum )
{
	m_memory = arcFile->LoadData( dataID, allocator->GetHeapBase(), 128 );

  // �g�p����o�b�t�@��ݒ�
  Load( allocator, m_memory, addTextureNum );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g���\�[�X�̔j��
 */
//-----------------------------------------------------------------------------
void LytRes::Release( void )
{
  // �ǉ��o�^�e�N�X�`���L�[���O��
  if( m_textureKeyNum ){
    for( int i=0; i<m_addTextureNum; i++ ){
      m_resAccessor->UnregistTexture( m_textureKeyArray[i] );
    }
    
    GflHeapFreeMemory(m_textureKeyArray);
    m_addTextureNum = 0;
    m_textureKeyNum = 0;
    m_textureKeyArray = NULL;
  }
  
  // Default�t�H���g���O��
  for( u32 i=0; i<LYTRES_DEFAULT_FONT_MAX; ++i ){
    if( m_useDefaultFont[i] ){
      m_resAccessor->UnregistFont( m_fontKey[i]);
      m_useDefaultFont[i]= false;
    }
  }
  
  // ���\�[�X�A�N�Z�T�[�j��
  GFL_DELETE m_resAccessor;
  // �o�b�t�@�j��
  if(m_memory){
    GflHeapFreeMemory(m_memory);
  }
  
  m_resAccessor = NULL;
  m_buff = NULL;
  m_memory = NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���C�A�E�g���\�[�X���ǂݍ��ݍς݂��`�F�b�N
 *
 *	@retval true    �ǂݍ��ݍς�
 *	@retval false   �ǂݍ��܂�Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
bool LytRes::IsDataIn(void) const
{
  return (m_buff != NULL);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�@�C�����擾
 *
 *	@return �t�@�C����
 */
//-----------------------------------------------------------------------------
u32 LytRes::GetFileNum( void ) const
{
  return m_buff->filenum;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�@�C�����擾
 *
 *	@param  lytDataID �f�[�^ID
 *
 *	@return �t�@�C����
 */
//-----------------------------------------------------------------------------
const char* LytRes::GetFileName( u32 lytDataID ) const
{
  u32 str_offset = lytDataID * LYT_BINARY_PATH_MAX;
  return &m_nameTbl[str_offset];
}


//-----------------------------------------------------------------------------
/**
 *  @brief  �ړI�̃��\�[�X���擾
 *
 *  @retval ���\�[�X�|�C���^  �Ȃ��ꍇ���i�łł�NULL���߂�B
 */
//-----------------------------------------------------------------------------
void* LytRes::GetResource( u32 lytDataID ) const
{
  void* ret = NULL;
  u32 str_offset = lytDataID * LYT_BINARY_PATH_MAX;

  //GFL_PRINT( "name %s\n", &m_nameTbl[str_offset] );

  ret = m_resAccessor->GetResource( nw::lyt::res::RESOURCETYPE_LAYOUT, &m_nameTbl[str_offset], NULL );
  GFL_ASSERT( ret ); // �ړI�̃��\�[�X���Ȃ��B @check
  return ret;
}

//-----------------------------------------------------------------------------
/**
 *      ���\�[�X�u���b�N�̎擾
 */
//-----------------------------------------------------------------------------
void* LytRes::GetAnimeResource( u32 lytDataID ) const
{
  void* ret = NULL;
  u32 str_offset = lytDataID * LYT_BINARY_PATH_MAX;

  //GFL_PRINT( "name %s\n", &m_nameTbl[str_offset] );

  ret = m_resAccessor->GetResource( nw::lyt::res::RESOURCETYPE_ANIMATION, &m_nameTbl[str_offset], NULL );
  GFL_ASSERT( ret ); // �ړI�̃��\�[�X���Ȃ��B @check
  return ret;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �e�N�X�`�������ׂă������ɔz�u����
 *
 *  @param  area    �]���惁�����@�iFCRAM/VRAMA/VRAMB�j
 */
//-----------------------------------------------------------------------------
void LytRes::TransferAllTexture( gfl::grp::MemoryArea area )
{
  int i;
  void* texres;
  nw::lyt::TextureInfo texInfo;
  u32 size;

  for( i=0; i<m_buff->filenum; i++ ){

    texres = m_resAccessor->GetResource( nw::lyt::res::RESOURCETYPE_TEXTURE, &m_nameTbl[i * LYT_BINARY_PATH_MAX], &size );
    if( texres && (size > 0) ){

      texInfo = nw::lyt::LoadTexture( texres, size, (area | GL_NO_COPY_FCRAM_DMP) );
      if( texInfo.IsValid() ){
        m_resAccessor->RegistTexture( &m_nameTbl[i * LYT_BINARY_PATH_MAX], texInfo );
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �e�N�X�`�����������ɔz�u����
 *
 *  @param  texArcIndex   �e�N�X�`�����C�A�E�g�f�[�^ID
 *  @param  area          �]���惁�����@�iFCRAM/VRAMA/VRAMB�j
 */
//-----------------------------------------------------------------------------
void LytRes::TransferIndexTexture( u32 texArcIndex, gfl::grp::MemoryArea area )
{
  void* texres;
  nw::lyt::TextureInfo texInfo;
  u32 size;


  texres = m_resAccessor->GetResource( nw::lyt::res::RESOURCETYPE_TEXTURE, &m_nameTbl[texArcIndex * LYT_BINARY_PATH_MAX], &size );
  if( texres && (size > 0) ){

    texInfo = nw::lyt::LoadTexture( texres, size, (area | GL_NO_COPY_FCRAM_DMP) );
    if( texInfo.IsValid() ){
      m_resAccessor->RegistTexture( &m_nameTbl[texArcIndex * LYT_BINARY_PATH_MAX], texInfo );
    }
  }
}



//-----------------------------------------------------------------------------
/**
 *  @brief  �e�N�X�`�����\�[�X���擾
 *
 *  @param  lytDataID   ���C�A�E�g�f�[�^ID
 *
 */
//-----------------------------------------------------------------------------
const LytResTextureInfo LytRes::GetTexture( u32 lytDataID ) const
{
  u32 str_offset = lytDataID * LYT_BINARY_PATH_MAX;
  
  return m_resAccessor->GetTexture( &m_nameTbl[str_offset] );
}


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
void* LytRes::GetTextureRes( u32 lytDataID, u32* size ) const
{
  u32 str_offset = lytDataID * LYT_BINARY_PATH_MAX;
  void* texres;

  texres = m_resAccessor->GetResource( nw::lyt::res::RESOURCETYPE_TEXTURE, &m_nameTbl[str_offset], size );
  if(texres && (*size > 0)){
    return texres;
  }

  return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���\�[�X�A�N�Z�T�[�̎擾
 */
//-----------------------------------------------------------------------------
LytResourceAccessor* LytRes::GetResourceAccessor(void) const
{
  return m_resAccessor;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �y�C�����̎擾
 */
//-----------------------------------------------------------------------------
const char * LytRes::GetPaneName( u32 paneEnum ) const
{
  if( paneEnum >= m_paneData->panenum ){
    GFL_ASSERT( paneEnum < m_paneData->panenum ); //@check
    return NULL;
  }

  return &m_paneData->nametbl[paneEnum<<LYT_PANE_BINARY_NAME_SHIFT_BIT];  // @suzuki
}

//----------------------------------------------------------------------------
/**
 *	@brief  �y�C�����C���f�b�N�X�����擾
 *
 *	@return �y�C�����C���f�b�N�X��
 */
//-----------------------------------------------------------------------------
u32 LytRes::GetPaneNum( void ) const
{
  return m_paneData->panenum;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���O�̃C���f�b�N�X���擾
 *
 *	@retval �C���f�b�N�X
 *	@retval LYTRES_PANENAME_ERROR ����Ȗ��O�Ȃ�
 */
//-----------------------------------------------------------------------------
u32 LytRes::GetPaneNameIndex( const char* name ) const
{ 
  int i;
  u32 str_offset = 0;
  int result;

  if( name ){
  
    for( i=0; i<m_paneData->panenum; i++ ){
      
      result = gfl::std::StringCmp( &m_paneData->nametbl[ str_offset ], name );
      if( result == 0 ){  //�������Ȃ炱��Index��Ԃ�
        return i;
      }
      str_offset += LYT_PANE_BINARY_NAME_STR_MAX;
    }
  }

  return LYTRES_PANENAME_ERROR;
}


//----------------------------------------------------------------------------
/**
 *	@brief  Default�t�H���g��ݒ�
 *
 *	@param	font    �t�H���g
 *	@param  name    ����
 *	@param  index   �C���f�b�N�X(0�`LYTRES_DEFAULT_FONT_MAX)
 */
//-----------------------------------------------------------------------------
void LytRes::SetDefaultFont( gfl::str::Font* font, const char* name, u32 index )
{
  if( index < LYTRES_DEFAULT_FONT_MAX ){
    if( m_useDefaultFont[index] == false ){

      //GFL_PRINT( "registFont1 [%x][%x][%x]\n", m_resAccessor, font, name );
      //GFL_PRINT( "registFont2 [%s]\n", name );

      m_fontKey[index] = m_resAccessor->RegistFont( name, font );
      m_useDefaultFont[index] = true;
      return ;
    }
    // �ݒ�ς�
    GFL_ASSERT(0); //@check
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �e�N�X�`����ǉ��o�^
 *
 *	@param	res     ���\�[�X
 */
//-----------------------------------------------------------------------------
void LytRes::RegistTexture( const LytRes* res, gfl::grp::MemoryArea area )
{
  u32 filenum = res->GetFileNum();
  nw::lyt::TextureInfo texInfo;
  const char* name;
  void* texres;
  u32 size;

  LytResourceAccessor * arc_resource = res->GetResourceAccessor();
  
  for( int i=0; i<filenum; i++ ){

    // �o�^�\���`�F�b�N
    if( m_textureKeyNum > m_addTextureNum ){
      name    = res->GetFileName( i );

      texres = arc_resource->GetResource( nw::lyt::res::RESOURCETYPE_TEXTURE, name, &size );

      if( texres ){
        texInfo = nw::lyt::LoadTexture( texres, size, (area | GL_NO_COPY_FCRAM_DMP) );

        //GFL_PRINT( "registTexture %s\n", name );
        m_textureKeyArray[m_addTextureNum] = m_resAccessor->RegistTexture( name, texInfo );
        m_addTextureNum ++;
      }
    }else{
      // �o�^�o�b�t�@�I�[�o�[
      GFL_ASSERT(0); //@check
    }
  }
}



} // g2d
} // grp
} // gfl
