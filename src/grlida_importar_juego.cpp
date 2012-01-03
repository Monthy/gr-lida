/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2011 Pedro A. Garcia Rosado Aka Monthy
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
**/

#include "grlida_importar_juego.h"

Q_DECLARE_METATYPE(QTreeWidgetItem*);
Q_DECLARE_METATYPE(QTreeWidget*);

ImportarTwPrototype::ImportarTwPrototype(QObject *parent)
    : QObject(parent)
{
	stHomeDir = fGrl.GRlidaHomePath();	// directorio de trabajo del GR-lida
	stTheme   = fGrl.ThemeGrl();

	QSettings settings(stHomeDir+"GR-lida.conf", QSettings::IniFormat);
	settings.beginGroup("OpcGeneral");
		stDirBaseGames = settings.value("DirBaseGames", "").toString();
	settings.endGroup();
}

void ImportarTwPrototype::addItemFind(const QString &titulo, const QString &plataforma, const QString &anno, const QString &url, QString icono)
{
	QTreeWidget *widget = qscriptvalue_cast<QTreeWidget*>(thisObject());
	QTreeWidgetItem *Item = new QTreeWidgetItem(widget);

	if( icono == "datos" ) icono = "tag";

	Item->setIcon(0, QIcon(stTheme+"img16/"+icono+".png") ); // Titulo
	Item->setText(0, titulo     ); // Titulo
	Item->setText(1, plataforma ); //
	Item->setText(2, anno       ); //
	Item->setText(3, url        ); // url ficha
}

void ImportarTwPrototype::addItemDatosFiles(const QString &nombre, const QString &crc, const QString &descripcion, const QString &size, const QString &path, QString icono)
{
	QTreeWidget *widget = qscriptvalue_cast<QTreeWidget*>(thisObject());
	QTreeWidgetItem *Item = new QTreeWidgetItem(widget);
	Item->setIcon(0, QIcon(stTheme+"img16/"+icono+".png") );
	Item->setText(0, nombre      );
	Item->setText(1, crc         );
	Item->setText(2, descripcion );
	Item->setText(3, size        );
	Item->setText(4, path        );
}

void ImportarTwPrototype::addItemMounts(const QString &path, const QString &label, const QString &tipo_as, const QString &letter, const QString &indx_cd, const QString &opt_mount, const QString &io_ctrl, const QString &select_mount)
{
	QString img_select_mount, dir_base_path;

	if( select_mount == "v" )
		img_select_mount = "s_"; else img_select_mount = "";

	QTreeWidget *widget = qscriptvalue_cast<QTreeWidget*>(thisObject());
	QTreeWidgetItem *Item = new QTreeWidgetItem(widget);

	dir_base_path = path;

	Item->setIcon(0, QIcon(fGrl.getIconMount(tipo_as, img_select_mount)) );
	Item->setText(0, dir_base_path.replace("{DirBaseGames}", stDirBaseGames) );
	Item->setText(1, label        );
	Item->setText(2, tipo_as      );
	Item->setText(3, letter       );
	Item->setText(4, indx_cd      );
	Item->setText(5, opt_mount    );
	Item->setText(6, io_ctrl      );
	Item->setText(7, select_mount );
}


// Importar Información del Juego
frmImportarJuego::frmImportarJuego(QHash<QString, QString> datos,
						  QHash<QString, QString> datos_svm,
						  QHash<QString, QString> datos_dbx,
						  QHash<QString, QString> datos_vdms, QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);

	stHomeDir      = fGrl.GRlidaHomePath();
	stTheme        = fGrl.ThemeGrl();
	stConfgDbxDir  = stHomeDir +"confdbx/";
	stConfgVdmSDir = stHomeDir +"confvdms/";
	stIconDir      = stHomeDir +"iconos/";
	stScriptsDir   = stHomeDir +"scripts/";
	stTempDir      = stHomeDir +"temp/";
	
	GRLConfig = fGrl.CargarGRLConfig( stHomeDir + "GR-lida.conf" );

	stFileBusqueda = stTempDir + "busqueda.html";
	stFileFicha    = stTempDir + "ficha.html";
	stCoversDir    = stHomeDir + "covers/";
	stThumbsDir    = stHomeDir + "thumbs/";

	DatosJuego    = datos;
	DatosScummVM  = datos_svm;
	DatosDosBox   = datos_dbx;
	DatosVDMSound = datos_vdms;

	ImportPathNew = new frmImportPath(this);
	httpdown      = new HttpDownload(this);
	httpdown_dos  = new HttpDownload(this);

	createConnections();	// Conecta los distintos botones con las funciones.
	CargarConfig();
	setTheme();

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmImportarJuego::~frmImportarJuego()
{	
	delete httpdown;
	delete httpdown_dos;
	delete ImportPathNew;
}

void frmImportarJuego::createConnections()
{
	connect( ui.cbxScriptURL, SIGNAL( activated(int) ), this, SLOT( on_changeScriptURL(int) ) );
	connect( ui.btnOk       , SIGNAL( clicked() ), this, SLOT( on_btnOk()             ) );
	connect( ui.btnAbrir    , SIGNAL( clicked() ), this, SLOT( on_btnAbrir()          ) );
	connect( ui.btnBuscar   , SIGNAL( clicked() ), this, SLOT( on_btnBuscar()         ) );
	connect( ui.btnVerInfo  , SIGNAL( clicked() ), this, SLOT( on_btnVerInfo()        ) );
	connect( ui.btnAbrirUrl , SIGNAL( clicked() ), this, SLOT( on_btnAbrirUrl()       ) );

	connect( ui.twListaBusqueda, SIGNAL( currentItemChanged( QTreeWidgetItem *, QTreeWidgetItem *) ), SLOT( on_twListaBusqueda_currentItemChanged( QTreeWidgetItem *, QTreeWidgetItem *) ) );
	connect( ui.twListaBusqueda, SIGNAL( itemClicked( QTreeWidgetItem*, int)), this, SLOT( on_twListaBusqueda_clicked(QTreeWidgetItem*)));
	connect( ui.twListaBusqueda, SIGNAL( itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( on_twListaBusqueda_Dblclicked(QTreeWidgetItem*) ));

	connect(httpdown, SIGNAL( StatusRequestFinished()        ), this, SLOT( isRequestFinished() ) );
	connect(httpdown, SIGNAL( statusLabelChanged(QString)    ), ui.lb_status, SLOT( setText(QString) ) );
	connect(httpdown, SIGNAL( StatusBtnDownloadChanged(bool) ), this, SLOT( on_changeEnabled(bool) ) );

	connect(httpdown_dos, SIGNAL( StatusRequestFinished()        ), this, SLOT( isRequestFinished() ) );
	connect(httpdown_dos, SIGNAL( statusLabelChanged(QString)    ), ui.lb_status, SLOT( setText(QString) ) );
	connect(httpdown_dos, SIGNAL( StatusBtnDownloadChanged(bool) ), this, SLOT( on_changeEnabled(bool) ) );
}

void frmImportarJuego::setTheme()
{
	setUpdatesEnabled( false );
	setWindowIcon( QIcon(stTheme+"img16/importar.png") );

	ui.twListaBusqueda->headerItem()->setIcon(0, QIcon(stTheme+"img16/bullet_black.png") );
//	ui.twListaBusqueda->headerItem()->setIcon(1, QIcon(stTheme+"img16/controller.png") );
//	ui.twListaBusqueda->headerItem()->setIcon(2, QIcon(stTheme+"img16/fecha.png") );

	ui.btnAbrir->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnAbrirUrl->setIcon( QIcon(stTheme+"img16/edit_enlace.png") );
	ui.btnBuscar->setIcon( QIcon(stTheme+"img16/zoom.png") );
	ui.btnAbortar->setIcon( QIcon(stTheme+"img16/cancelar.png") );
	ui.btnVerInfo->setIcon( QIcon(stTheme+"img16/datos_2.png") );
	ui.btnOk->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.btnCancelar->setIcon( QIcon(stTheme+"img16/cancelar.png") );

	ui.tabWidget->setTabIcon(0, QIcon(stTheme+"img16/datos_3.png") );
	ui.tabWidget->setTabIcon(1, QIcon(stTheme+"img16/archivos.png") );
	ui.tabWidget->setTabIcon(2, QIcon(stTheme+"img16/edit_enlace.png") );
	ui.tabWidget->setTabIcon(3, QIcon(stTheme+"img16/dosbox.png") );

	QFile LeerArchivo(stTheme + "tpl_juego_info.html");
	if( LeerArchivo.open( QIODevice::ReadOnly | QIODevice::Text ) )
	{
		QTextStream in( &LeerArchivo );
		str_html_old.clear();
		str_html_old = in.readAll();
	} else
		str_html_old = "";

	LeerArchivo.close();

// Reempla las Etiquetas
	str_html_old.replace("{lb_cover_front}"   , tr("Carátula Frontal")  , Qt::CaseSensitive);
	str_html_old.replace("{lb_detalles_juego}", tr("Detalles del juego"), Qt::CaseSensitive);
	str_html_old.replace("{lb_calificacion}"  , tr("Calificación")      , Qt::CaseSensitive);
	str_html_old.replace("{lb_otros_datos}"   , tr("Otros Datos")       , Qt::CaseSensitive);
	str_html_old.replace("{lb_usuario}"       , tr("Subido por")        , Qt::CaseSensitive);
	str_html_old.replace("{lb_subtitulo}"     , tr("Subtitulo")         , Qt::CaseSensitive);
	str_html_old.replace("{lb_compania}"      , tr("Publicado por")     , Qt::CaseSensitive);
	str_html_old.replace("{lb_desarrollador}" , tr("Desarrollado por")  , Qt::CaseSensitive);
	str_html_old.replace("{lb_anno}"          , tr("Publicado")         , Qt::CaseSensitive);
	str_html_old.replace("{lb_edad_recomendada}", tr("Edad recomendada"), Qt::CaseSensitive);
	str_html_old.replace("{lb_idioma}"        , tr("Idioma")            , Qt::CaseSensitive);
	str_html_old.replace("{lb_idioma_voces}"  , tr("Idioma voces")      , Qt::CaseSensitive);
	str_html_old.replace("{lb_formato}"       , tr("Formato")           , Qt::CaseSensitive);
	str_html_old.replace("{lb_genero}"        , tr("Genero")            , Qt::CaseSensitive);
	str_html_old.replace("{lb_tema}"          , tr("Tema")              , Qt::CaseSensitive);
	str_html_old.replace("{lb_perspectiva}"   , tr("Perspectiva")       , Qt::CaseSensitive);
	str_html_old.replace("{lb_sistemaop}"     , tr("Sistema Operativo") , Qt::CaseSensitive);
	str_html_old.replace("{lb_favorito}"      , tr("Favorito")          , Qt::CaseSensitive);
	str_html_old.replace("{lb_fecha}"         , tr("Añadido el")        , Qt::CaseSensitive);
	str_html_old.replace("{lb_graficos}"      , tr("Gráficos")          , Qt::CaseSensitive);
	str_html_old.replace("{lb_sonido}"        , tr("Sonido")            , Qt::CaseSensitive);
	str_html_old.replace("{lb_jugabilidad}"   , tr("Jugabilidad")       , Qt::CaseSensitive);
	str_html_old.replace("{lb_original}"      , tr("Original")          , Qt::CaseSensitive);
	str_html_old.replace("{lb_estado}"        , tr("Estado")            , Qt::CaseSensitive);
	str_html_old.replace("{lb_tipo_emu}"      , tr("Tipo Emu")          , Qt::CaseSensitive);
	str_html_old.replace("{lb_comentario}"    , tr("Descripción")       , Qt::CaseSensitive);
	str_html_old.replace("{lb_rating}"        , tr("Rating")            , Qt::CaseSensitive);

	setUpdatesEnabled( true );

	if( GRLConfig["font_usar"].toBool() )
		setStyleSheet(fGrl.StyleSheet()+"*{font-family:\""+GRLConfig["font_family"].toString()+"\";font-size:"+GRLConfig["font_size"].toString()+"pt;}");
	else
		setStyleSheet( fGrl.StyleSheet() );
}

void frmImportarJuego::CargarConfig()
{
	httpdown->setHidden(true);
	httpdown_dos->setHidden(true);

	if( GRLConfig["ProxyEnable"].toBool() )
	{
		httpdown->setHttpProxy( GRLConfig["ProxyType"].toInt(), GRLConfig["ProxyHost"].toString(), GRLConfig["ProxyPort"].toInt(), GRLConfig["ProxyUserName"].toString(), GRLConfig["ProxyPassword"].toString() );
		httpdown_dos->setHttpProxy( GRLConfig["ProxyType"].toInt(), GRLConfig["ProxyHost"].toString(), GRLConfig["ProxyPort"].toInt(), GRLConfig["ProxyUserName"].toString(), GRLConfig["ProxyPassword"].toString() );
	}

	ui.twListaBusqueda->header()->setStretchLastSection(false);
	ui.twListaBusqueda->header()->setMovable(false);
	ui.twListaBusqueda->header()->setResizeMode(0, QHeaderView::Stretch);
	ui.twListaBusqueda->header()->setResizeMode(1, QHeaderView::Interactive);
	ui.twListaBusqueda->header()->setResizeMode(2, QHeaderView::Interactive);
	ui.twListaBusqueda->setColumnWidth(1,60);
	ui.twListaBusqueda->setColumnWidth(2,35);

	ui.txtTituloBuscar->setText( DatosJuego["Dat_titulo"] );
	ui.txtTituloBuscar->setFocus();

	engine.setDefaultPrototype(qMetaTypeId<QTreeWidget*>(), engine.newQObject(&twProto));
	engine.globalObject().setProperty("twListaBusqueda", engine.newQObject(ui.twListaBusqueda));
	engine.globalObject().setProperty("twDatosFiles"   , engine.newQObject(ui.twDatosFiles));
	engine.globalObject().setProperty("twDatosURL"     , engine.newQObject(ui.twDatosURL));
	engine.globalObject().setProperty("twMounts"       , engine.newQObject(ui.twMontajes));

	CodecFileHtml.clear();
	CodecFileHtml.append("UTF-8");

	fGrl.CargarScriptsComboBox(stScriptsDir, ui.cbxScriptURL);

	int rowScript = ui.cbxScriptURL->findData( GRLConfig["url_xmldb"].toString() );
	if( rowScript < 0 ) rowScript = 1;

	ui.cbxScriptURL->setCurrentIndex( rowScript );
	on_changeScriptURL( rowScript );
}

void frmImportarJuego::on_btnOk()
{
	ImportPathNew->ui.twDatosJuego->clear();
//	InsertarItemDatosJuego(tr("Icono")             , DatosJuego["Dat_icono"]           , tmpDatosJuego["Dat_icono"]           , "Dat_icono"           );
	InsertarItemDatosJuego(tr("Título")            , DatosJuego["Dat_titulo"]          , tmpDatosJuego["Dat_titulo"]          , "Dat_titulo"          );
	InsertarItemDatosJuego(tr("Subtitulo")         , DatosJuego["Dat_subtitulo"]       , tmpDatosJuego["Dat_subtitulo"]       , "Dat_subtitulo"       );
	InsertarItemDatosJuego(tr("Genero")            , DatosJuego["Dat_genero"]          , tmpDatosJuego["Dat_genero"]          , "Dat_genero"          );
	InsertarItemDatosJuego(tr("Publicado por")     , DatosJuego["Dat_compania"]        , tmpDatosJuego["Dat_compania"]        , "Dat_compania"        );
	InsertarItemDatosJuego(tr("Desarrollado por")  , DatosJuego["Dat_desarrollador"]   , tmpDatosJuego["Dat_desarrollador"]   , "Dat_desarrollador"   );
	InsertarItemDatosJuego(tr("Tema")              , DatosJuego["Dat_tema"]            , tmpDatosJuego["Dat_tema"]            , "Dat_tema"            );
	InsertarItemDatosJuego(tr("Grupo")             , DatosJuego["Dat_grupo"]           , tmpDatosJuego["Dat_grupo"]           , "Dat_grupo"           );
	InsertarItemDatosJuego(tr("Perspectiva")       , DatosJuego["Dat_perspectiva"]     , tmpDatosJuego["Dat_perspectiva"]     , "Dat_perspectiva"     );
	InsertarItemDatosJuego(tr("Idioma")            , DatosJuego["Dat_idioma"]          , tmpDatosJuego["Dat_idioma"]          , "Dat_idioma"          );
	InsertarItemDatosJuego(tr("Idioma voces")      , DatosJuego["Dat_idioma_voces"]    , tmpDatosJuego["Dat_idioma_voces"]    , "Dat_idioma_voces"    );
	InsertarItemDatosJuego(tr("Formato")           , DatosJuego["Dat_formato"]         , tmpDatosJuego["Dat_formato"]         , "Dat_formato"         );
	InsertarItemDatosJuego(tr("Publicado")         , DatosJuego["Dat_anno"]            , tmpDatosJuego["Dat_anno"]            , "Dat_anno"            );
	InsertarItemDatosJuego(tr("Nº discos")         , DatosJuego["Dat_numdisc"]         , tmpDatosJuego["Dat_numdisc"]         , "Dat_numdisc"         );
	InsertarItemDatosJuego(tr("Sistema")           , DatosJuego["Dat_sistemaop"]       , tmpDatosJuego["Dat_sistemaop"]       , "Dat_sistemaop"       );
	InsertarItemDatosJuego(tr("Tamaño")            , DatosJuego["Dat_tamano"]          , tmpDatosJuego["Dat_tamano"]          , "Dat_tamano"          );
	InsertarItemDatosJuego(tr("Gráficos")          , DatosJuego["Dat_graficos"]        , tmpDatosJuego["Dat_graficos"]        , "Dat_graficos"        );
	InsertarItemDatosJuego(tr("Sonido")            , DatosJuego["Dat_sonido"]          , tmpDatosJuego["Dat_sonido"]          , "Dat_sonido"          );
	InsertarItemDatosJuego(tr("Jugabilidad")       , DatosJuego["Dat_jugabilidad"]     , tmpDatosJuego["Dat_jugabilidad"]     , "Dat_jugabilidad"     );
	InsertarItemDatosJuego(tr("Original")          , DatosJuego["Dat_original"]        , tmpDatosJuego["Dat_original"]        , "Dat_original"        );
	InsertarItemDatosJuego(tr("Estado")            , DatosJuego["Dat_estado"]          , tmpDatosJuego["Dat_estado"]          , "Dat_estado"          );
	InsertarItemDatosJuego(tr("Portada")           , DatosJuego["Dat_thumbs"]          , tmpDatosJuego["Dat_thumbs"]          , "Dat_thumbs"          );
	InsertarItemDatosJuego(tr("Carátula delantera"), DatosJuego["Dat_cover_front"]     , tmpDatosJuego["Dat_cover_front"]     , "Dat_cover_front"     );
	InsertarItemDatosJuego(tr("Carátula trasera")  , DatosJuego["Dat_cover_back"]      , tmpDatosJuego["Dat_cover_back"]      , "Dat_cover_back"      );
//	InsertarItemDatosJuego(tr("Añadido el")        , DatosJuego["Dat_fecha"]           , tmpDatosJuego["Dat_fecha"]           , "Dat_fecha"]          );
//	InsertarItemDatosJuego(tr("Tipo Emu")          , DatosJuego["Dat_tipo_emu"]        , tmpDatosJuego["Dat_tipo_emu"]        , "Dat_tipo_emu"        );
	InsertarItemDatosJuego(tr("Descripción")       , DatosJuego["Dat_comentario"]      , tmpDatosJuego["Dat_comentario"]      , "Dat_comentario"      );
	InsertarItemDatosJuego(tr("Favorito")          , DatosJuego["Dat_favorito"]        , tmpDatosJuego["Dat_favorito"]        , "Dat_favorito"        );
	InsertarItemDatosJuego(tr("Rating")            , DatosJuego["Dat_rating"]          , tmpDatosJuego["Dat_rating"]          , "Dat_rating"          );
	InsertarItemDatosJuego(tr("Edad recomendada")  , DatosJuego["Dat_edad_recomendada"], tmpDatosJuego["Dat_edad_recomendada"], "Dat_edad_recomendada");
	InsertarItemDatosJuego(tr("Subido por")        , DatosJuego["Dat_usuario"]         , tmpDatosJuego["Dat_usuario"]         , "Dat_usuario"         );
	InsertarItemDatosJuego(tr("Ejecutable")        , DatosJuego["Dat_path_exe"]        , tmpDatosJuego["Dat_path_exe"]        , "Dat_path_exe"        );
	InsertarItemDatosJuego(tr("Parametros Exe")    , DatosJuego["Dat_parametros_exe"]  , tmpDatosJuego["Dat_parametros_exe"]  , "Dat_parametros_exe"  );

	ImportPathNew->ui.tabDatConf->setTabEnabled(tab_datos   , true );
	ImportPathNew->ui.tabDatConf->setTabEnabled(tab_dosbox  , false);
	ImportPathNew->ui.tabDatConf->setTabEnabled(tab_scummvm , false);
	ImportPathNew->ui.tabDatConf->setTabEnabled(tab_vdmsound, false);
//---------------------------------------------------------------------------------------
//       CargarCoverFront
//---------------------------------------------------------------------------------------
	if( tmpDatosJuego["Dat_cover_front"] != "" || tmpDatosJuego["Dat_cover_front"] != "null" )
	{
		indx_fin_descarga = NohacerNada;
		if( !img_cover_front.isEmpty() && !img_url_cover_front.isEmpty() )
		{
			if( !file_cover_front.exists( stTempDir+img_cover_front ) )
				httpdown->downloadFile(img_url_cover_front, stTempDir + img_cover_front);
			else
				isRequestFinished();
		} else {
			img_cover_front = "";
			img_url_cover_front = "";
			tmpDatosJuego["Dat_cover_front"] = "";

			isRequestFinished();
		}
	} else
		tmpDatosJuego["Dat_cover_front"] = "";
//---------------------------------------------------------------------------------------
//       FIN CargarCoverFront
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//       CargarCoverBack
//---------------------------------------------------------------------------------------
	if( tmpDatosJuego["Dat_cover_back"] != "" || tmpDatosJuego["Dat_cover_back"] != "null" )
	{
		indx_fin_descarga = NohacerNada;
		if( !img_cover_back.isEmpty() && !img_url_cover_back.isEmpty() )
		{
			if( !file_cover_back.exists( stTempDir+img_cover_back ) )
				httpdown_dos->downloadFile(img_url_cover_back, stTempDir + img_cover_back);
			else
				isRequestFinished();
		} else {
			img_cover_back = "";
			img_url_cover_back = "";
			tmpDatosJuego["Dat_cover_back"] = "";

			isRequestFinished();
		}
	} else
		tmpDatosJuego["Dat_cover_back"] = "";
//---------------------------------------------------------------------------------------
//       FIN CargarCoverBack
//---------------------------------------------------------------------------------------

	if( tmpDatosJuego["Dat_tipo_emu"] == "dosbox" )
	{
		ImportPathNew->ui.tabDatConf->setTabEnabled(tab_datos   , true );
		ImportPathNew->ui.tabDatConf->setTabEnabled(tab_dosbox  , true );
		ImportPathNew->ui.tabDatConf->setTabEnabled(tab_scummvm , false);
		ImportPathNew->ui.tabDatConf->setTabEnabled(tab_vdmsound, false);

		if( QFile::exists(stConfgDbxDir + tmpDatosDosBox["Dbx_path_conf"] ) )
			ImportPathNew->ui.txtDbx_path_conf->setText( tmpDatosDosBox["Dbx_path_conf"].prepend(fGrl.HoraFechaActual("ddMMyyyy_HHmmss")+"-") );
		else
			ImportPathNew->ui.txtDbx_path_conf->setText( tmpDatosDosBox["Dbx_path_conf"] );

		ImportPathNew->ui.txtDbx_path_exe->setText( tmpDatosDosBox["Dbx_path_exe"].replace("{DirBaseGames}", GRLConfig["DirBaseGames"].toString() ) );
		ImportPathNew->ui.txtDbx_path_setup->setText( tmpDatosDosBox["Dbx_path_setup"].replace("{DirBaseGames}", GRLConfig["DirBaseGames"].toString() ) );
		ImportPathNew->ui.txtDbx_dosbox_language->setText( tmpDatosDosBox["Dbx_dosbox_language"] );
		ImportPathNew->ui.txtDbx_sdl_mapperfile->setText( tmpDatosDosBox["Dbx_sdl_mapperfile"] );
		ImportPathNew->ui.txtDbx_gus_ultradir->setText( tmpDatosDosBox["Dbx_gus_ultradir"] );
		ImportPathNew->ui.txtDbx_dosbox_captures->setText( tmpDatosDosBox["Dbx_dosbox_captures"] );
		ImportPathNew->ui.txtDbx_path_sonido->setText( tmpDatosDosBox["Dbx_path_sonido"] );
	}

	if( tmpDatosJuego["Dat_tipo_emu"] == "scummvm" )
	{
		ImportPathNew->ui.tabDatConf->setTabEnabled(tab_datos   , true );
		ImportPathNew->ui.tabDatConf->setTabEnabled(tab_dosbox  , false);
		ImportPathNew->ui.tabDatConf->setTabEnabled(tab_scummvm , true );
		ImportPathNew->ui.tabDatConf->setTabEnabled(tab_vdmsound, false);

		ImportPathNew->ui.txtSvm_path->setText( tmpDatosScummVM["Svm_path"].replace("{DirBaseGames}", GRLConfig["DirBaseGames"].toString() ) );
		ImportPathNew->ui.txtSvm_path_save->setText( tmpDatosScummVM["Svm_path_save"].replace("{DirBaseGames}", GRLConfig["DirBaseGames"].toString() ) );
		ImportPathNew->ui.txtSvm_path_setup->setText( tmpDatosScummVM["Svm_path_setup"].replace("{DirBaseGames}", GRLConfig["DirBaseGames"].toString() )  );
		ImportPathNew->ui.txtSvm_path_extra->setText( tmpDatosScummVM["Svm_path_extra"].replace("{DirBaseGames}", GRLConfig["DirBaseGames"].toString() )  );
		ImportPathNew->ui.txtSvm_path_capturas->setText( tmpDatosScummVM["Svm_path_capturas"] );
		ImportPathNew->ui.txtSvm_path_sonido->setText( tmpDatosScummVM["Svm_path_sonido"]   );
	}

	if( tmpDatosJuego["Dat_tipo_emu"] == "vdmsound" )
	{
		ImportPathNew->ui.tabDatConf->setTabEnabled(tab_datos   , true );
		ImportPathNew->ui.tabDatConf->setTabEnabled(tab_dosbox  , false);
		ImportPathNew->ui.tabDatConf->setTabEnabled(tab_scummvm , false);
		ImportPathNew->ui.tabDatConf->setTabEnabled(tab_vdmsound, true );

		if( QFile::exists(stConfgVdmSDir + DatosVDMSound["Vdms_path_conf"] ) )
			ImportPathNew->ui.txtVdms_path_conf->setText( DatosVDMSound["Vdms_path_conf"].prepend(fGrl.HoraFechaActual("ddMMyyyy_HHmmss")+"-") );
		else
			ImportPathNew->ui.txtVdms_path_conf->setText( DatosVDMSound["Vdms_path_conf"] );

		ImportPathNew->ui.txtVdms_path_exe->setText( DatosVDMSound["Vdms_path_exe"].replace("{DirBaseGames}", GRLConfig["DirBaseGames"].toString() ) );
	}

	if( ImportPathNew->exec() == QDialog::Accepted )
	{
		DatosJuego["Dat_thumbs_old"]      = DatosJuego["Dat_thumbs"];
		DatosJuego["Dat_cover_front_old"] = DatosJuego["Dat_cover_front"];
		DatosJuego["Dat_cover_back_old"]  = DatosJuego["Dat_cover_back"];

		for(int num = 0; num < ImportPathNew->ui.twDatosJuego->topLevelItemCount(); ++num )
		{
			QTreeWidgetItem *item = ImportPathNew->ui.twDatosJuego->topLevelItem( num );
			if( item->checkState(0) == Qt::Checked )
			{
				QString col_name = item->text(3);
				DatosJuego[col_name] = item->text(2);
			}
		}

		DatosJuego["Dat_tipo_emu"] = tmpDatosJuego["Dat_tipo_emu"];

		if( DatosJuego["Dat_thumbs_old"] != DatosJuego["Dat_thumbs"] )
			DatosJuego["Dat_thumbs_new"] = "true";
		else
			DatosJuego["Dat_thumbs_new"] = "false";

		if( DatosJuego["Dat_cover_front_old"] != DatosJuego["Dat_cover_front"] )
			DatosJuego["Dat_cover_front_new"] = "true";
		else
			DatosJuego["Dat_cover_front_new"] = "false";

		if( DatosJuego["Dat_cover_back_old"] != DatosJuego["Dat_cover_back"] )
			DatosJuego["Dat_cover_back_new"] = "true";
		else
			DatosJuego["Dat_cover_back_new"] = "false";

		if( tmpDatosJuego["Dat_tipo_emu"] == "dosbox" )
		{
			DatosDosBox["Dbx_path_conf"]       = ImportPathNew->ui.txtDbx_path_conf->text();
			DatosDosBox["Dbx_path_exe"]        = ImportPathNew->ui.txtDbx_path_exe->text();
			DatosDosBox["Dbx_path_setup"]      = ImportPathNew->ui.txtDbx_path_setup->text();
			DatosDosBox["Dbx_dosbox_language"] = ImportPathNew->ui.txtDbx_dosbox_language->text();
			DatosDosBox["Dbx_sdl_mapperfile"]  = ImportPathNew->ui.txtDbx_sdl_mapperfile->text();
			DatosDosBox["Dbx_gus_ultradir"]    = ImportPathNew->ui.txtDbx_gus_ultradir->text();
			DatosDosBox["Dbx_dosbox_captures"] = ImportPathNew->ui.txtDbx_dosbox_captures->text();
			DatosDosBox["Dbx_path_sonido"]     = ImportPathNew->ui.txtDbx_path_sonido->text();
		}

		if( tmpDatosJuego["Dat_tipo_emu"] == "scummvm" )
		{
			DatosScummVM["Svm_path"]          = ImportPathNew->ui.txtSvm_path->text();
			DatosScummVM["Svm_path_save"]     = ImportPathNew->ui.txtSvm_path_save->text();
			DatosScummVM["Svm_path_setup"]    = ImportPathNew->ui.txtSvm_path_setup->text();
			DatosScummVM["Svm_path_extra"]    = ImportPathNew->ui.txtSvm_path_extra->text();
			DatosScummVM["Svm_path_capturas"] = ImportPathNew->ui.txtSvm_path_capturas->text();
			DatosScummVM["Svm_path_sonido"]   = ImportPathNew->ui.txtSvm_path_sonido->text();
		}

		if( tmpDatosJuego["Dat_tipo_emu"] == "vdmsound" )
		{
			DatosVDMSound["Vdms_path_conf"] = ImportPathNew->ui.txtVdms_path_conf->text();
			DatosVDMSound["Vdms_path_exe"]  = ImportPathNew->ui.txtVdms_path_exe->text();
		}

		fGrl.GuardarKeyGRLConfig(stHomeDir + "GR-lida.conf", "OpcGeneral", "url_xmldb", ui.cbxScriptURL->itemData( ui.cbxScriptURL->currentIndex() ).toString() );

		QDialog::accept();
	} else
		QDialog::rejected();
}

void frmImportarJuego::on_btnAbrirUrl()
{
// Abre la URL con el navegador por defecto
	if( !stUrlWeb.isEmpty() )
		QDesktopServices::openUrl( stUrlWeb );
}

void frmImportarJuego::MostrarFichaHtml(QHash<QString, QString> datos)
{
	QString stThumbs, stIcono;

	setUpdatesEnabled( false );

	ui.txtHtmlInfo->clear();
	str_html_new.clear();
	str_html_new = str_html_old;

	stIcono = fGrl.getIconListaJuegos(datos["Dat_icono"], stIconDir);

	if( img_thumbs == "" )
		stThumbs = stTheme + "images/juego_sin_imagen.png";
	else
		stThumbs = stTempDir + img_thumbs;

// Reempla la info del juego.
	str_html_new.replace("{info_icono}"        , stIcono                   , Qt::CaseSensitive);
	str_html_new.replace("{info_titulo}"       , datos["Dat_titulo"]       , Qt::CaseSensitive);
	str_html_new.replace("{info_subtitulo}"    , datos["Dat_subtitulo"]    , Qt::CaseSensitive);
	str_html_new.replace("{info_genero}"       , datos["Dat_genero"]       , Qt::CaseSensitive);
	str_html_new.replace("{info_compania}"     , datos["Dat_compania"]     , Qt::CaseSensitive);
	str_html_new.replace("{info_desarrollador}", datos["Dat_desarrollador"], Qt::CaseSensitive);
	str_html_new.replace("{info_tema}"         , datos["Dat_tema"]         , Qt::CaseSensitive);
	str_html_new.replace("{info_perspectiva}"  , datos["Dat_perspectiva"]  , Qt::CaseSensitive);
	str_html_new.replace("{info_idioma}"       , datos["Dat_idioma"]       , Qt::CaseSensitive);
	str_html_new.replace("{info_idioma_voces}" , datos["Dat_idioma_voces"] , Qt::CaseSensitive);
	str_html_new.replace("{info_formato}"      , datos["Dat_formato"]      , Qt::CaseSensitive);
	str_html_new.replace("{info_anno}"         , datos["Dat_anno"]         , Qt::CaseSensitive);
	str_html_new.replace("{info_edad_recomendada}", stTheme +"img16/edad_"+ datos["Dat_edad_recomendada"] +".png", Qt::CaseSensitive);
//	str_html_new.replace("{info_numdisc}"      , datos["Dat_numdisc"]      , Qt::CaseSensitive);
	str_html_new.replace("{info_sistemaop}"    , datos["Dat_sistemaop"]    , Qt::CaseSensitive);
//	str_html_new.replace("{info_tamano}"       , datos["Dat_tamano"]       , Qt::CaseSensitive);
	str_html_new.replace("{info_graficos}"     , datos["Dat_graficos"]     , Qt::CaseSensitive);
	str_html_new.replace("{info_sonido}"       , datos["Dat_sonido"]       , Qt::CaseSensitive);
	str_html_new.replace("{info_jugabilidad}"  , datos["Dat_jugabilidad"]  , Qt::CaseSensitive);
	str_html_new.replace("{info_original}"     , datos["Dat_original"]     , Qt::CaseSensitive);
	str_html_new.replace("{info_estado}"       , datos["Dat_estado"]       , Qt::CaseSensitive);
	str_html_new.replace("{info_thumbs}"       , stThumbs                  , Qt::CaseSensitive);
//	str_html_new.replace("{info_cover_front}"  , datos["Dat_cover_front"]  , Qt::CaseSensitive);
//	str_html_new.replace("{info_cover_back}"   , datos["Dat_cover_back"]   , Qt::CaseSensitive);
	str_html_new.replace("{info_fecha}"        , datos["Dat_fecha"]        , Qt::CaseSensitive);
	str_html_new.replace("{info_tipo_emu}"     , datos["Dat_tipo_emu"]     , Qt::CaseSensitive);
	str_html_new.replace("{info_favorito}"     , datos["Dat_favorito"]     , Qt::CaseSensitive);
	str_html_new.replace("{info_rating}"       , datos["Dat_rating"]       , Qt::CaseSensitive);
	str_html_new.replace("{info_usuario}"      , datos["Dat_usuario"]      , Qt::CaseSensitive);
	str_html_new.replace("{info_comentario}"   , datos["Dat_comentario"]   , Qt::CaseSensitive);

	ui.txtHtmlInfo->setHtml( str_html_new );

	setUpdatesEnabled( true );

	ui.btnOk->setEnabled(true);
	if(ui.cbxScriptURL->currentIndex() > 0 )
		ui.btnVerInfo->setEnabled(true);
	else
		ui.btnVerInfo->setEnabled(false);
}

void frmImportarJuego::CargarScript(QString fileScript)
{
// Cargamos el script
	QFile scriptFile( fileScript );
	scriptFile.open(QIODevice::ReadOnly | QIODevice::Text);
	engine.evaluate( scriptFile.readAll() );
	scriptFile.close();

	CodecFileHtml.clear();
	if(engine.evaluate("url_charset").isValid())
		CodecFileHtml.append( engine.evaluate("url_charset").toString() );
	else
		CodecFileHtml.append("UTF-8");

	if(engine.evaluate("url_site").isValid())
		stUrlWeb = engine.evaluate("url_site").toString();
	else
		stUrlWeb = "";
}

void frmImportarJuego::on_changeScriptURL(int row)
{
	if( row >= 0)
	{
		if( ui.cbxScriptURL->itemData( row ).toString() == "_desde_archivo_" )
		{
			ui.btnAbrir->setEnabled(true);
			ui.txtTituloBuscar->setEnabled(false);
			ui.btnBuscar->setEnabled(false);
			ui.twListaBusqueda->setVisible(false);
			stUrlWeb = "";
		} else {
			ui.btnAbrir->setEnabled(false);
			ui.txtTituloBuscar->setEnabled(true);
			ui.btnBuscar->setEnabled(true);
			ui.twListaBusqueda->setVisible(true);

			CargarScript( stScriptsDir + ui.cbxScriptURL->itemData( row ).toString() );
		}
	}
}

void frmImportarJuego::on_twListaBusqueda_currentItemChanged(QTreeWidgetItem *item1, QTreeWidgetItem *item2)
{
	if( (item1)&&(item2) )
	{
		emit on_twListaBusqueda_clicked( ui.twListaBusqueda->currentItem() );
	} else
		return;
}

void frmImportarJuego::on_twListaBusqueda_clicked(QTreeWidgetItem *item)
{
	if( item )
	{
		ui.btnVerInfo->setEnabled(true);
	}
}

void frmImportarJuego::on_twListaBusqueda_Dblclicked(QTreeWidgetItem *item)
{
	if( item )
	{
		ui.btnVerInfo->setEnabled(true);
		ui.btnVerInfo->click();
	}
}

QString frmImportarJuego::LeerArchivoHTML(QString file_html)
{
	QTextCodec *codec = QTextCodec::codecForName(CodecFileHtml);
	QString text_html;
	QFile file( file_html );
	if( file.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
		QTextStream in( &file );
		in.setCodec(codec);
		text_html = in.readAll();

		text_html.replace("&#x27;", "'" );
		text_html.replace("&#x26;", "&" );
		text_html.replace("&#x22;", "\"");

		text_html.replace("&#39;" , "'" );
		text_html.replace("&#38;" , "&" );
		text_html.replace("&#34;" , "\"");

		text_html.replace("&apos;", "'" );
		text_html.replace("&amp;" , "&" );
		text_html.replace("&quot;", "\"");
	} else
		text_html = "";

	file.close();

	return text_html;
}

void frmImportarJuego::on_btnAbrir()
{
	bool siguiente = false;

	QString file_config = fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo para importarlo"), GRLConfig["DirImportar"].toString(), stHomeDir, tr("Soportados")+" (*.xml *.prof);;DB GR-lida (*.xml);;D-Fend Reloaded (*.prof);;"+tr("Todos los archivo")+" (*)", 0, false);

	QFileInfo fi( file_config );
	if( fi.exists() )
	{
		GRLConfig["DirImportar"] = fi.absolutePath();

		if( fi.suffix() == "xml" )
		{
			CargarScript(stHomeDir+"scripts/gr-lida.js");
			AnalyzePage( LeerArchivoHTML( file_config ), true);
			siguiente = true;
		}
		else if( fi.suffix() == "prof" )
		{
			AnalyzePage( file_config, true, true); // Indicamos que tipo DFend
			siguiente = true;
		} else
			siguiente = false;

		ui.btnOk->setEnabled(siguiente);
	} else
		GRLConfig["DirImportar"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir +"GR-lida.conf", "UltimoDirectorio", "DirImportar", GRLConfig["DirImportar"].toString() );
}

void frmImportarJuego::on_changeEnabled(bool estado)
{
	ui.cbxScriptURL->setEnabled(estado);
	ui.txtTituloBuscar->setEnabled(estado);
	ui.btnBuscar->setEnabled(estado);
	ui.twListaBusqueda->setEnabled(estado);

	emit ImportPathNew->on_changeEnabled(estado);
}

void frmImportarJuego::on_btnBuscar()
{
	if( ui.txtTituloBuscar->text() != "" && ui.cbxScriptURL->count() >= 0 )
	{
		QHash<QString, QString> url_filed;

		ui.btnVerInfo->setEnabled(false);
		ui.btnOk->setEnabled(false);

		ui.twListaBusqueda->clear();
		ui.txtHtmlInfo->clear();
		ui.twDatosFiles->clear();
		ui.twDatosURL->clear();
		ui.twMontajes->clear();
//---------------------------------------------------------------------------------------
//       URL BUSQUEDA
//---------------------------------------------------------------------------------------
		args.clear();
		ctor = engine.evaluate("UrlWebBusqueda");
		args << QScriptValue(&engine, ui.txtTituloBuscar->text() );

		QScriptValue m_array = ctor.call(QScriptValue(), args);
		QScriptValueIterator it(m_array);

		url_filed.clear();
		while(it.hasNext())
		{
			it.next();
			url_filed.insert(it.name(),it.value().toString());
		}

		indx_fin_descarga = AnalizarPaginaBusqueda;
		httpdown->downloadFile(url_filed["url"], stFileBusqueda, url_filed["metodo"], url_filed["c_post"]);
//---------------------------------------------------------------------------------------
//       FIN URL BUSQUEDA
//---------------------------------------------------------------------------------------
	}
}

void frmImportarJuego::on_btnVerInfo()
{
	int pos = ui.twListaBusqueda->indexOfTopLevelItem( ui.twListaBusqueda->currentItem() );
	if( ui.twListaBusqueda->topLevelItemCount() > 0 && pos != -1 )
	{
		ui.btnVerInfo->setEnabled(false);
		ui.btnOk->setEnabled(false);

		ui.txtHtmlInfo->clear();
		ui.twDatosFiles->clear();
		ui.twDatosURL->clear();
		ui.twMontajes->clear();

		indx_fin_descarga = AnalizarPaginaFicha;
		httpdown->downloadFile(ui.twListaBusqueda->currentItem()->text(3), stFileFicha);
	}
}

void frmImportarJuego::isRequestFinished()
{
	QString url_filed, PaginaBusqueda, PaginaFicha;
	switch( indx_fin_descarga )
	{
		case NohacerNada:
			//
		break;
		case MostrarFicha:
			MostrarFichaHtml( tmpDatosJuego );
		break;
		case AnalizarPaginaBusqueda:
//---------------------------------------------------------------------------------------
//       AnalizarPaginaBusqueda
//---------------------------------------------------------------------------------------
			PaginaBusqueda.clear();
			PaginaBusqueda = LeerArchivoHTML( stFileBusqueda );
			AddTitles( PaginaBusqueda );
//---------------------------------------------------------------------------------------
//       FIN AnalizarPaginaBusqueda
//---------------------------------------------------------------------------------------
		break;
		case AnalizarPaginaFicha:
//---------------------------------------------------------------------------------------
//       AnalizarPaginaFicha
//---------------------------------------------------------------------------------------
			PaginaFicha.clear();
			PaginaFicha = LeerArchivoHTML( stFileFicha );
			AnalyzePage( PaginaFicha );
//---------------------------------------------------------------------------------------
//       FIN AnalizarPaginaFicha
//---------------------------------------------------------------------------------------
		break;
		case CargarThumb:
//---------------------------------------------------------------------------------------
//       CargarThumb
//---------------------------------------------------------------------------------------
			indx_fin_descarga = MostrarFicha;
			if( !img_thumbs.isEmpty() && !img_url_cover_thumbs.isEmpty() )
			{
				if( !file_thumbs.exists( stTempDir + img_thumbs ) )
					httpdown_dos->downloadFile(img_url_cover_thumbs, stTempDir+img_thumbs);
				else
					isRequestFinished();
			}else {
				img_thumbs = "";
				img_url_cover_thumbs = "";
				tmpDatosJuego["Dat_thumbs"] = "";

				isRequestFinished();
			}
//---------------------------------------------------------------------------------------
//       FIN CargarThumb
//---------------------------------------------------------------------------------------
		break;
	}
}

void frmImportarJuego::AddTitles(QString ResultsPage)
{
	args.clear();
	ctor = engine.evaluate("AnalyzeFindPage");
	args << QScriptValue(&engine, ResultsPage );
	ctor.call(QScriptValue(), args);
}

void frmImportarJuego::AnalyzePage(QString Page, bool local, bool tipoDFend)
{
	QHash<QString, QString> datosImportar;
	datosImportar.clear();

	if( tipoDFend )
	{
		datosImportar = fGrl.Importar_Profile_DFend( Page );
	} else {
		args.clear();
		ctor = engine.evaluate("AnalyzeGamePage");
		args << QScriptValue(&engine, Page );
		args << QScriptValue(&engine, local );

		QScriptValue m_array = ctor.call(QScriptValue(), args);
		QScriptValueIterator it(m_array);

		while(it.hasNext())
		{
			it.next();
			datosImportar.insert(it.name(),it.value().toString());
		}
	}
// Datos ---------------------
	tmpDatosJuego.clear();
	tmpDatosJuego["Dat_icono"]         = datosImportar["Dat_icono"];
	tmpDatosJuego["Dat_titulo"]        = datosImportar["Dat_titulo"];
	tmpDatosJuego["Dat_subtitulo"]     = datosImportar["Dat_subtitulo"];
	tmpDatosJuego["Dat_genero"]        = datosImportar["Dat_genero"];
	tmpDatosJuego["Dat_compania"]      = datosImportar["Dat_compania"];
	tmpDatosJuego["Dat_desarrollador"] = datosImportar["Dat_desarrollador"];
	tmpDatosJuego["Dat_tema"]          = datosImportar["Dat_tema"];
	tmpDatosJuego["Dat_perspectiva"]   = datosImportar["Dat_perspectiva"];
	tmpDatosJuego["Dat_idioma"]        = datosImportar["Dat_idioma"];
	tmpDatosJuego["Dat_idioma_voces"]  = datosImportar["Dat_idioma_voces"];
	tmpDatosJuego["Dat_formato"]       = datosImportar["Dat_formato"];
	tmpDatosJuego["Dat_anno"]          = datosImportar["Dat_anno"];
	tmpDatosJuego["Dat_numdisc"]       = datosImportar["Dat_numdisc"];
	tmpDatosJuego["Dat_sistemaop"]     = datosImportar["Dat_sistemaop"];
	tmpDatosJuego["Dat_tamano"]        = datosImportar["Dat_tamano"];
	tmpDatosJuego["Dat_graficos"]      = datosImportar["Dat_graficos"];
	tmpDatosJuego["Dat_sonido"]        = datosImportar["Dat_sonido"];
	tmpDatosJuego["Dat_jugabilidad"]   = datosImportar["Dat_jugabilidad"];
	tmpDatosJuego["Dat_original"]      = datosImportar["Dat_original"];
	tmpDatosJuego["Dat_estado"]        = datosImportar["Dat_estado"];
	tmpDatosJuego["Dat_fecha"]         = fGrl.HoraFechaActual(GRLConfig["FormatoFecha"].toString());
	tmpDatosJuego["Dat_tipo_emu"]      = datosImportar["Dat_tipo_emu"];
	tmpDatosJuego["Dat_comentario"]    = datosImportar["Dat_comentario"];
	tmpDatosJuego["Dat_favorito"]      = "false";
	tmpDatosJuego["Dat_rating"]        = datosImportar["Dat_rating"];
	tmpDatosJuego["Dat_edad_recomendada"] = datosImportar["Dat_edad_recomendada"];
	tmpDatosJuego["Dat_usuario"]       = datosImportar["Dat_usuario"];
	tmpDatosJuego["Dat_path_exe"]      = datosImportar["Dat_path_exe"];
	tmpDatosJuego["Dat_parametros_exe"]= datosImportar["Dat_parametros_exe"];

// Nombre de la Imagen de las Caratulas --------
	img_thumbs.clear();
	img_cover_front.clear();
	img_cover_back.clear();
	if( datosImportar["Dat_thumbs"] == "" || datosImportar["Dat_thumbs"] == "null" )
		img_thumbs = "";
	else
		img_thumbs = "thumbs_"+datosImportar["Dat_thumbs"];

	if( datosImportar["Dat_cover_front"] == "" || datosImportar["Dat_cover_front"] == "null" )
		img_cover_front = "";
	else
		img_cover_front = "cover_front_"+datosImportar["Dat_cover_front"];

	if( datosImportar["Dat_cover_back"] == "" || datosImportar["Dat_cover_back"] == "null" )
		img_cover_back = "";
	else
		img_cover_back = "cover_back_"+datosImportar["Dat_cover_back"];

	tmpDatosJuego["Dat_thumbs"]      = img_thumbs;
	tmpDatosJuego["Dat_cover_front"] = img_cover_front;
	tmpDatosJuego["Dat_cover_back"]  = img_cover_back;

// Urls de descarga de la imagen
	img_url_cover_thumbs.clear();
	img_url_cover_front.clear();
	img_url_cover_back.clear();
	if(datosImportar["Dat_url_cover_thumbs"] == "")
		img_url_cover_thumbs = "";
	else
		img_url_cover_thumbs = datosImportar["Dat_url_cover_thumbs"];

	if(datosImportar["Dat_url_cover_front"] == "")
		img_url_cover_front = "";
	else
		img_url_cover_front = datosImportar["Dat_url_cover_front"];

	if(datosImportar["Dat_url_cover_back"] == "")
		img_url_cover_back = "";
	else
		img_url_cover_back  = datosImportar["Dat_url_cover_back"];

// DOSBox --------------------
	if( tmpDatosJuego["Dat_tipo_emu"] == "dosbox" )
	{
		tmpDatosDosBox.clear();
		tmpDatosDosBox["Dbx_sdl_fullscreen"]        = ""+datosImportar["Dbx_sdl_fullscreen"];
		tmpDatosDosBox["Dbx_sdl_fulldouble"]        = ""+datosImportar["Dbx_sdl_fulldouble"];
		tmpDatosDosBox["Dbx_sdl_fullfixed"]         = ""+datosImportar["Dbx_sdl_fullfixed"];
		tmpDatosDosBox["Dbx_sdl_fullresolution"]    = ""+datosImportar["Dbx_sdl_fullresolution"];
		tmpDatosDosBox["Dbx_sdl_windowresolution"]  = ""+datosImportar["Dbx_sdl_windowresolution"];
		tmpDatosDosBox["Dbx_sdl_output"]            = ""+datosImportar["Dbx_sdl_output"];
		tmpDatosDosBox["Dbx_sdl_hwscale"]           = ""+datosImportar["Dbx_sdl_hwscale"];
		tmpDatosDosBox["Dbx_sdl_autolock"]          = ""+datosImportar["Dbx_sdl_autolock"];
		tmpDatosDosBox["Dbx_sdl_sensitivity"]       = ""+datosImportar["Dbx_sdl_sensitivity"];
		tmpDatosDosBox["Dbx_sdl_waitonerror"]       = ""+datosImportar["Dbx_sdl_waitonerror"];
		tmpDatosDosBox["Dbx_sdl_priority"]          = ""+datosImportar["Dbx_sdl_priority"];
		tmpDatosDosBox["Dbx_sdl_mapperfile"]        = ""+datosImportar["Dbx_sdl_mapperfile"];
		tmpDatosDosBox["Dbx_sdl_usescancodes"]      = ""+datosImportar["Dbx_sdl_usescancodes"];
		tmpDatosDosBox["Dbx_dosbox_language"]       = ""+datosImportar["Dbx_dosbox_language"];
		tmpDatosDosBox["Dbx_dosbox_machine"]        = ""+datosImportar["Dbx_dosbox_machine"];
		tmpDatosDosBox["Dbx_dosbox_captures"]       = ""+datosImportar["Dbx_dosbox_captures"];
		tmpDatosDosBox["Dbx_dosbox_memsize"]        = ""+datosImportar["Dbx_dosbox_memsize"];
		tmpDatosDosBox["Dbx_render_frameskip"]      = ""+datosImportar["Dbx_render_frameskip"];
		tmpDatosDosBox["Dbx_render_aspect"]         = ""+datosImportar["Dbx_render_aspect"];
		tmpDatosDosBox["Dbx_render_scaler"]         = ""+datosImportar["Dbx_render_scaler"];
		tmpDatosDosBox["Dbx_cpu_core"]              = ""+datosImportar["Dbx_cpu_core"];
		tmpDatosDosBox["Dbx_cpu_cputype"]           = ""+datosImportar["Dbx_cpu_cputype"];
		tmpDatosDosBox["Dbx_cpu_cycles"]            = ""+datosImportar["Dbx_cpu_cycles"];
		tmpDatosDosBox["Dbx_cpu_cycles_realmode"]   = ""+datosImportar["Dbx_cpu_cycles_realmode"];
		tmpDatosDosBox["Dbx_cpu_cycles_protmode"]   = ""+datosImportar["Dbx_cpu_cycles_protmode"];
		tmpDatosDosBox["Dbx_cpu_cycles_limitmode"]  = ""+datosImportar["Dbx_cpu_cycles"];
		tmpDatosDosBox["Dbx_cpu_cycleup"]           = ""+datosImportar["Dbx_cpu_cycleup"];
		tmpDatosDosBox["Dbx_cpu_cycledown"]         = ""+datosImportar["Dbx_cpu_cycledown"];
		tmpDatosDosBox["Dbx_mixer_nosound"]         = ""+datosImportar["Dbx_mixer_nosound"];
		tmpDatosDosBox["Dbx_mixer_rate"]            = ""+datosImportar["Dbx_mixer_rate"];
		tmpDatosDosBox["Dbx_mixer_blocksize"]       = ""+datosImportar["Dbx_mixer_blocksize"];
		tmpDatosDosBox["Dbx_mixer_prebuffer"]       = ""+datosImportar["Dbx_mixer_prebuffer"];
		tmpDatosDosBox["Dbx_midi_mpu401"]           = ""+datosImportar["Dbx_midi_mpu401"];
		tmpDatosDosBox["Dbx_midi_intelligent"]      = ""+datosImportar["Dbx_midi_intelligent"];
		tmpDatosDosBox["Dbx_midi_device"]           = ""+datosImportar["Dbx_midi_device"];
		tmpDatosDosBox["Dbx_midi_config"]           = ""+datosImportar["Dbx_midi_config"];
		tmpDatosDosBox["Dbx_midi_mt32rate"]         = ""+datosImportar["Dbx_midi_mt32rate"];
		tmpDatosDosBox["Dbx_sblaster_sbtype"]       = ""+datosImportar["Dbx_sblaster_sbtype"];
		tmpDatosDosBox["Dbx_sblaster_sbbase"]       = ""+datosImportar["Dbx_sblaster_sbbase"];
		tmpDatosDosBox["Dbx_sblaster_irq"]          = ""+datosImportar["Dbx_sblaster_irq"];
		tmpDatosDosBox["Dbx_sblaster_dma"]          = ""+datosImportar["Dbx_sblaster_dma"];
		tmpDatosDosBox["Dbx_sblaster_hdma"]         = ""+datosImportar["Dbx_sblaster_hdma"];
		tmpDatosDosBox["Dbx_sblaster_mixer"]        = ""+datosImportar["Dbx_sblaster_mixer"];
		tmpDatosDosBox["Dbx_sblaster_oplmode"]      = ""+datosImportar["Dbx_sblaster_oplmode"];
		tmpDatosDosBox["Dbx_sblaster_oplemu"]       = ""+datosImportar["Dbx_sblaster_oplemu"];
		tmpDatosDosBox["Dbx_sblaster_oplrate"]      = ""+datosImportar["Dbx_sblaster_oplrate"];
		tmpDatosDosBox["Dbx_gus_gus"]               = ""+datosImportar["Dbx_gus_gus"];
		tmpDatosDosBox["Dbx_gus_gusrate"]           = ""+datosImportar["Dbx_gus_gusrate"];
		tmpDatosDosBox["Dbx_gus_gusbase"]           = ""+datosImportar["Dbx_gus_gusbase"];
		tmpDatosDosBox["Dbx_gus_irq1"]              = ""+datosImportar["Dbx_gus_irq1"];
		tmpDatosDosBox["Dbx_gus_irq2"]              = ""+datosImportar["Dbx_gus_irq2"];
		tmpDatosDosBox["Dbx_gus_dma1"]              = ""+datosImportar["Dbx_gus_dma1"];
		tmpDatosDosBox["Dbx_gus_dma2"]              = ""+datosImportar["Dbx_gus_dma2"];
		tmpDatosDosBox["Dbx_gus_ultradir"]          = ""+datosImportar["Dbx_gus_ultradir"];
		tmpDatosDosBox["Dbx_speaker_pcspeaker"]     = ""+datosImportar["Dbx_speaker_pcspeaker"];
		tmpDatosDosBox["Dbx_speaker_pcrate"]        = ""+datosImportar["Dbx_speaker_pcrate"];
		tmpDatosDosBox["Dbx_speaker_tandy"]         = ""+datosImportar["Dbx_speaker_tandy"];
		tmpDatosDosBox["Dbx_speaker_tandyrate"]     = ""+datosImportar["Dbx_speaker_tandyrate"];
		tmpDatosDosBox["Dbx_speaker_disney"]        = ""+datosImportar["Dbx_speaker_disney"];
		tmpDatosDosBox["Dbx_joystick_type"]         = ""+datosImportar["Dbx_joystick_type"];
		tmpDatosDosBox["Dbx_joystick_timed"]        = ""+datosImportar["Dbx_joystick_timed"];
		tmpDatosDosBox["Dbx_joystick_autofire"]     = ""+datosImportar["Dbx_joystick_autofire"];
		tmpDatosDosBox["Dbx_joystick_swap34"]       = ""+datosImportar["Dbx_joystick_swap34"];
		tmpDatosDosBox["Dbx_joystick_buttonwrap"]   = ""+datosImportar["Dbx_joystick_buttonwrap"];
		tmpDatosDosBox["Dbx_modem_modem"]           = ""+datosImportar["Dbx_modem_modem"];
		tmpDatosDosBox["Dbx_modem_comport"]         = ""+datosImportar["Dbx_modem_comport"];
		tmpDatosDosBox["Dbx_modem_listenport"]      = ""+datosImportar["Dbx_modem_listenport"];
		tmpDatosDosBox["Dbx_dserial_directserial"]  = ""+datosImportar["Dbx_dserial_directserial"];
		tmpDatosDosBox["Dbx_dserial_comport"]       = ""+datosImportar["Dbx_dserial_comport"];
		tmpDatosDosBox["Dbx_dserial_realport"]      = ""+datosImportar["Dbx_dserial_realport"];
		tmpDatosDosBox["Dbx_dserial_defaultbps"]    = ""+datosImportar["Dbx_dserial_defaultbps"];
		tmpDatosDosBox["Dbx_dserial_parity"]        = ""+datosImportar["Dbx_dserial_parity"];
		tmpDatosDosBox["Dbx_dserial_bytesize"]      = ""+datosImportar["Dbx_dserial_bytesize"];
		tmpDatosDosBox["Dbx_dserial_stopbit"]       = ""+datosImportar["Dbx_dserial_stopbit"];
		tmpDatosDosBox["Dbx_serial_1"]              = ""+datosImportar["Dbx_serial_1"];
		tmpDatosDosBox["Dbx_serial_2"]              = ""+datosImportar["Dbx_serial_2"];
		tmpDatosDosBox["Dbx_serial_3"]              = ""+datosImportar["Dbx_serial_3"];
		tmpDatosDosBox["Dbx_serial_4"]              = ""+datosImportar["Dbx_serial_4"];
		tmpDatosDosBox["Dbx_dos_xms"]               = ""+datosImportar["Dbx_dos_xms"];
		tmpDatosDosBox["Dbx_dos_ems"]               = ""+datosImportar["Dbx_dos_ems"];
		tmpDatosDosBox["Dbx_dos_umb"]               = ""+datosImportar["Dbx_dos_umb"];
		tmpDatosDosBox["Dbx_dos_keyboardlayout"]    = ""+datosImportar["Dbx_dos_keyboardlayout"];
		tmpDatosDosBox["Dbx_ipx_ipx"]               = ""+datosImportar["Dbx_ipx_ipx"];
		tmpDatosDosBox["Dbx_autoexec"]              = ""+datosImportar["Dbx_autoexec"];
		tmpDatosDosBox["Dbx_opt_autoexec"]          = ""+datosImportar["Dbx_opt_autoexec"];
		tmpDatosDosBox["Dbx_opt_loadfix"]           = ""+datosImportar["Dbx_opt_loadfix"];
		tmpDatosDosBox["Dbx_opt_loadfix_mem"]       = ""+datosImportar["Dbx_opt_loadfix_mem"];
		tmpDatosDosBox["Dbx_opt_consola_dbox"]      = ""+datosImportar["Dbx_opt_consola_dbox"];
		tmpDatosDosBox["Dbx_opt_cerrar_dbox"]       = ""+datosImportar["Dbx_opt_cerrar_dbox"];
		tmpDatosDosBox["Dbx_opt_cycle_sincronizar"] = ""+datosImportar["Dbx_opt_cycle_sincronizar"];
		tmpDatosDosBox["Dbx_path_conf"]             = ""+datosImportar["Dbx_path_conf"];
		tmpDatosDosBox["Dbx_path_sonido"]           = ""+datosImportar["Dbx_path_sonido"];
		tmpDatosDosBox["Dbx_path_exe"]              = ""+datosImportar["Dbx_path_exe"];
		tmpDatosDosBox["Dbx_path_setup"]            = ""+datosImportar["Dbx_path_setup"];
		tmpDatosDosBox["Dbx_parametros_exe"]        = ""+datosImportar["Dbx_parametros_exe"];
		tmpDatosDosBox["Dbx_parametros_setup"]      = ""+datosImportar["Dbx_parametros_setup"];

		if( tipoDFend )
		{
			QStringList str_Lista;
			QString tipoDrive, temp_opt_mount, IOCtrl_mount;
			int i = 0, num_mounts = 0;

			num_mounts = fGrl.StrToInt( datosImportar["NrOfMounts"] );
			if( num_mounts > 0 )
			{
				for( i = 0; i < num_mounts ; i++)
				{
					QTreeWidgetItem *twItemDfend = new QTreeWidgetItem(ui.twMontajes);

					str_Lista.clear();
					str_Lista = datosImportar[fGrl.IntToStr(i)].split("|");

					tipoDrive.clear();
					tipoDrive = str_Lista.value(1).toLower();// tipo_as

					if(tipoDrive == "drive")
						tipoDrive = "drive";
					else if(tipoDrive == "cdrom")
						tipoDrive = "cdrom";
					else if(tipoDrive == "floppy")
						tipoDrive = "floppy";
					else if(tipoDrive == "floppyimage")
						tipoDrive = "IMG_floppy";
					else if(tipoDrive == "cdromimage")
						tipoDrive = "IMG_iso";
					else if(tipoDrive == "image")
						tipoDrive = "IMG_hdd";
					else
						tipoDrive = "drive";

					temp_opt_mount.clear();
					temp_opt_mount = str_Lista.value(5);

					if( str_Lista.value(3) == "true" )
						IOCtrl_mount = "-ioctl";
					else if( str_Lista.value(3) == "false" )
						IOCtrl_mount = "";
					else if( str_Lista.value(3) == "NoIOCTL" )
						IOCtrl_mount = "-noioctl";
					else
						IOCtrl_mount = "";

					twItemDfend->setIcon(0, QIcon( fGrl.getIconMount(tipoDrive,"") ) );
					twItemDfend->setText(0, ""+str_Lista.value(0).replace("{DirBaseGames}", GRLConfig["DirBaseGames"].toString() ) ); // path
					twItemDfend->setText(1, ""+str_Lista.value(4) ); // label
					twItemDfend->setText(2, ""+tipoDrive          ); // tipo_as
					twItemDfend->setText(3, ""+str_Lista.value(2) ); // letter
					twItemDfend->setText(4, ""+fGrl.IntToStr(i)   ); // indx_cd
					twItemDfend->setText(5, ""+temp_opt_mount     ); // opt_mount
					twItemDfend->setText(6, ""+IOCtrl_mount       ); // io_ctrl
					twItemDfend->setText(7, "x"                   ); // select_mount
				}
			}
		}
	}

// ScummVM -------------------
	if( tmpDatosJuego["Dat_tipo_emu"] == "scummvm" )
	{
		tmpDatosScummVM.clear();
		tmpDatosScummVM["Svm_game"]            = ""+datosImportar["Svm_game"];
		tmpDatosScummVM["Svm_game_label"]      = ""+datosImportar["Svm_game_label"];
		tmpDatosScummVM["Svm_language"]        = ""+datosImportar["Svm_language"];
		tmpDatosScummVM["Svm_subtitles"]       = ""+datosImportar["Svm_subtitles"];
		tmpDatosScummVM["Svm_platform"]        = ""+datosImportar["Svm_platform"];
		tmpDatosScummVM["Svm_gfx_mode"]        = ""+datosImportar["Svm_gfx_mode"];
		tmpDatosScummVM["Svm_render_mode"]     = ""+datosImportar["Svm_render_mode"];
		tmpDatosScummVM["Svm_fullscreen"]      = ""+datosImportar["Svm_fullscreen"];
		tmpDatosScummVM["Svm_aspect_ratio"]    = ""+datosImportar["Svm_aspect_ratio"];
		tmpDatosScummVM["Svm_path"]            = ""+datosImportar["Svm_path"];
		tmpDatosScummVM["Svm_path_setup"]      = ""+datosImportar["Svm_path_setup"];
		tmpDatosScummVM["Svm_path_extra"]      = ""+datosImportar["Svm_path_extra"];
		tmpDatosScummVM["Svm_path_save"]       = ""+datosImportar["Svm_path_save"];
		tmpDatosScummVM["Svm_path_capturas"]   = ""+datosImportar["Svm_path_capturas"];
		tmpDatosScummVM["Svm_path_sonido"]     = ""+datosImportar["Svm_path_sonido"];
		tmpDatosScummVM["Svm_music_driver"]    = ""+datosImportar["Svm_music_driver"];
		tmpDatosScummVM["Svm_enable_gs"]       = ""+datosImportar["Svm_enable_gs"];
		tmpDatosScummVM["Svm_multi_midi"]      = ""+datosImportar["Svm_multi_midi"];
		tmpDatosScummVM["Svm_native_mt32"]     = ""+datosImportar["Svm_native_mt32"];
		tmpDatosScummVM["Svm_master_volume"]   = ""+datosImportar["Svm_master_volume"];
		tmpDatosScummVM["Svm_music_volume"]    = ""+datosImportar["Svm_music_volume"];
		tmpDatosScummVM["Svm_sfx_volume"]      = ""+datosImportar["Svm_sfx_volume"];
		tmpDatosScummVM["Svm_speech_volume"]   = ""+datosImportar["Svm_speech_volume"];
		tmpDatosScummVM["Svm_tempo"]           = ""+datosImportar["Svm_tempo"];
		tmpDatosScummVM["Svm_talkspeed"]       = ""+datosImportar["Svm_talkspeed"];
		tmpDatosScummVM["Svm_debuglevel"]      = ""+datosImportar["Svm_debuglevel"];
		tmpDatosScummVM["Svm_cdrom"]           = ""+datosImportar["Svm_cdrom"];
		tmpDatosScummVM["Svm_joystick_num"]    = ""+datosImportar["Svm_joystick_num"];
		tmpDatosScummVM["Svm_output_rate"]     = ""+datosImportar["Svm_output_rate"];
		tmpDatosScummVM["Svm_midi_gain"]       = ""+datosImportar["Svm_midi_gain"];
		tmpDatosScummVM["Svm_copy_protection"] = ""+datosImportar["Svm_copy_protection"];
		tmpDatosScummVM["Svm_sound_font"]      = ""+datosImportar["Svm_sound_font"];
		tmpDatosScummVM["Svm_walkspeed"]       = ""+datosImportar["Svm_walkspeed"];
		tmpDatosScummVM["Svm_opl_driver"]      = ""+datosImportar["Svm_opl_driver"];
		tmpDatosScummVM["Svm_disable_dithering"] = ""+datosImportar["Svm_disable_dithering"];
		tmpDatosScummVM["Svm_mute"]              = ""+datosImportar["Svm_mute"];
		tmpDatosScummVM["Svm_speech_mute"]       = ""+datosImportar["Svm_speech_mute"];
	}
// VDMSound ------------------
//----------------------------

	indx_fin_descarga = CargarThumb;
	isRequestFinished();
}

void frmImportarJuego::InsertarItemDatosJuego(QString etiqueta, QString old_dat, QString new_dat, QString key_dat)
{
	QTreeWidgetItem *item = new QTreeWidgetItem(ImportPathNew->ui.twDatosJuego);
	item->setCheckState(0, Qt::Checked);
	item->setIcon( 0, QIcon(stTheme +"img16/datos_3.png"));
	item->setText( 0, etiqueta );
	item->setText( 1, old_dat  );
	item->setText( 2, new_dat  );
	item->setText( 3, key_dat  );
}
