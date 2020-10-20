//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_Date.cpp
 *	@brief  �����Ǘ��N���X
 *	@author	tomoya takahashi
 *	@date		2011.09.10
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <std/gfl_Date.h>

namespace gfl
{
namespace std
{

  // �����R�A���
  bool              DateTime::m_Initialize = false;
  nn::fnd::DateTime DateTime::m_CoreDate;
  s32               DateTime::m_Wait = 0;
  
#if GFL_DEBUG

  DateTime::FakeMode DateTime::m_FakeMode = DateTime::FAKE_NONE;
  u32      DateTime::m_Rate     = 1;
  s64      DateTime::m_BeginSec = 0;
  s64      DateTime::m_CoreSec = 0;
  s64      DateTime::m_RecSec   = 0;
  nn::fnd::DateTime DateTime::m_FakeDate;
    
#endif
  
//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					����
*/
//-----------------------------------------------------------------------------


  //----------------------------------------------------------------------------
  /**
   *	@brief  ������
   */
  //-----------------------------------------------------------------------------
  void DateTime::Initialize( void )
  {
    // ���̎��Ԏ擾
    nn::fnd::DateTime date = nn::fnd::DateTime::GetNow();
    m_CoreDate = date;
    m_Wait = 0;

    // ����������
    m_Initialize = true;
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  �X�V����
   *
   *	@param isForceUpdate true���w�肷�邱�Ƃŋ����X�V
   */
  //-----------------------------------------------------------------------------
  void DateTime::Update( bool isForceUpdate )
  {
    if( !m_Initialize ){
      GFL_ASSERT( m_Initialize );
      return ;
    }
    
    // �ʏ�f�[�^�X�V
    {
      ++m_Wait;
      if( (m_Wait >= sc_DATE_UPDATE_WAIT) || isForceUpdate ){
        // ���̎��Ԏ擾
        nn::fnd::DateTime date = nn::fnd::DateTime::GetNow();
        m_CoreDate = date;
        m_Wait = 0;
      }
    }

#if GFL_DEBUG
    if( m_FakeMode )
    {
      UpdateFake();
    }
#endif
    
  }



#if GFL_DEBUG

  //----------------------------------------------------------------------------
  /**
   *	@brief  �t�F�C�N�^�C���J�n
   *
   *	@param	u32 rate  �i�ރX�s�[�h�̔{��
   */
  //-----------------------------------------------------------------------------
  void DateTime::StartFakeTime( u32 rate )
  {
    if(m_FakeMode == FAKE_NONE){
      m_FakeDate = m_CoreDate;
    }
    
    m_FakeMode = FAKE_FAST;
    m_Rate     = rate;

    nn::fnd::DateTime date2000;
    nn::fnd::TimeSpan time = m_FakeDate - date2000;
    m_BeginSec = time.GetSeconds();
    time = m_CoreDate - date2000;
    m_CoreSec  = time.GetSeconds();
    m_RecSec   = m_CoreSec;

  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  �t�F�C�N���[�h�I��
   */
  //-----------------------------------------------------------------------------
  void DateTime::StopFakeTime( void )
  {
    m_FakeMode = FAKE_NONE;
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ���Ԃƕ���Fix������B
   *
   *	@param	hour    ����
   *	@param	minute  ��
   */
  //-----------------------------------------------------------------------------
  void DateTime::FixFakeTime( s32 hour, s32 minute )
  {
    m_FakeMode = FAKE_FIX;
    m_FakeDate = m_CoreDate;
    m_FakeDate = m_FakeDate.ReplaceHour( hour );
    m_FakeDate = m_FakeDate.ReplaceMinute( minute );
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ���������ׂĎw�肵�ĂƂ߂�B
   */
  //-----------------------------------------------------------------------------
  void DateTime::FixFakeDate( const DateTime* p_date )
  {
    m_FakeMode = FAKE_FIX;
    m_FakeDate = p_date->m_Date;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  Fake���Ԃ̍X�V
   */
  //-----------------------------------------------------------------------------
  void DateTime::UpdateFake( void )
  {
    s64 now, fake;

    if( m_FakeMode != DateTime::FAKE_FAST ){
      return ;
    }

    nn::fnd::DateTime date2000;
    nn::fnd::TimeSpan time = m_CoreDate - date2000;
    now = time.GetSeconds();

    if( now == m_RecSec ){
      return ;
    }

    // �U���Ԃ̎Z�o
    fake = m_BeginSec + ((now - m_CoreSec) * m_Rate);
    m_RecSec = now;
    m_FakeDate = date2000 + nn::fnd::TimeSpan::FromSeconds( fake );
    
  }
    
  //-----------------------------------------------------------------------------
  /**
   *	@brief  FakeMode�̎擾
   */
  //-----------------------------------------------------------------------------
  DateTime::FakeMode DateTime::GetFakeMode( void )
  {
    return m_FakeMode;
  }
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  Fake���[�g�̎擾
   */
  //-----------------------------------------------------------------------------
  u32 DateTime::GetFakeRate( void )
  {
    return m_Rate;
  }
  
#endif


  //----------------------------------------------------------------------------
  /**
   *	@brief  �R���X�g���N�^
   *
   */
  //-----------------------------------------------------------------------------
  DateTime::DateTime( s32 year, s32 month, s32 day, s32 hour, s32 minute, s32 second, s32 millisecond ) : m_Date( year, month, day, hour, minute, second, millisecond )
  {
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  �R���X�g���N�^
   *
   *	@param	second    2000/01/01/00:00����̌o�ߕb��
   */
  //-----------------------------------------------------------------------------
  DateTime::DateTime( s64 second ) : m_Date()
  {
    this->SetDateTimeToSecond( second );
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  �R���X�g���N�^
   */
  //-----------------------------------------------------------------------------
  DateTime::DateTime( void ) : m_Date()
  {
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  DateTime::~DateTime()
  {
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  ���ݓ����̎擾
   *
   *	@param	p_date    �i�[��
   */
  //-----------------------------------------------------------------------------
  void DateTime::GetNow( DateTime* p_date )
  {
    GFL_ASSERT( m_Initialize );

    if( p_date ){
#if GFL_DEBUG
      if( m_FakeMode != DateTime::FAKE_NONE ){
        p_date->m_Date = m_FakeDate;
      }else{
        p_date->m_Date = m_CoreDate;
      }
#else
      p_date->m_Date = m_CoreDate;
#endif
    }
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ���̃R�s�[
   *
   *	@param	cp_src    �\�[�X
   *	@param	p_dst     �i�[��
   */
  //-----------------------------------------------------------------------------
  void DateTime::Copy( const DateTime* cp_src, DateTime* p_dst )
  {
    if( (p_dst != NULL) && (cp_src != NULL) ){
      p_dst->m_Date = cp_src->m_Date;
    }
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  �ŏ������̎擾
   *
   *	@param	p_date    �i�[��
   */
  //-----------------------------------------------------------------------------
  void DateTime::GetMin( DateTime* p_date )
  {
    p_date->m_Date = nn::fnd::DateTime::MIN_DATETIME;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  �ő�����̎擾
   *
   *	@param	p_date    �i�[��
   */
  //-----------------------------------------------------------------------------
  void DateTime::GetMax( DateTime* p_date )
  {
    p_date->m_Date = nn::fnd::DateTime::MAX_DATETIME;
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  �N�̐ݒ�
   *
   *	@param	s32 year 
   */
  //-----------------------------------------------------------------------------
  void DateTime::SetYear( s32 year )
  {
    m_Date = m_Date.ReplaceYear( year );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ���̐ݒ�
   *
   *	@param	s32 month 
   */
  //-----------------------------------------------------------------------------
  void DateTime::SetMonth( s32 month )
  {
    m_Date = m_Date.ReplaceMonth( month );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ���̐ݒ�
   *
   *	@param	s32 day 
   */
  //-----------------------------------------------------------------------------
  void DateTime::SetDay( s32 day )
  {
    m_Date = m_Date.ReplaceDay( day );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ���Ԃ̐ݒ�
   *
   *	@param	s32 hour 
   */
  //-----------------------------------------------------------------------------
  void DateTime::SetHour( s32 hour )
  {
    m_Date = m_Date.ReplaceHour( hour );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ���̐ݒ�
   *
   *	@param	s32 minute 
   */
  //-----------------------------------------------------------------------------
  void DateTime::SetMinute( s32 minute )
  {
    m_Date = m_Date.ReplaceMinute( minute );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  �b�̐ݒ�
   *
   *	@param	s32 second 
   */
  //-----------------------------------------------------------------------------
  void DateTime::SetSecond( s32 second )
  {
    m_Date = m_Date.ReplaceSecond( second );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  �~���b�̐ݒ�
   *
   *	@param	millisecond 
   */
  //-----------------------------------------------------------------------------
  void DateTime::SetMilliSecond( s32 millisecond )
  {
    m_Date = m_Date.ReplaceMilliSecond( millisecond );
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  �N�̎擾
   */
  //-----------------------------------------------------------------------------
  s32 DateTime::GetYear( void ) const
  {
    return m_Date.GetYear();
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ���̎擾
   */
  //-----------------------------------------------------------------------------
  s32 DateTime::GetMonth( void ) const
  {
    return m_Date.GetMonth();
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  �j���̎擾
   */
  //-----------------------------------------------------------------------------
  nn::fnd::Week DateTime::GetWeek( void ) const
  {
    return m_Date.GetWeek();
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ���̎擾
   */
  //-----------------------------------------------------------------------------
  s32 DateTime::GetDay( void ) const
  {
    return m_Date.GetDay();
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ���Ԃ̎擾
   */
  //-----------------------------------------------------------------------------
  s32 DateTime::GetHour( void ) const
  {
    return m_Date.GetHour();
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ���̎擾
   */
  //-----------------------------------------------------------------------------
  s32 DateTime::GetMinute( void ) const
  {
    return m_Date.GetMinute();
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  �b�̎擾
   */
  //-----------------------------------------------------------------------------
  s32 DateTime::GetSecond( void ) const
  {
    return m_Date.GetSecond();
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  �~���b�̎擾
   */
  //-----------------------------------------------------------------------------
  s32 DateTime::GetMilliSecond( void ) const
  {
    return m_Date.GetMilliSecond();
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  �p�����[�^�܂Ƃ߂Ď擾
   */
  //----------------------------------------------------------------------------
  nn::fnd::DateTimeParameters DateTime::GetParameters( void ) const
  {
    return m_Date.GetParameters();
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ������2000/1/1/00:00����̌o�ߓ����ɕϊ�����B
   *	@return�@�o�ߓ���
   */
  //-----------------------------------------------------------------------------
  s64 DateTime::GetDateTimeToDay( void ) const
  {
    nn::fnd::DateTime date2000;
    nn::fnd::TimeSpan time = m_Date - date2000;
    return time.GetDays();
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ������2000/1/1/00:00����̌o�ߎ��Ԃɕϊ�����B
   *	@return�@�o�ߕb��
   */
  //-----------------------------------------------------------------------------
  s64 DateTime::GetDateTimeToSecond( void ) const
  {
    nn::fnd::DateTime date2000;
    nn::fnd::TimeSpan time = m_Date - date2000;
    return time.GetSeconds();
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ������2000/1/1/00:00����̌o�ߎ��Ԃɕϊ�����B
   *	@return�@�o�߃~���b
   */
  //-----------------------------------------------------------------------------
  s64 DateTime::GetDateTimeToMilliSecond( void ) const
  {
    nn::fnd::DateTime date2000;
    nn::fnd::TimeSpan time = m_Date - date2000;
    return time.GetMilliSeconds();
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  2000/1/1/00:00����̌o�ߕb������A������ݒ肷��
   *  @param  second  �b��
   */
  //-----------------------------------------------------------------------------
  void DateTime::SetDateTimeToSecond( s64 second )
  {
    nn::fnd::DateTime date2000;
    nn::fnd::TimeSpan time = nn::fnd::TimeSpan::FromSeconds( second );

    m_Date = date2000 + time;
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  ����ȓ��������f����B
   *
   *	@retval true    ����
   *	@retval false   ��������
   */
  //-----------------------------------------------------------------------------
  bool DateTime::IsValidDate( void ) const
  {
    return nn::fnd::DateTime::IsValidParameters( 
        m_Date.GetYear(),
        m_Date.GetMonth(),
        m_Date.GetDay(),
        m_Date.GetHour(),
        m_Date.GetMinute(),
        m_Date.GetSecond(),
        m_Date.GetMilliSecond() );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ���邤�N�����f����B
   *
   *	@retval true    ���邤�N
   *	@retval false   ������
   */
  //-----------------------------------------------------------------------------
  bool DateTime::IsLeapYear( void ) const
  {
    return nn::fnd::DateTime::IsLeapYear( m_Date.GetYear() );
  }


} // namespace std
} //namespace gfl
