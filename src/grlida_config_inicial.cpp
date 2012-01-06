/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2012 Pedro A. Garcia Rosado Aka Monthy
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

#include "grlida_config_inicial.h"

frmConfigInicial::frmConfigInicial(QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);

	stHomeDir = fGrl.GRlidaHomePath();
	stTheme   = fGrl.ThemeGrl();

	createConnections();

	CargarConfig();
	setTheme();

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmConfigInicial::~frmConfigInicial()
{
	//
}

void frmConfigInicial::closeEvent( QCloseEvent *e )
{
	on_setLanguage( ui.cbxIdioma->currentIndex() );

	GuardarConfig();

	e->accept();
}

void  frmConfigInicial::createConnections()
{
	connect( ui.btnOk    , SIGNAL( clicked() ), this, SLOT( on_btnOk()     ) );
	connect( ui.btnDirDbx, SIGNAL( clicked() ), this, SLOT( on_btnDirDbx() ) );
	connect( ui.btnDirSvm, SIGNAL( clicked() ), this, SLOT( on_btnDirSvm() ) );
	connect( ui.cbxIdioma, SIGNAL(activated(int)), this, SLOT( on_setLanguage(int) ) );
}

void frmConfigInicial::CargarConfig()
{
	GRLConfig = fGrl.CargarGRLConfig( stHomeDir + "GR-lida.conf" );

	ui.chkDOSBoxDisp->setChecked( GRLConfig["DOSBoxDisp"].toBool() );
	ui.btnDirDbx->setEnabled( GRLConfig["DOSBoxDisp"].toBool() );
	ui.txtDirDbx->setEnabled( GRLConfig["DOSBoxDisp"].toBool() );
	ui.txtDirDbx->setText( GRLConfig["DirDOSBox"].toString() );
	ui.chkScummVMDisp->setChecked( GRLConfig["ScummVMDisp"].toBool() );
	ui.btnDirSvm->setEnabled( GRLConfig["ScummVMDisp"].toBool() );
	ui.txtDirSvm->setEnabled( GRLConfig["ScummVMDisp"].toBool() );
	ui.txtDirSvm->setText( GRLConfig["DirScummVM"].toString() );
	ui.chkVDMSoundDisp->setChecked( GRLConfig["VDMSoundDisp"].toBool() );
	stIdiomaSelect = GRLConfig["IdiomaSelect"].toString();

	if( GRLConfig["IdiomaExterno"].toBool() )
		fGrl.CargarIdiomasCombo(stHomeDir + "idiomas/", ui.cbxIdioma);
	else
		fGrl.CargarIdiomasCombo(":/idiomas/", ui.cbxIdioma);

	ui.cbxIdioma->setCurrentIndex( ui.cbxIdioma->findData( stIdiomaSelect ) );
	on_setLanguage( ui.cbxIdioma->currentIndex() );

	#ifdef Q_OS_WIN32
		ui.chkVDMSoundDisp->setEnabled(true);
	#else
		ui.chkVDMSoundDisp->setEnabled(false);
	#endif
}

void frmConfigInicial::GuardarConfig()
{
	GRLConfig["DOSBoxDisp"]   = ui.chkDOSBoxDisp->isChecked();
	GRLConfig["ScummVMDisp"]  = ui.chkScummVMDisp->isChecked();
	GRLConfig["VDMSoundDisp"] = ui.chkVDMSoundDisp->isChecked();
	GRLConfig["Primeravez"]   = ui.chkShowNext->isChecked();
	GRLConfig["DirDOSBox"]    = ui.txtDirDbx->text();
	GRLConfig["DirScummVM"]   = ui.txtDirSvm->text();
	GRLConfig["IdiomaSelect"] = stIdiomaSelect;

	fGrl.GuardarGRLConfig( stHomeDir + "GR-lida.conf", GRLConfig );
}

void frmConfigInicial::setTheme()
{
	setStyleSheet( fGrl.StyleSheet() );

	ui.btnOk->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.btnDirDbx->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDirSvm->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );

	if( GRLConfig["font_usar"].toBool() )
		setStyleSheet(fGrl.StyleSheet()+"*{font-family:\""+GRLConfig["font_family"].toString()+"\";font-size:"+GRLConfig["font_size"].toString()+"pt;}");
}

void frmConfigInicial::on_setLanguage(int idx_locale)
{
	if(idx_locale >= 0)
		stIdiomaSelect = ui.cbxIdioma->itemData(idx_locale, Qt::UserRole).toString();
	else
		stIdiomaSelect = "es_ES";

	if( GRLConfig["IdiomaExterno"].toBool() )
		translator.load(stHomeDir + "idiomas/gr-lida_" + stIdiomaSelect + ".qm");
	else
		translator.load(":/idiomas/gr-lida_" + stIdiomaSelect + ".qm");

	qApp->installTranslator(&translator);
	ui.retranslateUi(this);
}

void frmConfigInicial::on_btnOk()
{
	on_setLanguage( ui.cbxIdioma->currentIndex() );

	GuardarConfig();

	QDialog::accept();
}

void frmConfigInicial::on_btnDirDbx()
{
	ui.txtDirDbx->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona el Ejecutable del DOSBox"), GRLConfig["DirDbx"].toString(), ui.txtDirDbx->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtDirDbx->text() );
	if( fi.exists() )
		GRLConfig["DirDbx"] = fi.absolutePath();
	else
		GRLConfig["DirDbx"] = "";
}

void frmConfigInicial::on_btnDirSvm()
{
	ui.txtDirSvm->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona el Ejecutable del ScummVM"), GRLConfig["DirSvm"].toString(), ui.txtDirSvm->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtDirSvm->text() );
	if( fi.exists() )
		GRLConfig["DirSvm"] = fi.absolutePath();
	else
		GRLConfig["DirSvm"] = "";
}
