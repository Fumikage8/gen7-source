//======================================================================
/**
 * @file IconTex.h
 * @date 2015/10/14 12:30:11
 * @author uchida_yuto
 * @brief 2Dアイコンオブジェクト(テクスチャーを非同期でロードして張り付ける)
 * @note  AppLibに昇格予定
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __APP_TOOL_ICON_OBJECT_HEADER_H_INCLUDED__
#define __APP_TOOL_ICON_OBJECT_HEADER_H_INCLUDED__
#pragma once

//  gflib2のインクルード
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

//  nijiのインクルード
#include "AppLib/include/Tool/app_tool_LytTexData.h"
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
  /**
   * @class IconObject
   * @brief アイコンオブジェクト
   */
  class IconObject
  {
      GFL_FORBID_COPY_AND_ASSIGN(IconObject);
  public:
    enum ReplaceTexResult
    {
      REPLACE_TEX_RESULT_LOAD_IN  = 0,
      REPLACE_TEX_RESULT_LOAD_DUBLICATION,
    };

    struct INIT_PARAM
    {
      INIT_PARAM()
      {
        arcId = gfl2::fs::ArcFile::ARCID_NULL;
        sizeWidth = sizeHeight  = 0;
        colorFormat = gfl2::gfx::Format::A8R8G8B8;
        bCompFile   = false;
      }

      //  すでにファイルが開いているarcIDを指定
      gfl2::fs::ArcFile::ARCID  arcId;
      u32 sizeWidth, sizeHeight;
      bool bCompFile;
      gfl2::gfx::Format colorFormat;
    };

    //  必要なヒープサイズを取得
    //  正確に取得するにはテクスチャーのフォーマットも必要な気が
    static u32 GetUseHeapSize( const u32 width, const u32 height, const bool bCompFile, const gfl2::gfx::Format colorFormat = gfl2::gfx::Format::A8R8G8B8 );

    //  システムヒープサイズ
    static u32 GetUseSystemHeapSize();

    //  デバイスヒープサイズ
    static u32 GetUseDeviceHeapSize( const u32 width, const u32 height, const bool bCompFile, const gfl2::gfx::Format colorFormat = gfl2::gfx::Format::A8R8G8B8 );

    //  テクスチャーのヒープサイズ
    static u32 GetUseTexHeapSize( const u32 width, const u32 height, const gfl2::gfx::Format colorFormat = gfl2::gfx::Format::A8R8G8B8 );

    IconObject( const INIT_PARAM& rInitParam, gfl2::heap::HeapBase* pHeap );
    virtual ~IconObject();

    //  初期化
    //  true なら終了
    //  false なら実行中
    bool InitFunc();

    //  終了処理
    //  true なら終了
    //  false なら実行中
    bool EndFunc();

    //  更新処理
    void UpdateFunc();

    //  テクスチャーをロードして貼り付け
    ReplaceTexResult ReplaceTex( gfl2::lyt::LytPicture* pPane, const gfl2::fs::ArcFile::ARCDATID dataId );

    //  テクスチャーの貼り付けが終了している
    bool IsReplaceTex();

    //  テクスチャーデータ取得
    void* GetTexData();

    //  テクスチャーデータサイズを取得
    u32 GetTexSize();

    //  テクスチャーデータ制御クラスを取得
    app::tool::LytTexData*  GetTexDataCtrl();

    //  貼り付けをするか
    void SetEnableReplaceTex( const bool bFlg ) { m_bReplaceTex = bFlg; }

  private:
    enum
    {
      UPDATA_SEQ_IDLE       = 0,
      UPDATE_SEQ_LOAD_START,
      UPDATE_SEQ_LOAD_RUN,
      UPDATE_SEQ_LOAD_END
    };

    enum
    {
      DATA_ID_NONE  = 0xffffffff,
    };

    //  初期化
    void _Clear()
    {
      m_pPicturePane  = NULL;
      m_pLytTexData   = NULL;
      m_pLocalHeap    = NULL;
      m_pLocalTexHeap = NULL;
      m_arcDataId     = gfl2::fs::ArcFile::ARCDATID_NULL;

      m_updateSeq = UPDATA_SEQ_IDLE;
      m_initSeq   = 0;
      m_endSeq    = 0;
      m_loadedArcDataId = (u32)DATA_ID_NONE;
      m_bReplaceTex = true;
    }

    INIT_PARAM  m_initParam;

    gfl2::lyt::LytPicture*        m_pPicturePane;
    gfl2::fs::ArcFile::ARCDATID   m_arcDataId;
    app::tool::LytTexData*        m_pLytTexData;

    gfl2::heap::HeapBase*         m_pLocalHeap;
    gfl2::heap::HeapBase*         m_pLocalTexHeap;

    u32   m_updateSeq;

    u32   m_initSeq;
    u32   m_endSeq;

    u32   m_loadedArcDataId;
    bool  m_bReplaceTex;
  };

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

#endif // __APP_TOOL_ICON_OBJECT_HEADER_H_INCLUDED__