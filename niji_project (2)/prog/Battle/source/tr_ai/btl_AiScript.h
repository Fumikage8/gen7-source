/**
 * @file btl_AiScript.h
 * @brief 戦闘AIスクリプト
 * @author obata_toshihiro
 * @date 2015.04.02
 */

#ifndef BTL_AI_SCRIPT_H_INCLUDED
#define BTL_AI_SCRIPT_H_INCLUDED

#include <pawn/include/gfl2_Pawn.h>
#include "../btl_common.h"


GFL_NAMESPACE_BEGIN( btl )

class AiScriptCommandHandler;



/**
 * @class AiScript
 * @brief 戦闘AIスクリプトを管理する
 */
class AiScript
{
  GFL_FORBID_COPY_AND_ASSIGN( AiScript );

public:

  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param heapForScriptBinary  スクリプトバイナリを確保するヒープ
   * @param heapForFileRead      ファイル読み込みに使用する一時ヒープ
   */
  //---------------------------------------------------------------------------
  AiScript( HeapHandle heapForScriptBinary, HeapHandle heapForFileRead );

  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  virtual ~AiScript();



  //---------------------------------------------------------------------------
  /**
   * @brief スクリプトの読み込みを開始する
   * @param heapForScriptExec スクリプトの実行に使用する一時ヒープ
   * @param scriptNo          読み込むスクリプトNo.
   * @retval true  読み込みを開始した
   * @retval false 読み込みを開始できなかった
   */
  //---------------------------------------------------------------------------
  bool StartLoadScript( HeapHandle heapForScriptExec, BtlAiScriptNo scriptNo );

  //---------------------------------------------------------------------------
  /**
   * @brief スクリプトの読み込み終了を待つ
   * @retval true   読み込みが終了した
   * @retval false  読み込みが終了していない
   */
  //---------------------------------------------------------------------------
  bool WaitLoadScript( void );

  //---------------------------------------------------------------------------
  /**
   * @brief スクリプトの実行に必要なパラメータを設定する
   * @param commandHandler  関連づけるコマンドハンドラ
   */
  //---------------------------------------------------------------------------
  void SetExecParameter( AiScriptCommandHandler* commandHandler );

  //---------------------------------------------------------------------------
  /**
   * @brief スクリプトを実行する
   * @retval true   実行が終了した
   * @retval false  実行が終了していない
   */
  //---------------------------------------------------------------------------
  bool Execute( void );

  //---------------------------------------------------------------------------
  /**
   * @brief 読み込んだスクリプトを破棄する
   */
  //---------------------------------------------------------------------------
  void UnLoadScript( void );




  /**
   * @brief スクリプトの実行結果
   */
  struct Result
  {
    // 共通
    s32 score;  // 点数

    // 入れ替え
    bool isPokeChangeEnable;  // 入れ替え可能か？
  };

  //---------------------------------------------------------------------------
  /**
   * @brief 実行結果を取得する
   * @param[out] dest  実行結果の格納先
   */
  //---------------------------------------------------------------------------
  void GetResult( Result* dest );




#if PM_DEBUG
  //---------------------------------------------------------------------------
  /**
   * @brief スクリプトを破棄し、アーカイブファイルを開きなおす
   */
  //---------------------------------------------------------------------------
  void Reset( void );
#endif




private:
  void Setup( void );
  void Cleanup( void );
  void OpenArcFile( void );
  void CloseArcFile( void );
  void CreatePawnBuffer( void );
  void DeletePawnBuffer( void );
  u32  GetArcDataIndex( u32 scriptNo );



private:

  /**
   * @brief 読み込み状態
   */
  enum LoadState
  {
    LOAD_STATE_NOT_LOADED, // 読み込んでいない
    LOAD_STATE_LOADING,    // 読み込み中
    LOAD_STATE_LOADED,     // 読み込みが完了した
  };

  HeapHandle          m_heapForScriptBinary;
  HeapHandle          m_heapForFileRead;
  gfl2::fs::ArcFile*  m_arcHandle;
  gfl2::pawn::Pawn*   m_pawn;
  void*               m_pawnBuf;
  u32                 m_pawnBufSize;
  LoadState           m_loadState;
  u32                 m_loadedScriptDataId;
  u32                 m_loadingScriptSize;

};


GFL_NAMESPACE_END( btl )


#endif // BTL_AI_SCRIPT_H_INCLUDED