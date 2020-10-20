
#if !defined(GFL2_BIT_H_INCLUDED)
#define GFL2_BIT_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   gfl2_Bit.h
 * @date   2011.03.07
 * @date   2016/01/26 Tue. 18:32:23
 * @author Koji Kawada
 * @author muto_yu
 * @brief  ビット操作クラス
 * @note   旧作より移植
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <heap/include/gfl2_Heap.h>
#include  <debug/include/gfl2_Assert.h>
#include  <util/include/gfl2_std_string.h>


namespace gfl2 {
namespace base {
// =============================================================================
/**/
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

// ビット数をバイトサイズに変更するマクロ
#define BIT_NUM_TO_BYTE_SIZE(bit_num) ((bit_num+8-1)/8)

//-------------------------------------
///	ビット操作クラス(Non template)
//=====================================
class Bit
{
  GFL_FORBID_COPY_AND_ASSIGN(Bit);

public:
  //-----------------------------------------------------------------------------
  /*
   *  @brief         コンストラクタ
   *
   *  @param[in]     heap_memory   ヒープメモリ(デバイスメモリでなくてよい)
   *  @param[in]     u32           使用するビット数
   */
  //-----------------------------------------------------------------------------
  inline Bit( gfl2::heap::HeapBase* heap_memory, u32 bit_num )
    : m_bit_num(bit_num),
      m_buffer(NULL)
  {
    GFL_ASSERT_STOP( m_bit_num > 0 );
    m_buffer = GFL_NEW_ARRAY(heap_memory) u8[ BIT_NUM_TO_BYTE_SIZE(m_bit_num) ];
    GFL_ASSERT_STOP( m_buffer );
    OffAll();
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         デストラクタ
   */
  //-----------------------------------------------------------------------------
  inline ~Bit()  // 継承注意
  {
    GFL_DELETE_ARRAY m_buffer;
  }

  //-----------------------------------------------------------------------------
  /*
   *  @brief         領域を確保しているビット数を取得
   *  
   *  @retval        u32   ビット数
   */
  //-----------------------------------------------------------------------------
  inline u32 GetBitNum(void) const  {  return m_bit_num;  }

  //-----------------------------------------------------------------------------
  /*
   *  @brief         オンになっているビット数を取得
   *                 オンになっているビット数は最大でもGetBitNum個です。
   *  
   *  @retval        u32   ビット数
   */
  //-----------------------------------------------------------------------------
  inline u32 GetOnBitNum(void) const
  {
    u32 count = 0;
    for(u32 i=0; i<m_bit_num; ++i)
    {
      if( IsOn(i) )
      {
        ++count;
      }
    }
    return count;
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         オフになっているビット数を取得
   *                 オフになっているビット数は最大でもGetBitNum個です。
   *  
   *  @retval        u32   ビット数
   */
  //-----------------------------------------------------------------------------
  inline u32 GetOffBitNum(void) const
  {
    return ( GetBitNum() - GetOnBitNum() );
  }

  //-----------------------------------------------------------------------------
  /*
   *  @brief         ビットをオンにする
   *  
   *  @param[in]     index  何番目のビットか
   */
  //-----------------------------------------------------------------------------
  inline void On(u32 index)
  {
    GFL_ASSERT( index<m_bit_num );  //@check
    if( index<m_bit_num )
    {
      m_buffer[index/8] |= ( 0x01 << (index%8) );
    }
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         ビットをオフにする
   *  
   *  @param[in]     index  何番目のビットか
   */
  //-----------------------------------------------------------------------------
  inline void Off(u32 index)
  {
    GFL_ASSERT( index<m_bit_num );  //@check
    if( index<m_bit_num )
    {
      m_buffer[index/8] &= ( ~( 0x01 << (index%8) ) );
    }
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         ビットを設定する
   *  
   *  @param[in]     index  何番目のビットか
   *  @param[in]     value  設定する値(0 or 1)
   */
  //-----------------------------------------------------------------------------
  inline void SetBit(u32 index, u32 value)
  {
    GFL_ASSERT( value==0 || value==1 );  //@check
    (value == 0) ? Off(index) : On(index);
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         ビットを設定する
   *  
   *  @param[in]     index  何番目のビットか
   *  @param[in]     is_on  オンにするならtrue、オフにするならfalse
   */
  //-----------------------------------------------------------------------------
  inline void SetBit(u32 index, b32 is_on)
  {
    (is_on) ? On(index) : Off(index);
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         全てのビットをオンにする
   */
  //-----------------------------------------------------------------------------
  inline void OnAll(void)
  {
    gfl2::std::MemFill(m_buffer, 0xFF, BIT_NUM_TO_BYTE_SIZE(m_bit_num));
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         全てのビットをオフにする
   */
  //-----------------------------------------------------------------------------
  inline void OffAll(void)
  {
    gfl2::std::MemFill(m_buffer, 0x00, BIT_NUM_TO_BYTE_SIZE(m_bit_num));
  }

  //-----------------------------------------------------------------------------
  /*
   *  @brief         ビットの値を取得
   *  
   *  @retval        u32   オンなら1、オフなら0
   */
  //-----------------------------------------------------------------------------
  inline u32 GetBit(u32 index) const
  {
    GFL_ASSERT( index<m_bit_num );  //@check
    u32  value = 0; 
    if( index<m_bit_num )
    {
      value = ( ( (m_buffer[index/8]) >> (index%8) ) & 0x01 );
    }
    return value;
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         ビットがオンか
   *  
   *  @retval        b32   オンならtrue、オフならfalse
   */
  //-----------------------------------------------------------------------------
  inline b32 IsOn(u32 index) const
  {
    return ( GetBit(index) != 0 );
  }

  //-----------------------------------------------------------------------------
  /*
   *  @brief         ビットがオンになっているインデックスのうちで最小のインデックスを得る
   * 
   *  @param[out]    index  戻り値がtrueのときのみ有効な値が入っている。戻り値がfalseのときは不定値。何番目のビットがオンになっているかを代入して返す。
   *  @retval        b32    オンになっているビットが1つ以上あるならtrue、1つもないならfalse
   */
  //-----------------------------------------------------------------------------
  inline b32 GetOnBitMinIndex(u32* index) const
  {
    b32 exist = false;
    for(u32 i=0; i<m_bit_num; ++i)
    {
      if( IsOn(i) )
      {
        exist = true;
        if(index) *index = i;
        break;
      }
    }
    return exist;
  }

  //-----------------------------------------------------------------------------
  /*
   *  @brief         ビットがオフになっているインデックスのうちで最小のインデックスを得る
   * 
   *  @param[out]    index  戻り値がtrueのときのみ有効な値が入っている。戻り値がfalseのときは不定値。何番目のビットがオフになっているかを代入して返す。
   *  @retval        b32    オフになっているビットが1つ以上あるならtrue、1つもないならfalse
   */
  //-----------------------------------------------------------------------------
  inline b32 GetOffBitMinIndex(u32* index) const
  {
    b32 exist = false;
    for(u32 i=0; i<m_bit_num; ++i)
    {
      if( !IsOn(i) )
      {
        exist = true;
        if(index) *index = i;
        break;
      }
    }
    return exist;
  }

  //-----------------------------------------------------------------------------
  /*
   *  @brief         コピーする
   * 
   *  @param[in]     srcのビットをこれにコピーする。srcとこれは同じビット数でなければならない。
   *  @retval        b32    コピーできたときtrueを返す
   */
  //-----------------------------------------------------------------------------
  inline b32 Copy(const Bit* src)
  {
    if( src->GetBitNum() == this->GetBitNum() )
    {
      gfl2::std::MemCopy(src->m_buffer, this->m_buffer, BIT_NUM_TO_BYTE_SIZE(m_bit_num));
      return true;
    }
    else
    {
      return false;
    }
  }

private:
  u32  m_bit_num;
  u8*  m_buffer;   // m_buffer[ (m_bit_num+8-1)/8 ];  // 使用するビット数をギリギリまかなえるサイズのバッファ
};

#undef BIT_NUM_TO_BYTE_SIZE



} /*  namespace gfl2 */
} /*  namespace base */
#endif  /*  #if !defined(GFL2_BIT_H_INCLUDED)  */
