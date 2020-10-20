
/**
 *  デバッグ用に任意区間の処理時間を計測するための仕組み
 */

#ifndef  __BTL_DEBUG_TIMER_H__
#define  __BTL_DEBUG_TIMER_H__

#include <heap/include/gfl2_heap.h>
class  DebugTimer {
  public:
    DebugTimer();
    ~DebugTimer();


    //Tick使ってるからCTRに限定
  #if PM_DEBUG & GF_PLATFORM_CTR

  /**
   *  利用準備
   *  @param  pHeap  ヒープハンドル
   */
  void Setup( gfl2::heap::HeapBase* pHeap );

  /**
   *  区間開始
   *  @param  name   区間名称
   */
  void  Mark( const char* name );

  /**
   *  区間終了（必ず Mark と１対にすること）
   */
  void  EndMark( void );

  /**
   *  記録を処理時間降順でソート
   */
  void  Sort( void );

  /**
   *  内容をプリント
   */
  void  Print( const char* caption = NULL );

  /**
   *  内容をクリア
   */
  void  Clear( void );
  #else
  void Mark( const char* name ){};
  void EndMark( void ){};
  void Sort( void ){};
  void Print( const char* caption = NULL ){};
  void Clear( void ){};
  #endif

  private:

  #if PM_DEBUG
    enum {
      RECORD_ADD_UNIT = 32,
    };

    struct Record {
      const char* name;
      s64         tick_begin;
      s64         tick_total;
      s64         milli_sec;
      s64         micro_sec;
      double      percentage;
    };

    gfl2::heap::HeapBase*   m_heap;
    u32                    m_numRecord;
    u32                    m_numUsedRecord;
    Record*                m_record;

    Record*  getNewRecord( void );
    Record*  getRecentRecord( void );
    void     checkPrintParams( void );
   #endif
};


#endif


