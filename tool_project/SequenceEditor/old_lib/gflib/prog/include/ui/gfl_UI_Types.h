//=============================================================================
/**
 * @file   gfl_UI_Types.h
 * @brief  UI�V�X�e���Ŏg�p����^
 * @author obata_toshihiro
 * @date   2010.11.08
 */
//=============================================================================
#ifndef __GFL_UI_TYPES_H__
#define __GFL_UI_TYPES_H__ 
#pragma once

#if GFL_DEBUG
#define GFL_UI_USE_DEBUG_PAD  // ��`���Ă����, �f�o�b�O�p�b�h���L���ɂȂ�
#endif

// SBTS[631] �y�K�C�h���C���z�W���C���Z���T�̎g�p�̗L���ɂ���
// GFL_UI_USE_GYROSCOPE��`��Ԃł́A �W���C���Z���T�[�̏��������삵�Ȃ��悤�ɂ���B
#ifdef GFL_UI_USE_GYROSCOPE
  // GYROSCOPE�L�����̏���
#else
  // GYROSCOPE�������̏���
#endif

namespace gfl {
  namespace ui { 


    //-------------------------------------------------------------------------
    /**
     * @brief �����L�[�̓���
     */
    //-------------------------------------------------------------------------
    enum {
      DIR_LEFT  = 1<<0,
      DIR_RIGHT = 1<<1,
      DIR_UP    = 1<<2,
      DIR_DOWN  = 1<<3,
      
      DIR_ALL = DIR_LEFT|DIR_RIGHT|DIR_UP|DIR_DOWN,
    }; 
    typedef u8 Direction;


    //-------------------------------------------------------------------------
    /**
     * @brief �{�^��ID
     */
    //-------------------------------------------------------------------------
    enum ButtonID
    {
      BUTTONID_LEFT,
      BUTTONID_RIGHT,
      BUTTONID_UP,
      BUTTONID_DOWN,
      BUTTONID_A,
      BUTTONID_B,
      BUTTONID_X,
      BUTTONID_Y,
      BUTTONID_L,
      BUTTONID_R,
      BUTTONID_ZL,
      BUTTONID_ZR,
      BUTTONID_START,
      BUTTONID_SELECT,
      BUTTONID_HOME,
      BUTTONID_PLUS,
      BUTTONID_MINUS,
      BUTTONID_INVALID,  // �����l
      BUTTON_NUM,        // ����
    }; 


    //-------------------------------------------------------------------------
    /**
     * @brief �{�^���̓��͏�Ԃ�\���r�b�g�t���O
     */
    //-------------------------------------------------------------------------
    enum {
      BUTTON_LEFT    = 1<<BUTTONID_LEFT,
      BUTTON_RIGHT   = 1<<BUTTONID_RIGHT,
      BUTTON_UP      = 1<<BUTTONID_UP,
      BUTTON_DOWN    = 1<<BUTTONID_DOWN,
      BUTTON_A       = 1<<BUTTONID_A,
      BUTTON_B       = 1<<BUTTONID_B,
      BUTTON_X       = 1<<BUTTONID_X,
      BUTTON_Y       = 1<<BUTTONID_Y,
      BUTTON_L       = 1<<BUTTONID_L,
      BUTTON_R       = 1<<BUTTONID_R,
      BUTTON_ZL      = 1<<BUTTONID_ZL,
      BUTTON_ZR      = 1<<BUTTONID_ZR,
      BUTTON_START   = 1<<BUTTONID_START,
      BUTTON_SELECT  = 1<<BUTTONID_SELECT,
      BUTTON_HOME    = 1<<BUTTONID_HOME,
      BUTTON_PLUS    = 1<<BUTTONID_PLUS,
      BUTTON_MINUS   = 1<<BUTTONID_MINUS,
      BUTTON_INVALID = 1<<BUTTONID_INVALID,
      
      BUTTON_CROSS   = BUTTON_LEFT|BUTTON_RIGHT|BUTTON_UP|BUTTON_DOWN,
    }; 


  }  //namespace ui
}  //namespace gfl

#endif // __GFL_UI_TYPES_H__
