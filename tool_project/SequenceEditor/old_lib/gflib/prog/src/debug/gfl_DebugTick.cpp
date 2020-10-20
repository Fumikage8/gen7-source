//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DebugTick.cpp
 *	@brief  �`�b�N�o�b�t�@�����O�@
 *	@author	tomoya takahashi
 *	@date		2010.11.10
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>


#include "debug/gfl_DebugTick.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					����
*/
//-----------------------------------------------------------------------------

namespace gfl
{
namespace debug
{
  
#if GFL_DEBUG

//----------------------------------------------------------------------------
/**
 *	@brief  �`�b�N�o�b�t�@���e�����Z�b�g
 *
 *	@param	buff      �o�b�t�@
 *	@param	num       �v�f��
 */
//-----------------------------------------------------------------------------
void ResetTickBuffer( DebugTick* buff, int num )
{
  for( int i =0; i<num; i++ ){
    buff[i].printname  = NULL;
    buff[i].start = nn::os::Tick();
    buff[i].end   = nn::os::Tick();
    buff[i].ave   = nn::os::Tick();
    buff[i].count = 0;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  index�Ԗڂ̃o�b�t�@�����O���J�n
 *
 *	@param	buff      �o�b�t�@
 *	@param	index     �C���f�b�N�X
 *	@param	name      �\����
 */
//-----------------------------------------------------------------------------
void StartTickBuffer( DebugTick* buff, int index, const char* name )
{
  buff[index].printname  = name;
  buff[index].start = nn::os::Tick::GetSystemCurrent();
}

//----------------------------------------------------------------------------
/**
 *	@brief  index�Ԗڂ̃o�b�t�@�����O���I��
 *
 *	@param	buff    �o�b�t�@
 *	@param	index   �C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void EndTickBuffer( DebugTick* buff, int index )
{
  buff[index].end = nn::os::Tick::GetSystemCurrent();

  // �A�x���[�W�v�Z
  buff[index].ave += buff[index].end - buff[index].start;
  buff[index].count ++;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�b�t�@�̓��e���_���v
 *
 *	@param	buff  �o�b�t�@
 *	@param	num   �o�b�t�@��
 */
//-----------------------------------------------------------------------------
void DumpTickBuffer( const DebugTick* buff, int num )
{
  GFL_PRINT_EX( GFL_DEBUG_TICK_DUMP_PRINT_FLG, "---- debug::tick dump start ------\n" );
  for( int i=0; i<num; i++ ){

    if(buff[i].printname == NULL){ continue; }
    
    s64 now = buff[i].end - buff[i].start;
    s64 ave = buff[i].ave / static_cast<s64>(buff[i].count);

    GFL_PRINT_EX( GFL_DEBUG_TICK_DUMP_PRINT_FLG, "================================\n" );
    GFL_PRINT_EX( GFL_DEBUG_TICK_DUMP_PRINT_FLG, "name::%s\n", buff[i].printname );
    GFL_PRINT_EX( GFL_DEBUG_TICK_DUMP_PRINT_FLG, "tick %d\n", now );
    GFL_PRINT_EX( GFL_DEBUG_TICK_DUMP_PRINT_FLG, "micro %d\n", nn::os::Tick(now).ToTimeSpan().GetMicroSeconds() );
    GFL_PRINT_EX( GFL_DEBUG_TICK_DUMP_PRINT_FLG, "ave tick %d\n", ave );
    GFL_PRINT_EX( GFL_DEBUG_TICK_DUMP_PRINT_FLG, "ave micro %d\n", nn::os::Tick(ave).ToTimeSpan().GetMicroSeconds() );

  }
  GFL_PRINT_EX( GFL_DEBUG_TICK_DUMP_PRINT_FLG, "---- debug::tick dump end ------\n" );
}





//-----------------------------------------------------------------------------
/**
 *      �N���X�錾
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^
 */
//-----------------------------------------------------------------------------
DebugTickDump::DebugTickDump( u32 tblnum ) :
  m_bFlag(false)
{
  GFL_ASSERT( tblnum < TICK_BUFFER_MAX );
  m_TickTblNum = tblnum;
  
  // �S�̂̏����N���A
  ResetTickDump();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
DebugTickDump::~DebugTickDump()
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  �`�b�N�_���v�@�\��ON�EOFF
 *
 *	@param	flag �t���O  true:ON   false:OFF
 */
//-----------------------------------------------------------------------------
void DebugTickDump::SetFlag( bool flag )
{
  m_bFlag = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �`�b�N�_���v�@�\�̏�Ԃ��擾
 *
 *	@retval true  ON
 *	@retval false OFF
 */
//-----------------------------------------------------------------------------
bool DebugTickDump::IsFlagOn( void ) const
{
  return m_bFlag;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �`�b�N�o�b�t�@�̏��N���A
 */
//-----------------------------------------------------------------------------
void DebugTickDump::ResetTickDump( void )
{
  if(m_bFlag){
    ResetTickBuffer( m_TickTbl, m_TickTblNum );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �`�b�N�v���J�n
 *
 *	@param	index     �v����o�b�t�@�C���f�b�N�X
 *	@param  cp_name   �v�����ږ�
 */
//-----------------------------------------------------------------------------
void DebugTickDump::StartTickDump( u32 index, const char* cp_name )
{
  if(m_bFlag){
    StartTickBuffer( m_TickTbl, index, cp_name ); 
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �`�b�N�v���I��
 *
 *	@param	index   �v����C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void DebugTickDump::EndTickDump( u32 index )
{
  if(m_bFlag){
    EndTickBuffer( m_TickTbl, index );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �S�`�b�N�������O�֏o��
 */
//-----------------------------------------------------------------------------
void DebugTickDump::PrintAll( void ) const
{
  if(m_bFlag){
    DumpTickBuffer( m_TickTbl, m_TickTblNum );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  index�̃`�b�N�������O�֏o��
 *
 *	@param	index   �C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void DebugTickDump::Print( u32 index ) const
{
  if(m_bFlag){
    DumpTickBuffer( &m_TickTbl[index], 1 ); 
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���σʕb�擾
 *
 *	@param	index   �C���f�b�N�X
 *
 *	@return ���σʕb�擾
 */
//-----------------------------------------------------------------------------
s32 DebugTickDump::GetTickAveUs( u32 index )
{
  if(m_bFlag){
    s64 ave = m_TickTbl[index].ave / static_cast<s64>(m_TickTbl[index].count);
    return static_cast<s32>(nn::os::Tick(ave).ToTimeSpan().GetMicroSeconds());
  }
  return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���O�̃ʕb�擾
 *	i
 *
 *	@param	index   �C���f�b�N�X
 *
 *	@return ���σʕb�擾
 */
//-----------------------------------------------------------------------------
s32 DebugTickDump::GetTickNowUs( u32 index )
{
  if(m_bFlag){
    s64 now = m_TickTbl[index].end - m_TickTbl[index].start;
    return static_cast<s32>(nn::os::Tick(now).ToTimeSpan().GetMicroSeconds());
  }
  return 0;
}



#endif // GFL_DEBUG
  
}//namespace debug
}//namespace gfl
