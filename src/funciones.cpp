/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2018 Pedro A. Garcia Rosado Aka Monthy
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

#include <QObject>
#include <QDesktopServices>
#include <QSettings>
#include <QTextStream>
#include <QTextCodec>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QFileSystemWatcher>
#include <QUrl>
#include <QDateTime>
#include <QDirIterator>
#include <QPainter>
#include <QMessageBox>

#include "funciones.h"
#include "findfiles.h"

#include <QCryptographicHash>
#include "crc32qt.h"

Funciones::Funciones()
{
	stOS     = "win";
	stDirApp = homePath();
}

Funciones::~Funciones()
{
	//
}

// Compara distintas versiones y devuelve True si es nueva o mayor
stVersion Funciones::version_compare(QString ver_old, QString ver_new)
{
	stVersion version;
	version.isNew = false;
	version.info  = esIgual;

	QStringList v_old, v_new;
	v_old = ver_old.split(".");
	v_new = ver_new.split(".");

	if (v_old.size() < 3)
	{
		int num = 3 - v_old.size();
		for (int i = 0; i < num; ++i)
			v_old.insert(i, "0");
		ver_old = v_old.join(".");
	}

	if (v_new.size() < 3)
	{
		int num = 3 - v_new.size();
		for (int i = 0; i < num; ++i)
			v_new.insert(i, "0");
		ver_new = v_new.join(".");
	}

	if (v_new.at(0).toInt() > v_old.at(0).toInt())
	{
		version.isNew = true;
		version.info  = esMayor;
	}
	else // Major ver are =
	{
		if (v_new.at(1).toInt() > v_old.at(1).toInt())
		{
			version.isNew = true;
			version.info  = esMayor;
		}
		else if (v_new.at(1).toInt() == v_old.at(1).toInt()) // Minor ver are =
		{
			if (v_new.at(2).toInt() > v_old.at(2).toInt())
			{
				version.isNew = true;
				version.info  = esMayor;
			} else
				version.isNew = false;
		}
	}

	if (!version.isNew)
	{
		if (ver_old == ver_new)
			version.info = esIgual;
		else
			version.info = esMenor;
	}

	return version;
}

// Crea una copia de seguridad de la base de datos
void Funciones::crearCopiaSeguridad_DB(QString dir, QString version_grl)
{
	uint secs = QDateTime::currentDateTime().toTime_t();
	comprobarDirectorio(dir +"backup/");

	if (version_compare(version_grl, versionGrl()).isNew)
	{
		if (QFile::exists(dir +"backup/db_grl_v"+ version_grl +"_backup.grl"))
			QFile::copy(dir +"db_grl.grl", dir +"backup/db_grl_v"+ version_grl +"_"+ horaFechaActual(intToStr(secs), "ddMMyyyy_HHmmss") +"_backup.grl");
		else
			QFile::copy(dir +"db_grl.grl", dir +"backup/db_grl_v"+ version_grl +"_backup.grl");
	}
}

// Convierte de número a texto
QString Funciones::intToStr(int num)
{
	return QVariant(num).toString();
}

QString Funciones::intToStr(qint64 num)
{
	return QVariant(num).toString();
}

QString Funciones::intToStr(quint64 num)
{
	return QVariant(num).toString();
}

QString Funciones::intToStr(uint num)
{
	return QVariant(num).toString();
}

// Convierte de texto a número
int Funciones::strToInt(QString str)
{
	return str.toInt();
}

// Convierte texto "true", "yes", "1" a tipo bool
bool Funciones::strToBool(QString str)
{
	QString value = str.toLower();
	if (value == "true" || value == "yes" || value == "1")
		return true;
	else
		return false;
}

// Convierte bool a texto "true", "yes" indicando el tipo
QString Funciones::boolToStr(bool estado, bool type_yes)
{
	if (estado)
		return type_yes ? "yes": "true";
	else
		return type_yes ? "no": "false";
}

// Devuelve las cordenadas de posición
int Funciones::getPosition(int posText, bool wordWrap)
{
	int pos = Qt::AlignRight | Qt::AlignTop;
	switch (posText)
	{
		case 0: // topLeft
			pos = Qt::AlignLeft | Qt::AlignTop;
		break;
		case 1: // topCenter
			pos = Qt::AlignHCenter | Qt::AlignTop;
		break;
		case 2: // topRight
			pos = Qt::AlignRight | Qt::AlignTop;
		break;
		case 3: // centerLeft
			pos = Qt::AlignLeft | Qt::AlignVCenter;
		break;
		case 4: // center
			pos = Qt::AlignCenter;
		break;
		case 5: // centerRight
			pos = Qt::AlignRight | Qt::AlignVCenter;
		break;
		case 6: // bottomLeft
			pos = Qt::AlignLeft | Qt::AlignBottom;
		break;
		case 7: // bottomCenter
			pos = Qt::AlignHCenter | Qt::AlignBottom;
		break;
		case 8: // bottomRight
			pos = Qt::AlignRight | Qt::AlignBottom;
		break;
	}

	if (wordWrap)
		return pos | Qt::TextWordWrap;
	else
		return pos;
}

// Devuelve un QImagen en estado desactivado
QImage Funciones::imagenToDisabled(QString filename)
{
	QPixmap pixmap(filename);
	QIcon icon(pixmap);
	return icon.pixmap(pixmap.size(), QIcon::Disabled).toImage();
}

// Devuelve un QColor desde un QStringList o QString
QColor Funciones::getColor(QStringList color)
{
	int r = 0; int g = 0; int b = 0; int a = 255;

	if (color.isEmpty())
		color << "0" << "0" << "0";

	if (color.size() < 4)
	{
		int pos = color.size();
		for (int i = pos; i < 3; ++i)
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
	return getColor(color.split(","));
}

// Convierte un QColor a un QStringList o QString
QStringList Funciones::setColor(QColor color)
{
	QStringList list_color;
	int r = 0; int g = 0; int b = 0;

	r = qRed(color.rgb());
	g = qGreen(color.rgb());
	b = qBlue(color.rgb());

	list_color << intToStr(r) << intToStr(g) << intToStr(b);

	return list_color;
}

QString Funciones::setColorStr(QColor color)
{
	return setColor(color).join(",");
}

// Devuelve valor Script
QString Funciones::getValueScript(QScriptEngine &engine, QString key, QString str_default)
{
	QString str = str_default;
	if (engine.evaluate(key).isValid())
		str = engine.evaluate(key).toString();
	return str;
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
	return intToStr(secs);
}

// Devuelve la hora y la fecha
QString Funciones::horaFechaActual(QString fecha, QString formato)
{
	QDateTime dt;
	dt.setTime_t(fecha.toUInt());

	if (formato.isEmpty())
		return dt.toString("dd/MM/yyyy HH:mm:ss");
	else
		return dt.toString(formato);
}

// Crea y devuelve un listado de fechas desde 1970 a año actual
QStringList Funciones::listaFechas()
{
	QStringList fechas;
	const int fecha_actual = horaFechaActual(getTime(), "yyyy").toInt()+1;
	for (int fecha = 1970; fecha < fecha_actual; ++fecha)
		fechas << intToStr(fecha);
	return fechas;
}

bool Funciones::questionMsg(QString titulo, QString mensaje, QString texto_btn_si, QString texto_btn_no)
{
	texto_btn_si = texto_btn_si.isEmpty() ? tr("Si") : texto_btn_si;
	texto_btn_no = texto_btn_no.isEmpty() ? tr("No") : texto_btn_no;

	int respuesta = QMessageBox::question(0, titulo, mensaje, texto_btn_si, texto_btn_no, 0, 1);
	if (respuesta == 0)
		return true;
	return false;
}

// Devuelve una forma correcta de parametros
QString Funciones::getParametrosEXE(QString parametros)
{
	QStringList param_list, param_exe;
	param_list << parametros.split("|:|", QString::SkipEmptyParts);

	const int paramListSize = param_list.size();
	for (int i = 0; i < paramListSize; ++i)
		param_exe << param_list[i].replace("|", "");

	return param_exe.join("|");
}

// Devuelve una forma correcta
QString Funciones::str_end(QString str, QString str_fin)
{
	if (!str_fin.isEmpty())
	{
		if (!str.endsWith(str_fin, Qt::CaseInsensitive))
			str.append(str_fin);
	}
	return str;
}

// Elimina caracteres no permitidos por windows por ejemplo
QString Funciones::eliminar_caracteres(QString str, QString cambiar, QString sep)
{
	if (str.isEmpty())
		return "";
	else {
		str.replace(" " , "_");
		str.replace("\\", "");
		str.replace("/" , "");
		str.replace(":" , "");
		str.replace("*" , "");
		str.replace("?" , "");
		str.replace("\"", "");
		str.replace("<" , "");
		str.replace(">" , "");
		str.replace("|" , "");
		str.replace("." , "");

		if (!cambiar.isEmpty())
		{
			QStringList lista = cambiar.split(sep, QString::SkipEmptyParts);
			const int listSize = lista.size();
			for (int i = 0; i < listSize; ++i)
				str.replace(lista.at(i), "");
		}

		return str;
	}
}

// Obtiene una la lista de archivos
QFileInfoList Funciones::getListFiles(QString dir, QStringList filter, bool isSubDir)
{
	QFileInfoList list;
	QDirIterator it(dir, filter, QDir::Files | QDir::Readable | QDir::Hidden | QDir::NoDotAndDotDot | QDir::NoSymLinks, (isSubDir ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags));
	while (it.hasNext())
	{
		it.next();
		if (!it.fileInfo().isDir())
			list << it.fileInfo();
	}
	return list;
}

// Obtiene información del archivo
stFileInfo Funciones::getInfoFile(QString filename, TipoHash hash)
{
	stFileInfo info;

	if (filename.isEmpty())
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
		QFileInfo fi(filename);

		#ifdef Q_OS_WIN32
			info.Drive = filename.left(2) +"/";
		#else
			info.Drive = "/";
		#endif

		if (fi.absolutePath().endsWith("/"))
			info.Path = fi.absolutePath();
		else
			info.Path = fi.absolutePath().append("/");

		info.FilePath = info.Path + fi.fileName();
		info.Name     = fi.baseName();
		info.NameExt  = fi.fileName();
		info.cExt     = "."+ fi.completeSuffix().toLower();
		info.Ext      = "."+ fi.suffix().toLower();
		info.Size     = intToStr(fi.size());
		info.Exists   = fi.exists();
		info.isDir    = fi.isDir();

		if (hash == hashCrc32 || hash == hashAll)
			info.Crc32 = getHashFile(filename, hashCrc32);
		else
			info.Crc32 = "00000000";

		if (hash == hashSha1 || hash == hashAll)
			info.Sha1 = getHashFile(filename, hashSha1);
		else
			info.Sha1 = "";

		if (hash == hashMd5 || hash == hashAll)
			info.Md5 = getHashFile(filename, hashMd5);
		else
			info.Md5 = "";
	}

	return info;
}

// Obtiene el Hash del archivo
QString Funciones::getHashFile(QString filename, TipoHash hash)
{
	if (hash == hashCrc32)
	{
		Crc32Qt crc;
		return crc.getCRC32toString(filename);
	} else {
		QCryptographicHash::Algorithm tipoAlgorithm = QCryptographicHash::Sha1;
		if (hash == hashMd5)
			tipoAlgorithm = QCryptographicHash::Md5;
		else
			tipoAlgorithm = QCryptographicHash::Sha1;

		QCryptographicHash crypto(tipoAlgorithm);
		QFile file(filename);
		file.open(QFile::ReadOnly);
		while (!file.atEnd())
			crypto.addData(file.read(8192));
		file.close();

		return QString(crypto.result().toHex());
	}
}

// Devuelve un icono de montaje
QString Funciones::getIconMount(QString tipoDrive, QString select_mount)
{
	if (tipoDrive == "drive")
		return stTheme +"img16/"+ select_mount +"drive_hd.png";
	else if (tipoDrive == "cdrom")
		return stTheme +"img16/"+ select_mount +"drive_cdrom.png";
	else if (tipoDrive == "floppy")
		return stTheme +"img16/"+ select_mount +"drive_floppy.png";
	else if (tipoDrive == "IMG_floppy")
		return stTheme +"img16/"+ select_mount +"floppy_1.png";
	else if (tipoDrive == "IMG_multi_floppy")
		return stTheme +"img16/"+ select_mount +"floppy_1.png";
	else if (tipoDrive == "IMG_iso")
		return stTheme +"img16/"+ select_mount +"cd_iso.png";
	else if (tipoDrive == "IMG_multi_iso")
		return stTheme +"img16/"+ select_mount +"cd_multi_iso.png";
	else if (tipoDrive == "IMG_hdd")
		return stTheme +"img16/"+ select_mount +"drive_hd.png";
	else if (tipoDrive == "boot")
		return stTheme +"img16/"+ select_mount +"floppy_2.png";
	else
		return stTheme +"img16/sinimg.png";
}

// Crear Thumbs
// format  = BMP, JPG, JPEG, PNG
// quality = -1 = defecto, 0 a 100 peor a mejor calidad
bool Funciones::crearThumbs(QString origen, QString destino, int width, int height, int quality, bool ignoreAspectRatio, QString format)
{
	QImage img_src(origen);

	if (img_src.width() > width || img_src.height() > height)
		img_src = img_src.scaled(QSize(width, height), (ignoreAspectRatio ? Qt::IgnoreAspectRatio : Qt::KeepAspectRatio), Qt::SmoothTransformation);

	QImage img_final(QSize(img_src.width(), img_src.height()), QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&img_final);

//	painter.fillRect(img_final.rect(), (strcmp(format, "PNG") ? Qt::transparent : Qt::white));
	if (format.toUpper() == "PNG")
		painter.fillRect(img_final.rect(), Qt::transparent);
	else
		painter.fillRect(img_final.rect(), Qt::white);

	painter.drawImage(img_src.rect(), img_src);
	painter.end();

	return img_final.save(destino, format.toUpper().toStdString().c_str(), quality);
}

// Leer archivo de texto
QString Funciones::leerArchivo(QString filename, const QByteArray code)
{
	QString lineas;
	QFile file_in(filename);
	if (file_in.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream in(&file_in);
		if (!code.isEmpty())
			in.setCodec(QTextCodec::codecForName(code));
		lineas = in.readAll();
	} else
		lineas = "";
	file_in.close();

	return lineas;
}

// Guarda archivo de texto
bool Funciones::guardarArchivo(QString filename, QString txt_out, const QByteArray code)
{
	bool isOk = false;
	QFile file_out(filename);
	if (file_out.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream out(&file_out);
		if (!code.isEmpty())
			out.setCodec(QTextCodec::codecForName(code));
	//	out.setCodec("ISO 8859-1"); // ANSI
		out << txt_out << endl;
		out.flush();
		isOk = true;
	}
	file_out.close();

	return isOk;
}

// Copia o mueve un archivo de un directorio a otro
bool Funciones::opArchivo(QString origen, QString destino, bool mover, bool permisos, bool sobrescribir, bool preguntar)
{
	bool siguiente = false;
	bool isOk = false;

	QFile f_name(destino);
	if (f_name.exists())
	{
		if (sobrescribir)
		{
			f_name.remove();
			siguiente = true;
		} else {
			if (preguntar)
			{
				if (questionMsg("¿Sobrescribir?", "¿El archivo '"+ f_name.fileName() +"' ya existe quieres sobrescribirlo?"))
				{
					f_name.remove();
					siguiente = true;
				} else
					siguiente = false;
			} else
				siguiente = false;
		}
	} else
		siguiente = true;

	if (siguiente)
	{
		if (mover)
			isOk = f_name.rename(origen, destino);
		else
			isOk = f_name.copy(origen, destino);

		if (permisos)
			f_name.setPermissions(QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther |
								  QFile::WriteOwner | QFile::WriteUser | QFile::WriteGroup | QFile::WriteOther);
	} else
		isOk = false;


	f_name.close();
	return isOk;
}

// Copia o mueve varios archivos de un directorio a otro
void Funciones::opArchivos(QString dir_old, QString dir_new, QString filter, QString filter_sep, bool mover, bool permisos, bool sobrescribir, bool preguntar)
{
	QFileInfoList list_files = getListFiles(dir_old, filter.split(filter_sep, QString::SkipEmptyParts));
	const int listSize = list_files.size();
	for (int i = 0; i < listSize; ++i)
	{
		opArchivo(dir_old + list_files.at(i).fileName(), dir_new + list_files.at(i).fileName(), mover, permisos, sobrescribir, preguntar);
	}

	list_files.clear();
}

// Copia un archivo de un directorio a otro
bool Funciones::copiarArchivo(QString origen, QString destino, bool permisos, bool sobrescribir, bool preguntar)
{
	return opArchivo(origen, destino, false, permisos, sobrescribir, preguntar);
}

// Copia varios archivos de un directorio a otro
void Funciones::copiarArchivos(QString dir_old, QString dir_new, QString filter, QString filter_sep, bool permisos, bool sobrescribir, bool preguntar)
{
	opArchivos(dir_old, dir_new, filter, filter_sep, false, permisos, sobrescribir, preguntar);
}

// Mueve un archivo de un directorio a otro
bool Funciones::moverArchivo(QString origen, QString destino, bool permisos, bool sobrescribir, bool preguntar)
{
	return opArchivo(origen, destino, true, permisos, sobrescribir, preguntar);
}

// Mueve varios archivos de un directorio a otro
void Funciones::moverArchivos(QString dir_old, QString dir_new, QString filter, QString filter_sep, bool permisos, bool sobrescribir, bool preguntar)
{
	opArchivos(dir_old, dir_new, filter, filter_sep, true, permisos, sobrescribir, preguntar);
}

// Elimina archivos de un directorio
bool Funciones::eliminarArchivo(QString nameFile)
{
	QFile archivo;
	bool isOk = false;

	if (archivo.exists(nameFile))
		isOk = archivo.remove(nameFile);

	return isOk;
}

void Funciones::eliminarArchivos(QFileInfoList list_files)
{
	const int listSize = list_files.size();
	if (listSize > -1)
	{
		for (int i = 0; i < listSize; ++i)
			eliminarArchivo(list_files.at(i).absoluteFilePath());
	}
}

// Elimina un directorio si existe
bool Funciones::eliminarDirectorio(QString dir)
{
	QDir directorio;
	if (directorio.exists(dir))
	{
		if (directorio.isReadable())
			return directorio.rmdir(dir);
		else
			return false;
	} else
		return false;
}

// Renombra un directorio si existe
bool Funciones::renombrarDirectorio(QString dir_old, QString dir_new)
{
	QDir directorio;
	if (directorio.exists(dir_old))
		return directorio.rename(dir_old, dir_new);
	else
		return comprobarDirectorio(dir_new);
}

// Comprueba si un directorio existe y sino lo crea
bool Funciones::comprobarDirectorio(QString dir, bool info)
{
	if (!dir.isEmpty())
	{
		QDir directorio;
		if (directorio.exists(dir))
			return true;
		else {
			if (info)
				return false;
			else
				return directorio.mkpath(dir);
		}
	} else
		return false;
}

// Comprueba si un directorio si termina o empieza con barra o si ella
bool Funciones::comprobarBarraPath(QString dir, bool inicio, bool addChkSlashIni)
{
	bool tieneBarra = false;
	QStringList list;
	list << "\\" << "/";
	if (inicio && addChkSlashIni)
		list << ".\\" << "./";

	int i = 0;
	while (i < list.size() && !tieneBarra)
	{
		if (inicio)
		{
			if (dir.endsWith(list.at(i)))
				tieneBarra = true;
			else
				i++;
		} else {
			if (dir.startsWith(list.at(i)))
				tieneBarra = true;
			else
				i++;
		}
	}
	return tieneBarra;
}

// Compara dos estructuras stGrlDatos si son iguales.
bool Funciones::compararStGrlDatos(stGrlDatos a, stGrlDatos b)
{
	bool sonIguales = true;

	if (a.titulo != b.titulo)
		sonIguales = false;
//	if (a.icono != b.icono)
//		sonIguales = false;
	if (a.key != b.key)
		sonIguales = false;
	if (a.extra != b.extra)
		sonIguales = false;
	if (a.version != b.version)
		sonIguales = false;
	if (a.issvn != b.issvn)
		sonIguales = false;

	return sonIguales;
}

// Devuelve el directorio que usa el GR-lida
QString Funciones::homePath()
{
	QDir dirGrl;
	bool isWinOrMac = false;
	stDirApp    = "";
	stTheme     = "";
	stLngSelect = "";

	if (QFile::exists(QDir::currentPath() +"/GR-lida.conf"))
		stDirApp = QDir::currentPath() +"/";
	else {
		#ifdef Q_OS_WIN32
			isWinOrMac = true;
			stOS = "win";
		#else
			#ifdef Q_OS_MAC
				isWinOrMac = true;
				stOS = "mac";
			#else
				isWinOrMac = false;
				stOS = "linux";
			#endif
		#endif

		if (isWinOrMac)
		{
			stDirApp = QDir::homePath() +"/GR-lida/";
			if (dirGrl.exists(QDir::homePath() +"/.gr-lida/"))
				dirGrl.rename(QDir::homePath() +"/.gr-lida/", QDir::homePath() +"/GR-lida/");
		} else
			stDirApp = QDir::homePath() +"/.gr-lida/";
	}

	QSettings settings(stDirApp +"GR-lida.conf", QSettings::IniFormat);
	settings.beginGroup("SqlDatabase");
		QFileInfo info_db(getDirRelative(settings.value("db_host", "./db_grl.grl").toString()));
		settings.setValue("db_host", setDirRelative(stDirApp + info_db.fileName()));
	settings.endGroup();
	settings.beginGroup("OpcGeneral");
		setTheme(settings.value("NameDirTheme", "defecto").toString());
		setIdioma(settings.value("IdiomaSelect", "es_ES").toString());
	settings.endGroup();

	return stDirApp;
}

// Rutas relativas
QString Funciones::getRelative(QString dir, QString dir_relative)
{
	int pos_path = dir.indexOf(dir_relative);
	if (pos_path > -1)
		return "./"+ dir.remove(0, pos_path);
	else
		return  dir;
}

QString Funciones::setDirRelative(QString dir, QString dir_relative)
{
	dir.replace("\\", "/");
	dir_relative.replace("\\", "/");

	QString dir_base_game = "";

	if (dir_relative == "DosGames")
	{
		QSettings settings(stDirApp +"GR-lida.conf", QSettings::IniFormat);
		settings.beginGroup("OpcGeneral");
			dir_base_game = settings.value("DirBaseGames", "./DosGames/").toString().isEmpty() ? "./DosGames/" : settings.value("DirBaseGames").toString().replace("\\", "/");
		settings.endGroup();
	}

	if (!dir_relative.isEmpty())
	{
		if (!dir_relative.endsWith("/"))
			dir_relative.append("/");
	}

	if (dir_base_game.isEmpty())
	{
		if (dir_relative.startsWith("./"))
			dir_base_game = dir_relative;
		else
			dir_base_game = "./"+ dir_relative;
	}

	if (dir.startsWith("{DirBaseGames}", Qt::CaseInsensitive))
		dir.replace("{DirBaseGames}", dir_base_game);

	QString dir_final = dir;
	if (dir.startsWith(stDirApp, Qt::CaseInsensitive))
	{
		dir.replace(0, stDirApp.length(), "");
		if (dir.startsWith("/"))
			dir_final = dir.prepend(".");
		else
			dir_final = dir.prepend("./");
	}

	return dir_final;
}

QString Funciones::getDirRelative(QString dir, QString dir_relative)
{
	dir.replace("\\", "/");
	dir_relative.replace("\\", "/");

	QString dir_base_game = "";

	if (dir_relative == "DosGames")
	{
		QSettings settings(stDirApp +"GR-lida.conf", QSettings::IniFormat);
		settings.beginGroup("OpcGeneral");
			dir_base_game = settings.value("DirBaseGames", "./DosGames/").toString().isEmpty() ? "./DosGames/" : settings.value("DirBaseGames").toString().replace("\\", "/");
		settings.endGroup();
	}

	if (!dir_relative.isEmpty())
	{
		if (!dir_relative.endsWith("/"))
			dir_relative.append("/");
	}

	if (dir_base_game.isEmpty())
	{
		if (dir_relative.startsWith("./"))
			dir_base_game = dir_relative;
		else
			dir_base_game = "./"+ dir_relative;
	}

	if (dir_base_game.startsWith("./"))
		dir_base_game = stDirApp;
	else {
		if (dir.startsWith("{DirBaseGames}", Qt::CaseInsensitive))
			dir.replace("{DirBaseGames}", dir_base_game);

		if (dir.startsWith("./DosGames/"))
			dir.replace(0, 11, "./");
	}

	QString dir_final = dir;
	if (dir.startsWith("./"))
		dir_final = dir.replace(0, 2, dir_base_game);

	return dir_final;
}

// Devuelve el directorio del Theme a usar
void Funciones::setTheme(QString theme)
{
	QDir themeDir;
	if (theme.toLower() == "defecto" || theme.isEmpty())
		stTheme = stDirApp +"themes/defecto/";
	else
		stTheme = stDirApp +"themes/"+ theme +"/";

	if (!themeDir.exists(stTheme))
		stTheme = ":/";
}

// Devuelve el directorio del Idioma a usar
void Funciones::setIdioma(QString lng)
{
	if (lng.isEmpty())
		stLngSelect = "es_ES";
	else {
		QStringList lngs;
		lngs << "da_DK" << "en_EN" << "es_ES" << "fr_FR" << "hu_HU" << "ru_RU";
		stLngSelect = lngs.contains(lng) ? lng : "es_ES";
	}
}

// Carga la hoja de estilo y reemplaza el comodin <theme> por la ruta del theme que se use
QString Funciones::myStyleSheet(QString file_style)
{
	QString archivo = leerArchivo(stTheme + file_style, "UTF-8");
	archivo.replace("<theme>", stTheme);
//	archivo.replace("<bgimagen>", stTheme);
	return archivo +"\n";
}

QString Funciones::tplInfoJuego(QString file_tpl)
{
	QString tpl = "";
	if (QFile::exists(stTheme + file_tpl +".html"))
		tpl = leerArchivo(stTheme + file_tpl +".html", "UTF-8");
	else
		tpl = leerArchivo(":/"+ file_tpl +".html", "UTF-8");
	tpl.replace("<theme>", QUrl::fromLocalFile(stTheme).toString());

	QString tpl_css = "";
	if (QFile::exists(stTheme + file_tpl +".css"))
		tpl_css = leerArchivo(stTheme + file_tpl +".css", "UTF-8");
	else
		tpl_css = leerArchivo(":/"+ file_tpl +".css", "UTF-8");
	tpl_css.replace("<theme>", QUrl::fromLocalFile(stTheme).toString());

	tpl.replace("{tpl_css}", tpl_css);

	if (file_tpl.toLower() == "tpl_info_juego")
	{
	// Reempla las etiquetas
		tpl.replace("{lb_cover_front}"     , tr("Carátula frontal")  );
		tpl.replace("{lb_detalles_juego}"  , tr("Detalles del juego"));
		tpl.replace("{lb_calificacion}"    , tr("Calificación")      );
		tpl.replace("{lb_otros_datos}"     , tr("Otros datos")       );
		tpl.replace("{lb_usuario}"         , tr("Subido por")        );
		tpl.replace("{lb_subtitulo}"       , tr("Subtítulo")         );
		tpl.replace("{lb_compania}"        , tr("Publicado por")     );
		tpl.replace("{lb_desarrollador}"   , tr("Desarrollado por")  );
		tpl.replace("{lb_anno}"            , tr("Publicado")         );
		tpl.replace("{lb_edad_recomendada}", tr("Edad recomendada")  );
		tpl.replace("{lb_idioma}"          , tr("Idioma")            );
		tpl.replace("{lb_idioma_voces}"    , tr("Idioma voces")      );
		tpl.replace("{lb_formato}"         , tr("Formato")           );
		tpl.replace("{lb_genero}"          , tr("Genero")            );
		tpl.replace("{lb_tema}"            , tr("Tema")              );
		tpl.replace("{lb_grupo}"           , tr("Grupo")             );
		tpl.replace("{lb_perspectiva}"     , tr("Perspectiva")       );
		tpl.replace("{lb_sistemaop}"       , tr("Sistema operativo") );
		tpl.replace("{lb_favorito}"        , tr("Favorito")          );
		tpl.replace("{lb_gamepad}"         , tr("Joystick") +"/"+ tr("GamePad"));
		tpl.replace("{lb_fecha}"           , tr("Añadido el")        );
		tpl.replace("{lb_graficos}"        , tr("Gráficos")          );
		tpl.replace("{lb_sonido}"          , tr("Sonido")            );
		tpl.replace("{lb_jugabilidad}"     , tr("Jugabilidad")       );
		tpl.replace("{lb_original}"        , tr("Original")          );
		tpl.replace("{lb_estado}"          , tr("Estado")            );
		tpl.replace("{lb_tipo_emu}"        , tr("Tipo Emu")          );
		tpl.replace("{lb_comentario}"      , tr("Descripción")       );
		tpl.replace("{lb_rating}"          , tr("Rating")            );
		tpl.replace("{lb_numdisc}"         , tr("Nº discos")         );
	}
	else if (file_tpl.toLower() == "tpl_info_media")
	{
		tpl.replace("{lb_titulo}"      , tr("Título")       );
		tpl.replace("{lb_album_title}" , tr("Título album") );
		tpl.replace("{lb_album_artist}", tr("Artista album"));
	} else {

	}

	return tpl;
}

// Lista los gupos para la configuración de los iconos
QStringList Funciones::listItemIconConf(QString iniFileName)
{
	QStringList list_group;
	QSettings settings(iniFileName, QSettings::IniFormat);
	const int listSize = settings.childGroups().size();
	for (int i = 0; i < listSize; ++i)
	{
		if (settings.childGroups().at(i).startsWith("item_"))
			list_group << settings.childGroups().at(i).toLower();
	}
	return list_group;
}

// Obtiene la configuración de los distintos elementos
stLwItemCfg Funciones::getDefectLwItemConf(QSettings &settings)
{
	stLwItemCfg itemCfg;
	itemCfg.img_cover_top_path        = settings.value("img_cover_top"       , "images/list_cover_top.png").toString();
	itemCfg.img_cover_top_path_select = settings.value("img_cover_top_select", "images/list_cover_top_select.png").toString();

	if (!itemCfg.img_cover_top.load(stTheme + itemCfg.img_cover_top_path))
		itemCfg.img_cover_top.load(":/images/list_cover_top.png");
	if (!itemCfg.img_cover_top_select.load(stTheme + itemCfg.img_cover_top_path_select))
		itemCfg.img_cover_top_select.load(":/images/list_cover_top_select.png");

	itemCfg.img_cover_top_pos_x  = settings.value("img_cover_top_pos_x" , 22   ).toInt();
	itemCfg.img_cover_top_pos_y  = settings.value("img_cover_top_pos_y" , 22   ).toInt();
	itemCfg.img_cover_top_zindex = settings.value("img_cover_top_zindex", false).toBool();
	itemCfg.img_scaled           = settings.value("img_scaled"          , true ).toBool();
	itemCfg.img_scale_w          = settings.value("img_scale_w"         , 145  ).toInt();
	itemCfg.img_scale_h          = settings.value("img_scale_h"         , 186  ).toInt();
	itemCfg.img_scale_pos_x      = settings.value("img_scale_pos_x"     , 28   ).toInt();
	itemCfg.img_scale_pos_y      = settings.value("img_scale_pos_y"     , 28   ).toInt();
	itemCfg.tipo_emu_show        = settings.value("tipo_emu_show"       , true ).toBool();
	itemCfg.tipo_emu_pos_x       = settings.value("tipo_emu_pos_x"      , 15   ).toInt();
	itemCfg.tipo_emu_pos_y       = settings.value("tipo_emu_pos_y"      , 202  ).toInt();
	itemCfg.rating_show          = settings.value("rating_show"         , true ).toBool();
	itemCfg.rating_vertical      = settings.value("rating_vertical"     , false).toBool();
	itemCfg.rating_pos_x         = settings.value("rating_pos_x"        , 105  ).toInt();
	itemCfg.rating_pos_y         = settings.value("rating_pos_y"        , 202  ).toInt();
	itemCfg.title_bg_show        = settings.value("title_bg_show"       , true ).toBool();
	itemCfg.title_bg_path        = settings.value("title_bg"            , "images/list_cover_title_bg.png").toString();
	itemCfg.title_bg_path_select = settings.value("title_bg_select"     , "images/list_cover_title_bg.png").toString();

	if (!itemCfg.title_bg.load(stTheme + itemCfg.title_bg_path))
		itemCfg.title_bg.load(":/images/list_cover_title_bg.png");
	if (!itemCfg.title_bg_select.load(stTheme + itemCfg.title_bg_path_select))
		itemCfg.title_bg_select.load(":/images/list_cover_title_bg_select.png");

	itemCfg.title_bg_pos_x       = settings.value("title_bg_pos_x"      , 13  ).toInt();
	itemCfg.title_bg_pos_y       = settings.value("title_bg_pos_y"      , 222 ).toInt();
	itemCfg.title_show           = settings.value("title_show"          , true).toBool();
	itemCfg.title_pos_x          = settings.value("title_pos_x"         , 21  ).toInt();
	itemCfg.title_pos_y          = settings.value("title_pos_y"         , 222 ).toInt();
	itemCfg.title_width          = settings.value("title_width"         , 156 ).toInt();
	itemCfg.title_height         = settings.value("title_height"        , 16  ).toInt();
	itemCfg.title_max_caracteres = settings.value("title_max_caracteres", 25  ).toInt();
	itemCfg.title_font           = settings.value("title_font"          , "Tahoma").toString();
	itemCfg.title_font_size      = settings.value("title_font_size"     , 7).toInt();
	itemCfg.title_font_pos       = settings.value("title_font_pos"      , 4).toInt();
	itemCfg.title_font_color.setColor(getColor(settings.value("title_font_color", QStringList() << "0" << "0" << "0").toStringList()));
	itemCfg.title_font_color_select.setColor(getColor(settings.value("title_font_color_select", QStringList() << "0" << "0" << "0").toStringList()));
	itemCfg.title_font_wordwrap  = settings.value("title_font_wordwrap" , false).toBool();
	itemCfg.title_font_bold      = settings.value("title_font_bold"     , true ).toBool();
	itemCfg.title_font_italic    = settings.value("title_font_italic"   , false).toBool();
	itemCfg.title_font_underline = settings.value("title_font_underline", false).toBool();

	return itemCfg;
}

// Carga la configuración de los iconos de la lista en modo Icono
stLwIconCfg Funciones::cargarListWidgetIconConf(QString tabla, QString theme)
{
	QString file_cfg, dir_cfg, file_lwConf;
	stLwIconCfg lwConf;

	dir_cfg = theme.isEmpty() ? stTheme : theme;
	file_cfg = (tabla != "dbgrl" ? tabla +"_list_cfg.ini" : "list_cfg.ini");

	if (QFile::exists(dir_cfg + file_cfg))
		file_lwConf = dir_cfg + file_cfg;
	else
		file_lwConf = dir_cfg +"list_cfg.ini";

	QSettings settings(file_lwConf, QSettings::IniFormat);
	settings.beginGroup("tw_list_icon");
		lwConf.tw_icon_width  = settings.value("tw_icon_width" , 32).toInt();
		lwConf.tw_icon_height = settings.value("tw_icon_height", 29).toInt();
		lwConf.tw_alternating_row_colors = settings.value("tw_alternating_row_colors", true).toBool();
	settings.endGroup();
	settings.beginGroup("picflow_img");
		lwConf.pf_img_width   = settings.value("pf_img_width"  , 145).toInt();
		lwConf.pf_img_height  = settings.value("pf_img_height" , 186).toInt();
		lwConf.pf_img_fixsize = settings.value("pf_img_fixsize", true).toBool();
	settings.endGroup();
	settings.beginGroup("list_icon");
		lwConf.icon_width  = settings.value("icon_width" , 200).toInt();
		lwConf.icon_height = settings.value("icon_height", 243).toInt();
	settings.endGroup();

	lwConf.item.clear();
	settings.beginGroup("item_datos");
		lwConf.item.insert("item_datos", getDefectLwItemConf(settings));
	settings.endGroup();
	settings.beginGroup("item_dosbox");
		lwConf.item.insert("item_dosbox", getDefectLwItemConf(settings));
	settings.endGroup();
	settings.beginGroup("item_scummvm");
		lwConf.item.insert("item_scummvm", getDefectLwItemConf(settings));
	settings.endGroup();
	settings.beginGroup("item_vdmsound");
		lwConf.item.insert("item_vdmsound", getDefectLwItemConf(settings));
	settings.endGroup();

	QList<stGrlDatos> list_emu = cargaListaDatos(stDirApp +"datos/emu_list.txt", 4, "|");
	const int listSize = list_emu.size();
	for (int i = 0; i < listSize; ++i)
	{
		settings.beginGroup("item_"+ list_emu.at(i).key);
			lwConf.item.insert("item_"+ list_emu.at(i).key, getDefectLwItemConf(settings));
		settings.endGroup();
	}

	return lwConf;
}

// Crea la configuración de los iconos de la lista en modo Icono
void Funciones::guardarListWidgetIconConf(stLwIconCfg lwConf, QString tabla, QString theme)
{
	QString file_cfg, dir_cfg, file_lwConf;

	dir_cfg     = theme.isEmpty() ? stTheme : theme;
	file_cfg    = (tabla != "dbgrl" ? tabla +"_list_cfg.ini" : "list_cfg.ini");
	file_lwConf = dir_cfg + file_cfg;

	QSettings settings(file_lwConf, QSettings::IniFormat);
	settings.clear();
	settings.beginGroup("tw_list_icon");
		settings.setValue("tw_icon_width" , lwConf.tw_icon_width );
		settings.setValue("tw_icon_height", lwConf.tw_icon_height);
		settings.setValue("tw_alternating_row_colors", lwConf.tw_alternating_row_colors);
	settings.endGroup();
	settings.beginGroup("picflow_img");
		settings.setValue("pf_img_width"  , lwConf.pf_img_width  );
		settings.setValue("pf_img_height" , lwConf.pf_img_height );
		settings.setValue("pf_img_fixsize", lwConf.pf_img_fixsize);
	settings.endGroup();
	settings.beginGroup("list_icon");
		settings.setValue("icon_width" , lwConf.icon_width );
		settings.setValue("icon_height", lwConf.icon_height);
	settings.endGroup();

	QHash<QString, stLwItemCfg>::const_iterator itemCfg = lwConf.item.constBegin();
	while (itemCfg != lwConf.item.constEnd())
	{
		settings.beginGroup(itemCfg.key());
			settings.setValue("img_cover_top"          , itemCfg.value().img_cover_top_path  );
			settings.setValue("img_cover_top_select"   , itemCfg.value().img_cover_top_path_select);
			settings.setValue("img_cover_top_pos_x"    , itemCfg.value().img_cover_top_pos_x );
			settings.setValue("img_cover_top_pos_y"    , itemCfg.value().img_cover_top_pos_y );
			settings.setValue("img_cover_top_zindex"   , itemCfg.value().img_cover_top_zindex);
			settings.setValue("img_scaled"             , itemCfg.value().img_scaled          );
			settings.setValue("img_scale_w"            , itemCfg.value().img_scale_w         );
			settings.setValue("img_scale_h"            , itemCfg.value().img_scale_h         );
			settings.setValue("img_scale_pos_x"        , itemCfg.value().img_scale_pos_x     );
			settings.setValue("img_scale_pos_y"        , itemCfg.value().img_scale_pos_y     );
			settings.setValue("tipo_emu_show"          , itemCfg.value().tipo_emu_show       );
			settings.setValue("tipo_emu_pos_x"         , itemCfg.value().tipo_emu_pos_x      );
			settings.setValue("tipo_emu_pos_y"         , itemCfg.value().tipo_emu_pos_y      );
			settings.setValue("rating_show"            , itemCfg.value().rating_show         );
			settings.setValue("rating_vertical"        , itemCfg.value().rating_vertical     );
			settings.setValue("rating_pos_x"           , itemCfg.value().rating_pos_x        );
			settings.setValue("rating_pos_y"           , itemCfg.value().rating_pos_y        );
			settings.setValue("title_bg_show"          , itemCfg.value().title_bg_show       );
			settings.setValue("title_bg"               , itemCfg.value().title_bg_path       );
			settings.setValue("title_bg_select"        , itemCfg.value().title_bg_path_select);
			settings.setValue("title_bg_pos_x"         , itemCfg.value().title_bg_pos_x      );
			settings.setValue("title_bg_pos_y"         , itemCfg.value().title_bg_pos_y      );
			settings.setValue("title_show"             , itemCfg.value().title_show          );
			settings.setValue("title_pos_x"            , itemCfg.value().title_pos_x         );
			settings.setValue("title_pos_y"            , itemCfg.value().title_pos_y         );
			settings.setValue("title_width"            , itemCfg.value().title_width         );
			settings.setValue("title_height"           , itemCfg.value().title_height        );
			settings.setValue("title_max_caracteres"   , itemCfg.value().title_max_caracteres);
			settings.setValue("title_font"             , itemCfg.value().title_font          );
			settings.setValue("title_font_size"        , itemCfg.value().title_font_size     );
			settings.setValue("title_font_pos"         , itemCfg.value().title_font_pos      );
			settings.setValue("title_font_color"       , setColor(itemCfg.value().title_font_color.color()));
			settings.setValue("title_font_color_select", setColor(itemCfg.value().title_font_color_select.color()));
			settings.setValue("title_font_wordwrap"    , itemCfg.value().title_font_wordwrap );
			settings.setValue("title_font_bold"        , itemCfg.value().title_font_bold     );
			settings.setValue("title_font_italic"      , itemCfg.value().title_font_italic   );
			settings.setValue("title_font_underline"   , itemCfg.value().title_font_underline);
		settings.endGroup();
		++itemCfg;
	}
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
		config.db_port      = settings.value("db_port"     , 3306).toInt();
		config.db_orden_col = settings.value("db_orden_col", "titulo").toString();
		config.db_orden_by  = settings.value("db_orden_by" , "titulo").toString();
		config.db_orden     = settings.value("db_orden"    , "ASC").toString();
	settings.endGroup();

	settings.beginGroup("OpcGeneral");
		config.Primeravez    = settings.value("Primeravez"   , true).toBool();
		config.dirApp        = settings.value("dirApp"       , "homepath").toString();
		config.DirDOSBox     = settings.value("DirDOSBox"    , "").toString();
		config.DirScummVM    = settings.value("DirScummVM"   , "").toString();
		config.DirBaseGames  = settings.value("DirBaseGames" , "./DosGames/").toString().isEmpty() ? "./DosGames/" : settings.value("DirBaseGames").toString();
		config.DOSBoxDefault = settings.value("DOSBoxDefault", "dosbox").toString();
		config.DOSBoxDisp    = settings.value("DOSBoxDisp"   , false).toBool();
		config.ScummVMDisp   = settings.value("ScummVMDisp"  , false).toBool();
	#ifdef Q_OS_WIN32
		config.VDMSoundDisp = settings.value("VDMSoundDisp", false).toBool();
	#else
		config.VDMSoundDisp = false;
	#endif
		config.IdiomaSelect         = settings.value("IdiomaSelect"        , "es_ES").toString();
		config.OpenPdfExternal      = settings.value("OpenPdfExternal"     , false).toBool();
		config.url_xmldb            = settings.value("url_xmldb"           , "gr-lida.js").toString();
		config.FormatoFecha         = settings.value("FormatoFecha"        , "dd/MM/yyyy HH:mm:ss").toString();
		config.ToolbarBigIcon       = settings.value("ToolbarBigIcon"      , true).toBool();
		config.Style                = settings.value("Style"               , "").toString();
		config.StylePalette         = settings.value("StylePalette"        , false).toBool();
		config.NameDirTheme         = settings.value("NameDirTheme"        , "defecto").toString();
		config.IconoFav             = settings.value("IconoFav"            , "fav_0.png").toString();
		config.IndexTabArchivos     = settings.value("IndexTabArchivos"    , 0).toInt();
		config.VersionDBx           = settings.value("VersionDBx"          , "0.74").toString();
		config.DOSBoxSVN            = settings.value("DOSBoxSVN"           , false).toBool();
		config.DOSBoxSaveConfFile   = settings.value("DOSBoxSaveConfFile"  , true).toBool();
		config.ScummVMSaveConfFile  = settings.value("ScummVMSaveConfFile" , true).toBool();
		config.LastSelectGameID     = settings.value("LastSelectGameID"    , "0").toString();
		config.LastSelectCatID      = settings.value("LastSelectCatID"     , "0").toString();
		config.LastSelectNavID      = settings.value("LastSelectNavID"     , 0).toInt();
		config.LastSelectNavSubID   = settings.value("LastSelectNavSubID"  , 0).toInt();
		config.LastTabOptSelect     = settings.value("LastTabOptSelect"    , 0).toInt();
		config.LastSelectShortCutID = settings.value("LastSelectShortCutID", 0).toInt();
	settings.endGroup();

	settings.beginGroup("OpcSaveThumbs");
		config.thumb_width       = settings.value("thumb_width"      , 145).toInt();
		config.thumb_height      = settings.value("thumb_height"     , 186).toInt();
		config.thumb_quality     = settings.value("thumb_quality"    ,  95).toInt();
		config.thumb_format      = settings.value("thumb_format"     , "JPG").toString();
		config.thumb_img_width   = settings.value("thumb_img_width"  , 128).toInt();
		config.thumb_img_height  = settings.value("thumb_img_height" , 128).toInt();
		config.thumb_img_quality = settings.value("thumb_img_quality",  95).toInt();
	settings.endGroup();

	settings.beginGroup("OpcPicFlow");
		config.PicFlowType              = settings.value("PicFlowType"            , "CoverFlowLike").toString();
		config.PicFlowBgColor           = settings.value("PicFlowBgColor"         , QStringList() << "0" << "0" << "0").toStringList();
		config.PicFlowFontFamily        = settings.value("PicFlowFontFamily"      , "Tahoma").toString();
		config.PicFlowFontSize          = settings.value("PicFlowFontSize"        , 20).toInt();
		config.PicFlowFontBold          = settings.value("PicFlowFontBold"        , false).toInt();
		config.PicFlowFontItalic        = settings.value("PicFlowFontItalic"      , false).toInt();
		config.PicFlowFontColor         = settings.value("PicFlowFontColor"       , QStringList() << "255" << "255" << "255").toStringList();
		config.PicFlowMinHeight         = settings.value("PicFlowMinHeight"       , 260).toInt();
		config.PicFlowSkip              = settings.value("PicFlowSkip"            , 10).toInt();

		config.PicFlowShowTitle         = settings.value("PicFlowShowTitle"       , true).toBool();
		config.PicFlowTitlePos          = settings.value("PicFlowTitlePos"        , 0).toInt();
		config.PicFlowTitleMargin       = settings.value("PicFlowTitleMargin"     , QPoint(36, 0)).toPoint();

		config.PicFlowShowTitleIcon     = settings.value("PicFlowShowTitleIcon"   , true).toBool();
		config.PicFlowTitleIconPos      = settings.value("PicFlowTitleIconPos"    , 0).toInt();
		config.PicFlowTitleIconMargin   = settings.value("PicFlowTitleIconMargin" , QPoint(0, 0)).toPoint();

		config.PicFlowShowCaption       = settings.value("PicFlowShowCaption"     , true).toBool();
		config.PicFlowCaptionPos        = settings.value("PicFlowCaptionPos"      , 7).toInt();
		config.PicFlowCaptionMargin     = settings.value("PicFlowCaptionMargin"   , QPoint(0, 0)).toPoint();

		config.PicFlowShowNumber        = settings.value("PicFlowShowNumber"      , true).toBool();
		config.PicFlowNumberPos         = settings.value("PicFlowNumberPos"       , 2).toInt();
		config.PicFlowNumberMargin      = settings.value("PicFlowNumberMargin"    , QPoint(0, 0)).toPoint();
		config.PicFlowNumberFormat      = settings.value("PicFlowNumberFormat"    , "%1/%2").toString();

		config.PicFlowShowIconExtra     = settings.value("PicFlowShowIconExtra"   , false).toBool();
		config.PicFlowIconExtraPos      = settings.value("PicFlowIconExtraPos"    , 2).toInt();
		config.PicFlowIconExtraMargin   = settings.value("PicFlowIconExtraMargin" , QPoint(0, 0)).toPoint();

		config.PicFlowIconExtraUse         = settings.value("PicFlowIconExtraUse"        , "favorito").toString();
		config.PicFlowIconExtraImgFavorito = settings.value("PicFlowIconExtraImgFavorito", "img32/fav_0.png").toString();
		config.PicFlowIconExtraImgOriginal = settings.value("PicFlowIconExtraImgOriginal", "img32/original.png").toString();

		config.PicFlowMargin = settings.value("PicFlowMargin", QPoint(10, 10)).toPoint();

		config.PicFlowShowTriangle = settings.value("PicFlowShowTriangle", false).toBool();
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
		config.ProxyPort     = static_cast<quint16>(settings.value("ProxyPort", 1080).toInt());
		config.ProxyLogin    = settings.value("ProxyLogin"   , false).toBool();
		config.ProxyUserName = settings.value("ProxyUserName", ""   ).toString();
		config.ProxyPassword = settings.value("ProxyPassword", ""   ).toString();
	settings.endGroup();

	settings.beginGroup("MainState");
		config.main_fullscreen       = settings.value("main_fullscreen"      , false).toBool();
		config.main_maximized        = settings.value("main_maximized"       , false).toBool();
		config.main_geometry         = settings.value("main_geometry"        , "").toByteArray();
		config.main_state            = settings.value("main_state"           , "").toByteArray();
//		config.main_twJuegos_state   = settings.value("main_twJuegos_state"  , "").toByteArray();
		config.main_tvJuegos_state   = settings.value("main_tvJuegos_state"  , "").toByteArray();
		config.import_geometry       = settings.value("import_geometry"      , "").toByteArray();
		config.import_splitter_state = settings.value("import_splitter_state", "").toByteArray();
	settings.endGroup();

	settings.beginGroup("OpcVer");
		config.Pnl_AutoHideTitleBars            = settings.value("Pnl_AutoHideTitleBars"           , true ).toBool();
		config.Pnl_Nav_VisibleTitleBars         = settings.value("Pnl_Nav_VisibleTitleBars"        , true ).toBool();
		config.Pnl_Datos_VisibleTitleBars       = settings.value("Pnl_Datos_VisibleTitleBars"      , true ).toBool();
		config.Pnl_Files_VisibleTitleBars       = settings.value("Pnl_Files_VisibleTitleBars"      , true ).toBool();
		config.Pnl_Urls_VisibleTitleBars        = settings.value("Pnl_Urls_VisibleTitleBars"       , true ).toBool();
		config.Pnl_Videos_VisibleTitleBars      = settings.value("Pnl_Videos_VisibleTitleBars"     , true ).toBool();
		config.Pnl_Sonidos_VisibleTitleBars     = settings.value("Pnl_Sonidos_VisibleTitleBars"    , true ).toBool();
		config.Pnl_Capturas_VisibleTitleBars    = settings.value("Pnl_Capturas_VisibleTitleBars"   , true ).toBool();
		config.Pnl_Imagenes_VisibleTitleBars    = settings.value("Pnl_Imagenes_VisibleTitleBars"   , true ).toBool();
		config.Pnl_PictureFlow_VisibleTitleBars = settings.value("Pnl_PictureFlow_VisibleTitleBars", false).toBool();
		config.Pnl_Nav_IgnoreAutoHide           = settings.value("Pnl_Nav_IgnoreAutoHide"          , true ).toBool();
		config.Pnl_Datos_IgnoreAutoHide         = settings.value("Pnl_Datos_IgnoreAutoHide"        , false).toBool();
		config.Pnl_Files_IgnoreAutoHide         = settings.value("Pnl_Files_IgnoreAutoHide"        , false).toBool();
		config.Pnl_Urls_IgnoreAutoHide          = settings.value("Pnl_Urls_IgnoreAutoHide"         , false).toBool();
		config.Pnl_Videos_IgnoreAutoHide        = settings.value("Pnl_Videos_IgnoreAutoHide"       , false).toBool();
		config.Pnl_Sonidos_IgnoreAutoHide       = settings.value("Pnl_Sonidos_IgnoreAutoHide"      , false).toBool();
		config.Pnl_Capturas_IgnoreAutoHide      = settings.value("Pnl_Capturas_IgnoreAutoHide"     , false).toBool();
		config.Pnl_Imagenes_IgnoreAutoHide      = settings.value("Pnl_Imagenes_IgnoreAutoHide"     , false).toBool();
		config.Pnl_PictureFlow_IgnoreAutoHide   = settings.value("Pnl_PictureFlow_IgnoreAutoHide"  , true ).toBool();
		config.Pnl_Nav                          = settings.value("Pnl_Nav"                         , false).toBool();
		config.Pnl_Datos                        = settings.value("Pnl_Datos"                       , true ).toBool();
		config.Pnl_Files                        = settings.value("Pnl_Files"                       , false).toBool();
		config.Pnl_Urls                         = settings.value("Pnl_Urls"                        , false).toBool();
		config.Pnl_Videos                       = settings.value("Pnl_Videos"                      , false).toBool();
		config.Pnl_Sonidos                      = settings.value("Pnl_Sonidos"                     , false).toBool();
		config.Pnl_Capturas                     = settings.value("Pnl_Capturas"                    , false).toBool();
		config.Pnl_Imagenes                     = settings.value("Pnl_Imagenes"                    , false).toBool();
		config.Pnl_PictureFlow                  = settings.value("Pnl_PictureFlow"                 , true ).toBool();
		config.PictureFlowToCenter              = settings.value("PictureFlowToCenter"             , false).toBool();
		config.Pnl_Menubar                      = settings.value("Pnl_Menubar"                     , true ).toBool();
		config.Pnl_Toolbar                      = settings.value("Pnl_Toolbar"                     , true ).toBool();
		config.Pnl_ToolbarDatos                 = settings.value("Pnl_ToolbarDatos"                , true ).toBool();
		config.Pnl_Ordenar                      = settings.value("Pnl_Ordenar"                     , false).toBool();
		config.Pnl_ShortCut                     = settings.value("Pnl_ShortCut"                    , false).toBool();
		config.Pnl_StatusBar                    = settings.value("Pnl_StatusBar"                   , true ).toBool();
		config.ver_IconMode                     = settings.value("ver_IconMode"                    , true ).toBool();
		config.ver_col_Icono                    = settings.value("ver_col_Icono"                   , true ).toBool();
		config.ver_col_Subtitulo                = settings.value("ver_col_Subtitulo"               , false).toBool();
		config.ver_col_Genero                   = settings.value("ver_col_Genero"                  , false).toBool();
		config.ver_col_Compania                 = settings.value("ver_col_Compania"                , true ).toBool();
		config.ver_col_Desarrollador            = settings.value("ver_col_Desarrollador"           , false).toBool();
		config.ver_col_Tema                     = settings.value("ver_col_Tema"                    , false).toBool();
		config.ver_col_Grupo                    = settings.value("ver_col_Grupo"                   , false).toBool();
		config.ver_col_Perspectiva              = settings.value("ver_col_Perspectiva"             , false).toBool();
		config.ver_col_Idioma                   = settings.value("ver_col_Idioma"                  , false).toBool();
		config.ver_col_IdiomaVoces              = settings.value("ver_col_IdiomaVoces"             , false).toBool();
		config.ver_col_Formato                  = settings.value("ver_col_Formato"                 , false).toBool();
		config.ver_col_Anno                     = settings.value("ver_col_Anno"                    , true ).toBool();
		config.ver_col_NumDisc                  = settings.value("ver_col_NumDisc"                 , false).toBool();
		config.ver_col_SistemaOp                = settings.value("ver_col_SistemaOp"               , false).toBool();
		config.ver_col_Tamano                   = settings.value("ver_col_Tamano"                  , false).toBool();
		config.ver_col_Graficos                 = settings.value("ver_col_Graficos"                , false).toBool();
		config.ver_col_Sonido                   = settings.value("ver_col_Sonido"                  , false).toBool();
		config.ver_col_Jugabilidad              = settings.value("ver_col_Jugabilidad"             , false).toBool();
		config.ver_col_Original                 = settings.value("ver_col_Original"                , false).toBool();
		config.ver_col_Estado                   = settings.value("ver_col_Estado"                  , false).toBool();
		config.ver_col_Fecha                    = settings.value("ver_col_Fecha"                   , false).toBool();
		config.ver_col_TipoEmu                  = settings.value("ver_col_TipoEmu"                 , false).toBool();
		config.ver_col_Favorito                 = settings.value("ver_col_Favorito"                , false).toBool();
		config.ver_col_Rating                   = settings.value("ver_col_Rating"                  , true ).toBool();
		config.ver_col_Edad                     = settings.value("ver_col_Edad"                    , false).toBool();
		config.ver_col_Usuario                  = settings.value("ver_col_Usuario"                 , false).toBool();
	settings.endGroup();

	settings.beginGroup("ImageViewerState");
		config.img_maximized        = settings.value("img_maximized"       , false).toBool();
		config.img_fitToWindow      = settings.value("img_fitToWindow"     , false).toBool();
		config.img_verListaImagenes = settings.value("img_verListaImagenes", false).toBool();
	settings.endGroup();

	settings.beginGroup("OpcMultimedia");
		config.FormatsVideo         = settings.value("FormatsVideo", QStringList() << "*.avi" << "*.mkv" << "*.mov" << "*.mp4" << "*.mpeg" << "*.mpg" << "*.wmv").toStringList();
		config.FormatsMusic         = settings.value("FormatsMusic", QStringList() << "*.mp3" << "*.ogg" << "*.wav" << "*.wma" << "*.m4a").toStringList();
		config.FormatsImage         = settings.value("FormatsImage", QStringList() << "*.bmp" << "*.gif" << "*.jpg" << "*.jpeg" << "*.png" << "*.pbm" << "*.pgm" << "*.ppm" << "*.tiff" << "*.xbm" << "*.xpm" << "*.svg").toStringList();
		config.VerPlaylistVideo     = settings.value("VerPlaylistVideo"    , false).toBool();
		config.VerPlaylistVideoMain = settings.value("VerPlaylistVideoMain", false).toBool();
		config.VerPlaylistSound     = settings.value("VerPlaylistSound"    , false).toBool();
		config.VerPlaylistSoundMain = settings.value("VerPlaylistSoundMain", false).toBool();
		config.VerInfoMedia         = settings.value("VerInfoMedia"        , true ).toBool();
		config.VerInfoMediaMain     = settings.value("VerInfoMediaMain"    , true ).toBool();
	settings.endGroup();

	settings.beginGroup("UltimoDirectorio");
		config.DirBD         = settings.value("DirBD"        , "").toString();
		config.DirImportar   = settings.value("DirImportar"  , "").toString();
		config.DirExportPath = settings.value("DirExportPath", "").toString();
		config.DirOtherEmus  = settings.value("DirOtherEmus" , "").toString();
	// Datos de Juegos
		config.Img_Thumbs              = settings.value("Img_Thumbs"             , "").toString();
		config.Img_CoverFront          = settings.value("Img_CoverFront"         , "").toString();
		config.Img_CoverBack           = settings.value("Img_CoverBack"          , "").toString();
		config.Img_CoverLeft           = settings.value("Img_CoverLeft"          , "").toString();
		config.Img_CoverRight          = settings.value("Img_CoverRight"         , "").toString();
		config.Img_CoverTop            = settings.value("Img_CoverTop"           , "").toString();
		config.Img_CoverBottom         = settings.value("Img_CoverBottom"        , "").toString();
		config.Img_Imagenes            = settings.value("Img_Imagenes"           , "").toString();
		config.Img_Capturas            = settings.value("Img_Capturas"           , "").toString();
		config.Datos_Videos            = settings.value("Datos_Videos"           , "").toString();
		config.Datos_Sonidos           = settings.value("Datos_Sonidos"          , "").toString();
		config.DatosFiles_PathFile     = settings.value("DatosFiles_PathFile"    , "").toString();
		config.DatosFiles_PathExe      = settings.value("DatosFiles_PathExe"     , "").toString();
		config.DatosFiles_PathSetup    = settings.value("DatosFiles_PathSetup"   , "").toString();
		config.DatosFiles_PathCapturas = settings.value("DatosFiles_PathCapturas", "").toString();
	// DOSBox
		config.Dbx_path            = settings.value("Dbx_path"           , "").toString();
		config.Dbx_path_exe        = settings.value("Dbx_path_exe"       , "").toString();
		config.Dbx_path_setup      = settings.value("Dbx_path_setup"     , "").toString();
		config.Dbx_gus_ultradir    = settings.value("Dbx_gus_ultradir"   , "").toString();
		config.Dbx_sdl_mapperfile  = settings.value("Dbx_sdl_mapperfile" , "").toString();
		config.Dbx_dosbox_language = settings.value("Dbx_dosbox_language", "").toString();
		config.Montaje_path        = settings.value("Montaje_path"       , "").toString();
	// ScummVM
		config.Svm_path       = settings.value("Svm_path"      , "").toString();
		config.Svm_path_save  = settings.value("Svm_path_save" , "").toString();
		config.Svm_path_extra = settings.value("Svm_path_extra", "").toString();
		config.Svm_soundfont  = settings.value("Svm_soundfont" , "").toString();
	// VDMSound
		config.Vdms_path_exe = settings.value("Vdms_path_exe", "").toString();
		config.Vdms_icon     = settings.value("Vdms_icon"    , "").toString();
	settings.endGroup();

	settings.beginGroup("Updates");
		config.Version     = settings.value("Version"    , versionGrl()).toString();
		config.VerListSvm  = settings.value("VerListSvm" , versionSvm()).toString();
		config.chkVersion  = settings.value("chkVersion" , false).toBool();
		config.chkUpdateThemes   = settings.value("chkUpdateThemes", false).toBool();
		config.chkUpdateInterval = settings.value("chkUpdateInterval", 7).toInt();
		config.lastChkUpdate     = settings.value("lastChkUpdate", getTime()).toInt();
	settings.endGroup();

	config.isChangedToolbarBigIcon = false;
	config.isChangedTheme          = false;
	config.isChangedFormatoFecha   = false;
	config.isChangedCategoria      = false;
	config.isChangedEmuList        = false;
	config.isChangedIdioma         = false;
	config.isChangedListDOSBox     = false;
	config.isChangedShortcut       = false;
	config.isChangedFavorito       = false;

	return config;
}

// Guarda la configuración del GR-lida
void Funciones::guardarGRLConfig(QString iniFileName, stGrlCfg config)
{
	QSettings settings(iniFileName, QSettings::IniFormat);
	settings.clear();

	settings.beginGroup("SqlDatabase");
		settings.setValue("db_type"     , config.db_type     );
		settings.setValue("db_server"   , config.db_server   );
		settings.setValue("db_host"     , config.db_host     );
		settings.setValue("db_name"     , config.db_name     );
		settings.setValue("db_username" , config.db_username );
		settings.setValue("db_password" , config.db_password );
		settings.setValue("db_port"     , config.db_port     );
		settings.setValue("db_orden_col", config.db_orden_col);
		settings.setValue("db_orden_by" , config.db_orden_by );
		settings.setValue("db_orden"    , config.db_orden    );
	settings.endGroup();

	settings.beginGroup("OpcGeneral");
		settings.setValue("Primeravez"          , config.Primeravez          );
		settings.setValue("dirApp"              , config.dirApp              );
		settings.setValue("DirDOSBox"           , config.DirDOSBox           );
		settings.setValue("DirScummVM"          , config.DirScummVM          );
		settings.setValue("DirBaseGames"        , config.DirBaseGames        );
		settings.setValue("DOSBoxDefault"       , config.DOSBoxDefault       );
		settings.setValue("DOSBoxDisp"          , config.DOSBoxDisp          );
		settings.setValue("ScummVMDisp"         , config.ScummVMDisp         );
		settings.setValue("VDMSoundDisp"        , config.VDMSoundDisp        );
		settings.setValue("IdiomaSelect"        , config.IdiomaSelect        );
		settings.setValue("OpenPdfExternal"     , config.OpenPdfExternal     );
		settings.setValue("url_xmldb"           , config.url_xmldb           );
		settings.setValue("FormatoFecha"        , config.FormatoFecha        );
		settings.setValue("ToolbarBigIcon"      , config.ToolbarBigIcon      );
		settings.setValue("Style"               , config.Style               );
		settings.setValue("StylePalette"        , config.StylePalette        );
		settings.setValue("NameDirTheme"        , config.NameDirTheme        );
		settings.setValue("IconoFav"            , config.IconoFav            );
		settings.setValue("IndexTabArchivos"    , config.IndexTabArchivos    );
		settings.setValue("VersionDBx"          , config.VersionDBx          );
		settings.setValue("DOSBoxSVN"           , config.DOSBoxSVN           );
		settings.setValue("DOSBoxSaveConfFile"  , config.DOSBoxSaveConfFile  );
		settings.setValue("ScummVMSaveConfFile" , config.ScummVMSaveConfFile );
		settings.setValue("LastSelectGameID"    , config.LastSelectGameID    );
		settings.setValue("LastSelectCatID"     , config.LastSelectCatID     );
		settings.setValue("LastSelectNavID"     , config.LastSelectNavID     );
		settings.setValue("LastSelectNavSubID"  , config.LastSelectNavSubID  );
		settings.setValue("LastTabOptSelect"    , config.LastTabOptSelect    );
		settings.setValue("LastSelectShortCutID", config.LastSelectShortCutID);
	settings.endGroup();

	settings.beginGroup("OpcSaveThumbs");
		settings.setValue("thumb_width"      , config.thumb_width      );
		settings.setValue("thumb_height"     , config.thumb_height     );
		settings.setValue("thumb_quality"    , config.thumb_quality    );
		settings.setValue("thumb_format"     , config.thumb_format     );
		settings.setValue("thumb_img_width"  , config.thumb_img_width  );
		settings.setValue("thumb_img_height" , config.thumb_img_height );
		settings.setValue("thumb_img_quality", config.thumb_img_quality);
	settings.endGroup();

	settings.beginGroup("OpcPicFlow");
		settings.setValue("PicFlowType"            , config.PicFlowType            );
		settings.setValue("PicFlowBgColor"         , config.PicFlowBgColor         );
		settings.setValue("PicFlowFontFamily"      , config.PicFlowFontFamily      );
		settings.setValue("PicFlowFontSize"        , config.PicFlowFontSize        );
		settings.setValue("PicFlowFontBold"        , config.PicFlowFontBold        );
		settings.setValue("PicFlowFontItalic"      , config.PicFlowFontItalic      );
		settings.setValue("PicFlowFontColor"       , config.PicFlowFontColor       );
		settings.setValue("PicFlowMinHeight"       , config.PicFlowMinHeight       );
		settings.setValue("PicFlowSkip"            , config.PicFlowSkip            );

		settings.setValue("PicFlowShowTitle"       , config.PicFlowShowTitle       );
		settings.setValue("PicFlowTitlePos"        , config.PicFlowTitlePos        );
		settings.setValue("PicFlowTitleMargin"     , config.PicFlowTitleMargin     );

		settings.setValue("PicFlowShowTitleIcon"   , config.PicFlowShowTitleIcon   );
		settings.setValue("PicFlowTitleIconPos"    , config.PicFlowTitleIconPos    );
		settings.setValue("PicFlowTitleIconMargin" , config.PicFlowTitleIconMargin );

		settings.setValue("PicFlowShowCaption"     , config.PicFlowShowCaption     );
		settings.setValue("PicFlowCaptionPos"      , config.PicFlowCaptionPos      );
		settings.setValue("PicFlowCaptionMargin"   , config.PicFlowCaptionMargin   );

		settings.setValue("PicFlowShowNumber"      , config.PicFlowShowNumber      );
		settings.setValue("PicFlowNumberPos"       , config.PicFlowNumberPos       );
		settings.setValue("PicFlowNumberMargin"    , config.PicFlowNumberMargin    );
		settings.setValue("PicFlowNumberFormat"    , config.PicFlowNumberFormat    );

		settings.setValue("PicFlowShowIconExtra"   , config.PicFlowShowIconExtra   );
		settings.setValue("PicFlowIconExtraPos"    , config.PicFlowIconExtraPos    );
		settings.setValue("PicFlowIconExtraMargin" , config.PicFlowIconExtraMargin );

		settings.setValue("PicFlowIconExtraUse"        , config.PicFlowIconExtraUse        );
		settings.setValue("PicFlowIconExtraImgFavorito", config.PicFlowIconExtraImgFavorito);
		settings.setValue("PicFlowIconExtraImgOriginal", config.PicFlowIconExtraImgOriginal);

		settings.setValue("PicFlowMargin", config.PicFlowMargin);

		settings.setValue("PicFlowShowTriangle", config.PicFlowShowTriangle);
	settings.endGroup();

	settings.beginGroup("OpcFuente");
		settings.setValue("font_usar"  , config.font_usar  );
		settings.setValue("font_family", config.font_family);
		settings.setValue("font_size"  , config.font_size  );
	settings.endGroup();

	settings.beginGroup("HttpProxy");
		settings.setValue("ProxyEnable"  , config.ProxyEnable  );
		settings.setValue("ProxyType"    , config.ProxyType    );
		settings.setValue("ProxyHost"    , config.ProxyHost    );
		settings.setValue("ProxyPort"    , config.ProxyPort    );
		settings.setValue("ProxyLogin"   , config.ProxyLogin   );
		settings.setValue("ProxyUserName", config.ProxyUserName);
		settings.setValue("ProxyPassword", config.ProxyPassword);
	settings.endGroup();

	settings.beginGroup("MainState");
		settings.setValue("main_fullscreen"      , config.main_fullscreen      );
		settings.setValue("main_maximized"       , config.main_maximized       );
		settings.setValue("main_geometry"        , config.main_geometry        );
		settings.setValue("main_state"           , config.main_state           );
		settings.setValue("main_tvJuegos_state"  , config.main_tvJuegos_state  );
		settings.setValue("import_geometry"      , config.import_geometry      );
		settings.setValue("import_splitter_state", config.import_splitter_state);
	settings.endGroup();

	settings.beginGroup("OpcVer");
		settings.setValue("Pnl_AutoHideTitleBars"           , config.Pnl_AutoHideTitleBars           );
		settings.setValue("Pnl_Nav_VisibleTitleBars"        , config.Pnl_Nav_VisibleTitleBars        );
		settings.setValue("Pnl_Datos_VisibleTitleBars"      , config.Pnl_Datos_VisibleTitleBars      );
		settings.setValue("Pnl_Files_VisibleTitleBars"      , config.Pnl_Files_VisibleTitleBars      );
		settings.setValue("Pnl_Urls_VisibleTitleBars"       , config.Pnl_Urls_VisibleTitleBars       );
		settings.setValue("Pnl_Videos_VisibleTitleBars"     , config.Pnl_Videos_VisibleTitleBars     );
		settings.setValue("Pnl_Sonidos_VisibleTitleBars"    , config.Pnl_Sonidos_VisibleTitleBars    );
		settings.setValue("Pnl_Capturas_VisibleTitleBars"   , config.Pnl_Capturas_VisibleTitleBars   );
		settings.setValue("Pnl_Imagenes_VisibleTitleBars"   , config.Pnl_Imagenes_VisibleTitleBars   );
		settings.setValue("Pnl_PictureFlow_VisibleTitleBars", config.Pnl_PictureFlow_VisibleTitleBars);
		settings.setValue("Pnl_Nav_IgnoreAutoHide"          , config.Pnl_Nav_IgnoreAutoHide          );
		settings.setValue("Pnl_Datos_IgnoreAutoHide"        , config.Pnl_Datos_IgnoreAutoHide        );
		settings.setValue("Pnl_Files_IgnoreAutoHide"        , config.Pnl_Files_IgnoreAutoHide        );
		settings.setValue("Pnl_Urls_IgnoreAutoHide"         , config.Pnl_Urls_IgnoreAutoHide         );
		settings.setValue("Pnl_Videos_IgnoreAutoHide"       , config.Pnl_Videos_IgnoreAutoHide       );
		settings.setValue("Pnl_Sonidos_IgnoreAutoHide"      , config.Pnl_Sonidos_IgnoreAutoHide      );
		settings.setValue("Pnl_Capturas_IgnoreAutoHide"     , config.Pnl_Capturas_IgnoreAutoHide     );
		settings.setValue("Pnl_Imagenes_IgnoreAutoHide"     , config.Pnl_Imagenes_IgnoreAutoHide     );
		settings.setValue("Pnl_PictureFlow_IgnoreAutoHide"  , config.Pnl_PictureFlow_IgnoreAutoHide  );
		settings.setValue("Pnl_Nav"                         , config.Pnl_Nav                         );
		settings.setValue("Pnl_Datos"                       , config.Pnl_Datos                       );
		settings.setValue("Pnl_Files"                       , config.Pnl_Files                       );
		settings.setValue("Pnl_Urls"                        , config.Pnl_Urls                        );
		settings.setValue("Pnl_Videos"                      , config.Pnl_Videos                      );
		settings.setValue("Pnl_Sonidos"                     , config.Pnl_Sonidos                     );
		settings.setValue("Pnl_Capturas"                    , config.Pnl_Capturas                    );
		settings.setValue("Pnl_Imagenes"                    , config.Pnl_Imagenes                    );
		settings.setValue("Pnl_PictureFlow"                 , config.Pnl_PictureFlow                 );
		settings.setValue("PictureFlowToCenter"             , config.PictureFlowToCenter             );
		settings.setValue("Pnl_Menubar"                     , config.Pnl_Menubar                     );
		settings.setValue("Pnl_Toolbar"                     , config.Pnl_Toolbar                     );
		settings.setValue("Pnl_ToolbarDatos"                , config.Pnl_ToolbarDatos                );
		settings.setValue("Pnl_Ordenar"                     , config.Pnl_Ordenar                     );
		settings.setValue("Pnl_ShortCut"                    , config.Pnl_ShortCut                    );
		settings.setValue("Pnl_StatusBar"                   , config.Pnl_StatusBar                   );
		settings.setValue("ver_IconMode"                    , config.ver_IconMode                    );
		settings.setValue("ver_col_Icono"                   , config.ver_col_Icono                   );
		settings.setValue("ver_col_Subtitulo"               , config.ver_col_Subtitulo               );
		settings.setValue("ver_col_Genero"                  , config.ver_col_Genero                  );
		settings.setValue("ver_col_Compania"                , config.ver_col_Compania                );
		settings.setValue("ver_col_Desarrollador"           , config.ver_col_Desarrollador           );
		settings.setValue("ver_col_Tema"                    , config.ver_col_Tema                    );
		settings.setValue("ver_col_Grupo"                   , config.ver_col_Grupo                   );
		settings.setValue("ver_col_Perspectiva"             , config.ver_col_Perspectiva             );
		settings.setValue("ver_col_Idioma"                  , config.ver_col_Idioma                  );
		settings.setValue("ver_col_IdiomaVoces"             , config.ver_col_IdiomaVoces             );
		settings.setValue("ver_col_Formato"                 , config.ver_col_Formato                 );
		settings.setValue("ver_col_Anno"                    , config.ver_col_Anno                    );
		settings.setValue("ver_col_NumDisc"                 , config.ver_col_NumDisc                 );
		settings.setValue("ver_col_SistemaOp"               , config.ver_col_SistemaOp               );
		settings.setValue("ver_col_Tamano"                  , config.ver_col_Tamano                  );
		settings.setValue("ver_col_Graficos"                , config.ver_col_Graficos                );
		settings.setValue("ver_col_Sonido"                  , config.ver_col_Sonido                  );
		settings.setValue("ver_col_Jugabilidad"             , config.ver_col_Jugabilidad             );
		settings.setValue("ver_col_Original"                , config.ver_col_Original                );
		settings.setValue("ver_col_Estado"                  , config.ver_col_Estado                  );
		settings.setValue("ver_col_Fecha"                   , config.ver_col_Fecha                   );
		settings.setValue("ver_col_TipoEmu"                 , config.ver_col_TipoEmu                 );
		settings.setValue("ver_col_Favorito"                , config.ver_col_Favorito                );
		settings.setValue("ver_col_Rating"                  , config.ver_col_Rating                  );
		settings.setValue("ver_col_Edad"                    , config.ver_col_Edad                    );
		settings.setValue("ver_col_Usuario"                 , config.ver_col_Usuario                 );
	settings.endGroup();

	settings.beginGroup("ImageViewerState");
		settings.setValue("img_maximized"       , config.img_maximized       );
		settings.setValue("img_fitToWindow"     , config.img_fitToWindow     );
		settings.setValue("img_verListaImagenes", config.img_verListaImagenes);
	settings.endGroup();

	settings.beginGroup("OpcMultimedia");
		settings.setValue("FormatsVideo"        , config.FormatsVideo        );
		settings.setValue("FormatsMusic"        , config.FormatsMusic        );
		settings.setValue("FormatsImage"        , config.FormatsImage        );
		settings.setValue("VerPlaylistVideo"    , config.VerPlaylistVideo    );
		settings.setValue("VerPlaylistVideoMain", config.VerPlaylistVideoMain);
		settings.setValue("VerPlaylistSound"    , config.VerPlaylistSound    );
		settings.setValue("VerPlaylistSoundMain", config.VerPlaylistSoundMain);
		settings.setValue("VerInfoMedia"        , config.VerInfoMedia        );
		settings.setValue("VerInfoMediaMain"    , config.VerInfoMediaMain    );
	settings.endGroup();

	settings.beginGroup("UltimoDirectorio");
		settings.setValue("DirBD"        , config.DirBD        );
		settings.setValue("DirImportar"  , config.DirImportar  );
		settings.setValue("DirExportPath", config.DirExportPath);
		settings.setValue("DirOtherEmus" , config.DirOtherEmus );
	// Datos de Juegos
		settings.setValue("Img_Thumbs"             , config.Img_Thumbs             );
		settings.setValue("Img_CoverFront"         , config.Img_CoverFront         );
		settings.setValue("Img_CoverBack"          , config.Img_CoverBack          );
		settings.setValue("Img_CoverLeft"          , config.Img_CoverLeft          );
		settings.setValue("Img_CoverRight"         , config.Img_CoverRight         );
		settings.setValue("Img_CoverTop"           , config.Img_CoverTop           );
		settings.setValue("Img_CoverBottom"        , config.Img_CoverBottom        );
		settings.setValue("Img_Imagenes"           , config.Img_Imagenes           );
		settings.setValue("Img_Capturas"           , config.Img_Capturas           );
		settings.setValue("Datos_Videos"           , config.Datos_Videos           );
		settings.setValue("Datos_Sonidos"          , config.Datos_Sonidos          );
		settings.setValue("DatosFiles_PathFile"    , config.DatosFiles_PathFile    );
		settings.setValue("DatosFiles_PathExe"     , config.DatosFiles_PathExe     );
		settings.setValue("DatosFiles_PathSetup"   , config.DatosFiles_PathSetup   );
		settings.setValue("DatosFiles_PathCapturas", config.DatosFiles_PathCapturas);
	// DOSBox
		settings.setValue("Dbx_path"           , config.Dbx_path           );
		settings.setValue("Dbx_path_exe"       , config.Dbx_path_exe       );
		settings.setValue("Dbx_path_setup"     , config.Dbx_path_setup     );
		settings.setValue("Dbx_gus_ultradir"   , config.Dbx_gus_ultradir   );
		settings.setValue("Dbx_sdl_mapperfile" , config.Dbx_sdl_mapperfile );
		settings.setValue("Dbx_dosbox_language", config.Dbx_dosbox_language);
		settings.setValue("Montaje_path"       , config.Montaje_path       );
	// ScummVM
		settings.setValue("Svm_path"      , config.Svm_path      );
		settings.setValue("Svm_path_save" , config.Svm_path_save );
		settings.setValue("Svm_path_extra", config.Svm_path_extra);
		settings.setValue("Svm_soundfont" , config.Svm_soundfont );
	// VDMSound
		settings.setValue("Vdms_path_exe", config.Vdms_path_exe);
		settings.setValue("Vdms_icon"    , config.Vdms_icon    );
	settings.endGroup();

	settings.beginGroup("Updates");
		settings.setValue("Version"          , config.Version          );
		settings.setValue("VerListSvm"       , config.VerListSvm       );
		settings.setValue("chkVersion"       , config.chkVersion       );
		settings.setValue("chkUpdateThemes"  , config.chkUpdateThemes  );
		settings.setValue("chkUpdateInterval", config.chkUpdateInterval);
		settings.setValue("lastChkUpdate"    , config.lastChkUpdate    );
	settings.endGroup();
}

// Obtiene la dirección y el nombre del archivo atraves de QFileDialog
QString Funciones::ventanaAbrirArchivos(QWidget *parent, QString caption, QString dir, QString dir_relative, QString filter, QString *selectedFilter, bool isSave)
{
	QString base = getDirRelative(dir, dir_relative);

	if (!comprobarDirectorio(base, true))
		base = stDirApp;

	if (isSave)
		return QFileDialog::getSaveFileName(parent, caption, base, filter, selectedFilter);
	else
		return QFileDialog::getOpenFileName(parent, caption, base, filter, selectedFilter);
}

// Obtiene la dirección de una carpeta atraves de QFileDialog
QString Funciones::ventanaDirectorios(QWidget *parent, QString caption, QString dir, QString dir_relative)
{
	QString directorio = "";
	QString base       = getDirRelative(dir, dir_relative);

	if (!comprobarDirectorio(base, true))
		base = stDirApp;

	directorio = QFileDialog::getExistingDirectory(parent, caption, base, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if (!directorio.isEmpty() && comprobarDirectorio(directorio, true))
	{
		if (directorio.endsWith("/"))
			return directorio;
		else
			return directorio +"/";
	} else
		return "";
}

// Abre un archivo con el programa predeterminado
bool Funciones::abrirArchivo(QString filePath)
{
	return QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

// Abre un directorio
bool Funciones::abrirDirectorio(QString dir)
{
	return abrirArchivo(dir);
}

// Elimina un item de un QTreeWidget
void Funciones::deleteItemTree(QTreeWidgetItem *item)
{
	if (!item)
		return;
	int count = item->childCount();
	for (int i = count - 1; i >= 0; --i)
		deleteItemTree(item->child(i));

	delete item;
}

// Mueve hacia arriba una posición de un QTreeWidget
void Funciones::moveUpItemTw(QTreeWidget *twList)
{
	int current = twList->indexOfTopLevelItem(twList->currentItem());
	int next_pos = -1;
	if (current > 0 && current != -1)
	{
		next_pos = current - 1;

		QTreeWidgetItem *item = twList->topLevelItem(current);
		item = twList->takeTopLevelItem(current);
		twList->insertTopLevelItem(next_pos, item);
		twList->setCurrentItem(item);
	}
}

// Mueve hacia abajo una posición de un QTreeWidget
void Funciones::moveDownItemTw(QTreeWidget *twList)
{
	int current = twList->indexOfTopLevelItem(twList->currentItem());
	int next_pos = -1;
	if (current < (twList->topLevelItemCount() - 1) && current != -1)
	{
		next_pos = current + 1;

		QTreeWidgetItem *item = twList->topLevelItem(current);
		item = twList->takeTopLevelItem(current);
		twList->insertTopLevelItem(next_pos, item);
		twList->setCurrentItem(item);
	}
}

// Marca o desmarca todos los item de un QTreeWidget
void Funciones::checkStateAllItemTw(QTreeWidget *twList, int col, bool checked)
{
	const int count = twList->topLevelItemCount();
	if (count > 0)
	{
		for (int i = 0; i < count; ++i)
		{
			QTreeWidgetItem *item = twList->topLevelItem(i);
			if (checked)
				item->setCheckState(col, Qt::Checked);
			else
				item->setCheckState(col, Qt::Unchecked);
		}
	}
}

// Comprueba si existe la fila o subitem de la fila de un QTreeWidget
bool Funciones::comprobarItemTwList(QTreeWidget *twList, int row, int sub_row)
{
	if (twList->topLevelItemCount() > 0)
	{
		if (sub_row == -1)
		{
			if (twList->topLevelItem(row) != NULL)
			{
				twList->setCurrentItem(twList->topLevelItem(row));
				return true;
			} else
				comprobarItemTwList(twList, 0, -1);
		} else {
			if (twList->topLevelItem(row)->child(sub_row) != NULL)
			{
				twList->setCurrentItem(twList->topLevelItem(row)->child(sub_row));
				return true;
			} else
				comprobarItemTwList(twList, row, -1);
		}
	}
	return false;
}

// Activa o desactiva un boton asociado si existe un cambio en el item seleccionado del QTreeWidget
void Funciones::enabledButtonUpdateTwList(QTreeWidget *twList, QPushButton *btn, QString texto, int col)
{
	int pos = twList->indexOfTopLevelItem(twList->currentItem());
	if (twList->topLevelItemCount() > 0 && pos != -1)
	{
		if (texto != twList->currentItem()->text(col))
			btn->setEnabled(true);
		else
			btn->setEnabled(false);
	} else
		btn->setEnabled(false);
}


// Carga la lista de archivos en un QComboBox
void Funciones::cargarListaArchivosComboBox(TipoListCbx tipo, QComboBox *cbx, QString dir, QString dirImg, QString subDir, QString select_data, QString filter, QString filter_sep, bool isClear, QString contiene)
{
	stGrlDatos dato;
	QLocale locale;
	QStringList parts_lng;
	QString js_titulo, js_version, dirFiles = "";
	QScriptEngine engine;

	if (isClear)
		cbx->clear();

	switch (tipo)
	{
		case CbxListIcon:
			dirFiles = dir + dirImg;
		break;
		case CbxListLng:
			dirFiles = dir;
		break;
		case CbxListProf:
			dirFiles = dir;

			cbx->addItem(QIcon(stTheme + dirImg +"archivo_config.png"), tr("Configuración por defecto"), "_defecto_");
		break;
		case CbxListJs:
			dirFiles = dir;

			cbx->addItem(QIcon(stTheme + dirImg +"archivo_config.png"), tr("Importar desde un archivo") +": GR-lida XML, D-Fend Reloaded prof", "_desde_archivo_");
			cbx->addItem(QIcon(stTheme + dirImg +"DBGL.png")          , tr("Importar desde un archivo") +": DOSBox Game Launcher (DBGL)"      , "_desde_dbgl_"   );
		break;
	}

	QFileInfoList list_files = getListFiles(dirFiles, filter.split(filter_sep, QString::SkipEmptyParts));

	const int listSize = list_files.size();
	for (int i = 0; i < listSize; ++i)
	{
		switch (tipo)
		{
			case CbxListIcon:
				dato.icono  = list_files.at(i).absoluteFilePath();
				dato.titulo = subDir + list_files.at(i).completeBaseName();
				dato.extra  = "";
				dato.key    = subDir + list_files.at(i).fileName();
			break;
			case CbxListLng:
				parts_lng.clear();
				parts_lng = list_files.at(i).baseName().split("_");
				locale    = parts_lng.at(1) +"_"+ parts_lng.at(2);

				dato.icono = stTheme + dirImg +"sinimg.png";
				if (QFile::exists(stTheme + dirImg + parts_lng.at(2).toLower() +".png"))
					dato.icono = stTheme + dirImg + parts_lng.at(2).toLower() +".png";

				dato.titulo = locale.languageToString(locale.language()) +" ("+ locale.countryToString(locale.country()) +")"+ (dir == ":/idiomas/" ? "" : " "+ tr("exterior"));
				dato.extra  = "";
				dato.key    = list_files.at(i).absoluteFilePath();
			break;
			case CbxListProf:
				dato.icono  = stTheme + dirImg +"archivo_config.png";
				dato.titulo = list_files.at(i).completeBaseName();
				dato.extra  = "";
				dato.key    = list_files.at(i).fileName();
			break;
			case CbxListJs:
				engine.evaluate(leerArchivo(list_files.at(i).absoluteFilePath(), "UTF-8"));

				js_titulo  = getValueScript(engine, "title", list_files.at(i).baseName());
				js_version = getValueScript(engine, "version", "0");

				dato.icono  = stTheme + dirImg +"script.png";
				dato.titulo = js_titulo +" v"+ js_version;
				dato.extra  = js_version;
				dato.key    = list_files.at(i).fileName();

			break;
		}

		if (contiene.isEmpty() || dato.key.contains(contiene, Qt::CaseInsensitive))
			cbx->addItem(QIcon(dato.icono), dato.titulo, dato.key);
	}

	if (!select_data.isEmpty())
	{
		int row = cbx->findData(select_data, Qt::UserRole, (tipo == CbxListLng ? Qt::MatchContains : Qt::MatchExactly));
		cbx->setCurrentIndex(row);
	}
}

// Carga la lista de Idiomas en un QComboBox
void Funciones::cargarIdiomasComboBox(QComboBox *cbx, QString dirLng, QString dirImg, QString select_data, bool isClear)
{
	cargarListaArchivosComboBox(CbxListLng, cbx, dirLng, dirImg, "", select_data, "*.qm", ";", isClear);
}

// Carga la lista de Scripts en un QComboBox
void Funciones::cargarScriptsComboBox(QComboBox *cbx, QString dirScripts, QString dirImg, QString select_data)
{
	cargarListaArchivosComboBox(CbxListJs, cbx, dirScripts, dirImg, "", select_data, "*.js", ";");
}

// Carga la lista de Profiles en un QComboBox
void Funciones::cargarProfilesComboBox(QComboBox *cbx, QString dirProfiles, QString dirImg, QString select_data)
{
	cargarListaArchivosComboBox(CbxListProf, cbx, dirProfiles, dirImg, "", select_data, "*.conf;*.prof", ";");
}

// Carga Iconos e Imágenes soportadas en un QComboBox de un directorio
void Funciones::cargarIconosComboBox(QComboBox *cbx, QString dirIcon, QString dirImg, QString subDir, QString select_data, QString filter, bool isClear, QString contiene)
{
	cargarListaArchivosComboBox(CbxListIcon, cbx, dirIcon, dirImg, subDir, select_data, filter, ";", isClear, contiene);
}

void Funciones::cargarIconosGroupComboBox(QGroupComboBox *cbx, QString dirIcon, QString dirImg, QString subDir, QString filter, QString contiene)
{
	stGrlDatos dato;
	QFileInfoList list_files = getListFiles(dirIcon + dirImg, filter.split(";", QString::SkipEmptyParts));
	const int listSize = list_files.size();
	for (int i = 0; i < listSize; ++i)
	{
		dato.icono  = list_files.at(i).absoluteFilePath();
		dato.titulo = subDir + list_files.at(i).completeBaseName();
		dato.extra  = "";
		dato.key    = subDir + list_files.at(i).fileName();

		if (contiene.isEmpty() || dato.key.contains(contiene, Qt::CaseInsensitive))
			cbx->addItemChild(QIcon(dato.icono), dato.titulo, dato.key);
	}
}

// Carga los distintos archivos de la ruta indicada en un QListWidget
void Funciones::cargarArchivosLwLista(QListWidget *lwList, QString dir, QString filter, QString filter_sep, QString img, bool isClear, bool isThumbs, bool isSmall)
{
	QString archivo, thumbs;
	QFileInfoList list_files = getListFiles(dir, filter.split(filter_sep, QString::SkipEmptyParts));

	const int listSize = list_files.size();
	if (isClear)
		lwList->clear();

	if (listSize > -1)
	{
		for (int i = 0; i < listSize; ++i)
		{
			if (dir.endsWith("/"))
				archivo = dir + list_files.at(i).fileName();
			else
				archivo = dir +"/"+ list_files.at(i).fileName();

			QListWidgetItem *item = new QListWidgetItem;
			item->setData(Qt::UserRole, archivo);
			if (isThumbs)
			{
				QString _small = isSmall ? "small/" : "";
				QString ext_extra = isSmall ? ".jpg" : "";

				if (dir.endsWith("/"))
					thumbs = dir + _small + list_files.at(i).fileName() + ext_extra;
				else
					thumbs = dir +"/"+ _small + list_files.at(i).fileName() + ext_extra;

				if (QFile::exists(thumbs))
					item->setIcon(QIcon(thumbs));
				else
					item->setIcon(QIcon(stTheme +"images/juego_sin_imagen.png"));
			} else {
				if (QFile::exists(stTheme +"img24/"+ img))
					item->setIcon(QIcon(stTheme +"img24/"+ img));
				else
					item->setIcon(QIcon(stTheme +"img24/tag.png"));
				item->setText(list_files.at(i).fileName());
			}
			lwList->addItem(item);
		}
	}
}

// Carga la lista de datos de un archivo a un QTreeWidget
void Funciones::cargarDatosTwLista(QTreeWidget *twList, QString archivo, TipoListTw tipo, QString sep, bool isLng)
{
	QFile file_in(archivo);
	QStringList lista;
	QString linea, titulo, dato, extra, img, dir_img;
	int col_titulo, col_dato, col_nada;
	QFont m_font;
	m_font.setBold(true);
//	col_titulo = 0;
//	col_dato   = 1;
//	col_nada   = 2;

	if (!file_in.exists())
		file_in.setFileName(archivo.replace(stDirApp, ":/"));

//	twList->clear();
	if (file_in.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTreeWidgetItem *item;
		QList<QTreeWidgetItem *> items;
		QTextStream in(&file_in);
		in.setCodec("UTF-8");
		while (!in.atEnd())
		{
			linea = in.readLine();
			if (linea.isEmpty())
			{
				if (tipo == TwListSvm)
				{
					item = new QTreeWidgetItem;
					item->setIcon(0, QIcon(stTheme +"img_svm/level/sinimg.png"));
					item->setIcon(1, QIcon(stTheme +"img_svm/sinimg.png"));
					item->setText(0, "");
					item->setText(1, "");
					item->setText(2, "");
					item->setText(3, "");
					item->setText(4, "");

					items << item;
				}
			} else {
				lista = linea.split(sep);

				col_titulo = 0;
				col_dato   = 1;
				col_nada   = 2;
				dir_img    = stTheme +"img16/";

				item = new QTreeWidgetItem;
				switch (tipo)
				{
					case TwListDbx:
					case TwListSvm:
					case TwListEmu:
						titulo = lista.at(0);
						dato   = lista.at(1);
						extra  = lista.at(2);
						img    = lista.at(3);

						if (tipo == TwListDbx || tipo == TwListEmu)
						{
							if (!isLng)
								dir_img = stTheme +"img32/";
						}

						if (tipo == TwListSvm)
						{
							col_titulo = 1;
							col_dato   = 2;
							col_nada   = 0;
							dir_img    = stTheme +"img_svm/";

							if (dato.isEmpty())
							{
								item->setFont(1, m_font);
								item->setTextColor(1, QColor(0,0,0));
							}

							if (extra.isEmpty())
								item->setIcon(0, QIcon(stTheme +"img_svm/level/sinimg.png"));
							else
								item->setIcon(0, QIcon(stTheme +"img_svm/level/"+ extra +".png"));
						}
					break;
					case TwListSmile:
					case TwList2col:
						titulo = lista.at(0);
						dato   = "";
						extra  = "";
						img    = lista.at(1);

						if (tipo == TwListSmile)
							dir_img = stDirApp +"smiles/";
					break;
					case TwListTraduccion:
					case TwList3col:
						titulo = lista.at(0);
						dato   = lista.at(1);
						extra  = "";
						img    = lista.at(2);

						if (tipo == TwListTraduccion)
						{
							titulo  = lista.at(1);
							dato    = lista.at(0);

							item->setFont(0, m_font);
							item->setText(0, lista.at(0));
						}
					break;
				}

				if (img.isEmpty())
					img = "sinimg.png";

				item->setIcon(col_titulo, QIcon(dir_img +"sinimg.png"));
				if (QFile::exists(dir_img + img))
					item->setIcon(col_titulo, QIcon(dir_img + img));

				if (tipo == TwListDbx)
				{
					item->setTextAlignment(2, Qt::AlignCenter);
					item->setTextAlignment(3, Qt::AlignCenter);
					item->setText(0, titulo     );
					item->setText(1, dato       );
					item->setText(2, lista.at(4));
					item->setText(3, lista.at(5));
					item->setText(4, extra      );
					item->setText(5, img        );
				} else {
					item->setText(col_nada  , ""    );
					item->setText(col_titulo, titulo);
					item->setText(col_dato  , dato  );
					item->setText(3         , extra );
					item->setText(4         , img   );
				}

				items << item;
			}
		}
		twList->addTopLevelItems(items);
	}
	file_in.close();
}

// Guarda la lista de datos de un QTreeWidget a un archivo
void Funciones::guardarDatosTwLista(QTreeWidget *twList, QString archivo, TipoListTw tipo, QString sep)
{
	QFile file_out;
	file_out.setFileName(archivo);

	comprobarDirectorio(getInfoFile(archivo).Path);

	if (file_out.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream out(&file_out);
		out.setCodec("UTF-8");
		if (twList->topLevelItemCount() > 0)
		{
			const int listSize = twList->topLevelItemCount();
			for (int i = 0; i < listSize; ++i)
			{
				QTreeWidgetItem *item = twList->topLevelItem(i);
				switch (tipo)
				{
					case TwListSvm:
						if (item->text(1).isEmpty() && item->text(2).isEmpty())
							out << endl;
						else
							out << item->text(1) << sep << item->text(2) << sep << item->text(3) << sep << item->text(4) << endl;
					break;
					case TwListEmu:
						out << item->text(0) << sep << item->text(1) << sep << item->text(3) << sep << item->text(4) << endl;
					break;
					case TwListDbx:
						out << item->text(0) << sep << item->text(1) << sep << item->text(4) << sep << item->text(5) << sep << item->text(2) << sep << item->text(3) << endl;
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

// Carga la lista de datos
QList<stGrlDatos> Funciones::cargaListaDatos(QString archivo, int num_col, QString sep)
{
	QList<stGrlDatos> dato_list;
	stGrlDatos dato;
	QFile file_in(archivo);
	QStringList lista;
	QString linea;
	int key_col = 1;
	int img_col = 3;

	if (!file_in.exists())
		file_in.setFileName(archivo.replace(stDirApp, ":/"));

	switch (num_col)
	{
		case 2:
			key_col = 0;
			img_col = 1;
		break;
		case 3:
			img_col = 2;
		break;
	}

	dato_list.clear();
	if (file_in.open(QIODevice::ReadOnly) != 0)
	{
		QTextStream in(&file_in);
		in.setCodec("UTF-8");
		while (!in.atEnd())
		{
			linea = in.readLine();
			if (!linea.isEmpty())
			{
				lista = linea.split(sep);
					dato.titulo  = lista.at(0);
					dato.icono   = lista.at(img_col);
					dato.key     = lista.at(key_col);
					dato.extra   = (num_col == 4 || num_col == 6) ? lista.at(2) : "";
					dato.version = (num_col == 6) ? lista.at(4) : "";
					dato.issvn   = (num_col == 6) ? lista.at(5) : "";
				dato_list << dato;
			}
		}
		linea.clear();
	}

	return dato_list;
}

// Carga la lista de datos en un QHash
QHash<QString, stGrlDatos> Funciones::cargaDatosQHash(QString archivo, int num_col, QString sep)
{
	QList<stGrlDatos> list = cargaListaDatos(archivo, num_col, sep);
	QHash<QString, stGrlDatos> dato_list;
	const int listSize = list.size();
	for (int i = 0; i < listSize; ++i)
	{
		dato_list.insert(list.at(i).key, list.at(i));
	}

	return dato_list;
}

// Carga la lista de datos de un archivo a un QComboBox
void Funciones::cargarDatosComboBox(QComboBox *cbx, QString archivo, QString dirImg, QString select_data, int num_col, QString sep, bool isClear, bool keyVisible)
{
	QList<stGrlDatos> list = cargaListaDatos(archivo, num_col, sep);
	QString key_visible;

	if (isClear)
		cbx->clear();

	const int listSize = list.size();
	for (int i = 0; i < listSize; ++i)
	{
		key_visible = keyVisible ? "("+ list.at(i).key +") - " : "";

/*		QString icono = "sinimg.png";
		if (!list.at(i).icono.isEmpty())
		{
			if (QFile::exists(stTheme + dirImg + list.at(i).icono))
				icono = list.at(i).icono;
		}*/

		cbx->addItem(QIcon(stTheme + dirImg + list.at(i).icono), key_visible + list.at(i).titulo, list.at(i).key);
	}

	if (!select_data.isEmpty())
	{
		int row = cbx->findData(select_data, Qt::UserRole, Qt::MatchExactly);
		cbx->setCurrentIndex(row);
	}
}

// Carga la lista de datos de un archivo a un QCheckComboBox
void Funciones::cargarDatosCheckComboBox(QCheckComboBox *chk_cbx, QString archivo, QString dirImg, QString select_data, int num_col, QString sep, bool isClear)
{
	QList<stGrlDatos> list = cargaListaDatos(archivo, num_col, sep);

	if (isClear)
		chk_cbx->clear();

	const int listSize = list.size();
	for (int i = 0; i < listSize; ++i)
		chk_cbx->addItem(QIcon(stTheme + dirImg + list.at(i).icono), list.at(i).titulo, list.at(i).key);

	if (!select_data.isEmpty())
	{
		int row = chk_cbx->findData(select_data, Qt::UserRole, Qt::MatchExactly);
		chk_cbx->setCurrentIndex(row);
	}
}

// Remplaza texto por las imágenes de los emoticones
QString Funciones::reemplazaTextoSmiles(QString str, QHash<QString, stGrlDatos> lista)
{
	foreach (const stGrlDatos &smile, lista)
	{
		//QRegExp rx_smile(QRegExp::escape(smile.key) +"\\s");
		str.replace(smile.key, "<img src=\"smile_rs_"+ smile.key +"_"+ smile.icono +"\" alt=\""+ smile.key +"\" title=\""+ smile.key +"\"/> ");
	}
	return str;
}

// Devuelve imágenes para la info del juego
QString Funciones::getImgDatos(QHash<QString, stGrlDatos> datos, QStringList lista, bool isLng)
{
	QString img_lng = "";
	const int listSize = lista.size();
	for (int i=0; i < listSize; ++i)
	{
		if (!lista.at(i).isEmpty())
		{
			foreach (const stGrlDatos &dat, datos)
			{
				if (dat.titulo == lista.at(i) || dat.extra == lista.at(i))
				{
					if (isLng)
						img_lng.append("<img src=\"idioma_rs_"+ dat.icono +"\" alt=\""+ dat.titulo +"\" title=\""+ dat.titulo +"\"> "+ dat.titulo +" ");
					else
						img_lng.append("<img src=\""+ stTheme +"img16/"+ dat.icono +"\" alt=\""+ dat.titulo +"\" title=\""+ dat.titulo +"\"> "+ dat.titulo +" ");
				}
			}
		}
	}

	return img_lng;
}

void Funciones::insertaIconos(QHash<QString, QPixmap> &ico, QString dir, QString dir_img, QString prefix, QString filter, QString contiene)
{
	QPixmap img_ico;
	QFileInfoList list_files = getListFiles(dir + dir_img, filter.split(";", QString::SkipEmptyParts));
	const int listSize = list_files.size();
	for (int i = 0; i < listSize; ++i)
	{
		if (contiene.isEmpty() || list_files.at(i).fileName().contains(contiene, Qt::CaseInsensitive))
		{
			if (!img_ico.load(list_files.at(i).absoluteFilePath()))
				img_ico.load(":/"+ dir_img +"/sinimg.png");
			ico.insert(prefix + list_files.at(i).fileName(), img_ico);
		}
	}
}

/*
 * INICIO MONTAJES DE UNIDADES ----------------------------------------------------------------------------------
 * - Funcion para Poner nombres cortos en DOS.
 * - Estado: Beta v 0.0.4
 *
*/
QString Funciones::getNameTo8Caracter(QString name, int index)
{
	if (name.isEmpty())
		return "";
	else {
		QString final, str, ext;
		if (name.contains("."))
		{
			int last_dot = name.lastIndexOf(".");
			str = name.left(last_dot);
			ext = name.remove(0, last_dot);
			if (ext.size() > 4)
				ext = name.left(4);
		} else {
			ext = "";
			str = name;
		}

		int idx = intToStr(index).length()-1;

		// Contiene espacios
		if (str.contains(" "))
			final = str.replace(" ","").replace(".","").left(6-idx).append("~"+ intToStr(index)).append(ext).toUpper();
		else {
		// No contiene espacios
		// Si es mayor que 8 se covierte.
			if (str.length() > 8)
				final = str.left(6-idx).append("~"+ intToStr(index)).append(ext).toUpper();
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
	if (!longPath.isEmpty())
	{
		int index, idx_dos, listSize = 0;
		QString f_name, f_path, sep;
		QStringList listShortPath, shortPath, path_tmp, list_dir, list_dir_8names;
//#ifdef Q_OS_WIN32
//	sep = "\\";
//#else
	sep = "/";
//#endif
		QFileInfo fi(longPath.replace("\\", sep));
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
			QDir dir(str_end(path_tmp.join(sep), sep));
			list_dir = dir.entryList(QDir::NoDotAndDotDot | QDir::AllDirs);
			list_dir_8names.clear();
			idx_dos = 1;
			for (int n = 0; n < list_dir.size(); ++n)
			{
				if (n > 0)
				{
					if (getNameTo8Caracter(list_dir.at(n), idx_dos) == getNameTo8Caracter(list_dir.at(n-1), idx_dos))
						++idx_dos;
					else
						idx_dos = 1;
				}
				list_dir_8names << getNameTo8Caracter(list_dir.at(n), idx_dos);
			}
			index = list_dir.indexOf(listShortPath.at(i));
			if (index > -1)
				shortPath << list_dir_8names.at(index);

			path_tmp << listShortPath.at(i);
		}

		if (!f_name.isEmpty())
		{
			QDir dir_exe(f_path);
			list_dir = dir_exe.entryList(QDir::NoDotAndDotDot | QDir::Files);
			list_dir_8names.clear();
			idx_dos = 1;
			for (int n = 0; n < list_dir.size(); ++n)
			{
				if (n > 0)
				{
					if (getNameTo8Caracter(list_dir.at(n), idx_dos) == getNameTo8Caracter(list_dir.at(n-1), idx_dos))
						++idx_dos;
					else
						idx_dos = 1;
				}
				list_dir_8names << getNameTo8Caracter(list_dir.at(n), idx_dos);
			}
			index = list_dir.indexOf(f_name);
			if (index > -1)
				shortPath << list_dir_8names.at(index);
		}

//		return shortPath.join("\\");
		return shortPath.join("/");
	} else
		return "";
}

QList<stConfigDOSBoxMount> Funciones::getListCfgMount(QTreeWidget *twDbxMount)
{
	QList<stConfigDOSBoxMount> listMount;
	const int count_mount = twDbxMount->topLevelItemCount();
	if (count_mount > 0)
	{
		stConfigDOSBoxMount cfgMount;
		for (int num_mount = 0; num_mount < count_mount; ++num_mount)
		{
			QTreeWidgetItem *item = twDbxMount->topLevelItem(num_mount);
				cfgMount.id           = item->text(8);
				cfgMount.id_dosbox    = "";
				cfgMount.id_lista     = item->text(9);
				cfgMount.path         = item->text(0);
				cfgMount.label        = item->text(3);
				cfgMount.tipo_as      = item->text(2);
				cfgMount.letter       = item->text(1);
				cfgMount.indx_cd      = item->text(4);
				cfgMount.opt_mount    = item->text(5);
				cfgMount.io_ctrl      = item->text(6);
				cfgMount.select_mount = item->text(7);
				cfgMount.opt_size     = item->text(10);
				cfgMount.opt_freesize = item->text(11);
				cfgMount.freesize     = item->text(12);
			listMount.append(cfgMount);
		}
	}
	return listMount;
}

// Crea la configuración de los Montajes para el DOSBox
QStringList Funciones::creaConfigMontajes(QList<stConfigDOSBoxMount> listMount, stConfigDOSBox cfgDbx, bool isSetup)
{
	QString NombreEXEDbx, DirEXEDbx, Dbx_loadfix, Dbx_cerrardbx;
	QString mount_letra_primario, mount_dir_primario, mount_dir, montaje_boot, mount_freesize;
	QString mount_type, mount_drive, mount_letter, mount_label, mount_options, mount_ioctl;
	bool montaje_IMG = false;
	bool mount_Boot = false;
	QStringList listmontajes, lista_multiple_iso, lista_isos;
	const int listMountSize = listMount.size();

	QFileInfo fi(getDirRelative((isSetup ? cfgDbx.path_setup : cfgDbx.path_exe), "DosGames"));
	NombreEXEDbx = fi.fileName();
	DirEXEDbx    = fi.absolutePath();

	if (!DirEXEDbx.endsWith("/"))
		DirEXEDbx.append("/");

// loadfix
	if (strToBool(cfgDbx.opt_loadfix))
		Dbx_loadfix = "loadfix -"+ cfgDbx.opt_loadfix_mem +" ";
	else {
		if (fi.completeSuffix().toLower() == "bat")
			Dbx_loadfix = "call ";
		else
			Dbx_loadfix = "";
	}

// Cerrar DOSBox
	if (strToBool(cfgDbx.opt_cerrar_dbox))
		Dbx_cerrardbx = "exit";
	else
		Dbx_cerrardbx = "";

// Montajes
	listmontajes.clear();
	if (listMountSize > 0)
	{
		mount_dir_primario   = getDirRelative(listMount.at(0).path, "DosGames");
		mount_letra_primario = listMount.at(0).letter;

		for (int m = 0; m < listMountSize; ++m)
		{
		// Indicamos el directorio y la letra a montar
			mount_drive  = getDirRelative(listMount.at(m).path, "DosGames"); // Real Drive or Directory or Image ISO, IMA
			mount_letter = listMount.at(m).letter; // Emulated Drive letter

			if ((listMount.at(m).tipo_as == "floppy") || (listMount.at(m).tipo_as == "drive") || (listMount.at(m).tipo_as == "cdrom"))
			{
				if (!mount_drive.endsWith("/"))
					mount_drive.append("/");
			}

		// Situa el montaje primario independiente de donde este colocado
			if (listMount.at(m).select_mount == "v")
			{
				mount_dir_primario   = mount_drive;
				mount_letra_primario = mount_letter;
			}

		#ifdef Q_OS_WIN32
			mount_drive.replace("/","\\");
		#endif

			mount_label    = "";
			mount_freesize = "";
			mount_ioctl    = "";
			mount_options  = "";

		// Montando las unidades
		// Disco Duro, Montando Disquetera, CDs, Imágenes de Discos, disquetes y CDs
			if (listMount.at(m).tipo_as != "boot")
			{
				mount_Boot = false;

				if (listMount.at(m).tipo_as == "drive")
				{
					montaje_IMG = false;
					mount_type = "";

					if (strToBool(listMount.at(m).opt_freesize))
					{
						if (listMount.at(m).opt_size != "")
							mount_freesize = " -size "+ listMount.at(m).opt_size;
						else
							mount_freesize = " -freesize "+ listMount.at(m).freesize;
					}
				}

				if (listMount.at(m).tipo_as == "cdrom")
				{
					montaje_IMG  = false;
					mount_type  = " -t cdrom";
				// usecd
					if (listMount.at(m).indx_cd != "")
						mount_freesize = " -usecd "+ listMount.at(m).indx_cd;
				// ioctl
					if (listMount.at(m).io_ctrl != "")
						mount_ioctl = " "+ listMount.at(m).io_ctrl;
				// etiqueta
					if (listMount.at(m).label != "")
						mount_label = " -label "+ listMount.at(m).label;
				}

				if (listMount.at(m).tipo_as == "floppy")
				{
					montaje_IMG = false;
					mount_type = " -t floppy";

					if (strToBool(listMount.at(m).opt_freesize))
					{
						if (listMount.at(m).opt_size != "")
							mount_freesize = " -size "+ listMount.at(m).opt_size;
						else
							mount_freesize = " -freesize "+ listMount.at(m).freesize;
					}
				}

				if (listMount.at(m).tipo_as == "IMG_floppy")
				{
					montaje_IMG = true;
					mount_type = " -t floppy";

					if (listMount.at(m).opt_size != "")
						mount_freesize = " -size "+ listMount.at(m).opt_size;

					if (listMount.at(m).opt_mount != "")
						mount_options = " "+ listMount.at(m).opt_mount;
				}

				if (listMount.at(m).tipo_as == "IMG_iso")
				{
					montaje_IMG = true;
					mount_type = " -t cdrom";

					if (listMount.at(m).opt_mount != "")
						mount_options = " "+ listMount.at(m).opt_mount;
				}

				if (listMount.at(m).tipo_as == "IMG_multi_iso" || listMount.at(m).tipo_as == "IMG_multi_floppy")
				{
					montaje_IMG = true;
					if (listMount.at(m).tipo_as == "IMG_multi_floppy")
						mount_type = " -t floppy";
					else
						mount_type = " -t cdrom";

					lista_isos.clear();
					lista_isos << mount_drive.split("|", QString::SkipEmptyParts);

					lista_multiple_iso.clear();
					const int lmiso_Size = lista_isos.size();
					for (int i = 0; i < lmiso_Size; ++i)
					{
						QString path_multi_iso = getDirRelative(lista_isos.at(i), "DosGames");
					#ifdef Q_OS_WIN32
						path_multi_iso.replace("/","\\");
					#endif
						lista_multiple_iso << "\""+ path_multi_iso +"\"";
					}
					mount_drive.clear();
					mount_drive = lista_multiple_iso.join(" ");
				}

				if (listMount.at(m).tipo_as == "IMG_hdd")
				{
					montaje_IMG = true;
					mount_type = " -t hdd";

					if (listMount.at(m).opt_size != "")
						mount_freesize = " -size "+ listMount.at(m).opt_size;

					if (listMount.at(m).opt_mount != "")
						mount_options = " "+ listMount.at(m).opt_mount;
				}

				if (montaje_IMG == true)
				{
					if (listMount.at(m).tipo_as == "IMG_multi_iso" || listMount.at(m).tipo_as == "IMG_multi_floppy")
						listmontajes << "imgmount "+ mount_letter +" "+ mount_drive + mount_type;
					else
						listmontajes << "imgmount "+ mount_letter +" \""+  mount_drive +"\""+ mount_type + mount_freesize + mount_options;
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
					QString path_multi_boot = getDirRelative(lista_isos.at(i), "DosGames");
				#ifdef Q_OS_WIN32
					path_multi_boot.replace("/","\\");
				#endif
					lista_multiple_iso << "\""+ path_multi_boot +"\"";
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

	mount_dir = getShortPathName(DirEXEDbx).remove(getShortPathName(mount_dir_primario), Qt::CaseInsensitive).replace("/","\\");

	if (mount_Boot == false)
	{
		listmontajes << mount_letra_primario +":";
		listmontajes << "cd "+ mount_dir;
		listmontajes << Dbx_loadfix + getNameTo8Caracter(NombreEXEDbx) +" "+ cfgDbx.parametros_exe;
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
QString Funciones::crearArchivoConfigDbx(stDatosJuego datos, stConfigDOSBox cfgDbx, QList<stConfigDOSBoxMount> listMount, QString dir_app, QString tabla, QString pathSaveConfg, bool isSetup)
{
	QList<QString> url_list;
	stConfigScummVM cfgSvm;
	stConfigVDMSound cfgVdms;
	datos.tipo_emu = "dosbox";
	return exportarProfileGRlida(datos, url_list, cfgDbx, listMount, cfgSvm, cfgVdms, dir_app, tabla, pathSaveConfg, ExportDbx, isSetup);
}

// Crea la configuración del ScummVM
QString Funciones::creaIniScummVM(stConfigScummVM cfgSvm, QString pathSaveConfg)
{
	QString cfg_out = "";
	cfg_out.append("["+ cfgSvm.game_label +"]\n");
	cfg_out.append("description="+ cfgSvm.description +"\n");
	cfg_out.append("gameid="+ cfgSvm.game +"\n");
	cfg_out.append("language="+ cfgSvm.language +"\n");
	cfg_out.append("subtitles="+ cfgSvm.subtitles +"\n");
	cfg_out.append("platform="+ cfgSvm.platform +"\n");
	cfg_out.append("gfx_mode="+ cfgSvm.gfx_mode +"\n");
	cfg_out.append("render_mode="+ cfgSvm.render_mode +"\n");
	cfg_out.append("fullscreen="+ cfgSvm.fullscreen +"\n");
	cfg_out.append("aspect_ratio="+ cfgSvm.aspect_ratio +"\n");
	cfg_out.append("path="+ getDirRelative(cfgSvm.path, "DosGames") +"\n");
	cfg_out.append("extrapath="+ getDirRelative(cfgSvm.path_extra, "DosGames") +"\n");
	cfg_out.append("savepath="+ getDirRelative(cfgSvm.path_save, "DosGames") +"\n");
	cfg_out.append("music_driver="+ cfgSvm.music_driver +"\n");
	cfg_out.append("enable_gs="+ cfgSvm.enable_gs +"\n");
	cfg_out.append("multi_midi="+ cfgSvm.multi_midi +"\n");
	cfg_out.append("native_mt32="+ cfgSvm.native_mt32 +"\n");
	cfg_out.append("master_volume="+ cfgSvm.master_volume +"\n");
	cfg_out.append("music_volume="+ cfgSvm.music_volume +"\n");
	cfg_out.append("sfx_volume="+ cfgSvm.sfx_volume +"\n");
	cfg_out.append("speech_volume="+ cfgSvm.speech_volume +"\n");
	cfg_out.append("tempo="+ cfgSvm.tempo +"\n");
	cfg_out.append("talkspeed="+ cfgSvm.talkspeed +"\n");
	cfg_out.append("cdrom="+ cfgSvm.cdrom +"\n");
	cfg_out.append("joystick_num="+ cfgSvm.joystick_num +"\n");
	cfg_out.append("output_rate="+ cfgSvm.output_rate +"\n");
	cfg_out.append("midi_gain="+ cfgSvm.midi_gain +"\n");
	cfg_out.append("copy_protection="+ cfgSvm.copy_protection +"\n");
	cfg_out.append("soundfont="+ cfgSvm.sound_font +"\n");
	cfg_out.append("walkspeed="+ cfgSvm.walkspeed +"\n");
	cfg_out.append("originalsaveload="+ cfgSvm.originalsaveload +"\n");
	cfg_out.append("bright_palette="+ cfgSvm.bright_palette +"\n\n");

	cfg_out.append("[scummvm]\n");
	cfg_out.append("gui_theme=modern\n");
	cfg_out.append("gfx_mode=2x\n");
	cfg_out.append("fullscreen=false\n");
	cfg_out.append("extrapath="+ getDirRelative(cfgSvm.path_extra, "DosGames") +"\n");
	cfg_out.append("themepath="+ getDirRelative(cfgSvm.emu_svmpath) +"\n");
	cfg_out.append("autosave_period="+ cfgSvm.autosave_period +"\n");
	cfg_out.append("gui_language="+ stLngSelect +"\n");
	cfg_out.append("updates_check=0\n");

	if (pathSaveConfg.isEmpty())
	{
		cfg_out.append("\n[ExtraInfo]\n");
		cfg_out.append("game_label="+ cfgSvm.game_label +"\n");
		cfg_out.append("path_game="+ cfgSvm.path +"\n");
		cfg_out.append("path_extra="+ cfgSvm.path_extra +"\n");
		cfg_out.append("path_save="+ cfgSvm.path_save +"\n");
		cfg_out.append("mute="+ cfgSvm.mute +"\n");
		cfg_out.append("speech_mute="+ cfgSvm.speech_mute +"\n");
		cfg_out.append("debuglevel="+ cfgSvm.debuglevel +"\n");
		cfg_out.append("opl_driver="+ cfgSvm.opl_driver +"\n");
		cfg_out.append("disable_dithering="+ cfgSvm.disable_dithering +"\n");
		cfg_out.append("alt_intro="+ cfgSvm.alt_intro +"\n");
		cfg_out.append("boot_param="+ cfgSvm.boot_param +"\n");
		cfg_out.append("emu_svmpath="+ cfgSvm.emu_svmpath +"\n\n");
	}

	if (!pathSaveConfg.isEmpty())
	{
		if (comprobarDirectorio(getInfoFile(pathSaveConfg).Path))
			guardarArchivo(pathSaveConfg, cfg_out, "UTF-8");
	}

	return cfg_out;
}

// Crea el archivo de configuración del VDMSound
QString Funciones::crearArchivoConfigVdmS(stConfigVDMSound cfgVdms, QString pathSaveConfg)
{
	QString cfg_out = "";
	cfg_out.append("[program]\n");
	cfg_out.append("executable="+ getDirRelative(cfgVdms.path_exe, "DosGames") +"\n");
	cfg_out.append("workdir="+ getInfoFile(getDirRelative(cfgVdms.path_exe, "DosGames")).Path +"\n");
	cfg_out.append("params="+ cfgVdms.program_1 +"\n");
	cfg_out.append("icon="+ cfgVdms.program_2 +"\n\n");

	cfg_out.append("[vdms.debug]\n");
	cfg_out.append("useCustomCfg="+ cfgVdms.vdms_debug_1 +"\n");
	cfg_out.append("customCfg="+ cfgVdms.vdms_debug_2 +"\n\n");

	cfg_out.append("[winnt.dos]\n");
	cfg_out.append("useAutoexec="+ cfgVdms.winnt_dos_1 +"\n");
	cfg_out.append("autoexec="+ cfgVdms.winnt_dos_2 +"\n\n");

	cfg_out.append("[winnt.dosbox]\n");
	cfg_out.append("exitclose="+ cfgVdms.winnt_dosbox_1 +"\n");
	cfg_out.append("exitWarn="+ cfgVdms.winnt_dosbox_2 +"\n");
	cfg_out.append("fastPaste="+ cfgVdms.winnt_dosbox_3 +"\n\n");

	cfg_out.append("[winnt.storage]\n");
	cfg_out.append("useCDROM="+ cfgVdms.winnt_storage_1 +"\n");
	cfg_out.append("useNetware="+ cfgVdms.winnt_storage_2 +"\n\n");

	if (!pathSaveConfg.isEmpty())
	{
		if (comprobarDirectorio(getInfoFile(pathSaveConfg).Path))
			guardarArchivo(pathSaveConfg, cfg_out, "UTF-8");
	}

	return cfg_out;
}

// Exportar la configuración del GR-lida, Datos, DOSBox, ScummVM y VDMSound
QString Funciones::exportarProfileGRlida(stDatosJuego datos, QList<QString> url_list, stConfigDOSBox cfgDbx, QList<stConfigDOSBoxMount> listMount, stConfigScummVM cfgSvm, stConfigVDMSound cfgVdms, QString dir_app, QString tabla, QString pathSaveConfg, TipoCfg tipo_cfg, bool isSetup)
{
	QString cfg_out;
	const int listMountSize = listMount.size();

// Directorio del juego
	QString dirDatosGame = dir_app +"datosdb/"+ tabla +"/id-"+ datos.idgrl +"_"+ datos.titulo_guiones +"_"+ datos.tipo_emu +"/";

// DOSBox a usar
	QHash<QString, stGrlDatos> dbx_list = cargaDatosQHash(stDirApp +"datos/dbx_list.txt", 6, "|");

	QSettings settings(stDirApp +"GR-lida.conf", QSettings::IniFormat);
	settings.beginGroup("OpcGeneral");
		QString DOSBoxDefault = settings.value("DOSBoxDefault", "").toString();
	settings.endGroup();

	if (cfgDbx.dosbox_emu_key.isEmpty() || !dbx_list.contains(cfgDbx.dosbox_emu_key))
	{
		const int num_dbx_count = dbx_list.count(DOSBoxDefault);
		const int num_dbx_size  = dbx_list.size();
		if (num_dbx_count > 0 || num_dbx_size > 0)
		{
			QHash<QString, stGrlDatos>::iterator i_dbx = dbx_list.begin();
			cfgDbx.dosbox_emu_key = i_dbx.key();
		} else
			cfgDbx.dosbox_emu_key = DOSBoxDefault;
	//	conf_dosbox["dosbox_emu_key"] = grlCfg.DOSBoxDefault.isEmpty() ? "dosbox" : grlCfg.DOSBoxDefault;
	}

	stGrlDatos dosbox_dato_emu = dbx_list[cfgDbx.dosbox_emu_key];

	if (tipo_cfg == ExportGrlida)
	{
		cfg_out = leerArchivo(":/datos/grl_datos.xml", "UTF-8");
		if (datos.tipo_emu == "dosbox")
			cfg_out.replace("{grl_otra_configuracion}", "\n"+ leerArchivo(":/datos/grl_dosbox.xml", "UTF-8"));
		else if (datos.tipo_emu == "scummvm")
			cfg_out.replace("{grl_otra_configuracion}", "\n"+ leerArchivo(":/datos/grl_scummvm.xml", "UTF-8"));
		else if (datos.tipo_emu == "vdmsound")
			cfg_out.replace("{grl_otra_configuracion}", "\n"+ leerArchivo(":/datos/grl_vdmsound.xml", "UTF-8"));
		else
			cfg_out.replace("{grl_otra_configuracion}", "");
	} else {
		if (pathSaveConfg.isEmpty())
			cfg_out = leerArchivo(":/datos/dosbox_m.conf", "UTF-8");
		else {
			if (stLngSelect.isEmpty())
				cfg_out = leerArchivo(":/datos/dosbox.conf", "UTF-8");
			else
				cfg_out = leerArchivo(":/datos/"+ stLngSelect +"/dosbox.conf", "UTF-8");
		}
	}

	if (tipo_cfg == ExportDfend)
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
			if (i < 1)
			{
				cfg_dfend.append("WWW="+ url_list.at(i) +"\n");
				cfg_dfend.append("WWWName="+ url_list.at(i) +"\n");
			} else {
				cfg_dfend.append("WWW"+ intToStr(i+1) +"="+ url_list.at(i) +"\n");
				cfg_dfend.append("WWWName"+ intToStr(i+1) +"="+ url_list.at(i) +"\n");
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
		cfg_dfend.append("NrOfMounts="+ intToStr(listMountSize) +"\n");
		cfg_dfend.append("Tab=-1\n");
		cfg_dfend.append("Exe="+ cfgDbx.path_exe +"\n");
		cfg_dfend.append("Setup="+ cfgDbx.path_setup +"\n");
		cfg_dfend.append("GameParameters="+  cfgDbx.parametros_exe +"\n");
		cfg_dfend.append("SetupParameters="+  cfgDbx.parametros_setup +"\n");
//		cfg_dfend.append("ExtraDirs=\n");
//		cfg_dfend.append("DataDir=\n");

		if (listMountSize > 0)
		{
			for (int i = 0; i < listMountSize; ++i)
				cfg_dfend.append(intToStr(i) +"="+ listMount.at(i).path +";"+ listMount.at(i).tipo_as +";"+ listMount.at(i).label +";"+ listMount.at(i).io_ctrl +"\n");
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
	else if (tipo_cfg == ExportGrlida)
	{
		uint secs = QDateTime::currentDateTime().toTime_t();

		cfg_out.replace("{lastBuildDate}", horaFechaActual(intToStr(secs)));
		cfg_out.replace("{year}", horaFechaActual(intToStr(secs), "yyyy"));

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
//		cfg_out.replace("{Dat_path_capturas}"   , datos.path_capturas   );
		cfg_out.replace("{Dat_path_setup}"      , datos.path_setup      );
		cfg_out.replace("{Dat_parametros_setup}", datos.parametros_setup);
	} else
		cfg_out.replace("{Dbx_config_export_dfend}", "");

	if (datos.tipo_emu == "dosbox")
	{
	// [sdl]
		cfg_out.replace("{Dbx_sdl_fullscreen}"      , cfgDbx.sdl_fullscreen      );
		cfg_out.replace("{Dbx_sdl_fulldouble}"      , cfgDbx.sdl_fulldouble      );
		cfg_out.replace("{Dbx_sdl_fullfixed}"       , cfgDbx.sdl_fullfixed       );
		cfg_out.replace("{Dbx_sdl_fullresolution}"  , cfgDbx.sdl_fullresolution  );
		cfg_out.replace("{Dbx_sdl_windowresolution}", cfgDbx.sdl_windowresolution);
		cfg_out.replace("{Dbx_sdl_output}"          , cfgDbx.sdl_output          );
		cfg_out.replace("{Dbx_sdl_hwscale}"         , cfgDbx.sdl_hwscale         );
		cfg_out.replace("{Dbx_sdl_autolock}"        , cfgDbx.sdl_autolock        );
		cfg_out.replace("{Dbx_sdl_sensitivity}"     , cfgDbx.sdl_sensitivity     );
		cfg_out.replace("{Dbx_sdl_waitonerror}"     , cfgDbx.sdl_waitonerror     );
		cfg_out.replace("{Dbx_sdl_priority}"        , cfgDbx.sdl_priority        );
		cfg_out.replace("{Dbx_sdl_mapperfile}"      , cfgDbx.sdl_mapperfile      );
		cfg_out.replace("{Dbx_sdl_usescancodes}"    , cfgDbx.sdl_usescancodes    );

		if (dosbox_dato_emu.version <= "0.73")
			cfg_out.replace("{Dbx_sdl_extra}", "hwscale="+ cfgDbx.sdl_hwscale +"\n");
		else
			cfg_out.replace("{Dbx_sdl_extra}", "");

	// [dosbox]
		cfg_out.replace("{Dbx_dosbox_language}", cfgDbx.dosbox_language);
		cfg_out.replace("{Dbx_dosbox_machine}" , cfgDbx.dosbox_machine);
		cfg_out.replace("{Dbx_dosbox_captures}", dirDatosGame +"capturas/");
		cfg_out.replace("{Dbx_dosbox_memsize}" , cfgDbx.dosbox_memsize);
		cfg_out.replace("{Dbx_dosbox_emu_key}" , cfgDbx.dosbox_emu_key);

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

		if (cfgDbx.cpu_cycles != "auto" && cfgDbx.cpu_cycles != "max" && dosbox_dato_emu.version >= "0.73")
			cfg_out.replace("{Dbx_cpu_cycles}", "fixed "+ cfgDbx.cpu_cycles);
		else {
			QString cycles_realmode, cycles_protmode, cycles_limitmode;
			if (cfgDbx.cpu_cycles == "auto" && dosbox_dato_emu.version >= "0.74")
			{
				cycles_realmode = cycles_protmode = cycles_limitmode = "";
				if (!cfgDbx.cpu_cycles_realmode.isEmpty())
					cycles_realmode = " "+ cfgDbx.cpu_cycles_realmode;
				if (!cfgDbx.cpu_cycles_protmode.isEmpty())
					cycles_protmode = " "+ cfgDbx.cpu_cycles_protmode;
				if (!cfgDbx.cpu_cycles_limitmode.isEmpty())
					cycles_limitmode = " "+ cfgDbx.cpu_cycles_limitmode;

				cfg_out.replace("{Dbx_cpu_cycles}", "auto"+ cycles_realmode + cycles_protmode + cycles_limitmode);
			}
			else if (cfgDbx.cpu_cycles == "max" && dosbox_dato_emu.version >= "0.74")
			{
				cycles_protmode = cycles_limitmode = "";
				if (!cfgDbx.cpu_cycles_protmode.isEmpty())
					cycles_protmode = " "+ cfgDbx.cpu_cycles_protmode;
				if (!cfgDbx.cpu_cycles_limitmode.isEmpty())
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

		if (dosbox_dato_emu.version >= "0.73" || tipo_cfg == ExportGrlida)
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

		if (dosbox_dato_emu.version >= "0.73")
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

		if (dosbox_dato_emu.version >= "0.73" || tipo_cfg == ExportGrlida)
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
		if (tipo_cfg == ExportGrlida)
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
			if (dosbox_dato_emu.version <= "0.73")
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

		if (tipo_cfg == ExportDfend)
			cfg_out.replace("{Dbx_dos_version}", "ReportedDOSVersion="+ cfgDbx.dos_version +"\n");
		else if (tipo_cfg == ExportGrlida)
			cfg_out.replace("{Dbx_dos_version}", cfgDbx.dos_version);
		else
			cfg_out.replace("{Dbx_dos_version}", "");

	// [ipx]
		cfg_out.replace("{Dbx_ipx_ipx}", cfgDbx.ipx_ipx);

		if (tipo_cfg == ExportDbx)
		{
			if (strToBool(cfgDbx.ipx_ipx))
			{
				if (cfgDbx.ipx_type == "server")
					cfg_out.replace("{Dbx_ipx_config}", "IPXNET STARTSERVER "+ cfgDbx.ipx_port +"\n");
				else if (cfgDbx.ipx_type == "client")
					cfg_out.replace("{Dbx_ipx_config}", "IPXNET CONNECT "+ cfgDbx.ipx_ip +" "+ cfgDbx.ipx_port +"\n");
				else
					cfg_out.replace("{Dbx_ipx_config}", "");
			} else
				cfg_out.replace("{Dbx_ipx_config}", "");

		// [autoexec]
			QString autoexec = "";

			if (!cfgDbx.dos_version.isEmpty())
				autoexec.append("ver set "+ cfgDbx.dos_version +"\n\n");

			if (strToBool(cfgDbx.opt_autoexec))
				autoexec.append(cfgDbx.autoexec +"\n");
			else {
				// Creando el Autoexec
				QStringList listamontaje = creaConfigMontajes(listMount, cfgDbx, isSetup);
				const int listamontajeSize = listamontaje.size();
				for (int i = 0; i < listamontajeSize; ++i)
					autoexec.append(listamontaje.at(i) +"\n");
			}

			cfg_out.replace("{Dbx_autoexec}", autoexec);
		}
		else if (tipo_cfg == ExportGrlida)
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
			cfg_out.replace("{Dbx_path_exe}"        , cfgDbx.path_exe);
			cfg_out.replace("{Dbx_path_setup}"      , cfgDbx.path_setup);
			cfg_out.replace("{Dbx_parametros_exe}"  , cfgDbx.parametros_exe);
			cfg_out.replace("{Dbx_parametros_setup}", cfgDbx.parametros_setup);

		// Montajes
			QString cfg_mount = "";
			if (listMountSize > 0)
			{
				for (int i = 0; i < listMountSize; ++i)
				{
					cfg_mount.append("\n    <dosbox_montajes>\n");
					cfg_mount.append("      <Dbx_mount_path>"+ listMount.at(i).path +"</Dbx_mount_path>\n");
					cfg_mount.append("      <Dbx_mount_letter>"+ listMount.at(i).letter +"</Dbx_mount_letter>\n");
					cfg_mount.append("      <Dbx_mount_tipo_as>"+ listMount.at(i).tipo_as +"</Dbx_mount_tipo_as>\n");
					cfg_mount.append("      <Dbx_mount_label>"+ listMount.at(i).label +"</Dbx_mount_label>\n");
					cfg_mount.append("      <Dbx_mount_indx_cd>"+ listMount.at(i).indx_cd +"</Dbx_mount_indx_cd>\n");
					cfg_mount.append("      <Dbx_mount_opt_mount>"+ listMount.at(i).opt_mount +"</Dbx_mount_opt_mount>\n");
					cfg_mount.append("      <Dbx_mount_io_ctrl>"+ listMount.at(i).io_ctrl +"</Dbx_mount_io_ctrl>\n");
					cfg_mount.append("      <Dbx_mount_select_mount>"+ listMount.at(i).select_mount +"</Dbx_mount_select_mount>\n");
					cfg_mount.append("      <Dbx_mount_opt_size>"+ listMount.at(i).opt_size +"</Dbx_mount_opt_size>\n");
					cfg_mount.append("      <Dbx_mount_opt_freesize>"+ listMount.at(i).opt_freesize +"</Dbx_mount_opt_freesize>\n");
					cfg_mount.append("      <Dbx_mount_freesize>"+ listMount.at(i).freesize +"</Dbx_mount_freesize>\n");
					cfg_mount.append("    </dosbox_montajes>");
				}
			}
			cfg_out.replace("{dosbox_montajes}", cfg_mount);
		} else
			cfg_out.replace("{Dbx_autoexec}", "");
	}

	if (datos.tipo_emu == "scummvm")
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
		cfg_out.replace("{Svm_path}"             , cfgSvm.path           );
		cfg_out.replace("{Svm_path_extra}"       , cfgSvm.path_extra     );
		cfg_out.replace("{Svm_path_save}"        , cfgSvm.path_save      );
//		cfg_out.replace("{Svm_path_capturas}"    , cfgSvm.path_capturas  );
//		cfg_out.replace("{Svm_path_sonido}"      , cfgSvm.path_sonido    );
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

	if (datos.tipo_emu == "vdmsound")
	{
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

	if (!pathSaveConfg.isEmpty())
	{
		if (comprobarDirectorio(getInfoFile(pathSaveConfg).Path))
			guardarArchivo(pathSaveConfg, cfg_out);
	}

	return cfg_out;
}

// Importar la configuración del DOSBox del DFend
QHash<QString, QString> Funciones::importarProfileDFend(QString dir_app, QString filename, bool isDbx)
{
	QHash<QString, QString> cfgDFend;
	QString lineas = "";

	if (!isDbx)
	{
		lineas = leerArchivo(filename);
		lineas.replace("\\", "/");
		lineas.replace(";", "|");
		lineas.replace(",", "--");
	} else {
		lineas = filename;
		lineas.replace("\\", "/");
	}

	filename = dir_app +"temp/temp_config.prof";
	guardarArchivo(filename, lineas, "UTF-8");
	lineas.clear();

	QSettings settings(filename, QSettings::IniFormat);
// Datos del juego -----
	if (!isDbx)
	{
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
			cfgDFend["Dat_favorito"]         = boolToStr(settings.value("Favorite", "false").toBool());
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
				cfgDFend.insert("Dat_www_"+ intToStr(i) +"_name", settings.value("WWW"+ intToStr(i) +"Name", "").toString().replace("--", ";"));
				cfgDFend.insert("Dat_www_"+ intToStr(i), settings.value("WWW"+ intToStr(i), "").toString().replace("--", ";"));
			}
			cfgDFend["UserInfo"]= settings.value("UserInfo", "").toString().replace("--", ",");
		settings.endGroup();

	// Datos del emulador DOSBox ---
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
			cfgDFend["Dbx_opt_loadfix"]      = boolToStr(settings.value("Loadhigh", false).toBool());
			cfgDFend["Dbx_opt_loadfix_mem"]  = settings.value("LoadFixVal", "64").toString();
			cfgDFend["Dbx_opt_consola_dbox"] = boolToStr(settings.value("ConsoleWindow", false).toBool());
			cfgDFend["Dbx_opt_cerrar_dbox"]  = boolToStr(settings.value("CloseOnExit", true).toBool());
			cfgDFend["Dbx_opt_cycle_sincronizar"] = "false";
			cfgDFend["Dbx_ExtraFiles"]       = settings.value("ExtraFiles", "").toString();
			cfgDFend["Dbx_NrOfMounts"]       = settings.value("NrOfMounts", "0").toString();

			int num_mounts = cfgDFend["Dbx_NrOfMounts"].toInt();
			if (num_mounts > -1)
			{
				for (int n = 0; n < num_mounts; n++)
					cfgDFend.insert(intToStr(n), settings.value(intToStr(n), "").toString());
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

			if (settings.value("ProfileMode").toString().toLower() == "dosbox")
				cfgDFend["Dat_tipo_emu"] = "dosbox";
			else if (settings.value("ProfileMode").toString().toLower() == "scummvm")
				cfgDFend["Dat_tipo_emu"] = "scummvm";
			else
				cfgDFend["Dat_tipo_emu"] = "datos";
		settings.endGroup();
	} else {
	// Opciones
		cfgDFend["Dbx_opt_autoexec"]          = "false";
		cfgDFend["Dbx_opt_loadfix"]           = "false";
		cfgDFend["Dbx_opt_loadfix_mem"]       = "64";
		cfgDFend["Dbx_opt_consola_dbox"]      = "false";
		cfgDFend["Dbx_opt_cerrar_dbox"]       = "true";
		cfgDFend["Dbx_opt_cycle_sincronizar"] = "false";
	// Path
		cfgDFend["Dbx_path_exe"]         = "";
		cfgDFend["Dbx_path_setup"]       = "";
		cfgDFend["Dbx_parametros_exe"]   = "";
		cfgDFend["Dbx_parametros_setup"] = "";
	}

// [sdl]
	settings.beginGroup("sdl");
		cfgDFend["Dbx_sdl_fullscreen"]       = boolToStr(settings.value("fullscreen", false).toBool());
		cfgDFend["Dbx_sdl_fulldouble"]       = boolToStr(settings.value("fulldouble", false).toBool());
		cfgDFend["Dbx_sdl_fullfixed"]        = boolToStr(settings.value("fullfixed" , false).toBool()); // en desuso
		cfgDFend["Dbx_sdl_fullresolution"]   = settings.value("fullresolution", "original").toString();
		cfgDFend["Dbx_sdl_windowresolution"] = settings.value("windowresolution", "original").toString();
		cfgDFend["Dbx_sdl_output"]           = settings.value("output", "surface").toString();
		cfgDFend["Dbx_sdl_hwscale"]          = settings.value("hwscale", "1.00").toString();
		cfgDFend["Dbx_sdl_autolock"]         = boolToStr(settings.value("autolock", true).toBool());
		cfgDFend["Dbx_sdl_sensitivity"]      = settings.value("sensitivity", "100").toString();
		cfgDFend["Dbx_sdl_waitonerror"]      = boolToStr(settings.value("waitonerror", true).toBool());

		if (!isDbx)
			cfgDFend["Dbx_sdl_priority"] = settings.value("priority", "higher,normal").toString().replace("--", ",");
		else {
			QStringList list_priority  = settings.value("priority", QStringList() << "higher" << "normal").toStringList();
			cfgDFend["Dbx_sdl_priority"] = list_priority.join(",");
		}

		cfgDFend["Dbx_sdl_mapperfile"] = settings.value("mapperfile", "mapper.txt").toString().replace("--", ",");
		cfgDFend["Dbx_sdl_usescancodes"]     = boolToStr(settings.value("usescancodes", true).toBool());
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
		cfgDFend["Dbx_render_frameskip"]   = settings.value("frameskip", "0").toString();
		cfgDFend["Dbx_render_aspect"]      = boolToStr(settings.value("aspect", false).toBool());
		cfgDFend["Dbx_render_scaler"]      = settings.value("scaler", "normal2x").toString();
//		cfgDFend["Dbx_render_textmodelines"] = settings.value("TextModeLines", "25").toString();
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
		cfgDFend["Dbx_mixer_nosound"]   = boolToStr(settings.value("nosound", false).toBool());
		cfgDFend["Dbx_mixer_rate"]      = settings.value("rate"     , "44100").toString();
		cfgDFend["Dbx_mixer_blocksize"] = settings.value("blocksize", "1024").toString();
		cfgDFend["Dbx_mixer_prebuffer"] = settings.value("prebuffer", "20").toString();
	settings.endGroup();
// [midi]
	settings.beginGroup("midi");
		cfgDFend["Dbx_midi_mpu401"]      = settings.value("mpu401", "intelligent").toString();
		cfgDFend["Dbx_midi_intelligent"] = boolToStr(settings.value("midi_intelligent", true).toBool());
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
		cfgDFend["Dbx_sblaster_mixer"]   = boolToStr(settings.value("mixer", true).toBool());
		cfgDFend["Dbx_sblaster_oplmode"] = settings.value("oplmode", "auto").toString();
		cfgDFend["Dbx_sblaster_oplemu"]  = settings.value("oplemu" , "default").toString();
		cfgDFend["Dbx_sblaster_oplrate"] = settings.value("oplrate", "44100").toString();
	settings.endGroup();
// [gus]
	settings.beginGroup("gus");
		cfgDFend["Dbx_gus_gus"]      = boolToStr(settings.value("gus", false).toBool());
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
		cfgDFend["Dbx_speaker_pcspeaker"] = boolToStr(settings.value("pcspeaker", true).toBool());
		cfgDFend["Dbx_speaker_pcrate"]    = settings.value("pcrate"   , "44100").toString();
		cfgDFend["Dbx_speaker_tandy"]     = settings.value("tandy"    , "auto").toString();
		cfgDFend["Dbx_speaker_tandyrate"] = settings.value("tandyrate", "44100").toString();
		cfgDFend["Dbx_speaker_disney"]    = boolToStr(settings.value("disney", true).toBool());
	settings.endGroup();
// [joystick]
	settings.beginGroup("joystick");
		cfgDFend["Dbx_joystick_type"]       = settings.value("joysticktype", "auto").toString();
		cfgDFend["Dbx_joystick_timed"]      = boolToStr(settings.value("timed"     , true).toBool());
		cfgDFend["Dbx_joystick_autofire"]   = boolToStr(settings.value("autofire"  , false).toBool());
		cfgDFend["Dbx_joystick_swap34"]     = boolToStr(settings.value("swap34"    , false).toBool());
		cfgDFend["Dbx_joystick_buttonwrap"] = boolToStr(settings.value("buttonwrap", false).toBool());
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
		cfgDFend["Dbx_serial_1"] = settings.value("serial1", "dummy").toString().replace("--",",");
		cfgDFend["Dbx_serial_2"] = settings.value("serial2", "dummy").toString().replace("--",",");
		cfgDFend["Dbx_serial_3"] = settings.value("serial3", "disabled").toString().replace("--",",");
		cfgDFend["Dbx_serial_4"] = settings.value("serial4", "disabled").toString().replace("--",",");
	settings.endGroup();
// [dos]
	settings.beginGroup("dos");
		cfgDFend["Dbx_dos_xms"] = boolToStr(settings.value("xms", true).toBool());
		cfgDFend["Dbx_dos_ems"] = boolToStr(settings.value("ems", true).toBool());
		cfgDFend["Dbx_dos_umb"] = settings.value("umb", "true").toString();

		if (settings.value("keyboardlayout", "auto").toString().toLower() == "default")
			cfgDFend["Dbx_dos_keyboardlayout"] = "auto";
		else
			cfgDFend["Dbx_dos_keyboardlayout"] = settings.value("keyboardlayout", "auto").toString();

		cfgDFend["Dbx_dos_version"] = settings.value("ReportedDOSVersion", "").toString();
//		cfgDFend["Dbx_dos_codepage"] = settings.value("codepage", "").toString();
	settings.endGroup();
// [ipx]
	settings.beginGroup("ipx");
		cfgDFend["Dbx_ipx_ipx"]  = boolToStr(settings.value("ipx", false).toBool());
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
		cfgDFend["Svm_subtitles"]         = boolToStr(settings.value("Subtitles", true).toBool());
		cfgDFend["Svm_platform"]          = settings.value("platform", "pc").toString();
		cfgDFend["Svm_gfx_mode"]          = settings.value("Filter", "2x").toString();
		cfgDFend["Svm_render_mode"]       = settings.value("render_mode", "").toString();
		cfgDFend["Svm_fullscreen"]        = boolToStr(settings.value("fullscreen", false).toBool());
		cfgDFend["Svm_aspect_ratio"]      = boolToStr(settings.value("aspect_ratio", true).toBool());
		cfgDFend["Svm_path_game"]         = settings.value("GamePath", "").toString();
		cfgDFend["Svm_path_extra"]        = settings.value("path_extra", "").toString();
		cfgDFend["Svm_path_save"]         = settings.value("Savepath", "").toString();
		cfgDFend["Svm_path_capturas"]     = settings.value("path_capturas", "").toString();
		cfgDFend["Svm_path_sonido"]       = settings.value("path_sonido", "").toString();
		cfgDFend["Svm_music_driver"]      = settings.value("MusicDriver", "adlib").toString();
		cfgDFend["Svm_enable_gs"]         = boolToStr(settings.value("EnableGS", true).toBool());
		cfgDFend["Svm_multi_midi"]        = boolToStr(settings.value("MultiMIDI", true).toBool());
		cfgDFend["Svm_native_mt32"]       = boolToStr(settings.value("NativeMT32", true).toBool());
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
		cfgDFend["Svm_copy_protection"]   = boolToStr(settings.value("copy_protection", false).toBool());
		cfgDFend["Svm_sound_font"]        = settings.value("sound_font", "").toString();
		cfgDFend["Svm_walkspeed"]         = settings.value("Walkspeed", "0").toString();
		cfgDFend["Svm_opl_driver"]        = "";
		cfgDFend["Svm_disable_dithering"] = "";
		cfgDFend["Svm_alt_intro"]         = "";
		cfgDFend["Svm_boot_param"]        = "";
	settings.endGroup();

//	eliminarArchivo(dir_app +"temp/temp_config.prof");

	return cfgDFend;
}

// Importar la configuración del ScummVM
QHash<QString, QString> Funciones::importarIniScummVM(QString filename)
{
	QHash<QString, QString> cfgSvm;

	QSettings settings(filename, QSettings::IniFormat);
	settings.beginGroup("ExtraInfo");
		cfgSvm["Svm_game_label"]        = settings.value("game_label"       , "").toString();
		cfgSvm["Svm_path_game"]         = settings.value("path_game"        , "").toString();
		cfgSvm["Svm_path_extra"]        = settings.value("path_extra"       , "").toString();
		cfgSvm["Svm_path_save"]         = settings.value("path_save"        , "").toString();
		cfgSvm["Svm_mute"]              = settings.value("mute"             , "false").toString();
		cfgSvm["Svm_speech_mute"]       = settings.value("speech_mute"      , "false").toString();
		cfgSvm["Svm_debuglevel"]        = settings.value("debuglevel"       , "0").toString();
		cfgSvm["Svm_opl_driver"]        = settings.value("opl_driver"       , "auto").toString();
		cfgSvm["Svm_disable_dithering"] = settings.value("disable_dithering", "false").toString();
		cfgSvm["Svm_alt_intro"]         = settings.value("alt_intro"        , "false").toString();
		cfgSvm["Svm_boot_param"]        = settings.value("boot_param"       , "").toString();
		cfgSvm["Svm_emu_svmpath"]       = settings.value("emu_svmpath"      , "").toString();
	settings.endGroup();

	settings.beginGroup(cfgSvm["Svm_game_label"]);
		cfgSvm["Svm_description"]     = settings.value("description"    , "").toString();
		cfgSvm["Svm_game"]            = settings.value("gameid"         , "").toString();
		cfgSvm["Svm_language"]        = settings.value("language"       , "es").toString();
		cfgSvm["Svm_subtitles"]       = settings.value("subtitles"      , "true").toString();
		cfgSvm["Svm_platform"]        = settings.value("platform"       , "pc").toString();
		cfgSvm["Svm_gfx_mode"]        = settings.value("gfx_mode"       , "2x").toString();
		cfgSvm["Svm_render_mode"]     = settings.value("render_mode"    , "").toString();
		cfgSvm["Svm_fullscreen"]      = settings.value("fullscreen"     , "false").toString();
		cfgSvm["Svm_aspect_ratio"]    = settings.value("aspect_ratio"   , "false").toString();
		cfgSvm["Svm_music_driver"]    = settings.value("music_driver"   , "adlib").toString();
		cfgSvm["Svm_enable_gs"]       = settings.value("enable_gs"      , "false").toString();
		cfgSvm["Svm_multi_midi"]      = settings.value("multi_midi"     , "true").toString();
		cfgSvm["Svm_native_mt32"]     = settings.value("native_mt32"    , "false").toString();
		cfgSvm["Svm_master_volume"]   = settings.value("master_volume"  , "192").toString();
		cfgSvm["Svm_music_volume"]    = settings.value("music_volume"   , "192").toString();
		cfgSvm["Svm_sfx_volume"]      = settings.value("sfx_volume"     , "192").toString();
		cfgSvm["Svm_speech_volume"]   = settings.value("speech_volume"  , "192").toString();
		cfgSvm["Svm_tempo"]           = settings.value("tempo"          , "100").toString();
		cfgSvm["Svm_talkspeed"]       = settings.value("talkspeed"      , "60").toString();
		cfgSvm["Svm_cdrom"]           = settings.value("cdrom"          , "0").toString();
		cfgSvm["Svm_joystick_num"]    = settings.value("joystick_num"   , "0").toString();
		cfgSvm["Svm_output_rate"]     = settings.value("output_rate"    , "").toString();
		cfgSvm["Svm_midi_gain"]       = settings.value("midi_gain"      , "100").toString();
		cfgSvm["Svm_copy_protection"] = settings.value("copy_protection", "false").toString();
		cfgSvm["Svm_sound_font"]      = settings.value("soundfont"      , "").toString();
		cfgSvm["Svm_walkspeed"]       = settings.value("walkspeed"      , "0").toString();
	settings.endGroup();

	settings.beginGroup("scummvm");
		cfgSvm["Svm_autosave_period"] = settings.value("autosave_period", "300").toString();
	settings.endGroup();

	return cfgSvm;
}
