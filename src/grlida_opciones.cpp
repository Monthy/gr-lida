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
	originalPalette = QApplication::palette();

	ui.setupUi(this);

	connect( ui.btnOk     , SIGNAL( clicked() ), this, SLOT( on_btnOk()     ) );
	connect( ui.btnDirDbx , SIGNAL( clicked() ), this, SLOT( on_btnDirDbx() ) );
	connect( ui.btnDirSvm , SIGNAL( clicked() ), this, SLOT( on_btnDirSvm() ) );
	connect( ui.btnDirDB  , SIGNAL( clicked() ), this, SLOT( on_btnDirDB()  ) );
	connect( ui.btnDirBaseGames  , SIGNAL( clicked() ), this, SLOT( on_btnDirBaseGames()  ) );
	connect( ui.cbxIdioma , SIGNAL( activated(const QString &) ), this, SLOT( on_setLanguage(const QString &) ) );
	connect( ui.cmbStyle  , SIGNAL(activated(const QString &)), this, SLOT( on_changeStyle(const QString &)));
	connect( ui.cbxMotorDataBase, SIGNAL(activated(const QString &)), this, SLOT( on_changeTypeDB(const QString &)));
	connect( ui.twThemes        , SIGNAL( itemClicked( QTreeWidgetItem*, int)), this, SLOT( on_twThemes_clicked(QTreeWidgetItem*)));

	stHomeDir = fGrl.GRlidaHomePath();	// directorio de trabajo del GR-lida

	stTheme = fGrl.ThemeGrl();
	setTheme();

	ui.cmbStyle->clear();
	ui.cmbStyle->addItem( "Default" );
	ui.cmbStyle->addItems( QStyleFactory::keys() );

	QSettings settings( stHomeDir+"GR-lida.conf", QSettings::IniFormat ); 
	settings.beginGroup("OpcGeneral");
		ui.chkConfig_DOSBoxDisp->setChecked(settings.value("DOSBoxDisp", "false").toBool() );
		ui.btnDirDbx->setEnabled( settings.value("DOSBoxDisp", "false").toBool() );
		ui.txtDirDbx->setEnabled( settings.value("DOSBoxDisp", "false").toBool() );
		ui.txtDirDbx->setText( settings.value("DirDOSBox", "").toString() );
		ui.chkConfig_ScummVMDisp->setChecked(settings.value("ScummVMDisp", "false").toBool() );
		ui.btnDirSvm->setEnabled( settings.value("ScummVMDisp", "false").toBool() );
		ui.txtDirSvm->setEnabled( settings.value("ScummVMDisp", "false").toBool() );
		ui.txtDirSvm->setText( settings.value("DirScummVM", "").toString() );
		ui.txtDirBaseGames->setText( settings.value("DirBaseGames", "").toString() );
		ui.chkConfig_VDMSoundDisp->setChecked( settings.value("VDMSoundDisp", "false").toBool() );
		ui.chkConfig_ShowNext->setChecked( settings.value("Primeravez", "true").toBool() );
		IdiomaSelect = settings.value("IdiomaSelect", "es_ES").toString();
		ui.chkConfig_IdiomaExterno->setChecked( settings.value("IdiomaExterno" , "false").toBool() );
		stIconoFav    = settings.value("IconoFav", "fav_0.png").toString();
		stStyleSelect = settings.value("Style", "Default").toString();
		stNameDirTheme= settings.value("NameDirTheme", "defecto").toString();
		url_xmldb     = settings.value("url_xmldb", "").toString(); // GR-lida xmlDB
		ui.chkStylePalette->setChecked( settings.value("StylePalette","false").toBool());
	settings.endGroup();
	settings.beginGroup("SqlDatabase");
		stdb_type	= settings.value("db_type", "QSQLITE" ).toString();
		ui.txtDirBD->setText( settings.value("db_host", stHomeDir + "db_grl.grl" ).toString() );
		ui.txt_dbname->setText( settings.value("db_name", "" ).toString() );
		ui.txt_dbusername->setText( settings.value("db_username", "" ).toString() );
		ui.txt_dbpassword->setText( settings.value("db_password", "" ).toString() );
		ui.txt_dbport->setText( settings.value("db_port", "3306" ).toString() );
	settings.endGroup();
	UltimoPath.clear();
	settings.beginGroup("UltimoDirectorio");
		UltimoPath["DirDbx"]       = settings.value("DirDbx", "").toString();
		UltimoPath["DirSvm"]       = settings.value("DirSvm", "").toString();
		UltimoPath["DirBaseGames"] = settings.value("DirBaseGames", "").toString();
		UltimoPath["DirBD"]        = settings.value("DirBD", "").toString();
	settings.endGroup();

	CargarListaThemes();
//	ui.txt_dbpassword->setEchoMode(QLineEdit::Password);

	#ifdef Q_OS_WIN32
		ui.chkConfig_VDMSoundDisp->setEnabled(true);
	#else
		ui.chkConfig_VDMSoundDisp->setEnabled(false);
	#endif

	ui.cbxMotorDataBase->addItem( QIcon( stTheme+"img16/basedatos.png" ), "QSQLITE"	);
	ui.cbxMotorDataBase->addItem( QIcon( stTheme+"img16/basedatos.png" ), "QMYSQL"	);
	ui.cbxMotorDataBase->addItem( QIcon( stTheme+"img16/basedatos.png" ), "QPSQL"		);
	ui.cbxMotorDataBase->setCurrentIndex( ui.cbxMotorDataBase->findText( stdb_type, Qt::MatchContains) ); //

	ui.cmbStyle->setCurrentIndex( ui.cmbStyle->findText(stStyleSelect, Qt::MatchContains) ); //

	ui.cmbIconFav->addItem(QIcon(stTheme+"img16/fav_0.png"), "fav_0.png");
	ui.cmbIconFav->addItem(QIcon(stTheme+"img16/fav_1.png"), "fav_1.png");
	ui.cmbIconFav->addItem(QIcon(stTheme+"img16/fav_2.png"), "fav_2.png");
	ui.cmbIconFav->addItem(QIcon(stTheme+"img16/fav_3.png"), "fav_3.png");
	ui.cmbIconFav->setCurrentIndex( ui.cmbIconFav->findText(stIconoFav, Qt::MatchContains) ); //

	if(ui.chkConfig_IdiomaExterno->isChecked())
		fGrl.CargarIdiomasCombo( stHomeDir + "idiomas/", ui.cbxIdioma );	
	else
		fGrl.CargarIdiomasCombo( ":/idiomas/", ui.cbxIdioma );

	ui.cbxIdioma->setCurrentIndex( ui.cbxIdioma->findText(IdiomaSelect, Qt::MatchContains) ); //
	on_setLanguage( ui.cbxIdioma->currentText() );

	fGrl.CargarDatosComboBox(stHomeDir + "datos/xmldb.txt", ui.cbxDbXml, 1, false);
	ui.cbxDbXml->addItem( QIcon(stTheme+"img16/edit_enlace.png"), url_xmldb);
	ui.cbxDbXml->setCurrentIndex( ui.cbxDbXml->findText(url_xmldb, Qt::MatchContains) ); //

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmOpciones::~frmOpciones(){}

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
				emit on_twThemes_clicked( item_themes );
			}
		}
	}
}

void frmOpciones::setTheme()
{
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
	ui.tabWidget_Config->setTabIcon(0, QIcon(stTheme+"img16/opciones.png") );
	ui.tabWidget_Config->setTabIcon(1, QIcon(stTheme+"img16/basedatos.png") );
	ui.tabWidget_Config->setTabIcon(2, QIcon(stTheme+"img16/style.png") );
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
			stTheme = stHomeDir+"themes/"+ stNameDirTheme +"/";
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

void frmOpciones::on_changeTypeDB(const QString &typedb)
{
	if( typedb == "QSQLITE" )
		ui.btnDirDB->setEnabled(true);
	else
		ui.btnDirDB->setEnabled(false);
}

void frmOpciones::on_btnOk()
{
	QSettings settings( stHomeDir + "GR-lida.conf", QSettings::IniFormat ); 

	DatosConfiguracion.clear();
	if(ui.chkConfig_DOSBoxDisp->isChecked())
		DatosConfiguracion["DOSBoxDisp"]   = "true"; else DatosConfiguracion["DOSBoxDisp"]   = "false"; // DOSBox
	if(ui.chkConfig_ScummVMDisp->isChecked())
		DatosConfiguracion["ScummVMDisp"]  = "true"; else DatosConfiguracion["ScummVMDisp"]  = "false"; // ScummVM
	if(ui.chkConfig_VDMSoundDisp->isChecked())
		DatosConfiguracion["VDMSoundDisp"] = "true"; else DatosConfiguracion["VDMSoundDisp"] = "false"; // VDMSound
	if(ui.chkConfig_ShowNext->isChecked())
		DatosConfiguracion["Primeravez"]   = "true"; else DatosConfiguracion["Primeravez"]   = "false"; // primera vez
	if(ui.chkConfig_IdiomaExterno->isChecked())
		DatosConfiguracion["IdiomaExterno"]= "true"; else DatosConfiguracion["IdiomaExterno"] = "false"; // primera vez
	if(ui.chkStylePalette->isChecked())
		DatosConfiguracion["StylePalette"] = "true"; else DatosConfiguracion["StylePalette"] = "false"; // StylePalette

	DatosConfiguracion["DirDOSBox"]  = ui.txtDirDbx->text() ; // DirDbx
	DatosConfiguracion["DirScummVM"] = ui.txtDirSvm->text() ; // DirSvm

	if(ui.cbxDbXml->currentText()!="")
		url_xmldb = ui.cbxDbXml->currentText(); else url_xmldb = "";
	if(ui.cbxMotorDataBase->currentText()!="")
		stdb_type = ui.cbxMotorDataBase->currentText(); else stdb_type = ""; // SqlDatabase

	if(ui.cmbIconFav->currentText()!="")
		stIconoFav = ui.cmbIconFav->currentText(); else stIconoFav = "fav_0.png";

	if( stNameDirTheme == "defecto" || stNameDirTheme.isEmpty() )
		DatosConfiguracion["NameDirTheme"] = ":/";
	else		
		DatosConfiguracion["NameDirTheme"] = stHomeDir+"themes/"+ stNameDirTheme +"/";

	settings.beginGroup("OpcGeneral");
		settings.setValue("DOSBoxDisp"   , DatosConfiguracion["DOSBoxDisp"]		);
		settings.setValue("ScummVMDisp"  , DatosConfiguracion["ScummVMDisp"]	);
		settings.setValue("VDMSoundDisp" , DatosConfiguracion["VDMSoundDisp"]	);
		settings.setValue("Primeravez"   , DatosConfiguracion["Primeravez"]		);
		settings.setValue("DirDOSBox"    , DatosConfiguracion["DirDOSBox"]		);
		settings.setValue("DirScummVM"   , DatosConfiguracion["DirScummVM"]		);
		settings.setValue("DirBaseGames" , ui.txtDirBaseGames->text()			);
		settings.setValue("IdiomaSelect" , IdiomaSelect							);
		settings.setValue("IdiomaExterno", DatosConfiguracion["IdiomaExterno"]	);
		settings.setValue("Style"        , stStyleSelect						);
		settings.setValue("StylePalette" , DatosConfiguracion["StylePalette"]	);
		settings.setValue("IconoFav"     , stIconoFav							);
		settings.setValue("NameDirTheme" , stNameDirTheme                       );
		settings.setValue("url_xmldb"    , url_xmldb							);
	settings.endGroup();

	settings.beginGroup("SqlDatabase");
		settings.setValue("db_type"      , stdb_type							);
		settings.setValue("db_host"      , ui.txtDirBD->text()					);
		settings.setValue("db_name"      , ui.txt_dbname->text()				);
		settings.setValue("db_username"  , ui.txt_dbusername->text()			);
		settings.setValue("db_password"  , ui.txt_dbpassword->text()			);
		settings.setValue("db_port"      , ui.txt_dbport->text()				);
	settings.endGroup();

	QDialog::accept();
}

void frmOpciones::on_setLanguage(const QString txt_locale)
{
	QStringList parts = txt_locale.split(" - ");
	QTranslator translator;
	
	if(ui.chkConfig_IdiomaExterno->isChecked())
		translator.load( stHomeDir + "idiomas/gr-lida_" + parts.value(1)+".qm" );
	else
		translator.load(":/idiomas/gr-lida_" + parts.value(1)+".qm" );

	qApp->installTranslator(&translator);

	IdiomaSelect = parts.value(1);
	ui.retranslateUi(this);
}

void frmOpciones::on_btnDirDbx()
{
	ui.txtDirDbx->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), UltimoPath["DirDbx"], ui.txtDirDbx->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtDirDbx->text() );
	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		if( fi.exists() )
		{
			lastdir.setValue("DirDbx", fi.absolutePath()+"/" );
			UltimoPath["DirDbx"] = fi.absolutePath()+"/";
		} else {
			lastdir.setValue("DirDbx", "" );
			UltimoPath["DirDbx"] = "";	
		}
	lastdir.endGroup();
}

void frmOpciones::on_btnDirSvm()
{
	ui.txtDirSvm->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), UltimoPath["DirSvm"], ui.txtDirSvm->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtDirSvm->text() );
	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		if( fi.exists() )
		{
			lastdir.setValue("DirSvm", fi.absolutePath()+"/" );
			UltimoPath["DirSvm"] = fi.absolutePath()+"/";
		} else {
			lastdir.setValue("DirSvm", "" );
			UltimoPath["DirSvm"] = "";	
		}
	lastdir.endGroup();
}

void frmOpciones::on_btnDirDB()
{
	ui.txtDirBD->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), UltimoPath["DirBD"], ui.txtDirBD->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtDirBD->text() );
	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		if( fi.exists() )
		{
			lastdir.setValue("DirBD", fi.absolutePath()+"/" );
			UltimoPath["DirBD"] = fi.absolutePath()+"/";
		} else {
			lastdir.setValue("DirBD", "" );
			UltimoPath["DirBD"] = "";	
		}
	lastdir.endGroup();
}

void frmOpciones::on_btnDirBaseGames()
{
	ui.txtDirBaseGames->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), UltimoPath["DirBaseGames"], ui.txtDirBaseGames->text() ) );

	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		lastdir.setValue("DirBaseGames", ui.txtDirBaseGames->text()+"/" );
	lastdir.endGroup();
	UltimoPath["DirBaseGames"] = ui.txtDirBaseGames->text()+"/";
}
