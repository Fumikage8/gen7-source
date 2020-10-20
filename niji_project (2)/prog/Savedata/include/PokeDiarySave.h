//==============================================================================
/**
 * @file	  PokeDiarySave.h
 * @brief   ポケモンダイアリー用セーブデータ
 * @date    2012.09.28
 * @author  tamada
 */
// =============================================================================

#if !defined( __POKEDIARYSAVEDATA_H__ )
#define	__POKEDIARYSAVEDATA_H__

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_std_string.h>
#include "heap/include/gfl2_heap_manager.h"
#include "Savedata/include/SaveDataInterface.h"
#include "niji_reference_files/savedata/PokeDiaryDefine.h"

GFL_NAMESPACE_BEGIN(PokeDiary)
  //--------------------------------------------------------------------------
  /**
   * @brief   ポケモンダイアリーをセットする
   * @param[in] pdid  ポケモンダイアリーイベントID
   * @param[in] p1    パラメータ１
   * @param[in] p2    パラメータ２
   * @param[in] p3    パラメータ３
   *
   * @note  許可を得て、グローバル関数として公開しています。
   */
  //--------------------------------------------------------------------------
  extern void Set( PDID pdid, u16 p1 = 0, u16 p2 = 0, u16 p3 = 0 );

  //--------------------------------------------------------------------------
  /**
   * @brief   ポケモンダイアリーのパーソナルの状態を取得
   */
  //--------------------------------------------------------------------------
  extern bool GetPersonal( PDID pdid );
GFL_NAMESPACE_END(PokeDiary)

GFL_NAMESPACE_BEGIN(Savedata)

class PokeDiarySave : public SaveDataInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(PokeDiarySave);

  //グローバル関数pokediary::Setには内部を公開する
  friend void PokeDiary::Set( PDID pdid, u16 p1, u16 p2, u16 p3 );

public:
  // コンストラクタ
  PokeDiarySave( void );
  // デストラクタ
  virtual ~PokeDiarySave( void );

public:
	// SaveDataInterface実装
  //--------------------------------------------------------------------------
	/**
	 * @brief		読み込んだデータをセットする関数
	 * @param		pData		先頭のポインタ
	 */
	//--------------------------------------------------------------------------
  virtual void SetData(void* data);
  //--------------------------------------------------------------------------
	/**
   * @brief    データの先頭ポインタを得る関数
   * @return   先頭のポインタ
	 */
	//--------------------------------------------------------------------------
  virtual void* GetData(void);
  //--------------------------------------------------------------------------
	/**
   * @brief    データサイズ
   * @retval   バイト
	 */
	//--------------------------------------------------------------------------
  virtual size_t GetDataSize(void);
  //--------------------------------------------------------------------------
	/**
   * @brief セーブデータのクリア
   * @param heap　クリアに使用するテンポラリヒープ
	 */
	//--------------------------------------------------------------------------
  virtual void Clear( gfl2::heap::HeapBase * heap );


public:
  enum DiaryType {
    DIARYTYPE_NONE,
    DIARYTYPE_PERSONAL,
    DIARYTYPE_LIVE,
    DIARYTYPE_COUNT,
  };

  enum {
    DIARYTYPE_PERSONAL_MAX  = 30,
    DIARYTYPE_LIVE_MAX = 10,
    DIARYTYPE_COUNT_MAX = 50,

    LAST_DIARY_MAX = 3,
  };

#if PM_DEBUG
  void Dump( DiaryType type );
  void SetStr( wchar_t* outStr, DiaryType type, int index );
  void SetDebugData( DiaryType type, PDID id, int rom, int* data );
#endif

private:
  /// 着せ替え情報
  struct DIARY_FASHION_DATA
  {
    u8 dummy[16];
  };

public:
  /// 時間情報
  struct DIARY_DATE
  {
    u32 year:7;     ///< 年
    u32 month:4;    ///< 月
    u32 day:5;      ///< 日
    u32 hour:5;     ///< 時
    u32 minute:6;   ///< 分
    u32 half_sec:5; ///< 秒
  };
  /// アップロードするデータ構造
  struct DIARY_DATA
  {
    u16 rom_code:6;     /**< 生成データのロムバージョン*/
    u16 diary_type:2;   /**< ダイアリー種別*/
    u16 diary_id:8;     /**< ダイアリーID（種別内分類）*/
    u16 data[3];        /**< 固有データ */
    DIARY_DATE date;    /**< 日付 */
  };


  //-------------------------------------
  ///	セーブデータ
  //=====================================
	struct CoreData
	{
    DIARY_DATA DiaryPersonalData[ DIARYTYPE_PERSONAL_MAX ];
    DIARY_DATA DiaryLiveData[ DIARYTYPE_LIVE_MAX ];
    DIARY_DATA DiaryCountData[ DIARYTYPE_COUNT_MAX ];

    DIARY_FASHION_DATA DiaryFashion[ DIARYTYPE_PERSONAL_MAX ];

    u8 Reserve[256];
	};

public:

  //--------------------------------------------------------------------------
  /**
   * @brief   レイログ作成
   *          （最新のデータを入れる）
   * @param[out] outDiaryData   ダイアリーデータ
   * @return trueで取得成功
   */
  //--------------------------------------------------------------------------
  void CreatePlayLog( DIARY_DATA pOutDiaryData[] );

  //--------------------------------------------------------------------------
  /**
   * @brief   ポケモンダイアリーを取得する
   * @param[in] pdid  ポケモンダイアリーイベントID
   * @param[in] isReverse 　最新の方から取得するか
   * @param[out] outDiaryData    ダイアリーデータ
   * @return trueで取得成功
   */
  //--------------------------------------------------------------------------
  bool Get( PDID pdid, bool isReverse, DIARY_DATA* pOutDiaryData );

private:


	CoreData m_data;

  //--------------------------------------------------------------------------
  //  内部作業用関数
  //--------------------------------------------------------------------------
  void SetPersonal( PDID pdid, u16 p1, u16 p2, u16 p3, DIARY_DATE date, const void * fashion );
  void SetLive( PDID pdid, u16 p1, u16 p2, u16 p3, DIARY_DATE date );
  void SetCount( PDID pdid, u16 p1, u16 p2, u16 p3, DIARY_DATE date );

  static DIARY_DATA * SortAndGetPos( DIARY_DATA * dd, int max );
  static DIARY_DATA * GetPos( DIARY_DATA * dd, PDID pdid, int max );
  static DIARY_DATA * GetPosReverse( DIARY_DATA * dd, PDID pdid, int max );
  static void ClearSingleData( DIARY_DATA * dd );

  // @fix MMCat[223] レコードのパラメータの中にmomiji新ポケモンが含まれていないかチェック
  bool CheckDiaryParamMomijiPoke( const DIARY_DATA * dd );
  

};


GFL_NAMESPACE_END(Savedata)

#endif	/* __POKEDIARYSAVEDATA_H__ */

