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
	connect( ui.cbxIdioma , SIGNAL( activated(const QString &) ), this, SLOT( on_setLanguage(const QString &) ) );
    connect( ui.cmbStyle  , SIGNAL(activated(const QString &)), this, SLOT( on_changeStyle(const QString &)));
    connect( ui.cbxMotorDataBase, SIGNAL(activated(const QString &)), this, SLOT( on_changeTypeDB(const QString &)));

	stHomeDir = QDir::homePath()+"/.gr-lida/";	// directorio de trabajo del GR-lida
//	stHomeDir = QDir::currentPath()+"/";		// directorio de trabajo del GR-lida
//	stHomeDir = "./";							// directorio de trabajo del GR-lida
	
	ui.cmbStyle->clear();
	ui.cmbStyle->addItem( "Default" );
	ui.cmbStyle->addItems( QStyleFactory::keys() );

	QSettings settings( stHomeDir+"GR-lida.conf", QSettings::IniFormat ); 
	settings.beginGroup("OpcGeneral");
		ui.chkConfini_1->setChecked(settings.value("DOSBoxDisp"    , "false").toBool() );
		ui.btnDirDbx->setEnabled( settings.value("DOSBoxDisp"      , "false").toBool() );
		ui.txtDirDbx->setEnabled( settings.value("DOSBoxDisp"      , "false").toBool() );
		ui.txtDirDbx->setText( settings.value("DirDOSBox"       , "" ).toString()      );
		ui.chkConfini_2->setChecked(settings.value("ScummVMDisp"   , "false").toBool() );
		ui.btnDirSvm->setEnabled( settings.value("ScummVMDisp"     , "false").toBool() );
		ui.txtDirSvm->setEnabled( settings.value("ScummVMDisp"     , "false").toBool() );
		ui.txtDirSvm->setText( settings.value("DirScummVM"      , "" ).toString()      );
		ui.chkConfini_3->setChecked( settings.value("VDMSoundDisp" , "false").toBool() );
		ui.chkConfini_4->setChecked( settings.value("Primeravez"   , "true").toBool()  );
		IdiomaSelect  = settings.value("IdiomaSelect", "es_ES" ).toString();
		stStyleSelect = settings.value("Style", "Default" ).toString();
		url_xmldb = settings.value("url_xmldb", "http://freegamedatabase.110mb.com/" ).toString(); // GR-lida xmlDB
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

//	ui.txt_dbpassword->setEchoMode(QLineEdit::Password);

	#ifdef Q_OS_WIN32
		ui.chkConfini_3->setEnabled(true);
	#else
		ui.chkConfini_3->setEnabled(false);
	#endif

	ui.cbxMotorDataBase->addItem( QIcon( ":/img16/basedatos.png" ), "QSQLITE"	);
	ui.cbxMotorDataBase->addItem( QIcon( ":/img16/basedatos.png" ), "QMYSQL"	);
	ui.cbxMotorDataBase->addItem( QIcon( ":/img16/basedatos.png" ), "QPSQL"		);
	ui.cbxMotorDataBase->setCurrentIndex( ui.cbxMotorDataBase->findText( stdb_type, Qt::MatchContains) ); //

	ui.cmbStyle->setCurrentIndex( ui.cmbStyle->findText(stStyleSelect, Qt::MatchContains) ); //
	
	fGrl.CargarIdiomasCombo( stHomeDir + "idiomas/", ui.cbxIdioma );
	ui.cbxIdioma->setCurrentIndex( ui.cbxIdioma->findText(IdiomaSelect, Qt::MatchContains) ); //
	on_setLanguage( ui.cbxIdioma->currentText() );

	fGrl.CargarDatosComboBox(stHomeDir + "datos/xmldb.txt", ui.cbxDbXml, 1, false);
	ui.cbxDbXml->addItem( QIcon(":/img16/edit_enlace.png"), url_xmldb);
	ui.cbxDbXml->setCurrentIndex( ui.cbxDbXml->findText(url_xmldb, Qt::MatchContains) ); //

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmOpciones::~frmOpciones(){}

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

void frmOpciones::on_btnOk(){

	QSettings settings( stHomeDir+"GR-lida.conf", QSettings::IniFormat ); 

	DatosConfiguracion.clear();
	if(ui.chkConfini_1->isChecked())
	  DatosConfiguracion["DOSBoxDisp"] = "true"; else DatosConfiguracion["DOSBoxDisp"] = "false"; // DOSBox
	if(ui.chkConfini_2->isChecked())
	  DatosConfiguracion["ScummVMDisp"] = "true"; else DatosConfiguracion["ScummVMDisp"] = "false"; // ScummVM
	if(ui.chkConfini_3->isChecked())
	  DatosConfiguracion["VDMSoundDisp"] = "true"; else DatosConfiguracion["VDMSoundDisp"] = "false"; // vdmsound
	if(ui.chkConfini_4->isChecked())
	  DatosConfiguracion["Primeravez"] = "true"; else DatosConfiguracion["Primeravez"] = "false"; // primera vez
	if(ui.chkStylePalette->isChecked())
	  DatosConfiguracion["StylePalette"] = "true"; else DatosConfiguracion["StylePalette"] = "false"; // StylePalette

	DatosConfiguracion["DirDOSBox"]  = ui.txtDirDbx->text() ; // DirDbx
	DatosConfiguracion["DirScummVM"] = ui.txtDirSvm->text() ; // DirSvm

	if(ui.cbxDbXml->currentText()!="")
	  url_xmldb = ui.cbxDbXml->currentText(); else  url_xmldb = "";
	if(ui.cbxMotorDataBase->currentText()!="")
	  stdb_type = ui.cbxMotorDataBase->currentText(); else stdb_type = ""; // SqlDatabase

	settings.beginGroup("OpcGeneral");
		settings.setValue("DOSBoxDisp"   , DatosConfiguracion["DOSBoxDisp"]		);
		settings.setValue("ScummVMDisp"  , DatosConfiguracion["ScummVMDisp"]	);
		settings.setValue("VDMSoundDisp" , DatosConfiguracion["VDMSoundDisp"]	);
		settings.setValue("Primeravez"   , DatosConfiguracion["Primeravez"]		);
		settings.setValue("DirDOSBox"    , DatosConfiguracion["DirDOSBox"]		);
		settings.setValue("DirScummVM"   , DatosConfiguracion["DirScummVM"] 	);
		settings.setValue("IdiomaSelect" , IdiomaSelect          				);
		settings.setValue("Style"        , stStyleSelect         				);
	    settings.setValue("StylePalette" , DatosConfiguracion["StylePalette"]	);
		settings.setValue("url_xmldb"    , url_xmldb							);
	settings.endGroup();

	settings.beginGroup("SqlDatabase");
		settings.setValue("db_type"      , stdb_type							);
		settings.setValue("db_host"      , ui.txtDirBD->text()					);
		settings.setValue("db_name"      , ui.txt_dbname->text()				);
		settings.setValue("db_username"  , ui.txt_dbusername->text()			);
		settings.setValue("db_password"  , ui.txt_dbpassword->text()			);
		settings.setValue("db_port"		 , ui.txt_dbport->text()				);
	settings.endGroup();

	QDialog::accept();
}

void frmOpciones::on_setLanguage(const QString txt_locale)
{
	QStringList parts = txt_locale.split(" - ");
	QTranslator translator;
	translator.load( stHomeDir + "idiomas/gr-lida_" + parts.value(1)+".qm" );
	qApp->installTranslator(&translator);

	IdiomaSelect = parts.value(1);
	ui.retranslateUi(this);
}

void frmOpciones::on_btnDirDbx()
{
	ui.txtDirDbx->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtDirDbx->text(), tr("Todos los archivo (*)"), 0, false) );
}

void frmOpciones::on_btnDirSvm()
{
	ui.txtDirSvm->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtDirSvm->text(), tr("Todos los archivo (*)"), 0, false) );
}

void frmOpciones::on_btnDirDB()
{
	ui.txtDirBD->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtDirBD->text(), tr("Todos los archivo (*)"), 0, false) );
}
