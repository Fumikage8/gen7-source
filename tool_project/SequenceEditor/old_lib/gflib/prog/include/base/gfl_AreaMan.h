#ifndef __GFL_AREAMAN_H__
#define __GFL_AREAMAN_H__
//===========================================================================
/**
 * @file    gfl_AreaMan.h
 * @brief   GAMEFREAK Library  String Print System ( �̈�Ǘ��N���X )
 * @author  GAMEFREAK inc.
 * @date    2010.11.29
 */
//===========================================================================
#pragma once

#include  <gfl_Common.h>
#include  <gfl_Heap.h>

namespace gfl {
namespace base {

#define _GFL_BASE_AREAMAN_DEBUG_ENABLE  (0)


/**
 *  �̈�Ǘ��N���X
 */
class AreaMan {
  GFL_FORBID_COPY_AND_ASSIGN( AreaMan );

public:
  enum {
    RESULT_FAILED = 0xffffffffffffffff, ///< �m�ێ��s���̌��ʃR�[�h
    POS_NOT_FOUND = 0xffffffff,         ///< �̈悪�m�ۂł��Ȃ������ꍇ�̖߂�l
  };

  /**
   *  �̈�m�ی��ʃR�[�h
   */
  typedef  u64  ResultCode;

  //--------------------------------------------------------
  /**
   *  �R���X�g���N�^
   *
   * @param[in]   numBlocks   �Ǘ�����u���b�N��
   * @param[in]   pHeap       �q�[�v�n���h��
   */
  //--------------------------------------------------------
  AreaMan( u32 numBlocks, gfl::heap::HeapBase* pHeap );

  // �f�X�g���N�^
  ~AreaMan();

  //------------------------------------------------------------------
  /**
   * �̈�擪���疖���܂ŁA�󂢂Ă��鏊��T���Ċm�ہi�O�����猟���j
   *
   * @param[in]   numBlocks   �m�ۂ���u���b�N��
   *
   * @retval      AreaMan::ResultCode   ���ʃR�[�h�^�m�ۂł��Ȃ���� RESULT_FAILED
   */
  //------------------------------------------------------------------
  ResultCode  ReserveAuto( u32 numBlocks );

  //------------------------------------------------------------------
  /**
   * �̈�擪���疖���܂ŁA�󂢂Ă��鏊��T���Ċm�ہi������猟���j
   *
   * @param[in]   numBlocks   �m�ۂ���u���b�N��
   *
   * @retval      AreaMan::ResultCode   ���ʃR�[�h�^�m�ۂł��Ȃ���� RESULT_FAILED
   */
  //------------------------------------------------------------------
  ResultCode  ReserveAutoLo( u32 numBlocks );

  //------------------------------------------------------------------
  /**
   * �̈�w��͈͓�����A�󂢂Ă���ʒu��T���Ċm�ہi�O�����猟���j
   *
   * @param[in]   numBlockReserve   �m�ۂ���u���b�N��
   * @param[in]   startBlockPos     �����J�n�u���b�N�ʒu�i�őO=0, �Ō�=(�ő�u���b�N��-1)�j
   * @param[in]   numAreaBlocks     �����͈̓u���b�N��
   *
   * @retval      AreaMan::ResultCode   ���ʃR�[�h�^�m�ۂł��Ȃ���� RESULT_FAILED
   */
  //------------------------------------------------------------------
  ResultCode  ReserveArea( u32 numBlocks, u32 startBlockPos, u32 numAreaBlocks );

  //------------------------------------------------------------------
  /**
   * �̈�w��͈͓�����A�󂢂Ă���ʒu��T���Ċm�ہi������猟���j
   *
   * @param[in]   numBlockReserve   �m�ۂ���u���b�N��
   * @param[in]   startBlockPos     �����J�n�u���b�N�ʒu�i�őO=0, �Ō�=(�ő�u���b�N��-1) ��ReserveArea()�Ɠ����B0=�Ō�ł͂���܂���j
   * @param[in]   numAreaBlocks     �����͈̓u���b�N��
   *
   * @retval      AreaMan::ResultCode   ���ʃR�[�h�^�m�ۂł��Ȃ���� RESULT_FAILED
   */
  //------------------------------------------------------------------
  ResultCode  ReserveAreaLo( u32 numBlocks, u32 startBlockPos, u32 numAreaBlocks );


  //------------------------------------------------------------------
  /**
   * �\�񂵂��̈���J��
   *
   * @param[in]   resultCode    �m�ێ��̌��ʃR�[�h
   */
  //------------------------------------------------------------------
  void Release( ResultCode  reserveResultCode );

  //------------------------------------------------------------------
  /**
   * �A�������󂫗̈�̍ő�l��Ԃ�
   *
   * @retval    u32   �A���󂫃u���b�N��
   */
  //------------------------------------------------------------------
  u32  GetContiniusFreeBlockNum( void ) const;

  //------------------------------------------------------------------
  /**
   * �A�������󂫗̈�̍ő�l��Ԃ��i�J�E���g�J�n�ʒu���w��j
   *
   * @param[in]   startBlockPos     �����J�n�u���b�N�ʒu�i�őO=0, �Ō�=(�ő�u���b�N��-1) ��ReserveArea()�Ɠ����B0=�Ō�ł͂���܂���j
   * @param[in]   numAreaBlocks     �����͈̓u���b�N��
   *
   * @retval    u32   �A���󂫃u���b�N��
   */
  //------------------------------------------------------------------
  u32 GetContiniusFreeBlockNum( u32 startBlock, u32 numBlockArea ) const;

  //------------------------------------------------------------------
  /**
   * �A���C�������g�ݒ�i�u���b�N�P�ʁj
   *
   * @param[in]   alignment      �A���C�������g�w��
   */
  //------------------------------------------------------------------
  void SetAlign( u32 alignment )
  {
    m_alignment = alignment;
  }

  //-----------------------------------------------------------
  /**
   *  ���ʃR�[�h -> �m�ۈʒu�֕ϊ�
   */
  //-----------------------------------------------------------
  u32  ResCodeToBlockPos( ResultCode resCode ) const
  {
    if( resCode != POS_NOT_FOUND ){
      resCode >>= 32;
      return static_cast<u32>( resCode & 0xffffffff );
    }
    return POS_NOT_FOUND;
  }


private:
  //----------------------------------------------
  // Bit�v�Z�ȗ��p�e�[�u��
  //-----------------------------------------------
  static const u8 EmptyCount[256];
  static const u8 OpenFwdCount[256];
  static const u8 OpenBackCount[256];
  static const u8 FwdFillBit[9];
  static const u8 BackFillBit[9];

  u32   m_numBlocks;
  u32   m_areaByteSize;
  u32   m_alignment;
  u8*   m_pArea;
  bool  m_bPrintEnable;

  u32 numBlocksToByteSize( u32 numBlocks ) const;

  ResultCode reserveHi( u32 startPos, u32 numBlockArea, u32 numBlockReserve );
  ResultCode reserveLo( u32 startPos, u32 numBlockArea, u32 numBlockReserve );
  bool       checkStartBitEnough( u32 bytePos, u32 startBitIdx, u32 startBitWidth ) const;
  bool       checkEndBitEnough( u32 bytePos, u32 endBitWidth ) const;
  bool       checkStartBitEnoughLo( u32 bytePos, u32 startBitIdx, u32 startBitWidth ) const;
  bool       checkEndBitEnoughLo( u32 bytePos, u32 endBitWidth ) const;
  void       release_bit( u8* area, u32 pos, u32 num );
  bool       reserve_area( int pos, u32 blockNum );

  /**
   * �u���b�N�ʒu���T�C�Y->���ʃR�[�h�쐬
   */
  ResultCode  combineResCode( u32 pos, u32 size ) const
  {
    u64 result = pos;
    result <<= 32;
    result |= size;
    return result;
  }

  /**
   * ���ʃR�[�h�쐬 -> �u���b�N�ʒu���T�C�Y�֕���
   */
  void devideResCode( ResultCode code, u32* pos, u32* size ) const
  {
    *size = code & 0xffffffff;
    *pos  = (code >> 32) & 0xffffffff;
  }

  #if _GFL_BASE_AREAMAN_DEBUG_ENABLE
  void print_bit( u8 b ) const;
  #endif

  //----------------------------------------------
  // Debug Print Flag
  //-----------------------------------------------
  enum {
    PRINT_FLAG = (gfl::debug::PRINT_FLAG_CATEGORY_HEAP) | (gfl::debug::PRINT_FLAG_LEVEL2)
  };

};


} // end of namespace 'base'
} // end of namespace 'gfl'

#endif // __GFL_AREAMAN_H__
