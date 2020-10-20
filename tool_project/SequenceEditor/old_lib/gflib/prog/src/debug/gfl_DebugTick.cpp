//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DebugTick.cpp
 *	@brief  チックバッファリング　
 *	@author	tomoya takahashi
 *	@date		2010.11.10
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>


#include "debug/gfl_DebugTick.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------

namespace gfl
{
namespace debug
{
  
#if GFL_DEBUG

//----------------------------------------------------------------------------
/**
 *	@brief  チックバッファ内容をリセット
 *
 *	@param	buff      バッファ
 *	@param	num       要素数
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
 *	@brief  index番目のバッファリングを開始
 *
 *	@param	buff      バッファ
 *	@param	index     インデックス
 *	@param	name      表示名
 */
//-----------------------------------------------------------------------------
void StartTickBuffer( DebugTick* buff, int index, const char* name )
{
  buff[index].printname  = name;
  buff[index].start = nn::os::Tick::GetSystemCurrent();
}

//----------------------------------------------------------------------------
/**
 *	@brief  index番目のバッファリングを終了
 *
 *	@param	buff    バッファ
 *	@param	index   インデックス
 */
//-----------------------------------------------------------------------------
void EndTickBuffer( DebugTick* buff, int index )
{
  buff[index].end = nn::os::Tick::GetSystemCurrent();

  // アベレージ計算
  buff[index].ave += buff[index].end - buff[index].start;
  buff[index].count ++;
}

//----------------------------------------------------------------------------
/**
 *	@brief  バッファの内容をダンプ
 *
 *	@param	buff  バッファ
 *	@param	num   バッファ数
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
 *      クラス宣言
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 */
//-----------------------------------------------------------------------------
DebugTickDump::DebugTickDump( u32 tblnum ) :
  m_bFlag(false)
{
  GFL_ASSERT( tblnum < TICK_BUFFER_MAX );
  m_TickTblNum = tblnum;
  
  // 全体の情報をクリア
  ResetTickDump();
}

//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
DebugTickDump::~DebugTickDump()
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  チックダンプ機能のON・OFF
 *
 *	@param	flag フラグ  true:ON   false:OFF
 */
//-----------------------------------------------------------------------------
void DebugTickDump::SetFlag( bool flag )
{
  m_bFlag = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief  チックダンプ機能の状態を取得
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
 *	@brief  チックバッファの情報クリア
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
 *	@brief  チック計測開始
 *
 *	@param	index     計測先バッファインデックス
 *	@param  cp_name   計測項目名
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
 *	@brief  チック計測終了
 *
 *	@param	index   計測先インデックス
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
 *	@brief  全チック情報をログへ出力
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
 *	@brief  indexのチック情報をログへ出力
 *
 *	@param	index   インデックス
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
 *	@brief  平均μ秒取得
 *
 *	@param	index   インデックス
 *
 *	@return 平均μ秒取得
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
 *	@brief  直前のμ秒取得
 *	i
 *
 *	@param	index   インデックス
 *
 *	@return 平均μ秒取得
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
