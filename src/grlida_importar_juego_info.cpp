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

#include "grlida_importar_juego_info.h"

frmImportarJuegoInfo::frmImportarJuegoInfo(QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);

	connect( ui.btnOk        , SIGNAL( clicked() ), this, SLOT( on_btnOk() ) );
	connect( ui.btnVerInfo   , SIGNAL( clicked() ), this, SLOT( on_btnVerInfo() ) );
	connect( ui.twListaJuegos, SIGNAL( currentItemChanged( QTreeWidgetItem *, QTreeWidgetItem *) ), SLOT( on_treeWidget_currentItemChanged( QTreeWidgetItem *, QTreeWidgetItem *) ) );
	connect( ui.twListaJuegos, SIGNAL( itemActivated(QTreeWidgetItem*, int) ), this, SLOT( itemActivated(QTreeWidgetItem*) ) );
	connect( ui.cbxDbXml     , SIGNAL( activated(const QString &)), this, SLOT( on_changeURL_XML(const QString &)));
	connect( ui.btnBuscar    , SIGNAL( clicked() ), this, SLOT( fetch() ) );
	connect( ui.btnAbortar   , SIGNAL( clicked() ), &http, SLOT( abort() ) );
	connect(&http, SIGNAL( readyRead(const QHttpResponseHeader &) ), this, SLOT( readData(const QHttpResponseHeader &) ) );
	connect(&http, SIGNAL( requestFinished(int, bool) ), this, SLOT( finished(int, bool) ) );

	ui.btnVerInfo->setEnabled(false);
	ui.btnAbortar->setEnabled(false);
	ui.btnOk->setEnabled(true);
	
	stHomeDir = fGrl.GRlidaHomePath(); // directorio de trabajo del GR-lida
	stTheme   = fGrl.ThemeGrl();
	setTheme();

	QSettings settings( stHomeDir + "GR-lida.conf", QSettings::IniFormat); 
	settings.beginGroup("OpcGeneral");
		temp_url_xmldb = settings.value("url_xmldb", "http://laisladelabandoware.es/" ).toString(); // GR-lida
	settings.endGroup();

	if( temp_url_xmldb.isEmpty() )
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

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmImportarJuegoInfo::~frmImportarJuegoInfo()
{

}

void frmImportarJuegoInfo::setTheme()
{
	ui.btnBuscar->setIcon( QIcon(stTheme+"img16/edit_buscar.png") );
	ui.btnAbortar->setIcon( QIcon(stTheme+"img16/cancelar.png") );
	ui.btnVerInfo->setIcon( QIcon(stTheme+"img16/datos_2.png") );
	ui.btnOk->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.btnCancelar->setIcon( QIcon(stTheme+"img16/cancelar.png") );
	
	QFile LeerArchivo(stTheme+"tpl_juego_info.html");
	if( LeerArchivo.open( QIODevice::ReadOnly | QIODevice::Text ) )
	{
		QTextStream in( &LeerArchivo );
		in.setCodec("UTF-8");
		str_html_old.clear();
		str_html_old = in.readAll();
	} else
		str_html_old = "";

	LeerArchivo.close();	

	// Reempla las Etiquetas
	//str_html_old.replace("{dir_style_css}", "C:/eclipse/workspace/gr-lida/res/", Qt::CaseSensitive);
	str_html_old.replace("{lb_cover_front}", tr("Carátula Frontal") , Qt::CaseSensitive);
	str_html_old.replace("{lb_detalles_juego}", tr("Detalles del juego"), Qt::CaseSensitive);
	str_html_old.replace("{lb_calificacion}", tr("Calificación"), Qt::CaseSensitive);
	str_html_old.replace("{lb_otros_datos}", tr("Otros Datos"), Qt::CaseSensitive);
	str_html_old.replace("{lb_usuario}", tr("Subido por"), Qt::CaseSensitive);
	str_html_old.replace("{lb_subtitulo}", tr("Subtitulo"), Qt::CaseSensitive);
	str_html_old.replace("{lb_compania}", tr("Publicado por"), Qt::CaseSensitive);
	str_html_old.replace("{lb_desarrollador}", tr("Desarrollado por"), Qt::CaseSensitive);
	str_html_old.replace("{lb_anno}", tr("Publicado"), Qt::CaseSensitive);
	str_html_old.replace("{lb_idioma}", tr("Idioma"), Qt::CaseSensitive);
	str_html_old.replace("{lb_formato}", tr("Formato"), Qt::CaseSensitive);
	str_html_old.replace("{lb_genero}", tr("Genero"), Qt::CaseSensitive);
	str_html_old.replace("{lb_tema}", tr("Tema"), Qt::CaseSensitive);
	str_html_old.replace("{lb_sistemaop}", tr("Sistema Operativo"), Qt::CaseSensitive);
	str_html_old.replace("{lb_favorito}", tr("Favorito"), Qt::CaseSensitive);
	str_html_old.replace("{lb_fecha}", tr("Añadido el"), Qt::CaseSensitive);
	str_html_old.replace("{lb_graficos}", tr("Gráficos"), Qt::CaseSensitive);
	str_html_old.replace("{lb_sonido}", tr("Sonido"), Qt::CaseSensitive);
	str_html_old.replace("{lb_jugabilidad}", tr("Jugabilidad"), Qt::CaseSensitive);
	str_html_old.replace("{lb_original}", tr("Original"), Qt::CaseSensitive);
	str_html_old.replace("{lb_estado}", tr("Estado"), Qt::CaseSensitive);
	str_html_old.replace("{lb_tipo_emu}", tr("Tipo Emu"), Qt::CaseSensitive);
	str_html_old.replace("{lb_comentario}", tr("Descripción"), Qt::CaseSensitive);
}

void frmImportarJuegoInfo::on_changeURL_XML(const QString &url)
{
	url_xmldb = fGrl.url_correcta( url );
}

void frmImportarJuegoInfo::on_btnVerInfo()
{
	if( ui.cbxDbXml->currentText()!="")
		url_xmldb = fGrl.url_correcta( ui.cbxDbXml->currentText() );

	ui.btnOk->setEnabled(true);
	ui.cbxDbXml->setEnabled(false);
	ui.txtTituloBuscar->setReadOnly(true);
	ui.btnBuscar->setEnabled(false);
	ui.btnAbortar->setEnabled(true);

	ui.txtb_info->clear();
	DatosJuego.clear();
	xml.clear();

	QUrl url( url_xmldb );

	http.setHost( url.host() );

	connectionId = http.get( url_xmldb + "grlidadb.php?ver=juego&gid="+ui.twListaJuegos->currentItem()->text(1)+"&id_emu="+ui.twListaJuegos->currentItem()->text(5)+"&tipo_emu="+ui.twListaJuegos->currentItem()->text(6) );
}

void frmImportarJuegoInfo::on_btnOk()
{
	if( !DatosJuego.isEmpty() )
		QDialog::accept();
}

void frmImportarJuegoInfo::fetch()
{
	if( ui.cbxDbXml->currentText()!="")
		url_xmldb = fGrl.url_correcta( ui.cbxDbXml->currentText() );
	
	ui.btnOk->setEnabled(false);
	ui.btnVerInfo->setEnabled(false);
	ui.cbxDbXml->setEnabled(false);
	ui.txtTituloBuscar->setReadOnly(true);
	ui.btnBuscar->setEnabled(false);
	ui.btnAbortar->setEnabled(true);

	ui.txtb_info->clear();	
	DatosJuego.clear();	
	ui.twListaJuegos->clear();
	xml.clear();

	QUrl url( url_xmldb + "grlidadb.php?ver=lista" );

	http.setHost( url.host() );

	if( ui.txtTituloBuscar->text() != "" )
		connectionId = http.get( url_xmldb + "grlidadb.php?ver=lista&titulo="+ui.txtTituloBuscar->text() );
	else
		connectionId = http.get( url_xmldb + "grlidadb.php?ver=lista" );
}

void frmImportarJuegoInfo::readData(const QHttpResponseHeader &resp)
{
	if(resp.statusCode() != 200)
		http.abort();
	else {
		xml.addData( http.readAll() );
		parseXml();
	}
}

void frmImportarJuegoInfo::finished(int id, bool error)
{
	if(error)
	{
		qWarning("Received error during HTTP fetch.");
		ui.cbxDbXml->setEnabled(true);		
		ui.txtTituloBuscar->setReadOnly(false);
		ui.btnAbortar->setEnabled(false);
		ui.btnBuscar->setEnabled(true);
	}
	else if(id == connectionId)
	{
		ui.cbxDbXml->setEnabled(true);
		ui.txtTituloBuscar->setReadOnly(false);
		ui.btnAbortar->setEnabled(false);
		ui.btnBuscar->setEnabled(true);
	}
}

void frmImportarJuegoInfo::parseXml()
{
	while ( !xml.atEnd() )
	{
		xml.readNext();
		
		if( xml.isStartElement() )
		{
			if( xml.name() == "juego")
				str_id = xml.attributes().value("ID").toString();

			currentTag = xml.name().toString();
		}
		else if( xml.isEndElement() )
		{
			if( xml.name() == "datos" )
			{
				texto_html.clear();
				texto_html = str_html_old;

				str_Icon.clear();
				if( DatosJuego["icono"] == "datos" )
					str_Icon = stTheme+"img24/emu_datos.png";
				else if( DatosJuego["icono"] == "dosbox" )
					str_Icon = stTheme+"img24/emu_dbx.png";
				else if( DatosJuego["icono"] == "scummvm" )
					str_Icon = stTheme+"img24/emu_svm.png";
				else if( DatosJuego["icono"] == "vdmsound" )
					str_Icon = stTheme+"img24/emu_vdms.png";
				else 
					str_Icon = stTheme+"img24/emu_sin_imagen.png";

				// Reempla la info del juego.
				texto_html.replace("{info_icono}", str_Icon, Qt::CaseSensitive);
				//texto_html.replace("{info_icono}", "<img src=\""+str_Icon+"\" width=\"24\" height=\"24\">", Qt::CaseSensitive);
				texto_html.replace("{info_usuario}", DatosJuego["usuario"], Qt::CaseSensitive);
				texto_html.replace("{info_titulo}", DatosJuego["titulo"], Qt::CaseSensitive);
				texto_html.replace("{info_cover_front}", DatosJuego["cover_front"], Qt::CaseSensitive);
				texto_html.replace("{info_subtitulo}", DatosJuego["subtitulo"], Qt::CaseSensitive);
				texto_html.replace("{info_compania}", DatosJuego["compania"], Qt::CaseSensitive);
				texto_html.replace("{info_desarrollador}", DatosJuego["desarrollador"], Qt::CaseSensitive);
				texto_html.replace("{info_anno}", DatosJuego["anno"], Qt::CaseSensitive);
				texto_html.replace("{info_idioma}", DatosJuego["idioma"], Qt::CaseSensitive);
				texto_html.replace("{info_formato}", DatosJuego["formato"], Qt::CaseSensitive);
				texto_html.replace("{info_genero}", DatosJuego["genero"], Qt::CaseSensitive);
				texto_html.replace("{info_tema}", DatosJuego["tema"], Qt::CaseSensitive);
				texto_html.replace("{info_sistemaop}", DatosJuego["sistemaop"], Qt::CaseSensitive);
				texto_html.replace("{info_favorito}", DatosJuego["favorito"], Qt::CaseSensitive);
				texto_html.replace("{info_fecha}", DatosJuego["fecha"], Qt::CaseSensitive);
				texto_html.replace("{info_graficos}", DatosJuego["graficos"], Qt::CaseSensitive);
				texto_html.replace("{info_sonido}", DatosJuego["sonido"], Qt::CaseSensitive);
				texto_html.replace("{info_jugabilidad}", DatosJuego["jugabilidad"], Qt::CaseSensitive);
				texto_html.replace("{info_original}", DatosJuego["original"], Qt::CaseSensitive);
				texto_html.replace("{info_estado}", DatosJuego["estado"], Qt::CaseSensitive);
				texto_html.replace("{info_tipo_emu}", DatosJuego["tipo_emu"], Qt::CaseSensitive);
				texto_html.replace("{info_comentario}", DatosJuego["comentario"], Qt::CaseSensitive);

				ui.txtb_info->setHtml( texto_html );
			}
			if( xml.name() == "juego" )
			{
				QTreeWidgetItem *item = new QTreeWidgetItem(ui.twListaJuegos);

				if( DatosJuego["tipo_emu"] == "datos" )
					item->setIcon(0, QIcon(stTheme+"img24/emu_datos.png") );
				else if( DatosJuego["tipo_emu"] == "dosbox" )
					item->setIcon(0, QIcon(stTheme+"img24/emu_dbx.png") );
				else if( DatosJuego["tipo_emu"] == "scummvm" )
					item->setIcon(0, QIcon(stTheme+"img24/emu_svm.png") );
				else if( DatosJuego["tipo_emu"] == "vdmsound" )
					item->setIcon(0, QIcon(stTheme+"img24/emu_vdms.png") );
				else 
					item->setIcon(0, QIcon(stTheme+"img24/emu_sin_imagen.png") );

				item->setText(0, DatosJuego["titulo"]      );
				item->setText(1, str_id                    );
				item->setText(2, DatosJuego["thumbs"]      );
				item->setText(3, DatosJuego["descripcion"] );
				item->setText(4, DatosJuego["pubDate"]     );
				item->setText(5, str_id_emu                );
				item->setText(6, DatosJuego["tipo_emu"]    );
				item->setText(7, DatosJuego["usuario"]     );

				DatosJuego.clear();
				str_id.clear();
				str_id_emu.clear();
			}
		}
		else if( xml.isCharacters() && !xml.isWhitespace() )
		{
			if(currentTag == "usuario")
				DatosJuego["usuario"] = xml.text().toString();
			else if(currentTag == "icono")
				DatosJuego["icono"] = xml.text().toString();
			else if(currentTag == "titulo")
				DatosJuego["titulo"] = xml.text().toString();
			else if(currentTag == "subtitulo")
				DatosJuego["subtitulo"] = xml.text().toString();
			else if(currentTag == "genero")
				DatosJuego["genero"] = xml.text().toString();
			else if(currentTag == "compania")
				DatosJuego["compania"] = xml.text().toString();
			else if(currentTag == "desarrollador")
				DatosJuego["desarrollador"] = xml.text().toString();
			else if(currentTag == "tema")
				DatosJuego["tema"] = xml.text().toString();
			else if(currentTag == "idioma")
				DatosJuego["idioma"] = xml.text().toString();
			else if(currentTag == "formato")
				DatosJuego["formato"] = xml.text().toString();
			else if(currentTag == "anno")
				DatosJuego["anno"] = xml.text().toString();
			else if(currentTag == "numdisc")
				DatosJuego["numdisc"] = xml.text().toString();
			else if(currentTag == "sistemaop")
				DatosJuego["sistemaop"] = xml.text().toString();
			else if(currentTag == "tamano")
				DatosJuego["tamano"] = xml.text().toString();
			else if(currentTag == "graficos")
				DatosJuego["graficos"] = xml.text().toString();
			else if(currentTag == "sonido")
				DatosJuego["sonido"] = xml.text().toString();
			else if(currentTag == "jugabilidad")
				DatosJuego["jugabilidad"] = xml.text().toString();
			else if(currentTag == "original")
				DatosJuego["original"] = xml.text().toString();
			else if(currentTag == "estado")
				DatosJuego["estado"] = xml.text().toString();
			else if(currentTag == "thumbs")
				DatosJuego["thumbs"] = xml.text().toString();
			else if(currentTag == "cover_front")
				DatosJuego["cover_front"] = xml.text().toString();
			else if(currentTag == "cover_back")
				DatosJuego["cover_back"] = xml.text().toString();
			else if(currentTag == "fecha")
				DatosJuego["fecha"] = xml.text().toString();
			else if(currentTag == "descripcion")
				DatosJuego["descripcion"] = xml.text().toString();
			else if(currentTag == "pubDate")
				DatosJuego["pubDate"] = xml.text().toString();
			else if(currentTag == "tipo_emu")
				DatosJuego["tipo_emu"] = xml.text().toString();
			else if(currentTag == "comentario")
				DatosJuego["comentario"] = xml.text().toString();
			else if(currentTag == "id_emu")
				str_id_emu = xml.text().toString();

			DatosJuego["favorito"] = "false";
		}
	}
	if(xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError)
	{
		qWarning() << "XML ERROR:" << xml.lineNumber() << ": " << xml.errorString();
		http.abort();
	}
}

void frmImportarJuegoInfo::on_treeWidget_currentItemChanged(QTreeWidgetItem *item1, QTreeWidgetItem *item2)
{
	if( (item1)&&(item2) )
	{
		ui.btnOk->setEnabled(false);
		ui.btnVerInfo->setEnabled(true);
	} else
		return;
}

void frmImportarJuegoInfo::on_treeWidget_Dblclicked( QTreeWidgetItem *item)
{
	ui.btnOk->setEnabled(false);
	ui.btnVerInfo->setEnabled(true);
	ui.btnVerInfo->click();
}

void frmImportarJuegoInfo::itemActivated(QTreeWidgetItem * item)
{
	ui.btnOk->setEnabled(false);
	ui.btnVerInfo->setEnabled(true);
	ui.btnVerInfo->click();
}
