//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_GraphicsSystem.cpp
 *	@brief  �O���t�B�b�N�X�V�X�e��    �`����\�z�E�Ǘ�
 *	@author	tomoya takahashi
 *	@date		2010.10.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <nn.h>

#include <grp/gfl_GraphicsSystem.h>
#include <base/gfl_AreaMan.h>
#include <gfl_Math.h>
#include <gfl_Std.h>
#include <ro/gfl_RoManager.h>

//-----------------------------------------------------------------------------
/**
*    Singleton�C���X�^���X�̎��̂��`
*/
//-----------------------------------------------------------------------------
#if GFL_ENABLE_RO
SINGLETON_INSTANCE_DEFINITION(gfl::grp::GraphicsSystem);
template class gfl::base::SingletonAccessor<gfl::grp::GraphicsSystem>;
#endif


namespace gfl {
namespace grp {


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
const int ALIGNMENT_SYSTEM_BUFFER     = 4;
const int ALIGNMENT_VERTEX            = 4;
const int ALIGNMENT_TEXTURE           = 128;
const int ALIGNMENT_RENDER_BUFFER     = 64;
const int ALIGNMENT_DISPLAY_BUFFER    = 16;
const int ALIGNMENT_3D_COMMAND_BUFFER = 16;

enum ReadPixcelsState {
  READPIXCELS_REQ_NONE,
  READPIXCELS_REQ_ON_BLOCK,
  READPIXCELS_REQ_ON_LINER,
  READPIXCELS_REQ_WAIT,
};


// �������t���O
bool GraphicsSystem::m_Initialize = false;

// �Q�[���Ƃ��ė��̎����g�p���邩�H
bool GraphicsSystem::m_GameStereoFlag = true;



//-----------------------------------------------------------------------------
/**
 *					�f�t�H���g�������ݒ�
*/
//-----------------------------------------------------------------------------

const GraphicsSystemSetUp GraphicsSystemSetupDefault = {
  MODE_DOUBLE_BUFFER,                     // �o�b�t�@�����O���[�h
  true,                                  // �E�ڂ��g�p���邩�B
  false,                                 // ���̎�On��Ԃŏ��������邩�B

  // �f�B�X�v���C�o�b�t�@���
  {
    // DisplayBuffer UPPER
    {
      DISPLAY_UPPER,                      // �f�B�X�v���C�̎�ނł��B
      400,                                // �f�B�X�v���C�̉����ł��B
      240,                                // �f�B�X�v���C�̏c���ł��B
      TRANSFER_MODE_ANTIALIASE_NOT_USED,  // �e�B�X�v���C�o�b�t�@�̓]�����[�h�ł��B
      MEMORY_AREA_VRAMA,                  // �������m�ۃG���A�ł��B
      RENDER_COLOR_FORMAT_RGB8,          // �J���[�t�H�[�}�b�g�ł�
      false,                              // �]�����ɂw�����Ƀt���b�v���邩�ǂ�����\���܂��B
      0,                                  // �\�����s���n�_�w���W�ł��B
      0                                   // �\�����s���n�_�x���W�ł��B
    },
    // DisplayBuffer LOWER
    {
      DISPLAY_LOWER,                      // �f�B�X�v���C�̎�ނł��B
      320,                                // �f�B�X�v���C�̉����ł��B
      240,                                // �f�B�X�v���C�̏c���ł��B
      TRANSFER_MODE_ANTIALIASE_NOT_USED,  // �e�B�X�v���C�o�b�t�@�̓]�����[�h�ł��B
      MEMORY_AREA_VRAMB,                  // �������m�ۃG���A�ł��B
      RENDER_COLOR_FORMAT_RGB8,          // �J���[�t�H�[�}�b�g�ł�
      false,                              // �]�����ɂw�����Ƀt���b�v���邩�ǂ�����\���܂��B
      0,                                  // �\�����s���n�_�w���W�ł��B
      0                                   // �\�����s���n�_�x���W�ł��B
    },
    // DisplayBuffer UPPER_RIGHT
    {
      DISPLAY_UPPER_RIGHT,                // �f�B�X�v���C�̎�ނł��B
      400,                                // �f�B�X�v���C�̉����ł��B
      240,                                // �f�B�X�v���C�̏c���ł��B
      TRANSFER_MODE_ANTIALIASE_NOT_USED,  // �e�B�X�v���C�o�b�t�@�̓]�����[�h�ł��B
      MEMORY_AREA_VRAMA,                  // �������m�ۃG���A�ł��B
      RENDER_COLOR_FORMAT_RGB8,          // �J���[�t�H�[�}�b�g�ł�
      false,                              // �]�����ɂw�����Ƀt���b�v���邩�ǂ�����\���܂��B
      0,                                  // �\�����s���n�_�w���W�ł��B
      0                                   // �\�����s���n�_�x���W�ł��B
    },
  },

  // �t���[���o�b�t�@���
  // �t���[���o�b�t�@�𗬗p���邱�Ƃɂ��AVRAM�̈�������܂�
  // �g�킸�ɕ`�悪�\�ł��B
  {
    // FrameBuffer UPPER
    {
      DISPLAY_UPPER,                      // ���ʗp
      400,                                // �o�b�t�@�̉����ł��B
      240,                                // �o�b�t�@�̏c���ł��B
      RENDER_COLOR_FORMAT_RGBA8,          // �J���[�o�b�t�@�t�H�[�}�b�g
      RENDER_DEPTH_FORMAT_24_STENCIL8,    // �[�x�o�b�t�@�t�H�[�}�b�g
      MEMORY_AREA_VRAMA,                  // �`��Ώۂ̃J���[�o�b�t�@�̔z�u�ꏊ�ł��B
      MEMORY_AREA_VRAMB,                  // �`��Ώۂ̃f�v�X�o�b�t�@�̔z�u�ꏊ�ł��B
      DISPLAY_NONE,                       // ���̃t���[���o�b�t�@���I���W�i��
      0,                                  //Viewport���ł��B
      0,                                  //Viewport��ł��B
      400,                                //Viewport�E�ł��B
      240,                                //Viewport���ł��B
    },

    // FrameBuffer LOWER
    {
      DISPLAY_LOWER,                      // ����ʗp
      320,                                // �o�b�t�@�̉����ł��B
      240,                                // �o�b�t�@�̏c���ł��B
      RENDER_COLOR_FORMAT_RGBA8,          // �J���[�o�b�t�@�t�H�[�}�b�g
      RENDER_DEPTH_FORMAT_24_STENCIL8,    // �[�x�o�b�t�@�t�H�[�}�b�g
      MEMORY_AREA_NONE,                   // �`��Ώۂ̃J���[�o�b�t�@�̔z�u�ꏊ�ł��B
      MEMORY_AREA_NONE,                   // �`��Ώۂ̃f�v�X�o�b�t�@�̔z�u�ꏊ�ł��B
      DISPLAY_UPPER,                      // UPPER�̃t���[���o�b�t�@�𗬗p
      0,                                  //Viewport���ł��B
      0,                                  //Viewport��ł��B
      320,                                //Viewport�E�ł��B
      240,                                //Viewport���ł��B
    },

    // FrameBuffer UPPER_RIGHT
    {
      DISPLAY_UPPER_RIGHT,                // ����ʗp
      400,                                // �o�b�t�@�̉����ł��B
      240,                                // �o�b�t�@�̏c���ł��B
      RENDER_COLOR_FORMAT_RGBA8,          // �J���[�o�b�t�@�t�H�[�}�b�g
      RENDER_DEPTH_FORMAT_24_STENCIL8,    // �[�x�o�b�t�@�t�H�[�}�b�g
      MEMORY_AREA_NONE,                   // �`��Ώۂ̃J���[�o�b�t�@�̔z�u�ꏊ�ł��B
      MEMORY_AREA_NONE,                   // �`��Ώۂ̃f�v�X�o�b�t�@�̔z�u�ꏊ�ł��B
      DISPLAY_UPPER,                      // UPPER�̃t���[���o�b�t�@�𗬗p
      0,                                  //Viewport���ł��B
      0,                                  //Viewport��ł��B
      400,                                //Viewport�E�ł��B
      240,                                //Viewport���ł��B
    },
  },

  // �R�}���h���X�g�ݒ�
  {
    // MAIN
    {
      0x30000,                 // �o�b�t�@�T�C�Y�i�R�}���h�o�b�t�@�̑��T�C�Y�j
      48,                       // �L���[�C���O�R�}���h��
    },

    // UPPER
    {
      0x20000,                 // �o�b�t�@�T�C�Y�i�R�}���h�o�b�t�@�̑��T�C�Y�j
      16,                      // �L���[�C���O�R�}���h��
    },

    // LOWER
    {
      0x20000,                 // �o�b�t�@�T�C�Y�i�R�}���h�o�b�t�@�̑��T�C�Y�j
      16,                      // �L���[�C���O�R�}���h��
    },
  },

  // �Z�[�u�R�}���h���X�g�ݒ�
  {
    0x80000,                 // �o�b�t�@�T�C�Y�i�R�}���h�o�b�t�@�̑��T�C�Y�j
    16,                      // �L���[�C���O�R�}���h��
  },

  // �N���A�J���[
  {
    // ClearColor UPPER
    {
      0.20f, 0.20f, 0.20f, 1.0f, 1.0f, 0
    },

    // ClearColor LOWER
    {
      0.20f, 0.20f, 0.20f, 1.0f, 1.0f, 0
    },
  },
};


//-----------------------------------------------------------------------------
/**
 *					�������R���g���[���N���X��`
 *					*VRAM�������̊Ǘ��̂��߂ɁA���̃N���X�́A0xC60Byte�̑傫��������܂��B
*/
//-----------------------------------------------------------------------------
class MemoryControl
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(MemoryControl);

public:
  static const u32 AREAMAN_BLOCK_SIZE = 512;      // �G���A�}�l�[�W���@�u���b�N�T�C�Y (16x16 pixcel�@�� 16bit�J���[��size�j
  static const u32 AREAMAN_BLOCK_NUM  = 0x300000 / AREAMAN_BLOCK_SIZE; // �G���A�}�l�[�W���@�u���b�N��

  static const u32 VRAM_ALLOCK_BLOCK_NUM = 256;   // �G���A�}�l�[�W���@�A���P�[�g�\��

  enum ControlType
  {
    APPLICATION_TYPE = 0,     // �A�v���P�[�V��������^�C�v
    SYSTEM_TYPE,              // �V�X�e������^�C�v
  };

  enum AllocWay
  {
    ALLOC_TOP = 0,            // �������̏㑤����m��
    ALLOC_BOTTOM,             // �������̉�������m��
  };

public:
  // �R���X�g���N�^�E�f�X�g���N�^
  MemoryControl( heap::NwAllocator* deviceAllocator, u32 vramASize, u32 vramBSize );
  ~MemoryControl();

  // �A�v���P�[�V����������
  void InitializeApplication( heap::NwAllocator* deviceAllocator );
  void FinalizeApplication( void );
  bool IsInitializeApplication(void) const;

  // �������A���P�[�^
  void* Alloc( MemoryArea area, MemoryUseType type, u32 size );
  void Free( MemoryArea area, void* addr );

  //�@�R���g���[���^�C�v�ύX
  void SetControlType( ControlType type, AllocWay way );
  ControlType GetControlType(void) const;
  AllocWay GetAllocWay(void) const;

  // VRAM�c��T�C�Y�̎擾
  u32 GetVramAFreeSize( void ) const ;
  u32 GetVramBFreeSize( void ) const ;


private:

  ControlType           m_ControlType;  // ����^�C�v
  AllocWay              m_AllocWay;     // �m�ە���
  heap::NwAllocator*    m_UseDeviceAllocator;   // ����^�C�v�̃A���P�[�^
  u32                   m_UseVramAStartBlock;   // ����^�C�v��VRAM-A�@�m�ۃu���b�N�J�n�ʒu
  u32                   m_UseVramANumBlock;     // ����^�C�v��VRAM-A�@�m�ۃu���b�N��
  u32                   m_UseVramBStartBlock;   // ����^�C�v��VRAM-B�@�m�ۃu���b�N�J�n�ʒu
  u32                   m_UseVramBNumBlock;     // ����^�C�v��VRAM-B�@�m�ۃu���b�N��


  heap::NwAllocator*    m_SystemDeviceAllocator;  // �V�X�e�� �f�o�C�X�������A���P�[�^
  heap::NwAllocator*    m_ApplyDeviceAllocator;   // �A�v���@�f�o�C�X�������A���P�[�^


  gfl::base::AreaMan    m_AreaManVramA;
  gfl::base::AreaMan    m_AreaManVramB;

  gfl::base::AreaMan::ResultCode m_AllocTblVramA[VRAM_ALLOCK_BLOCK_NUM];
  gfl::base::AreaMan::ResultCode m_AllocTblVramB[VRAM_ALLOCK_BLOCK_NUM];

  // �V�X�e���������[���g�p����VRAM�T�C�Y
  u32                   m_SystemVramASize;
  u32                   m_SystemVramBSize;

#if GFL_DEBUG
  s32 m_countDevice;
  s32 m_countVramA;
  s32 m_countVramB;
#endif


private:
  
  gfl::base::AreaMan::ResultCode* GetClearAllocTbl( gfl::base::AreaMan::ResultCode* p_tbl ) const;
  gfl::base::AreaMan::ResultCode* GetAddrAllocTbl( gfl::base::AreaMan::ResultCode* p_tbl, uptr addr ) const;


  // �c�[���n�����o
  static u32 GetBlockAlign( u32 align );
  static u32 GetBlockSize( u32 size );

  // ControlType��Use��������ݒ�
  void SetUseMemory( ControlType control_type );
};

// ���Ԃ̐錾
MemoryControl* GraphicsSystem::m_memoryControl;

//-----------------------------------------------------------------------------
/**
 *					�o�b�t�@�R���g���[���N���X��`
*/
//-----------------------------------------------------------------------------
class BufferControl
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(BufferControl);

public:

  // �R���X�g���N�^�E�f�X�g���N�^
  BufferControl( heap::NwAllocator* heapAllocator );
  ~BufferControl();

  void Initialize( heap::NwAllocator* heapAllocator, const GraphicsSystemSetUp* setup, bool appUpperRightMode );
  void Finalize( void );

  // CretaeAppRightBuffer��DeleteAppRightBuffer�́A
  void CreateAppRightBuffer( heap::NwAllocator* deviceAllocator, const GraphicsSystemSetUp* setup, GraphicsSystem* p_graphics );
  void DeleteAppRightBufferReq( void );
  bool IsFinishDeleteAppRightBuffer( void ) const;
  bool IsCreateAppRightBuffer( void ) const;

  void StartRendering( DisplayMsk msk, const FrameBufferClearColor* clearColor, const FrameBufferClearFlag* clearFlag = NULL );
  void EndRendering( f32 parallaxDepthRange = gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_ONE );
  void EndRenderingEx( DisplayType first, DisplayType second, DisplayType third,
                       f32 parallaxDepthRange = gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_ONE );

  void SwapBuffer( void );
  void VSyncAfter( void );
  void VSyncAfterRunAsync( bool skip );

  DisplayBufferID GetCurrentDisplayBufferID( DisplayType display )const;
  FrameBufferID GetCurrentFrameBufferID( DisplayType display )const;
  RenderTarget* GetCurrentFrameBuffer( DisplayType display )const;
  CommandListID GetCurrentCommandListID( DisplayType display )const;
  CommandListID GetCurrentSaveCommandListID(void)const;

  bool BindCurrentCommandList( DisplayType display ) const;

  void StartCommandSave( void );
  void EndCommandSave( void );
  void ReuseCommandSave( void );

  void WaitDone( void );

  void RequestReadPixels( void* memoryTop, void* memoryTopRight, void* memoryBottom, u32 mode, RenderTarget::RenderBufferType type = RenderTarget::RENDERBUFFER_TYPE_COLOR );
  void RequestReadLinerPixels( void* memoryTop, void* memoryTopRight, void* memoryBottom, RenderTarget::RenderBufferType type = RenderTarget::RENDERBUFFER_TYPE_COLOR );
  void AddReadPixelsCommand( DisplayType display, DisplayType copy_display );
  void UpdateReadPixels( void );
  bool WaitReadPixels( void ) const;

  // �X���b�v�o�b�t�@�t���O�̎擾�ƃN���A
  bool IsSwapBuffer( void ) const { return m_swapBufferFlag; }
  void ClearSwapBufferFlag( void );

  // �V���O���o�b�t�@�t�H�[�X�t���O
  void ReqSingleBufferForce( bool flag );

  // ���̎��n�m�E�n�e�e���N�G�X�g
  void SetStereoModeReq( bool stereo_on );
  

  //�e�}�l�[�W���̎擾
  inline DisplayBufferManager* GetDisplayBufferManager(void)const { return m_displayBufferManager; }
  inline FrameBufferManager* GetFrameBufferManager(void)const { return m_frameBufferManager; }
  inline CommandListManager* GetCommandListManager(void)const { return m_commandListManager; }

  // �R�}���h���X�g���s��
  inline u32 GetDoneCount( void ) const { return m_doneCount; }


  inline bool IsRenderEnd( DisplayType type, f32 parallaxDepthRange )
  {
    if( type != gfl::grp::DISPLAY_UPPER_RIGHT ){ return true; }
    if( !m_appCreateRightBuffer ){ return false; }
    if( m_appRightBufferDeleteReq != DEL_REQ_NONE ){ return false; }
    if( parallaxDepthRange == gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_ZERO ){ return false; }
    return true;
  }

  inline bool IsRenderRightNone( DisplayType type, f32 parallaxDepthRange )
  {
    if( type != gfl::grp::DISPLAY_UPPER_RIGHT ){ return false; }
    if( !m_appCreateRightBuffer ){ return true; }
    if( m_appRightBufferDeleteReq != DEL_REQ_NONE ){ return true; }
    if( m_reqChangeStereoFlag ){ return true; }
    if( parallaxDepthRange == gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_ZERO ){ return true; }
    return false;
  }

#if GFL_DEBUG

  void StartGpuProfile( void );
  void StopGpuProfile( void );
  u32 GetGpuProfileResult( GpuProfileResult result ) const;
  const u32* GetGpuProfileResultPtr( GpuProfileResult result ) const;

  inline void SetGpuProfileFlag( bool flag ) { m_gpuProfileFlag = flag; }
  inline bool IsGpuProfileFlag( void ) const { return m_gpuProfileFlag; }

#endif // GFL_DEBUG

private:

  enum{
    DEL_REQ_NONE,
    DEL_REQ_RENDERING_OFF,
    DEL_REQ_RENDERING_END_WAIT,
    DEL_REQ_RENDERING_END,
  };

private:

  // double�o�b�t�@�����O���[�h�t���O
  b8                  m_doubleBufferFlag;
  // �X���b�v�o�b�t�@�t���O
  b8                  m_swapBufferFlag;

  // �����V���O���o�b�t�@�����O���[�h�t���O
  b8                  m_singleBufferForceFlag:2;
  b8                  m_reqSingleBufferForce:2;
  b8                  m_reqSingleBufferForceFlag:4;

  // �}�l�[�W��
  DisplayBufferManager* m_displayBufferManager;     // �f�B�X�v���C�o�b�t�@�}�l�[�W��
  FrameBufferManager*   m_frameBufferManager;       // �t���[���o�b�t�@�}�l�[�W��
  CommandListManager*   m_commandListManager;       // �R�}���h���X�g�}�l�[�W��

  // �V�X�e���ێ��o�b�t�@
  DisplayBufferID       m_displayBufferTbl[GRAPHICSSYSTEM_USE_DISPLAYBUFFER_NUM];
  FrameBufferID         m_frameBufferTbl[DISPLAY_BOTH_MAX]; // ���A���A�E�̃t���[���o�b�t�@ID
  CommandListID         m_commandListTbl[GRAPHICSSYSTEM_USE_COMMANDLIST_NUM];
  CommandListID         m_saveCommandListTbl[GRAPHICSSYSTEM_USE_SAVECOMMANDLIST_NUM];
  u32                   m_useDisplayBufferNum;
  u32                   m_useFrameBufferNum;
  u32                   m_useCommandListNum;
  u32                   m_useSaveCommandListNum;
  u32                   m_useDisplayNum;          // �p�ӂ���Ă���f�B�X�v���C�̐�
  b8                    m_useRightDisplay;        // �E�ڂ��g�p����Ƃ��ėp�ӂ���Ă���̂�
  b8                    m_appCreateRightBuffer;   // �E�ڗp�o�b�t�@���A�v���P�[�V�������쐬������Ԃ̂Ƃ�,TRUE
  u8                    m_appRightBufferDeleteReq;

  // �����_�����O�Ǘ�
  b8                    m_renderFlag;             // �����_�����O���Ԃ��Ǘ�����t���O
  DisplayMsk            m_renderMsk;              // �f�B�X�v���C�}�X�N

  // �X���b�v�o�b�t�@�Ǘ�
  u32                   m_swapDispCount;                      // �X���b�v�J�E���^
  u32                   m_swapCommandCount;                    // �X���b�v�J�E���^
  CommandListID         m_curentCommandList[COMMANDLIST_NUM]; // �J�����g�R�}���h���X�g
  CommandListID         m_curentSaveCommandList;              // �J�����g�Z�[�u�R�}���h���X�g
  DisplayBufferID       m_curentDisplay[DISPLAY_BOTH_MAX];    // �J�����g�f�B�X�v���C�o�b�t�@
  DisplayBufferID       m_rendEndDisplay[DISPLAY_BOTH_MAX];   // �����_�����O�I�������f�B�X�v���C�o�b�t�@
  DisplayBufferID       m_swapDisplay[DISPLAY_BOTH_MAX];      // �X���b�v�f�B�X�v���C�o�b�t�@
  CommandListID         m_swapCommandList;                    // �X���b�v�R�}���h���X�g

  // �R�}���h���X�g���s��
  u32                   m_doneCount;

  // StereoMode�ύXRequest
  bool  m_reqChangeStereoFlag;
  bool  m_reqChangeStereoMode;


  // ReadPixelsRequest
  u32                             m_reqReadPixcels;
  RenderTarget::RenderBufferType  m_reqReadPixcelsTypeTop;
  RenderTarget::RenderBufferType  m_reqReadPixcelsTypeTopRight;
  RenderTarget::RenderBufferType  m_reqReadPixcelsTypeBottom;
  void*                           m_reqReadPixcelsMemoryTop;
  void*                           m_reqReadPixcelsMemoryTopRight;
  void*                           m_reqReadPixcelsMemoryBottom;



#if GFL_DEBUG

  // �X���b�v�o�b�t�@�t���O
  bool m_gpuProfileFlag;
  bool m_gpuProfileRun;

  GLuint m_gpuProfileResultBusy[4];               // BUSY
  GLuint m_gpuProfileResultVertexShader[4][5];    // VertexShader
  GLuint m_gpuProfileResultPolygon[3];            // Polygon
  GLuint m_gpuProfileResultFragment;              // Fragment
  GLuint m_gpuProfileResultMemAccess[18];         // �������A�N�Z�X

  u32     m_gpuProfileResultWork[ GPU_PROFILE_RESULT_MAX ]; // �O��̌��ʊi�[��
  u32     m_gpuProfileResultLastWork[ GPU_PROFILE_RESULT_MAX ]; // �O��̎擾����

#endif // GFL_DEBUG

private:

  void SetCurrentSwapCountBuffer(void);
  void ClearCurrentCommandList( void );
  void SetSwapTargetRenderDisplay(void);
  void ClearSwapID(void);
  void ClearSwapCommandID(void);
  void ClearRenderEndID(void);
  void Swap(void);

  void DeleteAppRightBuffer( void );

  void createDisplayBuffer( gfl::heap::NwAllocator* heapAllocator, u32 index, bool double_buffer, const DisplayBufferSetUp* cp_setup );
  void deleteDisplayBuffer( u32 index );

  void createFrameBuffer( gfl::heap::NwAllocator* heapAllocator, const RenderBufferSetUp* cp_setup );
  void deleteFrameBuffer( u32 index );


  void createCommandList( gfl::heap::NwAllocator* heapAllocator, u32 index, bool double_buffer, const GraphicsSystemSetUp* setup );
  void deleteCommandList( u32 index );

};


//=============================================================================
/**
 *					�O���t�B�b�N�X�V�X�e������
*/
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *    @brief  �f�t�H���g�Z�b�g�A�b�v�f�[�^���g�p��������
 *
 *    @param  heapAllocator       �q�[�v�������A���P�[�^
 *    @param  deviceAllocator     �f�o�C�X�������A���P�[�^
 *    @param  systemVramASize      �V�X�e�����g�p����VRAM-A�T�C�Y
 *    @param  systemVramBSize      �V�X�e�����g�p����VRAM-B�T�C�Y
 */
//-----------------------------------------------------------------------------
GraphicsSystem::GraphicsSystem( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator, u32 systemVramASize, u32 systemVramBSize )
{
  Initialize( heapAllocator, deviceAllocator, systemVramASize, systemVramBSize, &GraphicsSystemSetupDefault, false );
}

//-----------------------------------------------------------------------------
/**
 *    @brief  �w��Z�b�g�A�b�v�f�[�^���g�p��������
 *
 *    @param  heapAllocator       �q�[�v�������A���P�[�^
 *    @param  deviceAllocator     �f�o�C�X�������A���P�[�^
 *    @param  setup               �Z�b�g�A�b�v���
 *    @param  systemVramASize      �V�X�e�����g�p����VRAM-A�T�C�Y
 *    @param  systemVramBSize      �V�X�e�����g�p����VRAM-B�T�C�Y
 */
//-----------------------------------------------------------------------------
GraphicsSystem::GraphicsSystem( heap::NwAllocator* allocator, heap::NwAllocator* deviceAllocator, const GraphicsSystemSetUp* setup, u32 systemVramASize, u32 systemVramBSize )
{
  Initialize( allocator, deviceAllocator, systemVramASize, systemVramBSize, setup, false );
}

//-----------------------------------------------------------------------------
/**
 *    @brief  �w��Z�b�g�A�b�v�f�[�^���g�p��������
 *
 *    @param  heapAllocator       �q�[�v�������A���P�[�^
 *    @param  deviceAllocator     �f�o�C�X�������A���P�[�^
 *    @param  setup               �Z�b�g�A�b�v���
 *    @param  appUpperRightMode   �E�ڂ̃o�b�t�@�𐶐�����B�@TRUE
 *    @param  systemVramASize      �V�X�e�����g�p����VRAM-A�T�C�Y
 *    @param  systemVramBSize      �V�X�e�����g�p����VRAM-B�T�C�Y
 */
//-----------------------------------------------------------------------------
GraphicsSystem::GraphicsSystem( heap::NwAllocator* allocator, heap::NwAllocator* deviceAllocator, const GraphicsSystemSetUp* setup, bool appUpperRightMode, u32 systemVramASize, u32 systemVramBSize )
{
  Initialize( allocator, deviceAllocator, systemVramASize, systemVramBSize, setup, appUpperRightMode );
}


//-----------------------------------------------------------------------------
/**
 *    @brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
GraphicsSystem::~GraphicsSystem()
{
  Finalize();
}


//-----------------------------------------------------------------------------
/**
 *    @brief  �O���t�B�b�N�X�V�X�e�������������݂��`�F�b�N
 *
 *    @retval true  �������ς�
 *    @retval false ��������
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::IsInitialize( void )
{
  return m_Initialize;
}


//-----------------------------------------------------------------------------
/**
 *    @brief  �Q�[���Ƃ��āA���̎����g�p����̂��̐ݒ�
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::SetGameStereoFlag( bool flag )
{
  m_GameStereoFlag = flag;
}

bool GraphicsSystem::IsGameStereoFlag( void )
{
  return m_GameStereoFlag;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ������
 *
 *	@param	allocator           �q�[�v�A���P�[�^
 *	@param	deviceAllocator     �f�o�C�X�A���P�[�^
 *	@param  systemVramASize     VRAM-A�T�C�Y
 *	@param  systemVramBSize     VRAM-B�T�C�Y
 *	@param	setup               �Z�b�g�A�b�v�f�[�^
 *	@param  appUpperRight       �E�ڃf�B�X�v���C�o�b�t�@�������I�Ɋm�ۂ��邩�H
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::Initialize( heap::NwAllocator* allocator, heap::NwAllocator* deviceAllocator, u32 systemVramASize, u32 systemVramBSize, const GraphicsSystemSetUp* setup, bool appUpperRight )
{
  heap::HeapBase* heap = allocator->GetHeapBase();

  if(m_memoryControl != NULL){
    GFL_ASSERT( m_memoryControl == NULL );
    Finalize();
  }

  // �A���P�[�^�̕ۑ�
  m_heapAllocator = allocator;
  m_deviceAllocator = deviceAllocator;

  // SetUp�f�[�^�ۑ�
  std::MemCopy( setup, &m_setup, sizeof(GraphicsSystemSetUp) );

  // �������}�l�[�W���̐���
  m_memoryControl = GFL_NEW(heap) MemoryControl( deviceAllocator, systemVramASize, systemVramBSize );

  // gx
  if (nngxInitialize(AllocateGraphicsMemory, DeallocateGraphicsMemory) == GL_FALSE)
  {
    // GxInitialize ���s
    GFL_ASSERT_STOP(0);
  }


  // �o�b�t�@�R���g���[���[����
  m_bufferControl = GFL_NEW(heap) BufferControl( allocator );

  // �o�b�t�@�R���g���[���[�̏������́A�������̏ォ����
  m_memoryControl->SetControlType( MemoryControl::SYSTEM_TYPE, MemoryControl::ALLOC_TOP );
  {
    m_bufferControl->Initialize( allocator, setup, appUpperRight );
  }
  m_memoryControl->SetControlType( MemoryControl::SYSTEM_TYPE, MemoryControl::ALLOC_TOP );


  // ���̎�ON
  if( setup->stereoOn ){
    nngxSetDisplayMode( NN_GX_DISPLAYMODE_STEREO );
  }else{
    nngxSetDisplayMode( NN_GX_DISPLAYMODE_NORMAL );
  }

  // ����������
  // VRAM�̑S�N���A���s���B�@�\�t�g�E�F�A���Z�b�g�΍�
  {
    gfl::grp::CommandListManager * comListMan = this->GetCommandListManager();
    gfl::grp::CommandListID comListID = this->GetCurrentCommandListID();
    {
      comListMan->Clear( comListID );
      comListMan->Bind( comListID );
      {
        // VRAM���N���A����R�}���h����
        nngxAddMemoryFillCommand( 
            reinterpret_cast<GLvoid*>(nngxGetVramStartAddr(NN_GX_MEM_VRAMA)), nngxGetVramSize(NN_GX_MEM_VRAMA), 0, 32,
            reinterpret_cast<GLvoid*>(nngxGetVramStartAddr(NN_GX_MEM_VRAMB)), nngxGetVramSize(NN_GX_MEM_VRAMB), 0, 32 );
        comListMan->RunAsync( comListID );
        comListMan->WaitDone();
        comListMan->Clear( comListID );
      }
    }
  }

  // VsyncWait
  nngxSwapBuffers(NN_GX_DISPLAY_BOTH);
  nngxWaitVSync(NN_GX_DISPLAY_BOTH);
  nngxStartLcdDisplay();



  //�@������
  m_Initialize = true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �폜����
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::Finalize( void )
{
  // m_memoryControl���Ō�ɔj�������̂ł���Ń`�F�b�N
  if( m_memoryControl != NULL ){

    //�@������
    m_Initialize = false;

    // �o�b�t�@�R���g���[�j��
    GFL_DELETE m_bufferControl;
    m_bufferControl = NULL;

    //VSyncWait
    nngxWaitVSync(NN_GX_DISPLAY_BOTH);

    // nngx�I��
    nngxFinalize();

    // �������}�l�[�W���j��
    GFL_DELETE m_memoryControl;
    m_memoryControl = NULL;


    m_heapAllocator = NULL;
    m_deviceAllocator = NULL;

    //GFL_PRINT( "nngxFinalize\n" );
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  ��ʍ\����ύX����B
 *
 *	@param	setup �ύX�����ʍ\���̃Z�b�g�A�b�v���
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::ChangeSetUp( const GraphicsSystemSetUp* setup, bool appUpperRight )
{
  MemoryControl::ControlType type = m_memoryControl->GetControlType();
  MemoryControl::AllocWay    way = m_memoryControl->GetAllocWay();

  // System�o�b�t�@����m��
  // �f�Љ��΍�Ƃ��āA������Ƃ�B
  m_memoryControl->SetControlType( MemoryControl::SYSTEM_TYPE, MemoryControl::ALLOC_BOTTOM );
  {
    // �������j��
    m_bufferControl->Finalize();

    m_heapAllocator->SetIsAllocLow( true );
    m_bufferControl->Initialize( m_heapAllocator, setup, appUpperRight );
    m_heapAllocator->SetIsAllocLow( false );
  }
  // �������R���g���[�������ɖ߂��B
  m_memoryControl->SetControlType( type, way );

  // SetUp�f�[�^�ۑ�
  std::MemCopy( setup, &m_setup, sizeof(GraphicsSystemSetUp) );

  // ���̎�ON
  if( setup->stereoOn ){
    nngxSetDisplayMode( NN_GX_DISPLAYMODE_STEREO );
  }else{
    nngxSetDisplayMode( NN_GX_DISPLAYMODE_NORMAL );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �A�v���P�[�V�����p�̃f�o�C�X�������AVRAM�Ǘ���������
 *
 *	@param	deviceAllocator           �A�v���P�[�V�����̃O���t�B�b�N�Ŏg�p����f�o�C�X�������̃A���P�[�^
 *
 *	VRAM�̈�ɂ́ASystemMemory�ŗ\�񂳂�Ă��镔���ȊO�̑S�Ă����蓖�Ă��܂��B
 *
 *	���̊֐����ĂԂƁA�A�v���P�[�V�����������ɐ؂�ւ��܂��B
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::InitializeApplicationMemory( heap::NwAllocator* deviceAllocator )
{
  m_memoryControl->InitializeApplication( deviceAllocator );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �A�v���P�[�V�����p�̃f�o�C�X�������AVRAM�Ǘ���j��
 *
 *	VRAM�ɉ���Y�ꂪ�����Assart�łƂ܂�܂��B
 *
 *	����ȍ~�ŁA�m�ۂ���郁�����́A�V�X�e������������m�ۂ���܂��B
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::FinalizeApplicationMemory( void )
{
  m_memoryControl->FinalizeApplication();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�v���P�[�V�������ŁA�g���������̎��o�b�t�@�����邩�`�F�b�N
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::IsInitializedAppUpperRightBuffer( void ) const
{
  return m_bufferControl->IsCreateAppRightBuffer();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�v���P�[�V�������ŁA�g���ŁA���̎����n�m�ɂ���B
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::InitializeAppUpperRightBuffer( heap::NwAllocator* deviceAllocator )
{
  // �o�t�@�R���g���[���Ɋg���̗��̎��o�b�t�@�𐶐�
  m_bufferControl->CreateAppRightBuffer( deviceAllocator, &m_setup, this );
  this->BindCurrentCommandList();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�v���P�[�V�������ŁA�g���Ŋm�ۂ����o�b�t�@���������B
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::ReqDeleteAppUpperRightBuffer( bool is_stereo_off )
{
  // �O�̂��߁A���̎����n�e�e��
  if( is_stereo_off ){
    SetStereoModeReq(false);
  }
  
  // ���N�G�X�g���s
  m_bufferControl->DeleteAppRightBufferReq();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�v���P�[�V�������ŁA�g���Ŋm�ۂ����o�b�t�@��������ꂽ�����`�F�b�N
 *	@return
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::IsDeleteAppUpperRightBuffer( void ) const
{
  return m_bufferControl->IsFinishDeleteAppRightBuffer();
}



//----------------------------------------------------------------------------
/**
 *	@brief  �V�X�e�����̃������m�ې���V�X�e���������ɐ؂�ւ���B
 *
 *	@param  mode    �V�X�e���������̃��[�h
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::BindSystemMemory( SystemMemoryMode mode )
{
  if( mode == SYSTEM_MEM_UNSETTLED ){

    // �ϓ��̂��郁�����m��
    m_memoryControl->SetControlType( MemoryControl::SYSTEM_TYPE, MemoryControl::ALLOC_BOTTOM );

  }else{

    // �ϓ��̂Ȃ��������m��
    m_memoryControl->SetControlType( MemoryControl::SYSTEM_TYPE, MemoryControl::ALLOC_TOP );

  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �V�X�e�����̃������m�ې���A�v���P�[�V�����������ɖ߂��B
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::UnBindSystemMemory( void )
{
  if( m_memoryControl->IsInitializeApplication() ){

    // �A�v���P�[�V�������������w��
    m_memoryControl->SetControlType( MemoryControl::APPLICATION_TYPE, MemoryControl::ALLOC_TOP );
  }else{

    // �ϓ��̂Ȃ��������Ƃ��ď�����
    m_memoryControl->SetControlType( MemoryControl::SYSTEM_TYPE, MemoryControl::ALLOC_TOP );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �������m�ې悪�V�X�e���������ɂȂ��Ă��邩�m�F����B
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::IsBindSystemMemory(void) const
{
  if( m_memoryControl->GetControlType() == MemoryControl::SYSTEM_TYPE ){
    return true;
  }
  return false;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���̎���ON/OFF�����N�G�X�g���܂��B ���ۂ̔��f�́AVSyncAfter�ɂȂ�܂��B
 *
 *	@param	stereo_on   ON:true   OFF:false
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::SetStereoModeReq( bool stereo_on )
{
  if( IsUseRightDisplay() ){
    m_bufferControl->SetStereoModeReq(stereo_on);
  }
}

#if GFL_DEBUG
//���̎����L���̎��������[�h���؂�ւ����Ȃ��̂Ńf�o�b�O�p�ɗp��
void GraphicsSystem::SetStereoModeReqDebug( bool stereo_on )
{
  m_bufferControl->SetStereoModeReq(stereo_on);
}
#endif

//----------------------------------------------------------------------------
/**
 *	@brief  �V���O���o�b�t�@�[�ɂ���t���O
 *
 *	@param	single_buffer_on   ON:true   OFF:false
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::SetSingleBufferReq( bool single_buffer_on )
{
  m_bufferControl->ReqSingleBufferForce( single_buffer_on );
}


//----------------------------------------------------------------------------
/**
 *	@brief    �ʏ탌���_�����O�J�n����
 *
 *	@param  msk   �`����s���ʐݒ�
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::StartRendering( DisplayMsk msk )
{
  m_bufferControl->StartRendering( msk, m_setup.clearColor, m_setup.clearFlag );
}


//----------------------------------------------------------------------------
/**
 *	@brief    �����_�����O�㏈��
 *
 *	@param    parallaxDepthRange  ���̎��̎����̕␳�l
 *	                               (gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_��
 *                                  gfl::grp::g3d::StereoCamera��GetDepthRange�֐��̖߂�l��n���ĉ������B)
 *
 *	���X���b�v����o�b�t�@ID�̕ۑ��Ȃǂ��s���܂��B
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::EndRendering( f32 parallaxDepthRange )
{
  m_bufferControl->EndRendering( parallaxDepthRange );
}



//----------------------------------------------------------------------------
/**
 *	@brief  �e��ʂ̃R�}���h�o�b�t�@�̎��s�����w��t���@�����_�����O�㏈��
 *
 *  @param  first     1�ԂɃR�}���h���s�������
 *  @param  second    2�ԂɃR�}���h���s�������
 *  @param  third     3�ԂɃR�}���h���s�������
 *	@param  parallaxDepthRange  ���̎��̎����̕␳�l
 *	                             (gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_��
 *                                gfl::grp::g3d::StereoCamera��GetDepthRange�֐��̖߂�l��n���ĉ������B)
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::EndRenderingEx( DisplayType first, DisplayType second, DisplayType third, f32 parallaxDepthRange )
{
  m_bufferControl->EndRenderingEx( first, second, third, parallaxDepthRange );
}


//----------------------------------------------------------------------------
/**
 *	@brief    �X���b�v�o�b�t�@�[����
 *
 *	���̊֐����s������A������VSyncWait���s���Ă��������B
 *
 *	���_�u���o�b�t�@�����O���[�h�̏ꍇ
 *	  �P�D�O�t���[���ŊJ�n�����̎��`�揈���̊�����҂��܂��B
 *	  �Q�D�O�̃t���[���ŕ`�悵���ʂ�LOD�ɔ��f���܂��B
 *
 *	���V���O���o�b�t�@�����O���[�h�̏ꍇ
 *	�@�P�D���t���[���Ő��������R�}���h�����s���܂��B
 *	�@�Q�D�`��̊�����҂��܂��B
 *	�@�R�D�`�挋�ʂ�LOD�ɔ��f���܂��B
 *
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::SwapBuffer( void )
{
  m_bufferControl->SwapBuffer();
#if GFL_DEBUG  
  //�`�悪���������̃t���[���Ƃ������ƂɂȂ�B
  //�O�t���[���̕`��C���X�^���X�͔j���ł���B
  ++s_DrawFrameCount;
#endif    
}


//----------------------------------------------------------------------------
/**
 *	@brief  �R�}���h���X�g�̎��s����������܂ő҂�
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::WaitDone( void )
{
  m_bufferControl->WaitDone();
}


//----------------------------------------------------------------------------
/**
 *	@brief    VSync�҂��@����
 *	@param  display   �f�B�X�v���C�^�C�v  �iVSync��҂�� DISPLAY_UPPER or DISPLAY_LOWER or DISPLAY_BOTH_MAX �j
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::WaitVSync( DisplayType display )
{
  static const u32 sc_NngxDisplay[ DISPLAY_BOTH_MAX+1 ] = {
    NN_GX_DISPLAY0,
    NN_GX_DISPLAY1,
    NN_GX_DISPLAY0,
    NN_GX_DISPLAY_BOTH,
  };

  if( display <= DISPLAY_BOTH_MAX )
  {

    nngxWaitVSync( sc_NngxDisplay[display] );
  }
  else
  {
    GFL_ASSERT(0);
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �X���b�v�o�b�t�@�@���N�G�X�g���o�Ă��邩�`�F�b�N
 *
 *	@retval true  �o�Ă���B    �iWaitVSync���Ă���VSyncAfter���ĂԕK�v������B�j
 *	@retval false �o�Ă��Ȃ��B
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::IsSwapBuffer( void ) const
{
  return m_bufferControl->IsSwapBuffer();
}

//----------------------------------------------------------------------------
/**
 *	@brief    VSync�㏈��
 *
 *	���̊֐����ĂԑO�ɕK��VSyncWait���s���Ă��������B
 *
 *	���_�u���o�b�t�@�����O���[�h�̏ꍇ
 *	  �R�D���t���[���Ő�������3D�R�}���h�����s���܂��B
 *	  �S�D�f�B�X�v���C�o�b�t�@�A�R�}���h���X�g�̏������ݐ��ύX���܂��B
 *
 *	���V���O���o�b�t�@�����O���[�h�̏ꍇ
 *	�@���ɂȂ�
 *
 *	@param  skip    ����̃R�}���h���s���X�L�b�v
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::VSyncAfter( bool skip )
{
  // �X���b�v�o�b�t�@���s��ꂽ�ꍇ�ɍs������
  if( m_bufferControl->IsSwapBuffer() ){

    // ���ۂ̃R�}���h���s�J�n����
    m_bufferControl->VSyncAfter();

    // �L���v�`����ɍs�������A�R�}���h���s
    m_bufferControl->VSyncAfterRunAsync( skip );

    // �X���b�v�o�b�t�@�t���O�̃N���A
    m_bufferControl->ClearSwapBufferFlag();
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�B�X�v���C�̃N���A���߂𔭍s����B
 *
 *	@param	display       �f�B�X�v���C
 *	@param  b_color       �J���[�̃N���A���s�����H(true:�s���j
 *	@param  clear_color   �N���A�J���[
 *	@param  b_depth       �f�v�X�̃N���A���s�����H(true:�s���j
 *	@param  clear_depth   �N���A�f�v�X�l
 *	@param  b_stencil     �X�e���V���̃N���A���s�����H(true:�s��)
 *	@param  clear_stencil �N���A�X�e���V���l
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::ClearBuffer( DisplayType display, b32 b_color, const gfl::grp::ColorF32& clear_color, b32 b_depth, f32 clear_depth, b32 b_stencil, u8 clear_stencil )
{
  RenderTarget* p_render = this->GetCurrentFrameBuffer( display );

  // �N���A�o�b�t�@���s���B
  if( this->BindCurrentCommandList(p_render) ){
    p_render->ClearBuffer( b_color, clear_color, b_depth, clear_depth, b_stencil, clear_stencil );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief    �V�X�e�������삵�Ă���J�����g�̃f�B�X�v���C�o�b�t�@ID���擾
 *
 *	@param    display   �ʎw��
 *
 *	@return   �J�����g�̃f�B�X�v���C�o�b�t�@ID
 */
//-----------------------------------------------------------------------------
DisplayBufferID GraphicsSystem::GetCurrentDisplayBufferID( DisplayType display )const
{
  return m_bufferControl->GetCurrentDisplayBufferID(display);
}

//----------------------------------------------------------------------------
/**
 *	@brief    �V�X�e�������삵�Ă���J�����g�̃t���[���o�b�t�@ID���擾
 *
 *	@param    display   �ʎw��
 *
 *	@return   �J�����g�̃t���[���o�b�t�@ID
 */
//-----------------------------------------------------------------------------
FrameBufferID GraphicsSystem::GetCurrentFrameBufferID( DisplayType display )const
{
  return m_bufferControl->GetCurrentFrameBufferID( display );
}

//----------------------------------------------------------------------------
/**
 *	@brief    �V�X�e�������삵�Ă���J�����g�̃t���[���o�b�t�@���擾
 *
 *	@param    display   �ʎw��
 *
 *	@return   �J�����g�̃t���[���o�b�t�@
 */
//-----------------------------------------------------------------------------
RenderTarget* GraphicsSystem::GetCurrentFrameBuffer( DisplayType display )const
{
  return m_bufferControl->GetCurrentFrameBuffer(display);
}


//----------------------------------------------------------------------------
/**
 *	@brief    �V�X�e�������삵�Ă���J�����g�̃R�}���h���X�gID���擾
 *
 *	@param  displayType �@�擾�������R�}���h���X�g�ɑΉ�������
 *
 *	@return   �J�����g�̃R�}���h���X�gID
 *
 *	displayType
 *	  DISPLAY_UPPER           ���� or ���ʗp�̃R�}���h���X�g
 *	  DISPLAY_UPPER_RIGHT     �E�ڗp�̃R�}���h���X�g
 *	  DISPLAY_LOWER           ����ʗp�̃R�}���h���X�g
 *	  DISPLAY_NONE            �I�t�X�N���[�������p�̃R�}���h���X�g�i�e�Ȃǁj
 */
//-----------------------------------------------------------------------------
CommandListID GraphicsSystem::GetCurrentCommandListID(DisplayType displayType)const
{
  return m_bufferControl->GetCurrentCommandListID(displayType);
}

//----------------------------------------------------------------------------
/**
 *	@brief    �V�X�e�������삵�Ă���J�����g�̃R�}���h���X�g���o�C���h����
 *
 *	@param	displayType �o�C���h�������R�}���h���X�g�ɑΉ�������
 *
 *	displayType
 *	  DISPLAY_UPPER           ���� or ���ʗp�̃R�}���h���X�g
 *	  DISPLAY_UPPER_RIGHT     �E�ڗp�̃R�}���h���X�g
 *	  DISPLAY_LOWER           ����ʗp�̃R�}���h���X�g
 *	  DISPLAY_NONE            �I�t�X�N���[�������p�̃R�}���h���X�g�i�e�Ȃǁj
 *
 *	@retval true  �R�}���h���X�g���o�C���h�ł����B
 *	@retval false �R�}���h���X�g���o�C���h�ł��Ȃ������I(���̎�����Ȃ��Ƃ���UPPER_RIGHT���w�肳�ꂽ�j
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::BindCurrentCommandList( DisplayType displayType )
{
  return m_bufferControl->BindCurrentCommandList( displayType );
}

//----------------------------------------------------------------------------
/**
 *	@brief    �����_�[�^�[�Q�b�g�Ɗ֘A�����R�}���h�o�b�t�@���o�C���h����
 *
 *	@param	renderTarget �@�`���ɂ����������_�[�^�[�Q�b�g
 *
 *	@retval true  �R�}���h���X�g���o�C���h�ł����B
 *	@retval false �R�}���h���X�g���o�C���h�ł��Ȃ������I(���̎�����Ȃ��Ƃ���UPPER_RIGHT���w�肳�ꂽ�j
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::BindCurrentCommandList( const gfl::grp::RenderTarget* renderTarget )
{
  if( renderTarget ){
    return m_bufferControl->BindCurrentCommandList( renderTarget->GetDisplayType() );
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *	@brief    �V�X�e�������삵�Ă���J�����g�̃Z�[�u�p�R�}���h���X�gID���擾
 *
 *	@return   �J�����g�̃Z�[�u�p�R�}���h���X�gID
 */
//-----------------------------------------------------------------------------
CommandListID GraphicsSystem::GetCurrentSaveCommandListID(void)const
{
  return m_bufferControl->GetCurrentSaveCommandListID();
}

//----------------------------------------------------------------------------
/**
 *	@brief    �Z�[�u�p�R�}���h���X�g�ւ̃R�}���h�~�ϊJ�n
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::StartCommandSave( void )
{
  m_bufferControl->StartCommandSave();
}

//----------------------------------------------------------------------------
/**
 *	@brief    �Z�[�u�p�R�}���h���X�g�ւ̃R�}���h�~�ϏI��
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::EndCommandSave( void )
{
  m_bufferControl->EndCommandSave();
}

//----------------------------------------------------------------------------
/**
 *	@brief    �Z�[�u�p�R�}���h���X�g�֒~�ς��ꂽ�ŐV�̓��e���J�����g�R�}���h���X�g�ɐݒ�
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::ReuseCommandSave( void )
{
  m_bufferControl->ReuseCommandSave();
}



//----------------------------------------------------------------------------
/**
 *	@brief    �f�t�H���g�N���A�J���[�̐ݒ�
 *
 *  @param  display   �ʎw��@�iDISPLAY_UPPER or DISPLAY_LOWER�j
 *	@param	red       R   0.0�`1.0
 *	@param	green     G   0.0�`1.0
 *	@param	blue      B   0.0�`1.0
 *	@param	alpha     A   0.0�`1.0
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::SetClearColor( DisplayType display, f32 red, f32 green, f32 blue, f32 alpha )
{
  if( display < DISPLAY_NORMAL_MAX )
  {
    m_setup.clearColor[display].red     = red;
    m_setup.clearColor[display].green   = green;
    m_setup.clearColor[display].blue    = blue;
    m_setup.clearColor[display].alpha   = alpha;
  }
  else
  {
    GFL_ASSERT(0);
  }
}

//-----------------------------------------------------------------------------
/**
  * @brief     �f�t�H���g�N���A�t���O�̐ݒ�
  * @param display     �ʎw��@�iDISPLAY_UPPER or DISPLAY_LOWER�j
  * @param color       �J���[�o�b�t�@�N���A  �L���@or ����
  * @param depth       �[�x�o�b�t�@�N���A  �L���@or ����
  * @param stencil     �X�e���V���o�b�t�@�N���A  �L���@or ����
  */
//-----------------------------------------------------------------------------
void GraphicsSystem::SetClearColorFlag( DisplayType display, b32 color, f32 depth, f32 stencil )
{
  if( display < DISPLAY_NORMAL_MAX )
  {
    m_setup.clearFlag[display].f_color     = color;
    m_setup.clearFlag[display].f_depth     = depth;
    m_setup.clearFlag[display].f_stencil   = stencil;
  }
  else
  {
    GFL_ASSERT(0);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    �f�B�X�v���C�o�b�t�@�}�l�[�W���̎擾
 */
//-----------------------------------------------------------------------------
DisplayBufferManager* GraphicsSystem::GetDisplayBufferManager( void )const
{
  return m_bufferControl->GetDisplayBufferManager();
}

//----------------------------------------------------------------------------
/**
 *	@brief    �t���[���o�b�t�@�}�l�[�W���̎擾
 */
//-----------------------------------------------------------------------------
FrameBufferManager* GraphicsSystem::GetFrameBufferManager( void )const
{
  return m_bufferControl->GetFrameBufferManager();
}

//----------------------------------------------------------------------------
/**
 *	@brief    �R�}���h���X�g�}�l�[�W���̎擾
 */
//-----------------------------------------------------------------------------
CommandListManager* GraphicsSystem::GetCommandListManager( void )const
{
  return m_bufferControl->GetCommandListManager();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �R�}���h���X�g���s�񐔁i��荞�ݒ��ӁI�j
 *
 *	@return �R�}���h���X�g���s��
 */
//-----------------------------------------------------------------------------
u32 GraphicsSystem::GetCommandListDoneCount( void ) const
{
  return m_bufferControl->GetDoneCount();
}

//----------------------------------------------------------------------------
/**
 *	@brief    ReadPixels�ɕK�v�ȃ������T�C�Y���擾����B
 */
//-----------------------------------------------------------------------------
u32 GraphicsSystem::GetReadPixelsMemorySize( RenderColorFormat colorFormat, u32 width, u32 height ) const
{
  u32 memorySize = 0;

  switch( colorFormat )
  {
  case RENDER_COLOR_FORMAT_NONE:
    memorySize = 0;
    break;
  case RENDER_COLOR_FORMAT_RGBA8:
    memorySize = 4;
    break;
  case RENDER_COLOR_FORMAT_RGB8:
    memorySize = 3;
    break;
  case RENDER_COLOR_FORMAT_RGBA4:
    memorySize = 2;
    break;
  case RENDER_COLOR_FORMAT_RGB5_A1:
    memorySize = 2;
    break;
  case RENDER_COLOR_FORMAT_RGB565:
    memorySize = 2;
    break;
  default:
    GFL_ASSERT_MSG( 0, "colorFormat does not exist" );
  }

  return ( width * height * memorySize );
}


//----------------------------------------------------------------------------
/**
 *	@brief    �J���[�o�b�t�@�̃s�N�Z�����̎擾Request
 *
 *	@param  memoryTop     ���ʍ��i�[��@�i�s�v�ȏꍇNULL�j
 *  @param  memoryTopRight���ʉE�i�[��@�i�s�v�ȏꍇNULL�j
 *	@param  memoryBottom  ����ʊi�[��    �i�s�v�ȏꍇNULL�j
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::RequestReadPixels( void* memoryTop, void* memoryTopRight, void* memoryBottom, RenderTarget::RenderBufferType type )

{
  m_bufferControl->RequestReadPixels( memoryTop, memoryTopRight, memoryBottom, READPIXCELS_REQ_ON_BLOCK, type );
}
// Up �� Bottom�݂̂̂Ƃ�
void GraphicsSystem::RequestReadPixels( void* memoryTop, void* memoryBottom, RenderTarget::RenderBufferType type )

{
  m_bufferControl->RequestReadPixels( memoryTop, NULL, memoryBottom, READPIXCELS_REQ_ON_BLOCK, type );
}

//���j�A���[�h
void GraphicsSystem::RequestReadLinerPixels( void* memoryTop, void* memoryTopRight, void* memoryBottom, RenderTarget::RenderBufferType type )

{
  m_bufferControl->RequestReadPixels( memoryTop, memoryTopRight, memoryBottom, READPIXCELS_REQ_ON_LINER, type );
}
// Up �� Bottom�݂̂̂Ƃ�
void GraphicsSystem::RequestReadLinerPixels( void* memoryTop, void* memoryBottom, RenderTarget::RenderBufferType type )

{
  m_bufferControl->RequestReadPixels( memoryTop, NULL, memoryBottom, READPIXCELS_REQ_ON_LINER, type );
}

//----------------------------------------------------------------------------
/**
 *	@brief    �J���[�o�b�t�@�̃s�N�Z�����̎擾�����҂�
 *	@retval true    ����
 *	@retval false   �r��
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::WaitReadPixels( void ) const
{
  return m_bufferControl->WaitReadPixels();
}



//----------------------------------------------------------------------------
/**
 *	@brief    VRAM-A�c��̈���擾
 */
//-----------------------------------------------------------------------------
u32 GraphicsSystem::GetVramAAllocatableSize( void ) const
{
  return m_memoryControl->GetVramAFreeSize();
}

//----------------------------------------------------------------------------
/**
 *	@brief    VRAM-B�c��̈���擾
 */
//-----------------------------------------------------------------------------
u32 GraphicsSystem::GetVramBAllocatableSize( void ) const
{
  return m_memoryControl->GetVramBFreeSize();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �������m��
 *
 *	@param	area  �������G���A
 *	@param	type  �g�p�^�C�v
 *	@param	size  �T�C�Y
 *
 *	@return ������
 */
//-----------------------------------------------------------------------------
void* GraphicsSystem::AllocMemory( MemoryArea area, MemoryUseType type, u32 size )
{
  return m_memoryControl->Alloc( area, type, size );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���������
 *
 *	@param	area        �G���A
 *	@param  p_memory    ������
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::FreeMemory( MemoryArea area, void* p_memory )
{
  if( m_memoryControl != NULL ){
    m_memoryControl->Free( area, p_memory );
  }
}


#if GFL_DEBUG
//----------------------------------------------------------------------------
/**
 *	@brief    �R�}���h���X�g�̏�Ԃ��_���v
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::DumpCommandList(void)const
{
  GFL_PRINT( "COMMANDLIST MAIN\n" );
  if( m_bufferControl->BindCurrentCommandList( DISPLAY_NONE ) ){
    m_bufferControl->GetCommandListManager()->Dump();
  }

  GFL_PRINT( "COMMANDLIST LEFT\n" );
  if( m_bufferControl->BindCurrentCommandList( DISPLAY_UPPER ) ){
    m_bufferControl->GetCommandListManager()->Dump();
  }

  GFL_PRINT( "COMMANDLIST RIGHT\n" );
  if( m_bufferControl->BindCurrentCommandList( DISPLAY_UPPER_RIGHT ) ){
    m_bufferControl->GetCommandListManager()->Dump();
  }

  GFL_PRINT( "COMMANDLIST LOWER\n" );
  if( m_bufferControl->BindCurrentCommandList( DISPLAY_LOWER ) ){
    m_bufferControl->GetCommandListManager()->Dump();
  }

  GFL_PRINT( "SAVE COMMANDLIST" );
  m_bufferControl->GetCommandListManager()->Bind( m_bufferControl->GetCurrentSaveCommandListID() );
  m_bufferControl->GetCommandListManager()->Dump();


  // ���ɖ߂��B
  m_bufferControl->BindCurrentCommandList( DISPLAY_NONE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  GpuProfile�t���O��ݒ�
 *
 *	@param	flag  �t���O
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::SetGpuProfileFlag( bool flag )
{
  m_bufferControl->SetGpuProfileFlag(flag);
}

//----------------------------------------------------------------------------
/**
 *	@brief  GpuProfile�t���O���擾
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::IsGpuProfileFlag( void ) const
{
  return m_bufferControl->IsGpuProfileFlag();
}

//----------------------------------------------------------------------------
/**
 *	@brief  1�O�̃����_�����O�ł�GPU�v���t�@�C�����O���ʎ擾
 *
 *	@param  GpuProfileResult
 *
 *	@return ���ʂ̐��l
 */
//-----------------------------------------------------------------------------
u32 GraphicsSystem::GetGpuProfileResult( GpuProfileResult result ) const
{
  return m_bufferControl->GetGpuProfileResult( result );
}

//----------------------------------------------------------------------------
/**
 *	@brief  1�O�̃����_�����O�ł�GPU�v���t�@�C�����O���ʎ擾
 *
 *	@param  GpuProfileResult
 *
 *	@return ���ʂ̐��l
 */
//-----------------------------------------------------------------------------
const u32* GraphicsSystem::GetGpuProfileResultPtr( GpuProfileResult result ) const
{
  return m_bufferControl->GetGpuProfileResultPtr( result );
}

#endif



//----------------------------------------------------------------------------
/**
 *	@brief  �O���t�B�b�N�X�������A���P�[�^
 *
 *	@param	area    �������G���A
 *	@param	aim     �g�p�p�r
 *	@param	id      ���g�p
 *	@param	size    �T�C�Y
 *
 *	@return �A�h���X
 */
//-----------------------------------------------------------------------------
void* GraphicsSystem::AllocateGraphicsMemory(GLenum area, GLenum aim, GLuint id, GLsizei size)
{
  GFL_UNUSED( id );

  if(m_memoryControl != NULL){
    return m_memoryControl->Alloc( static_cast<MemoryArea>(area), static_cast<MemoryUseType>(aim), static_cast<u32>(size) );
  }else{
    GFL_ASSERT( m_memoryControl != NULL ); //@check
    return NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �O���t�B�b�N�X�������f�A���P�[�^
 *
 *	@param	area    �������G���A
 *	@param	aim     �g�p�p�r
 *	@param	id      ���g�p
 *	@param	size    �T�C�Y
 */
//-----------------------------------------------------------------------------
void GraphicsSystem::DeallocateGraphicsMemory(GLenum area, GLenum aim, GLuint id, void* addr)
{
  GFL_UNUSED( id );
  GFL_UNUSED( aim );

  if( m_memoryControl != NULL ){
    m_memoryControl->Free( static_cast<MemoryArea>(area), addr );
  }else{
    GFL_ASSERT( m_memoryControl != NULL ); //@check
  }
}




// 
//----------------------------------------------------------------------------
/**
 *	@brief ���̎�������H
 */
//-----------------------------------------------------------------------------
bool GraphicsSystem::IsUseRightDisplay( void ) const
{
  if( (m_setup.useUpperRight || m_bufferControl->IsCreateAppRightBuffer()) && m_GameStereoFlag ){
    return true;
  }
  return false;
}

#if GFL_DEBUG  
u32   GraphicsSystem::s_DrawFrameCount = 0;

u32 GraphicsSystem::GetDrawFrameCount()
{
  return s_DrawFrameCount;
}
#endif

//=============================================================================
/**
 *					�o�b�t�@�R���g���[���N���X��`
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^
 *
 *	@param	heapAllocator           �q�[�v�A���P�[�^
 *	@param	setup                   �Z�b�g�A�b�v�f�[�^
 */
//-----------------------------------------------------------------------------
BufferControl::BufferControl(heap::NwAllocator* heapAllocator )
{

  // �e��}�l�[�W���쐬
  m_displayBufferManager  = GFL_NEW(heapAllocator->GetHeapBase()) DisplayBufferManager( heapAllocator, GRAPHICSSYSTEM_USE_DISPLAYBUFFER_NUM );
  m_frameBufferManager    = GFL_NEW(heapAllocator->GetHeapBase()) FrameBufferManager( heapAllocator, GRAPHICSSYSTEM_USE_FRAMEBUFFER_NUM );
  m_commandListManager    = GFL_NEW(heapAllocator->GetHeapBase()) CommandListManager( heapAllocator, GRAPHICSSYSTEM_TOTAL_COMMANDLIST_MAX );

  m_doneCount = 0;


#if GFL_DEBUG
  m_gpuProfileRun = false;
  m_gpuProfileFlag = false;
  for(u32 i=0; i<GPU_PROFILE_RESULT_MAX; ++i){
    m_gpuProfileResultWork[i] = 0;
    m_gpuProfileResultLastWork[i] = 0;
  }
#endif // GFL_DEBUG
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
BufferControl::~BufferControl()
{
  this->Finalize();

  // �e��o�b�t�@�j��
  GFL_DELETE m_commandListManager;
  m_commandListManager = NULL;
  GFL_DELETE m_frameBufferManager;
  m_frameBufferManager = NULL;
  GFL_DELETE m_displayBufferManager;
  m_displayBufferManager = NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �o�b�t�@�R���g���[���@������
 *
 *  @param  heapAllocator �q�[�v�A���P�[�^
 *	@param	setup         �Z�b�g�A�b�v
 */
//-----------------------------------------------------------------------------
void BufferControl::Initialize( heap::NwAllocator* heapAllocator, const GraphicsSystemSetUp* setup, bool appUpperRight )
{
  int i;


  // appUpperRight���[�h�̏ꍇ�B
  // m_useRightDisplay = false
  // m_useDisplayNum   = DISPLAY_BOTH_MAX
  // �E�ڃf�B�X�v���C�o�b�t�@�@=�@����
  // �E�ڃt���[���o�b�t�@      = �������Ȃ�
  // �E�ڃR�}���h�o�b�t�@      = �������Ȃ�

  // �E�ڎg�p���邩�H
  m_useRightDisplay = setup->useUpperRight;
  if( m_useRightDisplay || appUpperRight ){
    m_useDisplayNum = DISPLAY_BOTH_MAX;
  }else{
    m_useDisplayNum = DISPLAY_NORMAL_MAX;
  }

  m_singleBufferForceFlag = false;
  m_reqSingleBufferForce = false;

  // ���[�h�`�F�b�N
  if( setup->buffer_mode == MODE_DOUBLE_BUFFER ){
    m_doubleBufferFlag       = true;
    m_useDisplayBufferNum     = GRAPHICSSYSTEM_USE_DISPLAYBUFFER_NUM;
    m_useFrameBufferNum       = GRAPHICSSYSTEM_USE_FRAMEBUFFER_NUM;
    m_useCommandListNum       = GRAPHICSSYSTEM_USE_COMMANDLIST_NUM;
    m_useSaveCommandListNum   = GRAPHICSSYSTEM_USE_SAVECOMMANDLIST_NUM;
  }else{
    m_doubleBufferFlag       = false;
    m_useDisplayBufferNum     = GRAPHICSSYSTEM_USE_DISPLAYBUFFER_NUM/2;
    m_useFrameBufferNum       = GRAPHICSSYSTEM_USE_FRAMEBUFFER_NUM;
    m_useCommandListNum       = GRAPHICSSYSTEM_USE_COMMANDLIST_NUM/2;
    m_useSaveCommandListNum   = GRAPHICSSYSTEM_USE_SAVECOMMANDLIST_NUM/2;
  }


  // �f�B�X�v���C�o�b�t�@�m��
  for( i=0; i<DISPLAY_BOTH_MAX; i++ ){

    // �����̕K�v������o�b�t�@���`�F�b�N
    if( m_useDisplayNum <= i ){

      m_displayBufferTbl[i] = DISPLAYBUFFER_ID_ERROR;
      m_displayBufferTbl[i+DISPLAY_BOTH_MAX] = DISPLAYBUFFER_ID_ERROR;

    }else{

      this->createDisplayBuffer( heapAllocator, i, m_doubleBufferFlag, &setup->displaySetup[i] );
    }
  }

  // �e��ʗp�̃t���[���o�b�t�@�̊m�ۃ��[�N�̏�����
  for( i=0; i<GRAPHICSSYSTEM_USE_FRAMEBUFFER_NUM; i++ ){
    m_frameBufferTbl[i] = FRAMEBUFFER_ID_ERROR;
  }

  // �e��ʗp�̃t���[���o�b�t�@�̊m��
  for( i=0; i<GRAPHICSSYSTEM_USE_FRAMEBUFFER_NUM; i++ ){
    // DisplayType�͈̔̓`�F�b�N & �d���`�F�b�N
    GFL_ASSERT_STOP( setup->frameSetup[i].targetDisplay < DISPLAY_BOTH_MAX );
    GFL_ASSERT_STOP( m_frameBufferTbl[setup->frameSetup[i].targetDisplay] == FRAMEBUFFER_ID_ERROR );

    // �E�ڗp���K�v���H
    if( (m_useRightDisplay == false) && (setup->frameSetup[i].targetDisplay == DISPLAY_UPPER_RIGHT) ){
      continue;
    }
    this->createFrameBuffer( heapAllocator, &setup->frameSetup[i] );
  }

  // �R�}���h���X�g
  {
    for( i=0; i<GRAPHICSSYSTEM_USE_COMMANDLIST_NUM/2; i++ ){

      // �E�ڗp���K�v���H
      if( (m_useRightDisplay == false) && (i == COMMANDLIST_RIGHT) ){
        m_commandListTbl[i] = COMMANDLIST_ID_ERROR;
        m_commandListTbl[i + (GRAPHICSSYSTEM_USE_COMMANDLIST_NUM/2)] = COMMANDLIST_ID_ERROR;
        TOMOYA_PRINT( "GraphicsSystem right commandbuffer not create\n" );
        continue;
      }
      this->createCommandList( heapAllocator, i, m_doubleBufferFlag, setup );
    }
  }

  // �Z�[�u�p�R�}���h���X�g
  for( i=0; i<GRAPHICSSYSTEM_USE_SAVECOMMANDLIST_NUM; i++ ){
    if( i<m_useSaveCommandListNum ){
      m_saveCommandListTbl[i] = m_commandListManager->CreateBuffer( setup->saveCommandSetup );
    }else{
      m_saveCommandListTbl[i] = COMMANDLIST_ID_ERROR;
    }
  }


  // �����_�����O�ݒ�̏�����
  {
    m_renderFlag          = false;
  }

  // �X���b�v�o�b�t�@�Ǘ�
  {
    m_swapDispCount = 0;
    m_swapCommandCount = 0;

    // �܂����������Ă��Ȃ��̂ŏ���̓X���b�v�ł��Ȃ��B
    for( i=0; i<DISPLAY_BOTH_MAX; i++ ){
      m_curentDisplay[i] = DISPLAYBUFFER_ID_ERROR;
    }
    ClearSwapID();
    ClearRenderEndID();

    // swapCount�ɑΉ������o�b�t�@ID���J�����g�ɐݒ�
    SetCurrentSwapCountBuffer();
    ClearCurrentCommandList();

    // �V�R�}���h���X�g�̃o�C���h
    m_commandListManager->Bind( m_curentCommandList[COMMANDLIST_MAIN] );

    m_swapBufferFlag = false;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�b�t�@�R���g���[���@�j��
 */
//-----------------------------------------------------------------------------
void BufferControl::Finalize( void )
{
  // �m�ۂ����A�e�o�b�t�@��j��
  // �Z�[�u�R�}���h���X�g�@�j��
  for( u32 i=0; i<GRAPHICSSYSTEM_USE_SAVECOMMANDLIST_NUM; ++i ){
    if(m_saveCommandListTbl[i] != COMMANDLIST_ID_ERROR){
      m_commandListManager->DeleteBuffer( m_saveCommandListTbl[i] );
      m_saveCommandListTbl[i] = COMMANDLIST_ID_ERROR;
    }
  }

  // �R�}���h���X�g�@�j��
  for( u32 i=0; i<GRAPHICSSYSTEM_USE_COMMANDLIST_NUM/2; ++i ){
    this->deleteCommandList(i);
  }

  // �t���[���o�b�t�@�@�j��
  for( u32 i=0; i<DISPLAY_BOTH_MAX; ++i ){
    this->deleteFrameBuffer(i);
  }

  // �f�B�X�v���C�o�b�t�@�@�j��
  for( u32 i=0; i<GRAPHICSSYSTEM_USE_DISPLAYBUFFER_NUM; ++i ){
    this->deleteDisplayBuffer(i);
  }


  // �����o�ϐ��̃N���A
  m_singleBufferForceFlag = false;
  m_reqSingleBufferForce  = false;
  m_doubleBufferFlag      = false;
  m_swapBufferFlag        = false;
  m_useDisplayBufferNum   = 0;
  m_useFrameBufferNum     = 0;
  m_useCommandListNum     = 0;
  m_useSaveCommandListNum = 0;
  m_useDisplayNum         = 0;          // �p�ӂ���Ă���f�B�X�v���C�̐�
  m_useRightDisplay       = false;      // �E�ڂ��g�p����Ƃ��ėp�ӂ���Ă���̂�
  m_appCreateRightBuffer  = false;      // �E�ڗp�o�b�t�@���A�v���P�[�V�������쐬������Ԃ̂Ƃ�,TRUE
  m_renderFlag            = 0;          // �����_�����O���Ԃ��Ǘ�����t���O
  m_renderMsk             = static_cast<gfl::grp::DisplayMsk>(0);          // �f�B�X�v���C�}�X�N
  m_swapCommandCount      = 0;          // �X���b�v�J�E���^
  m_swapDispCount         = 0;


  for( u32 i=0; i<COMMANDLIST_NUM; ++i ){
    m_curentCommandList[i] = COMMANDLIST_ID_ERROR;
  }
  m_curentSaveCommandList = 0;

  for( u32 i=0; i<DISPLAY_BOTH_MAX; ++i )
  {
    m_curentDisplay[i]   = DISPLAYBUFFER_ID_ERROR;
    m_rendEndDisplay[i]  = DISPLAYBUFFER_ID_ERROR;
    m_swapDisplay[i]     = DISPLAYBUFFER_ID_ERROR;
  }
  m_swapCommandList = 0;                    // �X���b�v�R�}���h���X�g

}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�v���P�[�V�����ɂ��A�ォ��̗��̎��o�b�t�@�m��
 */
//-----------------------------------------------------------------------------
void BufferControl::CreateAppRightBuffer( heap::NwAllocator* deviceAllocator, const GraphicsSystemSetUp* setup, GraphicsSystem* p_graphics )
{
  // ���łɉE�ڃo�b�t�@������ꍇ�́A�������Ȃ��B
  if( m_useRightDisplay ){
    return ;
  }

  // �E�ڗp�o�t�@�𐶐����A�E�ڃ����_�����O���J�n����B
  m_useRightDisplay = true;
  // appUpperRight�p�̃R���X�g���N�^�Ő������Ă��Ȃ��B
  GFL_ASSERT_STOP( m_useDisplayNum == DISPLAY_BOTH_MAX );

  // �t���[���o�b�t�@�̊m��
  this->createFrameBuffer( deviceAllocator, &setup->frameSetup[DISPLAY_UPPER_RIGHT] );

  // �R�}���h���X�g
  this->createCommandList( deviceAllocator, DISPLAY_UPPER_RIGHT, m_doubleBufferFlag, setup );

  // �A�v���P�[�V�����ɂ���āA�E�ڃo�b�t�@���������ꂽ�B
  m_appCreateRightBuffer = true;

  
  // ���̎��p�̃o�b�t�@��ݒ肷��B
  this->SetCurrentSwapCountBuffer();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �E�ڂ̃o�b�t�@�j�����N�G�X�g
 */
//-----------------------------------------------------------------------------
void BufferControl::DeleteAppRightBufferReq( void )
{
  // �j�����̍ۃ��N�G�X�g�̏ꍇ�A��������B
  if( m_appRightBufferDeleteReq != DEL_REQ_NONE ){
    return ;
  }

  // ���������A�o�b�t�@���m�ۂ���Ă��Ȃ��ꍇ�́A��������B
  if( !m_appCreateRightBuffer ){
    return ;
  }
  
  m_appRightBufferDeleteReq = DEL_REQ_RENDERING_OFF;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �E�ڂ̃o�b�t�@�j�������܂�
 *
 *	@retval true    ����
 *	@retval false   �r��
 */
//-----------------------------------------------------------------------------
bool BufferControl::IsFinishDeleteAppRightBuffer( void ) const
{
  return (m_appRightBufferDeleteReq == DEL_REQ_NONE);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �E�ڂ̃o�b�t�@����������Ă��邩�`�F�b�N
 *
 *	@retval true
 *	@retval false
 */
//-----------------------------------------------------------------------------
bool BufferControl::IsCreateAppRightBuffer( void ) const
{
  return m_appCreateRightBuffer;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�v���P�[�V�����ɂ��A�ォ��̗��̎��o�b�t�@�̉��
 */
//-----------------------------------------------------------------------------
void BufferControl::DeleteAppRightBuffer( void )
{
  // �m�ۂ��ꂽ�o�b�t�@�����邩�`�F�b�N
  if( m_appCreateRightBuffer == false ){
    // �m�ۂ��ĂȂ��I
    return ;
  }
  m_appCreateRightBuffer = false;

  // �R�}���h���X�g
  this->deleteCommandList( DISPLAY_UPPER_RIGHT );

  // �t���[���o�b�t�@
  this->deleteFrameBuffer( DISPLAY_UPPER_RIGHT );

  // �E�ڗp�o�t�@���ɂ��Ă����B
  m_useRightDisplay = false;

  // �J�����g�̃o�b�t�@���X�V
  this->SetCurrentSwapCountBuffer();
}


//----------------------------------------------------------------------------
/**
 *	@brief  �����_�����O�J�n
 *
 *	@param  msk         �f�B�X�v���C�}�X�N
 *  @param  ClearColor �N���A�J���[
 */
//-----------------------------------------------------------------------------
void BufferControl::StartRendering( DisplayMsk msk, const FrameBufferClearColor* clearColor, const FrameBufferClearFlag* clearFlag )
{
  int i;
  if(m_renderFlag){
    GFL_ASSERT( m_renderFlag == false ); //@check
    EndRendering();
  }

  // �e��ʌ����A�t���[���o�b�t�@�N���A�������i�[
  for( i=0; i<DISPLAY_BOTH_MAX; ++i ){
    int index;
    if( msk & (1<<i) ){
      if( (i==DISPLAY_UPPER) || (i==DISPLAY_UPPER_RIGHT) ){
        index = DISPLAY_UPPER;
      }else{
        index = DISPLAY_LOWER;
      }
      if( BindCurrentCommandList( static_cast<gfl::grp::DisplayType>(i) ) ){  // �e��ʗp�̃R�}���h���X�g���o�C���h
        if ( clearFlag )
        {
          m_frameBufferManager->BindAndClear( m_frameBufferTbl[i], clearColor[index], clearFlag[index] );
        }
        else
        {
          FrameBufferClearFlag    allEnableFlag;
          m_frameBufferManager->BindAndClear( m_frameBufferTbl[i], clearColor[index], allEnableFlag );
        }
      }
    }
  }

  // �R�}���h���X�g�o�C���h
  BindCurrentCommandList( DISPLAY_NONE );  // �e��ʗp�̃R�}���h���X�g���o�C���h

  m_renderFlag = true;
  m_renderMsk  = msk;
}



//----------------------------------------------------------------------------
/**
 *	@brief  �����_�����O�@�㏈��
 *
 *	@param    parallaxDepthRange  ���̎��̎����̕␳�l
 *	                               (gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_��
 *                                  gfl::grp::g3d::StereoCamera��GetDepthRange�֐��̖߂�l��n���ĉ������B)
 */
//-----------------------------------------------------------------------------
void BufferControl::EndRendering( f32 parallaxDepthRange )
{
  EndRenderingEx( DISPLAY_UPPER, DISPLAY_UPPER_RIGHT, DISPLAY_LOWER, parallaxDepthRange );
}


//----------------------------------------------------------------------------
/**
 *	@brief    �e�f�B�X�v���C�o�b�t�@�ɓ]������t���[���o�b�t�@���w�肵�������_�����O�㏈��
 *
 *  @param  first     1�ԂɃR�}���h���s�������
 *  @param  second    2�ԂɃR�}���h���s�������
 *  @param  third     3�ԂɃR�}���h���s�������
 *  @param  parallaxDepthRange  ���̎��̎����̕␳�l
 *	                             (gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_��
 *                                gfl::grp::g3d::StereoCamera��GetDepthRange�֐��̖߂�l��n���ĉ������B)
 */
//-----------------------------------------------------------------------------
void BufferControl::EndRenderingEx( DisplayType first, DisplayType second, DisplayType third, f32 parallaxDepthRange )
{
  int i;

  if( m_renderFlag == false ) {
    return ;
  }


  // �J�����g�t���[���̏����J�����g�f�B�X�v���C�o�b�t�@�ɓ]�����鏈����
  // ���ꂼ��̃R�}���h���X�g�Ɋi�[
  for( i=0; i<m_useDisplayNum; ++i ){

    if( m_renderMsk & (1<<i) ){   // �����_�����O���s�����ʂ��`�F�b�N

      // �E�ڃo�b�t�@�j�����N�G�X�g���́A�E�ڂւ̃����_�����O�����́A�s��Ȃ��B
      if( IsRenderEnd(static_cast<gfl::grp::DisplayType>(i), parallaxDepthRange) ){

        if( BindCurrentCommandList( static_cast<gfl::grp::DisplayType>(i) ) ){
          const RenderTarget* renderTarget = m_frameBufferManager->GetRenderTarget( m_frameBufferTbl[i] );
          nngxFlush3DCommand();

          // �L���v�`�������s
          AddReadPixelsCommand( static_cast<gfl::grp::DisplayType>(i), static_cast<gfl::grp::DisplayType>(i) );

          // SBTS[2120] GFSBTS[933]
          // ���� ���ڕ`��̏ꍇ�ŁA�E�ڃ����_�����O�����Ȃ���Ԃł���΁A
          // �E�ڗp�o�b�t�@�ɍ��ډ摜���L���v�`��
          if( ( m_useDisplayNum > gfl::grp::DISPLAY_NORMAL_MAX ) // ���̎�����̉�ʐ�
              && ( i == gfl::grp::DISPLAY_UPPER ) // ���ډ摜
              && ( !IsRenderEnd(gfl::grp::DISPLAY_UPPER_RIGHT, parallaxDepthRange) )  // �E�ڃ����_�����O�Ȃ�
              )
          {
            // �E�ڂɍ��ډ摜���L���v�`��
            AddReadPixelsCommand( gfl::grp::DISPLAY_UPPER_RIGHT, static_cast<gfl::grp::DisplayType>(i) );
          }


          // �`��ʗp�̃R�}���h�o�b�t�@���o�C���h
          m_displayBufferManager->TransferRenderImage( m_curentDisplay[i], *renderTarget );

          /*
          //----------
          // ���̎��Ȃ��̂Ƃ��A�㍶��ʂ̃R�}���h�o�b�t�@�ɏ�E��ʂւ̓]���R�}���h��ǉ�����
          if(    ( m_useDisplayNum > gfl::grp::DISPLAY_NORMAL_MAX )
              && ( i == gfl::grp::DISPLAY_UPPER )
              && ( parallaxDepthRange == gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_ZERO ) 
              && ( m_appRightBufferDeleteReq == DEL_REQ_NONE ) )
          {
            // �E�ڂł��A���ڂ̃f�B�X�v���C�o�b�t�@���g���ă����_�����O
            // �X���b�v�o�b�t�@�ΏہB
            m_rendEndDisplay[gfl::grp::DISPLAY_UPPER_RIGHT] = m_curentDisplay[i];
            
            //AddReadPixelsCommand( gfl::grp::DISPLAY_UPPER_RIGHT, static_cast<gfl::grp::DisplayType>(i) );
            //m_displayBufferManager->TransferRenderImage( m_curentDisplay[gfl::grp::DISPLAY_UPPER_RIGHT], *renderTarget );
          }
          */
          // �X���b�v�o�b�t�@�ΏہB
          m_rendEndDisplay[i] = m_curentDisplay[i];
        }
      }

      // �E�ڃ����_�����O�������́A���ڂ̉�ʂ��o���B
      if( IsRenderRightNone(static_cast<gfl::grp::DisplayType>(i), parallaxDepthRange) ){
        // �E�ڂ̕`�悪�Ȃ��Ƃ��́A���ڂ̃f�B�X�v���C��ݒ�
        // �X���b�v�o�b�t�@�ΏہB
        m_rendEndDisplay[i] = m_curentDisplay[gfl::grp::DISPLAY_UPPER];
      }
    }
  }

  // Main�R�}���h�ɓ���
  // �܂��A�f�B�X�v���C�o�b�t�@�̓��e�𔽉f�R�}���h���~�ς���邪�A
  // ����́AMain�R�}���h�ɓ����B
  {
    DisplayType commandSchedule[ 3 ] = { first, second, third };

    // MAIN�@CommandList���o�C���h
    BindCurrentCommandList( DISPLAY_NONE );

    for( i=0; i<3; ++i ){
      if( m_renderMsk & (1<<commandSchedule[i]) ){


        // �o�b�t�@�����邩�`�F�b�N
        if( GetCurrentCommandListID( commandSchedule[i] ) == COMMANDLIST_ID_ERROR ){
          continue;
        }

        // �E�ڃo�b�t�@�j�����N�G�X�g���́A�E�ڂւ̃����_�����O�����́A�s��Ȃ��B
        if( IsRenderEnd(static_cast<gfl::grp::DisplayType>(commandSchedule[i]), parallaxDepthRange) ){
#if GFL_DEBUG
          {
            u32 restbuf, restreq;
            // �R�}���h���X�g�̎c��̈�`�F�b�N
            m_commandListManager->GetBufferStatus( GetCurrentCommandListID( commandSchedule[i] ), NULL, NULL, &restbuf, &restreq );

            if( restbuf < 0x1000 ){
              GFL_PRINT( "warning display %d commandList buffsize less 0x%x\n", commandSchedule[i], restbuf );
            }
            if( restreq < 8 ){
  //            GFL_PRINT( "warning display %d commandList reqcount less %d\n", commandSchedule[i], restreq ); // ���X�Əo�����ăe�X�g�ł��Ȃ��̂�
            }

          }
#endif
          {
            m_commandListManager->AddCommand( GetCurrentCommandListID( commandSchedule[i] ), false );
          }
        }
      }
    }
  }

  // ���̃X���b�v�f�B�X�v���C�Ɏw��
  // ���̎��s�R�}���hID�Ɏw��
  m_swapCommandList = GetCurrentCommandListID( DISPLAY_NONE );


  // �E�ڃo�b�t�@�j�������@DEL_REQ_RENDERING_OFF�@���@DEL_REQ_RENDERING_END_WAIT
  if( m_appRightBufferDeleteReq == DEL_REQ_RENDERING_OFF ){ m_appRightBufferDeleteReq = DEL_REQ_RENDERING_END_WAIT; }

#if GFL_DEBUG
  {
    u32 restbuf, restreq;
    // �R�}���h���X�g�̎c��̈�`�F�b�N
    m_commandListManager->GetBufferStatus( m_swapCommandList, NULL, NULL, &restbuf, &restreq );

    if( restbuf < 0x1000 ){
      GFL_PRINT( "warning MAIN commandList buffsize less 0x%x\n", restbuf );
    }
    if( restreq < 8 ){
      GFL_PRINT( "warning MAIN commandList reqcount less %d\n", restreq );
    }


    // �R�}���h���X�g�̎c��̈�`�F�b�N
    m_commandListManager->GetBufferStatus( m_curentSaveCommandList, NULL, NULL, &restbuf, &restreq );

    if( restbuf < 0x1000 ){
      GFL_PRINT( "warning SAVE commandList buffsize less 0x%x\n", restbuf );
    }
    if( restreq < 8 ){
      GFL_PRINT( "warning SAVE commandList reqcount less %d\n", restreq );
    }

  }
#endif

  // �L���b�V���t���b�V�����s���܂��B
  //
  // �t���b�V�����K�v�Ȃ̂́A�R�}���h�o�b�t�@�̗̈��
  // h3d::MaterialData::MakeCommandAndCache �������Ă���R�}���h�L���b�V���̗̈�ł����A
  // �����ȗ̈�ɕ����ĕ�����t���b�V����������A
  // �f�o�C�X�������S�̂ɑ΂��� 1 ��ɂ܂Ƃ߂ăt���b�V���������������ł��B
  nngxUpdateBuffer( reinterpret_cast<void*>(nn::os::GetDeviceMemoryAddress()),  nn::os::GetDeviceMemorySize() );
  
  // �����_�����O����
  m_renderFlag = false;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �R�}���h���X�g�̎��s����������܂ő҂�
 */
//-----------------------------------------------------------------------------
void BufferControl::WaitDone( void )
{
  m_commandListManager->WaitDone();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X���b�v�o�b�t�@
 *
   *	���̊֐����s������A������VSyncWait���s���Ă��������B
   *
   *	���_�u���o�b�t�@�����O���[�h�̏ꍇ
   *	  �P�D�O�t���[���ŊJ�n�����̎��`�揈���̊�����҂��܂��B
   *	  �Q�D�O�̃t���[���ŕ`�悵���ʂ�LOD�ɔ��f���܂��B
   *
   *	���V���O���o�b�t�@�����O���[�h�̏ꍇ
   *	�@�P�D���t���[���Ő��������R�}���h�����s���܂��B
   *	�@�Q�D�`��̊�����҂��܂��B
   *	�@�R�D�`�挋�ʂ�LOD�ɔ��f���܂��B
 */
//-----------------------------------------------------------------------------
void BufferControl::SwapBuffer( void )
{
  // �����V���O���o�b�t�@�؂�ւ�
  if( m_reqSingleBufferForce ){
    m_singleBufferForceFlag = m_reqSingleBufferForceFlag;
    m_reqSingleBufferForce = false;
  }
  
  // �_�u���o�b�t�@
  if( m_doubleBufferFlag && !m_singleBufferForceFlag ){

    // ���łɑ����Ă���R�}���h������̂ł���ΏI���܂ł܂�
    if( m_commandListManager->IsRuning() ){
      CommandListID runID = m_commandListManager->GetRuningCommandID();

      // �R�}���h�v������
      m_commandListManager->WaitDone();
    }

    // �S�o�b�t�@���X���b�v
    Swap();

    // ���t���[���`�抮���������̂��X���b�v�f�B�X�v���C�o�b�t�@�Ɋi�[
    SetSwapTargetRenderDisplay();
  }
  // �V���O���o�b�t�@
  else{
    // �����_�����O�I���f�B�X�v���C�����̏�ŃX���b�v����B
    SetSwapTargetRenderDisplay();

    // �R�}���h���s
    if(m_swapCommandList != COMMANDLIST_ID_ERROR){
      m_commandListManager->RunAsync( m_swapCommandList );
      GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM_HIGH, "RunCommand %d\n", m_swapCommandList );
      // �R�}���h�v������
      m_commandListManager->WaitDone();
    }

    // �S�o�b�t�@���X���b�v
    Swap();
  }

  m_swapBufferFlag = true;

  // �R�}���h���X�g���s��
  m_doneCount = m_doneCount + 1;

  // �E�ڃo�b�t�@�j�������@DEL_REQ_RENDERING_END_WAIT�@���@DEL_REQ_RENDERING_END
  if( m_appRightBufferDeleteReq == DEL_REQ_RENDERING_END_WAIT ){ m_appRightBufferDeleteReq = DEL_REQ_RENDERING_END; }

  /* ����ȍ~�ÂÂ��ׂ�����������܂��B�@VSync��@���Ȃ炸�AVSyncAfter�֐����ĂԂ��� */
}



//----------------------------------------------------------------------------
/**
 *	@brief  VSyncAfter���ԏ���
 *	���̊֐����ĂԑO�ɕK��VSyncWait���s���Ă��������B
 *
 *	���_�u���o�b�t�@�����O���[�h�̏ꍇ
 *	  �R�D���t���[���Ő�������3D�R�}���h�����s���܂��B
 *	  �S�D�f�B�X�v���C�o�b�t�@�A�R�}���h���X�g�̏������ݐ��ύX���܂��B
 *
 *	���V���O���o�b�t�@�����O���[�h�̏ꍇ
 *	�@���ɂȂ�
 */
//-----------------------------------------------------------------------------
void BufferControl::VSyncAfter( void )
{
  // �A�v���P�[�V���������������A�o�b�t�@�̔j�������B
  if( m_appRightBufferDeleteReq == DEL_REQ_RENDERING_END ){
    DeleteAppRightBuffer();
    m_appRightBufferDeleteReq = DEL_REQ_NONE;
  }
  

  //
  if( m_doubleBufferFlag && !m_singleBufferForceFlag ){
    m_swapCommandCount = 1-m_swapCommandCount;
    m_swapDispCount    = 1-m_swapDispCount;
  }

  // �X���b�v�o�b�t�@�J�E���g�ɂ������o�b�t�@�h�c��ݒ�
  SetCurrentSwapCountBuffer();
  ClearCurrentCommandList();

  // �V�R�}���h���X�g�̃o�C���h
  m_commandListManager->Bind( m_curentCommandList[COMMANDLIST_MAIN] );


  // �L���v�`�������̏�ԊǗ�
  // �R�}���h�����s����Ȃ��ƁA�L���v�`���������������ƂɂȂ�Ȃ��B
  // �����ŁA������҂K�v������B
  UpdateReadPixels();


  // StereoMode�ύX
  if( m_reqChangeStereoFlag ){
    if( m_reqChangeStereoMode ){
      nngxSetDisplayMode( NN_GX_DISPLAYMODE_STEREO );
    }else{
      nngxSetDisplayMode( NN_GX_DISPLAYMODE_NORMAL );
    }
    m_reqChangeStereoFlag = false;
  }

#if GFL_DEBUG
  // GPUProfile���s
  this->StopGpuProfile();
  this->StartGpuProfile();
#endif // GFL_DEBUG
}


//----------------------------------------------------------------------------
/**
 *	@brief  VSyncAfter���ԏ���
 *	���̊֐����ĂԑO�ɕK��VSyncWait���s���Ă��������B
 */
//-----------------------------------------------------------------------------
void BufferControl::VSyncAfterRunAsync( bool skip )
{
  // �_�u���o�b�t�@ ���s�J�n
  if( m_doubleBufferFlag && !m_singleBufferForceFlag ){

    // �R�}���h���s
    if(m_swapCommandList != COMMANDLIST_ID_ERROR){
      if( skip == false ){
        m_commandListManager->RunAsync( m_swapCommandList );
      }
      GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM_HIGH, "RunCommand %d\n", m_swapCommandList );
    }
  }


  // ���ꂼ��swap����o�b�t�@�̂h�c���N���A
  ClearSwapCommandID();
}



//----------------------------------------------------------------------------
/**
 *	@brief  �J�����g�̃f�B�X�v���C�o�b�t�@�h�c���擾
 *
 *	@param	display   �f�B�X�v���C�^�C�v
 *
 *	@return �J�����g�̃f�B�X�v���C�o�b�t�@�h�c
 */
//-----------------------------------------------------------------------------
DisplayBufferID BufferControl::GetCurrentDisplayBufferID( DisplayType display )const
{
  if(display >= DISPLAY_BOTH_MAX){
    GFL_ASSERT( display < DISPLAY_BOTH_MAX ); //@check
    return DISPLAYBUFFER_ID_ERROR;
  }

  if( (display == DISPLAY_UPPER_RIGHT) && (m_useDisplayNum <= display) ){
    // ���̃f�B�X�v���C�͗p�ӂ���Ă��܂���B
    GFL_ASSERT( 0 );
    return DISPLAYBUFFER_ID_ERROR;
  }

  return m_curentDisplay[ display ];
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����g�̃t���[���o�b�t�@�h�c���擾
 *
 *	@param	display   �f�B�X�v���C�^�C�v
 *
 *	@return �J�����g�̃t���[���o�b�t�@�h�c
 */
//-----------------------------------------------------------------------------
FrameBufferID BufferControl::GetCurrentFrameBufferID( DisplayType display )const
{
  if(display >= GRAPHICSSYSTEM_USE_FRAMEBUFFER_NUM){
    GFL_ASSERT( 0 );
    return FRAMEBUFFER_ID_ERROR;
  }
  return m_frameBufferTbl[display];
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����g�̃t���[���o�b�t�@���擾
 *
 *	@param	display   �f�B�X�v���C
 *
 *	@return �J�����g�̃t���[���o�b�t�@
 */
//-----------------------------------------------------------------------------
RenderTarget* BufferControl::GetCurrentFrameBuffer( DisplayType display ) const
{
  if(display >= GRAPHICSSYSTEM_USE_FRAMEBUFFER_NUM){
    GFL_ASSERT( 0 );
    return NULL;
  }
  return m_frameBufferManager->GetRenderTarget( m_frameBufferTbl[display] );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����g�̃R�}���h���X�g�h�c���擾
 *
 *	@return �R�}���h���X�g�h�c
 */
//-----------------------------------------------------------------------------
CommandListID BufferControl::GetCurrentCommandListID(DisplayType displayType)const
{
  // NONE�Ȃ�Main�̃R�}���h���X�g��Ԃ��B
  if( displayType == DISPLAY_NONE ){
    return m_curentCommandList[COMMANDLIST_MAIN];
  }

  // �f�B�X�v���C�^�C�v�̏������i�[����R�}���h���X�gID��Ԃ��B
  static const CommandListType DisplayToCommandList[ DISPLAY_BOTH_MAX ] ={
    COMMANDLIST_LEFT,
    COMMANDLIST_BOTTOM,
    COMMANDLIST_RIGHT,
  };

  if( displayType < DISPLAY_BOTH_MAX )
  {
    return m_curentCommandList[ DisplayToCommandList[displayType] ];
  }
  else
  {
    GFL_ASSERT(0);
    return m_curentCommandList[ DisplayToCommandList[0] ];
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �J�����g�̃Z�[�u�p�R�}���h���X�gID���擾
 *
 *	@return �Z�[�u�p�R�}���h���X�gID
 */
//-----------------------------------------------------------------------------
CommandListID BufferControl::GetCurrentSaveCommandListID(void)const
{
  return m_curentSaveCommandList;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �f�B�X�v���C�^�C�v�̃R�}���h���X�g���o�C���h����B
 *
 *	@param	display �f�B�X�v���C�^�C�v
 */
//-----------------------------------------------------------------------------
bool BufferControl::BindCurrentCommandList( DisplayType display ) const
{
  CommandListID commandID = GetCurrentCommandListID( display );

  if( commandID != COMMANDLIST_ID_ERROR ){
    m_commandListManager->Bind( commandID );
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �R�}���h�Z�[�u�J�n
 */
//-----------------------------------------------------------------------------
void BufferControl::StartCommandSave( void )
{
  m_commandListManager->StartCommandSave( m_curentSaveCommandList );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �R�}���h�Z�[�u�I��
 */
//-----------------------------------------------------------------------------
void BufferControl::EndCommandSave( void )
{
  m_commandListManager->EndCommandSave();
}

//----------------------------------------------------------------------------
/**
 *	@brief    �Z�[�u�p�R�}���h���X�g�֒~�ς��ꂽ�ŐV�̓��e���J�����g�R�}���h���X�g�ɐݒ�
 */
//-----------------------------------------------------------------------------
void BufferControl::ReuseCommandSave( void )
{
  m_commandListManager->ReuseCommandSave( m_curentSaveCommandList, false );
}


//----------------------------------------------------------------------------
/**
 *	@brief    �J���[�o�b�t�@�̃s�N�Z�����̎擾Request
 *
 *	@param  memoryTop     �㍶��ʊi�[��@�i�s�v�ȏꍇNULL�j
 *	@param  memoryTopRight��E��ʊi�[��@�i�s�v�ȏꍇNULL�j
 *	@param  memoryBottom  ����ʊi�[��  �i�s�v�ȏꍇNULL�j
 */
//-----------------------------------------------------------------------------
void BufferControl::RequestReadPixels( void* memoryTop, void* memoryTopRight, void* memoryBottom, u32 mode, RenderTarget::RenderBufferType type )
{
  m_reqReadPixcels              = mode;

  if( memoryTop ){
    m_reqReadPixcelsMemoryTop         = memoryTop;
    m_reqReadPixcelsTypeTop           = type;
  }

  if( memoryTopRight ){
    m_reqReadPixcelsMemoryTopRight    = memoryTopRight;
    m_reqReadPixcelsTypeTopRight      = type;
  }

  if( memoryBottom ){
    m_reqReadPixcelsMemoryBottom      = memoryBottom;
    m_reqReadPixcelsTypeBottom        = type;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    �s�N�Z���A�E�g�v�b�g�R�}���h��ǉ�
 *
 *	@param	display   �`���
 */
//-----------------------------------------------------------------------------
void BufferControl::AddReadPixelsCommand( DisplayType display, DisplayType copy_display )
{
  if( m_reqReadPixcels == READPIXCELS_REQ_ON_BLOCK ){

    if( ((display== DISPLAY_UPPER)) && m_reqReadPixcelsMemoryTop ){
      if( GetCurrentFrameBufferID(copy_display) != FRAMEBUFFER_ID_ERROR ){
        m_frameBufferManager->OutputImage( GetCurrentFrameBufferID(copy_display), m_reqReadPixcelsTypeTop, m_reqReadPixcelsMemoryTop );
      }
    }
    else if( ((display== DISPLAY_UPPER_RIGHT)) && m_reqReadPixcelsMemoryTopRight ){

      if( GetCurrentFrameBufferID(copy_display) != FRAMEBUFFER_ID_ERROR ){
        m_frameBufferManager->OutputImage( GetCurrentFrameBufferID(copy_display), m_reqReadPixcelsTypeTopRight, m_reqReadPixcelsMemoryTopRight );
      }
    }
    else if( ((display== DISPLAY_LOWER)) && m_reqReadPixcelsMemoryBottom ){
      if( GetCurrentFrameBufferID(copy_display) != FRAMEBUFFER_ID_ERROR ){
        m_frameBufferManager->OutputImage( GetCurrentFrameBufferID(copy_display), m_reqReadPixcelsTypeBottom, m_reqReadPixcelsMemoryBottom );
      }
    }

  }
  else if( m_reqReadPixcels == READPIXCELS_REQ_ON_LINER )
  {
    if( ((display== DISPLAY_UPPER)) && m_reqReadPixcelsMemoryTop ){
      if( GetCurrentFrameBufferID(copy_display) != FRAMEBUFFER_ID_ERROR ){
        m_frameBufferManager->OutputB2LImage( GetCurrentFrameBufferID(copy_display), m_reqReadPixcelsTypeTop, m_reqReadPixcelsMemoryTop );
      }
    }
    else if( ((display== DISPLAY_UPPER_RIGHT)) && m_reqReadPixcelsMemoryTopRight ){
      if( GetCurrentFrameBufferID(copy_display) != FRAMEBUFFER_ID_ERROR ){
        m_frameBufferManager->OutputB2LImage( GetCurrentFrameBufferID(copy_display), m_reqReadPixcelsTypeTopRight, m_reqReadPixcelsMemoryTopRight );
      }
    }
    else if( ((display== DISPLAY_LOWER)) && m_reqReadPixcelsMemoryBottom ){
      if( GetCurrentFrameBufferID(copy_display) != FRAMEBUFFER_ID_ERROR ){
        m_frameBufferManager->OutputB2LImage( GetCurrentFrameBufferID(copy_display), m_reqReadPixcelsTypeBottom, m_reqReadPixcelsMemoryBottom );
      }
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    �s�N�Z���A�E�g�v�b�g�@���[�N�Ǘ�
 */
//-----------------------------------------------------------------------------
void BufferControl::UpdateReadPixels( void )
{
  //�L���v�`��Request�𔭍s�́A�����@�i�@EndRendering�Ŕ��s�͍s���B�@�j
  if( (m_reqReadPixcels == READPIXCELS_REQ_ON_BLOCK) || (m_reqReadPixcels == READPIXCELS_REQ_ON_LINER) ){
    m_reqReadPixcels = READPIXCELS_REQ_WAIT;
    m_reqReadPixcelsMemoryTop     = NULL;
    m_reqReadPixcelsMemoryBottom  = NULL;
  }
  //�P�t���[����L���v�`�������҂���Ԃ�����
  else if( m_reqReadPixcels == READPIXCELS_REQ_WAIT ){
    m_reqReadPixcels = READPIXCELS_REQ_NONE;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    �J���[�o�b�t�@�̃s�N�Z�����̎擾�����҂�
 *	@retval true    ����
 *	@retval false   �r��
 */
//-----------------------------------------------------------------------------
bool BufferControl::WaitReadPixels( void ) const
{
  if( m_reqReadPixcels != READPIXCELS_REQ_NONE ){
    return false;
  }
  return true;
}



//----------------------------------------------------------------------------
/**
 *	@brief    �X���b�v�o�b�t�@�t���O�̃N���A
 */
//-----------------------------------------------------------------------------
void BufferControl::ClearSwapBufferFlag( void )
{
  m_swapBufferFlag = false;
}

// �V���O���o�b�t�@�t�H�[�X�t���O
void BufferControl::ReqSingleBufferForce( bool flag )
{
  m_reqSingleBufferForce = true;
  m_reqSingleBufferForceFlag = flag;
}

// ���̎��n�m�E�n�e�e���N�G�X�g
void BufferControl::SetStereoModeReq( bool stereo_on )
{
  m_reqChangeStereoFlag = true;
  m_reqChangeStereoMode = stereo_on;
}

#if GFL_DEBUG

//----------------------------------------------------------------------------
/**
 *	@brief  GPUProfile�@�J�n
 */
//-----------------------------------------------------------------------------
void BufferControl::StartGpuProfile( void )
{
  if( m_gpuProfileFlag == false ){
    return ;
  }
  if( m_gpuProfileRun == false ){

    nngxSetProfilingParameter( NN_GX_PROFILING_BUSY_SAMPLING_TIME_MICRO_SECOND, GPU_PROFILE_BUSY_MICRO_SECOND );
    nngxSetProfilingParameter( NN_GX_PROFILING_BUSY_SAMPLING_COUNT, 0 );
    nngxStartProfiling( NN_GX_PROFILING_BUSY );

    m_gpuProfileRun = true;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  GPUProfile�@�I��
 */
//-----------------------------------------------------------------------------
void BufferControl::StopGpuProfile( void )
{
  if( m_gpuProfileRun == false ){
    return ;
  }

  nngxStopProfiling( NN_GX_PROFILING_BUSY );

  m_gpuProfileRun = false;

  // �S�p�����[�^�擾
  {
    nngxGetProfilingResult( NN_GX_PROFILING_BUSY, m_gpuProfileResultBusy );
    nngxGetProfilingResult( NN_GX_PROFILING_VERTEX_SHADER0, m_gpuProfileResultVertexShader[0] );
    nngxGetProfilingResult( NN_GX_PROFILING_VERTEX_SHADER1, m_gpuProfileResultVertexShader[1] );
    nngxGetProfilingResult( NN_GX_PROFILING_VERTEX_SHADER2, m_gpuProfileResultVertexShader[2] );
    nngxGetProfilingResult( NN_GX_PROFILING_VERTEX_SHADER3, m_gpuProfileResultVertexShader[3] );
    nngxGetProfilingResult( NN_GX_PROFILING_POLYGON, m_gpuProfileResultPolygon );
    nngxGetProfilingResult( NN_GX_PROFILING_FRAGMENT, &m_gpuProfileResultFragment );
    nngxGetProfilingResult( NN_GX_PROFILING_MEMORY_ACCESS, m_gpuProfileResultMemAccess );
  }

  // ���̂܂܊i�[���Ă悢���́B
  {
    // BUSY
    for( u32 i=0; i<4; ++i ){
      m_gpuProfileResultWork[ GPU_PROFILE_BUSY_SHADER0 + (i*2) + 0 ] = m_gpuProfileResultBusy[i] >> 16;
      m_gpuProfileResultWork[ GPU_PROFILE_BUSY_SHADER0 + (i*2) + 1 ] = m_gpuProfileResultBusy[i] & 0xffff;
    }
  }

  // �O��ƍ���̍������i�[���镨�B
  {
    // VertexShader
    for( u32 i=0; i<4; ++i ){
      for( u32 j=0; j<5; ++j ){
        m_gpuProfileResultWork[ GPU_PROFILE_VERTEX0_PROGCOUNTER + (i*5) + j ] = m_gpuProfileResultVertexShader[i][j] - m_gpuProfileResultLastWork[ GPU_PROFILE_VERTEX0_PROGCOUNTER + (i*5) + j ];
        m_gpuProfileResultLastWork[ GPU_PROFILE_VERTEX0_PROGCOUNTER + (i*5) + j ] = m_gpuProfileResultVertexShader[i][j];
      }
    }

    // Polygon
    for( u32 i=0; i<3; ++i ){
      m_gpuProfileResultWork[GPU_PROFILE_POLYGON_INPUT_VERTEX + i] = m_gpuProfileResultPolygon[i] - m_gpuProfileResultLastWork[GPU_PROFILE_POLYGON_INPUT_VERTEX + i];
      m_gpuProfileResultLastWork[GPU_PROFILE_POLYGON_INPUT_VERTEX + i] = m_gpuProfileResultPolygon[i];
    }

    // Fragment
    m_gpuProfileResultWork[GPU_PROFILE_FRAGMENT_INPUT_FRAGMENT] = m_gpuProfileResultFragment - m_gpuProfileResultLastWork[GPU_PROFILE_FRAGMENT_INPUT_FRAGMENT];
    m_gpuProfileResultLastWork[GPU_PROFILE_FRAGMENT_INPUT_FRAGMENT] = m_gpuProfileResultFragment;

    // MemoryAccess
    for( u32 i=0; i<18; ++i ){
      m_gpuProfileResultWork[GPU_PROFILE_ACCESS_VRAMA_READ + i] = (m_gpuProfileResultMemAccess[i]*16) - (m_gpuProfileResultLastWork[GPU_PROFILE_ACCESS_VRAMA_READ + i] * 16);
      m_gpuProfileResultLastWork[GPU_PROFILE_ACCESS_VRAMA_READ + i] = m_gpuProfileResultMemAccess[i];
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  GPUProfile�@���ʎ擾
 *
 *	@param	result  RESULT�^�C�v
 *
 *	@return �O��̃����_�����O����
 */
//-----------------------------------------------------------------------------
u32 BufferControl::GetGpuProfileResult( GpuProfileResult result ) const
{
  if( result < GPU_PROFILE_RESULT_MAX ){
    return m_gpuProfileResultWork[result];
  }
  GFL_ASSERT( result < GPU_PROFILE_RESULT_MAX ); //@check
  return 0;
}


//----------------------------------------------------------------------------
/**
 *	@brief  1�O�̃����_�����O�ł�GPU�v���t�@�C�����O���ʎ擾
 *
 *	@param  GpuProfileResult
 *
 *	@return ���ʂ̐��l
 *  @lim    �J�E���^���I�[�o�[�t���[�����u�ԁ@�Ԉ�������l���A���Ă��܂��B
 */
//-----------------------------------------------------------------------------
const u32* BufferControl::GetGpuProfileResultPtr( GpuProfileResult result ) const
{
  if( result < GPU_PROFILE_RESULT_MAX ){
    return &m_gpuProfileResultWork[result];
  }
  GFL_ASSERT( result < GPU_PROFILE_RESULT_MAX ); //@check
  return &m_gpuProfileResultWork[0];
}

#endif // GFL_DEBUG



//----------------------------------------------------------------------------
/**
 *	@brief  swapCount�ɑΉ������o�b�t�@ID���J�����g�ɐݒ�
 */
//-----------------------------------------------------------------------------
void BufferControl::SetCurrentSwapCountBuffer(void)
{
  int i;
  int offset;

  // �f�B�X�v���C�o�b�t�@
  {
    offset = m_swapDispCount * DISPLAY_BOTH_MAX;

    for( i=0; i<m_useDisplayNum; ++i ){
      m_curentDisplay[i] = m_displayBufferTbl[ i + offset ];
    }
  }

  // �R�}���h���X�g
  {
    offset = m_swapCommandCount * COMMANDLIST_NUM;

    // �R�}���h���X�g�N���A
    for( i=0; i<COMMANDLIST_NUM; ++i ){
      m_curentCommandList[i]     = m_commandListTbl[ offset + i ];
    }
    m_curentSaveCommandList = m_saveCommandListTbl[ m_swapCommandCount ];
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����g�̃R�}���h���X�g���N���A����
 */
//-----------------------------------------------------------------------------
void BufferControl::ClearCurrentCommandList( void )
{
  u32 i;
  // �R�}���h���X�g�N���A
  for( i=0; i<COMMANDLIST_NUM; ++i ){
    if( m_curentCommandList[i] != COMMANDLIST_ID_ERROR  ){
      m_commandListManager->Clear( m_curentCommandList[i] );
    }
  }
  m_commandListManager->Clear( m_curentSaveCommandList );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����_�����O�����f�B�X�v���C���X���b�v�o�b�t�@����^�[�Q�b�g�̃f�B�X�v���C�ɐݒ�
 */
//-----------------------------------------------------------------------------
void BufferControl::SetSwapTargetRenderDisplay(void)
{
  std::MemCopy( m_rendEndDisplay, m_swapDisplay, sizeof(DisplayBufferID)*DISPLAY_BOTH_MAX );
  ClearRenderEndID();
}


//----------------------------------------------------------------------------
/**
 *	@brief  �f�B�X�v���C�o�b�t�@�X���b�v����
 */
//-----------------------------------------------------------------------------
void BufferControl::Swap(void)
{
  int i;
  GLenum display = NN_GX_DISPLAY_BOTH;
  bool upper = false;
  bool lower = false;

  GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM_HIGH, "SwapBuffer " );

  // �f�B�X�v���C�o�b�t�@�����A�N�e�B�u�ɂ���B
  for( i=0; i<DISPLAY_BOTH_MAX; i++ ){
    if( m_swapDisplay[i] != DISPLAYBUFFER_ID_ERROR )
    {
      //GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM_HIGH, " %d", m_swapDisplay[i] );
      m_displayBufferManager->ActivateBuffer( m_swapDisplay[i], static_cast<DisplayType>(i) );
      if( (i==DISPLAY_UPPER) || (i==DISPLAY_UPPER_RIGHT) ){
        upper = true;
      }else{
        lower = true;
      }
    }
  }

  GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM_HIGH, "\n" );

  // ����ʃ`�F�b�N
  if( upper || lower ){
    if( upper && lower ){
      display = NN_GX_DISPLAY_BOTH;
    }else if( upper ){
      display = NN_GX_DISPLAY0;
    }else if( lower ){
      display = NN_GX_DISPLAY1;
    }

    GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM_HIGH, "swap 0x%x\n", display );

    nngxSwapBuffers(display);
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �f�B�X�v���C�o�b�t�@�@�̐���
 *
 *	@param	heapAllocator     �A���P�[�^
 *	@param	index             �C���f�b�N�X
 *	@param	double_buffer     �_�u���o�b�t�@���[�h���H
 *	@param	cp_setup          �Z�b�g�A�b�v�p�����[�^
 */
//-----------------------------------------------------------------------------
void BufferControl::createDisplayBuffer( gfl::heap::NwAllocator* heapAllocator, u32 index, bool double_buffer, const DisplayBufferSetUp* cp_setup )
{
  // single�Ԗڂ̃o�b�t�@
  m_displayBufferTbl[index] = m_displayBufferManager->CreateBuffer( *cp_setup );

  // double�Ԗڂ̃o�b�t�@
  if( double_buffer ){
    m_displayBufferTbl[index+DISPLAY_BOTH_MAX] = m_displayBufferManager->CreateBuffer( *cp_setup );
    m_displayBufferManager->ActivateBuffer( m_displayBufferTbl[index+DISPLAY_BOTH_MAX], cp_setup->displayType );
  }else{
    m_displayBufferTbl[index+DISPLAY_BOTH_MAX] = DISPLAYBUFFER_ID_ERROR;
    m_displayBufferManager->ActivateBuffer( m_displayBufferTbl[index], cp_setup->displayType );

  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�B�X�v���C�o�b�t�@�̔j��
 */
//-----------------------------------------------------------------------------
void BufferControl::deleteDisplayBuffer( u32 index )
{
  if(m_displayBufferTbl[index] != DISPLAYBUFFER_ID_ERROR){
    m_displayBufferManager->DeleteBuffer( m_displayBufferTbl[index] );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t���[���o�b�t�@�̐���
 */
//-----------------------------------------------------------------------------
void BufferControl::createFrameBuffer( gfl::heap::NwAllocator* heapAllocator, const RenderBufferSetUp* cp_setup )
{
  GFL_ASSERT( cp_setup );
  GFL_ASSERT_STOP( cp_setup->targetDisplay < DISPLAY_BOTH_MAX );
  GFL_ASSERT_STOP( m_frameBufferTbl[cp_setup->targetDisplay] == FRAMEBUFFER_ID_ERROR );
  m_frameBufferTbl[cp_setup->targetDisplay] = m_frameBufferManager->CreateBuffer( heapAllocator, *cp_setup );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t���[���o�b�t�@�̔j��
 */
//-----------------------------------------------------------------------------
void BufferControl::deleteFrameBuffer( u32 index )
{
  if(m_frameBufferTbl[index] != COMMANDLIST_ID_ERROR){
    m_frameBufferManager->DeleteBuffer( m_frameBufferTbl[index] );
    m_frameBufferTbl[index] = FRAMEBUFFER_ID_ERROR;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �R�}���h���X�g�̐���
 */
//-----------------------------------------------------------------------------
void BufferControl::createCommandList( gfl::heap::NwAllocator* heapAllocator, u32 index, bool double_buffer, const GraphicsSystemSetUp* setup )
{
  static const u32 setup_index[] = {
    COMMANDLIST_SETUP_MAIN,
    COMMANDLIST_SETUP_UPPER,
    COMMANDLIST_SETUP_UPPER,
    COMMANDLIST_SETUP_LOWER,
  };

  m_commandListTbl[index] = m_commandListManager->CreateBuffer( setup->commandSetup[ setup_index[index] ] );


  // �_�u���o�b�t�@�����O�p�o�b�t�@�̊i�[
  if(m_doubleBufferFlag){
    m_commandListTbl[index + (GRAPHICSSYSTEM_USE_COMMANDLIST_NUM/2)] = m_commandListManager->CreateBuffer( setup->commandSetup[ setup_index[index] ] );
  }else{
    m_commandListTbl[index + (GRAPHICSSYSTEM_USE_COMMANDLIST_NUM/2)] = COMMANDLIST_ID_ERROR;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �R�}���h���X�g�̔j��
 */
//-----------------------------------------------------------------------------
void BufferControl::deleteCommandList( u32 index )
{
  if(m_commandListTbl[index] != COMMANDLIST_ID_ERROR){
    m_commandListManager->DeleteBuffer( m_commandListTbl[index] );
    m_commandListTbl[index] = COMMANDLIST_ID_ERROR;
  }

  if(m_doubleBufferFlag){
    if(m_commandListTbl[index + (GRAPHICSSYSTEM_USE_COMMANDLIST_NUM/2)] != COMMANDLIST_ID_ERROR){
      m_commandListManager->DeleteBuffer( m_commandListTbl[index + (GRAPHICSSYSTEM_USE_COMMANDLIST_NUM/2)] );
      m_commandListTbl[index + (GRAPHICSSYSTEM_USE_COMMANDLIST_NUM/2)] = COMMANDLIST_ID_ERROR;
    }
  }
}




//----------------------------------------------------------------------------
/**
 *	@brief  �X���b�v����o�b�t�@�h�c���N���A
 */
//-----------------------------------------------------------------------------
void BufferControl::ClearSwapID(void)
{
  int i;
  for( i=0; i<DISPLAY_BOTH_MAX; i++ ){
    m_swapDisplay[i] = DISPLAYBUFFER_ID_ERROR;
  }
  ClearSwapCommandID();
}

void BufferControl::ClearSwapCommandID(void)
{
  m_swapCommandList = COMMANDLIST_ID_ERROR;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����_�����O�I���@�f�B�X�v���C�o�b�t�@ID�̃N���A
 */
//-----------------------------------------------------------------------------
void BufferControl::ClearRenderEndID(void)
{
  int i;
  for( i=0; i<DISPLAY_BOTH_MAX; i++ ){
    m_rendEndDisplay[i] = DISPLAYBUFFER_ID_ERROR;
  }
}






//=============================================================================
/**
 *					�������R���g���[���N���X��`
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^
 *
 *	@param  deviceAllocator   �V�X�e���p�f�o�C�X�A���P�[�^
 *	@param  vramASize         �V�X�e���pVRAM-A�T�C�Y
 *	@param  vramBSize         �V�X�e���pVRAM-B�T�C�Y
 */
//-----------------------------------------------------------------------------
MemoryControl::MemoryControl(heap::NwAllocator* deviceAllocator, u32 vramASize, u32 vramBSize ) :
  m_AreaManVramA( AREAMAN_BLOCK_NUM, deviceAllocator->GetHeapBase() ),
  m_AreaManVramB( AREAMAN_BLOCK_NUM, deviceAllocator->GetHeapBase() )
{
  // �A���b�N�e�[�u��������
  gfl::std::MemFill( m_AllocTblVramA, 0xff, sizeof(gfl::base::AreaMan::ResultCode)*VRAM_ALLOCK_BLOCK_NUM );
  gfl::std::MemFill( m_AllocTblVramB, 0xff, sizeof(gfl::base::AreaMan::ResultCode)*VRAM_ALLOCK_BLOCK_NUM );

  // �A���P�[�^
  m_SystemDeviceAllocator = deviceAllocator;
  m_ApplyDeviceAllocator  = NULL;

  // vram�T�C�Y AREAMAN_BLOCK_SIZE�̔{���ɒ���
  m_SystemVramASize = vramASize + (vramASize % AREAMAN_BLOCK_SIZE);
  m_SystemVramBSize = vramBSize + (vramBSize % AREAMAN_BLOCK_SIZE);


  // �A�v���P�[�V�������������Ȃ��ꍇ�́A�V�X�e������������m��
  // �Œ胁��������m�ۂ���悤�ɂ���B
  this->SetUseMemory( SYSTEM_TYPE );
  // �������m�ە���
  m_AllocWay        = ALLOC_TOP;

}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
MemoryControl::~MemoryControl()
{
#if GFL_DEBUG
  bool error = false;

  if( m_countDevice != 0 ){
    GFL_PRINT( "GraphicsSystem deviceCount %d\n", m_countDevice );
    error = true;
  }

  if( m_countVramA != 0 ){
    GFL_PRINT( "GraphicsSystem vramACount %d\n", m_countVramA );
    error = true;
  }

  if( m_countVramB != 0 ){
    GFL_PRINT( "GraphicsSystem vramBCount %d\n", m_countVramB );
    error = true;
  }

  if( error ){
    GFL_ASSERT(0);
  }
#endif
}



//----------------------------------------------------------------------------
/**
 *	@brief  �A�v���P�[�V�����������ݒ�̏�����
 *
 *	@param	deviceAllocator   �A�v���P�[�V�����Ŏg�p����f�o�C�X������
 */
//-----------------------------------------------------------------------------
void MemoryControl::InitializeApplication( heap::NwAllocator* deviceAllocator )
{
  GFL_ASSERT( m_ApplyDeviceAllocator == NULL  );  // ���d�Ăяo���֎~
  m_ApplyDeviceAllocator = deviceAllocator;

  this->SetUseMemory( APPLICATION_TYPE );
  m_AllocWay = ALLOC_TOP; // �A�v���P�[�V�����́A�g�b�v����B
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�v���P�[�V�����������ݒ�̔j��
 */
//-----------------------------------------------------------------------------
void MemoryControl::FinalizeApplication( void )
{
  m_ApplyDeviceAllocator = NULL;

  if(m_ControlType == APPLICATION_TYPE){

    // �A�v���P�[�V����VRAM�̈悪���ꂢ�ȏ�Ԃ��`�F�b�N
#if GFL_DEBUG
    u32 vrama_free_block = m_AreaManVramA.GetContiniusFreeBlockNum( m_UseVramAStartBlock, m_UseVramANumBlock );
    u32 vramb_free_block = m_AreaManVramB.GetContiniusFreeBlockNum( m_UseVramBStartBlock, m_UseVramBNumBlock );
    bool error = false;
    
    if( vrama_free_block != m_UseVramANumBlock ){
      GFL_PRINT( "GraphicsSystem Application VRAMA Remain Memory blocknum %d\n", m_UseVramANumBlock - vrama_free_block );
      error = true;
    }
    if( vramb_free_block != m_UseVramBNumBlock ){
      GFL_PRINT( "GraphicsSystem Application VRAMB Remain Memory blocknum %d\n", m_UseVramBNumBlock - vramb_free_block );
      error = true;
    }
    GFL_ASSERT_MSG( !error, "VRAM�̉���Y�ꂪ����܂�\n" );
#endif // GFL_DEBUG

    this->SetUseMemory( SYSTEM_TYPE );
    m_AllocWay = ALLOC_TOP;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�v���P�[�V�����������ݒ肪����Ă��邩�H
 *
 *	@retval true  ����Ă���B
 *	@retval false ����Ă��Ȃ��B
 */
//-----------------------------------------------------------------------------
bool MemoryControl::IsInitializeApplication(void) const
{
  if( m_ApplyDeviceAllocator != NULL ){
    return true;
  }
  return false;
}


//----------------------------------------------------------------------------
/**
 *	@brief  Memory�̈�m��
 *
 *	@param	area    Memory�G���A
 *	@param  type    �g�p�p�r
 *	@param	size    �T�C�Y
 *
 *	@return �A�h���X
 */
//-----------------------------------------------------------------------------
void* MemoryControl::Alloc( MemoryArea area, MemoryUseType type, u32 size )
{
  u32 alignment;
  void * resultAddr = NULL;

  // �g�p�p�r���Ƃ̃A���C�����g�擾
  alignment = GraphicsSystem::GetAlignment( type );


  // �������m��
  switch (area)
  {
  case MEMORY_AREA_FCRAM:
    // FCRAM �̏ꍇ�� SDK �T�|�[�g�̊g���q�[�v���犄�蓖�Ă邽�ߊǗ����Ȃ�
    if( m_AllocWay == ALLOC_TOP ){
      resultAddr = GflHeapAllocMemoryAlign( m_UseDeviceAllocator->GetHeapBase(), size, alignment );
    }else{
      resultAddr = GflHeapAllocMemoryLowAlign( m_UseDeviceAllocator->GetHeapBase(), size, alignment );
    }

#if GFL_DEBUG
    m_countDevice ++;
#endif
    break;
  case MEMORY_AREA_VRAMA:
    {
      gfl::base::AreaMan::ResultCode* p_clearWk = GetClearAllocTbl( m_AllocTblVramA );
      uptr  addr;


      // AreaMan�̃u���b�N�T�C�Y��alignment���`�F�b�N���A
      // �G���A�u���b�N�P�ʂł̃A���C�����g���v�Z
      m_AreaManVramA.SetAlign( GetBlockAlign(alignment) );

      // VRAM�̃u���b�N���擾
      if( m_AllocWay == ALLOC_TOP ){
        *p_clearWk = m_AreaManVramA.ReserveArea( GetBlockSize(size), m_UseVramAStartBlock, m_UseVramANumBlock );
      }else{

        u32 searchStartBlock;
        u32 searchBlockNum;

        // �f�B�X�v���C�o�b�t�@�́A0x180000�����̃A�h���X�ɔz�u�����K�v������B
        // �iCTR�̃n�[�h�d�l�j
        if( (type == MEM_DISPLAYBUFFER) && (m_UseVramAStartBlock+m_UseVramANumBlock) >= GetBlockSize(0x180000) ){
          searchStartBlock = GetBlockSize(0x180000) - 1;
          searchBlockNum   = GetBlockSize(0x180000);
        }else{
          searchStartBlock = (m_UseVramAStartBlock+m_UseVramANumBlock) - 1;
          searchBlockNum    = m_UseVramANumBlock;
        }

        *p_clearWk = m_AreaManVramA.ReserveAreaLo( GetBlockSize(size), searchStartBlock, searchBlockNum );
      }

      addr = m_AreaManVramA.ResCodeToBlockPos( *p_clearWk );

      // �m�ۂɎ��s���Ă��Ȃ����`�F�b�N
      if(*p_clearWk == gfl::base::AreaMan::RESULT_FAILED){
        // �T�C�Y�I�[�o�[
        GFL_ASSERT_MSG( 0, "VRAMA over" );
        addr = 0; // �����I�ɓ]��
      }
      addr *= AREAMAN_BLOCK_SIZE;

#if GFL_DEBUG
      if( (type == MEM_DISPLAYBUFFER) && ((addr + size) > 0x180000) ){

        // �f�B�X�v���C�o�b�t�@�@�́@�T�C�Y�I�[�o�[
        GFL_ASSERT(0);
        addr = 0;
      }
#endif
      addr += nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMA);
      resultAddr = reinterpret_cast<void*>(addr);
    }
#if GFL_DEBUG
    m_countVramA ++;
#endif
    break;
  case MEMORY_AREA_VRAMB:
    {
      gfl::base::AreaMan::ResultCode* p_clearWk = GetClearAllocTbl( m_AllocTblVramB );
      uptr  addr;

      // AreaMan�̃u���b�N�T�C�Y��alignment���`�F�b�N���A
      // �G���A�u���b�N�P�ʂł̃A���C�����g���v�Z
      m_AreaManVramB.SetAlign( GetBlockAlign(alignment) );

      // VRAM�̃u���b�N���擾
      if( m_AllocWay == ALLOC_TOP ){
        *p_clearWk = m_AreaManVramB.ReserveArea( GetBlockSize(size), m_UseVramBStartBlock, m_UseVramBNumBlock );
      }else{

        u32 searchStartBlock;
        u32 searchBlockNum;

        // �f�B�X�v���C�o�b�t�@�́A0x180000�����̃A�h���X�ɔz�u�����K�v������B
        // �iCTR�̃n�[�h�d�l�j
        if( (type == MEM_DISPLAYBUFFER) && ((m_UseVramBStartBlock+m_UseVramBNumBlock) >= GetBlockSize(0x180000)) ){
          searchStartBlock = GetBlockSize(0x180000) - 1;
          searchBlockNum    = GetBlockSize(0x180000);
        }else{
          searchStartBlock  = (m_UseVramBStartBlock+m_UseVramBNumBlock) - 1;
          searchBlockNum    = m_UseVramBNumBlock;
        }

        *p_clearWk = m_AreaManVramB.ReserveAreaLo( GetBlockSize(size), searchStartBlock, searchBlockNum );
      }

      addr = m_AreaManVramB.ResCodeToBlockPos( *p_clearWk );

      // �m�ۂɎ��s���Ă��Ȃ����`�F�b�N
      if(*p_clearWk == gfl::base::AreaMan::RESULT_FAILED){
        // �T�C�Y�I�[�o�[
        GFL_ASSERT_MSG( 0, "VRAMB over" );
        addr = 0; // �����I�ɓ]��
      }
      addr *= AREAMAN_BLOCK_SIZE;

#if GFL_DEBUG
      if( (type == MEM_DISPLAYBUFFER) && ((addr + size) > 0x180000) ){
        // �f�B�X�v���C�o�b�t�@�@�́@�T�C�Y�I�[�o�[
        GFL_ASSERT(0);
        addr = 0;
      }
#endif
      addr += nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMB);
      resultAddr = reinterpret_cast<void*>(addr);
    }
#if GFL_DEBUG
    m_countVramB ++;
#endif
    break;
  default:
    GFL_ASSERT(0);
    break;
  }

#if GFL_DEBUG
  switch( area ){
  case MEMORY_AREA_FCRAM:
    GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM, "alloc memory type = FCRAM, size = 0x%x, align = %d, addr = 0x%x\n", size, alignment, resultAddr );
    break;
  case MEMORY_AREA_VRAMA:
    GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM, "alloc memory type = VRAMA, size = 0x%x, align = %d, addr = 0x%x\n", size, alignment, resultAddr );
    break;
  case MEMORY_AREA_VRAMB:
    GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM, "alloc memory type = VRAMB, size = 0x%x, align = %d, addr = 0x%x\n", size, alignment, resultAddr );
    break;
  }
#endif

  GFL_ASSERT(resultAddr);

  return resultAddr;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t���[
 *
 *	@param	area    �������G���A
 *	@param	addr    �A�h���X
 */
//-----------------------------------------------------------------------------
void MemoryControl::Free( MemoryArea area, void* addr )
{
#if GFL_DEBUG
  switch( area ){
  case MEMORY_AREA_FCRAM:
    GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM, "free memory type = FCRAM, addr = 0x%x vramA=%d vramB=%d\n", addr, m_countVramA, m_countVramB );
    break;
  case MEMORY_AREA_VRAMA:
    GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM, "free memory type = VRAMA, addr = 0x%x vramA=%d vramB=%d\n", addr, m_countVramA, m_countVramB );
    break;
  case MEMORY_AREA_VRAMB:
    GFL_PRINT_EX( GFL_DEBUG_PRINT_GRAPHICSSYSTEM, "free memory type = VRAMB, addr = 0x%x vramA=%d vramB=%d\n", addr, m_countVramA, m_countVramB );
    break;
  }
#endif

  switch (area)
  {
  case MEMORY_AREA_FCRAM:
    // �j��
    GflHeapFreeMemory( addr );
#if GFL_DEBUG
    m_countDevice --;
#endif
    break;

  case MEMORY_AREA_VRAMA:
    {
      uptr adress = reinterpret_cast<uptr>(addr);
      adress -= nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMA);


      gfl::base::AreaMan::ResultCode* p_addrWk = GetAddrAllocTbl( m_AllocTblVramA, adress );
      if( p_addrWk == NULL ){
        // �s���ȃA�h���X�B
        GFL_ASSERT( 0 );
      }else{
        m_AreaManVramA.Release( *p_addrWk );
        *p_addrWk = gfl::base::AreaMan::RESULT_FAILED;
      }
    }
#if GFL_DEBUG
    m_countVramA --;
#endif
    break;

  case MEMORY_AREA_VRAMB:
    {
      uptr adress = reinterpret_cast<uptr>(addr);
      adress -= nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMB);


      gfl::base::AreaMan::ResultCode* p_addrWk = GetAddrAllocTbl( m_AllocTblVramB, adress );
      if( p_addrWk == NULL ){
        // �s���ȃA�h���X�B
        GFL_ASSERT( 0 );
      }else{
        m_AreaManVramB.Release( *p_addrWk );
        *p_addrWk = gfl::base::AreaMan::RESULT_FAILED;
      }
    }
#if GFL_DEBUG
    m_countVramB --;
#endif
    break;

  default:
    GFL_ASSERT(0);
    break;
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  �R���g���[���^�C�v�̐ݒ�
 *
 *	@param	type  �^�C�v
 */
//-----------------------------------------------------------------------------
void MemoryControl::SetControlType( MemoryControl::ControlType type, MemoryControl::AllocWay way )
{
  this->SetUseMemory( type );
  m_AllocWay = way;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �R���g���[���^�C�v�̎擾
 *
 *	@return �R���g���[���^�C�v
 */
//-----------------------------------------------------------------------------
MemoryControl::ControlType MemoryControl::GetControlType(void) const
{
  return m_ControlType;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �������m�ە����̎擾
 *
 *	@return �������m�ە���
 */
//-----------------------------------------------------------------------------
MemoryControl::AllocWay MemoryControl::GetAllocWay(void) const
{
  return m_AllocWay;
}


//----------------------------------------------------------------------------
/**
 *	@brief  VRAM-A�@�ő�m�ۉ\�T�C�Y�̎擾
 *
 *	@return �ő�m�ۉ\�T�C�Y
 */
//-----------------------------------------------------------------------------
u32 MemoryControl::GetVramAFreeSize( void ) const
{
  return m_AreaManVramA.GetContiniusFreeBlockNum( m_UseVramAStartBlock, m_UseVramANumBlock ) * AREAMAN_BLOCK_SIZE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  VRAM-B�@�ő�m�ۉ\�T�C�Y�̎擾
 *
 *	@return �ő�m�ۉ\�T�C�Y
 */
//-----------------------------------------------------------------------------
u32 MemoryControl::GetVramBFreeSize( void ) const
{
  return m_AreaManVramB.GetContiniusFreeBlockNum( m_UseVramBStartBlock, m_UseVramBNumBlock ) * AREAMAN_BLOCK_SIZE;
}



//----------------------------------------------------------------------------
/**
 *	@brief  VRAM�Ǘ��̃��Z�b�g����
 *  @fix �L�򂳂�v�]�F�u�q�`�l�A�h���X�w��z�u�Ή�
 */
//-----------------------------------------------------------------------------
u32 GraphicsSystem::GetAlignment( MemoryUseType type )
{
  u32 alignment = ALIGNMENT_SYSTEM_BUFFER;
  switch (type)
  {
  case MEM_SYSTEM:
      alignment = ALIGNMENT_SYSTEM_BUFFER;
      break;
  case MEM_TEXTURE:
      alignment = ALIGNMENT_TEXTURE;
      break;
  case MEM_VERTEXBUFFER:
      alignment = ALIGNMENT_VERTEX;
      break;
  case MEM_RENDERBUFFER:
      alignment = ALIGNMENT_RENDER_BUFFER;
      break;
  case MEM_DISPLAYBUFFER:
      alignment = ALIGNMENT_DISPLAY_BUFFER;
      break;
  case MEM_COMMANDBUFFER:
      alignment = ALIGNMENT_3D_COMMAND_BUFFER;
      break;
  default:
      GFL_ASSERT(0);
      break;
  }

  return alignment;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ��̃A���P�[�g�e�[�u�����擾
 *
 *	@param	p_tbl   �e�[�u��
 *
 *	@return ��̃A���P�[�g�e�[�u���|�C���^
 */
//-----------------------------------------------------------------------------
gfl::base::AreaMan::ResultCode* MemoryControl::GetClearAllocTbl( gfl::base::AreaMan::ResultCode* p_tbl ) const
{
  for( int i=0; i<VRAM_ALLOCK_BLOCK_NUM; i++ ){
    if( p_tbl[i] == gfl::base::AreaMan::RESULT_FAILED ){
      return &p_tbl[i];
    }
  }
  // �m�ۂł���e�[�u���Ȃ�
  GFL_ASSERT_STOP(0);
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  VRAM�A�h���X�@����@�J�n����@�A���b�N�e�[�u�����擾
 *
 *	@param	p_tbl �A���b�N�e�[�u��
 *	@param	addr  �A�h���X
 *
 *	@return VRAM�A�h���X����J�n����A���b�N�e�[�u��
 */
//-----------------------------------------------------------------------------
gfl::base::AreaMan::ResultCode* MemoryControl::GetAddrAllocTbl( gfl::base::AreaMan::ResultCode* p_tbl, uptr addr ) const
{
  u32 start_block = addr / AREAMAN_BLOCK_SIZE;

  for( int i=0; i<VRAM_ALLOCK_BLOCK_NUM; i++ ){
    if( m_AreaManVramA.ResCodeToBlockPos(p_tbl[i]) == start_block ){
      return &p_tbl[i];
    }
  }
  // �m�ۂł���e�[�u���Ȃ�
  GFL_ASSERT_STOP(0);
  return NULL;
}



// �c�[���n�����o
//----------------------------------------------------------------------------
/**
 *	@brief  �o�C�g�P�ʂ̃A���C�����g����AreaMan�̃u���b�N�A���C�����g���v�Z
 *
 *	@param	u32 align   �o�C�g�P�ʂ̃A���C�����g
 *
 *	@return �u���b�N�P�ʂ̃A���C�����g
 */
//-----------------------------------------------------------------------------
u32 MemoryControl::GetBlockAlign( u32 align )
{
  u32 block_align = 1;

  if( align > AREAMAN_BLOCK_SIZE ){
    block_align = align / AREAMAN_BLOCK_SIZE;
    // ���܂肪����΁A�J�グ
    if( (align % AREAMAN_BLOCK_SIZE) != 0 ){
      block_align += 1;
    }
  }
  return block_align;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�C�g�P�ʂ�size����A�m�ۂ������u���b�N�����v�Z
 *
 *	@param	u32 size   �o�C�g�P�ʂ�size
 *
 *	@return �u���b�N�P�ʂ̊m��size
 */
//-----------------------------------------------------------------------------
u32 MemoryControl::GetBlockSize( u32 size )
{
  u32 block_size = 1;

  if( size >= AREAMAN_BLOCK_SIZE ){
    block_size = size / AREAMAN_BLOCK_SIZE;
    if( (size % AREAMAN_BLOCK_SIZE) > 0 ){
      block_size += 1; // �J�グ
    }
  }
  return block_size;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ControlType�ɑΉ������AUse����������ݒ�
 *
 *	@param	control_type  �ݒ肷��R���g���[���^�C�v
 */
//-----------------------------------------------------------------------------
void MemoryControl::SetUseMemory( MemoryControl::ControlType control_type )
{
  u32 systemVramABlockNum = ((m_SystemVramASize % AREAMAN_BLOCK_SIZE) == 0) ? (m_SystemVramASize / AREAMAN_BLOCK_SIZE) : (m_SystemVramASize / AREAMAN_BLOCK_SIZE) + 1;
  u32 systemVramBBlockNum = ((m_SystemVramBSize % AREAMAN_BLOCK_SIZE) == 0) ? (m_SystemVramBSize / AREAMAN_BLOCK_SIZE) : (m_SystemVramBSize / AREAMAN_BLOCK_SIZE) + 1;

  if(control_type == SYSTEM_TYPE){
    m_UseDeviceAllocator  = m_SystemDeviceAllocator;
    m_UseVramAStartBlock  = 0;
    m_UseVramANumBlock    = systemVramABlockNum;
    m_UseVramBStartBlock  = 0;
    m_UseVramBNumBlock    = systemVramBBlockNum;
    m_ControlType         = control_type;

  }
  else if( m_ApplyDeviceAllocator != NULL ){
    m_UseDeviceAllocator  = m_ApplyDeviceAllocator;
    m_UseVramAStartBlock  = systemVramABlockNum;
    m_UseVramANumBlock    = AREAMAN_BLOCK_NUM - m_UseVramAStartBlock;
    m_UseVramBStartBlock  = systemVramBBlockNum;
    m_UseVramBNumBlock    = AREAMAN_BLOCK_NUM - m_UseVramBStartBlock;
    m_ControlType         = control_type;
  }
  else {
    GFL_ASSERT_MSG(0, "�A�v���P�[�V����������������������Ă��܂���B\n");
    // �t���[�Y���
    m_UseDeviceAllocator  = m_SystemDeviceAllocator;
    m_UseVramAStartBlock  = 0;
    m_UseVramANumBlock    = systemVramABlockNum;
    m_UseVramBStartBlock  = 0;
    m_UseVramBNumBlock    = systemVramBBlockNum;
    m_ControlType         = SYSTEM_TYPE;
  }
}


} /* namespace end grp */
} /* namespace end gfl */
