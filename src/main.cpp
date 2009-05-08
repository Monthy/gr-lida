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
#include <QTranslator>
#include <QSplashScreen>

#include "grlida.h"
#include "grlida_config_inicial.h"

int main(int argc, char *argv[])
{
	Funciones fGrl;
	QTranslator translator;
	QHash<QString, QVariant> GRLConfig;
	QString stHomeDir, stIdiomaSelect;

	QApplication app(argc, argv);

	stHomeDir = fGrl.GRlidaHomePath();

	GRLConfig = fGrl.CargarGRLConfig( stHomeDir + "GR-lida.conf" );

	stIdiomaSelect = GRLConfig["IdiomaSelect"].toString();

	if( GRLConfig["Style"].toString() != "" || GRLConfig["Style"].toString() != "Default" )
		QApplication::setStyle( GRLConfig["Style"].toString() );

	if( GRLConfig["StylePalette"].toBool() )
		QApplication::setPalette(QApplication::style()->standardPalette());

	if( GRLConfig["Primeravez"].toBool() )
	{
		frmConfigInicial *ConfigInicial = new frmConfigInicial();
		if( ConfigInicial->exec() == QDialog::Accepted )
			stIdiomaSelect = ConfigInicial->stIdiomaSelect;
		delete ConfigInicial;
	}

	if( GRLConfig["IdiomaExterno"].toBool() )
		translator.load(stHomeDir + "idiomas/gr-lida_" + stIdiomaSelect + ".qm");
	else
		translator.load(":/idiomas/gr-lida_" + stIdiomaSelect + ".qm");

	app.installTranslator(&translator);

// Creamos el logo.
	QSplashScreen splash( QPixmap(":/images/splash.png") );
	splash.setFont( QFont("Helvetica", 10) );
	splash.show();

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Cargando Configuración..."), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Creando ventana Principal"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando la Configuración Inicial"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();

// Crear directorio de la aplicacion si no existe
	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Principal"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
	fGrl.ComprobarDirectorio( stHomeDir );

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Idiomas"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio idiomas si no existe
	fGrl.ComprobarDirectorio( stHomeDir + "idiomas" );

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Iconos"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio Themes si no existe
	fGrl.ComprobarDirectorio( stHomeDir + "themes" );

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Themes"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio iconos si no existe
	fGrl.ComprobarDirectorio( stHomeDir + "iconos" );

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Smiles"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio smiles si no existe
	fGrl.ComprobarDirectorio( stHomeDir + "smiles" );

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Datos"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio datos si no existe
	fGrl.ComprobarDirectorio( stHomeDir + "datos" );

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Scripts"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio scripts si no existe
	fGrl.ComprobarDirectorio( stHomeDir + "scripts" );

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Templates"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio templates si no existe
	fGrl.ComprobarDirectorio( stHomeDir + "templates" );

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Configuración del DOSBox"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio confDbx si no existe
	fGrl.ComprobarDirectorio( stHomeDir + "confdbx" );

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Roms"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
//	fGrl.ComprobarDirectorio(  );

#ifdef Q_OS_WIN32
	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Configuración del VdmSound"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio confVdms si no existe
	fGrl.ComprobarDirectorio( stHomeDir + "confvdms" );
#endif

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Thumbs"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio Thumbs si no existe
	fGrl.ComprobarDirectorio( stHomeDir + "thumbs" );

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Covers"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio Covers si no existe
	fGrl.ComprobarDirectorio( stHomeDir + "covers" );

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando carpeta Temp"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio Temp para las descargas temporales de imagenes, archivos
	fGrl.ComprobarDirectorio( stHomeDir + "temp" );

	GrLida w;

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Cargando Configuración de la Base de Datos..."), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
	w.ConectarBaseDatos();

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Comprobando archivos de Datos..."), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
	w.ComprobarArchivosDatos( GRLConfig["Version"].toString() );

	splash.showMessage(QObject::tr("Iniciando:")+" "+QObject::tr("Cargando Base de Datos..."), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
	w.CargarListaJuegosDB();

	w.show();
	splash.finish(&w);

	app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
	return app.exec();
}
