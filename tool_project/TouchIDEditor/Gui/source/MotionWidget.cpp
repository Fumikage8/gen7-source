//==============================================================================

#include "../include/MotionWidget.h"
#include "../include/MainWindow.h"

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotData.h>

#include "TouchIDEditor/HeapDefine.h"

/**
* @file	MotionWidget.cpp
* @brief	
* @author	kimura_shin
* @data	2012/03/26, 12:13
*/
// =============================================================================

#define KEY_MOTION "Motion"
#define KEY_FRAME "Frame"
#define MOTION_EXT ".gfmot"

//------------------------------------------------------------------------------
// Construct
//------------------------------------------------------------------------------
MotionWidget::MotionWidget( ) :
  QGroupBox( "モーション" ),
  //m_pCskla( 0 ),
  m_IsInitialized(false),
  m_pId( 0 ),
  m_pMatrix( 0 )
{
  QVBoxLayout* v_lay = DbgNew QVBoxLayout;
  QHBoxLayout* h_lay = DbgNew QHBoxLayout;

  QComboBox* cb = DbgNew QComboBox( );
  m_pMotionList = cb;
  connect( cb, SIGNAL( currentIndexChanged( const QString& ) ), this, SLOT( MotionChanged( const QString& ) ) );
  v_lay->addWidget( cb );

  h_lay->addWidget( DbgNew QLabel( "フレーム:" ) );
  QSlider* sl = DbgNew QSlider( Qt::Horizontal );
  m_pFrame = sl;
  sl->setMaximum( 0xff );

  connect( sl, SIGNAL( valueChanged( int ) ), this, SLOT( FrameChanged( int ) ) );
  h_lay->addWidget( sl );
  v_lay->addLayout( h_lay );

  setMaximumSize( 192, 128 );
  setLayout( v_lay );

  gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId(1);
  m_pHeap = GFL_CREATE_LOCAL_HEAP(pDevHeap, 0x1000000, gfl2::heap::HEAP_TYPE_EXP, false);

  gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEVICE);
  allocator = new GLAllocator(devHeap);

  gfl2::gfx::GLMemory::SetNewParameterAllocator(allocator);
  gfl2::gfx::GLMemory::SetInstanceAllocator(allocator);
}

MotionWidget::~MotionWidget()
{
  GFL_DELETE_HEAP(m_pHeap);
  Clear();

  delete allocator;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MotionWidget::SetMotionList()
{
  m_pMotionList->clear( );
  QDir dir( QDir::current( ) );

  QStringList lst( dir.entryList( QStringList( "*"MOTION_EXT ), QDir::Files ) );
  lst.replaceInStrings( MOTION_EXT, "" );

  m_pMotionList->addItems( lst );
}

//------------------------------------------------------------------------------
// Motion
//------------------------------------------------------------------------------
void MotionWidget::MotionChanged( const QString& fname )
{
  if(fname.isEmpty()
#ifdef WAIT_LOAD
    || MainWindow::GetInstance()->GetGLWindow()->GetGLWidget()->GetRenderState() == GLWidget::RENDER_NOW
#endif
    )
  {
      return;
  }

  Clear( );

  auto filename = QDir::currentPath().toStdString() + '/' + fname.toStdString() + MOTION_EXT;

  // アニメーションリソース読み込み
  gfl2::renderingengine::scenegraph::resource::GfMotData animationResourceData;
  animationResourceData.ReadFile(filename);

  m_pCurrentResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(allocator, &animationResourceData);

  // 読み込み失敗したら無視
  if (m_pCurrentResourceNode == NULL)
  {
    return;
  }

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode = MainWindow::GetInstance()->GetGLWindow()->GetGLWidget()->GetModelInstanceNode();

  // アニメーションコントローラー初期化
  if (!m_IsInitialized)
  {
    m_AnimationController.Initialize(m_pHeap, pModelInstanceNode);

    m_IsInitialized = true;
  }

  // アニメーション設定
  gfl2::animation::JointAnimationSlot* pJointSlot = m_AnimationController.GetJointSlot();
  pJointSlot->SetAnimationResource(m_pCurrentResourceNode);
  pJointSlot->SetStepFrame(0.0f);

  gfl::cmdl::Model* model = MainWindow::GetInstance( )->GetGLWindow( )->GetGLWidget( )->GetModel( GLWidget::MODEL_ORG );

  gfl::core::Matrix *mat;
  m_pMatrix = mat = DbgNew gfl::core::Matrix [model->GetBoneCount( )];

  m_pFrame->setMaximum(pJointSlot->GetFrameCount());
  m_pFrame->setValue(0);
  CalcMatrix( 0 );
}

//------------------------------------------------------------------------------
// calculate matrix
//------------------------------------------------------------------------------
void MotionWidget::CalcMatrix( const s32 frame )
{
  gfl2::animation::JointAnimationSlot* pJointSlot = m_AnimationController.GetJointSlot();
  pJointSlot->SetFrame(frame);

  m_AnimationController.Update();
  auto pModelInstanceNode = MainWindow::GetInstance()->GetGLWindow()->GetGLWidget()->GetModelInstanceNode();

  gfl::core::Matrix* matrix = GetMatrix();
  for (s32 i = 0; i < pModelInstanceNode->GetJointNum(); ++i, ++matrix)
  {
    auto pJointNode = pModelInstanceNode->GetJointInstanceNode(i);
    auto pParentNode = pJointNode->GetParent()->SafeCast<gfl2::renderingengine::scenegraph::instance::JointInstanceNode>();

    gfl2::math::Matrix34 localMatrix = pJointNode->GetLocalSRT().ToMatrix();

    gfl2::math::Matrix34 worldMatrix = (pParentNode != NULL)
      ? pParentNode->GetWorldMatrix() * localMatrix
      : localMatrix;

    pJointNode->SetWorldMatrix(worldMatrix);

    gfl2::math::Matrix34 skinningMatrix = worldMatrix * pJointNode->GetBindPoseMatrix();

    memcpy(matrix, &skinningMatrix, sizeof(f32) * 12);
  }
}


//------------------------------------------------------------------------------
// Frame
//------------------------------------------------------------------------------
void MotionWidget::FrameChanged( int val )
{
  CalcMatrix(val);
}
