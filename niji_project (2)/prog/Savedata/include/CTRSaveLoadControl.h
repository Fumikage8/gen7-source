#if   defined(GF_PLATFORM_CTR)
#ifndef __CTRSAVELOADCONTROL_H__
#define __CTRSAVELOADCONTROL_H__
//===================================================================
/**
 * @file    CTRSaveLoadControl.h
 * @brief   セーブロードのコントロールを行うクラス
 * @author  k.ohno
 * @date    2011.3.8
 */
//===================================================================
#pragma once
#include <Savedata/include/SaveLoadControl.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

#include <fs/include/CTR/gfl2_FsCtrBackupSystem.h>
#include <fs/include/CTR/gfl2_FsCtrBackupFile.h>

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(Savedata)

  class ISaveDataTotal;
  class CTRSaveLoadControlThread;
  struct FileInfo;

//--------------------------------------------------------------
/**
 * @class CTRSaveLoadControl
 */
//--------------------------------------------------------------
class CTRSaveLoadControl : public SaveLoadControl
{
  GFL_FORBID_COPY_AND_ASSIGN(CTRSaveLoadControl); //コピーコンストラクタ＋代入禁止

public:

  //--------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //--------------------------------------------------------------
  CTRSaveLoadControl(gfl2::heap::HeapBase* heap, int indexmax);

  //--------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //--------------------------------------------------------------
  virtual ~CTRSaveLoadControl();

  //--------------------------------------------------------------
  /**
   * @brief セーブデータの状態を取得する
   * @param[in] pTempHeap 作業用ヒープ
   * @return  ARC_STATUS  
   */
  //--------------------------------------------------------------
  virtual ARC_STATUS CheckArchiveStatus( gfl2::heap::HeapBase * pTempHeap );

  //--------------------------------------------------------------
  /**
   * @brief   セーブ領域作成
   * @param[in]   pHeap         作業用ヒープ
   * @retval  gfl2::fs::FsCtrBackupSystem::RESULT_OK       生成に成功
   * @retval  gfl2::fs::FsCtrBackupSystem::RESULT_NG_xxxx  生成に失敗
   * @caution 事前にセーブデータをクリアしておくこと
   */
  //--------------------------------------------------------------
  gfl2::fs::FsCtrBackupSystem::Result FormatAll( gfl2::heap::HeapBase* pHeap = NULL );

  //--------------------------------------------------------------
  /**
   * @brief   セーブデータアーカイブの非同期初期化処理開始
   */
  //--------------------------------------------------------------
  virtual void FormatStart( gfl2::heap::HeapBase * pTempHeap );

  //--------------------------------------------------------------
  /**
   * @brief   セーブデータアーカイブの非同期初期化処理メイン
   * @param[out]  res     処理結果を受け取るポインタ
   * @retval      true    処理終了
   * @retval      false   処理継続中
   */
  //--------------------------------------------------------------
  virtual bool FormatMain( gfl2::fs::FsCtrBackupSystem::Result * res );

  //--------------------------------------------------------------
  //--------------------------------------------------------------
  gfl2::fs::FsCtrBackupFile::Result SaveAtOnce( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total );

  //--------------------------------------------------------------
  //--------------------------------------------------------------
  virtual gfl2::fs::FsCtrBackupFile::Result LoadAtOnce( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total );

  //--------------------------------------------------------------
  /**
   * @brief   ファイルチェック
   * @param   save_total
   * @retval  ファイルが無ければfalse 初期化シーケンスにいく
   * @retval  ファイルがあればtrue 読み込み
   */
  //--------------------------------------------------------------
  bool CheckFileExist( ISaveDataTotal * save_total );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル（再）作成
   */
  //--------------------------------------------------------------
  gfl2::fs::FsCtrBackupSystem::Result CreateFile( gfl2::heap::HeapBase* pHeap, ISaveDataTotal * save_total );
  gfl2::fs::FsCtrBackupSystem::Result DeleteFile( gfl2::heap::HeapBase* pHeap, ISaveDataTotal * save_total );

  //--------------------------------------------------------------
  /**
   * @brief   ファイル（再）作成：開始
   */
  //--------------------------------------------------------------
  bool CreateStart( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total );
  //--------------------------------------------------------------
  /**
   * @brief   ファイル（再）作成：メイン
   */
  //--------------------------------------------------------------
  bool CreateMain( gfl2::fs::FsCtrBackupSystem::Result * result );

  //--------------------------------------------------------------
  /**
   * @brief   ロードを実行：開始
   * @param[in]   pHeap         作業用ヒープ
   * @param[in]   save_total    セーブデータ全体指定クラス
   */
  //--------------------------------------------------------------
  virtual bool LoadStart( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total );

  //--------------------------------------------------------------
  /**
   * @brief   ロードを実行：メイン
   * @return  bool  trueのとき、完了  falseのとき実行中
   *
   * -  gfl2::fs::FsCtrBackupSystem::RESULT_OK           ロード完了
   * -  gfl2::fs::FsCtrBackupSystem::RESULT_NG_xxxx      ロード失敗
   */
  //--------------------------------------------------------------
  virtual bool LoadMain( gfl2::fs::FsCtrBackupFile::Result * result );

  //--------------------------------------------------------------
  /**
   * @brief   セーブを実行：開始
   * @param[in]   pHeap         作業用ヒープ
   * @param[in]   save_total    セーブデータ全体指定クラス
   * @return  bool  trueのとき、完了  falseのとき実行中
   */
  //--------------------------------------------------------------
  virtual bool SaveStart( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total );

  //--------------------------------------------------------------
  /**
   * @brief   セーブを実行：メイン
   * @param[out]  result  結果を受け取るポインタ
   * @return  bool  trueのとき、完了  falseのとき実行中
   *
   * -  gfl2::fs::FsCtrBackupSystem::ACCESS_RESULT_OK       セーブ完了
   * -  gfl2::fs::FsCtrBackupSystem::ACCESS_RESULT_NG_xxxx  セーブ失敗
   */
  //--------------------------------------------------------------
  virtual bool SaveMain( gfl2::fs::FsCtrBackupFile::Result * result );

  //--------------------------------------------------------------
  /**
   * @brief   セーブを途中まで実行：開始
   * @param[in]   pHeap         作業用ヒープ
   * @param[in]   save_total    セーブデータ全体指定クラス
   *
   * @note  LastSave～と組みにして使用します。
   */
  //--------------------------------------------------------------
  virtual bool FirstSaveStart( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total );

  //--------------------------------------------------------------
  /**
   * @brief   セーブを途中まで実行：メイン
   * @param[out]  result  結果を受け取るポインタ
   * @return  bool  trueのとき、完了  falseのとき実行中
   *
   * -  gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK       セーブ完了
   * -  gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_NG_xxxx  セーブ失敗
   */
  //--------------------------------------------------------------
  virtual bool FirstSaveMain( gfl2::fs::FsCtrBackupFile::Result * result );

  //--------------------------------------------------------------
  /**
   * @brief セーブを途中まで実行：キャンセル
   *
   * @note  セーブデータ変更禁止処理の解除を行っている
   * @caution FirstSaveMainを完了してから呼び出してください。
   */
  //--------------------------------------------------------------
  virtual void FirstSaveCancel( void );

  //--------------------------------------------------------------
  /**
   * @brief   セーブを途中から完了処理
   * @param[in]   save_total    セーブデータ全体指定クラス
   * @note    エラーはすべて製品版では発生してはならないもの
   */
  //--------------------------------------------------------------
  virtual void LastSave( ISaveDataTotal * save_total );

  //--------------------------------------------------------------
  /**
   * @brief   セーブを途中から完了処理：開始
   * @param[in]   pHeap         作業用ヒープ
   * @param[in]   save_total    セーブデータ全体指定クラス
   *
   * @note  FirstSave～と組みにして使用します。
   * @note  FirstSaveMain完了後、通信同期をとって呼び出すことで
   * @note  人為的なデータの増殖などを予防するための機能です。
   */
  //--------------------------------------------------------------
  virtual bool LastSaveStart( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total );

  //--------------------------------------------------------------
  /**
   * @brief   セーブを途中から完了処理：メイン
   * @param[out]  result  結果を受け取るポインタ
   * @return  bool  trueのとき、完了  falseのとき実行中
   *
   * -  gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK       セーブ完了
   * -  gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_NG_xxxx  セーブ失敗
   */
  //--------------------------------------------------------------
  virtual bool LastSaveMain( gfl2::fs::FsCtrBackupFile::Result * result );


  //--------------------------------------------------------------
  /**
   * @brief   セーブデータの登録
   * @param[in]   save_total    セーブデータ全体指定クラス
   * @param[in]   name          セーブデータファイル名
   *
   * @note  ゲーム開始直後のみに呼び出します。
   */
  //--------------------------------------------------------------
  virtual void AddBackupEntry( ISaveDataTotal * save_total, const wchar_t * name );

#if PM_DEBUG
  //--------------------------------------------------------------
  /**
   * @brief テスト用にセーブ容量をダミーで最大サイズまで埋める
   */
  //--------------------------------------------------------------
  virtual void SetDebugDummySizeEnable( bool isEnable ){ m_isDebugDummySizeEnable = isEnable; };

  virtual nn::Result GetLastNNResult( void ){ return mpBackupSystem->GetLastNNResult(); }
#endif

  //--------------------------------------------------------------
  /**
   * @brief セーブデータを書き込むためのバッファサイズを取得する
   *
   * @param   save_total         セーブデータアクセスクラス
   * @param   isEnablePokemonBox ポケモンボックスデータを含めるならtrueを指定
   */
  //--------------------------------------------------------------
  virtual u32 GetSaveDataBufferSize( ISaveDataTotal * save_total, bool isEnablePokemonBox );

  //--------------------------------------------------------------
  /**
   * @brief バッファへセーブデータ書き込み
   *
   * @param   pBuffer            書き込み先バッファアドレス
   * @param   save_total         セーブデータアクセスクラス
   * @param   isEnablePokemonBox ポケモンボックスデータを含めるならtrueを指定
   */
  //--------------------------------------------------------------
  virtual void SaveToBuffer( u8* pBuffer, ISaveDataTotal* save_total, gfl2::heap::HeapBase* pHeap, bool isEnablePokemonBox );

#if PM_DEBUG

  //--------------------------------------------------------------
  /**
   * @brief バッファからセーブデータを読み込む（PGLデバッグ用）
   *
   * @param   pBuffer            読み込み先バッファアドレス
   * @param   save_total         セーブデータアクセスクラス
   * @param   isEnablePokemonBox ポケモンボックスデータを含めるならtrueを指定
   */
  //--------------------------------------------------------------
  virtual void LoadFromBuffer( const u8* pBuffer, ISaveDataTotal * save_total, gfl2::heap::HeapBase * pHeap, bool isEnablePokemonBox );



  wchar_t * GetFileName( int index ) const;
  size_t GetFileSize( int index ) const;

  gfl2::fs::FsCtrBackupFile::Result SaveRawDataAtOnce( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total );
#endif

//--------------------------------------------------------------
//
//    以下は外部非公開
//
//--------------------------------------------------------------
  friend class CTRSaveLoadControlThread;

private:
  enum THREAD_MODE{
    THREAD_MODE_NOTHING = 0,
    THREAD_MODE_FORMAT,
    THREAD_MODE_CREATE,
    THREAD_MODE_LOAD,
    THREAD_MODE_SAVE,
    THREAD_MODE_SAVE_FIRST,
    THREAD_MODE_SAVE_LAST,
  };

  gfl2::fs::FsCtrBackupSystem* mpBackupSystem;
  int mFileIndex;
  const int mFileIndexMax;  //セーブデータ最大

  FileInfo * mFileInfo;

  ARC_STATUS mArcStatus;

  THREAD_MODE m_threadMode;
  CTRSaveLoadControlThread * m_pMyThread;
  gfl2::fs::FsCtrBackupFile::Result m_threadFileResult;
  gfl2::fs::FsCtrBackupSystem::Result m_threadSystemResult;

  ISaveDataTotal * m_threadTarget;

#if PM_DEBUG
  bool m_isDebugDummySizeEnable;
  s64 m_dummyDataSize;
#endif

private:
  void StartThread( u8 priority, THREAD_MODE mode, gfl2::heap::HeapBase * pHeap, ISaveDataTotal * pSaveData );
  void ThreadCallBack(gfl2::heap::HeapBase * pHeap, ISaveDataTotal * pSaveData );
  bool IsAsyncFinished( THREAD_MODE mode );
  int GetEntryIndex( const ISaveDataTotal * save_total );

  gfl2::fs::FsCtrBackupFile::Result Save(
      gfl2::fs::FsCtrBackupFile * pFile, ISaveDataTotal * pSaveData, gfl2::heap::HeapBase * pHeap );
  gfl2::fs::FsCtrBackupFile::Result Load(
      gfl2::fs::FsCtrBackupFile * pFile, ISaveDataTotal * pSaveData, gfl2::heap::HeapBase * pHeap );

  void mount( void );
  void unmount( void );
  void commit( bool assert_crc_failure = true );

  void enterCS( ISaveDataTotal * save_total );
  void leaveCS( void );


}; //class CTRSaveLoadControl

GFL_NAMESPACE_END(Savedata)

#endif //__SAVELOADCONTROL_H__
#endif
