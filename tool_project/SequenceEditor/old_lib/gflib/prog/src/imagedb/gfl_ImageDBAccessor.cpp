//================================================================================
/**
 * @file   gfl_ImageDBAccessor.h
 * @brief  ImageDB�𗘗p����SD�J�[�h�ւ̎ʐ^���o�͗p�N���X
 * @author hisanao suzuki
 * @date   2012.2.13
 */
//================================================================================
#include <imagedb/gfl_ImageDBAccessor.h>


#ifdef GFL_USE_IMAGE_DB


namespace gfl {
namespace imagedb {


/**---------------------------------------------------------------------------
  @param �Ǝ��A���P�[�^
 *---------------------------------------------------------------------------*/
class MyAllocator : public nn::fnd::IAllocator
{
  public:
    void Initialize( void *buf, int size ){ m_Heap.Initialize( reinterpret_cast<uptr>( buf ), size ); }
    void Finalize(){ m_Heap.Finalize(); }

    virtual void* Allocate( size_t size, s32 alignment ){
      return m_Heap.Allocate( size, alignment );
    }
    virtual void Free( void* memory ){
      m_Heap.Free( memory );
    }

  private:
    nn::fnd::ExpHeap m_Heap;
};



//-------------------------------------------------------------------------------
/**
 * @@brief �R���X�g���N�^
 * @param  heap        �q�[�v
 * @param  deviceHeap  �f�o�C�X�q�[�v
 * @param  appUniqueId �A�v���P�[�V�����ŗLID(20bit)
 */
//-------------------------------------------------------------------------------
ImageDBAccessor::ImageDBAccessor( gfl::heap::HeapBase *heap, gfl::heap::HeapBase *deviceHeap, u32 appUniqueId ) :
  m_imgDbWork( NULL ),
  m_tmpAllocator( NULL ),
  m_heap( heap ),
  m_deviceHeap( deviceHeap ),
  m_topBuf( NULL ),
  m_topRightBuf( NULL),
  m_appUniqueId(appUniqueId),
  m_init( false ),
  m_resultDetail( imgdb::ResultSuccess )
{
  m_targetWidth  = 0;
  m_targetHeight = 0;
  m_format       = IMAGE_FORMAT;
  m_filePath[0]  = '\0';
}

//-------------------------------------------------------------------------------
/**
 * @brief �f�X�g���N�^
 */
//-------------------------------------------------------------------------------
ImageDBAccessor::~ImageDBAccessor()
{
  Finalize();
}

//-------------------------------------------------------------------------------
/**
 * @brief ImageDb���C�u�����̏����������i�J�n���ɂP�x�����R�[���j
 *
 * @param useHeapSize  ImageDb���C�u�����Ɋ��蓖�Ă�q�[�v�T�C�Y
 * @param targetWidth  �o�͌��\�[�X�ƂȂ��ʂ̉���
 * @param targetHeight �o�͌��\�[�X�ƂȂ��ʂ̏c��
 * @param isSimpleMode �C���[�W���[�e�B���e�B�[���g�p���邽�߂����̏��������s���Ȃ�true���w�肷��
 */
//-------------------------------------------------------------------------------
ImageDBAccessor::ResultCode ImageDBAccessor::Initialize( u32 useHeapSize, u32 targetWidth, u32 targetHeight, bool isSimpleMode )
{
  if( !m_init )
  {
    m_targetWidth  = targetWidth;
    m_targetHeight = targetHeight;

    if( m_imgDbWork == NULL )
    {
      m_imgDbWork = GflHeapAllocMemory( m_heap, useHeapSize );

      if( m_imgDbWork == NULL ){
        return E_RET_OUT_OF_MEMORY;
      }
    }

    if( m_tmpAllocator == NULL )
    {
      m_tmpAllocator = GFL_NEW( m_heap ) MyAllocator();

      if( m_tmpAllocator == NULL ){
        return E_RET_OUT_OF_MEMORY;
      }
      m_tmpAllocator->Initialize( m_imgDbWork, useHeapSize );
    }
    m_init = true;

    imgdb::Initialize( *m_tmpAllocator );
  }
  if( isSimpleMode ){
    return E_RET_SUCCESS;
  }

  return AllocateImageBuffer();
}


//-------------------------------------------------------------------------------
/**
 * @brief  �C���[�W�o�b�t�@�̊m��
 * @return �G���[�R�[�h
 */
//-------------------------------------------------------------------------------
ImageDBAccessor::ResultCode ImageDBAccessor::AllocateImageBuffer( void )
{
  // JPEG�ۑ��̊e��ݒ���s��
  size_t nImageSize   = GetBufferSize();

  FreeImageBuffer();

  GFL_PRINT( "ImageDBAccessor allocated size[%d]\n", nImageSize );

  m_topBuf = GflHeapAllocMemoryAlign( m_deviceHeap , nImageSize, IMAGE_ALIGN_SIZE );
  if( m_topBuf == NULL ){
    return E_RET_OUT_OF_MEMORY;
  }

  m_topRightBuf = GflHeapAllocMemoryAlign( m_deviceHeap , nImageSize, IMAGE_ALIGN_SIZE );
  if( m_topRightBuf == NULL ){
    return E_RET_OUT_OF_MEMORY;
  }
  return E_RET_SUCCESS;
}

//-------------------------------------------------------------------------------
/**
 * @brief �C���[�W�o�b�t�@�̉��
 */
//-------------------------------------------------------------------------------
void ImageDBAccessor::FreeImageBuffer( void )
{
  if( m_topBuf != NULL )
  {
    GflHeapFreeMemory( m_topBuf );
    m_topBuf = NULL;
  }
  if( m_topRightBuf != NULL ){
    GflHeapFreeMemory( m_topRightBuf );
    m_topRightBuf = NULL;
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief ImageDb���C�u�����̏I�������i�I�����ɂP�x�����R�[���j
 */
//-------------------------------------------------------------------------------
void ImageDBAccessor::Finalize( void )
{
  if( m_init )
  {
    imgdb::Finalize();

    FreeImageBuffer();

    if( m_tmpAllocator != NULL )
    {
      m_tmpAllocator->Finalize();
      GFL_SAFE_DELETE( m_tmpAllocator );
    }

    if( m_imgDbWork != NULL )
    {
      GflHeapFreeMemory( m_imgDbWork );
      m_imgDbWork = NULL;
    }
    m_init = false;
  }
}


//-------------------------------------------------------------------------------
/**
 * @brief �u���b�N�C���[�W���烊�j�A�C���[�W�֕ϊ�����
 *        ���{API��GPU�R�}���h�̂��߁A���s������1�t���[����v����I
 *
 * @param pSrcImg   �ϊ����ƂȂ�u���b�N�C���[�W�i�f�o�C�X�������K�{�I�j
 * @param pDstImg   ���j�A�C���[�W�i�[��i�f�o�C�X�������K�{�I�j
 * @param width     �C���[�W����
 * @param height    �C���[�W�c��
 * @param blockSize �u���b�N�C���[�W�̃T�C�Y�i8�A32�̂����ꂩ�j
 */
//-------------------------------------------------------------------------------
void ImageDBAccessor::ConvertImageBlock2Liner( const void * pSrcImg, void* pDstImg, u32 width, u32 height, u32 blockSize )
{
  nngxFlush3DCommand();
  nngxAddB2LTransferCommand( pSrcImg, width, height, gfl::grp::RENDER_COLOR_FORMAT_RGBA8,
                             pDstImg, width, height, gfl::grp::RENDER_COLOR_FORMAT_RGBA8,
                             NN_GX_ANTIALIASE_NOT_USED, GL_FALSE, blockSize );
}

//-------------------------------------------------------------------------------
/**
 * @brief ���j�A�C���[�W����u���b�N�C���[�W�֕ϊ�����
 *        ���{API��GPU�R�}���h�̂��߁A���s������1�t���[����v����I
 *
 * @param pSrcImg   �ϊ����ƂȂ郊�j�A�C���[�W�i�f�o�C�X�������K�{�I�j
 * @param pDstImg   �u���b�N�C���[�W�i�[��i�f�o�C�X�������K�{�I�j
 * @param width     �C���[�W����
 * @param height    �C���[�W�c��
 * @param blockSize �u���b�N�C���[�W�̃T�C�Y�i8�A32�̂����ꂩ�j
 */
//-------------------------------------------------------------------------------
void ImageDBAccessor::ConvertImageLiner2Block( const void * pSrcImg, void* pDstImg, u32 width, u32 height, u32 blockSize )
{
  nngxFlush3DCommand();
  nngxAddL2BTransferCommand( pSrcImg, pDstImg, width, height, gfl::grp::RENDER_COLOR_FORMAT_RGBA8, blockSize );
}

//-------------------------------------------------------------------------------
/**
 * @brief SD�J�[�h�ւ̎ʐ^�ۑ�
 *        �X�N���[���V���b�g��Pixel�C���[�W��n�����Ƃ�
 *        �ʐ^�f�[�^�Ƃ���SD�J�[�h�֕ۑ������B
 *
 *        �������ň����n��Pixel�f�[�^�̓��j�A�C���[�W�ł���K�v������B
 *          �u���b�N�C���[�W���烊�j�A�C���[�W�ւ̕ϊ���ConvertImageBlock2Liner()��
 *          �g�p���邱��
 *
 * @param pImageTop      ���Pixel�f�[�^(RGBA)
 * @param pImageTopRight ���̎��p�E���Pixel�f�[�^(RGBA)�BNULL���w�肷�邱�Ƃ��\�B
 * @param width          �C���[�W�f�[�^�̉���(2�ׂ̂���ł��邱�ƁI�j
 * @param height         �C���[�W�f�[�^�̏c��(2�ׂ̂���ł��邱�ƁI�j
 * @param pUserMakerNote ���[�U�[�m�[�g�p������
 * @param noteSize       ���[�U�[�m�[�g�p������T�C�Y
 * @param jpegQuality    Jpeg�̉摜�i���i1�`100)
 *
 * @return ���U���g�R�[�h
 */
//-------------------------------------------------------------------------------
ImageDBAccessor::ResultCode ImageDBAccessor::SaveToSDCard( const void* pImageTop, const void *pImageTopRight, u32 width, u32 height, const void* pUserMakerNote, u32 noteSize, int jpegQuality )
{
  if( !nn::fs::IsSdmcInserted() ){
     return E_RET_SDCARD_NOEXIST;
  }
  if( !nn::fs::IsSdmcWritable() ){
    return E_RET_SDCARD_READONLY;
  }
  if( (jpegQuality < 1) || (jpegQuality > 100) ){
    GFL_PRINT( "invalid jpegQuality(%d)!!\n", jpegQuality );
    return E_RET_FAILED;  // �i���w�肪�s��
  }

  CopyStereoView2Memory( const_cast<void*>(pImageTop),      const_cast<void*>(m_topBuf),      width, height, m_targetWidth, m_targetHeight );
  CopyStereoView2Memory( const_cast<void*>(pImageTopRight), const_cast<void*>(m_topRightBuf), width, height, m_targetWidth, m_targetHeight );

  if( pImageTopRight != NULL )
  {
    GFL_PRINT( "use MpSaver()\n" );
    imgdb::MpSaver* saver = GFL_NEW( m_heap ) imgdb::MpSaver();
    saver->SetJpegQuality( jpegQuality );
    m_resultDetail = SaveScreenshotToSDCard( saver, m_topBuf, m_topRightBuf, pUserMakerNote, noteSize );
    GFL_DELETE( saver );
  }
  else
  {
    GFL_PRINT( "use JpegSaver()\n" );
    imgdb::JpegSaver *saver = GFL_NEW( m_heap ) imgdb::JpegSaver();
    saver->SetJpegQuality( jpegQuality );
    m_resultDetail = SaveScreenshotToSDCard( saver, m_topBuf, pUserMakerNote, noteSize );
    GFL_DELETE( saver );
  }

  if( m_resultDetail != imgdb::ResultSuccess ){
    return E_RET_FAILED;
  }

  return E_RET_SUCCESS;
}

//-------------------------------------------------------------------------------
/**
 * @brief SD�J�[�h����̎ʐ^��ǂݍ���
 *        �i�[��̃o�b�t�@�T�C�Y�́AGetBufferSize()�ɂĎ擾���邱��
 *
 *        ���擾�����Pixel�f�[�^�̓��j�A�C���[�W�ł���B
 *          �u���b�N�C���[�W�ւ̕ϊ���ConvertImageLiner2Block()���g�p���邱��
 *
 * @param filePath       �ǂݍ��ݑΏۂƂȂ�ʐ^�̃t�@�C���p�X
 * @param pImageTop      �f�[�^�i�[��o�b�t�@
 * @param pImageTopRight ���̎��p�E���Pixel�f�[�^�i�[��o�b�t�@�BNULL���w�肷�邱�Ƃ��\�B
 * @param width          �i�[��o�b�t�@�̉摜����(2�ׂ̂���ł��邱�ƁI�j
 * @param height         �i�[��o�b�t�@�̉摜�c��(2�ׂ̂���ł��邱�ƁI�j
 *
 * @return ���U���g�R�[�h
 */
//-------------------------------------------------------------------------------
ImageDBAccessor::ResultCode ImageDBAccessor::LoadFromSDCard( const wchar_t * filePath, void* pImageTop, void *pImageTopRight, u32 width, u32 height )
{
  if( !nn::fs::IsSdmcInserted() ){
     return E_RET_SDCARD_NOEXIST;
  }
  if( !nn::fs::IsSdmcWritable() ){
    return E_RET_SDCARD_READONLY;
  }

  if( pImageTopRight != NULL )
  {
    GFL_PRINT( "use MpLoader()\n" );
    imgdb::MpLoader* loader = GFL_NEW( m_heap ) imgdb::MpLoader();
    m_resultDetail = LoadScreenshotFromSDCard( loader, filePath, m_topBuf, m_topRightBuf );
    GFL_DELETE( loader );
  }
  else
  {
    GFL_PRINT( "use JpegLoader()\n" );
    imgdb::JpegLoader *loader = GFL_NEW( m_heap ) imgdb::JpegLoader();
    m_resultDetail = LoadScreenshotFromSDCard( loader, filePath, m_topBuf );
    GFL_DELETE( loader );
  }
  CopyMemory2StereoView( m_topBuf,      pImageTop,      width, height, m_targetWidth, m_targetHeight );
  CopyMemory2StereoView( m_topRightBuf, pImageTopRight, width, height, m_targetWidth, m_targetHeight );

  return E_RET_SUCCESS;
}

//-------------------------------------------------------------------------------
/**
 * @brief ��ʂ�Pixel�C���[�W���o�͗p�o�b�t�@�փR�s�[
 * @param srcBuf       �]�����pPiexelImage
 * @param dstBuf       �]����pPiexelImage
 * @param srcWidth     ����PiexlImage�̉���(2�ׂ̂���ł��邱�Ɓj
 * @param srcHeight    ����PiexlImage�̏c��(2�ׂ̂���ł��邱�Ɓj
 * @param dstWidth     �o�͐�C���[�W�o�b�t�@�̉���
 * @param dstHeight    �o�͐�C���[�W�o�b�t�@�̏c��
 */
//-------------------------------------------------------------------------------
void ImageDBAccessor::CopyStereoView2Memory( void* srcBuf, void* dstBuf, u32 srcWidth, u32 srcHeight, u32 dstWidth, u32 dstHeight )
{
  if( srcBuf != NULL )
  {
    u8* base    = static_cast<u8*>(srcBuf);
    u8* copy    = static_cast<u8*>(dstBuf);
    u32 offsetX = srcWidth-dstWidth;

    for( int x=0;x<dstWidth;x++)
    {
      u32 baseAdr = (x+offsetX)*srcHeight;
      for( int y=0;y<dstHeight;y++)
      {
        const u32 copyAdr = (dstWidth*dstHeight)-((dstWidth-x)+y*dstWidth);
        gfl::std::MemCopy( &base[(baseAdr<<2)], &copy[(copyAdr<<2)], 4 );
        baseAdr++;
      }
    }
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief �C���[�W�o�b�t�@�����ʂ�Pixel�C���[�W�փR�s�[
 * @param srcBuf       �]�����pPiexelImage
 * @param dstBuf       �]����pPiexelImage
 * @param srcWidth     ����PiexlImage�̉���(512�𐄏��j
 * @param srcHeight    ����PiexlImage�̏c��(256�𐄏��j
 * @param dstWidth     �o�͐�C���[�W�o�b�t�@�̉���
 * @param dstHeight    �o�͐�C���[�W�o�b�t�@�̏c��
 */
//-------------------------------------------------------------------------------
void ImageDBAccessor::CopyMemory2StereoView( void* srcBuf, void* dstBuf, u32 srcWidth, u32 srcHeight, u32 dstWidth, u32 dstHeight )
{
  if( dstBuf != NULL )
  {
    u8 *base    = static_cast<u8*>(srcBuf);
    u8 *copy    = static_cast<u8*>(dstBuf);

    for( int y=0;y<dstHeight;y++)
    {
      u32 copyAdr = (dstHeight-(y+1))*srcWidth;
      for( int x=0;x<dstWidth;x++)
      {
        const u32 baseAdr = (dstWidth*dstHeight)-((dstWidth-x)+y*dstWidth);
        gfl::std::MemCopy( &base[(baseAdr<<2)], &copy[(copyAdr<<2)], 4 );
        copyAdr++;
      }
    }
  }
}

//-------------------------------------------------------------------------------
 /**
  * @brief  SD�J�[�h����̓ǂݍ��ݏ���(���̎��p�j
  * @param  loader         ���[�h�N���X
  * @param  filePath       �ǂݍ��ݑΏۃt�@�C���p�X
  * @param  memNormalBuf   �f�[�^�i�[��o�b�t�@
  * @param  memRightBuf    ���̎��p�E���Pixel�f�[�^�i�[��o�b�t�@�BNULL���w�肷�邱�Ƃ��\�B
  * @return imgdb::Result  �G���[�R�[�h
  */
 //-------------------------------------------------------------------------------
imgdb::Result ImageDBAccessor::LoadScreenshotFromSDCard( imgdb::MpLoader* loader, const wchar_t* filePath, void* memNormalBuf, void* memRightBuf )
{
  loader->SetFilePath( filePath );
  loader->SetFormat( m_format );
  loader->SetBuffer(memNormalBuf,memRightBuf, GetBufferSize() );

  imgdb::Result ret = loader->Load();

  #if GFL_DEBUG
  if( ret == imgdb::ResultSuccess ){
    GFL_PRINT( "LoadFromSDCard() imgdb::MpLoader : success[%d][%d]\n", loader->GetRawWidth(), loader->GetRawHeight() );
  }
  else{
    GFL_PRINT( "LoadFromSDCard() imgdb::MpLoader : faild\n" );
  }
  #endif

  return ret;
}

//-------------------------------------------------------------------------------
 /**
  * @brief  SD�J�[�h����̓ǂݍ��ݏ���(�񗧑̎��p�j
  * @param  loader         ���[�h�N���X
  * @param  filePath       �ǂݍ��ݑΏۃt�@�C���p�X
  * @param  memNormalBuf   �f�[�^�i�[��o�b�t�@
  * @return imgdb::Result  �G���[�R�[�h
  */
 //-------------------------------------------------------------------------------
imgdb::Result ImageDBAccessor::LoadScreenshotFromSDCard( imgdb::JpegLoader* loader, const wchar_t* filePath, void* memNormalBuf )
{
  loader->SetFilePath( filePath );
  loader->SetFormat( m_format );

  GFL_PRINT( "BufferSize[%u][%u]\n", GetBufferSize(), loader->GetBufferSize() );

  loader->SetBuffer(memNormalBuf, GetBufferSize() );

  imgdb::Result ret = loader->Load();

  #if GFL_DEBUG
  if( ret == imgdb::ResultSuccess ){
    GFL_PRINT( "LoadFromSDCard() imgdb::JpegLoader : success[%d][%d]\n", loader->GetRawWidth(), loader->GetRawHeight() );
  }
  else{
    GFL_PRINT( "LoadFromSDCard() imgdb::JpegLoader : faild\n" );
  }
  #endif

  return ret;
}

//-------------------------------------------------------------------------------
 /**
  * @brief SD�J�[�h�ۑ�����(���̎��p�j
  * @param  saver          �Z�[�u�N���X
  * @param  memNormalBuf   �摜�f�[�^�o�b�t�@(ARGB�t�H�[�}�b�g�j
  * @param  memRightBuf    �E�摜�f�[�^�o�b�t�@(ARGB�t�H�[�}�b�g�j
  * @param  pUserMakerNote ���[�U�[�m�[�g������(NULL�w�莞�͖��ݒ�Ƃ��Ĉ���)
  * @param  noteSize       ���[�U�[�m�[�g������T�C�Y�i�O�w�莞�͖��ݒ�Ƃ��Ĉ���)
  * @return imgdb::Result  �G���[�R�[�h
  */
 //-------------------------------------------------------------------------------
imgdb::Result ImageDBAccessor::SaveScreenshotToSDCard( imgdb::MpSaver* saver, const void* memNormalBuf, const void* memRightBuf, const void* pUserMakerNote, u32 noteSize )
{
  saver->SetRaw(memNormalBuf,memRightBuf, m_targetWidth, m_targetHeight, m_format);
  saver->SetTitleUniqueId( m_appUniqueId );
  saver->SetUserMakerNote(pUserMakerNote, noteSize );
  saver->SetScreenshotFlag( true );

  imgdb::Result ret = saver->Save();

  if( ret == imgdb::ResultSuccess ){
    ::std::wcscpy( m_filePath, saver->GetSavedFilePath() );
    GFL_PRINT( "SaveToSDCard() imgdb::MpSaver : success[%ls]\n", m_filePath );
  }
  else{
    GFL_PRINT( "SaveToSDCard() imgdb::MpSaver : faild\n" );
    m_filePath[0] = '\0';
  }
  return ret;
}

//-------------------------------------------------------------------------------
 /**
  * @brief SD�J�[�h�ۑ�����(�񗧑̎��p�j
  * @param  saver          �Z�[�u�N���X
  * @param  memNormalBuf   �摜�f�[�^�o�b�t�@(ARGB�t�H�[�}�b�g�j
  * @param  pUserMakerNote ���[�U�[�m�[�g������(NULL�w�莞�͖��ݒ�Ƃ��Ĉ���)
  * @param  noteSize       ���[�U�[�m�[�g������T�C�Y�i�O�w�莞�͖��ݒ�Ƃ��Ĉ���)
  * @return imgdb::Result  �G���[�R�[�h
  */
 //-------------------------------------------------------------------------------
imgdb::Result ImageDBAccessor::SaveScreenshotToSDCard( imgdb::JpegSaver* saver, const void* memNormalBuf, const void* pUserMakerNote, u32 noteSize )
{
  saver->SetRaw( memNormalBuf, m_targetWidth, m_targetHeight, m_format);
  saver->SetTitleUniqueId( m_appUniqueId );
  saver->SetUserMakerNote(pUserMakerNote, noteSize );
  saver->SetScreenshotFlag( true );

  imgdb::Result ret = saver->Save();

  if( ret == imgdb::ResultSuccess )
  {
    ::std::wcscpy( m_filePath, saver->GetSavedFilePath() );
    GFL_PRINT( "SaveToSDCard() imgdb::JpegSaver : success[%ls]\n", m_filePath );
  }
  else{
    GFL_PRINT( "SaveToSDCard() imgdb::JpegSaver : faild\n" );
    m_filePath[0] = '\0';
  }
  return ret;
}

//-------------------------------------------------------------------------------
/**
 * @brief  SaveToSDCard()���s���ʂ̏ڍׂȃG���[�R�[�h���擾����
 * @return �G���[�񋓒萔
 */
 //-------------------------------------------------------------------------------
imgdb::Result ImageDBAccessor::GetResultDetail( void )
{
  return m_resultDetail;
}

//-------------------------------------------------------------------------------
/**
 * @brief  SaveToSDCard()��SD�J�[�h�ɕۑ����ꂽ�ʐ^�t�@�C���̐�΃p�X���擾����
 *         ���ۑ��A�܂��͕ۑ����s���́A�󕶎����ԋp�����B
 * @return �ʐ^�t�@�C���̃t�@�C�����i��΃p�X�j
 */
 //-------------------------------------------------------------------------------
const wchar_t * ImageDBAccessor::GetSavedFilePath() const
{
  return m_filePath;
}

//-------------------------------------------------------------------------------
/**
 * @brief  �ǂݍ���Piexel�f�[�^���i�[����ׂɕK�v�ȃo�b�t�@�̃T�C�Y���擾����B
 * @return �ǂݍ��݂ɕK�v�ƂȂ�o�b�t�@�T�C�Y�i���̎����͂P��ʕ��̃o�b�t�@�T�C�Y�j�B
 */
 //-------------------------------------------------------------------------------
size_t ImageDBAccessor::GetBufferSize() const
{
  return imgdb::JpegMpBaseLoader::ComputeImageSize(m_targetWidth, m_targetHeight, m_format);
}


//-------------------------------------------------------------------------------
/**
 * @brief ImageDb�����p����SD�J�[�h�A�[�J�C�u���}�E���g����Ă��邩�`�F�b�N
 *
 * @return SD�J�[�h���}�E���g����Ă���Ȃ�true��ԋp
 */
//-------------------------------------------------------------------------------
bool ImageDBAccessor::IsMountSdmc( void ) const
{
  return imgdb::mnt::IsMountSdmc();
}


//-------------------------------------------------------------------------------
/**
 * @brief ImageDb�����p����SD�J�[�h�A�[�J�C�u���}�E���g����
 *
 * @return SD�J�[�h�̃}�E���g�ɐ��������Ȃ�true��ԋp
 */
//-------------------------------------------------------------------------------
bool ImageDBAccessor::MountSdmc( void )
{
  return imgdb::mnt::MountSdmc();
}


//-------------------------------------------------------------------------------
/**
 * @brief ImageDb�����p����SD�J�[�h�A�[�J�C�u���A���}�E���g����
 *
 * @note  SD�J�[�h�������ꂽ��A�v�����Ŗ{�֐����Ăяo���ăA���}�E���g���Ă�������
 */
//-------------------------------------------------------------------------------
void ImageDBAccessor::UnmountSdmc( void )
{
  imgdb::mnt::UnmountSdmc();
}


//-------------------------------------------------------------------------------
/**
 * @brief  �Ǘ�����Ă���SD�J�[�h���̎ʐ^�������擾����
 *
 * @return �ʐ^�����BSD�J�[�h���}�E���g����Ă��Ȃ��Ȃ畉�̒l��Ԃ�
 */
 //-------------------------------------------------------------------------------
int ImageDBAccessor::GetRegisteredPictureNum( void ) const
{
  if(!IsMountSdmc()){
    return -1;
  }
  return imgdb::util::GetRegisteredPictureNum();
}


//-------------------------------------------------------------------------------
/**
 * @brief  SD�J�[�h�w�ۑ��\�Ȏʐ^���������ς���
 *
 * @param  pResult �B�e�\�Ȃ�ResultSuccess()���A�B�e�s�\�Ȃ炻��ȊO���i�[
 *
 * @note   pResult�Ɋi�[�����l�́A�ȉ��̂Ƃ���B
 * @note   �EResultErrorOverNumImages
 * @note   �EResultErrorFileOperationFailure
 * @note   �EResultErrorNotExistStorage
 * @note   �EResultErrorWriteProtect
 * @note   �EResultErrorInsufficientStorageSpace
 * @note   �EResultErrorReadOnlyDbFile
 * @note   �EResultErrorNotAvailableStorage
 *
 * @return �ʐ^�̎c��B�e����
 */
 //-------------------------------------------------------------------------------
int ImageDBAccessor::EstimateRemainPictureNum( imgdb::Result* pResult )
{
  return imgdb::util::EstimateRemainPictureNum(pResult);
}


//-------------------------------------------------------------------------------
/**
 * @brief  SD�J�[�h���̎ʐ^���폜����
 * @param  pPath �폜�ΏۂƂȂ�ʐ^�ւ̃t�@�C���p�X
 * @return �G���[�񋓒萔
 */
 //-------------------------------------------------------------------------------
imgdb::Result ImageDBAccessor::DeletePicture( const wchar_t* pPath )
{
  return imgdb::util::DeletePicture( pPath, m_appUniqueId );
}




} // namespace imagedb
} // namespace gfl

#endif

