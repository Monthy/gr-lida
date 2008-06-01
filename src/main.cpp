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

#include <QApplication>
#include <QSplashScreen>
#include <QMessageBox>
#include <QProcess>
#include <QtCore>
#include <QtGui>
#include <QTranslator>

#include "grlida.h"
#include "grlida_config_inicial.h"

int main(int argc, char *argv[])
{
	QTranslator translator;
	QString stHomeDir, stDirPath, stIdiomaSelect, Version_GRL, stStyleSelect;
	bool ejecutado_por_primeravez , chk_StylePalette, chk_IdiomaExterno;
	Funciones fGrl;

	QApplication app(argc, argv);

	//stHomeDir = QDir::homePath()+"/.gr-lida/";
	stHomeDir = fGrl.GRlidaHomePath();	// Indicamos el directorio del usuario

	QSplashScreen splash( QPixmap(":/images/splash.png") );
	splash.setFont( QFont("Helvetica", 10) );
	splash.show();
	
	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Creando ventana principal"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
	
	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta principal"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();

// Crear directorio de la aplicacion si no existe
	QDir appDir;
	if(!appDir.exists(stHomeDir)) appDir.mkdir( stHomeDir );

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando la configuración Inicial"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();

// Comprueba y sino crea la configuracion por defecto
	QFile appConfg(stHomeDir+"GR-lida.conf");
	if( !appConfg.exists() )
	{
		QSettings settings( stHomeDir+"GR-lida.conf", QSettings::IniFormat ); 
		settings.beginGroup("OpcGeneral");
			settings.setValue("DOSBoxDisp"   , "false");
			settings.setValue("ScummVMDisp"  , "false");
			settings.setValue("VDMSoundDisp" , "false");
			settings.setValue("Primeravez"   , "true");
			settings.setValue("IdiomaSelect" , "es_ES");
			settings.setValue("IdiomaExterno", "false");
			settings.setValue("url_xmldb"    , "");		
			settings.setValue("Style"        , "Default");
			settings.setValue("NameDirTheme" , "defecto");
			settings.setValue("StylePalette" , "false");
			settings.setValue("IconoFav"     , "fav_0.png");
		settings.endGroup();
		settings.beginGroup("OpcVer");
			settings.setValue("Pnl_Datos"    , "true" );
			settings.setValue("Pnl_Capturas" , "false");
			settings.setValue("Pnl_FilesUrl" , "false");
			settings.setValue("Pnl_Ordenar"  , "false");
		settings.endGroup();
		settings.beginGroup("SqlDatabase");
			settings.setValue("db_type"		, "QSQLITE" ); // QSQLITE, QMYSQL, QPSQL
			settings.setValue("db_host"		, stHomeDir+"db_grl.grl"); // localhost, archivo
			settings.setValue("db_name"		, ""	); // Nombre base de datos
			settings.setValue("db_username"	, ""	); // Nombre del Usuario
			settings.setValue("db_password"	, ""	); // Password
			settings.setValue("db_port"		, "3306"); // Puerto
			settings.setValue("db_Orden_ColTabla", "titulo");
			settings.setValue("db_Orden_By"      , "titulo");
			settings.setValue("db_Orden"         , "ASC"   );			
		settings.endGroup();
		ejecutado_por_primeravez = true;
		chk_StylePalette = false;
		stIdiomaSelect = "es_ES" ;
		chk_IdiomaExterno = false;
		Version_GRL = "";
	} else {
		QSettings settings( stHomeDir+"GR-lida.conf", QSettings::IniFormat ); 
		settings.beginGroup("OpcGeneral");
			ejecutado_por_primeravez = settings.value("Primeravez", "true").toBool();
			stIdiomaSelect   = settings.value("IdiomaSelect", "es_ES").toString();
			chk_IdiomaExterno= settings.value("IdiomaExterno" , "false").toBool();
		    stStyleSelect    = settings.value("Style", "Default").toString();
		    chk_StylePalette = settings.value("StylePalette", "false").toBool();
		settings.endGroup();
		settings.beginGroup("Updates");
			Version_GRL = settings.value("Version", "").toString();
		settings.endGroup();	
	}
	if( stStyleSelect!="" || stStyleSelect!="Default" )
		QApplication::setStyle( stStyleSelect );

	if( chk_StylePalette )
		QApplication::setPalette(QApplication::style()->standardPalette());

	if( ejecutado_por_primeravez )
	{
		frmConfigInicial * ConfigInicial = new frmConfigInicial();
		if( ConfigInicial->exec() == QDialog::Accepted )
			stIdiomaSelect = ConfigInicial->IdiomaSelect; 
	}
	
	if(chk_IdiomaExterno)
		translator.load( stHomeDir + "idiomas/gr-lida_" + stIdiomaSelect + ".qm" );
	else
		translator.load( ":/idiomas/gr-lida_" + stIdiomaSelect + ".qm" );

	app.installTranslator(&translator);	
	
	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Idiomas"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio idiomas si no existe
	QDir langDir;
	if(!langDir.exists(stHomeDir+"idiomas")) langDir.mkdir( stHomeDir+"idiomas" );

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Iconos"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio Themes si no existe
	QDir themesDir;
	if(!themesDir.exists(stHomeDir+"themes")) themesDir.mkdir( stHomeDir+"themes" );

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Themes"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio iconos si no existe
	QDir iconDir;
	if(!iconDir.exists(stHomeDir+"iconos")) iconDir.mkdir(stHomeDir+"iconos");

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Smiles"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio smiles si no existe
	QDir smilesDir;
	if(!smilesDir.exists(stHomeDir+"smiles")) smilesDir.mkdir(stHomeDir+"smiles");

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Datos"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio datos si no existe
	QDir datosDir;
	if(!datosDir.exists(stHomeDir+"datos")) datosDir.mkdir(stHomeDir+"datos");

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Templates"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio templates si no existe
	QDir templatesDir;
	if(!templatesDir.exists(stHomeDir+"templates")) templatesDir.mkdir(stHomeDir+"templates");

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Configuración del DOSBox"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio confDbx si no existe
	QDir confDbxDir;
	if(!confDbxDir.exists(stHomeDir + "confdbx")) confDbxDir.mkdir(stHomeDir + "confdbx");

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Roms"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();

#ifdef Q_OS_WIN32
	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Configuración del VdmSound"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio confVdms si no existe
	QDir confVdmsDir;
	if(!confVdmsDir.exists(stHomeDir+"confvdms")) confVdmsDir.mkdir(stHomeDir+"confvdms");
#endif

// Crear directorio confVdms si no existe
	QDir thumbsDir;
	if(!thumbsDir.exists(stHomeDir+"thumbs")) thumbsDir.mkdir(stHomeDir+"thumbs");

// Crear directorio confVdms si no existe
	QDir coversDir;
	if(!coversDir.exists(stHomeDir+"covers")) coversDir.mkdir(stHomeDir+"covers");

// Crear directorio temp para las descargas temporales de imagenes, archivos
	QDir tempDir;
	if(!tempDir.exists(stHomeDir+"temp")) coversDir.mkdir(stHomeDir+"temp");
	
	GrLida w;

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Cargando Configuración..."), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
	w.CargarConfigInicial();

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando archivos de Datos..."), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
	w.ComprobarArchivosDatos( Version_GRL );

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Cargando Base de Datos..."), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
	w.CargarBaseDatos();

	w.show();
	splash.finish(&w);

	app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
	return app.exec();
}
