/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2014 Pedro A. Garcia Rosado Aka Monthy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
**/

#include "grlida_importar_juego.h"

// INICIO ImportarTwPrototype -----------------------------------------------------------------------------------
ImportarTwPrototype::ImportarTwPrototype(QString dir_theme, QObject *parent)
	: QObject(parent)
{
	stTheme = dir_theme;
}

void ImportarTwPrototype::addItemFind(const QString &titulo, const QString &plataforma, const QString &anno, const QString &url, QString icono)
{
	if( icono == "datos" ) icono = "tag";

	QTreeWidget *widget = qscriptvalue_cast<QTreeWidget*>(thisObject());
	QTreeWidgetItem *item = new QTreeWidgetItem(widget);
	item->setIcon(0, QIcon(stTheme +"img16/"+ icono +".png") );
	item->setTextAlignment(2, Qt::AlignCenter);
	item->setText(0, titulo     );
	item->setText(1, plataforma );
	item->setText(2, anno       );
	item->setText(3, url        );
}

void ImportarTwPrototype::addItemDatosFiles(const QString &nombre, const QString &crc, const QString &descripcion, const QString &size, const QString &path, QString icono)
{
	QTreeWidget *widget = qscriptvalue_cast<QTreeWidget*>(thisObject());
	QTreeWidgetItem *item = new QTreeWidgetItem(widget);
	item->setIcon(0, QIcon(stTheme +"img16/"+ icono +".png") );
	item->setTextAlignment(1, Qt::AlignCenter);
	item->setTextAlignment(3, Qt::AlignCenter);
	item->setText(0, nombre      );
	item->setText(1, crc         );
	item->setText(2, descripcion );
	item->setText(3, size        );
	item->setText(4, path        );
}

void ImportarTwPrototype::addItemMounts(const QString &path, const QString &label, const QString &tipo_as, const QString &letter, const QString &indx_cd, const QString &opt_mount, const QString &io_ctrl, const QString &select_mount, const QString &opt_size, const QString &opt_freesize, const QString &freesize)
{
	QTreeWidget *widget = qscriptvalue_cast<QTreeWidget*>(thisObject());
	QTreeWidgetItem *item = new QTreeWidgetItem(widget);

	item->setTextAlignment(1, Qt::AlignCenter);
	item->setTextAlignment(2, Qt::AlignCenter);
	item->setIcon( 0, QIcon(fGrl.getIconMount(stTheme, tipo_as, (select_mount == "v")? "s_" : "")) );
	item->setText( 0, fGrl.setDirRelative(path, "DosGames") );
	item->setText( 1, letter       );
	item->setText( 2, tipo_as      );
	item->setText( 3, label        );
	item->setText( 4, indx_cd      );
	item->setText( 5, opt_mount    );
	item->setText( 6, io_ctrl      );
	item->setText( 7, select_mount );
	item->setText( 8, ""           );
	item->setText( 9, ""           );
	item->setText(10, opt_size     );
	item->setText(11, opt_freesize );
	item->setText(12, freesize     );
}
// FIN ImportarTwPrototype --------------------------------------------------------------------------------------

frmImportarJuego::frmImportarJuego(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, stDatosJuego datos, stConfigDOSBox datos_dbx, stConfigScummVM datos_svm, stConfigVDMSound datos_vdms, bool m_editando, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmImportarJuego)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql       = m_sql;
	grlCfg    = m_cfg;
	categoria = m_categoria;
	Editando  = m_editando;

	DatosJuego    = datos;
	DatosDosBox   = datos_dbx;
	DatosScummVM  = datos_svm;
	DatosVDMSound = datos_vdms;

	grlDir.Home     = fGrl->GRlidaHomePath();
	grlDir.Confdbx  = grlDir.Home +"confdbx/"+ categoria.tabla +"/";
	grlDir.Confvdms = grlDir.Home +"confvdms/"+ categoria.tabla +"/";
	grlDir.Datos    = grlDir.Home +"datos/";
	grlDir.Scripts  = grlDir.Home +"scripts/";
	grlDir.Smiles   = grlDir.Home +"smiles/";
	grlDir.Temp     = grlDir.Home +"temp/";

	cargarConfig();
	createConnections();
	setTheme();

// centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmImportarJuego::~frmImportarJuego()
{
	delete httpdown;
	delete twProto;
	delete engine;
	delete fGrl;
	delete ui;
}

void frmImportarJuego::createConnections()
{
	connect(httpdown, SIGNAL( statusFinished()       ), this         , SLOT( statusFinished()       ) );
	connect(httpdown, SIGNAL( statusLabel(QString)   ), ui->lb_status, SLOT( setText(QString)       ) );
	connect(httpdown, SIGNAL( statusBtnEnabled(bool) ), this         , SLOT( statusBtnEnabled(bool) ) );
}

void frmImportarJuego::cargarConfig()
{
	fGrl->setIdioma(grlCfg.IdiomaSelect);

	ImportPathNew = new frmImportPath(grlCfg, categoria, this);
	ImportPathNew->ui->tabDatConf->setTabEnabled(tabDatos   , true );
	ImportPathNew->ui->tabDatConf->setTabEnabled(tabDOSBox  , false);
	ImportPathNew->ui->tabDatConf->setTabEnabled(tabScummVM , false);
	ImportPathNew->ui->tabDatConf->setTabEnabled(tabVDMSound, false);

	ImportPathNew->ui->twDatosJuego->header()->setStretchLastSection(true);
	ImportPathNew->ui->twDatosJuego->header()->setMovable(false);
	ImportPathNew->ui->twDatosJuego->header()->setResizeMode(0, QHeaderView::Fixed);
	ImportPathNew->ui->twDatosJuego->header()->setResizeMode(1, QHeaderView::ResizeToContents);
	ImportPathNew->ui->twDatosJuego->setColumnWidth(0, 20 );
	ImportPathNew->ui->twDatosJuego->setColumnWidth(1, 120);
	ImportPathNew->ui->twDatosJuego->setColumnWidth(2, 160);

	twProto = new ImportarTwPrototype(fGrl->Theme(), this);
	engine  = new QScriptEngine(this);

	engine->setDefaultPrototype(qMetaTypeId<QTreeWidget*>(), engine->newQObject(twProto));
	engine->globalObject().setProperty("twListaBusqueda", engine->newQObject(ui->twListaBusqueda));
	engine->globalObject().setProperty("twDatosFiles"   , engine->newQObject(ui->twDatosFiles));
	engine->globalObject().setProperty("twDatosURL"     , engine->newQObject(ui->twDatosURL));
	engine->globalObject().setProperty("twMounts"       , engine->newQObject(ui->twMontajes));

	httpdown = new HttpDownload(ImportPathNew);
	httpdown->setHidden(true);

	if( grlCfg.ProxyEnable )
		httpdown->setHttpProxy(grlCfg.ProxyType, grlCfg.ProxyHost, grlCfg.ProxyPort.toInt(), grlCfg.ProxyUserName, grlCfg.ProxyPassword);

	emu_list.clear();
	smiles_list.clear();
	idiomas_list.clear();
	edades_list.clear();
	emu_list     = fGrl->cargaDatosQHash(grlDir.Datos +"emu_list.txt", 4, "|");
	smiles_list  = fGrl->cargaDatosQHash(grlDir.Datos +"smiles.txt"  , 2, "\",\"");
	idiomas_list = fGrl->cargaDatosQHash(grlDir.Datos + fGrl->Idioma() +"/idiomas.txt", 3, "|");
	edades_list  = fGrl->cargaDatosQHash(grlDir.Datos + fGrl->Idioma() +"/edad_recomendada.txt", 3, "|");

// Configuración del twListaBusqueda
	ui->twListaBusqueda->header()->setMovable(false);
	ui->twListaBusqueda->header()->setStretchLastSection(false);
	ui->twListaBusqueda->header()->setResizeMode(0, QHeaderView::Stretch);
	ui->twListaBusqueda->header()->setResizeMode(1, QHeaderView::Interactive);
	ui->twListaBusqueda->header()->setResizeMode(2, QHeaderView::Interactive);
	ui->twListaBusqueda->setColumnWidth(1, 60);
	ui->twListaBusqueda->setColumnWidth(2, 35);

// Configuración del twDatosFiles
	ui->twDatosFiles->header()->setStretchLastSection(true);
	ui->twDatosFiles->header()->setMovable(false);
	ui->twDatosFiles->header()->setResizeMode(0, QHeaderView::Interactive);
	ui->twDatosFiles->header()->setResizeMode(1, QHeaderView::Fixed      );
	ui->twDatosFiles->header()->setResizeMode(2, QHeaderView::Interactive);
	ui->twDatosFiles->header()->setResizeMode(3, QHeaderView::Interactive);
	ui->twDatosFiles->header()->setResizeMode(4, QHeaderView::Interactive);
	ui->twDatosFiles->setColumnWidth(0, 200 );
	ui->twDatosFiles->setColumnWidth(1, 60  );
	ui->twDatosFiles->setColumnWidth(2, 100 );
	ui->twDatosFiles->setColumnWidth(3, 50  );
	ui->twDatosFiles->setColumnWidth(4, 50  );

// Configuración del twDatosURL
	ui->twDatosURL->header()->setStretchLastSection(true);
	ui->twDatosURL->header()->setMovable(false);
	ui->twDatosURL->header()->setResizeMode(0, QHeaderView::Interactive);
	ui->twDatosURL->header()->setResizeMode(1, QHeaderView::Interactive);
	ui->twDatosURL->setColumnWidth(0, 250 );

// Configuración del twMontajes
	ui->twMontajes->header()->setMovable(false);
	ui->twMontajes->header()->setStretchLastSection(false);
	ui->twMontajes->header()->setResizeMode(0, QHeaderView::Stretch);
	ui->twMontajes->setColumnWidth(1, 40);
	ui->twMontajes->setColumnWidth(2, 80);
	ui->twMontajes->setColumnWidth(3, 80);

	ui->txtTituloBuscar->setText( DatosJuego.titulo );
	ui->txtTituloBuscar->setFocus();

	codecFileHtml.clear();
	codecFileHtml.append("UTF-8");

	fGrl->cargarScriptsComboBox(ui->cbxScriptURL, grlDir.Scripts, grlCfg.url_xmldb);
	emit on_cbxScriptURL_activated( ui->cbxScriptURL->currentIndex() );
}

void frmImportarJuego::setTheme()
{
	setWindowIcon( QIcon(fGrl->Theme() +"img16/importar.png") );

	ui->btnAbrir->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnAbrirUrl->setIcon( QIcon(fGrl->Theme() +"img16/edit_enlace.png") );
	ui->btnBuscar->setIcon( QIcon(fGrl->Theme() +"img16/zoom.png") );
	ui->btnAbortar->setIcon( QIcon(fGrl->Theme() +"img16/cancelar.png") );
	ui->btnVerInfo->setIcon( QIcon(fGrl->Theme() +"img16/datos_2.png") );
	ui->btnOk->setIcon( QIcon(fGrl->Theme() +"img16/aplicar.png") );
	ui->btnCancelar->setIcon( QIcon(fGrl->Theme() +"img16/cancelar.png") );

	ui->tabWidget->setTabIcon(0, QIcon(fGrl->Theme() +"img16/datos_3.png") );
	ui->tabWidget->setTabIcon(1, QIcon(fGrl->Theme() +"img16/archivos.png") );
	ui->tabWidget->setTabIcon(2, QIcon(fGrl->Theme() +"img16/edit_enlace.png") );
	ui->tabWidget->setTabIcon(3, QIcon(fGrl->Theme() +"img16/dosbox.png") );

	str_html_old = "";
	if( QFile::exists(fGrl->ThemeApp() +"tpl_juego_info.html") )
		str_html_old = fGrl->leerArchivo(fGrl->ThemeApp() +"tpl_juego_info.html", "UTF-8");
	else
		str_html_old = fGrl->leerArchivo(":/tpl_juego_info.html", "UTF-8");
// Reempla las Etiquetas
	str_html_old.replace("{lb_cover_front}"     , tr("Carátula frontal")  );
	str_html_old.replace("{lb_detalles_juego}"  , tr("Detalles del juego"));
	str_html_old.replace("{lb_calificacion}"    , tr("Calificación")      );
	str_html_old.replace("{lb_otros_datos}"     , tr("Otros datos")       );
	str_html_old.replace("{lb_usuario}"         , tr("Subido por")        );
	str_html_old.replace("{lb_subtitulo}"       , tr("Subtitulo")         );
	str_html_old.replace("{lb_compania}"        , tr("Publicado por")     );
	str_html_old.replace("{lb_desarrollador}"   , tr("Desarrollado por")  );
	str_html_old.replace("{lb_anno}"            , tr("Publicado")         );
	str_html_old.replace("{lb_edad_recomendada}", tr("Edad recomendada")  );
	str_html_old.replace("{lb_idioma}"          , tr("Idioma")            );
	str_html_old.replace("{lb_idioma_voces}"    , tr("Idioma voces")      );
	str_html_old.replace("{lb_formato}"         , tr("Formato")           );
	str_html_old.replace("{lb_genero}"          , tr("Genero")            );
	str_html_old.replace("{lb_tema}"            , tr("Tema")              );
	str_html_old.replace("{lb_grupo}"           , tr("Grupo")             );
	str_html_old.replace("{lb_perspectiva}"     , tr("Perspectiva")       );
	str_html_old.replace("{lb_sistemaop}"       , tr("Sistema operativo") );
	str_html_old.replace("{lb_favorito}"        , tr("Favorito")          );
	str_html_old.replace("{lb_fecha}"           , tr("Añadido el")        );
	str_html_old.replace("{lb_graficos}"        , tr("Gráficos")          );
	str_html_old.replace("{lb_sonido}"          , tr("Sonido")            );
	str_html_old.replace("{lb_jugabilidad}"     , tr("Jugabilidad")       );
	str_html_old.replace("{lb_original}"        , tr("Original")          );
	str_html_old.replace("{lb_estado}"          , tr("Estado")            );
	str_html_old.replace("{lb_tipo_emu}"        , tr("Tipo Emu")          );
	str_html_old.replace("{lb_comentario}"      , tr("Descripción")       );
	str_html_old.replace("{lb_rating}"          , tr("Rating")            );
	str_html_old.replace("{lb_numdisc}"         , tr("Nº discos")         );
}

void frmImportarJuego::cargarScript(QString fileScript)
{
// Cargamos el script
	engine->evaluate( fGrl->leerArchivo(fileScript) );

	codecFileHtml.clear();
	if( engine->evaluate("url_charset").isValid() )
		codecFileHtml.append( engine->evaluate("url_charset").toString() );
	else
		codecFileHtml.append("UTF-8");

	if( engine->evaluate("url_site").isValid() )
		str_url_web = engine->evaluate("url_site").toString();
	else
		str_url_web = "";
}

void frmImportarJuego::mostrarFichaHtml(QHash<QString, QString> datos)
{
	QString dat_idioma, dat_idioma_voces, dat_img_edad, dat_icono, dat_thumbs, dat_rating;
	QString str_html_new = str_html_old;

	if( !datos["Dat_thumbs"].isEmpty() && QFile::exists(grlDir.Temp +"thumbs_"+ datos["Dat_thumbs"]) )
		dat_thumbs = grlDir.Temp +"thumbs_"+ datos["Dat_thumbs"];
	else {
		datos["Dat_thumbs"] = "juego_sin_imagen.png";
		dat_thumbs = fGrl->ThemeApp() +"images/"+ datos["Dat_thumbs"];
	}

	if( QFile::exists( fGrl->ThemeApp() +"img24/cat/"+ datos["Dat_icono"] ) )
		dat_icono = fGrl->ThemeApp() +"img24/cat/"+ datos["Dat_icono"];
	else
		dat_icono = ":/img24/cat/sinimg.png";

	ui->txtHtmlInfo->clear();
	ui->txtHtmlInfo->document()->addResource(QTextDocument::ImageResource, QUrl("thumb_rs_"+ datos["Dat_thumbs"]), QImage(dat_thumbs));
	ui->txtHtmlInfo->document()->addResource(QTextDocument::ImageResource, QUrl("icono_rs_"+ datos["Dat_icono"]), QImage(dat_icono));
	ui->txtHtmlInfo->document()->addResource(QTextDocument::ImageResource, QUrl("img_rs_star_on.png"), QImage(fGrl->Theme() +"images/star_on.png"));
	ui->txtHtmlInfo->document()->addResource(QTextDocument::ImageResource, QUrl("img_rs_star_off.png"), QImage(fGrl->Theme() +"images/star_off.png"));
	ui->txtHtmlInfo->document()->addResource(QTextDocument::ImageResource, QUrl("edad_rs_"+ edades_list[datos["Dat_edad_recomendada"]].icono), QImage(fGrl->Theme() +"img16/"+ edades_list[datos["Dat_edad_recomendada"]].icono));

	foreach (const stGrlDatos &smile, smiles_list)
		ui->txtHtmlInfo->document()->addResource(QTextDocument::ImageResource, QUrl("smile_rs_"+ smile.icono), QImage(grlDir.Smiles + smile.icono));

	foreach (const stGrlDatos &idioma, idiomas_list)
		ui->txtHtmlInfo->document()->addResource(QTextDocument::ImageResource, QUrl("idioma_rs_"+ idioma.icono), QImage(fGrl->Theme() +"img16/lng/"+ idioma.icono));

	dat_idioma       = fGrl->getImgDatos(idiomas_list, datos["Dat_idioma"].split(";")      , true);
	dat_idioma_voces = fGrl->getImgDatos(idiomas_list, datos["Dat_idioma_voces"].split(";"), true);
	dat_img_edad     = "<img src=\":edad_rs_"+ edades_list[datos["Dat_edad_recomendada"]].icono +"\" alt=\""+ edades_list[datos["Dat_edad_recomendada"]].titulo +"\" title=\""+ edades_list[datos["Dat_edad_recomendada"]].titulo +"\"> ";

	int total_rating = datos["Dat_rating"].toInt();
	dat_rating = "";
	for (int i=0; i < 5; ++i)
	{
		if( i < total_rating )
			dat_rating.append("<img src=\":img_rs_star_on.png\" alt=\""+ fGrl->IntToStr(i+1) +"\" title=\""+ fGrl->IntToStr(i+1) +"\"> ");
		else
			dat_rating.append("<img src=\":img_rs_star_off.png\"> ");
	}

// Reempla la info del juego.
	str_html_new.replace("{info_icono}"               , ":icono_rs_"+ datos["Dat_icono"]);
	str_html_new.replace("{info_titulo}"              , Qt::escape(datos["Dat_titulo"])    );
	str_html_new.replace("{info_subtitulo}"           , Qt::escape(datos["Dat_subtitulo"]) );
	str_html_new.replace("{info_genero}"              , datos["Dat_genero"]        );
	str_html_new.replace("{info_compania}"            , datos["Dat_compania"]      );
	str_html_new.replace("{info_desarrollador}"       , datos["Dat_desarrollador"] );
	str_html_new.replace("{info_tema}"                , datos["Dat_tema"]          );
	str_html_new.replace("{info_grupo}"               , datos["Dat_grupo"]         );
	str_html_new.replace("{info_perspectiva}"         , datos["Dat_perspectiva"]   );
	str_html_new.replace("{info_idioma}"              , dat_idioma          );
	str_html_new.replace("{info_idioma_voces}"        , dat_idioma_voces    );
	str_html_new.replace("{info_formato}"             , datos["Dat_formato"]       );
	str_html_new.replace("{info_anno}"                , datos["Dat_anno"]          );
	str_html_new.replace("{info_numdisc}"             , datos["Dat_numdisc"]       );
	str_html_new.replace("{info_img_edad_recomendada}", dat_img_edad        );
	str_html_new.replace("{info_edad_recomendada}"    , edades_list[datos["Dat_edad_recomendada"]].titulo );
	str_html_new.replace("{info_sistemaop}"           , datos["Dat_sistemaop"]     );
	str_html_new.replace("{info_tamano}"              , datos["Dat_tamano"]        );
	str_html_new.replace("{info_graficos}"            , datos["Dat_graficos"]      );
	str_html_new.replace("{info_sonido}"              , datos["Dat_sonido"]        );
	str_html_new.replace("{info_jugabilidad}"         , datos["Dat_jugabilidad"]   );
	str_html_new.replace("{info_original}"            , datos["Dat_original"]      );
	str_html_new.replace("{info_estado}"              , datos["Dat_estado"]        );
	str_html_new.replace("{info_thumbs}"              , ":thumb_rs_"+ datos["Dat_thumbs"]);
	str_html_new.replace("{info_fecha}"               , datos["Dat_fecha"]         );
	str_html_new.replace("{info_tipo_emu}"            , datos["Dat_tipo_emu"]      );
	str_html_new.replace("{info_favorito}"            , datos["Dat_favorito"] == "true" ? tr("Si") : tr("No") );
	str_html_new.replace("{info_rating}"              , dat_rating          );
	str_html_new.replace("{info_usuario}"             , datos["Dat_usuario"]       );
	str_html_new.replace("{info_comentario}", fGrl->reemplazaTextoSmiles(datos["Dat_comentario"], smiles_list) );
	str_html_new.replace("<dir_scripts>", grlDir.Scripts);

	ui->txtHtmlInfo->setHtml( str_html_new );

	for (int i = 1; i < 10; ++i)
	{
		if( !datos["Dat_www_"+ fGrl->IntToStr(i)].isEmpty() )
		{
			QTreeWidgetItem *item_url = new QTreeWidgetItem(ui->twDatosURL);
			item_url->setIcon(0, QIcon(fGrl->Theme() +"img16/html.png") );
			item_url->setText(0, datos["Dat_www_"+ fGrl->IntToStr(i)] );
			item_url->setText(1, datos["Dat_www_"+ fGrl->IntToStr(i) +"_name"] );
			item_url->setText(2, "");
			item_url->setText(3, "");
			item_url->setText(4, "");
		}
	}

	QStringList list_ExtraFiles = datos["Dbx_ExtraFiles"].split("|", QString::SkipEmptyParts);
	const int count_extrafiles = list_ExtraFiles.size();
	if( count_extrafiles > -1 )
	{
		for (int i = 0; i < count_extrafiles; ++i)
		{
			stFileInfo info = fGrl->getInfoFile(list_ExtraFiles.at(i));

			QTreeWidgetItem *item_files = new QTreeWidgetItem(ui->twDatosFiles);
			item_files->setTextAlignment(1, Qt::AlignCenter);
			item_files->setTextAlignment(3, Qt::AlignCenter);
			item_files->setText( 0, info.Name     );
			item_files->setText( 1, info.Crc32    );
			item_files->setText( 2, ""            );
			item_files->setText( 3, info.Size     );
			item_files->setText( 4, info.FilePath );
			item_files->setText( 5, "archivo"     );
			item_files->setText( 6, ""            );
			item_files->setText( 7, ""            );
			item_files->setText( 8, ""            );
		}
	}
// DOSBox --------------------
	if( datos["Dat_tipo_emu"] == "dosbox" )
	{
		if( datos["tipoDFend"] == "true" )
		{
			const int num_mounts = datos["Dbx_NrOfMounts"].toInt();
			if( num_mounts > -1 )
			{
				QStringList list_mount;
				QString dir_base_path, tipo_as, io_ctrl, opt_mount, opt_size, opt_freesize, freesize;

				for (int i = 0; i < num_mounts ; ++i)
				{
					QTreeWidgetItem *item = new QTreeWidgetItem(ui->twMontajes);

					list_mount.clear();
					list_mount = datos[fGrl->IntToStr(i)].split("|");

					dir_base_path = list_mount.at(0);
					tipo_as       = list_mount.at(1).toLower();

					if(tipo_as == "drive")
						tipo_as = "drive";
					else if(tipo_as == "cdrom")
						tipo_as = "cdrom";
					else if(tipo_as == "floppy")
						tipo_as = "floppy";
					else if(tipo_as == "floppyimage")
						tipo_as = "IMG_floppy";
					else if(tipo_as == "cdromimage")
						tipo_as = "IMG_iso";
					else if(tipo_as == "image")
						tipo_as = "IMG_hdd";
					else
						tipo_as = "drive";

					if( list_mount.at(3) == "true" )
						io_ctrl = "-ioctl";
					else if( list_mount.at(3) == "dx" )
						io_ctrl = "-ioctl_dx";
					else if( list_mount.at(3) == "dio" )
						io_ctrl = "-ioctl_dio";
					else if( list_mount.at(3) == "mci" )
						io_ctrl = "-ioctl_mci";
					else if( list_mount.at(3) == "NoIOCTL" )
						io_ctrl = "-noioctl";
					else
						io_ctrl = "";

					opt_freesize = "false";
					opt_size     = "";
					freesize     = "250";

					if( !list_mount.at(5).isEmpty() )
					{
						opt_freesize = "true";
						if( list_mount.at(5).contains(",") )
						{
							opt_size     = list_mount.at(5);
							freesize     = "250";
						} else {
							opt_size     = "";
							freesize     = list_mount.at(5);
						}
					}

					item->setTextAlignment(1, Qt::AlignCenter);
					item->setTextAlignment(2, Qt::AlignCenter);
					item->setIcon( 0, QIcon(fGrl->getIconMount(fGrl->Theme(), tipo_as, "")) );
					item->setText( 0, fGrl->setDirRelative(dir_base_path, "DosGames") ); // path
					item->setText( 1, list_mount.at(2)  ); // letter
					item->setText( 2, tipo_as           );
					item->setText( 3, list_mount.at(4)  ); // label
					item->setText( 4, fGrl->IntToStr(i) ); // indx_cd
					item->setText( 5, opt_mount         );
					item->setText( 6, io_ctrl           );
					item->setText( 7, "x"               );
					item->setText( 8, ""                );
					item->setText( 9, ""                );
					item->setText(10, opt_size          );
					item->setText(11, opt_freesize      );
					item->setText(12, freesize          );
				}
			}
		}
	}

	ui->btnOk->setEnabled(true);
}

QString frmImportarJuego::leerArchivoHTML(QString file_html)
{
	QString text_html = fGrl->leerArchivo(file_html, codecFileHtml);

	if( text_html.isEmpty() )
		text_html = "";
	else {
		text_html.replace("&#x27;","'");
		text_html.replace("&#x26;","&");
		text_html.replace("&amp;","&");
	//	text_html.replace("&#xE9;","Ã©");
	//	text_html.replace("&#xE0;","Ã ");
		text_html.replace("&#039;","'");

	//	text_html.replace("&ntilde;","ñ");
	//	text_html.replace("&aacute;","á");
	//	text_html.replace("&eacute;","é");
	//	text_html.replace("&iacute;","í");
	//	text_html.replace("&oacute;","ó");
	//	text_html.replace("&uacute;","ú");
		text_html.replace("&nbsp;"," ");
	}

	return text_html;
}

void frmImportarJuego::downloadFile(QString dat_key, QString dat_url_key, QString tipo)
{
	if( !datosImportar[dat_key].isEmpty() && !datosImportar[dat_url_key].isEmpty() )
	{
		if( datosImportar["isLocal"] == "true" )
		{
			if( !QFile::exists( grlDir.Temp + tipo + datosImportar[dat_key] ) )
				QFile::copy(datosImportar[dat_url_key], grlDir.Temp + tipo + datosImportar[dat_key]);

			emit statusFinished();
		} else {
			if( !QFile::exists( grlDir.Temp + tipo + datosImportar[dat_key] ) )
				httpdown->downloadFile(datosImportar[dat_url_key], grlDir.Temp + tipo + datosImportar[dat_key], "GET");
			else
				emit statusFinished();
		}
	} else {
		datosImportar[dat_key]     = "";
		datosImportar[dat_url_key] = "";

		emit statusFinished();
	}
}

void frmImportarJuego::addTitles(QString filename)
{
	args.clear();
	ctor = engine->evaluate("AnalyzeFindPage");
	args << QScriptValue(engine, leerArchivoHTML(filename));
	ctor.call(QScriptValue(), args);
}

void frmImportarJuego::analyzePage(QString filename, bool local, bool tipoDFend)
{
	datosImportar.clear();
	datosImportar["tipoDFend"] = fGrl->BoolToStr(tipoDFend);
	datosImportar["isLocal"]   = fGrl->BoolToStr(local);
	datosImportar["filename"]  = fGrl->getInfoFile(filename).Path;

	if( local )
	{
		datosImportar["Dat_url_cover_thumbs"] = datosImportar["filename"] + datosImportar["Dat_thumbs"];
		datosImportar["Dat_url_cover_front"]  = datosImportar["filename"] + datosImportar["Dat_cover_front"];
		datosImportar["Dat_url_cover_back"]   = datosImportar["filename"] + datosImportar["Dat_cover_back"];
	}

	if( tipoDFend )
		datosImportar = fGrl->importarProfileDFend(grlDir.Home, categoria.tabla, filename);
	else {
		args.clear();
		ctor = engine->evaluate("AnalyzeGamePage");
		args << QScriptValue(engine, leerArchivoHTML(filename));
		args << QScriptValue(engine, local);

		QScriptValue m_array = ctor.call(QScriptValue(), args);
		QScriptValueIterator it(m_array);

		while( it.hasNext() )
		{
			it.next();
			datosImportar.insert(it.name(), ""+ it.value().toString());
		}
	}

	index_fin_descarga = CargarThumb;
	emit statusFinished();
}

void frmImportarJuego::insertarDatosJuego(QString etiqueta, QString icono, QString old_dat, QString new_dat, QString key_dat)
{
	QTreeWidgetItem *item = new QTreeWidgetItem(ImportPathNew->ui->twDatosJuego);
	item->setCheckState(0, Qt::Checked);
	item->setText( 0, "" );
	item->setIcon( 1, QIcon(fGrl->Theme() +"img16/"+ icono));
	item->setText( 1, etiqueta );
	item->setText( 2, old_dat  );
	item->setText( 3, new_dat  );
	item->setText( 4, key_dat  );
}

void frmImportarJuego::setDatosJuego(QHash<QString, QString> datos)
{
// Datos ---------------------
//	DatosJuego.idgrl            = ""+ datos["Dat_idgrl"];
//	DatosJuego.icono            = ""+ datos["Dat_icono"];
	DatosJuego.titulo           = ""+ datos["Dat_titulo"];
	DatosJuego.subtitulo        = ""+ datos["Dat_subtitulo"];
	DatosJuego.genero           = ""+ datos["Dat_genero"];
	DatosJuego.compania         = ""+ datos["Dat_compania"];
	DatosJuego.desarrollador    = ""+ datos["Dat_desarrollador"];
	DatosJuego.tema             = ""+ datos["Dat_tema"];
	DatosJuego.grupo            = ""+ datos["Dat_grupo"];
	DatosJuego.perspectiva      = ""+ datos["Dat_perspectiva"];
	DatosJuego.idioma           = ""+ datos["Dat_idioma"];
	DatosJuego.idioma_voces     = ""+ datos["Dat_idioma_voces"];
	DatosJuego.formato          = ""+ datos["Dat_formato"];
	DatosJuego.anno             = ""+ datos["Dat_anno"];
	DatosJuego.numdisc          = ""+ datos["Dat_numdisc"];
	DatosJuego.sistemaop        = ""+ datos["Dat_sistemaop"];
	DatosJuego.tamano           = ""+ datos["Dat_tamano"];
	DatosJuego.graficos         = ""+ datos["Dat_graficos"];
	DatosJuego.sonido           = ""+ datos["Dat_sonido"];
	DatosJuego.jugabilidad      = ""+ datos["Dat_jugabilidad"];
	DatosJuego.original         = ""+ datos["Dat_original"];
	DatosJuego.estado           = ""+ datos["Dat_estado"];
	DatosJuego.fecha            = ""+ DatosJuego.fecha.isEmpty() ? fGrl->getTime() : DatosJuego.fecha;
	TipoEmu                     = datos["Dat_tipo_emu"].isEmpty() ? "datos" : datos["Dat_tipo_emu"];
	DatosJuego.comentario       = ""+ datos["Dat_comentario"];
	DatosJuego.favorito         = ""+ datos["Dat_favorito"].isEmpty() ? "false" : datos["Dat_favorito"];
	DatosJuego.rating           = ""+ datos["Dat_rating"];
	DatosJuego.edad_recomendada = ""+ datos["Dat_edad_recomendada"];
	DatosJuego.usuario          = ""+ datos["Dat_usuario"];
	DatosJuego.path_exe         = ""+ datos["Dat_path_exe"];
	DatosJuego.path_setup       = ""+ datos["Dat_path_setup"];
	DatosJuego.parametros_exe   = ""+ datos["Dat_parametros_exe"];
	DatosJuego.parametros_setup = ""+ datos["Dat_parametros_setup"];
	DatosJuego.path_capturas    = ""+ datos["Dat_path_capturas"];

	isSoloDatos = ImportPathNew->ui->chk_soloDatos->isChecked();

	if( DatosJuego.thumbs != datos["Dat_thumbs"] )
	{
		DatosJuego.thumbs = "thumbs_"+ datos["Dat_thumbs"];
		DatosJuego.thumbs_new = true;
	} else
		DatosJuego.thumbs_new = false;

	if( DatosJuego.cover_front != datos["Dat_cover_front"] )
	{
		DatosJuego.cover_front = "cover_front_"+ datos["Dat_cover_front"];
		DatosJuego.cover_front_new = true;
	} else
		DatosJuego.cover_front_new = false;

	if( DatosJuego.cover_back != datos["Dat_cover_back"] )
	{
		DatosJuego.cover_back = "cover_back_"+ datos["Dat_cover_back"];
		DatosJuego.cover_back_new = true;
	} else
		DatosJuego.cover_back_new = false;

// DOSBox --------------------
	if( TipoEmu == "dosbox" && !isSoloDatos )
	{
		datos["Dbx_path_conf"]       = ImportPathNew->ui->txtDbx_path_conf->text();
		datos["Dbx_path_exe"]        = ImportPathNew->ui->txtDbx_path_exe->text();
		datos["Dbx_path_setup"]      = ImportPathNew->ui->txtDbx_path_setup->text();
		datos["Dbx_dosbox_language"] = ImportPathNew->ui->txtDbx_dosbox_language->text();
		datos["Dbx_sdl_mapperfile"]  = ImportPathNew->ui->txtDbx_sdl_mapperfile->text();
		datos["Dbx_gus_ultradir"]    = ImportPathNew->ui->txtDbx_gus_ultradir->text();
		datos["Dbx_dosbox_captures"] = ImportPathNew->ui->txtDbx_dosbox_captures->text();
		datos["Dbx_path_sonido"]     = ImportPathNew->ui->txtDbx_path_sonido->text();

	// DOSBox version a usar
		if( datos["Dbx_dosbox_emu_key"].isEmpty() )
			datos["Dbx_dosbox_emu_key"] = grlCfg.DOSBoxDefault.isEmpty() ? "dosbox" : grlCfg.DOSBoxDefault;

	// [sdl]
		DatosDosBox.sdl_fullscreen        = ""+ datos["Dbx_sdl_fullscreen"];
		DatosDosBox.sdl_fulldouble        = ""+ datos["Dbx_sdl_fulldouble"];
		DatosDosBox.sdl_fullfixed         = ""+ datos["Dbx_sdl_fullfixed"];
		DatosDosBox.sdl_fullresolution    = ""+ datos["Dbx_sdl_fullresolution"];
		DatosDosBox.sdl_windowresolution  = ""+ datos["Dbx_sdl_windowresolution"];
		DatosDosBox.sdl_output            = ""+ datos["Dbx_sdl_output"];
		DatosDosBox.sdl_hwscale           = ""+ datos["Dbx_sdl_hwscale"];
		DatosDosBox.sdl_autolock          = ""+ datos["Dbx_sdl_autolock"];
		DatosDosBox.sdl_sensitivity       = ""+ datos["Dbx_sdl_sensitivity"];
		DatosDosBox.sdl_waitonerror       = ""+ datos["Dbx_sdl_waitonerror"];
		DatosDosBox.sdl_priority          = ""+ datos["Dbx_sdl_priority"];
		DatosDosBox.sdl_mapperfile        = ""+ datos["Dbx_sdl_mapperfile"];
		DatosDosBox.sdl_usescancodes      = ""+ datos["Dbx_sdl_usescancodes"];
	// [dosbox]
		DatosDosBox.dosbox_language       = ""+ datos["Dbx_dosbox_language"];
		DatosDosBox.dosbox_machine        = ""+ datos["Dbx_dosbox_machine"];
		DatosDosBox.dosbox_captures       = ""+ datos["Dbx_dosbox_captures"];
		DatosDosBox.dosbox_memsize        = ""+ datos["Dbx_dosbox_memsize"];
		DatosDosBox.dosbox_emu_key        = ""+ datos["Dbx_dosbox_emu_key"];
	// [render]
		DatosDosBox.render_frameskip      = ""+ datos["Dbx_render_frameskip"];
		DatosDosBox.render_aspect         = ""+ datos["Dbx_render_aspect"];
		DatosDosBox.render_scaler         = ""+ datos["Dbx_render_scaler"];
	// [cpu]
		DatosDosBox.cpu_core              = ""+ datos["Dbx_cpu_core"];
		DatosDosBox.cpu_cputype           = ""+ datos["Dbx_cpu_cputype"];
		DatosDosBox.cpu_cycles            = ""+ datos["Dbx_cpu_cycles"];
		DatosDosBox.cpu_cycles_realmode   = ""+ datos["Dbx_cpu_cycles_realmode"];
		DatosDosBox.cpu_cycles_protmode   = ""+ datos["Dbx_cpu_cycles_protmode"];
		DatosDosBox.cpu_cycles_limitmode  = ""+ datos["Dbx_cpu_cycles"];
		DatosDosBox.cpu_cycleup           = ""+ datos["Dbx_cpu_cycleup"];
		DatosDosBox.cpu_cycledown         = ""+ datos["Dbx_cpu_cycledown"];
	// [mixer]
		DatosDosBox.mixer_nosound         = ""+ datos["Dbx_mixer_nosound"];
		DatosDosBox.mixer_rate            = ""+ datos["Dbx_mixer_rate"];
		DatosDosBox.mixer_blocksize       = ""+ datos["Dbx_mixer_blocksize"];
		DatosDosBox.mixer_prebuffer       = ""+ datos["Dbx_mixer_prebuffer"];
	// [midi]
		DatosDosBox.midi_mpu401           = ""+ datos["Dbx_midi_mpu401"];
		DatosDosBox.midi_intelligent      = ""+ datos["Dbx_midi_intelligent"];
		DatosDosBox.midi_device           = ""+ datos["Dbx_midi_device"];
		DatosDosBox.midi_config           = ""+ datos["Dbx_midi_config"];
		DatosDosBox.midi_mt32rate         = ""+ datos["Dbx_midi_mt32rate"];
	// [sblaster]
		DatosDosBox.sblaster_sbtype       = ""+ datos["Dbx_sblaster_sbtype"];
		DatosDosBox.sblaster_sbbase       = ""+ datos["Dbx_sblaster_sbbase"];
		DatosDosBox.sblaster_irq          = ""+ datos["Dbx_sblaster_irq"];
		DatosDosBox.sblaster_dma          = ""+ datos["Dbx_sblaster_dma"];
		DatosDosBox.sblaster_hdma         = ""+ datos["Dbx_sblaster_hdma"];
		DatosDosBox.sblaster_mixer        = ""+ datos["Dbx_sblaster_mixer"];
		DatosDosBox.sblaster_oplmode      = ""+ datos["Dbx_sblaster_oplmode"];
		DatosDosBox.sblaster_oplemu       = ""+ datos["Dbx_sblaster_oplemu"];
		DatosDosBox.sblaster_oplrate      = ""+ datos["Dbx_sblaster_oplrate"];
	// [gus]
		DatosDosBox.gus_gus               = ""+ datos["Dbx_gus_gus"];
		DatosDosBox.gus_gusrate           = ""+ datos["Dbx_gus_gusrate"];
		DatosDosBox.gus_gusbase           = ""+ datos["Dbx_gus_gusbase"];
		DatosDosBox.gus_irq1              = ""+ datos["Dbx_gus_irq1"];
		DatosDosBox.gus_irq2              = ""+ datos["Dbx_gus_irq2"];
		DatosDosBox.gus_dma1              = ""+ datos["Dbx_gus_dma1"];
		DatosDosBox.gus_dma2              = ""+ datos["Dbx_gus_dma2"];
		DatosDosBox.gus_ultradir          = ""+ datos["Dbx_gus_ultradir"];
	// [speaker]
		DatosDosBox.speaker_pcspeaker     = ""+ datos["Dbx_speaker_pcspeaker"];
		DatosDosBox.speaker_pcrate        = ""+ datos["Dbx_speaker_pcrate"];
		DatosDosBox.speaker_tandy         = ""+ datos["Dbx_speaker_tandy"];
		DatosDosBox.speaker_tandyrate     = ""+ datos["Dbx_speaker_tandyrate"];
		DatosDosBox.speaker_disney        = ""+ datos["Dbx_speaker_disney"];
	// [joystick]
		DatosDosBox.joystick_type         = ""+ datos["Dbx_joystick_type"];
		DatosDosBox.joystick_timed        = ""+ datos["Dbx_joystick_timed"];
		DatosDosBox.joystick_autofire     = ""+ datos["Dbx_joystick_autofire"];
		DatosDosBox.joystick_swap34       = ""+ datos["Dbx_joystick_swap34"];
		DatosDosBox.joystick_buttonwrap   = ""+ datos["Dbx_joystick_buttonwrap"];
	// [modem]
		DatosDosBox.modem_modem           = ""+ datos["Dbx_modem_modem"];
		DatosDosBox.modem_comport         = ""+ datos["Dbx_modem_comport"];
		DatosDosBox.modem_listenport      = ""+ datos["Dbx_modem_listenport"];
	// [dserial]
		DatosDosBox.dserial_directserial  = ""+ datos["Dbx_dserial_directserial"];
		DatosDosBox.dserial_comport       = ""+ datos["Dbx_dserial_comport"];
		DatosDosBox.dserial_realport      = ""+ datos["Dbx_dserial_realport"];
		DatosDosBox.dserial_defaultbps    = ""+ datos["Dbx_dserial_defaultbps"];
		DatosDosBox.dserial_parity        = ""+ datos["Dbx_dserial_parity"];
		DatosDosBox.dserial_bytesize      = ""+ datos["Dbx_dserial_bytesize"];
		DatosDosBox.dserial_stopbit       = ""+ datos["Dbx_dserial_stopbit"];
	// [serial]
		DatosDosBox.serial_1              = ""+ datos["Dbx_serial_1"];
		DatosDosBox.serial_2              = ""+ datos["Dbx_serial_2"];
		DatosDosBox.serial_3              = ""+ datos["Dbx_serial_3"];
		DatosDosBox.serial_4              = ""+ datos["Dbx_serial_4"];
	// [dos]
		DatosDosBox.dos_xms               = ""+ datos["Dbx_dos_xms"];
		DatosDosBox.dos_ems               = ""+ datos["Dbx_dos_ems"];
		DatosDosBox.dos_umb               = ""+ datos["Dbx_dos_umb"];
		DatosDosBox.dos_keyboardlayout    = ""+ datos["Dbx_dos_keyboardlayout"];
		DatosDosBox.dos_version           = ""+ datos["Dbx_dos_version"];
	// [ipx]
		DatosDosBox.ipx_ipx               = ""+ datos["Dbx_ipx_ipx"];
		DatosDosBox.ipx_type              = ""+ datos["Dbx_ipx_type"];
		DatosDosBox.ipx_port              = ""+ datos["Dbx_ipx_port"];
		DatosDosBox.ipx_ip                = ""+ datos["Dbx_ipx_ip"];
	// [autoexec]
		DatosDosBox.autoexec              = ""+ datos["Dbx_autoexec"];
	// Opciones
		DatosDosBox.opt_autoexec          = ""+ datos["Dbx_opt_autoexec"];
		DatosDosBox.opt_loadfix           = ""+ datos["Dbx_opt_loadfix"];
		DatosDosBox.opt_loadfix_mem       = ""+ datos["Dbx_opt_loadfix_mem"];
		DatosDosBox.opt_consola_dbox      = ""+ datos["Dbx_opt_consola_dbox"];
		DatosDosBox.opt_cerrar_dbox       = ""+ datos["Dbx_opt_cerrar_dbox"];
		DatosDosBox.opt_cycle_sincronizar = ""+ datos["Dbx_opt_cycle_sincronizar"];
	// Path
		DatosDosBox.path_conf             = ""+ datos["Dbx_path_conf"];
		DatosDosBox.path_sonido           = ""+ datos["Dbx_path_sonido"];
		DatosDosBox.path_exe              = ""+ datos["Dbx_path_exe"];
		DatosDosBox.path_setup            = ""+ datos["Dbx_path_setup"];
		DatosDosBox.parametros_exe        = ""+ datos["Dbx_parametros_exe"];
		DatosDosBox.parametros_setup      = ""+ datos["Dbx_parametros_setup"];
	}

// ScummVM -------------------
	if( TipoEmu == "scummvm" && !isSoloDatos )
	{
		datos["Svm_path_game"]     = ImportPathNew->ui->txtSvm_path_game->text();
		datos["Svm_path_save"]     = ImportPathNew->ui->txtSvm_path_save->text();
		datos["Svm_path_extra"]    = ImportPathNew->ui->txtSvm_path_extra->text();
		datos["Svm_path_capturas"] = ImportPathNew->ui->txtSvm_path_capturas->text();
		datos["Svm_path_sonido"]   = ImportPathNew->ui->txtSvm_path_sonido->text();

		DatosScummVM.game            = ""+ datos["Svm_game"];
		DatosScummVM.game_label      = ""+ datos["Svm_game_label"];
		DatosScummVM.language        = ""+ datos["Svm_language"];
		DatosScummVM.subtitles       = ""+ datos["Svm_subtitles"];
		DatosScummVM.platform        = ""+ datos["Svm_platform"];
		DatosScummVM.gfx_mode        = ""+ datos["Svm_gfx_mode"];
		DatosScummVM.render_mode     = ""+ datos["Svm_render_mode"];
		DatosScummVM.fullscreen      = ""+ datos["Svm_fullscreen"];
		DatosScummVM.aspect_ratio    = ""+ datos["Svm_aspect_ratio"];
		DatosScummVM.path_game       = ""+ datos["Svm_path_game"];
		DatosScummVM.path_extra      = ""+ datos["Svm_path_extra"];
		DatosScummVM.path_save       = ""+ datos["Svm_path_save"];
		DatosScummVM.path_capturas   = ""+ datos["Svm_path_capturas"];
		DatosScummVM.path_sonido     = ""+ datos["Svm_path_sonido"];
		DatosScummVM.music_driver    = ""+ datos["Svm_music_driver"];
		DatosScummVM.enable_gs       = ""+ datos["Svm_enable_gs"];
		DatosScummVM.multi_midi      = ""+ datos["Svm_multi_midi"];
		DatosScummVM.native_mt32     = ""+ datos["Svm_native_mt32"];
		DatosScummVM.master_volume   = ""+ datos["Svm_master_volume"];
		DatosScummVM.music_volume    = ""+ datos["Svm_music_volume"];
		DatosScummVM.sfx_volume      = ""+ datos["Svm_sfx_volume"];
		DatosScummVM.speech_volume   = ""+ datos["Svm_speech_volume"];
		DatosScummVM.tempo           = ""+ datos["Svm_tempo"];
		DatosScummVM.talkspeed       = ""+ datos["Svm_talkspeed"];
		DatosScummVM.debuglevel      = ""+ datos["Svm_debuglevel"];
		DatosScummVM.cdrom           = ""+ datos["Svm_cdrom"];
		DatosScummVM.joystick_num    = ""+ datos["Svm_joystick_num"];
		DatosScummVM.output_rate     = ""+ datos["Svm_output_rate"];
		DatosScummVM.midi_gain       = ""+ datos["Svm_midi_gain"];
		DatosScummVM.copy_protection = ""+ datos["Svm_copy_protection"];
		DatosScummVM.sound_font      = ""+ datos["Svm_sound_font"];
		DatosScummVM.walkspeed       = ""+ datos["Svm_walkspeed"];
		DatosScummVM.opl_driver      = ""+ datos["Svm_opl_driver"];
	}

// VDMSound ------------------
	if( TipoEmu == "vdmsound" && !isSoloDatos )
	{
		datos["Vdms_path_conf"] = ImportPathNew->ui->txtVdms_path_conf->text();
		datos["Vdms_path_exe"]  = ImportPathNew->ui->txtVdms_path_exe->text();

		DatosVDMSound.path_conf       = ""+ datos["Vdms_path_conf"];
		DatosVDMSound.path_exe        = ""+ datos["Vdms_path_exe"];
		DatosVDMSound.vdms_debug_1    = ""+ datos["Vdms_vdms_debug_1"];
		DatosVDMSound.vdms_debug_2    = ""+ datos["Vdms_vdms_debug_2"];
		DatosVDMSound.winnt_dos_1     = ""+ datos["Vdms_winnt_dos_1"];
		DatosVDMSound.winnt_dos_2     = ""+ datos["Vdms_winnt_dos_2"];
		DatosVDMSound.winnt_dosbox_1  = ""+ datos["Vdms_winnt_dosbox_1"];
		DatosVDMSound.winnt_dosbox_2  = ""+ datos["Vdms_winnt_dosbox_2"];
		DatosVDMSound.winnt_dosbox_3  = ""+ datos["Vdms_winnt_dosbox_3"];
		DatosVDMSound.winnt_storage_1 = ""+ datos["Vdms_winnt_storage_1"];
		DatosVDMSound.winnt_storage_2 = ""+ datos["Vdms_winnt_storage_2"];
	}
}

stDatosJuego frmImportarJuego::getDatosJuegos(bool getTipoEmu)
{
	if( getTipoEmu )
		DatosJuego.tipo_emu = TipoEmu;
	return DatosJuego;
}

void frmImportarJuego::on_btnOk_clicked()
{
	ImportPathNew->ui->twDatosJuego->clear();
//	insertarDatosJuego(tr("Icono")             , "datos_3.png", DatosJuego.icono           , datosImportar["Dat_icono"]           , "Dat_icono"           );
	insertarDatosJuego(tr("Título")            , "datos_3.png", DatosJuego.titulo          , datosImportar["Dat_titulo"]          , "Dat_titulo"          );
	insertarDatosJuego(tr("Subtitulo")         , "datos_3.png", DatosJuego.subtitulo       , datosImportar["Dat_subtitulo"]       , "Dat_subtitulo"       );
	insertarDatosJuego(tr("Genero")            , "datos_3.png", DatosJuego.genero          , datosImportar["Dat_genero"]          , "Dat_genero"          );
	insertarDatosJuego(tr("Publicado por")     , "datos_3.png", DatosJuego.compania        , datosImportar["Dat_compania"]        , "Dat_compania"        );
	insertarDatosJuego(tr("Desarrollado por")  , "datos_3.png", DatosJuego.desarrollador   , datosImportar["Dat_desarrollador"]   , "Dat_desarrollador"   );
	insertarDatosJuego(tr("Tema")              , "datos_3.png", DatosJuego.tema            , datosImportar["Dat_tema"]            , "Dat_tema"            );
	insertarDatosJuego(tr("Grupo")             , "datos_3.png", DatosJuego.grupo           , datosImportar["Dat_grupo"]           , "Dat_grupo"           );
	insertarDatosJuego(tr("Perspectiva")       , "datos_3.png", DatosJuego.perspectiva     , datosImportar["Dat_perspectiva"]     , "Dat_perspectiva"     );
	insertarDatosJuego(tr("Idioma")            , "datos_3.png", DatosJuego.idioma          , datosImportar["Dat_idioma"]          , "Dat_idioma"          );
	insertarDatosJuego(tr("Idioma voces")      , "datos_3.png", DatosJuego.idioma_voces    , datosImportar["Dat_idioma_voces"]    , "Dat_idioma_voces"    );
	insertarDatosJuego(tr("Formato")           , "datos_3.png", DatosJuego.formato         , datosImportar["Dat_formato"]         , "Dat_formato"         );
	insertarDatosJuego(tr("Publicado")         , "datos_3.png", DatosJuego.anno            , datosImportar["Dat_anno"]            , "Dat_anno"            );
	insertarDatosJuego(tr("Nº discos")         , "datos_3.png", DatosJuego.numdisc         , datosImportar["Dat_numdisc"]         , "Dat_numdisc"         );
	insertarDatosJuego(tr("Sistema")           , "datos_3.png", DatosJuego.sistemaop       , datosImportar["Dat_sistemaop"]       , "Dat_sistemaop"       );
	insertarDatosJuego(tr("Tamaño")            , "datos_3.png", DatosJuego.tamano          , datosImportar["Dat_tamano"]          , "Dat_tamano"          );
	insertarDatosJuego(tr("Gráficos")          , "datos_3.png", DatosJuego.graficos        , datosImportar["Dat_graficos"]        , "Dat_graficos"        );
	insertarDatosJuego(tr("Sonido")            , "datos_3.png", DatosJuego.sonido          , datosImportar["Dat_sonido"]          , "Dat_sonido"          );
	insertarDatosJuego(tr("Jugabilidad")       , "datos_3.png", DatosJuego.jugabilidad     , datosImportar["Dat_jugabilidad"]     , "Dat_jugabilidad"     );
	insertarDatosJuego(tr("Original")          , "datos_3.png", DatosJuego.original        , datosImportar["Dat_original"]        , "Dat_original"        );
	insertarDatosJuego(tr("Estado")            , "datos_3.png", DatosJuego.estado          , datosImportar["Dat_estado"]          , "Dat_estado"          );
	insertarDatosJuego(tr("Portada")           , "datos_3.png", DatosJuego.thumbs          , datosImportar["Dat_thumbs"]          , "Dat_thumbs"          );
	insertarDatosJuego(tr("Carátula delantera"), "datos_3.png", DatosJuego.cover_front     , datosImportar["Dat_cover_front"]     , "Dat_cover_front"     );
	insertarDatosJuego(tr("Carátula trasera")  , "datos_3.png", DatosJuego.cover_back      , datosImportar["Dat_cover_back"]      , "Dat_cover_back"      );
//	insertarDatosJuego(tr("Añadido el")        , "datos_3.png", DatosJuego.fecha           , datosImportar["Dat_fecha"]           , "Dat_fecha"]          );
//	insertarDatosJuego(tr("Tipo Emu")          , "datos_3.png", DatosJuego.tipo_emu        , datosImportar["Dat_tipo_emu"]        , "Dat_tipo_emu"        );
	insertarDatosJuego(tr("Descripción")       , "datos_3.png", DatosJuego.comentario      , datosImportar["Dat_comentario"]      , "Dat_comentario"      );
	insertarDatosJuego(tr("Favorito")          , "datos_3.png", DatosJuego.favorito        , datosImportar["Dat_favorito"]        , "Dat_favorito"        );
	insertarDatosJuego(tr("Rating")            , "datos_3.png", DatosJuego.rating          , datosImportar["Dat_rating"]          , "Dat_rating"          );
	insertarDatosJuego(tr("Edad recomendada")  , "datos_3.png", DatosJuego.edad_recomendada, datosImportar["Dat_edad_recomendada"], "Dat_edad_recomendada");
	insertarDatosJuego(tr("Subido por")        , "datos_3.png", DatosJuego.usuario         , datosImportar["Dat_usuario"]         , "Dat_usuario"         );
	insertarDatosJuego(tr("Ejecutable")        , "datos_3.png", DatosJuego.path_exe        , datosImportar["Dat_path_exe"]        , "Dat_path_exe"        );
	insertarDatosJuego(tr("Parametros exe")    , "datos_3.png", DatosJuego.parametros_exe  , datosImportar["Dat_parametros_exe"]  , "Dat_parametros_exe"  );
	insertarDatosJuego(tr("Capturas")          , "datos_3.png", DatosJuego.path_capturas   , datosImportar["Dat_path_capturas"]   , "Dat_path_capturas"   );
	insertarDatosJuego(tr("Ejecutable setup")  , "datos_3.png", DatosJuego.path_setup      , datosImportar["Dat_path_setup"]      , "Dat_path_setup"      );
	insertarDatosJuego(tr("Parametros setup")  , "datos_3.png", DatosJuego.parametros_setup, datosImportar["Dat_parametros_setup"], "Dat_parametros_setup");

	index_fin_descarga = CargarCoverFront;
	emit statusFinished();

	if( datosImportar["Dat_tipo_emu"] == "datos" )
	{
		ImportPathNew->ui->chk_soloDatos->setEnabled(false);
		ImportPathNew->ui->chk_soloDatos->setChecked(true);
	} else {
		ImportPathNew->ui->chk_soloDatos->setEnabled(true);
		ImportPathNew->ui->chk_soloDatos->setChecked(false);
	}

	if( datosImportar["Dat_tipo_emu"] == "dosbox" )
	{
		ImportPathNew->ui->tabDatConf->setTabEnabled(tabDatos   , true );
		ImportPathNew->ui->tabDatConf->setTabEnabled(tabDOSBox  , true );
		ImportPathNew->ui->tabDatConf->setTabEnabled(tabScummVM , false);
		ImportPathNew->ui->tabDatConf->setTabEnabled(tabVDMSound, false);

		stFileInfo f_info = fGrl->getInfoFile(grlDir.Confdbx + datosImportar["Dbx_path_conf"]);
		if( f_info.Exists && !Editando )
			ImportPathNew->ui->txtDbx_path_conf->setText(f_info.Name +"_"+ fGrl->HoraFechaActual(fGrl->getTime(), "ddMMyyyy_HHmmss") +".conf");
		else
			ImportPathNew->ui->txtDbx_path_conf->setText(f_info.Name +".conf");

		ImportPathNew->ui->txtDbx_path_exe->setText( fGrl->setDirRelative(datosImportar["Dbx_path_exe"], "DosGames" ) );
		ImportPathNew->ui->txtDbx_path_setup->setText( fGrl->setDirRelative(datosImportar["Dbx_path_setup"], "DosGames") );
		ImportPathNew->ui->txtDbx_dosbox_language->setText( datosImportar["Dbx_dosbox_language"] );
		ImportPathNew->ui->txtDbx_sdl_mapperfile->setText( datosImportar["Dbx_sdl_mapperfile"] );
		ImportPathNew->ui->txtDbx_gus_ultradir->setText( datosImportar["Dbx_gus_ultradir"] );
		ImportPathNew->ui->txtDbx_dosbox_captures->setText( datosImportar["Dbx_dosbox_captures"] );
		ImportPathNew->ui->txtDbx_path_sonido->setText( datosImportar["Dbx_path_sonido"] );
	}

	if( datosImportar["Dat_tipo_emu"] == "scummvm" )
	{
		ImportPathNew->ui->tabDatConf->setTabEnabled(tabDatos   , true );
		ImportPathNew->ui->tabDatConf->setTabEnabled(tabDOSBox  , false);
		ImportPathNew->ui->tabDatConf->setTabEnabled(tabScummVM , true );
		ImportPathNew->ui->tabDatConf->setTabEnabled(tabVDMSound, false);

		ImportPathNew->ui->txtSvm_path_game->setText( fGrl->setDirRelative(datosImportar["Svm_path_game"], "DosGames") );
		ImportPathNew->ui->txtSvm_path_save->setText( fGrl->setDirRelative(datosImportar["Svm_path_save"], "DosGames") );
		ImportPathNew->ui->txtSvm_path_extra->setText( fGrl->setDirRelative(datosImportar["Svm_path_extra"], "DosGames") );
		ImportPathNew->ui->txtSvm_path_capturas->setText( datosImportar["Svm_path_capturas"] );
		ImportPathNew->ui->txtSvm_path_sonido->setText( datosImportar["Svm_path_sonido"] );
	}

	if( datosImportar["Dat_tipo_emu"] == "vdmsound" )
	{
		ImportPathNew->ui->tabDatConf->setTabEnabled(tabDatos   , true );
		ImportPathNew->ui->tabDatConf->setTabEnabled(tabDOSBox  , false);
		ImportPathNew->ui->tabDatConf->setTabEnabled(tabScummVM , false);
		ImportPathNew->ui->tabDatConf->setTabEnabled(tabVDMSound, true );

		stFileInfo f_info = fGrl->getInfoFile(grlDir.Confvdms + datosImportar["Vdms_path_conf"]);
		if( f_info.Exists )
			ImportPathNew->ui->txtVdms_path_conf->setText(f_info.Name +"_"+ fGrl->HoraFechaActual(fGrl->getTime(), "ddMMyyyy_HHmmss") +".vlp");
		else
			ImportPathNew->ui->txtVdms_path_conf->setText(f_info.Name +".vlp");

		ImportPathNew->ui->txtVdms_path_exe->setText( fGrl->setDirRelative(datosImportar["Vdms_path_exe"], "DosGames") );
	}

	if( ImportPathNew->exec() == QDialog::Accepted )
	{
		grlCfg = ImportPathNew->getGrlCfg();
		grlCfg.url_xmldb = ui->cbxScriptURL->itemData( ui->cbxScriptURL->currentIndex() ).toString();

		const int count_num = ImportPathNew->ui->twDatosJuego->topLevelItemCount();
		for (int num = 0; num < count_num; ++num)
		{
			QTreeWidgetItem *item = ImportPathNew->ui->twDatosJuego->topLevelItem( num );
			QString col_name = item->text(4);
			if( item->checkState(0) == Qt::Checked )
				datosImportar[col_name] = item->text(3);
			else
				datosImportar[col_name] = item->text(2);
		}

		setDatosJuego(datosImportar);

		QDialog::accept();
	} else
		grlCfg.url_xmldb = ui->cbxScriptURL->itemData( ui->cbxScriptURL->currentIndex() ).toString();
}

void frmImportarJuego::on_btnCancelar_clicked()
{
	QDialog::reject();
}

void frmImportarJuego::on_btnAbrir_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo para importarlo"), grlCfg.DirImportar, "", tr("Soportados") +" (*.xml *.prof);;DB GR-lida (*.xml);;D-Fend Reloaded (*.prof);;"+ tr("Todos los archivo") +" (*)");

	if( !archivo.isEmpty() )
	{
		stFileInfo f_info = fGrl->getInfoFile( archivo );
		if( f_info.Exists )
		{
			bool siguiente = false;
			grlCfg.DirImportar = fGrl->setDirRelative(f_info.Path);

			if( f_info.Ext == ".xml" )
			{
				cargarScript(grlDir.Scripts +"gr-lida.js");
				analyzePage(archivo, true);
				siguiente = true;
			}
			else if( f_info.Ext == ".prof" )
			{
				analyzePage(archivo, true, true); // Indicamos que es tipo DFend
				siguiente = true;
			} else
				siguiente = false;

			ui->btnOk->setEnabled( siguiente );

			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "DirImportar", grlCfg.DirImportar);
		}
	}
}

void frmImportarJuego::on_btnAbrirUrl_clicked()
{
// Abre la URL con el navegador por defecto
	if( !str_url_web.isEmpty() )
		QDesktopServices::openUrl( QUrl(str_url_web) );
}

void frmImportarJuego::on_btnBuscar_clicked()
{
	if( !ui->txtTituloBuscar->text().isEmpty() && ui->cbxScriptURL->count() > -1 )
	{
		QHash<QString, QString> url_filed;

		ui->btnVerInfo->setEnabled(false);
		ui->btnOk->setEnabled(false);

		ui->twListaBusqueda->clear();
		ui->txtHtmlInfo->clear();
		ui->twDatosFiles->clear();
		ui->twDatosURL->clear();
		ui->twMontajes->clear();

		args.clear();
		ctor = engine->evaluate("UrlWebBusqueda");
		args << QScriptValue(engine, ui->txtTituloBuscar->text() );

		QScriptValue m_array = ctor.call(QScriptValue(), args);
		QScriptValueIterator it(m_array);

		url_filed.clear();
		while( it.hasNext() )
		{
			it.next();
			url_filed.insert(it.name(), it.value().toString());
		}

		index_fin_descarga = AnalizarPaginaBusqueda;
		httpdown->downloadFile(url_filed["url"], grlDir.Temp +"busqueda.html", url_filed["metodo"], url_filed["c_post"]);
	}
}

void frmImportarJuego::on_btnAbortar_clicked()
{
	emit httpdown->cancelDownload();
}

void frmImportarJuego::on_btnVerInfo_clicked()
{
	int pos = ui->twListaBusqueda->indexOfTopLevelItem( ui->twListaBusqueda->currentItem() );
	if( ui->twListaBusqueda->topLevelItemCount() > 0 && pos != -1 )
	{
		ui->btnVerInfo->setEnabled(false);
		ui->btnOk->setEnabled(false);

		ui->txtHtmlInfo->clear();
		ui->twDatosFiles->clear();
		ui->twDatosURL->clear();
		ui->twMontajes->clear();

		index_fin_descarga = AnalizarPaginaFicha;
		httpdown->downloadFile(ui->twListaBusqueda->currentItem()->text(3), grlDir.Temp +"ficha.html");
	}
}

void frmImportarJuego::on_cbxScriptURL_activated(int index)
{
	if( index > -1 )
	{
		grlCfg.url_xmldb = ui->cbxScriptURL->itemData( index ).toString();
		if( grlCfg.url_xmldb == "_desde_archivo_" )
		{
			ui->btnAbrir->setEnabled(true);
			ui->txtTituloBuscar->setEnabled(false);
			ui->btnBuscar->setEnabled(false);
			ui->twListaBusqueda->setVisible(false);
		} else {
			ui->btnAbrir->setEnabled(false);
			ui->txtTituloBuscar->setEnabled(true);
			ui->btnBuscar->setEnabled(true);
			ui->twListaBusqueda->setVisible(true);

			cargarScript(grlDir.Scripts + grlCfg.url_xmldb);
		}
	}
}

void frmImportarJuego::on_twListaBusqueda_itemClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
		ui->btnVerInfo->setEnabled(true);
}

void frmImportarJuego::on_twListaBusqueda_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
	{
		ui->btnVerInfo->setEnabled(true);
		ui->btnVerInfo->click();
	}
}

void frmImportarJuego::on_twListaBusqueda_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if( current != previous )
		emit on_twListaBusqueda_itemClicked(current, 0);
}

void frmImportarJuego::on_txtTituloBuscar_returnPressed()
{
	emit on_btnBuscar_clicked();
}

void frmImportarJuego::statusBtnEnabled(bool estado)
{
	ui->cbxScriptURL->setEnabled(estado);
	ui->txtTituloBuscar->setEnabled(estado);
	ui->btnBuscar->setEnabled(estado);
	ui->twListaBusqueda->setEnabled(estado);
	ui->btnAbortar->setEnabled(!estado);

	emit ImportPathNew->statusBtnEnabled(estado);
}

void frmImportarJuego::statusFinished()
{
	switch( index_fin_descarga )
	{
		case NohacerNada:
			//
		break;
		case MostrarFicha:
			mostrarFichaHtml( datosImportar );
		break;
		case AnalizarPaginaBusqueda:
			addTitles(grlDir.Temp +"busqueda.html");
		break;
		case AnalizarPaginaFicha:
			analyzePage(grlDir.Temp +"ficha.html");
		break;
		case CargarThumb:
			index_fin_descarga = MostrarFicha;
			downloadFile("Dat_thumbs", "Dat_url_cover_thumbs", "thumbs_");
		break;
		case CargarCoverFront:
			index_fin_descarga = CargarCoverBack;
			downloadFile("Dat_cover_front", "Dat_url_cover_front", "cover_front_");
		break;
		case CargarCoverBack:
			index_fin_descarga = NohacerNada;
			downloadFile("Dat_cover_back", "Dat_url_cover_back", "cover_back_");
		break;
	}
}
