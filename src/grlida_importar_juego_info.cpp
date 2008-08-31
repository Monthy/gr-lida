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

frmImportarJuegoInfo::frmImportarJuegoInfo(QString titulo_busqueda, QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);

	stHomeDir = fGrl.GRlidaHomePath(); // directorio de trabajo del GR-lida
	stTheme   = fGrl.ThemeGrl();
	setTheme();

	createConnections();

	ui.txtTituloBuscar->setText( titulo_busqueda );
	ui.btnVerInfo->setEnabled(false);
	ui.btnAbortar->setEnabled(false);
	ui.btnOk->setEnabled(true);

	ui.twListaBusqueda->header()->setStretchLastSection(false);
	ui.twListaBusqueda->header()->setMovable(false);
	adjustarHeader();
	//ui.twListaBusqueda->header()->setResizeMode(0, QHeaderView::Stretch );
	//ui.twListaBusqueda->header()->setResizeMode(1, QHeaderView::ResizeToContents );
	//ui.twListaBusqueda->header()->setResizeMode(2, QHeaderView::Fixed   );
	//ui.twListaBusqueda->setColumnWidth(1,90);
	//ui.twListaBusqueda->setColumnWidth(2,40);
	//ui.twListaBusqueda->setContextMenuPolicy(Qt::CustomContextMenu);

	ui.cbxDbXml->clear();
	ui.cbxDbXml->addItem("http://www.mobygames.com/");
	ui.cbxDbXml->addItem("http://www.laisladelabandoware.es/");
	ui.cbxDbXml->setCurrentIndex(0);
	stUrlSelect = ui.cbxDbXml->currentText()+"/";

	stFileBusqueda   = stHomeDir+"temp/busqueda.html";
	stFileFichaJuego = stHomeDir+"temp/ficha.html";

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmImportarJuegoInfo::~frmImportarJuegoInfo()
{
	//
}

void frmImportarJuegoInfo::createConnections()
{
	ImportPathNew = new frmImportPath();
	connect( ui.cbxDbXml  , SIGNAL( activated(QString)), this, SLOT( on_changeURL_XML(QString)));
	connect( ui.btnBuscar , SIGNAL( clicked() ), this, SLOT( on_Buscar() ));
	connect( ui.btnVerInfo, SIGNAL( clicked() ), this, SLOT( on_btnVerInfo() ) );
	connect( ui.btnOk     , SIGNAL( clicked() ), this, SLOT( on_btnOk() ) );

	connect( ui.twListaBusqueda, SIGNAL( currentItemChanged( QTreeWidgetItem *, QTreeWidgetItem *) ), SLOT( on_twListaBusqueda_currentItemChanged( QTreeWidgetItem *, QTreeWidgetItem *) ) );
	connect( ui.twListaBusqueda, SIGNAL( itemClicked( QTreeWidgetItem*, int)), this, SLOT( on_twListaBusqueda_clicked(QTreeWidgetItem*)));
	connect( ui.twListaBusqueda, SIGNAL( itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( on_twListaBusqueda_Dblclicked(QTreeWidgetItem*) ));

	httpdown = new HttpDownload(ImportPathNew);
	connect(httpdown, SIGNAL( StatusRequestFinished() ), this, SLOT( isRequestFinished() ) );
	connect(httpdown, SIGNAL( statusLabelChanged(QString) ), ui.lb_status, SLOT( setText(QString) ) );
	connect(httpdown, SIGNAL( StatusBtnDownloadChanged(bool) ), ui.btnBuscar, SLOT( setEnabled(bool) ) );
	connect(httpdown, SIGNAL( StatusBtnDownloadChanged(bool) ), ui.twListaBusqueda, SLOT( setEnabled(bool) ) );
	connect(httpdown, SIGNAL( StatusBtnDownloadChanged(bool) ), ImportPathNew->ui.btnOk, SLOT( setEnabled(bool) ) );
	connect(ui.btnAbortar, SIGNAL( clicked() ), httpdown, SLOT( cancelDownload() ) );
}

void frmImportarJuegoInfo::setTheme()
{
	setStyleSheet( fGrl.StyleSheet() );

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
	str_html_old.replace("{lb_rating}", tr("Rating"), Qt::CaseSensitive);
}

void frmImportarJuegoInfo::adjustarHeader()
{
	ui.twListaBusqueda->header()->setResizeMode(0, QHeaderView::Stretch );
	ui.twListaBusqueda->header()->setResizeMode(1, QHeaderView::ResizeToContents );
	ui.twListaBusqueda->header()->setResizeMode(2, QHeaderView::Fixed   );
	//ui.twListaBusqueda->setColumnWidth(1,90);
	ui.twListaBusqueda->setColumnWidth(2,40);
}

void frmImportarJuegoInfo::on_btnOk()
{
	ImportPathNew->ui.gBox_path_emu->setEnabled( false );
	ImportPathNew->ui.gBox_path_emu->setTitle("Path");

	if(DatosJuego["thumbs"]=="null" || DatosJuego["thumbs"].isEmpty() )
	{
		img_thumbs = "null";
		ImportPathNew->ui.txtDatosPath_Thumbs->setText("");
		ImportPathNew->ui.txtDatosPath_Thumbs->setEnabled(false);
		ImportPathNew->ui.btnDirPath_Datos_Thumbs->setEnabled(false);
	} else {
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
		indx_fin_descarga = CargarCoverFront;
		if( !file_cover_front.exists( img_cover_front ) )
		{
			httpdown->downloadFile( stUrlSelect+"images/covers/large/"+DatosJuego["cover_front"], img_cover_front);
		}
		ImportPathNew->ui.txtDatosPath_CoverFront->setText( stHomeDir+"covers/cover_front_"+DatosJuego["cover_front"] );
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
		indx_fin_descarga = CargarCoverBack;
		if( !file_cover_front.exists( img_cover_back ) )
		{
			httpdown->downloadFile( stUrlSelect+"images/covers/large/"+DatosJuego["cover_back"], img_cover_back);
		}
		ImportPathNew->ui.txtDatosPath_CoverBack->setText( stHomeDir+"covers/cover_back_"+DatosJuego["cover_back"] );
		ImportPathNew->ui.txtDatosPath_CoverBack->setEnabled(true);
		ImportPathNew->ui.btnDirPath_Datos_CoverBack->setEnabled(true);
	}

	if( ImportPathNew->exec() == QDialog::Accepted )
	{
		DatosJuego["thumbs"]      = ImportPathNew->ui.txtDatosPath_Thumbs->text();		// thumbs
		DatosJuego["cover_front"] = ImportPathNew->ui.txtDatosPath_CoverFront->text();	// cover_front
		DatosJuego["cover_back"]  = ImportPathNew->ui.txtDatosPath_CoverBack->text();	// cover_back

		if(DatosJuego["thumbs"]!="null" || !DatosJuego["thumbs"].isEmpty() )
		{
			if( file_thumbs.exists( img_thumbs ) )
			{
				file_thumbs.copy( img_thumbs, DatosJuego["thumbs"] );
				//file_thumbs.remove( stHomeDir + "temp/thumbs_" + img_thumbs );
			}
		}

		if(DatosJuego["cover_front"]!="null" || !DatosJuego["cover_front"].isEmpty() )
		{
			if( file_cover_front.exists( img_cover_front ) )
			{
				file_cover_front.copy( img_cover_front, DatosJuego["cover_front"] );
				file_cover_front.remove( img_cover_front );
			}
		}

		if(DatosJuego["cover_back"]!="null" || !DatosJuego["cover_back"].isEmpty() )
		{
			if( file_cover_back.exists( img_cover_back ) )
			{
				file_cover_back.copy( img_cover_back , DatosJuego["cover_back"] );
				file_cover_back.remove( img_cover_back );
			}
		}

		QDialog::accept();
	} else
		QDialog::rejected();
}

void frmImportarJuegoInfo::on_Abortar()
{
	//
}

void frmImportarJuegoInfo::MostrarFichaHtml(QHash<QString, QString> datos)
{
	QString str_Icon;
	ui.txtb_info->clear();
	str_html_new.clear();
	str_html_new = str_html_old;

	str_Icon.clear();
	if( datos["icono"] == "datos" )
		str_Icon = stTheme+"img24/emu_datos.png";
	else if( datos["icono"] == "dosbox" )
		str_Icon = stTheme+"img24/emu_dbx.png";
	else if( datos["icono"] == "scummvm" )
		str_Icon = stTheme+"img24/emu_svm.png";
	else if( datos["icono"] == "vdmsound" )
		str_Icon = stTheme+"img24/emu_vdms.png";
	else
		str_Icon = stTheme+"img24/emu_sin_imagen.png";

//	img_thumbs      = "C:/eclipse/workspace/ScriptMobyGames/temp/thumbs_"+img_filename;
//	img_cover_front = "C:/eclipse/workspace/ScriptMobyGames/temp/cover_front_"+img_filename;

// Reempla la info del juego.
	str_html_new.replace("{info_icono}", str_Icon, Qt::CaseSensitive);
	//str_html_new.replace("{info_icono}", "<img src=\""+str_Icon+"\" width=\"24\" height=\"24\">", Qt::CaseSensitive);
	str_html_new.replace("{info_usuario}", datos["usuario"], Qt::CaseSensitive);
	str_html_new.replace("{info_titulo}", datos["titulo"], Qt::CaseSensitive);
//	str_html_new.replace("{info_thumbs}", stHomeDir + "temp/thumbs_"+datos["thumbs"], Qt::CaseSensitive);
//	str_html_new.replace("{info_thumbs}",  datos["thumbs"], Qt::CaseSensitive);
	if(img_thumbs=="")
		str_html_new.replace("{info_thumbs}",  ":/images/sin_imagen.png", Qt::CaseSensitive);
	else
		str_html_new.replace("{info_thumbs}",  img_thumbs, Qt::CaseSensitive);
	str_html_new.replace("{info_subtitulo}", datos["subtitulo"], Qt::CaseSensitive);
	str_html_new.replace("{info_compania}", datos["compania"], Qt::CaseSensitive);
	str_html_new.replace("{info_desarrollador}", datos["desarrollador"], Qt::CaseSensitive);
	str_html_new.replace("{info_anno}", datos["anno"], Qt::CaseSensitive);
	str_html_new.replace("{info_idioma}", datos["idioma"], Qt::CaseSensitive);
	str_html_new.replace("{info_formato}", datos["formato"], Qt::CaseSensitive);
	str_html_new.replace("{info_genero}", datos["genero"], Qt::CaseSensitive);
	str_html_new.replace("{info_tema}", datos["tema"], Qt::CaseSensitive);
	str_html_new.replace("{info_sistemaop}", datos["sistemaop"], Qt::CaseSensitive);
	str_html_new.replace("{info_favorito}", datos["favorito"], Qt::CaseSensitive);
	str_html_new.replace("{info_fecha}", datos["fecha"], Qt::CaseSensitive);
	str_html_new.replace("{info_graficos}", datos["graficos"], Qt::CaseSensitive);
	str_html_new.replace("{info_sonido}", datos["sonido"], Qt::CaseSensitive);
	str_html_new.replace("{info_jugabilidad}", datos["jugabilidad"], Qt::CaseSensitive);
	str_html_new.replace("{info_original}", datos["original"], Qt::CaseSensitive);
	str_html_new.replace("{info_estado}", datos["estado"], Qt::CaseSensitive);
	str_html_new.replace("{info_tipo_emu}", datos["tipo_emu"], Qt::CaseSensitive);
	str_html_new.replace("{info_comentario}", datos["comentario"], Qt::CaseSensitive);
	str_html_new.replace("{info_rating}", datos["rating"], Qt::CaseSensitive);

	ui.txtb_info->setHtml( str_html_new );

	ui.twListaBusqueda->setEnabled(true);
	ui.btnVerInfo->setEnabled(true);
	ui.btnOk->setEnabled(true);
}

void frmImportarJuegoInfo::on_changeURL_XML(const QString &url)
{
	stUrlSelect = fGrl.url_correcta( url );
}

void frmImportarJuegoInfo::on_twListaBusqueda_currentItemChanged(QTreeWidgetItem *item1, QTreeWidgetItem *item2)
{
	if( (item1)&&(item2) )
	{
		emit on_twListaBusqueda_clicked( ui.twListaBusqueda->currentItem() );
	} else
		return;
}

void frmImportarJuegoInfo::on_twListaBusqueda_clicked(QTreeWidgetItem *item)
{
	if( item )
	{
		ui.btnVerInfo->setEnabled(true);
	}
}

void frmImportarJuegoInfo::on_twListaBusqueda_Dblclicked(QTreeWidgetItem *item)
{
	if( item )
	{
		ui.btnVerInfo->setEnabled(true);
		ui.btnVerInfo->click();
	}
}

QString frmImportarJuegoInfo::LeerArchivoHTML(QString str_file_html)
{
	QString text_html;
	QFile file_html( str_file_html );
	if( file_html.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
		QTextStream in( &file_html );
		in.setCodec("UTF-8");
		text_html = in.readAll();
	} else
		text_html = "";

	file_html.close();

	text_html.replace("&#x27;","'");
	text_html.replace("&#x26;","&");
	text_html.replace("&nbsp;"," ");
//	text_html.replace("&#xE9;","é");
//	text_html.replace("&#xE0;","à");

	return text_html;
}

void frmImportarJuegoInfo::on_Buscar()
{
	QString url_filed;

	ui.btnOk->setEnabled(false);
	ui.btnVerInfo->setEnabled(false);
	ui.txtb_info->clear();
	if( stUrlSelect.contains("www.mobygames.com") )
	{
		// http://www.mobygames.com/search/quick/p,-1/q,Alone/showOnly,9/
		if( ui.txtTituloBuscar->text() != "" )
			url_filed = "http://www.mobygames.com/search/quick/p,-1/q,"+ui.txtTituloBuscar->text()+"/showOnly,9/";
	}
	else if( stUrlSelect.contains("www.laisladelabandoware.es") )
	{
		// http://www.laisladelabandoware.es/grlidadb.php?ver=lista&titulo=Alone
		if( ui.txtTituloBuscar->text() != "" )
			url_filed = "http://www.laisladelabandoware.es/grlidadb.php?ver=lista&titulo="+ui.txtTituloBuscar->text();
		else
			url_filed = "http://www.laisladelabandoware.es/grlidadb.php?ver=lista";
	}

	indx_fin_descarga = AnalizarPaginaBusqueda;
	httpdown->downloadFile(url_filed, stFileBusqueda);
}

void frmImportarJuegoInfo::on_btnVerInfo()
{
	int pos = ui.twListaBusqueda->indexOfTopLevelItem( ui.twListaBusqueda->currentItem() );
	if( ui.twListaBusqueda->topLevelItemCount()>0 && pos!=-1 )
	{
		QString url_filed;

		ui.btnOk->setEnabled(false);
		ui.twListaBusqueda->setEnabled(false);
		ui.btnVerInfo->setEnabled(false);
		ui.txtb_info->clear();
		if( stUrlSelect.contains("www.mobygames.com") )
		{
			indx_fin_descarga = AnalizarPaginaFichaMobyGames;
			url_filed = ui.twListaBusqueda->currentItem()->text(3);
		}
		else if( stUrlSelect.contains("www.laisladelabandoware.es") )
		{
			indx_fin_descarga = AnalizarPaginaFichaLIDA;
			url_filed = ui.twListaBusqueda->currentItem()->text(3);
		}
		httpdown->downloadFile(url_filed, stFileFichaJuego);
	}
}

void frmImportarJuegoInfo::isRequestFinished()
{
	switch( indx_fin_descarga )
	{
		case NohacerNada:
			//
		break;
		case AnalizarPagina:
			//AnalyzeFindPage( LeerArchivoHTML(stFileBusqueda) );
			//AnalyzeGamePageMobyGames( LeerArchivoHTML(stFileFichaJuego) );
		break;
		case AnalizarPaginaBusqueda:
			AnalyzeFindPage( LeerArchivoHTML(stFileBusqueda) );
		break;
		case AnalizarPaginaFichaLIDA:
			AnalyzeGamePageLIDA( LeerArchivoHTML(stFileFichaJuego) );
		break;
		case AnalizarPaginaFichaMobyGames:
			AnalyzeGamePageMobyGames( LeerArchivoHTML(stFileFichaJuego) );
		break;
		case CargarThumb:
//			ui.label_1->setPixmap(QPixmap(img_thumbs));
			MostrarFichaHtml(DatosJuego);
/*
			indx_fin_descarga = CargarCoverFront;
			if( file_cover_front.exists( img_cover_front ) )
				ui.label_2->setPixmap(QPixmap(img_cover_front));
			else
				httpdown->downloadFile(stUrlSelect + "images/covers/large/"+img_filename, img_cover_front);
*/
		break;
		case CargarCoverFront:
			//
		break;
		case CargarCoverBack:
			//
		break;
	}
}

void frmImportarJuegoInfo::AnalyzeFindPage(QString Page)
{
	if( stUrlSelect.contains("www.mobygames.com") )
	{
		if( Page.indexOf("found the following results:") != -1)
		{
			AddGamesTitlesMobyGames( Page );
			ui.lb_status->setText("añadiendo");
		} else
			ui.lb_status->setText("nada");
	}
	else if( stUrlSelect.contains("www.laisladelabandoware.es") )
	{
		AddGamesTitlesLIDA( Page );
	}

}

//---------------------------------------------------------------------
// PARTE: MOBYGAMES
//---------------------------------------------------------------------
void frmImportarJuegoInfo::AnalyzeGamePageMobyGames(QString Page)
{
	QString url_filed;
	QRegExp rx;
	int pos = 0;

	DatosJuego.clear();

	rx.setPatternSyntax( QRegExp::RegExp );

// Titulo ------------------
// <div id="gameTitle"><a href="/game/dos/alone-in-the-dark">Alone in the Dark</a></div>
	rx.setPattern("<div id=\"gameTitle\"><a href=\"/game/([^/]+)/([^\"]+)\">([^\"<>]*)</a></div>");
	pos = 0;
	while((pos = rx.indexIn(Page, pos)) != -1 )
	{
		DatosJuego["titulo"] = rx.cap(3);
		pos += rx.matchedLength();
	}
// FIN Titulo --------------
	if( Page.indexOf("Developed by</div>") != -1)
	{
// Published by ------------
		DatosJuego["compania"] = AnalyzeCategoriasMobyGames(Page, "Published by</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Developed by",
									"<a href=\"/company/([^/]+)\">([^\"<>]*)</a>");
// Developed by ------------
		DatosJuego["desarrollador"] = AnalyzeCategoriasMobyGames(Page, "Developed by</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Released",
									"<a href=\"/company/([^/]+)\">([^\"<>]*)</a>");
	} else {
// Published by ------------
		DatosJuego["compania"] = AnalyzeCategoriasMobyGames(Page, "Published by</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Released",
									"<a href=\"/company/([^/]+)\">([^\"<>]*)</a>");
		DatosJuego["desarrollador"] = "nada";
	}
// Released ---------------
	DatosJuego["anno"] = AnalyzeCategoriasMobyGames(Page, "Released</div><div [^>]*>([^?]*)</div>","<a href=\"/game/([^\"<>]*)/release-info\">([^\"<>]*)</a>");

	if( Page.indexOf("Platforms</div>") != -1)
	{
// Platforms --------------
		DatosJuego["sistemaop"] = AnalyzeCategoriasMobyGames(Page, "Platforms</div><div [^>]*>([^?]*)</div></div></td>",
									"<a href=\"/game/([^\"<>]*)\">([^\"<>]*)</a>");
	}
	else if( Page.indexOf("Platform</div>") != -1)
	{
// Platform ---------------
		DatosJuego["sistemaop"] = AnalyzeCategoriasMobyGames(Page, "Platform</div><div [^>]*>([^?]*)</div></div></td>\n    <td width=\"48%\"><div id=\"coreGameGenre\">",
									"<a href=\"([^\"<>]*)\">([^\"<>]*)</a>");
	}
	if( Page.indexOf("Also For</div>") != -1)
	{
// Also For ---------------
		DatosJuego["sistemaop"] = AnalyzeCategoriasMobyGames(Page, "Also For</div><div [^>]*>([^?]*)</div></div></td>",
									"<a href=\"/game/([^\"<>]*)\">([^\"<>]*)</a>");
	}

	if( Page.indexOf("aDeSe Rating</div>") != -1)
	{
// aDeSe Rating -----------
		DatosJuego["adese_rating"] = AnalyzeCategoriasMobyGames(Page, "aDeSe Rating</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Genre",
									"<a href=\"/attribute/([^\"<>]*)\">([^\"<>]*)</a>");
	}else
		DatosJuego["adese_rating"] = "nada";

	if( Page.indexOf("Perspective</div>") != -1)
	{
// Genre ------------------
		DatosJuego["genero"] = AnalyzeCategoriasMobyGames(Page, "Genre</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Perspective",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>");
// Perspective ------------
		DatosJuego["perspective"] = AnalyzeCategoriasMobyGames(Page, "Perspective</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Non-Sport",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>");
	} else {
// Genre ------------------
		DatosJuego["genero"] = AnalyzeCategoriasMobyGames(Page, "Genre</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Non-Sport",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>");
		DatosJuego["perspective"] = "nada";
	}
	if( Page.indexOf("Misc</div>") != -1)
	{
// Non-Sport --------------
		DatosJuego["tema"] = AnalyzeCategoriasMobyGames(Page, "Non-Sport</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Misc",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>");
// Misc -------------------
		DatosJuego["misc"] = AnalyzeCategoriasMobyGames(Page, "Misc</div><div [^>]*>([^?]*)</div></div></div></td>",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>");
	} else {
// Non-Sport --------------
		DatosJuego["tema"] = AnalyzeCategoriasMobyGames(Page, "Non-Sport</div><div [^>]*>([^?]*)</div></div></div></td>",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>");
		DatosJuego["misc"] = "nada";
	}
// Description ------------
	rx.setPattern("<h2 class=\"m5\">Description</h2>([^[]*)<div class=\"sideBarLinks\">");
	pos = 0;
	while((pos = rx.indexIn(Page, pos)) != -1 )
	{
		DatosJuego["comentario"] = rx.cap(1).replace("<a href=\"","<a href=\"http://www.mobygames.com");
		pos += rx.matchedLength();
	}
// MobyScore --------------
	//	<div class="fr scoreBoxMed scoreHi">4.0</div>
	//  <div class="fr scoreBoxMed scoreMed">3.1</div>
	//  <div class="fr scoreBoxMed scoreNone">...</div>
	rx.setPattern("<div class=\"fr scoreBoxMed([^\"<>]*)\">([^\"<>]*)</div>");
	pos = 0;
	while((pos = rx.indexIn(Page, pos)) != -1 )
	{
		QString strRating;
		int rating = qRound( rx.cap(2).toDouble() ); //qreal valueA = 2.3;
		DatosJuego["rating"] = strRating.setNum(rating);
		pos += rx.matchedLength();
	}

	DatosJuego["icono"]       = "datos";
	DatosJuego["subtitulo"]   = "";
	DatosJuego["idioma"]      = "";
	DatosJuego["formato"]     = "";
	DatosJuego["numdisc"]     = "";
	DatosJuego["tamano"]      = "";
	DatosJuego["graficos"]    = "1";
	DatosJuego["sonido"]      = "1";
	DatosJuego["jugabilidad"] = "1";
	DatosJuego["original"]    = "false";
	DatosJuego["estado"]      = "";
	DatosJuego["cover_back"]  = "";
	DatosJuego["fecha"]       = "";
	DatosJuego["tipo_emu"]    = "datos";
	DatosJuego["favorito"]    = "false";

// Covers Small - Large ---
	//http://www.mobygames.com/images/covers/small/1122389272-00.jpg
	//http://www.mobygames.com/images/covers/large/1122389272-00.jpg
	img_filename.clear();
	img_thumbs.clear();
	img_cover_front.clear();
	if( Page.indexOf("http://www.mobygames.com/images/covers/small/notonfile.gif",0) != -1)
	{
		img_filename    = "";
		img_thumbs      = "";
		img_cover_front = "";
	}
	else if( Page.indexOf("http://www.mobygames.com/images/covers/small/",0) != -1)
	{
		rx.setPattern("<a href=\"/game/([^/]+)/([^\"]+)/cover-art/gameCoverId,([0-9]+)/\"><img alt=\"([^?]*)\" src=\"http://www.mobygames.com/images/covers/small/([^\"]+)\"");
		pos = 0;
		while((pos = rx.indexIn(Page, pos)) != -1 )
		{
			img_filename = rx.cap(5);
			pos += rx.matchedLength();
		}
		img_thumbs      = stHomeDir+"temp/thumbs_"+img_filename;
		img_cover_front = stHomeDir+"temp/cover_front_"+img_filename;
	}
	DatosJuego["thumbs"]      = img_filename;
	DatosJuego["cover_front"] = img_filename;

	indx_fin_descarga = CargarThumb;
	if(!img_filename.isEmpty())
	{
		if( !file_thumbs.exists( img_thumbs ) )
		{
			HttpDownload *httpdown_dos = new HttpDownload();
			connect(httpdown_dos, SIGNAL( StatusRequestFinished() ), this, SLOT( isRequestFinished() ) );

			httpdown_dos->downloadFile(stUrlSelect+"images/covers/small/"+img_filename, img_thumbs);
		} else
			isRequestFinished();
	}else {
//		ui.txtb_info->setHtml("sin imagen: "+img_filename);
//		ui.label_1->setPixmap(QPixmap(":/images/sin_imagen.png"));
//		ui.label_2->setPixmap(QPixmap(":/images/sin_imagen.png"));
		MostrarFichaHtml(DatosJuego);
	}
/*
	ui.listWidget->clear();
	ui.listWidget->addItem("Ficha del Juego");
	ui.listWidget->addItem("Titulo: "+ DatosJuego["titulo"] );
	ui.listWidget->addItem("Genre: " + DatosJuego["genero"] );
	ui.listWidget->addItem("Published by: "+ DatosJuego["compania"] );
	ui.listWidget->addItem("Developed by: "+DatosJuego["desarrollador"] );
	ui.listWidget->addItem("Non-Sport: "+DatosJuego["tema"] );
	ui.listWidget->addItem("Released: "+DatosJuego["anno"] );
	ui.listWidget->addItem("Platform: "+ DatosJuego["sistemaop"]);
	ui.listWidget->addItem("Platforms: "+ DatosJuego["sistemaop"]);
	ui.listWidget->addItem("Also For: "+ DatosJuego["sistemaop"]);
	ui.listWidget->addItem("Covers Small: "+DatosJuego["thumbs"] );
	ui.listWidget->addItem("Covers Large: "+DatosJuego["cover_front"] );
	ui.listWidget->addItem("Description: "+DatosJuego["comentario"] );
	ui.listWidget->addItem("MobyScore: "+DatosJuego["rating"] );
//-- extras
	ui.listWidget->addItem("aDeSe Rating: "+DatosJuego["adese_rating"]);
	ui.listWidget->addItem("Perspective: "+DatosJuego["perspective"] );
	ui.listWidget->addItem("Misc: "+DatosJuego["misc"] );
*/
}

QString frmImportarJuegoInfo::AnalyzeCategoriasMobyGames(QString Page, QString stRegExp, QString stRegExpDos, int indxExp)
{
	QRegExp rx;
	QStringList lista;
	QString strTemp;
	int pos = 0;

	rx.setPattern( stRegExp );
	pos = 0;
	while((pos = rx.indexIn(Page, pos)) != -1 )
	{
		strTemp = rx.cap(1);
		pos += rx.matchedLength();
	}

	rx.setPattern( stRegExpDos );
	pos = 0;
	lista.clear();
	while((pos = rx.indexIn(strTemp, pos)) != -1 )
	{
		lista << rx.cap( indxExp);
		pos += rx.matchedLength();
	}

	return lista.join(", ");
}

void frmImportarJuegoInfo::AddGamesTitlesMobyGames(QString ResultsPage)
{
// Expresion Regular para obtener los datos de la Busqueda
// <a href="/game/dos/alone-in-the-dark">Alone in the Dark</a><br>by Infogrames -- 1992<br>DOS<br>
// rx.cap(0) = toda la url
// <a href="/game/(1)/(2)">(3)</a><br>by (4) -- (5)<br>(6)<br>
	QRegExp rx("<a href=\"/game/([^/]+)/([^\"]+)\">([^\"<>]*)</a><br>by ([^\"<>]*) -- ([^\"<>]*)<br>([^\"<>]*)<br>");
	int pos = 0;

	ui.twListaBusqueda->clear();
	while((pos = rx.indexIn(ResultsPage, pos)) != -1 )
	{
		//Para generar la URL de la ficha
		//http://www.mobygames.com/game/dos/alone-in-the-dark
		QString url_ficha = "http://www.mobygames.com/game/"+rx.cap(1)+"/"+rx.cap(2);

		QTreeWidgetItem *Item = new QTreeWidgetItem(ui.twListaBusqueda);
		Item->setText(0, rx.cap(3) ); // Titulo
		Item->setText(1, rx.cap(6) ); // Plataforma
		Item->setText(2, rx.cap(5) ); // Fecha
		Item->setText(3, url_ficha ); // url ficha

		pos += rx.matchedLength();
	}
	adjustarHeader();
}

//---------------------------------------------------------------------
// PARTE: LA ISLA DEL ABANDOWARE
//---------------------------------------------------------------------
void frmImportarJuegoInfo::AnalyzeGamePageLIDA(QString Page)
{
	QString currentTag, str_id, str_titulo;

	DatosJuego.clear();
	img_filename.clear();
	img_thumbs.clear();
	img_cover_front.clear();

	xml.clear();
	xml.addData( Page );
	while ( !xml.atEnd() )
	{
		xml.readNext();

		if( xml.isStartElement() )
			currentTag = xml.name().toString();
		else if( xml.isEndElement() )
		{
			if( xml.name() == "datos" )
			{
				img_filename    = DatosJuego["thumbs"];
				img_thumbs      = stHomeDir+"temp/thumbs_"+DatosJuego["thumbs"];
				img_cover_front = stHomeDir+"temp/cover_front_"+DatosJuego["cover_front"];

				if(DatosJuego["thumbs"]=="" && DatosJuego["thumbs"]=="null")
					img_thumbs      = "";
				if(DatosJuego["cover_front"]=="" && DatosJuego["cover_front"]=="null")
					img_cover_front = "";

				indx_fin_descarga = CargarThumb;
				if( !file_thumbs.exists( img_thumbs ) )
				{
					HttpDownload *httpdown_dos = new HttpDownload();
					connect(httpdown_dos, SIGNAL( StatusRequestFinished() ), this, SLOT( isRequestFinished() ) );

					httpdown_dos->downloadFile(stUrlSelect+"images/covers/small/"+img_filename, img_thumbs);
				} else
					isRequestFinished();
			}
		}
		else if( xml.isCharacters() && !xml.isWhitespace() )
		{
			if(currentTag == "usuario")
				DatosJuego["usuario"] = xml.text().toString();
			else if(currentTag == "icono")
				DatosJuego["icono"] = xml.text().toString();
			else if(currentTag == "titulo")
				DatosJuego["titulo"] += xml.text().toString();
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
			else if(currentTag == "tipo_emu")
				DatosJuego["tipo_emu"] = xml.text().toString();
			else if(currentTag == "comentario")
				DatosJuego["comentario"] = xml.text().toString();

			DatosJuego["favorito"] = "false";
			DatosJuego["rating"]   = "0";
		}
	}
	if(xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError)
	{
		qWarning() << "XML ERROR:" << xml.lineNumber() << ": " << xml.errorString();
	}
}

void frmImportarJuegoInfo::AddGamesTitlesLIDA(QString ResultsPage)
{
	QString currentTag, str_id, str_titulo, str_publicado;

	ui.twListaBusqueda->clear();
	xml.clear();
	xml.addData( ResultsPage );
	while( !xml.atEnd() )
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
			if( xml.name() == "juego" )
			{
				QTreeWidgetItem *item = new QTreeWidgetItem(ui.twListaBusqueda);

				//item->setIcon(0, QIcon(stTheme+"img24/emu_datos.png") );
				item->setText(0, str_titulo );
				item->setText(1, "DOS"      );
				item->setText(2, str_publicado );
				item->setText(3, "http://www.laisladelabandoware.es/grlidadb.php?ver=juego&gid="+str_id+"&id_emu=0&tipo_emu=datos");

				str_id.clear();
				str_titulo.clear();
				str_publicado.clear();
			}
		}
		else if( xml.isCharacters() && !xml.isWhitespace() )
		{
			if(currentTag == "titulo")
				str_titulo += xml.text().toString();
			else if(currentTag == "pubDate")
				str_publicado += xml.text().toString();
		}
	}
	adjustarHeader();
	if(xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError)
	{
		qWarning() << "XML ERROR:" << xml.lineNumber() << ": " << xml.errorString();
		//http->abort();
	}
}
