#include <debug/include/gfl2_Assert.h>

#include "./btl_DebugPrint.h"
#include "./btl_DebugTimer.h"

DebugTimer::DebugTimer( void )
{
  #if PM_DEBUG
    m_heap = NULL;
    m_numRecord = 0;
    m_numUsedRecord = 0;
    m_record = NULL;
  #endif
}
DebugTimer::~DebugTimer()
{
  #if PM_DEBUG
  if( m_record ){
    GflHeapFreeMemory( m_record );
  }
  #endif
}


#if PM_DEBUG & GF_PLATFORM_CTR
/**
 * 利用準備
 */
void DebugTimer::Setup( gfl2::heap::HeapBase* pHeap )
{
  m_heap = pHeap;
}


/**
 *  区間開始
 *  @param  name   区間名称
 */
void DebugTimer::Mark( const char* name )
{
  Record*  rec = this->getNewRecord();
  if( rec )
  {
    rec->name = name;
    rec->tick_total = 0;
    rec->tick_begin = nn::os::Tick::GetSystemCurrent();
  }
}

/**
 *  区間終了（必ず Mark と１対にすること）
 */
void DebugTimer::EndMark( void )
{
  Record*  rec = this->getRecentRecord();
  if( rec )
  {
    s64  tick_end = nn::os::Tick::GetSystemCurrent();
    rec->tick_total = tick_end - rec->tick_begin;
  }
}

/**
 *  記録を処理時間降順でソート
 */
void DebugTimer::Sort( void )
{
  for(u32 i=0; i<m_numUsedRecord; ++i)
  {
    Record  rec_max = m_record[i];
    u32     k = i;
    for(u32 j=i+1; j<m_numUsedRecord; ++j)
    {
      if( rec_max.tick_total < m_record[j].tick_total )
      {
        k = j;
        rec_max = m_record[j];
      }
    }

    {
      Record  tmp = m_record[i];
      m_record[i] = m_record[k];
      m_record[k] = tmp;
    }
  }
}

/**
 *  内容をプリント
 */
void DebugTimer::Print( const char* caption )
{
  this->checkPrintParams();

  if( caption ){
    BTL_PRINT("%s\n", caption);
  }

  BTL_PRINT(" MilliSec : MicroSec\n");
  for(u32 i=0; i<m_numUsedRecord; ++i)
  {
    BTL_PRINT("%9lld : %9lld  (%4.1f%%) // %s\n", m_record[i].milli_sec, m_record[i].micro_sec, m_record[i].percentage, m_record[i].name );
  }
}

/**
 *  内容をクリア
 */
void DebugTimer::Clear( void )
{
  m_numUsedRecord = 0;
}

/**
 *  新しい記録領域を取得
 */
DebugTimer::Record* DebugTimer::getNewRecord( void )
{
  if( m_numUsedRecord < m_numRecord )
  {
    return &(m_record[ m_numUsedRecord++ ]);
  }

  u32 nextNumRecord = m_numRecord + RECORD_ADD_UNIT;
  Record*  pRecord = (Record*)(GflHeapAllocMemory( m_heap, sizeof(Record) * nextNumRecord ));
  if( pRecord )
  {
    if( m_record )
    {
      for(u32 i=0; i<m_numRecord; ++i){
        pRecord[i] = m_record[i];
      }
      GflHeapFreeMemory( m_record );
    }
    m_record = pRecord;
    m_numRecord = nextNumRecord;
    return &(m_record[ m_numUsedRecord++ ]);
  }

  GFL_ASSERT(0);
  return NULL;
}
/**
 *  最近使った記録領域を取得
 */
DebugTimer::Record* DebugTimer::getRecentRecord( void )
{
  if( m_numUsedRecord )
  {
    return &(m_record[ m_numUsedRecord-1 ]);
  }
  GFL_ASSERT(0);
  return NULL;
}
/**
 *  Printするために必要な情報を記録領域に書き込む
 */
void  DebugTimer::checkPrintParams( void )
{
  s64  total_micro_sec = 0;

  for(u32 i=0; i<m_numUsedRecord; ++i)
  {
    nn::os::Tick  t( m_record[i].tick_total );
    m_record[i].milli_sec = t.ToMilliSeconds();
    m_record[i].micro_sec = t.ToMicroSeconds();
    total_micro_sec += m_record[i].micro_sec;
  }

  for(u32 i=0; i<m_numUsedRecord; ++i)
  {
    m_record[i].percentage = (double)(m_record[i].micro_sec * 100) / (double)(total_micro_sec);
  }
}

#endif  // #if PM_DEBUG



