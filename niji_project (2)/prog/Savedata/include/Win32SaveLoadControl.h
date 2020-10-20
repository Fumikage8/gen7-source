
#if   defined(GF_PLATFORM_WIN32)
#ifndef __SAVELOADCONTROLWIN32_H__
#define __SAVELOADCONTROLWIN32_H__
//===================================================================
/**
 * @file    Win32SaveLoadControl.h
 * @brief   セーブロードのコントロールを行うクラス
 * @author  
 * @date    
 */
//===================================================================
#pragma once
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/SaveLoadControl.h>
#include <Savedata/include/ISaveDataTotal.h>
#include <Savedata/include/SaveDataInterface.h>
#include <Savedata/include/SaveData.h>

GFL_NAMESPACE_BEGIN(Savedata)

#define WIN32_SAVE_FILENAME L"R:/home/momiji/exec/savedata_win32/niji_win32_save.dat"
#define WIN32_SAVE_FILEPATH L"R:/home/momiji/exec/savedata_win32"

//--------------------------------------------------------------
/**
 * @class Win32SaveLoadControl
 */
//--------------------------------------------------------------
class Win32SaveLoadControl : public SaveLoadControl
{
public:
  enum
  {
    SAVE_BUFFER_SIZE = 512 * 1024,
  };

public:

  Win32SaveLoadControl()
    : m_pHeap(NULL)
    , m_pSaveTotal(NULL)
  {};
  virtual ~Win32SaveLoadControl(){};

public:
  //--------------------------------------------------------------
  /**
   * @brief   セーブデータの登録
   * @param[in]   save_total    セーブデータ全体指定クラス
   * @param[in]   name          セーブデータファイル名
   *
   * @note  ゲーム開始直後のみに呼び出します。
   */
  //--------------------------------------------------------------
  virtual void AddBackupEntry( ISaveDataTotal * save_total, const wchar_t * name )
  {
  }

  //--------------------------------------------------------------
  /**
   * @brief   セーブデータアーカイブの非同期初期化処理開始
   */
  //--------------------------------------------------------------
  virtual void FormatStart( gfl2::heap::HeapBase * pTempHeap )
  {
    
  }

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
  /**
   * @brief   ロードを実行：開始
   * @param[in]   pHeap         作業用ヒープ
   * @param[in]   save_total    セーブデータ全体指定クラス
   */
  //--------------------------------------------------------------
  virtual bool LoadStart( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total )
  {
    m_pSaveTotal = save_total;
    m_pHeap = pHeap;
    return true;
  }

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

  virtual gfl2::fs::FsCtrBackupFile::Result LoadAtOnce( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total );

  //--------------------------------------------------------------
  /**
   * @brief   セーブを実行：開始
   * @param[in]   pHeap         作業用ヒープ
   * @param[in]   save_total    セーブデータ全体指定クラス
   * @return  bool  trueのとき、完了  falseのとき実行中
   */
  //--------------------------------------------------------------
  virtual bool SaveStart( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total )
  {
    m_pSaveTotal = save_total;
    m_pHeap = pHeap;
    return true;
  }

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
  virtual bool FirstSaveStart( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total )
  {
    return true;
  }

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
  virtual bool FirstSaveMain( gfl2::fs::FsCtrBackupFile::Result * result )
  {
    return true;
  }

  //--------------------------------------------------------------
  /**
   * @brief セーブを途中まで実行：キャンセル
   *
   * @note  セーブデータ変更禁止処理の解除を行っている
   * @caution FirstSaveMainを完了してから呼び出してください。
   */
  //--------------------------------------------------------------
  virtual void FirstSaveCancel( void )
  {
  }

  //--------------------------------------------------------------
  /**
   * @brief   セーブを途中から完了処理
   * @param[in]   save_total    セーブデータ全体指定クラス
   * @note    エラーはすべて製品版では発生してはならないもの
   */
  //--------------------------------------------------------------
  virtual void LastSave( ISaveDataTotal * save_total )
  {
  }

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
  virtual bool LastSaveStart( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total )
  {
    return true;
  }

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
  virtual bool LastSaveMain( gfl2::fs::FsCtrBackupFile::Result * result )
  {
    return true;
  }


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
   * @brief セーブデータを書き込むためのバッファサイズを取得する
   *
   * @param   save_total         セーブデータアクセスクラス
   * @param   isEnablePokemonBox ポケモンボックスデータを含めるならtrueを指定
   */
  //--------------------------------------------------------------
  virtual u32 GetSaveDataBufferSize( ISaveDataTotal * save_total, bool isEnablePokemonBox ){ return 0; };

  //--------------------------------------------------------------
  /**
   * @brief バッファへセーブデータ書き込み
   *
   * @param   pBuffer            書き込み先バッファアドレス
   * @param   save_total         セーブデータアクセスクラス
   * @param   isEnablePokemonBox ポケモンボックスデータを含めるならtrueを指定
   */
  //--------------------------------------------------------------
  virtual void SaveToBuffer( u8* pBuffer, ISaveDataTotal* save_total, gfl2::heap::HeapBase* pHeap, bool isEnablePokemonBox ){};

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
  virtual void LoadFromBuffer( const u8* pBuffer, ISaveDataTotal * save_total, gfl2::heap::HeapBase * pHeap, bool isEnablePokemonBox ){};


  //--------------------------------------------------------------
  /**
   * @brief テスト用にセーブ容量をダミーで最大サイズまで埋める
   */
  //--------------------------------------------------------------
  virtual void SetDebugDummySizeEnable( bool isEnable ){};
#endif

private:

  gfl2::heap::HeapBase * m_pHeap;
  ISaveDataTotal * m_pSaveTotal;
}; 

GFL_NAMESPACE_END(Savedata)

#endif //__SAVELOADCONTROLWIN32_H__
#endif
