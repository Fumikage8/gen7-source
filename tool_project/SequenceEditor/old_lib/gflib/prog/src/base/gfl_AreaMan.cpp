//===========================================================================
/**
 * @file    gfl_AreaMan.cpp
 * @brief   GAMEFREAK Library  String Print System ( �̈�Ǘ��N���X )
 * @author  GAMEFREAK inc.
 * @date    2010.11.29
 */
//===========================================================================
#include <gfl_Common.h>

#include <base/gfl_AreaMan.h>


namespace gfl {
namespace base {

//----------------------------------------------
// Bit�v�Z�ȗ��p�e�[�u��
//----------------------------------------------
const u8 AreaMan::EmptyCount[256] = {
    8,7,6,6,5,5,5,5,4,4,4,4,4,4,4,4, 4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    5,4,3,3,2,2,2,2,3,2,2,2,2,2,2,2, 4,3,2,2,2,2,2,2,3,2,2,2,2,2,2,2,
    6,5,4,4,3,3,3,3,3,2,2,2,2,2,2,2, 4,3,2,2,2,1,1,1,3,2,1,1,2,1,1,1,
    5,4,3,3,2,2,2,2,3,2,1,1,2,1,1,1, 4,3,2,2,2,1,1,1,3,2,1,1,2,1,1,1,
    7,6,5,5,4,4,4,4,3,3,3,3,3,3,3,3, 4,3,2,2,2,2,2,2,3,2,2,2,2,2,2,2,
    5,4,3,3,2,2,2,2,3,2,1,1,2,1,1,1, 4,3,2,2,2,1,1,1,3,2,1,1,2,1,1,1,
    6,5,4,4,3,3,3,3,3,2,2,2,2,2,2,2, 4,3,2,2,2,1,1,1,3,2,1,1,2,1,1,1,
    5,4,3,3,2,2,2,2,3,2,1,1,2,1,1,1, 4,3,2,2,2,1,1,1,3,2,1,1,2,1,1,0,
};

const u8 AreaMan::OpenFwdCount[256] = {
    8,7,6,6,5,5,5,5,4,4,4,4,4,4,4,4, 3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

const u8 AreaMan::OpenBackCount[256] = {
    8,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0, 4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0, 4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0, 4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0, 4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    7,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0, 4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0, 4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0, 4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0, 4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
};

const u8 AreaMan::FwdFillBit[9] = {
  0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff
};

const u8 AreaMan::BackFillBit[9] = {
  0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff,
};


/**
 *  �R���X�g���N�^
 */
AreaMan::AreaMan( u32 numBlocks, gfl::heap::HeapBase* pHeap )
{
  m_numBlocks     = numBlocks;
  m_areaByteSize  = numBlocksToByteSize( numBlocks );
  m_pArea         = reinterpret_cast<u8*>( GflHeapAllocMemory(pHeap, m_areaByteSize) );
  m_alignment     = 1;

  #if _GFL_BASE_AREAMAN_DEBUG_ENABLE
  m_bPrintEnable = false;
  #endif

}
/**
 *  �f�X�g���N�^
 */
AreaMan::~AreaMan()
{
  if( m_pArea )
  {
    GflHeapFreeMemory( m_pArea );
  }
}



//=============================================================================================
/**
 * �̈�擪���疖���܂ŁA�󂢂Ă��鏊��T���Ċm�ہi�O�����猟���j
 *
 * @param[in]   numBlocks   �m�ۂ���u���b�N��
 *
 * @retval      AreaMan::ResultCode   ���ʃR�[�h�^�m�ۂł��Ȃ���� RESULT_FAILED
 */
//=============================================================================================
AreaMan::ResultCode
AreaMan::ReserveAuto( u32 numBlocks )
{
  if( m_pArea )
  {
    return ReserveArea( numBlocks, 0, m_numBlocks );
  }
  else
  {
    GFL_ASSERT(0);
    return RESULT_FAILED;
  }
}
//=============================================================================================
/**
 * �̈�擪���疖���܂ŁA�󂢂Ă��鏊��T���Ċm�ہi������猟���j
 *
 * @param[in]   numBlocks   �m�ۂ���u���b�N��
 *
 * @retval      AreaMan::ResultCode   ���ʃR�[�h�^�m�ۂł��Ȃ���� RESULT_FAILED
 */
//=============================================================================================
AreaMan::ResultCode
AreaMan::ReserveAutoLo( u32 numBlocks )
{
  if( m_pArea )
  {
    return ReserveAreaLo( numBlocks, m_numBlocks-1, m_numBlocks );
  }
  else
  {
    GFL_ASSERT(0);
    return RESULT_FAILED;
  }
}
//=============================================================================================
/**
 * �̈�w��͈͓�����A�󂢂Ă���ʒu��T���Ċm�ہi�O�����猟���j
 *
 * @param[in]   numBlockReserve   �m�ۂ���u���b�N��
 * @param[in]   startBlockPos     �����J�n�u���b�N�ʒu�i�őO=0, �Ō�=(�ő�u���b�N��-1)�j
 * @param[in]   numAreaBlocks     �����͈̓u���b�N��
 *
 * @retval      AreaMan::ResultCode   �m�ۂł�����m�ۈʒu�^�m�ۂł��Ȃ���� RESULT_FAILED
 */
//=============================================================================================
AreaMan::ResultCode
AreaMan::ReserveArea( u32 numBlockReserve, u32 startBlockPos, u32 numAreaBlocks )
{
  if( m_pArea == NULL )
  {
    GFL_ASSERT(0);
    return RESULT_FAILED;
  }

  #if _GFL_BASE_AREAMAN_DEBUG_ENABLE
  if( m_bPrintEnable )
  {
    GFL_PRINT( "AREAMAN[%p] reserve %d blocks , start:%d, area:%d\n",
          m_pArea, numBlockReserve, startBlockPos, numAreaBlocks );
  }
  #endif

  if( (numAreaBlocks < numBlockReserve)
  ||  ((startBlockPos + numBlockReserve) > m_numBlocks )
  ){
    #ifdef _GFL_BASE_AREAMAN_DEBUG_ENABLE
    if( m_bPrintEnable )
    {
      GFL_PRINT( "** reserve failed %dblocks (pat A) ** \n", numBlockReserve );
    }
    #endif
    return RESULT_FAILED;
  }

  {
    ResultCode ret = reserveHi( startBlockPos, numAreaBlocks, numBlockReserve );
    return ret;
  }
}
//=============================================================================================
/**
 * �̈�w��͈͓�����A�󂢂Ă���ʒu��T���Ċm�ہi������猟���j
 *
 * @param[in]   numBlockReserve   �m�ۂ���u���b�N��
 * @param[in]   startBlockPos     �����J�n�u���b�N�ʒu�i�őO=0, �Ō�=(�ő�u���b�N��-1)�j
 * @param[in]   numAreaBlocks     �����͈̓u���b�N��
 *
 * @retval      AreaMan::ResultCode   �m�ۂł�����m�ۈʒu�^�m�ۂł��Ȃ���� RESULT_FAILED
 */
//=============================================================================================
AreaMan::ResultCode
AreaMan::ReserveAreaLo( u32 numBlockReserve, u32 startBlockPos, u32 numAreaBlocks )
{
  if( m_pArea == NULL )
  {
    GFL_ASSERT(0);
    return RESULT_FAILED;
  }

  #if _GFL_BASE_AREAMAN_DEBUG_ENABLE
  if( m_bPrintEnable )
  {
    GFL_PRINT("[AREAMAN] startBlock:%d, numAreaBlocks:%d, numBlockReserve:%d\n",
        startBlockPos, numAreaBlocks, numBlockReserve );
  }
  #endif

  GFL_ASSERT_MSG( numAreaBlocks >= numBlockReserve, "areasize:%d, reserve:%d\n",
          numAreaBlocks, numBlockReserve );
  GFL_ASSERT( startBlockPos < m_numBlocks );
  GFL_ASSERT( (int)numBlockReserve <= (int)(startBlockPos+1) );

  #if _GFL_BASE_AREAMAN_DEBUG_ENABLE
  if( m_bPrintEnable )
  {
    GFL_PRINT( "AREAMAN[%p] reserve %d blocks , area:%d - %d\n",
          m_pArea, numBlockReserve, startBlockPos, startBlockPos-numAreaBlocks+1);
  }
  #endif

  if( numAreaBlocks < numBlockReserve )
  {
    #if _GFL_BASE_AREAMAN_DEBUG_ENABLE
    if( m_bPrintEnable )
    {
      GFL_PRINT("** reserve failed %dblocks (pat A) ** \n", numBlockReserve);
    }
    #endif
    return RESULT_FAILED;
  }

  {
    ResultCode ret = reserveLo( startBlockPos, numAreaBlocks, numBlockReserve );

    return ret;
  }
}

//=============================================================================================
/**
 * �\�񂵂��̈���J��
 *
 * @param[in]   resultCode    �m�ێ��̌��ʃR�[�h
 */
//=============================================================================================
void
AreaMan::Release( ResultCode  resultCode )
{
  if( m_pArea == NULL )
  {
    GFL_ASSERT(0);
    return;
  }


  u32  pos, numBlocks;

  devideResCode( resultCode, &pos, &numBlocks );

  GFL_ASSERT((pos+numBlocks) <= m_numBlocks);

  {
    int p;
    p = pos % 8;
    pos /= 8;

    if( (p+numBlocks) <= 8 )
    {
      release_bit( &m_pArea[pos], p, numBlocks );
    }
    else
    {
      int bits, bytes, rem;

      p = 8-p;
      bits = numBlocks - p;
      bytes = bits / 8;
      rem = bits % 8;

      GFL_ASSERT( (m_pArea[pos] & (BackFillBit[p])) == BackFillBit[p] );
      m_pArea[pos++] &= (~BackFillBit[p]);

      while(bytes--)
      {
        GFL_ASSERT(m_pArea[pos]==0xff);
        m_pArea[pos++] = 0x00;
      }

      GFL_ASSERT( (m_pArea[pos] & (FwdFillBit[rem])) == FwdFillBit[rem] );
      m_pArea[pos] &= (~FwdFillBit[rem]);
    }
  }

}
//=============================================================================================
/**
 * �A�������󂫗̈�̍ő�l��Ԃ�
 *
 * @retval    u32   �A���󂫃u���b�N��
 */
//=============================================================================================
u32
AreaMan::GetContiniusFreeBlockNum( u32 startBlock, u32 numBlockArea ) const
{
  if( startBlock + numBlockArea >= m_numBlocks ){
    numBlockArea = m_numBlocks - startBlock;
  }

  if( numBlockArea == 0 ){
    return 0;
  }

  int bytePos = startBlock / 8;
  int bytePosEnd = (startBlock + (numBlockArea-1)) / 8;
  int remBits = 8 - (startBlock % 8);
  int remBitsEnd = (startBlock+numBlockArea) % 8;

  bytePosEnd += (remBitsEnd==0);

  int startRemBitsLimit = 0;
  if( bytePos == bytePosEnd ){
    startRemBitsLimit = 8 - (startBlock + numBlockArea) % 8;
    remBitsEnd = 0;
  }

  GFL_PRINT_EX(PRINT_FLAG, "--Checking..block=%d�`%d bytePos=%d, bytePosEnd=%d, remBits=%d (minLimit=%d), remBitsEnd=%d, startArea:%02x\n",
            startBlock, (startBlock+numBlockArea-1), bytePos, bytePosEnd, remBits, startRemBitsLimit, remBitsEnd, m_pArea[bytePos] );

  int cont, max, currentBit;

  cont = max = 0;
  while( remBits > startRemBitsLimit )
  {
    if( m_pArea[bytePos] & (1 <<(remBits-1)) ){
      cont = 0;
      GFL_PRINT_EX(PRINT_FLAG, "  reset cont pat P (%02x), %d\n", m_pArea[bytePos], remBits );
    }
    else
    {
      if( ++cont > max ){
        max = cont;
        GFL_PRINT_EX(PRINT_FLAG, "  max update_P bytePos:%d=%02x -> %d\n", bytePos, m_pArea[bytePos], max );
      }
    }
    --remBits;
  }

  while( ++bytePos < bytePosEnd )
  {
    currentBit = m_pArea[bytePos];

    if( EmptyCount[currentBit] == 8 )
    {
      cont += 8;
      if( cont > max ){
        max = cont;
        GFL_PRINT_EX(PRINT_FLAG, "  max update_A bytePos:%d=%02x  -> %d\n", bytePos, m_pArea[bytePos], max );
      }
    }
    else
    {
      cont += (OpenFwdCount[currentBit] <= 8)? OpenFwdCount[currentBit] : 8;
      if( cont > max ){
        max = cont;
        GFL_PRINT_EX(PRINT_FLAG, "  max update_B bytePos:%d=%02x  -> %d\n", bytePos, m_pArea[bytePos], max );
      }
      cont = 0;
      GFL_PRINT_EX(PRINT_FLAG, "  reset cont pat A\n");

      for( int j=OpenFwdCount[currentBit]; j<8; ++j )
      {
        if( currentBit & (0x80 >> j) ){
          cont = 0;
          GFL_PRINT_EX(PRINT_FLAG, "  reset cont pat B\n");
        }
        else{
          if( ++cont > max ){
            max = cont;
            GFL_PRINT_EX(PRINT_FLAG, "  max update_C bytePos:%d=%02x  -> %d\n", bytePos, m_pArea[bytePos], max );
          }
        }
      }
    }
  }
  GFL_PRINT_EX(PRINT_FLAG, "  (check rem bits cont=%d, END:%d)\n", cont, remBitsEnd );
  for(int i=0; i<remBitsEnd; ++i)
  {
    if( (m_pArea[bytePos] & (0x80 >> i))  == 0 )
    {
      if( ++cont > max ){
        max = cont;
        GFL_PRINT_EX(PRINT_FLAG, "  max update_D bytePos:%d=%02x  -> %d\n", bytePos, m_pArea[bytePos], max );
      }
    }
    else{
      cont = 0;
    }
  }

  return max;
}
//=============================================================================================
/**
 * �A�������󂫗̈�̍ő�l��Ԃ�
 *
 * @retval    u32   �A���󂫃u���b�N��
 */
//=============================================================================================
u32
AreaMan::GetContiniusFreeBlockNum( void ) const
{
  int max, cont, endBit, currentBit;

  max = cont = 0;

  endBit = (m_numBlocks >= 8)? 8 : m_numBlocks;

//  GFL_PRINT("AreaByteSize=%d ===============\n", m_areaByteSize);

  for( int bytePos=0; bytePos<m_areaByteSize; ++bytePos )
  {
    endBit = (m_numBlocks - bytePos*8);
    if( endBit >= 8 ){ endBit = 8; }

    currentBit = m_pArea[bytePos];

    if( EmptyCount[currentBit] == 8 )
    {
      cont += endBit;
      if( cont > max ){
        max = cont;
      }
    }
    else
    {
      cont += (OpenFwdCount[currentBit] <= endBit)? OpenFwdCount[currentBit] : endBit;
      if( cont > max ){
        max = cont;
      }
      cont = 0;

      for( int j=OpenFwdCount[currentBit]; j<endBit; ++j )
      {
        if( currentBit & (0x80 >> j) ){
          cont = 0;
        }
        else{
          if( ++cont > max ){
            max = cont;
          }
        }
      }
    }
  }


  return max;
}

//======================================================================================================
//======================================================================================================


/**
 *  �̈�u���b�N��->�Ǘ��o�C�g���ϊ�
 */
u32
AreaMan::numBlocksToByteSize( u32 numBlocks ) const
{
  u32 size = (numBlocks / 8) + ((numBlocks % 8) != 0);
//  while( size % 4 ){ size++; }
  return size;
}


//------------------------------------------------------------------
/**
 * �O������̒T���������i8�u���b�N���傫���̈�p�j
 *
 * @param[in]   man               �}�l�[�W��
 * @param[in]   startPos          �T���J�n�u���b�N
 * @param[in]   numBlockArea      �T���u���b�N�͈�
 * @param[in]   numBlockReserve   �m�ۂ������u���b�N��
 *
 * @retval  AreaMan::ResultCode       �m�ۂł����ʒu�i�ł��Ȃ���� POS_NOT_FOUND�j
 */
//------------------------------------------------------------------
AreaMan::ResultCode
AreaMan::reserveHi( u32 startPos, u32 numBlockArea, u32 numBlockReserve )
{
  u32 pos, step;
  u32 endPos, bytePos;
  u32 startBitIdx, startBitWidth, endBitWidth, mdlByteWidth;

  endPos = startPos + numBlockArea - numBlockReserve;

  if( m_alignment > 1 )
  {
    while( startPos % m_alignment ){
      ++startPos;
    }
    if( startPos > endPos ){
      return RESULT_FAILED;
    }
    step = m_alignment;
  }
  else
  {
    step = 1;
  }

  for( pos=startPos; pos<=endPos; pos+=step )
  {
    bytePos = pos / 8;
    startBitIdx   = pos % 8;
    startBitWidth = 8 - startBitIdx;

    if( startBitWidth >= numBlockReserve ){
      startBitWidth = numBlockReserve;
      endBitWidth   = 0;
      mdlByteWidth  = 0;
    }
    else{
      u32 rest = numBlockReserve - startBitWidth;
      mdlByteWidth = rest / 8;
      endBitWidth  = rest % 8;
    }

    if( !checkStartBitEnough(bytePos, startBitIdx, startBitWidth) ){
      continue;
    }

    u32 i;
    for(i=0; i<mdlByteWidth; ++i)
    {
      ++bytePos;
      if( m_pArea[bytePos] != 0 ){
        break;
      }
    }
    if( i != mdlByteWidth ){
      continue;
    }

    if( endBitWidth )
    {
      if( !checkEndBitEnough(++bytePos, endBitWidth) ){
        continue;
      }
    }

    if(reserve_area(pos, numBlockReserve))
    {
      return combineResCode( pos, numBlockReserve );
    }
    else
    {
      GFL_ASSERT(0);
      return RESULT_FAILED;
    }

  }

  return RESULT_FAILED;
}
/**
 *  �O������m�ێ��A�ŏ���8bit�ȓ����m�ۂł��邩�`�F�b�N
 */
bool
AreaMan::checkStartBitEnough( u32 bytePos, u32 startBitIdx, u32 startBitWidth ) const
{
  GFL_ASSERT(bytePos < m_areaByteSize);
  GFL_ASSERT(startBitIdx < 8);
  GFL_ASSERT((startBitWidth > 0) && (startBitWidth <=8));

  u32 bit_mask = FwdFillBit[ startBitWidth ] >> startBitIdx;

  if( (m_pArea[bytePos] & bit_mask) == 0 ){
    return true;
  }

  return false;
}
/**
 *  �O������m�ێ��A�Ō��8bit�ȓ����m�ۂł��邩�`�F�b�N
 */
bool
AreaMan::checkEndBitEnough( u32 bytePos, u32 endBitWidth ) const
{
  GFL_ASSERT(bytePos < m_areaByteSize);
  GFL_ASSERT(endBitWidth <= 8);

  return ((m_pArea[bytePos] & FwdFillBit[endBitWidth]) == 0);
}

//------------------------------------------------------------------
/**
 * ����T���������i8�u���b�N���傫���̈�p�j
 *
 * @param[in]   startPos        �T���J�n�u���b�N�i100���w�肵����A100,99,98 ... �ƒT������j
 * @param[in]   numAreaBlocks   �T���u���b�N�͈�
 * @param[in]   numBlockReserve �m�ۂ������u���b�N��
 *
 * @retval  AreaMan::ResultCode      ���ʃR�[�h�i�m�ۂł��Ȃ���� RESULT_FAILED �j
 */
//------------------------------------------------------------------
AreaMan::ResultCode
AreaMan::reserveLo( u32 startPos, u32 numAreaBlocks, u32 numBlockReserve )
{
  int pos, step;
  int endPos, bytePos;
  int startBitIdx, startBitWidth, endBitWidth, mdlByteWidth;

  endPos = startPos - numAreaBlocks + numBlockReserve;

  if( m_alignment > 1 )
  {
    while( startPos % m_alignment )
    {
      --startPos;
    }
    if( startPos < endPos ){
      return RESULT_FAILED;
    }
    step = m_alignment;
  }
  else
  {
    step = 1;
  }

  for( pos=startPos; pos>=endPos; pos-=step )
  {
    bytePos = pos / 8;
    startBitIdx   = pos % 8;
    startBitWidth = startBitIdx + 1;

    if( startBitWidth >= numBlockReserve ){
      startBitWidth = numBlockReserve;
      endBitWidth   = 0;
      mdlByteWidth  = 0;
    }
    else{
      u32 rest = numBlockReserve - startBitWidth;
      mdlByteWidth = rest / 8;
      endBitWidth  = rest % 8;
    }

    if( !checkStartBitEnoughLo(bytePos, startBitIdx, startBitWidth) ){
      continue;
    }

    u32 i;
    for(i=0; i<mdlByteWidth; ++i)
    {
      --bytePos;
      if( m_pArea[bytePos] != 0 ){
        break;
      }
    }
    if( i != mdlByteWidth ){
      continue;
    }

    if( endBitWidth )
    {
      if( !checkEndBitEnoughLo(--bytePos, endBitWidth) ){
        continue;
      }
    }

    {
      int p = pos - numBlockReserve + 1;

      if(reserve_area(p, numBlockReserve))
      {
        return combineResCode( p, numBlockReserve );
      }
      else
      {
        GFL_ASSERT(0);
        return RESULT_FAILED;
      }
    }

  }

  return RESULT_FAILED;
}
/**
 *  �������m�ێ��A�ŏ���8bit�ȓ����m�ۂł��邩�`�F�b�N
 */
bool
AreaMan::checkStartBitEnoughLo( u32 bytePos, u32 startBitIdx, u32 startBitWidth ) const
{
  GFL_ASSERT(bytePos < m_areaByteSize);
  GFL_ASSERT(startBitIdx < 8);
  GFL_ASSERT((startBitWidth > 0) && (startBitWidth <=8));

  if( (startBitIdx+1) >= startBitWidth )
  {
    u32 bit_mask = BackFillBit[ startBitWidth ] << (7 - startBitIdx);
    if( (m_pArea[bytePos] & bit_mask) == 0 ){
      return true;
    }
  }
  else
  {
    GFL_ASSERT(0);
  }

  return false;
}
/**
 *  �������m�ێ��A�Ō��8bit�ȓ����m�ۂł��邩�`�F�b�N
 */
bool
AreaMan::checkEndBitEnoughLo( u32 bytePos, u32 endBitWidth ) const
{
  GFL_ASSERT(bytePos < m_areaByteSize);
  GFL_ASSERT(endBitWidth <= 8);

  return ((m_pArea[bytePos] & BackFillBit[endBitWidth]) == 0);
}

//------------------------------------------------------------------
/**
 * �̈���bit���w��u���b�N�����������
 *
 * @param   area
 * @param   pos
 * @param   num
 */
//------------------------------------------------------------------
void
AreaMan::release_bit( u8* area, u32 pos, u32 num )
{
  GFL_ASSERT((pos+num)<=8);

  {
    u8 bit = FwdFillBit[num] >> pos;
    GFL_ASSERT( (bit&(*area)) == bit );
    (*area) &= (~bit);
  }
}


//--------------------------------------------------------------------------------------
/**
 * �̈�m�ۃR�A����
 *
 * @param[in]   pos         �m�ۊJ�n�u���b�N�ʒu
 * @param[in]   blockNum    �m�ۃu���b�N��
 *
 * @retval  bool        �m�ۂł����� true
 */
//--------------------------------------------------------------------------------------
bool
AreaMan::reserve_area( int pos, u32 blockNum )
{
  int start_pos, start_bit_count, start_bit, rem, bytes;

  if( (pos+blockNum) > m_numBlocks )
  {
    GFL_ASSERT_MSG(0, "pos=%d,blockNum=%d\n", pos, blockNum);
    return false;
  }

  start_pos = pos % 8;
  start_bit_count = 8 - start_pos;
  pos /= 8;

  if( start_bit_count >= blockNum )
  {
    GFL_ASSERT_STOP(blockNum<GFL_NELEMS(FwdFillBit));
    start_bit = FwdFillBit[blockNum] >> start_pos;
    bytes = 0;
    rem = 0;
  }
  else
  {
    GFL_ASSERT_STOP(start_bit_count<GFL_NELEMS(BackFillBit));
    start_bit = BackFillBit[start_bit_count];
    bytes = (blockNum - start_bit_count) / 8;
    rem = (blockNum - start_bit_count) % 8;
  }

  if( (m_pArea[pos] & start_bit) != 0 )
  {
    #if _GFL_BASE_AREAMAN_DEBUG_ENABLE
    if( m_bPrintEnable )
    {
      GFL_PRINT("pos=%d, start_bit=");
      print_bit( start_bit );
      GFL_PRINT(" area=");
      print_bit( m_pArea[pos] );
    }
    #endif

    GFL_ASSERT(0);
    return false;
  }


  m_pArea[pos++] |= start_bit;
  while( bytes-- )
  {
    GFL_ASSERT(m_pArea[pos]==0);
    m_pArea[pos++] = 0xff;
  }
  if( rem )
  {
    GFL_ASSERT_STOP((m_pArea[pos]&FwdFillBit[rem])==0);
    m_pArea[pos] |= FwdFillBit[rem];
  }

  return true;
}



#if _GFL_BASE_AREAMAN_DEBUG_ENABLE
void
AreaMan::print_bit( u8 b ) const
{
  static const char* let[] = { "0", "1" };

  nn::dbg::detail::PutString( let[ (b>>7)&1 ] );
  nn::dbg::detail::PutString( let[ (b>>6)&1 ] );
  nn::dbg::detail::PutString( let[ (b>>5)&1 ] );
  nn::dbg::detail::PutString( let[ (b>>4)&1 ] );
  nn::dbg::detail::PutString( let[ (b>>3)&1 ] );
  nn::dbg::detail::PutString( let[ (b>>2)&1 ] );
  nn::dbg::detail::PutString( let[ (b>>1)&1 ] );
  nn::dbg::detail::PutString( let[ (b>>0)&1 ] );
}
#endif



} /* end of namespace str */
} /* end of namespace gfl */

