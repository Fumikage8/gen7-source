//==============================================================================
/**
 * @file	macroDialog.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/07/29, 15:59
 */
// =============================================================================

#include "macroDialog.h"
#include "save_xml_define.h"
#include "localDef.h"

static const f32 scF32Max = 100000.0f;
static const f32 scF32Min = -100000.0f;

void ComboBox::SearchText(const QString &str )
{
  this->clear();
  for( int i=0 ; i<m_LabelList.size() ; i++ )
  {
    QString listStr = m_LabelList[i];
    if( listStr.toLower().contains( str.toLower() ) == true )
    {
      QString label = listStr.right(listStr.size()-mCutStrNumLeft).left( listStr.size()-mCutStrNumRight );
		  this->addItem( label );
    }
  }
}

void DoubleSpinBox::UpdateValue(double val)
{
  if( mSyncSwitch->checkState() == Qt::Checked )
  {
    mOtherBox[0]->setValue(val);
    mOtherBox[1]->setValue(val);
  }
}


//==============================================================================
//==============================================================================
QString MacroDialog::mSearchStr;

MacroDialog::MacroDialog( QWidget* parent, gfl::xml::Node* node, gfl::xml::Node* load_node_base , gfl::xml::Node* define_node , gfl::xml::Node* path_def_node )
: QDialog( parent ), m_pCommandNode( node ), m_pLoadNode( load_node_base )
{
	setWindowTitle( node->AttributeString( "brief" ) );

  gfl::xml::Node *load_node = load_node_base->FirstNode();
	QVBoxLayout* layout = new QVBoxLayout;

  //フレーム設定
  {
		QHBoxLayout* hlay = new QHBoxLayout;
    int sFrame = m_pLoadNode->AttributeInteger(XML_NAME_COMMAND_ATTR_SFRAME);
    int eFrame = m_pLoadNode->AttributeInteger(XML_NAME_COMMAND_ATTR_EFRAME);
    for (int i=0;i<2;i++ )
    {
      hlay->addWidget( new QLabel( QString( (i==0?"開始フレーム":"終了フレーム" ) ) ));
      m_frame[i] = new QSpinBox;
		  m_frame[i]->setAlignment( Qt::AlignRight );
		  m_frame[i]->setMaximum( 65536 );
		  m_frame[i]->setMinimum( 0 );
		  m_frame[i]->setValue( (i==0?sFrame:eFrame ) );
  		hlay->addWidget( m_frame[i] );
    }
		layout->addLayout( hlay );
  }
  {
    const int r   = m_pLoadNode->AttributeInteger( XML_NAME_COMMAND_ATTR_COL_R , 255 );
    const int g   = m_pLoadNode->AttributeInteger( XML_NAME_COMMAND_ATTR_COL_G , 0 );
    const int b   = m_pLoadNode->AttributeInteger( XML_NAME_COMMAND_ATTR_COL_B , 0 );
    mCol.setRgb( r,g,b );
  }
  //

	for( node = node->FirstNode( ); node; node = node->NextSibling( ) ){
		QString labelname( node->AttributeString( "val" ) );
		QLabel* lb = new QLabel( labelname.size( ) ? labelname : node->AttributeString( "comment" ) );
    lb->setToolTip( node->AttributeString( "comment" ) );

		QString type( node->AttributeString( "type" ) );
		QString initVal( node->AttributeString( "init" ) );

		if( type == "COMBOBOX_TEXT" ){
			ComboBox* cb = new ComboBox;
      cb->SetMode(0);
      
      gfl::xml::Node *attr_node = node;
      
      if( node->AttributeString( "attr_group" , NULL ) )
      {
        attr_node = define_node->FirstNode( node->AttributeString( "attr_group") );
      }
      
			for( s32 i = 0;; ++i ){
        QString defLabel(attr_node->AttributeString( "attr_label" + QString::number( i ) ));
				if( defLabel.size( ) ){
          QString com( define_node->FirstNode(defLabel)->AttributeString("comment") );
					cb->addItem( com );
					cb->GetLabelList( )->append( defLabel );
          
          if( defLabel == initVal )
          {
            cb->setCurrentIndex(i);
          }

				} else {
					break;
				}
			}
			
      if( load_node )
      {
			  gfl::xml::Node* c_node = load_node->FirstNode( node->name( ) );
			  if(c_node){
				  c8* ret = (c8*)c_node->AttributeString( "value0" );
				  s32 idx=0;
				  if(isalpha(ret[0])){
					  for( s32 i = 0; i < cb->GetLabelList()->size(); ++i ){
						  if(cb->GetLabelList()->at(i) == ret){
							  idx = i;
							  break;
						  }
					  }
				  }else{
					  idx = atoi(ret);
				  }
				  cb->setCurrentIndex(idx);
			  }
      }
			
			layout->addWidget( lb );
			layout->addWidget( cb );

			QList<QWidget*> lst;
			lst << cb;
			m_WidgetMap[node->name( )] = lst;
		} else if( type == "VALUE_VECFX32" ){
      float min = node->AttributeFloat( "min",( s32 )scF32Min );
      float max = node->AttributeFloat( "max",( s32 )scF32Max );

      //名前と同期ボタン
      QHBoxLayout* hlay1 = new QHBoxLayout;
      hlay1->addWidget( lb );
			QCheckBox* cb = new QCheckBox();
      hlay1->addWidget( cb );
      hlay1->addStretch();

      QHBoxLayout* hlay2 = new QHBoxLayout;
			QList<QWidget*> lst;
      DoubleSpinBox* dsp[3];
			for( s32 i = 0; i < gfl::core::TXYZ; ++i ){
        c8 *defLabel[3] = {"X","Y","Z"};
        c8 *lb = (c8*)node->AttributeString( "label" + QString::number( i ), defLabel[i] );
  		  QLabel *subLb = new QLabel( QString( lb ) );
				hlay2->addWidget( subLb );

        //hlay2->addWidget( new QLabel( QString( 'X' + i ) ) );
				dsp[i] = new DoubleSpinBox;
				dsp[i]->setAlignment( Qt::AlignRight );
				dsp[i]->setMaximum( max );
				dsp[i]->setMinimum( min );
				dsp[i]->setValue( atof( GetInitValue( node, load_node, i ) ) );

        dsp[i]->SetSyncSwitch(cb);
				hlay2->addWidget( dsp[i] );
				lst << dsp[i];
			}
      dsp[0]->SetOtherBox(dsp[1],dsp[2]);
      dsp[1]->SetOtherBox(dsp[0],dsp[2]);
      dsp[2]->SetOtherBox(dsp[0],dsp[1]);
    	connect( dsp[0] , SIGNAL(valueChanged(double)) , dsp[0] , SLOT(UpdateValue(double)) );	
    	connect( dsp[1] , SIGNAL(valueChanged(double)) , dsp[1] , SLOT(UpdateValue(double)) );	
    	connect( dsp[2] , SIGNAL(valueChanged(double)) , dsp[2] , SLOT(UpdateValue(double)) );	

      
			layout->addLayout( hlay1 );
			layout->addLayout( hlay2 );

      //デフォルト同期
  		QString isSync( node->AttributeString( "sync","0" ) );
      if( isSync == "1" )
      {
        cb->setCheckState(Qt::Checked);
      }

			m_WidgetMap[node->name( )] = lst;
		} else if( type == "VALUE_VECINT" ){
      int min = node->AttributeInteger( "min",( s32 )scF32Min );
      int max = node->AttributeInteger( "max",( s32 )scF32Max );
			QHBoxLayout* hlay = new QHBoxLayout;
			QList<QWidget*> lst;
			for( s32 i = 0; i < gfl::core::TXYZ; ++i ){
				hlay->addWidget( new QLabel( QString( 'X' + i ) ) );
				QSpinBox* sp = new QSpinBox;
				sp->setAlignment( Qt::AlignRight );
				sp->setMaximum( ( s32 )max );
				sp->setMinimum( ( s32 )min );
				sp->setValue( atoi( GetInitValue( node, load_node, i ) ) );
				hlay->addWidget( sp );
				lst << sp;
			}
			layout->addWidget( lb );
			layout->addLayout( hlay );

			m_WidgetMap[node->name( )] = lst;
		} else if( type == "VALUE_INT" ){
      int min = node->AttributeInteger( "min",( s32 )scF32Min );
      int max = node->AttributeInteger( "max",( s32 )scF32Max );

      QSpinBox* sp = new QSpinBox( );

			sp->setMaximum( ( s32 )max );
			sp->setMinimum( ( s32 )min );

			sp->setValue( atoi( GetInitValue( node, load_node ) ) );
			sp->setToolTip( node->AttributeString( "comment" ) );

			QHBoxLayout* hlay = new QHBoxLayout;
			hlay->addWidget( lb );
			hlay->addWidget( sp );
			layout->addLayout( hlay );

			QList<QWidget*> lst;
			lst << sp;
			m_WidgetMap[node->name( )] = lst;
		} else if( type == "VALUE_FX32" ){
      float min = node->AttributeFloat( "min",( s32 )scF32Min );
      float max = node->AttributeFloat( "max",( s32 )scF32Max );
      
      QDoubleSpinBox* dsp = new QDoubleSpinBox;
			dsp->setToolTip( node->AttributeString( "comment" ) );
			dsp->setMaximum( max );
			dsp->setMinimum( min );
			dsp->setValue( atof( GetInitValue( node, load_node ) ) );
			QHBoxLayout* hlay = new QHBoxLayout;
			hlay->addWidget( lb );
			hlay->addWidget( dsp );
			layout->addLayout( hlay );

			QList<QWidget*> lst;
			lst << dsp;
			m_WidgetMap[node->name( )] = lst;
		} else if( type == "VALUE_ANGLE" ){
			QSpinBox* sp = new QSpinBox( );
			sp->setMaximum( 360 );
			sp->setValue( atoi( GetInitValue( node, load_node ) ) );
			sp->setToolTip( node->AttributeString( "comment" ) );
			QHBoxLayout* hlay = new QHBoxLayout;
			hlay->addWidget( lb );
			hlay->addWidget( sp );
			layout->addLayout( hlay );

			QList<QWidget*> lst;
			lst << sp;
			m_WidgetMap[node->name( )] = lst;
		} else if( type == "VALUE_SLIDER" ){
			QSlider* sl = new QSlider(Qt::Horizontal);
      int min = node->AttributeInteger( "min",( s32 )scF32Min );
      int max = node->AttributeInteger( "max",( s32 )scF32Max );
			sl->setMaximum( max );
			sl->setMinimum( min );
			sl->setValue( atoi( GetInitValue( node, load_node ) ) );
			sl->setToolTip( node->AttributeString( "comment" ) );

			QSpinBox* sp = new QSpinBox( );
			sp->setMaximum( max );
			sp->setMinimum( min );
			sp->setValue( atoi( GetInitValue( node, load_node ) ) );
			sp->setToolTip( node->AttributeString( "comment" ) );

    	connect( sp, SIGNAL( valueChanged(int) ), sl, SLOT( setValue(int) ) );
    	connect( sl, SIGNAL( valueChanged(int) ), sp, SLOT( setValue(int) ) );

			QHBoxLayout* hlay = new QHBoxLayout;
			hlay->addWidget( lb );
			hlay->addWidget( sl );
			hlay->addWidget( sp );
			layout->addLayout( hlay );

			QList<QWidget*> lst;
			lst << sp;
			m_WidgetMap[node->name( )] = lst;
		} else if( type == "VALUE_CHECKBOX" ){
    
			QCheckBox* cb = new QCheckBox();
      cb->setText( lb->text() );
			cb->setToolTip( node->AttributeString( "comment" ) );
      
      const int initVal = atoi( GetInitValue( node, load_node ) );
      if( initVal == 1 )
      {
        cb->setCheckState( Qt::Checked );
      }
      else
      {
        cb->setCheckState( Qt::Unchecked );
      }

      QHBoxLayout* hlay = new QHBoxLayout;
      QSpacerItem *spacer1 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
			hlay->addItem( spacer1 );
			hlay->addWidget( cb );
			hlay->setSpacing(0);
			layout->addLayout( hlay );

      delete lb;  //使わない

      QList<QWidget*> lst;
			lst << cb;
			m_WidgetMap[node->name( )] = lst;

    }
		else if( type == "VALUE_STRING" )
    {
      QLineEdit *le = new QLineEdit( );
      le->setMaxLength( node->AttributeInteger( "length",SEQEDIT_VALUE_STR_LEN_DEFAULT ));
      le->setText(GetInitValue( node, load_node, 0,true ));


      QHBoxLayout* hlay = new QHBoxLayout;
			hlay->addWidget( lb );
			hlay->addWidget( le );
			layout->addLayout( hlay );

			QList<QWidget*> lst;
			lst << le;
			m_WidgetMap[node->name( )] = lst;
    }
    else if( type == "FILE_GARC" )
    {
      //ファイルを開く
      QString path = ConvertPathDefine( path_def_node , node->AttributeString( "file" ) );
      QFile gaixFile(path);
      QString extension = node->AttributeString("extension");
      if( gaixFile.open(QIODevice::ReadOnly) )
      {
			  ComboBox* cb = new ComboBox;
        cb->SetMode(1);
        QFileInfo fileInfo( gaixFile );
        QString baseName = fileInfo.completeBaseName();
        QTextStream gaixText(&gaixFile); 

        //再編集時用処理
        char* defName = NULL;
        if( load_node )
        {
		      gfl::xml::Node* c_node = load_node->FirstNode( node->name( ) );
		      if(c_node)
          {
			      defName = (c8*)c_node->AttributeString( "value0" );
          }
        }

        int readSeq = 0;
        int idx = 0;
        while( !gaixText.atEnd() )
        {
          //行読み
          QString str = gaixText.readLine();
          
          switch( readSeq )
          {
          case 0:
            //とりあえずenum行まで読む
            if( str.left(4) == "enum" )
            {
              readSeq = 1; 
            }
            break;
          case 1:
            //enum行終端チェック
            if( str.left(2) == "};" )
            {
              readSeq = 2; 
            }
            else
            {
              //ARC名解釈
              QStringList list = str.split(" ", QString::SkipEmptyParts);
              QString label = list[0].right(list[0].size()-6-baseName.size()); //[GARC_ファイル名_]を排除
              if( extension.size() == 0 || 
                  label.right( extension.size() ) == extension )
              {
						    cb->addItem( label );
                cb->GetLabelList( )->append( list[0] );
                
                //再編集時の初期値かチェックする
                if( defName && defName == list[0] )
                {
    					    cb->setCurrentIndex(idx);
                }
                idx++;
              }
            }
            
            break;
          }

          //gfl::core::Debug::PrintConsole("[%s]\n",str.toLocal8Bit().constData() );
        }


        layout->addWidget( lb );
        QHBoxLayout* hlay = new QHBoxLayout;
  			QLineEdit* le = new QLineEdit();
        hlay->addWidget( cb );
        hlay->addWidget( le );
  			layout->addLayout( hlay );

        cb->SetCutStrNumLeft( 6+baseName.size() );
        connect( le, SIGNAL( textChanged(const QString &) ), cb, SLOT( SearchText(const QString &) ) );
        connect( le, SIGNAL( textChanged(const QString &) ), this, SLOT( SetSearchStr(const QString &) ) );

        QList<QWidget*> lst;
			  lst << cb;
			  m_WidgetMap[node->name( )] = lst;
      }
		  else
      {
        //gfl::core::Debug::PrintConsole("gaix [%s] is not found!!\n",defNode->AttributeString("value"));
      }
    }//( type == "FILE_GARC" )
    else if( type == "FILE_CSID" )
    {
      //ファイルを開く
      QString path = ConvertPathDefine( path_def_node , node->AttributeString( "file" ) );
      QFile gaixFile(path);
      QString keyWord = node->AttributeString("keyword");
      if( gaixFile.open(QIODevice::ReadOnly) )
      {
			  ComboBox* cb = new ComboBox;
        cb->SetMode(1);
        QFileInfo fileInfo( gaixFile );
        QString baseName = fileInfo.completeBaseName();
        QTextStream gaixText(&gaixFile); 
        
        //再編集時用処理
        char* defName = NULL;
        if( load_node )
        {
		      gfl::xml::Node* c_node = load_node->FirstNode( node->name( ) );
		      if(c_node)
          {
			      defName = (c8*)c_node->AttributeString( "value0" );
          }
        }
        int idx = 0;
        int readSeq = 0;

        //デフォルト追加
        int defNo = node->AttributeInteger("use_default",0);
        if( defNo != 0 )
        {
          QString defStr("Default");
			    cb->addItem( defStr );
			    cb->GetLabelList( )->append( defStr );
        
          //再編集時の初期値かチェックする
          if( defName && defName == defStr )
          {
			      cb->setCurrentIndex(idx);
          }
          idx++;
        }



        while( !gaixText.atEnd() )
        {
          //行読み
          QString str = gaixText.readLine();
          
          switch( readSeq )
          {
          case 0:
            //とりあえずenum行まで読む
            if( str.left(4) == "enum" )
            {
              readSeq = 1; 
              //2行空読み
              str = gaixText.readLine();
              str = gaixText.readLine();
            }
            break;
          case 1:
            //enum行終端チェック
            if( str.left(2) == "};" )
            {
              readSeq = 2; 
            }
            else
            {
              //ARC名解釈
              QStringList list = str.split(" ", QString::SkipEmptyParts);
              if( list.size() >= 3 )
              {
                /*
                gfl::core::Debug::PrintConsole("[%s][%s][%s]\n"
                      ,list[0].toLocal8Bit().constData()
                      ,list[1].toLocal8Bit().constData()
                      ,list[2].toLocal8Bit().constData());
                */

                if( keyWord.size() == 0 || 
                    list[0].left( keyWord.size() ) == keyWord )
                {
  						    cb->addItem( list[0] );
  						    cb->GetLabelList( )->append( list[0] );
                
                  //再編集時の初期値かチェックする
                  if( defName && defName == list[0] )
                  {
    					      cb->setCurrentIndex(idx);
                  }
                  idx++;

                }

                /*
                QString label = list[0].right(list[0].size()-6-baseName.size()); //[GARC_ファイル名_]を排除
  						  cb->addItem( label );
  						  cb->GetLabelList( )->append( list[0] );
                
                //再編集時の初期値かチェックする
                if( defName && defName == list[0] )
                {
    					    cb->setCurrentIndex(idx);
                }
                idx++;
                */
              }
            }
            
            break;
          }

          //gfl::core::Debug::PrintConsole("[%s]\n",str.toLocal8Bit().constData() );
        }
			  layout->addWidget( lb );
        QHBoxLayout* hlay = new QHBoxLayout;
  			QLineEdit* le = new QLineEdit();
        hlay->addWidget( cb );
        hlay->addWidget( le );
  			layout->addLayout( hlay );

        connect( le, SIGNAL( textChanged(const QString &) ), cb, SLOT( SearchText(const QString &) ) );
        connect( le, SIGNAL( textChanged(const QString &) ), this, SLOT( SetSearchStr(const QString &) ) );

        QList<QWidget*> lst;
			  lst << cb;
			  m_WidgetMap[node->name( )] = lst;
      }
		  else
      {
        //gfl::core::Debug::PrintConsole("gaix [%s] is not found!!\n",defNode->AttributeString("value"));
      }
    } //( type == "FILE_CSID" )
    else if( type == "FILE_CSID_MID" )
    {
      //ファイルを開く
      QString path = ConvertPathDefine( path_def_node , node->AttributeString( "file" ) );
      QFile gaixFile(path);
      QString keyWord = node->AttributeString("keyword");
      if( gaixFile.open(QIODevice::ReadOnly) )
      {
			  ComboBox* cb = new ComboBox;
        cb->SetMode(1);
        QFileInfo fileInfo( gaixFile );
        QString baseName = fileInfo.completeBaseName();
        QTextStream gaixText(&gaixFile); 
        
        //再編集時用処理
        char* defName = NULL;
        if( load_node )
        {
		      gfl::xml::Node* c_node = load_node->FirstNode( node->name( ) );
		      if(c_node)
          {
			      defName = (c8*)c_node->AttributeString( "value0" );
          }
        }
        int idx = 0;
        int readSeq = 0;

        //デフォルト追加
        int defNo = node->AttributeInteger("use_default",0);
        if( defNo != 0 )
        {
          QString defStr("Default");
			    cb->addItem( defStr );
			    cb->GetLabelList( )->append( defStr );
        
          //再編集時の初期値かチェックする
          if( defName && defName == defStr )
          {
			      cb->setCurrentIndex(idx);
          }
          idx++;
        }
        while( !gaixText.atEnd() )
        {
          //行読み
          QString str = gaixText.readLine();
          
          switch( readSeq )
          {
          case 0:
            //とりあえずenum行まで読む
            if( str.left(4) == "enum" )
            {
              readSeq = 1; 
              //2行空読み
              str = gaixText.readLine();
              str = gaixText.readLine();
            }
            break;
          case 1:
            //enum行終端チェック
            if( str.left(2) == "};" )
            {
              readSeq = 2; 
            }
            else
            {
              //ARC名解釈
              QStringList list = str.split(" ", QString::SkipEmptyParts);
              if( list.size() >= 3 )
              {
                /*
                gfl::core::Debug::PrintConsole("[%s][%s][%s]\n"
                      ,list[0].toLocal8Bit().constData()
                      ,list[1].toLocal8Bit().constData()
                      ,list[2].toLocal8Bit().constData());
                */

                if( keyWord.size() == 0 || 
                    list[0].left( keyWord.size() ) == keyWord )
                {
  						    cb->addItem( list[0] );
  						    cb->GetLabelList( )->append( list[0] );
                
                  //再編集時の初期値かチェックする
                  if( defName && defName == list[0] )
                  {
    					      cb->setCurrentIndex(idx);
                  }
                  idx++;

                }

                /*
                QString label = list[0].right(list[0].size()-6-baseName.size()); //[GARC_ファイル名_]を排除
  						  cb->addItem( label );
  						  cb->GetLabelList( )->append( list[0] );
                
                //再編集時の初期値かチェックする
                if( defName && defName == list[0] )
                {
    					    cb->setCurrentIndex(idx);
                }
                idx++;
                */
              }
            }
            
            break;
          }

          //gfl::core::Debug::PrintConsole("[%s]\n",str.toLocal8Bit().constData() );
        }
			  layout->addWidget( lb );
        QHBoxLayout* hlay = new QHBoxLayout;
  			QLineEdit* le = new QLineEdit();
        hlay->addWidget( cb );
        hlay->addWidget( le );
  			layout->addLayout( hlay );

        connect( le, SIGNAL( textChanged(const QString &) ), cb, SLOT( SearchText(const QString &) ) );
        connect( le, SIGNAL( textChanged(const QString &) ), this, SLOT( SetSearchStr(const QString &) ) );

        QList<QWidget*> lst;
			  lst << cb;
			  m_WidgetMap[node->name( )] = lst;
      }
		  else
      {
        //gfl::core::Debug::PrintConsole("gaix [%s] is not found!!\n",defNode->AttributeString("value"));
      }
    } //( type == "FILE_CSID_MID" )
    else if( type == "FILE_MESSAGE" )
    {
      //ファイルを開く
      QString path = ConvertPathDefine( path_def_node , node->AttributeString( "file" ) );
      QFile gaixFile(path);
      QString keyWord = node->AttributeString("keyword");
      if( gaixFile.open(QIODevice::ReadOnly) )
      {
			  ComboBox* cb = new ComboBox;
        cb->SetMode(1);
        QFileInfo fileInfo( gaixFile );
        QString baseName = fileInfo.completeBaseName();
        QTextStream gaixText(&gaixFile); 
        
        //再編集時用処理
        char* defName = NULL;
        if( load_node )
        {
		      gfl::xml::Node* c_node = load_node->FirstNode( node->name( ) );
		      if(c_node)
          {
			      defName = (c8*)c_node->AttributeString( "value0" );
          }
        }
        int idx = 0;
        int readSeq = 0;

        //デフォルト追加
        int defNo = node->AttributeInteger("use_default",0);
        if( defNo != 0 )
        {
          QString defStr("Default");
			    cb->addItem( defStr );
			    cb->GetLabelList( )->append( defStr );
        
          //再編集時の初期値かチェックする
          if( defName && defName == defStr )
          {
			      cb->setCurrentIndex(idx);
          }
          idx++;
        }



        while( !gaixText.atEnd() )
        {
          //行読み
          QString str = gaixText.readLine();
          
          switch( readSeq )
          {
          case 0:
            //とりあえずdefine行まで読む
            if( str.left(7) == "#define" )
            {
              readSeq = 1; 
              //1行空読み
              str = gaixText.readLine();
            }
            break;
          case 1:
            //enum行終端チェック
            if( str.size() <= 0 )
            {
              readSeq = 2; 
            }
            else
            {
              //MessageHeader名解釈
              QStringList list = str.split("\t", QString::SkipEmptyParts);
              if( list.size() >= 2 )
              {
                /*
                gfl::core::Debug::PrintConsole("[%s][%s][%s]\n"
                      ,list[0].toLocal8Bit().constData()
                      ,list[1].toLocal8Bit().constData()
                      ,list[2].toLocal8Bit().constData());
                */

                if( keyWord.size() == 0 || 
                    list[1].left( keyWord.size() ) == keyWord )
                {
  						    cb->addItem( list[1] );
  						    cb->GetLabelList( )->append( list[1] );
                
                  //再編集時の初期値かチェックする
                  if( defName && defName == list[1] )
                  {
    					      cb->setCurrentIndex(idx);
                  }
                  idx++;

                }

                /*
                QString label = list[0].right(list[0].size()-6-baseName.size()); //[GARC_ファイル名_]を排除
  						  cb->addItem( label );
  						  cb->GetLabelList( )->append( list[0] );
                
                //再編集時の初期値かチェックする
                if( defName && defName == list[0] )
                {
    					    cb->setCurrentIndex(idx);
                }
                idx++;
                */
              }
            }
            
            break;
          }

          //gfl::core::Debug::PrintConsole("[%s]\n",str.toLocal8Bit().constData() );
        }
			  layout->addWidget( lb );
				layout->addWidget( cb );

        QList<QWidget*> lst;
			  lst << cb;
			  m_WidgetMap[node->name( )] = lst;
      }
		  else
      {
        //gfl::core::Debug::PrintConsole("gaix [%s] is not found!!\n",defNode->AttributeString("value"));
      }
    } //( type == "FILE_MESSAGE" )
    else if( type == "FILE_DIALOG" || type == "FILE_DIALOG_COMBOBOX" || type == "COMBOBOX_HEADER" ){ // よくわからん…
			ExtDialog* pb = new ExtDialog( node->AttributeString( "ext" ), node->name( ) );

			QHBoxLayout* hlay = new QHBoxLayout;
			hlay->addWidget( lb );
			hlay->addWidget( pb );
			layout->addLayout( hlay );

			connect( pb, SIGNAL( clicked( bool ) ), this, SLOT( Clicked( bool ) ) );

			QList<QWidget*> lst;
			lst << pb;
			m_WidgetMap[node->name( )] = lst;
    }
    else if( type == "SPLIT_LINE" )
    {
			QFrame* line = new QFrame;
      line->setFrameShape(QFrame::HLine);
      line->setFrameShadow(QFrame::Sunken);

			layout->addWidget( line );
		}
    else {
			QMessageBox::critical( this, "Error", "unsupported type " + type );
			//				gfl::core::Debug::PrintConsole( "else ? %s\n", type.toAscii( ).data( ) );
		}
	}

  //コメント
  {
    layout->addWidget( new QLabel( QString( "コメント" ) ));
    m_comment = new QTextEdit();
    m_comment->setFixedHeight( 51 );//大体３行
    m_comment->setTabChangesFocus(true);  //Tabで切り替えられるように
    m_comment->setPlainText( m_pLoadNode->AttributeString( XML_NAME_COMMAND_ATTR_COMMENT ) );
    layout->addWidget( m_comment );
  }
  //
  
  //ボタン群
  {
		QHBoxLayout* hlay = new QHBoxLayout;
    QPushButton *colBtn = new QPushButton( "color" , this );
    QDialogButtonBox *buttonBox;

    mColLbl =new QLabel(this);
    QPalette pal;
    pal.setColor( QPalette::Background , mCol );
    mColLbl->setAutoFillBackground(true);
    mColLbl->setPalette(pal);
    mColLbl->setMinimumWidth(24);

	  buttonBox = new QDialogButtonBox( this );
	  buttonBox->setOrientation( Qt::Horizontal );
	  buttonBox->setStandardButtons( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );

    hlay->addWidget( colBtn );
    hlay->addWidget( mColLbl );
    QSpacerItem *spacer1 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
    hlay->addItem( spacer1 );
    hlay->addWidget( buttonBox );

    layout->addLayout( hlay );

	  QObject::connect( buttonBox, SIGNAL( accepted( ) ), this, SLOT( accept( ) ) );
	  QObject::connect( buttonBox, SIGNAL( rejected( ) ), this, SLOT( rejected( ) ) );
	  QObject::connect( colBtn, SIGNAL( clicked( ) ), this, SLOT( callColor( ) ) );
  }

	setLayout( layout );

  //外からexecで呼びます
	//show( );
}

//==============================================================================
//==============================================================================
void MacroDialog::accept( )
{
/*
  if( m_pLoadNode )
  {
		GetCommandDialog( )->GetDataNode( )->remove_node( m_pLoadNode );
	}
*/
  
  //フレームとコメント
  {
    int sFrame = m_frame[0]->value();
    int eFrame = m_frame[1]->value();
    m_pLoadNode->SetAttribute( XML_NAME_COMMAND_ATTR_SFRAME , sFrame );
    m_pLoadNode->SetAttribute( XML_NAME_COMMAND_ATTR_EFRAME , eFrame );

    QString str = m_comment->toPlainText();

    m_pLoadNode->SetAttribute( XML_NAME_COMMAND_ATTR_COMMENT , str );
  }

  {
    gfl::xml::Node* befNode = m_pLoadNode->FirstNode( m_pCommandNode->name( ) );
    if( befNode )
    {
      m_pLoadNode->remove_node( befNode );
    }
  }
  {
    int r,g,b;
    mCol.getRgb(&r,&g,&b);
    m_pLoadNode->SetAttribute( XML_NAME_COMMAND_ATTR_COL_R , r );
    m_pLoadNode->SetAttribute( XML_NAME_COMMAND_ATTR_COL_G , g );
    m_pLoadNode->SetAttribute( XML_NAME_COMMAND_ATTR_COL_B , b );
  }

  gfl::xml::Node* node = m_pLoadNode->AddNode( m_pCommandNode->name( ));
	QMapIterator<QString, QList<QWidget*> > i( m_WidgetMap );
	while( i.hasNext( ) ){
		i.next( );

		gfl::xml::Node* c_node = node->AddNode( i.key( ) );

		for( s32 j = 0; j < i.value( ).size( ); ++j ){
			ComboBox* cb = dynamic_cast < ComboBox* >( i.value( ).at( j ) );
			QString val( "value" + QString::number( j ) );
			if( cb ){

        switch( cb->GetMode() )
        {
        case 0:
				  c_node->AddAttribute( val, cb->GetLabelList( )->at( cb->currentIndex( ) ) );
          break;
        case 1:
          QString target = cb->itemText( cb->currentIndex( ) ); //こっちだと ○○_hoge_bcmdl が □□○○_hoge_bcmdl に引っかかってしまう
          //QString target = cb->GetLabelList( )->value(cb->currentIndex( )); //こっちだとGARC_～が入るので大丈夫。
                                                                              //だけど、絞り込んだ状態のIndexから、全体を参照するため、絞込み時の選択が
          gfl::core::Debug::PrintConsole("[%s]\n",target.toLocal8Bit().constData() );
          for( int i=0 ; i < cb->GetLabelList( )->size() ; i++ )
          {
            QString listStr = cb->GetLabelList( )->value(i);
            gfl::core::Debug::PrintConsole("   [%s]\n",listStr.toLocal8Bit().constData() );
            if( listStr.contains( target ) == true )
            {
    				  c_node->AddAttribute( val, listStr );
              gfl::core::Debug::PrintConsole("Hit [%s]\n",listStr.toLocal8Bit().constData() );
              break;
            }
          }
          break;
        }
        continue;
			}
			QDoubleSpinBox* dsp = dynamic_cast < QDoubleSpinBox* >( i.value( ).at( j ) );
			if( dsp ){
				c_node->AddAttribute( val, dsp->value( ) );
				continue;
			}
			QSpinBox* sp = dynamic_cast < QSpinBox* >( i.value( ).at( j ) );
			if( sp ){
				c_node->AddAttribute( val, sp->value( ) );
				continue;
			}
			QCheckBox* chBox = dynamic_cast < QCheckBox* >( i.value( ).at( j ) );
			if( chBox ){
        if( chBox->checkState() == Qt::Checked )
        {
  				c_node->AddAttribute( val, 1 );
        }
        else
        {
  				c_node->AddAttribute( val, 0 );
        }
				continue;
			}
			QLineEdit* le = dynamic_cast < QLineEdit* >( i.value( ).at( j ) );
			if( le ){
				c_node->AddAttribute( val, le->text( ) );
				continue;
      }
		}

		//		gfl::core::Debug::PrintConsole( "%s %d\n", i.key().toAscii().data(), i.value().size() );
	}

	//GetCommandDialog( )->UpdateMacroList( );
  //外にQTDialogAcceptが返ってこなかった。
  QDialog::accept();
	//close( );
}


//==============================================================================
//==============================================================================
void MacroDialog::rejected( )
{
	// ensure exit?
	close( );
}

void MacroDialog::callColor( )
{
  QColorDialog colDiag(mCol,this);
	int ret = colDiag.exec();
	if( ret == QDialog::Accepted )
	{
    mCol = colDiag.selectedColor();
    QPalette pal;
    pal.setColor( QPalette::Background , mCol );
    mColLbl->setPalette(pal);
  }
}

//==============================================================================
//==============================================================================
void MacroDialog::Clicked( bool )
{
	ExtDialog* ext = ( ExtDialog* )sender( );
	QString str = QFileDialog::getOpenFileName( this, "File Open", QDir::currentPath( ), "EXT (*" + ext->text( ) + ")" );

	if( str.size( ) ){
		; // 
	}
}
//==============================================================================
//==============================================================================
c8* MacroDialog::GetInitValue( gfl::xml::Node* node, gfl::xml::Node* load_node, const s32 id , bool isStr )
{
	if( load_node ){
		gfl::xml::Node* c_node = load_node->FirstNode( node->name( ) );
    if( c_node )
    {
  		return ( c8* )c_node->AttributeString( "value" + QString::number( id ) );
    }
	}
  {
		c8* init = node->AttributeString( "init" + QString::number( id ) );
		if( isalpha( *init ) )
    {
      if( isStr == true )
      {
        return init;
      }
      else
      {
        return "0";
      }
		}
		return( c8* )init;
	}
}
