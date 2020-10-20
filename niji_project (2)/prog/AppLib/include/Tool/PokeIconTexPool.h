// ============================================================================
/*
 * @file PokeIconTexPool.h
 * @brief ポケモンアイコン全データを丸ごと作成するクラスです。
 * @date 2015.12.16
 * @author yuto_uchida
 * @note
 *      ポケモンアイコンデータを丸ごと全部作成して、テクスチャーとして貼り付けることができます。
 *      メモリに余裕がある場合は使えます。
 *      丸ごとアイコンのgarcファイルサイズは約410kbyte(2015/12/16の段階)です。
 */
// ============================================================================
#if !defined( APP_TOOL_POKE_ICON_TEX_POOL_H_INCLUDE )
#define APP_TOOL_POKE_ICON_TEX_POOL_H_INCLUDE
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <fs/include/gfl2_Fs.h>
#include <layout/include/gfl2_Layout.h>
#include <heap/include/gfl2_Heap.h>

//  nijiのインクルード
#include <AppLib/include/Tool/PokeIcon.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

  //  ポケモンアイコンtexデータ
  //  テクスチャーロードに必要なバッファ確保とtexinfoの管理をしている
  class PokeIconTexData
  {
    friend class PokeIconTexPool;

    GFL_FORBID_COPY_AND_ASSIGN( PokeIconTexData );

  public:

    PokeIconTexData( gfl2::heap::HeapBase* pHeap );
    virtual ~PokeIconTexData();

    //  作成したテクスチャーデータからペインに貼り付け
    bool ReplectPaneTexture(
        gfl2::lyt::LytPicture*        pPane,
        u32                           materialIndex,
        u32                           textureIndex );

  private:
    void _DeleteTexture( const u32 dataIdx );

    void  _Clear()
    {
      m_nowDataIdx  = 0;
    }

    struct TexData
    {
      gfl2::lyt::LytResTextureInfo texInfo;  // テクスチャ情報
      void* pTexBuffer;
      u32   texSize;
      bool  bTexInfoAlive;

      TexData()
      {
        pTexBuffer    = NULL;
        texSize       = 0;
        bTexInfoAlive = false;
      }
    };

    TexData m_aTexData[ 2 ];
    u32   m_nowDataIdx;
  };

  //  ポケモンアイコンデータ一括管理クラス
  class PokeIconTexPool
  {
    GFL_FORBID_COPY_AND_ASSIGN( PokeIconTexPool );
  public:
    PokeIconTexPool( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pRomAsyncFileManager );
    virtual~PokeIconTexPool();

    //  テクスチャーバッファを作成
    //  あらかじめテクスチャーバッファを確保するのに使える
    static void* CreateTextureBuffer( gfl2::heap::HeapBase* pHeap );

    //  テクスチャーデータを破棄
    static void DeleteTextureBuffer( void* pTexData );

    //  初回処理
    //  true が返ってくるまで呼び出し続ける
    bool  InitFunc();

    //  終了処理
    //  true が返ってくるまで呼び出し続ける
    bool  EndFunc();

    //  テクスチャーデータを作成
    //  内部でメモリ確保しているので注意が必要
    //  作成したのは必ず破棄関数「DeleteTextureBuffer」で破棄する
    //  ※ PokeIconTexDataクラスを使わない場合は使用
    void* CreateTexture(
        gfl2::heap::HeapBase*         pDevMem,
        gfl2::heap::HeapBase*         pTempMem,
        const PokeTool::SimpleParam&  rSp,
        bool                          bFlip );

    //  作成したテクスチャーデータからペインに貼り付け
    //  ※ PokeIconTexDataクラスを使わない場合は使用
    bool ReplectPaneTexture(
        void*                         pTexBuffer,
        size_t                        texSize,
        gfl2::lyt::LytPicture*        pPane,
        u32                           materialIndex,
        u32                           textureIndex,
        gfl2::lyt::LytResTextureInfo* pOutTextureInfo );

    //  データクラスにテクスチャーデータを作成(SimpleParam版)
    bool CreatePokeData(
      PokeIconTexData*              pPokeIconTexData,
      gfl2::heap::HeapBase*         pTempMem,
      const PokeTool::SimpleParam&  rSp,
      bool                          bFlip );

    //  データクラスにテクスチャーデータを作成(CoreParam版)
    bool CreatePokeData(
      PokeIconTexData*              pPokeIconTexData,
      gfl2::heap::HeapBase*         pTempMem,
      const pml::pokepara::CoreParam& rPp,
      bool                          bFlip );

  private:

    void _Clear()
    {
      m_initSeq = 0;
      m_endSeq  = 0;
      m_arcId   = gfl2::fs::ArcFile::ARCID_NULL;

      m_bInit = false;
      m_bEnd  = false;

      m_pHeap       = NULL;
      m_pTexBuffer  = NULL;

      m_pRomAsyncFileManager  = NULL;
    }

    //  データクラスにテクスチャーデータを作成
    bool _CreatePokeData( PokeIconTexData* pOutPokeIconTexData, const gfl2::fs::ArcFile::ARCDATID datId, gfl2::heap::HeapBase* pTempMem );

    u8  m_initSeq;
    bool  m_bInit;

    u8  m_endSeq;
    bool  m_bEnd;

    gfl2::heap::HeapBase*       m_pHeap;
    gfl2::fs::AsyncFileManager* m_pRomAsyncFileManager;

    void*                 m_pTexBuffer;
    u32                   m_arcId;


  };

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

#endif // APP_TOOL_POKE_ICON_TEX_POOL_H_INCLUDE
