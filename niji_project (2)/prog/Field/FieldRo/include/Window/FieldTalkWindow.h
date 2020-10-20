//==============================================================================
/**
 * @file        FieldTalkWindow.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/04/21(火) 13:03:11
 */
//==============================================================================

#if !defined(__FIELDTALKWINDOW_H__) // 重複定義防止
#define __FIELDTALKWINDOW_H__ // 重複定義防止マクロ
#pragma once

#include "AppLib/include/Tool/TalkWindow.h"   //

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

#include <pml/include/pmlib.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Tool )
class TalkWindow;
GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )


/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldWindow )

class FieldWindowManager;

#if PM_DEBUG
#define DEBUG_MSGLABEL
#endif  // PM_DEBUG


#define  WIN_TYPE_TALK_LAST   WIN_TYPE_GIZA_DOWN

//------------------------------------------------------------------------------
/**
 * @class       FieldTalkWindow
 * @brief       
 * @author      N.Takeda
 * @date        2015/04/30(木) 16:25:30
 */
//------------------------------------------------------------------------------
class FieldTalkWindow
{
  
  enum{
    LOCAL_OPEN_START,
    LOCAL_READ_START,
    LOCAL_READ_WAIT,
    LOCAL_CLOSE_WAIT,
#ifdef DEBUG_MSGLABEL
    DEBUG_OPEN_START,
    DEBUG_READ_START,
    DEBUG_READ_WAIT,
    DEBUG_CLOSE_WAIT,
#endif  // DEBUG_MSGLABEL
    LOCAL_MAX,
  };
  
  static const u32 MSG_BUFLEN = (500*3)+1;

  app::util::Heap                         m_appUtilHeap;
  
  App::Tool::TalkWindow                   *m_pTalkWindow;
  gfl2::heap::HeapBase                    *m_pHeap;
  gfl2::str::StrBuf                       *m_pJoinFestaPlayerName;
 
  u32                                     m_uArcId;
  u32                                     m_uGaixNo;
  u32                                     m_uMsgId;
  gfl2::str::MsgData*                     m_pMsgData;
  gfl2::math::Vector3                     m_cPos;
  App::Tool::TalkWindowType               m_eTalkWindowType;
  Savedata::ConfigSave::MSG_SPEED         m_eMsgSpeed;
  s32                                     m_iTime;
  
  Field::Fieldmap                         *m_pFieldmap;
  Field::FieldWindow::FieldWindowManager  *m_pFieldWindowManager;
  
  s32                                     m_ReadSeq;
  
  void                                    *m_pMsgBuff;
  void                                    *m_pArcReadBuff;
  bool                                    m_bDecide;
  bool                                    m_isMsgDataCreate;
  bool                                    m_isMsgBufferCreate;
 
  s32                                     m_iMouseId;
  size_t                                  m_ResSize;

public:
  void  SetJoinFestaPlayerName( const gfl2::str::STRCODE* pJoinFestaPlayerName ){ m_pJoinFestaPlayerName->SetStr( pJoinFestaPlayerName ); }
  void  SetTimer( const s32 time ){ m_iTime = time; }
  void  SetWindowType( App::Tool::TalkWindowType window_type ){ m_eTalkWindowType = window_type;  }
  void  SetWindowPos( const s32 _iX, const s32 _iY, const s32 _iZ = 0 )
  {
    const gfl2::math::Vector3 vec( (f32)_iX, (f32)_iY, (f32)_iZ );
    m_cPos = vec;
  }
  void  SetMessageSpeed( Savedata::ConfigSave::MSG_SPEED spd ){ m_eMsgSpeed = spd; }
  void  InitMsgData( const u32 arcid, const u32 gaixno );
  void  InitMsgData( gfl2::str::MsgData* pMsgData, const u32 arcid, const u32 gaixno );
  void  InitMsgData( void* pMsgBuf, const u32 arcid, const u32 gaixno );
  void  SetMessage( u32 msgid ){ m_uMsgId = msgid;  }
  void  CloseMsgWin();
  void  End();
  void  SetMsgCursorUserVisible( bool flg );
  void  SetMouseId( const s32 _iMouseId ){  m_iMouseId = _iMouseId; }
  s32 GetMouseId() const {  return m_iMouseId;  }
  
  void  NextMessage( u32 msgid );
  
  bool  Update();

#ifdef DEBUG_MSGLABEL
  gfl2::heap::HeapBase                    *m_pDebHeap;
  void                                    *m_pArcDebLabelBuff;
  u32                                     m_uArcDebLabelSize;
  u32                                     m_uArcDebLabelId;
  u32                                     m_uGaixNoDebLabelId;
  enum{ ARCDEBSTRINGSIZE = 64 };    ///< ラベル文字列サイズ.
#endif  // DEBUG_MSGLABEL
  
  bool  GetDecide(){  return m_bDecide; }
  bool  IsSuspend(){  return m_bDecide; }
  
  App::Tool::TalkWindow::WIN_STATUS  GetStatus( void ) const { return (m_pTalkWindow) ? m_pTalkWindow->GetStatus() : App::Tool::TalkWindow::WIN_STATUS_SETUP_WAIT; }

  void SetVisible( bool isVisible);
  bool IsVisible();

  void SetTimerIconVisible( bool visible_f );
  bool IsTimerIconClose();

  ///        コンストラクタ.
  FieldTalkWindow( Field::Fieldmap *_pFieldmap );
  FieldTalkWindow( Field::FieldWindow::FieldWindowManager *_pFieldWindowManager );
  ///        デストラクタ.
  /*virtual*/ ~FieldTalkWindow();

private:
  //リソースのロード
  void LoadResourceReq();
  bool LoadResourceWait();
  void UnloadResourceReq();
  bool UnloadResourceWait();

};


GFL_NAMESPACE_END( FieldWindow )
GFL_NAMESPACE_END( Field )

#endif // __FIELDTALKWINDOW_H__ 重複定義防止
