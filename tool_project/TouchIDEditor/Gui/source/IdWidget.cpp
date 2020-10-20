//==============================================================================

#include "../include/IdWidget.h"
#include "../include/MainWindow.h"

#include <rapidxml/include/gfl_Xml.h>

/**
* @file	IdWidget.cpp
* @brief	
* @author	kimura_shin
* @data	2012/03/22, 10:22
*/
// =============================================================================

#define KEY_ID_COL "Id"

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
IdWidget::IdWidget( ) : QGroupBox( "ID" )
{
  QListWidget* cb = DbgNew QListWidget;

  m_pIdList = cb;
  gfl::xml::Document doc;
  QString fname("TouchId.xml");

  if( doc.Load( fname, true ) )
  {
    gfl::xml::Node* node = doc.RootNode( );

    u8 id = 0;

    for( gfl::xml::Node* c_node = node->FirstNode( "ID" ); c_node; c_node = c_node->NextSibling( "ID" ) ){
      QString col(c_node->AttributeString( "Color" ));
      cb->addItem( DbgNew IdItem( c_node->AttributeString( "Text" ), col, QString(c_node->AttributeString( "Id" )).split(' ') ) );
      u32 rgb = QColor(col).rgba();
      rgb = ((rgb>>16)&0xff) | ((rgb<<16)&0xff0000) | (rgb&0xff00ff00);

      if( m_IdMap.value(rgb) ){
        m_Dublicated += m_IdMapString.value(rgb) + "と" + QString(c_node->AttributeString( "Text" )) + "の ID色が重複しています\n";
      }

      m_IdMap.insert( rgb, id++ );
      m_IdMapString.insert( rgb, c_node->AttributeString( "Text" ) );
    }
  }
  else
  {
    cb->addItem( DbgNew IdItem( fname + "タッチIDファイルがないか、壊れています。\n", QString( "#000000" ), QStringList("COL_BG") ));
  }

  cb->setMaximumSize( 128, 128 );
  cb->setCurrentRow( MainWindow::GetInstance( )->GetSettings( )->value( KEY_ID_COL, 0 ).toInt( ) );
  connect( cb, SIGNAL( itemClicked( QListWidgetItem* ) ), this, SLOT( IdChanged( QListWidgetItem* ) ) );
  QHBoxLayout* h_lay = DbgNew QHBoxLayout;
  h_lay->addWidget( cb );

  QVBoxLayout* main_lay = DbgNew QVBoxLayout;
  QVBoxLayout* v_lay = DbgNew QVBoxLayout;

  main_lay->addLayout( h_lay );

  // Qlabel だとうまくいかない…
  QPushButton* pb = DbgNew QPushButton( this );

  if(cb->currentItem( ))
  {
    pb->setPalette( QPalette( QColor( ( ( IdItem* )cb->currentItem( ) )->GetColor( ) ) ) );
  }

  pb->setMinimumSize( 172, 32 );
  pb->setDisabled( true );
  m_pIdButton = pb;

  v_lay->addWidget( pb );
  main_lay->addLayout( v_lay );

  setLayout( main_lay );

  setMaximumSize( 192, 192 );
}

//------------------------------------------------------------------------------
// Id changed
//------------------------------------------------------------------------------
void IdWidget::IdChanged( QListWidgetItem* item )
{
  MainWindow::GetInstance( )->GetSettings( )->setValue( KEY_ID_COL, m_pIdList->currentRow( ) );
  m_pIdButton->setPalette( QPalette( QColor( ( ( IdItem* )item )->GetColor( ) ) ) );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool IdWidget::SetCurrentIdColor(const QColor& col)
{
  for( s32 i = 0; i < m_pIdList->count(); ++i ){
    IdItem* item = (IdItem*)m_pIdList->item(i);
    if(item->GetColor() == col){
      m_pIdList->setCurrentRow(i);
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void IdWidget::CheckId(const QString& fpath)
{
  QString fname(fpath.section('/',-1));
  for( s32 i = 0; i < m_pIdList->count(); ++i ){
    IdItem* item = (IdItem*)m_pIdList->item(i);
    const QStringList& lst = item->GetId();
    bool show_flag;
    if( lst.size() ){
      show_flag = false;
      for( s32 j = 0; j < lst.size(); ++j ){
        if(fname.startsWith(lst.at(j))){
          show_flag = true;
          break;
        }
      }
    }else{
      show_flag = true;
    }

    if(!show_flag && m_pIdList->currentItem() == item){
      m_pIdList->setCurrentRow(0);
      m_pIdButton->setPalette( QPalette( QColor( ( ( IdItem* )m_pIdList->item(0) )->GetColor( ) ) ) );
    }

    m_pIdList->item(i)->setHidden( !show_flag );
  }
}
