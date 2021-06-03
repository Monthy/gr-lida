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

#include <QDialog>
#include <QBoxLayout>
#include <QTextBrowser>
#include <QPushButton>

#include "dbsql.h"

dbSql::dbSql(QString db_type, QString db_server, QString db_host, QString db_name, QString db_username, QString db_password, int db_port, QString dirApp, QString lng, QString theme)
{
	isOpenDb   = false;
	qpsql_oids = "";
	qpsql_text = "LONGTEXT";

	dbType = db_type; // QSQLITE, QMYSQL, QPSQL
	if (dbType == "QMYSQL")
		dbTypeId = "INT(11) UNSIGNED NOT NULL AUTO_INCREMENT";
	else if (dbType == "QPSQL")
	{
		dbTypeId   = "SERIAL";
		qpsql_oids = "WITH (OIDS=FALSE)";
		qpsql_text = "TEXT";
	} else if (dbType == "QSQLITE")
		dbTypeId = "INTEGER NOT NULL";
	else
		dbTypeId = "INTEGER NOT NULL";

	if (dbType == "QMYSQL" || dbType == "QPSQL")
	{
		sqldb = QSqlDatabase::addDatabase(dbType, "qt_sql_dbgrl");
		sqldb.setHostName(db_server);
		sqldb.setDatabaseName(db_name);
		sqldb.setUserName(db_username);
		sqldb.setPassword(db_password);
		sqldb.setPort(db_port);
	} else {
		sqldb = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_dbgrl");
		sqldb.setDatabaseName(db_host);
	}

	isOpenDb = sqldb.open();

	stDirApp = dirApp;
	setTheme(theme);
	setIdioma(lng);

	sqldb.transaction();
}

dbSql::~dbSql()
{
	if (isOpenDb)
		close();
}

void dbSql::comit()
{
	if (!sqldb.commit())
		sqldb.rollback();
}

// Retorna el último ID insertado
QString dbSql::lastInsertId(QSqlQuery &query, QString sql_col)
{
	if (dbType == "QPSQL")
	{
		query.next();
		return query.record().value(sql_col).toString();
	} else
		return query.lastInsertId().toString();
}

QString dbSql::qpsql_pkey(QString tabla)
{
	if (dbType == "QPSQL")
		return "CONSTRAINT "+ tabla +"_pkey ";
	else
		return "";
}

QString dbSql::qpsql_return_id(QString sql_col)
{
	if (dbType == "QPSQL")
		return " RETURNING "+ sql_col;
	else
		return "";
}

// Convierte texto "true", "yes", "1" a tipo bool
bool dbSql::strToBool(QString str)
{
	QString value = str.toLower();
	if (value == "true" || value == "yes" || value == "1")
		return true;
	else
		return false;
}

// Convierte bool a texto "true", "yes" indicando el tipo
QString dbSql::boolToStr(bool estado, bool type_yes)
{
	if (estado)
		return type_yes ? "yes": "true";
	else
		return type_yes ? "no": "false";
}

// Elimina o convierte acentos.
QString dbSql::removeAccents(QString str, QString sep, bool to_lower)
{
	QStringList txt_out, txt_in, lista;
	QString output, linea;

	txt_in.clear();
	txt_out.clear();
	QFile file_in(":/datos/text_accent_convert.txt");
	if (file_in.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream in(&file_in);
		in.setCodec("UTF-8");
		while (!in.atEnd())
		{
			linea = in.readLine();
			if (!linea.isEmpty())
			{
				lista = linea.split("-@-");
				if (lista.size() == 2)
				{
					txt_in << lista.at(0);
					txt_out << lista.at(1);
				}
			}
		}
	}
	file_in.close();

	output = "";
	for (int i = 0; i < str.length(); i++)
	{
		QChar c = str[i];
		int index = txt_in.indexOf(c);
		if (index < 0)
			output.append(c);
		else {
			QString replacement = txt_out.at(index);
			output.append(replacement);
		}
	}
	output = output.normalized(QString::NormalizationForm_KD);
	output.replace(QRegExp("[\\s]+"), " ");
	output.replace(" ", sep);

	if (to_lower)
		return output.toLower();
	else
		return output;
}

// Elimina etiquetas de html
QString dbSql::removeHtmlTag(QString str)
{
	str.replace("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd\">", "");
	str.replace("<html>" , "");
	str.replace("<head>" , "");
	str.replace("</head>", "");
	str.replace("<head/>", "");
	str.replace("<body>" , "");
	str.replace("</body>", "");
	str.replace("</html>", "");
	return str;
}

// Devuelve el tiempo actual, formato unix
QString dbSql::getTime()
{
	uint secs = QDateTime::currentDateTime().toTime_t();
	return QVariant(secs).toString();
}

// Convierte fecha 'dd/MM/yyyy HH:mm:ss' a formato unix
QString dbSql::setTime(QString fecha, QString formato, bool to_unix)
{
	if (to_unix)
	{
		uint secs = QDateTime::fromString(fecha, formato).toTime_t();
		return QVariant(secs).toString();
	} else {
		QDateTime dt;
		dt.setTime_t(fecha.toUInt());

		if (formato.isEmpty())
			return dt.toString("dd/MM/yyyy HH:mm:ss");
		else
			return dt.toString(formato);
	}
}

void dbSql::setTheme(QString theme)
{
	stTheme = theme;
}

void dbSql::setIdioma(QString lng)
{
	stLngSelect = lng;
}

// Devuelve si el archivos admiten traducción
QString dbSql::getArchivoIsLng(QString archivo)
{
	QStringList lista;
	lista << "edad_recomendada.txt" << "formatos.txt" << "generos.txt" << "idiomas.txt";
	lista << "numdisc.txt" << "perspectivas.txt" << "svm_idioma.txt" << "temas.txt";
	lista << "posiciones.txt";

	if (lista.contains(archivo))
		return stLngSelect +"/";
	else
		return "";
}

// Devuelve si el archivos es de 3 columnas
bool dbSql::getArchivoIs3Col(QString archivo)
{
	QStringList lista;
	lista << "dbx_scaler.txt" << "dbx_scaler_svn.txt" << "dbx_sbtype.txt" << "dbx_sb_oplmode.txt" << "dbx_output.txt" << "dbx_output_svn.txt" << "dbx_machine.txt" << "dbx_machine_svn.txt";
	lista << "dbx_joystick.txt" << "dbx_cpu_type.txt" << "dbx_cpu_type_svn.txt" << "dbx_cpu_core.txt" << "svm_render_mode.txt" << "svm_platform.txt";
	lista << "svm_music_driver.txt" << "svm_gfxmode.txt" << "edad_recomendada.txt";// << "dbx_keyboardlayout.txt";
	lista << "svm_idioma.txt" << "idiomas.txt" << "posiciones.txt";

	if (lista.contains(archivo))
		return true;
	else
		return false;
}

// Convierte bytes
QString dbSql::covertir_bytes(double size)
{
	QStringList list;
	list << "KB" << "MB" << "GB" << "TB";
	QStringListIterator i(list);
	QString unidad("bytes");

	while (size >= 1024.0 && i.hasNext())
	{
		unidad = i.next();
		size /= 1024.0;
	}

	return QString().setNum(size, 'f', 2) +" "+ unidad;
}

// Cierra la base de datos
bool dbSql::close()
{
	sqldb.close();
	sqldb = QSqlDatabase();
	sqldb.close();

	return isOpenDb = false;
}

// Retorna si esta abierta o no la base de datos
bool dbSql::dbisOpen()
{
	return isOpenDb;
}

// Muestra una ventana de información
void dbSql::ventanaInfo(QString titulo, QString icono, int w, int h, QString contenido, bool isTextoPlano)
{
	QVBoxLayout *mainLayout;
	QTextBrowser *textInfo;
	QHBoxLayout *hLayout;
	QSpacerItem *hSpacer;
	QPushButton *btnAceptar;

	QDialog *Dialog = new QDialog();
	Dialog->resize(w, h);
	Dialog->setWindowTitle(titulo);

	if (QFile::exists(icono))
		Dialog->setWindowIcon(QIcon(stTheme +"img16/"+ icono));
	else
		Dialog->setWindowIcon(QIcon(stTheme +"img16/informacion.png"));

	mainLayout = new QVBoxLayout(Dialog);
	mainLayout->setContentsMargins(4, 4, 4, 4);
		textInfo = new QTextBrowser(Dialog);
		textInfo->setReadOnly(true);

	if (isTextoPlano)
	{
		textInfo->setOpenExternalLinks(false);
		textInfo->setOpenLinks(false);
		textInfo->setPlainText(contenido);
	} else {
		textInfo->setOpenExternalLinks(true);
		textInfo->setOpenLinks(true);
		textInfo->setHtml(contenido.replace("\n","<br/>"));
	}

	mainLayout->addWidget(textInfo);
		hLayout = new QHBoxLayout();
			hSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		hLayout->addItem(hSpacer);
			btnAceptar = new QPushButton(Dialog);
			btnAceptar->setIcon(QIcon(stTheme +"img16/aplicar.png"));
			btnAceptar->setText(tr("Aceptar"));
			btnAceptar->setMinimumSize(QSize(80, 24));
		hLayout->addWidget(btnAceptar);
	mainLayout->addLayout(hLayout);

	connect(btnAceptar, SIGNAL(clicked()), Dialog, SLOT(accept()));
	Dialog->exec();

	delete Dialog;
}

// Comprueba si el Query se ha ejecutado bien.
bool dbSql::chequearQuery(QSqlQuery &query)
{
	if (query.lastError().type() != QSqlError::NoError)
	{
		ventanaInfo("SQL info", "basedatos.png", 400, 300, "SQL:\n"+ query.lastQuery() +"\n\nError:\n"+ query.lastError().text());
		return false;
	}

	return true;
}

// Elimina un item de la tabla indicada.
bool dbSql::eliminarItemTable(QString tabla, QString Id)
{
	if (tabla.isEmpty() && Id.isEmpty())
		return false;
	else {
		QSqlQuery query(sqldb);
		query.exec("DELETE FROM "+ tabla +" WHERE id="+ Id +";");

		return chequearQuery(query);
	}
}

// Lista las tablas de la base de datos
QStringList dbSql::listarTablas()
{
	return sqldb.tables(QSql::Tables);
}

// Cuenta el número de registros que tiene
int dbSql::getCount(QString tabla, QString sql_where, QString sql_col, QString col_value)
{
	Q_UNUSED(col_value);
	QSqlQuery query(sqldb);
	int numRows = 0;

	if (sql_col.isEmpty())
		sql_col = "COUNT(*)";

	query.exec("SELECT "+ sql_col +" FROM "+ tabla +" "+ sql_where);
	if (chequearQuery(query))
	{
		if (sql_col == "COUNT(*)")
		{
			query.next();
			numRows = query.value(0).toInt();
		} else {
			if (sqldb.driver()->hasFeature(QSqlDriver::QuerySize))
				numRows = query.size();
			else {
			// this can be very slow
				query.last();
				numRows = query.at() + 1;
			}
//			do {
//				QStringList list = query.record().value(sql_col).toString().split(";", QString::SkipEmptyParts);
//				if (list.contains(col_value, Qt::CaseInsensitive))
//					++numRows;
//			} while (query.next());
		}
	} else
		numRows = 0;

	if (numRows < 0)
		numRows = 0;

	return numRows;
}

// Devuelve los datos correspondientes a la tabla, id y columnas indicadas
QHash<QString, QString> dbSql::getDatos(QString tabla, QString sql_where, QString sql_col, QString sep)
{
	QSqlQuery query(sqldb);
	QHash<QString, QString> datos;

	query.exec("SELECT "+ sql_col +" FROM "+ tabla +" "+ sql_where);
	if (query.first())
	{
		QStringList list_col = sql_col.split(sep, QString::SkipEmptyParts);

		datos.clear();
		const int listSize = list_col.size();
		for (int i = 0; i < listSize; ++i)
			datos.insert(list_col.at(i), query.record().value(list_col.value(i)).toString());
	}

	return datos;
}

// Actualiza el orden de un QTreeWidget y en la base de datos
void dbSql::actualizaOrdenTreeWidget(QTreeWidget *twOrden, QString tabla, QString col_id, QString col_orden, int coltw_id, int coltw_order)
{
	QSqlQuery query(sqldb);

	const int count_orden = twOrden->topLevelItemCount();
	if (count_orden > 0)
	{
		for (int orden = 0; orden < count_orden; ++orden)
		{
			QTreeWidgetItem *item = twOrden->topLevelItem(orden);
			item->setText(coltw_order, QVariant(orden).toString());

			query.prepare("UPDATE "+ tabla +" SET "+ col_orden +" = :"+ col_orden +" WHERE "+ col_id +" = :"+ col_id +";");
			query.bindValue(":"+ col_orden, orden);
			query.bindValue(":"+ col_id, item->text(coltw_id));
			query.exec();

			chequearQuery(query);
		}
	}
}

// Existe columna de una tabla
bool dbSql::comprobarColumnaTabla(QString tabla, QString columna)
{
	QSqlQuery query(sqldb);
	query.exec("SELECT * FROM "+ tabla +" LIMIT 1;");

	bool existe = false;

	if (chequearQuery(query))
	{
		if (query.first())
			existe = query.record().value(columna).isValid();
	}

	return existe;
}

// Renombrar una tabla
bool dbSql::renombrarTabla(QString old_name, QString new_name)
{
// Obtenemos las tablas de la base de datos.
	QStringList lista_tablas = listarTablas();

	if (lista_tablas.contains(new_name, Qt::CaseInsensitive) || !lista_tablas.contains(old_name, Qt::CaseInsensitive))
		return false;
	else {
		QSqlQuery query(sqldb);
		query.exec("ALTER TABLE "+ old_name +" RENAME TO "+ new_name +";");

		return chequearQuery(query);
	}
}

bool dbSql::eliminarTabla(QString tabla)
{
	QSqlQuery query(sqldb);
	query.exec("DROP TABLE "+ tabla +";");

	return chequearQuery(query);
}

// Crea una tabla nueva para Datos
bool dbSql::crearTablaDatos(QString tabla)
{
	if (tabla.isEmpty())
		return false;
	else {
		QSqlQuery query(sqldb);

		query.exec("CREATE TABLE IF NOT EXISTS "+ tabla +" ("
		"	idgrl					"+ dbTypeId +","
		"	icono					VARCHAR(255) NOT NULL DEFAULT '',"
		"	titulo					VARCHAR(255) NOT NULL DEFAULT '',"
		"	titulo_guiones			VARCHAR(255) NOT NULL DEFAULT '',"
		"	subtitulo				VARCHAR(255) NOT NULL DEFAULT '',"
		"	genero					VARCHAR(255) NOT NULL DEFAULT '',"
		"	compania				VARCHAR(255) NOT NULL DEFAULT '',"
		"	desarrollador			VARCHAR(255) NOT NULL DEFAULT '',"
		"	tema					VARCHAR(255) NOT NULL DEFAULT '',"
		"	grupo					VARCHAR(255) NOT NULL DEFAULT '',"
		"	perspectiva				VARCHAR(255) NOT NULL DEFAULT '',"
		"	idioma					VARCHAR(50) NOT NULL DEFAULT '',"
		"	idioma_voces			VARCHAR(50) NOT NULL DEFAULT '',"
		"	formato					VARCHAR(50) NOT NULL DEFAULT '',"
		"	anno					VARCHAR(4) NOT NULL DEFAULT '',"
		"	numdisc					VARCHAR(50) NOT NULL DEFAULT '',"
		"	sistemaop				VARCHAR(200) NOT NULL DEFAULT '',"
		"	tamano					VARCHAR(50) NOT NULL DEFAULT '',"
		"	graficos				INTEGER NOT NULL DEFAULT 0,"
		"	sonido					INTEGER NOT NULL DEFAULT 0,"
		"	jugabilidad				INTEGER NOT NULL DEFAULT 0,"
		"	original				VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	estado					VARCHAR(255) NOT NULL DEFAULT '',"
		"	thumbs					VARCHAR(255) NOT NULL DEFAULT '',"
		"	cover_front				VARCHAR(255) NOT NULL DEFAULT '',"
		"	cover_back				VARCHAR(255) NOT NULL DEFAULT '',"
		"	cover_left				VARCHAR(255) NOT NULL DEFAULT '',"
		"	cover_right				VARCHAR(255) NOT NULL DEFAULT '',"
		"	cover_top				VARCHAR(255) NOT NULL DEFAULT '',"
		"	cover_bottom			VARCHAR(255) NOT NULL DEFAULT '',"
		"	fecha					VARCHAR(50) NOT NULL DEFAULT '',"
		"	tipo_emu				VARCHAR(255) NOT NULL DEFAULT 'datos',"
		"	comentario				"+ qpsql_text +","
		"	favorito				VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	gamepad					VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	rating					INTEGER NOT NULL DEFAULT 0,"
		"	edad_recomendada		VARCHAR(2) NOT NULL DEFAULT 'nd',"
		"	usuario					VARCHAR(255) NOT NULL DEFAULT '',"
		"	path_exe				VARCHAR(255) NOT NULL DEFAULT '',"
		"	parametros_exe			VARCHAR(255) NOT NULL DEFAULT '',"
		"	compatibilidad_exe		VARCHAR(255) NOT NULL DEFAULT 'false|',"
		"	path_setup				VARCHAR(255) NOT NULL DEFAULT '',"
		"	parametros_setup		VARCHAR(255) NOT NULL DEFAULT '',"
		"	compatibilidad_setup	VARCHAR(255) NOT NULL DEFAULT 'false|',"
		"	"+ qpsql_pkey(tabla) +" PRIMARY KEY(idgrl)"
		") "+ qpsql_oids +";");

		return chequearQuery(query);
	}
}

// Crea la tabla dbgrl_mnu_nav
bool dbSql::crearTablaMnuNav(bool isTemp)
{
	QSqlQuery query(sqldb);
	QString temporal = isTemp ? "_temp" : "";

	query.exec("CREATE TABLE IF NOT EXISTS dbgrl_mnu_nav"+ temporal +" ("
	"	id						"+ dbTypeId +","
	"	titulo					VARCHAR(255) NOT NULL DEFAULT '',"
	"	col_value				VARCHAR(255) NOT NULL DEFAULT '',"
	"	col_name				VARCHAR(255) NOT NULL DEFAULT '',"
	"	sql_query				TEXT,"
	"	archivo					VARCHAR(255) NOT NULL DEFAULT '',"
	"	img						VARCHAR(255) NOT NULL DEFAULT 'sinimg.png',"
	"	orden					INTEGER NOT NULL DEFAULT 0,"
	"	mostrar					VARCHAR(5) NOT NULL DEFAULT 'true',"
	"	expanded				VARCHAR(5) NOT NULL DEFAULT 'true',"
	"	"+ qpsql_pkey("dbgrl_mnu_nav") +" PRIMARY KEY (id)"
	") "+ qpsql_oids +";");

	return chequearQuery(query);
}

// Crea las tablas de la base de datos si es necesario
void dbSql::crearTablas()
{
	QSqlQuery query(sqldb);
	QStringList lista_tablas = listarTablas();

// Renombramos tabla
	renombrarTabla("dbgrl_file", "dbgrl_files");
	renombrarTabla("dbgrl_url" , "dbgrl_urls");

// Añade las distintas tablas si no están disponibles
// Tabla principal - dbgrl
	if (!lista_tablas.contains("dbgrl"))
		crearTablaDatos("dbgrl");

// Tabla - dbgrl_emu_dosbox
	if (!lista_tablas.contains("dbgrl_emu_dosbox"))
	{
		query.exec("CREATE TABLE IF NOT EXISTS dbgrl_emu_dosbox ("
		"	id						"+ dbTypeId +","
		"	idgrl					INTEGER NOT NULL DEFAULT 1,"
		"	idcat					INTEGER NOT NULL DEFAULT 1,"
		"	sdl_fullscreen			VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	sdl_fulldouble			VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	sdl_fullfixed			VARCHAR(5) NOT NULL DEFAULT 'false',"		// DOSBox 0.63
		"	sdl_fullresolution		VARCHAR(15) NOT NULL DEFAULT 'original',"
		"	sdl_windowresolution	VARCHAR(15) NOT NULL DEFAULT 'original',"
		"	sdl_output				VARCHAR(15) NOT NULL DEFAULT 'surface',"
		"	sdl_hwscale				VARCHAR(15) NOT NULL DEFAULT '1.00',"		// DOSBox 0.63
		"	sdl_autolock			VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	sdl_sensitivity			VARCHAR(3) NOT NULL DEFAULT '100',"
		"	sdl_waitonerror			VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	sdl_priority			VARCHAR(20) NOT NULL DEFAULT 'higher,normal',"
		"	sdl_mapperfile			VARCHAR(255) NOT NULL DEFAULT 'mapper.txt',"
		"	sdl_usescancodes		VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	dosbox_language			VARCHAR(255) NOT NULL DEFAULT '',"
		"	dosbox_machine			VARCHAR(20) NOT NULL DEFAULT 'svga_s3',"
		"	dosbox_memsize			VARCHAR(4) NOT NULL DEFAULT '16',"
		"	dosbox_emu_key			VARCHAR(255) NOT NULL DEFAULT 'dosbox',"
		"	render_frameskip		VARCHAR(2) NOT NULL DEFAULT '0',"
		"	render_aspect			VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	render_scaler			VARCHAR(15) NOT NULL DEFAULT 'normal2x',"
		"	cpu_core				VARCHAR(10) NOT NULL DEFAULT 'auto',"
		"	cpu_cputype				VARCHAR(20) NOT NULL DEFAULT 'auto',"
		"	cpu_cycles				VARCHAR(10) NOT NULL DEFAULT 'auto',"
		"	cpu_cycles_realmode		VARCHAR(10) NOT NULL DEFAULT '',"
		"	cpu_cycles_protmode		VARCHAR(10) NOT NULL DEFAULT '',"
		"	cpu_cycles_limitmode	VARCHAR(10) NOT NULL DEFAULT '',"
		"	cpu_cycleup				VARCHAR(10) NOT NULL DEFAULT '10',"
		"	cpu_cycledown			VARCHAR(10) NOT NULL DEFAULT '20',"
		"	mixer_nosound			VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	mixer_rate				VARCHAR(10) NOT NULL DEFAULT '44100',"
		"	mixer_blocksize			VARCHAR(10) NOT NULL DEFAULT '1024',"
		"	mixer_prebuffer			VARCHAR(10) NOT NULL DEFAULT '20',"
		"	midi_mpu401				VARCHAR(15) NOT NULL DEFAULT 'intelligent',"
		"	midi_intelligent		VARCHAR(5) NOT NULL DEFAULT 'true',"		// DOSBox 0.63
		"	midi_device				VARCHAR(10) NOT NULL DEFAULT 'default',"
		"	midi_config				VARCHAR(255) NOT NULL DEFAULT '',"
		"	midi_mt32rate			VARCHAR(10) NOT NULL DEFAULT '44100',"		// DOSBox 0.65
		"	sblaster_sbtype			VARCHAR(10) NOT NULL DEFAULT 'sb16',"
		"	sblaster_sbbase			VARCHAR(4) NOT NULL DEFAULT '220',"
		"	sblaster_irq			VARCHAR(4) NOT NULL DEFAULT '7',"
		"	sblaster_dma			VARCHAR(4) NOT NULL DEFAULT '1',"
		"	sblaster_hdma			VARCHAR(4) NOT NULL DEFAULT '5',"
		"	sblaster_mixer			VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	sblaster_oplmode		VARCHAR(10) NOT NULL DEFAULT 'auto',"
		"	sblaster_oplemu			VARCHAR(10) NOT NULL DEFAULT 'default',"
		"	sblaster_oplrate		VARCHAR(10) NOT NULL DEFAULT '44100',"
		"	gus_gus					VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	gus_gusrate				VARCHAR(10) NOT NULL DEFAULT '44100',"
		"	gus_gusbase				VARCHAR(4) NOT NULL DEFAULT '240',"
		"	gus_irq1				VARCHAR(4) NOT NULL DEFAULT '5',"
		"	gus_irq2				VARCHAR(4) NOT NULL DEFAULT '5',"
		"	gus_dma1				VARCHAR(4) NOT NULL DEFAULT '3',"
		"	gus_dma2				VARCHAR(4) NOT NULL DEFAULT '3',"
		"	gus_ultradir			VARCHAR(255) NOT NULL DEFAULT 'C:\\ULTRASND',"
		"	speaker_pcspeaker		VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	speaker_pcrate			VARCHAR(10) NOT NULL DEFAULT '44100',"
		"	speaker_tandy			VARCHAR(5) NOT NULL DEFAULT 'auto',"		// DOSBox 0.65
		"	speaker_tandyrate		VARCHAR(10) NOT NULL DEFAULT '44100',"
		"	speaker_disney			VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	joystick_type			VARCHAR(50) NOT NULL DEFAULT 'auto',"		// DOSBox 0.70
		"	joystick_timed			VARCHAR(5) NOT NULL DEFAULT 'true',"		// DOSBox 0.70
		"	joystick_autofire		VARCHAR(5) NOT NULL DEFAULT 'false',"		// DOSBox 0.70
		"	joystick_swap34			VARCHAR(5) NOT NULL DEFAULT 'false',"		// DOSBox 0.70
		"	joystick_buttonwrap		VARCHAR(5) NOT NULL DEFAULT 'false',"		// DOSBox 0.70
		"	modem_modem				VARCHAR(5) NOT NULL DEFAULT 'false',"		// DOSBox 0.63
		"	modem_comport			VARCHAR(4) NOT NULL DEFAULT '2',"			// DOSBox 0.63
		"	modem_listenport		VARCHAR(4) NOT NULL DEFAULT '23',"			// DOSBox 0.63
		"	dserial_directserial	VARCHAR(5) NOT NULL DEFAULT 'false',"		// DOSBox 0.63
		"	dserial_comport			VARCHAR(2) NOT NULL DEFAULT '1',"			// DOSBox 0.63
		"	dserial_realport		VARCHAR(5) NOT NULL DEFAULT 'COM1',"		// DOSBox 0.63
		"	dserial_defaultbps		VARCHAR(5) NOT NULL DEFAULT '1200',"		// DOSBox 0.63
		"	dserial_parity			VARCHAR(2) NOT NULL DEFAULT 'N',"			// DOSBox 0.63
		"	dserial_bytesize		VARCHAR(2) NOT NULL DEFAULT '8',"			// DOSBox 0.63
		"	dserial_stopbit			VARCHAR(2) NOT NULL DEFAULT '1',"			// DOSBox 0.63
		"	serial_1				TEXT,"
		"	serial_2				TEXT,"
		"	serial_3				TEXT,"
		"	serial_4				TEXT,"
		"	dos_xms					VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	dos_ems					VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	dos_umb					VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	dos_keyboardlayout		VARCHAR(5) NOT NULL DEFAULT 'auto',"
		"	dos_version				VARCHAR(5) NOT NULL DEFAULT '',"
		"	ipx_ipx					VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	ipx_type				VARCHAR(10) NOT NULL DEFAULT 'none',"
		"	ipx_port				VARCHAR(5) NOT NULL DEFAULT '213',"
		"	ipx_ip					VARCHAR(255) NOT NULL DEFAULT '',"
		"	autoexec				"+ qpsql_text +","
		"	autoexec_ini			"+ qpsql_text +","
		"	autoexec_fin			"+ qpsql_text +","
		"	autoexec_ini_exe		"+ qpsql_text +","
		"	autoexec_fin_exe		"+ qpsql_text +","
		"	opt_autoexec			VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	opt_loadfix				VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	opt_loadfix_mem			VARCHAR(5) NOT NULL DEFAULT '64',"
		"	opt_consola_dbox		VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	opt_cerrar_dbox			VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	opt_cycle_sincronizar	VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	path_exe				VARCHAR(255) NOT NULL DEFAULT '',"
		"	path_setup				VARCHAR(255) NOT NULL DEFAULT '',"
		"	parametros_exe			VARCHAR(255) NOT NULL DEFAULT '',"
		"	parametros_setup		VARCHAR(255) NOT NULL DEFAULT '',"
		"	"+ qpsql_pkey("dbgrl_emu_dosbox") +" PRIMARY KEY(id)"
		") "+ qpsql_oids +";");

		chequearQuery(query);
	}

// Tabla - dbgrl_emu_dosbox_mount
	if (!lista_tablas.contains("dbgrl_emu_dosbox_mount"))
	{
		query.exec("CREATE TABLE IF NOT EXISTS dbgrl_emu_dosbox_mount ("
		"	id						"+ dbTypeId +","
		"	id_dosbox				INTEGER NOT NULL DEFAULT 1,"
		"	id_lista				VARCHAR(255) NOT NULL DEFAULT '0',"
		"	path					VARCHAR(255) NOT NULL DEFAULT '',"
		"	label					VARCHAR(255) NOT NULL DEFAULT '',"
		"	tipo_as					VARCHAR(50) NOT NULL DEFAULT 'drive',"
		"	letter					VARCHAR(255) NOT NULL DEFAULT 'C',"
		"	indx_cd					VARCHAR(255) NOT NULL DEFAULT '',"
		"	opt_mount				VARCHAR(255) NOT NULL DEFAULT '',"
		"	io_ctrl					VARCHAR(50) NOT NULL DEFAULT '',"
		"	select_mount			VARCHAR(10) NOT NULL DEFAULT 'x',"
		"	opt_size				VARCHAR(255) NOT NULL DEFAULT '',"
		"	opt_freesize			VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	freesize				INTEGER NOT NULL DEFAULT 250,"
		"	"+ qpsql_pkey("dbgrl_emu_dosbox_mount") +" PRIMARY KEY(id)"
		") "+ qpsql_oids +";");

		chequearQuery(query);
	}

// Tabla - dbgrl_emu_scummvm
	if (!lista_tablas.contains("dbgrl_emu_scummvm"))
	{
		query.exec("CREATE TABLE IF NOT EXISTS dbgrl_emu_scummvm ("
		"	id						"+ dbTypeId +","
		"	idgrl					INTEGER NOT NULL DEFAULT 1,"
		"	idcat					INTEGER NOT NULL DEFAULT 1,"
		"	game					VARCHAR(255) NOT NULL DEFAULT '',"
		"	game_label				VARCHAR(255) NOT NULL DEFAULT '',"
		"	language				VARCHAR(4) NOT NULL DEFAULT 'es',"
		"	subtitles				VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	platform				VARCHAR(50) NOT NULL DEFAULT 'pc',"
		"	gfx_mode				VARCHAR(50) NOT NULL DEFAULT '2x',"
		"	render_mode				VARCHAR(50) NOT NULL DEFAULT '',"
		"	fullscreen				VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	filtering				VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	aspect_ratio			VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	path					VARCHAR(255) NOT NULL DEFAULT '',"
		"	path_extra				VARCHAR(255) NOT NULL DEFAULT '',"
		"	path_save				VARCHAR(255) NOT NULL DEFAULT '',"
		"	music_driver			VARCHAR(50) NOT NULL DEFAULT 'adlib',"
		"	enable_gs				VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	multi_midi				VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	native_mt32				VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	mute					VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	master_volume			INTEGER NOT NULL DEFAULT 192,"
		"	music_volume			INTEGER NOT NULL DEFAULT 192,"
		"	sfx_volume				INTEGER NOT NULL DEFAULT 192,"
		"	speech_volume			INTEGER NOT NULL DEFAULT 192,"
		"	speech_mute				VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	tempo					INTEGER NOT NULL DEFAULT 100,"
		"	talkspeed				INTEGER NOT NULL DEFAULT 60,"
		"	debuglevel				INTEGER NOT NULL DEFAULT 0,"
		"	cdrom					INTEGER NOT NULL DEFAULT 0,"
		"	joystick_num			INTEGER NOT NULL DEFAULT 0,"
		"	output_rate				VARCHAR(10) NOT NULL DEFAULT '',"
		"	midi_gain				INTEGER NOT NULL DEFAULT 100,"
		"	copy_protection			VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	sound_font				VARCHAR(255) NOT NULL DEFAULT '',"
		"	walkspeed				INTEGER NOT NULL DEFAULT 0,"
		"	opl_driver				VARCHAR(10) NOT NULL DEFAULT 'auto',"
		"	disable_dithering		VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	alt_intro				VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	boot_param				VARCHAR(15) NOT NULL DEFAULT '',"
		"	autosave_period			VARCHAR(4) NOT NULL DEFAULT '300',"
		"	originalsaveload		VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	bright_palette			VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	guioptions				VARCHAR(255) NOT NULL DEFAULT '',"
		"	"+ qpsql_pkey("dbgrl_emu_scummvm") +" PRIMARY KEY(id)"
		") "+ qpsql_oids +";");

		chequearQuery(query);
	}

// Tabla - dbgrl_emu_vdmsound
	if (!lista_tablas.contains("dbgrl_emu_vdmsound"))
	{
		query.exec("CREATE TABLE IF NOT EXISTS dbgrl_emu_vdmsound ("
		"	id						"+ dbTypeId +","
		"	idgrl					INTEGER NOT NULL DEFAULT 1,"
		"	idcat					INTEGER NOT NULL DEFAULT 1,"
		"	path_exe				TEXT,"
		"	program					TEXT,"
		"	vdms_debug				TEXT,"
		"	winnt_dos				TEXT,"
		"	winnt_dosbox			TEXT,"
		"	winnt_storage			TEXT,"
		"	"+ qpsql_pkey("dbgrl_emu_vdmsound") +" PRIMARY KEY(id)"
		") "+ qpsql_oids +";");

		chequearQuery(query);
	}

// Tabla - dbgrl_files
	if (!lista_tablas.contains("dbgrl_files"))
	{
		query.exec("CREATE TABLE IF NOT EXISTS dbgrl_files ("
		"	id						"+ dbTypeId +","
		"	idgrl					INTEGER NOT NULL DEFAULT 1,"
		"	idcat					INTEGER NOT NULL DEFAULT 1,"
		"	nombre					VARCHAR(255) NOT NULL DEFAULT '',"
		"	crc						VARCHAR(8) NOT NULL DEFAULT '00000000',"
		"	descripcion				VARCHAR(255) NOT NULL DEFAULT '',"
		"	path					VARCHAR(255) NOT NULL DEFAULT '',"
		"	size					VARCHAR(50) NOT NULL DEFAULT '',"
		"	tipo					VARCHAR(50) NOT NULL DEFAULT '',"
		"	"+ qpsql_pkey("dbgrl_files") +" PRIMARY KEY(id)"
		") "+ qpsql_oids +";");

		chequearQuery(query);
	}

// Tabla - dbgrl_urls
	if (!lista_tablas.contains("dbgrl_urls"))
	{
		query.exec("CREATE TABLE IF NOT EXISTS dbgrl_urls ("
		"	id						"+ dbTypeId +","
		"	idgrl					INTEGER NOT NULL DEFAULT 1,"
		"	idcat					INTEGER NOT NULL DEFAULT 1,"
		"	url						VARCHAR(255) NOT NULL DEFAULT '',"
		"	descripcion				VARCHAR(255) NOT NULL DEFAULT '',"
		"	"+ qpsql_pkey("dbgrl_urls") +" PRIMARY KEY(id)"
		") "+ qpsql_oids +";");

		chequearQuery(query);
	}

// Tabla - dbgrl_categorias
	if (!lista_tablas.contains("dbgrl_categorias"))
	{
		query.exec("CREATE TABLE IF NOT EXISTS dbgrl_categorias ("
		"	id						"+ dbTypeId +","
		"	tabla					VARCHAR(255) NOT NULL DEFAULT 'dbgrl',"
		"	titulo					VARCHAR(255) NOT NULL DEFAULT '',"
		"	img						VARCHAR(255) NOT NULL DEFAULT 'sinimg.png',"
		"	orden					INTEGER NOT NULL DEFAULT 0,"
		"	emu_show				TEXT,"
		"	"+ qpsql_pkey("dbgrl_categorias") +" PRIMARY KEY (id)"
		") "+ qpsql_oids +";");

		if (chequearQuery(query))
		{
			stGrlCats cat_def;
			cat_def.tabla    = "dbgrl";
			cat_def.titulo   = "Db GR-lida";
			cat_def.img      = "cat/pc.png";
			cat_def.orden    = "0";
			cat_def.emu_show = "all";
			cat_def.total    = 0;
			insertaCategoria(cat_def);
		}
	}

// comprobar si categiria esta vacia para añadir almenos una

// Tabla - dbgrl_mnu_nav
	if (!lista_tablas.contains("dbgrl_mnu_nav"))
	{
		if (crearTablaMnuNav())
		{
			insertaMenuNav(tr("Generos")        , "{value}", "genero"       , "WHERE {col_name} LIKE ('%{col_value}%')", "generos.txt"  , "datos_3.png", 0, true, true);
			insertaMenuNav(tr("Temas")          , "{value}", "tema"         , "WHERE {col_name} LIKE ('%{col_value}%')", "temas.txt"    , "datos_3.png", 1, true, true);
			insertaMenuNav(tr("Desarrolladores"), "{value}", "desarrollador", "WHERE {col_name} LIKE ('%{col_value}%')", "companias.txt", "datos_3.png", 2, true, true);
			insertaMenuNav(tr("Compañias")      , "{value}", "compania"     , "WHERE {col_name} LIKE ('%{col_value}%')", "companias.txt", "datos_3.png", 3, true, true);
			insertaMenuNav(tr("Años")           , "{value}", "anno"         , "WHERE {col_name} LIKE ('%{col_value}%')", "fechas.txt"   , "fecha.png"  , 4, true, true);
			insertaMenuNav(tr("Idiomas")        , "{value}", "idioma"       , "WHERE {col_name} LIKE ('%{col_value}%')", "idiomas.txt"  , "idiomas.png", 5, true, true);
		}
	}

// Tabla - dbgrl_mnu_shortcut
	if (!lista_tablas.contains("dbgrl_mnu_shortcut"))
	{
		query.exec("CREATE TABLE IF NOT EXISTS dbgrl_mnu_shortcut ("
		"	id						"+ dbTypeId +","
		"	titulo					VARCHAR(255) NOT NULL DEFAULT '',"
		"	key_sequence			VARCHAR(255) NOT NULL DEFAULT '',"
		"	sql_query				TEXT,"
		"	img						VARCHAR(255) NOT NULL DEFAULT 'sinimg.png',"
		"	orden					INTEGER NOT NULL DEFAULT 0,"
		"	mostrar					VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	separador				VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	"+ qpsql_pkey("dbgrl_mnu_shortcut") +" PRIMARY KEY (id)"
		") "+ qpsql_oids +";");

		insertaMnuShortcut(tr("Todos"), "", "", "gr-lida.png", 0, true, false);
	}

// Añade las columnas si no están disponibles
// Tabla - dbgrl
	QSqlQuery query_cat(sqldb);
	query_cat.exec("SELECT tabla FROM dbgrl_categorias;");
	if (query_cat.first())
	{
		do {
			QString tabla = query_cat.record().value("tabla").toString();

			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN grupo VARCHAR(255) NOT NULL DEFAULT '';");
			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN perspectiva VARCHAR(255) NOT NULL DEFAULT '';");
			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN idioma_voces VARCHAR(50) NOT NULL DEFAULT '';");
			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN favorito VARCHAR(5) NOT NULL DEFAULT 'false';");
			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN gamepad VARCHAR(5) NOT NULL DEFAULT 'false';");
			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN rating INTEGER NOT NULL DEFAULT 0;");
			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN edad_recomendada VARCHAR(2) NOT NULL DEFAULT 'nd';");
			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN usuario VARCHAR(255) NOT NULL DEFAULT '';");
			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN path_exe VARCHAR(255) NOT NULL DEFAULT '';");
			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN parametros_exe VARCHAR(255) NOT NULL DEFAULT '';");
			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN path_setup VARCHAR(255) NOT NULL DEFAULT '';");
			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN parametros_setup VARCHAR(255) NOT NULL DEFAULT '';");
			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN cover_left VARCHAR(255) NOT NULL DEFAULT '';");
			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN cover_right VARCHAR(255) NOT NULL DEFAULT '';");
			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN cover_top VARCHAR(255) NOT NULL DEFAULT '';");
			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN cover_bottom VARCHAR(255) NOT NULL DEFAULT '';");
			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN titulo_guiones VARCHAR(255) NOT NULL DEFAULT '';");
			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN compatibilidad_exe VARCHAR(255) NOT NULL DEFAULT 'false|';");
			query.exec("ALTER TABLE "+ tabla +" ADD COLUMN compatibilidad_setup VARCHAR(255) NOT NULL DEFAULT 'false|';");
		} while (query_cat.next());
	}
	query_cat.clear();

// Tabla - dbgrl_emu_dosbox
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN idcat INTEGER NOT NULL DEFAULT 1;");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN cpu_cputype VARCHAR(20) NOT NULL DEFAULT 'auto';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN sblaster_oplemu VARCHAR(10) NOT NULL DEFAULT 'default';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN cpu_cycles_realmode VARCHAR(10) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN cpu_cycles_protmode VARCHAR(10) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN cpu_cycles_limitmode VARCHAR(10) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN dos_version VARCHAR(5) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN ipx_type VARCHAR(5) NOT NULL DEFAULT 'none';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN ipx_port VARCHAR(5) NOT NULL DEFAULT '213';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN ipx_ip VARCHAR(255) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN dosbox_emu_key VARCHAR(255) NOT NULL DEFAULT 'dosbox';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN autoexec_ini "+ qpsql_text +";");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN autoexec_fin "+ qpsql_text +";");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN autoexec_ini_exe "+ qpsql_text +";");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN autoexec_fin_exe "+ qpsql_text +";");
// Tabla - dbgrl_emu_dosbox_mount
	query.exec("ALTER TABLE dbgrl_emu_dosbox_mount ADD COLUMN opt_size VARCHAR(255) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox_mount ADD COLUMN opt_freesize VARCHAR(5) NOT NULL DEFAULT 'false';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox_mount ADD COLUMN freesize INTEGER NOT NULL DEFAULT 250;");
// Tabla - dbgrl_emu_scummvm
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN idcat INTEGER NOT NULL DEFAULT 1;");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN output_rate VARCHAR(10) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN midi_gain INTEGER NOT NULL DEFAULT 100;");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN copy_protection VARCHAR(5) NOT NULL DEFAULT 'false';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN sound_font VARCHAR(255) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN game_label VARCHAR(255) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN walkspeed INTEGER NOT NULL DEFAULT 0;");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN opl_driver VARCHAR(10) NOT NULL DEFAULT 'auto';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN disable_dithering VARCHAR(5) NOT NULL DEFAULT 'false';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN mute VARCHAR(5) NOT NULL DEFAULT 'false';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN speech_mute VARCHAR(5) NOT NULL DEFAULT 'false';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN alt_intro VARCHAR(5) NOT NULL DEFAULT 'false';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN boot_param VARCHAR(5) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN autosave_period VARCHAR(4) NOT NULL DEFAULT '300';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN filtering VARCHAR(5) NOT NULL DEFAULT 'false';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN originalsaveload VARCHAR(5) NOT NULL DEFAULT 'false';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN bright_palette VARCHAR(5) NOT NULL DEFAULT 'false';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN guioptions VARCHAR(255) NOT NULL DEFAULT '';");
// Tabla - dbgrl_emu_vdmsound
	query.exec("ALTER TABLE dbgrl_emu_vdmsound ADD COLUMN idcat INTEGER NOT NULL DEFAULT 1;");
// Tabla - dbgrl_files
	query.exec("ALTER TABLE dbgrl_files ADD COLUMN idcat INTEGER NOT NULL DEFAULT 1;");
	query.exec("ALTER TABLE dbgrl_files ADD COLUMN tipo VARCHAR(50) NOT NULL DEFAULT '';");
// Tabla - dbgrl_urls
	query.exec("ALTER TABLE dbgrl_urls ADD COLUMN idcat INTEGER NOT NULL DEFAULT 1;");

// Guardamos los cambios en la base de datos.
	comit();
}

// INICIO Datos Juego -------------------------------------------------------------------------------------------
// Datos por defecto
QString dbSql::setDefDatValue(bool isQHash, QString value, QString def)
{
	if (isQHash)
		return value.isEmpty() ? def : value;
	else
		return def;
}

stDatosJuego dbSql::getDefectDatos(QString icono, QHash<QString, QString> dato)
{
	bool isQHash = dato.isEmpty() ? false : true;
	icono = icono.isEmpty() ? "datos.png" : icono;
	stDatosJuego datos;
	datos.idgrl = setDefDatValue(isQHash, dato["Dat_idgrl"], "");
//--
	datos.icono                = setDefDatValue(isQHash, dato["Dat_icono"]               , icono);
	datos.titulo               = setDefDatValue(isQHash, dato["Dat_titulo"]              , "");
	datos.titulo_guiones       = setDefDatValue(isQHash, dato["Dat_titulo_guiones"]      , (!datos.titulo.isEmpty() ? removeAccents(datos.titulo) : ""));
	datos.subtitulo            = setDefDatValue(isQHash, dato["Dat_subtitulo"]           , "");
	datos.genero               = setDefDatValue(isQHash, dato["Dat_genero"]              , "");
	datos.compania             = setDefDatValue(isQHash, dato["Dat_compania"]            , "");
	datos.desarrollador        = setDefDatValue(isQHash, dato["Dat_desarrollador"]       , "");
	datos.tema                 = setDefDatValue(isQHash, dato["Dat_tema"]                , "");
	datos.grupo                = setDefDatValue(isQHash, dato["Dat_grupo"]               , "");
	datos.perspectiva          = setDefDatValue(isQHash, dato["Dat_perspectiva"]         , "");
	datos.idioma               = setDefDatValue(isQHash, dato["Dat_idioma"]              , "");
	datos.idioma_voces         = setDefDatValue(isQHash, dato["Dat_idioma_voces"]        , "");
	datos.formato              = setDefDatValue(isQHash, dato["Dat_formato"]             , "");
	datos.anno                 = setDefDatValue(isQHash, dato["Dat_anno"]                , "");
	datos.numdisc              = setDefDatValue(isQHash, dato["Dat_numdisc"]             , "");
	datos.sistemaop            = setDefDatValue(isQHash, dato["Dat_sistemaop"]           , "");
	datos.tamano               = setDefDatValue(isQHash, dato["Dat_tamano"]              , "");
	datos.graficos             = setDefDatValue(isQHash, dato["Dat_graficos"]            , "0");
	datos.sonido               = setDefDatValue(isQHash, dato["Dat_sonido"]              , "0");
	datos.jugabilidad          = setDefDatValue(isQHash, dato["Dat_jugabilidad"]         , "0");
	datos.original             = setDefDatValue(isQHash, dato["Dat_original"]            , "false");
	datos.estado               = setDefDatValue(isQHash, dato["Dat_estado"]              , "");
	datos.thumbs               = setDefDatValue(isQHash, dato["Dat_thumbs"]              , "");
	datos.cover_front          = setDefDatValue(isQHash, dato["Dat_cover_front"]         , "");
	datos.cover_back           = setDefDatValue(isQHash, dato["Dat_cover_back"]          , "");
	datos.cover_left           = setDefDatValue(isQHash, dato["Dat_cover_left"]          , "");
	datos.cover_right          = setDefDatValue(isQHash, dato["Dat_cover_right"]         , "");
	datos.cover_top            = setDefDatValue(isQHash, dato["Dat_cover_top"]           , "");
	datos.cover_bottom         = setDefDatValue(isQHash, dato["Dat_cover_bottom"]        , "");
	datos.fecha                = setDefDatValue(isQHash, dato["Dat_fecha"]               , QVariant(QDateTime::currentDateTime().toTime_t()).toString());
	datos.tipo_emu             = setDefDatValue(isQHash, dato["Dat_tipo_emu"]            , icono);
	datos.comentario           = setDefDatValue(isQHash, dato["Dat_comentario"]          , "");
	datos.favorito             = setDefDatValue(isQHash, dato["Dat_favorito"]            , "false");
	datos.gamepad              = setDefDatValue(isQHash, dato["Dat_gamepad"]             , "false");
	datos.rating               = setDefDatValue(isQHash, dato["Dat_rating"]              , "0");
	datos.edad_recomendada     = setDefDatValue(isQHash, dato["Dat_edad_recomendada"]    , "nd");
	datos.usuario              = setDefDatValue(isQHash, dato["Dat_usuario"]             , "");
	datos.path_exe             = setDefDatValue(isQHash, dato["Dat_path_exe"]            , "");
	datos.parametros_exe       = setDefDatValue(isQHash, dato["Dat_parametros_exe"]      , "");
	datos.compatibilidad_exe   = setDefDatValue(isQHash, dato["Dat_compatibilidad_exe"]  , "false|");
	datos.path_setup           = setDefDatValue(isQHash, dato["Dat_path_setup"]          , "");
	datos.parametros_setup     = setDefDatValue(isQHash, dato["Dat_parametros_setup"]    , "");
	datos.compatibilidad_setup = setDefDatValue(isQHash, dato["Dat_compatibilidad_setup"], "false|");
// --
	datos.thumbs_url             = setDefDatValue(isQHash, dato["Dat_thumbs_url"]            , "");
	datos.cover_front_url        = setDefDatValue(isQHash, dato["Dat_cover_front_url"]       , "");
	datos.cover_front_url_small  = setDefDatValue(isQHash, dato["Dat_cover_front_url_small"] , "");
	datos.cover_back_url         = setDefDatValue(isQHash, dato["Dat_cover_back_url"]        , "");
	datos.cover_back_url_small   = setDefDatValue(isQHash, dato["Dat_cover_back_url_small"]  , "");
	datos.cover_left_url         = setDefDatValue(isQHash, dato["Dat_cover_left_url"]        , "");
	datos.cover_left_url_small   = setDefDatValue(isQHash, dato["Dat_cover_left_url_small"]  , "");
	datos.cover_right_url        = setDefDatValue(isQHash, dato["Dat_cover_right_url"]       , "");
	datos.cover_right_url_small  = setDefDatValue(isQHash, dato["Dat_cover_right_url_small"] , "");
	datos.cover_top_url          = setDefDatValue(isQHash, dato["Dat_cover_top_url"]         , "");
	datos.cover_top_url_small    = setDefDatValue(isQHash, dato["Dat_cover_top_url_small"]   , "");
	datos.cover_bottom_url       = setDefDatValue(isQHash, dato["Dat_cover_bottom_url"]      , "");
	datos.cover_bottom_url_small = setDefDatValue(isQHash, dato["Dat_cover_bottom_url_small"], "");
// --
	datos.game_dir = "id-"+ datos.idgrl +"_"+ datos.titulo_guiones +"_"+ datos.tipo_emu;

	return datos;
}

stDatosJuego dbSql::show_Datos(QString tabla, QString IDgrl)
{
	QSqlQuery query(sqldb);
	stDatosJuego datos;

	query.exec("SELECT * FROM "+ tabla +" WHERE idgrl="+ IDgrl +" LIMIT 0,1;");
	if (chequearQuery(query) && query.first())
	{
		datos.idgrl                = query.record().value("idgrl").toString();
		datos.icono                = query.record().value("icono").toString();
		datos.titulo               = query.record().value("titulo").toString();
		datos.titulo_guiones       = query.record().value("titulo_guiones").toString();
		datos.subtitulo            = query.record().value("subtitulo").toString();
		datos.genero               = query.record().value("genero").toString();
		datos.compania             = query.record().value("compania").toString();
		datos.desarrollador        = query.record().value("desarrollador").toString();
		datos.tema                 = query.record().value("tema").toString();
		datos.grupo                = query.record().value("grupo").toString();
		datos.perspectiva          = query.record().value("perspectiva").toString();
		datos.idioma               = query.record().value("idioma").toString();
		datos.idioma_voces         = query.record().value("idioma_voces").toString();
		datos.formato              = query.record().value("formato").toString();
		datos.anno                 = query.record().value("anno").toString();
		datos.numdisc              = query.record().value("numdisc").toString();
		datos.sistemaop            = query.record().value("sistemaop").toString();
		datos.tamano               = query.record().value("tamano").toString();
		datos.graficos             = query.record().value("graficos").toString();
		datos.sonido               = query.record().value("sonido").toString();
		datos.jugabilidad          = query.record().value("jugabilidad").toString();
		datos.original             = query.record().value("original").toString().toLower();
		datos.estado               = query.record().value("estado").toString();
		datos.thumbs               = query.record().value("thumbs").toString();
		datos.cover_front          = query.record().value("cover_front").toString();
		datos.cover_back           = query.record().value("cover_back").toString();
		datos.cover_left           = query.record().value("cover_left").toString();
		datos.cover_right          = query.record().value("cover_right").toString();
		datos.cover_top            = query.record().value("cover_top").toString();
		datos.cover_bottom         = query.record().value("cover_bottom").toString();
		datos.fecha                = query.record().value("fecha").toString();
		datos.tipo_emu             = query.record().value("tipo_emu").toString();
		datos.comentario           = query.record().value("comentario").toString();
		datos.favorito             = query.record().value("favorito").toString().toLower();
		datos.gamepad              = query.record().value("gamepad").toString().toLower();
		datos.rating               = query.record().value("rating").toString();
		datos.edad_recomendada     = query.record().value("edad_recomendada").toString();
		datos.usuario              = query.record().value("usuario").toString();
		datos.path_exe             = query.record().value("path_exe").toString();
		datos.parametros_exe       = query.record().value("parametros_exe").toString();
		datos.compatibilidad_exe   = query.record().value("compatibilidad_exe").toString();
		datos.path_setup           = query.record().value("path_setup").toString();
		datos.parametros_setup     = query.record().value("parametros_setup").toString();
		datos.compatibilidad_setup = query.record().value("compatibilidad_setup").toString();
// --
		datos.thumbs_url             = "";
		datos.cover_front_url        = "";
		datos.cover_front_url_small  = "";
		datos.cover_back_url         = "";
		datos.cover_back_url_small   = "";
		datos.cover_left_url         = "";
		datos.cover_left_url_small   = "";
		datos.cover_right_url        = "";
		datos.cover_right_url_small  = "";
		datos.cover_top_url          = "";
		datos.cover_top_url_small    = "";
		datos.cover_bottom_url       = "";
		datos.cover_bottom_url_small = "";
// --
		datos.game_dir = "id-"+ datos.idgrl +"_"+ datos.titulo_guiones +"_"+ datos.tipo_emu;
	} else
		datos = getDefectDatos();

	return datos;
}

QString dbSql::insertaDatos(QString tabla, stDatosJuego datos)
{
	QSqlQuery query(sqldb);
	QString strSQL;
	strSQL = "INSERT INTO "+ tabla +" ("
			"icono, titulo, titulo_guiones, subtitulo, genero, compania, desarrollador, tema, grupo, perspectiva, idioma, idioma_voces, formato, anno, numdisc, "
			"sistemaop, tamano, graficos, sonido, jugabilidad, original, estado, thumbs, cover_front, cover_back, cover_left, cover_right, cover_top, cover_bottom, "
			"fecha, tipo_emu, comentario, favorito, gamepad, rating, edad_recomendada, usuario, path_exe, parametros_exe, compatibilidad_exe, path_setup, parametros_setup, "
			"compatibilidad_setup "
		") VALUES ("
			":icono, :titulo, :titulo_guiones, :subtitulo, :genero, :compania, :desarrollador, :tema, :grupo, :perspectiva, :idioma, :idioma_voces, :formato, :anno, :numdisc, "
			":sistemaop, :tamano, :graficos, :sonido, :jugabilidad, :original, :estado, :thumbs, :cover_front, :cover_back, :cover_left, :cover_right, :cover_top, :cover_bottom, "
			":fecha, :tipo_emu, :comentario, :favorito, :gamepad, :rating, :edad_recomendada, :usuario, :path_exe, :parametros_exe, :compatibilidad_exe, :path_setup, :parametros_setup, "
			":compatibilidad_setup "
		")"+ qpsql_return_id("idgrl");

	query.prepare(strSQL);
	query.bindValue(":icono"               , datos.icono               );
	query.bindValue(":titulo"              , datos.titulo              );
	query.bindValue(":titulo_guiones"      , datos.titulo_guiones      );
	query.bindValue(":subtitulo"           , datos.subtitulo           );
	query.bindValue(":genero"              , datos.genero              );
	query.bindValue(":compania"            , datos.compania            );
	query.bindValue(":desarrollador"       , datos.desarrollador       );
	query.bindValue(":tema"                , datos.tema                );
	query.bindValue(":grupo"               , datos.grupo               );
	query.bindValue(":perspectiva"         , datos.perspectiva         );
	query.bindValue(":idioma"              , datos.idioma              );
	query.bindValue(":idioma_voces"        , datos.idioma_voces        );
	query.bindValue(":formato"             , datos.formato             );
	query.bindValue(":anno"                , datos.anno                );
	query.bindValue(":numdisc"             , datos.numdisc             );
	query.bindValue(":sistemaop"           , datos.sistemaop           );
	query.bindValue(":tamano"              , datos.tamano              );
	query.bindValue(":graficos"            , datos.graficos            );
	query.bindValue(":sonido"              , datos.sonido              );
	query.bindValue(":jugabilidad"         , datos.jugabilidad         );
	query.bindValue(":original"            , datos.original.toLower()  );
	query.bindValue(":estado"              , datos.estado              );
	query.bindValue(":thumbs"              , datos.thumbs              );
	query.bindValue(":cover_front"         , datos.cover_front         );
	query.bindValue(":cover_back"          , datos.cover_back          );
	query.bindValue(":cover_left"          , datos.cover_left          );
	query.bindValue(":cover_right"         , datos.cover_right         );
	query.bindValue(":cover_top"           , datos.cover_top           );
	query.bindValue(":cover_bottom"        , datos.cover_bottom        );
	query.bindValue(":fecha"               , datos.fecha               );
	query.bindValue(":tipo_emu"            , datos.tipo_emu            );
	query.bindValue(":comentario"          , removeHtmlTag(datos.comentario));
	query.bindValue(":favorito"            , datos.favorito.toLower()  );
	query.bindValue(":gamepad"             , datos.gamepad.toLower()   );
	query.bindValue(":rating"              , datos.rating              );
	query.bindValue(":edad_recomendada"    , datos.edad_recomendada    );
	query.bindValue(":usuario"             , datos.usuario             );
	query.bindValue(":path_exe"            , datos.path_exe            );
	query.bindValue(":parametros_exe"      , datos.parametros_exe      );
	query.bindValue(":compatibilidad_exe"  , datos.compatibilidad_exe  );
	query.bindValue(":path_setup"          , datos.path_setup          );
	query.bindValue(":parametros_setup"    , datos.parametros_setup    );
	query.bindValue(":compatibilidad_setup", datos.compatibilidad_setup);
	query.exec();

	if (chequearQuery(query))
		return lastInsertId(query, "idgrl");
	else
		return "";
}

bool dbSql::actualizaDatos(QString tabla, stDatosJuego datos)
{
	QSqlQuery query(sqldb);
	QString strSQL = "UPDATE "+ tabla +" SET "
			"icono = :icono, titulo = :titulo, titulo_guiones = :titulo_guiones, subtitulo = :subtitulo, genero = :genero, "
			"compania = :compania, desarrollador = :desarrollador, tema = :tema, grupo = :grupo, perspectiva = :perspectiva, "
			"idioma = :idioma, idioma_voces = :idioma_voces, formato = :formato, anno = :anno, numdisc = :numdisc, "
			"sistemaop = :sistemaop, tamano = :tamano, graficos = :graficos, sonido = :sonido, jugabilidad = :jugabilidad, "
			"original = :original, estado = :estado, thumbs = :thumbs, cover_front = :cover_front, cover_back = :cover_back, "
			"cover_left = :cover_left, cover_right = :cover_right, cover_top = :cover_top, cover_bottom = :cover_bottom, "
			"fecha = :fecha, tipo_emu = :tipo_emu, comentario = :comentario, favorito = :favorito, gamepad = :gamepad, rating = :rating, "
			"edad_recomendada = :edad_recomendada, usuario = :usuario, path_exe = :path_exe, parametros_exe = :parametros_exe, "
			"compatibilidad_exe = :compatibilidad_exe, path_setup = :path_setup, parametros_setup = :parametros_setup, "
			"compatibilidad_setup = :compatibilidad_setup WHERE idgrl = :idgrl;";

	query.prepare(strSQL);
	query.bindValue(":icono"               , datos.icono               );
	query.bindValue(":titulo"              , datos.titulo              );
	query.bindValue(":titulo_guiones"      , datos.titulo_guiones      );
	query.bindValue(":subtitulo"           , datos.subtitulo           );
	query.bindValue(":genero"              , datos.genero              );
	query.bindValue(":compania"            , datos.compania            );
	query.bindValue(":desarrollador"       , datos.desarrollador       );
	query.bindValue(":tema"                , datos.tema                );
	query.bindValue(":grupo"               , datos.grupo               );
	query.bindValue(":perspectiva"         , datos.perspectiva         );
	query.bindValue(":idioma"              , datos.idioma              );
	query.bindValue(":idioma_voces"        , datos.idioma_voces        );
	query.bindValue(":formato"             , datos.formato             );
	query.bindValue(":anno"                , datos.anno                );
	query.bindValue(":numdisc"             , datos.numdisc             );
	query.bindValue(":sistemaop"           , datos.sistemaop           );
	query.bindValue(":tamano"              , datos.tamano              );
	query.bindValue(":graficos"            , datos.graficos            );
	query.bindValue(":sonido"              , datos.sonido              );
	query.bindValue(":jugabilidad"         , datos.jugabilidad         );
	query.bindValue(":original"            , datos.original            );
	query.bindValue(":estado"              , datos.estado              );
	query.bindValue(":thumbs"              , datos.thumbs              );
	query.bindValue(":cover_front"         , datos.cover_front         );
	query.bindValue(":cover_back"          , datos.cover_back          );
	query.bindValue(":cover_left"          , datos.cover_left          );
	query.bindValue(":cover_right"         , datos.cover_right         );
	query.bindValue(":cover_top"           , datos.cover_top           );
	query.bindValue(":cover_bottom"        , datos.cover_bottom        );
	query.bindValue(":fecha"               , datos.fecha               );
	query.bindValue(":tipo_emu"            , datos.tipo_emu            );
	query.bindValue(":comentario"          , removeHtmlTag(datos.comentario));
	query.bindValue(":favorito"            , datos.favorito            );
	query.bindValue(":gamepad"             , datos.gamepad             );
	query.bindValue(":rating"              , datos.rating              );
	query.bindValue(":edad_recomendada"    , datos.edad_recomendada    );
	query.bindValue(":usuario"             , datos.usuario             );
	query.bindValue(":path_exe"            , datos.path_exe            );
	query.bindValue(":parametros_exe"      , datos.parametros_exe      );
	query.bindValue(":compatibilidad_exe"  , datos.compatibilidad_exe  );
	query.bindValue(":path_setup"          , datos.path_setup          );
	query.bindValue(":parametros_setup"    , datos.parametros_setup    );
	query.bindValue(":compatibilidad_setup", datos.compatibilidad_setup);
	query.bindValue(":idgrl"               , datos.idgrl               );
	query.exec();

	return chequearQuery(query);
}

bool dbSql::actualizaDatosItem(QString tabla, QString IDgrl, QString m_key, QString m_value, QString m_where)
{
	QSqlQuery query(sqldb);

	query.prepare("UPDATE "+ tabla +" SET "+ m_key +" = :"+ m_key +" WHERE "+ m_where +" = :"+ m_where +";");
	query.bindValue(":"+ m_key, m_value);
	query.bindValue(":"+ m_where, IDgrl);
	query.exec();

	return chequearQuery(query);
}
// FIN Datos Juego ----------------------------------------------------------------------------------------------

// INICIO Config DOSBox -----------------------------------------------------------------------------------------
// DOSBox por defecto
stConfigDOSBox dbSql::getDefectDOSBox(QHash<QString, QString> dato)
{
	bool isQHash = dato.isEmpty() ? false : true;
	stConfigDOSBox datos;
	datos.id    = setDefDatValue(isQHash, dato["Dbx_id"]   , "");
	datos.idgrl = setDefDatValue(isQHash, dato["Dbx_idgrl"], "");
	datos.idcat = setDefDatValue(isQHash, dato["Dbx_idcat"], "");
// [sdl]
	datos.sdl_fullscreen        = setDefDatValue(isQHash, dato["Dbx_sdl_fullscreen"]       , "true");
	datos.sdl_fulldouble        = setDefDatValue(isQHash, dato["Dbx_sdl_fulldouble"]       , "false");
	datos.sdl_fullfixed         = setDefDatValue(isQHash, dato["Dbx_sdl_fullfixed"]        , "false");
	datos.sdl_fullresolution    = setDefDatValue(isQHash, dato["Dbx_sdl_fullresolution"]   , "0x0");
	datos.sdl_windowresolution  = setDefDatValue(isQHash, dato["Dbx_sdl_windowresolution"] , "original");
	datos.sdl_output            = setDefDatValue(isQHash, dato["Dbx_sdl_output"]           , "ddraw");
	datos.sdl_hwscale           = setDefDatValue(isQHash, dato["Dbx_sdl_hwscale"]          , "1.00");
	datos.sdl_autolock          = setDefDatValue(isQHash, dato["Dbx_sdl_autolock"]         , "true");
	datos.sdl_sensitivity       = setDefDatValue(isQHash, dato["Dbx_sdl_sensitivity"]      , "100");
	datos.sdl_waitonerror       = setDefDatValue(isQHash, dato["Dbx_sdl_waitonerror"]      , "true");
	datos.sdl_priority          = setDefDatValue(isQHash, dato["Dbx_sdl_priority"]         , "higher,normal");
	datos.sdl_mapperfile        = setDefDatValue(isQHash, dato["Dbx_sdl_mapperfile"]       , "mapper.map");
	datos.sdl_usescancodes      = setDefDatValue(isQHash, dato["Dbx_sdl_usescancodes"]     , "true");
// [dosbox]
	datos.dosbox_language       = setDefDatValue(isQHash, dato["Dbx_dosbox_language"]      , "");
	datos.dosbox_machine        = setDefDatValue(isQHash, dato["Dbx_dosbox_machine"]       , "svga_s3");
	datos.dosbox_captures       = setDefDatValue(isQHash, dato["Dbx_dosbox_captures"]      , "");
	datos.dosbox_memsize        = setDefDatValue(isQHash, dato["Dbx_dosbox_memsize"]       , "16");
	datos.dosbox_emu_key        = setDefDatValue(isQHash, dato["Dbx_dosbox_emu_key"]       , "dosbox");
// [render]
	datos.render_frameskip      = setDefDatValue(isQHash, dato["Dbx_render_frameskip"]     , "0");
	datos.render_aspect         = setDefDatValue(isQHash, dato["Dbx_render_aspect"]        , "true");
	datos.render_scaler         = setDefDatValue(isQHash, dato["Dbx_render_scaler"]        , "normal2x");
// [cpu]
	datos.cpu_core              = setDefDatValue(isQHash, dato["Dbx_cpu_core"]             , "auto");
	datos.cpu_cputype           = setDefDatValue(isQHash, dato["Dbx_cpu_cputype"]          , "auto");
	datos.cpu_cycles            = setDefDatValue(isQHash, dato["Dbx_cpu_cycles"]           , "auto");
	datos.cpu_cycles_realmode   = setDefDatValue(isQHash, dato["Dbx_cpu_cycles_realmode"]  , "");
	datos.cpu_cycles_protmode   = setDefDatValue(isQHash, dato["Dbx_cpu_cycles_protmode"]  , "");
	datos.cpu_cycles_limitmode  = setDefDatValue(isQHash, dato["Dbx_cpu_cycles_limitmode"] , "");
	datos.cpu_cycleup           = setDefDatValue(isQHash, dato["Dbx_cpu_cycleup"]          , "10");
	datos.cpu_cycledown         = setDefDatValue(isQHash, dato["Dbx_cpu_cycledown"]        , "20");
// [mixer]
	datos.mixer_nosound         = setDefDatValue(isQHash, dato["Dbx_mixer_nosound"]        , "false");
	datos.mixer_rate            = setDefDatValue(isQHash, dato["Dbx_mixer_rate"]           , "44100");
	datos.mixer_blocksize       = setDefDatValue(isQHash, dato["Dbx_mixer_blocksize"]      , "1024");
	datos.mixer_prebuffer       = setDefDatValue(isQHash, dato["Dbx_mixer_prebuffer"]      , "20");
// [midi]
	datos.midi_mpu401           = setDefDatValue(isQHash, dato["Dbx_midi_mpu401"]          , "intelligent");
	datos.midi_intelligent      = setDefDatValue(isQHash, dato["Dbx_midi_intelligent"]     , "true");
	datos.midi_device           = setDefDatValue(isQHash, dato["Dbx_midi_device"]          , "default");
	datos.midi_config           = setDefDatValue(isQHash, dato["Dbx_midi_config"]          , "");
	datos.midi_mt32rate         = setDefDatValue(isQHash, dato["Dbx_midi_mt32rate"]        , "44100");
// [sblaster]
	datos.sblaster_sbtype       = setDefDatValue(isQHash, dato["Dbx_sblaster_sbtype"]      , "sb16");
	datos.sblaster_sbbase       = setDefDatValue(isQHash, dato["Dbx_sblaster_sbbase"]      , "220");
	datos.sblaster_irq          = setDefDatValue(isQHash, dato["Dbx_sblaster_irq"]         , "7");
	datos.sblaster_dma          = setDefDatValue(isQHash, dato["Dbx_sblaster_dma"]         , "1");
	datos.sblaster_hdma         = setDefDatValue(isQHash, dato["Dbx_sblaster_hdma"]        , "5");
	datos.sblaster_mixer        = setDefDatValue(isQHash, dato["Dbx_sblaster_mixer"]       , "true");
	datos.sblaster_oplmode      = setDefDatValue(isQHash, dato["Dbx_sblaster_oplmode"]     , "auto");
	datos.sblaster_oplemu       = setDefDatValue(isQHash, dato["Dbx_sblaster_oplemu"]      , "default");
	datos.sblaster_oplrate      = setDefDatValue(isQHash, dato["Dbx_sblaster_oplrate"]     , "44100");
// [gus]
	datos.gus_gus               = setDefDatValue(isQHash, dato["Dbx_gus_gus"]              , "false");
	datos.gus_gusrate           = setDefDatValue(isQHash, dato["Dbx_gus_gusrate"]          , "44100");
	datos.gus_gusbase           = setDefDatValue(isQHash, dato["Dbx_gus_gusbase"]          , "240");
	datos.gus_irq1              = setDefDatValue(isQHash, dato["Dbx_gus_irq1"]             , "5");
	datos.gus_irq2              = setDefDatValue(isQHash, dato["Dbx_gus_irq2"]             , "5");
	datos.gus_dma1              = setDefDatValue(isQHash, dato["Dbx_gus_dma1"]             , "3");
	datos.gus_dma2              = setDefDatValue(isQHash, dato["Dbx_gus_dma2"]             , "3");
	datos.gus_ultradir          = setDefDatValue(isQHash, dato["Dbx_gus_ultradir"]         , "C:\\ULTRASND");
// [speaker]
	datos.speaker_pcspeaker     = setDefDatValue(isQHash, dato["Dbx_speaker_pcspeaker"]    , "true");
	datos.speaker_pcrate        = setDefDatValue(isQHash, dato["Dbx_speaker_pcrate"]       , "44100");
	datos.speaker_tandy         = setDefDatValue(isQHash, dato["Dbx_speaker_tandy"]        , "auto");
	datos.speaker_tandyrate     = setDefDatValue(isQHash, dato["Dbx_speaker_tandyrate"]    , "44100");
	datos.speaker_disney        = setDefDatValue(isQHash, dato["Dbx_speaker_disney"]       , "true");
// [joystick]
	datos.joystick_type         = setDefDatValue(isQHash, dato["Dbx_joystick_type"]        , "auto");
	datos.joystick_timed        = setDefDatValue(isQHash, dato["Dbx_joystick_timed"]       , "true");
	datos.joystick_autofire     = setDefDatValue(isQHash, dato["Dbx_joystick_autofire"]    , "false");
	datos.joystick_swap34       = setDefDatValue(isQHash, dato["Dbx_joystick_swap34"]      , "false");
	datos.joystick_buttonwrap   = setDefDatValue(isQHash, dato["Dbx_joystick_buttonwrap"]  , "false");
// [modem]
	datos.modem_modem           = setDefDatValue(isQHash, dato["Dbx_modem_modem"]          , "false");
	datos.modem_comport         = setDefDatValue(isQHash, dato["Dbx_modem_comport"]        , "2");
	datos.modem_listenport      = setDefDatValue(isQHash, dato["Dbx_modem_listenport"]     , "23");
	datos.modem_irq             = setDefDatValue(isQHash, dato["Dbx_modem_irq"]            , "");
// [dserial]
	datos.dserial_directserial  = setDefDatValue(isQHash, dato["Dbx_dserial_directserial"] , "false");
	datos.dserial_comport       = setDefDatValue(isQHash, dato["Dbx_dserial_comport"]      , "1");
	datos.dserial_realport      = setDefDatValue(isQHash, dato["Dbx_dserial_realport"]     , "COM1");
	datos.dserial_defaultbps    = setDefDatValue(isQHash, dato["Dbx_dserial_defaultbps"]   , "1200");
	datos.dserial_parity        = setDefDatValue(isQHash, dato["Dbx_dserial_parity"]       , "N");
	datos.dserial_bytesize      = setDefDatValue(isQHash, dato["Dbx_dserial_bytesize"]     , "8");
	datos.dserial_stopbit       = setDefDatValue(isQHash, dato["Dbx_dserial_stopbit"]      , "1");
// [serial]
	datos.serial_1              = setDefDatValue(isQHash, dato["Dbx_serial_1"]             , "dummy");
	datos.serial_2              = setDefDatValue(isQHash, dato["Dbx_serial_2"]             , "dummy");
	datos.serial_3              = setDefDatValue(isQHash, dato["Dbx_serial_3"]             , "disabled");
	datos.serial_4              = setDefDatValue(isQHash, dato["Dbx_serial_4"]             , "disabled");
// [dos]
	datos.dos_xms               = setDefDatValue(isQHash, dato["Dbx_dos_xms"]              , "true");
	datos.dos_ems               = setDefDatValue(isQHash, dato["Dbx_dos_ems"]              , "true");
	datos.dos_umb               = setDefDatValue(isQHash, dato["Dbx_dos_umb"]              , "true");
	datos.dos_keyboardlayout    = setDefDatValue(isQHash, dato["Dbx_dos_keyboardlayout"]   , "auto");
	datos.dos_version           = setDefDatValue(isQHash, dato["Dbx_dos_version"]          , "");
// [ipx]
	datos.ipx_ipx               = setDefDatValue(isQHash, dato["Dbx_ipx_ipx"]              , "false");
	datos.ipx_type              = setDefDatValue(isQHash, dato["Dbx_ipx_type"]             , "none");
	datos.ipx_port              = setDefDatValue(isQHash, dato["Dbx_ipx_port"]             , "213");
	datos.ipx_ip                = setDefDatValue(isQHash, dato["Dbx_ipx_ip"]               , "");
// [autoexec]
	datos.autoexec              = setDefDatValue(isQHash, dato["Dbx_autoexec"]             , "");
	datos.autoexec_ini          = setDefDatValue(isQHash, dato["Dbx_autoexec_ini"]         , "");
	datos.autoexec_fin          = setDefDatValue(isQHash, dato["Dbx_autoexec_fin"]         , "");
	datos.autoexec_ini_exe      = setDefDatValue(isQHash, dato["Dbx_autoexec_ini_exe"]     , "");
	datos.autoexec_fin_exe      = setDefDatValue(isQHash, dato["Dbx_autoexec_fin_exe"]     , "");
// Opciones
	datos.opt_autoexec          = setDefDatValue(isQHash, dato["Dbx_opt_autoexec"]         , "false");
	datos.opt_loadfix           = setDefDatValue(isQHash, dato["Dbx_opt_loadfix"]          , "false");
	datos.opt_loadfix_mem       = setDefDatValue(isQHash, dato["Dbx_opt_loadfix_mem"]      , "64");
	datos.opt_consola_dbox      = setDefDatValue(isQHash, dato["Dbx_opt_consola_dbox"]     , "false");
	datos.opt_cerrar_dbox       = setDefDatValue(isQHash, dato["Dbx_opt_cerrar_dbox"]      , "true");
	datos.opt_cycle_sincronizar = setDefDatValue(isQHash, dato["Dbx_opt_cycle_sincronizar"], "false");
// Path
	datos.path_exe              = setDefDatValue(isQHash, dato["Dbx_path_exe"]             , "");
	datos.path_setup            = setDefDatValue(isQHash, dato["Dbx_path_setup"]           , "");
	datos.parametros_exe        = setDefDatValue(isQHash, dato["Dbx_parametros_exe"]       , "");
	datos.parametros_setup      = setDefDatValue(isQHash, dato["Dbx_parametros_setup"]     , "");

	return datos;
}

stConfigDOSBox dbSql::showConfg_DOSBox(QString IDgrl, QString IDcat)
{
	return showConfg_DOSBox_SqlQuery("SELECT * FROM dbgrl_emu_dosbox WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat +" LIMIT 0,1;");
}

stConfigDOSBox dbSql::showConfg_DOSBox(QString IDdosbox)
{
	return showConfg_DOSBox_SqlQuery("SELECT * FROM dbgrl_emu_dosbox WHERE id="+ IDdosbox +" LIMIT 0,1;");
}

stConfigDOSBox dbSql::showConfg_DOSBox_SqlQuery(QString sql_query)
{
	QSqlQuery query(sqldb);
	stConfigDOSBox cfgDbx;

	query.exec(sql_query);
	if (chequearQuery(query))
	{
		if (query.first())
		{
			cfgDbx.id                    = query.record().value("id").toString();
			cfgDbx.idgrl                 = query.record().value("idgrl").toString();
			cfgDbx.idcat                 = query.record().value("idcat").toString();
		// [sdl]
			cfgDbx.sdl_fullscreen        = query.record().value("sdl_fullscreen").toString();
			cfgDbx.sdl_fulldouble        = query.record().value("sdl_fulldouble").toString();
			cfgDbx.sdl_fullfixed         = query.record().value("sdl_fullfixed").toString();
			cfgDbx.sdl_fullresolution    = query.record().value("sdl_fullresolution").toString();
			cfgDbx.sdl_windowresolution  = query.record().value("sdl_windowresolution").toString();
			cfgDbx.sdl_output            = query.record().value("sdl_output").toString();
			cfgDbx.sdl_hwscale           = query.record().value("sdl_hwscale").toString();
			cfgDbx.sdl_autolock          = query.record().value("sdl_autolock").toString();
			cfgDbx.sdl_sensitivity       = query.record().value("sdl_sensitivity").toString();
			cfgDbx.sdl_waitonerror       = query.record().value("sdl_waitonerror").toString();
			cfgDbx.sdl_priority          = query.record().value("sdl_priority").toString();
			cfgDbx.sdl_mapperfile        = query.record().value("sdl_mapperfile").toString();
			cfgDbx.sdl_usescancodes      = query.record().value("sdl_usescancodes").toString();
		// [dosbox]
			cfgDbx.dosbox_language       = query.record().value("dosbox_language").toString();
			cfgDbx.dosbox_machine        = query.record().value("dosbox_machine").toString();
			cfgDbx.dosbox_memsize        = query.record().value("dosbox_memsize").toString();
			cfgDbx.dosbox_emu_key        = query.record().value("dosbox_emu_key").toString();
		// [render]
			cfgDbx.render_frameskip      = query.record().value("render_frameskip").toString();
			cfgDbx.render_aspect         = query.record().value("render_aspect").toString();
			cfgDbx.render_scaler         = query.record().value("render_scaler").toString();
		// [cpu]
			cfgDbx.cpu_core              = query.record().value("cpu_core").toString();
			cfgDbx.cpu_cputype           = query.record().value("cpu_cputype").toString();
			cfgDbx.cpu_cycles            = query.record().value("cpu_cycles").toString();
			cfgDbx.cpu_cycles_realmode   = query.record().value("cpu_cycles_realmode").toString();
			cfgDbx.cpu_cycles_protmode   = query.record().value("cpu_cycles_protmode").toString();
			cfgDbx.cpu_cycles_limitmode  = query.record().value("cpu_cycles_limitmode").toString();
			cfgDbx.cpu_cycleup           = query.record().value("cpu_cycleup").toString();
			cfgDbx.cpu_cycledown         = query.record().value("cpu_cycledown").toString();
		// [mixer]
			cfgDbx.mixer_nosound         = query.record().value("mixer_nosound").toString();
			cfgDbx.mixer_rate            = query.record().value("mixer_rate").toString();
			cfgDbx.mixer_blocksize       = query.record().value("mixer_blocksize").toString();
			cfgDbx.mixer_prebuffer       = query.record().value("mixer_prebuffer").toString();
		// [midi]
			cfgDbx.midi_mpu401           = query.record().value("midi_mpu401").toString();
			cfgDbx.midi_intelligent      = query.record().value("midi_intelligent").toString();
			cfgDbx.midi_device           = query.record().value("midi_device").toString();
			cfgDbx.midi_config           = query.record().value("midi_config").toString();
			cfgDbx.midi_mt32rate         = query.record().value("midi_mt32rate").toString();
		// [sblaster]
			cfgDbx.sblaster_sbtype       = query.record().value("sblaster_sbtype").toString();
			cfgDbx.sblaster_sbbase       = query.record().value("sblaster_sbbase").toString();
			cfgDbx.sblaster_irq          = query.record().value("sblaster_irq").toString();
			cfgDbx.sblaster_dma          = query.record().value("sblaster_dma").toString();
			cfgDbx.sblaster_hdma         = query.record().value("sblaster_hdma").toString();
			cfgDbx.sblaster_mixer        = query.record().value("sblaster_mixer").toString();
			cfgDbx.sblaster_oplmode      = query.record().value("sblaster_oplmode").toString();
			cfgDbx.sblaster_oplemu       = query.record().value("sblaster_oplemu").toString();
			cfgDbx.sblaster_oplrate      = query.record().value("sblaster_oplrate").toString();
		// [gus]
			cfgDbx.gus_gus               = query.record().value("gus_gus").toString();
			cfgDbx.gus_gusrate           = query.record().value("gus_gusrate").toString();
			cfgDbx.gus_gusbase           = query.record().value("gus_gusbase").toString();
			cfgDbx.gus_irq1              = query.record().value("gus_irq1").toString();
			cfgDbx.gus_irq2              = query.record().value("gus_irq2").toString();
			cfgDbx.gus_dma1              = query.record().value("gus_dma1").toString();
			cfgDbx.gus_dma2              = query.record().value("gus_dma2").toString();
			cfgDbx.gus_ultradir          = query.record().value("gus_ultradir").toString();
		// [speaker]
			cfgDbx.speaker_pcspeaker     = query.record().value("speaker_pcspeaker").toString();
			cfgDbx.speaker_pcrate        = query.record().value("speaker_pcrate").toString();
			cfgDbx.speaker_tandy         = query.record().value("speaker_tandy").toString();
			cfgDbx.speaker_tandyrate     = query.record().value("speaker_tandyrate").toString();
			cfgDbx.speaker_disney        = query.record().value("speaker_disney").toString();
		// [joystick]
			cfgDbx.joystick_type         = query.record().value("joystick_type").toString();
			cfgDbx.joystick_timed        = query.record().value("joystick_timed").toString();
			cfgDbx.joystick_autofire     = query.record().value("joystick_autofire").toString();
			cfgDbx.joystick_swap34       = query.record().value("joystick_swap34").toString();
			cfgDbx.joystick_buttonwrap   = query.record().value("joystick_buttonwrap").toString();
		// [modem]
			cfgDbx.modem_modem           = query.record().value("modem_modem").toString();
			cfgDbx.modem_comport         = query.record().value("modem_comport").toString();
			cfgDbx.modem_listenport      = query.record().value("modem_listenport").toString();
		// [dserial]
			cfgDbx.dserial_directserial  = query.record().value("dserial_directserial").toString();
			cfgDbx.dserial_comport       = query.record().value("dserial_comport").toString();
			cfgDbx.dserial_realport      = query.record().value("dserial_realport").toString();
			cfgDbx.dserial_defaultbps    = query.record().value("dserial_defaultbps").toString();
			cfgDbx.dserial_parity        = query.record().value("dserial_parity").toString();
			cfgDbx.dserial_bytesize      = query.record().value("dserial_bytesize").toString();
			cfgDbx.dserial_stopbit       = query.record().value("dserial_stopbit").toString();
			cfgDbx.serial_1              = query.record().value("serial_1").toString();
			cfgDbx.serial_2              = query.record().value("serial_2").toString();
			cfgDbx.serial_3              = query.record().value("serial_3").toString();
			cfgDbx.serial_4              = query.record().value("serial_4").toString();
		// [dos]
			cfgDbx.dos_xms               = query.record().value("dos_xms").toString();
			cfgDbx.dos_ems               = query.record().value("dos_ems").toString();
			cfgDbx.dos_umb               = query.record().value("dos_umb").toString();
			cfgDbx.dos_keyboardlayout    = query.record().value("dos_keyboardlayout").toString();
		// [ipx]
			cfgDbx.ipx_ipx               = query.record().value("ipx_ipx").toString();
			cfgDbx.ipx_type              = query.record().value("ipx_type").toString();
			cfgDbx.ipx_port              = query.record().value("ipx_port").toString();
			cfgDbx.ipx_ip                = query.record().value("ipx_ip").toString();
		// [autoexec]
			cfgDbx.autoexec              = query.record().value("autoexec").toString();
			cfgDbx.autoexec_ini          = query.record().value("autoexec_ini").toString();
			cfgDbx.autoexec_fin          = query.record().value("autoexec_fin").toString();
			cfgDbx.autoexec_ini_exe      = query.record().value("autoexec_ini_exe").toString();
			cfgDbx.autoexec_fin_exe      = query.record().value("autoexec_fin_exe").toString();
		// Opciones
			cfgDbx.opt_autoexec          = query.record().value("opt_autoexec").toString();
			cfgDbx.opt_loadfix           = query.record().value("opt_loadfix").toString();
			cfgDbx.opt_loadfix_mem       = query.record().value("opt_loadfix_mem").toString();
			cfgDbx.opt_consola_dbox      = query.record().value("opt_consola_dbox").toString();
			cfgDbx.opt_cerrar_dbox       = query.record().value("opt_cerrar_dbox").toString();
			cfgDbx.opt_cycle_sincronizar = query.record().value("opt_cycle_sincronizar").toString();
		// Otras opciones
			cfgDbx.path_exe              = query.record().value("path_exe").toString();
			cfgDbx.path_setup            = query.record().value("path_setup").toString();
			cfgDbx.parametros_exe        = query.record().value("parametros_exe").toString();
			cfgDbx.parametros_setup      = query.record().value("parametros_setup").toString();
		} else
			cfgDbx = getDefectDOSBox();
	} else
		cfgDbx = getDefectDOSBox();

	return cfgDbx;
}

QString dbSql::insertaDbx(stConfigDOSBox cfgDbx)
{
	QSqlQuery query(sqldb);
	QString strSQL;
	strSQL = "INSERT INTO dbgrl_emu_dosbox ("
			"idgrl, idcat, sdl_fullscreen, sdl_fulldouble, sdl_fullfixed, sdl_fullresolution, sdl_windowresolution, sdl_output, "
			"sdl_hwscale, sdl_autolock, sdl_sensitivity, sdl_waitonerror, sdl_priority, sdl_mapperfile, sdl_usescancodes, "
			"dosbox_language, dosbox_machine, dosbox_memsize, dosbox_emu_key, render_frameskip, render_aspect, render_scaler, "
			"cpu_core, cpu_cputype, cpu_cycles, cpu_cycles_realmode, cpu_cycles_protmode, cpu_cycles_limitmode, cpu_cycleup, cpu_cycledown, mixer_nosound, mixer_rate, mixer_blocksize, mixer_prebuffer, "
			"midi_mpu401, midi_intelligent, midi_device, midi_config, midi_mt32rate, sblaster_sbtype, sblaster_sbbase, sblaster_irq, "
			"sblaster_dma, sblaster_hdma, sblaster_mixer, sblaster_oplmode, sblaster_oplemu, sblaster_oplrate, gus_gus, gus_gusrate, gus_gusbase, "
			"gus_irq1, gus_irq2, gus_dma1, gus_dma2, gus_ultradir, speaker_pcspeaker, speaker_pcrate, speaker_tandy, speaker_tandyrate, "
			"speaker_disney, joystick_type, joystick_timed, joystick_autofire, joystick_swap34, joystick_buttonwrap, "
			"modem_modem, modem_comport, modem_listenport, dserial_directserial, dserial_comport, "
			"dserial_realport, dserial_defaultbps, dserial_parity, dserial_bytesize, dserial_stopbit, serial_1, serial_2, serial_3, "
			"serial_4, dos_xms, dos_ems, dos_umb, dos_keyboardlayout, ipx_ipx, ipx_type, ipx_port, ipx_ip, autoexec, autoexec_ini, autoexec_fin, autoexec_ini_exe, autoexec_fin_exe, "
			"opt_autoexec, opt_loadfix, opt_loadfix_mem, opt_consola_dbox, opt_cerrar_dbox, opt_cycle_sincronizar, path_exe, path_setup, parametros_exe, parametros_setup"
		") VALUES ("
			":idgrl, :idcat, :sdl_fullscreen, :sdl_fulldouble, :sdl_fullfixed, :sdl_fullresolution, :sdl_windowresolution, :sdl_output, "
			":sdl_hwscale, :sdl_autolock, :sdl_sensitivity, :sdl_waitonerror, :sdl_priority, :sdl_mapperfile, :sdl_usescancodes, "
			":dosbox_language, :dosbox_machine, :dosbox_memsize, :dosbox_emu_key, :render_frameskip, :render_aspect, :render_scaler, "
			":cpu_core, :cpu_cputype, :cpu_cycles, :cpu_cycles_realmode, :cpu_cycles_protmode, :cpu_cycles_limitmode, :cpu_cycleup, :cpu_cycledown, :mixer_nosound, :mixer_rate, :mixer_blocksize, :mixer_prebuffer, "
			":midi_mpu401, :midi_intelligent, :midi_device, :midi_config, :midi_mt32rate, :sblaster_sbtype, :sblaster_sbbase, :sblaster_irq, "
			":sblaster_dma, :sblaster_hdma, :sblaster_mixer, :sblaster_oplmode, :sblaster_oplemu, :sblaster_oplrate, :gus_gus, :gus_gusrate, :gus_gusbase, "
			":gus_irq1, :gus_irq2, :gus_dma1, :gus_dma2, :gus_ultradir, :speaker_pcspeaker, :speaker_pcrate, :speaker_tandy, :speaker_tandyrate, "
			":speaker_disney, :joystick_type, :joystick_timed, :joystick_autofire, :joystick_swap34, :joystick_buttonwrap, "
			":modem_modem, :modem_comport, :modem_listenport, :dserial_directserial, :dserial_comport, "
			":dserial_realport, :dserial_defaultbps, :dserial_parity, :dserial_bytesize, :dserial_stopbit, :serial_1, :serial_2, :serial_3, "
			":serial_4, :dos_xms, :dos_ems, :dos_umb, :dos_keyboardlayout, :ipx_ipx, :ipx_type, :ipx_port, :ipx_ip, :autoexec, :autoexec_ini, :autoexec_fin, :autoexec_ini_exe, :autoexec_fin_exe, "
			":opt_autoexec, :opt_loadfix, :opt_loadfix_mem, :opt_consola_dbox, :opt_cerrar_dbox, :opt_cycle_sincronizar, :path_exe, :path_setup, :parametros_exe, :parametros_setup"
		")"+ qpsql_return_id("id");

	query.prepare(strSQL);
	query.bindValue(":idgrl"                , cfgDbx.idgrl                );
	query.bindValue(":idcat"                , cfgDbx.idcat                );
	query.bindValue(":sdl_fullscreen"       , cfgDbx.sdl_fullscreen       );
	query.bindValue(":sdl_fulldouble"       , cfgDbx.sdl_fulldouble       );
	query.bindValue(":sdl_fullfixed"        , cfgDbx.sdl_fullfixed        );
	query.bindValue(":sdl_fullresolution"   , cfgDbx.sdl_fullresolution   );
	query.bindValue(":sdl_windowresolution" , cfgDbx.sdl_windowresolution );
	query.bindValue(":sdl_output"           , cfgDbx.sdl_output           );
	query.bindValue(":sdl_hwscale"          , cfgDbx.sdl_hwscale          );
	query.bindValue(":sdl_autolock"         , cfgDbx.sdl_autolock         );
	query.bindValue(":sdl_sensitivity"      , cfgDbx.sdl_sensitivity      );
	query.bindValue(":sdl_waitonerror"      , cfgDbx.sdl_waitonerror      );
	query.bindValue(":sdl_priority"         , cfgDbx.sdl_priority         );
	query.bindValue(":sdl_mapperfile"       , cfgDbx.sdl_mapperfile       );
	query.bindValue(":sdl_usescancodes"     , cfgDbx.sdl_usescancodes     );
	query.bindValue(":dosbox_language"      , cfgDbx.dosbox_language      );
	query.bindValue(":dosbox_machine"       , cfgDbx.dosbox_machine       );
	query.bindValue(":dosbox_memsize"       , cfgDbx.dosbox_memsize       );
	query.bindValue(":dosbox_emu_key"       , cfgDbx.dosbox_emu_key       );
	query.bindValue(":render_frameskip"     , cfgDbx.render_frameskip     );
	query.bindValue(":render_aspect"        , cfgDbx.render_aspect        );
	query.bindValue(":render_scaler"        , cfgDbx.render_scaler        );
	query.bindValue(":cpu_core"             , cfgDbx.cpu_core             );
	query.bindValue(":cpu_cputype"          , cfgDbx.cpu_cputype          );
	query.bindValue(":cpu_cycles"           , cfgDbx.cpu_cycles           );
	query.bindValue(":cpu_cycles_realmode"  , cfgDbx.cpu_cycles_realmode  );
	query.bindValue(":cpu_cycles_protmode"  , cfgDbx.cpu_cycles_protmode  );
	query.bindValue(":cpu_cycles_limitmode" , cfgDbx.cpu_cycles_limitmode );
	query.bindValue(":cpu_cycleup"          , cfgDbx.cpu_cycleup          );
	query.bindValue(":cpu_cycledown"        , cfgDbx.cpu_cycledown        );
	query.bindValue(":mixer_nosound"        , cfgDbx.mixer_nosound        );
	query.bindValue(":mixer_rate"           , cfgDbx.mixer_rate           );
	query.bindValue(":mixer_blocksize"      , cfgDbx.mixer_blocksize      );
	query.bindValue(":mixer_prebuffer"      , cfgDbx.mixer_prebuffer      );
	query.bindValue(":midi_mpu401"          , cfgDbx.midi_mpu401          );
	query.bindValue(":midi_intelligent"     , cfgDbx.midi_intelligent     );
	query.bindValue(":midi_device"          , cfgDbx.midi_device          );
	query.bindValue(":midi_config"          , cfgDbx.midi_config          );
	query.bindValue(":midi_mt32rate"        , cfgDbx.midi_mt32rate        );
	query.bindValue(":sblaster_sbtype"      , cfgDbx.sblaster_sbtype      );
	query.bindValue(":sblaster_sbbase"      , cfgDbx.sblaster_sbbase      );
	query.bindValue(":sblaster_irq"         , cfgDbx.sblaster_irq         );
	query.bindValue(":sblaster_dma"         , cfgDbx.sblaster_dma         );
	query.bindValue(":sblaster_hdma"        , cfgDbx.sblaster_hdma        );
	query.bindValue(":sblaster_mixer"       , cfgDbx.sblaster_mixer       );
	query.bindValue(":sblaster_oplmode"     , cfgDbx.sblaster_oplmode     );
	query.bindValue(":sblaster_oplemu"      , cfgDbx.sblaster_oplemu      );
	query.bindValue(":sblaster_oplrate"     , cfgDbx.sblaster_oplrate     );
	query.bindValue(":gus_gus"              , cfgDbx.gus_gus              );
	query.bindValue(":gus_gusrate"          , cfgDbx.gus_gusrate          );
	query.bindValue(":gus_gusbase"          , cfgDbx.gus_gusbase          );
	query.bindValue(":gus_irq1"             , cfgDbx.gus_irq1             );
	query.bindValue(":gus_irq2"             , cfgDbx.gus_irq2             );
	query.bindValue(":gus_dma1"             , cfgDbx.gus_dma1             );
	query.bindValue(":gus_dma2"             , cfgDbx.gus_dma2             );
	query.bindValue(":gus_ultradir"         , cfgDbx.gus_ultradir         );
	query.bindValue(":speaker_pcspeaker"    , cfgDbx.speaker_pcspeaker    );
	query.bindValue(":speaker_pcrate"       , cfgDbx.speaker_pcrate       );
	query.bindValue(":speaker_tandy"        , cfgDbx.speaker_tandy        );
	query.bindValue(":speaker_tandyrate"    , cfgDbx.speaker_tandyrate    );
	query.bindValue(":speaker_disney"       , cfgDbx.speaker_disney       );
	query.bindValue(":joystick_type"        , cfgDbx.joystick_type        );
	query.bindValue(":joystick_timed"       , cfgDbx.joystick_timed       );
	query.bindValue(":joystick_autofire"    , cfgDbx.joystick_autofire    );
	query.bindValue(":joystick_swap34"      , cfgDbx.joystick_swap34      );
	query.bindValue(":joystick_buttonwrap"  , cfgDbx.joystick_buttonwrap  );
	query.bindValue(":modem_modem"          , cfgDbx.modem_modem          );
	query.bindValue(":modem_comport"        , cfgDbx.modem_comport        );
	query.bindValue(":modem_listenport"     , cfgDbx.modem_listenport     );
	query.bindValue(":dserial_directserial" , cfgDbx.dserial_directserial );
	query.bindValue(":dserial_comport"      , cfgDbx.dserial_comport      );
	query.bindValue(":dserial_realport"     , cfgDbx.dserial_realport     );
	query.bindValue(":dserial_defaultbps"   , cfgDbx.dserial_defaultbps   );
	query.bindValue(":dserial_parity"       , cfgDbx.dserial_parity       );
	query.bindValue(":dserial_bytesize"     , cfgDbx.dserial_bytesize     );
	query.bindValue(":dserial_stopbit"      , cfgDbx.dserial_stopbit      );
	query.bindValue(":serial_1"             , cfgDbx.serial_1             );
	query.bindValue(":serial_2"             , cfgDbx.serial_2             );
	query.bindValue(":serial_3"             , cfgDbx.serial_3             );
	query.bindValue(":serial_4"             , cfgDbx.serial_4             );
	query.bindValue(":dos_xms"              , cfgDbx.dos_xms              );
	query.bindValue(":dos_ems"              , cfgDbx.dos_ems              );
	query.bindValue(":dos_umb"              , cfgDbx.dos_umb              );
	query.bindValue(":dos_keyboardlayout"   , cfgDbx.dos_keyboardlayout   );
	query.bindValue(":ipx_ipx"              , cfgDbx.ipx_ipx              );
	query.bindValue(":ipx_type"             , cfgDbx.ipx_type             );
	query.bindValue(":ipx_port"             , cfgDbx.ipx_port             );
	query.bindValue(":ipx_ip"               , cfgDbx.ipx_ip               );
	query.bindValue(":autoexec"             , cfgDbx.autoexec             );
	query.bindValue(":autoexec_ini"         , cfgDbx.autoexec_ini         );
	query.bindValue(":autoexec_fin"         , cfgDbx.autoexec_fin         );
	query.bindValue(":autoexec_ini_exe"     , cfgDbx.autoexec_ini_exe     );
	query.bindValue(":autoexec_fin_exe"     , cfgDbx.autoexec_fin_exe     );
	query.bindValue(":opt_autoexec"         , cfgDbx.opt_autoexec         );
	query.bindValue(":opt_loadfix"          , cfgDbx.opt_loadfix          );
	query.bindValue(":opt_loadfix_mem"      , cfgDbx.opt_loadfix_mem      );
	query.bindValue(":opt_consola_dbox"     , cfgDbx.opt_consola_dbox     );
	query.bindValue(":opt_cerrar_dbox"      , cfgDbx.opt_cerrar_dbox      );
	query.bindValue(":opt_cycle_sincronizar", cfgDbx.opt_cycle_sincronizar);
	query.bindValue(":path_exe"             , cfgDbx.path_exe             );
	query.bindValue(":path_setup"           , cfgDbx.path_setup           );
	query.bindValue(":parametros_exe"       , cfgDbx.parametros_exe       );
	query.bindValue(":parametros_setup"     , cfgDbx.parametros_setup     );
	query.exec();

	if (chequearQuery(query))
		return lastInsertId(query, "id");
	else
		return "";
}

bool dbSql::actualizaDbx(stConfigDOSBox cfgDbx)
{
	QSqlQuery query(sqldb);
	QString strSQL = "UPDATE dbgrl_emu_dosbox SET "
		"sdl_fullscreen = :sdl_fullscreen, sdl_fulldouble = :sdl_fulldouble, sdl_fullfixed = :sdl_fullfixed, sdl_fullresolution = :sdl_fullresolution, "
		"sdl_windowresolution = :sdl_windowresolution, sdl_output = :sdl_output, sdl_hwscale = :sdl_hwscale, sdl_autolock = :sdl_autolock, "
		"sdl_sensitivity = :sdl_sensitivity, sdl_waitonerror = :sdl_waitonerror, sdl_priority = :sdl_priority, sdl_mapperfile = :sdl_mapperfile, "
		"sdl_usescancodes = :sdl_usescancodes, dosbox_language = :dosbox_language, dosbox_machine = :dosbox_machine, "
		"dosbox_memsize = :dosbox_memsize, dosbox_emu_key = :dosbox_emu_key, render_frameskip = :render_frameskip, render_aspect = :render_aspect, render_scaler = :render_scaler, "
		"cpu_core = :cpu_core, cpu_cputype = :cpu_cputype, cpu_cycles = :cpu_cycles, cpu_cycles_realmode = :cpu_cycles_realmode, "
		"cpu_cycles_protmode = :cpu_cycles_protmode, cpu_cycles_limitmode = :cpu_cycles_limitmode, cpu_cycleup = :cpu_cycleup, cpu_cycledown = :cpu_cycledown, mixer_nosound = :mixer_nosound, "
		"mixer_rate = :mixer_rate, mixer_blocksize = :mixer_blocksize, mixer_prebuffer = :mixer_prebuffer, midi_mpu401 = :midi_mpu401, midi_intelligent = :midi_intelligent, "
		"midi_device = :midi_device, midi_config = :midi_config, midi_mt32rate = :midi_mt32rate, sblaster_sbtype = :sblaster_sbtype, sblaster_sbbase = :sblaster_sbbase, "
		"sblaster_irq = :sblaster_irq, sblaster_dma = :sblaster_dma, sblaster_hdma = :sblaster_hdma, sblaster_mixer = :sblaster_mixer, sblaster_oplmode = :sblaster_oplmode, "
		"sblaster_oplemu = :sblaster_oplemu, sblaster_oplrate = :sblaster_oplrate, gus_gus = :gus_gus, gus_gusrate = :gus_gusrate, gus_gusbase = :gus_gusbase, gus_irq1 = :gus_irq1, gus_irq2 = :gus_irq2, "
		"gus_dma1 = :gus_dma1, gus_dma2 = :gus_dma2, gus_ultradir = :gus_ultradir, speaker_pcspeaker = :speaker_pcspeaker, speaker_pcrate = :speaker_pcrate, "
		"speaker_tandy = :speaker_tandy, speaker_tandyrate = :speaker_tandyrate, speaker_disney = :speaker_disney, joystick_type = :joystick_type, "
		"joystick_timed = :joystick_timed, joystick_autofire = :joystick_autofire, joystick_swap34 = :joystick_swap34, joystick_buttonwrap = :joystick_buttonwrap, "
		"modem_modem = :modem_modem, modem_comport = :modem_comport, modem_listenport = :modem_listenport, dserial_directserial = :dserial_directserial, "
		"dserial_comport = :dserial_comport, dserial_realport = :dserial_realport, dserial_defaultbps = :dserial_defaultbps, dserial_parity = :dserial_parity, "
		"dserial_bytesize = :dserial_bytesize, dserial_stopbit = :dserial_stopbit, serial_1 = :serial_1, serial_2 = :serial_2, serial_3 = :serial_3, serial_4 = :serial_4, dos_xms = :dos_xms, "
		"dos_ems = :dos_ems, dos_umb = :dos_umb, dos_keyboardlayout = :dos_keyboardlayout, ipx_ipx = :ipx_ipx, ipx_type = :ipx_type, ipx_port = :ipx_port, ipx_ip = :ipx_ip, autoexec = :autoexec, "
		"autoexec_ini = :autoexec_ini, autoexec_fin = :autoexec_fin, autoexec_ini_exe = :autoexec_ini_exe, autoexec_fin_exe = :autoexec_fin_exe, "
		"opt_autoexec = :opt_autoexec, opt_loadfix = :opt_loadfix, opt_loadfix_mem = :opt_loadfix_mem, opt_consola_dbox = :opt_consola_dbox, "
		"opt_cerrar_dbox = :opt_cerrar_dbox, opt_cycle_sincronizar = :opt_cycle_sincronizar, path_exe = :path_exe, path_setup = :path_setup, parametros_exe = :parametros_exe, parametros_setup = :parametros_setup "
		"WHERE id = :id;";

	query.prepare(strSQL);
	query.bindValue(":sdl_fullscreen"       , cfgDbx.sdl_fullscreen       );
	query.bindValue(":sdl_fulldouble"       , cfgDbx.sdl_fulldouble       );
	query.bindValue(":sdl_fullfixed"        , cfgDbx.sdl_fullfixed        );
	query.bindValue(":sdl_fullresolution"   , cfgDbx.sdl_fullresolution   );
	query.bindValue(":sdl_windowresolution" , cfgDbx.sdl_windowresolution );
	query.bindValue(":sdl_output"           , cfgDbx.sdl_output           );
	query.bindValue(":sdl_hwscale"          , cfgDbx.sdl_hwscale          );
	query.bindValue(":sdl_autolock"         , cfgDbx.sdl_autolock         );
	query.bindValue(":sdl_sensitivity"      , cfgDbx.sdl_sensitivity      );
	query.bindValue(":sdl_waitonerror"      , cfgDbx.sdl_waitonerror      );
	query.bindValue(":sdl_priority"         , cfgDbx.sdl_priority         );
	query.bindValue(":sdl_mapperfile"       , cfgDbx.sdl_mapperfile       );
	query.bindValue(":sdl_usescancodes"     , cfgDbx.sdl_usescancodes     );
	query.bindValue(":dosbox_language"      , cfgDbx.dosbox_language      );
	query.bindValue(":dosbox_machine"       , cfgDbx.dosbox_machine       );
	query.bindValue(":dosbox_memsize"       , cfgDbx.dosbox_memsize       );
	query.bindValue(":dosbox_emu_key"       , cfgDbx.dosbox_emu_key       );
	query.bindValue(":render_frameskip"     , cfgDbx.render_frameskip     );
	query.bindValue(":render_aspect"        , cfgDbx.render_aspect        );
	query.bindValue(":render_scaler"        , cfgDbx.render_scaler        );
	query.bindValue(":cpu_core"             , cfgDbx.cpu_core             );
	query.bindValue(":cpu_cputype"          , cfgDbx.cpu_cputype          );
	query.bindValue(":cpu_cycles"           , cfgDbx.cpu_cycles           );
	query.bindValue(":cpu_cycles_realmode"  , cfgDbx.cpu_cycles_realmode  );
	query.bindValue(":cpu_cycles_protmode"  , cfgDbx.cpu_cycles_protmode  );
	query.bindValue(":cpu_cycles_limitmode" , cfgDbx.cpu_cycles_limitmode );
	query.bindValue(":cpu_cycleup"          , cfgDbx.cpu_cycleup          );
	query.bindValue(":cpu_cycledown"        , cfgDbx.cpu_cycledown        );
	query.bindValue(":mixer_nosound"        , cfgDbx.mixer_nosound        );
	query.bindValue(":mixer_rate"           , cfgDbx.mixer_rate           );
	query.bindValue(":mixer_blocksize"      , cfgDbx.mixer_blocksize      );
	query.bindValue(":mixer_prebuffer"      , cfgDbx.mixer_prebuffer      );
	query.bindValue(":midi_mpu401"          , cfgDbx.midi_mpu401          );
	query.bindValue(":midi_intelligent"     , cfgDbx.midi_intelligent     );
	query.bindValue(":midi_device"          , cfgDbx.midi_device          );
	query.bindValue(":midi_config"          , cfgDbx.midi_config          );
	query.bindValue(":midi_mt32rate"        , cfgDbx.midi_mt32rate        );
	query.bindValue(":sblaster_sbtype"      , cfgDbx.sblaster_sbtype      );
	query.bindValue(":sblaster_sbbase"      , cfgDbx.sblaster_sbbase      );
	query.bindValue(":sblaster_irq"         , cfgDbx.sblaster_irq         );
	query.bindValue(":sblaster_dma"         , cfgDbx.sblaster_dma         );
	query.bindValue(":sblaster_hdma"        , cfgDbx.sblaster_hdma        );
	query.bindValue(":sblaster_mixer"       , cfgDbx.sblaster_mixer       );
	query.bindValue(":sblaster_oplmode"     , cfgDbx.sblaster_oplmode     );
	query.bindValue(":sblaster_oplemu"      , cfgDbx.sblaster_oplemu      );
	query.bindValue(":sblaster_oplrate"     , cfgDbx.sblaster_oplrate     );
	query.bindValue(":gus_gus"              , cfgDbx.gus_gus              );
	query.bindValue(":gus_gusrate"          , cfgDbx.gus_gusrate          );
	query.bindValue(":gus_gusbase"          , cfgDbx.gus_gusbase          );
	query.bindValue(":gus_irq1"             , cfgDbx.gus_irq1             );
	query.bindValue(":gus_irq2"             , cfgDbx.gus_irq2             );
	query.bindValue(":gus_dma1"             , cfgDbx.gus_dma1             );
	query.bindValue(":gus_dma2"             , cfgDbx.gus_dma2             );
	query.bindValue(":gus_ultradir"         , cfgDbx.gus_ultradir         );
	query.bindValue(":speaker_pcspeaker"    , cfgDbx.speaker_pcspeaker    );
	query.bindValue(":speaker_pcrate"       , cfgDbx.speaker_pcrate       );
	query.bindValue(":speaker_tandy"        , cfgDbx.speaker_tandy        );
	query.bindValue(":speaker_tandyrate"    , cfgDbx.speaker_tandyrate    );
	query.bindValue(":speaker_disney"       , cfgDbx.speaker_disney       );
	query.bindValue(":joystick_type"        , cfgDbx.joystick_type        );
	query.bindValue(":joystick_timed"       , cfgDbx.joystick_timed       );
	query.bindValue(":joystick_autofire"    , cfgDbx.joystick_autofire    );
	query.bindValue(":joystick_swap34"      , cfgDbx.joystick_swap34      );
	query.bindValue(":joystick_buttonwrap"  , cfgDbx.joystick_buttonwrap  );
	query.bindValue(":modem_modem"          , cfgDbx.modem_modem          );
	query.bindValue(":modem_comport"        , cfgDbx.modem_comport        );
	query.bindValue(":modem_listenport"     , cfgDbx.modem_listenport     );
	query.bindValue(":dserial_directserial" , cfgDbx.dserial_directserial );
	query.bindValue(":dserial_comport"      , cfgDbx.dserial_comport      );
	query.bindValue(":dserial_realport"     , cfgDbx.dserial_realport     );
	query.bindValue(":dserial_defaultbps"   , cfgDbx.dserial_defaultbps   );
	query.bindValue(":dserial_parity"       , cfgDbx.dserial_parity       );
	query.bindValue(":dserial_bytesize"     , cfgDbx.dserial_bytesize     );
	query.bindValue(":dserial_stopbit"      , cfgDbx.dserial_stopbit      );
	query.bindValue(":serial_1"             , cfgDbx.serial_1             );
	query.bindValue(":serial_2"             , cfgDbx.serial_2             );
	query.bindValue(":serial_3"             , cfgDbx.serial_3             );
	query.bindValue(":serial_4"             , cfgDbx.serial_4             );
	query.bindValue(":dos_xms"              , cfgDbx.dos_xms              );
	query.bindValue(":dos_ems"              , cfgDbx.dos_ems              );
	query.bindValue(":dos_umb"              , cfgDbx.dos_umb              );
	query.bindValue(":dos_keyboardlayout"   , cfgDbx.dos_keyboardlayout   );
	query.bindValue(":ipx_ipx"              , cfgDbx.ipx_ipx              );
	query.bindValue(":ipx_type"             , cfgDbx.ipx_type             );
	query.bindValue(":ipx_port"             , cfgDbx.ipx_port             );
	query.bindValue(":ipx_ip"               , cfgDbx.ipx_ip               );
	query.bindValue(":autoexec"             , cfgDbx.autoexec             );
	query.bindValue(":autoexec_ini"         , cfgDbx.autoexec_ini         );
	query.bindValue(":autoexec_fin"         , cfgDbx.autoexec_fin         );
	query.bindValue(":autoexec_ini_exe"     , cfgDbx.autoexec_ini_exe     );
	query.bindValue(":autoexec_fin_exe"     , cfgDbx.autoexec_fin_exe     );
	query.bindValue(":opt_autoexec"         , cfgDbx.opt_autoexec         );
	query.bindValue(":opt_loadfix"          , cfgDbx.opt_loadfix          );
	query.bindValue(":opt_loadfix_mem"      , cfgDbx.opt_loadfix_mem      );
	query.bindValue(":opt_consola_dbox"     , cfgDbx.opt_consola_dbox     );
	query.bindValue(":opt_cerrar_dbox"      , cfgDbx.opt_cerrar_dbox      );
	query.bindValue(":opt_cycle_sincronizar", cfgDbx.opt_cycle_sincronizar);
	query.bindValue(":path_exe"             , cfgDbx.path_exe             );
	query.bindValue(":path_setup"           , cfgDbx.path_setup           );
	query.bindValue(":parametros_exe"       , cfgDbx.parametros_exe       );
	query.bindValue(":parametros_setup"     , cfgDbx.parametros_setup     );
	query.bindValue(":id"                   , cfgDbx.id                   );
	query.exec();

	return chequearQuery(query);
}

QList<stConfigDOSBoxMount> dbSql::showConfg_DOSBoxMount(QString IDdosbox)
{
	QSqlQuery query(sqldb);
	QList<stConfigDOSBoxMount> listMount;

	listMount.clear();
	query.exec("SELECT id, id_dosbox, id_lista, path, label, tipo_as, letter, indx_cd, opt_mount, io_ctrl, select_mount, opt_size, opt_freesize, freesize FROM dbgrl_emu_dosbox_mount WHERE id_dosbox="+ IDdosbox +" ORDER BY id_lista");
	if (chequearQuery(query))
	{
		if (query.first())
		{
			stConfigDOSBoxMount cfgMount;
			do {
				cfgMount.id           = query.record().value("id").toString();				// Id
				cfgMount.id_dosbox    = query.record().value("id").toString();				// id_dosbox
				cfgMount.id_lista     = query.record().value("id_lista").toString();		// Id_lista
				cfgMount.path         = query.record().value("path").toString();			// Directorio o iso
				cfgMount.label        = query.record().value("label").toString();			// Etiqueta
				cfgMount.tipo_as      = query.record().value("tipo_as").toString();			// Tipo de montaje
				cfgMount.letter       = query.record().value("letter").toString();			// Letra de montaje
				cfgMount.indx_cd      = query.record().value("indx_cd").toString();			// Index de la unidad de cd-rom
				cfgMount.opt_mount    = query.record().value("opt_mount").toString();		// Opciones del cd-rom
				cfgMount.io_ctrl      = query.record().value("io_ctrl").toString();			// Cd/dvd
				cfgMount.select_mount = query.record().value("select_mount").toString();	// Primer montaje
				cfgMount.opt_size     = query.record().value("opt_size").toString();
				cfgMount.opt_freesize = query.record().value("opt_freesize").toString();
				cfgMount.freesize     = query.record().value("freesize").toString();
				listMount << cfgMount;
			} while (query.next());
		}
	}
	return listMount;
}

QString dbSql::insertaUnMontajeDbx(stConfigDOSBoxMount cfgDbxMount)
{
	QSqlQuery query(sqldb);
	QString strSQL;
	strSQL = "INSERT INTO dbgrl_emu_dosbox_mount ("
			"id_dosbox, id_lista, path, label, tipo_as, letter, indx_cd, opt_mount, io_ctrl, select_mount, opt_size, opt_freesize, freesize"
		") VALUES ("
			":id_dosbox, :id_lista, :path, :label, :tipo_as, :letter, :indx_cd, :opt_mount, :io_ctrl, :select_mount, :opt_size, :opt_freesize, :freesize"
		")"+ qpsql_return_id("id");

	query.prepare(strSQL);
	query.bindValue(":id_dosbox"   , cfgDbxMount.id_dosbox   );
	query.bindValue(":id_lista"    , cfgDbxMount.id_lista    );
	query.bindValue(":path"        , cfgDbxMount.path        );
	query.bindValue(":label"       , cfgDbxMount.label       );
	query.bindValue(":tipo_as"     , cfgDbxMount.tipo_as     );
	query.bindValue(":letter"      , cfgDbxMount.letter      );
	query.bindValue(":indx_cd"     , cfgDbxMount.indx_cd     );
	query.bindValue(":opt_mount"   , cfgDbxMount.opt_mount   );
	query.bindValue(":io_ctrl"     , cfgDbxMount.io_ctrl     );
	query.bindValue(":select_mount", cfgDbxMount.select_mount);
	query.bindValue(":opt_size"    , cfgDbxMount.opt_size    );
	query.bindValue(":opt_freesize", cfgDbxMount.opt_freesize);
	query.bindValue(":freesize"    , cfgDbxMount.freesize    );
	query.exec();

	if (chequearQuery(query))
		return lastInsertId(query, "id");
	else
		return "";
}

void dbSql::insertaMontajesDbx(QList<stConfigDOSBoxMount> listMount, QString IDdbx)
{
	const int listMountSize = listMount.size();
	for (int i = 0; i < listMountSize; ++i)
	{
		listMount[i].id_dosbox = IDdbx;
		listMount[i].id_lista  = QVariant(i).toString();
		insertaUnMontajeDbx(listMount.at(i));
	}
}

bool dbSql::actualizaMontajeDbx(stConfigDOSBoxMount cfgDbxMount)
{
	QSqlQuery query(sqldb);
	QString strSQL = "UPDATE dbgrl_emu_dosbox_mount SET "
		"id_lista = :id_lista, path = :path, label = :label, "
		"tipo_as = :tipo_as, letter = :letter, indx_cd = :indx_cd, opt_mount = :opt_mount, io_ctrl = :io_ctrl, "
		"select_mount = :select_mount, opt_size = :opt_size, opt_freesize = :opt_freesize, freesize = :freesize "
		"WHERE id = :id;";

	query.prepare(strSQL);
	query.bindValue(":id_lista"    , cfgDbxMount.id_lista    );
	query.bindValue(":path"        , cfgDbxMount.path        );
	query.bindValue(":label"       , cfgDbxMount.label       );
	query.bindValue(":tipo_as"     , cfgDbxMount.tipo_as     );
	query.bindValue(":letter"      , cfgDbxMount.letter      );
	query.bindValue(":indx_cd"     , cfgDbxMount.indx_cd     );
	query.bindValue(":opt_mount"   , cfgDbxMount.opt_mount   );
	query.bindValue(":io_ctrl"     , cfgDbxMount.io_ctrl     );
	query.bindValue(":select_mount", cfgDbxMount.select_mount);
	query.bindValue(":opt_size"    , cfgDbxMount.opt_size    );
	query.bindValue(":opt_freesize", cfgDbxMount.opt_freesize);
	query.bindValue(":freesize"    , cfgDbxMount.freesize    );
	query.bindValue(":id"          , cfgDbxMount.id          );
	query.exec();

	return chequearQuery(query);
}

bool dbSql::eliminarMontajeDbx(QString IDmountdbx)
{
	return eliminarItemTable("dbgrl_emu_dosbox_mount", IDmountdbx);
}
// FIN Config DOSBox --------------------------------------------------------------------------------------------

// INICIO Config ScummVM ----------------------------------------------------------------------------------------
// ScummVM por defecto
stConfigScummVM dbSql::getDefectScummVM(QHash<QString, QString> dato)
{
	bool isQHash = dato.isEmpty() ? false : true;
	stConfigScummVM datos;
	datos.id    = setDefDatValue(isQHash, dato["Svm_id"]   , "");
	datos.idgrl = setDefDatValue(isQHash, dato["Svm_idgrl"], "");
	datos.idcat = setDefDatValue(isQHash, dato["Svm_idcat"], "");
//--
	datos.game              = setDefDatValue(isQHash, dato["Svm_game"]             , "");
	datos.game_label        = setDefDatValue(isQHash, dato["Svm_game_label"]       , "");
	datos.language          = setDefDatValue(isQHash, dato["Svm_language"]         , "es");
	datos.subtitles         = setDefDatValue(isQHash, dato["Svm_subtitles"]        , "true");
	datos.platform          = setDefDatValue(isQHash, dato["Svm_platform"]         , "pc");
	datos.gfx_mode          = setDefDatValue(isQHash, dato["Svm_gfx_mode"]         , "2x");
	datos.render_mode       = setDefDatValue(isQHash, dato["Svm_render_mode"]      , "");
	datos.fullscreen        = setDefDatValue(isQHash, dato["Svm_fullscreen"]       , "false");
	datos.filtering         = setDefDatValue(isQHash, dato["Svm_filtering"]        , "false");
	datos.aspect_ratio      = setDefDatValue(isQHash, dato["Svm_aspect_ratio"]     , "false");
	datos.path               = setDefDatValue(isQHash, dato["Svm_path"]             , "");
	datos.path_extra        = setDefDatValue(isQHash, dato["Svm_path_extra"]       , "");
	datos.path_save         = setDefDatValue(isQHash, dato["Svm_path_save"]        , "");
	datos.music_driver      = setDefDatValue(isQHash, dato["Svm_music_driver"]     , "adlib");
	datos.enable_gs         = setDefDatValue(isQHash, dato["Svm_enable_gs"]        , "false");
	datos.multi_midi        = setDefDatValue(isQHash, dato["Svm_multi_midi"]       , "true");
	datos.native_mt32       = setDefDatValue(isQHash, dato["Svm_native_mt32"]      , "false");
	datos.mute              = setDefDatValue(isQHash, dato["Svm_mute"]             , "false");
	datos.master_volume     = setDefDatValue(isQHash, dato["Svm_master_volume"]    , "192");
	datos.music_volume      = setDefDatValue(isQHash, dato["Svm_music_volume"]     , "192");
	datos.sfx_volume        = setDefDatValue(isQHash, dato["Svm_sfx_volume"]       , "192");
	datos.speech_volume     = setDefDatValue(isQHash, dato["Svm_speech_volume"]    , "192");
	datos.speech_mute       = setDefDatValue(isQHash, dato["Svm_speech_mute"]      , "false");
	datos.tempo             = setDefDatValue(isQHash, dato["Svm_tempo"]            , "100");
	datos.talkspeed         = setDefDatValue(isQHash, dato["Svm_talkspeed"]        , "60");
	datos.debuglevel        = setDefDatValue(isQHash, dato["Svm_debuglevel"]       , "0");
	datos.cdrom             = setDefDatValue(isQHash, dato["Svm_cdrom"]            , "0");
	datos.joystick_num      = setDefDatValue(isQHash, dato["Svm_joystick_num"]     , "0");
	datos.output_rate       = setDefDatValue(isQHash, dato["Svm_output_rate"]      , "");
	datos.midi_gain         = setDefDatValue(isQHash, dato["Svm_midi_gain"]        , "100");
	datos.copy_protection   = setDefDatValue(isQHash, dato["Svm_copy_protection"]  , "false");
	datos.sound_font        = setDefDatValue(isQHash, dato["Svm_sound_font"]       , "");
	datos.walkspeed         = setDefDatValue(isQHash, dato["Svm_walkspeed"]        , "0");
	datos.opl_driver        = setDefDatValue(isQHash, dato["Svm_opl_driver"]       , "auto");
	datos.disable_dithering = setDefDatValue(isQHash, dato["Svm_disable_dithering"], "false");
	datos.alt_intro         = setDefDatValue(isQHash, dato["Svm_alt_intro"]        , "false");
	datos.boot_param        = setDefDatValue(isQHash, dato["Svm_boot_param"]       , "");
	datos.autosave_period   = setDefDatValue(isQHash, dato["Svm_autosave_period"]  , "300");
	datos.originalsaveload  = setDefDatValue(isQHash, dato["Svm_originalsaveload"], "false");
	datos.bright_palette    = setDefDatValue(isQHash, dato["Svm_bright_palette"]  , "false");
	datos.guioptions        = setDefDatValue(isQHash, dato["Svm_guioptions"]      , "");
//--
	datos.description       = setDefDatValue(isQHash, dato["Svm_description"], "");
	datos.emu_svmpath       = setDefDatValue(isQHash, dato["Svm_emu_svmpath"], "");

	return datos;
}

stConfigScummVM dbSql::showConfg_ScummVM(QString IDgrl, QString IDcat)
{
	return showConfg_ScummVM_SqlQuery("SELECT * FROM dbgrl_emu_scummvm WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat +" LIMIT 0,1;");
}

stConfigScummVM dbSql::showConfg_ScummVM(QString IDscummvm)
{
	return showConfg_ScummVM_SqlQuery("SELECT * FROM dbgrl_emu_scummvm WHERE id="+ IDscummvm +" LIMIT 0,1;");
}

stConfigScummVM dbSql::showConfg_ScummVM_SqlQuery(QString sql_query)
{
	QSqlQuery query(sqldb);
	stConfigScummVM cfgSvm;

	query.exec(sql_query);
	if (chequearQuery(query))
	{
		if (query.first())
		{
			cfgSvm.id                = query.record().value("id").toString();
			cfgSvm.idgrl             = query.record().value("idgrl").toString();
			cfgSvm.idcat             = query.record().value("idcat").toString();
			cfgSvm.game              = query.record().value("game").toString();
			cfgSvm.game_label        = query.record().value("game_label").toString();
			cfgSvm.language          = query.record().value("language").toString();
			cfgSvm.subtitles         = query.record().value("subtitles").toString();
			cfgSvm.platform          = query.record().value("platform").toString();
			cfgSvm.gfx_mode          = query.record().value("gfx_mode").toString();
			cfgSvm.render_mode       = query.record().value("render_mode").toString();
			cfgSvm.fullscreen        = query.record().value("fullscreen").toString();
			cfgSvm.filtering         = query.record().value("filtering").toString();
			cfgSvm.aspect_ratio      = query.record().value("aspect_ratio").toString();
			cfgSvm.path              = query.record().value("path").toString();
			cfgSvm.path_extra        = query.record().value("path_extra").toString();
			cfgSvm.path_save         = query.record().value("path_save").toString();
			cfgSvm.music_driver      = query.record().value("music_driver").toString();
			cfgSvm.enable_gs         = query.record().value("enable_gs").toString();
			cfgSvm.multi_midi        = query.record().value("multi_midi").toString();
			cfgSvm.native_mt32       = query.record().value("native_mt32").toString();
			cfgSvm.mute              = query.record().value("mute").toString();
			cfgSvm.master_volume     = query.record().value("master_volume").toString();
			cfgSvm.music_volume      = query.record().value("music_volume").toString();
			cfgSvm.sfx_volume        = query.record().value("sfx_volume").toString();
			cfgSvm.speech_volume     = query.record().value("speech_volume").toString();
			cfgSvm.speech_mute       = query.record().value("speech_mute").toString();
			cfgSvm.tempo             = query.record().value("tempo").toString();
			cfgSvm.talkspeed         = query.record().value("talkspeed").toString();
			cfgSvm.debuglevel        = query.record().value("debuglevel").toString();
			cfgSvm.cdrom             = query.record().value("cdrom").toString();
			cfgSvm.joystick_num      = query.record().value("joystick_num").toString();
			cfgSvm.output_rate       = query.record().value("output_rate").toString();
			cfgSvm.midi_gain         = query.record().value("midi_gain").toString();
			cfgSvm.copy_protection   = query.record().value("copy_protection").toString();
			cfgSvm.sound_font        = query.record().value("sound_font").toString();
			cfgSvm.walkspeed         = query.record().value("walkspeed").toString();
			cfgSvm.opl_driver        = query.record().value("opl_driver").toString();
			cfgSvm.disable_dithering = query.record().value("disable_dithering").toString();
			cfgSvm.alt_intro         = query.record().value("alt_intro").toString();
			cfgSvm.boot_param        = query.record().value("boot_param").toString();
			cfgSvm.autosave_period   = query.record().value("autosave_period").toString();
			cfgSvm.originalsaveload  = query.record().value("originalsaveload").toString();
			cfgSvm.bright_palette    = query.record().value("bright_palette").toString();
			cfgSvm.guioptions        = query.record().value("guioptions").toString();
		//--
			cfgSvm.description       = "";
			cfgSvm.emu_svmpath       = "";
		} else
			cfgSvm = getDefectScummVM();
	} else
		cfgSvm = getDefectScummVM();

	return cfgSvm;
}

QString dbSql::insertaSvm(stConfigScummVM cfgSvm)
{
	QSqlQuery query(sqldb);
	QString strSQL;
	strSQL = "INSERT INTO dbgrl_emu_scummvm ("
			"idgrl, idcat, game, game_label, language, subtitles, platform, gfx_mode, render_mode, fullscreen, filtering, aspect_ratio, path, "
			"path_extra, path_save, music_driver, enable_gs, multi_midi, native_mt32, mute, master_volume, music_volume, "
			"sfx_volume, speech_volume, speech_mute, tempo, talkspeed, debuglevel, cdrom, joystick_num, output_rate, "
			"midi_gain, copy_protection, sound_font, walkspeed, opl_driver, disable_dithering, alt_intro, boot_param, autosave_period, "
			"originalsaveload, bright_palette, guioptions"
		") VALUES ("
			":idgrl, :idcat, :game, :game_label, :language, :subtitles, :platform, :gfx_mode, :render_mode, :fullscreen, :filtering, :aspect_ratio, :path, "
			":path_extra, :path_save, :music_driver, :enable_gs, :multi_midi, :native_mt32, :mute, :master_volume, :music_volume, "
			":sfx_volume, :speech_volume, :speech_mute, :tempo, :talkspeed, :debuglevel, :cdrom, :joystick_num, :output_rate, "
			":midi_gain, :copy_protection, :sound_font, :walkspeed, :opl_driver, :disable_dithering, :alt_intro, :boot_param, :autosave_period, "
			":originalsaveload, :bright_palette, :guioptions"
		")"+ qpsql_return_id("id");

	query.prepare(strSQL);
	query.bindValue(":idgrl"            , cfgSvm.idgrl            );
	query.bindValue(":idcat"            , cfgSvm.idcat            );
	query.bindValue(":game"             , cfgSvm.game             );
	query.bindValue(":game_label"       , cfgSvm.game_label       );
	query.bindValue(":language"         , cfgSvm.language         );
	query.bindValue(":subtitles"        , cfgSvm.subtitles        );
	query.bindValue(":platform"         , cfgSvm.platform         );
	query.bindValue(":gfx_mode"         , cfgSvm.gfx_mode         );
	query.bindValue(":render_mode"      , cfgSvm.render_mode      );
	query.bindValue(":fullscreen"       , cfgSvm.fullscreen       );
	query.bindValue(":filtering"        , cfgSvm.filtering        );
	query.bindValue(":aspect_ratio"     , cfgSvm.aspect_ratio     );
	query.bindValue(":path"             , cfgSvm.path             );
	query.bindValue(":path_extra"       , cfgSvm.path_extra       );
	query.bindValue(":path_save"        , cfgSvm.path_save        );
	query.bindValue(":music_driver"     , cfgSvm.music_driver     );
	query.bindValue(":enable_gs"        , cfgSvm.enable_gs        );
	query.bindValue(":multi_midi"       , cfgSvm.multi_midi       );
	query.bindValue(":native_mt32"      , cfgSvm.native_mt32      );
	query.bindValue(":mute"             , cfgSvm.mute             );
	query.bindValue(":master_volume"    , cfgSvm.master_volume    );
	query.bindValue(":music_volume"     , cfgSvm.music_volume     );
	query.bindValue(":sfx_volume"       , cfgSvm.sfx_volume       );
	query.bindValue(":speech_volume"    , cfgSvm.speech_volume    );
	query.bindValue(":speech_mute"      , cfgSvm.speech_mute      );
	query.bindValue(":tempo"            , cfgSvm.tempo            );
	query.bindValue(":talkspeed"        , cfgSvm.talkspeed        );
	query.bindValue(":debuglevel"       , cfgSvm.debuglevel       );
	query.bindValue(":cdrom"            , cfgSvm.cdrom            );
	query.bindValue(":joystick_num"     , cfgSvm.joystick_num     );
	query.bindValue(":output_rate"      , cfgSvm.output_rate      );
	query.bindValue(":midi_gain"        , cfgSvm.midi_gain        );
	query.bindValue(":copy_protection"  , cfgSvm.copy_protection  );
	query.bindValue(":sound_font"       , cfgSvm.sound_font       );
	query.bindValue(":walkspeed"        , cfgSvm.walkspeed        );
	query.bindValue(":opl_driver"       , cfgSvm.opl_driver       );
	query.bindValue(":disable_dithering", cfgSvm.disable_dithering);
	query.bindValue(":alt_intro"        , cfgSvm.alt_intro        );
	query.bindValue(":boot_param"       , cfgSvm.boot_param       );
	query.bindValue(":autosave_period"  , cfgSvm.autosave_period  );
	query.bindValue(":originalsaveload" , cfgSvm.originalsaveload );
	query.bindValue(":bright_palette"   , cfgSvm.bright_palette   );
	query.bindValue(":guioptions"       , cfgSvm.guioptions       );
	query.exec();

	if (chequearQuery(query))
		return lastInsertId(query, "id");
	else
		return "";
}

bool dbSql::actualizaSvm(stConfigScummVM cfgSvm)
{
	QSqlQuery query(sqldb);
	QString strSQL = "UPDATE dbgrl_emu_scummvm SET "
		"game = :game, game_label = :game_label, language = :language, subtitles = :subtitles, platform = :platform, gfx_mode = :gfx_mode, "
		"render_mode = :render_mode, fullscreen = :fullscreen, filtering = :filtering, aspect_ratio = :aspect_ratio, path = :path, path_extra = :path_extra, "
		"path_save = :path_save, music_driver = :music_driver, enable_gs = :enable_gs, multi_midi = :multi_midi, native_mt32 = :native_mt32, "
		"mute = :mute, master_volume = :master_volume, music_volume = :music_volume, sfx_volume = :sfx_volume, speech_volume = :speech_volume, "
		"speech_mute = :speech_mute, tempo = :tempo, talkspeed = :talkspeed, debuglevel = :debuglevel, cdrom = :cdrom, joystick_num = :joystick_num, "
		"output_rate = :output_rate, midi_gain = :midi_gain, copy_protection = :copy_protection, sound_font = :sound_font, walkspeed = :walkspeed, "
		"opl_driver = :opl_driver, disable_dithering = :disable_dithering, alt_intro = :alt_intro, boot_param = :boot_param, autosave_period = :autosave_period, "
		"originalsaveload = :originalsaveload, bright_palette = :bright_palette, guioptions = :guioptions "
		"WHERE id = :id;";

	query.prepare(strSQL);
//	query.bindValue(":idgrl"            , cfgSvm.idgrl            );
//	query.bindValue(":idcat"            , cfgSvm.idcat            );
	query.bindValue(":game"             , cfgSvm.game             );
	query.bindValue(":game_label"       , cfgSvm.game_label       );
	query.bindValue(":language"         , cfgSvm.language         );
	query.bindValue(":subtitles"        , cfgSvm.subtitles        );
	query.bindValue(":platform"         , cfgSvm.platform         );
	query.bindValue(":gfx_mode"         , cfgSvm.gfx_mode         );
	query.bindValue(":render_mode"      , cfgSvm.render_mode      );
	query.bindValue(":fullscreen"       , cfgSvm.fullscreen       );
	query.bindValue(":filtering"         , cfgSvm.filtering       );
	query.bindValue(":aspect_ratio"     , cfgSvm.aspect_ratio     );
	query.bindValue(":path"             , cfgSvm.path             );
	query.bindValue(":path_extra"       , cfgSvm.path_extra       );
	query.bindValue(":path_save"        , cfgSvm.path_save        );
	query.bindValue(":music_driver"     , cfgSvm.music_driver     );
	query.bindValue(":enable_gs"        , cfgSvm.enable_gs        );
	query.bindValue(":multi_midi"       , cfgSvm.multi_midi       );
	query.bindValue(":native_mt32"      , cfgSvm.native_mt32      );
	query.bindValue(":mute"             , cfgSvm.mute             );
	query.bindValue(":master_volume"    , cfgSvm.master_volume    );
	query.bindValue(":music_volume"     , cfgSvm.music_volume     );
	query.bindValue(":sfx_volume"       , cfgSvm.sfx_volume       );
	query.bindValue(":speech_volume"    , cfgSvm.speech_volume    );
	query.bindValue(":speech_mute"      , cfgSvm.speech_mute      );
	query.bindValue(":tempo"            , cfgSvm.tempo            );
	query.bindValue(":talkspeed"        , cfgSvm.talkspeed        );
	query.bindValue(":debuglevel"       , cfgSvm.debuglevel       );
	query.bindValue(":cdrom"            , cfgSvm.cdrom            );
	query.bindValue(":joystick_num"     , cfgSvm.joystick_num     );
	query.bindValue(":output_rate"      , cfgSvm.output_rate      );
	query.bindValue(":midi_gain"        , cfgSvm.midi_gain        );
	query.bindValue(":copy_protection"  , cfgSvm.copy_protection  );
	query.bindValue(":sound_font"       , cfgSvm.sound_font       );
	query.bindValue(":walkspeed"        , cfgSvm.walkspeed        );
	query.bindValue(":opl_driver"       , cfgSvm.opl_driver       );
	query.bindValue(":disable_dithering", cfgSvm.disable_dithering);
	query.bindValue(":alt_intro"        , cfgSvm.alt_intro        );
	query.bindValue(":boot_param"       , cfgSvm.boot_param       );
	query.bindValue(":autosave_period"  , cfgSvm.autosave_period  );
	query.bindValue(":originalsaveload" , cfgSvm.originalsaveload );
	query.bindValue(":bright_palette"   , cfgSvm.bright_palette   );
	query.bindValue(":guioptions"       , cfgSvm.guioptions       );
	query.bindValue(":id"               , cfgSvm.id               );
	query.exec();

	return chequearQuery(query);
}
// FIN Config ScummVM --------------------------------------------------------------------------------------------

// INICIO Config VDMSound ----------------------------------------------------------------------------------------
// VDMSound por defecto
stConfigVDMSound dbSql::getDefectVDMSound(QHash<QString, QString> dato)
{
	bool isQHash = dato.isEmpty() ? false : true;
	stConfigVDMSound datos;
	datos.id    = setDefDatValue(isQHash, dato["Vdms_id"]   , "");
	datos.idgrl = setDefDatValue(isQHash, dato["Vdms_idgrl"], "");
	datos.idcat = setDefDatValue(isQHash, dato["Vdms_idcat"], "");
//--
	datos.path_exe        = setDefDatValue(isQHash, dato["Vdms_path_exe"]       , "");
	datos.program_1       = setDefDatValue(isQHash, dato["Vdms_program_1"]      , "");
	datos.program_2       = setDefDatValue(isQHash, dato["Vdms_program_2"]      , "");
	datos.vdms_debug_1    = setDefDatValue(isQHash, dato["Vdms_vdms_debug_1"]   , "no");
	datos.vdms_debug_2    = setDefDatValue(isQHash, dato["Vdms_vdms_debug_2"]   , "");
	datos.winnt_dos_1     = setDefDatValue(isQHash, dato["Vdms_winnt_dos_1"]    , "no");
	datos.winnt_dos_2     = setDefDatValue(isQHash, dato["Vdms_winnt_dos_2"]    , "");
	datos.winnt_dosbox_1  = setDefDatValue(isQHash, dato["Vdms_winnt_dosbox_1"] , "yes");
	datos.winnt_dosbox_2  = setDefDatValue(isQHash, dato["Vdms_winnt_dosbox_2"] , "no");
	datos.winnt_dosbox_3  = setDefDatValue(isQHash, dato["Vdms_winnt_dosbox_3"] , "no");
	datos.winnt_storage_1 = setDefDatValue(isQHash, dato["Vdms_winnt_storage_1"], "yes");
	datos.winnt_storage_2 = setDefDatValue(isQHash, dato["Vdms_winnt_storage_2"], "no");

	return datos;
}

stConfigVDMSound dbSql::showConfg_VDMSound(QString IDgrl, QString IDcat)
{
	QSqlQuery query(sqldb);
	stConfigVDMSound cfgVdms;

	query.exec("SELECT * FROM dbgrl_emu_vdmsound WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat +" LIMIT 0,1;");
	if (chequearQuery(query))
	{
		if (query.first())
		{
			QStringList list_program       = query.record().value("program").toString().split("|");
			QStringList list_vdms_debug    = query.record().value("vdms_debug").toString().split("|");
			QStringList list_winnt_dos     = query.record().value("winnt_dos").toString().split("|");
			QStringList list_winnt_dosbox  = query.record().value("winnt_dosbox").toString().split("|");
			QStringList list_winnt_storage = query.record().value("winnt_storage").toString().split("|");

			cfgVdms.id              = query.record().value("id").toString();
			cfgVdms.idgrl           = query.record().value("idgrl").toString();
			cfgVdms.idcat           = query.record().value("idcat").toString();
		//--
			cfgVdms.path_exe        = query.record().value("path_exe").toString();
			cfgVdms.program_1       = ""+ list_program.value(0, "");
			cfgVdms.program_2       = ""+ list_program.value(1, "");
			cfgVdms.vdms_debug_1    = ""+ list_vdms_debug.value(0, "no");
			cfgVdms.vdms_debug_2    = ""+ list_vdms_debug.value(1, "");
			cfgVdms.winnt_dos_1     = ""+ list_winnt_dos.value(0, "no");
			cfgVdms.winnt_dos_2     = ""+ list_winnt_dos.value(1, "");
			cfgVdms.winnt_dosbox_1  = ""+ list_winnt_dosbox.value(0, "yes");
			cfgVdms.winnt_dosbox_2  = ""+ list_winnt_dosbox.value(1, "no");
			cfgVdms.winnt_dosbox_3  = ""+ list_winnt_dosbox.value(2, "no");
			cfgVdms.winnt_storage_1 = ""+ list_winnt_storage.value(0, "yes");
			cfgVdms.winnt_storage_2 = ""+ list_winnt_storage.value(1, "no");
		} else
			cfgVdms = getDefectVDMSound();
	} else
		cfgVdms = getDefectVDMSound();

	return cfgVdms;
}

QString dbSql::insertaVdms(stConfigVDMSound cfgVdms)
{
	QSqlQuery query(sqldb);
	QString strSQL;
	strSQL = "INSERT INTO dbgrl_emu_vdmsound ("
			"idgrl, idcat, path_exe, program, vdms_debug, winnt_dos, winnt_dosbox, winnt_storage "
		") VALUES ("
			":idgrl, :idcat, :path_exe, :program, :vdms_debug, :winnt_dos, :winnt_dosbox, :winnt_storage "
		")"+ qpsql_return_id("id");

	query.prepare(strSQL);
	query.bindValue(":idgrl"        , cfgVdms.idgrl);
	query.bindValue(":idcat"        , cfgVdms.idcat);
	query.bindValue(":path_exe"     , cfgVdms.path_exe);
	query.bindValue(":program"      , cfgVdms.program_1 +"|"+ cfgVdms.program_2);
	query.bindValue(":vdms_debug"   , cfgVdms.vdms_debug_1 +"|"+ cfgVdms.vdms_debug_2);
	query.bindValue(":winnt_dos"    , cfgVdms.winnt_dos_1 +"|"+ cfgVdms.winnt_dos_2);
	query.bindValue(":winnt_dosbox" , cfgVdms.winnt_dosbox_1 +"|"+ cfgVdms.winnt_dosbox_2 +"|"+ cfgVdms.winnt_dosbox_3);
	query.bindValue(":winnt_storage", cfgVdms.winnt_storage_1 +"|"+ cfgVdms.winnt_storage_2);
	query.exec();

	if (chequearQuery(query))
		return lastInsertId(query, "id");
	else
		return "";
}

bool dbSql::actualizaVdms(stConfigVDMSound cfgVdms)
{
	QSqlQuery query(sqldb);
	QString strSQL = "UPDATE dbgrl_emu_vdmsound SET "
		"path_exe = :path_exe, program = :program, vdms_debug = :vdms_debug, "
		"winnt_dos = :winnt_dos, winnt_dosbox = :winnt_dosbox, winnt_storage = :winnt_storage "
		"WHERE id = :id;";

	query.prepare(strSQL);
	query.bindValue(":path_exe"     , cfgVdms.path_exe);
	query.bindValue(":program"      , cfgVdms.program_1 +"|"+ cfgVdms.program_2);
	query.bindValue(":vdms_debug"   , cfgVdms.vdms_debug_1 +"|"+ cfgVdms.vdms_debug_2);
	query.bindValue(":winnt_dos"    , cfgVdms.winnt_dos_1 +"|"+ cfgVdms.winnt_dos_2);
	query.bindValue(":winnt_dosbox" , cfgVdms.winnt_dosbox_1 +"|"+ cfgVdms.winnt_dosbox_2 +"|"+ cfgVdms.winnt_dosbox_3);
	query.bindValue(":winnt_storage", cfgVdms.winnt_storage_1 +"|"+ cfgVdms.winnt_storage_2);
	query.bindValue(":id"           , cfgVdms.id);
	query.exec();

	return chequearQuery(query);
}
// FIN Config VDMSound ------------------------------------------------------------------------------------------

// INICIO Archivos ----------------------------------------------------------------------------------------------
// Muestra los archivos en un QTreeWidget
void dbSql::cargarDatosFiles(QTreeWidget *twFiles, QString IDgrl, QString IDcat, QString sql_where)
{
	QSqlQuery query(sqldb);

	if (sql_where.isEmpty())
		sql_where = "ORDER BY nombre ASC";

	twFiles->clear();
	query.exec("SELECT id, idgrl, idcat, nombre, crc, descripcion, path, size, tipo FROM dbgrl_files WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat +" "+ sql_where);
	if (query.first())
	{
		QList<QTreeWidgetItem *> items;
		do {
			QTreeWidgetItem *item_files = new QTreeWidgetItem;

			QString file_ico = query.record().value("tipo").toString();
			if (file_ico == "manual")
				item_files->setIcon(0, QIcon(stTheme +"img16/cat/datos.png"));
			else if (file_ico == "pdf")
				item_files->setIcon(0, QIcon(stTheme +"img16/pdf.png"));
			else if (file_ico == "ruleta")
				item_files->setIcon(0, QIcon(stTheme +"img16/ruleta.png"));
			else if (file_ico == "archivo")
				item_files->setIcon(0, QIcon(stTheme +"img16/archivos.png"));
			else
				item_files->setIcon(0, QIcon(stTheme +"img16/archivos.png"));

			item_files->setTextAlignment(1, Qt::AlignCenter);
			item_files->setTextAlignment(3, Qt::AlignCenter);
			item_files->setText(0, query.record().value("nombre").toString());
			item_files->setText(1, query.record().value("crc").toString());
			item_files->setText(2, query.record().value("descripcion").toString());
			item_files->setText(3, covertir_bytes(query.record().value("size").toDouble()));
			item_files->setText(4, query.record().value("path").toString());
			item_files->setText(5, query.record().value("tipo").toString());
			item_files->setText(6, query.record().value("id").toString());
			item_files->setText(7, query.record().value("idgrl").toString());
			item_files->setText(8, query.record().value("idcat").toString());
			item_files->setText(9, query.record().value("size").toString());

			items << item_files;
		} while (query.next());
		twFiles->addTopLevelItems(items);
	}
}

QString dbSql::insertaUnFiles(stDatosFiles datos)
{
	QSqlQuery query(sqldb);
	QString strSQL;
	strSQL = "INSERT INTO dbgrl_files ("
			"idgrl, idcat, nombre, crc, descripcion, path, size, tipo"
		") VALUES ("
			":idgrl, :idcat, :nombre, :crc, :descripcion, :path, :size, :tipo"
		")"+ qpsql_return_id("id");

	query.prepare(strSQL);
	query.bindValue(":idgrl"      , datos.idgrl      );
	query.bindValue(":idcat"      , datos.idcat      );
	query.bindValue(":nombre"     , datos.nombre     );
	query.bindValue(":crc"        , datos.crc        );
	query.bindValue(":descripcion", datos.descripcion);
	query.bindValue(":path"       , datos.path       );
	query.bindValue(":size"       , datos.size       );
	query.bindValue(":tipo"       , datos.tipo       );
	query.exec();

	if (chequearQuery(query))
		return lastInsertId(query, "id");
	else
		return "";
}

void dbSql::insertaFiles(QTreeWidget *twFiles, QString IDgrl, QString IDcat)
{
	QSqlQuery query(sqldb);
	QString strSQL = "INSERT INTO dbgrl_files ("
			"idgrl, idcat, nombre, crc, descripcion, path, size, tipo"
		") VALUES ("
			":idgrl, :idcat, :nombre, :crc, :descripcion, :path, :size, :tipo"
		")";

	const int count_file = twFiles->topLevelItemCount();
	for (int num_file = 0; num_file < count_file; ++num_file)
	{
		QTreeWidgetItem *item_files = twFiles->topLevelItem(num_file);
		query.prepare(strSQL);
		query.bindValue(":idgrl"      , IDgrl              );
		query.bindValue(":idcat"      , IDcat              );
		query.bindValue(":nombre"     , item_files->text(0));
		query.bindValue(":crc"        , item_files->text(1));
		query.bindValue(":descripcion", item_files->text(2));
		query.bindValue(":path"       , item_files->text(4));
		query.bindValue(":size"       , item_files->text(9));
		query.bindValue(":tipo"       , item_files->text(5));
		query.exec();

		if (chequearQuery(query))
			item_files->setText(6, lastInsertId(query, "id"));
	}
}

bool dbSql::actualizaFiles(stDatosFiles datos)
{
	QSqlQuery query(sqldb);

	query.prepare("UPDATE dbgrl_files SET nombre = :nombre, crc = :crc, descripcion = :descripcion, path = :path, size = :size, tipo = :tipo WHERE id = :id;");
	query.bindValue(":nombre"     , datos.nombre     );
	query.bindValue(":crc"        , datos.crc        );
	query.bindValue(":descripcion", datos.descripcion);
	query.bindValue(":path"       , datos.path       );
	query.bindValue(":size"       , datos.size       );
	query.bindValue(":tipo"       , datos.tipo       );
	query.bindValue(":id"         , datos.id         );
	query.exec();

	return chequearQuery(query);
}

bool dbSql::eliminarFiles(QString IDFiles)
{
	return eliminarItemTable("dbgrl_files", IDFiles);
}
// FIN Archivos -------------------------------------------------------------------------------------------------

// INICIO URL ---------------------------------------------------------------------------------------------------
// Muestra las URLs en un QList
/*QList<QString> dbSql::getDatosUrls(QString IDgrl, QString IDcat, QString sql_where)
{
	QSqlQuery query(sqldb);
	QList<QString> url_list;

	if (sql_where.isEmpty())
		sql_where = "ORDER BY url ASC";

	url_list.clear();
	query.exec("SELECT id, url FROM dbgrl_urls WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat +" "+ sql_where);
	if (query.first())
	{
		do {
			url_list.insert(query.record().value("id").toInt(), query.record().value("url").toString());
		} while (query.next());
	}

	return url_list;
}*/

void dbSql::cargarDatosUrls(QTreeWidget *twUrls, QString IDgrl, QString IDcat, QString sql_where)
{
	QSqlQuery query(sqldb);

	if (sql_where.isEmpty())
		sql_where = "ORDER BY url ASC";

	twUrls->clear();
	query.exec("SELECT id, idgrl, idcat, url, descripcion FROM dbgrl_urls WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat +" "+ sql_where);
	if (query.first())
	{
		QList<QTreeWidgetItem *> items;
		do {
			QTreeWidgetItem *item_url = new QTreeWidgetItem;
			item_url->setIcon(0, QIcon(stTheme +"img16/html.png"));
			item_url->setText(0, query.record().value("url").toString());
			item_url->setText(1, query.record().value("descripcion").toString());
			item_url->setText(2, query.record().value("id").toString());
			item_url->setText(3, query.record().value("idgrl").toString());
			item_url->setText(4, query.record().value("idcat").toString());

			items << item_url;
		} while (query.next());
		twUrls->addTopLevelItems(items);
	}
}

QString dbSql::insertaUnURL(stDatosUrls datos)
{
	QSqlQuery query(sqldb);
	QString strSQL = "INSERT INTO dbgrl_urls (idgrl, idcat, url, descripcion) VALUES (:idgrl, :idcat, :url, :descripcion)"+ qpsql_return_id("id");

	query.prepare(strSQL);
	query.bindValue(":idgrl"      , datos.idgrl      );
	query.bindValue(":idcat"      , datos.idcat      );
	query.bindValue(":url"        , datos.url        );
	query.bindValue(":descripcion", datos.descripcion);
	query.exec();

	if (chequearQuery(query))
		return lastInsertId(query, "id");
	else
		return "";
}

void dbSql::insertaURL(QTreeWidget *twUrls, QString IDgrl, QString IDcat)
{
	QSqlQuery query(sqldb);
	QString strSQL = "INSERT INTO dbgrl_urls (idgrl, idcat, url, descripcion) VALUES (:idgrl, :idcat, :url, :descripcion)"+ qpsql_return_id("id");

	const int count_url = twUrls->topLevelItemCount();
	for (int num_url = 0; num_url < count_url; ++num_url)
	{
		QTreeWidgetItem *item_url = twUrls->topLevelItem(num_url);
		query.prepare(strSQL);
		query.bindValue(":idgrl"      , IDgrl            );
		query.bindValue(":idcat"      , IDcat            );
		query.bindValue(":url"        , item_url->text(0));
		query.bindValue(":descripcion", item_url->text(1));
		query.exec();

		if (chequearQuery(query))
			item_url->setText(2, lastInsertId(query, "id"));
	}
}

bool dbSql::actualizaURL(stDatosUrls datos)
{
	QSqlQuery query(sqldb);

	query.prepare("UPDATE dbgrl_urls SET url = :url, descripcion = :descripcion WHERE id = :id;");
	query.bindValue(":url"        , datos.url        );
	query.bindValue(":descripcion", datos.descripcion);
	query.bindValue(":id"         , datos.id         );
	query.exec();

	return chequearQuery(query);
}

bool dbSql::eliminarURL(QString IDUrl)
{
	return eliminarItemTable("dbgrl_urls", IDUrl);
}
// FIN URL ------------------------------------------------------------------------------------------------------

// INICIO Categorias --------------------------------------------------------------------------------------------
QList<stGrlCats> dbSql::getCategorias(QString sql_where)
{
	QSqlQuery query(sqldb);
	stGrlCats cat;
	QList<stGrlCats> cat_list;

	cat_list.clear();
	query.exec("SELECT id, tabla, titulo, img, orden, emu_show FROM dbgrl_categorias "+ sql_where +" ORDER BY orden ASC;");
	if (query.first())
	{
		do {
			cat.id       = query.record().value("id").toString();
			cat.tabla    = query.record().value("tabla").toString();
			cat.titulo   = query.record().value("titulo").toString();
			cat.img      = query.record().value("img").toString();
			cat.orden    = query.record().value("orden").toString();
			cat.emu_show = query.record().value("emu_show").toString();
			cat.total    = getCount(cat.tabla);
			cat_list.insert(cat.id.toInt(), cat);
		} while (query.next());
	} else {
		cat.tabla    = "dbgrl";
		cat.titulo   = "Db GR-lida";
		cat.img      = "cat/pc.png";
		cat.orden    = "0";
		cat.emu_show = "all";
		cat.total    = 0;
		cat.id       = insertaCategoria(cat);
		cat_list.insert(cat.id.toInt(), cat);
	}

	return cat_list;
}

void dbSql::cargarCategorias(QTreeWidget *twCat)
{
	QList<stGrlCats> list = getCategorias();
	const int listSize = list.size();
	twCat->clear();
	QList<QTreeWidgetItem *> items;
	for (int i = 0; i < listSize; ++i)
	{
		QTreeWidgetItem *item_cat = new QTreeWidgetItem;
		if (QFile::exists(stTheme +"img32/"+ list.at(i).img))
			item_cat->setIcon(0, QIcon(stTheme +"img32/"+ list.at(i).img));
		else
			item_cat->setIcon(0, QIcon(":/img32/sinimg.png"));

		item_cat->setTextAlignment(2, Qt::AlignCenter);
		item_cat->setText(0, list.at(i).titulo  );
		item_cat->setText(1, list.at(i).tabla   );
		item_cat->setText(2, QVariant(list.at(i).total).toString());
		item_cat->setText(3, list.at(i).id      );
		item_cat->setText(4, list.at(i).img     );
		item_cat->setText(5, list.at(i).orden   );
		item_cat->setText(6, list.at(i).emu_show);

		items << item_cat;
	}
	twCat->addTopLevelItems(items);
}

QString dbSql::insertaCategoria(stGrlCats cat)
{
	QSqlQuery query(sqldb);
	QString strSQL = "INSERT INTO dbgrl_categorias (tabla, titulo, img, orden, emu_show) VALUES (:tabla, :titulo, :img, :orden, :emu_show)"+ qpsql_return_id("id");

	query.prepare(strSQL);
	query.bindValue(":tabla"   , cat.tabla   );
	query.bindValue(":titulo"  , cat.titulo  );
	query.bindValue(":img"     , cat.img     );
	query.bindValue(":orden"   , cat.orden   );
	query.bindValue(":emu_show", cat.emu_show);
	query.exec();

	if (chequearQuery(query))
	{
		QString last_id = lastInsertId(query, "id");
		crearTablaDatos(cat.tabla);
		return last_id;
	} else
		return "";
}

bool dbSql::actualizaCategoria(stGrlCats cat)
{
	QSqlQuery query(sqldb);

	query.prepare("UPDATE dbgrl_categorias SET tabla = :tabla, titulo = :titulo, img = :img, emu_show = :emu_show WHERE id = :id;");
	query.bindValue(":tabla"   , cat.tabla   );
	query.bindValue(":titulo"  , cat.titulo  );
	query.bindValue(":img"     , cat.img     );
	query.bindValue(":emu_show", cat.emu_show);
	query.bindValue(":id"      , cat.id      );
	query.exec();

	return chequearQuery(query);
}

bool dbSql::eliminarCategoria(QString IDcat)
{
	stGrlCats cat = getCategorias("WHERE id = "+ IDcat).first();
	if (cat.id.isEmpty())
		return false;
	else {
		if (cat.tabla == "dbgrl")
			return false;
		else {
			QSqlQuery query(sqldb);
		// Datos
			query.exec("DELETE FROM dbgrl_urls WHERE idcat="+ cat.id);
			query.exec("DELETE FROM dbgrl_files WHERE idcat="+ cat.id);
		// ScummVM
			query.exec("DELETE FROM dbgrl_emu_scummvm WHERE idcat="+ cat.id);
		// DOSBox
			query.exec("SELECT id FROM dbgrl_emu_dosbox WHERE idcat="+ cat.id);
			if (query.first())
			{
				do {
					QString IDdbx = query.record().value("id").toString();
					query.exec("DELETE FROM dbgrl_emu_dosbox_mount WHERE id_dosbox="+ IDdbx);
				} while (query.next());
			}
			query.exec("DELETE FROM dbgrl_emu_dosbox WHERE idcat="+ cat.id);
		// VDMSound
			query.exec("DELETE FROM dbgrl_emu_vdmsound WHERE idcat="+ cat.id);
		// Tabla y categoria
			query.exec("DROP TABLE "+ cat.tabla +";");
			query.exec("DELETE FROM dbgrl_categorias WHERE id="+ cat.id +";");

			return chequearQuery(query);
		}
	}
}
// FIN Categorias -----------------------------------------------------------------------------------------------

// INICIO MenuNav -----------------------------------------------------------------------------------------------
void dbSql::cargarMenuNav(QTreeWidget *twMnuNav, QString tabla, bool isOpt)
{
	QSqlQuery query(sqldb);
	QTreeWidgetItem *item;
	QStringList lista;
	QFile file_in;
	QString linea, sLng, titulo, col_value, tmp_col_value, col_name, sql_query, tmp_query, archivo, img, orden, dir_img;
	QFont m_font;
	int col_img   = 1;
	int total     = 0;
	int total_sub = 0;
	int id        = twMnuNav->topLevelItemCount();
	dir_img       = stTheme +"img16/";

	// Listado de fechas desde 1970 a año actual
	QStringList fechas;
	const int fecha_actual = setTime(getTime(), "yyyy", false).toInt() + 1;
	for (int fecha = 1970; fecha < fecha_actual; ++fecha)
		fechas << QVariant(fecha).toString();
	const int listFechasSize = fechas.size();
	//--

	query.exec("SELECT id, titulo, col_value, col_name, sql_query, archivo, img, orden, mostrar, expanded FROM dbgrl_mnu_nav ORDER BY orden ASC;");
	if (chequearQuery(query))
	{
		if (query.first())
		{
			do {
				if (strToBool(query.record().value("mostrar").toString()) || isOpt)
				{
					titulo    = query.record().value("titulo").toString();
					col_value = query.record().value("col_value").toString();
					col_name  = query.record().value("col_name").toString();
					sql_query = query.record().value("sql_query").toString();
					archivo   = query.record().value("archivo").toString();
					img       = query.record().value("img").toString();
					orden     = query.record().value("orden").toString();
					sLng      = getArchivoIsLng(archivo);

					item = new QTreeWidgetItem;

					item->setIcon(0, QIcon(dir_img +"sinimg.png"));
					if (QFile::exists(dir_img + img))
						item->setIcon(0, QIcon(dir_img + img));

					item->setTextAlignment(1, Qt::AlignCenter);
					item->setText(0, titulo);

					if (isOpt)
					{
						item->setText(1, col_value);
						item->setText(2, col_name);
						item->setText(3, sql_query);
						item->setText(4, archivo);
						item->setText(5, query.record().value("mostrar").toString());
						item->setText(6, query.record().value("expanded").toString());
						item->setText(7, img);
						item->setText(8, orden);
						item->setText(9, query.record().value("id").toString());
					} else {
						if (archivo.toLower() == "defecto")
						{
							tmp_col_value = col_value;
							if (tmp_col_value == "{value}")
								tmp_col_value = titulo;

							tmp_query = sql_query;
							tmp_query.replace("{col_value}", tmp_col_value.replace("'", "''")).replace("{col_name}", col_name);

							total = getCount(tabla, tmp_query, col_name, tmp_col_value);
							m_font.setBold((total > 0) ? true : false);
							item->setFont(1, m_font);
							item->setText(1, QString::number(total));
							item->setText(2, tmp_query);
							item->setText(3, img);
							item->setText(4, QVariant(id).toString());
						} else {
							item->setText(1, "");
							item->setText(2, "");
							item->setText(3, img);
							item->setText(4, QVariant(id).toString());
							item->setFirstColumnSpanned(true);

							if (getArchivoIs3Col(archivo))
								col_img = 2;
							else
								col_img = 1;

							if (archivo == "fechas.txt")
							{
								// recorre un array con las fechas.
								QList<QTreeWidgetItem *> sub_items_;
								for (int i = 0; i < listFechasSize; ++i)
								{
									titulo = fechas.at(i);

								//	ui->cbxDat_anno->addItem(QIcon(fGrl->theme() +"img16/fecha.png"), fechas.at(i), fechas.at(i));

									tmp_col_value = col_value;
									if (tmp_col_value == "{value}")
										tmp_col_value = titulo;

									tmp_query = sql_query;
									tmp_query.replace("{col_value}", tmp_col_value.replace("'", "''")).replace("{col_name}", col_name);
									img       = "fecha.png";

									if (img.isEmpty())
										img = "sinimg.png";

									total_sub = getCount(tabla, tmp_query, col_name, tmp_col_value);
									m_font.setBold((total_sub > 0) ? true : false);

									QTreeWidgetItem *sub_item = new QTreeWidgetItem;

									sub_item->setIcon(0, QIcon(dir_img +"sinimg.png"));
									if (QFile::exists(dir_img + img))
										sub_item->setIcon(0, QIcon(dir_img + img));

									sub_item->setFont(1, m_font);
									sub_item->setTextAlignment(1, Qt::AlignCenter);
									sub_item->setText(0, titulo);
									sub_item->setText(1, QString::number(total_sub));
									sub_item->setText(2, tmp_query);
									sub_item->setText(3, img);
									sub_item->setText(4, QVariant(id).toString());

									sub_items_ << sub_item;
								}
								item->addChildren(sub_items_);
							} else {
								// recorre un archivo por linea.

								if (QFile::exists(stDirApp +"datos/"+ sLng + archivo))
									file_in.setFileName(stDirApp +"datos/"+ sLng + archivo);
								else
									file_in.setFileName(":/datos/"+ sLng + archivo);

								if (file_in.open(QIODevice::ReadOnly | QIODevice::Text))
								{
									QTextStream in(&file_in);
									in.setCodec("UTF-8");

									QList<QTreeWidgetItem *> sub_items;
									do {
										linea = in.readLine();

										if (!linea.isEmpty())
										{
											lista = linea.split("|");

											titulo = lista.value(0);
											if (!titulo.isEmpty())
											{
												tmp_col_value = col_value;
												if (tmp_col_value == "{value}")
													tmp_col_value = titulo;

												tmp_query = sql_query;
												tmp_query.replace("{col_value}", tmp_col_value.replace("'", "''")).replace("{col_name}", col_name);
												img       = lista.value(col_img);

												if (img.isEmpty())
													img = "sinimg.png";

												total_sub = getCount(tabla, tmp_query, col_name, tmp_col_value);
												m_font.setBold((total_sub > 0) ? true : false);

												QTreeWidgetItem *sub_item = new QTreeWidgetItem;

												sub_item->setIcon(0, QIcon(dir_img +"sinimg.png"));
												if (QFile::exists(dir_img + img))
													sub_item->setIcon(0, QIcon(dir_img + img));

												sub_item->setFont(1, m_font);
												sub_item->setTextAlignment(1, Qt::AlignCenter);
												sub_item->setText(0, titulo);
												sub_item->setText(1, QString::number(total_sub));
												sub_item->setText(2, tmp_query);
												sub_item->setText(3, img);
												sub_item->setText(4, QVariant(id).toString());

												sub_items << sub_item;
											}
										}
									} while (!linea.isNull());
									item->addChildren(sub_items);
								}
								file_in.close();
							}

							item->setText(6, query.record().value("expanded").toString());
						}
						id++;
					}
					twMnuNav->addTopLevelItem(item);
					item->setExpanded(strToBool(item->text(6)));
				}
			} while (query.next());
		}
	}
}

QString dbSql::insertaMenuNav(QString titulo, QString col_value, QString col_name, QString sql_query, QString archivo, QString img, int orden, bool mostrar, bool expanded)
{
	QSqlQuery query(sqldb);
	QString strSQL;
	strSQL = "INSERT INTO dbgrl_mnu_nav ("
			"titulo, col_value, col_name, sql_query, archivo, img, orden, mostrar, expanded"
		") VALUES ("
			":titulo, :col_value, :col_name, :sql_query, :archivo, :img, :orden, :mostrar, :expanded"
		")"+ qpsql_return_id("id");

	query.prepare(strSQL);
	query.bindValue(":titulo"   , titulo    );
	query.bindValue(":col_value", col_value );
	query.bindValue(":col_name" , col_name  );
	query.bindValue(":sql_query", sql_query );
	query.bindValue(":archivo"  , archivo   );
	query.bindValue(":img"      , img       );
	query.bindValue(":orden"    , orden     );
	query.bindValue(":mostrar"  , boolToStr(mostrar));
	query.bindValue(":expanded" , boolToStr(expanded));
	query.exec();

	if (chequearQuery(query))
		return lastInsertId(query, "id");
	else
		return "";
}

bool dbSql::actualizaMenuNav(QString IDmnu, QString titulo, QString col_value, QString col_name, QString sql_query, QString archivo, QString img, int orden, bool mostrar, bool expanded)
{
	QSqlQuery query(sqldb);
	QString strSQL = "UPDATE dbgrl_mnu_nav SET "
		"titulo = :titulo, col_value = :col_value, col_name = :col_name, sql_query = :sql_query, "
		"archivo = :archivo, img = :img, orden = :orden, mostrar = :mostrar, expanded = :expanded "
		"WHERE id = :id;";

	query.prepare(strSQL);
	query.bindValue(":titulo"   , titulo   );
	query.bindValue(":col_value", col_value);
	query.bindValue(":col_name" , col_name );
	query.bindValue(":sql_query", sql_query);
	query.bindValue(":archivo"  , archivo  );
	query.bindValue(":img"      , img      );
	query.bindValue(":orden"    , orden    );
	query.bindValue(":mostrar"  , boolToStr(mostrar));
	query.bindValue(":expanded" , boolToStr(expanded));
	query.bindValue(":id"       , IDmnu);
	query.exec();

	return chequearQuery(query);
}

bool dbSql::eliminarMenuNav(QString IDmnu)
{
	return eliminarItemTable("dbgrl_mnu_nav", IDmnu);
}
// FIN MenuNav --------------------------------------------------------------------------------------------------

// INICIO MnuShortcut -------------------------------------------------------------------------------------------
QString dbSql::insertaMnuShortcut(QString titulo, QString key_sequence, QString sql_query, QString img, int orden, bool mostrar, bool separador)
{
	QSqlQuery query(sqldb);
	QString strSQL;
	strSQL = "INSERT INTO dbgrl_mnu_shortcut ("
			"titulo, key_sequence, sql_query, img, orden, mostrar, separador"
		") VALUES ("
			":titulo, :key_sequence, :sql_query, :img, :orden, :mostrar, :separador"
		")"+ qpsql_return_id("id");

	query.prepare(strSQL);
	query.bindValue(":titulo"      , titulo      );
	query.bindValue(":key_sequence", key_sequence);
	query.bindValue(":sql_query"   , sql_query   );
	query.bindValue(":img"         , img         );
	query.bindValue(":orden"       , orden       );
	query.bindValue(":mostrar"     , boolToStr(mostrar));
	query.bindValue(":separador"   , boolToStr(separador));
	query.exec();

	if (chequearQuery(query))
		return lastInsertId(query, "id");
	else
		return "";
}

bool dbSql::actualizaMnuShortcut(QString IDmnu, QString titulo, QString key_sequence, QString sql_query, QString img, int orden, bool mostrar, bool separador)
{
	QSqlQuery query(sqldb);
	QString strSQL = "UPDATE dbgrl_mnu_shortcut SET "
		"titulo = :titulo, key_sequence = :key_sequence, sql_query = :sql_query, img = :img, orden = :orden, mostrar = :mostrar, separador = :separador "
		"WHERE id = :id;";

	query.prepare(strSQL);
	query.bindValue(":titulo"      , titulo      );
	query.bindValue(":key_sequence", key_sequence);
	query.bindValue(":sql_query"   , sql_query   );
	query.bindValue(":img"         , img         );
	query.bindValue(":orden"       , orden       );
	query.bindValue(":mostrar"     , boolToStr(mostrar));
	query.bindValue(":separador"   , boolToStr(separador));
	query.bindValue(":id"          , IDmnu);
	query.exec();

	return chequearQuery(query);
}

bool dbSql::eliminarMnuShortcut(QString IDmnu)
{
	return eliminarItemTable("dbgrl_mnu_shortcut", IDmnu);
}
// FIN MnuShortcut ----------------------------------------------------------------------------------------------
