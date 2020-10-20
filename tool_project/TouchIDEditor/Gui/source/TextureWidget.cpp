//==============================================================================

#include "../include/TextureWidget.h"
#include "../include/MainWindow.h"

/**
* @file TextureWidget.cpp
* @brief
* @author kimura_shin
* @data	2012/03/22, 16:08
*/
// =============================================================================

#define KEY_TEX_TAB "TextureTab"

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
TextureWidget::TextureWidget( )
{
  QVBoxLayout* v_lay = DbgNew QVBoxLayout;
  QTabWidget* p_tw = DbgNew QTabWidget;
  m_pOriginalTab = p_tw;
  v_lay->addWidget( p_tw );
  setLayout( v_lay );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void TextureWidget::LoadTexture( gfl::cmdl::Model* model, gfl::cmdl::Model* id_model )
{
  m_pOriginalTab->clear( );
  m_MaterialTreatment.clear( );

  QMap<QString, int> MaterialToValue;
  s32 id = 0;

#define SET_MATERIAL_TREATMENT(name,txt) MaterialToValue.insert(#name,id++);
#include "../include/TextureWidgetDef.h"

  QStringList mate( gfl::qt::File::LoadLines( "Material.txt" ) );
  QMap<QString, int> NameToValue;
  for( s32 j = 0; j < mate.size( ); ++j ){
    NameToValue.insert( mate.at( j ).section( ' ', 0, 0 ), MaterialToValue.value( mate.at( j ).section( ' ', 1, 1 ) ) );
  }

  // マテリアルウィジェット作成
  QString tabname;
  MaterialWidget* mate_tw = NULL;
  m_Materials.clear( );
  s32 cnt;
  for( s32 i = 0; i < model->GetTextureTabInfo( )->size( ); ++i ){
    QPixmap p;
    auto& tabInfo = model->GetTextureTabInfo()->at(i);
    QImage image(tabInfo.GetData(), tabInfo.GetWidth(), tabInfo.GetHeight(), QImage::Format_ARGB32);
    p = QPixmap::fromImage(image);

    QLabel* lb = DbgNew QLabel;
    lb->setPixmap( p );
    QString iname( model->GetTextureTabInfo( )->at( i ).GetMaterial( )->GetName( ) );
    if( iname != tabname ){
      mate_tw = DbgNew MaterialWidget;
      m_Materials.append( mate_tw );
      mate_tw->m_pTextureWidget = this;
      mate_tw->m_pMaterial = id_model->GetTextureTabInfo( )->at( i ).GetMaterial( );
      mate_tw->m_TypeIndex = NameToValue.value( iname, mate_tw->m_pMaterial->m_TranslucencyKind == "Layer1" ? MATERIAL_IGNORE : MATERIAL_SPECIAL );

      // マテリアル別のコンボボックスを追加
      QWidget* wid = DbgNew QWidget;
      QVBoxLayout* lay = DbgNew QVBoxLayout( );
      MaterialComboBox* cb = DbgNew MaterialComboBox;

#define SET_MATERIAL_TREATMENT(name,txt)	cb->addItem(txt);
#include "../include/TextureWidgetDef.h"

      m_MaterialTreatment.insert( iname, cb );
      mate_tw->m_pType = cb;
      cb->setCurrentIndex( NameToValue.value( iname, 1 ) );
      cb->m_pMaterialWidget = mate_tw;

      connect( GetOriginalTab(), SIGNAL( currentChanged( int ) ), mate_tw, SLOT( SetCurrentTabIndex( int ) ) );
      connect( mate_tw, SIGNAL( currentChanged( int ) ), cb, SLOT( SetCurrentIndex( int ) ) );
      connect( cb, SIGNAL( currentIndexChanged( int ) ), mate_tw, SLOT( MaterialChanged( int ) ) );

      QHBoxLayout* h_lay = DbgNew QHBoxLayout;
      QPushButton* pb = DbgNew QPushButton( "全マテリアル変更" );
      connect( pb, SIGNAL( clicked( ) ), mate_tw, SLOT( ChangeAll( ) ) );
      h_lay->addWidget( cb );
      h_lay->addWidget( pb );
      lay->addLayout( h_lay );
      lay->addWidget( cb );
      lay->addWidget( mate_tw );
      wid->setLayout( lay );
      m_pOriginalTab->addTab( wid, iname );
      tabname = iname;
      cnt = 0;
    }
    mate_tw->addTab( lb, QString::number( cnt++ ) );
  }

  // ID モデルのテクスチャー名を再設定するためにクリアする
  id_model->GetTextureTabInfo( )->clear( ); 

  // 重複していないテクスチャファイルでタブイメージを作成
  for( s32 i = 0; i < model->GetTextureTabInfo( )->size( ); ++i ){
    QString fname( MainWindow::GetIdTexFileName( model->GetTextureTabInfo( )->at( i ).GetFileName( ) ) );

    // それぞれのテクスチャを張り替える　重複しているファイルの場合がある
    gfl::cmdl::Material* mate = id_model->GetMaterial( );
    auto& tabInfo = model->GetTextureTabInfo()->at(i);
    id_model->GetTextureTabInfo()->append(gfl::cmdl::Model::TextureTabInfo(fname, mate, tabInfo.GetData(), tabInfo.GetWidth(), tabInfo.GetHeight()));
  }

  SetTypeIndexChange(false);

  // 自動おまじない機能
  for( s32 i = 0; i < m_Materials.size( ); ++i ){
    m_Materials.at( i )->ChangeAllByValue( 0 );
  }
  for( s32 i = 0; i < m_Materials.size( ); ++i ){
    m_Materials.at( i )->ChangeAllByValue( 1 );
  }

  SetTypeIndexChange(true);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MaterialWidget::MaterialChanged( int val )
{
  switch( val ){
  case TextureWidget::MATERIAL_IGNORE:
    m_pMaterial->SetShaderType( gfl::cmdl::Material::ShaderType::Ignore );
    break;
  case TextureWidget::MATERIAL_SPECIAL:
    m_pMaterial->SetShaderType( gfl::cmdl::Material::ShaderType::SimpleIDRender );
    break;
  default:
    m_pMaterial->SetShaderType( gfl::cmdl::Material::ShaderType::ModelRender );
    break;
  }
  if( m_pTextureWidget->IsTypeIndexChange() ){
    m_TypeIndex = val;
  }
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MaterialWidget::ChangeAllByValue(s32 type)
{
#ifdef MAYA_INPORT
  TextureWidget* tw = MainWindow::GetInstance( )->GetTextureWidget( );
  for( s32 i = 0; i < tw->GetMaterials( ).size( ); ++i ){
    tw->GetMaterials( ).at( i )->m_pType->setCurrentIndex( type );
  }
#endif
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MaterialWidget::ChangeAll( )
{
#ifdef MAYA_INPORT
  ChangeAllByValue( m_pType->currentIndex( ) );
#endif
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MaterialComboBox::SetCurrentIndex( int )
{
  setCurrentIndex( m_pMaterialWidget->m_TypeIndex );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void MaterialWidget::SetCurrentTabIndex( int )
{
  m_pType->setCurrentIndex( m_TypeIndex );
}
