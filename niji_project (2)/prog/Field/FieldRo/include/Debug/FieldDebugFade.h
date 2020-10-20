//======================================================================
/**
 * @file FieldDebugFade.h
 * @date 2015/07/24 16:08:03
 * @author miyachi_soichi
 * @brief デバッグメニュー：フェード
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_DEBUG_FADE_H_INCLUDED__
#define __FIELD_DEBUG_FADE_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(debug)
class DebugWinGroup;
GFL_NAMESPACE_END(debug)
GFL_NAMESPACE_END(gfl2)
GFL_NAMESPACE_BEGIN(AppLib)
GFL_NAMESPACE_BEGIN(Fade)
class DemoFade;
GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_END(AppLib)

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
  * @class DebugFade
  * @brief フェード用デバッグ
  */
class DebugFade
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugFade);

public:
  /**
   *  @brief  コンストラクタ
   */
  DebugFade( void );

  /**
   *  @brief  デストラクタ
   */
  ~DebugFade( void );

  /**
   *  @brief  初期化
   */
  void Initialize( gfl2::heap::HeapBase *pHeap, gfl2::str::MsgData* pMsgData );

  /**
   *  @brief  破棄
   */
  void Terminate( void );

  /**
   *  @brief  メニューの作成
   */
  void CreateDebugMenu( gfl2::debug::DebugWinGroup *root );

  inline u32 GetDisp( void ){ return m_nDisp; }
  inline void SetDisp( u32 disp ){ m_nDisp = disp; }

  inline u32 GetOutType( void ){ return m_nOutType; }
  inline void SetOutType( u32 type ){ m_nOutType = type; }

  inline u32 GetInType( void ){ return m_nInType; }
  inline void SetInType( u32 type ){ m_nInType = type; }

  inline bool GetColor( void ){ return m_bBlack; }
  inline void SetColor( bool color ){ m_bBlack = color; }

  gfl2::heap::HeapBase *GetHeap(){ return m_pHeap; }

  inline gfl2::str::MsgData* GetMessageData( void ) { return m_pMsgData; }

  void InitDemoTest( void );
  void EndDemoTest( void );
  void RequestDemoTest( void );


private:
  gfl2::heap::HeapBase  *m_pHeap;
  u32                   m_nDisp;
  u32                   m_nOutType;
  u32                   m_nInType;

  bool                  m_bBlack;

  gfl2::ro::Module        **m_ppRoModules;
  AppLib::Fade::DemoFade  *m_pDemoFade;

  gfl2::str::MsgData*   m_pMsgData;
};

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_DEBUG_FADE_H_INCLUDED__
