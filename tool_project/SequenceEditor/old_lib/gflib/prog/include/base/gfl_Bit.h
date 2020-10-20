#ifndef __GFL_BIT_H__
#define __GFL_BIT_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_Bit.h
 *	@brief  �r�b�g����N���X
 *	@author	Koji Kawada
 *	@date		2011.03.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

// CTR_SDK

// NW4C

// gflib
#include <gfl_Macros.h>
#include <gfl_Debug.h>
#include <gfl_Std.h>

namespace gfl {
namespace base {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

// �r�b�g�����o�C�g�T�C�Y�ɕύX����}�N��
#define BIT_NUM_TO_BYTE_SIZE(bit_num) ((bit_num+8-1)/8)

//-------------------------------------
///	�r�b�g����N���X(Non template)
//=====================================
class Bit
{
  GFL_FORBID_COPY_AND_ASSIGN(Bit);

public:
  //-----------------------------------------------------------------------------
  /*
   *  @brief         �R���X�g���N�^
   *
   *  @param[in]     heap_memory   �q�[�v������(�f�o�C�X�������łȂ��Ă悢)
   *  @param[in]     u32           �g�p����r�b�g��
   */
  //-----------------------------------------------------------------------------
  inline Bit( gfl::heap::HeapBase* heap_memory, u32 bit_num )
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
   *  @brief         �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  inline ~Bit()  // �p������
  {
    GFL_DELETE_ARRAY m_buffer;
  }

  //-----------------------------------------------------------------------------
  /*
   *  @brief         �̈���m�ۂ��Ă���r�b�g�����擾
   *  
   *  @retval        u32   �r�b�g��
   */
  //-----------------------------------------------------------------------------
  inline u32 GetBitNum(void) const  {  return m_bit_num;  }

  //-----------------------------------------------------------------------------
  /*
   *  @brief         �I���ɂȂ��Ă���r�b�g�����擾
   *                 �I���ɂȂ��Ă���r�b�g���͍ő�ł�GetBitNum�ł��B
   *  
   *  @retval        u32   �r�b�g��
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
   *  @brief         �I�t�ɂȂ��Ă���r�b�g�����擾
   *                 �I�t�ɂȂ��Ă���r�b�g���͍ő�ł�GetBitNum�ł��B
   *  
   *  @retval        u32   �r�b�g��
   */
  //-----------------------------------------------------------------------------
  inline u32 GetOffBitNum(void) const
  {
    return ( GetBitNum() - GetOnBitNum() );
  }

  //-----------------------------------------------------------------------------
  /*
   *  @brief         �r�b�g���I���ɂ���
   *  
   *  @param[in]     index  ���Ԗڂ̃r�b�g��
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
   *  @brief         �r�b�g���I�t�ɂ���
   *  
   *  @param[in]     index  ���Ԗڂ̃r�b�g��
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
   *  @brief         �r�b�g��ݒ肷��
   *  
   *  @param[in]     index  ���Ԗڂ̃r�b�g��
   *  @param[in]     value  �ݒ肷��l(0 or 1)
   */
  //-----------------------------------------------------------------------------
  inline void SetBit(u32 index, u32 value)
  {
    GFL_ASSERT( value==0 || value==1 );  //@check
    (value == 0) ? Off(index) : On(index);
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         �r�b�g��ݒ肷��
   *  
   *  @param[in]     index  ���Ԗڂ̃r�b�g��
   *  @param[in]     is_on  �I���ɂ���Ȃ�true�A�I�t�ɂ���Ȃ�false
   */
  //-----------------------------------------------------------------------------
  inline void SetBit(u32 index, b32 is_on)
  {
    (is_on) ? On(index) : Off(index);
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         �S�Ẵr�b�g���I���ɂ���
   */
  //-----------------------------------------------------------------------------
  inline void OnAll(void)
  {
    gfl::std::MemFill(m_buffer, 0xFF, BIT_NUM_TO_BYTE_SIZE(m_bit_num));
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         �S�Ẵr�b�g���I�t�ɂ���
   */
  //-----------------------------------------------------------------------------
  inline void OffAll(void)
  {
    gfl::std::MemFill(m_buffer, 0x00, BIT_NUM_TO_BYTE_SIZE(m_bit_num));
  }

  //-----------------------------------------------------------------------------
  /*
   *  @brief         �r�b�g�̒l���擾
   *  
   *  @retval        u32   �I���Ȃ�1�A�I�t�Ȃ�0
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
   *  @brief         �r�b�g���I����
   *  
   *  @retval        b32   �I���Ȃ�true�A�I�t�Ȃ�false
   */
  //-----------------------------------------------------------------------------
  inline b32 IsOn(u32 index) const
  {
    return ( GetBit(index) != 0 );
  }

  //-----------------------------------------------------------------------------
  /*
   *  @brief         �r�b�g���I���ɂȂ��Ă���C���f�b�N�X�̂����ōŏ��̃C���f�b�N�X�𓾂�
   * 
   *  @param[out]    index  �߂�l��true�̂Ƃ��̂ݗL���Ȓl�������Ă���B�߂�l��false�̂Ƃ��͕s��l�B���Ԗڂ̃r�b�g���I���ɂȂ��Ă��邩�������ĕԂ��B
   *  @retval        b32    �I���ɂȂ��Ă���r�b�g��1�ȏ゠��Ȃ�true�A1���Ȃ��Ȃ�false
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
   *  @brief         �r�b�g���I�t�ɂȂ��Ă���C���f�b�N�X�̂����ōŏ��̃C���f�b�N�X�𓾂�
   * 
   *  @param[out]    index  �߂�l��true�̂Ƃ��̂ݗL���Ȓl�������Ă���B�߂�l��false�̂Ƃ��͕s��l�B���Ԗڂ̃r�b�g���I�t�ɂȂ��Ă��邩�������ĕԂ��B
   *  @retval        b32    �I�t�ɂȂ��Ă���r�b�g��1�ȏ゠��Ȃ�true�A1���Ȃ��Ȃ�false
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
   *  @brief         �R�s�[����
   * 
   *  @param[in]     src�̃r�b�g������ɃR�s�[����Bsrc�Ƃ���͓����r�b�g���łȂ���΂Ȃ�Ȃ��B
   *  @retval        b32    �R�s�[�ł����Ƃ�true��Ԃ�
   */
  //-----------------------------------------------------------------------------
  inline b32 Copy(const Bit* src)
  {
    if( src->GetBitNum() == this->GetBitNum() )
    {
      gfl::std::MemCopy(src->m_buffer, this->m_buffer, BIT_NUM_TO_BYTE_SIZE(m_bit_num));
      return true;
    }
    else
    {
      return false;
    }
  }

private:
  u32  m_bit_num;
  u8*  m_buffer;   // m_buffer[ (m_bit_num+8-1)/8 ];  // �g�p����r�b�g�����M���M���܂��Ȃ���T�C�Y�̃o�b�t�@
};

#undef BIT_NUM_TO_BYTE_SIZE

}  // namespace base
}  // namespace gfl

#endif // __GFL_BIT_H__
