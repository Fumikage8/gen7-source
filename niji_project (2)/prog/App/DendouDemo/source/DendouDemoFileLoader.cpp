//======================================================================
/**
 * @file DendouDemoFileLoader.cpp
 * @date 2016/03/11 15:44:54
 * @author nagihashi_toru
 * @brief ファイル読み込み
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

//system
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <debug/include/gfl2_Assert.h>
#include <System/include/PokemonVersion.h>

//mine
#include "DendouDemoFileLoader.h"

//resource
#if defined(GF_PLATFORM_CTR)
#include <niji_conv_header/app/dendou_demo/dendou_binlink_ctr.h>
#else
#include <niji_conv_header/app/dendou_demo/dendou_binlink_win.h>
#endif //CTR
#include <arc_index/DendouDemo.gaix>
#include <arc_def_index/arc_def.h>

namespace app {
  namespace dendou_demo {

    namespace {
    enum
    {
      SEQ_IDLE,
      SEQ_ARC_OPEN,
      SEQ_LOAD,
      SEQ_ARC_CLOSE,
      SEQ_END,
    };

    //binlink_ctr/win.h でインデックスがずれるためココで統一
#if defined(GF_PLATFORM_CTR)
    static const u32 BL_IDX_BG_GFBMDLP = BL_IDX_DENDOU_BINLINK_CTR_SD9060_DENDOUIRI_LOOP_BG_CTR_GFBMDLP;
    static const u32 BL_IDX_BG_GFBMOT = BL_IDX_DENDOU_BINLINK_CTR_SD9060_DENDOUIRI_LOOP_BG_GFBMOT;
    static const u32 BL_IDX_CAM_GFBMOT = BL_IDX_DENDOU_BINLINK_CTR_SD9060_DENDOUIRI_LOOP_CAM_GFBMOT;
    static const u32 BL_IDX_CAM_GFBENV = BL_IDX_DENDOU_BINLINK_CTR_SD9060_DENDOUIRI_LOOP_CAM_GFBENV;
    static const u32 BL_IDX_APPLYT_JPN = BL_IDX_DENDOU_BINLINK_CTR_DENDOUDEMO_APPLYT;
    static const u32 BL_IDX_APPLYT_CHN = BL_IDX_DENDOU_BINLINK_CTR_DENDOUDEMO_CHN_APPLYT;
    static const u32 BL_IDX_APPLYT_DEU = BL_IDX_DENDOU_BINLINK_CTR_DENDOUDEMO_DEU_APPLYT;
    static const u32 BL_IDX_APPLYT_ESP_F = BL_IDX_DENDOU_BINLINK_CTR_DENDOUDEMO_ESP_F_APPLYT;
    static const u32 BL_IDX_APPLYT_ESP = BL_IDX_DENDOU_BINLINK_CTR_DENDOUDEMO_ESP_APPLYT;
    static const u32 BL_IDX_APPLYT_FRA = BL_IDX_DENDOU_BINLINK_CTR_DENDOUDEMO_FRA_APPLYT;
    static const u32 BL_IDX_APPLYT_ITA = BL_IDX_DENDOU_BINLINK_CTR_DENDOUDEMO_ITA_APPLYT;
    static const u32 BL_IDX_APPLYT_ITA_F = BL_IDX_DENDOU_BINLINK_CTR_DENDOUDEMO_ITA_F_APPLYT;
    static const u32 BL_IDX_APPLYT_KOR = BL_IDX_DENDOU_BINLINK_CTR_DENDOUDEMO_KOR_APPLYT;
    static const u32 BL_IDX_APPLYT_TWN = BL_IDX_DENDOU_BINLINK_CTR_DENDOUDEMO_TWN_APPLYT;
    static const u32 BL_IDX_APPLYT_USA = BL_IDX_DENDOU_BINLINK_CTR_DENDOUDEMO_USA_APPLYT;
    static const u32 BL_IDX_STAR_BPTCL = BL_IDX_DENDOU_BINLINK_CTR_ED_SD9060_END_STAR_BPTCL;
    static const u32 BL_IDX_SPOT_01_GFBMDLP = BL_IDX_DENDOU_BINLINK_CTR_SD9060_DENDOUIRI_LOOP_SPOT01_CTR_GFBMDLP;
    static const u32 BL_IDX_SPOT_02_GFBMDLP = BL_IDX_DENDOU_BINLINK_CTR_SD9060_DENDOUIRI_LOOP_SPOT02_CTR_GFBMDLP;
    static const u32 BL_IDX_SPOT_03_GFBMDLP = BL_IDX_DENDOU_BINLINK_CTR_SD9060_DENDOUIRI_LOOP_SPOT03_CTR_GFBMDLP;
    static const u32 BL_IDX_SPOT_04_GFBMDLP = BL_IDX_DENDOU_BINLINK_CTR_SD9060_DENDOUIRI_LOOP_SPOT04_CTR_GFBMDLP;
    static const u32 BL_IDX_SPOT_05_GFBMDLP = BL_IDX_DENDOU_BINLINK_CTR_SD9060_DENDOUIRI_LOOP_SPOT05_CTR_GFBMDLP;
    static const u32 BL_IDX_SPOT_06_GFBMDLP = BL_IDX_DENDOU_BINLINK_CTR_SD9060_DENDOUIRI_LOOP_SPOT06_CTR_GFBMDLP;
#else
    static const u32 BL_IDX_BG_GFBMDLP = BL_IDX_DENDOU_BINLINK_WIN_SD9060_DENDOUIRI_LOOP_BG_WINGL_GFBMDLP;
    static const u32 BL_IDX_BG_GFBMOT = BL_IDX_DENDOU_BINLINK_WIN_SD9060_DENDOUIRI_LOOP_BG_GFBMOT;
    static const u32 BL_IDX_CAM_GFBMOT = BL_IDX_DENDOU_BINLINK_WIN_SD9060_DENDOUIRI_LOOP_CAM_GFBMOT;
    static const u32 BL_IDX_CAM_GFBENV = BL_IDX_DENDOU_BINLINK_WIN_SD9060_DENDOUIRI_LOOP_CAM_GFBENV;
    static const u32 BL_IDX_APPLYT_JPN = BL_IDX_DENDOU_BINLINK_WIN_DENDOUDEMO_APPLYT;
    static const u32 BL_IDX_APPLYT_CHN = BL_IDX_DENDOU_BINLINK_WIN_DENDOUDEMO_CHN_APPLYT;
    static const u32 BL_IDX_APPLYT_DEU = BL_IDX_DENDOU_BINLINK_WIN_DENDOUDEMO_DEU_APPLYT;
    static const u32 BL_IDX_APPLYT_ESP_F = BL_IDX_DENDOU_BINLINK_WIN_DENDOUDEMO_ESP_F_APPLYT;
    static const u32 BL_IDX_APPLYT_ESP = BL_IDX_DENDOU_BINLINK_WIN_DENDOUDEMO_ESP_APPLYT;
    static const u32 BL_IDX_APPLYT_FRA = BL_IDX_DENDOU_BINLINK_WIN_DENDOUDEMO_FRA_APPLYT;
    static const u32 BL_IDX_APPLYT_ITA = BL_IDX_DENDOU_BINLINK_WIN_DENDOUDEMO_ITA_APPLYT;
    static const u32 BL_IDX_APPLYT_ITA_F = BL_IDX_DENDOU_BINLINK_WIN_DENDOUDEMO_ITA_F_APPLYT;
    static const u32 BL_IDX_APPLYT_KOR = BL_IDX_DENDOU_BINLINK_WIN_DENDOUDEMO_KOR_APPLYT;
    static const u32 BL_IDX_APPLYT_TWN = BL_IDX_DENDOU_BINLINK_WIN_DENDOUDEMO_TWN_APPLYT;
    static const u32 BL_IDX_APPLYT_USA = BL_IDX_DENDOU_BINLINK_WIN_DENDOUDEMO_USA_APPLYT;
    static const u32 BL_IDX_STAR_BPTCL = BL_IDX_DENDOU_BINLINK_WIN_ED_SD9060_END_STAR_BPTCL;
    static const u32 BL_IDX_SPOT_01_GFBMDLP = BL_IDX_DENDOU_BINLINK_WIN_SD9060_DENDOUIRI_LOOP_SPOT01_WINGL_GFBMDLP;
    static const u32 BL_IDX_SPOT_02_GFBMDLP = BL_IDX_DENDOU_BINLINK_WIN_SD9060_DENDOUIRI_LOOP_SPOT02_WINGL_GFBMDLP;
    static const u32 BL_IDX_SPOT_03_GFBMDLP = BL_IDX_DENDOU_BINLINK_WIN_SD9060_DENDOUIRI_LOOP_SPOT03_WINGL_GFBMDLP;
    static const u32 BL_IDX_SPOT_04_GFBMDLP = BL_IDX_DENDOU_BINLINK_WIN_SD9060_DENDOUIRI_LOOP_SPOT04_WINGL_GFBMDLP;
    static const u32 BL_IDX_SPOT_05_GFBMDLP = BL_IDX_DENDOU_BINLINK_WIN_SD9060_DENDOUIRI_LOOP_SPOT05_WINGL_GFBMDLP;
    static const u32 BL_IDX_SPOT_06_GFBMDLP = BL_IDX_DENDOU_BINLINK_WIN_SD9060_DENDOUIRI_LOOP_SPOT06_WINGL_GFBMDLP;
#endif
    }

    /**
     * @fn
     * コンストラクタ
     * @brief コンストラクタ
     * @param[in] heap クラス確保用ヒープ
     * @param[in] fileManager ファイル読み込みマネージャ
     */
    FileLoader::FileLoader( gfl2::heap::HeapBase * heap, gfl2::fs::AsyncFileManager * fileManager ) :
      mFileManager(fileManager),
      mHeapForBuff(NULL),
      mHeapForReq(NULL),
      mHeapForUncompresse(NULL),
      mFileBuffer(NULL),
      mSeq(SEQ_IDLE),
      mPlayerSex(0)
    {
      mBinLinker = GFL_NEW(heap) gfl2::fs::BinLinkerAccessor();
    }

    /**
     * @fn
     * デストラクタ
     * @brief  デストラクタ
     */
    FileLoader::~FileLoader(void)
    {
      GflHeapSafeFreeMemory( mFileBuffer );
      GFL_SAFE_DELETE( mBinLinker );
    }
    /**
     * @fn
     * 非同期読み込み開始
     * @brief  非同期読み込み開始
     * @param[in] playerSex プレイヤーの性別
     * @param[in] heapForBuff ファイルバッファ確保用のヒープ
     * @param[in] heapForReq  読み込みリクエストで一時的に使うヒープ
     * @param[in] heapForUncompresse 解凍用に一時的に使うヒープ
     */
    void FileLoader::StartLoadAsync( u8 playerSex, gfl2::heap::HeapBase * heapForBuff, gfl2::heap::HeapBase * heapForReq, gfl2::heap::HeapBase * heapForUncompresse )
    {
      GFL_ASSERT( mSeq == SEQ_IDLE );

      GflHeapSafeFreeMemory( mFileBuffer );
      mFileBuffer = NULL;
      mBinLinker->Initialize(NULL);

      mPlayerSex = playerSex;
      mHeapForBuff = heapForBuff;
      mHeapForReq = heapForReq;
      mHeapForUncompresse = heapForUncompresse;
      mSeq = SEQ_ARC_OPEN;
    }

    /**
     * @fn
     * 非同期読み込み処理
     * @brief  非同期読み込み処理
     */
    void FileLoader::Update(void)
    {
      static const gfl2::fs::ArcFile::ARCID DENDOU_ARCID = ARCID_DENDOU_DEMO;

      switch( mSeq )
      {
      case SEQ_IDLE:
        // wait
        return;

      case SEQ_ARC_OPEN:
        {
          gfl2::fs::AsyncFileManager::ArcFileOpenReq oreq;
          oreq.arcId       = DENDOU_ARCID;
          oreq.heapForFile = mHeapForReq;
          oreq.heapForReq  = mHeapForReq;
          mFileManager->AddArcFileOpenReq(oreq);
        }
        mSeq++;
        //fall
      case SEQ_LOAD:
        if( mFileManager->IsArcFileOpenFinished( DENDOU_ARCID ))
        {
          {
            gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
            req.arcId = DENDOU_ARCID;
            req.datId = GARC_DendouDemo_dendou_binlink_win_pack_COMP; //winとありますがctr版も同じ0番です
            req.ppBuf = &mFileBuffer;
            req.heapForBuf = mHeapForBuff;
            req.heapForReq  = mHeapForReq;
            req.heapForCompressed = mHeapForUncompresse;
            req.align = 128;
            mFileManager->AddArcFileLoadDataReq(req);
          }
          mSeq++;
        }
        else{
          return;  //読込中
        }
      case SEQ_ARC_CLOSE:
        if( mFileManager->IsArcFileLoadDataFinished(&mFileBuffer) )
        {
          mBinLinker->Initialize(mFileBuffer);
          gfl2::fs::AsyncFileManager::ArcFileCloseReq creq;
          creq.arcId       = DENDOU_ARCID;
          creq.heapForReq  = mHeapForReq;
          mFileManager->AddArcFileCloseReq(creq);
          mSeq++;
        }
        else{
          return; //読込中
        }
      case SEQ_END:
        if(mFileManager->IsArcFileCloseFinished(DENDOU_ARCID) )
        {
          mSeq = SEQ_IDLE;
        }
        return;
      default:
        GFL_ASSERT(0);
        return;
      }
    }
    /**
     * @fn
     * 非同期待ち
     * @brief  非同期読待ち
     * @return true で非同期読み込み完了、falseでそれ以外
     */
    bool FileLoader::IsComplete(void) const
    {
      return mSeq == SEQ_IDLE && mFileBuffer != NULL;
    }
    /**
     * @fn
     * ファイルバッファの取得
     * @brief  ファイルバッファの取得
     * @return ファイルバッファ
     */
    void * FileLoader::GetBGModelFileBuffer(void) const
    {
      return mBinLinker->GetData( BL_IDX_BG_GFBMDLP );
    }
    void * FileLoader::GetBGMotionFileBuffer(void) const
    {
      return mBinLinker->GetData( BL_IDX_BG_GFBMOT );
    }
    void * FileLoader::GetCameraMotionFileBuffer(void) const
    {
      return mBinLinker->GetData( BL_IDX_CAM_GFBMOT );
    }
    void * FileLoader::GetDrawEnvFileBuffer(void) const
    {
      return mBinLinker->GetData( BL_IDX_CAM_GFBENV );
    }
    void * FileLoader::GetLayoutFileBuffer(void) const
    {
      //言語毎のレイアウトを読みだす処理
      static const struct {
        u16 language;
        u16 binlinkerIndexForMale;
        u16 binlinkerIndexForFemale;
      } LanguageToBinLinkerIndex[] = {

        { POKEMON_LANG_JAPAN,   BL_IDX_APPLYT_JPN, BL_IDX_APPLYT_JPN },
        { POKEMON_LANG_ENGLISH, BL_IDX_APPLYT_USA, BL_IDX_APPLYT_USA },
        { POKEMON_LANG_FRANCE,  BL_IDX_APPLYT_FRA, BL_IDX_APPLYT_FRA },
        { POKEMON_LANG_ITALY,   BL_IDX_APPLYT_ITA, BL_IDX_APPLYT_ITA_F }, //@note イタリアはチャンピオンの文字が男性名詞、女性名詞が分かれる
        { POKEMON_LANG_GERMANY, BL_IDX_APPLYT_DEU, BL_IDX_APPLYT_DEU },
        { POKEMON_LANG_SPAIN,   BL_IDX_APPLYT_ESP, BL_IDX_APPLYT_ESP_F }, //@note スペインはチャンピオンの文字が男性名詞、女性名詞が分かれる
        { POKEMON_LANG_KOREA,   BL_IDX_APPLYT_KOR, BL_IDX_APPLYT_KOR },
        { POKEMON_LANG_CHINA,   BL_IDX_APPLYT_CHN, BL_IDX_APPLYT_CHN },
        { POKEMON_LANG_TAIWAN,  BL_IDX_APPLYT_TWN, BL_IDX_APPLYT_TWN },
      };

      for (int i = 0; i < GFL_NELEMS(LanguageToBinLinkerIndex); i++)
      {
        if( LanguageToBinLinkerIndex[i].language == System::GetLang() )
        {
          if( mPlayerSex == PM_MALE )
          {
            return mBinLinker->GetData( LanguageToBinLinkerIndex[i].binlinkerIndexForMale );
          }
          else
          {
            return mBinLinker->GetData( LanguageToBinLinkerIndex[i].binlinkerIndexForFemale );
          }
        }
      }

      GFL_ASSERT( 0 );
      return mBinLinker->GetData( BL_IDX_APPLYT_JPN );
    }
    void * FileLoader::GetParticleFileBuffer(void) const
    {
      return mBinLinker->GetData( BL_IDX_STAR_BPTCL );
    }
    void * FileLoader::GetSpotModelFileBuffer(int index ) const
    {
      static const u32 SpotMdl[] =
      {
        BL_IDX_SPOT_01_GFBMDLP,
        BL_IDX_SPOT_02_GFBMDLP,
        BL_IDX_SPOT_03_GFBMDLP,
        BL_IDX_SPOT_04_GFBMDLP,
        BL_IDX_SPOT_05_GFBMDLP,
        BL_IDX_SPOT_06_GFBMDLP,
      };

      GFL_ASSERT( index < GFL_NELEMS(SpotMdl) );

      return mBinLinker->GetData( SpotMdl[index] );
    }

  } //dendou_demo
} //app
