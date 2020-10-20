#ifndef __GFL_DEBUGTICK_H__
#define __GFL_DEBUGTICK_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DebugTick.h
 *	@brief  �`�b�N�_���v�\���V�X�e��
 *	@author	tomoya takahashi
 *	@date		2010.11.10
 *
 *
 *	�g�p���@
 *	�@�P�D�o�b�t�@�̊m��
 *	�@�@�I���W�i���̃l�[���X�y�[�X���ɁA�O���[�o���ϐ����`���܂��B
 *      GFL_DEBUG_TICK_SETUP(VALNAME, 10 ); // ���̃}�N�����ŁA�O���[�o���ϐ����錾����܂��B
 *
 *    �Q�D�`�b�N�_���v�@�\��L���ɂ���B 
 *      // �_���v�@�\�̃t���O�͏����lfalse�ł��B�@
 *      //�`�F�b�N����Ƃ��́A�K���@�\��ON�ɂ��Ă��������B
 *    �@GFL_DEBUG_TICK_SET_FLAG(VALNAME,true);
 *
 *    �R�D�v������
 *    �@{
 *        GFL_DEBUG_TICK_START(VALNAME, 0, "�`�揈��" );    // �v���J�n
 *
 *        Draw( MAIN );
 *
 *        GFL_DEBUG_TICK_END(VALNAME,0);  // �v���I��
 *      }
 *
 *    �S�D�v������\��
 *    �@GFL_DEBUG_TICK_DUMP_ALL(VALNAME); //�S���̕\��
 *
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn.h>

#include <gfl_Macros.h>
#include <debug/gfl_DebugPrint.h>

namespace gfl
{
namespace debug
{
#undef  GFL_DEBUG
#define  GFL_DEBUG 0
#undef  GFL_RELEASE
#define GFL_RELEASE

#if GFL_DEBUG
  
//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
// �_���v�\��
#define GFL_DEBUG_TICK_DUMP_PRINT_FLG (gfl::debug::PRINT_FLAG_LEVEL2 | gfl::debug::PRINT_FLAG_CATEGORY_DEBUG)



//-----------------------------------------------------------------------------
/**
 *        �}�N����`
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �K�v�ȃo�b�t�@�̒�`
 *
 *	@param	valName �ϐ���
 *  @param  num     �o�b�t�@�v�f�ő吔
 */
//-----------------------------------------------------------------------------
#define GFL_DEBUG_TICK_SETUP(valName, num)   static gfl::debug::DebugTickDump valName(num)

//----------------------------------------------------------------------------
/**
 *	@brief  �_���v�@�\��ON�EOFF
 *
 *	@param	valName �ϐ���
 *	@param  flag    �_���v�@�\�t���O�@true:ON   false:OFF
 */
//-----------------------------------------------------------------------------
#define GFL_DEBUG_TICK_SET_FLAG(valName, flag) valName.SetFlag( (flag) )

//----------------------------------------------------------------------------
/**
 *	@brief  �_���v���̃��Z�b�g
 *
 *	@param	valName �ϐ���
 */
//-----------------------------------------------------------------------------
#define GFL_DEBUG_TICK_RESET(valName)      valName.ResetTickDump()

//----------------------------------------------------------------------------
/**
 *	@brief  �`�b�N�v���J�n
 *
 *	@param	valName �ϐ���
 *	@param  index   �i�[��C���f�b�N�X
 *	@param  name    ��񖼁i���O�o�͂Ɏg�p�j
 */
//-----------------------------------------------------------------------------
#define GFL_DEBUG_TICK_START(valName, index, name) valName.StartTickDump( (index), (name) )

//----------------------------------------------------------------------------
/**
 *	@brief  �`�b�N�v���I��
 *
 *	@param	valName �ϐ���
 *	@param  index   �i�[��C���f�b�N�X
 */
//-----------------------------------------------------------------------------
#define GFL_DEBUG_TICK_END(valName, index) valName.EndTickDump( (index) )

// 
//----------------------------------------------------------------------------
/**
 *	@brief  �_���v
 *
 *	@param	valName �ϐ���
 *	@param  index   �i�[��C���f�b�N�X
 */
//-----------------------------------------------------------------------------
#define GFL_DEBUG_TICK_DUMP_ALL(valName) valName.PrintAll()
#define GFL_DEBUG_TICK_DUMP(valName, index) valName.Print( (index) )



//----------------------------------------------------------------------------
/**
 *	@brief  �ʕb���擾
 *
 *  @param  index �C���f�b�N�X
 *
 *  @retval �ʕb 
 */
//-----------------------------------------------------------------------------
#define GFL_DEBUG_TICK_US_GET(valName, index) valName.GetTickNowUs((index))




//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
typedef struct
{
  const char * printname;
  nn::os::Tick start; 
  nn::os::Tick end;
  nn::os::Tick ave;
  int          count;
} DebugTick;


//-----------------------------------------------------------------------------
/**
 *					�֐���`
*/
//-----------------------------------------------------------------------------
extern void ResetTickBuffer( DebugTick* buff, int num );
extern void StartTickBuffer( DebugTick* buff, int index, const char* name );
extern void EndTickBuffer( DebugTick* buff, int index );
extern void DumpTickBuffer( const DebugTick* buff, int num );
  
//-----------------------------------------------------------------------------
/**
 *      �N���X�錾
 *
 *      �����i�łɃf�[�^���܂߂Ȃ��ׂɁA
 *      �@���̃N���X�𒼐ڎg�p�����A���ɒ�`����Ă���}�N�����g�p���Ă��������B
 *      
 */
//-----------------------------------------------------------------------------
class DebugTickDump
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugTickDump);

public:
  enum{
    TICK_BUFFER_MAX = 16    // tblnum�Ɏw��ł���ő吔
  };
  
public:
  // �R���X�g���N�^
  DebugTickDump( u32 tblnum );

  // �f�X�g���N�^
  ~DebugTickDump();

  //----------------------------------------------------------------------------
  /**
   *	@brief  �`�b�N�_���v�@�\��ON�EOFF
   *
   *	@param	flag �t���O  true:ON   false:OFF
   */
  //-----------------------------------------------------------------------------
  void SetFlag( bool flag );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �`�b�N�_���v�@�\�̏�Ԃ��擾
   *
   *	@retval true  ON
   *	@retval false OFF
   */
  //-----------------------------------------------------------------------------
  bool IsFlagOn( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  �`�b�N�o�b�t�@�̏��N���A
   */
  //-----------------------------------------------------------------------------
  void ResetTickDump( void );

  // �`�b�N�v���J�n
  void StartTickDump( u32 index, const char* cp_name );

  // �`�b�N�v���I��
  void EndTickDump( u32 index );


  // �`�b�N���̕\��
  void PrintAll( void ) const;
  void Print( u32 index ) const;

  // �`�b�N���ϒl�擾
  s32 GetTickAveUs( u32 index );

  // �`�b�N���O�l���擾
  s32 GetTickNowUs( u32 index );
  

  
private:
  
  bool        m_bFlag;

  DebugTick   m_TickTbl[TICK_BUFFER_MAX];
  u32         m_TickTblNum;
};


#else     // GFL_DEBUG

#define GFL_DEBUG_TICK_SETUP(valName, num)  /**/ 
#define GFL_DEBUG_TICK_SET_FLAG(valName, flag) /**/ 
#define GFL_DEBUG_TICK_RESET(valName) /**/ 
#define GFL_DEBUG_TICK_START(valName, index, name) /**/
#define GFL_DEBUG_TICK_END(valName, index) /**/
#define GFL_DEBUG_TICK_DUMP_ALL(valName) /**/
#define GFL_DEBUG_TICK_DUMP(valName, index) /**/

#endif    // GFL_DEBUG  


}//namespace debug
}//namespace gfl
#endif // __GFL_DEBUGTICK_H__
