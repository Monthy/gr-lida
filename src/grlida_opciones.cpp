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
#include "grlida_opciones.h"

frmOpciones::frmOpciones(QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);

	stHomeDir = fGrl.GRlidaHomePath();	// directorio de trabajo del GR-lida
	stTheme   = fGrl.ThemeGrl();

	createConnections();

	CargarConfig();

	setTheme();

	CargarListaThemes();

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmOpciones::~frmOpciones()
{
	//
}

void frmOpciones::closeEvent( QCloseEvent *e )
{
	GuardarConfig();

	e->accept();
}

void frmOpciones::createConnections()
{
	connect( ui.btnOk           , SIGNAL( clicked() ), this, SLOT( on_btnOk()     ) );
	connect( ui.btnDirDbx       , SIGNAL( clicked() ), this, SLOT( on_btnDirDbx() ) );
	connect( ui.btnDirSvm       , SIGNAL( clicked() ), this, SLOT( on_btnDirSvm() ) );
	connect( ui.btnDirDB        , SIGNAL( clicked() ), this, SLOT( on_btnDirDB()  ) );
	connect( ui.btnDirBaseGames , SIGNAL( clicked() ), this, SLOT( on_btnDirBaseGames()  ) );
	connect( ui.cbxIdioma       , SIGNAL( activated(int) ), this, SLOT( on_setLanguage(int) ) );
	connect( ui.cbxStyle        , SIGNAL( activated(const QString &)), this, SLOT( on_changeStyle(const QString &)));
	connect( ui.cbxMotorDataBase, SIGNAL( activated(int)), this, SLOT( on_changeTypeDB(int)));
	connect( ui.twThemes        , SIGNAL( itemClicked( QTreeWidgetItem*, int)), this, SLOT( on_twThemes_clicked(QTreeWidgetItem*)));
	connect( ui.btnOpenUrl      , SIGNAL( clicked() ), this, SLOT( on_btnOpenUrl() ) );
	connect( ui.btnInfoFormatoFecha, SIGNAL( clicked() ), this, SLOT( on_InfoFormatoFecha() ) );
}

void frmOpciones::CargarConfig()
{
	GRLConfig = fGrl.CargarGRLConfig( stHomeDir + "GR-lida.conf" );

// Idioma
	stIdiomaSelect = GRLConfig["IdiomaSelect"].toString();
	ui.chkIdiomaExterno->setChecked( GRLConfig["IdiomaExterno"].toBool() );
// DOSBox
	ui.chkDOSBoxDisp->setChecked( GRLConfig["DOSBoxDisp"].toBool() );
	ui.btnDirDbx->setEnabled( GRLConfig["DOSBoxDisp"].toBool() );
	ui.txtDirDbx->setEnabled( GRLConfig["DOSBoxDisp"].toBool() );
	ui.txtDirDbx->setText( GRLConfig["DirDOSBox"].toString() );
// ScummVM
	ui.chkScummVMDisp->setChecked( GRLConfig["ScummVMDisp"].toBool() );
	ui.btnDirSvm->setEnabled( GRLConfig["ScummVMDisp"].toBool() );
	ui.txtDirSvm->setEnabled( GRLConfig["ScummVMDisp"].toBool() );
	ui.txtDirSvm->setText( GRLConfig["DirScummVM"].toString() );
// VDMSound
	ui.chkVDMSoundDisp->setChecked( GRLConfig["VDMSoundDisp"].toBool() );
// Otras Opciones
	ui.txtDirBaseGames->setText( GRLConfig["DirBaseGames"].toString() );
	url_xmldb = GRLConfig["url_xmldb"].toString();
	ui.chkShowNext->setChecked( GRLConfig["Primeravez"].toBool() );
	ui.txtFormatoFecha->setText( GRLConfig["FormatoFecha"].toString() );
// Base de Datos
	stdb_type = GRLConfig["db_type"].toString();
	ui.txt_dbserver->setText( GRLConfig["db_server"].toString() );
	ui.txtDirBD->setText( GRLConfig["db_host"].toString() );
	ui.txt_dbname->setText( GRLConfig["db_name"].toString() );
	ui.txt_dbusername->setText( GRLConfig["db_username"].toString() );
	ui.txt_dbpassword->setText( GRLConfig["db_password"].toString() );
	ui.txt_dbport->setText( GRLConfig["db_port"].toString() );
// Estilos-Themes
	stStyleSelect = GRLConfig["Style"].toString();
	ui.chkStylePalette->setChecked( GRLConfig["StylePalette"].toBool() );
	stIconoFav = GRLConfig["IconoFav"].toString();
	stPicFlowReflection = GRLConfig["PicFlowReflection"].toString();
	ui.spinBox_SkipPicFlow->setValue( GRLConfig["Skip_PicFlow"].toInt() );
	stNameDirTheme = GRLConfig["NameDirTheme"].toString();

	#ifdef Q_OS_WIN32
		ui.chkVDMSoundDisp->setEnabled(true);
	#else
		ui.chkVDMSoundDisp->setEnabled(false);
	#endif

	ui.cbxStyle->clear();
	ui.cbxStyle->addItem( "Default" );
	ui.cbxStyle->addItems( QStyleFactory::keys() );
	ui.cbxStyle->setCurrentIndex( ui.cbxStyle->findText(stStyleSelect, Qt::MatchContains) );

	ui.cbxMotorDataBase->clear();
	ui.cbxMotorDataBase->addItem( QIcon(stTheme+"img16/basedatos.png"), tr("Base de Datos")+" SQLite"    , "QSQLITE");
	ui.cbxMotorDataBase->addItem( QIcon(stTheme+"img16/basedatos.png"), tr("Base de Datos")+" MySQL"     , "QMYSQL" );
	ui.cbxMotorDataBase->addItem( QIcon(stTheme+"img16/basedatos.png"), tr("Base de Datos")+" PostgreSQL", "QPSQL"  );
	ui.cbxMotorDataBase->setCurrentIndex( ui.cbxMotorDataBase->findData( stdb_type ) );

	ui.cbxIconFav->clear();
	ui.cbxIconFav->addItem( QIcon(stTheme+"img16/fav_0.png"), tr("Favorito")+": 0" ,"fav_0.png");
	ui.cbxIconFav->addItem( QIcon(stTheme+"img16/fav_1.png"), tr("Favorito")+": 1" ,"fav_1.png");
	ui.cbxIconFav->addItem( QIcon(stTheme+"img16/fav_2.png"), tr("Favorito")+": 2" ,"fav_2.png");
	ui.cbxIconFav->addItem( QIcon(stTheme+"img16/fav_3.png"), tr("Favorito")+": 3" ,"fav_3.png");
	ui.cbxIconFav->setCurrentIndex( ui.cbxIconFav->findData(stIconoFav, Qt::UserRole, Qt::MatchContains) );

	ui.cbxPicFlowReflection->clear();
	ui.cbxPicFlowReflection->addItem( QIcon(stTheme+"img16/capturas.png"), "BlurredReflection");
	ui.cbxPicFlowReflection->addItem( QIcon(stTheme+"img16/capturas.png"), "PlainReflection");
	ui.cbxPicFlowReflection->setCurrentIndex( ui.cbxPicFlowReflection->findText(stPicFlowReflection, Qt::MatchContains) );

	if( ui.chkIdiomaExterno->isChecked() )
		fGrl.CargarIdiomasCombo( stHomeDir + "idiomas/", ui.cbxIdioma );
	else
		fGrl.CargarIdiomasCombo(":/idiomas/", ui.cbxIdioma);

	ui.cbxIdioma->setCurrentIndex( ui.cbxIdioma->findData( stIdiomaSelect ) );
	on_setLanguage( ui.cbxIdioma->currentIndex() );

	fGrl.CargarScriptsComboBox(stHomeDir+"scripts/", ui.cbxScriptURL);

	int rowScript = ui.cbxScriptURL->findData( url_xmldb );
	if( rowScript < 0 ) rowScript = 1;

	ui.cbxScriptURL->setCurrentIndex( rowScript );

// Servidor Proxy
	ui.cbxTypeProxy->clear();
	ui.cbxTypeProxy->addItem( QIcon(stTheme+"img16/edit_enlace.png"), tr("No usar proxy"), "2");
	ui.cbxTypeProxy->addItem( QIcon(stTheme+"img16/edit_enlace.png"), tr("El proxy se determina sobre la base de la aplicación"), "0");
	ui.cbxTypeProxy->addItem( QIcon(stTheme+"img16/edit_enlace.png"), tr("Socks5 proxy se utilizara")           , "1");
	ui.cbxTypeProxy->addItem( QIcon(stTheme+"img16/edit_enlace.png"), tr("HTTP se utiliza proxy transparente")  , "3");
	ui.cbxTypeProxy->addItem( QIcon(stTheme+"img16/edit_enlace.png"), tr("HTTP proxy sólo para las solicitudes"), "4");
	ui.cbxTypeProxy->setCurrentIndex( 2 );

	ui.chkEnableProxy->setChecked( GRLConfig["ProxyEnable"].toBool() );
	ui.cbxTypeProxy->setCurrentIndex( ui.cbxTypeProxy->findData( GRLConfig["ProxyType"].toInt() ) );
	ui.txt_px_host->setText( GRLConfig["ProxyHost"].toString() );
	ui.txt_px_puerto->setText( GRLConfig["ProxyPort"].toString() );
	ui.chkLogin->setChecked( GRLConfig["ProxyLogin"].toBool() );
	ui.txt_px_username->setText( GRLConfig["ProxyUserName"].toString() );
	ui.txt_px_password->setText( GRLConfig["ProxyPassword"].toString() );

}

void frmOpciones::GuardarConfig()
{
// SqlDatabase
	GRLConfig["db_type"]           = stdb_type;
	GRLConfig["db_server"]         = ui.txt_dbserver->text();
	GRLConfig["db_host"]           = ui.txtDirBD->text();
	GRLConfig["db_name"]           = ui.txt_dbname->text();
	GRLConfig["db_username"]       = ui.txt_dbusername->text();
	GRLConfig["db_password"]       = ui.txt_dbpassword->text();
	GRLConfig["db_port"]           = ui.txt_dbport->text();
// OpcGeneral
	GRLConfig["Primeravez"]        = ui.chkShowNext->isChecked();
	GRLConfig["DirDOSBox"]         = ui.txtDirDbx->text();
	GRLConfig["DirScummVM"]        = ui.txtDirSvm->text();
	GRLConfig["DOSBoxDisp"]        = ui.chkDOSBoxDisp->isChecked();
	GRLConfig["ScummVMDisp"]       = ui.chkScummVMDisp->isChecked();
	GRLConfig["VDMSoundDisp"]      = ui.chkVDMSoundDisp->isChecked();
	GRLConfig["IdiomaSelect"]      = stIdiomaSelect;
	GRLConfig["IdiomaExterno"]     = ui.chkIdiomaExterno->isChecked();
	GRLConfig["DirBaseGames"]      = ui.txtDirBaseGames->text();
	GRLConfig["url_xmldb"]         = ui.cbxScriptURL->itemData( ui.cbxScriptURL->currentIndex() ).toString();
	GRLConfig["FormatoFecha"]      = ui.txtFormatoFecha->text();
	GRLConfig["Style"]             = stStyleSelect;
	GRLConfig["StylePalette"]      = ui.chkStylePalette->isChecked();
	GRLConfig["NameDirTheme"]      = stNameDirTheme;
	GRLConfig["IconoFav"]          = ui.cbxIconFav->itemData(ui.cbxIconFav->currentIndex(), Qt::UserRole).toString();
	GRLConfig["PicFlowReflection"] = ui.cbxPicFlowReflection->currentText();
	GRLConfig["Skip_PicFlow"]      = ui.spinBox_SkipPicFlow->value();
	GRLConfig["stTheme"]           = stTheme;
// Servidor Proxy
	GRLConfig["ProxyEnable"]       = ui.chkEnableProxy->isChecked();
	GRLConfig["ProxyType"]         = ui.cbxTypeProxy->itemData(ui.cbxTypeProxy->currentIndex(), Qt::UserRole).toInt();
	GRLConfig["ProxyHost"]         = ui.txt_px_host->text();
	GRLConfig["ProxyPort"]         = ui.txt_px_puerto->text();
	GRLConfig["ProxyLogin"]        = ui.chkLogin->isChecked();
	GRLConfig["ProxyUserName"]     = ui.txt_px_username->text();
	GRLConfig["ProxyPassword"]     = ui.txt_px_password->text();

	DatosConfiguracion = GRLConfig;

	fGrl.GuardarGRLConfig( stHomeDir + "GR-lida.conf", GRLConfig );
}

void frmOpciones::setTheme()
{
	setStyleSheet( fGrl.StyleSheet() );
	setWindowIcon( QIcon(stTheme+"img16/barra_herramintas.png") );

	ui.btnOk->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.btnCancel->setIcon( QIcon(stTheme+"img16/cancelar.png") );
	ui.btnDirDbx->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDirSvm->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDirDB->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDirBaseGames->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDirDbx_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDirSvm_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDirDB_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDirBaseGames_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnInfoFormatoFecha->setIcon( QIcon(stTheme+"img16/informacion.png") );

	ui.tabWidget_Config->setTabIcon(0, QIcon(stTheme+"img16/opciones.png") );
	ui.tabWidget_Config->setTabIcon(1, QIcon(stTheme+"img16/basedatos.png") );
	ui.tabWidget_Config->setTabIcon(2, QIcon(stTheme+"img16/style.png") );
	ui.tabWidget_Config->setTabIcon(3, QIcon(stTheme+"img16/edit_enlace.png") );

	ui.cbxMotorDataBase->setItemIcon(0, QIcon(stTheme+"img16/basedatos.png") );
	ui.cbxMotorDataBase->setItemIcon(1, QIcon(stTheme+"img16/basedatos.png") );
	ui.cbxMotorDataBase->setItemIcon(2, QIcon(stTheme+"img16/basedatos.png") );

	ui.cbxIconFav->setItemIcon(0, QIcon(stTheme+"img16/fav_0.png") );
	ui.cbxIconFav->setItemIcon(1, QIcon(stTheme+"img16/fav_1.png") );
	ui.cbxIconFav->setItemIcon(2, QIcon(stTheme+"img16/fav_2.png") );
	ui.cbxIconFav->setItemIcon(3, QIcon(stTheme+"img16/fav_3.png") );

	ui.cbxPicFlowReflection->setItemIcon(0, QIcon(stTheme+"img16/capturas.png") );
	ui.cbxPicFlowReflection->setItemIcon(1, QIcon(stTheme+"img16/capturas.png") );
}

void frmOpciones::CargarListaThemes()
{
	QString stAutor, stVersion;
	QDir dir( stHomeDir+"/themes/" );
	QStringList themes_Lista = dir.entryList(QDir::NoDotAndDotDot | QDir::AllDirs );

	ui.twThemes->clear();
	QTreeWidgetItem *item_themes = new QTreeWidgetItem( ui.twThemes );
	item_themes->setText( 0 , "defecto" );
	item_themes->setIcon( 0 , QIcon(stTheme+"img16/style.png") );
	item_themes->setText( 1 , "-" );
	item_themes->setText( 2 , "-" );
	item_themes->setSelected(true);
	ui.lb_theme_example->setPixmap( QPixmap(stTheme+"images/juego_sin_imagen.png") );

	setUpdatesEnabled( false );
	for ( int i = 0; i < themes_Lista.size(); i++ )
	{
		if( dir.exists(stHomeDir+"/themes/"+themes_Lista[i]) )
		{
			QSettings settings( stHomeDir+"/themes/"+themes_Lista[i]+"/info.ini", QSettings::IniFormat );
			settings.beginGroup("info");
				stAutor = settings.value("autor", "-").toString();
				stVersion = settings.value("version", "-").toString();
			settings.endGroup();

			QTreeWidgetItem *item_themes = new QTreeWidgetItem( ui.twThemes );
			item_themes->setText( 0 , themes_Lista[i] );
			item_themes->setIcon( 0 , QIcon(stTheme+"img16/style.png") );
			item_themes->setText( 1 , stAutor );
			item_themes->setText( 2 , stVersion );

			if( themes_Lista[i] == stNameDirTheme)
			{
				ui.twThemes->clearSelection();
				ui.twThemes->setCurrentItem( item_themes );
				on_twThemes_clicked( item_themes );
			}
		}
	}
	setUpdatesEnabled( true );
}

void frmOpciones::on_btnOk()
{
	GuardarConfig();

	QDialog::accept();
}

void frmOpciones::on_btnDirDbx()
{
	ui.txtDirDbx->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona el Ejecutable del DOSBox"), GRLConfig["DirDbx"].toString(), ui.txtDirDbx->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtDirDbx->text() );
	if( fi.exists() )
		GRLConfig["DirDbx"] = fi.absolutePath();
	else
		GRLConfig["DirDbx"] = "";
}

void frmOpciones::on_btnDirSvm()
{
	ui.txtDirSvm->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona el Ejecutable del ScummVM"), GRLConfig["DirSvm"].toString(), ui.txtDirSvm->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtDirSvm->text() );
	if( fi.exists() )
		GRLConfig["DirSvm"] = fi.absolutePath();
	else
		GRLConfig["DirSvm"] = "";
}

void frmOpciones::on_btnDirDB()
{
	ui.txtDirBD->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["DirBD"].toString(), ui.txtDirBD->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtDirBD->text() );
	if( fi.exists() )
		GRLConfig["DirBD"] = fi.absolutePath();
	else
		GRLConfig["DirBD"] = "";
}

void frmOpciones::on_btnDirBaseGames()
{
	ui.txtDirBaseGames->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["DirBaseGames"].toString(), ui.txtDirBaseGames->text() ) );
	GRLConfig["DirBaseGames"] = ui.txtDirBaseGames->text();
}

void frmOpciones::on_setLanguage(int idx_locale)
{
	QTranslator translator;

	if(idx_locale >= 0)
		stIdiomaSelect = ui.cbxIdioma->itemData(idx_locale, Qt::UserRole).toString();
	else
		stIdiomaSelect = "es_ES";

	if( ui.chkIdiomaExterno->isChecked() )
		translator.load(stHomeDir + "idiomas/gr-lida_" + stIdiomaSelect + ".qm");
	else
		translator.load(":/idiomas/gr-lida_" + stIdiomaSelect + ".qm");

	qApp->installTranslator(&translator);
	ui.retranslateUi(this);
}

void frmOpciones::on_changeStyle(const QString &styleName)
{
	stStyleSelect = styleName;
	QApplication::setStyle(QStyleFactory::create(styleName));
	changePalette();
}

void frmOpciones::changePalette()
{
	if (ui.chkStylePalette->isChecked())
		QApplication::setPalette(QApplication::style()->standardPalette());
	else
		QApplication::setPalette(originalPalette);
}

void frmOpciones::on_changeTypeDB(int idx_typedb)
{
	if(idx_typedb >= 0)
		stdb_type = ui.cbxMotorDataBase->itemData(idx_typedb, Qt::UserRole).toString();
	else
		stdb_type = "QSQLITE";

	if( stdb_type == "QSQLITE" )
		ui.btnDirDB->setEnabled(true);
	else
		ui.btnDirDB->setEnabled(false);
}

void frmOpciones::on_twThemes_clicked( QTreeWidgetItem *item )
{
	if( item )
	{
		stNameDirTheme = item->text(0);
		if(stNameDirTheme == "defecto" || stNameDirTheme.isEmpty()  )
		{
			stTheme.clear();
			stTheme = ":/";
			ui.lb_theme_example->setPixmap( QPixmap(stTheme+"images/juego_sin_imagen.png") );
		}else {
			stTheme.clear();
			stTheme = stHomeDir+"themes/"+ stNameDirTheme+"/" ;
			ui.lb_theme_example->setPixmap( QPixmap(stHomeDir+"/themes/"+item->text(0)+"/preview.png") );
		}
		setTheme();
	} else {
		ui.lb_theme_example->setPixmap( QPixmap(stTheme+"images/juego_sin_imagen.png") );
		return;
	}
}

void frmOpciones::on_twThemes_currentItemChanged( QTreeWidgetItem *item1, QTreeWidgetItem *item2)
{
	if( (item1)&&(item2) )
		emit on_twThemes_clicked( ui.twThemes->currentItem() );
	else
		return;
}

void frmOpciones::on_btnOpenUrl()
{
// Cargamos el script
	QScriptEngine engine;
	QFile scriptFile( ui.cbxScriptURL->itemData( ui.cbxScriptURL->currentIndex() ).toString() );
	scriptFile.open(QIODevice::ReadOnly | QIODevice::Text);
	engine.evaluate( scriptFile.readAll() );
	scriptFile.close();

// Abre la URL con el navegador por defecto
	if(engine.evaluate("url_site").isValid())
		QDesktopServices::openUrl( engine.evaluate("url_site").toString() );
}

void frmOpciones::on_InfoFormatoFecha()
{
	fGrl.VentanaInfo( tr("Información sobre el formato de la fecha y hora"),
		stTheme+"img16/informacion.png", 500, 400,
		tr("Devuelve una cadena con formato de acuerdo a la cadena de formato dada.") + "<br>" +
		tr("Lista básica de parámetros:") + "<br>"
		"<table border=\"0\" align=\"center\" cellspacing=\"1\" cellpadding=\"2\">"
		"  <thead>"
		"    <tr>"
		"      <td bgcolor=\"#ecce64\"><p align=\"center\">"+tr("Expresión")+"</p></td>"
		"      <td bgcolor=\"#ecce64\"><p align=\"center\">"+tr("Salida")+"</p></td>"
		"    </tr>"
		"  </thead>"
		"  <tr>"
		"    <td bgcolor=\"#f0f0f0\"><p>d</p></td>"
		"    <td bgcolor=\"#f0f0f0\"><p>"+tr("Día del mes sin ceros iniciales (1 a 31).")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#e4e4e4\"><p>dd</p></td>"
		"    <td bgcolor=\"#e4e4e4\"><p>"+tr("Día del mes, 2 dígitos con ceros iniciales (01 a 31).")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#f0f0f0\"><p>ddd</p></td>"
		"    <td bgcolor=\"#f0f0f0\"><p>"+tr("Una representación textual de un día, tres letras (e.j. 'Lun' a 'Dom').")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#e4e4e4\"><p>dddd</p></td>"
		"    <td bgcolor=\"#e4e4e4\"><p>"+tr("Una representación textual completa del día de la semana (e.j. 'Lunes' a 'Domingo').")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#f0f0f0\"><p>M</p></td>"
		"    <td bgcolor=\"#f0f0f0\"><p>"+tr("Representación numérica de un mes, sin ceros iniciales (1-12).")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#e4e4e4\"><p>MM</p></td>"
		"    <td bgcolor=\"#e4e4e4\"><p>"+tr("Representación numérica de un mes, con ceros iniciales (01-12).")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#f0f0f0\"><p>MMM</p></td>"
		"    <td bgcolor=\"#f0f0f0\"><p>"+tr("Una representación textual corta de un mes, tres letras (e.j. 'Ene' a 'Dic').")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#e4e4e4\"><p>MMMM</p></td>"
		"    <td bgcolor=\"#e4e4e4\"><p>"+tr("Una representación textual completa de un mes, como Enero o Marzo (e.j. 'Enero' a 'Diciembre').")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#f0f0f0\"><p>yy</p></td>"
		"    <td bgcolor=\"#f0f0f0\"><p>"+tr("Una representación de dos dígitos de un año (00-99).")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#e4e4e4\"><p>yyyy</p></td>"
		"    <td bgcolor=\"#e4e4e4\"><p>"+tr("Una representación numérica completa de un año, 4 dígitos.")+"</p></td>"
		"  </tr>"
		"</table>"
		"<br><br>"+tr("Estas expresiones pueden ser utilizadas para el tiempo.")+"<br>"
		"<table border=\"0\" align=\"center\" cellspacing=\"1\" cellpadding=\"2\">"
		"  <thead>"
		"    <tr>"
		"      <td bgcolor=\"#ecce64\"><p align=\"center\">"+tr("Expresión")+"</p></td>"
		"      <td bgcolor=\"#ecce64\"><p align=\"center\">"+tr("Salida")+"</p></td>"
		"    </tr>"
		"  </thead>"
		"  <tr>"
		"    <td bgcolor=\"#f0f0f0\"><p>h</p></td>"
		"    <td bgcolor=\"#f0f0f0\"><p>"+tr("La hora sin ceros iniciales (0 a 23 o 1 a 12 si AM/PM display)")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#e4e4e4\"><p>hh</p></td>"
		"    <td bgcolor=\"#e4e4e4\"><p>"+tr("La hora con ceros iniciales (00 a 23 o de 01 a 12 si AM/PM display)")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#f0f0f0\"><p>H</p></td>"
		"    <td bgcolor=\"#f0f0f0\"><p>"+tr("La hora sin ceros iniciales (0 a 23, incluso con AM/PM display)")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#e4e4e4\"><p>HH</p></td>"
		"    <td bgcolor=\"#e4e4e4\"><p>"+tr("La hora con un cero (00 a 23, incluso con AM/PM display)")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#f0f0f0\"><p>m</p></td>"
		"    <td bgcolor=\"#f0f0f0\"><p>"+tr("Los minutos sin ceros iniciales (0 a 59)")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#e4e4e4\"><p>mm</p></td>"
		"    <td bgcolor=\"#e4e4e4\"><p>"+tr("Los minutos con ceros iniciales (00 a 59)")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#f0f0f0\"><p>s</p></td>"
		"    <td bgcolor=\"#f0f0f0\"><p>"+tr("Los segundos sin ceros iniciales (0 a 59)")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#e4e4e4\"><p>ss</p></td>"
		"    <td bgcolor=\"#e4e4e4\"><p>"+tr("Los segundos con ceros iniciales (00 a 59)")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#f0f0f0\"><p>z</p></td>"
		"    <td bgcolor=\"#f0f0f0\"><p>"+tr("Los milisegundos sin ceros iniciales (0 a 999)")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#e4e4e4\"><p>zzz</p></td>"
		"    <td bgcolor=\"#e4e4e4\"><p>"+tr("Los milisegundos con ceros iniciales (000 a 999)")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#f0f0f0\"><p>AP or A</p></td>"
		"    <td bgcolor=\"#f0f0f0\"><p>"+tr("Interpretar como un AM/PM del tiempo. AP debe ser \"AM\" o \"PM\".")+"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td bgcolor=\"#e4e4e4\"><p>ap or a</p></td>"
		"    <td bgcolor=\"#e4e4e4\"><p>"+tr("Interpretar como un AM/PM del tiempo. AP debe ser \"am\" o \"pm\".")+"</p></td>"
		"  </tr>"
		"</table>"
	);
}
