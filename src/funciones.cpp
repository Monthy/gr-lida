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

#include "funciones.h"

Funciones::Funciones()
{
	stTheme = ThemeGrl();
}

Funciones::~Funciones(){}

// Muestra el Nombre de la plataforma donde esta compilada
QString Funciones::get_Plataforma()
{
	QString plataforma;
	#ifdef Q_OS_WIN32
		plataforma = "Windows";
	#endif
	#ifdef Q_OS_MAC
		plataforma = "Mac";
	#endif
	#ifdef Q_OS_BSD4
		plataforma = "BSD 4.4";
	#endif
	#ifdef Q_OS_BSDI
		plataforma = "BSD/OS";
	#endif
	#ifdef Q_OS_FREEBSD
		plataforma = "FreeBSD";
	#endif
	#ifdef Q_OS_LINUX
		plataforma = "Linux";
	#endif
	#ifdef Q_OS_LYNX
		plataforma = "LynxOS";
	#endif
	#ifdef Q_OS_MSDOS
		plataforma = "MS-DOS and Windows";
	#endif
	#ifdef Q_OS_NETBSD
		plataforma = "NetBSD";
	#endif
	#ifdef Q_OS_OS2
		plataforma = "OS/2";
	#endif
	#ifdef Q_OS_OPENBSD
		plataforma = "OpenBSD";
	#endif
	#ifdef Q_OS_SOLARIS
		plataforma = "Sun Solaris";
	#endif
//	#ifdef Q_OS_UNIX
//		plataforma = "UNIX BSD/SYSV";
//	#endif

	return plataforma;
}

// Crea una copia de seguridad de la Base de Datos
void Funciones::CrearCopiaSeguridad_DB(QString dir, QString Version_GRL)
{
	if( Version_GRL != stVersionGrl() && Version_GRL < stVersionGrl() )
	{
		if( QFile::exists(dir +"db_grl_v"+ Version_GRL +"_backup.grl") )
			QFile::copy( dir +"db_grl.grl", dir +"db_grl_v"+ Version_GRL +"_"+HoraFechaActual("ddMMyyyy_HHmmss")+"_backup.grl");
		else
			QFile::copy( dir +"db_grl.grl", dir +"db_grl_v"+ Version_GRL +"_backup.grl");
	}
}

// Convierte de Número a Texto
QString Funciones::IntToStr(int num)
{
	QString temp;
	return temp.setNum(num);
}

// Convierte de Texto a Número
int Funciones::StrToInt(QString text)
{
	return text.toInt();
}

// Convierte Texto "true", "yes", "1" a tipo Bool
bool Funciones::StrToBool(QString text)
{
	text.toLower();
	if( text == "true" || text == "yes" || text == "1" )
		return true;
	else
		return false;
}

// Convierte Bool a Texto "true", "yes" indicando el tipo
QString Funciones::BoolToStr(bool estado, bool type_yes)
{
	if( estado )
	{
		if(type_yes)
			return "yes";
		else
			return "true";
	} else {
		if(type_yes)
			return "no";
		else
			return "false";
	}
}

// Devuelve la hora y la fecha actual
QString Funciones::HoraFechaActual(QString formato)
{
	QDateTime dt = QDateTime::currentDateTime();

	if( formato.isEmpty() )
		return dt.toString("dd/MM/yyyy HH:mm:ss");
	else
		return dt.toString( formato );
}

// Devuelve una URL correcta
QString Funciones::url_correcta(QString url)
{
	if( !url.isEmpty() )
	{
		bool url_ok,  s_url_ok;

		s_url_ok = url.startsWith("https://");

		if( !s_url_ok )
		{
			url_ok = url.startsWith("http://");
			if( url_ok == false )
				url.prepend("http://");
		}

		url_ok = url.endsWith("/");
		if( url_ok == false )
			url.append("/");

		return url;
	} else
		return "";
}

// Elimina caracteres no permitidos por windows por ejemplo
QString Funciones::eliminar_caracteres(QString str)
{
	if( str.isEmpty() )
		return "";
	else {
		str.replace(" " , "_");
		str.replace("\\", "" );
		str.replace("/" , "" );
		str.replace(":" , "" );
		str.replace("*" , "" );
		str.replace("?" , "" );
		str.replace("\"", "" );
		str.replace("<" , "" );
		str.replace(">" , "" );
		str.replace("|" , "" );
		str.replace("." , "" );

		return str;
	}
}

// Obtiene la extension del archivo
QString Funciones::getExtension(QString str)
{
	QFileInfo f_info( str );
	return "."+ f_info.completeSuffix();
}

QString Funciones::getIconListaJuegos(QString icono, QString iconoDir)
{
	iconoDir = GRlidaHomePath()+"iconos/";

	if( icono.isEmpty() || icono == "" )
		return stTheme+"img24/emu_sin_imagen.png";
	else if( icono == "datos" )
		return stTheme+"img24/emu_datos.png";
	else if( icono == "dosbox" )
		return stTheme+"img24/emu_dbx.png";
	else if( icono == "scummvm" )
		return stTheme+"img24/emu_svm.png";
	else if( icono == "vdmsound" )
		return stTheme+"img24/emu_vdms.png";
	else {
		if( QFile::exists(iconoDir + icono) )
			return iconoDir + icono;
		else
			return stTheme+"img24/emu_sin_imagen.png";
	}
}

QString Funciones::getIconMount(QString tipoDrive, QString select_mount)
{
	if( tipoDrive == "drive" )
		return stTheme+"img16/"+select_mount+"drive_hd.png";
	else if( tipoDrive == "cdrom" )
		return stTheme+"img16/"+select_mount+"drive_cdrom.png";
	else if( tipoDrive == "floppy" )
		return stTheme+"img16/"+select_mount+"drive_floppy.png";
	else if( tipoDrive == "IMG_floppy" )
		return stTheme+"img16/"+select_mount+"floppy_1.png";
	else if( tipoDrive == "IMG_iso" )
		return stTheme+"img16/"+select_mount+"cd_iso.png";
	else if( tipoDrive == "IMG_multi_iso" )
		return stTheme+"img16/"+select_mount+"cd_multi_iso.png";
	else if( tipoDrive == "IMG_hdd" )
		return stTheme+"img16/"+select_mount+"drive_hd.png";
	else if(tipoDrive=="boot")
		return stTheme+"img16/"+select_mount+"floppy_2.png";
	else
		return stTheme+"img16/sinimg.png";
}

// Devuelve el directorio que usa el GR-lida
QString Funciones::GRlidaHomePath()
{
	QDir dirGrl;
	QString dirhomepath, dirApp, dirDb, fileDb;
	dirhomepath.clear();
	dirApp.clear();

	if( QFile::exists(QDir::currentPath()+"/GR-lida.conf") )
	{
		QSettings settings( QDir::currentPath()+"/GR-lida.conf", QSettings::IniFormat );
		settings.beginGroup("OpcGeneral");
			dirApp = settings.value("DirApp", "HomePath").toString().toLower();
		settings.endGroup();
	} else
		dirApp = "homepath";

	if( dirApp == "currentpath" )
		dirhomepath = QDir::currentPath()+"/";
	else {
		#ifdef Q_OS_WIN32 | Q_OS_MAC //Q_WS_WIN
			dirhomepath = QDir::homePath()+"/GR-lida/";

			if(dirGrl.exists(QDir::homePath()+"/.gr-lida/"))
			{
				dirGrl.rename(QDir::homePath()+"/.gr-lida/", QDir::homePath()+"/GR-lida/");

				QSettings settings( dirhomepath+"GR-lida.conf", QSettings::IniFormat );
				settings.beginGroup("SqlDatabase");
					fileDb = settings.value("db_host", dirhomepath+"/db_grl.grl").toString();
				settings.endGroup();

				QFileInfo info_db(fileDb);
				GuardarKeyGRLConfig(dirhomepath+"GR-lida.conf", "SqlDatabase", "db_host", dirhomepath+info_db.fileName());
			}
		#else
			dirhomepath = QDir::homePath()+"/.gr-lida/";
		#endif
	}

	return dirhomepath;
}

// Devuelve el directorio del Theme a usar
QString Funciones::ThemeGrl()
{
	QString dirApp, nameDirTheme, theme;
	QDir themeDir;

	dirApp = GRlidaHomePath();

	QSettings settings( dirApp+"/GR-lida.conf", QSettings::IniFormat );
	settings.beginGroup("OpcGeneral");
		nameDirTheme = settings.value("NameDirTheme", "defecto").toString();
	settings.endGroup();

	if( nameDirTheme == "defecto" || nameDirTheme.isEmpty() )
		theme = ":/";
	else {
		if( themeDir.exists(dirApp+"themes/"+ nameDirTheme +"/") )
			theme = dirApp+"themes/"+ nameDirTheme +"/";
		else
			theme = ":/";
	}
	return theme;
}

void Funciones::setTheme()
{
	stTheme = ThemeGrl();
}

// Carga la hoja de estilo y reemplazando el comodin <theme> por el que se use
QString Funciones::StyleSheet(bool list)
{
	QString stDirTheme;
	if( list )
		stDirTheme = stTheme + "StyleSheetList.qss";
	else
		stDirTheme = stTheme + "StyleSheet.qss";

	QFile file( stDirTheme );
	file.open( QIODevice::ReadOnly );
	QString styleSheet = QLatin1String( file.readAll() );
	styleSheet.replace("<theme>", stTheme);
	file.close();

	return styleSheet;
}

// Carga la configuración de los iconos de la lista en modo Icono
QHash<QString, QVariant> Funciones::CargarListWidgetIconConf()
{
	QString nameDirTheme;
	QHash<QString, QVariant> config;
	config.clear();

	QSettings GRLsettings( GRlidaHomePath()+"GR-lida.conf", QSettings::IniFormat );
	GRLsettings.beginGroup("OpcGeneral");
		nameDirTheme = GRLsettings.value("NameDirTheme", "defecto").toString();
	GRLsettings.endGroup();

	if( nameDirTheme == "defecto" || nameDirTheme == "" )
	{
		config["icon_width"]           = 181;
		config["icon_height"]          = 238;
		config["img_scaled"]           = true;
		config["img_scale_w"]          = 128;
		config["img_scale_h"]          = 180;
		config["img_scale_pos_x"]      = 27;
		config["img_scale_pos_y"]      = 30;
		config["tipo_emu_show"]        = true;
		config["tipo_emu_pos_x"]       = 11;
		config["tipo_emu_pos_y"]       = 200;
		config["rating_show"]          = true;
		config["rating_pos_x"]         = 91;
		config["rating_pos_y"]         = 200;
		config["title_show"]           = true;
		config["title_pos_x"]          = 13;
		config["title_pos_y"]          = 220;
		config["title_width"]          = 155;
		config["title_height"]         = 16;
		config["title_font"]           = "Tahoma";
		config["title_font_size"]      = 7;
		config["title_font_bold"]      = true;
		config["title_font_italic"]    = false;
		config["title_font_underline"] = false;
		config["title_max_caracteres"] = 27;
	} else {
		QSettings settings( stTheme+"info.ini", QSettings::IniFormat );
		settings.beginGroup("list_icon");
			config["icon_width"]           = settings.value("icon_width" , 181).toInt();
			config["icon_height"]          = settings.value("icon_height", 238).toInt();
			config["img_scaled"]           = settings.value("img_scaled" , true).toBool();
			config["img_scale_w"]          = settings.value("img_scale_w", 128).toInt();
			config["img_scale_h"]          = settings.value("img_scale_h", 180).toInt();
			config["img_scale_pos_x"]      = settings.value("img_scale_pos_x", 27).toInt();
			config["img_scale_pos_y"]      = settings.value("img_scale_pos_y", 30).toInt();
			config["tipo_emu_show"]        = settings.value("tipo_emu_show"  , true).toBool();
			config["tipo_emu_pos_x"]       = settings.value("tipo_emu_pos_x" , 11).toInt();
			config["tipo_emu_pos_y"]       = settings.value("tipo_emu_pos_y" , 200).toInt();
			config["rating_show"]          = settings.value("rating_show"  , true).toBool();
			config["rating_pos_x"]         = settings.value("rating_pos_x" , 91).toInt();
			config["rating_pos_y"]         = settings.value("rating_pos_y" , 200).toInt();
			config["title_show"]           = settings.value("title_show"  , true).toBool();
			config["title_pos_x"]          = settings.value("title_pos_x" , 13).toInt();
			config["title_pos_y"]          = settings.value("title_pos_y" , 220).toInt();
			config["title_width"]          = settings.value("title_width" , 155).toInt();
			config["title_height"]         = settings.value("title_height", 16).toInt();
			config["title_font"]           = settings.value("title_font"  , "Tahoma").toString();
			config["title_font_size"]      = settings.value("title_font_size", 7).toInt();
			config["title_font_bold"]      = settings.value("title_font_bold", true).toBool();
			config["title_font_italic"]    = settings.value("title_font_italic", false).toBool();
			config["title_font_underline"] = settings.value("title_font_underline", false).toBool();
			config["title_max_caracteres"] = settings.value("title_max_caracteres", 27).toInt();
		settings.endGroup();
	}
	return config;
}

// Carga la configuracion del GR-lida
QHash<QString, QVariant> Funciones::CargarGRLConfig(QString iniFileName)
{
	QHash<QString, QVariant> config;
	config.clear();

	QSettings settings(iniFileName, QSettings::IniFormat);
	settings.beginGroup("SqlDatabase");
		config["db_type"]           = settings.value("db_type"          , "QSQLITE").toString();
		config["db_server"]         = settings.value("db_server"        , "localhost").toString();
		config["db_host"]           = settings.value("db_host"          , QDir::toNativeSeparators(GRlidaHomePath()+"db_grl.grl") ).toString();
		config["db_name"]           = settings.value("db_name"          , ""      ).toString();
		config["db_username"]       = settings.value("db_username"      , ""      ).toString();
		config["db_password"]       = settings.value("db_password"      , ""      ).toString();
		config["db_port"]           = settings.value("db_port"          , "3306"  ).toString();
		config["db_Orden_ColTabla"] = settings.value("db_Orden_ColTabla", "titulo").toString();
		config["db_Orden_By"]       = settings.value("db_Orden_By"      , "titulo").toString();
		config["db_Orden"]          = settings.value("db_Orden"         , "ASC"   ).toString();
	settings.endGroup();

	settings.beginGroup("OpcGeneral");
		config["Primeravez"]   = settings.value("Primeravez" , true ).toBool();
		config["DirDOSBox"]    = settings.value("DirDOSBox"  , ""   ).toString();
		config["DirScummVM"]   = settings.value("DirScummVM" , ""   ).toString();
		config["DOSBoxDisp"]   = settings.value("DOSBoxDisp" , false).toBool();
		config["ScummVMDisp"]  = settings.value("ScummVMDisp", false).toBool();
	#ifdef Q_OS_WIN32
		config["VDMSoundDisp"] = settings.value("VDMSoundDisp", false).toBool();
	#else
		config["VDMSoundDisp"] = false;
	#endif
		config["IdiomaSelect"]      = settings.value("IdiomaSelect" , "es_ES").toString();
		config["IdiomaExterno"]     = settings.value("IdiomaExterno", false  ).toBool();
		config["DirBaseGames"]      = ""+settings.value("DirBaseGames" , ""  ).toString();
		config["url_xmldb"]         = settings.value("url_xmldb"    , ""     ).toString();
		config["FormatoFecha"]      = settings.value("FormatoFecha" , "dd/MM/yyyy HH:mm:ss").toString();
		config["Style"]             = settings.value("Style"        , ""     ).toString();
		config["StylePalette"]      = settings.value("StylePalette" , false  ).toBool();
		config["NameDirTheme"]      = settings.value("NameDirTheme" , "defecto"  ).toString();
		config["IconoFav"]          = settings.value("IconoFav"     , "fav_0.png").toString();
		config["PicFlowReflection"] = settings.value("PicFlowReflection", "PlainReflection").toString();
		config["Skip_PicFlow"]      = settings.value("Skip_PicFlow" , 10).toInt();
		config["IndexTabArchivos"]  = settings.value("IndexTabArchivos", 0).toInt();
		config["VersionDBx"]        = settings.value("VersionDBx", "0.74").toString();
	settings.endGroup();

	settings.beginGroup("OpcFuente");
		config["font_usar"]   = settings.value("font_usar"  , false).toBool();
		config["font_family"] = settings.value("font_family", "Tahoma").toString();
		config["font_size"]   = settings.value("font_size"  , 8).toInt();
	settings.endGroup();

	settings.beginGroup("HttpProxy");
		config["ProxyEnable"]   = settings.value("ProxyEnable"  , false).toBool();
		config["ProxyType"]     = settings.value("ProxyType"    , 2    ).toInt();
		config["ProxyHost"]     = settings.value("ProxyHost"    , ""   ).toString();
		config["ProxyPort"]     = settings.value("ProxyPort"    , 1080 ).toInt();
		config["ProxyLogin"]    = settings.value("ProxyEnable"  , false).toBool();
		config["ProxyUserName"] = settings.value("ProxyUserName", ""   ).toString();
		config["ProxyPassword"] = settings.value("ProxyPassword", ""   ).toString();
	settings.endGroup();

	settings.beginGroup("MainState");
		config["maximized"] = settings.value("maximized", false).toBool();
		config["geometry"]  = settings.value("geometry", "").toByteArray();
		config["state"]     = settings.value("state", "").toByteArray();
	settings.endGroup();

	settings.beginGroup("OpcVer");
		config["Pnl_Nav"]         = settings.value("Pnl_Nav"        , false).toBool();
		config["Pnl_FilesUrl"]    = settings.value("Pnl_FilesUrl"   , false).toBool();
		config["Pnl_Datos"]       = settings.value("Pnl_Datos"      , true ).toBool();
		config["Pnl_PictureFlow"] = settings.value("Pnl_PictureFlow", true ).toBool();
		config["Pnl_Capturas"]    = settings.value("Pnl_Capturas"   , false).toBool();
		config["Pnl_Ordenar"]     = settings.value("Pnl_Ordenar"    , false).toBool();
		config["Pnl_Toolbar"]     = settings.value("Pnl_Toolbar"    , true ).toBool();
		config["ver_Compania"]    = settings.value("ver_Compania"   , false).toBool();
		config["ver_Anno"]        = settings.value("ver_Anno"       , false).toBool();
		config["ver_Rating"]      = settings.value("ver_Rating"     , true ).toBool();
		config["ver_IconMode"]    = settings.value("ver_IconMode"   , false).toBool();
	settings.endGroup();

	settings.beginGroup("ImageViewerState");
		config["img_maximized"]    = settings.value("maximized"  , false).toBool();
		config["fitToWindow"]      = settings.value("fitToWindow", false).toBool();
		config["verListaImagenes"] = settings.value("verListaImagenes", false).toBool();
	settings.endGroup();

	settings.beginGroup("UltimoDirectorio");
		config["DirBD"]               = settings.value("DirBD" , "").toString();
		config["DirDbx"]              = settings.value("DirDbx", "").toString();
		config["DirSvm"]              = settings.value("DirSvm", "").toString();
		config["DirImportar"]         = settings.value("DirImportar"  , "").toString();
		config["DirExportPath"]       = settings.value("DirExportPath", "").toString();
	// Datos de Juegos
		config["Img_Thumbs"]          = settings.value("Img_Thumbs"         , "").toString();
		config["Img_CoverFront"]      = settings.value("Img_CoverFront"     , "").toString();
		config["Img_CoverBack"]       = settings.value("Img_CoverBack"      , "").toString();
		config["DatosFiles_PathFile"] = settings.value("DatosFiles_PathFile", "").toString();
		config["DatosFiles_PathExe"]  = settings.value("DatosFiles_PathExe" , "").toString();
	// DOSBox
		config["Dbx_path_exe"]        = settings.value("Dbx_path_exe"       , "").toString();
		config["Dbx_path_setup"]      = settings.value("Dbx_path_setup"     , "").toString();
		config["Dbx_gus_ultradir"]    = settings.value("Dbx_gus_ultradir"   , "").toString();
		config["Dbx_sdl_mapperfile"]  = settings.value("Dbx_sdl_mapperfile" , "").toString();
		config["Dbx_dosbox_language"] = settings.value("Dbx_dosbox_language", "").toString();
		config["Dbx_dosbox_captures"] = settings.value("Dbx_dosbox_captures", "").toString();
		config["Dbx_path_sonido"]     = settings.value("Dbx_path_sonido"    , "").toString();
	// ScummVM
		config["Svm_path"]            = settings.value("Svm_path"         , "").toString();
		config["Svm_savepath"]        = settings.value("Svm_savepath"     , "").toString();
		config["Svm_extrapath"]       = settings.value("Svm_extrapath"    , "").toString();
		config["Svm_path_capturas"]   = settings.value("Svm_path_capturas", "").toString();
		config["Svm_path_sonido"]     = settings.value("Svm_path_sonido"  , "").toString();
		config["Svm_path_setup"]      = settings.value("Svm_path_setup"   , "").toString();
		config["Svm_soundfont"]       = settings.value("Svm_soundfont"    , "").toString();
	// VDMSound
		config["Vdms_path_exe"]       = settings.value("Vdms_path_exe", "").toString();
		config["Vdms_icon"]           = settings.value("Vdms_icon"    , "").toString();
	// Montaje
		config["Montaje_path"]        = settings.value("Montaje_path" , "").toString();
	settings.endGroup();

	settings.beginGroup("Updates");
		config["chkVersion"] = settings.value("chkVersion", false).toBool();
		config["Version"]    = settings.value("Version"   , stVersionGrl()).toString();
	settings.endGroup();

	return config;
}

// Guarda la configuración del GR-lida
void Funciones::GuardarGRLConfig(QString iniFileName, QHash<QString, QVariant> config)
{
	QSettings settings(iniFileName, QSettings::IniFormat);
	settings.clear();

	settings.beginGroup("SqlDatabase");
		settings.setValue("db_type"          , config["db_type"]           );
		settings.setValue("db_server"        , config["db_server"]         );
		settings.setValue("db_host"          , config["db_host"]           );
		settings.setValue("db_name"          , config["db_name"]           );
		settings.setValue("db_username"      , config["db_username"]       );
		settings.setValue("db_password"      , config["db_password"]       );
		settings.setValue("db_port"          , config["db_port"]           );
		settings.setValue("db_Orden_ColTabla", config["db_Orden_ColTabla"] );
		settings.setValue("db_Orden_By"      , config["db_Orden_By"]       );
		settings.setValue("db_Orden"         , config["db_Orden"]          );
	settings.endGroup();

	settings.beginGroup("OpcGeneral");
		settings.setValue("Primeravez"  , config["Primeravez"]   );
		settings.setValue("DirDOSBox"   , config["DirDOSBox"]    );
		settings.setValue("DirScummVM"  , config["DirScummVM"]   );
		settings.setValue("DOSBoxDisp"  , config["DOSBoxDisp"]   );
		settings.setValue("ScummVMDisp" , config["ScummVMDisp"]  );
	#ifdef Q_OS_WIN32
		settings.setValue("VDMSoundDisp", config["VDMSoundDisp"] );
	#else
		settings.setValue("VDMSoundDisp", false );
	#endif
		settings.setValue("IdiomaSelect"     , config["IdiomaSelect"]      );
		settings.setValue("IdiomaExterno"    , config["IdiomaExterno"]     );
		settings.setValue("DirBaseGames"     , config["DirBaseGames"]      );
		settings.setValue("url_xmldb"        , config["url_xmldb"]         );
		settings.setValue("FormatoFecha"     , config["FormatoFecha"]      );
		settings.setValue("Style"            , config["Style"]             );
		settings.setValue("StylePalette"     , config["StylePalette"]      );
		settings.setValue("NameDirTheme"     , config["NameDirTheme"]      );
		settings.setValue("IconoFav"         , config["IconoFav"]          );
		settings.setValue("PicFlowReflection", config["PicFlowReflection"] );
		settings.setValue("Skip_PicFlow"     , config["Skip_PicFlow"]      );
		settings.setValue("IndexTabArchivos" , config["IndexTabArchivos"]  );
		settings.setValue("VersionDBx"       , config["VersionDBx"]        );
	settings.endGroup();

	settings.beginGroup("OpcFuente");
		settings.setValue("font_usar"  , config["font_usar"]   );
		settings.setValue("font_family", config["font_family"] );
		settings.setValue("font_size"  , config["font_size"]   );
	settings.endGroup();

	settings.beginGroup("HttpProxy");
		settings.setValue("ProxyEnable"  , config["ProxyEnable"]   );
		settings.setValue("ProxyType"    , config["ProxyType"]     );
		settings.setValue("ProxyHost"    , config["ProxyHost"]     );
		settings.setValue("ProxyPort"    , config["ProxyPort"]     );
		settings.setValue("ProxyLogin"   , config["ProxyLogin"]    );
		settings.setValue("ProxyUserName", config["ProxyUserName"] );
		settings.setValue("ProxyPassword", config["ProxyPassword"] );
	settings.endGroup();

	settings.beginGroup("MainState");
		settings.setValue("maximized", config["maximized"] );
		settings.setValue("geometry" , config["geometry"]  );
		settings.setValue("state"    , config["state"]     );
	settings.endGroup();

	settings.beginGroup("OpcVer");
		settings.setValue("Pnl_Nav"        , config["Pnl_Nav"]         );
		settings.setValue("Pnl_FilesUrl"   , config["Pnl_FilesUrl"]    );
		settings.setValue("Pnl_Datos"      , config["Pnl_Datos"]       );
		settings.setValue("Pnl_PictureFlow", config["Pnl_PictureFlow"] );
		settings.setValue("Pnl_Capturas"   , config["Pnl_Capturas"]    );
		settings.setValue("Pnl_Ordenar"    , config["Pnl_Ordenar"]     );
		settings.setValue("Pnl_Toolbar"    , config["Pnl_Toolbar"]     );
		settings.setValue("ver_Compania"   , config["ver_Compania"]    );
		settings.setValue("ver_Anno"       , config["ver_Anno"]        );
		settings.setValue("ver_Rating"     , config["ver_Rating"]      );
		settings.setValue("ver_IconMode"   , config["ver_IconMode"]    );
	settings.endGroup();

	settings.beginGroup("ImageViewerState");
		settings.setValue("maximized"       , config["img_maximized"]   );
		settings.setValue("fitToWindow"     , config["fitToWindow"]     );
		settings.setValue("verListaImagenes", config["verListaImagenes"]);
	settings.endGroup();

	settings.beginGroup("UltimoDirectorio");
		settings.setValue("DirBD" , config["DirBD"]  );
		settings.setValue("DirDbx", config["DirDbx"] );
		settings.setValue("DirSvm", config["DirSvm"] );
		settings.setValue("DirImportar"  , config["DirImportar"]   );
		settings.setValue("DirExportPath", config["DirExportPath"] );
	// Datos de Juegos
		settings.setValue("Img_Thumbs"         , config["Img_Thumbs"]     );
		settings.setValue("Img_CoverFront"     , config["Img_CoverFront"] );
		settings.setValue("Img_CoverBack"      , config["Img_CoverBack"]  );
		settings.setValue("DatosFiles_PathFile", config["DatosFiles_PathFile"] );
		settings.setValue("DatosFiles_PathExe", config["DatosFiles_PathExe"] );
	// DOSBox
		settings.setValue("Dbx_path_exe"       , config["Dbx_path_exe"]        );
		settings.setValue("Dbx_path_setup"     , config["Dbx_path_setup"]      );
		settings.setValue("Dbx_gus_ultradir"   , config["Dbx_gus_ultradir"]    );
		settings.setValue("Dbx_sdl_mapperfile" , config["Dbx_sdl_mapperfile"]  );
		settings.setValue("Dbx_dosbox_language", config["Dbx_dosbox_language"] );
		settings.setValue("Dbx_dosbox_captures", config["Dbx_dosbox_captures"] );
		settings.setValue("Dbx_path_sonido"    , config["Dbx_path_sonido"]     );
	// ScummVM
		settings.setValue("Svm_path"         , config["Svm_path"]          );
		settings.setValue("Svm_savepath"     , config["Svm_savepath"]      );
		settings.setValue("Svm_extrapath"    , config["Svm_extrapath"]     );
		settings.setValue("Svm_path_capturas", config["Svm_path_capturas"] );
		settings.setValue("Svm_path_sonido"  , config["Svm_path_sonido"]   );
		settings.setValue("Svm_path_setup"   , config["Svm_path_setup"]    );
		settings.setValue("Svm_soundfont"    , config["Svm_soundfont"]     );
	// VDMSound
		settings.setValue("Vdms_path_exe", config["Vdms_path_exe"] );
		settings.setValue("Vdms_icon"    , config["Vdms_icon"]     );
	// Montaje
		settings.setValue("Montaje_path" , config["Montaje_path"]  );
	settings.endGroup();

	settings.beginGroup("Updates");
		settings.setValue("chkVersion", config["chkVersion"] );
		settings.setValue("Version"   , stVersionGrl()       );
	settings.endGroup();
}

// Guarda la configuración del GR-lida
void Funciones::GuardarKeyGRLConfig(QString iniFileName, QString grupo, QString key, QString value)
{
	QSettings settings(iniFileName, QSettings::IniFormat);
	settings.beginGroup(grupo);
		settings.setValue(key, value);
	settings.endGroup();
}

// Comprueba si un directorio existe y sino lo crea
void Funciones::ComprobarDirectorio(QString nameDir)
{
	QDir directorio;
	if( !directorio.exists( nameDir ) )
		directorio.mkdir( nameDir );
}

// Obtiene la dirección y el nombre del archivo atraves de QFileDialog
QString Funciones::VentanaAbrirArchivos(const QString caption, const QString dir, const QString tmp_dir, const QString filter, QString *selectedFilter, bool Open_Save)
{
	QString archivo, base;
	QFileInfo fi;

	fi.setFile( dir );
	if( fi.exists() )
		base = dir;
	else
		base = GRlidaHomePath();

	if( Open_Save == true )
		archivo = QFileDialog::getSaveFileName(0, caption, base, filter, selectedFilter);
	else
		archivo = QFileDialog::getOpenFileName(0, caption, base, filter, selectedFilter);

	if( !archivo.isEmpty() )
		return archivo;
	else
		return tmp_dir;
}

// Obtiene la dirección de una carpeta atraves de QFileDialog
QString Funciones::VentanaDirectorios(const QString caption, const QString dir, const QString tmp_dir)
{
	QString directorio = QFileDialog::getExistingDirectory(0, caption, dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );

	if( !directorio.isEmpty() )
	{
		if( directorio.endsWith("/") )
			return directorio;
		else
			return directorio+"/";
	} else {
		if( !tmp_dir.isEmpty() )
		{
			if( tmp_dir.endsWith("/") )
				return tmp_dir;
			else
				return tmp_dir+"/";
		} else
			return "";
	}
}

// Abre un archivo con el programa predeterminado
bool Funciones::abrirArchivo(QString filePath)
{
	return QDesktopServices::openUrl( QUrl::fromLocalFile( filePath ) );
}

// Abre un directorio
bool Funciones::abrirDirectorio(QString dirPath)
{
	return abrirArchivo(dirPath);
}

// Crea una ventana para mostrar Información
void Funciones::VentanaInfo(QString titulo, QString icono, int w, int h, QString contenido)
{
	QString str_html;
	QFile LeerArchivo(stTheme+"tpl_info.html");
	if( LeerArchivo.open( QIODevice::ReadOnly | QIODevice::Text ) )
	{
		QTextStream in( &LeerArchivo );
		in.setCodec("UTF-8");
		str_html.clear();
		str_html = in.readAll();
	} else
		str_html = "";
	LeerArchivo.close();

	QDialog *Dialog = new QDialog(0);
	QVBoxLayout *mainLayout;
	QTextBrowser *textInfo;
	QHBoxLayout *hLayout;
	QSpacerItem *hSpacer;
	QPushButton *btnAceptar;

	Dialog->resize(w, h);

	textInfo = new QTextBrowser(Dialog);
	textInfo->setReadOnly(true);
	textInfo->setOpenExternalLinks(true);
	textInfo->setOpenLinks(true);

	hSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	btnAceptar = new QPushButton(Dialog);

	hLayout = new QHBoxLayout();
	hLayout->addItem(hSpacer);
	hLayout->addWidget(btnAceptar);

	mainLayout = new QVBoxLayout(Dialog);
	mainLayout->addWidget(textInfo);
	mainLayout->addLayout(hLayout);

	QObject::connect( btnAceptar, SIGNAL( clicked() ), Dialog, SLOT( accept() ) );
	btnAceptar->setText( QObject::tr("Aceptar") );
	btnAceptar->setIcon( QIcon(stTheme+"img16/aplicar.png") );

	str_html.replace("<theme>", stTheme);
	str_html.replace("{contenido_info}", contenido);
	textInfo->setHtml( str_html );

	Dialog->setWindowTitle( titulo );
	Dialog->setWindowIcon(QIcon(icono));
	Dialog->setStyleSheet( StyleSheet() );
	Dialog->exec();
	delete Dialog;
}

// Elimina un item de un TreeWidget
void Funciones::DeleteItemTree(QTreeWidgetItem *item)
{
	if( !item )
		return;

	for(int i = item->childCount() - 1; i >= 0; i--)
	{
		DeleteItemTree(item->child(i));
	}

	delete item;
}

// Carga la lista de Scripts en un QComboBox
void Funciones::CargarScriptsComboBox(const QString dirScripts, QComboBox *myCombobox, QString filter)
{
	CargarIdiomasCombo(dirScripts, myCombobox, filter, true);
}

// Carga la lista de Idiomas en un QComboBox
void Funciones::CargarIdiomasCombo(const QString dirLng, QComboBox *myCombobox, QString filter, bool isScript)
{
	QDir dir( dirLng );
	QDir::Filters filters = QDir::Files | QDir::Readable;
	QDir::SortFlags sort = QDir::Name;
	QFileInfoList entries = dir.entryInfoList(QStringList() << filter, filters, sort);

	myCombobox->clear();
	if( isScript )
		myCombobox->addItem(QIcon(stTheme+"img16/archivo_config.png"), QObject::tr("Importar desde un archivo:")+" GR-lida XML, D-Fend Reloaded prof", "_desde_archivo_");

	foreach (QFileInfo file, entries)
	{
		if( isScript )
		{
			QScriptEngine engine;
			QString js_titulo, js_version;
			QFile scriptFile( file.absoluteFilePath() );
			scriptFile.open(QIODevice::ReadOnly | QIODevice::Text);
			engine.evaluate( scriptFile.readAll() );
			scriptFile.close();

			if(engine.evaluate("title").isValid())
				js_titulo = engine.evaluate("title").toString();
			else
				js_titulo = file.baseName();

			if(engine.evaluate("version").isValid())
				js_version = engine.evaluate("version").toString();
			else
				js_version = "0";

			myCombobox->addItem(QIcon(stTheme+"img16/edit_enlace.png"), js_titulo +" v"+ js_version, file.fileName());
		} else {
			// pick country and language out of the file name
			QStringList parts = file.baseName().split("_");
			QString tmp_locale = parts.value(1) + "_" + parts.value(2);

			QLocale locale(tmp_locale);
			QString language = QLocale::languageToString( locale.language() );
			QString country  = QLocale::countryToString( locale.country() );
			QString nameLang = language + " (" + country + ")";

			myCombobox->addItem(QIcon(stTheme+"img_lng/"+parts.value(2).toLower()+".png"), nameLang, tmp_locale );
		}
	}
}

// Carga una lista de datos en un QComboBox
void Funciones::CargarDatosComboBox(QString archivo, QComboBox *myCombobox, int num_col, bool idioma_svm)
{
	QStringList lista, listaTemp;
	QString icono, iconoTemp;
	QFile file( archivo );
	if( file.open(QIODevice::ReadOnly) != 0 )
	{
		QTextStream in(&file);
		in.setCodec("UTF-8");

		while ( !in.atEnd() )
			listaTemp << in.readLine();

		myCombobox->clear();
		for( int i = 0; i < listaTemp.count(); i++ )
		{
			lista = listaTemp[i].split( "|" );

			icono.clear();
			icono = stTheme+"img16/sinimg.png";

			if( num_col == 3 )
				iconoTemp = lista.value(2);
			else
				iconoTemp = lista.value(1);

			if( iconoTemp.isEmpty() || iconoTemp == "sinimg"  )
				icono = stTheme+"img16/sinimg.png";
			else {
				if( idioma_svm )
					icono = stTheme+"img_lng/"+iconoTemp+".png";
				else
					icono = stTheme+"img16/"+iconoTemp+".png";
			}

			if( !QFile::exists(icono) )
				icono = stTheme+"img16/sinimg.png";

			switch ( num_col )
			{
				case 1: // 1 columna
					myCombobox->addItem( QIcon( icono ), lista.value(0) );
				break;
				case 2: // 2 columna
					myCombobox->addItem( QIcon( icono ), lista.value(0), lista.value(1) );
				break;
				case 3: // 3 columna
					myCombobox->addItem( QIcon( icono ), lista.value(0), lista.value(1) );
				break;
				default:
					myCombobox->addItem( QIcon( icono ), lista.value(0) );
				break;
			}
		}
	}
	file.close();
}

// Carga Iconos e imagenes soportadas en un QComboBox de un directorio
void Funciones::CargarIconosComboBox(QString iconDir, QComboBox *myCombobox, QStringList filters)
{
	QDir dir( iconDir );
	dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	dir.setSorting(QDir::Name);
	dir.setNameFilters(filters);
	QFileInfoList list = dir.entryInfoList();
	myCombobox->clear();
	myCombobox->addItem( QIcon( stTheme+"img24/emu_datos.png" ), "datos"   , "datos"    );
	myCombobox->addItem( QIcon( stTheme+"img24/emu_dbx.png"   ), "dosbox"  , "dosbox"   );
	myCombobox->addItem( QIcon( stTheme+"img24/emu_svm.png"   ), "scummvm" , "scummvm"  );
	myCombobox->addItem( QIcon( stTheme+"img24/emu_vdms.png"  ), "vdmsound", "vdmsound" );
	for(int i = 0; i < list.size(); ++i)
	{
		QFileInfo fileInfo = list.at(i);
		QPixmap pixmap(iconDir + fileInfo.fileName() );
		myCombobox->addItem( QIcon( pixmap ), fileInfo.completeBaseName(), fileInfo.fileName() );
	}
}

// Carga la lista del ScummVM en un QTreeWidget
void Funciones::CargarDatosListaSvm(QString archivo, QTreeWidget *myTreeWidget)
{
// Abrimos la lista de compatibilidad del ScummVM y rellenamos el twScummVM
	QFile file( archivo );
	QStringList svm_Lista, svm_ListaTemp;

	myTreeWidget->clear();
	if( file.open(QIODevice::ReadOnly)!=0 )
	{
		QTextStream in(&file);
		in.setCodec("UTF-8");

		while ( !in.atEnd() )
			svm_ListaTemp << in.readLine();

		for( int i = 0; i < svm_ListaTemp.size(); i++ )
		{
			if( svm_ListaTemp[i] != "" )
			{
				svm_Lista = svm_ListaTemp[i].split( "|" );

				QTreeWidgetItem *item = new QTreeWidgetItem( myTreeWidget );
				item->setText( 1, svm_Lista.value(0) );
				if( svm_Lista.value(1) == "" )
				{
					item->setIcon( 0, QIcon(stTheme+"imgsvm/svmlist_space.png") );

					item->setTextColor( 1, QColor(0,0,0) );
					item->setFont( 1, QFont("Times", 10, QFont::Bold));
					if( svm_Lista.value(2) == "-1" || svm_Lista.value(2) == "" )
						item->setIcon( 1, QIcon(stTheme+"imgsvm/svmlist_space.png") );
					else
						item->setIcon( 1, QIcon(stTheme+"imgsvm/"+svm_Lista.value(2)+".png") );
				} else {
					item->setIcon( 0, QIcon(stTheme+"imgsvm/"+svm_Lista.value(2)+".png") );

					if (QFile::exists(stTheme+"icon_svm/"+svm_Lista.value(3)) )
						item->setIcon( 1, QIcon(stTheme+"icon_svm/"+svm_Lista.value(3)) );
					else
						item->setIcon( 1, QIcon(stTheme+"icon_svm/sinimg.png") );
				}

				item->setText( 2, svm_Lista.value(1) );
			}else {
				QTreeWidgetItem *item = new QTreeWidgetItem( myTreeWidget );
				item->setText( 0, "" );
				item->setText( 1, "" );
				item->setText( 2, "" );
				item->setIcon( 0, QIcon(stTheme+"icon_svm/sinimg.png") );
				item->setIcon( 1, QIcon(stTheme+"icon_svm/sinimg.png") );
			}
		}
	}
	file.close();
}

//Carga la lista de los Smiles y los devuelve en un QHash<QString, QString>
QHash<QString, QString> Funciones::Cargar_Smiles(QString archivo)
{
	QFile file( archivo );
	QStringList smiles_Lista, smiles_ListaTemp;
	QHash<QString, QString> listSmailes;

	listSmailes.clear();
	if( file.open(QIODevice::ReadOnly)!=0 )
	{
		QTextStream in(&file);
		in.setCodec("UTF-8");

		while ( !in.atEnd() )
			smiles_ListaTemp << in.readLine();

		for( int i = 0; i < smiles_ListaTemp.size(); i++ )
		{
			smiles_Lista = smiles_ListaTemp[i].split( "\",\"" );
			listSmailes.insert( smiles_Lista.value(0), smiles_Lista.value(1) );
		}
	}
	return listSmailes;
}

//Carga la lista de los Smiles en un TreeWidget y los devuelve en un QHash<QString, QString>
QHash<QString, QString> Funciones::Cargar_Smiles(QString archivo, QTreeWidget *myTreeWidget)
{
	QFile file( archivo );
	QStringList smiles_Lista, smiles_ListaTemp;
	QHash<QString, QString> listSmailes;

	myTreeWidget->clear();
	listSmailes.clear();
	if( file.open(QIODevice::ReadOnly)!=0 )
	{
		QTextStream in(&file);
		in.setCodec("UTF-8");

		while ( !in.atEnd() )
			smiles_ListaTemp << in.readLine();

		for( int i = 0; i < smiles_ListaTemp.size(); i++ )
		{
			smiles_Lista = smiles_ListaTemp[i].split( "\",\"" );
			listSmailes.insert( smiles_Lista.value(0), smiles_Lista.value(1) );

			QTreeWidgetItem *item_smiles = new QTreeWidgetItem( myTreeWidget );
			item_smiles->setText( 0, smiles_Lista.value(0)          ); // codigo
			item_smiles->setIcon( 0, QIcon( smiles_Lista.value(1) ) ); // imagen
		}
	}

	return listSmailes;
}

// Remplaza texto por las imagenes de los emoticones
QString Funciones::ReemplazaTextoSmiles(QString str, QHash<QString, QString> lista)
{
	QHash<QString, QString>::const_iterator i_Hash;

	for( i_Hash = lista.constBegin(); i_Hash != lista.constEnd(); ++i_Hash )
		str.replace( i_Hash.key(), "<img src=\""+i_Hash.value()+"\" />");

	return str;
}

// Carga los Distintos datos para Ordenar
QStringList Funciones::CargaDatosListas(QString archivo, QString delimitador)
{
	QFile file( archivo );
	QStringList lista, listaTemp, listaDatos;
	//QHash<QString, QString> listaDatos;

	listaDatos.clear();
	if( file.open(QIODevice::ReadOnly)!=0 )
	{
		QTextStream in(&file);
		in.setCodec("UTF-8");

		while ( !in.atEnd() )
			listaTemp << in.readLine();

		for( int i = 0; i < listaTemp.size(); i++ )
		{
			lista = listaTemp[i].split( delimitador );
			listaDatos << lista.value(0);
		}
	}
	return listaDatos;
}

void Funciones::CargarListaDeCaptura(QTreeWidget *myTreeWidget, const QString directorio, QStringList filters, bool isImagen)
{
	myTreeWidget->clear();
	QString archivo, extension;
	QDir dir( directorio );
	dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	dir.setSorting(QDir::Name);
	dir.setNameFilters(filters);

	if( dir.exists() )
	{
		QFileInfoList list = dir.entryInfoList();

		for (int i = 0; i < list.size(); ++i)
		{
			QFileInfo fileInfo = list.at(i);

			extension = fileInfo.completeSuffix();

			if( directorio.endsWith("/") )
				archivo = directorio + fileInfo.fileName();
			else
				archivo = directorio + "/" + fileInfo.fileName();

			QTreeWidgetItem *item = new QTreeWidgetItem( myTreeWidget );
			if( isImagen )
			{
				item->setIcon( 0, QIcon( archivo ) );
				item->setText( 0, "" );
			} else {
			// Videos
				if( extension == "avi" || extension == "mkv" || extension == "mov" ||
					extension == "mp4" ||extension == "mpeg" || extension == "mpg" ||
					extension == "wmv" )
					item->setIcon( 0, QIcon(stTheme+"img16/img_tv2x.png") );
			// Sonidos
				else if( extension == "mp3" || extension == "ogg" || extension == "wav" ||
						 extension == "wma" )
					item->setIcon( 0, QIcon(stTheme+"img16/img_audio.png") );
				else
					item->setIcon( 0, QIcon(stTheme+"img16/tag.png") );
				item->setText( 0, fileInfo.fileName() );
			}
			item->setText( 1, archivo );
		}
	}
}

// Crea la configuración del ScummVM
void Funciones::CreaIniScummVM(QString dirIni, QHash<QString, QString> conf_Svm)
{
	QSettings settings(dirIni, QSettings::IniFormat);
	settings.clear();
	settings.beginGroup( conf_Svm["Svm_game_label"] );
		settings.setValue("description"    , conf_Svm["Svm_description"]    );
		settings.setValue("gameid"         , conf_Svm["Svm_game"]           ); // monkey
		settings.setValue("language"       , conf_Svm["Svm_language"]       ); // es
		settings.setValue("subtitles"      , conf_Svm["Svm_subtitles"]      ); // true
		settings.setValue("platform"       , conf_Svm["Svm_platform"]       ); // pc
		settings.setValue("gfx_mode"       , conf_Svm["Svm_gfx_mode"]       ); // 2x
		settings.setValue("render_mode"    , conf_Svm["Svm_render_mode"]    ); // hercGreen
		settings.setValue("fullscreen"     , conf_Svm["Svm_fullscreen"]     ); // true
		settings.setValue("aspect_ratio"   , conf_Svm["Svm_aspect_ratio"]   ); // true
		settings.setValue("path"           , conf_Svm["Svm_path"]           );
		settings.setValue("extrapath"      , conf_Svm["Svm_path_extra"]     );
		settings.setValue("savepath"       , conf_Svm["Svm_path_save"]      );
		settings.setValue("music_driver"   , conf_Svm["Svm_music_driver"]   ); // adlib
		settings.setValue("enable_gs"      , conf_Svm["Svm_enable_gs"]      ); // true
		settings.setValue("multi_midi"     , conf_Svm["Svm_multi_midi"]     ); // true
		settings.setValue("native_mt32"    , conf_Svm["Svm_native_mt32"]    ); // true
		settings.setValue("master_volume"  , conf_Svm["Svm_master_volume"]  ); // 192
		settings.setValue("music_volume"   , conf_Svm["Svm_music_volume"]   ); // 192
		settings.setValue("sfx_volume"     , conf_Svm["Svm_sfx_volume"]     ); // 192
		settings.setValue("speech_volume"  , conf_Svm["Svm_speech_volume"]  ); // 192
		settings.setValue("tempo"          , conf_Svm["Svm_tempo"]          ); // 0
		settings.setValue("talkspeed"      , conf_Svm["Svm_talkspeed"]      ); // 107
		settings.setValue("cdrom"          , conf_Svm["Svm_cdrom"]          ); // 0
		settings.setValue("joystick_num"   , conf_Svm["Svm_joystick_num"]   ); // -1
		settings.setValue("output_rate"    , conf_Svm["Svm_output_rate"]    ); // 44100
		settings.setValue("midi_gain"      , conf_Svm["Svm_midi_gain"]      ); // 257
		settings.setValue("copy_protection", conf_Svm["Svm_copy_protection"]); // false
		settings.setValue("soundfont"      , conf_Svm["Svm_sound_font"]     );
		settings.setValue("walkspeed"      , conf_Svm["Svm_walkspeed"]      );
	settings.endGroup();

	settings.beginGroup("scummvm");
		settings.setValue("gui_theme" ,"modern");
		settings.setValue("gfx_mode"  ,"2x"    );
		settings.setValue("fullscreen","false" );
	settings.endGroup();
}

/*
 * INICIO MONTAJES DE UNIDADES ----------------------------------------------------------------------------------
 *
*/
// Funcion para Poner nombres cortos en DOS.
// Estado: Beta v 0.0.2
QString Funciones::getShortPathName(QString longPath)
{
/*
 * - Archivos 8 carateres maximo
 * - Nombres sin espacios
 *
*/
	if( longPath != "" )
	{
		int listSize = 0;
		QString str, extension;
		QStringList listShortPath, shortPath;

		shortPath.clear();
		listShortPath.clear();

		QFileInfo fi( longPath );
		if( fi.isFile() )
		{
			extension = "." + fi.completeSuffix();
			listShortPath = longPath.replace("/","\\").remove(extension).split("\\");
		} else
			listShortPath = longPath.replace("/","\\").split("\\");

		listSize = listShortPath.size();
		for( int i = 0; i < listSize; ++i )
		{
			str.clear();
			str = listShortPath.value(i);

			// Contiene espacios
			if( str.contains(" ") )
				shortPath << str.replace(" ","").left(6).append("~1");
			else {
			// No contiene espacios
			// Si es mayor que 8 se covierte.
				if( str.length() > 8 )
					shortPath << str.left(6).append("~1");
				else
					shortPath << str;
			}
		}

		if( fi.isFile() )
			return shortPath.join("\\") + extension;
		else
			return shortPath.join("\\");
	} else
		return "";
}

// Crea la configuración de los Montajes para el DOSBox
QStringList Funciones::CreaConfigMontajes(QTreeWidget *myTreeWidget, const QHash<QString, QString> datos)
{
	QString NombreEXEDbx, DirEXEDbx, Dbx_loadfix, Dbx_cerrardbx;
	QString mount_letra_primario, mount_dir_primario, mount_dir, montaje_boot;
	QString mount_type, mount_drive, mount_letter, mount_label, mount_Options, mount_IOCtrl;
	int num_mount = 0;
	bool montaje_IMG = false;
	bool mount_Boot = false;
	QStringList listmontajes, lista_multiple_iso, lista_isos;

	QFileInfo fi( datos["Dbx_path_exe"] );
	NombreEXEDbx = fi.fileName();		// Nombre del ejecutable
	DirEXEDbx    = fi.absolutePath();	// Directorio donde esta

	if( DirEXEDbx.endsWith("/") )
		DirEXEDbx.replace("/","\\");
	else
		DirEXEDbx.append("/").replace("/","\\");

// loadfix
	if( datos["Dbx_opt_loadfix"] == "true" )
	{
		Dbx_loadfix = "loadfix ";
		if( datos["Dbx_opt_loadfix_mem"] != "" && datos["Dbx_opt_loadfix_mem"] != "64" )
			Dbx_loadfix.append("-"+datos["Dbx_opt_loadfix_mem"]+" ");
	} else
		Dbx_loadfix = "";

// Cerrar DOSBox
	if( datos["Dbx_opt_cerrar_dbox"] == "true" )
		Dbx_cerrardbx = "exit";
	else
		Dbx_cerrardbx = "";

// Montajes
	listmontajes.clear();
	for( num_mount = 0; num_mount < myTreeWidget->topLevelItemCount(); num_mount++ )
	{
		QTreeWidgetItem *item = myTreeWidget->topLevelItem( num_mount );

	// Indicamos el directorio y la letra a montar
		mount_drive  = item->text(0); // Real Drive or Directory or Image ISO, IMA
		mount_letter = item->text(3); // Emulated Drive letter

		if( (item->text(2) == "floppy") || (item->text(2) == "drive") || (item->text(2) == "cdrom") )
		{
			#ifdef Q_OS_WIN32
				mount_drive.replace("/","\\");
				if( !mount_drive.endsWith("\\") )
					mount_drive.append("\\");
			#endif
		} else {
			#ifdef Q_OS_WIN32
				mount_drive.replace("/","\\");
			#endif
		}

	// Situa el montaje primario independiente de donde este colocado
		if( item->text(7) == "v")
		{
			mount_letra_primario = mount_letter;
			mount_dir_primario   = mount_drive;
		} else {
			mount_letra_primario = myTreeWidget->topLevelItem(0)->text(3);
			mount_dir_primario   = myTreeWidget->topLevelItem(0)->text(0);
		}

	//Montando las unidades
		if( item->text(2) != "boot")
		{
			mount_Boot = false;
		// Opciones de montaje
			if( item->text(5) != "" )
				mount_Options = " " + item->text(5);
			else
				mount_Options = "";

		// Etiqueta de las unidades.
			if( item->text(1) != "" )
				mount_label = " -label " + item->text(1);
			else
				mount_label = "";

			if( item->text(6) != "" )
				mount_IOCtrl = " " + item->text(6);
			else
				mount_IOCtrl = "";

		// Montando Disquetera, Disco Duro, CDs, Imagenes de Discos, disquetes y CDs
			if( item->text(2) == "floppy")
			{
				montaje_IMG = false;
				mount_type = " -t floppy";
			}

			if( item->text(2) == "drive" )
			{
				montaje_IMG = false;
				mount_type = "";
			}

			if( item->text(2) == "cdrom" )
			{
				montaje_IMG  = false;
				mount_type  = " -t cdrom";

				if( item->text(4) != "" )
					mount_type.append( " -usecd "+ item->text(4) );
			}

			if( item->text(2) == "IMG_floppy")
			{
				montaje_IMG = true;
				mount_type = " -t floppy";
			}

			if( item->text(2) == "IMG_iso")
			{
				montaje_IMG = true;
				mount_type = " -t iso";
			}

			if( item->text(2) == "IMG_multi_iso")
			{
				montaje_IMG = true;
				mount_type = " -t iso";

				lista_isos.clear();
				lista_isos << mount_drive.split("|", QString::SkipEmptyParts);

				lista_multiple_iso.clear();
				for( int i = 0; i < lista_isos.size(); ++i )
				{
					lista_multiple_iso << "\""+getShortPathName( lista_isos.value(i) )+"\"";
				}
				mount_drive.clear();
				mount_drive = lista_multiple_iso.join(" ");
			}

			if( item->text(2) == "IMG_hdd")
			{
				montaje_IMG = true;
				mount_type = " -t hdd";
			}

			if( montaje_IMG == true )
			{
				if( item->text(2) == "IMG_multi_iso")
					listmontajes << "imgmount " + mount_letter + " " + mount_drive + mount_type + mount_label;
				else
					listmontajes << "imgmount " + mount_letter + " \"" + mount_drive + "\"" + mount_type + mount_label;
			} else
				listmontajes << "mount " + mount_letter + " \"" + mount_drive + "\"" + mount_type + mount_IOCtrl + mount_Options + mount_label;
		} else {
			mount_Boot = true;

		// Montaje multiple del BOOT
			lista_isos.clear();
			lista_isos << mount_drive.split("|", QString::SkipEmptyParts);

			lista_multiple_iso.clear();
			for( int i = 0; i < lista_isos.size(); ++i )
			{
				lista_multiple_iso << "\""+getShortPathName( lista_isos.value(i) )+"\"";
			}
			mount_drive.clear();
			mount_drive = lista_multiple_iso.join(" ");

			montaje_boot = "boot \"" + mount_drive + "\" -l"+mount_letter;
		}
	}

	mount_dir_primario.replace("/","\\");
	if( !mount_dir_primario.endsWith("\\") )
		mount_dir_primario.append("\\");

	mount_dir = DirEXEDbx;
	mount_dir.remove(mount_dir_primario);
//	if( mount_dir == "\\" ) mount_dir.remove(0,1);

	if( mount_Boot == false )
	{
		listmontajes << mount_letra_primario + ":";
		listmontajes << "cd " + getShortPathName( mount_dir );
		listmontajes << Dbx_loadfix + NombreEXEDbx + " " + datos["Dbx_parametros_exe"];
	} else {
		listmontajes << montaje_boot;
	}

	listmontajes << Dbx_cerrardbx;

	return listmontajes;
}

/*
 * FIN MONTAJES DE UNIDADES -------------------------------------------------------------------------------------
 *
*/

// Crea el archivo de configuración del DOSBox
void Funciones::CrearArchivoConfigDbx(const QHash<QString, QString> datos, const QHash<QString, QString> datosDbx, QTreeWidget *treeWidget, const QString pathSaveConfg, bool exportToDFend)
{
	QFile file_out( pathSaveConfg );
	if ( file_out.open(QIODevice::WriteOnly | QIODevice::Text) )
	{
		QSettings settings( GRlidaHomePath()+"/GR-lida.conf", QSettings::IniFormat );
		settings.beginGroup("OpcGeneral");
			QString versionDbx = settings.value("VersionDBx", "0.72").toString();
		settings.endGroup();

		QTextStream out(&file_out);
		out.setCodec("UTF-8");

		if( exportToDFend )
		{
			out << "[Extra]" << endl;
			out << "Exe=" << datosDbx["Dbx_path_exe"] << endl;
			//out << "Environment=" << datosDbx[""] << endl;
			//out << "Tab=" << datosDbx[""] << endl;
			//out << "Tab2=" << datosDbx[""] << endl;
			out << "Loadhigh=" << datosDbx["Dbx_opt_loadfix"] << endl;
			out << "CloseOnExit=" << datosDbx["Dbx_opt_cerrar_dbox"] << endl;
			//out << "AutoMountCDs=" << datosDbx[""] << endl;
			//out << "Overridegamestart=" << datosDbx[""] << endl;
			//out << "OverrideMount=" << datosDbx[""] << endl;
			//out << "LoadFixVal=" << datosDbx["Dbx_opt_loadfix_mem"] << endl;
			//out << "ExeMD5=" << datosDbx[""] << endl;
			out << "Setup=" << datosDbx["Dbx_path_setup"] << endl;
			//out << "SetupMD5=" << datosDbx[""] << endl;
			out << "GameParameters=" << datosDbx["Dbx_parametros_exe"] << endl;
			out << "SetupParameters=" << datosDbx["Dbx_parametros_setup"] << endl;
			//out << "ExtraDirs=" << datosDbx[""] << endl;
			//out << "LastModification=" << datosDbx[""] << endl;
			//out << "DataDir=" << datosDbx[""] << endl;
			out << "NrOfMounts=" << treeWidget->topLevelItemCount() << endl;
			for(int num_mount = 0; num_mount < treeWidget->topLevelItemCount(); num_mount++ )
			{
				QTreeWidgetItem *item = treeWidget->topLevelItem( num_mount );
				out << num_mount << "=" << item->text(0) << ";" << item->text(2) << ";" << item->text(3) << ";" << item->text(6) << endl;
			}
			out << "autoexec=" << datosDbx["Dbx_autoexec"] << endl << endl;
			//out << "BootImage=" << datosDbx[""] << endl;
			//out << "Finalization=" << datosDbx[""] << endl;
			//out << "CustomSettings=" << datosDbx[""] << endl;

			QString notes;
			notes.clear();
			notes = datos["Dat_comentario"];
			notes.replace("<br/>\n","[13][10]").replace("<br/>","[13]").replace("\n","[13]");

			out << "[ExtraInfo]" << endl;
			out << "Genre=" << datos["Dat_genero"] << endl;
			out << "Developer=" << datos["Dat_desarrollador"] << endl;
			out << "Publisher=" << datos["Dat_compania"] << endl;
			out << "Year=" << datos["Dat_anno"] << endl;
			out << "Language=" << datos["Dat_idioma"] << endl;
			//out << "UserInfo=" << datos["Dat_"] << endl;
			out << "Favorite=" << datos["Dat_favorito"] << endl;
			out << "Name=" << datos["Dat_titulo"] << endl;
			out << "Icon=" << datos["Dat_icono"] << endl;
			out << "Notes=" << notes << endl << endl;
			//out << "WWW=" << datosDbx[""] << endl;
		}

		out << "# This is the configurationfile for DOSBox " << versionDbx << "." << endl;
		out << "# Lines starting with a # are commentlines and are ignored by DOSBox." << endl;
		out << "# They are used to (briefly) document the effect of each option." << endl << endl;

		out << "[sdl]" << endl;
		out << "#       fullscreen: Start dosbox directly in fullscreen. (Press ALT-Enter to go back)" << endl;
		out << "#       fulldouble: Use double buffering in fullscreen. It can reduce screen flickering, but it can also result in a slow DOSBox." << endl;
		out << "#   fullresolution: What resolution to use for fullscreen: original or fixed size (e.g. 1024x768)." << endl;
		out << "#                     Using your monitor's native resolution with aspect=true might give the best results." << endl;
		out << "#                     If you end up with small window on a large screen, try an output different from surface." << endl;
		out << "# windowresolution: Scale the window to this size IF the output device supports hardware scaling." << endl;
		out << "#                     (output=surface does not!)" << endl;
		out << "#           output: What video system to use for output." << endl;
		out << "#                   Possible values: surface, overlay, opengl, openglnb, ddraw." << endl;
		out << "#         autolock: Mouse will automatically lock, if you click on the screen. (Press CTRL-F10 to unlock)" << endl;
		out << "#      sensitivity: Mouse sensitivity." << endl;
		out << "#      waitonerror: Wait before closing the console if dosbox has an error." << endl;
		out << "#         priority: Priority levels for dosbox. Second entry behind the comma is for when dosbox is not focused/minimized." << endl;
		out << "#                     pause is only valid for the second entry." << endl;
		out << "#                   Possible values: lowest, lower, normal, higher, highest, pause." << endl;
		out << "#       mapperfile: File used to load/save the key/event mappings from. Resetmapper only works with the defaul value." << endl;
		out << "#     usescancodes: Avoid usage of symkeys, might not work on all operating systems." << endl << endl;

		out << "fullscreen=" << datosDbx["Dbx_sdl_fullscreen"] << endl;
		out << "fulldouble=" << datosDbx["Dbx_sdl_fulldouble"] << endl;
//		out << "fullfixed=" << datosDbx["Dbx_sdl_fullfixed"] << endl; // DOSBox 0.63 ya en desuso
		out << "fullresolution=" << datosDbx["Dbx_sdl_fullresolution"] << endl;
		out << "windowresolution=" << datosDbx["Dbx_sdl_windowresolution"] << endl;
		out << "output=" << datosDbx["Dbx_sdl_output"] << endl;
		if( versionDbx <= "0.73" )
			out << "hwscale=" << datosDbx["Dbx_sdl_hwscale"] << endl; // DOSBox 0.63
		out << "autolock=" << datosDbx["Dbx_sdl_autolock"] << endl;
		out << "sensitivity=" << datosDbx["Dbx_sdl_sensitivity"] << endl;
		out << "waitonerror=" << datosDbx["Dbx_sdl_waitonerror"] << endl;
		out << "priority=" << datosDbx["Dbx_sdl_priority"] << endl;
		out << "mapperfile=" << datosDbx["Dbx_sdl_mapperfile"] << endl;
		out << "usescancodes=" << datosDbx["Dbx_sdl_usescancodes"] << endl << endl;

		out << "[dosbox]" << endl;
		out << "# language: Select another language file." << endl;
		out << "#  machine: The type of machine tries to emulate." << endl;
		out << "#           Possible values: hercules, cga, tandy, pcjr, ega, vgaonly, svga_s3, svga_et3000, svga_et4000, svga_paradise, vesa_nolfb, vesa_oldvbe." << endl;
		out << "# captures: Directory where things like wave, midi, screenshot get captured." << endl;
		out << "#  memsize: Amount of memory DOSBox has in megabytes." << endl;
		out << "#             This value is best left at its default to avoid problems with some games," << endl;
		out << "#             though few games might require a higher value." << endl;
		out << "#             There is generally no speed advantage when raising this value." << endl << endl;

		if( !datosDbx["Dbx_dosbox_language"].isEmpty() || datosDbx["Dbx_dosbox_language"]!=" ")
		out << "language=" << datosDbx["Dbx_dosbox_language"] << endl;
		out << "machine=" << datosDbx["Dbx_dosbox_machine"] << endl;
		out << "captures=" << datosDbx["Dbx_dosbox_captures"] << endl;
		out << "memsize=" << datosDbx["Dbx_dosbox_memsize"] << endl << endl;

		out << "[render]" << endl;
		out << "# frameskip: How many frames DOSBox skips before drawing one." << endl;
		out << "#    aspect: Do aspect correction, if your output method doesn't support scaling this can slow things down!." << endl;
		out << "#    scaler: Scaler used to enlarge/enhance low resolution modes." << endl;
		out << "#              If 'forced' is appended, then the scaler will be used even if the result might not be desired." << endl;
		out << "#            Possible values: none, normal2x, normal3x, advmame2x, advmame3x, advinterp2x, advinterp3x, hq2x, hq3x, 2xsai, super2xsai, supereagle, tv2x, tv3x, rgb2x, rgb3x, scan2x, scan3x." << endl << endl;

		out << "frameskip=" << datosDbx["Dbx_render_frameskip"] << endl;
		out << "aspect=" << datosDbx["Dbx_render_aspect"] << endl;
		out << "scaler=" << datosDbx["Dbx_render_scaler"] << endl << endl;

		out << "[cpu]" << endl;
		out << "#      core: CPU Core used in emulation. auto will switch to dynamic if available and appropriate." << endl;
		out << "#            Possible values: auto, dynamic, normal, simple." << endl;
		out << "#   cputype: CPU Type used in emulation. auto is the fastest choice." << endl;
		out << "#            Possible values: auto, 386, 386_slow, 486_slow, pentium_slow, 386_prefetch." << endl;
		out << "#    cycles: Amount of instructions DOSBox tries to emulate each millisecond." << endl;
		out << "#            Setting this value too high results in sound dropouts and lags." << endl;
		out << "#            Cycles can be set in 3 ways:" << endl;
		out << "#              'auto'          tries to guess what a game needs." << endl;
		out << "#                              It usually works, but can fail for certain games." << endl;
		out << "#              'fixed #number' will set a fixed amount of cycles. This is what you usually need if 'auto' fails." << endl;
		out << "#                              (Example: fixed 4000)." << endl;
		out << "#              'max'           will allocate as much cycles as your computer is able to handle." << endl;
		out << "#            " << endl;
		out << "#            Possible values: auto, fixed, max." << endl;
		out << "#   cycleup: Amount of cycles to decrease/increase with keycombo.(CTRL-F11/CTRL-F12)" << endl;
		out << "# cycledown: Setting it lower than 100 will be a percentage." << endl << endl;

		out << "core=" << datosDbx["Dbx_cpu_core"] << endl;
		out << "cputype=" << datosDbx["Dbx_cpu_cputype"] << endl;

		if( datosDbx["Dbx_cpu_cycles"] != "auto" && datosDbx["Dbx_cpu_cycles"] != "max" && versionDbx >= "0.73")
			out << "cycles=fixed " << datosDbx["Dbx_cpu_cycles"] << endl;
		else
			out << "cycles=" << datosDbx["Dbx_cpu_cycles"] << endl;

		out << "cycleup=" << datosDbx["Dbx_cpu_cycleup"] << endl;
		out << "cycledown=" << datosDbx["Dbx_cpu_cycledown"] << endl << endl;

		out << "[mixer]" << endl;
		out << "#   nosound: Enable silent mode, sound is still emulated though." << endl;
		out << "#      rate: Mixer sample rate, setting any device's rate higher than this will probably lower their sound quality." << endl;
		out << "#            Possible values: 44100, 48000, 32000, 22050, 16000, 11025, 8000, 49716." << endl;
		out << "# blocksize: Mixer block size, larger blocks might help sound stuttering but sound will also be more lagged." << endl;
		out << "#            Possible values: 1024, 2048, 4096, 8192, 512, 256." << endl;
		out << "# prebuffer: How many milliseconds of data to keep on top of the blocksize." << endl << endl;

		out << "nosound=" << datosDbx["Dbx_mixer_nosound"] << endl;
		out << "rate=" << datosDbx["Dbx_mixer_rate"] << endl;
		out << "blocksize=" << datosDbx["Dbx_mixer_blocksize"] << endl;
		out << "prebuffer=" << datosDbx["Dbx_mixer_prebuffer"] << endl << endl;

		out << "[midi]" << endl;
		out << "#     mpu401: Type of MPU-401 to emulate." << endl;
		out << "#             Possible values: intelligent, uart, none." << endl;
		out << "# mididevice: Device that will receive the MIDI data from MPU-401." << endl;
		out << "#             Possible values: default, win32, alsa, oss, coreaudio, coremidi, none." << endl;
		out << "# midiconfig: Special configuration options for the device driver. This is usually the id of the device you want to use." << endl;
		out << "#               See the README/Manual for more details." << endl << endl;

		out << "mpu401=" << datosDbx["Dbx_midi_mpu401"] << endl;

		if( versionDbx >= "0.73" )
		{
			out << "mididevice=" << datosDbx["Dbx_midi_device"] << endl;
			out << "midiconfig=" << datosDbx["Dbx_midi_config"] << endl << endl;
		} else {
			out << "intelligent=" << datosDbx["Dbx_midi_intelligent"] << endl; // DOSBox 0.63
			out << "device=" << datosDbx["Dbx_midi_device"] << endl;
			out << "config=" << datosDbx["Dbx_midi_config"] << endl;
			out << "mt32rate=" << datosDbx["Dbx_midi_mt32rate"] << endl << endl; // DOSBox 0.63
		}

		out << "[sblaster]" << endl;
		out << "#  sbtype: Type of Soundblaster to emulate. gb is Gameblaster." << endl;
		out << "#          Possible values: sb1, sb2, sbpro1, sbpro2, sb16, gb, none." << endl;
		out << "#  sbbase: The IO address of the soundblaster." << endl;
		out << "#          Possible values: 220, 240, 260, 280, 2a0, 2c0, 2e0, 300." << endl;
		out << "#     irq: The IRQ number of the soundblaster." << endl;
		out << "#          Possible values: 7, 5, 3, 9, 10, 11, 12." << endl;
		out << "#     dma: The DMA number of the soundblaster." << endl;
		out << "#          Possible values: 1, 5, 0, 3, 6, 7." << endl;
		out << "#    hdma: The High DMA number of the soundblaster." << endl;
		out << "#          Possible values: 1, 5, 0, 3, 6, 7." << endl;
		out << "# sbmixer: Allow the soundblaster mixer to modify the DOSBox mixer." << endl;
		out << "# oplmode: Type of OPL emulation. On 'auto' the mode is determined by sblaster type. All OPL modes are Adlib-compatible, except for 'cms'." << endl;
		out << "#          Possible values: auto, cms, opl2, dualopl2, opl3, none." << endl;
		out << "#  oplemu: Provider for the OPL emulation. compat might provide better quality (see oplrate as well)." << endl;
		out << "#          Possible values: default, compat, fast." << endl;
		out << "# oplrate: Sample rate of OPL music emulation. Use 49716 for highest quality (set the mixer rate accordingly)." << endl;
		out << "#          Possible values: 44100, 49716, 48000, 32000, 22050, 16000, 11025, 8000." << endl << endl;

		out << "sbtype=" << datosDbx["Dbx_sblaster_sbtype"] << endl;
		out << "sbbase=" << datosDbx["Dbx_sblaster_sbbase"] << endl;
		out << "irq=" << datosDbx["Dbx_sblaster_irq"] << endl;
		out << "dma=" << datosDbx["Dbx_sblaster_dma"] << endl;
		out << "hdma=" << datosDbx["Dbx_sblaster_hdma"] << endl;

		if( versionDbx >= "0.73" )
			out << "sbmixer=" << datosDbx["Dbx_sblaster_mixer"] << endl;
		else
			out << "mixer=" << datosDbx["Dbx_sblaster_mixer"] << endl;

		out << "oplmode=" << datosDbx["Dbx_sblaster_oplmode"] << endl;
		out << "oplemu=" << datosDbx["Dbx_sblaster_oplemu"] << endl;
		out << "oplrate=" << datosDbx["Dbx_sblaster_oplrate"] << endl << endl;

		out << "[gus]" << endl;
		out << "#      gus: Enable the Gravis Ultrasound emulation." << endl;
		out << "#  gusrate: Sample rate of Ultrasound emulation." << endl;
		out << "#           Possible values: 44100, 48000, 32000, 22050, 16000, 11025, 8000, 49716." << endl;
		out << "#  gusbase: The IO base address of the Gravis Ultrasound." << endl;
		out << "#           Possible values: 240, 220, 260, 280, 2a0, 2c0, 2e0, 300." << endl;
		out << "#   gusirq: The IRQ number of the Gravis Ultrasound." << endl;
		out << "#           Possible values: 5, 3, 7, 9, 10, 11, 12." << endl;
		out << "#   gusdma: The DMA channel of the Gravis Ultrasound." << endl;
		out << "#           Possible values: 3, 0, 1, 5, 6, 7." << endl;
		out << "# ultradir: Path to Ultrasound directory. In this directory" << endl;
		out << "#           there should be a MIDI directory that contains" << endl;
		out << "#           the patch files for GUS playback. Patch sets used" << endl;
		out << "#           with Timidity should work fine." << endl << endl;

		out << "gus=" << datosDbx["Dbx_gus_gus"] << endl;
		out << "gusrate=" << datosDbx["Dbx_gus_gusrate"] << endl;
		out << "gusbase=" << datosDbx["Dbx_gus_gusbase"] << endl;
		if( versionDbx >= "0.73" )
		{
			out << "gusirq=" << datosDbx["Dbx_gus_irq1"] << endl;
			out << "gusdma=" << datosDbx["Dbx_gus_dma1"] << endl;
		} else {
			out << "irq1=" << datosDbx["Dbx_gus_irq1"] << endl;
			out << "irq2=" << datosDbx["Dbx_gus_irq2"] << endl;
			out << "dma1=" << datosDbx["Dbx_gus_dma1"] << endl;
			out << "dma2=" << datosDbx["Dbx_gus_dma2"] << endl;
		}
		out << "ultradir=" << datosDbx["Dbx_gus_ultradir"] << endl << endl;

		out << "[speaker]" << endl;
		out << "# pcspeaker: Enable PC-Speaker emulation." << endl;
		out << "#    pcrate: Sample rate of the PC-Speaker sound generation." << endl;
		out << "#            Possible values: 44100, 48000, 32000, 22050, 16000, 11025, 8000, 49716." << endl;
		out << "#     tandy: Enable Tandy Sound System emulation. For 'auto', emulation is present only if machine is set to 'tandy'." << endl;
		out << "#            Possible values: auto, on, off." << endl;
		out << "# tandyrate: Sample rate of the Tandy 3-Voice generation." << endl;
		out << "#            Possible values: 44100, 48000, 32000, 22050, 16000, 11025, 8000, 49716." << endl;
		out << "#    disney: Enable Disney Sound Source emulation. (Covox Voice Master and Speech Thing compatible)." << endl << endl;

		out << "pcspeaker=" << datosDbx["Dbx_speaker_pcspeaker"] << endl;
		out << "pcrate=" << datosDbx["Dbx_speaker_pcrate"] << endl;
		out << "tandy=" << datosDbx["Dbx_speaker_tandy"] << endl;
		out << "tandyrate=" << datosDbx["Dbx_speaker_tandyrate"] << endl;
		out << "disney=" << datosDbx["Dbx_speaker_disney"] << endl << endl;

		out << "[joystick]" << endl;
		out << "# joysticktype: Type of joystick to emulate: auto (default), none," << endl;
		out << "#               2axis (supports two joysticks)," << endl;
		out << "#               4axis (supports one joystick, first joystick used)," << endl;
		out << "#               4axis_2 (supports one joystick, second joystick used)," << endl;
		out << "#               fcs (Thrustmaster), ch (CH Flightstick)." << endl;
		out << "#               none disables joystick emulation." << endl;
		out << "#               auto chooses emulation depending on real joystick(s)." << endl;
		out << "#               (Remember to reset dosbox's mapperfile if you saved it earlier)" << endl;
		out << "#               Possible values: auto, 2axis, 4axis, 4axis_2, fcs, ch, none." << endl;
		out << "#        timed: enable timed intervals for axis. Experiment with this option, if your joystick drifts (away)." << endl;
		out << "#     autofire: continuously fires as long as you keep the button pressed." << endl;
		out << "#       swap34: swap the 3rd and the 4th axis. can be useful for certain joysticks." << endl;
		out << "#   buttonwrap: enable button wrapping at the number of emulated buttons." << endl << endl;

		out << "joysticktype=" << datosDbx["Dbx_joystick_type"] << endl;
		out << "timed=" << datosDbx["Dbx_joystick_timed"] << endl;
		out << "autofire=" << datosDbx["Dbx_joystick_autofire"] << endl;
		out << "swap34=" << datosDbx["Dbx_joystick_swap34"] << endl;
		out << "buttonwrap=" << datosDbx["Dbx_joystick_buttonwrap"] << endl << endl;

		if( versionDbx <= "0.73" )
		{
			out << "[modem]" << endl; // DOSBox 0.63
			out << "modem=" << datosDbx["Dbx_modem_modem"] << endl;
			out << "comport=" << datosDbx["Dbx_modem_comport"] << endl;
			out << "listenport=" << datosDbx["Dbx_modem_listenport"] << endl << endl;

			out << "[directserial]" << endl; // DOSBox 0.63
			out << "directserial=" << datosDbx["Dbx_dserial_directserial"] << endl;
			out << "comport=" << datosDbx["Dbx_dserial_comport"] << endl;
			out << "realport=" << datosDbx["Dbx_dserial_realport"] << endl;
			out << "defaultbps=" << datosDbx["Dbx_dserial_defaultbps"] << endl;
			out << "parity=" << datosDbx["Dbx_dserial_parity"] << endl;
			out << "bytesize=" << datosDbx["Dbx_dserial_bytesize"] << endl;
			out << "stopbit=" << datosDbx["Dbx_dserial_stopbit"] << endl << endl;
		}
		out << "[serial]" << endl;
		out << "# serial1: set type of device connected to com port." << endl;
		out << "#          Can be disabled, dummy, modem, nullmodem, directserial." << endl;
		out << "#          Additional parameters must be in the same line in the form of" << endl;
		out << "#          parameter:value. Parameter for all types is irq (optional)." << endl;
		out << "#          for directserial: realport (required), rxdelay (optional)." << endl;
		out << "#                           (realport:COM1 realport:ttyS0)." << endl;
		out << "#          for modem: listenport (optional)." << endl;
		out << "#          for nullmodem: server, rxdelay, txdelay, telnet, usedtr," << endl;
		out << "#                         transparent, port, inhsocket (all optional)." << endl;
		out << "#          Example: serial1=modem listenport:5000" << endl;
		out << "#          Possible values: dummy, disabled, modem, nullmodem, directserial." << endl;
		out << "# serial2: see serial1" << endl;
		out << "#          Possible values: dummy, disabled, modem, nullmodem, directserial" << endl;
		out << "# serial3: see serial1" << endl;
		out << "#          Possible values: dummy, disabled, modem, nullmodem, directserial." << endl;
		out << "# serial4: see serial1" << endl;
		out << "#          Possible values: dummy, disabled, modem, nullmodem, directserial." << endl << endl;

		out << "serial1=" << datosDbx["Dbx_serial_1"] << endl;
		out << "serial2=" << datosDbx["Dbx_serial_2"] << endl;
		out << "serial3=" << datosDbx["Dbx_serial_3"] << endl;
		out << "serial4=" << datosDbx["Dbx_serial_4"] << endl << endl;

		out << "[dos]" << endl;
		out << "#            xms: Enable XMS support." << endl;
		out << "#            ems: Enable EMS support." << endl;
		out << "#            umb: Enable UMB support." << endl;
		out << "# keyboardlayout: Language code of the keyboard layout (or none)." << endl << endl;

		out << "xms=" << datosDbx["Dbx_dos_xms"] << endl;
		out << "ems=" << datosDbx["Dbx_dos_ems"] << endl;
		out << "umb=" << datosDbx["Dbx_dos_umb"] << endl;
		out << "keyboardlayout=" << datosDbx["Dbx_dos_keyboardlayout"] << endl << endl;

		out << "[ipx]" << endl;
		out << "# ipx: Enable ipx over UDP/IP emulation." << endl << endl;

		out << "ipx=" << datosDbx["Dbx_ipx_ipx"] << endl << endl;

		if( !exportToDFend )
		{
			out << "[autoexec]" << endl;
			out << "# Lines in this section will be run at startup." << endl;
			out << "# You can put your MOUNT lines here." << endl << endl;

			if( datosDbx["Dbx_opt_autoexec"] == "true" )
				out << datosDbx["Dbx_autoexec"] << endl;
			else {
				// Creando el Autoexec
				QStringList listamontaje;
				QHash<QString, QString> datos_montaje;
				int i = 0;

				datos_montaje.clear();
				datos_montaje["Dbx_path_exe"]        = datosDbx["Dbx_path_exe"];
				datos_montaje["Dbx_parametros_exe"]  = datosDbx["Dbx_parametros_exe"];
				datos_montaje["Dbx_opt_loadfix_mem"] = datosDbx["Dbx_opt_loadfix_mem"];
				datos_montaje["Dbx_opt_loadfix"]     = datosDbx["Dbx_opt_loadfix"];
				datos_montaje["Dbx_opt_cerrar_dbox"] = datosDbx["Dbx_opt_cerrar_dbox"];

				listamontaje.clear();
				listamontaje = CreaConfigMontajes( treeWidget, datos_montaje );

				for(i = 0; i < listamontaje.size(); ++i)
					out << listamontaje.value( i ) << endl;
			}
		}

		out.flush();
		file_out.close();
	}
}

// Crea el archivo de configuración del VDMSound
void Funciones::CrearArchivoConfigVdmS(const QHash<QString, QString> datosVdms, const QString pathSaveConfg)
{
	QSettings * settings = new QSettings(pathSaveConfg, QSettings::IniFormat);
	QFileInfo workdir( datosVdms["Vdms_path_exe"] );

	settings->beginGroup("program");
		settings->setValue("executable"  , datosVdms["Vdms_path_exe"]  );
		settings->setValue("workdir"     , workdir.absolutePath() );
		settings->setValue("params"      , datosVdms["Vdms_program_1"] );
		settings->setValue("icon"        , datosVdms["Vdms_program_2"] );
	settings->endGroup();

	settings->beginGroup("vdms.debug");
		settings->setValue("useCustomCfg", datosVdms["Vdms_vdms_debug_1"] );
		settings->setValue("customCfg"   , datosVdms["Vdms_vdms_debug_2"] );
	settings->endGroup();

	settings->beginGroup("winnt.dos");
		settings->setValue("useAutoexec" , datosVdms["Vdms_winnt_dos_1"] );
		settings->setValue("autoexec"    , datosVdms["Vdms_winnt_dos_2"] );
	settings->endGroup();

	settings->beginGroup("winnt.dosbox");
		settings->setValue("exitclose"   , datosVdms["Vdms_winnt_dosbox_1"] );
		settings->setValue("exitWarn"    , datosVdms["Vdms_winnt_dosbox_2"] );
		settings->setValue("fastPaste"   , datosVdms["Vdms_winnt_dosbox_3"] );
	settings->endGroup();

	settings->beginGroup("winnt.storage");
		settings->setValue("useCDROM"    , datosVdms["Vdms_winnt_storage_1"] );
		settings->setValue("useNetware"  , datosVdms["Vdms_winnt_storage_2"] );
	settings->endGroup();

	delete settings;
}

// Exportar la configuracion del DOSBox para el GR-lida
void Funciones::Exportar_Profile_GRLida(const QHash<QString, QString> datos, const QHash<QString, QString> datos_emu, QTreeWidget *treeWidget, const QString pathSaveConfg, int exportGRLVersion)
{
	QString strDat, strDbx, strSvm, strVdms, str_dosbox, strDbxMount;
	if(exportGRLVersion==2)
	{
		strDat  = "Dat_";
		strDbx  = "Dbx_";
		strSvm  = "Svm_";
		strVdms = "Vdms_";
		str_dosbox = "dosbox_";
		strDbxMount = "Dbx_mount_";
	} else {
		strDat  = "";
		strDbx  = "";
		strSvm  = "";
		strVdms = "";
		str_dosbox = "";
		strDbxMount = "";
	}

	QFile file_out( pathSaveConfg );
	if ( file_out.open(QIODevice::WriteOnly | QIODevice::Text) )
	{
		QTextStream out(&file_out);
		out.setCodec("UTF-8");

		out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << endl;
		out << "<grlida_db version=\"0.1.0\">" << endl;
		out << "  <lista_db>" << endl;
		out << "    <title>Export GR-lida</title>" << endl;
		out << "    <link></link>" << endl;
		out << "    <description></description>" << endl;
		out << "    <lastBuildDate>" << HoraFechaActual() << "</lastBuildDate>" << endl;
//		out << "    <language>es-ES</language>" << endl;
		out << "    <copyright>Copyright 2003-2010</copyright>" << endl;

		out << "    <datos>" << endl;
		out << "      <"+strDat+"usuario>" <<  datos["Dat_usuario"] << "</"+strDat+"usuario>" << endl;
		out << "      <"+strDat+"icono>" << datos["Dat_icono"] << "</"+strDat+"icono>" << endl;
		out << "      <"+strDat+"titulo>" <<  datos["Dat_titulo"] << "</"+strDat+"titulo>" << endl;
		out << "      <"+strDat+"subtitulo>" <<  datos["Dat_subtitulo"] << "</"+strDat+"subtitulo>" << endl;
		out << "      <"+strDat+"genero>" << datos["Dat_genero"] << "</"+strDat+"genero>" << endl;
		out << "      <"+strDat+"compania>" << datos["Dat_compania"] << "</"+strDat+"compania>" << endl;
		out << "      <"+strDat+"desarrollador>" << datos["Dat_desarrollador"] << "</"+strDat+"desarrollador>" << endl;
		out << "      <"+strDat+"tema>" << datos["Dat_tema"] << "</"+strDat+"tema>" << endl;
		out << "      <"+strDat+"perspectiva>" << datos["Dat_perspectiva"] << "</"+strDat+"perspectiva>" << endl;
		out << "      <"+strDat+"idioma>" << datos["Dat_idioma"] << "</"+strDat+"idioma>" << endl;
		out << "      <"+strDat+"formato>" << datos["Dat_formato"] << "</"+strDat+"formato>" << endl;
		out << "      <"+strDat+"anno>" <<  datos["Dat_anno"] << "</"+strDat+"anno>" << endl;
		out << "      <"+strDat+"numdisc>" <<  datos["Dat_numdisc"] << "</"+strDat+"numdisc>" << endl;
		out << "      <"+strDat+"sistemaop>" << datos["Dat_sistemaop"] << "</"+strDat+"sistemaop>" << endl;
		out << "      <"+strDat+"tamano>" << datos["Dat_tamano"] << "</"+strDat+"tamano>" << endl;
		out << "      <"+strDat+"graficos>" << datos["Dat_graficos"] << "</"+strDat+"graficos>" << endl;
		out << "      <"+strDat+"sonido>" << datos["Dat_sonido"] << "</"+strDat+"sonido>" << endl;
		out << "      <"+strDat+"jugabilidad>" << datos["Dat_jugabilidad"] << "</"+strDat+"jugabilidad>" << endl;
		out << "      <"+strDat+"original>" << datos["Dat_original"] << "</"+strDat+"original>" << endl;
		out << "      <"+strDat+"estado>" << datos["Dat_estado"] << "</"+strDat+"estado>" << endl;
		out << "      <"+strDat+"thumbs>" << datos["Dat_thumbs"] << "</"+strDat+"thumbs>" << endl;
		out << "      <"+strDat+"cover_front>" << datos["Dat_cover_front"] << "</"+strDat+"cover_front>" << endl;
		out << "      <"+strDat+"cover_back>" << datos["Dat_cover_back"] << "</"+strDat+"cover_back>" << endl;
		out << "      <"+strDat+"fecha>" << datos["Dat_fecha"] << "</"+strDat+"fecha>" << endl;
		out << "      <"+strDat+"tipo_emu>" << datos["Dat_tipo_emu"] << "</"+strDat+"tipo_emu>" << endl;
		out << "      <"+strDat+"comentario>" << datos["Dat_comentario"] << "</"+strDat+"comentario>" << endl;
		out << "      <"+strDat+"favorito>" << datos["Dat_favorito"] << "</"+strDat+"favorito>" << endl;
		out << "      <"+strDat+"rating>" << datos["Dat_rating"] << "</"+strDat+"rating>" << endl;
		out << "      <"+strDat+"edad_recomendada>" << datos["Dat_edad_recomendada"] << "</"+strDat+"edad_recomendada>" << endl;
		out << "      <"+strDat+"path_exe>" << datos["Dat_path_exe"] << "</"+strDat+"path_exe>" << endl;
		out << "      <"+strDat+"parametros_exe>" << datos["Dat_parametros_exe"] << "</"+strDat+"parametros_exe>" << endl;
		out << "    </datos>" << endl;

		if( datos["Dat_tipo_emu"] == "dosbox" )
		{
			out << "    <dosbox>" << endl;
//			out << "      <"+strDbx+"usuario>" <<  datos_emu["user_name"] << "</"+strDbx+"usuario>" << endl;
			out << "      <"+strDbx+"sdl_fullscreen>" << datos_emu["Dbx_sdl_fullscreen"] << "</"+strDbx+"sdl_fullscreen>" << endl;
			out << "      <"+strDbx+"sdl_fulldouble>" << datos_emu["Dbx_sdl_fulldouble"] << "</"+strDbx+"sdl_fulldouble>" << endl;
//			out << "      <"+strDbx+"sdl_fullfixed>" << datos_emu["Dbx_sdl_fullfixed"] << "</"+strDbx+"sdl_fullfixed>" << endl; // en desuso
			out << "      <"+strDbx+"sdl_fullresolution>" << datos_emu["Dbx_sdl_fullresolution"] << "</"+strDbx+"sdl_fullresolution>" << endl;
			out << "      <"+strDbx+"sdl_windowresolution>" << datos_emu["Dbx_sdl_windowresolution"] << "</"+strDbx+"sdl_windowresolution>" << endl;
			out << "      <"+strDbx+"sdl_output>" << datos_emu["Dbx_sdl_output"] << "</"+strDbx+"sdl_output>" << endl;
			out << "      <"+strDbx+"sdl_hwscale>" << datos_emu["Dbx_sdl_hwscale"] << "</"+strDbx+"sdl_hwscale>" << endl;
			out << "      <"+strDbx+"sdl_autolock>" << datos_emu["Dbx_sdl_autolock"] << "</"+strDbx+"sdl_autolock>" << endl;
			out << "      <"+strDbx+"sdl_sensitivity>" << datos_emu["Dbx_sdl_sensitivity"] << "</"+strDbx+"sdl_sensitivity>" << endl;
			out << "      <"+strDbx+"sdl_waitonerror>" << datos_emu["Dbx_sdl_waitonerror"] << "</"+strDbx+"sdl_waitonerror>" << endl;
			out << "      <"+strDbx+"sdl_priority>" << datos_emu["Dbx_sdl_priority"] << "</"+strDbx+"sdl_priority>" << endl;
			out << "      <"+strDbx+"sdl_mapperfile>" << datos_emu["Dbx_sdl_mapperfile"] << "</"+strDbx+"sdl_mapperfile>" << endl;
			out << "      <"+strDbx+"sdl_usescancodes>" << datos_emu["Dbx_sdl_usescancodes"] << "</"+strDbx+"sdl_usescancodes>" << endl;
			out << "      <"+strDbx+"dosbox_language>" << datos_emu["Dbx_dosbox_language"] << "</"+strDbx+"dosbox_language>" << endl;
			out << "      <"+strDbx+"dosbox_machine>" << datos_emu["Dbx_dosbox_machine"] << "</"+strDbx+"dosbox_machine>" << endl;
			out << "      <"+strDbx+"dosbox_captures>" << datos_emu["Dbx_dosbox_captures"] << "</"+strDbx+"dosbox_captures>" << endl;
			out << "      <"+strDbx+"dosbox_memsize>" << datos_emu["Dbx_dosbox_memsize"] << "</"+strDbx+"dosbox_memsize>" << endl;
			out << "      <"+strDbx+"render_frameskip>" << datos_emu["Dbx_render_frameskip"] << "</"+strDbx+"render_frameskip>" << endl;
			out << "      <"+strDbx+"render_aspect>" << datos_emu["Dbx_render_aspect"] << "</"+strDbx+"render_aspect>" << endl;
			out << "      <"+strDbx+"render_scaler>" << datos_emu["Dbx_render_scaler"] << "</"+strDbx+"render_scaler>" << endl;
			out << "      <"+strDbx+"cpu_core>" << datos_emu["Dbx_cpu_core"] << "</"+strDbx+"cpu_core>" << endl;
			out << "      <"+strDbx+"cpu_cputype>" << datos_emu["Dbx_cpu_cputype"] << "</"+strDbx+"cpu_cputype>" << endl;
			out << "      <"+strDbx+"cpu_cycles>" << datos_emu["Dbx_cpu_cycles"] << "</"+strDbx+"cpu_cycles>" << endl;
			out << "      <"+strDbx+"cpu_cycleup>" << datos_emu["Dbx_cpu_cycleup"] << "</"+strDbx+"cpu_cycleup>" << endl;
			out << "      <"+strDbx+"cpu_cycledown>" << datos_emu["Dbx_cpu_cycledown"] << "</"+strDbx+"cpu_cycledown>" << endl;
			out << "      <"+strDbx+"mixer_nosound>" << datos_emu["Dbx_mixer_nosound"] << "</"+strDbx+"mixer_nosound>" << endl;
			out << "      <"+strDbx+"mixer_rate>" << datos_emu["Dbx_mixer_rate"] << "</"+strDbx+"mixer_rate>" << endl;
			out << "      <"+strDbx+"mixer_blocksize>" << datos_emu["Dbx_mixer_blocksize"] << "</"+strDbx+"mixer_blocksize>" << endl;
			out << "      <"+strDbx+"mixer_prebuffer>" << datos_emu["Dbx_mixer_prebuffer"] << "</"+strDbx+"mixer_prebuffer>" << endl;
			out << "      <"+strDbx+"midi_mpu401>" << datos_emu["Dbx_midi_mpu401"] << "</"+strDbx+"midi_mpu401>" << endl;
			out << "      <"+strDbx+"midi_intelligent>" << datos_emu["Dbx_midi_intelligent"] << "</"+strDbx+"midi_intelligent>" << endl;
			out << "      <"+strDbx+"midi_device>" << datos_emu["Dbx_midi_device"] << "</"+strDbx+"midi_device>" << endl;
			out << "      <"+strDbx+"midi_config>" << datos_emu["Dbx_midi_config"] << "</"+strDbx+"midi_config>" << endl;
			out << "      <"+strDbx+"midi_mt32rate>" << datos_emu["Dbx_midi_mt32rate"] << "</"+strDbx+"midi_mt32rate>" << endl;
			out << "      <"+strDbx+"sblaster_sbtype>" << datos_emu["Dbx_sblaster_sbtype"] << "</"+strDbx+"sblaster_sbtype>" << endl;
			out << "      <"+strDbx+"sblaster_sbbase>" << datos_emu["Dbx_sblaster_sbbase"] << "</"+strDbx+"sblaster_sbbase>" << endl;
			out << "      <"+strDbx+"sblaster_irq>" << datos_emu["Dbx_sblaster_irq"] << "</"+strDbx+"sblaster_irq>" << endl;
			out << "      <"+strDbx+"sblaster_dma>" << datos_emu["Dbx_sblaster_dma"] << "</"+strDbx+"sblaster_dma>" << endl;
			out << "      <"+strDbx+"sblaster_hdma>" << datos_emu["Dbx_sblaster_hdma"] << "</"+strDbx+"sblaster_hdma>" << endl;
			out << "      <"+strDbx+"sblaster_mixer>" << datos_emu["Dbx_sblaster_mixer"] << "</"+strDbx+"sblaster_mixer>" << endl;
			out << "      <"+strDbx+"sblaster_oplmode>" << datos_emu["Dbx_sblaster_oplmode"] << "</"+strDbx+"sblaster_oplmode>" << endl;
			out << "      <"+strDbx+"sblaster_oplemu>" << datos_emu["Dbx_sblaster_oplemu"] << "</"+strDbx+"sblaster_oplemu>" << endl;
			out << "      <"+strDbx+"sblaster_oplrate>" << datos_emu["Dbx_sblaster_oplrate"] << "</"+strDbx+"sblaster_oplrate>" << endl;
			out << "      <"+strDbx+"gus_gus>" << datos_emu["Dbx_gus_gus"] << "</"+strDbx+"gus_gus>" << endl;
			out << "      <"+strDbx+"gus_gusrate>" << datos_emu["Dbx_gus_gusrate"] << "</"+strDbx+"gus_gusrate>" << endl;
			out << "      <"+strDbx+"gus_gusbase>" << datos_emu["Dbx_gus_gusbase"] << "</"+strDbx+"gus_gusbase>" << endl;
			out << "      <"+strDbx+"gus_irq1>" << datos_emu["Dbx_gus_irq1"] << "</"+strDbx+"gus_irq1>" << endl;
			out << "      <"+strDbx+"gus_irq2>" << datos_emu["Dbx_gus_irq2"] << "</"+strDbx+"gus_irq2>" << endl;
			out << "      <"+strDbx+"gus_dma1>" << datos_emu["Dbx_gus_dma1"] << "</"+strDbx+"gus_dma1>" << endl;
			out << "      <"+strDbx+"gus_dma2>" << datos_emu["Dbx_gus_dma2"] << "</"+strDbx+"gus_dma2>" << endl;
			out << "      <"+strDbx+"gus_ultradir>" << datos_emu["Dbx_gus_ultradir"] << "</"+strDbx+"gus_ultradir>" << endl;
			out << "      <"+strDbx+"speaker_pcspeaker>" << datos_emu["Dbx_speaker_pcspeaker"] << "</"+strDbx+"speaker_pcspeaker>" << endl;
			out << "      <"+strDbx+"speaker_pcrate>" << datos_emu["Dbx_speaker_pcrate"] << "</"+strDbx+"speaker_pcrate>" << endl;
			out << "      <"+strDbx+"speaker_tandy>" << datos_emu["Dbx_speaker_tandy"] << "</"+strDbx+"speaker_tandy>" << endl;
			out << "      <"+strDbx+"speaker_tandyrate>" << datos_emu["Dbx_speaker_tandyrate"] << "</"+strDbx+"speaker_tandyrate>" << endl;
			out << "      <"+strDbx+"speaker_disney>" << datos_emu["Dbx_speaker_disney"] << "</"+strDbx+"speaker_disney>" << endl;
			out << "      <"+strDbx+"joystick_type>" << datos_emu["Dbx_joystick_type"] << "</"+strDbx+"joystick_type>" << endl;
			out << "      <"+strDbx+"joystick_timed>" << datos_emu["Dbx_joystick_timed"] << "</"+strDbx+"joystick_timed>" << endl;
			out << "      <"+strDbx+"joystick_autofire>" << datos_emu["Dbx_joystick_autofire"] << "</"+strDbx+"joystick_autofire>" << endl;
			out << "      <"+strDbx+"joystick_swap34>" << datos_emu["Dbx_joystick_swap34"] << "</"+strDbx+"joystick_swap34>" << endl;
			out << "      <"+strDbx+"joystick_buttonwrap>" << datos_emu["Dbx_joystick_buttonwrap"] << "</"+strDbx+"joystick_buttonwrap>" << endl;
			out << "      <"+strDbx+"modem_modem>" << datos_emu["Dbx_modem_modem"] << "</"+strDbx+"modem_modem>" << endl;
			out << "      <"+strDbx+"modem_comport>" << datos_emu["Dbx_modem_comport"] << "</"+strDbx+"modem_comport>" << endl;
			out << "      <"+strDbx+"modem_listenport>" << datos_emu["Dbx_modem_listenport"] << "</"+strDbx+"modem_listenport>" << endl;
			out << "      <"+strDbx+"dserial_directserial>" << datos_emu["Dbx_dserial_directserial"] << "</"+strDbx+"dserial_directserial>" << endl;
			out << "      <"+strDbx+"dserial_comport>" << datos_emu["Dbx_dserial_comport"] << "</"+strDbx+"dserial_comport>" << endl;
			out << "      <"+strDbx+"dserial_realport>" << datos_emu["Dbx_dserial_realport"] << "</"+strDbx+"dserial_realport>" << endl;
			out << "      <"+strDbx+"dserial_defaultbps>" << datos_emu["Dbx_dserial_defaultbps"] << "</"+strDbx+"dserial_defaultbps>" << endl;
			out << "      <"+strDbx+"dserial_parity>" << datos_emu["Dbx_dserial_parity"] << "</"+strDbx+"dserial_parity>" << endl;
			out << "      <"+strDbx+"dserial_bytesize>" << datos_emu["Dbx_dserial_bytesize"] << "</"+strDbx+"dserial_bytesize>" << endl;
			out << "      <"+strDbx+"dserial_stopbit>" << datos_emu["Dbx_dserial_stopbit"] << "</"+strDbx+"dserial_stopbit>" << endl;
			out << "      <"+strDbx+"serial_1>" << datos_emu["Dbx_serial_1"] << "</"+strDbx+"serial_1>" << endl;
			out << "      <"+strDbx+"serial_2>" << datos_emu["Dbx_serial_2"] << "</"+strDbx+"serial_2>" << endl;
			out << "      <"+strDbx+"serial_3>" << datos_emu["Dbx_serial_3"] << "</"+strDbx+"serial_3>" << endl;
			out << "      <"+strDbx+"serial_4>" << datos_emu["Dbx_serial_4"] << "</"+strDbx+"serial_4>" << endl;
			out << "      <"+strDbx+"dos_xms>" << datos_emu["Dbx_dos_xms"] << "</"+strDbx+"dos_xms>" << endl;
			out << "      <"+strDbx+"dos_ems>" << datos_emu["Dbx_dos_ems"] << "</"+strDbx+"dos_ems>" << endl;
			out << "      <"+strDbx+"dos_umb>" << datos_emu["Dbx_dos_umb"] << "</"+strDbx+"dos_umb>" << endl;
			out << "      <"+strDbx+"dos_keyboardlayout>" << datos_emu["Dbx_dos_keyboardlayout"] << "</"+strDbx+"dos_keyboardlayout>" << endl;
			out << "      <"+strDbx+"ipx_ipx>" << datos_emu["Dbx_ipx_ipx"] << "</"+strDbx+"ipx_ipx>" << endl;
			out << "      <"+strDbx+"autoexec>" << datos_emu["Dbx_autoexec"] << "</"+strDbx+"autoexec>" << endl;
			out << "      <"+strDbx+"opt_autoexec>" << datos_emu["Dbx_opt_autoexec"] << "</"+strDbx+"opt_autoexec>" << endl;
			out << "      <"+strDbx+"opt_loadfix>" << datos_emu["Dbx_opt_loadfix"] << "</"+strDbx+"opt_loadfix>" << endl;
			out << "      <"+strDbx+"opt_loadfix_mem>" << datos_emu["Dbx_opt_loadfix_mem"] << "</"+strDbx+"opt_loadfix_mem>" << endl;
			out << "      <"+strDbx+"opt_consola_dbox>" << datos_emu["Dbx_opt_consola_dbox"] << "</"+strDbx+"opt_consola_dbox>" << endl;
			out << "      <"+strDbx+"opt_cerrar_dbox>" << datos_emu["Dbx_opt_cerrar_dbox"] << "</"+strDbx+"opt_cerrar_dbox>" << endl;
			out << "      <"+strDbx+"opt_cycle_sincronizar>" << datos_emu["Dbx_opt_cycle_sincronizar"] << "</"+strDbx+"opt_cycle_sincronizar>" << endl;
			out << "      <"+strDbx+"path_conf>" << datos_emu["Dbx_path_conf"] << "</"+strDbx+"path_conf>" << endl;
			out << "      <"+strDbx+"path_sonido>" << datos_emu["Dbx_path_sonido"] << "</"+strDbx+"path_sonido>" << endl;
			out << "      <"+strDbx+"path_exe>" << datos_emu["Dbx_path_exe"] << "</"+strDbx+"path_exe>" << endl;
			out << "      <"+strDbx+"path_setup>" << datos_emu["Dbx_path_setup"] << "</"+strDbx+"path_setup>" << endl;
			out << "      <"+strDbx+"parametros_exe>" << datos_emu["Dbx_parametros_exe"] << "</"+strDbx+"parametros_exe>" << endl;
			out << "      <"+strDbx+"parametros_setup>" << datos_emu["Dbx_parametros_setup"] << "</"+strDbx+"parametros_setup>" << endl;
			out << "    </dosbox>" << endl;

			for(int num_mount = 0; num_mount < treeWidget->topLevelItemCount(); num_mount++ )
			{
				QTreeWidgetItem *item = treeWidget->topLevelItem( num_mount );
				out << "    <"+str_dosbox+"montajes>" << endl;
				out << "      <"+strDbxMount+"id_lista>" << item->text(9) << "</"+strDbxMount+"id_lista>" << endl;
				out << "      <"+strDbxMount+"path>" << item->text(0) << "<"+strDbxMount+"/path>" << endl;
				out << "      <"+strDbxMount+"label>" << item->text(1) << "</"+strDbxMount+"label>" << endl;
				out << "      <"+strDbxMount+"tipo_as>" << item->text(2) << "</"+strDbxMount+"tipo_as>" << endl;
				out << "      <"+strDbxMount+"letter>" << item->text(3) << "</"+strDbxMount+"letter>" << endl;
				out << "      <"+strDbxMount+"indx_cd>" << item->text(4) << "</"+strDbxMount+"indx_cd>" << endl;
				out << "      <"+strDbxMount+"opt_mount>" << item->text(5) << "</"+strDbxMount+"opt_mount>" << endl;
				out << "      <"+strDbxMount+"io_ctrl>" << item->text(6) << "</"+strDbxMount+"io_ctrl>" << endl;
				out << "      <"+strDbxMount+"select_mount>" << item->text(7) << "</"+strDbxMount+"select_mount>" << endl;
				out << "    </"+str_dosbox+"montajes>" << endl;
			}
		}

		if( datos["Dat_tipo_emu"] == "scummvm" )
		{
			out << "    <scummvm>" << endl;
			out << "      <"+strSvm+"game>" << datos_emu["Svm_game"] << "</"+strSvm+"game>" << endl;
			out << "      <"+strSvm+"game_label>" << datos_emu["Svm_game_label"] << "</"+strSvm+"game_label>" << endl;
			out << "      <"+strSvm+"language>" << datos_emu["Svm_language"] << "</"+strSvm+"language>" << endl;
			out << "      <"+strSvm+"subtitles>" << datos_emu["Svm_subtitles"] << "</"+strSvm+"subtitles>" << endl;
			out << "      <"+strSvm+"platform>" << datos_emu["Svm_platform"] << "</"+strSvm+"platform>" << endl;
			out << "      <"+strSvm+"gfx_mode>" << datos_emu["Svm_gfx_mode"] << "</"+strSvm+"gfx_mode>" << endl;
			out << "      <"+strSvm+"render_mode>" << datos_emu["Svm_render_mode"] << "</"+strSvm+"render_mode>" << endl;
			out << "      <"+strSvm+"fullscreen>" << datos_emu["Svm_fullscreen"] << "</"+strSvm+"fullscreen>" << endl;
			out << "      <"+strSvm+"aspect_ratio>" << datos_emu["Svm_aspect_ratio"] << "</"+strSvm+"aspect_ratio>" << endl;
			out << "      <"+strSvm+"path>" << datos_emu["Svm_path"] << "</"+strSvm+"path>" << endl;
			out << "      <"+strSvm+"path_setup>" << datos_emu["Svm_path_setup"] << "</"+strSvm+"path_setup>" << endl;
			out << "      <"+strSvm+"path_extra>" << datos_emu["Svm_path_extra"] << "</"+strSvm+"path_extra>" << endl;
			out << "      <"+strSvm+"path_save>" << datos_emu["Svm_path_save"] << "</"+strSvm+"path_save>" << endl;
			out << "      <"+strSvm+"path_capturas>" << datos_emu["Svm_path_capturas"] << "</"+strSvm+"path_capturas>" << endl;
			out << "      <"+strSvm+"path_sonido>" << datos_emu["Svm_path_sonido"] << "</"+strSvm+"path_sonido>" << endl;
			out << "      <"+strSvm+"music_driver>" << datos_emu["Svm_music_driver"] << "</"+strSvm+"music_driver>" << endl;
			out << "      <"+strSvm+"enable_gs>" << datos_emu["Svm_enable_gs"] << "</"+strSvm+"enable_gs>" << endl;
			out << "      <"+strSvm+"multi_midi>" << datos_emu["Svm_multi_midi"] << "</"+strSvm+"multi_midi>" << endl;
			out << "      <"+strSvm+"native_mt32>" << datos_emu["Svm_native_mt32"] << "</"+strSvm+"native_mt32>" << endl;
			out << "      <"+strSvm+"master_volume>" << datos_emu["Svm_master_volume"] << "</"+strSvm+"master_volume>" << endl;
			out << "      <"+strSvm+"music_volume>" << datos_emu["Svm_music_volume"] << "</"+strSvm+"music_volume>" << endl;
			out << "      <"+strSvm+"sfx_volume>" << datos_emu["Svm_sfx_volume"] << "</"+strSvm+"sfx_volume>" << endl;
			out << "      <"+strSvm+"speech_volume>" << datos_emu["Svm_speech_volume"] << "</"+strSvm+"speech_volume>" << endl;
			out << "      <"+strSvm+"tempo>" << datos_emu["Svm_tempo"] << "</"+strSvm+"tempo>" << endl;
			out << "      <"+strSvm+"talkspeed>" << datos_emu["Svm_talkspeed"] << "</"+strSvm+"talkspeed>" << endl;
			out << "      <"+strSvm+"debuglevel>" << datos_emu["Svm_debuglevel"] << "</"+strSvm+"debuglevel>" << endl;
			out << "      <"+strSvm+"cdrom>" << datos_emu["Svm_cdrom"] << "</"+strSvm+"cdrom>" << endl;
			out << "      <"+strSvm+"joystick_num>" << datos_emu["Svm_joystick_num"] << "</"+strSvm+"joystick_num>" << endl;
			out << "      <"+strSvm+"output_rate>" << datos_emu["Svm_output_rate"] << "</"+strSvm+"output_rate>" << endl;
			out << "      <"+strSvm+"midi_gain>" << datos_emu["Svm_midi_gain"] << "</"+strSvm+"midi_gain>" << endl;
			out << "      <"+strSvm+"copy_protection>" << datos_emu["Svm_copy_protection"] << "</"+strSvm+"copy_protection>" << endl;
			out << "      <"+strSvm+"sound_font>" << datos_emu["Svm_sound_font"] << "</"+strSvm+"sound_font>" << endl;
			out << "    </scummvm>" << endl;
		}

		out << "  </lista_db>" << endl;
		out << "</grlida_db>" << endl;
	}
}

// Carga la lista de los perfiles preconfigurados en un QComboBox
void Funciones::Cargar_Profile_DFend_ComboBox(QString dirProfiles, QComboBox *myCombobox)
{
	QDir dir( dirProfiles );
	QString filter = "*.prof";
	QDir::Filters filters = QDir::Files | QDir::Readable;
	QDir::SortFlags sort  = QDir::Name;
	QFileInfoList entries = dir.entryInfoList( QStringList() << filter, filters, sort);

	myCombobox->clear();
	myCombobox->addItem( QIcon(stTheme+"img16/archivo_config.png"), QObject::tr("Configuración por Defecto"), "<defecto>" );
	foreach (QFileInfo file, entries)
	{
		myCombobox->addItem( QIcon(stTheme+"img16/archivo_config.png"), file.completeBaseName(), file.fileName() );
	}
}

// Exportar la configuración del DOSBox para el DFend
void Funciones::Exportar_Profile_DFend(const QHash<QString, QString> datos, const QHash<QString, QString> datosDbx, QTreeWidget *treeWidget, const QString pathSaveConfg)
{
	CrearArchivoConfigDbx(datos, datosDbx, treeWidget, pathSaveConfg, true);
}

// Importar la configuración del DOSBox para el DFend
QHash<QString, QString> Funciones::Importar_Profile_DFend(QString fileName)
{
	QString linea, fileTemp, strTemp, info_name_conf;
	QHash<QString, QString> profileDFend;
	QStringList list_priority;
	int n = 0, num_mounts = 0;
	bool str_ok;

	fileTemp = GRlidaHomePath()+"temp/temp_config.prof";

// Leer archivo ------------------------------------------------------
	QFile file_in( fileName );
	file_in.open( QIODevice::ReadOnly | QIODevice::Text );

	QTextStream in(&file_in);
	in.setCodec("UTF-8");

	linea.clear();
	linea = in.readAll();
	linea.replace("\\", "\\\\");
	linea.replace(";", "|");
	linea.replace(",", "--");

	file_in.close();

// Escribir archivo --------------------------------------------------
	QFile file_out( fileTemp );
	file_out.open( QIODevice::WriteOnly | QIODevice::Text );

	QTextStream out(&file_out);
		out.setCodec("UTF-8");
		out << linea;
		out.flush();

	file_out.close();
	linea.clear();

	profileDFend.clear();
	QSettings settings( fileTemp , QSettings::IniFormat );

// Datos del juego -----
	settings.beginGroup("ExtraInfo");
		QString icono = settings.value("Icon", "dosbox").toString().replace("--", ",");
		if( icono != "" )
			profileDFend["Dat_icono"] = icono;
		else
			profileDFend["Dat_icono"] = "dosbox";

		profileDFend["Dat_titulo"]        = settings.value("Name", "").toString().replace("--", ",");
		profileDFend["Dat_subtitulo"]     = "";
		profileDFend["Dat_genero"]        = settings.value("Genre", "").toString().replace("--", ",");
		profileDFend["Dat_compania"]      = settings.value("Publisher", "").toString().replace("--", ",");
		profileDFend["Dat_desarrollador"] = settings.value("Developer", "").toString().replace("--", ",");
		profileDFend["Dat_tema"]          = "";
		profileDFend["Dat_idioma"]        = settings.value("Language", "").toString().replace("--", ",");
		profileDFend["Dat_formato"]       = "";
		profileDFend["Dat_anno"]          = settings.value("Year", "").toString().replace("--", ",");
		profileDFend["Dat_numdisc"]       = "";
		profileDFend["Dat_sistemaop"]     = "";
		profileDFend["Dat_tamano"]        = "";
		profileDFend["Dat_graficos"]      = "1";
		profileDFend["Dat_sonido"]        = "1";
		profileDFend["Dat_jugabilidad"]   = "1";
		profileDFend["Dat_original"]      = "false";
		profileDFend["Dat_estado"]        = "";
		profileDFend["Dat_thumbs"]        = "";
		profileDFend["Dat_cover_front"]   = "";
		profileDFend["Dat_cover_back"]    = "";
		profileDFend["Dat_comentario"]    = settings.value("Notes", "").toString().replace("--", ",").replace("[13][10]","<br/>\n").replace("[13]","\n");
		profileDFend["Dat_favorito"]      = BoolToStr( settings.value("Favorite","false").toBool() );
		profileDFend["Dat_rating"]        = "0";
		profileDFend["Dat_usuario"]       = "";
		profileDFend["WWW"]                 = settings.value("WWW"      , "").toString().replace("--", ",");
		profileDFend["UserInfo"]            = settings.value("UserInfo" , "").toString().replace("--", ",");
	settings.endGroup();

// Datos del emulador DOSBox ---
	profileDFend["Dbx_opt_autoexec"]          = "false";
	profileDFend["Dbx_opt_consola_dbox"]      = "false";
	profileDFend["Dbx_opt_cycle_sincronizar"] = "false";
	profileDFend["Dbx_path_sonido"]           = "";

	info_name_conf = eliminar_caracteres( profileDFend["Dat_titulo"] );
	strTemp = info_name_conf;

	str_ok = strTemp.endsWith(".conf");
	if(str_ok == false)
		strTemp.append(".conf");

	QFile appConfg( QDir::homePath()+"/.gr-lida/confdbx/"+ strTemp );
	if( appConfg.exists() )
		profileDFend["Dbx_path_conf"] = HoraFechaActual("ddMMyyyy_HHmmss") +"-"+ info_name_conf;
	else
		profileDFend["Dbx_path_conf"] = strTemp;

	str_ok = profileDFend["Dbx_path_conf"].endsWith(".conf");
	if(str_ok == false)
		profileDFend["Dbx_path_conf"].append(".conf");

	settings.beginGroup("Extra");
		profileDFend["Dbx_path_exe"]         = settings.value("Exe"             ,""  ).toString().replace("--", ",");
		profileDFend["Dbx_path_setup"]       = settings.value("Setup"           ,""  ).toString().replace("--", ",");
		profileDFend["Dbx_parametros_exe"]   = settings.value("GameParameters"  ,""  ).toString().replace("--", ",");
		profileDFend["Dbx_parametros_setup"] = settings.value("SetupParameters" ,""  ).toString().replace("--", ",");
		profileDFend["Dbx_autoexec"]         = settings.value("autoexec"        ,""  ).toString().replace("--", ",");
		profileDFend["Dbx_opt_loadfix_mem"]  = settings.value("LoadFixVal"      ,"64").toString();
		profileDFend["NrOfMounts"]           = settings.value("NrOfMounts"      ,"0" ).toString();
		profileDFend["Dbx_opt_cerrar_dbox"]  = BoolToStr( settings.value("CloseOnExit","false").toBool() );
		profileDFend["Dbx_opt_loadfix"]      = BoolToStr( settings.value("Loadhigh","false").toBool() );

		num_mounts = StrToInt( profileDFend["NrOfMounts"] );
		if( num_mounts > 0 )
		{
			for( n = 0; n < num_mounts; n++)
				profileDFend.insert(IntToStr(n), settings.value(IntToStr(n),"").toString().replace("--",",") );
		}

		//profileDFend["ExeMD5"]            = settings.value("ExeMD5"           ,"").toString();
		//profileDFend["LastModification"]  = settings.value("LastModification" ,"").toString();
		//profileDFend["Environment"]       = settings.value("Environment"      ,"").toString();
		//profileDFend["Tab"]               = settings.value("Tab"              ,"").toString();
		//profileDFend["Tab2"]              = settings.value("Tab2"             ,"").toString();
		//profileDFend["SetupMD5"]          = settings.value("SetupMD5"         ,"").toString();
		//profileDFend["DataDir"]           = settings.value("DataDir"          ,"").toString();
		//profileDFend["Overridegamestart"] = settings.value("Overridegamestart","").toString();
		//profileDFend["OverrideMount"]     = settings.value("OverrideMount"    ,"").toString();
		//profileDFend["AutoMountCDs"]      = settings.value("AutoMountCDs"     ,"").toString();
		//profileDFend["ExtraDirs"]         = settings.value("ExtraDirs"        ,"").toString();
		//profileDFend["ExtraFiles"]        = settings.value("ExtraFiles"       ,"").toString();
		//profileDFend["ExtraExe0"]         = settings.value("ExtraExe0"        ,"").toString();
		//profileDFend["0"]                 = settings.value("0"                ,"").toString();
		//profileDFend["1"]                 = settings.value("1"                ,"").toString();
		//profileDFend["2"]                 = settings.value("2"                ,"").toString();
		//profileDFend["3"]                 = settings.value("3"                ,"").toString();
		//profileDFend["4"]                 = settings.value("4"                ,"").toString();
		//profileDFend["5"]                 = settings.value("5"                ,"").toString();
		//profileDFend["6"]                 = settings.value("6"                ,"").toString();
		//profileDFend["7"]                 = settings.value("7"                ,"").toString();
		//profileDFend["8"]                 = settings.value("8"                ,"").toString();
		//profileDFend["9"]                 = settings.value("9"                ,"").toString();
		//profileDFend["BootImage"]         = settings.value("BootImage"        ,"").toString();
		//profileDFend["Finalization"]      = settings.value("Finalization"     ,"").toString();
		//profileDFend["CustomSettings"]    = settings.value("CustomSettings"   ,"").toString();
		//profileDFend["ScreenshotListScreenshot"] = settings.value("ScreenshotListScreenshot","").toString();

		if( settings.value("ProfileMode").toString().toLower() == "dosbox" )
			profileDFend["Dat_tipo_emu"] = "dosbox";
		else if( settings.value("ProfileMode").toString().toLower() == "scummvm" )
			profileDFend["Dat_tipo_emu"] = "scummvm";
		else
			profileDFend["Dat_tipo_emu"] = "datos";

	settings.endGroup();

	settings.beginGroup("sdl");
		profileDFend["Dbx_sdl_fullscreen"]       = BoolToStr( settings.value("fullscreen","false").toBool() );
		profileDFend["Dbx_sdl_fulldouble"]       = BoolToStr( settings.value("fulldouble","false").toBool() );
//		profileDFend["Dbx_sdl_fullfixed"]        = BoolToStr( settings.value("fullfixed","false").toBool() ); // en desuso
		profileDFend["Dbx_sdl_fullresolution"]   = settings.value("fullresolution"  ,"original").toString();
		profileDFend["Dbx_sdl_windowresolution"] = settings.value("windowresolution","original").toString();
		profileDFend["Dbx_sdl_output"]           = settings.value("output"          ,"surface" ).toString();
		profileDFend["Dbx_sdl_hwscale"]          = settings.value("hwscale"         ,"1.00"    ).toString();
		profileDFend["Dbx_sdl_autolock"]         = BoolToStr( settings.value("autolock","true").toBool() );
		profileDFend["Dbx_sdl_sensitivity"]      = settings.value("sensitivity", "100").toString();
		profileDFend["Dbx_sdl_waitonerror"]      = BoolToStr( settings.value("waitonerror","true").toBool() );
		profileDFend["Dbx_sdl_priority"]         = settings.value("priority"  , "higher,normal").toString().replace("--", ",");
		profileDFend["Dbx_sdl_mapperfile"]       = settings.value("mapperfile", "mapper.txt"   ).toString().replace("--", ",");
		profileDFend["Dbx_sdl_usescancodes"]     = BoolToStr( settings.value("usescancodes","true").toBool() );
//		profileDFend["Dbx_sdl_dosboxlanguage"]   = settings.value("DOSBoxlanguage","").toString();
	settings.endGroup();

	settings.beginGroup("dosbox");
		profileDFend["Dbx_dosbox_language"] = settings.value("language","").toString().replace("--", ",");
		profileDFend["Dbx_dosbox_machine"]  = settings.value("machine" ,"vga").toString();
		profileDFend["Dbx_dosbox_captures"] = settings.value("captures","capture").toString().replace("--", ",");
		profileDFend["Dbx_dosbox_memsize"]  = settings.value("memsize" ,"16").toString();
	settings.endGroup();

	settings.beginGroup("render");
		profileDFend["Dbx_render_frameskip"] = settings.value("frameskip","0").toString();
		profileDFend["Dbx_render_aspect"]    = BoolToStr( settings.value("aspect","false").toBool() );
		profileDFend["Dbx_render_scaler"]    = settings.value("scaler"   ,"normal2x").toString();
//		profileDFend["Dbx_render_textmodelines"] = settings.value("TextModeLines","25").toString();
	settings.endGroup();

	settings.beginGroup("cpu");
		profileDFend["Dbx_cpu_core"]      = settings.value("core"     ,"auto").toString();
		profileDFend["Dbx_cpu_cputype"]   = settings.value("type"     ,"auto").toString();
		profileDFend["Dbx_cpu_cycles"]    = settings.value("cycles"   ,"auto").toString();
		profileDFend["Dbx_cpu_cycleup"]   = settings.value("cycleup"  ,"500").toString();
		profileDFend["Dbx_cpu_cycledown"] = settings.value("cycledown","20").toString();
	settings.endGroup();

	settings.beginGroup("mixer");
		profileDFend["Dbx_mixer_nosound"]   = BoolToStr( settings.value("nosound","false").toBool() );
		profileDFend["Dbx_mixer_rate"]      = settings.value("rate"     ,"22050").toString();
		profileDFend["Dbx_mixer_blocksize"] = settings.value("blocksize","2048").toString();
		profileDFend["Dbx_mixer_prebuffer"] = settings.value("prebuffer","10").toString();
	settings.endGroup();

	settings.beginGroup("midi");
		profileDFend["Dbx_midi_mpu401"]      = settings.value("mpu401"       ,"intelligent").toString();
		profileDFend["Dbx_midi_intelligent"] = BoolToStr( settings.value("midi_intelligent","true").toBool() );
		profileDFend["Dbx_midi_device"]      = settings.value("device"       ,"default").toString();
		profileDFend["Dbx_midi_config"]      = settings.value("config"       ,"").toString();
		profileDFend["Dbx_midi_mt32rate"]    = settings.value("mt32rate"     ,"").toString();
	settings.endGroup();

	settings.beginGroup("sblaster");
		profileDFend["Dbx_sblaster_sbtype"] = settings.value("sbtype" ,"sb16").toString();
		profileDFend["Dbx_sblaster_sbbase"] = settings.value("sbbase" ,"220").toString();
		profileDFend["Dbx_sblaster_irq"]    = settings.value("irq"    ,"7").toString();
		profileDFend["Dbx_sblaster_dma"]    = settings.value("dma"    ,"1").toString();
		profileDFend["Dbx_sblaster_hdma"]   = settings.value("hdma"   ,"5").toString();
		profileDFend["Dbx_sblaster_mixer"]   = BoolToStr( settings.value("mixer","true").toBool() );
		profileDFend["Dbx_sblaster_oplmode"] = settings.value("oplmode","auto").toString();
		profileDFend["Dbx_sblaster_oplemu"]  = settings.value("oplemu" ,"default").toString();
		profileDFend["Dbx_sblaster_oplrate"] = settings.value("oplrate","22050").toString();
	settings.endGroup();

	settings.beginGroup("gus");
		profileDFend["Dbx_gus_gus"]      = BoolToStr( settings.value("gus","true").toBool() );
		profileDFend["Dbx_gus_gusrate"]  = settings.value("gusrate" ,"22050").toString();
		profileDFend["Dbx_gus_gusbase"]  = settings.value("gusbase" ,"240").toString();
		profileDFend["Dbx_gus_irq1"]     = settings.value("irq1"    ,"5").toString();
		profileDFend["Dbx_gus_irq2"]     = settings.value("irq2"    ,"5").toString();
		profileDFend["Dbx_gus_dma1"]     = settings.value("dma1"    ,"3").toString();
		profileDFend["Dbx_gus_dma2"]     = settings.value("dma2"    ,"3").toString();
		profileDFend["Dbx_gus_ultradir"] = settings.value("ultradir","C:\\ULTRASND").toString().replace("--",",");
	settings.endGroup();

	settings.beginGroup("speaker");
		profileDFend["Dbx_speaker_pcspeaker"] = BoolToStr( settings.value("pcspeaker","true").toBool() );
		profileDFend["Dbx_speaker_pcrate"]    = settings.value("pcrate"   ,"22050").toString();
		profileDFend["Dbx_speaker_tandy"]     = settings.value("tandy"    ,"auto").toString();
		profileDFend["Dbx_speaker_tandyrate"] = settings.value("tandyrate","22050").toString();
		profileDFend["Dbx_speaker_disney"]    = BoolToStr( settings.value("disney","true").toBool() );
	settings.endGroup();

	settings.beginGroup("joystick");
		profileDFend["Dbx_joystick_type"]       = settings.value("joysticktype","auto").toString();
		profileDFend["Dbx_joystick_timed"]      = BoolToStr( settings.value("timed","true").toBool() );
		profileDFend["Dbx_joystick_autofire"]   = BoolToStr( settings.value("autofire","false").toBool() );
		profileDFend["Dbx_joystick_swap34"]     = BoolToStr( settings.value("swap34","false").toBool() );
		profileDFend["Dbx_joystick_buttonwrap"] = BoolToStr( settings.value("buttonwrap","true").toBool() );
	settings.endGroup();

	profileDFend["Dbx_modem_modem"]          = settings.value("modem"       ,"true").toString();
	profileDFend["Dbx_modem_comport"]        = settings.value("comport"     ,"1").toString();
	profileDFend["Dbx_modem_listenport"]     = settings.value("listenport"  ,"23").toString();
	profileDFend["Dbx_dserial_directserial"] = settings.value("directserial","true").toString();
	profileDFend["Dbx_dserial_comport"]      = settings.value("comport"     ,"1").toString();
	profileDFend["Dbx_dserial_realport"]     = settings.value("realport"    ,"COM1").toString();
	profileDFend["Dbx_dserial_defaultbps"]   = settings.value("defaultbps"  ,"1200").toString();
	profileDFend["Dbx_dserial_parity"]       = settings.value("parity"      ,"N").toString();
	profileDFend["Dbx_dserial_bytesize"]     = settings.value("bytesize"    ,"8").toString();
	profileDFend["Dbx_dserial_stopbit"]      = settings.value("stopbit"     ,"1").toString();

	settings.beginGroup("serial");
		profileDFend["Dbx_serial_1"] = settings.value("serial1", "dummy").toString().replace("--",",");
		profileDFend["Dbx_serial_2"] = settings.value("serial2", "dummy").toString().replace("--",",");
		profileDFend["Dbx_serial_3"] = settings.value("serial3", "disabled").toString().replace("--",",");
		profileDFend["Dbx_serial_4"] = settings.value("serial4", "disabled").toString().replace("--",",");
	settings.endGroup();

	settings.beginGroup("dos");
		profileDFend["Dbx_dos_xms"] = BoolToStr( settings.value("xms","true").toBool() );
		profileDFend["Dbx_dos_ems"] = BoolToStr( settings.value("ems","true").toBool() );
		profileDFend["Dbx_dos_umb"] = settings.value("umb", "true").toString();

		if( settings.value("keyboardlayout","auto").toString() == "default")
			profileDFend["Dbx_dos_keyboardlayout"] = "auto";
		else
			profileDFend["Dbx_dos_keyboardlayout"] = settings.value("keyboardlayout", "auto").toString();

//		profileDFend["Dbx_dos_codepage"] = settings.value("codepage", "").toString();
	settings.endGroup();

	settings.beginGroup("ipx");
		profileDFend["Dbx_ipx_ipx"] = BoolToStr( settings.value("ipx","false").toBool() );
	settings.endGroup();

// Datos del emulador ScummVM --
	settings.beginGroup("ScummVM");
		profileDFend["Svm_game"]          = settings.value("GameName"     , "").toString();
		profileDFend["Svm_game_label"]    = settings.value("GameName"     , "").toString();
		profileDFend["Svm_language"]      = settings.value("Language"     , "es").toString();
		profileDFend["Svm_subtitles"]     = BoolToStr( settings.value("Subtitles", "true").toBool() );
	profileDFend["Svm_platform"]          = settings.value("platform"     , "pc").toString();
		profileDFend["Svm_gfx_mode"]      = settings.value("Filter"       , "2x").toString();
	profileDFend["Svm_render_mode"]       = settings.value("render_mode"  , "").toString();
	profileDFend["Svm_fullscreen"]        = BoolToStr( settings.value("fullscreen"   , "false").toBool() );
	profileDFend["Svm_aspect_ratio"]      = BoolToStr( settings.value("aspect_ratio" , "true").toBool() );
		profileDFend["Svm_path"]          = settings.value("GamePath"     , "").toString();
	profileDFend["Svm_path_setup"]        = settings.value("path_setup"   , "").toString();
	profileDFend["Svm_path_extra"]        = settings.value("path_extra"   , "").toString();
		profileDFend["Svm_path_save"]     = settings.value("Savepath"     , "").toString();
	profileDFend["Svm_path_capturas"]     = settings.value("path_capturas", "").toString();
	profileDFend["Svm_path_sonido"]       = settings.value("path_sonido"  , "").toString();
		profileDFend["Svm_music_driver"]  = settings.value("MusicDriver"  , "adlib").toString();
		profileDFend["Svm_enable_gs"]     = BoolToStr( settings.value("EnableGS"     , "true").toBool() );
		profileDFend["Svm_multi_midi"]    = BoolToStr( settings.value("MultiMIDI"    , "true").toBool() );
		profileDFend["Svm_native_mt32"]   = BoolToStr( settings.value("NativeMT32"   , "true").toBool() );
		profileDFend["Svm_master_volume"] = settings.value("MusicVolume"  , "192").toString();
		profileDFend["Svm_music_volume"]  = settings.value("MusicVolume"  , "192").toString();
		profileDFend["Svm_sfx_volume"]    = settings.value("SFXVolume"    , "192").toString();
		profileDFend["Svm_speech_volume"] = settings.value("SpeechVolume" , "192").toString();
	profileDFend["Svm_tempo"]             = settings.value("tempo"        , "100").toString();
		profileDFend["Svm_talkspeed"]     = settings.value("TalkSpeed"    , "60").toString();
	profileDFend["Svm_debuglevel"]        = settings.value("debuglevel"   , "0").toString();
		profileDFend["Svm_cdrom"]         = settings.value("CDROM"        , "0").toString();
		profileDFend["Svm_joystick_num"]  = settings.value("JoystickNum"  , "0").toString();
		profileDFend["Svm_output_rate"]   = settings.value("SampleRate"   , "<defecto>").toString();
		profileDFend["Svm_midi_gain"]     = settings.value("MIDIGain"     , "100").toString();
	profileDFend["Svm_copy_protection"]   = BoolToStr( settings.value("copy_protection", "false").toBool() );
	profileDFend["Svm_sound_font"]        = settings.value("sound_font"   , "").toString();
		profileDFend["Svm_walkspeed"]     = settings.value("Walkspeed"    , "0").toString();
	settings.endGroup();

	if( file_out.exists() )
		file_out.remove();

	return profileDFend;
}

// Crea la imagen de la lista
void Funciones::CrearCoverList(QHash<QString, QVariant> datos, QHash<QString, QVariant> config)
{
	QImage cover_src;
	QImage cover_top;
	QImage cover_scaled;
	QImage cover_result;
	QImage img_tipo_emu;
	QImage star_on(stTheme+"images/star_on.png");
	QImage star_off(stTheme+"images/star_off.png");

	if(datos["Dat_tipo_emu"].toString()=="")
		img_tipo_emu.load(stTheme+"img16/sinimg.png");
	else if(datos["Dat_tipo_emu"].toString()=="datos")
		img_tipo_emu.load(stTheme+"img16/datos_3.png");
	else if(datos["Dat_tipo_emu"].toString()=="dosbox")
		img_tipo_emu.load(stTheme+"img16/dosbox.png");
	else if(datos["Dat_tipo_emu"].toString()=="scummvm")
		img_tipo_emu.load(stTheme+"img16/scummvm.png");
	else if(datos["Dat_tipo_emu"].toString()=="vdmsound")
		img_tipo_emu.load(stTheme+"img16/vdmsound.png");
	else
		img_tipo_emu.load(stTheme+"img16/sinimg.png");

	cover_src.load( datos["Dat_img_src"].toString() );
	cover_top.load(stTheme+"images/list_cover_top.png");

	if( config["img_scaled"].toBool() )
		cover_scaled = cover_src.scaled(config["img_scale_w"].toInt(), config["img_scale_h"].toInt(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	else
		cover_scaled = cover_src;

	cover_result = QImage(config["icon_width"].toInt(), config["icon_height"].toInt(), QImage::Format_ARGB32_Premultiplied);
	QPainter painter_result(&cover_result);
	painter_result.setCompositionMode(QPainter::CompositionMode_Source);
	painter_result.fillRect(cover_result.rect(), Qt::transparent);

	painter_result.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter_result.drawImage(config["img_scale_pos_x"].toInt(), config["img_scale_pos_y"].toInt(), cover_scaled);

	painter_result.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter_result.drawImage(0, 0, cover_top);

	if( config["tipo_emu_show"].toBool() )
		painter_result.drawImage(config["tipo_emu_pos_x"].toInt(), config["tipo_emu_pos_y"].toInt(), img_tipo_emu);// tipo emu

	if( config["title_show"].toBool() )
	{
		QRectF rect(config["title_pos_x"].toInt(), config["title_pos_y"].toInt(), config["title_width"].toInt(), config["title_height"].toInt());
		QFont font;
		font.setFamily( config["title_font"].toString() );
		font.setPointSize( config["title_font_size"].toInt() );
		font.setBold( config["title_font_bold"].toBool() );
		font.setItalic( config["title_font_italic"].toBool() );
		font.setUnderline( config["title_font_underline"].toBool() );
		font.setStyleStrategy(QFont::PreferAntialias);
		painter_result.setFont( font );

		if(datos["Dat_titulo"].toString().size() > config["title_max_caracteres"].toInt())
			painter_result.drawText(rect, Qt::AlignCenter, datos["Dat_titulo"].toString().left(config["title_max_caracteres"].toInt())+".." );
		else
			painter_result.drawText(rect, Qt::AlignCenter, datos["Dat_titulo"].toString());
	}

//	if( datos["Dat_rating_visible"].toBool() || config["rating_show"].toBool() )
	if( datos["Dat_rating_visible"].toBool() )
	{
		int x = config["rating_pos_x"].toInt();
		for (int i = 0; i < 5; ++i)
		{
			if(i < datos["Dat_rating"].toInt())
				painter_result.drawImage(x, config["rating_pos_y"].toInt(), star_on );
			else
				painter_result.drawImage(x, config["rating_pos_y"].toInt(), star_off );
			x += 16;
		}
	}
	painter_result.setCompositionMode(QPainter::CompositionMode_DestinationOver);
	painter_result.fillRect(cover_result.rect(), Qt::transparent);
	painter_result.end();

	cover_result.save( datos["Dat_destino"].toString()+datos["Dat_img_cover"].toString()+".png", "PNG", -1);
}
