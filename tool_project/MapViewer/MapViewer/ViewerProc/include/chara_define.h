#pragma once
//=================================================================================================
/**
 *  @file  chara_define.h
 *  @brief  niji�p�L�����w���`
 *  @author  tamada@gamefreak.co.jp
 *  @date  2014.05.21
 *
 *  niji�ɂ����ẮA�o�g���p�E�t�B�[���h�p�œ���̃L�������f�����g�p���邽�߁A���ꂵ����`��p�ӂ���B
 */
//=================================================================================================

namespace chara {

  //-------------------------------------------------------------------------------------------------
  /**
  *  @enum
  *  @brief �L�����N�^�[�w��ID�̒�`
  */
  //-------------------------------------------------------------------------------------------------
  enum RSC_ID
  {
    RSCID_HERO,
    RSCID_PIKACYU,
    RSCID_MAP_01_KABE,
    RSCID_MAP_01_YUKA,
    RSCID_MAP00_00_00,   // ����
    RSCID_MAP00_00_01,   // �E�� 
    RSCID_MAP00_01_00,   // ����
    RSCID_MAP00_01_01,   // �E��
    RSCID_MAP_TEST_HEIGHT,  // 4�u���b�N���̍���
    RSCID_MAP00_TEST,   // 
    RSCID_ATTACH_OBJ_TSURIZAO,

    // �������׌v���p�L�������f��
    RSCID_EDGE_TEST_00,   // ID�J���[�G�b�W�i�\��}�e���A����2�j
    RSCID_EDGE_TEST_01,   // ���_�J���[�G�b�W�i�\��}�e���A����2�j
    RSCID_MAT_TEST_00,   // �\��}�e���A����2�iedge_test_00�Ɠ������j
    RSCID_MAT_TEST_01,   // 1�}�e���A����UV�Z�b�g��2�A�e�N�X�`���ɃA���t�@�`�����l�����܂܂��

    RSCID_TEST_KISEKAE,   // �����ւ����ؗp
    RSCID_TEST_KENTAUROS, // �|�P���C�h���ؗp
    RSCID_TEST_NPC_1500,  // 1500�|���S��NPC���f���iLl���f���j
    RSCID_TEST_NPC_2000,  // 2000�|���S��NPC���f���iLh���f���j
    RSCID_TEST_RIM,       // �������C�g���ؗp

    RSCID_VIEWER_MAP,

    RSCID_NUM
  };

  //-------------------------------------------------------------------------------------------------
  /**
  *  @brief �L�����̃��[�V�����w��ID
  *  ����A�S�L��������ID�Ŏw�肷�邱�Ƃ�z�肵�Ă���
  */
  //-------------------------------------------------------------------------------------------------
  enum 
  {
    MOTION_ID_WAIT = 0,
    MOTION_ID_WALK,
    MOTION_ID_RUN,
    MOTION_ID_WALK_FAST,
    MOTION_ID_TSURI_START,
    MOTION_ID_MAX
  };

}; //end of namespace chara
