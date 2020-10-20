#ifndef __GFL_FONT_H__
#define __GFL_FONT_H__
//===========================================================================
/**
 * @file  gfl_Font.h
 * @brief GAMEFREAK Library  String Print System ( Font )
 * @author  GAMEFREAK inc.
 * @date  2010.10.27
 */
//===========================================================================
#pragma once

#include <nn/types.h>
#include <nw/font.h>
#include <gfl_Heap.h>

namespace gfl {
namespace str {

/**
 *  �t�H���g���N���X�i�C���X�^���X�쐬�s�j
 */
typedef nw::font::Font           Font;

/**
 *  ���\�[�X�t�H���g�N���X
 * �i���^�C�v�̃t�H���g�𓱓����邩�ǂ����͖��m��B��{�I�ɂ�����g���Ă��������j
 */
typedef nw::font::ResFont        ResFont;


  //----------------------------------------------------------------------
  /**
   * @brief   ���\�[�X�t�H���g�N���X�쐬
   *
   * @param   fontFilePath    [in] �t�H���g�t�@�C���̃p�X���w��
   * @param   heap            [in] �\�z�p�̃������m�ۂɎg���q�[�v�n���h��
   * @param   devHeap         [in] �\�z�p�̃f�o�C�X�������m�ۂɎg���q�[�v�n���h��
   *
   * @retval  ResFont*        �\�z�ł�����L���ȃt�H���g�N���X�|�C���^�^���s������NULL
   */
  //----------------------------------------------------------------------
    ResFont*
      CreateResFont(  const char* fontFilePath,
                      gfl::heap::HeapBase* heap,
                      gfl::heap::HeapBase* devHeap
      );

  //----------------------------------------------------------------------
  /**
   * @brief   ���\�[�X�t�H���g�N���X�j��
   *
   * @param   pFont       [in] �t�H���g�N���X�|�C���^
   */
  //----------------------------------------------------------------------
    void
      DeleteResFont( ResFont* pFont );


} /* end of namespace str */
} /* end of namespace gfl */



#endif // __GFL_FONT_H__
