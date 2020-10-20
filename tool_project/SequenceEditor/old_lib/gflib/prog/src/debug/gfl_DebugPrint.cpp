//======================================================================
/**
 * @file	gfl_DebugPrint.cpp
 * @brief	�f�o�b�O�o��
 * @author	ariizumi
 * @data	10/10/13
 */
//======================================================================

#include <gflib.h>
#include <debug/gfl_DebugPrint.h>

//�v���b�g�t�H�[�����Ƃ̒�`
#include <nn/dbg.h>
//�v���b�g�t�H�[�����Ƃ̒�` �����܂�



namespace gfl
{
namespace debug
{


#if GFL_DEBUG_PRINT

  static const int defaultPrintFlag = PRINT_FLAG_LEVEL1;
  
  static u16 printFlag = defaultPrintFlag;
  static u16 printEnable = 1;

  //----------------------------------------------------------------------
  /**
   * @brief   �t���O�̏����� GFL_PRINT_FLAG_LEVEL1���ݒ肳��܂�
   */
  //----------------------------------------------------------------------
  void ResetPrintFlag()
  {
    printFlag = defaultPrintFlag;
  }
  
  //----------------------------------------------------------------------
  /**
   * @brief   �t���O�̐ݒ�(�㏑��)
   *
   * @param[in]    flag �t���O�r�b�g
   */
  //----------------------------------------------------------------------
  void SetPrintFlag( const int flag )
  {
    printFlag = flag;
  }
  bool TogglePrintFlg(void)
  {
    printEnable = 1 - printEnable;
    return printEnable;
  }

  
  //----------------------------------------------------------------------
  /**
   * @brief   �t���O�̐ݒ�(�ǉ�)
   *
   * @param[in]    flag �t���O�r�b�g
   */
  //----------------------------------------------------------------------
  void AddPrintFlag( const int flag )
  {
    printFlag |= flag;
  }
  
  //----------------------------------------------------------------------
  /**
   * @brief   �t���O�̐ݒ�(�폜)
   *
   * @param[in]    flag �t���O�r�b�g
   */
  //----------------------------------------------------------------------
  void SubPrintFlag( const int flag )
  {
    printFlag -= (printFlag&flag);
  }
  
  //----------------------------------------------------------------------
  /**
   * @brief   ���O�o��
   *
   * @param[in]    flag �t���O�r�b�g
   * @param[in]    fmt  �o��(�ψ����Ή�)
   */
  //----------------------------------------------------------------------
  void PrintCore( const int flag , const char *fmt, ... )
  {
    if(!printEnable){
      return;
    }
    if( flag == (printFlag&flag) )
    {
      va_list vlist;
      va_start(vlist, fmt);

      //�v���b�g�t�H�[�����Ƃ̏���
      
      //�ψ������n���Ȃ��̂œ����֐��g�p
      (void)nn::dbg::detail::VPrintf(fmt,vlist);

      //�v���b�g�t�H�[�����Ƃ̏��� �����܂�

      va_end(vlist);
    }
  }

  void PrintCore2( const char* fmt , ... )
  {
    if(!printEnable){
      return;
    }
    va_list vlist;
    va_start(vlist, fmt);

    //�v���b�g�t�H�[�����Ƃ̏���
    
    //�ψ������n���Ȃ��̂œ����֐��g�p
    (void)nn::dbg::detail::VPrintf(fmt,vlist);

    //�v���b�g�t�H�[�����Ƃ̏��� �����܂�

    va_end(vlist);
  }
  
  //==============================================================================================
  /**
   * ���胆�[�U�̊��ŃR���p�C�������ꍇ�̂ݓ��삷��Printf�֐�
   *
   * @param   fmt       OS_TPrintf�ɓn�������t��������
   *
   */
  //==============================================================================================
  void UserPrint( const char * fmt, ... )
  {
    if(!printEnable){
      return;
    }
    //����̓w�b�_�ł��Ȃ���gflib��make�����l�̂��o�Ă��܂��E�E�E
    va_list vlist;
    va_start(vlist, fmt);

    //�v���b�g�t�H�[�����Ƃ̏���
    
    //�ψ������n���Ȃ��̂œ����֐��g�p
    (void)nn::dbg::detail::VPrintf(fmt,vlist);

    //�v���b�g�t�H�[�����Ƃ̏��� �����܂�

    va_end(vlist);
  }



#else  //GFL_DEBUG_PRINT
void ResetPrintFlag(){}
bool TogglePrintFlg(void){ return true; }
void SetPrintFlag( const int flag ){GFL_UNUSED(flag);}
void AddPrintFlag( const int flag ){GFL_UNUSED(flag);}
void SubPrintFlag( const int flag ){GFL_UNUSED(flag);}
void UserPrint( const char * fmt, ... ){GFL_UNUSED( fmt );}
#endif  //GFL_DEBUG_PRINT


}  // namespace debug
}  // namespace gfl

