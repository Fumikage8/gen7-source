//=============================================================================
/**
 * @file   gfl_UI_RepeatTimer.cpp
 * @brief  リピート入力管理
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#include <nn.h>
#include "gfl_Macros.h"
#include "ui/gfl_UI_Device.h"
#include "./gfl_UI_RepeatTimer.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param start_wait  最初のリピート判定までの待ち時間
     * @param interval    リピート判定の間隔
     */
    //-------------------------------------------------------------------------
    RepeatTimer::RepeatTimer( u32 start_wait, u32 interval ) :
      m_repeatParam(),
      m_repeatCount( 0 )
    {
      m_repeatParam.start_wait = start_wait;
      m_repeatParam.interval = interval;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    RepeatTimer::~RepeatTimer()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief リピートタイミングかどうかを調べる
     */
    //-------------------------------------------------------------------------
    bool RepeatTimer::IsRepeat( void ) const
    {
      return (m_repeatCount == 0);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief リピート管理状態を更新する
     */
    //-------------------------------------------------------------------------
    void RepeatTimer::Update( void )
    {
      if( m_repeatCount <= 0 ) {
        m_repeatCount = m_repeatParam.interval;
      }
      m_repeatCount--;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief リピート管理状態をリセットする
     */
    //-------------------------------------------------------------------------
    void RepeatTimer::Reset( void )
    {
      m_repeatCount = m_repeatParam.start_wait;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief リピート入力の検出パラメータを設定する
     */
    //-------------------------------------------------------------------------
    void RepeatTimer::SetRepeatParam( const Device::RepeatParam& param )
    {
      m_repeatParam = param;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief リピート入力の検出パラメータを取得する
     */
    //-------------------------------------------------------------------------
    void RepeatTimer::GetRepeatParam( Device::RepeatParam* p_param ) const
    {
      *p_param = m_repeatParam;
    }


  } //namespace ui
} //namespace gfl
