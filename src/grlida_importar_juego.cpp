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
frmImportarJuego::frmImportarJuego(QString titulo_busqueda, QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);

	stHomeDir = fGrl.GRlidaHomePath();
	stTheme   = fGrl.ThemeGrl();
	GRLConfig = fGrl.CargarGRLConfig( stHomeDir + "GR-lida.conf" );

	stFileBusqueda = stHomeDir + "temp/busqueda.html";
	stFileFicha    = stHomeDir + "temp/ficha.html";
	stTempDir      = stHomeDir + "temp/";
	stCoversDir    = stHomeDir + "covers/";
	stThumbsDir    = stHomeDir + "thumbs/";

	connect( ui.cbxScriptURL, SIGNAL( activated(int) ), this, SLOT( on_changeScriptURL(int) ) );
	connect( ui.btnOk       , SIGNAL( clicked() ), this, SLOT( on_btnOk()             ) );
	connect( ui.btnAbrir    , SIGNAL( clicked() ), this, SLOT( on_btnAbrir()          ) );
	connect( ui.btnBuscar   , SIGNAL( clicked() ), this, SLOT( on_btnBuscar()         ) );
	connect( ui.btnVerInfo  , SIGNAL( clicked() ), this, SLOT( on_btnVerInfo()        ) );
	connect( ui.btnAbrirUrl , SIGNAL( clicked() ), this, SLOT( on_btnAbrirUrl()       ) );

	connect( ui.twListaBusqueda, SIGNAL( currentItemChanged( QTreeWidgetItem *, QTreeWidgetItem *) ), SLOT( on_twListaBusqueda_currentItemChanged( QTreeWidgetItem *, QTreeWidgetItem *) ) );
	connect( ui.twListaBusqueda, SIGNAL( itemClicked( QTreeWidgetItem*, int)), this, SLOT( on_twListaBusqueda_clicked(QTreeWidgetItem*)));
	connect( ui.twListaBusqueda, SIGNAL( itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( on_twListaBusqueda_Dblclicked(QTreeWidgetItem*) ));

	httpdown = new HttpDownload(this);
	httpdown->setHidden(true);
	connect(httpdown, SIGNAL( StatusRequestFinished()        ), this, SLOT( isRequestFinished() ) );
	connect(httpdown, SIGNAL( statusLabelChanged(QString)    ), ui.lb_status, SLOT( setText(QString) ) );
	connect(httpdown, SIGNAL( StatusBtnDownloadChanged(bool) ), this, SLOT( on_changeEnabled(bool) ) );

	if( GRLConfig["ProxyEnable"].toBool() )
		httpdown->setHttpProxy( GRLConfig["ProxyType"].toInt(), GRLConfig["ProxyHost"].toString(), GRLConfig["ProxyPort"].toInt(), GRLConfig["ProxyUserName"].toString(), GRLConfig["ProxyPassword"].toString() );

	setTheme();

	ui.twListaBusqueda->header()->setStretchLastSection(false);
	ui.twListaBusqueda->header()->setMovable(false);
	ui.twListaBusqueda->header()->setResizeMode(0, QHeaderView::Stretch);
//	ui.twListaBusqueda->header()->setResizeMode(1, QHeaderView::Interactive);
//	ui.twListaBusqueda->header()->setResizeMode(2, QHeaderView::Interactive);
//	ui.twListaBusqueda->header()->setResizeMode(2, QHeaderView::Fixed);
	ui.twListaBusqueda->setColumnWidth(1,60);
	ui.twListaBusqueda->setColumnWidth(2,35);

	ui.txtTituloBuscar->setText( titulo_busqueda );
	ui.txtTituloBuscar->setFocus();

	engine.setDefaultPrototype(qMetaTypeId<QTreeWidget*>(), engine.newQObject(&twProto));
	engine.globalObject().setProperty("twListaBusqueda", engine.newQObject(ui.twListaBusqueda));
	engine.globalObject().setProperty("twDatosFiles"   , engine.newQObject(ui.twDatosFiles));
	engine.globalObject().setProperty("twDatosURL"     , engine.newQObject(ui.twDatosURL));
	engine.globalObject().setProperty("twMounts"       , engine.newQObject(ui.twMontajes));

	CodecFileHtml.clear();
	CodecFileHtml.append("UTF-8");

	fGrl.CargarScriptsComboBox(stHomeDir+"scripts/", ui.cbxScriptURL);

	int rowScript = ui.cbxScriptURL->findData( GRLConfig["url_xmldb"].toString() );
	if( rowScript < 0 ) rowScript = 1;

	ui.cbxScriptURL->setCurrentIndex( rowScript );
	on_changeScriptURL( rowScript );

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmImportarJuego::~frmImportarJuego()
{

}

void frmImportarJuego::on_btnOk()
{
	frmImportPath *ImportPathNew = new frmImportPath();

	ImportPathNew->ui.gBox_path_emu->setEnabled( false );
	ImportPathNew->ui.gBox_path_emu->setTitle("Path");
//---------------------------------------------------------------------------------------
//       CargarCoverFront
//---------------------------------------------------------------------------------------
	if( DatosJuego["Dat_cover_front"] != "" || DatosJuego["Dat_cover_front"] != "null" )
	{
		indx_fin_descarga = NohacerNada;
		if( !img_cover_front.isEmpty() && !img_url_cover_front.isEmpty() )
		{
			if( !file_cover_front.exists( stTempDir+img_cover_front ) )
			{
				connect(httpdown, SIGNAL( StatusBtnDownloadChanged(bool) ), ImportPathNew , SLOT( on_changeEnabled(bool) ) );
				httpdown->downloadFile(img_url_cover_front, stTempDir + img_cover_front);
			} else
				isRequestFinished();
		} else {
			img_cover_front = "";
			img_url_cover_front = "";
			DatosJuego["Dat_cover_front"] = "";

			isRequestFinished();
		}
	} else
		DatosJuego["Dat_cover_front"] = "";
//---------------------------------------------------------------------------------------
//       FIN CargarCoverFront
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//       CargarCoverBack
//---------------------------------------------------------------------------------------
	if( DatosJuego["Dat_cover_back"] != "" || DatosJuego["Dat_cover_back"] != "null" )
	{
		indx_fin_descarga = NohacerNada;
		if( !img_cover_back.isEmpty() && !img_url_cover_back.isEmpty() )
		{
			if( !file_cover_back.exists( stTempDir+img_cover_back ) )
			{
				HttpDownload *httpdown_dos = new HttpDownload(this);
				httpdown_dos->setHidden(true);
				connect(httpdown_dos, SIGNAL( StatusRequestFinished() ), this, SLOT( isRequestFinished() ) );
				connect(httpdown_dos, SIGNAL( StatusBtnDownloadChanged(bool) ), ImportPathNew, SLOT( on_changeEnabled(bool) ) );

				if( GRLConfig["ProxyEnable"].toBool() )
					httpdown_dos->setHttpProxy( GRLConfig["ProxyType"].toInt(), GRLConfig["ProxyHost"].toString(), GRLConfig["ProxyPort"].toInt(), GRLConfig["ProxyUserName"].toString(), GRLConfig["ProxyPassword"].toString() );

				httpdown_dos->downloadFile(img_url_cover_back, stTempDir + img_cover_back);
			} else
				isRequestFinished();
		} else {
			img_cover_back = "";
			img_url_cover_back = "";
			DatosJuego["Dat_cover_back"] = "";

			isRequestFinished();
		}
	} else
		DatosJuego["Dat_cover_back"] = "";
//---------------------------------------------------------------------------------------
//       FIN CargarCoverBack
//---------------------------------------------------------------------------------------

	if( DatosJuego["Dat_tipo_emu"] == "dosbox" )
	{
		if( QFile::exists(stHomeDir +"confdbx/"+ DatosDosBox["Dbx_path_conf"] ) )
			ImportPathNew->ui.txtPath_Dbx_1->setText( DatosDosBox["Dbx_path_conf"].prepend(fGrl.HoraFechaActual("ddMMyyyy_HHmmss")+"-") );
		else
			ImportPathNew->ui.txtPath_Dbx_1->setText( DatosDosBox["Dbx_path_conf"] ); // path_conf

		ImportPathNew->ui.gBox_path_emu->setEnabled( true );
		ImportPathNew->ui.gBox_path_emu->setTitle("Path - DOSBox");
		ImportPathNew->ui.wizardPath->setCurrentIndex(0);
	//	ImportPathNew->ui.txtPath_Dbx_1->setText( DatosDosBox["Dbx_path_conf"]       );		// path_conf
		ImportPathNew->ui.txtPath_Dbx_2->setText( DatosDosBox["Dbx_path_exe"].replace("{DirBaseGames}", GRLConfig["DirBaseGames"].toString() ) );	// path_exe
		ImportPathNew->ui.txtPath_Dbx_3->setText( DatosDosBox["Dbx_path_setup"].replace("{DirBaseGames}", GRLConfig["DirBaseGames"].toString() ) );	// path_setup
		ImportPathNew->ui.txtPath_Dbx_4->setText( DatosDosBox["Dbx_dosbox_language"] );		// dosbox_language
		ImportPathNew->ui.txtPath_Dbx_5->setText( DatosDosBox["Dbx_sdl_mapperfile"]  );		// sdl_mapperfile
		ImportPathNew->ui.txtPath_Dbx_6->setText( DatosDosBox["Dbx_gus_ultradir"]    );		// gus_ultradir
		ImportPathNew->ui.txtPath_Dbx_7->setText( DatosDosBox["Dbx_dosbox_captures"] );		// dosbox_captures
		ImportPathNew->ui.txtPath_Dbx_8->setText( DatosDosBox["Dbx_path_sonido"]     );		// path_sonido
	}

	if( DatosJuego["Dat_tipo_emu"] == "scummvm" )
	{
		ImportPathNew->ui.gBox_path_emu->setEnabled( true );
		ImportPathNew->ui.gBox_path_emu->setTitle("Path - ScummVM");
		ImportPathNew->ui.wizardPath->setCurrentIndex(1);
		ImportPathNew->ui.txtPath_Svm_1->setText( DatosScummVM["Svm_path"].replace("{DirBaseGames}"      , GRLConfig["DirBaseGames"].toString() ) );	// path
		ImportPathNew->ui.txtPath_Svm_2->setText( DatosScummVM["Svm_path_save"].replace("{DirBaseGames}" , GRLConfig["DirBaseGames"].toString() ) );	// path_save
		ImportPathNew->ui.txtPath_Svm_3->setText( DatosScummVM["Svm_path_setup"].replace("{DirBaseGames}", GRLConfig["DirBaseGames"].toString() ) );	// path_setup
		ImportPathNew->ui.txtPath_Svm_4->setText( DatosScummVM["Svm_path_extra"].replace("{DirBaseGames}", GRLConfig["DirBaseGames"].toString() ) );	// path_extra
		ImportPathNew->ui.txtPath_Svm_5->setText( DatosScummVM["Svm_path_capturas"] );	// path_capturas
		ImportPathNew->ui.txtPath_Svm_6->setText( DatosScummVM["Svm_path_sonido"]   );	// path_sonido
	}

	if( DatosJuego["Dat_tipo_emu"] == "vdmsound" )
	{
		if( QFile::exists(stHomeDir +"confvdms/"+ DatosVDMSound["Vdms_path_conf"] ) )
			ImportPathNew->ui.txtPath_Dbx_1->setText( DatosVDMSound["Vdms_path_conf"].prepend(fGrl.HoraFechaActual("ddMMyyyy_HHmmss")+"-") );
		else
			ImportPathNew->ui.txtPath_Dbx_1->setText( DatosVDMSound["Vdms_path_conf"] ); // path_conf

		ImportPathNew->ui.gBox_path_emu->setEnabled( true );
		ImportPathNew->ui.gBox_path_emu->setTitle("Path - VDMSound");
		ImportPathNew->ui.wizardPath->setCurrentIndex(2);
	//	ImportPathNew->ui.txtPath_Vdms_1->setText( DatosVDMSound["Vdms_path_conf"] );	// path_conf
		ImportPathNew->ui.txtPath_Vdms_2->setText( DatosVDMSound["Vdms_path_exe"].replace("{DirBaseGames}", GRLConfig["DirBaseGames"].toString() ) );	// path_exe
	}

	if( ImportPathNew->exec() == QDialog::Accepted )
	{
		if( DatosJuego["Dat_tipo_emu"] == "dosbox" )
		{
			DatosDosBox["Dbx_path_conf"]      = ImportPathNew->ui.txtPath_Dbx_1->text();	// path_conf
			DatosDosBox["Dbx_path_exe"]       = ImportPathNew->ui.txtPath_Dbx_2->text();	// path_exe
			DatosDosBox["Dbx_path_setup"]     = ImportPathNew->ui.txtPath_Dbx_3->text();	// path_setup
			DatosDosBox["Dbx_dosbox_language"]= ImportPathNew->ui.txtPath_Dbx_4->text();	// dosbox_language
			DatosDosBox["Dbx_sdl_mapperfile"] = ImportPathNew->ui.txtPath_Dbx_5->text();	// sdl_mapperfile
			DatosDosBox["Dbx_gus_ultradir"]   = ImportPathNew->ui.txtPath_Dbx_6->text();	// gus_ultradir
			DatosDosBox["Dbx_dosbox_captures"]= ImportPathNew->ui.txtPath_Dbx_7->text();	// dosbox_captures
			DatosDosBox["Dbx_path_sonido"]    = ImportPathNew->ui.txtPath_Dbx_8->text();	// path_sonido
		}

		if( DatosJuego["Dat_tipo_emu"] == "scummvm" )
		{
			DatosScummVM["Svm_path"]          = ImportPathNew->ui.txtPath_Svm_1->text();	// path
			DatosScummVM["Svm_path_save"]     = ImportPathNew->ui.txtPath_Svm_2->text();	// path_save
			DatosScummVM["Svm_path_setup"]    = ImportPathNew->ui.txtPath_Svm_3->text();	// path_setup
			DatosScummVM["Svm_path_extra"]    = ImportPathNew->ui.txtPath_Svm_4->text();	// path_extra
			DatosScummVM["Svm_path_capturas"] = ImportPathNew->ui.txtPath_Svm_5->text();	// path_capturas
			DatosScummVM["Svm_path_sonido"]   = ImportPathNew->ui.txtPath_Svm_6->text();	// path_sonido
		}

		if( DatosJuego["Dat_tipo_emu"] == "vdmsound" )
		{
			DatosVDMSound["Vdms_path_conf"] = ImportPathNew->ui.txtPath_Vdms_1->text();		// path_conf
			DatosVDMSound["Vdms_path_exe"  ]= ImportPathNew->ui.txtPath_Vdms_2->text();		// path_exe
		}

		fGrl.GuardarKeyGRLConfig(stHomeDir + "GR-lida.conf", "OpcGeneral", "url_xmldb", ui.cbxScriptURL->itemData( ui.cbxScriptURL->currentIndex() ).toString() );

		QDialog::accept();
	} else
		QDialog::rejected();

	delete ImportPathNew;
}

void frmImportarJuego::setTheme()
{
	setUpdatesEnabled( false );

	setStyleSheet( fGrl.StyleSheet() );
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

	stIcono = fGrl.getIconListaJuegos(datos["Dat_icono"], stHomeDir + "iconos/");

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

			CargarScript( stHomeDir +"scripts/"+ui.cbxScriptURL->itemData( row ).toString() );
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

		text_html.replace("&#x27;","'");
		text_html.replace("&#x26;","&");
	//	text_html.replace("&#xE9;","Ã©");
	//	text_html.replace("&#xE0;","Ã ");
		text_html.replace("&aacute;","á");
		text_html.replace("&eacute;","é");
		text_html.replace("&iacute;","í");
		text_html.replace("&oacute;","ó");
		text_html.replace("&uacute;","ú");
		text_html.replace("&nbsp;"," ");
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

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","DirImportar", GRLConfig["DirImportar"].toString() );
}

void frmImportarJuego::on_changeEnabled(bool estado)
{
	ui.cbxScriptURL->setEnabled(estado);
	ui.txtTituloBuscar->setEnabled(estado);
	ui.btnBuscar->setEnabled(estado);
	ui.twListaBusqueda->setEnabled(estado);
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
//		if( url_filed["tipo"] == "POST" )
//			httpdown->downloadFile(url_filed["url"], stFileBusqueda, true, true, url_filed["c_post"]);
//		else
//			httpdown->downloadFile(url_filed["url"], stFileBusqueda, true);

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
			MostrarFichaHtml( DatosJuego );
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
				{
					HttpDownload *httpdown_dos = new HttpDownload(this);
					httpdown_dos->setHidden(true);
					connect(httpdown_dos, SIGNAL( StatusRequestFinished() ), this, SLOT( isRequestFinished() ) );

					if( GRLConfig["ProxyEnable"].toBool() )
						httpdown_dos->setHttpProxy( GRLConfig["ProxyType"].toInt(), GRLConfig["ProxyHost"].toString(), GRLConfig["ProxyPort"].toInt(), GRLConfig["ProxyUserName"].toString(), GRLConfig["ProxyPassword"].toString() );

					httpdown_dos->downloadFile(img_url_cover_thumbs, stTempDir+img_thumbs);
				} else
					isRequestFinished();
			}else {
				img_thumbs = "";
				img_url_cover_thumbs = "";
				DatosJuego["Dat_thumbs"] = "";

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
	DatosJuegoTemp.clear();

	if( tipoDFend )
	{
		DatosJuegoTemp = fGrl.Importar_Profile_DFend( Page );
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
			DatosJuegoTemp.insert(it.name(),it.value().toString());
		}
	}
// Datos ---------------------
	DatosJuego.clear();
	DatosJuego["Dat_icono"]         = DatosJuegoTemp["Dat_icono"];
	DatosJuego["Dat_titulo"]        = DatosJuegoTemp["Dat_titulo"];
	DatosJuego["Dat_subtitulo"]     = DatosJuegoTemp["Dat_subtitulo"];
	DatosJuego["Dat_genero"]        = DatosJuegoTemp["Dat_genero"];
	DatosJuego["Dat_compania"]      = DatosJuegoTemp["Dat_compania"];
	DatosJuego["Dat_desarrollador"] = DatosJuegoTemp["Dat_desarrollador"];
	DatosJuego["Dat_tema"]          = DatosJuegoTemp["Dat_tema"];
	DatosJuego["Dat_perspectiva"]   = DatosJuegoTemp["Dat_perspectiva"];
	DatosJuego["Dat_idioma"]        = DatosJuegoTemp["Dat_idioma"];
	DatosJuego["Dat_idioma_voces"]  = DatosJuegoTemp["Dat_idioma_voces"];
	DatosJuego["Dat_formato"]       = DatosJuegoTemp["Dat_formato"];
	DatosJuego["Dat_anno"]          = DatosJuegoTemp["Dat_anno"];
	DatosJuego["Dat_numdisc"]       = DatosJuegoTemp["Dat_numdisc"];
	DatosJuego["Dat_sistemaop"]     = DatosJuegoTemp["Dat_sistemaop"];
	DatosJuego["Dat_tamano"]        = DatosJuegoTemp["Dat_tamano"];
	DatosJuego["Dat_graficos"]      = DatosJuegoTemp["Dat_graficos"];
	DatosJuego["Dat_sonido"]        = DatosJuegoTemp["Dat_sonido"];
	DatosJuego["Dat_jugabilidad"]   = DatosJuegoTemp["Dat_jugabilidad"];
	DatosJuego["Dat_original"]      = DatosJuegoTemp["Dat_original"];
	DatosJuego["Dat_estado"]        = DatosJuegoTemp["Dat_estado"];
	DatosJuego["Dat_fecha"]         = fGrl.HoraFechaActual(GRLConfig["FormatoFecha"].toString());
	DatosJuego["Dat_tipo_emu"]      = DatosJuegoTemp["Dat_tipo_emu"];
	DatosJuego["Dat_comentario"]    = DatosJuegoTemp["Dat_comentario"];
	DatosJuego["Dat_favorito"]      = "false";
	DatosJuego["Dat_rating"]        = DatosJuegoTemp["Dat_rating"];
	DatosJuego["Dat_edad_recomendada"] = DatosJuegoTemp["Dat_edad_recomendada"];
	DatosJuego["Dat_usuario"]       = DatosJuegoTemp["Dat_usuario"];
	DatosJuego["Dat_path_exe"]      = DatosJuegoTemp["Dat_path_exe"];
	DatosJuego["Dat_parametros_exe"]= DatosJuegoTemp["Dat_parametros_exe"];

// Nombre de la Imagen de las Caratulas --------
	img_thumbs.clear();
	img_cover_front.clear();
	img_cover_back.clear();
	if( DatosJuegoTemp["Dat_thumbs"] == "" || DatosJuegoTemp["Dat_thumbs"] == "null" )
		img_thumbs = "";
	else
		img_thumbs = "thumbs_"+DatosJuegoTemp["Dat_thumbs"];

	if( DatosJuegoTemp["Dat_cover_front"] == "" || DatosJuegoTemp["Dat_cover_front"] == "null" )
		img_cover_front = "";
	else
		img_cover_front = "cover_front_"+DatosJuegoTemp["Dat_cover_front"];

	if( DatosJuegoTemp["Dat_cover_back"] == "" || DatosJuegoTemp["Dat_cover_back"] == "null" )
		img_cover_back = "";
	else
		img_cover_back = "cover_back_"+DatosJuegoTemp["Dat_cover_back"];

	DatosJuego["Dat_thumbs"]      = img_thumbs;
	DatosJuego["Dat_cover_front"] = img_cover_front;
	DatosJuego["Dat_cover_back"]  = img_cover_back;

// Urls de descarga de la imagen
	img_url_cover_thumbs.clear();
	img_url_cover_front.clear();
	img_url_cover_back.clear();
	if(DatosJuegoTemp["Dat_url_cover_thumbs"] == "")
		img_url_cover_thumbs = "";
	else
		img_url_cover_thumbs = DatosJuegoTemp["Dat_url_cover_thumbs"];

	if(DatosJuegoTemp["Dat_url_cover_front"] == "")
		img_url_cover_front = "";
	else
		img_url_cover_front = DatosJuegoTemp["Dat_url_cover_front"];

	if(DatosJuegoTemp["Dat_url_cover_back"] == "")
		img_url_cover_back = "";
	else
		img_url_cover_back  = DatosJuegoTemp["Dat_url_cover_back"];

// DOSBox --------------------
	if( DatosJuego["Dat_tipo_emu"] == "dosbox" )
	{
		DatosDosBox.clear();
		DatosDosBox["Dbx_sdl_fullscreen"]        = ""+DatosJuegoTemp["Dbx_sdl_fullscreen"];
		DatosDosBox["Dbx_sdl_fulldouble"]        = ""+DatosJuegoTemp["Dbx_sdl_fulldouble"];
		DatosDosBox["Dbx_sdl_fullfixed"]         = ""+DatosJuegoTemp["Dbx_sdl_fullfixed"];
		DatosDosBox["Dbx_sdl_fullresolution"]    = ""+DatosJuegoTemp["Dbx_sdl_fullresolution"];
		DatosDosBox["Dbx_sdl_windowresolution"]  = ""+DatosJuegoTemp["Dbx_sdl_windowresolution"];
		DatosDosBox["Dbx_sdl_output"]            = ""+DatosJuegoTemp["Dbx_sdl_output"];
		DatosDosBox["Dbx_sdl_hwscale"]           = ""+DatosJuegoTemp["Dbx_sdl_hwscale"];
		DatosDosBox["Dbx_sdl_autolock"]          = ""+DatosJuegoTemp["Dbx_sdl_autolock"];
		DatosDosBox["Dbx_sdl_sensitivity"]       = ""+DatosJuegoTemp["Dbx_sdl_sensitivity"];
		DatosDosBox["Dbx_sdl_waitonerror"]       = ""+DatosJuegoTemp["Dbx_sdl_waitonerror"];
		DatosDosBox["Dbx_sdl_priority"]          = ""+DatosJuegoTemp["Dbx_sdl_priority"];
		DatosDosBox["Dbx_sdl_mapperfile"]        = ""+DatosJuegoTemp["Dbx_sdl_mapperfile"];
		DatosDosBox["Dbx_sdl_usescancodes"]      = ""+DatosJuegoTemp["Dbx_sdl_usescancodes"];
		DatosDosBox["Dbx_dosbox_language"]       = ""+DatosJuegoTemp["Dbx_dosbox_language"];
		DatosDosBox["Dbx_dosbox_machine"]        = ""+DatosJuegoTemp["Dbx_dosbox_machine"];
		DatosDosBox["Dbx_dosbox_captures"]       = ""+DatosJuegoTemp["Dbx_dosbox_captures"];
		DatosDosBox["Dbx_dosbox_memsize"]        = ""+DatosJuegoTemp["Dbx_dosbox_memsize"];
		DatosDosBox["Dbx_render_frameskip"]      = ""+DatosJuegoTemp["Dbx_render_frameskip"];
		DatosDosBox["Dbx_render_aspect"]         = ""+DatosJuegoTemp["Dbx_render_aspect"];
		DatosDosBox["Dbx_render_scaler"]         = ""+DatosJuegoTemp["Dbx_render_scaler"];
		DatosDosBox["Dbx_cpu_core"]              = ""+DatosJuegoTemp["Dbx_cpu_core"];
		DatosDosBox["Dbx_cpu_cputype"]           = ""+DatosJuegoTemp["Dbx_cpu_cputype"];
		DatosDosBox["Dbx_cpu_cycles"]            = ""+DatosJuegoTemp["Dbx_cpu_cycles"];
		DatosDosBox["Dbx_cpu_cycles_realmode"]   = ""+DatosJuegoTemp["Dbx_cpu_cycles_realmode"];
		DatosDosBox["Dbx_cpu_cycles_protmode"]   = ""+DatosJuegoTemp["Dbx_cpu_cycles_protmode"];
		DatosDosBox["Dbx_cpu_cycles_limitmode"]  = ""+DatosJuegoTemp["Dbx_cpu_cycles"];
		DatosDosBox["Dbx_cpu_cycleup"]           = ""+DatosJuegoTemp["Dbx_cpu_cycleup"];
		DatosDosBox["Dbx_cpu_cycledown"]         = ""+DatosJuegoTemp["Dbx_cpu_cycledown"];
		DatosDosBox["Dbx_mixer_nosound"]         = ""+DatosJuegoTemp["Dbx_mixer_nosound"];
		DatosDosBox["Dbx_mixer_rate"]            = ""+DatosJuegoTemp["Dbx_mixer_rate"];
		DatosDosBox["Dbx_mixer_blocksize"]       = ""+DatosJuegoTemp["Dbx_mixer_blocksize"];
		DatosDosBox["Dbx_mixer_prebuffer"]       = ""+DatosJuegoTemp["Dbx_mixer_prebuffer"];
		DatosDosBox["Dbx_midi_mpu401"]           = ""+DatosJuegoTemp["Dbx_midi_mpu401"];
		DatosDosBox["Dbx_midi_intelligent"]      = ""+DatosJuegoTemp["Dbx_midi_intelligent"];
		DatosDosBox["Dbx_midi_device"]           = ""+DatosJuegoTemp["Dbx_midi_device"];
		DatosDosBox["Dbx_midi_config"]           = ""+DatosJuegoTemp["Dbx_midi_config"];
		DatosDosBox["Dbx_midi_mt32rate"]         = ""+DatosJuegoTemp["Dbx_midi_mt32rate"];
		DatosDosBox["Dbx_sblaster_sbtype"]       = ""+DatosJuegoTemp["Dbx_sblaster_sbtype"];
		DatosDosBox["Dbx_sblaster_sbbase"]       = ""+DatosJuegoTemp["Dbx_sblaster_sbbase"];
		DatosDosBox["Dbx_sblaster_irq"]          = ""+DatosJuegoTemp["Dbx_sblaster_irq"];
		DatosDosBox["Dbx_sblaster_dma"]          = ""+DatosJuegoTemp["Dbx_sblaster_dma"];
		DatosDosBox["Dbx_sblaster_hdma"]         = ""+DatosJuegoTemp["Dbx_sblaster_hdma"];
		DatosDosBox["Dbx_sblaster_mixer"]        = ""+DatosJuegoTemp["Dbx_sblaster_mixer"];
		DatosDosBox["Dbx_sblaster_oplmode"]      = ""+DatosJuegoTemp["Dbx_sblaster_oplmode"];
		DatosDosBox["Dbx_sblaster_oplemu"]       = ""+DatosJuegoTemp["Dbx_sblaster_oplemu"];
		DatosDosBox["Dbx_sblaster_oplrate"]      = ""+DatosJuegoTemp["Dbx_sblaster_oplrate"];
		DatosDosBox["Dbx_gus_gus"]               = ""+DatosJuegoTemp["Dbx_gus_gus"];
		DatosDosBox["Dbx_gus_gusrate"]           = ""+DatosJuegoTemp["Dbx_gus_gusrate"];
		DatosDosBox["Dbx_gus_gusbase"]           = ""+DatosJuegoTemp["Dbx_gus_gusbase"];
		DatosDosBox["Dbx_gus_irq1"]              = ""+DatosJuegoTemp["Dbx_gus_irq1"];
		DatosDosBox["Dbx_gus_irq2"]              = ""+DatosJuegoTemp["Dbx_gus_irq2"];
		DatosDosBox["Dbx_gus_dma1"]              = ""+DatosJuegoTemp["Dbx_gus_dma1"];
		DatosDosBox["Dbx_gus_dma2"]              = ""+DatosJuegoTemp["Dbx_gus_dma2"];
		DatosDosBox["Dbx_gus_ultradir"]          = ""+DatosJuegoTemp["Dbx_gus_ultradir"];
		DatosDosBox["Dbx_speaker_pcspeaker"]     = ""+DatosJuegoTemp["Dbx_speaker_pcspeaker"];
		DatosDosBox["Dbx_speaker_pcrate"]        = ""+DatosJuegoTemp["Dbx_speaker_pcrate"];
		DatosDosBox["Dbx_speaker_tandy"]         = ""+DatosJuegoTemp["Dbx_speaker_tandy"];
		DatosDosBox["Dbx_speaker_tandyrate"]     = ""+DatosJuegoTemp["Dbx_speaker_tandyrate"];
		DatosDosBox["Dbx_speaker_disney"]        = ""+DatosJuegoTemp["Dbx_speaker_disney"];
		DatosDosBox["Dbx_joystick_type"]         = ""+DatosJuegoTemp["Dbx_joystick_type"];
		DatosDosBox["Dbx_joystick_timed"]        = ""+DatosJuegoTemp["Dbx_joystick_timed"];
		DatosDosBox["Dbx_joystick_autofire"]     = ""+DatosJuegoTemp["Dbx_joystick_autofire"];
		DatosDosBox["Dbx_joystick_swap34"]       = ""+DatosJuegoTemp["Dbx_joystick_swap34"];
		DatosDosBox["Dbx_joystick_buttonwrap"]   = ""+DatosJuegoTemp["Dbx_joystick_buttonwrap"];
		DatosDosBox["Dbx_modem_modem"]           = ""+DatosJuegoTemp["Dbx_modem_modem"];
		DatosDosBox["Dbx_modem_comport"]         = ""+DatosJuegoTemp["Dbx_modem_comport"];
		DatosDosBox["Dbx_modem_listenport"]      = ""+DatosJuegoTemp["Dbx_modem_listenport"];
		DatosDosBox["Dbx_dserial_directserial"]  = ""+DatosJuegoTemp["Dbx_dserial_directserial"];
		DatosDosBox["Dbx_dserial_comport"]       = ""+DatosJuegoTemp["Dbx_dserial_comport"];
		DatosDosBox["Dbx_dserial_realport"]      = ""+DatosJuegoTemp["Dbx_dserial_realport"];
		DatosDosBox["Dbx_dserial_defaultbps"]    = ""+DatosJuegoTemp["Dbx_dserial_defaultbps"];
		DatosDosBox["Dbx_dserial_parity"]        = ""+DatosJuegoTemp["Dbx_dserial_parity"];
		DatosDosBox["Dbx_dserial_bytesize"]      = ""+DatosJuegoTemp["Dbx_dserial_bytesize"];
		DatosDosBox["Dbx_dserial_stopbit"]       = ""+DatosJuegoTemp["Dbx_dserial_stopbit"];
		DatosDosBox["Dbx_serial_1"]              = ""+DatosJuegoTemp["Dbx_serial_1"];
		DatosDosBox["Dbx_serial_2"]              = ""+DatosJuegoTemp["Dbx_serial_2"];
		DatosDosBox["Dbx_serial_3"]              = ""+DatosJuegoTemp["Dbx_serial_3"];
		DatosDosBox["Dbx_serial_4"]              = ""+DatosJuegoTemp["Dbx_serial_4"];
		DatosDosBox["Dbx_dos_xms"]               = ""+DatosJuegoTemp["Dbx_dos_xms"];
		DatosDosBox["Dbx_dos_ems"]               = ""+DatosJuegoTemp["Dbx_dos_ems"];
		DatosDosBox["Dbx_dos_umb"]               = ""+DatosJuegoTemp["Dbx_dos_umb"];
		DatosDosBox["Dbx_dos_keyboardlayout"]    = ""+DatosJuegoTemp["Dbx_dos_keyboardlayout"];
		DatosDosBox["Dbx_ipx_ipx"]               = ""+DatosJuegoTemp["Dbx_ipx_ipx"];
		DatosDosBox["Dbx_autoexec"]              = ""+DatosJuegoTemp["Dbx_autoexec"];
		DatosDosBox["Dbx_opt_autoexec"]          = ""+DatosJuegoTemp["Dbx_opt_autoexec"];
		DatosDosBox["Dbx_opt_loadfix"]           = ""+DatosJuegoTemp["Dbx_opt_loadfix"];
		DatosDosBox["Dbx_opt_loadfix_mem"]       = ""+DatosJuegoTemp["Dbx_opt_loadfix_mem"];
		DatosDosBox["Dbx_opt_consola_dbox"]      = ""+DatosJuegoTemp["Dbx_opt_consola_dbox"];
		DatosDosBox["Dbx_opt_cerrar_dbox"]       = ""+DatosJuegoTemp["Dbx_opt_cerrar_dbox"];
		DatosDosBox["Dbx_opt_cycle_sincronizar"] = ""+DatosJuegoTemp["Dbx_opt_cycle_sincronizar"];
		DatosDosBox["Dbx_path_conf"]             = ""+DatosJuegoTemp["Dbx_path_conf"];
		DatosDosBox["Dbx_path_sonido"]           = ""+DatosJuegoTemp["Dbx_path_sonido"];
		DatosDosBox["Dbx_path_exe"]              = ""+DatosJuegoTemp["Dbx_path_exe"];
		DatosDosBox["Dbx_path_setup"]            = ""+DatosJuegoTemp["Dbx_path_setup"];
		DatosDosBox["Dbx_parametros_exe"]        = ""+DatosJuegoTemp["Dbx_parametros_exe"];
		DatosDosBox["Dbx_parametros_setup"]      = ""+DatosJuegoTemp["Dbx_parametros_setup"];

		if( tipoDFend )
		{
			QStringList str_Lista;
			QString tipoDrive, temp_opt_mount, IOCtrl_mount;
			int i = 0, num_mounts = 0;

			num_mounts = fGrl.StrToInt( DatosJuegoTemp["NrOfMounts"] );
			if( num_mounts > 0 )
			{
				for( i = 0; i < num_mounts ; i++)
				{
					QTreeWidgetItem *twItemDfend = new QTreeWidgetItem(ui.twMontajes);

					str_Lista.clear();
					str_Lista = DatosJuegoTemp[fGrl.IntToStr(i)].split("|");

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
	if( DatosJuego["Dat_tipo_emu"] == "scummvm" )
	{
		DatosScummVM.clear();
		DatosScummVM["Svm_game"]            = ""+DatosJuegoTemp["Svm_game"];
		DatosScummVM["Svm_game_label"]      = ""+DatosJuegoTemp["Svm_game_label"];
		DatosScummVM["Svm_language"]        = ""+DatosJuegoTemp["Svm_language"];
		DatosScummVM["Svm_subtitles"]       = ""+DatosJuegoTemp["Svm_subtitles"];
		DatosScummVM["Svm_platform"]        = ""+DatosJuegoTemp["Svm_platform"];
		DatosScummVM["Svm_gfx_mode"]        = ""+DatosJuegoTemp["Svm_gfx_mode"];
		DatosScummVM["Svm_render_mode"]     = ""+DatosJuegoTemp["Svm_render_mode"];
		DatosScummVM["Svm_fullscreen"]      = ""+DatosJuegoTemp["Svm_fullscreen"];
		DatosScummVM["Svm_aspect_ratio"]    = ""+DatosJuegoTemp["Svm_aspect_ratio"];
		DatosScummVM["Svm_path"]            = ""+DatosJuegoTemp["Svm_path"];
		DatosScummVM["Svm_path_setup"]      = ""+DatosJuegoTemp["Svm_path_setup"];
		DatosScummVM["Svm_path_extra"]      = ""+DatosJuegoTemp["Svm_path_extra"];
		DatosScummVM["Svm_path_save"]       = ""+DatosJuegoTemp["Svm_path_save"];
		DatosScummVM["Svm_path_capturas"]   = ""+DatosJuegoTemp["Svm_path_capturas"];
		DatosScummVM["Svm_path_sonido"]     = ""+DatosJuegoTemp["Svm_path_sonido"];
		DatosScummVM["Svm_music_driver"]    = ""+DatosJuegoTemp["Svm_music_driver"];
		DatosScummVM["Svm_enable_gs"]       = ""+DatosJuegoTemp["Svm_enable_gs"];
		DatosScummVM["Svm_multi_midi"]      = ""+DatosJuegoTemp["Svm_multi_midi"];
		DatosScummVM["Svm_native_mt32"]     = ""+DatosJuegoTemp["Svm_native_mt32"];
		DatosScummVM["Svm_master_volume"]   = ""+DatosJuegoTemp["Svm_master_volume"];
		DatosScummVM["Svm_music_volume"]    = ""+DatosJuegoTemp["Svm_music_volume"];
		DatosScummVM["Svm_sfx_volume"]      = ""+DatosJuegoTemp["Svm_sfx_volume"];
		DatosScummVM["Svm_speech_volume"]   = ""+DatosJuegoTemp["Svm_speech_volume"];
		DatosScummVM["Svm_tempo"]           = ""+DatosJuegoTemp["Svm_tempo"];
		DatosScummVM["Svm_talkspeed"]       = ""+DatosJuegoTemp["Svm_talkspeed"];
		DatosScummVM["Svm_debuglevel"]      = ""+DatosJuegoTemp["Svm_debuglevel"];
		DatosScummVM["Svm_cdrom"]           = ""+DatosJuegoTemp["Svm_cdrom"];
		DatosScummVM["Svm_joystick_num"]    = ""+DatosJuegoTemp["Svm_joystick_num"];
		DatosScummVM["Svm_output_rate"]     = ""+DatosJuegoTemp["Svm_output_rate"];
		DatosScummVM["Svm_midi_gain"]       = ""+DatosJuegoTemp["Svm_midi_gain"];
		DatosScummVM["Svm_copy_protection"] = ""+DatosJuegoTemp["Svm_copy_protection"];
		DatosScummVM["Svm_sound_font"]      = ""+DatosJuegoTemp["Svm_sound_font"];
		DatosScummVM["Svm_walkspeed"]       = ""+DatosJuegoTemp["Svm_walkspeed"];
		DatosScummVM["Svm_opl_driver"]      = ""+DatosJuegoTemp["Svm_opl_driver"];
	}
// VDMSound ------------------
//----------------------------

	indx_fin_descarga = CargarThumb;
	isRequestFinished();
}
