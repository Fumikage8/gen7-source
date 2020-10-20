//=============================================================================
/**
 *
 *	@file		gfl_Fade.h
 *	@brief  �t�F�[�h�V�X�e��
 *	@author hosaka genya -> takada keisuke
 *	@data		2011.06.07 -> 2012.08.06
 *
 */
//=============================================================================
#ifndef __GFL_FADE_H__
#define __GFL_FADE_H__
#pragma once


namespace gfl {
namespace grp {

const int FADE_DEFAULT_SYNC = 2; ///< ����̃t�F�[�h����

namespace util {
class Texture;
}

class RequestUnit;

/**
 * @brief �t�F�[�h�N���X
 */
class Fade : public base::Singleton<Fade> {
    GFL_FORBID_COPY_AND_ASSIGN(Fade);
public:

    /**
     * @brief �\���f�B�X�v���C��`
     */
    typedef enum {
        SETUP_UPPER = 0, ///< ����
        SETUP_MAX
    } SETUP_DISPLAY_TYPE;

    /**
     * @brief ���ʃA�j��ID��`
     */
    enum {
        ANMID_UPPER_TEST_IN = 0,
        ANMID_UPPER_TEST_OUT,
        ANMID_UPPER_WC_IN,
        ANMID_UPPER_WC_OUT,
        ANMID_UPPER_WB_IN,
        ANMID_UPPER_WB_OUT,
        ANMID_UPPER_WS_IN_UP,
        ANMID_UPPER_WS_OUT_UP,
        ANMID_UPPER_WS_IN_DOWN,
        ANMID_UPPER_WS_OUT_DOWN,
        ANMID_UPPER_WS_IN_LEFT,
        ANMID_UPPER_WS_OUT_LEFT,
        ANMID_UPPER_WS_IN_RIGHT,
        ANMID_UPPER_WS_OUT_RIGHT,
        ANMID_UPPER_WR_IN,
        ANMID_UPPER_WR_OUT,
        ANMID_UPPER_NUM,
        ANMID_UPPER_NULL,
    };

    /**
     * @brief ���C�A�E�g���\�[�X�f�[�^
     */
    struct LYT_RES_DATA {
        gfl::fs::ArcFile::ARCID arcID; ///< ���C�A�E�g���\�[�X���܂܂�Ă���A�[�J�C�u
        gfl::fs::ArcFile::ARCDATID arcDatID; ///< ���C�A�E�g���\�[�X�i?BCLYT�j
        u32 addTexNum; ///< �w��̃��\�[�X�ɒǉ��o�^���郊�\�[�X���̍ő吔
    };

    /**
     * @brief ���C�A�E�g���[�N�Ǘ�
     */
    typedef struct {
        g2d::LytWk* pLytWk; ///< ���C�A�E�g���[�N
        SETUP_DISPLAY_TYPE setupDisp; ///< �`����
        bool enable; ///< �\���ݒ�
    } LAYOUT_WORK;

    /**
     * @brief ���C�A�E�g���[�N�ݒ�f�[�^
     */
    typedef struct {
        u32 resIndex; ///< �ݒ肵�����\�[�X�̃C���f�b�N�X
        g2d::LytArcIndex layoutDataID; ///< resID���̃��C�A�E�g�f�[�^ID
        const g2d::LytArcIndex * anmDataIDTbl; ///< �A�j���[�V�����f�[�^ID�e�[�u�� �A�j���̂Ȃ��ꍇ��NULL
        u32 anmTblNum; ///< �A�j���[�V�����f�[�^ID�e�[�u���� �A�j���̂Ȃ��ꍇ��0
        u32 anmMax; ///< �o�^�A�j���[�V�����ő吔
        bool calculateMtxFlag; ///< ��\���y�C���ɍs��v�Z���s����
        SETUP_DISPLAY_TYPE setupDisp; ///< �f�B�X�v���C
        bool enable; ///< �\���ݒ�
    } LYTWK_DATA;

    /**
     * @brief �R�[���o�b�N�f�[�^
     */
    typedef struct {
        LAYOUT_WORK* ExternalLytWork; ///< ���C�A�E�g���[�N�Ǘ��ւ̃|�C���^
        void* CallBackWork; ///< �|�C���^�̈����ɂ͒��ӂ��邱��(�O���Ǘ�)
    } CALLBACK_DATA;

    /**
     * @brief ��ʎw��
     */
    enum DISP {
        DISP_APPER = 0, ///< ����
        DISP_LOWER, ///< �����
        DISP_DOUBLE, ///< �����
        DISP_MAX = DISP_DOUBLE, ///< ��ʐ�
    };

    /**
     * @brief F.O.�^�C�v�w��
     */
    enum OUTTYPE {
        OUTTYPE_BLACK, ///< ��
        OUTTYPE_WHITE, ///< ��
        OUTTYPE_BLACK_CAPTURE, ///< ��ʃL���v�`���ō��ɓr���܂�F.O.�i�K��F.O.���I����Ă���F.I.���邱�Ɓj
        OUTTYPE_WHITE_CAPTURE, ///< ��ʃL���v�`���Ŕ��ɓr���܂�F.O.�i�K��F.O.���I����Ă���F.I.���邱�Ɓj
        OUTTYPE_CAPTURE, ///< ��ʃL���v�`��
        OUTTYPE_CIRCLE_SHRINK, ///< �~�`���k
        OUTTYPE_BALL, ///< �{�[��
        OUTTYPE_UP, ///< ��
        OUTTYPE_DOWN, ///< ��
        OUTTYPE_LEFT, ///< ��
        OUTTYPE_RIGHT, ///< �E
        OUTTYPE_DIAMOND, ///< �H�`
        OUTTYPE_EXTERNAL_LAYOUT_DATA, ///< �O�����C�A�E�g�f�[�^
    };

    /**
     * @brief F.I.�^�C�v�w��
     */
    enum INTYPE {
        INTYPE_BLACK, ///< ��
        INTYPE_WHITE, ///< ��
        INTYPE_BLACK_CAPTURE, ///< ��ʃL���v�`���ō��ɓr���܂�F.O.�i�K��F.O.���I����Ă���F.I.���邱�Ɓj
        INTYPE_WHITE_CAPTURE, ///< ��ʃL���v�`���Ŕ��ɓr���܂�F.O.�i�K��F.O.���I����Ă���F.I.���邱�Ɓj
        INTYPE_CAPTURE, ///< ��ʃL���v�`��
        INTYPE_CIRCLE_SHRINK, ///< �~�`���k
        INTYPE_BALL, ///< �{�[��
        INTYPE_UP, ///< ��
        INTYPE_DOWN, ///< ��
        INTYPE_LEFT, ///< ��
        INTYPE_RIGHT, ///< �E
        INTYPE_DIAMOND, ///< �H�`
    };

    typedef bool (FadeCallbackFunc) (CALLBACK_DATA* data); ///< FadeCallbackFunc�֐��̒�`

    static const s32 SCREEN_WIDTH = 512; ///< ��ʂ̉���
    static const s32 SCREAN_HEIGHT = 256; ///< ��ʂ̏c��
    static const int BLOCK_WIDTH_SIZE = 8;
    static const int BLOCK_HEIGHT_SIZE = 8;

    /**
     * @brief �R���X�g���N�^
     * @param heap_allocator �q�[�v�A���P�[�^
     * @param device_allocator �f�o�C�X�q�[�v�A���P�[�^
     * @param grpsys �O���t�B�b�N�V�X�e��
     * @param lyt_res ���C�A�E�g���\�[�X
     */
    Fade(heap::NwAllocator* heap_allocator, heap::NwAllocator* device_allocator, GraphicsSystem* grpsys, const void* lyt_res = NULL);

    /**
     *@brief  �f�X�g���N�^
     */
    ~Fade(void);

    /**
     *@brief  �又��
     */
    void Update(void);

    /**
     * @brief F.O.���N�G�X�g
     * @param disp �Ώۖ�
     * @param start_col �X�^�[�g���̉�ʐF(0~1.0f) NULL���ƑO��̐F����X�^�[�g
     * @param end_col �I�����̉�ʐF(0~1.0f)
     * @param sync F.O.�ɔ�₷����
     * @param is_keep_buffer �J�n���̃o�b�t�@��ۑ����邩�ǂ���
     */
    void RequestOut(DISP disp, const math::VEC4* start_col, const math::VEC4* end_col, u32 sync = FADE_DEFAULT_SYNC, bool is_keep_buffer = false);

    /**
     * @brief F.O.���N�G�X�g��^�C�v�w��Ł�
     * @param disp �Ώۖ�
     * @param type F.O.�^�C�v
     * @param sync F.O.�ɔ�₷����
     * @param is_keep_buffer �J�n���̃o�b�t�@��ۑ����邩�ǂ���
     */
    void RequestOut(DISP disp = DISP_DOUBLE, OUTTYPE type = OUTTYPE_CAPTURE, u32 sync = FADE_DEFAULT_SYNC, bool is_keep_buffer = false);

    /**
     * @brief F.O.���N�G�X�g��O��Layout�A�j���[�V�����Ł�
     * @param lyt_res �O�����C�A�E�g���\�[�X
     * @param anmNum �A�j���[�V������
     * @param is_keep_buffer �J�n���̃o�b�t�@��ۑ����邩�ǂ���
     */
    void RequestOut(const void* lyt_res, u8 anmNum, bool is_keep_buffer = false);

    /**
     * @brief F.I.���N�G�X�g
     * @param disp �Ώۖ�
     * @param sync F.I.�ɔ�₷����
     */
    void RequestIn(DISP disp = DISP_DOUBLE, u32 sync = FADE_DEFAULT_SYNC);

    /**
     * @brief F.I.���N�G�X�g��^�C�v�w��Ł�
     * @param disp �Ώۖ�
     * @param type F.I.�^�C�v
     * @param sync F.I.�ɔ�₷����
     */
    void RequestIn_Ex(DISP disp = DISP_DOUBLE, INTYPE type = INTYPE_BLACK, u32 sync = FADE_DEFAULT_SYNC);

    /**
     * @brief �����I��F.O.��Ԃɂ���
     * @param disp �Ώۖ�
     * @param col �I�����̉�ʐF 
     */
    void ForceOut(DISP disp, const math::VEC4* col);

    /**
     * @brief �t�F�[�h���I���������ǂ������擾
     * @return �t�F�[�h���I���������ǂ��� 
     */
    bool IsEnd(void);

    /**
     * @brief �|�[�YON/OFF
     * @param is_pause �|�[�Y�t���O
     */
    void SetPause(bool is_pause);

    /**
     * @brief �|�[�Y��Ԏ擾
     * @retval �|�[�Y���
     */
    bool IsPause(void) const;

    /**
     * @brief �t�F�[�h��ԃ��Z�b�g
     * @param disp �Ώۖ�
     */
    void Reset(DISP disp);

    /**
     * @brief �t�F�[�h�����J�b�g�t���O���Z�b�g 
     * @param disp �Ώۖ�
     * @param isCutProcess �J�b�g���邩�ǂ���
     */
    void SetIsCutProcess(DISP disp, bool isCutProcess);

    /**
     * @brief �t�F�[�h�p�̃J���[�o�b�t�@���擾
     * @param type
     * @param id
     * @return �J���[�o�b�t�@
     */
    void* GetColorBuffer(DisplayType type, int id);

    /**
     * @brief �t�F�[�h�p�̃e�N�X�`�����[�N�̎擾
     * @param type DISPLAY_UPPER��DISPLAY_LOWER
     * @param id ��Ȃ�Q���A���Ȃ�P�������Ă���
     */
    util::Texture* GetTextureWork(DisplayType type, int id);

    /**
     * @brief 2D�֘A�𐶐�
     * @param lyt_res ���C�A�E�g���\�[�X
     */
    void Create2D(const void* lyt_res);

    /**
     * @brief ���C�A�E�g�V�X�e���쐬
     */
    void CreateLayoutSystem(void);

    /**
     * @brief ���C�A�E�g���쐬
     */
    void CreateLayoutEnv(void);

    /**
     * @brief ���C�A�E�g���[�N�쐬
     * @param lytMax �쐬��
     */
    void CreateLayoutWork(u32 lytMax);

    /**
     * @brief �O���f�[�^�p�̃��C�A�E�g���[�N�쐬
     * @param lytMax �쐬��
     */
    void CreateLayoutWorkOfExternalData(u32 lytMax = 1);

    /**
     * @brief ���C�A�E�g���\�[�X�쐬
     */
    void CreateLayoutResource(void);

    /**
     * @brief �O���f�[�^�p�̃��C�A�E�g���\�[�X�쐬
     */
    void CreateLayoutResourceOfExternalData(void);

    /**
     * @brief ���C�A�E�g���\�[�X�ǂݍ��� 
     * @param lyt_res ���C�A�E�g���\�[�X
     * @param resTbl ���C�A�E�g���\�[�X�f�[�^
     * @param comp ���k�t���O
     */
    void LoadLayoutResource(const void* lyt_res, const LYT_RES_DATA* resTbl, bool comp = false);

    /**
     * @brief �O���f�[�^�p�̃��C�A�E�g���\�[�X�ǂݍ���
     * @param lyt_res ���C�A�E�g���\�[�X
     */
    void LoadLayoutResourceOfExternalData(const void* lyt_res);

    /**
     * @brief �e�N�X�`���]��
     * @param area  �]���G���A
     */
    void TransferLayoutTexture(MemoryArea area = MEMORY_AREA_FCRAM);

    /**
     * @brief �O���f�[�^�p�̃e�N�X�`���]��
     * @param area �]���G���A
     */
    void TransferLayoutTextureOfExternalData(MemoryArea area = MEMORY_AREA_FCRAM);

    /**
     * @brief ���C�A�E�g���[�N���Z�b�g���� 
     * @param layoutData ���C�A�E�g���[�N�f�[�^
     * @param id �Z�b�g���郌�C�A�E�g��ID
     * @param num �Z�b�g����f�[�^��(�f�t�H���g=1)
     */
    void SetLayoutWork(const LYTWK_DATA* layoutData, u32 id, u32 num);

    /**
     * @brief �O���f�[�^�p�̃��C�A�E�g���[�N���Z�b�g
     * @param anmNum �A�j���̐�
     */
    void SetLayoutWorkOfExternalData(u8 anmNum);

    /**
     * @brief ���C�A�E�g���\�[�X�폜
     */
    void DeleteLayoutResource(void);

    /**
     * @brief �O���f�[�^�p�̃��C�A�E�g���\�[�X�폜
     */
    void DeleteLayoutResourceOfExternalData(void);

    /**
     * @brief ���C�A�E�g���[�N�폜
     */
    void DeleteLayoutWork(void);

    /**
     * @brief �O���f�[�^�p�̃��C�A�E�g���[�N�폜
     */
    void DeleteLayoutWorkOfExternalData(void);

    /**
     * @brief ���C�A�E�g���폜
     */
    void DeleteLayoutEnv(void);

    /**
     * @brief ���C�A�E�g�V�X�e���폜
     */
    void DeleteLayoutSystem(void);

    /**
     * @brief �R�[���o�b�N�̃Z�b�g 
     * @memo ���C�A�E�g�V�X�e�����폜
     * @param pCallbackFunc �R�[���o�b�N�֐�
     * @param pWork �R�[���o�b�N���[�N
     */
    void SetCallback(FadeCallbackFunc* pCallbackFunc, void* pWork);

    /**
     * @brief F.I.�ҋ@��Ԃ��ǂ������擾
     * @param �Ώۖ�
     */
    bool IsFadeInStandbyState(DISP disp);

private:
    bool mIsCreate2D; ///< 2D���������ꂽ�ǂ���
    static const g2d::LytWkSetUp LytWkDefaultSetup[2]; ///< ���C�A�E�g���[�N�̊���ݒ� 
    g2d::LytSys* mLayoutSystem; // ���C�A�E�g�V�X�e��
    g2d::Env* mUpperLayoutEnv; // ���ʂ̃��C�A�E�g��
    g2d::Env* mLowerLayoutEnv; // ����ʂ̃��C�A�E�g��
    LAYOUT_WORK* mLayoutWork; // ���C�A�E�g���[�N�Ǘ��f�[�^
    u32 mLwkMax; // ���C�A�E�g���[�N�Ǘ��f�[�^��
    g2d::LytResID mLayoutResID; // ���\�[�XID

    bool mIsCreate2DOfExternalData; ///< �O���f�[�^�p��2D���������ꂽ���ǂ���
    LAYOUT_WORK* mLayoutWorkOfExternalData; // �O���f�[�^�p�̃��C�A�E�g���[�N�Ǘ��f�[�^
    u32 mLwkMaxOfExternalData; // �O���f�[�^�p�̃��C�A�E�g���[�N�Ǘ��f�[�^��
    g2d::LytResID mLayoutResIDOfExternalData; // �O���f�[�^�p�̃��\�[�XID

    heap::NwAllocator* m_HeapAllocator; ///< �q�[�v�A���P�[�^�[
    heap::NwAllocator* m_DeviceAllocator; ///< �f�o�C�X�q�[�v�A���P�[�^�[
    GraphicsSystem* m_GraphicsSystem; ///< �O���t�B�b�N�V�X�e��
    RequestUnit* m_RequestUnit[ DISPLAY_NORMAL_MAX ]; ///< ���j�b�g

    FadeCallbackFunc* m_pCallbackFunc; ///< �R�[���o�b�N�֐��|�C���^
    void* m_pCallbackWork; ///< �R�[���o�b�N���[�N

    static const int SYNC_CAPTURE_OUT = 2; ///< �L���v�`�� �A�E�g�ɂ����鎞��

    /**
     * @brief F.O.���N�G�X�g��L���v�`���^�C�v��
     * @param disp �Ώۖ�
     */
    void RequestOutCapture(DISP disp);

    /**
     * @brief F.O.���N�G�X�g��Color�L���v�`���^�C�v�� 
     * @param disp �Ώۖ�
     * @param start_col �X�^�[�g���̉�ʐF
     * @param end_col �I�����̉�ʐF
     * @param sync �L���v�`��F.O.�ɔ�₷����
     */
    void RequestOutColorCapture(DISP disp, const math::VEC4* start_col, const math::VEC4* end_col, u32 sync);

    /**
     * @brief F.O.���N�G�X�g��Layout�A�j���[�V�����^�C�v�� 
     * @param disp �Ώۖ�
     * @param type ���C�A�E�g�^�C�v
     * @param is_keep_buffer �J�n���̃o�b�t�@��ۑ����邩�ǂ���
     */
    void RequestOutLayoutAnimation(DISP disp, OUTTYPE type, bool is_keep_buffer);

    /**
     * @brief F.O.���N�G�X�g��O��Layout�A�j���[�V�����^�C�v�� 
     * @param disp �Ώۖ�
     * @param type ���C�A�E�g�^�C�v
     * @param is_keep_buffer �J�n���̃o�b�t�@��ۑ����邩�ǂ���
     */
    void RequestOutLayoutAnimationOfExternalData(DISP disp, OUTTYPE type, bool is_keep_buffer);
};

/**
 * @brief �又��
 * @note kujira�ł̓V�X�e���ŃR�[������̂ŁA�A�v���P�ʂŃR�[������K�v�͂Ȃ� 
 */
extern void Fade_Update(void);

/**
 * @brief F.O.���N�G�X�g 
 * @param disp �Ώۖ�
 * @param start_col �X�^�[�g���̉�ʐF
 * @param end_col �I�����̉�ʐF
 * @param sync F.O.�ɔ�₷����
 * @param is_keep_buffer �J�n���̃o�b�t�@��ۑ����邩�ǂ���
 */
extern void Fade_RequestOut(Fade::DISP disp, const math::VEC4* start_col, const math::VEC4* end_col, u32 sync = FADE_DEFAULT_SYNC, bool is_keep_buffer = false);

/**
 * @brief F.I.���N�G�X�g��^�C�v�w��Ł�
 * @param disp �Ώۖ�
 * @param type F.I.�^�C�v
 * @param sync F.I.�ɔ�₷����
 */
extern void Fade_RequestOut(Fade::DISP disp = Fade::DISP_DOUBLE, Fade::OUTTYPE type = Fade::OUTTYPE_CAPTURE, u32 sync = FADE_DEFAULT_SYNC, bool is_keep_buffer = false);

/**
 * @brief F.O.���N�G�X�g��O��Layout�A�j���[�V�����Ł�
 * @param lyt_res �O�����C�A�E�g���\�[�X
 * @param anmNum �A�j���[�V������
 * @param is_keep_buffer �J�n���̃o�b�t�@��ۑ����邩�ǂ���
 */
extern void Fade_RequestOut(const void* lyt_res, u8 anmNum, bool is_keep_buffer = false);

/**
 * @brief F.I.���N�G�X�g
 * @param disp �Ώۖ�
 * @param sync F.I.�ɔ�₷����
 */
extern void Fade_RequestIn(Fade::DISP disp = Fade::DISP_DOUBLE, u32 sync = FADE_DEFAULT_SYNC);

/**
 * @brief F.I.���N�G�X�g��^�C�v�w��Ł�
 * @param disp �Ώۖ�
 * @param type F.I.�^�C�v
 * @param sync F.I.�ɔ�₷����
 */
extern void Fade_RequestIn_Ex(Fade::DISP disp = Fade::DISP_DOUBLE, Fade::INTYPE type = Fade::INTYPE_BLACK, u32 sync = FADE_DEFAULT_SYNC);

/**
 * @brief �����I��F.O.��Ԃɂ���
 * @param disp �Ώۖ�
 * @param col �I�����̉�ʐF 
 */
extern void Fade_ForceOut(Fade::DISP disp, const math::VEC4* col);

/**
 * @brief �t�F�[�h���I���������ǂ������擾
 * @return �t�F�[�h���I���������ǂ��� 
 */
extern bool Fade_IsEnd(void);

/**
 * @brief �|�[�YON/OFF
 * @param is_pause �|�[�Y�t���O
 */
extern void Fade_SetPause(bool is_pause);

/**
 * @brief �|�[�Y��Ԏ擾
 * @retval �|�[�Y���
 */
extern bool Fade_IsPause(void);

/**
 * @brief �t�F�[�h��ԃ��Z�b�g
 * @param disp �Ώۖ�
 */
extern void Fade_Reset(Fade::DISP disp);

/**
 * @brief �t�F�[�h�����J�b�g�t���O���Z�b�g 
 * @param disp �Ώۖ�
 * @param isCutProcess �J�b�g���邩�ǂ���
 */
extern void Fade_SetIsCutProcess(Fade::DISP disp, bool isCutProcess);

/**
 * @brief �R�[���o�b�N�̃Z�b�g 
 * @memo ���C�A�E�g�V�X�e�����폜
 * @param pCallbackFunc �R�[���o�b�N�֐�
 * @param pWork �R�[���o�b�N���[�N
 */
extern void Fade_SetCallback(Fade::FadeCallbackFunc* func, void *work = NULL);

/**
 * @brief F.I.�ҋ@��Ԃ��ǂ������擾
 * @param �Ώۖ�
 */
extern bool Fade_IsFadeInStandbyState(Fade::DISP disp);

} // namespace grp
} // namespace gfl

#endif // __GFL_FADE_H__
