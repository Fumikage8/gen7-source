//=============================================================================
/**
 * @file   gfl_UI_RepeatTimer.cpp
 * @brief  ���s�[�g���͊Ǘ�
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
     * @brief �R���X�g���N�^
     * @param start_wait  �ŏ��̃��s�[�g����܂ł̑҂�����
     * @param interval    ���s�[�g����̊Ԋu
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
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    RepeatTimer::~RepeatTimer()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���s�[�g�^�C�~���O���ǂ����𒲂ׂ�
     */
    //-------------------------------------------------------------------------
    bool RepeatTimer::IsRepeat( void ) const
    {
      return (m_repeatCount == 0);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���s�[�g�Ǘ���Ԃ��X�V����
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
     * @brief ���s�[�g�Ǘ���Ԃ����Z�b�g����
     */
    //-------------------------------------------------------------------------
    void RepeatTimer::Reset( void )
    {
      m_repeatCount = m_repeatParam.start_wait;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���s�[�g���͂̌��o�p�����[�^��ݒ肷��
     */
    //-------------------------------------------------------------------------
    void RepeatTimer::SetRepeatParam( const Device::RepeatParam& param )
    {
      m_repeatParam = param;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���s�[�g���͂̌��o�p�����[�^���擾����
     */
    //-------------------------------------------------------------------------
    void RepeatTimer::GetRepeatParam( Device::RepeatParam* p_param ) const
    {
      *p_param = m_repeatParam;
    }


  } //namespace ui
} //namespace gfl
