//======================================================================
/**
 * @file    SystemFont.cpp
 * @brief   �V�X�e���t�H���g�̏������E�擾
 * @author  ariizumi
 * @author  Hiroyuki Nakamura
 * @date    11/04/07
 *
 * @li  sango��system_font.cpp���ڐA
 */
//======================================================================
#include <../include/SystemFont.h>
#include <base/include/gfl2_Singleton.h>

#include <../../System/include/PokemonVersion.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/Font.gaix>

#if defined(GF_PLATFORM_CTR)
#include <nn/pl.h>
#endif


//-----------------------------------------------------------------------------
/**
*    Singleton�C���X�^���X�̎��̂��`
*/
//-----------------------------------------------------------------------------
#if GFL_RO_ENABLE
SINGLETON_INSTANCE_DEFINITION(view_print::SystemFont);
template class gfl2::base::SingletonAccessor<view_print::SystemFont>;
#endif

GFL_NAMESPACE_BEGIN(view_print)
//--------------------------------------------------------------
//
//--------------------------------------------------------------

SystemFont::SystemFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem ,bool bJpnFontLoadRegion)
  : mMainFont(NULL)
  , mStdFont(NULL)
  , mKujiraFontBuffer(NULL)
  , mKujiraFont(NULL)
  , mNumFont(NULL)
  , mNumFontBuffer(NULL)
  , mBattleFont(NULL)
  , mBattleFontBuffer(NULL)
{
  bool bSuccess;

  mpHeapDev = deviceMem;

#if defined(GF_PLATFORM_CTR)
  //�����̋��L�t�H���g�̓ǂݍ���
  if (nn::pl::GetSharedFontLoadState() != nn::pl::SHARED_FONT_LOAD_STATE_LOADED)
  {
    NN_UTIL_PANIC_IF_FAILED(nn::pl::InitializeSharedFont());

    // ���L�t�H���g�̃��[�h����������܂őҋ@
    while (nn::pl::GetSharedFontLoadState() != nn::pl::SHARED_FONT_LOAD_STATE_LOADED)
    {
      // ���L�t�H���g�̃��[�h�Ɏ��s���Ă��Ȃ����m�F
      if (nn::pl::GetSharedFontLoadState() == nn::pl::SHARED_FONT_LOAD_STATE_FAILED)
      {
          NN_TPANIC_("failed to load shared font!\n");
      }
      nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(10));
      ARIIZUMI_PRINT("loading SharedFont ...\n");
    }
    ARIIZUMI_PRINT("load SharedFont\n");
  }
  mStdFontBuffer = nn::pl::GetSharedFontAddress();
  // �t�H���g���\�[�X���Z�b�g���܂�
  mStdFont = GFL_NEW(heapMem)gfl2::str::ResFont();
  bSuccess = mStdFont->SetResource( mStdFontBuffer );
  NW_ASSERT( bSuccess );
#elif defined(GF_PLATFORM_WIN32)
  // @todo PC�͂ǂ����邩�H
  mStdFont = NULL;
#endif

  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

  { // �t�@�C���I�[�v��
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.fileName    = NULL;                                         // �t�@�C�����B�I�[��'\0'�ɂȂ��Ă��邱�ƁBarcId��ARCID_NULL�̂Ƃ��̂ݎg����B  // ****�L�[****
    req.arcId       = ARCID_FONT;                                   // ARCID�BARCID_NULL�̂Ƃ���fileName���g����B  // ****�L�[****
    req.prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // ���N�G�X�g�̎��s�D��x(�l�������������D��x������)
    req.heapForFile = heapMem;                                      // �q�[�v�B�t�@�C���C���X�^���X�̍쐬�ƍ����e�[�u���̍쐬�Ɏg�p�BClose����܂ō��L�̂��̂��ێ������B
    req.heapForReq  = heapMem;                                      // ���N�G�X�g�I���܂ňꎞ�I�Ɏg�p����q�[�v�B�����e�[�u���̍쐬�̂��߂ɂ��g�p����BArcSrc�̃C���X�^���X��\�[�X�e�[�u���̍쐬�̂��߂ɂ��g�p����B
    man->SyncArcFileOpen( req );
  }

  {
    u32 arcId;
    // ���{�ꂪ�ς܂�Ă��Ȃ��{�̂ɂ�镪��
    if(!bJpnFontLoadRegion)
    {
      arcId = GARC_Font_cbf_std_kujira_for_jpuseu_BFFNT;
    }
    else
    {
#if 1
      arcId = GARC_Font_cbf_std_kujira_for_jpuseu_BFFNT;
#else
      GFL_ASSERT( 0 );
      arcId = GARC_font_kujira_for_jpuseu_BCFNT;  // @todo ������
#endif
    }
    const gfl2::fs::ArcFile * arc_file = man->GetArcFile( ARCID_FONT );  //@todo ���݃t�H���g���傫���̂ŁA�������[���m�ۂł��Ȃ���Γ����Ȃ��悤�ɏ����������Ak.ohno
    size_t fileSize;
    {
      gfl2::fs::Result fs_result = arc_file->GetDataSize( &fileSize, arcId, heapMem );
      if( fs_result.IsSuccess() == false )
      {
        GFL_ASSERT(0);
      }
    }
    mKujiraFontBuffer = GflHeapAllocMemoryAlign( deviceMem, fileSize, nw::font::GlyphDataAlignment );
    if(mKujiraFontBuffer){ // �ǂݍ���
      gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq req;
      req.fileName          = NULL;                                         // �t�@�C�����B�I�[��'\0'�ɂȂ��Ă��邱�ƁBarcId��ARCID_NULL�̂Ƃ��̂ݎg����B
      req.arcId             = ARCID_FONT;                                   // ARCID�BARCID_NULL�̂Ƃ���fileName���g����B
      req.datId             = arcId;                                        // �A�[�J�C�u�t�@�C�����f�[�^ID
      req.prio              = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // ���N�G�X�g�̎��s�D��x(�l�������������D��x������)
      req.pBuf              = mKujiraFontBuffer;                            // �Ăяo�����ł��炩���߃��������m�ۂ��Ă������o�b�t�@�B�����Ƀt�@�C������ǂݍ��񂾓��e������B  // ****�L�[****
      req.bufSize           = fileSize;                                     // pBuf�̃T�C�Y�B�f�[�^�T�C�Y(�p�f�B���O���܂�)�ȏ�ɂ��ĉ������B
      req.pRealReadSize     = NULL;                                         // ���ۂɓǂݍ��񂾃T�C�Y�Bcompressed��true�̂Ƃ��𓀌�̃T�C�Y�BNULL�ł��悢�B
      req.heapForReq        = NULL;                                         // ���N�G�X�g�I���܂ňꎞ�I�Ɏg�p����q�[�v�B�w�u�����Łv���uArcSrc���g��Ȃ��Ƃ��v�x��NULL�ł悢�BArcSrc���g���Ƃ��̓t�@�C���I�[�v���ɂ��g�p����B
      req.heapForCompressed = NULL;                                         // �𓀂���ꍇ�Ɉꎞ�I�Ɏg�p����q�[�v�B�𓀂��Ȃ��ꍇ��NULL�ł悢�B
      man->SyncArcFileLoadDataBuf( req );
      mKujiraFont = GFL_NEW(heapMem) gfl2::str::ResFont();
      bSuccess = mKujiraFont->SetResource( mKujiraFontBuffer );
      NW_ASSERT( bSuccess );
    }
  }

  //kujira�p�t�H���g
  //mKujiraFont = gfl::str::CreateResFont( "rom:/font/kujira_base.bcfnt", heapMem, deviceMem );

  //���؂̋��L�t�H���g�쐬
#if defined(GF_PLATFORM_CTR)
  if(mKujiraFont){
    mMainFont = GFL_NEW(heapMem) nw::font::PairFont(mKujiraFont,mStdFont);
  }
#elif defined(GF_PLATFORM_WIN32)
  // @todo PC�͂ǂ����邩�H
  mMainFont = mKujiraFont;
#endif
  //mMainFont = mSharedFont;
  //mMainFont = GFL_NEW(heapMem)nw::font::PairFont(mKujiraFont,mStdFont);
  //mMainFont = mStdFont;

/*
  nw::font::CharWidths charData;
  charData = mKujiraFont->GetDefaultCharWidths();
  ARIIZUMI_PRINT("Kujira[%d][%d][%d][%d][%d]\n",charData.left,charData.glyphWidth ,charData.charWidth,mKujiraFont->GetWidth(),mKujiraFont->GetHeight() );
  ARIIZUMI_PRINT("Kujira[%d][%d][%d]\n",mKujiraFont->GetBaselinePos(),mKujiraFont->GetCellHeight(),mKujiraFont->GetCellWidth() );
  ARIIZUMI_PRINT("Kujira[%d]\n",mKujiraFont->GetTextureFormat() );
  charData = mStdFont->GetDefaultCharWidths();
  ARIIZUMI_PRINT("Shared[%d][%d][%d][%d][%d]\n",charData.left,charData.glyphWidth ,charData.charWidth,mStdFont->GetWidth(),mStdFont->GetHeight() );
  ARIIZUMI_PRINT("Shared[%d][%d][%d]\n",mStdFont->GetBaselinePos(),mStdFont->GetCellHeight(),mStdFont->GetCellWidth() );
  ARIIZUMI_PRINT("Shared[%d]\n",mStdFont->GetTextureFormat() );
  charData = mMainFont->GetDefaultCharWidths();
  ARIIZUMI_PRINT("Pair  [%d][%d][%d][%d][%d]\n",charData.left,charData.glyphWidth ,charData.charWidth,mMainFont->GetWidth(),mMainFont->GetHeight() );
  ARIIZUMI_PRINT("Pair  [%d][%d][%d]\n",mMainFont->GetBaselinePos(),mMainFont->GetCellHeight(),mMainFont->GetCellWidth() );
  ARIIZUMI_PRINT("Pair  [%d]\n",mMainFont->GetTextureFormat() );
  ARIIZUMI_PRINT("[%x][%d]\n",mFontArc,mFontArc->GetDataCount());
*/  

}

SystemFont::~SystemFont()
{
  Terminate();
  GflHeapSafeFreeMemory(mKujiraFontBuffer);
  ///GFL_SAFE_DELETE( mKujiraFontBuffer );
  ///GFL_SAFE_DELETE( mMainFont );
  GFL_SAFE_DELETE( mKujiraFont );
  GFL_SAFE_DELETE( mStdFont );

  { // �t�@�C���N���[�Y
    gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    req.fileName   = NULL;                                        // �t�@�C�����B�I�[��'\0'�ɂȂ��Ă��邱�ƁBarcId��ARCID_NULL�̂Ƃ��̂ݎg����B  // ****�L�[****
    req.arcId      = ARCID_FONT;                                  // ARCID�BARCID_NULL�̂Ƃ���fileName���g����B  // ****�L�[****
    req.prio       = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL; // ���N�G�X�g�̎��s�D��x(�l�������������D��x������)
    req.heapForReq = NULL;                                        // ���N�G�X�g�I���܂ňꎞ�I�Ɏg�p����q�[�v�B�����łł�NULL�ł悢�B
    man->SyncArcFileClose( req );
  }
}

void SystemFont::Initialize(void)
{
/* @todo font2�ŏ������@�\

  // �`��p�o�b�t�@�̐ݒ�B (�������x�������̂��߁B)
  // ������gx�n�̊֐����K�v�ȈׁAnngxInitialize��(g3d::GraphicSystem��������)
  gfl2::heap::HeapBase *heap = mpHeapDev;

  {
    const size_t drawBufferSize = nw::font::ResFont::GetDrawBufferSize( mStdFontBuffer );
    void* drawBuffer = GflHeapAllocMemoryAlign( heap , drawBufferSize, 128 );
    ARIIZUMI_PRINT("SystemFont Alloc drawbuffer[%d]byte\n",drawBufferSize);
    OHNO_PRINT("SystemFont Alloc drawbuffer[%d]byte\n",drawBufferSize);
    NW_NULL_ASSERT(drawBuffer);
    mStdFont->SetDrawBuffer( drawBuffer );
  }
  {
    const size_t drawBufferSize = nw::font::ResFont::GetDrawBufferSize( mKujiraFontBuffer );
    void* drawBuffer = GflHeapAllocMemoryAlign( heap , drawBufferSize, 128 );
    ARIIZUMI_PRINT("SystemFont Alloc drawbuffer[%d]byte\n",drawBufferSize);
    OHNO_PRINT("SystemFont Alloc drawbuffer[%d]byte\n",drawBufferSize);
    NW_NULL_ASSERT(drawBuffer);
    mKujiraFont->SetDrawBuffer( drawBuffer );
  }
*/
}

void SystemFont::Terminate()
{
/* @todo font2�ŏ������@�\

  // �`��p�o�b�t�@�̉����Ɖ��
  {
    void *const drawBuffer = mKujiraFont->SetDrawBuffer( NULL );
    if ( drawBuffer != NULL )
    {
      GflHeapFreeMemory( drawBuffer );
    }
  }
  {
    void *const drawBuffer = mStdFont->SetDrawBuffer( NULL );
    if ( drawBuffer != NULL )
    {
      GflHeapFreeMemory( drawBuffer );
    }
  }
*/
}

//�g���t�H���g�֘A
//----------------------------------------------------------------------------
/**
 *  @brief  num�t�H���g�̍쐬
 *          ���ɃO���[�o���֐��ł�����܂��B���������g���Ă��������B
 *
 *  @param  heap::HeapBase *heap  �g�p�q�[�v
 */
//-----------------------------------------------------------------------------
void SystemFont::CreateNumFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem)
{
  if( mNumFont == NULL )
  {
    gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

    const gfl2::fs::ArcFile * arc_file = man->GetArcFile( ARCID_FONT );
    size_t fileSize;
    {
      gfl2::fs::Result fs_result = arc_file->GetDataSize( &fileSize, GARC_Font_num_font_BFFNT, heapMem );
      if( fs_result.IsSuccess() == false )
      {
        GFL_ASSERT(0);
      }
    }
    mNumFontBuffer = GflHeapAllocMemoryAlign( deviceMem, fileSize, nw::font::GlyphDataAlignment );

    gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq req;
    req.fileName          = NULL;                                         // �t�@�C�����B�I�[��'\0'�ɂȂ��Ă��邱�ƁBarcId��ARCID_NULL�̂Ƃ��̂ݎg����B
    req.arcId             = ARCID_FONT;                                   // ARCID�BARCID_NULL�̂Ƃ���fileName���g����B
    req.datId             = GARC_Font_num_font_BFFNT;                     // �A�[�J�C�u�t�@�C�����f�[�^ID
    req.prio              = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // ���N�G�X�g�̎��s�D��x(�l�������������D��x������)
    req.pBuf              = mNumFontBuffer;                               // �Ăяo�����ł��炩���߃��������m�ۂ��Ă������o�b�t�@�B�����Ƀt�@�C������ǂݍ��񂾓��e������B  // ****�L�[****
    req.bufSize           = fileSize;                                     // pBuf�̃T�C�Y�B�f�[�^�T�C�Y(�p�f�B���O���܂�)�ȏ�ɂ��ĉ������B
    req.pRealReadSize     = NULL;                                         // ���ۂɓǂݍ��񂾃T�C�Y�Bcompressed��true�̂Ƃ��𓀌�̃T�C�Y�BNULL�ł��悢�B
    req.heapForReq        = NULL;                                         // ���N�G�X�g�I���܂ňꎞ�I�Ɏg�p����q�[�v�B�w�u�����Łv���uArcSrc���g��Ȃ��Ƃ��v�x��NULL�ł悢�BArcSrc���g���Ƃ��̓t�@�C���I�[�v���ɂ��g�p����B
    req.heapForCompressed = NULL;                                         // �𓀂���ꍇ�Ɉꎞ�I�Ɏg�p����q�[�v�B�𓀂��Ȃ��ꍇ��NULL�ł悢�B
    man->SyncArcFileLoadDataBuf( req );

    mNumFont = GFL_NEW(heapMem) gfl2::str::ResFont();
    const bool bSuccess = mNumFont->SetResource( mNumFontBuffer );
    NW_ASSERT( bSuccess );

/* @todo font2�ŏ������@�\
    const size_t drawBufferSize = nw::font::ResFont::GetDrawBufferSize( mNumFontBuffer );
    void* drawBuffer = GflHeapAllocMemoryAlign( deviceMem , drawBufferSize, 128 );
    ARIIZUMI_PRINT("NumFontSize[%d] buffer[%d]\n",fileSize,drawBufferSize );
    NW_NULL_ASSERT(drawBuffer);
    mNumFont->SetDrawBuffer( drawBuffer );
*/
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  num�t�H���g�̔j��
 *          ���ɃO���[�o���֐��ł�����܂��B���������g���Ă��������B
 */
//-----------------------------------------------------------------------------
void SystemFont::DeleteNumFont(void)
{
  if( mNumFont )
  {
/* @todo font2�ŏ������@�\
    void *const drawBuffer = mNumFont->SetDrawBuffer( NULL );
    if ( drawBuffer != NULL )
    {
      GflHeapFreeMemory( drawBuffer );
    }
*/
    GFL_DELETE mNumFont;
    GflHeapFreeMemory( mNumFontBuffer );

    mNumFont = NULL;
    mNumFontBuffer = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �o�g���t�H���g�̍쐬
 *          ���ɃO���[�o���֐��ł�����܂��B���������g���Ă��������B
 *
 *  @param  heap::HeapBase *heap  �g�p�q�[�v
 */
//-----------------------------------------------------------------------------
void SystemFont::CreateBattleFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem)
{
  if( mBattleFont == NULL )
  {
    gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

    const gfl2::fs::ArcFile * arc_file = man->GetArcFile( ARCID_FONT );
    size_t fileSize;
    {
      gfl2::fs::Result fs_result = arc_file->GetDataSize( &fileSize, GARC_Font_kujira_battle_BFFNT, heapMem );
      if( fs_result.IsSuccess() == false )
      {
        GFL_ASSERT(0);
      }
    }
    mBattleFontBuffer = GflHeapAllocMemoryAlign( deviceMem, fileSize, nw::font::GlyphDataAlignment );

    gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq req;
    req.fileName          = NULL;                                         // �t�@�C�����B�I�[��'\0'�ɂȂ��Ă��邱�ƁBarcId��ARCID_NULL�̂Ƃ��̂ݎg����B
    req.arcId             = ARCID_FONT;                                   // ARCID�BARCID_NULL�̂Ƃ���fileName���g����B
    req.datId             = GARC_Font_kujira_battle_BFFNT;                // �A�[�J�C�u�t�@�C�����f�[�^ID
    req.prio              = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // ���N�G�X�g�̎��s�D��x(�l�������������D��x������)
    req.pBuf              = mBattleFontBuffer;                            // �Ăяo�����ł��炩���߃��������m�ۂ��Ă������o�b�t�@�B�����Ƀt�@�C������ǂݍ��񂾓��e������B  // ****�L�[****
    req.bufSize           = fileSize;                                     // pBuf�̃T�C�Y�B�f�[�^�T�C�Y(�p�f�B���O���܂�)�ȏ�ɂ��ĉ������B
    req.pRealReadSize     = NULL;                                         // ���ۂɓǂݍ��񂾃T�C�Y�Bcompressed��true�̂Ƃ��𓀌�̃T�C�Y�BNULL�ł��悢�B
    req.heapForReq        = NULL;                                         // ���N�G�X�g�I���܂ňꎞ�I�Ɏg�p����q�[�v�B�w�u�����Łv���uArcSrc���g��Ȃ��Ƃ��v�x��NULL�ł悢�BArcSrc���g���Ƃ��̓t�@�C���I�[�v���ɂ��g�p����B
    req.heapForCompressed = NULL;                                         // �𓀂���ꍇ�Ɉꎞ�I�Ɏg�p����q�[�v�B�𓀂��Ȃ��ꍇ��NULL�ł悢�B
    man->SyncArcFileLoadDataBuf( req );

    mBattleFont = GFL_NEW(heapMem) gfl2::str::ResFont();
    const bool bSuccess = mBattleFont->SetResource( mBattleFontBuffer );
    NW_ASSERT( bSuccess );

/* @todo font2�ŏ������@�\
    const size_t drawBufferSize = nw::font::ResFont::GetDrawBufferSize( mBattleFontBuffer );
    void* drawBuffer = GflHeapAllocMemoryAlign( deviceMem , drawBufferSize, 128 );
    ARIIZUMI_PRINT("BattleFontSize[%d] buffer[%d]\n",fileSize,drawBufferSize );
    NW_NULL_ASSERT(drawBuffer);
    mBattleFont->SetDrawBuffer( drawBuffer );
*/
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �o�g���t�H���g�̔j��
 *          ���ɃO���[�o���֐��ł�����܂��B���������g���Ă��������B
 */
//-----------------------------------------------------------------------------
void SystemFont::DeleteBattleFont(void)
{
  if( mBattleFont )
  {
/* @todo font2�ŏ������@�\
    void *const drawBuffer = mBattleFont->SetDrawBuffer( NULL );
    if ( drawBuffer != NULL )
    {
      GflHeapFreeMemory( drawBuffer );
    }
*/
    GFL_DELETE mBattleFont;
    GflHeapFreeMemory( mBattleFontBuffer );

    mBattleFont = NULL;
    mBattleFontBuffer = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �_���t�H���g�̍쐬
 *          ���ɃO���[�o���֐��ł�����܂��B���������g���Ă��������B
 *
 *  @param  heap::HeapBase *heap  �g�p�q�[�v
 */
//-----------------------------------------------------------------------------
/*
void SystemFont::CreateTenjiFont(gfl::heap::HeapBase *heapMem , gfl::heap::HeapBase *deviceMem)
{
  if( mTenjiFont == NULL )
  {
    u32 fileSize = mFontArc->GetDataSize(GARC_font_tenji_font_BCFNT);
    ARIIZUMI_PRINT("[%x][%d]\n",mFontArc,mFontArc->GetDataCount());
    mTenjiFontBuffer = GflHeapAllocMemoryAlign( deviceMem, fileSize, nw::font::GlyphDataAlignment );
    mFontArc->LoadData( GARC_font_tenji_font_BCFNT , mTenjiFontBuffer );

    mTenjiFont = GFL_NEW(heapMem)gfl::str::ResFont();
    const bool bSuccess = mTenjiFont->SetResource( mTenjiFontBuffer );
    NW_ASSERT( bSuccess );

    const size_t drawBufferSize = nw::font::ResFont::GetDrawBufferSize( mTenjiFontBuffer );
    void* drawBuffer = GflHeapAllocMemoryAlign( deviceMem , drawBufferSize, 128 );
    ARIIZUMI_PRINT("TenjiFontSize[%d] buffer[%d]\n",fileSize,drawBufferSize );
    NW_NULL_ASSERT(drawBuffer);
    mTenjiFont->SetDrawBuffer( drawBuffer );
  }
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  �_���t�H���g�̔j��
 *          ���ɃO���[�o���֐��ł�����܂��B���������g���Ă��������B
 */
//-----------------------------------------------------------------------------
/*
void SystemFont::DeleteTenjiFont(void)
{
  if( mTenjiFont )
  {
    void *const drawBuffer = mTenjiFont->SetDrawBuffer( NULL );
    if ( drawBuffer != NULL )
    {
      GflHeapFreeMemory( drawBuffer );
    }
    GFL_DELETE mTenjiFont;
    GflHeapFreeMemory( mTenjiFontBuffer );

    mTenjiFont = NULL;
    mTenjiFontBuffer = NULL;
  }
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  �A�v�����̏�����
 *          ���vgx������
 *
 *  @param  heap::HeapBase *heap  �g�p�q�[�v
 */
//-----------------------------------------------------------------------------
void SystemFont_Initialize(void)
{
  GFL_SINGLETON_INSTANCE(SystemFont)->Initialize();
}

//----------------------------------------------------------------------------
/**
 *  @brief  �A�v�����̉��
 */
//-----------------------------------------------------------------------------
void SystemFont_Terminate()
{
  GFL_SINGLETON_INSTANCE(SystemFont)->Terminate();
}

//----------------------------------------------------------------------------
/**
 *  @brief  �t�H���g�̎擾
 *
 *  @return gfl::str::Font �t�H���g
 */
//-----------------------------------------------------------------------------
gfl2::str::Font* SystemFont_GetFont()
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->GetFont();
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���L�t�H���g�o�b�t�@�̎擾
 *          ResFont����Ȃ��Ǝ擾�ł��Ȃ��̂ŁA���ĉ��t�H���g�̃o�b�t�@
 *
 *  @return void* �t�H���g�o�b�t�@
 */
//-----------------------------------------------------------------------------
void* SystemFont_GetFontBuffer()
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->GetFontBuffer();
}

//�����t�H���g�̂ݎ擾(�����p)
gfl2::str::Font* SystemFont_GetSharedFont()
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->GetSharedFont();
}


//----------------------------------------------------------------------------
/**
 *  @brief  num�t�H���g�̍쐬
 *  @param  heap::HeapBase *heap  �g�p�q�[�v
 */
//-----------------------------------------------------------------------------
void SystemFont_CreateNumFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem)
{
  GFL_SINGLETON_INSTANCE(SystemFont)->CreateNumFont(heapMem,deviceMem);
}

//----------------------------------------------------------------------------
/**
 *  @brief  num�t�H���g�̔j��
 */
//-----------------------------------------------------------------------------
void SystemFont_DeleteNumFont(void)
{
  GFL_SINGLETON_INSTANCE(SystemFont)->DeleteNumFont();
}

//----------------------------------------------------------------------------
/**
 *  @brief  num�t�H���g�̎擾
 *  @return gfl::str::Font �t�H���g
 */
//-----------------------------------------------------------------------------
gfl2::str::Font* SystemFont_GetNumFont(void)
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->GetNumFont();
}

//----------------------------------------------------------------------------
/**
 *  @brief  num���L�t�H���g�o�b�t�@�̎擾
 *  @return void* �t�H���g�o�b�t�@
 */
//-----------------------------------------------------------------------------
void* SystemFont_GetNumFontBuffer(void)
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->GetNumFontBuffer();
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ǉ����ꂽnum�t�H���g��������
 */
//-----------------------------------------------------------------------------
void SystemFont_ResetNumFontInfo(void)
{
  gfl2::lyt::LytSys::ResetDefaultFontInfo(print::DEFAULT_FONT_LAYOUT_NUM_FONT);
}

//�g���t�H���g�֘A
//----------------------------------------------------------------------------
/**
 *  @brief  �o�g���t�H���g�̍쐬
 *  @param  heap::HeapBase *heap  �g�p�q�[�v
 */
//-----------------------------------------------------------------------------
void SystemFont_CreateBattleFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem)
{
  GFL_SINGLETON_INSTANCE(SystemFont)->CreateBattleFont(heapMem,deviceMem);
}

//----------------------------------------------------------------------------
/**
 *  @brief  �o�g���t�H���g�̔j��
 */
//-----------------------------------------------------------------------------
void SystemFont_DeleteBattleFont(void)
{
  GFL_SINGLETON_INSTANCE(SystemFont)->DeleteBattleFont();
}

//----------------------------------------------------------------------------
/**
 *  @brief  �o�g���t�H���g�̎擾
 *  @return gfl::str::Font �t�H���g
 */
//-----------------------------------------------------------------------------
gfl2::str::Font* SystemFont_GetBattleFont(void)
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->GetBattleFont();
}

//----------------------------------------------------------------------------
/**
 *  @brief  �o�g�����L�t�H���g�o�b�t�@�̎擾
 *  @return void* �t�H���g�o�b�t�@
 */
//-----------------------------------------------------------------------------
void* SystemFont_GetBattleFontBuffer(void)
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->GetBattleFontBuffer();
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ǉ����ꂽ�o�g���t�H���g��������
 */
//-----------------------------------------------------------------------------
void SystemFont_ResetBattleFontInfo(void)
{
  gfl2::lyt::LytSys::ResetDefaultFontInfo(print::DEFAULT_FONT_LAYOUT_BATTLE_FONT);
}

//----------------------------------------------------------------------------
/**
 *  @brief  �_���t�H���g�̍쐬
 *  @param  heap::HeapBase *heap  �g�p�q�[�v
 */
//-----------------------------------------------------------------------------
/*
void SystemFont_CreateTenjiFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem)
{
  GFL_SINGLETON_INSTANCE(SystemFont)->CreateTenjiFont(heapMem,deviceMem);
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  �_���t�H���g�̔j��
 */
//-----------------------------------------------------------------------------
/*
void SystemFont_DeleteTenjiFont(void)
{
  GFL_SINGLETON_INSTANCE(SystemFont)->DeleteTenjiFont();
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  �_���t�H���g�̎擾
 *  @return gfl::str::Font �t�H���g
 */
//-----------------------------------------------------------------------------
/*
gfl2::str::Font* SystemFont_GetTenjiFont(void)
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->GetTenjiFont();
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  �_�����L�t�H���g�o�b�t�@�̎擾
 *  @return void* �t�H���g�o�b�t�@
 */
//-----------------------------------------------------------------------------
/*
void* SystemFont_GetTenjiFontBuffer(void)
{
  return GFL_SINGLETON_INSTANCE(SystemFont)->GetTenjiFontBuffer();
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  default�t�H���g���O�ԖڂɃZ�b�g����
 */
//-----------------------------------------------------------------------------
void SystemFont_SetLytSystemDefaultFont(void)
{
  gfl2::lyt::LytSys::SetDefaultFont( SystemFont_GetFont( ), print::LAYOUT_DEFAULT_FONT, print::DEFAULT_FONT_IDX_KUJIRA_DEFAULT );
}

//----------------------------------------------------------------------------
/**
 *  @brief  num�t�H���g���P�ԖڂɃZ�b�g����
 */
//-----------------------------------------------------------------------------
void SystemFont_SetLytSystemNumFont(void)
{
  gfl2::lyt::LytSys::SetDefaultFont( SystemFont_GetNumFont(), print::LAYOUT_NUM_FONT, print::DEFAULT_FONT_LAYOUT_NUM_FONT );
}


//----------------------------------------------------------------------------
/**
 *  @brief  �o�g���t�H���g���Q�ԖڂɃZ�b�g����
 */
//-----------------------------------------------------------------------------
void SystemFont_SetLytSystemBattleFont(void)
{
  gfl2::lyt::LytSys::SetDefaultFont( SystemFont_GetBattleFont(), print::LAYOUT_BATTLE_FONT, print::DEFAULT_FONT_LAYOUT_BATTLE_FONT );
}

//----------------------------------------------------------------------------
/**
 *  @brief  tenji�t�H���g���R�ԖڂɃZ�b�g����
 *  @return void* �t�H���g�o�b�t�@
 */
//-----------------------------------------------------------------------------
/*
void SystemFont_SetLytSystemTenjiFont(void)
{
  gfl2::lyt::LytSys::SetDefaultFont( SystemFont_GetTenjiFont(), print::LAYOUT_TENJI_FONT, print::DEFAULT_FONT_LAYOUT_TENJI_FONT );
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  �ǉ����ꂽ�t�H���g��������
 */
//-----------------------------------------------------------------------------
void SystemFont_ResetDefaultFontInfo(void)
{
  for(int i = (print::DEFAULT_FONT_IDX_KUJIRA_DEFAULT+1) ;i < print::DEFAULT_FONT_IDX_MAX ; i++){
    gfl2::lyt::LytSys::ResetDefaultFontInfo(i);
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ǉ����ꂽ�_���t�H���g������
 */
//-----------------------------------------------------------------------------
/*
void SystemFont_ResetTenjiFont(void)
{
  gfl2::lyt::LytSys::ResetDefaultFontInfo(print::DEFAULT_FONT_LAYOUT_TENJI_FONT);
}
*/


GFL_NAMESPACE_END(view_print)
