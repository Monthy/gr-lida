/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006  Pedro A. Garcia Rosado Aka Monthy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 *
**/

#include "grlida_importar_juego.h"
#include "grlida_importpath.h"
#include "ui_login_url.h" // Qt 4.3.x

frmImportarJuego::frmImportarJuego(QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);
// Conecta los distintos botones
	connect( ui.btnUpdateList , SIGNAL(clicked()), this, SLOT(on_btnUpdateList()));
	connect( ui.btnPrevious   , SIGNAL(clicked()), this, SLOT(on_btnPrevious()));
	connect( ui.btnNext       , SIGNAL(clicked()), this, SLOT(on_btnNext()));
	connect( ui.btnOk         , SIGNAL(clicked()), this, SLOT(on_btnOk()));
	connect( ui.twListaJuegos , SIGNAL( itemClicked( QTreeWidgetItem*, int )), this, SLOT( on_treeWidget_clicked(QTreeWidgetItem *) ));
	connect( ui.twListaJuegos , SIGNAL( itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( on_treeWidget_Dblclicked(QTreeWidgetItem *) ));
	connect( ui.cbxDbXml      , SIGNAL(activated(const QString &)), this, SLOT( on_changeURL_XML(const QString &)));
	connect( ui.btnDirFileXML , SIGNAL(clicked()), this, SLOT(on_btnDirFileXML()));

	stHomeDir = fGrl.GRlidaHomePath();	// directorio de trabajo del GR-lida
	xml_ListaJuegos = stHomeDir + "tmp_lista_juegos.xml";
	xml_InfoJuegos  = stHomeDir + "tmp_info_juego.xml";
	
	stTheme = fGrl.ThemeGrl();
	setTheme();

	QSettings settings( stHomeDir + "GR-lida.conf", QSettings::IniFormat); 
	settings.beginGroup("OpcGeneral");
		temp_url_xmldb = settings.value("url_xmldb", "http://laisladelabandoware.es/" ).toString(); // GR-lida
		DirBaseGames   = settings.value("DirBaseGames", "").toString() ;
	settings.endGroup();

	if( url_xmldb.isEmpty() )
		url_xmldb = "http://laisladelabandoware.es/";
	else
		url_xmldb = fGrl.url_correcta( temp_url_xmldb );

	ui.cbxDbXml->clear();
	ui.cbxDbXml->addItem( QIcon(stTheme+"img16/edit_enlace.png"), url_xmldb);

	QStringList cbx_Lista, cbx_ListaTemp;
	QPixmap pixmap;
	QFile file_xmldb( stHomeDir + "datos/xmldb.txt" );
	if(file_xmldb.open(QIODevice::ReadOnly)!=0 )
	{
		QTextStream in(&file_xmldb);
		in.setCodec("UTF-8");
		while( !in.atEnd() )
			cbx_ListaTemp << in.readLine();

		for( int i = 0; i < cbx_ListaTemp.count(); i++ )
		{
			cbx_Lista = cbx_ListaTemp[i].split( "|" );
			pixmap.load(stTheme+"img16/"+cbx_Lista.value(1)+".png");
			if( pixmap.isNull() )
				pixmap.load(stTheme+"img16/sinimg.png");
			ui.cbxDbXml->addItem( QIcon( pixmap ), fGrl.url_correcta( cbx_Lista.value(0) ) );
		}
	}
	file_xmldb.close();

	if( url_xmldb.isEmpty() )
		ui.cbxDbXml->setCurrentIndex( 0 );
	else
		ui.cbxDbXml->setCurrentIndex( ui.cbxDbXml->findText(url_xmldb, Qt::MatchContains) );

	progressDialog = new QProgressDialog(this);
	http = new QHttp(this);

	connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(httpRequestFinished(int, bool)));
	connect(http, SIGNAL(dataReadProgress(int, int)), this, SLOT(updateDataReadProgress(int, int)));
	connect(http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)), this, SLOT(readResponseHeader(const QHttpResponseHeader &)));
	connect(http, SIGNAL(authenticationRequired(const QString &, quint16, QAuthenticator *)), this, SLOT(slotAuthenticationRequired(const QString &, quint16, QAuthenticator *))); // Qt 4.3.x
	connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));

	ui.btnUpdateList->setDefault(true);
	ui.wizardImport->setCurrentIndex(0);

	indx_fin_descarga = 0;
	xml_read( xml_ListaJuegos );

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmImportarJuego::~frmImportarJuego()
{
	delete http;
	delete file;
	delete progressDialog;
}

void frmImportarJuego::setTheme()
{
	ui.btnOk->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.btnCancelar->setIcon( QIcon(stTheme+"img16/cancelar.png") );
	ui.btnPrevious->setIcon( QIcon(stTheme+"img16/mp_rebobinar_atras.png") );
	ui.btnNext->setIcon( QIcon(stTheme+"img16/mp_rebobinar_adelante.png") );
	ui.btnUpdateList->setIcon( QIcon(stTheme+"img16/actualizar.png") );
	ui.btnDirFileXML->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDirFileXML_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.tabWidget->setTabIcon(0, QIcon(stTheme+"img16/importar.png") );
	ui.tabWidget->setTabIcon(1, QIcon(stTheme+"img16/edit_enlace.png") );
	ui.tabWidget->setTabIcon(2, QIcon(stTheme+"img16/drive_hd.png") );
}

void frmImportarJuego::on_changeURL_XML(const QString &url)
{
	url_xmldb = fGrl.url_correcta( url );
}

void frmImportarJuego::on_btnPrevious()
{
	if( ui.rbt_desde_URL->isChecked() )
	{
		ui.wizardImport->setCurrentIndex(ui.wizardImport->currentIndex()-1);
		ui.btnNext->setEnabled( true );
		if(ui.wizardImport->currentIndex() == 1 )
		{
			ui.twMontajes->clear();
			ui.twDatosFiles->clear();
			ui.twDatosURL->clear();
			ui.lb_thumbs->setPixmap( QPixmap(stTheme+"images/juego_sin_imagen.png") );

			for(int i = 0; i < ui.twDatosInfo->topLevelItemCount(); i++)
				ui.twDatosInfo->topLevelItem(i)->setText(1,"");

			ui.btnOk->setEnabled(false);
		}
		if(ui.wizardImport->currentIndex() < 1 )
			ui.btnPrevious->setEnabled(false);
	}
	else if( ui.rbt_desde_XML->isChecked() )
	{
		ui.wizardImport->setCurrentIndex(0);
		ui.btnOk->setEnabled(false);
		ui.btnNext->setEnabled( true );
		ui.btnPrevious->setEnabled(false);
	}
}

void frmImportarJuego::on_btnNext()
{
	bool siguiente = false;

	if( ui.rbt_desde_URL->isChecked() )
	{
		siguiente = true;

		if( ui.wizardImport->currentIndex() == 1 )
		{
			if((ui.twListaJuegos->isItemSelected(ui.twListaJuegos->currentItem())) && (ui.twListaJuegos->currentItem()->text(1)!=""))
			{
				indx_fin_descarga = 1;
				url_filed = url_xmldb + "grlidadb.php?ver=juego&gid="+ui.twListaJuegos->currentItem()->text(1)+"&id_emu="+ui.twListaJuegos->currentItem()->text(5)+"&tipo_emu="+ui.twListaJuegos->currentItem()->text(6);

				ui.twMontajes->clear();
				ui.twDatosFiles->clear();
				ui.twDatosURL->clear();

				downloadFile( url_filed, xml_InfoJuegos );

				if(ui.twListaJuegos->currentItem()->text(6)=="dosbox")
					ui.tabWidget->setTabEnabled(2,true);
				else
					ui.tabWidget->setTabEnabled(2,false);

				siguiente = true;
			} else
				siguiente = false;
		}
		if( siguiente == true )
		{
			ui.btnPrevious->setEnabled(true);
			ui.wizardImport->setCurrentIndex(ui.wizardImport->currentIndex()+1);
		}
		if( ui.wizardImport->currentIndex() >= 2)
		{
			ui.btnOk->setEnabled(true);
			ui.btnOk->setFocus();
			ui.btnNext->setEnabled(false);
		}
	}
	else if( ui.rbt_desde_XML->isChecked() )
	{
		siguiente = false;
		QString tipo_file;
		
		tipo_file = ui.DirFileXML->text();

		if( tipo_file.endsWith(".xml") )
		{
			indx_fin_descarga = 1;

			ui.twMontajes->clear();
			ui.twDatosFiles->clear();
			ui.twDatosURL->clear();

			xml_read( ui.DirFileXML->text() );
			MostrarDatosJuegoInfo();
			siguiente = true;
		}
		else if( tipo_file.endsWith(".prof") )
		{
			ui.twMontajes->clear();
			ui.twDatosFiles->clear();
			ui.twDatosURL->clear();

			setProfile_DFend( tipo_file );
			MostrarDatosJuegoInfo();
			siguiente = true;
		} else
			siguiente = false;

		if( siguiente == true )
		{
			ui.wizardImport->setCurrentIndex(2);
			ui.btnOk->setEnabled(true);
			ui.btnOk->setFocus();
			ui.btnNext->setEnabled(false);
			ui.btnPrevious->setEnabled(true);
		}
	}
}

void frmImportarJuego::on_btnOk()
{
	QFile file_thumbs, file_cover_front, file_cover_back;

	frmImportPath * ImportPathNew = new frmImportPath();

	ImportPathNew->ui.gBox_path_emu->setEnabled( false );
	ImportPathNew->ui.gBox_path_emu->setTitle("Path");

	img_thumbs.clear();
	img_cover_front.clear();
	img_cover_back.clear();
	
	if(DatosJuego["thumbs"]=="null" || DatosJuego["thumbs"].isEmpty() )
	{
		img_thumbs = "null";
		ImportPathNew->ui.txtDatosPath_Thumbs->setText("");
		ImportPathNew->ui.txtDatosPath_Thumbs->setEnabled(false);
		ImportPathNew->ui.btnDirPath_Datos_Thumbs->setEnabled(false);
	} else {
		img_thumbs = DatosJuego["thumbs"];
		ImportPathNew->ui.txtDatosPath_Thumbs->setText( stHomeDir + "thumbs/thumbs_"+DatosJuego["thumbs"] );
		ImportPathNew->ui.txtDatosPath_Thumbs->setEnabled(true);
		ImportPathNew->ui.btnDirPath_Datos_Thumbs->setEnabled(true);
	}

	if(DatosJuego["cover_front"]=="null" || DatosJuego["cover_front"].isEmpty() )
	{
		img_cover_front = "null";
		ImportPathNew->ui.txtDatosPath_CoverFront->setText("");
		ImportPathNew->ui.txtDatosPath_CoverFront->setEnabled(false);
		ImportPathNew->ui.btnDirPath_Datos_CoverFront->setEnabled(false);
	} else {
		if( !file_cover_front.exists( stHomeDir + "temp/cover_front_" + DatosJuego["cover_front"] ) )
		{
			indx_fin_descarga = 3;
			url_filed = url_xmldb + "images/covers/large/" + DatosJuego["cover_front"];
			downloadFile( url_filed, stHomeDir + "temp/cover_front_" + DatosJuego["cover_front"]);
		}
		img_cover_front = DatosJuego["cover_front"];
		ImportPathNew->ui.txtDatosPath_CoverFront->setText( stHomeDir + "covers/cover_front_" + DatosJuego["cover_front"] );
		ImportPathNew->ui.txtDatosPath_CoverFront->setEnabled(true);
		ImportPathNew->ui.btnDirPath_Datos_CoverFront->setEnabled(true);
	}

	if(DatosJuego["cover_back"]=="null" || DatosJuego["cover_back"]=="")
	{
		img_cover_back = "null";
		ImportPathNew->ui.txtDatosPath_CoverBack->setText("");
		ImportPathNew->ui.txtDatosPath_CoverBack->setEnabled(false);
		ImportPathNew->ui.btnDirPath_Datos_CoverBack->setEnabled(false);
	} else {
		if( !file_cover_front.exists( stHomeDir + "covers/cover_back_" + DatosJuego["cover_back"] ) )
		{
			indx_fin_descarga = 3;
			url_filed = url_xmldb + "images/covers/large/" + DatosJuego["cover_back"];
			downloadFile( url_filed, stHomeDir + "temp/cover_back_" + DatosJuego["cover_back"]);
		}
		img_cover_back = DatosJuego["cover_back"];
		ImportPathNew->ui.txtDatosPath_CoverBack->setText( stHomeDir + "covers/cover_back_" + DatosJuego["cover_back"] );
		ImportPathNew->ui.txtDatosPath_CoverBack->setEnabled(true);
		ImportPathNew->ui.btnDirPath_Datos_CoverBack->setEnabled(true);
	}

	if(DatosJuego["tipo_emu"] =="dosbox")
	{
		ImportPathNew->ui.gBox_path_emu->setEnabled( true );
		ImportPathNew->ui.gBox_path_emu->setTitle("Path - DOSBox");
		ImportPathNew->ui.wizardPath->setCurrentIndex(0);
		ImportPathNew->ui.txtPath_Dbx_1->setText( QDir::toNativeSeparators( DatosDosBox["path_conf"] ) );			// path_conf
		ImportPathNew->ui.txtPath_Dbx_2->setText( QDir::toNativeSeparators( DatosDosBox["path_exe"].replace("{DirBaseGames}", DirBaseGames+"/") ) );	// path_exe
		ImportPathNew->ui.txtPath_Dbx_3->setText( QDir::toNativeSeparators( DatosDosBox["path_setup"].replace("{DirBaseGames}", DirBaseGames+"/") ) );	// path_setup
		ImportPathNew->ui.txtPath_Dbx_4->setText( QDir::toNativeSeparators( DatosDosBox["dosbox_language"] ) );		// dosbox_language
		ImportPathNew->ui.txtPath_Dbx_5->setText( QDir::toNativeSeparators( DatosDosBox["sdl_mapperfile"] ) );		// sdl_mapperfile
		ImportPathNew->ui.txtPath_Dbx_6->setText( QDir::toNativeSeparators( DatosDosBox["gus_ultradir"] ) );		// gus_ultradir
		ImportPathNew->ui.txtPath_Dbx_7->setText( QDir::toNativeSeparators( DatosDosBox["dosbox_captures"] ) );		// dosbox_captures
		ImportPathNew->ui.txtPath_Dbx_8->setText( QDir::toNativeSeparators( DatosDosBox["path_sonido"] ) );			// path_sonido
	}

	if(DatosJuego["tipo_emu"]=="scummvm")
	{
		ImportPathNew->ui.gBox_path_emu->setEnabled( true );
		ImportPathNew->ui.gBox_path_emu->setTitle("Path - ScummVM");
		ImportPathNew->ui.wizardPath->setCurrentIndex(1);
		ImportPathNew->ui.txtPath_Svm_1->setText( QDir::toNativeSeparators( DatosScummvm["path"].replace("{DirBaseGames}", DirBaseGames+"/") ) );		// path
		ImportPathNew->ui.txtPath_Svm_2->setText( QDir::toNativeSeparators( DatosScummvm["path_save"].replace("{DirBaseGames}", DirBaseGames+"/") ) );	// path_save
		ImportPathNew->ui.txtPath_Svm_3->setText( QDir::toNativeSeparators( DatosScummvm["path_setup"].replace("{DirBaseGames}", DirBaseGames+"/") ) );	// path_setup
		ImportPathNew->ui.txtPath_Svm_4->setText( QDir::toNativeSeparators( DatosScummvm["path_extra"].replace("{DirBaseGames}", DirBaseGames+"/") ) );	// path_extra
		ImportPathNew->ui.txtPath_Svm_5->setText( QDir::toNativeSeparators( DatosScummvm["path_capturas"] ) );	// path_capturas
		ImportPathNew->ui.txtPath_Svm_6->setText( QDir::toNativeSeparators( DatosScummvm["path_sonido"] ) );	// path_sonido
	}

	if(DatosJuego["tipo_emu"]=="vdmsound")
	{
		ImportPathNew->ui.gBox_path_emu->setEnabled( true );
		ImportPathNew->ui.gBox_path_emu->setTitle("Path - VDMSound");
		ImportPathNew->ui.wizardPath->setCurrentIndex(2);
		ImportPathNew->ui.txtPath_Vdms_1->setText( QDir::toNativeSeparators( DatosVDMSound["path_conf"] ) );	// path_conf
		ImportPathNew->ui.txtPath_Vdms_2->setText( QDir::toNativeSeparators( DatosVDMSound["path_exe"].replace("{DirBaseGames}", DirBaseGames+"/") ) );	// path_exe
	}

	if( ImportPathNew->exec() == QDialog::Accepted )
	{
		DatosJuego["thumbs"]      = ImportPathNew->ui.txtDatosPath_Thumbs->text();		// thumbs
		DatosJuego["cover_front"] = ImportPathNew->ui.txtDatosPath_CoverFront->text();	// cover_front
		DatosJuego["cover_back"]  = ImportPathNew->ui.txtDatosPath_CoverBack->text();	// cover_back

		if(DatosJuego["tipo_emu"]=="dosbox")
		{
			DatosDosBox["path_conf"]       = ImportPathNew->ui.txtPath_Dbx_1->text();	// path_conf
			DatosDosBox["path_exe"]        = ImportPathNew->ui.txtPath_Dbx_2->text();	// path_exe
			DatosDosBox["path_setup"]      = ImportPathNew->ui.txtPath_Dbx_3->text();	// path_setup
			DatosDosBox["dosbox_language"] = ImportPathNew->ui.txtPath_Dbx_4->text();	// path_lang
			DatosDosBox["sdl_mapperfile"]  = ImportPathNew->ui.txtPath_Dbx_5->text();	// path_mapper
			DatosDosBox["gus_ultradir"]    = ImportPathNew->ui.txtPath_Dbx_6->text();	// path_gus
			DatosDosBox["dosbox_captures"] = ImportPathNew->ui.txtPath_Dbx_7->text();	// path_capturas
			DatosDosBox["path_sonido"]     = ImportPathNew->ui.txtPath_Dbx_8->text();	// path_sonido
		}

		if(DatosJuego["tipo_emu"]=="scummvm")
		{
			DatosScummvm["path"]          = ImportPathNew->ui.txtPath_Svm_1->text();	// path
			DatosScummvm["path_save"]     = ImportPathNew->ui.txtPath_Svm_2->text();	// path_save
			DatosScummvm["path_setup"]    = ImportPathNew->ui.txtPath_Svm_3->text();	// path_setup
			DatosScummvm["path_extra"]    = ImportPathNew->ui.txtPath_Svm_4->text();	// path_extra
			DatosScummvm["path_capturas"] = ImportPathNew->ui.txtPath_Svm_5->text();	// path_capturas
			DatosScummvm["path_sonido"]   = ImportPathNew->ui.txtPath_Svm_6->text();	// path_sonido
		}

		if(DatosJuego["tipo_emu"]=="vdmsound")
		{
			DatosVDMSound["path_conf"] = ImportPathNew->ui.txtPath_Vdms_1->text();		// path_conf
			DatosVDMSound["path_exe"]  = ImportPathNew->ui.txtPath_Vdms_2->text();		// path_exe
		}

		if(DatosJuego["thumbs"]!="null" || !DatosJuego["thumbs"].isEmpty() )
		{
			if( file_thumbs.exists( stHomeDir + "temp/thumbs_" + img_thumbs ) )
			{
				file_thumbs.copy( stHomeDir + "temp/thumbs_" + img_thumbs, DatosJuego["thumbs"] );
				//file_thumbs.remove( stHomeDir + "temp/thumbs_" + img_thumbs );
			}
		}

		if(DatosJuego["cover_front"]!="null" || !DatosJuego["cover_front"].isEmpty() )
		{
			if( file_cover_front.exists( stHomeDir + "temp/cover_front_" + img_cover_front ) )
			{
				file_cover_front.copy( stHomeDir + "temp/cover_front_" + img_cover_front, DatosJuego["cover_front"] );
				file_cover_front.remove( stHomeDir + "temp/cover_front_" + img_cover_front );
			}
		}

		if(DatosJuego["cover_back"]!="null" || !DatosJuego["cover_back"].isEmpty() )
		{
			if( file_cover_back.exists( stHomeDir + "temp/cover_back_" + img_cover_back ) )
			{
				file_cover_back.copy( stHomeDir + "temp/cover_back_" + img_cover_back , DatosJuego["cover_back"] );
				file_cover_back.remove( stHomeDir + "temp/cover_back_" + img_cover_back );
			}
		}

		//if( ui.cbxDbXml->currentText()!="")
		//	url_xmldb = fGrl.url_correcta(ui.cbxDbXml->currentText()); else  url_xmldb = "";

		QSettings settings( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
		settings.beginGroup("OpcGeneral");
			settings.setValue("url_xmldb", url_xmldb );
		settings.endGroup();

		QDialog::accept();
	} else
		QDialog::rejected();
}

void frmImportarJuego::on_btnDirFileXML()
{
	ui.DirFileXML->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), ui.DirFileXML->text(), ui.DirFileXML->text(), tr("Soportados")+" (*.xml *.prof);;DB GR-lida (*.xml);;D-Fend Reloaded (*.prof);;"+tr("Todos los archivo")+" (*)", 0, false) );
}

void frmImportarJuego::on_btnUpdateList()
{
	ui.twListaJuegos->clear();
	indx_fin_descarga = 0;

	if( ui.cbxDbXml->currentText()!="")
		url_filed = fGrl.url_correcta( ui.cbxDbXml->currentText() ) + "grlidadb.xml";
	else
		url_filed = url_xmldb + "grlidadb.xml";

	downloadFile( url_filed, xml_ListaJuegos );
}

void frmImportarJuego::on_xml_open()
{
	ui.twListaJuegos->clear();
	indx_fin_descarga = 0;
	xml_read( xml_ListaJuegos );
}

bool frmImportarJuego::xml_read(QString fileName)
{
	QString errorStr;
	int errorLine;
	int errorColumn;
	if(!fileName.isEmpty())
	{
		QFile file(fileName);
		if(file.exists())
		{
			if(file.open(QFile::ReadOnly | QFile::Text))
			{
				if(!domDocument.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
				{
					QMessageBox::information(window(), "GR-lida DB",
						tr("Error en Parse en la linea %1, columna %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr));
					return false;
				}

				QDomElement root = domDocument.documentElement();
				if(root.tagName() != "grlida_db")
				{
					QMessageBox::information(window(), "GR-lida DB", tr("No es un archivo de")+ " GR-lida DB");
					return false;
				}
				else if(root.hasAttribute("version") && root.attribute("version") != "0.1.0")
				{
					QMessageBox::information(window(), "GR-lida DB", tr("El archivo no corresponde a la versión") + " 0.1.0");
					return false;
				}

				QDomElement child = root.firstChildElement("lista_db");
				while(!child.isNull())
				{
					parseListaJuegos( child );
					child = child.nextSiblingElement("lista_db");
				}
				return true;
			} else {
				QMessageBox::warning(this, "GR-lida DB", tr("No se ha podido leer el archivo %1:\n%2.").arg(fileName).arg(file.errorString()));
				return false;
			}
		} else
			return false;
	}
	return true;
}

void frmImportarJuego::parseListaJuegos(const QDomElement &element)
{
	if( indx_fin_descarga == 0 )
	{
		ui.lb_infoTitulo->setText(      element.firstChildElement("title").text()         );
		ui.lb_infoLink->setText(        element.firstChildElement("link").text()          );
		ui.lb_infoBuildDate->setText(   element.firstChildElement("lastBuildDate").text() );
		ui.lb_infoDescripcion->setText( element.firstChildElement("description").text()   );
		ui.lbl_DateFile->setText(       element.firstChildElement("lastBuildDate").text() );
		ui.lbl_DateFileLast->setText("-");

		QDomElement child = element.firstChildElement();
		while(!child.isNull())
		{
			if(child.tagName() == "lista_db")
				parseListaJuegos(child);
			else if(child.tagName() == "juego")
			{
				QTreeWidgetItem *childItem = new QTreeWidgetItem(ui.twListaJuegos);
				if(child.firstChildElement("tipo_emu").text()=="datos")
					childItem->setIcon(0, QIcon(stTheme+"img24/emu_datos.png"));
				else if(child.firstChildElement("tipo_emu").text()=="dosbox")
					childItem->setIcon(0, QIcon(stTheme+"img24/emu_dbx.png"));
				else if(child.firstChildElement("tipo_emu").text()=="scummvm")
					childItem->setIcon(0, QIcon(stTheme+"img24/emu_svm.png"));
				else if(child.firstChildElement("tipo_emu").text()=="vdmsound")
					childItem->setIcon(0, QIcon(stTheme+"img24/emu_vdms.png"));
				else 
					childItem->setIcon(0, QIcon(stTheme+"img24/emu_sin_imagen.png"));

				childItem->setText(0, child.firstChildElement("titulo").text());
				childItem->setText(1, child.attribute("ID") );
				childItem->setText(2, child.firstChildElement("thumbs").text() );
				childItem->setText(3, child.firstChildElement("descripcion").text() );
				childItem->setText(4, child.firstChildElement("pubDate").text() );
				childItem->setText(5, child.firstChildElement("id_emu").text() );
				childItem->setText(6, child.firstChildElement("tipo_emu").text() );
			}
			//else if(child.tagName() == "separator")
			//{
			//	QTreeWidgetItem *childItem = new QTreeWidgetItem(ui.twListaJuegos);
			//	childItem->setText(0, QString(30, 0xB7));
			//}
			child = child.nextSiblingElement();
		}
	}
	if( indx_fin_descarga == 1 )
	{
		ui.lb_usuario_1->setText("");
		ui.lb_usuario_2->setText("");
		ui.lb_usuario_3->setText("");
		QDomElement child = element.firstChildElement();

		while(!child.isNull())
		{
			if(child.tagName() == "lista_db")
			{
				parseListaJuegos(child);
			}
			else if(child.tagName() == "datos")
			{
				ui.lb_titulo_juego->setText(child.firstChildElement("titulo").text());
				ui.lb_usuario_1->setText(child.firstChildElement("usuario").text()  );

				DatosJuego.clear();
				DatosJuego["icono"]         = child.firstChildElement("icono").text();			// icono
				DatosJuego["titulo"]        = child.firstChildElement("titulo").text();			// titulo
				DatosJuego["subtitulo"]     = child.firstChildElement("subtitulo").text();		// subtitulo
				DatosJuego["genero"]        = child.firstChildElement("genero").text();			// genero
				DatosJuego["compania"]      = child.firstChildElement("compania").text();		// compania
				DatosJuego["desarrollador"] = child.firstChildElement("desarrollador").text();	// desarrollador
				DatosJuego["tema"]          = child.firstChildElement("tema").text();			// tema
				DatosJuego["idioma"]        = child.firstChildElement("idioma").text();			// idioma
				DatosJuego["formato"]       = child.firstChildElement("formato").text();		// formato
				DatosJuego["anno"]          = child.firstChildElement("anno").text();			// anno
				DatosJuego["numdisc"]       = child.firstChildElement("numdisc").text();		// numdisc
				DatosJuego["sistemaop"]     = child.firstChildElement("sistemaop").text();		// sistemaop
				DatosJuego["tamano"]        = child.firstChildElement("tamano").text();			// tamano
				DatosJuego["graficos"]      = child.firstChildElement("graficos").text();		// graficos
				DatosJuego["sonido"]        = child.firstChildElement("sonido").text();			// sonido
				DatosJuego["jugabilidad"]   = child.firstChildElement("jugabilidad").text();	// jugabilidad
				DatosJuego["original"]      = child.firstChildElement("original").text();		// original
				DatosJuego["estado"]        = child.firstChildElement("estado").text();			// estado
				DatosJuego["thumbs"]        = child.firstChildElement("thumbs").text();			// thumbs
				DatosJuego["cover_front"]   = child.firstChildElement("cover_front").text();	// cover_front
				DatosJuego["cover_back"]    = child.firstChildElement("cover_back").text();		// cover_back
				DatosJuego["fecha"]         = fGrl.HoraFechaActual();							// fecha d/m/a h:m:s
				DatosJuego["tipo_emu"]      = child.firstChildElement("tipo_emu").text();		// tipo_emu
				DatosJuego["comentario"]    = child.firstChildElement("comentario").text();		// comentario
				DatosJuego["favorito"]      = "false";											//favorito

				MostrarDatosJuegoInfo();

			}
			else if(child.tagName() == "datos_files")
			{
				QTreeWidgetItem *childItem_files = new QTreeWidgetItem(ui.twDatosFiles);
				childItem_files->setIcon(0, QIcon(stTheme+"img16/importar.png")					);
				childItem_files->setText(0, child.firstChildElement("nombre").text()		);
				childItem_files->setText(1, child.firstChildElement("crc").text()			);
				childItem_files->setText(2, child.firstChildElement("descripcion").text()	);
				childItem_files->setText(3, child.firstChildElement("size").text()			);
				childItem_files->setText(4, child.firstChildElement("path").text()			);

			}
			else if(child.tagName() == "scummvm")
			{
				ui.lb_usuario_3->setText(child.firstChildElement("usuario").text() );
				DatosScummvm.clear();
				DatosScummvm["game"]            = child.firstChildElement("game").text();			// game
				DatosScummvm["language"]        = child.firstChildElement("language").text();		// language
				DatosScummvm["subtitles"]       = child.firstChildElement("subtitles").text();		// subtitles
				DatosScummvm["platform"]        = child.firstChildElement("platform").text();		// platform
				DatosScummvm["gfx_mode"]        = child.firstChildElement("gfx_mode").text();		// gfx_mode
				DatosScummvm["render_mode"]     = child.firstChildElement("render_mode").text();	// render_mode
				DatosScummvm["fullscreen"]      = child.firstChildElement("fullscreen").text();		// fullscreen
				DatosScummvm["aspect_ratio"]    = child.firstChildElement("aspect_ratio").text();	// aspect_ratio
				DatosScummvm["path"]            = child.firstChildElement("path").text();			// path
				DatosScummvm["path_setup"]      = child.firstChildElement("path_setup").text();		// path_setup
				DatosScummvm["path_extra"]      = child.firstChildElement("path_extra").text();		// path_extra
				DatosScummvm["path_save"]       = child.firstChildElement("path_save").text();		// path_save
				DatosScummvm["path_capturas"]   = child.firstChildElement("path_capturas").text();	// path_capturas
				DatosScummvm["path_sonido"]     = child.firstChildElement("path_sonido").text();	// path_sonido
				DatosScummvm["music_driver"]    = child.firstChildElement("music_driver").text();	// music_driver
				DatosScummvm["enable_gs"]       = child.firstChildElement("enable_gs").text();		// enable_gs
				DatosScummvm["multi_midi"]      = child.firstChildElement("multi_midi").text();		// multi_midi
				DatosScummvm["native_mt32"]     = child.firstChildElement("native_mt32").text();	// native_mt32
				DatosScummvm["master_volume"]   = child.firstChildElement("master_volume").text();	// master_volume
				DatosScummvm["music_volume"]    = child.firstChildElement("music_volume").text();	// music_volume
				DatosScummvm["sfx_volume"]      = child.firstChildElement("sfx_volume").text();		// sfx_volume
				DatosScummvm["speech_volume"]   = child.firstChildElement("speech_volume").text();	// speech_volume
				DatosScummvm["tempo"]           = child.firstChildElement("tempo").text();			// tempo
				DatosScummvm["talkspeed"]       = child.firstChildElement("talkspeed").text();		// talkspeed
				DatosScummvm["debuglevel"]      = child.firstChildElement("debuglevel").text();		// debuglevel
				DatosScummvm["cdrom"]           = child.firstChildElement("cdrom").text();			// cdrom
				DatosScummvm["joystick_num"]    = child.firstChildElement("joystick_num").text();	// joystick_num
				DatosScummvm["output_rate"]     = "";												// output_rate
				DatosScummvm["midi_gain"]       = "100";											// midi_gain
				DatosScummvm["copy_protection"] = "false";											// copy_protection
				DatosScummvm["sound_font"]      = "";												// sound_font

			}
			else if(child.tagName() == "vdmsound")
			{
				ui.lb_usuario_4->setText(child.firstChildElement("usuario").text() );
				DatosVDMSound.clear();
				DatosVDMSound["path_conf"]     = child.firstChildElement("path_conf").text();		//
				DatosVDMSound["path_exe"]      = child.firstChildElement("path_exe").text();		//
				DatosVDMSound["program"]       = child.firstChildElement("program").text();			//
				DatosVDMSound["vdms_debug"]    = child.firstChildElement("vdms_debug").text();		//
				DatosVDMSound["winnt_dos"]     = child.firstChildElement("winnt_dos").text();		//
				DatosVDMSound["winnt_dosbox"]  = child.firstChildElement("winnt_dosbox").text();	//
				DatosVDMSound["winnt_storage"] = child.firstChildElement("winnt_storage").text();	//

			}
			else if(child.tagName() == "dosbox")
			{
				ui.lb_usuario_2->setText(child.firstChildElement("usuario").text() );
				DatosDosBox.clear();
				DatosDosBox["sdl_fullscreen"]        = child.firstChildElement("sdl_fullscreen").text();		// sdl_fullscreen
				DatosDosBox["sdl_fulldouble"]        = child.firstChildElement("sdl_fulldouble").text();		// sdl_fulldouble
				DatosDosBox["sdl_fullfixed"]         = child.firstChildElement("sdl_fullfixed").text();			// sdl_fullfixed
				DatosDosBox["sdl_fullresolution"]    = child.firstChildElement("sdl_fullresolution").text();	// sdl_fullresolution
				DatosDosBox["sdl_windowresolution"]  = child.firstChildElement("sdl_windowresolution").text();	// sdl_windowresolution
				DatosDosBox["sdl_output"]            = child.firstChildElement("sdl_output").text();			// sdl_output
				DatosDosBox["sdl_hwscale"]           = child.firstChildElement("sdl_hwscale").text();			// sdl_hwscale
				DatosDosBox["sdl_autolock"]          = child.firstChildElement("sdl_autolock").text();			// sdl_autolock
				DatosDosBox["sdl_sensitivity"]       = child.firstChildElement("sdl_sensitivity").text();		// sdl_sensitivity
				DatosDosBox["sdl_waitonerror"]       = child.firstChildElement("sdl_waitonerror").text();		// sdl_waitonerror
				DatosDosBox["sdl_priority"]          = child.firstChildElement("sdl_priority").text();			// sdl_priority
				DatosDosBox["sdl_mapperfile"]        = child.firstChildElement("sdl_mapperfile").text();		// sdl_mapperfile
				DatosDosBox["sdl_usescancodes"]      = child.firstChildElement("sdl_usescancodes").text();		// sdl_usescancodes
				DatosDosBox["dosbox_language"]       = child.firstChildElement("dosbox_language").text();		// dosbox_language
				DatosDosBox["dosbox_machine"]        = child.firstChildElement("dosbox_machine").text();		// dosbox_machine
				DatosDosBox["dosbox_captures"]       = child.firstChildElement("dosbox_captures").text();		// dosbox_captures
				DatosDosBox["dosbox_memsize"]        = child.firstChildElement("dosbox_memsize").text();		// dosbox_memsize
				DatosDosBox["render_frameskip"]      = child.firstChildElement("render_frameskip").text();		// render_frameskip
				DatosDosBox["render_aspect"]         = child.firstChildElement("render_aspect").text();			// render_aspect
				DatosDosBox["render_scaler"]         = child.firstChildElement("render_scaler").text();			// render_scaler
				DatosDosBox["cpu_core"]              = child.firstChildElement("cpu_core").text();				// cpu_core
				DatosDosBox["cpu_cycles"]            = child.firstChildElement("cpu_cycles").text();			// cpu_cycles
				DatosDosBox["cpu_cycleup"]           = child.firstChildElement("cpu_cycleup").text();			// cpu_cycleup
				DatosDosBox["cpu_cycledown"]         = child.firstChildElement("cpu_cycledown").text();			// cpu_cycledown
				DatosDosBox["mixer_nosound"]         = child.firstChildElement("mixer_nosound").text();			// mixer_nosound
				DatosDosBox["mixer_rate"]            = child.firstChildElement("mixer_rate").text();			// mixer_rate
				DatosDosBox["mixer_blocksize"]       = child.firstChildElement("mixer_blocksize").text();		// mixer_blocksize
				DatosDosBox["mixer_prebuffer"]       = child.firstChildElement("mixer_prebuffer").text();		// mixer_prebuffer
				DatosDosBox["midi_mpu401"]           = child.firstChildElement("midi_mpu401").text();			// midi_mpu401
				DatosDosBox["midi_intelligent"]      = child.firstChildElement("midi_intelligent").text();		// midi_intelligent
				DatosDosBox["midi_device"]           = child.firstChildElement("midi_device").text();			// midi_device
				DatosDosBox["midi_config"]           = child.firstChildElement("midi_config").text();			// midi_config
				DatosDosBox["midi_mt32rate"]         = child.firstChildElement("midi_mt32rate").text();			// midi_mt32rate
				DatosDosBox["sblaster_sbtype"]       = child.firstChildElement("sblaster_sbtype").text();		// sblaster_sbtype
				DatosDosBox["sblaster_sbbase"]       = child.firstChildElement("sblaster_sbbase").text();		// sblaster_sbbase
				DatosDosBox["sblaster_irq"]          = child.firstChildElement("sblaster_irq").text();			// sblaster_irq
				DatosDosBox["sblaster_dma"]          = child.firstChildElement("sblaster_dma").text();			// sblaster_dma
				DatosDosBox["sblaster_hdma"]         = child.firstChildElement("sblaster_hdma").text();			// sblaster_hdma
				DatosDosBox["sblaster_mixer"]        = child.firstChildElement("sblaster_mixer").text();		// sblaster_mixer
				DatosDosBox["sblaster_oplmode"]      = child.firstChildElement("sblaster_oplmode").text();		// sblaster_oplmode
				DatosDosBox["sblaster_oplrate"]      = child.firstChildElement("sblaster_oplrate").text();		// sblaster_oplrate
				DatosDosBox["gus_gus"]               = child.firstChildElement("gus_gus").text();				// gus_gus
				DatosDosBox["gus_gusrate"]           = child.firstChildElement("gus_gusrate").text();			// gus_gusrate
				DatosDosBox["gus_gusbase"]           = child.firstChildElement("gus_gusbase").text();			// gus_gusbase
				DatosDosBox["gus_irq1"]              = child.firstChildElement("gus_irq1").text();				// gus_irq1
				DatosDosBox["gus_irq2"]              = child.firstChildElement("gus_irq2").text();				// gus_irq2
				DatosDosBox["gus_dma1"]              = child.firstChildElement("gus_dma1").text();				// gus_dma1
				DatosDosBox["gus_dma2"]              = child.firstChildElement("gus_dma2").text();				// gus_dma2
				DatosDosBox["gus_ultradir"]          = child.firstChildElement("gus_ultradir").text();			// gus_ultradir
				DatosDosBox["speaker_pcspeaker"]     = child.firstChildElement("speaker_pcspeaker").text();		// speaker_pcspeaker
				DatosDosBox["speaker_pcrate"]        = child.firstChildElement("speaker_pcrate").text();		// speaker_pcrate
				DatosDosBox["speaker_tandy"]         = child.firstChildElement("speaker_tandy").text();			// speaker_tandy
				DatosDosBox["speaker_tandyrate"]     = child.firstChildElement("speaker_tandyrate").text();		// speaker_tandyrate
				DatosDosBox["speaker_disney"]        = child.firstChildElement("speaker_disney").text();		// speaker_disney
				DatosDosBox["joystick_type"]         = child.firstChildElement("joystick_type").text();			// joystick_type
				DatosDosBox["joystick_timed"]        = child.firstChildElement("joystick_timed").text();		// joystick_timed
				DatosDosBox["joystick_autofire"]     = child.firstChildElement("joystick_autofire").text();		// joystick_autofire
				DatosDosBox["joystick_swap34"]       = child.firstChildElement("joystick_swap34").text();		// joystick_swap34
				DatosDosBox["joystick_buttonwrap"]   = child.firstChildElement("joystick_buttonwrap").text();	// joystick_buttonwrap
				DatosDosBox["modem_modem"]           = child.firstChildElement("modem_modem").text();			// modem_modem
				DatosDosBox["modem_comport"]         = child.firstChildElement("modem_comport").text();			// modem_comport
				DatosDosBox["modem_listenport"]      = child.firstChildElement("modem_listenport").text();		// modem_listenport
				DatosDosBox["dserial_directserial"]  = child.firstChildElement("dserial_directserial").text();	// dserial_directserial
				DatosDosBox["dserial_comport"]       = child.firstChildElement("dserial_comport").text();		// dserial_comport
				DatosDosBox["dserial_realport"]      = child.firstChildElement("dserial_realport").text();		// dserial_realport
				DatosDosBox["dserial_defaultbps"]    = child.firstChildElement("dserial_defaultbps").text();	// dserial_defaultbps
				DatosDosBox["dserial_parity"]        = child.firstChildElement("dserial_parity").text();		// dserial_parity
				DatosDosBox["dserial_bytesize"]      = child.firstChildElement("dserial_bytesize").text();		// dserial_bytesize
				DatosDosBox["dserial_stopbit"]       = child.firstChildElement("dserial_stopbit").text();		// dserial_stopbit
				DatosDosBox["serial_1"]              = child.firstChildElement("serial_1").text();				// serial_1
				DatosDosBox["serial_2"]              = child.firstChildElement("serial_2").text();				// serial_2
				DatosDosBox["serial_3"]              = child.firstChildElement("serial_3").text();				// serial_3
				DatosDosBox["serial_4"]              = child.firstChildElement("serial_4").text();				// serial_4
				DatosDosBox["dos_xms"]               = child.firstChildElement("dos_xms").text();				// dos_xms
				DatosDosBox["dos_ems"]               = child.firstChildElement("dos_ems").text();				// dos_ems
				DatosDosBox["dos_umb"]               = child.firstChildElement("dos_umb").text();				// dos_umb
				DatosDosBox["dos_keyboardlayout"]    = child.firstChildElement("dos_keyboardlayout").text();	// dos_keyboardlayout
				DatosDosBox["ipx_ipx"]               = child.firstChildElement("ipx_ipx").text();				// ipx_ipx
				DatosDosBox["autoexec"]              = child.firstChildElement("autoexec").text();				// autoexec
				DatosDosBox["opt_autoexec"]          = child.firstChildElement("opt_autoexec").text();			// opt_autoexec
				DatosDosBox["opt_loadfix"]           = child.firstChildElement("opt_loadfix").text();			// opt_loadfix
				DatosDosBox["opt_loadfix_mem"]       = child.firstChildElement("opt_loadfix_mem").text();		// opt_loadfix_mem
				DatosDosBox["opt_consola_dbox"]      = child.firstChildElement("opt_consola_dbox").text();		// opt_consola_dbox
				DatosDosBox["opt_cerrar_dbox"]       = child.firstChildElement("opt_cerrar_dbox").text();		// opt_cerrar_dbox
				DatosDosBox["opt_cycle_sincronizar"] = child.firstChildElement("opt_cycle_sincronizar").text();	// opt_cycle_sincronizar
				DatosDosBox["path_conf"]             = child.firstChildElement("path_conf").text();				// path_conf
				DatosDosBox["path_sonido"]           = child.firstChildElement("path_sonido").text();			// path_sonido
				DatosDosBox["path_exe"]              = child.firstChildElement("path_exe").text();				// path_exe
				DatosDosBox["path_setup"]            = child.firstChildElement("path_setup").text();			// path_setup
				DatosDosBox["parametros_exe"]        = child.firstChildElement("parametros_exe").text();		// parametros_exe
				DatosDosBox["parametros_setup"]      = child.firstChildElement("parametros_setup").text();		// parametros_setup

			}
			else if(child.tagName() == "montajes")
			{
				QTreeWidgetItem *childItem = new QTreeWidgetItem(ui.twMontajes);

				QString tipoDrive = child.firstChildElement("tipo_as").text();
				if(tipoDrive=="drive")
					childItem->setIcon( 0, QIcon(stTheme+"img16/drive_hd.png") );
				if(tipoDrive=="cdrom")
					childItem->setIcon( 0, QIcon(stTheme+"img16/drive_cdrom.png") );
				if(tipoDrive=="floppy")
					childItem->setIcon( 0, QIcon(stTheme+"img16/drive_floppy.png") );
				if(tipoDrive=="IMG_floppy")
					childItem->setIcon( 0, QIcon(stTheme+"img16/floppy_1.png") );
				if(tipoDrive=="IMG_iso")
					childItem->setIcon( 0, QIcon(stTheme+"img16/cd_iso.png") );
				if(tipoDrive=="IMG_hdd")
					childItem->setIcon(0, QIcon(stTheme+"img16/drive_hd.png") );

				childItem->setText(0, child.firstChildElement("path").text().replace("{DirBaseGames}", DirBaseGames) );
				childItem->setText(1, child.firstChildElement("label").text()        );
				childItem->setText(2, child.firstChildElement("tipo_as").text()      );
				childItem->setText(3, child.firstChildElement("letter").text()       );
				childItem->setText(4, child.firstChildElement("indx_cd").text()      );
				childItem->setText(5, child.firstChildElement("opt_mount").text()    );
				childItem->setText(6, child.firstChildElement("io_ctrl").text()      );
				childItem->setText(7, child.firstChildElement("select_mount").text() );

			}

			child = child.nextSiblingElement();
		}
	}
}

void frmImportarJuego::setProfile_DFend(QString fileName)
{
	QStringList str_Lista;
	QString tipoDrive, temp_opt_mount;
	int i=0, num_mounts=0;

	DatosDosBox.clear();
	DatosDosBox = fGrl.Importar_Profile_DFend( fileName );	

//	DatosDosBox["WWW"]
//	DatosDosBox["UserInfo"]

	DatosJuego.clear();
	DatosJuego["icono"]         = DatosDosBox["icono"];		// icono
	DatosJuego["titulo"]        = DatosDosBox["titulo"];	// titulo
	DatosJuego["subtitulo"]     = "";						// subtitulo
	DatosJuego["genero"]        = DatosDosBox["genero"];	// genero
	DatosJuego["compania"]      = DatosDosBox["compania"];	// compania
	DatosJuego["desarrollador"] = DatosDosBox["desarrollador"];	// desarrollador
	DatosJuego["tema"]          = "";						// tema
	DatosJuego["idioma"]        = DatosDosBox["idioma"];	// idioma
	DatosJuego["formato"]       = "";						// formato
	DatosJuego["anno"]          = DatosDosBox["anno"];		// anno
	DatosJuego["numdisc"]       = "";						// numdisc
	DatosJuego["sistemaop"]     = "";						// sistemaop
	DatosJuego["tamano"]        = "";						// tamano
	DatosJuego["graficos"]      = "0";						// graficos
	DatosJuego["sonido"]        = "0";						// sonido
	DatosJuego["jugabilidad"]   = "0";						// jugabilidad
	DatosJuego["original"]      = "false";					// original
	DatosJuego["estado"]        = "";						// estado
	DatosJuego["thumbs"]        = "null";					// thumbs
	DatosJuego["cover_front"]   = "null";					// cover_front
	DatosJuego["cover_back"]    = "null";					// cover_back
	DatosJuego["fecha"]         = fGrl.HoraFechaActual();	// fecha d/m/a h:m:s
	DatosJuego["tipo_emu"]      = "dosbox"; 				// tipo_emu
	DatosJuego["comentario"]    = DatosDosBox["comentario"];// comentario
	DatosJuego["favorito"]      = DatosDosBox["favorito"];	//favorito

	ui.twMontajes->clear();
	num_mounts = fGrl.StrToInt( DatosDosBox["NrOfMounts"] );
	if( num_mounts > 0 )
	{
		for(i=0; i < num_mounts ; i++)
		{
			QTreeWidgetItem *twItemDfend = new QTreeWidgetItem(ui.twMontajes);

			str_Lista.clear();
			str_Lista = DatosDosBox[fGrl.IntToStr(i)].split("|");

			tipoDrive.clear();
			tipoDrive = str_Lista.value(1).toLower();// tipo_as

			if(tipoDrive=="drive")
			{
				twItemDfend->setIcon( 0, QIcon(stTheme+"img16/drive_hd.png") );
				tipoDrive = "drive";
			}
			if(tipoDrive=="cdrom")
			{
				twItemDfend->setIcon( 0, QIcon(stTheme+"img16/drive_cdrom.png") );
				tipoDrive = "cdrom";
			}
			if(tipoDrive=="floppy")
			{
				twItemDfend->setIcon( 0, QIcon(stTheme+"img16/drive_floppy.png") );
				tipoDrive = "floppy";
			}
			if(tipoDrive=="floppyimage")
			{
				twItemDfend->setIcon( 0, QIcon(stTheme+"img16/floppy_1.png") );
				tipoDrive = "IMG_floppy";
			}
			if(tipoDrive=="cdromimage")
			{
				twItemDfend->setIcon( 0, QIcon(stTheme+"img16/cd_iso.png") );
				tipoDrive = "IMG_iso";
			}
			if(tipoDrive=="image")
			{
				twItemDfend->setIcon(0, QIcon(stTheme+"img16/drive_hd.png") );
				tipoDrive = "IMG_hdd";
			}
			temp_opt_mount.clear();
			temp_opt_mount = str_Lista.value(5);

			twItemDfend->setText(0, ""+str_Lista.value(0).replace("{DirBaseGames}", DirBaseGames) ); // path
			twItemDfend->setText(1, ""+str_Lista.value(4) ); // label
			twItemDfend->setText(2, ""+tipoDrive          ); // tipo_as
			twItemDfend->setText(3, ""+str_Lista.value(2) ); // letter
			twItemDfend->setText(4, ""+fGrl.IntToStr(i)   ); // indx_cd
			twItemDfend->setText(5, ""+temp_opt_mount     ); // opt_mount
			twItemDfend->setText(6, ""+str_Lista.value(3) ); // io_ctrl
			twItemDfend->setText(7, "x"                    ); // select_mount

		}
	}
}

void frmImportarJuego::MostrarDatosJuegoInfo()
{
	ui.twDatosInfo->topLevelItem(0)->setText(1 , DatosJuego["icono"] );			// icono
	ui.twDatosInfo->topLevelItem(1)->setText(1 , DatosJuego["titulo"] );		// titulo
	ui.twDatosInfo->topLevelItem(2)->setText(1 , DatosJuego["subtitulo"] );		// subtitulo
	ui.twDatosInfo->topLevelItem(3)->setText(1 , DatosJuego["genero"] );		// genero
	ui.twDatosInfo->topLevelItem(4)->setText(1 , DatosJuego["compania"] );		// compania
	ui.twDatosInfo->topLevelItem(5)->setText(1 , DatosJuego["desarrollador"]);	// desarrollador
	ui.twDatosInfo->topLevelItem(6)->setText(1 , DatosJuego["tema"] );			// tema
	ui.twDatosInfo->topLevelItem(7)->setText(1 , DatosJuego["idioma"]);			// idioma
	ui.twDatosInfo->topLevelItem(8)->setText(1 , DatosJuego["formato"] );		// formato
	ui.twDatosInfo->topLevelItem(9)->setText(1 , DatosJuego["anno"] );			// anno
	ui.twDatosInfo->topLevelItem(10)->setText(1, DatosJuego["numdisc"] );		// numdisc
	ui.twDatosInfo->topLevelItem(11)->setText(1, DatosJuego["sistemaop"] );		// sistemaop
	ui.twDatosInfo->topLevelItem(12)->setText(1, DatosJuego["tamano"] );		// tamano
	ui.twDatosInfo->topLevelItem(13)->setText(1, DatosJuego["graficos"] );		// graficos
	ui.twDatosInfo->topLevelItem(14)->setText(1, DatosJuego["sonido"] );		// sonido
	ui.twDatosInfo->topLevelItem(15)->setText(1, DatosJuego["jugabilidad"]);	// jugabilidad
	ui.twDatosInfo->topLevelItem(16)->setText(1, DatosJuego["original"]);		// original
	ui.twDatosInfo->topLevelItem(17)->setText(1, DatosJuego["estado"] );		// estado
	ui.twDatosInfo->topLevelItem(18)->setText(1, DatosJuego["thumbs"] );		// thumbs
	ui.twDatosInfo->topLevelItem(19)->setText(1, DatosJuego["cover_front"] );	// cover_front
	ui.twDatosInfo->topLevelItem(20)->setText(1, DatosJuego["cover_back"]);		// cover_back
	ui.twDatosInfo->topLevelItem(21)->setText(1, DatosJuego["fecha"] );			// fecha d/m/a h:m:s
	ui.twDatosInfo->topLevelItem(22)->setText(1, DatosJuego["tipo_emu"] );		// tipo_emu
	ui.twDatosInfo->topLevelItem(23)->setText(1, DatosJuego["comentario"] );	// comentario
	
	QFile file_thumbs;	// thumbs
	if( file_thumbs.exists(stHomeDir + "temp/thumbs_"+DatosJuego["thumbs"]) )
		ui.lb_thumbs->setPixmap( QPixmap(stHomeDir + "temp/thumbs_"+DatosJuego["thumbs"]) );
	else
		ui.lb_thumbs->setPixmap( QPixmap(stTheme+"images/juego_sin_imagen.png") );
}

void frmImportarJuego::downloadFile(QString urlfile, QString fileName)
{
	QUrl url( urlfile );
	QFileInfo fileInfo( url.path() );
	//QString fileName = fileInfo.fileName();
	//if(fileName.isEmpty())
	//	fileName = "index.html";
	//if(QFile::exists(fileName) && (fileName!=xml_InfoJuegos || fileName!=xml_InfoJuegos) )
	if( QFile::exists(fileName) )
	{
		//if(QMessageBox::question(this, "GR-lida", tr("Ya existe un archivo con el mismo nombre '%1' en el directorio actual. ¿Quieres sobreescribirlo?").arg(fileName),
		//	QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Cancel)
		//	return;
		QFile::remove(fileName);
	}

	file = new QFile(fileName);
	if(!file->open(QIODevice::WriteOnly))
	{
		QMessageBox::information(this, "GR-lida",tr("No se ha podido guardar el archivo %1: %2.").arg(fileName).arg(file->errorString()));
		delete file;
		file = 0;
		return;
	}

	QHttp::ConnectionMode mode = url.scheme().toLower() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp; // Qt 4.3.x
	http->setHost(url.host(), mode, url.port() == -1 ? 0 : url.port());

	if(!url.userName().isEmpty())
		http->setUser(url.userName(), url.password());

	httpRequestAborted = false;
	httpGetId = http->get( urlfile , file);

	progressDialog->setWindowTitle("GR-lida DB");
	progressDialog->setLabelText(tr("Descargando %1.").arg(fileName));
	ui.btnUpdateList->setEnabled(false);
}

void frmImportarJuego::cancelDownload()
{
	ui.statusLabel->setText(tr("Descarga cancelada."));
	httpRequestAborted = true;
	http->abort();
	ui.btnUpdateList->setEnabled(true);
}

void frmImportarJuego::httpRequestFinished(int requestId, bool error)
{
	if(requestId != httpGetId)
		return;
	if(httpRequestAborted)
	{
		if(file)
		{
			file->close();
			file->remove();
			delete file;
			file = 0;
		}
		progressDialog->hide();
		return;
	}

	if(requestId != httpGetId)
		return;

	progressDialog->hide();
	file->flush();
	file->close();

	if(error)
	{
		file->remove();
		QMessageBox::information(this, "GR-lida DB", tr("No se ha podido guardar el archivo: %1.").arg(http->errorString()));
	
	} else {
		QString fileName = QFileInfo(QUrl( url_filed ).path()).fileName();
		ui.statusLabel->setText(tr("Descargado en el directorio: %1.").arg(fileName));
		switch( indx_fin_descarga )
		{
			case 0:
				xml_read( xml_ListaJuegos );
			break;
			case 1:
				xml_read( xml_InfoJuegos );
			break;
			case 2:
				ui.textBrowser->setHtml( texto_html );
			break;
			case 3:
				//
			break;
		}
	}
	ui.btnUpdateList->setEnabled(true);
	delete file;
	file = 0;
}

void frmImportarJuego::readResponseHeader(const QHttpResponseHeader &responseHeader)
{
	if(responseHeader.statusCode() != 200)
	{
		QMessageBox::information(this, "GR-lida DB", tr("La descarga ha fallado: %1.").arg(responseHeader.reasonPhrase()));
		httpRequestAborted = true;
		progressDialog->hide();
		http->abort();
		return;
	}
}

void frmImportarJuego::updateDataReadProgress(int bytesRead, int totalBytes)
{
	if(httpRequestAborted)
		return;
	progressDialog->setMaximum(totalBytes);
	progressDialog->setValue(bytesRead);
}

void frmImportarJuego::enableDownloadButton()
{
	ui.btnUpdateList->setEnabled(!url_filed.isEmpty());
}

// para la Qt 4.3.x
void frmImportarJuego::slotAuthenticationRequired(const QString &hostName, quint16, QAuthenticator *authenticator)
{
	QDialog dlg;
	Ui::Dialog ui;
	ui.setupUi(&dlg);
	dlg.adjustSize();
	ui.siteDescription->setText(tr("%1 en %2").arg(authenticator->realm()).arg(hostName));
	
	if(dlg.exec() == QDialog::Accepted)
	{
		authenticator->setUser(ui.userEdit->text());
		authenticator->setPassword(ui.passwordEdit->text());
	}
}

void frmImportarJuego::on_treeWidget_clicked( QTreeWidgetItem *item)
{
	QString temp_html_img;
	bool descarga_img;

	if( item )
	{
		temp_html_img.clear();
		if( item->text(2)=="null" || item->text(2).isEmpty() )
		{
			descarga_img = false;
			temp_html_img = "<img src=\""+stTheme+"images/juego_sin_imagen.png\" width=\"145\" height=\"178\" hspace=\"4\" vspace=\"4\" border=\"0\" align=\"right\" />";
		} else {
			QFile file_thumbs;	// thumbs

			temp_html_img = "<img src=\""+stHomeDir+"temp/thumbs_"+item->text(2)+"\" width=\"145\" height=\"178\" hspace=\"4\" vspace=\"4\" border=\"0\" align=\"right\" />";

			if( !file_thumbs.exists( stHomeDir + "temp/thumbs_"+ item->text(2) ) )
			{
				indx_fin_descarga = 2;
				descarga_img = true;
				url_filed = url_xmldb + "images/covers/small/"+ item->text(2);
				downloadFile( url_filed , stHomeDir + "temp/thumbs_"+ item->text(2) );
			} else
				descarga_img = false;
		}
	
		ui.textBrowser->clear();
		texto_html.clear();
		texto_html.append("<html>");
		texto_html.append("<head>");
		texto_html.append("<title>"+item->text(0)+"</title>");
		texto_html.append("</head>");
		texto_html.append("<body>");
		texto_html.append( temp_html_img + item->text(3) );
		texto_html.append("</body>");
		texto_html.append("</html>");

		if( !descarga_img )
			ui.textBrowser->setHtml( texto_html );

		ui.btnNext->setEnabled(true);

	} else {
		ui.btnNext->setEnabled(false);
		return;
	}
}

void frmImportarJuego::on_treeWidget_Dblclicked(QTreeWidgetItem *item)
{
	if( item )
	{
		ui.twMontajes->clear();
		ui.twDatosFiles->clear();
		ui.twDatosURL->clear();
		indx_fin_descarga = 1;

		url_filed = url_xmldb + "grlidadb.php?ver=juego&gid="+item->text(1)+"&id_emu="+item->text(5)+"&tipo_emu="+item->text(6);
		downloadFile( url_filed , xml_InfoJuegos );

		ui.btnPrevious->setEnabled(true);
		ui.btnOk->setEnabled(true);
		ui.wizardImport->setCurrentIndex(2);

		if(item->text(6)=="dosbox")
			ui.tabWidget->setTabEnabled(2,true);
		else
			ui.tabWidget->setTabEnabled(2,false);
	} else
		return;
}
