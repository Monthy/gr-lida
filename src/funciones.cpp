/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2013 Pedro A. Garcia Rosado Aka Monthy
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

#include "funciones.h"

Funciones::Funciones()
{
	stDirApp    = GRlidaHomePath();
	stTheme     = Theme();
	stThemeApp  = ThemeApp();
	stLngSelect = Idioma();
}

Funciones::~Funciones()
{
	//
}

// Compara distintas version y devuelve True si es nueva o mayor
bool Funciones::version_compare(QString ver_old, QString ver_new)
{
	QStringList v_old, v_new;
	v_old = ver_old.split(".");
	v_new = ver_new.split(".");

	if( v_old.size() < 3 )
	{
		int num = 3 - v_old.size();
		for(int i = 0; i < num; ++i)
			v_old.insert(i, "0");
		ver_old = v_old.join(".");
	}

	if( v_new.size() < 3 )
	{
		int num = 3 - v_new.size();
		for(int i = 0; i < num; ++i)
			v_new.insert(i, "0");
		ver_new = v_new.join(".");
	}

	if( v_new.at(0).toInt() > v_old.at(0).toInt() )
		return true;
	else // Major ver are =
	{
		if( v_new.at(1).toInt() > v_old.at(1).toInt() )
			return true;
		else // Minor ver are =
		{
			if( v_new.at(2).toInt() > v_old.at(2).toInt() )
				return true;
			else
				return false;
		}
	}
}

// Crea una copia de seguridad de la base de datos
void Funciones::crearCopiaSeguridad_DB(QString dir, QString version_grl)
{
	if( version_compare(version_grl, versionGrl()) )
	{
		if( QFile::exists(dir +"db_grl_v"+ version_grl +"_backup.grl") )
			QFile::copy(dir +"db_grl.grl", dir +"db_grl_v"+ version_grl +"_"+ HoraFechaActual(getTime(), "ddMMyyyy_HHmmss") +"_backup.grl");
		else
			QFile::copy(dir +"db_grl.grl", dir +"db_grl_v"+ version_grl +"_backup.grl");
	}
}

// Convierte de número a texto
QString Funciones::IntToStr(int num)
{
	return QVariant(num).toString();
}

// Convierte de texto a número
int Funciones::StrToInt(QString str)
{
	return str.toInt();
}

// Convierte texto "true", "yes", "1" a tipo bool
bool Funciones::StrToBool(QString str)
{
	QString value = str.toLower();
	if( value == "true" || value == "yes" || value == "1" )
		return true;
	else
		return false;
}

// Convierte bool a texto "true", "yes" indicando el tipo
QString Funciones::BoolToStr(bool estado, bool type_yes)
{
	if( estado )
		return type_yes ? "yes": "true";
	else
		return type_yes ? "no": "false";
}

// Devuelve un QColor desde un QStringList o QString
QColor Funciones::getColor(QStringList color)
{
	int r = 0; int g = 0; int b = 0; int a = 255;

	if( color.isEmpty() )
		color << "0" << "0" << "0";

	if( color.size() < 4 )
	{
		int pos = color.size();
		for(int i = pos; i < 3; ++i)
			color.insert(i, "0");
		color.insert(3, "255");
	}

	r = color.at(0).toInt();
	g = color.at(1).toInt();
	b = color.at(2).toInt();
	a = color.at(3).toInt();

	return QColor(r, g, b, a);
}

QColor Funciones::getColor(QString color)
{
	return getColor( color.split(",") );
}

// Convierte un QColor a un QStringList o QString
QStringList Funciones::setColor(QColor color)
{
	QStringList list_color;
	int r = 0; int g = 0; int b = 0;

	r = qRed(color.rgb());
	g = qGreen(color.rgb());
	b = qBlue(color.rgb());

	list_color << IntToStr(r) << IntToStr(g) << IntToStr(b);

	return list_color;
}

QString Funciones::setColorStr(QColor color)
{
	return setColor(color).join(",");
}

// Convierte bytes
QString Funciones::covertir_bytes(float size)
{
	QStringList list;
	list << "KB" << "MB" << "GB" << "TB";
	QStringListIterator i(list);
	QString unidad("bytes");

	while( size >= 1024.0 && i.hasNext() )
	{
		unidad = i.next();
		size /= 1024.0;
	}

	return QString().setNum(size, 'f', 2) +" "+ unidad;
}

// Convierte la lista de un QListWidget a QStringList
QStringList Funciones::lwToStrList(QListWidget *lwlist)
{
	QStringList list;
	const int listSize = lwlist->count();
	for (int i = 0; i < listSize; ++i)
		list.insert(i, lwlist->item(i)->text());

	return list;
}

// Devuelve el tiempo actual, formato unix
QString Funciones::getTime()
{
	uint secs = QDateTime::currentDateTime().toTime_t();
	return IntToStr(secs);
}

// Convierte fecha 'dd/MM/yyyy HH:mm:ss' a formato unix
QString Funciones::setTime(QString fecha, QString formato)
{
	uint secs = QDateTime::fromString(fecha, formato).toTime_t();
	return IntToStr(secs);
}

// Devuelve la hora y la fecha
QString Funciones::HoraFechaActual(QString fecha, QString formato)
{
	QDateTime dt;
	dt.setTime_t( fecha.toInt() );

	if( formato.isEmpty() )
		return dt.toString("dd/MM/yyyy HH:mm:ss");
	else
		return dt.toString( formato );
}

// Devuelve una la forma correcta
QString Funciones::str_correcto(QString str, QString str_ini, QString str_end)
{
	if( !str_ini.isEmpty() )
	{
		if( !str.startsWith(str_ini, Qt::CaseInsensitive) )
			str.prepend(str_ini);
	}

	if( !str_end.isEmpty() )
	{
		if( !str.endsWith(str_end, Qt::CaseInsensitive) )
			str.append(str_end);
	}

	return str;
}

QString Funciones::str_start(QString str, QString str_ini)
{
	return str_correcto(str, str_ini, "");
}

QString Funciones::str_end(QString str, QString str_end)
{
	return str_correcto(str, "", str_end);
}

// Elimina caracteres no permitidos por windows por ejemplo
QString Funciones::eliminar_caracteres(QString str, QString cambiar, QString sep)
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

		if( !cambiar.isEmpty() )
		{
			QStringList lista = cambiar.split(sep, QString::SkipEmptyParts);
			const int listSize = lista.size();
			for (int i = 0; i < listSize; ++i)
				str.replace(lista.at(i), "");
		}

		return str;
	}
}

// Devuelve si el archivos admiten traducción
QString Funciones::getArchivoIsLng(QString archivo)
{
	QStringList lista;
	lista << "edad_recomendada.txt" << "formatos.txt" << "generos.txt" << "idiomas.txt";
	lista << "numdisc.txt" << "perspectivas.txt" << "svm_idioma.txt" << "temas.txt";

	if( lista.contains(archivo) )
	{
		if( stLngSelect.isEmpty() )
			stLngSelect = Idioma();

		return stLngSelect +"/";
	} else
		return "";
}

// Devuelve si el archivos es de 3 columnas
bool Funciones::getArchivoIs3Col(QString archivo)
{
	QStringList lista;
	lista << "dbx_scaler.txt" << "dbx_scaler_svn.txt" << "dbx_sbtype.txt" << "dbx_sb_oplmode.txt" << "dbx_output.txt" << "dbx_output_svn.txt" << "dbx_machine.txt" << "dbx_machine_svn.txt";
	lista << "dbx_joystick.txt" << "dbx_cpu_type.txt" << "dbx_cpu_type_svn.txt" << "dbx_cpu_core.txt" << "svm_render_mode.txt" << "svm_platform.txt";
	lista << "svm_music_driver.txt" << "svm_gfxmode.txt" << "edad_recomendada.txt";// << "dbx_keyboardlayout.txt";
	lista << "svm_idioma.txt" << "idiomas.txt"  << "grupos.txt";

	if( lista.contains(archivo) )
		return true;
	else
		return false;
}

// Busca y obtiene la ruta de un archivo
QString Funciones::getFindFile(QString dir, QString filename)
{
	QString resultado = "";
	QDirIterator iterator(dir, QDirIterator::Subdirectories);
	while (iterator.hasNext())
	{
		iterator.next();
		if( !iterator.fileInfo().isDir() )
		{
			QString file_name = iterator.fileName().toLower();
			if( file_name == filename )
			{
				resultado = QDir::toNativeSeparators( iterator.filePath() );
				break;
			}
		}
	}
	return resultado;
}

// Obtiene una la lista de archivos
QFileInfoList Funciones::getListFiles(QString dir, QStringList filter)
{
	QDir m_dir( dir );
	return m_dir.entryInfoList(filter, QDir::Files | QDir::Readable | QDir::Hidden | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::Name);
}

// Obtiene información del archivo
stFileInfo Funciones::getInfoFile(QString filename, TipoHash hash)
{
	stFileInfo info;

	if( filename.isEmpty() )
	{
		info.Drive    = "";
		info.Path     = "";
		info.FilePath = "";
		info.Name     = "";
		info.NameExt  = "";
		info.cExt     = "";
		info.Ext      = "";
		info.Size     = "";
		info.Exists   = false;
		info.isDir    = false;
		info.Crc32    = "00000000";
		info.Sha1     = "";
		info.Md5      = "";
	} else {
		QFileInfo fi( filename );
		#ifdef Q_OS_WIN32
			info.Drive = filename.left(2) +"\\";
		#else
			info.Drive = "/";
		#endif
		if( fi.absolutePath().endsWith("/") || fi.absolutePath().endsWith("\\") )
			info.Path = QDir::toNativeSeparators( fi.absolutePath() );
		else
			info.Path = QDir::toNativeSeparators( fi.absolutePath().append("/") );
		info.FilePath = info.Path + fi.fileName();
		info.Name     = fi.baseName();
		info.NameExt  = fi.fileName();
		info.cExt     = "."+ fi.completeSuffix().toLower();
		info.Ext      = "."+ fi.suffix().toLower();
		info.Size     = IntToStr( fi.size() );
		info.Exists   = fi.exists();
		info.isDir = fi.isDir();

		if( hash == hashCrc32 || hash == hashAll )
			info.Crc32 = getHashFile(filename, hashCrc32);
		else
			info.Crc32 = "00000000";

		if( hash == hashSha1 || hash == hashAll )
			info.Sha1 = getHashFile(filename, hashSha1);
		else
			info.Sha1 = "";

		if( hash == hashMd5 || hash == hashAll )
			info.Md5 = getHashFile(filename, hashMd5);
		else
			info.Md5 = "";
	}

	return info;
}

// Obtiene el Hash del archivo
QString Funciones::getHashFile(QString filename, TipoHash hash)
{
	if( hash == hashCrc32 )
	{
		Crc32Qt crc;
		return crc.getCRC32toString(filename);
	} else {
		QCryptographicHash::Algorithm tipoAlgorithm = QCryptographicHash::Sha1;
		if( hash == hashMd5 )
			tipoAlgorithm = QCryptographicHash::Md5;
		else
			tipoAlgorithm = QCryptographicHash::Sha1;

		QCryptographicHash crypto(tipoAlgorithm);
		QFile file( filename );
		file.open(QFile::ReadOnly);
		while( !file.atEnd() )
			crypto.addData(file.read(8192));
		file.close();

		return QString(crypto.result().toHex());
	}
}

// Devuelve un icono montaje
QString Funciones::getIconMount(QString theme, QString tipoDrive, QString select_mount)
{
	if( tipoDrive == "drive" )
		return theme +"img16/"+ select_mount +"drive_hd.png";
	else if( tipoDrive == "cdrom" )
		return theme +"img16/"+ select_mount +"drive_cdrom.png";
	else if( tipoDrive == "floppy" )
		return theme +"img16/"+ select_mount +"drive_floppy.png";
	else if( tipoDrive == "IMG_floppy" )
		return theme +"img16/"+ select_mount +"floppy_1.png";
	else if( tipoDrive == "IMG_iso" )
		return theme +"img16/"+ select_mount +"cd_iso.png";
	else if( tipoDrive == "IMG_multi_iso" )
		return theme +"img16/"+ select_mount +"cd_multi_iso.png";
	else if( tipoDrive == "IMG_hdd" )
		return theme +"img16/"+ select_mount +"drive_hd.png";
	else if( tipoDrive == "boot" )
		return theme +"img16/"+ select_mount +"floppy_2.png";
	else
		return theme +"img16/sinimg.png";
}

// Leer archivo de texto
QString Funciones::leerArchivo(QString filename, const QByteArray code)
{
	QString lineas;
	QFile file_in( filename );
	if( file_in.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
		QTextStream in( &file_in );
		if( !code.isEmpty() )
			in.setCodec(QTextCodec::codecForName(code));
		lineas = in.readAll();
	} else
		lineas = "";
	file_in.close();

	return lineas;
}

// Copia un archivo de un directorio a otro.
bool Funciones::copiarArchivo(QString origen, QString destino, bool permisos, bool sobrescribir)
{
	QFile f_name(destino);
	if( sobrescribir && f_name.exists() )
		f_name.remove();
	bool isOk = f_name.copy(origen, destino);
	if( permisos )
		f_name.setPermissions(QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther |
							  QFile::WriteOwner | QFile::WriteUser | QFile::WriteGroup | QFile::WriteOther);
	return isOk;
}
/*
// Comprueba el archivo y si existe aumenta un digito
QString Funciones::comprobarArchivo(QString dir, QString filename)
{
	if( QFile::exists(dir + filename) )
	{
		int i = 1;
		filename.remove(0, 1);
		while( QFile::exists(dir + fGrl.IntToStr(i) + filename ) )
			++i;
		filename.prepend(fGrl.IntToStr(i));
	}
	return filename;
}
*/
// Elimina un directorio si existe
bool Funciones::eliminarDirectorio(QString nameDir)
{
	QDir directorio;
	if( directorio.exists( nameDir ) )
		return directorio.rmdir( nameDir );
	else
		return false;
}

// Renombra un directorio existe
void Funciones::renombrarDirectorio(QString oldName, QString newName)
{
	QDir directorio;
	if( directorio.exists( oldName ) )
		directorio.rename( oldName, newName );
	else
		comprobarDirectorio( newName );
}

// Comprueba si un directorio existe y sino lo crea
bool Funciones::comprobarDirectorio(QString nameDir, bool info)
{
	if( !nameDir.isEmpty() )
	{
		QDir directorio;
		if( directorio.exists( nameDir ) )
			return true;
		else {
			if( info )
				return false;
			else
				return directorio.mkpath( nameDir );
		}
	} else
		return false;
}

// Devuelve el directorio que usa el GR-lida
QString Funciones::GRlidaHomePath()
{
	QSettings settings;
	QDir dirGrl;
	bool isWinOrMac = false;
	stDirApp    = "";
	stTheme     = "";
	stLngSelect = "";

	if( QFile::exists(QDir::currentPath() +"/GR-lida.conf") )
	{
		stDirApp = QDir::currentPath() +"/";

		settings.setPath(QSettings::IniFormat, QSettings::UserScope, stDirApp +"GR-lida.conf");
		settings.beginGroup("OpcGeneral");
			setTheme( settings.value("NameDirTheme", "defecto").toString() );
			setIdioma( settings.value("IdiomaSelect", "es_ES").toString() );
		settings.endGroup();
	} else {
		#ifdef Q_OS_WIN32
			isWinOrMac = true;
		#else
			#ifdef Q_OS_MAC
				isWinOrMac = true;
			#else
				isWinOrMac = false;
			#endif
		#endif

		if( isWinOrMac )
		{
			stDirApp = QDir::homePath() +"/GR-lida/";
			if( dirGrl.exists(QDir::homePath() +"/.gr-lida/") )
			{
				dirGrl.rename(QDir::homePath() +"/.gr-lida/", QDir::homePath() +"/GR-lida/");

				settings.setPath(QSettings::IniFormat, QSettings::UserScope, stDirApp +"GR-lida.conf");
				settings.beginGroup("SqlDatabase");
					QString fileDb = settings.value("db_host", "./db_grl.grl").toString();
				settings.endGroup();

				QFileInfo info_db( getDirRelative(fileDb) );
				guardarKeyGRLConfig(stDirApp +"GR-lida.conf", "SqlDatabase", "db_host", setDirRelative(stDirApp + info_db.fileName()));
			}

			settings.setPath(QSettings::IniFormat, QSettings::UserScope, stDirApp +"GR-lida.conf");
			settings.beginGroup("OpcGeneral");
				setTheme( settings.value("NameDirTheme", "defecto").toString() );
				setIdioma( settings.value("IdiomaSelect", "es_ES").toString() );
			settings.endGroup();
		} else {
			stDirApp = QDir::homePath() +"/.gr-lida/";

			settings.setPath(QSettings::IniFormat, QSettings::UserScope, stDirApp +"GR-lida.conf");
			settings.beginGroup("OpcGeneral");
				setTheme( settings.value("NameDirTheme", "defecto").toString() );
				setIdioma( settings.value("IdiomaSelect", "es_ES").toString() );
			settings.endGroup();
		}
	}

	return stDirApp;
}


// Rutas relativas
QString Funciones::setDirRelative(QString dir, QString carpeta)
{
	QString dir_base_game = "";
	QString stDirBaseApp  = QDir::toNativeSeparators(stDirApp);

	if( carpeta == "DosGames")
	{
		QSettings settings(stDirApp +"GR-lida.conf", QSettings::IniFormat);
		settings.beginGroup("OpcGeneral");
			dir_base_game = settings.value("DirBaseGames", "./DosGames/").toString().isEmpty() ? "./DosGames/" : QDir::toNativeSeparators(settings.value("DirBaseGames").toString());
		settings.endGroup();
	}

	if( !carpeta.isEmpty() )
		carpeta.append("/");

	if( dir_base_game.isEmpty() )
		dir_base_game = "./"+ carpeta;

	if( dir.startsWith("{DirBaseGames}", Qt::CaseInsensitive) )
		dir.replace("{DirBaseGames}", dir_base_game);

	if( dir.startsWith(stDirBaseApp, Qt::CaseInsensitive) )
	{
		dir.replace(0, stDirBaseApp.length(), "");

		if( dir.startsWith("/") || dir.startsWith("\\") )
			return QDir::toNativeSeparators( dir.prepend(".") );
		else
			return QDir::toNativeSeparators( dir.prepend("./") );
	} else
		return QDir::toNativeSeparators( dir );
}

QString Funciones::getDirRelative(QString dir, QString carpeta)
{
	QString dir_base_game = "";
	QString stDirBaseApp  = QDir::toNativeSeparators(stDirApp);

	if( carpeta == "DosGames" )
	{
		QSettings settings(stDirApp +"GR-lida.conf", QSettings::IniFormat);
		settings.beginGroup("OpcGeneral");
			dir_base_game = settings.value("DirBaseGames", "./DosGames/").toString().isEmpty() ? "./DosGames/" : QDir::toNativeSeparators(settings.value("DirBaseGames").toString());
		settings.endGroup();
	}

	if( !carpeta.isEmpty() )
		carpeta.append("/");

	if( dir_base_game.isEmpty() )
		dir_base_game = "./"+ carpeta;

	if( dir_base_game.startsWith("./") || dir_base_game.startsWith(".\\") )
		dir_base_game = stDirBaseApp;
	else {
		if( dir.startsWith("./DosGames/") || dir.startsWith(".\\DosGames\\") )
			dir.replace(0, 11, "./");
	}

	if( dir.startsWith("./") || dir.startsWith(".\\") )
		return QDir::toNativeSeparators( dir.replace(0, 2, dir_base_game) );
	else
		return QDir::toNativeSeparators( dir );
}

// Devuelve el directorio del Theme a usar
void Funciones::setTheme(QString theme)
{
	QDir themeDir;

	if( theme.toLower() == "defecto" || theme.isEmpty() )
	{
	//	if( themeDir.exists(stDirApp +"themes/defecto/") )
	//		stTheme = stDirApp +"themes/defecto/";
	//	else
			stTheme = ":/";
	} else {
		if( themeDir.exists(stDirApp +"themes/"+ theme +"/") )
			stTheme = stDirApp +"themes/"+ theme +"/";
		else {
		//	if( themeDir.exists(stDirApp +"themes/defecto/") )
		//		stTheme = stDirApp +"themes/defecto/";
		//	else
				stTheme = ":/";
		}
	}

	if( stTheme == ":/" )
		stThemeApp = stDirApp +"themes/defecto/";
	else
		stThemeApp = stTheme;
}

void Funciones::setIdioma(QString lng)
{
	if( lng.isEmpty() )
		stLngSelect = "es_ES";
	else {
		QStringList lngs;
		lngs << "da_DK" << "en_EN" << "es_ES" << "fr_FR" << "hu_HU" << "ru_RU";

		if( lngs.contains(lng) )
			stLngSelect = lng;
		else
			stLngSelect = "es_ES";
	}
}

// Carga la hoja de estilo y reemplazando el comodin <theme> por el que se use
QString Funciones::myStyleSheet(bool isList)
{
	if( isList )
		return leerArchivo(stThemeApp +"StyleSheetList.qss", "UTF-8").replace("<theme>", stThemeApp) +"\n";
	else
		return leerArchivo(stThemeApp +"StyleSheet.qss", "UTF-8").replace("<theme>", stThemeApp) +"\n";
}

// Carga la configuración de los iconos de la lista en modo Icono
stLwIconCfg Funciones::cargarListWidgetIconConf(QString tabla)
{
	QString file_cfg, dir_cfg, file_lwConf;
	stLwIconCfg lwConf;

	dir_cfg  = (stTheme == ":/" ? stThemeApp : stTheme);
	file_cfg = (tabla != "dbgrl" ? tabla +"_list_cfg.ini" : "list_cfg.ini");

	if( QFile::exists(dir_cfg + file_cfg) )
		file_lwConf = dir_cfg + file_cfg;
	else
		file_lwConf = dir_cfg +"list_cfg.ini";

	QSettings settings(file_lwConf, QSettings::IniFormat);
	settings.beginGroup("tw_list_icon");
		lwConf.tw_icon_width  = settings.value("tw_icon_width" , 24).toInt();
		lwConf.tw_icon_height = settings.value("tw_icon_height", 24).toInt();
	settings.endGroup();
	settings.beginGroup("picflow_img");
		lwConf.pf_img_width  = settings.value("pf_img_width" , 145).toInt();
		lwConf.pf_img_height = settings.value("pf_img_height", 186).toInt();
	settings.endGroup();
	settings.beginGroup("list_icon");
		lwConf.icon_width           = settings.value("icon_width"          , 200     ).toInt();
		lwConf.icon_height          = settings.value("icon_height"         , 243     ).toInt();
		lwConf.img_cover_top        = settings.value("img_cover_top"       , "images/list_cover_top.png").toString();
		lwConf.img_cover_top_select = settings.value("img_cover_top_select", "images/list_cover_top_select.png").toString();
		lwConf.img_cover_top_pos_x  = settings.value("img_cover_top_pos_x" , 22      ).toInt();
		lwConf.img_cover_top_pos_y  = settings.value("img_cover_top_pos_y" , 22      ).toInt();
		lwConf.img_scaled           = settings.value("img_scaled"          , true    ).toBool();
		lwConf.img_scale_w          = settings.value("img_scale_w"         , 145     ).toInt();
		lwConf.img_scale_h          = settings.value("img_scale_h"         , 186     ).toInt();
		lwConf.img_scale_pos_x      = settings.value("img_scale_pos_x"     , 28      ).toInt();
		lwConf.img_scale_pos_y      = settings.value("img_scale_pos_y"     , 28      ).toInt();
		lwConf.tipo_emu_show        = settings.value("tipo_emu_show"       , true    ).toBool();
		lwConf.tipo_emu_pos_x       = settings.value("tipo_emu_pos_x"      , 15      ).toInt();
		lwConf.tipo_emu_pos_y       = settings.value("tipo_emu_pos_y"      , 202     ).toInt();
		lwConf.rating_show          = settings.value("rating_show"         , true    ).toBool();
		lwConf.rating_vertical      = settings.value("rating_vertical"     , false   ).toBool();
		lwConf.rating_pos_x         = settings.value("rating_pos_x"        , 105     ).toInt();
		lwConf.rating_pos_y         = settings.value("rating_pos_y"        , 202     ).toInt();
		lwConf.title_bg_show        = settings.value("title_bg_show"       , true    ).toBool();
		lwConf.title_bg             = settings.value("title_bg"            , "images/list_cover_title_bg.png").toString();
		lwConf.title_bg_select      = settings.value("title_bg_select"     , "images/list_cover_title_bg.png").toString();
		lwConf.title_bg_pos_x       = settings.value("title_bg_pos_x"      , 13      ).toInt();
		lwConf.title_bg_pos_y       = settings.value("title_bg_pos_y"      , 222     ).toInt();
		lwConf.title_show           = settings.value("title_show"          , true    ).toBool();
		lwConf.title_pos_x          = settings.value("title_pos_x"         , 21      ).toInt();
		lwConf.title_pos_y          = settings.value("title_pos_y"         , 222     ).toInt();
		lwConf.title_width          = settings.value("title_width"         , 156     ).toInt();
		lwConf.title_height         = settings.value("title_height"        , 16      ).toInt();
		lwConf.title_max_caracteres = settings.value("title_max_caracteres", 25      ).toInt();
		lwConf.title_font           = settings.value("title_font"          , "Tahoma").toString();
		lwConf.title_font_size      = settings.value("title_font_size"     , 7       ).toInt();
		lwConf.title_font_color     = settings.value("title_font_color"    , QStringList() << "0" << "0" << "0").toStringList();
		lwConf.title_font_color_select = settings.value("title_font_color_select", QStringList() << "0" << "0" << "0").toStringList();
		lwConf.title_font_bold      = settings.value("title_font_bold"     , true    ).toBool();
		lwConf.title_font_italic    = settings.value("title_font_italic"   , false   ).toBool();
		lwConf.title_font_underline = settings.value("title_font_underline", false   ).toBool();
	settings.endGroup();

	return lwConf;
}

// Crea la configuración de los iconos de la lista en modo Icono
void Funciones::guardarListWidgetIconConf(stLwIconCfg lwConf, QString tabla)
{
	QString file_cfg, dir_cfg, file_lwConf;

	dir_cfg     = (stTheme == ":/" ? stThemeApp : stTheme);
	file_cfg    = (tabla != "dbgrl" ? tabla +"_list_cfg.ini" : "list_cfg.ini");
	file_lwConf = dir_cfg + file_cfg;

	QSettings settings(file_lwConf, QSettings::IniFormat);
	settings.clear();
	settings.beginGroup("tw_list_icon");
		settings.setValue("tw_icon_width" , lwConf.tw_icon_width);
		settings.setValue("tw_icon_height", lwConf.tw_icon_height);
	settings.endGroup();
	settings.beginGroup("picflow_img");
		settings.setValue("pf_img_width" , lwConf.pf_img_width);
		settings.setValue("pf_img_height", lwConf.pf_img_height);
	settings.endGroup();
	settings.beginGroup("list_icon");
		settings.setValue("icon_width"             , lwConf.icon_width             );
		settings.setValue("icon_height"            , lwConf.icon_height            );
		settings.setValue("img_cover_top"          , lwConf.img_cover_top          );
		settings.setValue("img_cover_top_select"   , lwConf.img_cover_top_select   );
		settings.setValue("img_cover_top_pos_x"    , lwConf.img_cover_top_pos_x    );
		settings.setValue("img_cover_top_pos_y"    , lwConf.img_cover_top_pos_y    );
		settings.setValue("img_scaled"             , lwConf.img_scaled             );
		settings.setValue("img_scale_w"            , lwConf.img_scale_w            );
		settings.setValue("img_scale_h"            , lwConf.img_scale_h            );
		settings.setValue("img_scale_pos_x"        , lwConf.img_scale_pos_x        );
		settings.setValue("img_scale_pos_y"        , lwConf.img_scale_pos_y        );
		settings.setValue("tipo_emu_show"          , lwConf.tipo_emu_show          );
		settings.setValue("tipo_emu_pos_x"         , lwConf.tipo_emu_pos_x         );
		settings.setValue("tipo_emu_pos_y"         , lwConf.tipo_emu_pos_y         );
		settings.setValue("rating_show"            , lwConf.rating_show            );
		settings.setValue("rating_vertical"        , lwConf.rating_vertical        );
		settings.setValue("rating_pos_x"           , lwConf.rating_pos_x           );
		settings.setValue("rating_pos_y"           , lwConf.rating_pos_y           );
		settings.setValue("title_bg_show"          , lwConf.title_bg_show          );
		settings.setValue("title_bg"               , lwConf.title_bg               );
		settings.setValue("title_bg_select"        , lwConf.title_bg_select        );
		settings.setValue("title_bg_pos_x"         , lwConf.title_bg_pos_x         );
		settings.setValue("title_bg_pos_y"         , lwConf.title_bg_pos_y         );
		settings.setValue("title_show"             , lwConf.title_show             );
		settings.setValue("title_pos_x"            , lwConf.title_pos_x            );
		settings.setValue("title_pos_y"            , lwConf.title_pos_y            );
		settings.setValue("title_width"            , lwConf.title_width            );
		settings.setValue("title_height"           , lwConf.title_height           );
		settings.setValue("title_max_caracteres"   , lwConf.title_max_caracteres   );
		settings.setValue("title_font"             , lwConf.title_font             );
		settings.setValue("title_font_size"        , lwConf.title_font_size        );
		settings.setValue("title_font_color"       , lwConf.title_font_color       );
		settings.setValue("title_font_color_select", lwConf.title_font_color_select);
		settings.setValue("title_font_bold"        , lwConf.title_font_bold        );
		settings.setValue("title_font_italic"      , lwConf.title_font_italic      );
		settings.setValue("title_font_underline"   , lwConf.title_font_underline   );
	settings.endGroup();
}

// Carga la configuracion del GR-lida
stGrlCfg Funciones::cargarGRLConfig(QString iniFileName)
{
	stGrlCfg config;

	QSettings settings(iniFileName, QSettings::IniFormat);
	settings.beginGroup("SqlDatabase");
		config.db_type      = settings.value("db_type"     , "QSQLITE").toString();
		config.db_server    = settings.value("db_server"   , "localhost").toString();
		config.db_host      = settings.value("db_host"     , "./db_grl.grl").toString();
		config.db_name      = settings.value("db_name"     , "").toString();
		config.db_username  = settings.value("db_username" , "").toString();
		config.db_password  = settings.value("db_password" , "").toString();
		config.db_port      = settings.value("db_port"     , "3306").toString();
		config.db_orden_col = settings.value("db_orden_col", "titulo").toString();
		config.db_orden_by  = settings.value("db_orden_by" , "titulo").toString();
		config.db_orden     = settings.value("db_orden"    , "ASC").toString();
	settings.endGroup();

	settings.beginGroup("OpcGeneral");
		config.Primeravez    = settings.value("Primeravez"   , true).toBool();
		config.DirApp        = settings.value("DirApp"       , "homepath").toString();
		config.DirDOSBox     = settings.value("DirDOSBox"    , "").toString();
		config.DirScummVM    = settings.value("DirScummVM"   , "").toString();
		config.DirBaseGames  = settings.value("DirBaseGames" , "./DosGames/").toString().isEmpty() ? "./DosGames/" : QDir::toNativeSeparators( settings.value("DirBaseGames").toString() );
		config.DOSBoxDefault = settings.value("DOSBoxDefault", "dosbox").toString();
		config.DOSBoxDisp    = settings.value("DOSBoxDisp"   , false).toBool();
		config.ScummVMDisp   = settings.value("ScummVMDisp"  , false).toBool();
	#ifdef Q_OS_WIN32
		config.VDMSoundDisp = settings.value("VDMSoundDisp", false).toBool();
	#else
		config.VDMSoundDisp = false;
	#endif
		config.IdiomaSelect        = settings.value("IdiomaSelect"       , "es_ES").toString();
		config.OpenPdfExternal     = settings.value("OpenPdfExternal"    , false  ).toBool();
		config.url_xmldb           = settings.value("url_xmldb"          , "gr-lida.js").toString();
		config.FormatoFecha        = settings.value("FormatoFecha"       , "dd/MM/yyyy HH:mm:ss").toString();
		config.ToolbarBigIcon      = settings.value("ToolbarBigIcon"     , true).toBool();
		config.Style               = settings.value("Style"              , "").toString();
		config.StylePalette        = settings.value("StylePalette"       , false).toBool();
		config.NameDirTheme        = settings.value("NameDirTheme"       , "defecto").toString();
		config.IconoFav            = settings.value("IconoFav"           , "fav_0.png").toString();
		config.PicFlowType         = settings.value("PicFlowType"        , "CoverFlowLike").toString();
		config.PicFlowBgColor      = settings.value("PicFlowBgColor"     , QStringList() << "0" << "0" << "0").toStringList();
		config.PicFlowShowNumPos   = settings.value("PicFlowShowNumPos"  , false).toBool();
		config.PicFlowShowTriangle = settings.value("PicFlowShowTriangle", false).toBool();
		config.PicFlowMinHeight    = settings.value("PicFlowMinHeight"   , 260).toInt();
		config.Skip_PicFlow        = settings.value("Skip_PicFlow"       , 10).toInt();
		config.IndexTabArchivos    = settings.value("IndexTabArchivos"   , 0).toInt();
		config.VersionDBx          = settings.value("VersionDBx"         , "0.74").toString();
		config.DOSBoxSVN           = settings.value("DOSBoxSVN"          , false).toBool();
		config.LastSelectCatID     = settings.value("LastSelectCatID"    , "0").toString();
		config.LastTabOptSelect    = settings.value("LastTabOptSelect"   , 0).toInt();
	settings.endGroup();

	settings.beginGroup("OpcFuente");
		config.font_usar   = settings.value("font_usar"  , false   ).toBool();
		config.font_family = settings.value("font_family", "Tahoma").toString();
		config.font_size   = settings.value("font_size"  , "8"     ).toString();
	settings.endGroup();

	settings.beginGroup("HttpProxy");
		config.ProxyEnable   = settings.value("ProxyEnable"  , false).toBool();
		config.ProxyType     = settings.value("ProxyType"    , 2    ).toInt();
		config.ProxyHost     = settings.value("ProxyHost"    , ""   ).toString();
		config.ProxyPort     = settings.value("ProxyPort"    , "1080").toString();
		config.ProxyLogin    = settings.value("ProxyLogin"   , false).toBool();
		config.ProxyUserName = settings.value("ProxyUserName", ""   ).toString();
		config.ProxyPassword = settings.value("ProxyPassword", ""   ).toString();
	settings.endGroup();

	settings.beginGroup("MainState");
		config.main_fullscreen     = settings.value("main_fullscreen"    , false).toBool();
		config.main_maximized      = settings.value("main_maximized"     , false).toBool();
		config.main_geometry       = settings.value("main_geometry"      , "").toByteArray();
		config.main_state          = settings.value("main_state"         , "").toByteArray();
		config.main_twJuegos_state = settings.value("main_twJuegos_state", "").toByteArray();
	settings.endGroup();

	settings.beginGroup("OpcVer");
		config.Pnl_Nav               = settings.value("Pnl_Nav"              , false).toBool();
		config.Pnl_FilesUrl          = settings.value("Pnl_FilesUrl"         , false).toBool();
		config.Pnl_Datos             = settings.value("Pnl_Datos"            , true ).toBool();
		config.Pnl_PictureFlow       = settings.value("Pnl_PictureFlow"      , true ).toBool();
		config.Pnl_Capturas          = settings.value("Pnl_Capturas"         , false).toBool();
		config.Pnl_Menubar           = settings.value("Pnl_Menubar"          , true ).toBool();
		config.Pnl_Toolbar           = settings.value("Pnl_Toolbar"          , true ).toBool();
		config.Pnl_Ordenar           = settings.value("Pnl_Ordenar"          , false).toBool();
		config.Pnl_StatusBar         = settings.value("Pnl_StatusBar"        , true ).toBool();
		config.PictureFlowToCenter   = settings.value("PictureFlowToCenter"  , false).toBool();
		config.ver_IconMode          = settings.value("ver_IconMode"         , false).toBool();
		config.ver_col_Icono         = settings.value("ver_col_Icono"        , true ).toBool();
		config.ver_col_Subtitulo     = settings.value("ver_col_Subtitulo"    , false).toBool();
		config.ver_col_Genero        = settings.value("ver_col_Genero"       , false).toBool();
		config.ver_col_Compania      = settings.value("ver_col_Compania"     , true ).toBool();
		config.ver_col_Desarrollador = settings.value("ver_col_Desarrollador", false).toBool();
		config.ver_col_Tema          = settings.value("ver_col_Tema"         , false).toBool();
		config.ver_col_Grupo         = settings.value("ver_col_Grupo"        , false).toBool();
		config.ver_col_Perspectiva   = settings.value("ver_col_Perspectiva"  , false).toBool();
		config.ver_col_Idioma        = settings.value("ver_col_Idioma"       , false).toBool();
		config.ver_col_IdiomaVoces   = settings.value("ver_col_IdiomaVoces"  , false).toBool();
		config.ver_col_Formato       = settings.value("ver_col_Formato"      , false).toBool();
		config.ver_col_Anno          = settings.value("ver_col_Anno"         , true ).toBool();
		config.ver_col_NumDisc       = settings.value("ver_col_NumDisc"      , false).toBool();
		config.ver_col_SistemaOp     = settings.value("ver_col_SistemaOp"    , false).toBool();
		config.ver_col_Tamano        = settings.value("ver_col_Tamano"       , false).toBool();
		config.ver_col_Graficos      = settings.value("ver_col_Graficos"     , false).toBool();
		config.ver_col_Sonido        = settings.value("ver_col_Sonido"       , false).toBool();
		config.ver_col_Jugabilidad   = settings.value("ver_col_Jugabilidad"  , false).toBool();
		config.ver_col_Original      = settings.value("ver_col_Original"     , false).toBool();
		config.ver_col_Estado        = settings.value("ver_col_Estado"       , false).toBool();
		config.ver_col_Fecha         = settings.value("ver_col_Fecha"        , false).toBool();
		config.ver_col_TipoEmu       = settings.value("ver_col_TipoEmu"      , false).toBool();
		config.ver_col_Favorito      = settings.value("ver_col_Favorito"     , false).toBool();
		config.ver_col_Rating        = settings.value("ver_col_Rating"       , true ).toBool();
		config.ver_col_Edad          = settings.value("ver_col_Edad"         , false).toBool();
		config.ver_col_Usuario       = settings.value("ver_col_Usuario"      , false).toBool();
	settings.endGroup();

	settings.beginGroup("ImageViewerState");
		config.img_maximized        = settings.value("img_maximized"       , false).toBool();
		config.img_fitToWindow      = settings.value("img_fitToWindow"     , false).toBool();
		config.img_verListaImagenes = settings.value("img_verListaImagenes", false).toBool();
	settings.endGroup();

	settings.beginGroup("OpcMultimedia");
		config.FormatsVideo = settings.value("FormatsVideo", QStringList() << "*.avi" << "*.mkv" << "*.mov" << "*.mp4" << "*.mpeg" << "*.mpg" << "*.wmv").toStringList();
		config.FormatsMusic = settings.value("FormatsMusic", QStringList() << "*.mp3" << "*.ogg" << "*.wav" << "*.wma").toStringList();
		config.FormatsImage = settings.value("FormatsImage", QStringList() << "*.bmp" << "*.gif" << "*.jpg" << "*.jpeg" << "*.png" << "*.pbm" << "*.pgm" << "*.ppm" << "*.tiff" << "*.xbm" << "*.xpm" << "*.svg").toStringList();
	settings.endGroup();

	settings.beginGroup("UltimoDirectorio");
		config.DirBD         = QDir::toNativeSeparators(settings.value("DirBD" , "").toString());
		config.DirImportar   = QDir::toNativeSeparators(settings.value("DirImportar"  , "").toString());
		config.DirExportPath = QDir::toNativeSeparators(settings.value("DirExportPath", "").toString());
		config.DirOtherEmus  = QDir::toNativeSeparators(settings.value("DirOtherEmus", "").toString());
	// Datos de Juegos
		config.Img_Thumbs          = QDir::toNativeSeparators(settings.value("Img_Thumbs"                 , "").toString());
		config.Img_CoverFront      = QDir::toNativeSeparators(settings.value("Img_CoverFront"             , "").toString());
		config.Img_CoverBack       = QDir::toNativeSeparators(settings.value("Img_CoverBack"              , "").toString());
		config.DatosFiles_PathFile = QDir::toNativeSeparators(settings.value("DatosFiles_PathFile"        , "").toString());
		config.DatosFiles_PathExe  = QDir::toNativeSeparators(settings.value("DatosFiles_PathExe"         , "").toString());
		config.DatosFiles_PathSetup    = QDir::toNativeSeparators(settings.value("DatosFiles_PathSetup"   , "").toString());
		config.DatosFiles_PathCapturas = QDir::toNativeSeparators(settings.value("DatosFiles_PathCapturas", "").toString());
	// DOSBox
		config.Dbx_path            = QDir::toNativeSeparators(settings.value("Dbx_path"           , "").toString());
		config.Dbx_path_exe        = QDir::toNativeSeparators(settings.value("Dbx_path_exe"       , "").toString());
		config.Dbx_path_setup      = QDir::toNativeSeparators(settings.value("Dbx_path_setup"     , "").toString());
		config.Dbx_gus_ultradir    = QDir::toNativeSeparators(settings.value("Dbx_gus_ultradir"   , "").toString());
		config.Dbx_sdl_mapperfile  = QDir::toNativeSeparators(settings.value("Dbx_sdl_mapperfile" , "").toString());
		config.Dbx_dosbox_language = QDir::toNativeSeparators(settings.value("Dbx_dosbox_language", "").toString());
		config.Dbx_dosbox_captures = QDir::toNativeSeparators(settings.value("Dbx_dosbox_captures", "").toString());
		config.Dbx_path_sonido     = QDir::toNativeSeparators(settings.value("Dbx_path_sonido"    , "").toString());
		config.Montaje_path        = QDir::toNativeSeparators(settings.value("Montaje_path"       , "").toString());
	// ScummVM
		config.Svm_path          = QDir::toNativeSeparators(settings.value("Svm_path"         , "").toString());
		config.Svm_path_game     = QDir::toNativeSeparators(settings.value("Svm_path_game"    , "").toString());
		config.Svm_path_save     = QDir::toNativeSeparators(settings.value("Svm_path_save"    , "").toString());
		config.Svm_path_extra    = QDir::toNativeSeparators(settings.value("Svm_path_extra"   , "").toString());
		config.Svm_path_capturas = QDir::toNativeSeparators(settings.value("Svm_path_capturas", "").toString());
		config.Svm_path_sonido   = QDir::toNativeSeparators(settings.value("Svm_path_sonido"  , "").toString());
		config.Svm_soundfont     = QDir::toNativeSeparators(settings.value("Svm_soundfont"    , "").toString());
	// VDMSound
		config.Vdms_path_exe = QDir::toNativeSeparators(settings.value("Vdms_path_exe", "").toString());
		config.Vdms_icon     = QDir::toNativeSeparators(settings.value("Vdms_icon"    , "").toString());
	settings.endGroup();

	settings.beginGroup("Updates");
		config.chkVersion  = settings.value("chkVersion" , false).toBool();
		config.SoloUpdates = settings.value("SoloUpdates", false).toBool();
		config.Version     = settings.value("Version"    , versionGrl()).toString();
		config.VerListSvm  = settings.value("VerListSvm" , versionSvm()).toString();
	settings.endGroup();

	config.isChangedCategoria  = false;
	config.isChangedEmuList    = false;
	config.isChangedIdioma     = false;
	config.isChangedListDOSBox = false;

	setIdioma( config.IdiomaSelect );
	setTheme( config.NameDirTheme );

	return config;
}

// Guarda la configuración del GR-lida
void Funciones::guardarGRLConfig(QString iniFileName, stGrlCfg config)
{
	QSettings settings(iniFileName, QSettings::IniFormat);
	settings.clear();

	settings.beginGroup("SqlDatabase");
		settings.setValue("db_type"     , config.db_type      );
		settings.setValue("db_server"   , config.db_server    );
		settings.setValue("db_host"     , config.db_host      );
		settings.setValue("db_name"     , config.db_name      );
		settings.setValue("db_username" , config.db_username  );
		settings.setValue("db_password" , config.db_password  );
		settings.setValue("db_port"     , config.db_port      );
		settings.setValue("db_orden_col", config.db_orden_col );
		settings.setValue("db_orden_by" , config.db_orden_by  );
		settings.setValue("db_orden"    , config.db_orden     );
	settings.endGroup();

	settings.beginGroup("OpcGeneral");
		settings.setValue("Primeravez"         , config.Primeravez          );
		settings.setValue("DirApp"             , config.DirApp              );
		settings.setValue("DirDOSBox"          , config.DirDOSBox           );
		settings.setValue("DirScummVM"         , config.DirScummVM          );
		settings.setValue("DirBaseGames"       , config.DirBaseGames        );
		settings.setValue("DOSBoxDefault"      , config.DOSBoxDefault       );
		settings.setValue("DOSBoxDisp"         , config.DOSBoxDisp          );
		settings.setValue("ScummVMDisp"        , config.ScummVMDisp         );
		settings.setValue("VDMSoundDisp"       , config.VDMSoundDisp        );
		settings.setValue("IdiomaSelect"       , config.IdiomaSelect        );
		settings.setValue("OpenPdfExternal"    , config.OpenPdfExternal     );
		settings.setValue("url_xmldb"          , config.url_xmldb           );
		settings.setValue("FormatoFecha"       , config.FormatoFecha        );
		settings.setValue("ToolbarBigIcon"     , config.ToolbarBigIcon      );
		settings.setValue("Style"              , config.Style               );
		settings.setValue("StylePalette"       , config.StylePalette        );
		settings.setValue("NameDirTheme"       , config.NameDirTheme        );
		settings.setValue("IconoFav"           , config.IconoFav            );
		settings.setValue("PicFlowType"        , config.PicFlowType         );
		settings.setValue("PicFlowBgColor"     , config.PicFlowBgColor      );
		settings.setValue("PicFlowShowNumPos"  , config.PicFlowShowNumPos   );
		settings.setValue("PicFlowShowTriangle", config.PicFlowShowTriangle );
		settings.setValue("PicFlowMinHeight"   , config.PicFlowMinHeight    );
		settings.setValue("Skip_PicFlow"       , config.Skip_PicFlow        );
		settings.setValue("IndexTabArchivos"   , config.IndexTabArchivos    );
		settings.setValue("VersionDBx"         , config.VersionDBx          );
		settings.setValue("DOSBoxSVN"          , config.DOSBoxSVN           );
		settings.setValue("LastSelectCatID"    , config.LastSelectCatID     );
		settings.setValue("LastTabOptSelect"   , config.LastTabOptSelect    );
	settings.endGroup();

	settings.beginGroup("OpcFuente");
		settings.setValue("font_usar"  , config.font_usar   );
		settings.setValue("font_family", config.font_family );
		settings.setValue("font_size"  , config.font_size   );
	settings.endGroup();

	settings.beginGroup("HttpProxy");
		settings.setValue("ProxyEnable"  , config.ProxyEnable   );
		settings.setValue("ProxyType"    , config.ProxyType     );
		settings.setValue("ProxyHost"    , config.ProxyHost     );
		settings.setValue("ProxyPort"    , config.ProxyPort     );
		settings.setValue("ProxyLogin"   , config.ProxyLogin    );
		settings.setValue("ProxyUserName", config.ProxyUserName );
		settings.setValue("ProxyPassword", config.ProxyPassword );
	settings.endGroup();

	settings.beginGroup("MainState");
		settings.setValue("main_fullscreen"    , config.main_fullscreen     );
		settings.setValue("main_maximized"     , config.main_maximized      );
		settings.setValue("main_geometry"      , config.main_geometry       );
		settings.setValue("main_state"         , config.main_state          );
		settings.setValue("main_twJuegos_state", config.main_twJuegos_state );
	settings.endGroup();

	settings.beginGroup("OpcVer");
		settings.setValue("Pnl_Nav"              , config.Pnl_Nav               );
		settings.setValue("Pnl_FilesUrl"         , config.Pnl_FilesUrl          );
		settings.setValue("Pnl_Datos"            , config.Pnl_Datos             );
		settings.setValue("Pnl_PictureFlow"      , config.Pnl_PictureFlow       );
		settings.setValue("Pnl_Capturas"         , config.Pnl_Capturas          );
		settings.setValue("Pnl_Menubar"          , config.Pnl_Menubar           );
		settings.setValue("Pnl_Toolbar"          , config.Pnl_Toolbar           );
		settings.setValue("Pnl_Ordenar"          , config.Pnl_Ordenar           );
		settings.setValue("Pnl_StatusBar"        , config.Pnl_StatusBar         );
		settings.setValue("PictureFlowToCenter"  , config.PictureFlowToCenter   );
		settings.setValue("ver_IconMode"         , config.ver_IconMode          );
		settings.setValue("ver_col_Icono"        , config.ver_col_Icono         );
		settings.setValue("ver_col_Subtitulo"    , config.ver_col_Subtitulo     );
		settings.setValue("ver_col_Genero"       , config.ver_col_Genero        );
		settings.setValue("ver_col_Compania"     , config.ver_col_Compania      );
		settings.setValue("ver_col_Desarrollador", config.ver_col_Desarrollador );
		settings.setValue("ver_col_Tema"         , config.ver_col_Tema          );
		settings.setValue("ver_col_Grupo"        , config.ver_col_Grupo         );
		settings.setValue("ver_col_Perspectiva"  , config.ver_col_Perspectiva   );
		settings.setValue("ver_col_Idioma"       , config.ver_col_Idioma        );
		settings.setValue("ver_col_IdiomaVoces"  , config.ver_col_IdiomaVoces   );
		settings.setValue("ver_col_Formato"      , config.ver_col_Formato       );
		settings.setValue("ver_col_Anno"         , config.ver_col_Anno          );
		settings.setValue("ver_col_NumDisc"      , config.ver_col_NumDisc       );
		settings.setValue("ver_col_SistemaOp"    , config.ver_col_SistemaOp     );
		settings.setValue("ver_col_Tamano"       , config.ver_col_Tamano        );
		settings.setValue("ver_col_Graficos"     , config.ver_col_Graficos      );
		settings.setValue("ver_col_Sonido"       , config.ver_col_Sonido        );
		settings.setValue("ver_col_Jugabilidad"  , config.ver_col_Jugabilidad   );
		settings.setValue("ver_col_Original"     , config.ver_col_Original      );
		settings.setValue("ver_col_Estado"       , config.ver_col_Estado        );
		settings.setValue("ver_col_Fecha"        , config.ver_col_Fecha         );
		settings.setValue("ver_col_TipoEmu"      , config.ver_col_TipoEmu       );
		settings.setValue("ver_col_Favorito"     , config.ver_col_Favorito      );
		settings.setValue("ver_col_Rating"       , config.ver_col_Rating        );
		settings.setValue("ver_col_Edad"         , config.ver_col_Edad          );
		settings.setValue("ver_col_Usuario"      , config.ver_col_Usuario       );
	settings.endGroup();

	settings.beginGroup("ImageViewerState");
		settings.setValue("img_maximized"       , config.img_maximized        );
		settings.setValue("img_fitToWindow"     , config.img_fitToWindow      );
		settings.setValue("img_verListaImagenes", config.img_verListaImagenes );
	settings.endGroup();

	settings.beginGroup("OpcMultimedia");
		settings.setValue("FormatsVideo", config.FormatsVideo );
		settings.setValue("FormatsMusic", config.FormatsMusic );
		settings.setValue("FormatsImage", config.FormatsImage );
	settings.endGroup();

	settings.beginGroup("UltimoDirectorio");
		settings.setValue("DirBD"        , config.DirBD         );
		settings.setValue("DirImportar"  , config.DirImportar   );
		settings.setValue("DirExportPath", config.DirExportPath );
		settings.setValue("DirOtherEmus" , config.DirOtherEmus  );
	// Datos de Juegos
		settings.setValue("Img_Thumbs"             , config.Img_Thumbs              );
		settings.setValue("Img_CoverFront"         , config.Img_CoverFront          );
		settings.setValue("Img_CoverBack"          , config.Img_CoverBack           );
		settings.setValue("DatosFiles_PathFile"    , config.DatosFiles_PathFile     );
		settings.setValue("DatosFiles_PathExe"     , config.DatosFiles_PathExe      );
		settings.setValue("DatosFiles_PathSetup"   , config.DatosFiles_PathSetup    );
		settings.setValue("DatosFiles_PathCapturas", config.DatosFiles_PathCapturas );
	// DOSBox
		settings.setValue("Dbx_path"           , config.Dbx_path            );
		settings.setValue("Dbx_path_exe"       , config.Dbx_path_exe        );
		settings.setValue("Dbx_path_setup"     , config.Dbx_path_setup      );
		settings.setValue("Dbx_gus_ultradir"   , config.Dbx_gus_ultradir    );
		settings.setValue("Dbx_sdl_mapperfile" , config.Dbx_sdl_mapperfile  );
		settings.setValue("Dbx_dosbox_language", config.Dbx_dosbox_language );
		settings.setValue("Dbx_dosbox_captures", config.Dbx_dosbox_captures );
		settings.setValue("Dbx_path_sonido"    , config.Dbx_path_sonido     );
		settings.setValue("Montaje_path"       , config.Montaje_path        );
	// ScummVM
		settings.setValue("Svm_path"         , config.Svm_path          );
		settings.setValue("Svm_path_game"    , config.Svm_path_game     );
		settings.setValue("Svm_path_save"    , config.Svm_path_save     );
		settings.setValue("Svm_path_extra"   , config.Svm_path_extra    );
		settings.setValue("Svm_path_capturas", config.Svm_path_capturas );
		settings.setValue("Svm_path_sonido"  , config.Svm_path_sonido   );
		settings.setValue("Svm_soundfont"    , config.Svm_soundfont     );
	// VDMSound
		settings.setValue("Vdms_path_exe", config.Vdms_path_exe );
		settings.setValue("Vdms_icon"    , config.Vdms_icon     );
	settings.endGroup();

	settings.beginGroup("Updates");
		settings.setValue("chkVersion" , config.chkVersion  );
		settings.setValue("SoloUpdates", config.SoloUpdates );
		settings.setValue("Version"    , config.Version     );
		settings.setValue("VerListSvm" , config.VerListSvm  );
	settings.endGroup();
}

// Guarda la configuración del GR-lida
void Funciones::guardarKeyGRLConfig(QString iniFileName, QString grupo, QString key, QString value)
{
	QSettings settings(iniFileName, QSettings::IniFormat);
	settings.beginGroup(grupo);
		settings.setValue(key, value);
	settings.endGroup();
}

// Obtiene la dirección y el nombre del archivo atraves de QFileDialog
QString Funciones::ventanaAbrirArchivos(QString caption, QString dir, QString dir_relative, QString filter, QString *selectedFilter, bool isSave)
{
	QString archivo = "";
	QString base    = getDirRelative(dir, dir_relative);

	if( !comprobarDirectorio(base, true) )
		base = stDirApp;

	if( isSave )
		archivo = QFileDialog::getSaveFileName(0, caption, base, filter, selectedFilter);
	else
		archivo = QFileDialog::getOpenFileName(0, caption, base, filter, selectedFilter);

	if( !archivo.isEmpty() && QFile::exists(archivo) )
		return QDir::toNativeSeparators( archivo );
	else
		return "";
}

// Obtiene la dirección de una carpeta atraves de QFileDialog
QString Funciones::ventanaDirectorios(QString caption, QString dir, QString dir_relative)
{
	QString directorio = "";
	QString base       = getDirRelative(dir, dir_relative);

	if( !comprobarDirectorio(base, true) )
		base = stDirApp;

	directorio = QFileDialog::getExistingDirectory(0, caption, base, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if( !directorio.isEmpty() && comprobarDirectorio(directorio, true) )
	{
		if( directorio.endsWith("/") || directorio.endsWith("\\") )
			return QDir::toNativeSeparators( directorio );
		else
			return QDir::toNativeSeparators(directorio +"/");
	} else
		return "";
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
void Funciones::ventanaInfo(QString titulo, QString icono, int w, int h, QString contenido, bool isTextoPlano)
{
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

	connect( btnAceptar, SIGNAL( clicked() ), Dialog, SLOT( accept() ) );
	btnAceptar->setText( tr("Aceptar") );
	btnAceptar->setIcon( QIcon(stTheme +"img16/aplicar.png") );
	btnAceptar->setIconSize(QSize(16, 16));
	btnAceptar->setMinimumSize(QSize(80, 24));

	if( isTextoPlano )
		textInfo->setPlainText(contenido);
	else {
		QString str_html = "{contenido_info}";
		if( QFile::exists(stThemeApp +"tpl_info.html") )
			str_html = leerArchivo(stThemeApp +"tpl_info.html", "UTF-8");
		else
			str_html = leerArchivo(":/tpl_info.html", "UTF-8");

		str_html.replace("<theme>", stTheme);
		str_html.replace("{contenido_info}", contenido.replace("\n","<br/>"));
		textInfo->setHtml( str_html );
	}

	if( QFile::exists(icono) )
		Dialog->setWindowIcon(QIcon(icono));
	else
		Dialog->setWindowIcon(QIcon(":/img16/informacion.png"));

	Dialog->setWindowTitle( titulo );
	Dialog->setStyleSheet( myStyleSheet() );
	Dialog->exec();

	delete Dialog;
}

// Elimina un item de un TreeWidget
void Funciones::deleteItemTree(QTreeWidgetItem *item)
{
	if( !item )
		return;
	int count = item->childCount();
	for (int i = count - 1; i >= 0; --i)
		deleteItemTree(item->child(i));

	delete item;
}

// Mueve hacia arriba una posición de un QTreeWidget
void Funciones::moveUpItemTw(QTreeWidget *twList)
{
	int current = twList->indexOfTopLevelItem( twList->currentItem() );
	int next_pos = -1;
	if( current > 0 && current != -1 )
	{
		next_pos = current - 1;

		QTreeWidgetItem *item = twList->topLevelItem( current );
		item = twList->takeTopLevelItem( current );
		twList->insertTopLevelItem(next_pos, item);
		twList->setCurrentItem( item );
	}
}

// Mueve hacia abajo una posición de un QTreeWidget
void Funciones::moveDownItemTw(QTreeWidget *twList)
{
	int current = twList->indexOfTopLevelItem( twList->currentItem() );
	int next_pos = -1;
	if( current < (twList->topLevelItemCount() - 1) && current != -1 )
	{
		next_pos = current + 1;

		QTreeWidgetItem *item = twList->topLevelItem( current );
		item = twList->takeTopLevelItem( current );
		twList->insertTopLevelItem(next_pos, item);
		twList->setCurrentItem( item );
	}
}

// Carga la lista de archivos
QList<stGrlDatos> Funciones::cargarListaArchivos(QString m_dir, TipoListCbx tipo, QString filter, QString filter_sep)
{
	QList<stGrlDatos> dato_list;
	stGrlDatos dato;
	QString language, js_titulo, js_version;
	QLocale locale;
	QStringList parts_lng;
	QScriptEngine engine;
	QFileInfoList list_files = getListFiles(m_dir, filter.split(filter_sep, QString::SkipEmptyParts));

	dato_list.clear();
	switch ( tipo )
	{
		case CbxListIcon:
		case CbxListLng:
		case CbxListJs:
//
		break;
		case CbxListProf:
			dato.titulo  = tr("Configuración por defecto");
			dato.icono   = stTheme +"img16/archivo_config.png";
			dato.extra   = "";
			dato.key     = "_defecto_";

			dato_list << dato;
		break;
		case CbxListJsi:
			dato.titulo  = tr("Importar desde un archivo") +": GR-lida XML, D-Fend Reloaded prof";
			dato.icono   = stTheme +"img16/archivo_config.png";
			dato.extra   = "";
			dato.key     = "_desde_archivo_";

			dato_list << dato;
		break;
	};

	const int listSize = list_files.size();
	for (int i = 0; i < listSize; ++i)
	{
		switch ( tipo )
		{
			case CbxListIcon:
				dato.icono  = m_dir + list_files.at(i).fileName();
				dato.titulo = list_files.at(i).completeBaseName();
				dato.extra  = "";
				dato.key    = list_files.at(i).fileName();
			break;
			case CbxListLng:
				parts_lng.clear();
				parts_lng = list_files.at(i).baseName().split("_");
				language  = parts_lng.at(1) +"_"+ parts_lng.at(2);
				locale    = language;

				if( QFile::exists(stTheme +"img_lng/"+ parts_lng.at(2).toLower() +".png") )
					dato.icono = stTheme +"img_lng/"+ parts_lng.at(2).toLower() +".png";
				else
					dato.icono = stTheme +"img_lng/sinimg.png";

				dato.titulo = locale.languageToString(locale.language()) +" ("+ locale.countryToString(locale.country()) +")"+ (m_dir == ":/idiomas/" ? "" : " "+ tr("exterior"));
				dato.extra  = "";
				dato.key    = list_files.at(i).absoluteFilePath();
			break;
			case CbxListProf:
				dato.icono  = stTheme +"img16/archivo_config.png";
				dato.titulo = list_files.at(i).completeBaseName();
				dato.extra  = "";
				dato.key    = list_files.at(i).fileName();
			break;
			case CbxListJs:
			case CbxListJsi:
				engine.evaluate( leerArchivo(list_files.at(i).absoluteFilePath(), "UTF-8") );

				if( engine.evaluate("title").isValid() )
					js_titulo = engine.evaluate("title").toString();
				else
					js_titulo = list_files.at(i).baseName();

				if( engine.evaluate("version").isValid() )
					js_version = engine.evaluate("version").toString();
				else
					js_version = "0";

				dato.icono  = stTheme +"img16/script.png";
				dato.titulo = js_titulo +" v"+ js_version;
				dato.extra  = js_version;
				dato.key    = list_files.at(i).fileName();
			break;
		};

		dato_list << dato;
	}

	return dato_list;
}

void Funciones::cargarListaArchivosComboBox(QComboBox *cbx, QString m_dir, QString select_data, TipoListCbx tipo, QString filter, QString filter_sep, bool isClear)
{
	QList<stGrlDatos> list = cargarListaArchivos(m_dir, tipo, filter, filter_sep);

	if( isClear )
		cbx->clear();

	const int listSize = list.size();
	for (int i = 0; i < listSize; ++i)
		cbx->addItem(QIcon(list.at(i).icono), list.at(i).titulo, list.at(i).key);

	if( !select_data.isEmpty() )
	{
		int row = cbx->findData(select_data, Qt::UserRole, Qt::MatchExactly);
		cbx->setCurrentIndex( row );
	}
}

void Funciones::cargarListaArchivosCheckComboBox(QCheckComboBox *cbx, QString m_dir, QString select_data, TipoListCbx tipo, QString filter, QString filter_sep)
{
	QList<stGrlDatos> list = cargarListaArchivos(m_dir, tipo, filter, filter_sep);
	cbx->clear();
	const int listSize = list.size();
	for (int i = 0; i < listSize; ++i)
		cbx->addItem(QIcon(list.at(i).icono), list.at(i).titulo, list.at(i).key);

	int row = cbx->findData(select_data, Qt::UserRole, Qt::MatchExactly);
	cbx->setCurrentIndex( row );
}

// Carga la lista de Idiomas en un QComboBox
void Funciones::cargarIdiomasComboBox(QComboBox *cbx, QString dirLng, bool isClear, QString select_data, QString filter)
{
	cargarListaArchivosComboBox(cbx, dirLng, select_data, CbxListLng, filter, ";", isClear);
}

// Carga la lista de Scripts en un QComboBox
void Funciones::cargarScriptsComboBox(QComboBox *cbx, QString dirScripts, QString select_data, QString filter)
{
	cargarListaArchivosComboBox(cbx, dirScripts, select_data, CbxListJsi, filter);
}

// Carga Iconos e imagenes soportadas en un QComboBox de un directorio
void Funciones::cargarIconosComboBox(QComboBox *cbx, QString iconDir, QString select_data, QString filter)
{
	cargarListaArchivosComboBox(cbx, iconDir, select_data, CbxListIcon, filter);
}

// Carga las distintos archivos la ruta indicada en una Lista.
void Funciones::cargarArchivosTwLista(QTreeWidget *twList, QString m_dir, QString filter, QString filter_sep, QString img)
{
	QString archivo;
	QFileInfoList list_files = getListFiles(m_dir, filter.split(filter_sep, QString::SkipEmptyParts));

	const int listSize = list_files.size();
	twList->clear();
	if( listSize > -1 )
	{
		for (int i = 0; i < listSize; ++i)
		{
			if( m_dir.endsWith("/") || m_dir.endsWith("\\") )
				archivo = m_dir + list_files.at(i).fileName();
			else
				archivo = m_dir +"/"+ list_files.at(i).fileName();

			QTreeWidgetItem *item = new QTreeWidgetItem( twList );
			if( img.isEmpty() )
			{
				if( QFile::exists(archivo) )
					item->setIcon( 0, QIcon( archivo ) );
				else
					item->setIcon( 0, QIcon(stThemeApp +"images/juego_sin_imagen.png") );
				item->setText( 0, "" );
			} else {
				if( QFile::exists(stTheme +"img16/"+ img) )
					item->setIcon( 0, QIcon(stTheme +"img16/"+ img) );
				else
					item->setIcon( 0, QIcon(stTheme +"img16/tag.png") );
				item->setText( 0, list_files.at(i).fileName() );
			}
			item->setText( 1, archivo );
		}
	}
}

// Carga la lista de datos de un archivo a un QTreeWidget
void Funciones::cargarDatosTwLista(QTreeWidget *twList, QString archivo, TipoListTw tipo, QString sep, bool isLng)
{
	QFile file_in( archivo );
	QStringList lista;
	QString linea, titulo, dato, extra, img, dir_img;
	int col_titulo, col_dato, col_nada;
	QFont m_font;
	m_font.setBold(true);
	col_titulo = 0;
	col_dato   = 1;
	col_nada   = 2;

	if( !file_in.exists() )
		file_in.setFileName(archivo.replace(stDirApp, ":/"));

//	twList->clear();
	if( file_in.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
		QTextStream in( &file_in );
		in.setCodec("UTF-8");
		while ( !in.atEnd() )
		{
			linea = in.readLine();
			if( linea.isEmpty() )
			{
				QTreeWidgetItem *item = new QTreeWidgetItem( twList );
				switch ( tipo )
				{
					case TwListSvm:
						item->setIcon( 0, QIcon(stTheme +"img_svm/sinimg.png") );
						item->setIcon( 1, QIcon(stTheme +"icon_svm/sinimg.png") );
					break;
					case TwListEmu:
						item->setIcon( 0, QIcon(stTheme +"img16/sinimg.png") );
					break;
					case TwListDbx:
						item->setIcon( 0, QIcon(stTheme +"img16/sinimg.png") );
					break;
					case TwListTraduccion:
						item->setIcon( 1, QIcon(stTheme +"img16/sinimg.png") );
					break;
					case TwListSmile:
						item->setIcon( 0, QIcon(stTheme +"img16/sinimg.png") );
					break;
					case TwList2col:
						item->setIcon( 0, QIcon(stTheme +"img16/sinimg.png") );
					break;
					case TwList3col:
						item->setIcon( 0, QIcon(stTheme +"img16/sinimg.png") );
					break;
				}
				item->setText( 0, "" );
				item->setText( 1, "" );
				item->setText( 2, "" );
				item->setText( 3, "" );
				item->setText( 4, "" );
			} else {
				lista = linea.split( sep );

				QTreeWidgetItem *item = new QTreeWidgetItem( twList );
				switch ( tipo )
				{
					case TwListDbx:
						titulo = lista.at(0);
						dato   = lista.at(1);
						extra  = lista.at(2);
						img    = lista.at(3);
						col_titulo = 0;
						dir_img    = stTheme +"img16/";
					break;
					case TwListSvm:
					case TwListEmu:
						titulo = lista.at(0);
						dato   = lista.at(1);
						extra  = lista.at(2);
						img    = lista.at(3);

						if( tipo == TwListEmu )
						{
							if( isLng )
								dir_img = stTheme +"img_lng/";
							else
								dir_img = stThemeApp +"img16_cat/";
						} else {
							dir_img = stTheme +"icon_svm/";
							col_nada   = 0;
							col_titulo = 1;
							col_dato   = 2;

							if( dato.isEmpty() )
							{
								item->setTextColor( 1, QColor(0,0,0) );
								item->setFont( 1, m_font);
							}

							if( extra.isEmpty() )
								item->setIcon( 0, QIcon(stTheme +"img_svm/sinimg.png") );
							else
								item->setIcon( 0, QIcon(stTheme +"img_svm/"+ extra +".png") );
						}
					break;
					case TwListSmile:
					case TwList2col:
						titulo = lista.at(0);
						dato   = "";
						extra  = "";
						img    = lista.at(1);

						if( isLng )
							dir_img = stTheme +"img_lng/";
						else
							dir_img = stTheme +"img16/";

						if( tipo == TwListSmile )
							dir_img = stTheme +"smiles/";
					break;
					case TwListTraduccion:
					case TwList3col:
						titulo = lista.at(0);
						dato   = lista.at(1);
						extra  = "";
						img    = lista.at(2);

						if( isLng )
							dir_img = stTheme +"img_lng/";
						else
							dir_img = stTheme +"img16/";

						if( tipo == TwListTraduccion )
						{
							titulo  = lista.at(1);
							dato    = lista.at(0);
							dir_img = stTheme +"img_lng/";

							item->setFont( 0, m_font         );
							item->setText( 0, lista.at(0) );
						}
					break;
				}

				if( img.isEmpty() )
					img = "sinimg.png";

				if( QFile::exists(dir_img + img) )
					item->setIcon( col_titulo, QIcon(dir_img + img) );
				else
					item->setIcon( col_titulo, QIcon(dir_img +"sinimg.png") );

				if( tipo == TwListDbx )
				{
					item->setTextAlignment(2, Qt::AlignCenter);
					item->setTextAlignment(3, Qt::AlignCenter);
					item->setText( 0, titulo      );
					item->setText( 1, dato        );
					item->setText( 2, lista.at(4) );
					item->setText( 3, lista.at(5) );
					item->setText( 4, extra       );
				} else {
					item->setText( col_nada  , ""     );
					item->setText( col_titulo, titulo );
					item->setText( col_dato  , dato   );
					item->setText( 3         , extra  );
					item->setText( 4         , img    );
				}
			}
		}
	}
	file_in.close();
}

// Guarda la lista de datos de un QTreeWidget a un archivo
void Funciones::guardarDatosTwLista(QTreeWidget *twList, QString archivo, TipoListTw tipo, QString sep)
{
	QFile file_out;
	file_out.setFileName( archivo );

	comprobarDirectorio( getInfoFile(archivo).Path );

	if( file_out.open(QIODevice::WriteOnly | QIODevice::Text) )
	{
		QTextStream out(&file_out);
		out.setCodec("UTF-8");

		if( twList->topLevelItemCount() > 0 )
		{
			const int listSize = twList->topLevelItemCount();
			for (int i = 0; i < listSize; ++i)
			{
				QTreeWidgetItem *item = twList->topLevelItem( i );

				switch ( tipo )
				{
					case TwListSvm:
						if( item->text(1).isEmpty() && item->text(2).isEmpty() )
							out << endl;
						else
							out << item->text(1) << sep << item->text(2) << sep << item->text(3) << sep << item->text(4) << endl;
					break;
					case TwListEmu:
						out << item->text(0) << sep << item->text(1) << sep << item->text(3) << sep << item->text(4) << endl;
					break;
					case TwListDbx:
						out << item->text(0) << sep << item->text(1) << sep << item->text(4) << sep << "dosbox.png" << sep << item->text(2) << sep << item->text(3) << endl;
					break;
					case TwListTraduccion:
						//
					break;
					case TwListSmile:
					case TwList2col:
						out << item->text(0) << sep << item->text(4) << endl;
					break;
					case TwList3col:
						out << item->text(0) << sep << item->text(1) << sep << item->text(4) << endl;
					break;
				}
			}
		}

		out.flush();
	}
	file_out.close();
}

// Carga la lista de datos.
QList<stGrlDatos> Funciones::cargaListaDatos(QString archivo, int num_col, QString sep)
{
	QList<stGrlDatos> dato_list;
	stGrlDatos dato;
	QFile file_in( archivo );
	QStringList lista;
	QString linea;
	int key_col = 1;
	int img_col = 2;

	if( !file_in.exists() )
		file_in.setFileName(archivo.replace(stDirApp, ":/"));

	switch ( num_col )
	{
		case 2:
			key_col = 0;
			img_col = 1;
		break;
		case 3:
			key_col = 1;
			img_col = 2;
		break;
		case 4:
		case 6:
			key_col = 1;
			img_col = 3;
		break;
	}

	dato_list.clear();
	if( file_in.open(QIODevice::ReadOnly) != 0 )
	{
		QTextStream in(&file_in);
		in.setCodec("UTF-8");
		while ( !in.atEnd() )
		{
			linea = in.readLine();
			if( !linea.isEmpty() )
			{
				lista = linea.split( sep );

				dato.titulo  = lista.at(0);
				dato.icono   = lista.at(img_col);
				dato.extra   = (num_col == 4 || num_col == 6) ? lista.at(2) : "";
				dato.version = (num_col == 6) ? lista.at(4) : "";
				dato.issvn   = (num_col == 6) ? lista.at(5) : "";
				dato.key     = lista.at(key_col);

				dato_list << dato;
			}
		}
	}

	return dato_list;
}

// Carga la lista de datos en un QHash.
QHash<QString, stGrlDatos> Funciones::cargaDatosQHash(QString archivo, int num_col, QString sep)
{
	QList<stGrlDatos> list = cargaListaDatos(archivo, num_col, sep);
	QHash<QString, stGrlDatos> dato_list;
	const int listSize = list.size();
	for (int i = 0; i < listSize; ++i)
		dato_list.insert(list.at(i).key, list.at(i));

	return dato_list;
}

// Carga la lista de datos de un archivo a un QComboBox
void Funciones::cargarDatosComboBox(QComboBox *cbx, QString archivo, QString theme, QString select_data, int num_col, QString sep, bool isClear, bool keyVisible)
{
	QList<stGrlDatos> list = cargaListaDatos(archivo, num_col, sep);
	QString key_visible;

	if( isClear )
		cbx->clear();

	const int listSize = list.size();
	for (int i = 0; i < listSize; ++i)
	{
		key_visible = keyVisible ? "("+ list.at(i).key +") - " : "";
		cbx->addItem(QIcon(theme + list.at(i).icono), key_visible + list.at(i).titulo, list.at(i).key);
	}

	if( !select_data.isEmpty() )
	{
		int row = cbx->findData(select_data, Qt::UserRole, Qt::MatchExactly);
		cbx->setCurrentIndex( row );
	}
}

// Carga la lista de datos de un archivo a un QCheckComboBox
void Funciones::cargarDatosCheckComboBox(QCheckComboBox *chk_cbx, QString archivo, QString theme, QString select_data, int num_col, QString sep)
{
	QList<stGrlDatos> list = cargaListaDatos(archivo, num_col, sep);

	chk_cbx->clear();
	const int listSize = list.size();
	for (int i = 0; i < listSize; ++i)
		chk_cbx->addItem(QIcon(theme + list.at(i).icono), list.at(i).titulo, list.at(i).key);

	int row = chk_cbx->findData(select_data, Qt::UserRole, Qt::MatchExactly);
	chk_cbx->setCurrentIndex( row );
}

// Remplaza texto por las imagenes de los emoticones
QString Funciones::reemplazaTextoSmiles(QString str, QHash<QString, stGrlDatos> lista)
{
	foreach (const stGrlDatos &smile, lista)
		str.replace(smile.key, "<img src=\":smile_rs_"+ smile.icono +"\" alt=\""+ smile.key +"\" title=\""+ smile.key +"\"> ");
	return str;
}

// Devuelve imagenes para la info del juego
QString Funciones::getImgDatos(QHash<QString, stGrlDatos> datos, QStringList lista, bool isLng)
{
	QString img_lng = "";
	const int listSize = lista.size();
	for (int i=0; i < listSize; ++i)
	{
		if( !lista.at(i).isEmpty() )
		{
			foreach (const stGrlDatos &dat, datos)
			{
				if( dat.titulo == lista.at(i) || dat.extra == lista.at(i) )
				{
					if( isLng )
						img_lng.append("<img src=\":idioma_rs_"+ dat.icono +"\" alt=\""+ dat.titulo +"\" title=\""+ dat.titulo +"\"> "+ dat.titulo +" ");
					else
						img_lng.append("<img src=\""+ stTheme +"img16/"+ dat.icono +"\" alt=\""+ dat.titulo +"\" title=\""+ dat.titulo +"\"> "+ dat.titulo +" ");
				}
			}
		}
	}

	return img_lng;
}

void Funciones::insertaIconos(QHash<QString, QPixmap> &ico, QString m_dir, QString contiene, QString filter, bool isExt)
{
	QFileInfoList list_files = getListFiles(m_dir, filter.split(";", QString::SkipEmptyParts));
	QPixmap img_ico;
	const int listSize = list_files.size();
	for (int i=0; i < listSize; ++i)
	{
		if( contiene.isEmpty() )
		{
			if( !img_ico.load(m_dir + list_files.at(i).fileName()) )
				img_ico.load(":/img16/sinimg.png");

			if( isExt )
				ico.insert(list_files.at(i).fileName(), img_ico);
			else
				ico.insert(list_files.at(i).baseName(), img_ico);
		} else {
			if( list_files.at(i).fileName().contains(contiene, Qt::CaseInsensitive) )
			{
				if( !img_ico.load(m_dir + list_files.at(i).fileName()) )
					img_ico.load(":/img16/sinimg.png");

				if( isExt )
					ico.insert(list_files.at(i).fileName(), img_ico);
				else
					ico.insert(list_files.at(i).baseName(), img_ico);
			}
		}
	}
}

// Datos por defecto.
stDatosJuego Funciones::getDefectDatos(QString icono, QHash<QString, QString> dato)
{
	bool isQHash = dato.isEmpty() ? false : true;
	icono = icono.isEmpty() ? "datos" : icono;
	stDatosJuego datos;
	datos.idgrl = isQHash ? dato["Dat_idgrl"] : "";
//--
	datos.icono            = isQHash ? dato["Dat_icono"]            : icono +".png";
	datos.titulo           = isQHash ? dato["Dat_titulo"]           : "";
	datos.subtitulo        = isQHash ? dato["Dat_subtitulo"]        : "";
	datos.genero           = isQHash ? dato["Dat_genero"]           : "";
	datos.compania         = isQHash ? dato["Dat_compania"]         : "";
	datos.desarrollador    = isQHash ? dato["Dat_desarrollador"]    : "";
	datos.tema             = isQHash ? dato["Dat_tema"]             : "";
	datos.grupo            = isQHash ? dato["Dat_grupo"]            : "";
	datos.perspectiva      = isQHash ? dato["Dat_perspectiva"]      : "";
	datos.idioma           = isQHash ? dato["Dat_idioma"]           : "";
	datos.idioma_voces     = isQHash ? dato["Dat_idioma_voces"]     : "";
	datos.formato          = isQHash ? dato["Dat_formato"]          : "";
	datos.anno             = isQHash ? dato["Dat_anno"]             : "";
	datos.numdisc          = isQHash ? dato["Dat_numdisc"]          : "";
	datos.sistemaop        = isQHash ? dato["Dat_sistemaop"]        : "";
	datos.tamano           = isQHash ? dato["Dat_tamano"]           : "";
	datos.graficos         = isQHash ? dato["Dat_graficos"]         : "0";
	datos.sonido           = isQHash ? dato["Dat_sonido"]           : "0";
	datos.jugabilidad      = isQHash ? dato["Dat_jugabilidad"]      : "0";
	datos.original         = isQHash ? dato["Dat_original"]         : "false";
	datos.estado           = isQHash ? dato["Dat_estado"]           : "";
	datos.thumbs           = isQHash ? dato["Dat_thumbs"]           : "";
	datos.thumbs_new       = isQHash ? StrToBool(dato["Dat_thumbs_new"])      : false;
	datos.cover_front      = isQHash ? dato["Dat_cover_front"]      : "";
	datos.cover_front_new  = isQHash ? StrToBool(dato["Dat_cover_front_new"]) : false;
	datos.cover_back       = isQHash ? dato["Dat_cover_back"]       : "";
	datos.cover_back_new   = isQHash ? StrToBool(dato["Dat_cover_back_new"])  : false;
	datos.fecha            = isQHash ? dato["Dat_fecha"]            : "";
	datos.tipo_emu         = isQHash ? dato["Dat_tipo_emu"]         : icono;
	datos.comentario       = isQHash ? dato["Dat_comentario"]       : "";
	datos.favorito         = isQHash ? dato["Dat_favorito"]         : "false";
	datos.rating           = isQHash ? dato["Dat_rating"]           : "0";
	datos.edad_recomendada = isQHash ? dato["Dat_edad_recomendada"] : "nd";
	datos.usuario          = isQHash ? dato["Dat_usuario"]          : "";
	datos.path_exe         = isQHash ? dato["Dat_path_exe"]         : "";
	datos.parametros_exe   = isQHash ? dato["Dat_parametros_exe"]   : "";
	datos.path_capturas    = isQHash ? dato["Dat_path_capturas"]    : "";
	datos.path_setup       = isQHash ? dato["Dat_path_setup"]       : "";
	datos.parametros_setup = isQHash ? dato["Dat_parametros_setup"] : "";

	return datos;
}

// DOSBox por defecto.
stConfigDOSBox Funciones::getDefectDOSBox(QHash<QString, QString> dato)
{
	bool isQHash = dato.isEmpty() ? false : true;
	stConfigDOSBox datos;
	datos.id    = isQHash ? dato["Dbx_id"]    : "";
	datos.idgrl = isQHash ? dato["Dbx_idgrl"] : "";
	datos.idcat = isQHash ? dato["Dbx_idcat"] : "";
// [sdl]
	datos.sdl_fullscreen        = isQHash ? dato["Dbx_sdl_fullscreen"]        : "false";
	datos.sdl_fulldouble        = isQHash ? dato["Dbx_sdl_fulldouble"]        : "false";
	datos.sdl_fullfixed         = isQHash ? dato["Dbx_sdl_fullfixed"]         : "false";
	datos.sdl_fullresolution    = isQHash ? dato["Dbx_sdl_fullresolution"]    : "original";
	datos.sdl_windowresolution  = isQHash ? dato["Dbx_sdl_windowresolution"]  : "original";
	datos.sdl_output            = isQHash ? dato["Dbx_sdl_output"]            : "surface";
	datos.sdl_hwscale           = isQHash ? dato["Dbx_sdl_hwscale"]           : "1.00";
	datos.sdl_autolock          = isQHash ? dato["Dbx_sdl_autolock"]          : "true";
	datos.sdl_sensitivity       = isQHash ? dato["Dbx_sdl_sensitivity"]       : "100";
	datos.sdl_waitonerror       = isQHash ? dato["Dbx_sdl_waitonerror"]       : "true";
	datos.sdl_priority          = isQHash ? dato["Dbx_sdl_priority"]          : "higher,normal";
	datos.sdl_mapperfile        = isQHash ? dato["Dbx_sdl_mapperfile"]        : "mapper.txt";
	datos.sdl_usescancodes      = isQHash ? dato["Dbx_sdl_usescancodes"]      : "true";
// [dosbox]
	datos.dosbox_language       = isQHash ? dato["Dbx_dosbox_language"]       : "";
	datos.dosbox_machine        = isQHash ? dato["Dbx_dosbox_machine"]        : "svga_s3";
	datos.dosbox_captures       = isQHash ? dato["Dbx_dosbox_captures"]       : "capture";
	datos.dosbox_memsize        = isQHash ? dato["Dbx_dosbox_memsize"]        : "16";
	datos.dosbox_emu_key        = isQHash ? dato["Dbx_dosbox_emu_key"]        : "dosbox";
// [render]
	datos.render_frameskip      = isQHash ? dato["Dbx_render_frameskip"]      : "0";
	datos.render_aspect         = isQHash ? dato["Dbx_render_aspect"]         : "false";
	datos.render_scaler         = isQHash ? dato["Dbx_render_scaler"]         : "normal2x";
// [cpu]
	datos.cpu_core              = isQHash ? dato["Dbx_cpu_core"]              : "auto";
	datos.cpu_cputype           = isQHash ? dato["Dbx_cpu_cputype"]           : "auto";
	datos.cpu_cycles            = isQHash ? dato["Dbx_cpu_cycles"]            : "auto";
	datos.cpu_cycles_realmode   = isQHash ? dato["Dbx_cpu_cycles_realmode"]   : "";
	datos.cpu_cycles_protmode   = isQHash ? dato["Dbx_cpu_cycles_protmode"]   : "";
	datos.cpu_cycles_limitmode  = isQHash ? dato["Dbx_cpu_cycles_limitmode"]  : "";
	datos.cpu_cycleup           = isQHash ? dato["Dbx_cpu_cycleup"]           : "10";
	datos.cpu_cycledown         = isQHash ? dato["Dbx_cpu_cycledown"]         : "20";
// [mixer]
	datos.mixer_nosound         = isQHash ? dato["Dbx_mixer_nosound"]         : "false";
	datos.mixer_rate            = isQHash ? dato["Dbx_mixer_rate"]            : "44100";
	datos.mixer_blocksize       = isQHash ? dato["Dbx_mixer_blocksize"]       : "1024";
	datos.mixer_prebuffer       = isQHash ? dato["Dbx_mixer_prebuffer"]       : "20";
// [midi]
	datos.midi_mpu401           = isQHash ? dato["Dbx_midi_mpu401"]           : "intelligent";
	datos.midi_intelligent      = isQHash ? dato["Dbx_midi_intelligent"]      : "true";
	datos.midi_device           = isQHash ? dato["Dbx_midi_device"]           : "default";
	datos.midi_config           = isQHash ? dato["Dbx_midi_config"]           : "";
	datos.midi_mt32rate         = isQHash ? dato["Dbx_midi_mt32rate"]         : "44100";
// [sblaster]
	datos.sblaster_sbtype       = isQHash ? dato["Dbx_sblaster_sbtype"]       : "sb16";
	datos.sblaster_sbbase       = isQHash ? dato["Dbx_sblaster_sbbase"]       : "220";
	datos.sblaster_irq          = isQHash ? dato["Dbx_sblaster_irq"]          : "7";
	datos.sblaster_dma          = isQHash ? dato["Dbx_sblaster_dma"]          : "1";
	datos.sblaster_hdma         = isQHash ? dato["Dbx_sblaster_hdma"]         : "5";
	datos.sblaster_mixer        = isQHash ? dato["Dbx_sblaster_mixer"]        : "true";
	datos.sblaster_oplmode      = isQHash ? dato["Dbx_sblaster_oplmode"]      : "auto";
	datos.sblaster_oplemu       = isQHash ? dato["Dbx_sblaster_oplemu"]       : "default";
	datos.sblaster_oplrate      = isQHash ? dato["Dbx_sblaster_oplrate"]      : "44100";
// [gus]
	datos.gus_gus               = isQHash ? dato["Dbx_gus_gus"]               : "false";
	datos.gus_gusrate           = isQHash ? dato["Dbx_gus_gusrate"]           : "44100";
	datos.gus_gusbase           = isQHash ? dato["Dbx_gus_gusbase"]           : "240";
	datos.gus_irq1              = isQHash ? dato["Dbx_gus_irq1"]              : "5";
	datos.gus_irq2              = isQHash ? dato["Dbx_gus_irq2"]              : "5";
	datos.gus_dma1              = isQHash ? dato["Dbx_gus_dma1"]              : "3";
	datos.gus_dma2              = isQHash ? dato["Dbx_gus_dma2"]              : "3";
	datos.gus_ultradir          = isQHash ? dato["Dbx_gus_ultradir"]          : "C:\\ULTRASND";
// [speaker]
	datos.speaker_pcspeaker     = isQHash ? dato["Dbx_speaker_pcspeaker"]     : "true";
	datos.speaker_pcrate        = isQHash ? dato["Dbx_speaker_pcrate"]        : "44100";
	datos.speaker_tandy         = isQHash ? dato["Dbx_speaker_tandy"]         : "auto";
	datos.speaker_tandyrate     = isQHash ? dato["Dbx_speaker_tandyrate"]     : "44100";
	datos.speaker_disney        = isQHash ? dato["Dbx_speaker_disney"]        : "true";
// [joystick]
	datos.joystick_type         = isQHash ? dato["Dbx_joystick_type"]         : "auto";
	datos.joystick_timed        = isQHash ? dato["Dbx_joystick_timed"]        : "true";
	datos.joystick_autofire     = isQHash ? dato["Dbx_joystick_autofire"]     : "false";
	datos.joystick_swap34       = isQHash ? dato["Dbx_joystick_swap34"]       : "false";
	datos.joystick_buttonwrap   = isQHash ? dato["Dbx_joystick_buttonwrap"]   : "false";
// [modem]
	datos.modem_modem           = isQHash ? dato["Dbx_modem_modem"]           : "false";
	datos.modem_comport         = isQHash ? dato["Dbx_modem_comport"]         : "2";
	datos.modem_listenport      = isQHash ? dato["Dbx_modem_listenport"]      : "23";
// [dserial]
	datos.dserial_directserial  = isQHash ? dato["Dbx_dserial_directserial"]  : "false";
	datos.dserial_comport       = isQHash ? dato["Dbx_dserial_comport"]       : "1";
	datos.dserial_realport      = isQHash ? dato["Dbx_dserial_realport"]      : "COM1";
	datos.dserial_defaultbps    = isQHash ? dato["Dbx_dserial_defaultbps"]    : "1200";
	datos.dserial_parity        = isQHash ? dato["Dbx_dserial_parity"]        : "N";
	datos.dserial_bytesize      = isQHash ? dato["Dbx_dserial_bytesize"]      : "8";
	datos.dserial_stopbit       = isQHash ? dato["Dbx_dserial_stopbit"]       : "1";
// [serial]
	datos.serial_1              = isQHash ? dato["Dbx_serial_1"]              : "dummy";
	datos.serial_2              = isQHash ? dato["Dbx_serial_2"]              : "dummy";
	datos.serial_3              = isQHash ? dato["Dbx_serial_3"]              : "disabled";
	datos.serial_4              = isQHash ? dato["Dbx_serial_4"]              : "disabled";
// [dos]
	datos.dos_xms               = isQHash ? dato["Dbx_dos_xms"]               : "true";
	datos.dos_ems               = isQHash ? dato["Dbx_dos_ems"]               : "true";
	datos.dos_umb               = isQHash ? dato["Dbx_dos_umb"]               : "true";
	datos.dos_keyboardlayout    = isQHash ? dato["Dbx_dos_keyboardlayout"]    : "auto";
	datos.dos_version           = isQHash ? dato["Dbx_dos_version"]           : "";
// [ipx]
	datos.ipx_ipx               = isQHash ? dato["Dbx_ipx_ipx"]               : "false";
	datos.ipx_type              = isQHash ? dato["Dbx_ipx_type"]              : "none";
	datos.ipx_port              = isQHash ? dato["Dbx_ipx_port"]              : "213";
	datos.ipx_ip                = isQHash ? dato["Dbx_ipx_ip"]                : "";
// [autoexec]
	datos.autoexec              = isQHash ? dato["Dbx_autoexec"]              : "";
// Opciones
	datos.opt_autoexec          = isQHash ? dato["Dbx_opt_autoexec"]          : "false";
	datos.opt_loadfix           = isQHash ? dato["Dbx_opt_loadfix"]           : "false";
	datos.opt_loadfix_mem       = isQHash ? dato["Dbx_opt_loadfix_mem"]       : "64";
	datos.opt_consola_dbox      = isQHash ? dato["Dbx_opt_consola_dbox"]      : "false";
	datos.opt_cerrar_dbox       = isQHash ? dato["Dbx_opt_cerrar_dbox"]       : "true";
	datos.opt_cycle_sincronizar = isQHash ? dato["Dbx_opt_cycle_sincronizar"] : "false";
// Path
	datos.path_conf             = isQHash ? dato["Dbx_path_conf"]             : "";
	datos.path_sonido           = isQHash ? dato["Dbx_path_sonido"]           : "waves";
	datos.path_exe              = isQHash ? dato["Dbx_path_exe"]              : "";
	datos.path_setup            = isQHash ? dato["Dbx_path_setup"]            : "";
	datos.parametros_exe        = isQHash ? dato["Dbx_parametros_exe"]        : "";
	datos.parametros_setup      = isQHash ? dato["Dbx_parametros_setup"]      : "";

	return datos;
}

// ScummVM por defecto.
stConfigScummVM Funciones::getDefectScummVM(QHash<QString, QString> dato)
{
	bool isQHash = dato.isEmpty() ? false : true;
	stConfigScummVM datos;
	datos.id    = isQHash ? dato["Svm_id"]    : "";
	datos.idgrl = isQHash ? dato["Svm_idgrl"] : "";
	datos.idcat = isQHash ? dato["Svm_idcat"] : "";
//--
	datos.game              = isQHash ? dato["Svm_game"]              : "";
	datos.game_label        = isQHash ? dato["Svm_game_label"]        : "";
	datos.language          = isQHash ? dato["Svm_language"]          : "es";
	datos.subtitles         = isQHash ? dato["Svm_subtitles"]         : "true";
	datos.platform          = isQHash ? dato["Svm_platform"]          : "pc";
	datos.gfx_mode          = isQHash ? dato["Svm_gfx_mode"]          : "2x";
	datos.render_mode       = isQHash ? dato["Svm_render_mode"]       : "";
	datos.fullscreen        = isQHash ? dato["Svm_fullscreen"]        : "false";
	datos.aspect_ratio      = isQHash ? dato["Svm_aspect_ratio"]      : "false";
	datos.path_game         = isQHash ? dato["Svm_path_game"]         : "";
	datos.path_extra        = isQHash ? dato["Svm_path_extra"]        : "";
	datos.path_save         = isQHash ? dato["Svm_path_save"]         : "";
	datos.path_capturas     = isQHash ? dato["Svm_path_capturas"]     : "";
	datos.path_sonido       = isQHash ? dato["Svm_path_sonido"]       : "";
	datos.music_driver      = isQHash ? dato["Svm_music_driver"]      : "adlib";
	datos.enable_gs         = isQHash ? dato["Svm_enable_gs"]         : "false";
	datos.multi_midi        = isQHash ? dato["Svm_multi_midi"]        : "true";
	datos.native_mt32       = isQHash ? dato["Svm_native_mt32"]       : "false";
	datos.mute              = isQHash ? dato["Svm_mute"]              : "false";
	datos.master_volume     = isQHash ? dato["Svm_master_volume"]     : "192";
	datos.music_volume      = isQHash ? dato["Svm_music_volume"]      : "192";
	datos.sfx_volume        = isQHash ? dato["Svm_sfx_volume"]        : "192";
	datos.speech_volume     = isQHash ? dato["Svm_speech_volume"]     : "192";
	datos.speech_mute       = isQHash ? dato["Svm_speech_mute"]       : "false";
	datos.tempo             = isQHash ? dato["Svm_tempo"]             : "100";
	datos.talkspeed         = isQHash ? dato["Svm_talkspeed"]         : "60";
	datos.debuglevel        = isQHash ? dato["Svm_debuglevel"]        : "0";
	datos.cdrom             = isQHash ? dato["Svm_cdrom"]             : "0";
	datos.joystick_num      = isQHash ? dato["Svm_joystick_num"]      : "0";
	datos.output_rate       = isQHash ? dato["Svm_output_rate"]       : "";
	datos.midi_gain         = isQHash ? dato["Svm_midi_gain"]         : "100";
	datos.copy_protection   = isQHash ? dato["Svm_copy_protection"]   : "false";
	datos.sound_font        = isQHash ? dato["Svm_sound_font"]        : "";
	datos.walkspeed         = isQHash ? dato["Svm_walkspeed"]         : "0";
	datos.opl_driver        = isQHash ? dato["Svm_opl_driver"]        : "auto";
	datos.disable_dithering = isQHash ? dato["Svm_disable_dithering"] : "false";
	datos.alt_intro         = isQHash ? dato["Svm_alt_intro"]         : "false";
	datos.boot_param        = isQHash ? dato["Svm_boot_param"]        : "";
//--
	datos.description       = isQHash ? dato["Svm_description"] : "";
	datos.emu_svmpath       = isQHash ? dato["Svm_emu_svmpath"] : "";

	return datos;
}

// VDMSound por defecto.
stConfigVDMSound Funciones::getDefectVDMSound(QHash<QString, QString> dato)
{
	bool isQHash = dato.isEmpty() ? false : true;
	stConfigVDMSound datos;
	datos.id    = isQHash ? dato["Vdms_id"]    : "";
	datos.idgrl = isQHash ? dato["Vdms_idgrl"] : "";
	datos.idcat = isQHash ? dato["Vdms_idcat"] : "";
//--
	datos.path_conf       = isQHash ? dato["Vdms_path_conf"]       : "";
	datos.path_exe        = isQHash ? dato["Vdms_path_exe"]        : "";
	datos.program_1       = isQHash ? dato["Vdms_program_1"]       : "";
	datos.program_2       = isQHash ? dato["Vdms_program_2"]       : "";
	datos.vdms_debug_1    = isQHash ? dato["Vdms_vdms_debug_1"]    : "no";
	datos.vdms_debug_2    = isQHash ? dato["Vdms_vdms_debug_2"]    : "";
	datos.winnt_dos_1     = isQHash ? dato["Vdms_winnt_dos_1"]     : "no";
	datos.winnt_dos_2     = isQHash ? dato["Vdms_winnt_dos_2"]     : "";
	datos.winnt_dosbox_1  = isQHash ? dato["Vdms_winnt_dosbox_1"]  : "yes";
	datos.winnt_dosbox_2  = isQHash ? dato["Vdms_winnt_dosbox_2"]  : "no";
	datos.winnt_dosbox_3  = isQHash ? dato["Vdms_winnt_dosbox_3"]  : "no";
	datos.winnt_storage_1 = isQHash ? dato["Vdms_winnt_storage_1"] : "yes";
	datos.winnt_storage_2 = isQHash ? dato["Vdms_winnt_storage_2"] : "no";

	return datos;
}

/*
 * INICIO MONTAJES DE UNIDADES ----------------------------------------------------------------------------------
 * - Funcion para Poner nombres cortos en DOS.
 * - Estado: Beta v 0.0.4
 *
*/
QString Funciones::getNameTo8Caracter(QString name, int index)
{
	if( name.isEmpty() )
		return "";
	else {
		QString final, str, ext;
		if( name.contains(".") )
		{
			int last_dot = name.lastIndexOf(".");
			str = name.left(last_dot);
			ext = name.remove(0, last_dot);
			if( ext.size() > 4 )
				ext = name.left(4);
		} else {
			ext = "";
			str = name;
		}

		int idx = IntToStr(index).length()-1;

		// Contiene espacios
		if( str.contains(" ") )
			final = str.replace(" ","").replace(".","").left(6-idx).append("~"+ IntToStr(index)).append(ext).toUpper();
		else {
		// No contiene espacios
		// Si es mayor que 8 se covierte.
			if( str.length() > 8 )
				final = str.left(6-idx).append("~"+ IntToStr(index)).append(ext).toUpper();
			else
				final = str.append(ext);
		}

		return final;
	}
}

QString Funciones::getShortPathName(QString longPath)
{
/*
 * - Archivos 8 carateres maximo
 * - Nombres sin espacios
 * - Carpetas con extensión
 * - Carpetas similares nombre~1, nombre~2...
*/
	if( !longPath.isEmpty() )
	{
		int index, idx_dos, listSize = 0;
		QString f_name, f_path, sep;
		QStringList listShortPath, shortPath, path_tmp, list_dir, list_dir_8names;
#ifdef Q_OS_WIN32
	sep = "\\";
#else
	sep = "/";
#endif
		QFileInfo fi( longPath.replace("\\", sep) );
		f_name = fi.fileName();
		f_path = fi.absolutePath().append("/").replace("/", sep);

		shortPath.clear();
		listShortPath.clear();
		listShortPath = f_path.split(sep);
		listSize = listShortPath.size();

		path_tmp << listShortPath.at(0);
		shortPath << listShortPath.at(0);

		for (int i = 1; i < listSize-1; ++i)
		{
			QDir dir( str_end(path_tmp.join(sep), sep) );
			list_dir = dir.entryList(QDir::NoDotAndDotDot | QDir::AllDirs);
			list_dir_8names.clear();
			idx_dos = 1;
			for(int n = 0; n < list_dir.size(); ++n)
			{
				if( n > 0)
				{
					if( getNameTo8Caracter(list_dir.at(n), idx_dos) == getNameTo8Caracter(list_dir.at(n-1), idx_dos) )
						++idx_dos;
					else
						idx_dos = 1;
				}
				list_dir_8names << getNameTo8Caracter(list_dir.at(n), idx_dos);
			}
			index = list_dir.indexOf( listShortPath.at(i) );
			if( index > -1 )
				shortPath << list_dir_8names.at(index);

			path_tmp << listShortPath.at(i);
		}

		if( !f_name.isEmpty() )
		{
			QDir dir_exe( f_path );
			list_dir = dir_exe.entryList(QDir::NoDotAndDotDot | QDir::Files);
			list_dir_8names.clear();
			idx_dos = 1;
			for(int n = 0; n < list_dir.size(); ++n)
			{
				if( n > 0)
				{
					if( getNameTo8Caracter(list_dir.at(n), idx_dos) == getNameTo8Caracter(list_dir.at(n-1), idx_dos) )
						++idx_dos;
					else
						idx_dos = 1;
				}
				list_dir_8names << getNameTo8Caracter(list_dir.at(n), idx_dos);
			}
			index = list_dir.indexOf( f_name );
			if( index > -1 )
				shortPath << list_dir_8names.at(index);
		}

		return shortPath.join("\\");
	} else
		return "";
}

// Crea la configuración de los Montajes para el DOSBox
QStringList Funciones::creaConfigMontajes(QTreeWidget *twDbxMount, stConfigDOSBox cfgDbx)
{
	QString NombreEXEDbx, DirEXEDbx, Dbx_loadfix, Dbx_cerrardbx;
	QString mount_letra_primario, mount_dir_primario, mount_dir, montaje_boot, mount_freesize;
	QString mount_type, mount_drive, mount_letter, mount_label, mount_options, mount_ioctl;
	int num_mount = 0;
	bool montaje_IMG = false;
	bool mount_Boot = false;
	QStringList listmontajes, lista_multiple_iso, lista_isos;

	QFileInfo fi( getDirRelative(cfgDbx.path_exe, "DosGames") );
	NombreEXEDbx = fi.fileName();
	DirEXEDbx    = fi.absolutePath().replace("/","\\");

	if( !DirEXEDbx.endsWith("\\") )
		DirEXEDbx.append("\\");

// loadfix
	if( StrToBool( cfgDbx.opt_loadfix ) )
		Dbx_loadfix = "loadfix -"+ cfgDbx.opt_loadfix_mem +" ";
	else {
		if( fi.completeSuffix().toLower() == "bat" )
			Dbx_loadfix = "call ";
		else
			Dbx_loadfix = "";
	}

// Cerrar DOSBox
	if( StrToBool( cfgDbx.opt_cerrar_dbox ) )
		Dbx_cerrardbx = "exit";
	else
		Dbx_cerrardbx = "";

// Montajes
	listmontajes.clear();
	if( twDbxMount->topLevelItemCount() > 0 )
	{
		mount_dir_primario   = getDirRelative(twDbxMount->topLevelItem(0)->text(0), "DosGames");
		mount_letra_primario = twDbxMount->topLevelItem(0)->text(1);

		const int count_mount = twDbxMount->topLevelItemCount();
		for (num_mount = 0; num_mount < count_mount; ++num_mount)
		{
			QTreeWidgetItem *item = twDbxMount->topLevelItem( num_mount );

		// Indicamos el directorio y la letra a montar
			mount_drive  = getDirRelative(item->text(0), "DosGames"); // Real Drive or Directory or Image ISO, IMA
			mount_letter = item->text(1); // Emulated Drive letter

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
			if( item->text(7) == "v" )
			{
				mount_dir_primario   = mount_drive;
				mount_letra_primario = mount_letter;
			}

			mount_label    = "";
			mount_freesize = "";
			mount_ioctl    = "";
			mount_options  = "";

		// Montando las unidades
		// Disco Duro, Montando Disquetera, CDs, Imagenes de Discos, disquetes y CDs
			if( item->text(2) != "boot")
			{
				mount_Boot = false;

				if( item->text(2) == "drive" )
				{
					montaje_IMG = false;
					mount_type = "";

					if( StrToBool( item->text(11) ) )
					{
						if( item->text(10) != "" )
							mount_freesize = " -size "+ item->text(10);
						else
							mount_freesize = " -freesize "+ item->text(12);
					}
				}

				if( item->text(2) == "cdrom" )
				{
					montaje_IMG  = false;
					mount_type  = " -t cdrom";
				// usecd
					if( item->text(4) != "" )
						mount_freesize = " -usecd "+ item->text(4);
				// ioctl
					if( item->text(6) != "" )
						mount_ioctl = " "+ item->text(6);
				// etiqueta
					if( item->text(3) != "" )
						mount_label = " -label "+ item->text(3);
				}

				if( item->text(2) == "floppy" )
				{
					montaje_IMG = false;
					mount_type = " -t floppy";

					if( StrToBool( item->text(11) ) )
					{
						if( item->text(10) != "" )
							mount_freesize = " -size "+ item->text(10);
						else
							mount_freesize = " -freesize "+ item->text(12);
					}
				}

				if( item->text(2) == "IMG_floppy" )
				{
					montaje_IMG = true;
					mount_type = " -t floppy";

					if( item->text(10) != "" )
						mount_freesize = " -size "+ item->text(10);

					if( item->text(5) != "" )
						mount_options = " "+ item->text(5);
				}

				if( item->text(2) == "IMG_iso" )
				{
					montaje_IMG = true;
					mount_type = " -t cdrom";

					if( item->text(5) != "" )
						mount_options = " "+ item->text(5);
				}

				if( item->text(2) == "IMG_multi_iso" )
				{
					montaje_IMG = true;
					mount_type = " -t cdrom";

					lista_isos.clear();
					lista_isos << mount_drive.split("|", QString::SkipEmptyParts);

					lista_multiple_iso.clear();
					const int lmiso_Size = lista_isos.size();
					for (int i = 0; i < lmiso_Size; ++i)
						lista_multiple_iso << "\""+ getShortPathName( getDirRelative(lista_isos.at(i), "DosGames") ) +"\"";
					mount_drive.clear();
					mount_drive = lista_multiple_iso.join(" ");
				}

				if( item->text(2) == "IMG_hdd" )
				{
					montaje_IMG = true;
					mount_type = " -t hdd";

					if( item->text(10) != "" )
						mount_freesize = " -size "+ item->text(10);

					if( item->text(5) != "" )
						mount_options = " "+ item->text(5);
				}

				if( montaje_IMG == true )
				{
					if( item->text(2) == "IMG_multi_iso" )
						listmontajes << "imgmount "+ mount_letter +" "+ mount_drive + mount_type;
					else
						listmontajes << "imgmount "+ mount_letter +" \""+ mount_drive +"\""+ mount_type + mount_freesize + mount_options;
				} else
					listmontajes << "mount "+ mount_letter +" \""+ mount_drive +"\""+ mount_type + mount_freesize + mount_ioctl + mount_label;
			} else {
				mount_Boot = true;

			// Montaje multiple del BOOT
				lista_isos.clear();
				lista_isos << mount_drive.split("|", QString::SkipEmptyParts);

				lista_multiple_iso.clear();
				const int lmisoSize = lista_isos.size();
				for (int i = 0; i < lmisoSize; ++i)
				{
					lista_multiple_iso << "\""+ getShortPathName( getDirRelative(lista_isos.at(i), "DosGames") ) +"\"";
				}
				mount_drive.clear();
				mount_drive = lista_multiple_iso.join(" ");
				montaje_boot = "boot "+ mount_drive +" -l "+ mount_letter;
			}
		}
	} else {
		mount_letra_primario = "";
		mount_dir_primario   = "";
		montaje_boot = "";
	}

	mount_dir_primario.replace("/","\\");
	if( !mount_dir_primario.endsWith("\\") )
		mount_dir_primario.append("\\");

	mount_dir = getShortPathName( DirEXEDbx ).remove( getShortPathName( mount_dir_primario ), Qt::CaseInsensitive);

	if( mount_Boot == false )
	{
		listmontajes << mount_letra_primario +":";
		listmontajes << "cd "+ mount_dir;
		listmontajes << Dbx_loadfix + getNameTo8Caracter( NombreEXEDbx ) +" "+ cfgDbx.parametros_exe;
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
void Funciones::crearArchivoConfigDbx(stDatosJuego datos, QList<QString> url_list, stConfigDOSBox cfgDbx, QTreeWidget *twDbxMount, QString dir_app, QString tabla, QString pathSaveConfg, TipoCfg tipo_cfg)
{
	datos.tipo_emu = "dosbox";
	exportarProfileGRlida(datos, url_list, cfgDbx, twDbxMount, getDefectScummVM(), getDefectVDMSound(), dir_app, tabla, pathSaveConfg, tipo_cfg);
}

void Funciones::exportarProfileGRlida(stDatosJuego datos, QList<QString> url_list, stConfigDOSBox cfgDbx, QTreeWidget *twDbxMount, stConfigScummVM cfgSvm, stConfigVDMSound cfgVdms, QString dir_app, QString tabla, QString pathSaveConfg, TipoCfg tipo_cfg)
{
	QString cfg_out;//, dirBaseGames

	QHash<QString, stGrlDatos> dbx_list = cargaDatosQHash(stDirApp +"datos/dbx_list.txt", 6, "|");
	stGrlDatos dbx_dato;
	dbx_dato.titulo = "DOSBox";
	dbx_dato.icono  = "dosbox.png";
	dbx_dato.key    = "dosbox";
	QSettings settings(stDirApp +"GR-lida.conf", QSettings::IniFormat);
	settings.beginGroup("OpcGeneral");
//		dirBaseGames = settings.value("DirBaseGames", "./DosGames/").toString();
		dbx_dato.extra   = settings.value("DirDOSBox", "").toString();
		dbx_dato.issvn   = settings.value("DOSBoxSVN", false).toBool();
		dbx_dato.version = settings.value("VersionDBx", "0.74").toString();
	settings.endGroup();
	dbx_list.insert(dbx_dato.key, dbx_dato);
// DOSBox a usar
	stGrlDatos dosbox_dato_emu = dbx_list[cfgDbx.dosbox_emu_key];

	if( tipo_cfg == ExportGrlida )
	{
		cfg_out = leerArchivo(":/datos/grl_datos.xml", "UTF-8");
		if( datos.tipo_emu == "dosbox" )
			cfg_out.replace("{grl_otra_configuracion}", "\n"+ leerArchivo(":/datos/grl_dosbox.xml", "UTF-8"));
		else if( datos.tipo_emu == "scummvm" )
			cfg_out.replace("{grl_otra_configuracion}", "\n"+ leerArchivo(":/datos/grl_scummvm.xml", "UTF-8"));
		else if( datos.tipo_emu == "vdmsound" )
			cfg_out.replace("{grl_otra_configuracion}", "\n"+ leerArchivo(":/datos/grl_vdmsound.xml", "UTF-8"));
		else
			cfg_out.replace("{grl_otra_configuracion}", "");
	} else {
		if( stLngSelect.isEmpty() )
			cfg_out = leerArchivo(":/datos/dosbox.conf", "UTF-8");
		else
			cfg_out = leerArchivo(":/datos/"+ stLngSelect +"/dosbox.conf", "UTF-8");
	}

	if( tipo_cfg == ExportDfend )
	{
		QString cfg_dfend = "\n";
		cfg_dfend.append("[ExtraInfo]\n");
		cfg_dfend.append("Icon="+ datos.icono +"\n");
		cfg_dfend.append("Language="+ datos.idioma +"\n");
		cfg_dfend.append("Favorite="+ datos.favorito +"\n");
		cfg_dfend.append("Name="+ datos.titulo +"\n");
		cfg_dfend.append("Genre="+ datos.genero +"\n");
		cfg_dfend.append("Developer="+ datos.desarrollador +"\n");
		cfg_dfend.append("Publisher="+ datos.compania +"\n");
		cfg_dfend.append("Year="+ datos.anno +"\n");
//		cfg_dfend.append("UserInfo="+ datos +"\n");
		cfg_dfend.append("Notes="+ datos.comentario.replace("<br/>\n","[13][10]").replace("<br/>","[13]").replace("\n","[13]") +"\n");

		const int url_listSize = url_list.size();
		for (int i = 0; i < url_listSize; ++i)
		{
			if( i < 1 )
			{
				cfg_dfend.append("WWW="+ url_list.at(i) +"\n");
				cfg_dfend.append("WWWName="+ url_list.at(i) +"\n");
			} else {
				cfg_dfend.append("WWW"+ IntToStr(i+1) +"="+ url_list.at(i) +"\n");
				cfg_dfend.append("WWWName"+ IntToStr(i+1) +"="+ url_list.at(i) +"\n");
			}
		}
		cfg_dfend.append("\n");

		cfg_dfend.append("[Extra]\n");
//		cfg_dfend.append("BootImage="+ datos +"\n");
//		cfg_dfend.append("Environment="+ datos +"\n");
		cfg_dfend.append("Loadhigh="+ cfgDbx.opt_loadfix +"\n");
		cfg_dfend.append("CloseOnExit="+ cfgDbx.opt_cerrar_dbox +"\n");
//		cfg_dfend.append("Overridegamestart="+ datos +"\n");
//		cfg_dfend.append("OverrideMount="+ datos +"\n");
		cfg_dfend.append("LoadFixVal="+ cfgDbx.opt_loadfix_mem +"\n");
		cfg_dfend.append("NrOfMounts="+ IntToStr(twDbxMount->topLevelItemCount()) +"\n");
		cfg_dfend.append("Tab=-1\n");
		cfg_dfend.append("Exe="+ cfgDbx.path_exe +"\n");
		cfg_dfend.append("Setup="+ cfgDbx.path_setup +"\n");
		cfg_dfend.append("GameParameters="+  cfgDbx.parametros_exe +"\n");
		cfg_dfend.append("SetupParameters="+  cfgDbx.parametros_setup +"\n");
//		cfg_dfend.append("ExtraDirs=\n");
//		cfg_dfend.append("DataDir=\n");

		const int count_mount = twDbxMount->topLevelItemCount();
		if( count_mount > 0 )
		{
			for (int num_mount = 0; num_mount < count_mount; ++num_mount)
			{
				QTreeWidgetItem *item = twDbxMount->topLevelItem( num_mount );
				cfg_dfend.append(IntToStr(num_mount) +"="+ item->text(0) +";"+ item->text(2) +";"+ item->text(3) +";"+ item->text(6) +"\n");
			}
		}

		cfg_dfend.append("autoexec="+  cfgDbx.autoexec.replace("\n", "[13][10]") +"\n");
//		cfg_dfend.append("CustomSettings="+ datos +"\n");
//		cfg_dfend.append("AutoMountCDs="+ datos +"\n");
//		cfg_dfend.append("SecureMode="+ datos +"\n");
		cfg_dfend.append("Tab2=-1\n");
		cfg_dfend.append("ExeMD5="+ getHashFile(cfgDbx.path_exe, hashMd5) +"\n");
		cfg_dfend.append("SetupMD5="+ getHashFile(cfgDbx.path_setup, hashMd5) +"\n");
//		cfg_dfend.append("ExtraFiles="+ datos +"\n");
//		cfg_dfend.append("LastModification="+ datos +"\n");
//		cfg_dfend.append("ScreenshotListScreenshot="+ datos +"\n");
//		cfg_dfend.append("ExtraExe0="+ datos +"\n");
//		cfg_dfend.append("ExtraExe1="+ datos +"\n");
//		cfg_dfend.append("ExtraExe2="+ datos +"\n");
//		cfg_dfend.append("ExtraExe3="+ datos +"\n");
//		cfg_dfend.append("ExtraExe4="+ datos +"\n");
//		cfg_dfend.append("ExtraExe5="+ datos +"\n");
//		cfg_dfend.append("ExtraExe6="+ datos +"\n");
//		cfg_dfend.append("ExtraExe7="+ datos +"\n");
//		cfg_dfend.append("ExtraExe8="+ datos +"\n");
//		cfg_dfend.append("ExtraExe9="+ datos +"\n");
//		cfg_dfend.append("Finalization="+ datos +"\n");
		cfg_dfend.append("ProfileMode=DOSBox\n");
		cfg_dfend.append("\n");

//		cfg_dfend.append("[ExtraCommands]\n");
//		cfg_dfend.append("BeforeExecution.Wait="+ datos +"\n");
//		cfg_dfend.append("BeforeExecution.Minimized="+ datos +"\n");
//		cfg_dfend.append("AfterExecution.Minimized="+ datos +"\n");
//		cfg_dfend.append("BeforeExecution="+ datos +"\n");
//		cfg_dfend.append("AfterExecution="+ datos +"\n");
//		cfg_dfend.append("\n");

//		cfg_dfend.append("[ExtraGameIdentify]\n");
//		cfg_dfend.append("File1.Filename="+ datos +"\n");
//		cfg_dfend.append("File1.Checksum="+ datos +"\n");
//		cfg_dfend.append("File2.Filename="+ datos +"\n");
//		cfg_dfend.append("File2.Checksum="+ datos +"\n");
//		cfg_dfend.append("File3.Filename="+ datos +"\n");
//		cfg_dfend.append("File3.Checksum="+ datos +"\n");
//		cfg_dfend.append("File4.Filename="+ datos +"\n");
//		cfg_dfend.append("File4.Checksum="+ datos +"\n");
//		cfg_dfend.append("File5.Filename="+ datos +"\n");
//		cfg_dfend.append("File5.Checksum="+ datos +"\n");
//		cfg_dfend.append("\n");

		cfg_out.replace("{Dbx_config_export_dfend}", cfg_dfend);
	}
	else if( tipo_cfg == ExportGrlida )
	{
		cfg_out.replace("{lastBuildDate}", HoraFechaActual( getTime() ) );

		cfg_out.replace("{Dat_icono}"           , datos.icono           );
		cfg_out.replace("{Dat_titulo}"          , datos.titulo          );
		cfg_out.replace("{Dat_subtitulo}"       , datos.subtitulo       );
		cfg_out.replace("{Dat_genero}"          , datos.genero          );
		cfg_out.replace("{Dat_compania}"        , datos.compania        );
		cfg_out.replace("{Dat_desarrollador}"   , datos.desarrollador   );
		cfg_out.replace("{Dat_tema}"            , datos.tema            );
		cfg_out.replace("{Dat_grupo}"           , datos.grupo           );
		cfg_out.replace("{Dat_perspectiva}"     , datos.perspectiva     );
		cfg_out.replace("{Dat_idioma}"          , datos.idioma          );
		cfg_out.replace("{Dat_idioma_voces}"    , datos.idioma_voces    );
		cfg_out.replace("{Dat_formato}"         , datos.formato         );
		cfg_out.replace("{Dat_anno}"            , datos.anno            );
		cfg_out.replace("{Dat_numdisc}"         , datos.numdisc         );
		cfg_out.replace("{Dat_sistemaop}"       , datos.sistemaop       );
		cfg_out.replace("{Dat_tamano}"          , datos.tamano          );
		cfg_out.replace("{Dat_graficos}"        , datos.graficos        );
		cfg_out.replace("{Dat_sonido}"          , datos.sonido          );
		cfg_out.replace("{Dat_jugabilidad}"     , datos.jugabilidad     );
		cfg_out.replace("{Dat_original}"        , datos.original        );
		cfg_out.replace("{Dat_estado}"          , datos.estado          );
		cfg_out.replace("{Dat_thumbs}"          , datos.thumbs          );
		cfg_out.replace("{Dat_cover_front}"     , datos.cover_front     );
		cfg_out.replace("{Dat_cover_back}"      , datos.cover_back      );
		cfg_out.replace("{Dat_fecha}"           , datos.fecha           );
		cfg_out.replace("{Dat_tipo_emu}"        , datos.tipo_emu        );
		cfg_out.replace("{Dat_comentario}"      , datos.comentario      );
		cfg_out.replace("{Dat_favorito}"        , datos.favorito        );
		cfg_out.replace("{Dat_rating}"          , datos.rating          );
		cfg_out.replace("{Dat_edad_recomendada}", datos.edad_recomendada);
		cfg_out.replace("{Dat_usuario}"         , datos.usuario         );
		cfg_out.replace("{Dat_path_exe}"        , datos.path_exe        );
		cfg_out.replace("{Dat_parametros_exe}"  , datos.parametros_exe  );
		cfg_out.replace("{Dat_path_capturas}"   , datos.path_capturas   );
		cfg_out.replace("{Dat_path_setup}"      , datos.path_setup      );
		cfg_out.replace("{Dat_parametros_setup}", datos.parametros_setup);
	} else
		cfg_out.replace("{Dbx_config_export_dfend}", "");

	if( datos.tipo_emu == "dosbox" )
	{
	// [sdl]
		cfg_out.replace("{Dbx_sdl_fullscreen}"      , cfgDbx.sdl_fullscreen);
		cfg_out.replace("{Dbx_sdl_fulldouble}"      , cfgDbx.sdl_fulldouble);
		cfg_out.replace("{Dbx_sdl_fullfixed}"       , cfgDbx.sdl_fullfixed);
		cfg_out.replace("{Dbx_sdl_fullresolution}"  , cfgDbx.sdl_fullresolution);
		cfg_out.replace("{Dbx_sdl_windowresolution}", cfgDbx.sdl_windowresolution);
		cfg_out.replace("{Dbx_sdl_output}"          , cfgDbx.sdl_output);
		cfg_out.replace("{Dbx_sdl_hwscale}"         , cfgDbx.sdl_hwscale);
		cfg_out.replace("{Dbx_sdl_autolock}"        , cfgDbx.sdl_autolock);
		cfg_out.replace("{Dbx_sdl_sensitivity}"     , cfgDbx.sdl_sensitivity);
		cfg_out.replace("{Dbx_sdl_waitonerror}"     , cfgDbx.sdl_waitonerror);
		cfg_out.replace("{Dbx_sdl_priority}"        , cfgDbx.sdl_priority);
		cfg_out.replace("{Dbx_sdl_mapperfile}"      , cfgDbx.sdl_mapperfile);
		cfg_out.replace("{Dbx_sdl_usescancodes}"    , cfgDbx.sdl_usescancodes);

		if( dosbox_dato_emu.version <= "0.73" )
			cfg_out.replace("{Dbx_sdl_extra}", "hwscale="+ cfgDbx.sdl_hwscale +"\n");
		else
			cfg_out.replace("{Dbx_sdl_extra}", "");

	// [dosbox]
		cfg_out.replace("{Dbx_dosbox_language}", cfgDbx.dosbox_language);
		cfg_out.replace("{Dbx_dosbox_machine}" , cfgDbx.dosbox_machine);

		if( cfgDbx.dosbox_captures == "" || cfgDbx.dosbox_captures == "capture" )
			cfg_out.replace("{Dbx_dosbox_captures}", dir_app +"capturas/"+ tabla +"/id-"+ datos.idgrl +"_"+ eliminar_caracteres(datos.titulo) +"_"+ datos.tipo_emu);
		else
			cfg_out.replace("{Dbx_dosbox_captures}", cfgDbx.dosbox_captures);

		cfg_out.replace("{Dbx_dosbox_memsize}", cfgDbx.dosbox_memsize);
		cfg_out.replace("{Dbx_dosbox_emu_key}", cfgDbx.dosbox_emu_key);

	// [render]
		cfg_out.replace("{Dbx_render_frameskip}", cfgDbx.render_frameskip);
		cfg_out.replace("{Dbx_render_aspect}"   , cfgDbx.render_aspect);
		cfg_out.replace("{Dbx_render_scaler}"   , cfgDbx.render_scaler);

	// [cpu]
		cfg_out.replace("{Dbx_cpu_core}"            , cfgDbx.cpu_core);
		cfg_out.replace("{Dbx_cpu_cputype}"         , cfgDbx.cpu_cputype);
		cfg_out.replace("{Dbx_cpu_cycles_realmode}" , cfgDbx.cpu_cycles_realmode);
		cfg_out.replace("{Dbx_cpu_cycles_protmode}" , cfgDbx.cpu_cycles_protmode);
		cfg_out.replace("{Dbx_cpu_cycles_limitmode}", cfgDbx.cpu_cycles_limitmode);

		if( cfgDbx.cpu_cycles != "auto" && cfgDbx.cpu_cycles != "max" && dosbox_dato_emu.version >= "0.73" )
			cfg_out.replace("{Dbx_cpu_cycles}", "fixed "+ cfgDbx.cpu_cycles);
		else {
			QString cycles_realmode, cycles_protmode, cycles_limitmode;
			if( cfgDbx.cpu_cycles == "auto" && dosbox_dato_emu.version >= "0.74" )
			{
				cycles_realmode = cycles_protmode = cycles_limitmode = "";
				if( !cfgDbx.cpu_cycles_realmode.isEmpty() )
					cycles_realmode = " "+ cfgDbx.cpu_cycles_realmode;
				if( !cfgDbx.cpu_cycles_protmode.isEmpty() )
					cycles_protmode = " "+ cfgDbx.cpu_cycles_protmode;
				if( !cfgDbx.cpu_cycles_limitmode.isEmpty() )
					cycles_limitmode = " "+ cfgDbx.cpu_cycles_limitmode;

				cfg_out.replace("{Dbx_cpu_cycles}", "auto"+ cycles_realmode + cycles_protmode + cycles_limitmode);
			}
			else if( cfgDbx.cpu_cycles == "max" && dosbox_dato_emu.version >= "0.74" )
			{
				cycles_protmode = cycles_limitmode = "";
				if( !cfgDbx.cpu_cycles_protmode.isEmpty() )
					cycles_protmode = " "+ cfgDbx.cpu_cycles_protmode;
				if( !cfgDbx.cpu_cycles_limitmode.isEmpty() )
					cycles_limitmode = " "+ cfgDbx.cpu_cycles_limitmode;

				cfg_out.replace("{Dbx_cpu_cycles}", "max"+ cycles_protmode + cycles_limitmode);
			} else
				cfg_out.replace("{Dbx_cpu_cycles}", cfgDbx.cpu_cycles);
		}

		cfg_out.replace("{Dbx_cpu_cycleup}"  , cfgDbx.cpu_cycleup);
		cfg_out.replace("{Dbx_cpu_cycledown}", cfgDbx.cpu_cycledown);

	// [mixer]
		cfg_out.replace("{Dbx_mixer_nosound}"  , cfgDbx.mixer_nosound);
		cfg_out.replace("{Dbx_mixer_rate}"     , cfgDbx.mixer_rate);
		cfg_out.replace("{Dbx_mixer_blocksize}", cfgDbx.mixer_blocksize);
		cfg_out.replace("{Dbx_mixer_prebuffer}", cfgDbx.mixer_prebuffer);

	// [midi]
		cfg_out.replace("{Dbx_midi_mpu401}"     , cfgDbx.midi_mpu401);
		cfg_out.replace("{Dbx_midi_intelligent}", cfgDbx.midi_intelligent);

		if( dosbox_dato_emu.version >= "0.73" || tipo_cfg == ExportGrlida )
		{
			cfg_out.replace("{Dbx_midi_device}", cfgDbx.midi_device);
			cfg_out.replace("{Dbx_midi_config}", cfgDbx.midi_config);
		} else {
			cfg_out.replace("mididevice={Dbx_midi_device}", "intelligent="+ cfgDbx.midi_intelligent +"\ndevice="+ cfgDbx.midi_device +"\n");
			cfg_out.replace("midiconfig={Dbx_midi_config}", "config="+ cfgDbx.midi_config +"\nmt32rate="+ cfgDbx.midi_mt32rate +"\n");
		}

		cfg_out.replace("{Dbx_midi_mt32rate}", cfgDbx.midi_mt32rate);

	// [sblaster]
		cfg_out.replace("{Dbx_sblaster_sbtype}" , cfgDbx.sblaster_sbtype);
		cfg_out.replace("{Dbx_sblaster_sbbase}" , cfgDbx.sblaster_sbbase);
		cfg_out.replace("{Dbx_sblaster_irq}"    , cfgDbx.sblaster_irq);
		cfg_out.replace("{Dbx_sblaster_dma}"    , cfgDbx.sblaster_dma);
		cfg_out.replace("{Dbx_sblaster_hdma}"   , cfgDbx.sblaster_hdma);

		if( dosbox_dato_emu.version >= "0.73" )
			cfg_out.replace("{Dbx_sblaster_mixer}", cfgDbx.sblaster_mixer);
		else
			cfg_out.replace("sbmixer={Dbx_sblaster_mixer}", "mixer="+ cfgDbx.sblaster_mixer);

		cfg_out.replace("{Dbx_sblaster_oplmode}", cfgDbx.sblaster_oplmode);
		cfg_out.replace("{Dbx_sblaster_oplemu}" , cfgDbx.sblaster_oplemu);
		cfg_out.replace("{Dbx_sblaster_oplrate}", cfgDbx.sblaster_oplrate);

	// [gus]
		cfg_out.replace("{Dbx_gus_gus}"     , cfgDbx.gus_gus);
		cfg_out.replace("{Dbx_gus_gusrate}" , cfgDbx.gus_gusrate);
		cfg_out.replace("{Dbx_gus_gusbase}" , cfgDbx.gus_gusbase);

		if( dosbox_dato_emu.version >= "0.73" || tipo_cfg == ExportGrlida )
		{
			cfg_out.replace("{Dbx_gus_irq1}", cfgDbx.gus_irq1);
			cfg_out.replace("{Dbx_gus_irq2}", cfgDbx.gus_irq2);
			cfg_out.replace("{Dbx_gus_dma1}", cfgDbx.gus_dma1);
			cfg_out.replace("{Dbx_gus_dma2}", cfgDbx.gus_dma2);
		} else {
			cfg_out.replace("gusirq={Dbx_gus_irq1}", "irq1="+ cfgDbx.gus_irq1 +"\nirq2="+ cfgDbx.gus_irq2 +"\n");
			cfg_out.replace("gusdma={Dbx_gus_dma1}", "dma1="+ cfgDbx.gus_dma1 +"\ndma2="+ cfgDbx.gus_dma2 +"\n");
		}

		cfg_out.replace("{Dbx_gus_ultradir}", cfgDbx.gus_ultradir);

	// [speaker]
		cfg_out.replace("{Dbx_speaker_pcspeaker}", cfgDbx.speaker_pcspeaker);
		cfg_out.replace("{Dbx_speaker_pcrate}"   , cfgDbx.speaker_pcrate);
		cfg_out.replace("{Dbx_speaker_tandy}"    , cfgDbx.speaker_tandy);
		cfg_out.replace("{Dbx_speaker_tandyrate}", cfgDbx.speaker_tandyrate);
		cfg_out.replace("{Dbx_speaker_disney}"   , cfgDbx.speaker_disney);

	// [joystick]
		cfg_out.replace("{Dbx_joystick_type}"      , cfgDbx.joystick_type);
		cfg_out.replace("{Dbx_joystick_timed}"     , cfgDbx.joystick_timed);
		cfg_out.replace("{Dbx_joystick_autofire}"  , cfgDbx.joystick_autofire);
		cfg_out.replace("{Dbx_joystick_swap34}"    , cfgDbx.joystick_swap34);
		cfg_out.replace("{Dbx_joystick_buttonwrap}", cfgDbx.joystick_buttonwrap);

	// [dserial]
		if( tipo_cfg == ExportGrlida )
		{
			cfg_out.replace("{Dbx_modem_modem}"         , cfgDbx.modem_modem);
			cfg_out.replace("{Dbx_modem_comport}"       , cfgDbx.modem_comport);
			cfg_out.replace("{Dbx_modem_listenport}"    , cfgDbx.modem_listenport);
			cfg_out.replace("{Dbx_dserial_directserial}", cfgDbx.dserial_directserial);
			cfg_out.replace("{Dbx_dserial_comport}"     , cfgDbx.dserial_comport);
			cfg_out.replace("{Dbx_dserial_realport}"    , cfgDbx.dserial_realport);
			cfg_out.replace("{Dbx_dserial_defaultbps}"  , cfgDbx.dserial_defaultbps);
			cfg_out.replace("{Dbx_dserial_parity}"      , cfgDbx.dserial_parity);
			cfg_out.replace("{Dbx_dserial_bytesize}"    , cfgDbx.dserial_bytesize);
			cfg_out.replace("{Dbx_dserial_stopbit}"     , cfgDbx.dserial_stopbit);
		} else {
			if( dosbox_dato_emu.version <= "0.73" )
			{
				QString moden_dserial = "\n[modem]\n"; // DOSBox 0.63
				moden_dserial.append("modem="+ cfgDbx.modem_modem +"\n");
				moden_dserial.append("comport="+ cfgDbx.modem_comport +"\n");
				moden_dserial.append("listenport="+ cfgDbx.modem_listenport +"\n\n");

				moden_dserial.append("[directserial]\n"); // DOSBox 0.63
				moden_dserial.append("directserial="+ cfgDbx.dserial_directserial +"\n");
				moden_dserial.append("comport="+ cfgDbx.dserial_comport +"\n");
				moden_dserial.append("realport="+ cfgDbx.dserial_realport +"\n");
				moden_dserial.append("defaultbps="+ cfgDbx.dserial_defaultbps +"\n");
				moden_dserial.append("parity="+ cfgDbx.dserial_parity +"\n");
				moden_dserial.append("bytesize="+ cfgDbx.dserial_bytesize +"\n");
				moden_dserial.append("stopbit="+ cfgDbx.dserial_stopbit +"\n\n");

				cfg_out.replace("{Dbx_moden_y_directserial}", moden_dserial);
			} else
				cfg_out.replace("{Dbx_moden_y_directserial}", "");
		}

	// [serial]
		cfg_out.replace("{Dbx_serial_1}", cfgDbx.serial_1);
		cfg_out.replace("{Dbx_serial_2}", cfgDbx.serial_2);
		cfg_out.replace("{Dbx_serial_3}", cfgDbx.serial_3);
		cfg_out.replace("{Dbx_serial_4}", cfgDbx.serial_4);

	// [dos]
		cfg_out.replace("{Dbx_dos_xms}", cfgDbx.dos_xms);
		cfg_out.replace("{Dbx_dos_ems}", cfgDbx.dos_ems);
		cfg_out.replace("{Dbx_dos_umb}", cfgDbx.dos_umb);

		QHash<QString, stGrlDatos> dbx_keyboardlayout_list = cargaDatosQHash(dir_app +"datos/dbx_keyboardlayout.txt", 4, "|");
		QHash<QString, QString> dbx_keyboardlayout;
		foreach (const stGrlDatos &dat_kb, dbx_keyboardlayout_list)
			dbx_keyboardlayout.insert(dat_kb.key, dat_kb.extra);
		cfg_out.replace("{Dbx_dos_keyboardlayout}", dbx_keyboardlayout[cfgDbx.dos_keyboardlayout]);

		if( tipo_cfg == ExportDfend )
			cfg_out.replace("{Dbx_dos_version}", "ReportedDOSVersion="+ cfgDbx.dos_version +"\n");
		else
			cfg_out.replace("{Dbx_dos_version}", cfgDbx.dos_version);

	// [ipx]
		cfg_out.replace("{Dbx_ipx_ipx}", cfgDbx.ipx_ipx);

		if( tipo_cfg == ExportDbx )
		{
			if( StrToBool( cfgDbx.ipx_ipx ) )
			{
				if( cfgDbx.ipx_type == "server" )
					cfg_out.replace("{Dbx_ipx_config}", "IPXNET STARTSERVER "+ cfgDbx.ipx_port +"\n");
				else if( cfgDbx.ipx_type == "client" )
					cfg_out.replace("{Dbx_ipx_config}", "IPXNET CONNECT "+ cfgDbx.ipx_ip +" "+ cfgDbx.ipx_port +"\n");
				else
					cfg_out.replace("{Dbx_ipx_config}", "");
			} else
				cfg_out.replace("{Dbx_ipx_config}", "");

		// [autoexec]
			QString autoexec = "";

			if( !cfgDbx.dos_version.isEmpty() )
				autoexec.append("ver set "+ cfgDbx.dos_version +"\n\n");

			if( StrToBool( cfgDbx.opt_autoexec ) )
				autoexec.append( cfgDbx.autoexec +"\n");
			else {
				// Creando el Autoexec
				QStringList listamontaje = creaConfigMontajes( twDbxMount, cfgDbx );
				const int listamontajeSize = listamontaje.size();
				for (int i = 0; i < listamontajeSize; ++i)
					autoexec.append(listamontaje.at(i) +"\n");
			}

			cfg_out.replace("{Dbx_autoexec}", autoexec);
		}
		else if( tipo_cfg == ExportGrlida )
		{
		// [ipx]
			cfg_out.replace("{Dbx_ipx_type}", cfgDbx.ipx_type);
			cfg_out.replace("{Dbx_ipx_port}", cfgDbx.ipx_port);
			cfg_out.replace("{Dbx_ipx_ip}"  , cfgDbx.ipx_ip);

		// [autoexec]
			cfg_out.replace("{Dbx_autoexec}", cfgDbx.autoexec);

		// Opciones
			cfg_out.replace("{Dbx_opt_autoexec}"         , cfgDbx.opt_autoexec);
			cfg_out.replace("{Dbx_opt_loadfix}"          , cfgDbx.opt_loadfix);
			cfg_out.replace("{Dbx_opt_loadfix_mem}"      , cfgDbx.opt_loadfix_mem);
			cfg_out.replace("{Dbx_opt_consola_dbox}"     , cfgDbx.opt_consola_dbox);
			cfg_out.replace("{Dbx_opt_cerrar_dbox}"      , cfgDbx.opt_cerrar_dbox);
			cfg_out.replace("{Dbx_opt_cycle_sincronizar}", cfgDbx.opt_cycle_sincronizar);
		// Path
			cfg_out.replace("{Dbx_path_conf}"       , cfgDbx.path_conf);
			cfg_out.replace("{Dbx_path_sonido}"     , cfgDbx.path_sonido);
			cfg_out.replace("{Dbx_path_exe}"        , cfgDbx.path_exe);
			cfg_out.replace("{Dbx_path_setup}"      , cfgDbx.path_setup);
			cfg_out.replace("{Dbx_parametros_exe}"  , cfgDbx.parametros_exe);
			cfg_out.replace("{Dbx_parametros_setup}", cfgDbx.parametros_setup);

		// Montajes
			QString cfg_mount = "";
			const int count_mount_grl = twDbxMount->topLevelItemCount();
			if( count_mount_grl > 0 )
			{
				for (int num_mount_grl = 0; num_mount_grl < count_mount_grl; ++num_mount_grl)
				{
					QTreeWidgetItem *item_mount_grl = twDbxMount->topLevelItem( num_mount_grl );
					cfg_mount.append("\n    <dosbox_montajes>\n");
					cfg_mount.append("      <Dbx_mount_path>"+ item_mount_grl->text(0) +"</Dbx_mount_path>\n");
					cfg_mount.append("      <Dbx_mount_letter>"+ item_mount_grl->text(1) +"</Dbx_mount_letter>\n");
					cfg_mount.append("      <Dbx_mount_tipo_as>"+ item_mount_grl->text(2) +"</Dbx_mount_tipo_as>\n");
					cfg_mount.append("      <Dbx_mount_label>"+ item_mount_grl->text(3) +"</Dbx_mount_label>\n");
					cfg_mount.append("      <Dbx_mount_indx_cd>"+ item_mount_grl->text(4) +"</Dbx_mount_indx_cd>\n");
					cfg_mount.append("      <Dbx_mount_opt_mount>"+ item_mount_grl->text(5) +"</Dbx_mount_opt_mount>\n");
					cfg_mount.append("      <Dbx_mount_io_ctrl>"+ item_mount_grl->text(6) +"</Dbx_mount_io_ctrl>\n");
					cfg_mount.append("      <Dbx_mount_select_mount>"+ item_mount_grl->text(7) +"</Dbx_mount_select_mount>\n");
					cfg_mount.append("      <Dbx_mount_opt_size>"+ item_mount_grl->text(10) +"</Dbx_mount_opt_size>\n");
					cfg_mount.append("      <Dbx_mount_opt_freesize>"+ item_mount_grl->text(11) +"</Dbx_mount_opt_freesize>\n");
					cfg_mount.append("      <Dbx_mount_freesize>"+ item_mount_grl->text(12) +"</Dbx_mount_freesize>\n");
					cfg_mount.append("    </dosbox_montajes>");
				}
			}
			cfg_out.replace("{dosbox_montajes}", cfg_mount);
		} else
			cfg_out.replace("{Dbx_autoexec}", "");
	}

	if( datos.tipo_emu == "scummvm" )
	{
		cfg_out.replace("{Svm_game}"             , cfgSvm.game           );
		cfg_out.replace("{Svm_game_label}"       , cfgSvm.game_label     );
		cfg_out.replace("{Svm_language}"         , cfgSvm.language       );
		cfg_out.replace("{Svm_subtitles}"        , cfgSvm.subtitles      );
		cfg_out.replace("{Svm_platform}"         , cfgSvm.platform       );
		cfg_out.replace("{Svm_gfx_mode}"         , cfgSvm.gfx_mode       );
		cfg_out.replace("{Svm_render_mode}"      , cfgSvm.render_mode    );
		cfg_out.replace("{Svm_fullscreen}"       , cfgSvm.fullscreen     );
		cfg_out.replace("{Svm_aspect_ratio}"     , cfgSvm.aspect_ratio   );
		cfg_out.replace("{Svm_path_game}"        , cfgSvm.path_game      );
		cfg_out.replace("{Svm_path_extra}"       , cfgSvm.path_extra     );
		cfg_out.replace("{Svm_path_save}"        , cfgSvm.path_save      );
		cfg_out.replace("{Svm_path_capturas}"    , cfgSvm.path_capturas  );
		cfg_out.replace("{Svm_path_sonido}"      , cfgSvm.path_sonido    );
		cfg_out.replace("{Svm_music_driver}"     , cfgSvm.music_driver   );
		cfg_out.replace("{Svm_enable_gs}"        , cfgSvm.enable_gs      );
		cfg_out.replace("{Svm_multi_midi}"       , cfgSvm.multi_midi     );
		cfg_out.replace("{Svm_native_mt32}"      , cfgSvm.native_mt32    );
		cfg_out.replace("{Svm_mute}"             , cfgSvm.mute           );
		cfg_out.replace("{Svm_master_volume}"    , cfgSvm.master_volume  );
		cfg_out.replace("{Svm_music_volume}"     , cfgSvm.music_volume   );
		cfg_out.replace("{Svm_sfx_volume}"       , cfgSvm.sfx_volume     );
		cfg_out.replace("{Svm_speech_volume}"    , cfgSvm.speech_volume  );
		cfg_out.replace("{Svm_speech_mute}"      , cfgSvm.speech_mute    );
		cfg_out.replace("{Svm_tempo}"            , cfgSvm.tempo          );
		cfg_out.replace("{Svm_talkspeed}"        , cfgSvm.talkspeed      );
		cfg_out.replace("{Svm_debuglevel}"       , cfgSvm.debuglevel     );
		cfg_out.replace("{Svm_cdrom}"            , cfgSvm.cdrom          );
		cfg_out.replace("{Svm_joystick_num}"     , cfgSvm.joystick_num   );
		cfg_out.replace("{Svm_output_rate}"      , cfgSvm.output_rate    );
		cfg_out.replace("{Svm_midi_gain}"        , cfgSvm.midi_gain      );
		cfg_out.replace("{Svm_copy_protection}"  , cfgSvm.copy_protection);
		cfg_out.replace("{Svm_sound_font}"       , cfgSvm.sound_font     );
		cfg_out.replace("{Svm_walkspeed}"        , cfgSvm.walkspeed      );
		cfg_out.replace("{Svm_opl_driver}"       , cfgSvm.opl_driver     );
		cfg_out.replace("{Svm_disable_dithering}", cfgSvm.disable_dithering);
		cfg_out.replace("{Svm_alt_intro}"        , cfgSvm.alt_intro      );
		cfg_out.replace("{Svm_boot_param}"       , cfgSvm.boot_param     );
	}

	if( datos.tipo_emu == "vdmsound" )
	{
		cfg_out.replace("{Vdms_path_conf}"      , cfgVdms.path_conf      );
		cfg_out.replace("{Vdms_path_exe}"       , cfgVdms.path_exe       );
		cfg_out.replace("{Vdms_program_1}"      , cfgVdms.program_1      );
		cfg_out.replace("{Vdms_program_2}"      , cfgVdms.program_2      );
		cfg_out.replace("{Vdms_vdms_debug_1}"   , cfgVdms.vdms_debug_1   );
		cfg_out.replace("{Vdms_vdms_debug_2}"   , cfgVdms.vdms_debug_2   );
		cfg_out.replace("{Vdms_winnt_dos_1}"    , cfgVdms.winnt_dos_1    );
		cfg_out.replace("{Vdms_winnt_dos_2}"    , cfgVdms.winnt_dos_2    );
		cfg_out.replace("{Vdms_winnt_dosbox_1}" , cfgVdms.winnt_dosbox_1 );
		cfg_out.replace("{Vdms_winnt_dosbox_2}" , cfgVdms.winnt_dosbox_2 );
		cfg_out.replace("{Vdms_winnt_dosbox_3}" , cfgVdms.winnt_dosbox_3 );
		cfg_out.replace("{Vdms_winnt_storage_1}", cfgVdms.winnt_storage_1);
		cfg_out.replace("{Vdms_winnt_storage_2}", cfgVdms.winnt_storage_2);
	}

	QFile file_out( pathSaveConfg );
	if ( file_out.open(QIODevice::WriteOnly | QIODevice::Text) )
	{
		QTextStream out(&file_out);
		out.setCodec("UTF-8");
		out << cfg_out << endl;
		out.flush();
		file_out.close();
	}
}

// Crea la configuración del ScummVM
void Funciones::creaIniScummVM(stConfigScummVM cfgSvm, QString archivo)
{
	QFile file_out;
	file_out.setFileName( archivo );

	comprobarDirectorio( getInfoFile(archivo).Path );

	if( file_out.open(QIODevice::WriteOnly | QIODevice::Text) )
	{
		QTextStream out(&file_out);
		out.setCodec("UTF-8");
		cfgSvm.path_extra = getDirRelative(cfgSvm.path_extra, "DosGames");

		out << "[" << cfgSvm.game_label << "]" << endl;
		out << "description"     << "=" << cfgSvm.description     << endl;
		out << "gameid"          << "=" << cfgSvm.game            << endl;
		out << "language"        << "=" << cfgSvm.language        << endl;
		out << "subtitles"       << "=" << cfgSvm.subtitles       << endl;
		out << "platform"        << "=" << cfgSvm.platform        << endl;
		out << "gfx_mode"        << "=" << cfgSvm.gfx_mode        << endl;
		out << "render_mode"     << "=" << cfgSvm.render_mode     << endl;
		out << "fullscreen"      << "=" << cfgSvm.fullscreen      << endl;
		out << "aspect_ratio"    << "=" << cfgSvm.aspect_ratio    << endl;
		out << "path"            << "=" << getDirRelative(cfgSvm.path_game, "DosGames") << endl;
		out << "extrapath"       << "=" << cfgSvm.path_extra      << endl;
		out << "savepath"        << "=" << getDirRelative(cfgSvm.path_save, "DosGames") << endl;
		out << "music_driver"    << "=" << cfgSvm.music_driver    << endl;
		out << "enable_gs"       << "=" << cfgSvm.enable_gs       << endl;
		out << "multi_midi"      << "=" << cfgSvm.multi_midi      << endl;
		out << "native_mt32"     << "=" << cfgSvm.native_mt32     << endl;
		out << "master_volume"   << "=" << cfgSvm.master_volume   << endl;
		out << "music_volume"    << "=" << cfgSvm.music_volume    << endl;
		out << "sfx_volume"      << "=" << cfgSvm.sfx_volume      << endl;
		out << "speech_volume"   << "=" << cfgSvm.speech_volume   << endl;
		out << "tempo"           << "=" << cfgSvm.tempo           << endl;
		out << "talkspeed"       << "=" << cfgSvm.talkspeed       << endl;
		out << "cdrom"           << "=" << cfgSvm.cdrom           << endl;
		out << "joystick_num"    << "=" << cfgSvm.joystick_num    << endl;
		out << "output_rate"     << "=" << cfgSvm.output_rate     << endl;
		out << "midi_gain"       << "=" << cfgSvm.midi_gain       << endl;
		out << "copy_protection" << "=" << cfgSvm.copy_protection << endl;
		out << "soundfont"       << "=" << cfgSvm.sound_font      << endl;
		out << "walkspeed"       << "=" << cfgSvm.walkspeed       << endl << endl;

		out << "[scummvm]" << endl;
		out << "gui_theme"  << "=" << "modern" << endl;
		out << "gfx_mode"   << "=" << "2x"     << endl;
		out << "fullscreen" << "=" << "false"  << endl;
		out << "extrapath"  << "=" << cfgSvm.path_extra << endl;
		out << "themepath"  << "=" << getDirRelative(cfgSvm.emu_svmpath) << endl << endl;

		out.flush();
	}
	file_out.close();
}

// Crea el archivo de configuración del VDMSound
void Funciones::crearArchivoConfigVdmS(stConfigVDMSound cfgVdms, QString pathSaveConfg)
{
	QFile file_out;
	QString file_name_conf;
	stFileInfo f_info = getInfoFile(pathSaveConfg);

	comprobarDirectorio( f_info.Path );

	if( f_info.Exists )
		file_name_conf = f_info.Path + f_info.Name +"_"+ HoraFechaActual(getTime(), "ddMMyyyy_HHmmss") + f_info.Ext;
	else
		file_name_conf = f_info.FilePath;

	file_out.setFileName( file_name_conf );
	if( file_out.open(QIODevice::WriteOnly | QIODevice::Text) )
	{
		QTextStream out(&file_out);
		out.setCodec("UTF-8");

		out << "[program]" << endl;
		out << "executable" << "=" << cfgVdms.path_exe  << endl;
		out << "workdir"    << "=" << getInfoFile( cfgVdms.path_exe ).Path << endl;
		out << "params"     << "=" << cfgVdms.program_1 << endl;
		out << "icon"       << "=" << cfgVdms.program_2 << endl << endl;

		out << "[vdms.debug]" << endl;
		out << "useCustomCfg" << "=" << cfgVdms.vdms_debug_1 << endl;
		out << "customCfg"    << "=" << cfgVdms.vdms_debug_2 << endl << endl;

		out << "[winnt.dos]" << endl;
		out << "useAutoexec" << "=" << cfgVdms.winnt_dos_1 << endl;
		out << "autoexec"    << "=" << cfgVdms.winnt_dos_2 << endl << endl;

		out << "[winnt.dosbox]" << endl;
		out << "exitclose" << "=" << cfgVdms.winnt_dosbox_1 << endl;
		out << "exitWarn"  << "=" << cfgVdms.winnt_dosbox_2 << endl;
		out << "fastPaste" << "=" << cfgVdms.winnt_dosbox_3 << endl << endl;

		out << "[winnt.storage]" << endl;
		out << "useCDROM"   << "=" << cfgVdms.winnt_storage_1 << endl;
		out << "useNetware" << "=" << cfgVdms.winnt_storage_2 << endl << endl;

		out.flush();
	}
	file_out.close();
}

// Importar la configuración del DOSBox del DFend
QHash<QString, QString> Funciones::importarProfileDFend(QString dir_app, QString tabla, QString filename)
{
	QHash<QString, QString> cfgDFend;
//	QSettings optgrl(dir_app +"GR-lida.conf", QSettings::IniFormat);
//	optgrl.beginGroup("OpcGeneral");
//		QString dirBaseGames = optgrl.value("DirBaseGames", "./DosGames/").toString();
//	optgrl.endGroup();

// Leer archivo ------------------------------------------------------
	QString lineas = leerArchivo(filename);
	lineas.replace("\\", "\\\\");
	lineas.replace(";", "|");
	lineas.replace(",", "--");

// Escribir archivo --------------------------------------------------
	QFile file_out(dir_app +"temp/temp_config.prof");
	file_out.open(QIODevice::WriteOnly | QIODevice::Text);

	QTextStream out(&file_out);
	out.setCodec("UTF-8");
	out << lineas;
	out.flush();
	file_out.close();
	lineas.clear();

	QSettings settings(dir_app +"temp/temp_config.prof", QSettings::IniFormat);

// Datos del juego -----
	settings.beginGroup("ExtraInfo");
		QString icono = settings.value("Icon", "dosbox").toString().replace("--", ",");
		cfgDFend["Dat_icono"]            = icono.isEmpty() ? "dosbox" : icono;
		cfgDFend["Dat_titulo"]           = settings.value("Name", "").toString().replace("--", ";");
		cfgDFend["Dat_subtitulo"]        = "";
		cfgDFend["Dat_genero"]           = settings.value("Genre", "").toString().replace("--", ";");
		cfgDFend["Dat_compania"]         = settings.value("Publisher", "").toString().replace("--", ";");
		cfgDFend["Dat_desarrollador"]    = settings.value("Developer", "").toString().replace("--", ";");
		cfgDFend["Dat_tema"]             = "";
		cfgDFend["Dat_grupo"]            = "";
		cfgDFend["Dat_perspectiva"]      = "";
		cfgDFend["Dat_idioma"]           = settings.value("Language", "").toString().replace("--", ";");
		cfgDFend["Dat_idioma_voces"]     = "";
		cfgDFend["Dat_formato"]          = "";
		cfgDFend["Dat_anno"]             = settings.value("Year", "").toString().replace("--", ";");
		cfgDFend["Dat_numdisc"]          = "";
		cfgDFend["Dat_sistemaop"]        = "";
		cfgDFend["Dat_tamano"]           = "";
		cfgDFend["Dat_graficos"]         = "0";
		cfgDFend["Dat_sonido"]           = "0";
		cfgDFend["Dat_jugabilidad"]      = "0";
		cfgDFend["Dat_original"]         = "false";
		cfgDFend["Dat_estado"]           = "";
		cfgDFend["Dat_thumbs"]           = "";
		cfgDFend["Dat_cover_front"]      = "";
		cfgDFend["Dat_cover_back"]       = "";
		cfgDFend["Dat_fecha"]            = "";
		cfgDFend["Dat_tipo_emu"]         = "dosbox";
		cfgDFend["Dat_comentario"]       = settings.value("Notes", "").toString().replace("--", ";").replace("[13][10]", "<br/>\n").replace("[13]", "\n");
		cfgDFend["Dat_favorito"]         = BoolToStr( settings.value("Favorite", "false").toBool() );
		cfgDFend["Dat_rating"]           = "0";
		cfgDFend["Dat_edad_recomendada"] = "";
		cfgDFend["Dat_usuario"]          = "";
		cfgDFend["Dat_path_exe"]         = "";
		cfgDFend["Dat_parametros_exe"]   = "";
		cfgDFend["Dat_path_capturas"]    = "";
		cfgDFend["Dat_path_setup"]       = "";
		cfgDFend["Dat_parametros_setup"] = "";
	//--
		cfgDFend["Dat_www_1_name"] = settings.value("WWWName", "").toString().replace("--", ";");
		cfgDFend["Dat_www_1"]      = settings.value("WWW", "").toString().replace("--", ";");
		for (int i = 2; i < 10; ++i)
		{
			cfgDFend.insert("Dat_www_"+ IntToStr(i) +"_name", settings.value("WWW"+ IntToStr(i) +"Name", "").toString().replace("--", ";") );
			cfgDFend.insert("Dat_www_"+ IntToStr(i), settings.value("WWW"+ IntToStr(i), "").toString().replace("--", ";") );
		}
		cfgDFend["UserInfo"]= settings.value("UserInfo", "").toString().replace("--", ",");
	settings.endGroup();

// Datos del emulador DOSBox ---
	QString file_name_conf = eliminar_caracteres( cfgDFend["Dat_titulo"] );
	stFileInfo f_info = getInfoFile(dir_app +"confdbx/"+ tabla +"/"+ file_name_conf +".conf");

	if( f_info.Exists )
		cfgDFend["Dbx_path_conf"] = f_info.Name +"_"+ HoraFechaActual(getTime(), "ddMMyyyy_HHmmss") +".conf";
	else
		cfgDFend["Dbx_path_conf"] = f_info.Name +".conf";

	cfgDFend["Dbx_path_sonido"] = "";

	settings.beginGroup("Extra");
	// Path
		cfgDFend["Dbx_path_exe"]         = settings.value("Exe"            , "").toString().replace("--", ",");
		cfgDFend["Dbx_path_setup"]       = settings.value("Setup"          , "").toString().replace("--", ",");
		cfgDFend["Dbx_parametros_exe"]   = settings.value("GameParameters" , "").toString().replace("--", ",");
		cfgDFend["Dbx_parametros_setup"] = settings.value("SetupParameters", "").toString().replace("--", ",");
	// [autoexec]
		cfgDFend["Dbx_autoexec"] = settings.value("autoexec", "").toString().replace("--", ",").replace("[13][10]", "\n");
	// Opciones
		cfgDFend["Dbx_opt_autoexec"]     = cfgDFend["Dbx_autoexec"].isEmpty() ? "false" : "true";
		cfgDFend["Dbx_opt_loadfix"]      = BoolToStr( settings.value("Loadhigh", false).toBool() );
		cfgDFend["Dbx_opt_loadfix_mem"]  = settings.value("LoadFixVal", "64").toString();
		cfgDFend["Dbx_opt_consola_dbox"] = BoolToStr( settings.value("ConsoleWindow", false).toBool() );
		cfgDFend["Dbx_opt_cerrar_dbox"]  = BoolToStr( settings.value("CloseOnExit", true).toBool() );
		cfgDFend["Dbx_opt_cycle_sincronizar"] = "false";
		cfgDFend["Dbx_ExtraFiles"]       = settings.value("ExtraFiles", "").toString();
		cfgDFend["Dbx_NrOfMounts"]       = settings.value("NrOfMounts", "0").toString();

		int num_mounts = cfgDFend["Dbx_NrOfMounts"].toInt();
		if( num_mounts > -1 )
		{
			for(int n = 0; n < num_mounts; n++)
				cfgDFend.insert(IntToStr(n), settings.value(IntToStr(n), "").toString() );
		}

//		cfgDFend["ExeMD5"]            = settings.value("ExeMD5"           ,"").toString();
//		cfgDFend["LastModification"]  = settings.value("LastModification" ,"").toString();
//		cfgDFend["Environment"]       = settings.value("Environment"      ,"").toString();
//		cfgDFend["SetupMD5"]          = settings.value("SetupMD5"         ,"").toString();
//		cfgDFend["DataDir"]           = settings.value("DataDir"          ,"").toString();
//		cfgDFend["Overridegamestart"] = settings.value("Overridegamestart","").toString();
//		cfgDFend["OverrideMount"]     = settings.value("OverrideMount"    ,"").toString();
//		cfgDFend["AutoMountCDs"]      = settings.value("AutoMountCDs"     ,"").toString();
//		cfgDFend["ExtraDirs"]         = settings.value("ExtraDirs"        ,"").toString();
//		cfgDFend["ExtraExe0"]         = settings.value("ExtraExe0"        ,"").toString();
//		cfgDFend["BootImage"]         = settings.value("BootImage"        ,"").toString();
//		cfgDFend["Finalization"]      = settings.value("Finalization"     ,"").toString();
//		cfgDFend["CustomSettings"]    = settings.value("CustomSettings"   ,"").toString();
//		cfgDFend["ScreenshotListScreenshot"] = settings.value("ScreenshotListScreenshot","").toString();

		if( settings.value("ProfileMode").toString().toLower() == "dosbox" )
			cfgDFend["Dat_tipo_emu"] = "dosbox";
		else if( settings.value("ProfileMode").toString().toLower() == "scummvm" )
			cfgDFend["Dat_tipo_emu"] = "scummvm";
		else
			cfgDFend["Dat_tipo_emu"] = "datos";
	settings.endGroup();

// [sdl]
	settings.beginGroup("sdl");
		cfgDFend["Dbx_sdl_fullscreen"]       = BoolToStr( settings.value("fullscreen", false).toBool() );
		cfgDFend["Dbx_sdl_fulldouble"]       = BoolToStr( settings.value("fulldouble", false).toBool() );
		cfgDFend["Dbx_sdl_fullfixed"]        = BoolToStr( settings.value("fullfixed" , false).toBool() ); // en desuso
		cfgDFend["Dbx_sdl_fullresolution"]   = settings.value("fullresolution"  , "original").toString();
		cfgDFend["Dbx_sdl_windowresolution"] = settings.value("windowresolution", "original").toString();
		cfgDFend["Dbx_sdl_output"]           = settings.value("output"          , "surface" ).toString();
		cfgDFend["Dbx_sdl_hwscale"]          = settings.value("hwscale"         , "1.00"    ).toString();
		cfgDFend["Dbx_sdl_autolock"]         = BoolToStr( settings.value("autolock", true).toBool() );
		cfgDFend["Dbx_sdl_sensitivity"]      = settings.value("sensitivity", "100").toString();
		cfgDFend["Dbx_sdl_waitonerror"]      = BoolToStr( settings.value("waitonerror", true).toBool() );
		cfgDFend["Dbx_sdl_priority"]         = settings.value("priority"  , "higher,normal").toString().replace("--", ",");
		cfgDFend["Dbx_sdl_mapperfile"]       = settings.value("mapperfile", "mapper.txt"   ).toString().replace("--", ",");
		cfgDFend["Dbx_sdl_usescancodes"]     = BoolToStr( settings.value("usescancodes", true).toBool() );
//		cfgDFend["Dbx_sdl_dosboxlanguage"]   = settings.value("DOSBoxlanguage", "default").toString();
	settings.endGroup();
// [dosbox]
	settings.beginGroup("dosbox");
		cfgDFend["Dbx_dosbox_language"] = settings.value("language", "").toString().replace("--", ",");
		cfgDFend["Dbx_dosbox_machine"]  = settings.value("machine" , "svga_s3").toString();
		cfgDFend["Dbx_dosbox_captures"] = settings.value("captures", "capture").toString().replace("--", ",");
		cfgDFend["Dbx_dosbox_memsize"]  = settings.value("memsize" , "16").toString();
	settings.endGroup();
// [render]
	settings.beginGroup("render");
		cfgDFend["Dbx_render_frameskip"] = settings.value("frameskip", "0").toString();
		cfgDFend["Dbx_render_aspect"]    = BoolToStr( settings.value("aspect", false).toBool() );
		cfgDFend["Dbx_render_scaler"]    = settings.value("scaler", "normal2x").toString();
//		cfgDFend["Dbx_render_textmodelines = settings.value("TextModeLines", "25").toString();
	settings.endGroup();
// [cpu]
	settings.beginGroup("cpu");
		cfgDFend["Dbx_cpu_core"]         = settings.value("core"     , "auto").toString();
		cfgDFend["Dbx_cpu_cputype"]      = settings.value("type"     , "auto").toString();
		cfgDFend["Dbx_cpu_cycles"]       = settings.value("cycles"   , "auto").toString();
		cfgDFend["cpu_cycles_realmode"]  = "";
		cfgDFend["cpu_cycles_protmode"]  = "";
		cfgDFend["cpu_cycles_limitmode"] = "";
		cfgDFend["Dbx_cpu_cycleup"]      = settings.value("cycleup"  , "10").toString();
		cfgDFend["Dbx_cpu_cycledown"]    = settings.value("cycledown", "20").toString();
	settings.endGroup();
// [mixer]
	settings.beginGroup("mixer");
		cfgDFend["Dbx_mixer_nosound"]   = BoolToStr( settings.value("nosound", false).toBool() );
		cfgDFend["Dbx_mixer_rate"]      = settings.value("rate"     , "44100").toString();
		cfgDFend["Dbx_mixer_blocksize"] = settings.value("blocksize", "1024").toString();
		cfgDFend["Dbx_mixer_prebuffer"] = settings.value("prebuffer", "20").toString();
	settings.endGroup();
// [midi]
	settings.beginGroup("midi");
		cfgDFend["Dbx_midi_mpu401"]      = settings.value("mpu401", "intelligent").toString();
		cfgDFend["Dbx_midi_intelligent"] = BoolToStr( settings.value("midi_intelligent", true).toBool() );
		cfgDFend["Dbx_midi_device"]      = settings.value("device"  , "default").toString();
		cfgDFend["Dbx_midi_config"]      = settings.value("config"  , "").toString();
		cfgDFend["Dbx_midi_mt32rate"]    = settings.value("mt32rate", "44100").toString();
	settings.endGroup();
// [sblaster]
	settings.beginGroup("sblaster");
		cfgDFend["Dbx_sblaster_sbtype"]  = settings.value("sbtype", "sb16").toString();
		cfgDFend["Dbx_sblaster_sbbase"]  = settings.value("sbbase", "220").toString();
		cfgDFend["Dbx_sblaster_irq"]     = settings.value("irq"   , "7").toString();
		cfgDFend["Dbx_sblaster_dma"]     = settings.value("dma"   , "1").toString();
		cfgDFend["Dbx_sblaster_hdma"]    = settings.value("hdma"  , "5").toString();
		cfgDFend["Dbx_sblaster_mixer"]   = BoolToStr( settings.value("mixer", true).toBool() );
		cfgDFend["Dbx_sblaster_oplmode"] = settings.value("oplmode", "auto").toString();
		cfgDFend["Dbx_sblaster_oplemu"]  = settings.value("oplemu" , "default").toString();
		cfgDFend["Dbx_sblaster_oplrate"] = settings.value("oplrate", "44100").toString();
	settings.endGroup();
// [gus]
	settings.beginGroup("gus");
		cfgDFend["Dbx_gus_gus"]      = BoolToStr( settings.value("gus", false).toBool() );
		cfgDFend["Dbx_gus_gusrate"]  = settings.value("gusrate" , "44100").toString();
		cfgDFend["Dbx_gus_gusbase"]  = settings.value("gusbase" , "240").toString();
		cfgDFend["Dbx_gus_irq1"]     = settings.value("irq1"    , "5").toString();
		cfgDFend["Dbx_gus_irq2"]     = settings.value("irq2"    , "5").toString();
		cfgDFend["Dbx_gus_dma1"]     = settings.value("dma1"    , "3").toString();
		cfgDFend["Dbx_gus_dma2"]     = settings.value("dma2"    , "3").toString();
		cfgDFend["Dbx_gus_ultradir"] = settings.value("ultradir", "C:\\ULTRASND").toString().replace("--",",");
	settings.endGroup();
// [speaker]
	settings.beginGroup("speaker");
		cfgDFend["Dbx_speaker_pcspeaker"] = BoolToStr( settings.value("pcspeaker", true).toBool() );
		cfgDFend["Dbx_speaker_pcrate"]    = settings.value("pcrate"   , "44100").toString();
		cfgDFend["Dbx_speaker_tandy"]     = settings.value("tandy"    , "auto").toString();
		cfgDFend["Dbx_speaker_tandyrate"] = settings.value("tandyrate", "44100").toString();
		cfgDFend["Dbx_speaker_disney"]    = BoolToStr( settings.value("disney", true).toBool() );
	settings.endGroup();
// [joystick]
	settings.beginGroup("joystick");
		cfgDFend["Dbx_joystick_type"]       = settings.value("joysticktype", "auto").toString();
		cfgDFend["Dbx_joystick_timed"]      = BoolToStr( settings.value("timed"     , true ).toBool() );
		cfgDFend["Dbx_joystick_autofire"]   = BoolToStr( settings.value("autofire"  , false).toBool() );
		cfgDFend["Dbx_joystick_swap34"]     = BoolToStr( settings.value("swap34"    , false).toBool() );
		cfgDFend["Dbx_joystick_buttonwrap"] = BoolToStr( settings.value("buttonwrap", false).toBool() );
	settings.endGroup();
// [modem]
	cfgDFend["Dbx_modem_modem"]          = "false";
	cfgDFend["Dbx_modem_comport"]        = "2";
	cfgDFend["Dbx_modem_listenport"]     = "23";
// [dserial]
	cfgDFend["Dbx_dserial_directserial"] = "false";
	cfgDFend["Dbx_dserial_comport"]      = "1";
	cfgDFend["Dbx_dserial_realport"]     = "COM1";
	cfgDFend["Dbx_dserial_defaultbps"]   = "1200";
	cfgDFend["Dbx_dserial_parity"]       = "N";
	cfgDFend["Dbx_dserial_bytesize"]     = "8";
	cfgDFend["Dbx_dserial_stopbit"]      = "1";
// [serial]
	settings.beginGroup("serial");
		cfgDFend["Dbx_serial_1"] = settings.value("serial1", "dummy"   ).toString().replace("--",",");
		cfgDFend["Dbx_serial_2"] = settings.value("serial2", "dummy"   ).toString().replace("--",",");
		cfgDFend["Dbx_serial_3"] = settings.value("serial3", "disabled").toString().replace("--",",");
		cfgDFend["Dbx_serial_4"] = settings.value("serial4", "disabled").toString().replace("--",",");
	settings.endGroup();
// [dos]
	settings.beginGroup("dos");
		cfgDFend["Dbx_dos_xms"] = BoolToStr( settings.value("xms", true).toBool() );
		cfgDFend["Dbx_dos_ems"] = BoolToStr( settings.value("ems", true).toBool() );
		cfgDFend["Dbx_dos_umb"] = settings.value("umb", "true").toString();

		if( settings.value("keyboardlayout", "auto").toString().toLower() == "default" )
			cfgDFend["Dbx_dos_keyboardlayout"] = "auto";
		else
			cfgDFend["Dbx_dos_keyboardlayout"] = settings.value("keyboardlayout", "auto").toString();

		cfgDFend["Dbx_dos_version"] = settings.value("ReportedDOSVersion", "").toString();
//		cfgDFend["Dbx_dos_codepage"] = settings.value("codepage", "").toString();
	settings.endGroup();
// [ipx]
	settings.beginGroup("ipx");
		cfgDFend["Dbx_ipx_ipx"]  = BoolToStr( settings.value("ipx", false).toBool() );
		cfgDFend["Dbx_ipx_type"] = settings.value("type"   , "none").toString();
		cfgDFend["Dbx_ipx_port"] = settings.value("port"   , "213").toString();
		cfgDFend["Dbx_ipx_ip"]   = settings.value("address", "").toString();
	settings.endGroup();

// Datos del emulador ScummVM --
// tipo_emu scummvm ---
	settings.beginGroup("ScummVM");
		cfgDFend["Svm_game"]              = settings.value("GameName", "").toString();
		cfgDFend["Svm_game_label"]        = settings.value("GameName", "").toString();
		cfgDFend["Svm_language"]          = settings.value("Language", "es").toString();
		cfgDFend["Svm_subtitles"]         = BoolToStr( settings.value("Subtitles", true).toBool() );
		cfgDFend["Svm_platform"]          = settings.value("platform", "pc").toString();
		cfgDFend["Svm_gfx_mode"]          = settings.value("Filter", "2x").toString();
		cfgDFend["Svm_render_mode"]       = settings.value("render_mode", "").toString();
		cfgDFend["Svm_fullscreen"]        = BoolToStr( settings.value("fullscreen", false).toBool() );
		cfgDFend["Svm_aspect_ratio"]      = BoolToStr( settings.value("aspect_ratio", true).toBool() );
		cfgDFend["Svm_path_game"]         = settings.value("GamePath", "").toString();
		cfgDFend["Svm_path_extra"]        = settings.value("path_extra", "").toString();
		cfgDFend["Svm_path_save"]         = settings.value("Savepath", "").toString();
		cfgDFend["Svm_path_capturas"]     = settings.value("path_capturas", "").toString();
		cfgDFend["Svm_path_sonido"]       = settings.value("path_sonido", "").toString();
		cfgDFend["Svm_music_driver"]      = settings.value("MusicDriver", "adlib").toString();
		cfgDFend["Svm_enable_gs"]         = BoolToStr( settings.value("EnableGS", true).toBool() );
		cfgDFend["Svm_multi_midi"]        = BoolToStr( settings.value("MultiMIDI", true).toBool() );
		cfgDFend["Svm_native_mt32"]       = BoolToStr( settings.value("NativeMT32", true).toBool() );
		cfgDFend["Svm_mute"]              = "false";
		cfgDFend["Svm_master_volume"]     = settings.value("MusicVolume", "192").toString();
		cfgDFend["Svm_music_volume"]      = settings.value("MusicVolume", "192").toString();
		cfgDFend["Svm_sfx_volume"]        = settings.value("SFXVolume", "192").toString();
		cfgDFend["Svm_speech_volume"]     = settings.value("SpeechVolume", "192").toString();
		cfgDFend["Svm_speech_mute"]       = "false";
		cfgDFend["Svm_tempo"]             = settings.value("tempo", "100").toString();
		cfgDFend["Svm_talkspeed"]         = settings.value("TalkSpeed", "60").toString();
		cfgDFend["Svm_debuglevel"]        = settings.value("debuglevel", "0").toString();
		cfgDFend["Svm_cdrom"]             = settings.value("CDROM", "0").toString();
		cfgDFend["Svm_joystick_num"]      = settings.value("JoystickNum", "0").toString();
		cfgDFend["Svm_output_rate"]       = settings.value("SampleRate", "").toString();
		cfgDFend["Svm_midi_gain"]         = settings.value("MIDIGain", "100").toString();
		cfgDFend["Svm_copy_protection"]   = BoolToStr( settings.value("copy_protection", false).toBool() );
		cfgDFend["Svm_sound_font"]        = settings.value("sound_font", "").toString();
		cfgDFend["Svm_walkspeed"]         = settings.value("Walkspeed", "0").toString();
		cfgDFend["Svm_opl_driver"]        = "";
		cfgDFend["Svm_disable_dithering"] = "";
		cfgDFend["Svm_alt_intro"]         = "";
		cfgDFend["Svm_boot_param"]        = "";
	settings.endGroup();

	if( file_out.exists() )
		file_out.remove();

	return cfgDFend;
}
