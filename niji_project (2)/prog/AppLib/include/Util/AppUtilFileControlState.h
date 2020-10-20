#ifndef __APP_UTIL_FILE_CONTROL_STATE_H__
#define __APP_UTIL_FILE_CONTROL_STATE_H__
//=============================================================================================
/**
 * @file   AppUtilFileControlState.h
 * @brief  ファイル制御ステート
 * @author yuto.uchida
 * @date   2015.5.15
 * @note
 *         arcファイルを開いて、dataをロードして、arcファイルを閉じる制御する
 *         インスタンスを破棄できるかどうかは必ず下記の関数を使ってチェックして true が返ってきたら破棄する
 *          bool  IsDelete();
 */
//=============================================================================================
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

#include <debug/include/gfl2_assert.h>

#include <heap/include/gfl2_heap.h>

#include <fs/include/gfl2_FsArcFile.h>

GFL_NAMESPACE_BEGIN(app) 
GFL_NAMESPACE_BEGIN(util) 

  //  ファイル制御ステート
  class AppUtilFileControlState
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN( AppUtilFileControlState );

  public:
    enum FileStateType
    {
      //  何もしていない
      FILE_STATE_TYPE_NONE  = 0,

      FILE_STATE_TYPE_OPEN,

      //  マニュアルファイル処理の処理アイドリング
      //  ファイル制御をして終了したら下記の状態に遷移
      FILE_STATE_TYPE_MANUAL_PROC_IDLE,

      FILE_STATE_TYPE_LOAD,
      FILE_STATE_TYPE_CLOSE,
      FILE_STATE_TYPE_END
    };

    //  Start関数でファイルを開く、End関数でファイルを閉じる処理をする
    //  ファイルの開閉を使う側で手動制御するため用(マニュアル制御)
    AppUtilFileControlState( gfl2::fs::ArcFile::ARCID arcId, gfl2::heap::HeapBase* pUseHaep, bool bFileComp, u32 align, gfl2::fs::ToolDefine::ArcLangType::Tag lang = gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG );

    //  ファイルの開く、ロード、閉じるの処理をする自動処理用(オート制御)
    AppUtilFileControlState( gfl2::fs::ArcFile::ARCID arcId, gfl2::fs::ArcFile::ARCDATID dataId, gfl2::heap::HeapBase* pUseHaep, bool bFileComp, u32 align, gfl2::fs::ToolDefine::ArcLangType::Tag lang = gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG );

    virtual ~AppUtilFileControlState();

    //------------------------------------------------------------------
    /**
     * @brief   ファイル制御開始！！
     * @note
     *          すでに開始しているのであれば処理しない
     *          再び開始するにはEnd()関数を呼んできちんと終わらせないといけない
     *          きちんと終わっているいるかは IsEnd()関数で取得
     */
    //------------------------------------------------------------------
    virtual bool Start();

    //------------------------------------------------------------------
    /**
     * @brief   ファイルバイナリロード開始
     * @note
     *        下記のコンストラクタを呼び出した場合に有効
     *        AppUtilFileControlState( gfl2::fs::ArcFile::ARCID arcId, gfl2::heap::HeapBase* pUseHaep, bool bFileComp, u32 align, gfl2::fs::ToolDefine::ArcLangType::Tag lang );
     */
    //------------------------------------------------------------------
    void Load( gfl2::fs::ArcFile::ARCDATID dataId, gfl2::fs::ToolDefine::ArcLangType::Tag lang = gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG );

    //------------------------------------------------------------------
    /**
     * @brief   処理をきちんと終わらせる
     * @note
     *          ロードしたデータが削除されるので注意
     */
    //------------------------------------------------------------------
    virtual void End();

    //------------------------------------------------------------------
    /**
     * @brief   処理がきちんと終わっているか
     * @return  true -> きちんと終わってインスタンスを破棄できる / false -> まだちゃんと終わっていないのでインスタンスの破棄もできない
     */
    //------------------------------------------------------------------
    bool IsEnd();

    //  ファイル制御を開始しているかどうか
    bool IsStart() const { return m_bStart; }

    //  マニュアル制御がアイドリング中
    bool IsManualIdleState() const { return ( m_state == FILE_STATE_TYPE_MANUAL_PROC_IDLE ); }

    //------------------------------------------------------------------
    /**
     * @brief   ファイル制御が終了しているかどうか
     * @return  true - > Start()関数を呼んでファイル制御が一通り終わっている / false -> ファイル制御がまだ終わっていない or そもそもファイル制御をしていない
     * @note
     *          ※ファイル制御をしていない場合でも false が返ってくる
     *            ファイル制御しているかどうかは IsStart() 関数からみてもらいたい。
     */
    //------------------------------------------------------------------
    bool IsCtrlEnd();

    //  制御更新(更新しないとクラス内の挙動や値が変わらないので常に呼び続けるのを推奨)
    void Update();

    //  状態取得
    //  この状態を見て、ファイルステートクラスの制御を使う側が決める
    FileStateType GetState() const { return m_state; }

    //  ロードバッファアドレスを取得
    void* GetLoadDataBuffer() { return m_pLoadBuffer; }

  private:

    //  変数初期化
    void _Clear()
    {
      m_arcId                 = gfl2::fs::ArcFile::ARCID_NULL;
      m_dataId                = gfl2::fs::ArcFile::ARCDATID_NULL;

      m_pUseHeap              = NULL;
      m_pLoadBuffer           = NULL;

      m_fileAlign             = 0;
      m_fileLang              = gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG;
      m_bFileComp             = false;
      m_bOpenCloseHeapLowwer  = false;
      m_bSeqAuto              = true;
      m_bEnd                  = false;
      m_bStart                = false;
      m_bOpen                 = false;

      m_state         = FILE_STATE_TYPE_NONE;
    }

    virtual bool _StartFileOpen();
    virtual bool _StartFileLoad();
    virtual bool _StartFileClose();

    //  ステート変更
    void _SetState( const FileStateType state );
      
    gfl2::fs::ArcFile::ARCID    m_arcId;
    gfl2::fs::ArcFile::ARCDATID m_dataId;

    gfl2::heap::HeapBase* m_pUseHeap;
    void*                 m_pLoadBuffer;

    u32                   m_fileAlign;
    gfl2::fs::ToolDefine::ArcLangType::Tag m_fileLang;

    bool                  m_bFileComp;
    bool                  m_bOpenCloseHeapLowwer;
    bool                  m_bSeqAuto;
    bool                  m_bStart;
    bool                  m_bEnd;
    bool                  m_bOpen;

    FileStateType m_state;
  };

GFL_NAMESPACE_END(util) 
GFL_NAMESPACE_END(app) 

#endif //__APP_UTIL_FILE_CONTROL_STATE_H__
