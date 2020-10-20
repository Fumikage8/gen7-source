//==============================================================================
/**
 * @file  gfl2_PawnDebug.h
 *  
 * @author  kimura_shin
 * @data  2011/05/25, 10:40
 */
// =============================================================================

#if !defined(__GFL2_PAWNDEBUG_H__)  // 重複定義防止
#define __GFL2_PAWNDEBUG_H__  // 重複定義防止マクロ
#pragma once

#if GFL_PAWN_DEBUG

#include "..\..\macro\include\gfl2_Macros.h"
//#define _TCHAR_DEFINED
#include "core/include/gfl2_HashList.h"
#include "core/include/gfl2_Utility.h"

#include "rtti/include/gfl2_RttiUnity.h"


namespace gfl2 {
namespace pawn {

class PawnDebug : public PawnBase {
public:

  /**
   *   デバッグ実行モード
   */

  enum {
    PAWN_DEBUG_EXEC_MODE_NONE, //!< なし 
    PAWN_DEBUG_EXEC_MODE_STEP, //!< ステップ実行
    PAWN_DEBUG_EXEC_MODE_NEXT, //!< ネクスト実行
    PAWN_DEBUG_EXEC_MODE_CONTINUE //!< コンティニュー
  };

  enum {
    RETURN_CONTINUE = 1,

    VARIABLE_MAX = 32 // とりあえず３２個
  };

GFL_RTTI_ROOT( PawnDebug )

  static b32 s_Initialized;
  // Variable

private:
  AMX_DBG m_AmxDebug;

  // BreakPoint

  class BreakPoint {
    GFL_FORBID_COPY_AND_ASSIGN( BreakPoint );
  public:

    BreakPoint( c8* key, gfl2::heap::HeapBase* heap );

    ~BreakPoint( void ) {
//      GFL_DELETE_ARRAY m_pKeyword;
    }

    static const c8 * GetKeyword( const void* cls ) {
      return static_cast<const BreakPoint*> (cls)->m_pKeyword;
    }

  private:
    c8* m_pKeyword;
  };

  class BreakPointList : public gfl2::core::HashList<BreakPoint> {
  public:

    BreakPointList( gfl2::heap::HeapBase* heap ) : gfl2::core::HashList<BreakPoint>(BreakPoint::GetKeyword, 32, (gfl2::heap::HeapBase*)heap ) {
    }

    ~BreakPointList( void ) {
      clear_and_delete<BreakPoint > ();
    }
  };

  BreakPointList* m_pBreakPoint;

  // pointer
  const c8* m_pOldFileName;
  const c8* m_pCurrentFileName;

  const c8* m_pOldFunction;
  const c8* m_pCurrentFunction;

  u8* m_pDebugData;
  const PawnBase* m_pSharedParent;
#if 0 //@attention DEPEND_ON_OLD_MODULE nijiではHIO常時接続非対応
#ifdef GF_PLATFORM_CTR
  gfl2::hio::Host* m_pHost;
#endif
#endif
  gfl2::heap::HeapBase* m_pHeapDebug;

  enum {
    PAWN_INSTANCE_MAX = 32
  };
  static PawnDebug *s_InstanceList[PAWN_INSTANCE_MAX]; // リストじゃないけどいいか
  u32 m_AmxHash;

  // 2 byte data
  u16 m_OldLine;
  u16 m_CurrentLine;

  // 1 byte data
  //  u8 m_DebugMode;
  u8 m_Pause;
  u8 m_DebugSleep;

  // Debug Control Function
  void DebugContinue( void* ptr = 0 );
  void DebugNext( void* ptr = 0 );
  void DebugStep( void* ptr = 0 );

  //  void AppendBreakPoint( void* ptr = 0 );
  //  void RemoveBreakPoint( void* ptr = 0 );
  void SetBreakPoint( void* ptr = 0 );

  static int PawnDebugBreak( AMX* amx );
  int DebugBreakInstance( AMX* amx );
  s32 ExecuteDebug( void );

  //  PawnDebugFile* GetPawnDebugFile( const u32 fhash );

  //  void PawnDebugValuable(  );
  //  void EnterDebugMode( void );

  int AMXAPI dbg_LoadInfo( const void* data );
  void dbg_GetVariables( AMX_DBG *amxdbg, ucell scopeaddr );
  void UpdateVariables( void );
  // Suspend に必要　Debugじゃなくても使う

  void SetDebugHook( void ) {
    amx_SetDebugHook( GetAmx( ), PawnDebugBreak );
  }

  u32 CalcAmxHash( const c8 * const fname );

protected:
  void LoadPawnDebugInfo( const void* buffer );
  // Suspend に必要　Debugじゃなくても使う

  AMX_DBG* GetAmxDebug( void ) {
    return &m_AmxDebug;
  }

  bool CheckBreakPoint( void );

  class Variable {
  public:

    enum {
      TYPE_INTEGER,
      TYPE_FLOAT
    };

    void SetVariable( const c8* name, const AMX_DBG_SYMBOL* sym ) {
      m_pName = name;
      m_pSym = sym;
    }

    const c8* m_pName;
    const AMX_DBG_SYMBOL* m_pSym;
    void* m_pValue;
    f32 m_F32Value;
    s32 m_S32Value;
    s32 m_Type;
  };

  //  List<Variable> m_Variables;

  Variable m_Variables[VARIABLE_MAX];
  s32 m_VariableCount;

public:
  PawnDebug( gfl2::heap::HeapBase* heap_debug = NULL );
  virtual ~PawnDebug( void );

  void  Initialize( gfl2::heap::HeapBase* heap_debug );

  const c8* GetCurrentFileName( ) const {
    return m_pCurrentFileName;
  }

  const c8* GetOldFileName( ) const {
    return m_pOldFileName;
  }

  u16 GetCurrentLine( ) const {
    return m_CurrentLine;
  }

  u16 GetOldLine( ) const {
    return m_OldLine;
  }

  void SetOldLine( u16 OldLine ) {
    this->m_OldLine = OldLine;
  }

  void SetOldFileName( const c8* OldFileName ) {
    this->m_pOldFileName = OldFileName;
  }
  /**
   * ロード
   * @param buffer　Pawnスクリプトデータポインタ
   * @param size　データサイズ
   */
  virtual void Load( const void* buffer, const size_t size, const c8 * const amxname = "" );
  virtual void LoadShare( const PawnBase* org );
  void LoadPcFile( void* );
  void* LoadPcFile( void* fname, size_t* size );
  void SetPara( void* );

  void InitializePawnDebug( void );
  void ClearPawnDebug( void );

  void EnterDebugMode( void* ptr = 0 );

#if 0 ////@attention DEPEND_ON_OLD_MODULE nijiではHIO常時接続非対応
#if /*GFL_PAWN_DEBUG && */defined(GF_PLATFORM_CTR)

  gfl2::hio::Host* GetHost( void ) {
    return m_pHost;
  }
  // デバッグ情報をクライアントに送る
  void RecieveDebugDataFromClient( gfl2::hio::HostRead* hr );
  bool SendDebugDataToClient( gfl2::hio::HostWrite* hw );
  static void UpdateDebug( gfl2::hio::Host* );
#endif
#endif

  void SetPause( u8 Pause ) {
    this->m_Pause = Pause;
  }

  u8 GetPause( ) const {
    return m_Pause;
  }

  virtual s32 Execute( void );
  //  virtual s32 Update(void);

  u8* GetDebugData( ) const {
    return m_pDebugData;
  }

  /**
   * 最大使用時のスタックバイト数を取得
   * @return バイト数
   */
  size_t GetStackMax( ) const {
    return (size_t) GetAmx( )->userdata[GFL_AMX_MAX_STACK];
  }

  /**
   * 最大使用時のヒープバイト数を取得
   * @return バイト数
   */
  size_t GetHeapMax( ) const {
    return (size_t) GetAmx( )->userdata[GFL_AMX_MAX_HEAP];
  }

  /**
   * 最大使用量のスタックとヒープをプリントする
   * @remark １６セル分のマージンを加算して表示
   */
  void PrintStackHeapMax( void );
};

}
}

#endif //GFL_PAWN_DEBUG

#endif // __GFL2_PAWNDEBUG_H__ 重複定義防止
