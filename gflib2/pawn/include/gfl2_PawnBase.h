//==============================================================================
/**
 * @file    gfl_PawnBase.h
 * @brief   Pawn のラッパークラス.
 * @author    kimura_shin
 * @date    2014/10/23(木) 14:49:48 N.Takeda  更新.
 */
//==============================================================================

#if !defined(__GFL2_PAWNBASE_H__) // 重複定義防止
#define __GFL2_PAWNBASE_H__ // 重複定義防止マクロ
#pragma once

#include "gfl2_Amx.h"

namespace gfl2 {
namespace pawn {


#ifdef GFL_LITTLE_ENDIAN
  #define EndianSwap32( a ) a
  #define EndianSwap16( a ) a
#else
  /**
   * エンディアンスワップ関数  ２バイト１６ビット版
   * @param a スワップする変数またはクラス
   * @return エンディアンスワップされた値
   */
  static inline u16 EndianSwap16( u16 a ) {
    return ((a & 0xff) << 8) | ((a & 0xff00) >> 8);
  }

  /**
   * エンディアンスワップ関数  ４バイト３２ビット版
   * @param a スワップする変数またはクラス
   * @return エンディアンスワップされた値
   */
  static inline u32 EndianSwap32( u32 a ) {
    return ((a & 0xff) << 24) | ((a & 0xff00) << 8) | ((a & 0xff0000) >> 8) | ((a & 0xff000000) >> 24);
  }
#endif  // GFL_LITTLE_ENDIAN


//------------------------------------------------------------------------------
/**
 * @class   PawnBase
 * @brief   Pawnラッパー基底クラス　デバッグ情報を含めない
 * @date    2014/10/28(火) 15:06:58
 */
//------------------------------------------------------------------------------
class PawnBase
{
private:
  AMX     m_Amx;      ///< pawn 実体
  const void  *m_pBuffer;   ///< スマポにしたいよ?
  u32     m_BufferSize; ///< バッファサイズ
  
protected:
  s32           m_SuspendFrame;
  s16           m_ExecuteReturnValue;
  u16           m_HaltFlag; ///< 1bit is enough
  //  cell m_PawnReturnValue;

  PawnBase();
  virtual ~PawnBase( void );


  /**
   * @note  N.Takeda
   * メモリ関連基底.
   */
  virtual u8* PawnNew( const s32 iSize, const s32 iCategory = 0 )
  {
    return new u8 [ iSize ];
  }
  virtual void  PawnDel( u8 *pPtr )
  {
    delete [] pPtr;
  }

  /**
   * @note  N.Takeda
   * メモリ操作関連.
   */
  virtual void  PawnMemCpy( void *pD, const void *pS, const s32 iLength )
  {
  //  memcpy( pD, pS, iLength );
  }
  virtual void  PawnMemFill( void *pPtr, const u8 _uVal, const s32 iLength )
  {
  //  memset( pPtr, _uVal, iLength );
  }

public:
  /// Pawn が所持できるユーザーデータポインタ
  enum {
    GFL_AMX_THIS,
    GFL_AMX_MAX_STACK,
    GFL_AMX_MAX_HEAP,
    GFL_AMX_END         ///< AMX_USERNUM を超えてはいけない。現在は４つまで。
  };

  /**
   * ロード
   * @param buffer　Pawnスクリプトデータポインタ
   * @param size　データサイズ
   */
  virtual void Load( const void* buffer, const size_t size, const c8 * const amxname = "" );
  virtual void LoadShare( const PawnBase* org );

  /// 関数名から関数のインデックスを取得
  s32 GetIndex( const c8 * const com );

  /**
   * スタックに整数を積む
   * @param val 整数
   */
  inline void PushInteger( s32 val ) {
    amx_Push( GetAmx( ), val );
  }

  /**
   * スタックに配列を積む
   * @param val　配列
   * @param cnt　配列サイズ
   * @return 確保されたメモリポインタ
   */
  template<typename T>
  inline cell* PushArray( T val[], s32 cnt ) {
    cell* adr;
    amx_Push( GetAmx( ), cnt ); ///< Pawnで受け取る場合は引数の順番が逆になる！
    amx_PushArray( GetAmx( ), 0, &adr, (cell*) val, cnt );
    return adr;
  }

  /**
   * 配列をプッシュした時には開放が必要
   * @param val
   */
  inline void ReleaseArray( cell* adr ) {
    amx_Release( GetAmx( ), (cell) adr );
  }

  /**
   * スタックに浮動小数を積む
   * @param val 浮動小数
   */
  inline void PushFloat( f32 val ) {
    amx_Push( GetAmx( ), amx_ftoc( val ) );
  }

  /**
   * スタックに文字列を積む
   * @param str 文字列
   */
  inline void PushString( c8* str ) {
    amx_PushString( GetAmx( ), 0, 0, str, 0, 0 );
  }

  /**
   *   C の関数をテーブルにセットするマクロ
   * @param  func 関数
   * @remark 文字列名、関数に展開する
   */
#define GFL_PAWN_FUNC(func) { # func, func },

  /**
   *   C の関数をテーブルにセットするマクロ
   * @param  pre 関数の頭につける文字列
   * @param  func 関数
   * @remark 文字列名、関数に展開する。ライブラリー化したいときに便利
   */
#define GFL_PAWN_FUNC_PREFIX(pre,func) { # func, pre ## func },

  /**
   *   関数テーブルの終端
   */
#define GFL_PAWN_FUNC_END { 0, 0 }

  /**
   *   C の関数テーブル宣言
   * @remark 例）static const GFL_PAWN_FUNC_TABLE sFuncTable[] = {\n
   *    GFL_PAWN_FUNC(Function)\n
   *    GFL_PAWN_FUNC_END\n
   *  };\n
   */
#define GFL_PAWN_FUNC_TABLE AMX_NATIVE_INFO

  /// C のスクリプト関数テーブルを Pawn に登録する.
  void RegisterPawnFunction( const GFL_PAWN_FUNC_TABLE native_func_tbl[] );
  
  /**
   * Pawn のパブリック変数のポインタを取得
   * @param str　変数名
   * @return 変数ポインタ
   */
  cell* GetGlobalParameterPointer( const c8 * const str );

  inline s32 GetGlobalParameterInteger( const c8 * const str, const s32 def = 0 ) {
    cell* ptr = GetGlobalParameterPointer( str );
    return ptr ? * reinterpret_cast<s32*> (ptr) : def;
  }

  inline f32 GetGlobalParameterFloat( const c8 * const str, const f32 def = 0.0f ) {
    cell* ptr = GetGlobalParameterPointer( str );
    return ptr ? amx_ctof( *ptr ) : def;
  }

  inline void SetGlobalParameterInteger( const c8 * const str, s32 val ) {
    cell* ptr = GetGlobalParameterPointer( str );
    if( ptr ){
      *reinterpret_cast<s32*> (ptr) = val;
    }
  }

  inline void SetGlobalParameterFloat( const c8 * const str, f32 val ) {
    cell* ptr = GetGlobalParameterPointer( str );
    if( ptr ){
      *reinterpret_cast<f32*> (ptr) = val;
    }
  }

  /// インデックス番号の関数を実行.
  cell CallPawnFunction( const s32 index );

  /**
   *   関数名でＰａｗｎ関数をコールする
   * @param  name 関数名
   * @return Ｐａｗｎ関数からの返り値
   */
  inline cell CallPawnFunction( const c8 * const name )
  {
    /// 関数名から関数のインデックスを取得
    const s32 idx = GetIndex( name );
    if( 0 <= idx ){
      /// インデックス番号の関数を実行.
      return CallPawnFunction( idx );
    }
    return -1;
  }

  /**
   * メイン関数を実行
   * @return Pawnからの返り値
   */
  virtual s32 Execute( void );
  s32 ExecuteStep( void );

  /**
   * amx を取得
   * @return amx ポインタ
   */
  AMX* GetAmx( ) {
    return &m_Amx;
  }

  const AMX* GetAmx( ) const {
    return &m_Amx;
  }

  /**
   *  ユーザー定義したクラスポインタ
   * @param amx AMX からクラスを取得
   * @return クラスポインタ
   */
  template<typename T>
  static T GetClass( AMX* amx )
  {
    return static_cast<T> (amx->userdata[GFL_AMX_THIS]);
  }

  void* IsAvailable( void )
  {
    return GetAmx( )->userdata[GFL_AMX_THIS];
  }

  void InitializeAmx( void );
  void ClearAmx( void );

  const u8* GetBuffer( ) const
  {
    return (u8*) m_pBuffer;
  }

  u32 GetBufferSize( void ) const
  {
    return m_BufferSize;
  }

  /// Update() の返り値

  enum {
    GFL_PAWN_NOT_AVAILABLE = -3,
    GFL_PAWN_DEBUGGING,
    GFL_PAWN_SUSPENDING,
    GFL_PAWN_FINISHED = AMX_ERR_NONE,
    GFL_PAWN_EXECUTE = AMX_ERR_SLEEP,
  };

  /// フレームを進めていいのか？

  inline bool IsExecuted( s32 ret )
  {
    return GFL_PAWN_SUSPENDING < ret;
  }

  inline bool IsFinished( s32 ret )
  {
    return ret == GFL_PAWN_FINISHED;
  }

/**
 * デバッグ情報を含んだAMXなのか？
 * @param buffer　AMXバッファ
 * @return 真　デバッグ情報を含んだAMX
 */ 
  inline bool IsDebug( const void* buffer){
    return ( reinterpret_cast<const AMX_HEADER*>( buffer )->flags & AMX_FLAG_DEBUG  ) !=0; ///< int -> bool WarnningC4800解決.
  }
  /**
   *   更新
   * @param  無し
   * @return　GFL_PAWN_*
   */

  s32 Update( void );

  /**
   * スリープ状態を解除してMainの初めにプログラムポインタを戻す
   */
  void ClearSleepAndRestart( void )
  {
    AMX* amx = GetAmx( );
    if( amx->base ){
      amx->hea=amx->hlw;    ///< ヒープをリセット
      amx->stk=amx->stp;    ///< スタックをリセット
    }
    m_SuspendFrame = 0;
    m_ExecuteReturnValue = AMX_ERR_NONE;
  }

  inline void SetSuspendFrame( s32 SuspendFrame )
  {
    this->m_SuspendFrame = SuspendFrame;
  }

  inline bool IsFinished( void )
  {
    return m_ExecuteReturnValue == GFL_PAWN_FINISHED;
  }

  /**
   * スクリプトの強制中断
   */
  void Halt( void )
  {
    m_HaltFlag = true;
    /// 中断させる
    amx_RaiseError( GetAmx( ), AMX_ERR_SLEEP );
  }

  /**
   * 強制中断されたスクリプトを再開
   */
  void Resume( void )
  {
    m_HaltFlag = false;
  }

  /**
   * 強制中断中か？
   * @return 真　中断中　偽　中断中でない
   */
  bool IsHalting( ) const
  {
    return static_cast<bool>(m_HaltFlag !=0); ///< int -> bool WarnningC4800解決.
  }

#if GFL_PAWN_DEBUG
  static const c8 * const s_cErrorMessage[];
  #define PAWN_ASSERT(err,mes) GFL_ASSERT_MSG( (err == AMX_ERR_NONE || err == AMX_ERR_SLEEP), "%s : %s\n", mes, gfl2::pawn::PawnBase::s_cErrorMessage[err] )
#else
  #define PAWN_ASSERT(err,mes)
#endif
};

}
}

#endif // __GFL2_PAWNBASE_H__ 重複定義防止
