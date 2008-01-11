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
	connect( ui.btnUpdateList , SIGNAL(clicked()), this, SLOT(on_download()));
	connect( ui.btnPrevious   , SIGNAL(clicked()), this, SLOT(on_btnPrevious()));
	connect( ui.btnNext       , SIGNAL(clicked()), this, SLOT(on_btnNext()));
	connect( ui.btnOk         , SIGNAL(clicked()), this, SLOT(on_btnOk()));
	connect( ui.twListaJuegos , SIGNAL( itemClicked( QTreeWidgetItem*, int )), this, SLOT( on_treeWidget_clicked(QTreeWidgetItem *) ));
	connect( ui.twListaJuegos , SIGNAL( itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( on_treeWidget_Dblclicked(QTreeWidgetItem *) )); 
	connect( ui.cbxDbXml      , SIGNAL(activated(const QString &)), this, SLOT( on_changeURL_XML(const QString &)));

	stHomeDir = QDir::homePath()+"/.gr-lida/";	// directorio de trabajo del GR-lida
//	stHomeDir = QDir::currentPath()+"/";		// directorio de trabajo del GR-lida
//	stHomeDir = "./";							// directorio de trabajo del GR-lida

	QStringList cbx_Lista, cbx_ListaTemp;
	QPixmap pixmap;
	QFile file_xmldb( stHomeDir + "datos/xmldb.txt" );
	if (file_xmldb.open(QIODevice::ReadOnly)!=0 ){
		QTextStream in(&file_xmldb);
		in.setCodec("ISO 8859-1");
		while ( !in.atEnd() ) cbx_ListaTemp << in.readLine();
		ui.cbxDbXml->clear();
		for ( int i = 0; i < cbx_ListaTemp.count(); i++ ) {
			cbx_Lista = cbx_ListaTemp[i].split( "|" );
			pixmap.load(":/img16/"+cbx_Lista.value(1)+".png");
			if( pixmap.isNull() )
				pixmap.load(":/img16/sinimg.png");
			ui.cbxDbXml->addItem( QIcon( pixmap ), url_correcta( cbx_Lista.value(0).toLatin1() ) );
		}
	}
	file_xmldb.close();

	QSettings settings( stHomeDir + "GR-lida.conf", QSettings::IniFormat); 
	settings.beginGroup("OpcGeneral");
	  url_xmldb = url_correcta( settings.value("url_xmldb", "" ).toString() ); // GR-lida
	settings.endGroup();

	xml_ListaJuegos = stHomeDir + "tmp_lista_juegos.xml";
	xml_InfoJuegos  = stHomeDir + "tmp_info_juego.xml";
 
	ui.cbxDbXml->addItem( QIcon(":/img16/edit_enlace.png"), url_xmldb);
	ui.cbxDbXml->setCurrentIndex( ui.cbxDbXml->findText(url_xmldb, Qt::MatchContains) ); //
     
	progressDialog = new QProgressDialog(this);
	http = new QHttp(this);
	
  connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(httpRequestFinished(int, bool)));
  connect(http, SIGNAL(dataReadProgress(int, int)), this, SLOT(updateDataReadProgress(int, int)));
  connect(http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)), this, SLOT(readResponseHeader(const QHttpResponseHeader &)));
  connect(http, SIGNAL(authenticationRequired(const QString &, quint16, QAuthenticator *)), this, SLOT(slotAuthenticationRequired(const QString &, quint16, QAuthenticator *)));  // Qt 4.3.x
  connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));	
	
	ui.btnUpdateList->setDefault(true);
	ui.stackedWidget->setCurrentIndex(0);

	indx_fin_descarga = 0;
	xml_read( xml_ListaJuegos );

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmImportarJuego::~frmImportarJuego(){
	delete http;
	delete file;
	delete progressDialog;
}

QString frmImportarJuego::url_correcta(QString url)
{
	bool url_ok;
    url_ok = url.startsWith("http://");
   	if(url_ok==false) url.prepend("http://");
     
  	url_ok = url.endsWith("/");
	if(url_ok==false) url.append("/");

	return url;
}

void frmImportarJuego::on_changeURL_XML(const QString &url)
{
	url_xmldb = url_correcta( url );
}

void frmImportarJuego::on_btnPrevious()
{
	ui.btnNext->setText(tr("Siguiente"));
	ui.btnPrevious->setEnabled(false);
	ui.btnNext->setEnabled(false);
	ui.btnOk->setEnabled(false);
	ui.stackedWidget->setCurrentIndex(0);
}

void frmImportarJuego::on_btnNext()
{
	//ui.twDatosInfo->clear();
	ui.twMontajes->clear();
	ui.twDatosFiles->clear();
	ui.twDatosURL->clear();
	indx_fin_descarga = 1;
	url_filed = url_xmldb + "grlidadb.php?ver=juego&gid="+ui.twListaJuegos->currentItem()->text(1)+"&id_emu="+ui.twListaJuegos->currentItem()->text(5)+"&tipo_emu="+ui.twListaJuegos->currentItem()->text(6);
	downloadFile( url_filed , xml_InfoJuegos );
	
	ui.btnNext->setText(tr("Actualizar"));
	ui.btnPrevious->setEnabled(true);
	ui.btnOk->setEnabled(true);
	ui.stackedWidget->setCurrentIndex(1);
	
	if(ui.twListaJuegos->currentItem()->text(6)=="dosbox")
		ui.tabWidget->setTabEnabled(2,true); else ui.tabWidget->setTabEnabled(2,false);
}

void frmImportarJuego::on_btnOk()
{
	frmImportPath * ImportPathNew = new frmImportPath();
	
	ImportPathNew->ui.tabWidget_ImportPath->setTabEnabled (1, false )		;
	ImportPathNew->ui.txtPath_Datos_1->setText( DatosJuego["thumbs"] )		; // thumbs
	ImportPathNew->ui.txtPath_Datos_2->setText( DatosJuego["cover_front"] )	; // cover_front
	ImportPathNew->ui.txtPath_Datos_3->setText( DatosJuego["cover_back"] )	; // cover_back

	if(DatosJuego["tipo_emu"] =="dosbox"){
		ImportPathNew->ui.tabWidget_ImportPath->setTabEnabled (1, true )	;
		ImportPathNew->ui.wizardPath->setCurrentIndex(0);
		ImportPathNew->ui.txtPath_Dbx_1->setText( QDir::toNativeSeparators( DatosDosBox["path_conf"]) ); // path_conf
		ImportPathNew->ui.txtPath_Dbx_2->setText( QDir::toNativeSeparators( DatosDosBox["path_exe"]) ); // path_exe
		ImportPathNew->ui.txtPath_Dbx_3->setText( QDir::toNativeSeparators( DatosDosBox["path_setup"]) ); // path_setup
		ImportPathNew->ui.txtPath_Dbx_4->setText( QDir::toNativeSeparators( DatosDosBox["dosbox_language"]) ); // dosbox_language
		ImportPathNew->ui.txtPath_Dbx_5->setText( QDir::toNativeSeparators( DatosDosBox["sdl_mapperfile"]) ); // sdl_mapperfile
		ImportPathNew->ui.txtPath_Dbx_6->setText( QDir::toNativeSeparators( DatosDosBox["gus_ultradir"]) ); // gus_ultradir
		ImportPathNew->ui.txtPath_Dbx_7->setText( QDir::toNativeSeparators( DatosDosBox["dosbox_captures"]) ); // dosbox_captures
		ImportPathNew->ui.txtPath_Dbx_8->setText( QDir::toNativeSeparators( DatosDosBox["path_sonido"]) ); // path_sonido
	}
	if(DatosJuego["tipo_emu"]=="scummvm"){
		ImportPathNew->ui.tabWidget_ImportPath->setTabEnabled (1, true )	;
		ImportPathNew->ui.wizardPath->setCurrentIndex(1);
		ImportPathNew->ui.txtPath_Svm_1->setText( QDir::toNativeSeparators( DatosScummvm["path"] ) ); // path
		ImportPathNew->ui.txtPath_Svm_2->setText( QDir::toNativeSeparators( DatosScummvm["path_save"]) ); // path_save
		ImportPathNew->ui.txtPath_Svm_3->setText( QDir::toNativeSeparators( DatosScummvm["path_setup"] ) ); // path_setup
		ImportPathNew->ui.txtPath_Svm_4->setText( QDir::toNativeSeparators( DatosScummvm["path_extra"]) ); // path_extra
		ImportPathNew->ui.txtPath_Svm_5->setText( QDir::toNativeSeparators( DatosScummvm["path_capturas"]) ); // path_capturas
		ImportPathNew->ui.txtPath_Svm_6->setText( QDir::toNativeSeparators( DatosScummvm["path_sonido"]) ); // path_sonido	
	}
	if(DatosJuego["tipo_emu"]=="vdmsound"){
		ImportPathNew->ui.tabWidget_ImportPath->setTabEnabled (1, true )	;
		ImportPathNew->ui.wizardPath->setCurrentIndex(2);
		ImportPathNew->ui.txtPath_Vdms_1->setText( QDir::toNativeSeparators( DatosVDMSound["path_conf"] ) );
		ImportPathNew->ui.txtPath_Vdms_2->setText( QDir::toNativeSeparators( DatosVDMSound["path_exe"] ) );
	}	
    if ( ImportPathNew->exec() == QDialog::Accepted ) {
		DatosJuego["thumbs"]      = ImportPathNew->ui.txtPath_Datos_1->text()		; // thumbs
		DatosJuego["cover_front"] = ImportPathNew->ui.txtPath_Datos_2->text()		; // cover_front
		DatosJuego["cover_back"]  = ImportPathNew->ui.txtPath_Datos_3->text()		; // cover_back

		if(DatosJuego["tipo_emu"]=="dosbox"){
			DatosDosBox["path_conf"]    = ImportPathNew->ui.txtPath_Dbx_1->text()	; // path_conf
			DatosDosBox["path_exe"]     = ImportPathNew->ui.txtPath_Dbx_2->text()	; // path_exe
			DatosDosBox["path_setup"]   = ImportPathNew->ui.txtPath_Dbx_3->text()	; // path_setup
			DatosDosBox["dosbox_language"]= ImportPathNew->ui.txtPath_Dbx_4->text()	; // path_lang
			DatosDosBox["sdl_mapperfile"] = ImportPathNew->ui.txtPath_Dbx_5->text()	; // path_mapper
			DatosDosBox["gus_ultradir"]   = ImportPathNew->ui.txtPath_Dbx_6->text()	; // path_gus
			DatosDosBox["dosbox_captures"]= ImportPathNew->ui.txtPath_Dbx_7->text()	; // path_capturas
			DatosDosBox["path_sonido"]  = ImportPathNew->ui.txtPath_Dbx_8->text()	; // path_sonido
		}
		if(DatosJuego["tipo_emu"]=="scummvm"){
			DatosScummvm["path"]         = ImportPathNew->ui.txtPath_Svm_1->text()	; // path
			DatosScummvm["path_save"]    = ImportPathNew->ui.txtPath_Svm_2->text()	; // path_save
			DatosScummvm["path_setup"]   = ImportPathNew->ui.txtPath_Svm_3->text()	; // path_setup
			DatosScummvm["path_extra"]   = ImportPathNew->ui.txtPath_Svm_4->text()	; // path_extra
			DatosScummvm["path_capturas"]= ImportPathNew->ui.txtPath_Svm_5->text()	; // path_capturas
			DatosScummvm["path_sonido"]  = ImportPathNew->ui.txtPath_Svm_6->text()	; // path_sonido	
		}	
		if(DatosJuego["tipo_emu"]=="vdmsound"){
			DatosVDMSound["path_conf"]= ImportPathNew->ui.txtPath_Vdms_1->text();
			DatosVDMSound["path_exe"] = ImportPathNew->ui.txtPath_Vdms_2->text();
		}	
		QFile file_thumbs(stHomeDir + "temp_img_thumbs.jpg");  // thumbs
		if( file_thumbs.exists(stHomeDir + "temp_img_thumbs.jpg") ){
			file_thumbs.open(QFile::ReadOnly);
			file_thumbs.copy( DatosJuego["thumbs"] );
		}
		file_thumbs.close();	
		
		int num_files;
		for ( num_files = 0; num_files < ui.twDatosFiles->topLevelItemCount(); num_files++ ) {
			QTreeWidgetItem *item = ui.twDatosFiles->topLevelItem( num_files );
			downloadFile( url_xmldb+"downloads/juegos/"+item->text(4) , stHomeDir+"roms/"+item->text(0) );
		}
	

		if(ui.cbxDbXml->currentText()!="")
		  url_xmldb = url_correcta(ui.cbxDbXml->currentText()); else  url_xmldb = "";
		
		QSettings settings( stHomeDir+"GR-lida.conf", QSettings::IniFormat ); 
		settings.beginGroup("OpcGeneral");
		  settings.setValue("url_xmldb", url_xmldb );
		settings.endGroup();
	}
	QDialog::accept();
}

void frmImportarJuego::on_download()
{
	ui.twListaJuegos->clear();
	indx_fin_descarga = 0;
	url_filed = url_xmldb + "grlidadb.xml";
	downloadFile( url_filed , xml_ListaJuegos );
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
	if (!fileName.isEmpty()){
		QFile file(fileName);
		if (file.exists()){
			if (file.open(QFile::ReadOnly | QFile::Text)) {
				if (!domDocument.setContent(&file, true, &errorStr, &errorLine, &errorColumn)) {
					QMessageBox::information(window(), "GR-lida DB",
						tr("Error en Parse en la linea %1, columna %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr));
					return false;
				}

				QDomElement root = domDocument.documentElement();
				if (root.tagName() != "grlida_db") {
					QMessageBox::information(window(), "GR-lida DB", tr("No es un archivo de")+ " GR-lida DB");
					return false;
				} else if (root.hasAttribute("version") && root.attribute("version") != "0.1.0") {
					QMessageBox::information(window(), "GR-lida DB", tr("El archivo no corresponde a la version 0.1.0"));
					return false;
				}

				QDomElement child = root.firstChildElement("lista_db");
				while (!child.isNull()) {
					parseListaJuegos( child );
					child = child.nextSiblingElement("lista_db");
				}
				return true;
			}else{
				QMessageBox::warning(this, "GR-lida DB", tr("No se ha podido leer el archivo %1:\n%2.").arg(fileName).arg(file.errorString()));
				return false;
			}
		}else
		return false;
	}
	return true;
}

void frmImportarJuego::parseListaJuegos(const QDomElement &element)
{
	if( indx_fin_descarga == 0 ){
		ui.lb_infoTitulo->setText(      element.firstChildElement("title").text()         );
		ui.lb_infoLink->setText(        element.firstChildElement("link").text()          );
		ui.lb_infoBuildDate->setText(   element.firstChildElement("lastBuildDate").text() );
		ui.lb_infoDescripcion->setText( element.firstChildElement("description").text()   );

		QDomElement child = element.firstChildElement();
		while (!child.isNull()) {
			if (child.tagName() == "lista_db") {
				parseListaJuegos(child);
			} else if (child.tagName() == "juego") {
				QTreeWidgetItem *childItem = new QTreeWidgetItem(ui.twListaJuegos); 
				if(child.firstChildElement("tipo_emu").text()=="datos")
					childItem->setIcon(0, QIcon(":/img24/emu_datos.png"));
				else if(child.firstChildElement("tipo_emu").text()=="dosbox")
					childItem->setIcon(0, QIcon(":/img24/emu_dbx.png"));
				else if(child.firstChildElement("tipo_emu").text()=="scummvm")
					childItem->setIcon(0, QIcon(":/img24/emu_svm.png"));
				else if(child.firstChildElement("tipo_emu").text()=="vdmsound")
					childItem->setIcon(0, QIcon(":/img24/emu_vdms.png"));
				else 
					childItem->setIcon(0, QIcon(":/img24/emu_sin_imagen.png"));
				childItem->setText(0, child.firstChildElement("titulo").text());
				childItem->setText(1, child.attribute("ID") );
				childItem->setText(2, child.firstChildElement("thumbs").text() );
				childItem->setText(3, child.firstChildElement("descripcion").text() );
				childItem->setText(4, child.firstChildElement("pubDate").text() );
				childItem->setText(5, child.firstChildElement("id_emu").text() );
				childItem->setText(6, child.firstChildElement("tipo_emu").text() );
			} else if (child.tagName() == "separator") {
				QTreeWidgetItem *childItem = new QTreeWidgetItem(ui.twListaJuegos); 
				childItem->setText(0, QString(30, 0xB7));
			}
			child = child.nextSiblingElement();
		}
	}
	if( indx_fin_descarga == 1 ){
		ui.lb_usuario_1->setText("");
		ui.lb_usuario_2->setText("");
		ui.lb_usuario_3->setText("");
		QDomElement child = element.firstChildElement();
		while (!child.isNull()) {
			if (child.tagName() == "lista_db") {
				parseListaJuegos(child);
			} else if (child.tagName() == "datos") {
				ui.lb_titulo_juego->setText(child.firstChildElement("titulo").text());
				ui.lb_usuario_1->setText(child.firstChildElement("usuario").text()  );
				
				QFile file_thumbs;  // thumbs
				if( file_thumbs.exists(stHomeDir + "temp_img_thumbs.jpg") ){
					ui.lb_thumbs->setPixmap( QPixmap(stHomeDir + "temp_img_thumbs.jpg") );
				}else{
					ui.lb_thumbs->setPixmap( QPixmap(":/img24/juego_sin_imagen.png") );
				}
				DatosJuego.clear();
				DatosJuego["icono"] = child.firstChildElement("icono").text()		; //0 icono
				DatosJuego["titulo"] = child.firstChildElement("titulo").text()		; //1 titulo
				DatosJuego["subtitulo"] = child.firstChildElement("subtitulo").text()	; //2 subtitulo
				DatosJuego["genero"] = child.firstChildElement("genero").text()		; //3 genero
				DatosJuego["compania"] = child.firstChildElement("compania").text()	; //4 compania
				DatosJuego["desarrollador"] = child.firstChildElement("desarrollador").text();//5 desarrollador
				DatosJuego["tema"] = child.firstChildElement("tema").text()		; //6 tema
				DatosJuego["idioma"] = child.firstChildElement("idioma").text()		; //7 idioma
				DatosJuego["formato"] = child.firstChildElement("formato").text()		; //8 formato
				DatosJuego["anno"] = child.firstChildElement("anno").text()		; //9 anno
				DatosJuego["numdisc"] = child.firstChildElement("numdisc").text()		; //10 numdisc
				DatosJuego["sistemaop"] = child.firstChildElement("sistemaop").text()	; //11 sistemaop
				DatosJuego["tamano"] = child.firstChildElement("tamano").text()		; //12 tamano
				DatosJuego["graficos"] = child.firstChildElement("graficos").text()	; //13 graficos
				DatosJuego["sonido"] = child.firstChildElement("sonido").text()		; //14 sonido
				DatosJuego["jugabilidad"] = child.firstChildElement("jugabilidad").text()	; //15 jugabilidad
				DatosJuego["original"] = child.firstChildElement("original").text() 	; //16 original
				DatosJuego["estado"] = child.firstChildElement("estado").text()		; //17 estado
				DatosJuego["thumbs"] = child.firstChildElement("thumbs").text()		; //18 thumbs
				DatosJuego["cover_front"] = child.firstChildElement("cover_front").text()	; //19 cover_front
				DatosJuego["cover_back"] = child.firstChildElement("cover_back").text()	; //20 cover_back
				DatosJuego["fecha"] = fGrl.HoraFechaActual();	//child.firstChildElement("fecha").text()		; // fecha d/m/a h:m:s
				DatosJuego["tipo_emu"] = child.firstChildElement("tipo_emu").text()	; // tipo_emu
				DatosJuego["comentario"] = child.firstChildElement("comentario").text()	; // comentario	
	
				//for (int i = 0; i < ui.twDatosInfo->topLevelItemCount(); ++i)
				//	ui.twDatosInfo->topLevelItem(i)->setText(1,DatosJuego[i]);

				ui.twDatosInfo->topLevelItem(0)->setText(1 , DatosJuego["icono"] )		; //0 icono
				ui.twDatosInfo->topLevelItem(1)->setText(1 , DatosJuego["titulo"] )		; //1 titulo
				ui.twDatosInfo->topLevelItem(2)->setText(1 , DatosJuego["subtitulo"] )	; //2 subtitulo
				ui.twDatosInfo->topLevelItem(3)->setText(1 , DatosJuego["genero"] )		; //3 genero
				ui.twDatosInfo->topLevelItem(4)->setText(1 , DatosJuego["compania"] )	; //4 compania
				ui.twDatosInfo->topLevelItem(5)->setText(1 , DatosJuego["desarrollador"] );//5 desarrollador
				ui.twDatosInfo->topLevelItem(6)->setText(1 , DatosJuego["tema"] )		; //6 tema
				ui.twDatosInfo->topLevelItem(7)->setText(1 , DatosJuego["idioma"])		; //7 idioma
				ui.twDatosInfo->topLevelItem(8)->setText(1 , DatosJuego["formato"] )		; //8 formato
				ui.twDatosInfo->topLevelItem(9)->setText(1 , DatosJuego["anno"] )		; //9 anno
				ui.twDatosInfo->topLevelItem(10)->setText(1, DatosJuego["numdisc"] )	; //10 numdisc
				ui.twDatosInfo->topLevelItem(11)->setText(1, DatosJuego["sistemaop"] )	; //11 sistemaop
				ui.twDatosInfo->topLevelItem(12)->setText(1, DatosJuego["tamano"] )		; //12 tamano
				ui.twDatosInfo->topLevelItem(13)->setText(1, DatosJuego["graficos"] )	; //13 graficos
				ui.twDatosInfo->topLevelItem(14)->setText(1, DatosJuego["sonido"] )		; //14 sonido
				ui.twDatosInfo->topLevelItem(15)->setText(1, DatosJuego["jugabilidad"])	; //15 jugabilidad
				ui.twDatosInfo->topLevelItem(16)->setText(1, DatosJuego["original"]) 	; //16 original
				ui.twDatosInfo->topLevelItem(17)->setText(1, DatosJuego["estado"] )		; //17 estado
				ui.twDatosInfo->topLevelItem(18)->setText(1, DatosJuego["thumbs"] )		; //18 thumbs
				ui.twDatosInfo->topLevelItem(19)->setText(1, DatosJuego["cover_front"] ); //19 cover_front
				ui.twDatosInfo->topLevelItem(20)->setText(1, DatosJuego["cover_back"])	; //20 cover_back
				ui.twDatosInfo->topLevelItem(21)->setText(1, DatosJuego["fecha"] )		; // fecha d/m/a h:m:s
				ui.twDatosInfo->topLevelItem(22)->setText(1, DatosJuego["tipo_emu"] )	; // tipo_emu
				ui.twDatosInfo->topLevelItem(23)->setText(1, DatosJuego["comentario"] )	; // comentario	

				if(DatosJuego["thumbs"]!="")
					DatosJuego["thumbs"] = stHomeDir +"thumbs/"+ DatosJuego["thumbs"];
				if(DatosJuego["cover_front"]!="")
					DatosJuego["cover_front"] = stHomeDir +"covers/"+ DatosJuego["cover_front"];
				if(DatosJuego["cover_back"]!="")
					DatosJuego["cover_back"] = stHomeDir +"covers/"+ DatosJuego["cover_back"];
			} else if (child.tagName() == "datos_files") {
				QTreeWidgetItem *childItem_files = new QTreeWidgetItem(ui.twDatosFiles);
				childItem_files->setIcon(0, QIcon(":/img16/importar.png")					);
				childItem_files->setText(0, child.firstChildElement("nombre").text()		);
				childItem_files->setText(1, child.firstChildElement("crc").text()			);
				childItem_files->setText(2, child.firstChildElement("descripcion").text()	);
				childItem_files->setText(3, child.firstChildElement("size").text()			);
				childItem_files->setText(4, child.firstChildElement("path").text()			);
			} else if (child.tagName() == "scummvm") {
				ui.lb_usuario_3->setText(child.firstChildElement("usuario").text() )   ;
				DatosScummvm.clear();
				DatosScummvm["game"] = child.firstChildElement("game").text()			; //0  game
				DatosScummvm["language"] = child.firstChildElement("language").text()		; //1  language
				DatosScummvm["subtitles"] = child.firstChildElement("subtitles").text()		; //2  subtitles
				DatosScummvm["platform"] = child.firstChildElement("platform").text()		; //3  platform
				DatosScummvm["gfx_mode"] = child.firstChildElement("gfx_mode").text()		; //4  gfx_mode
				DatosScummvm["render_mode"] = child.firstChildElement("render_mode").text()	; //5  render_mode
				DatosScummvm["fullscreen"] = child.firstChildElement("fullscreen").text()	; //6  fullscreen
				DatosScummvm["aspect_ratio"] = child.firstChildElement("aspect_ratio").text()	; //7  aspect_ratio
				DatosScummvm["path"] =	child.firstChildElement("path").text()			; //8  path
				DatosScummvm["path_setup"] = child.firstChildElement("path_setup").text()	; //9  path_setup
				DatosScummvm["path_extra"] = child.firstChildElement("path_extra").text()	; //10 path_extra
				DatosScummvm["path_save"] = child.firstChildElement("path_save").text()		; //11 path_save
				DatosScummvm["path_capturas"] = child.firstChildElement("path_capturas").text()	; //12 path_capturas
				DatosScummvm["path_sonido"] = child.firstChildElement("path_sonido").text()	; //13 path_sonido
				DatosScummvm["music_driver"] = child.firstChildElement("music_driver").text()	; //14 music_driver
				DatosScummvm["enable_gs"] = child.firstChildElement("enable_gs").text()		; //15 enable_gs
				DatosScummvm["multi_midi"] = child.firstChildElement("multi_midi").text()	; //16 multi_midi
				DatosScummvm["native_mt32"] = child.firstChildElement("native_mt32").text()	; //17 native_mt32
				DatosScummvm["master_volume"] = child.firstChildElement("master_volume").text()	; //18 master_volume
				DatosScummvm["music_volume"] =	child.firstChildElement("music_volume").text() 	; //19 music_volume
				DatosScummvm["sfx_volume"] =	child.firstChildElement("sfx_volume").text()	; //20 sfx_volume
				DatosScummvm["speech_volume"] =	child.firstChildElement("speech_volume").text()	; //21 speech_volume
				DatosScummvm["tempo"] =	child.firstChildElement("tempo").text()			; //22 tempo
				DatosScummvm["talkspeed"] =	child.firstChildElement("talkspeed").text()		; //23 talkspeed
				DatosScummvm["debuglevel"] = child.firstChildElement("debuglevel").text()	; //24 debuglevel
				DatosScummvm["cdrom"] =	child.firstChildElement("cdrom").text()			; //25 cdrom ui.cbxSvm_6->currentIndex()
				DatosScummvm["joystick_num"] =	child.firstChildElement("joystick_num").text()  ; //26 joystick_num 
			} else if (child.tagName() == "vdmsound") {
				ui.lb_usuario_4->setText(child.firstChildElement("usuario").text() )   ;
				DatosVDMSound.clear();
				DatosVDMSound["path_conf"]     = child.firstChildElement("path_conf").text()	;
				DatosVDMSound["path_exe"]      = child.firstChildElement("path_exe").text()		;
				DatosVDMSound["program"]       = child.firstChildElement("program").text()		;
				DatosVDMSound["vdms_debug"]    = child.firstChildElement("vdms_debug").text()	;
				DatosVDMSound["winnt_dos"]     = child.firstChildElement("winnt_dos").text()	;
				DatosVDMSound["winnt_dosbox"]  = child.firstChildElement("winnt_dosbox").text()	;
				DatosVDMSound["winnt_storage"] = child.firstChildElement("winnt_storage").text();
			} else if (child.tagName() == "dosbox") {
				ui.lb_usuario_2->setText(child.firstChildElement("usuario").text() )   ;
				DatosDosBox.clear();
				DatosDosBox["sdl_fullscreen"] = child.firstChildElement("sdl_fullscreen").text()			; //0 sdl_fullscreen
				DatosDosBox["sdl_fulldouble"] = child.firstChildElement("sdl_fulldouble").text()			; //1 sdl_fulldouble
				DatosDosBox["sdl_fullfixed"] = child.firstChildElement("sdl_fullfixed").text()			; //2 sdl_fullfixed
				DatosDosBox["sdl_fullresolution"] = child.firstChildElement("sdl_fullresolution").text()		; //3 sdl_fullresolution
				DatosDosBox["sdl_windowresolution"] = child.firstChildElement("sdl_windowresolution").text()	; //4 sdl_windowresolution
				DatosDosBox["sdl_output"] = child.firstChildElement("sdl_output").text()				; //5 sdl_output
				DatosDosBox["sdl_hwscale"] =  child.firstChildElement("sdl_hwscale").text()			; //6 sdl_hwscale
				DatosDosBox["sdl_autolock"] = child.firstChildElement("sdl_autolock").text()			; //7 sdl_autolock
				DatosDosBox["sdl_sensitivity"] = child.firstChildElement("sdl_sensitivity").text()		; //8 sdl_sensitivity
				DatosDosBox["sdl_waitonerror"] = child.firstChildElement("sdl_waitonerror").text()		; //9 sdl_waitonerror											
				DatosDosBox["sdl_priority"] = child.firstChildElement("sdl_priority").text()			; //10 sdl_priority
				DatosDosBox["sdl_mapperfile"] = child.firstChildElement("sdl_mapperfile").text()			; //11 sdl_mapperfile
				DatosDosBox["sdl_usescancodes"] = child.firstChildElement("sdl_usescancodes").text()		; //12 sdl_usescancodes
				DatosDosBox["dosbox_language"] = child.firstChildElement("dosbox_language").text()		; //13 dosbox_language
				DatosDosBox["dosbox_machine"] = child.firstChildElement("dosbox_machine").text()			; //14 dosbox_machine
				DatosDosBox["dosbox_captures"] = child.firstChildElement("dosbox_captures").text()		; //15 dosbox_captures
				DatosDosBox["dosbox_memsize"] = child.firstChildElement("dosbox_memsize").text()			; //16 dosbox_memsize
				DatosDosBox["render_frameskip"] = child.firstChildElement("render_frameskip").text()		; //17 render_frameskip
				DatosDosBox["render_aspect"] = child.firstChildElement("render_aspect").text()			; //18 render_aspect
				DatosDosBox["render_scaler"] = child.firstChildElement("render_scaler").text() 			; //19 render_scaler
				DatosDosBox["cpu_core"] = child.firstChildElement("cpu_core").text() 				; //20 cpu_core
				DatosDosBox["cpu_cycles"] = child.firstChildElement("cpu_cycles").text() 			; //21 cpu_cycles
				DatosDosBox["cpu_cycleup"] = child.firstChildElement("cpu_cycleup").text() 			; //22 cpu_cycleup
				DatosDosBox["cpu_cycledown"] = child.firstChildElement("cpu_cycledown").text() 			; //23 cpu_cycledown
				DatosDosBox["mixer_nosound"] = child.firstChildElement("mixer_nosound").text() 			; //24 mixer_nosound
				DatosDosBox["mixer_rate"] = child.firstChildElement("mixer_rate").text()				; //25 mixer_rate
				DatosDosBox["mixer_blocksize"] = child.firstChildElement("mixer_blocksize").text()		; //26 mixer_blocksize
				DatosDosBox["mixer_prebuffer"] = child.firstChildElement("mixer_prebuffer").text() 		; //27 mixer_prebuffer
				DatosDosBox["midi_mpu401"] = child.firstChildElement("midi_mpu401").text() 			; //28 midi_mpu401
				DatosDosBox["midi_intelligent"] = child.firstChildElement("midi_intelligent").text() 		; //29 midi_intelligent
				DatosDosBox["midi_device"] = child.firstChildElement("midi_device").text()			; //30 midi_device
				DatosDosBox["midi_config"] = child.firstChildElement("midi_config").text()			; //31 midi_config
				DatosDosBox["midi_mt32rate"] = child.firstChildElement("midi_mt32rate").text()			; //32 midi_mt32rate
				DatosDosBox["sblaster_sbtype"] = child.firstChildElement("sblaster_sbtype").text() 		; //33 sblaster_sbtype
				DatosDosBox["sblaster_sbbase"] = child.firstChildElement("sblaster_sbbase").text()		; //34 sblaster_sbbase
				DatosDosBox["sblaster_irq"] = child.firstChildElement("sblaster_irq").text()			; //35 sblaster_irq
				DatosDosBox["sblaster_dma"] = child.firstChildElement("sblaster_dma").text()			; //36 sblaster_dma
				DatosDosBox["sblaster_hdma"] = child.firstChildElement("sblaster_hdma").text() 			; //37 sblaster_hdma
				DatosDosBox["sblaster_mixer"] = child.firstChildElement("sblaster_mixer").text() 		; //38 sblaster_mixer
				DatosDosBox["sblaster_oplmode"] = child.firstChildElement("sblaster_oplmode").text() 		; //39 sblaster_oplmode
				DatosDosBox["sblaster_oplrate"] = child.firstChildElement("sblaster_oplrate").text() 		; //40 sblaster_oplrate
				DatosDosBox["gus_gus"] = child.firstChildElement("gus_gus").text()				; //41 gus_gus
				DatosDosBox["gus_gusrate"] = child.firstChildElement("gus_gusrate").text()			; //42 gus_gusrate
				DatosDosBox["gus_gusbase"] = child.firstChildElement("gus_gusbase").text() 			; //43 gus_gusbase
				DatosDosBox["gus_irq1"] = child.firstChildElement("gus_irq1").text()				; //44 gus_irq1
				DatosDosBox["gus_irq2"] = child.firstChildElement("gus_irq2").text() 				; //45 gus_irq2
				DatosDosBox["gus_dma1"] = child.firstChildElement("gus_dma1").text() 				; //46 gus_dma1
				DatosDosBox["gus_dma2"] = child.firstChildElement("gus_dma2").text()				; //47 gus_dma2
				DatosDosBox["gus_ultradir"] = child.firstChildElement("gus_ultradir").text()			; //48 gus_ultradir
				DatosDosBox["speaker_pcspeaker"] = child.firstChildElement("speaker_pcspeaker").text()		; //49 speaker_pcspeaker
				DatosDosBox["speaker_pcrate"] = child.firstChildElement("speaker_pcrate").text()			; //50 speaker_pcrate
				DatosDosBox["speaker_tandy"] = child.firstChildElement("speaker_tandy").text()			; //51 speaker_tandy
				DatosDosBox["speaker_tandyrate"] = child.firstChildElement("speaker_tandyrate").text()		; //52 speaker_tandyrate
				DatosDosBox["speaker_disney"] = child.firstChildElement("speaker_disney").text()			; //53 speaker_disney
				DatosDosBox["joystick_type"] = child.firstChildElement("joystick_type").text()			; //54 joystick_type
				DatosDosBox["joystick_timed"] = child.firstChildElement("joystick_timed").text()			; //86 joystick_timed
				DatosDosBox["joystick_autofire"] = child.firstChildElement("joystick_autofire").text()		; //87 joystick_autofire
				DatosDosBox["joystick_swap34"] = child.firstChildElement("joystick_swap34").text()		; //88 joystick_swap34
				DatosDosBox["joystick_buttonwrap"] = child.firstChildElement("joystick_buttonwrap").text()	; //89 joystick_buttonwrap
				DatosDosBox["modem_modem"] = child.firstChildElement("modem_modem").text()			; //55 modem_modem
				DatosDosBox["modem_comport"] = child.firstChildElement("modem_comport").text()			; //56 modem_comport
				DatosDosBox["modem_listenport"] = child.firstChildElement("modem_listenport").text()		; //57 modem_listenport
				DatosDosBox["dserial_directserial"] = child.firstChildElement("dserial_directserial").text()	; //58 dserial_directserial
				DatosDosBox["dserial_comport"] = child.firstChildElement("dserial_comport").text()		; //59 dserial_comport
				DatosDosBox["dserial_realport"] = child.firstChildElement("dserial_realport").text() 		; //60 dserial_realport
				DatosDosBox["dserial_defaultbps"] = child.firstChildElement("dserial_defaultbps").text() 	; //61 dserial_defaultbps
				DatosDosBox["dserial_parity"] = child.firstChildElement("dserial_parity").text()			; //62 dserial_parity
				DatosDosBox["dserial_bytesize"] = child.firstChildElement("dserial_bytesize").text()		; //63 dserial_bytesize
				DatosDosBox["dserial_stopbit"] = child.firstChildElement("dserial_stopbit").text()		; //64 dserial_stopbit
				DatosDosBox["serial_1"] = child.firstChildElement("serial_1").text()				; //65 serial_1
				DatosDosBox["serial_2"] = child.firstChildElement("serial_2").text()				; //66 serial_2
				DatosDosBox["serial_3"] = child.firstChildElement("serial_3").text()				; //67 serial_3
				DatosDosBox["serial_4"] = child.firstChildElement("serial_4").text()				; //68 serial_4
				DatosDosBox["dos_xms"] = child.firstChildElement("dos_xms").text()				; //69 dos_xms
				DatosDosBox["dos_ems"] = child.firstChildElement("dos_ems").text()				; //70 dos_ems
				DatosDosBox["dos_umb"] = child.firstChildElement("dos_umb").text()				; //71 dos_umb
				DatosDosBox["dos_keyboardlayout"] = child.firstChildElement("dos_keyboardlayout").text()		; //90 dos_keyboardlayout
				DatosDosBox["ipx_ipx"] = child.firstChildElement("ipx_ipx").text()				; //72 ipx_ipx
				DatosDosBox["autoexec"] = child.firstChildElement("autoexec").text()				; //73 autoexec
				DatosDosBox["opt_autoexec"] = child.firstChildElement("opt_autoexec").text()			; //74 opt_autoexec
				DatosDosBox["opt_loadfix"] = child.firstChildElement("opt_loadfix").text()			; //75 opt_loadfix
				DatosDosBox["opt_loadfix_mem"] = child.firstChildElement("opt_loadfix_mem").text()		; //76 opt_loadfix_mem
				DatosDosBox["opt_consola_dbox"] = child.firstChildElement("opt_consola_dbox").text()		; //77 opt_consola_dbox
				DatosDosBox["opt_cerrar_dbox"] = child.firstChildElement("opt_cerrar_dbox").text()		; //78 opt_cerrar_dbox
				DatosDosBox["opt_cycle_sincronizar"] = child.firstChildElement("opt_cycle_sincronizar").text()	; //79 opt_cycle_sincronizar
				DatosDosBox["path_conf"] = child.firstChildElement("path_conf").text()				; //80 path_conf
				DatosDosBox["path_sonido"] = child.firstChildElement("path_sonido").text()			; //81 path_sonido
				DatosDosBox["path_exe"] = child.firstChildElement("path_exe").text()				; //82 path_exe
				DatosDosBox["path_setup"] = child.firstChildElement("path_setup").text()				; //83 path_setup
				DatosDosBox["parametros_exe"] = child.firstChildElement("parametros_exe").text()			; //84 parametros_exe
				DatosDosBox["parametros_setup"] = child.firstChildElement("parametros_setup").text()		; //85 parametros_setup
			} else if (child.tagName() == "montajes") {
				QTreeWidgetItem *childItem = new QTreeWidgetItem(ui.twMontajes);
				
				QString tipoDrive = child.firstChildElement("tipo_as").text();
				if(tipoDrive=="drive")
					childItem->setIcon( 0, QIcon(":/img16/drive_hd.png") );
				if(tipoDrive=="cdrom")
					childItem->setIcon( 0, QIcon(":/img16/drive_cdrom.png") );
				if(tipoDrive=="floppy")
					childItem->setIcon( 0, QIcon(":/img16/drive_floppy.png") );
				if(tipoDrive=="IMG_floppy")
					childItem->setIcon( 0, QIcon(":/img16/floppy_1.png") );
				if(tipoDrive=="IMG_iso")
					childItem->setIcon( 0, QIcon(":/img16/cd_iso.png") );
				if(tipoDrive=="IMG_hdd")
					childItem->setIcon(0, QIcon(":/img16/drive_hd.png") );
				
				childItem->setText(0, child.firstChildElement("path").text()         );
				childItem->setText(1, child.firstChildElement("label").text()        );
				childItem->setText(2, child.firstChildElement("tipo_as").text()      );
				childItem->setText(3, child.firstChildElement("letter").text()       );
				childItem->setText(4, child.firstChildElement("indx_cd").text()      );
				childItem->setText(5, child.firstChildElement("opt_mount").text()    );
				childItem->setText(6, child.firstChildElement("io_ctrl").text()      );
				childItem->setText(7, child.firstChildElement("select_mount").text() );
			} else if (child.tagName() == "separator") {
				QTreeWidgetItem *childItem = new QTreeWidgetItem(ui.twMontajes); 
				childItem->setText(0, QString(30, 0xB7));
			}
			child = child.nextSiblingElement();
		}
	}
}

void frmImportarJuego::downloadFile(QString urlfile, QString fileName)
 {
	QUrl url( urlfile );
	QFileInfo fileInfo( url.path() );
	//QString fileName = fileInfo.fileName();
//    if (fileName.isEmpty())
//        fileName = "index.html";

    //if (QFile::exists(fileName) && (fileName!=xml_InfoJuegos || fileName!=xml_InfoJuegos) ) {
    if ( QFile::exists(fileName) ) {
   //     if (QMessageBox::question(this, "GR-lida", 
   //         tr("There already exists a file called %1 in the current directory. Overwrite?").arg(fileName),
   //         QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel)
   //         == QMessageBox::Cancel)
   //         return;
        QFile::remove(fileName);
    }

	file = new QFile(fileName);
	if (!file->open(QIODevice::WriteOnly)) {
		QMessageBox::information(this, "GR-lida",tr("No se ha podido guardar el archivo %1: %2.").arg(fileName).arg(file->errorString()));
		delete file;
		file = 0;
		return;
	}

  QHttp::ConnectionMode mode = url.scheme().toLower() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp; // Qt 4.3.x
  http->setHost(url.host(), mode, url.port() == -1 ? 0 : url.port()); // Qt 4.3.x
//  http->setHost(url.host(), url.port() != -1 ? url.port() : 80);

  if (!url.userName().isEmpty())
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
	if (requestId != httpGetId)
		return;
	if (httpRequestAborted) {
		if (file) {
			file->close();
			file->remove();
			delete file;
			file = 0;
		}
		progressDialog->hide();
		return;
	}

	if (requestId != httpGetId)
		return;

	progressDialog->hide();
	file->flush();
	file->close();

	if (error) {
		file->remove();
		QMessageBox::information(this, "GR-lida DB", tr("No se ha podido guardar el archivo: %1.").arg(http->errorString()));
	
	} else {
		QString fileName = QFileInfo(QUrl( url_filed ).path()).fileName();
		ui.statusLabel->setText(tr("Descargado en el directorio: %1.").arg(fileName));
		switch ( indx_fin_descarga ) {
			case 0:
				xml_read( xml_ListaJuegos );
			break;
			case 1:
				xml_read( xml_InfoJuegos );
			break;
			case 2:
				ui.textBrowser->setHtml( texto_html );
			break;
		}
	}
	ui.btnUpdateList->setEnabled(true);
	delete file;
	file = 0;
}

void frmImportarJuego::readResponseHeader(const QHttpResponseHeader &responseHeader)
{
	if (responseHeader.statusCode() != 200) {
		QMessageBox::information(this, "GR-lida DB", tr("La descarga ha fallado: %1.").arg(responseHeader.reasonPhrase()));
		httpRequestAborted = true;
		progressDialog->hide();
		http->abort();
		return;
	}
}

void frmImportarJuego::updateDataReadProgress(int bytesRead, int totalBytes)
{
	if (httpRequestAborted)
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
    ui.siteDescription->setText(tr("%1 at %2").arg(authenticator->realm()).arg(hostName));
    
    if (dlg.exec() == QDialog::Accepted) {
        authenticator->setUser(ui.userEdit->text());
        authenticator->setPassword(ui.passwordEdit->text());
    }
}

void frmImportarJuego::on_treeWidget_clicked( QTreeWidgetItem *item)
{
	if( item ){
		indx_fin_descarga = 2;
		url_filed = url_xmldb + "images/covers/small/"+ item->text(2) ;
		downloadFile( url_filed , stHomeDir + "temp_img_thumbs.jpg");
		ui.textBrowser->clear();
		texto_html.clear();
		texto_html.append("<html");
		texto_html.append("<head>");
//		texto_html.append("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\" />");
		texto_html.append("<title>"+item->text(0)+"</title>");
		texto_html.append("</head>");
		texto_html.append("<body>");
		texto_html.append("<img src=\""+stHomeDir+"temp_img_thumbs.jpg\" width=\"145\" height=\"178\" hspace=\"4\" vspace=\"4\" border=\"0\" align=\"right\" />"+ item->text(3) );
//		texto_html.append("<table width=\"100%\" border=\"0\" cellspacing=\"1\" cellpadding=\"2\">");
//		texto_html.append("  <tr>");
//		texto_html.append("    <td width=\"148\" align=\"center\" valign=\"top\"><img src=\""+stHomeDir+"temp_img_thumbs.jpg\" width=\"145\" height=\"180\" border=\"0\" /></td>");
//		texto_html.append("    <td valign=\"top\">"+ item->text(3) +"</td>");
//		texto_html.append("  </tr>");
//		texto_html.append("</table>");
		texto_html.append("</body>");
		texto_html.append("</html>");
		ui.btnNext->setEnabled(true);
	}else{
		ui.btnNext->setEnabled(false);
		return;
	}
}

void frmImportarJuego::on_treeWidget_Dblclicked(QTreeWidgetItem *item)
{
	if( item ){
		ui.twDatosInfo->clear();
		ui.twMontajes->clear();
		ui.twDatosFiles->clear();
		ui.twDatosURL->clear();
		indx_fin_descarga = 1;
		url_filed = url_xmldb + "grlidadb.php?ver=juego&gid="+item->text(1)+"&id_emu="+item->text(5)+"&tipo_emu="+item->text(6);
		downloadFile( url_filed , xml_InfoJuegos );

		ui.btnPrevious->setEnabled(true);
		ui.btnOk->setEnabled(true);
		ui.stackedWidget->setCurrentIndex(1);

		if(item->text(6)=="dosbox")
			ui.tabWidget->setTabEnabled(2,true); else ui.tabWidget->setTabEnabled(2,false);
	}else return;
}
