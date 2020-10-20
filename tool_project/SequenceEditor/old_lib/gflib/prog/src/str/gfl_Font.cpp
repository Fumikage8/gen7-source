//===========================================================================
/**
 * @file  gfl_Font.cpp
 * @brief GAMEFREAK Library  String Print System ( Font )
 * @author  GAMEFREAK inc.
 * @date  2010.10.27
 */
//===========================================================================

#include <nw/font.h>

#include <gfl_Common.h>
#include <gfl_fs.h>
#include <gfl_Heap.h>

#include "str/gfl_Font.h"

namespace gfl {
namespace str {

//----------------------------------------------------------------------
// �f�o�b�O�o�͗L��
//----------------------------------------------------------------------
enum {
  FONT_PRINT_FLAG = gfl::debug::PRINT_FLAG_CATEGORY_STRING | gfl::debug::PRINT_FLAG_LEVEL2
};




//----------------------------------------------------------------------
/**
 * @brief   ���\�[�X�t�H���g�N���X�쐬
 *
 * @param   fontFilePath    [in] �t�H���g�t�@�C���̃p�X���w��
 * @param   heap            [in] �\�z�p�̃������m�ۂɎg���q�[�v�n���h��
 *
 * @retval  ResFont*        �\�z�ł�����L���ȃt�H���g�N���X�|�C���^�^���s������NULL
 */
//----------------------------------------------------------------------
ResFont*
CreateResFont( const char* fontFilePath, gfl::heap::HeapBase* heap, gfl::heap::HeapBase* devHeap )
{
  ResFont  *pFont = GFL_NEW(heap) ResFont();

  // �t�H���g���\�[�X�����[�h���܂�
  gfl::fs::ReadFile fontReader( fontFilePath );

  u32 fileSize = fontReader.GetSize();
  if ( fileSize == 0 ){
    GFL_ASSERT(0);
    return false;
  }

  NN_LOG( "[FONT] Begin create ResFont, path=%s, size=%08x, align=%d\n",
              fontFilePath, fileSize, nw::font::GlyphDataAlignment );

  void* buffer = GflHeapAllocMemoryAlign( devHeap, fileSize, nw::font::GlyphDataAlignment );
  if( buffer == NULL )
  {
    GFL_ASSERT(0);
    return NULL;
  }

  s32 readSize = fontReader.Read( buffer, fileSize );
  if (readSize != fileSize)
  {
    GFL_ASSERT(0);
    GflHeapFreeMemory( buffer );
    return NULL;
  }

  // �t�H���g���\�[�X���Z�b�g���܂�
  bool bSuccess = pFont->SetResource( buffer );

  //--- ���Ƀ��\�[�X���Z�b�g�ς݂ł��邩�C���[�h�ς݂ł��邩�A���\�[�X���s���ȏꍇ�Ɏ��s���܂��B
  if( !bSuccess )
  {
    GFL_ASSERT(0);
    GflHeapFreeMemory( buffer );
    return NULL;
  }

  // �`��p�o�b�t�@��ݒ肵�܂��B
  const u32 drawBufferSize = ResFont::GetDrawBufferSize( buffer );

  GFL_PRINT_EX( FONT_PRINT_FLAG, "[FONT] drawBufferSize=%08x\n", drawBufferSize );

  void* drawBuffer = GflHeapAllocMemoryAlign( heap, drawBufferSize, 4 );
  if( drawBuffer == NULL ){
    GFL_ASSERT(0);
    GflHeapFreeMemory( buffer );
    return NULL;
  }

  pFont->SetDrawBuffer( drawBuffer );

  GFL_PRINT_EX( FONT_PRINT_FLAG, "[FONT] ResFont create succeed\n" );

  return pFont;
}

//----------------------------------------------------------------------
/**
 * @brief   ���\�[�X�t�H���g�N���X�j��
 *
 * @param   pFont       [in] �t�H���g�N���X�|�C���^
 */
//----------------------------------------------------------------------
void
DeleteResFont( ResFont* pFont )
{
  GFL_ASSERT(pFont);
  if( pFont ){
    void * buffer;

    buffer = pFont->GetDrawBuffer();
    GflHeapFreeMemory( buffer );

    buffer = pFont->RemoveResource();
    GflHeapFreeMemory( buffer );

    GFL_DELETE( pFont );
  }
}



} /* end of namespace str */
} /* end of namespace gfl */



