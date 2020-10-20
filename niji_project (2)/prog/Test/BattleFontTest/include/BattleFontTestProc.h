#if PM_DEBUG

#if !defined(__BATTLE_FONT_TEST_PROC_H_INCLUDED__)
#define __BATTLE_FONT_TEST_PROC_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    BattleFontTestProc.h
 * @date    2016/10/03 11:46:34
 * @author  fukushima_yuya
 * @brief   バトルフォントテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <GameSys/include/GameProc.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Str.h>
#include <ui/include/gfl2_UI_DeviceManager.h>
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>


//----------------------------------------------------------------------
// @brief   前方宣言
//----------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(System)
class nijiAllocator;
GFL_NAMESPACE_END(System)
// ---[ end ]---

//----------------------------------------------------------------------
// @brief   マクロ
//----------------------------------------------------------------------
#ifndef SET_CREATE_FUNC
#define SET_CREATE_FUNC( funcname ) \
  bool Create##funcname( void );    \
  bool Delete##funcname( void );
#endif


//----------------------------------------------------------------------
// @brief   バトルフォントテスト
//----------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(BattleFont)

class BattleFontTestProc
  : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleFontTestProc );

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  BattleFontTestProc( void );
  ~BattleFontTestProc( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   継承関数群
  //-------------------------------------------------------
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* manager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* manager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* manager );
  virtual void DrawFunc( gfl2::proc::Manager* manager, gfl2::gfx::CtrDisplayNo dispNo );

private:
  SET_CREATE_FUNC( Allocator );
  SET_CREATE_FUNC( RenderingPipeLine );
  SET_CREATE_FUNC( BattleFont );
  SET_CREATE_FUNC( DrawUtilText );
  SET_CREATE_FUNC( MsgData );
  SET_CREATE_FUNC( StrBuf );

private:
  void InputButton( void );

private:
  void DrawText( void );

private:
  enum {
    LANGUAGE_JAPAN,
    LANGUAGE_ENGLISH,
    LANGUAGE_FRANCE,
    LANGUAGE_ITALY,
    LANGUAGE_GERMANY,
    LANGUAGE_SPAIN,
    LANGUAGE_KOREA,
    LANGUAGE_CHS,
    LANGUAGE_CHT,
    LANGUAGE_MAX,
  };

  static const u32 LINE_MAX = 17;

private:
  gfl2::heap::NwAllocator*    m_pMemAllocator;
  gfl2::heap::NwAllocator*    m_pDevAllocator;
  System::nijiAllocator*      m_pNijiAllocator;

  gfl2::util::DrawUtilText*                 m_pDrawUtilText;
  gfl2::util::UtilTextRenderingPipeLine*    m_pRenderingPipeLine;

  gfl2::ui::DeviceManager*    m_pDeviceManager;

  gfl2::str::MsgData*         m_pMsgData[LANGUAGE_MAX];
  gfl2::str::StrBuf*          m_pStrBuf[LINE_MAX];
  gfl2::str::StrBuf*          m_pTempStrBuf;

  gfl2::proc::Result          m_ProcResult;

private:
  u8    m_MainSeq;
  u8    m_SubSeq;

  s32   m_StartX;
  u32   m_PageNum;
};

GFL_NAMESPACE_END(BattleFont)
GFL_NAMESPACE_END(Test)


#endif  // __BATTLE_FONT_TEST_PROC_H_INCLUDED__

#endif  // #if PM_DEBUG