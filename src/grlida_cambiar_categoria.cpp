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

#include <QDesktopWidget>

#include "grlida_cambiar_categoria.h"
#include "ui_cambiar_categoria.h"

frmCambiarCategoria::frmCambiarCategoria(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QString id_game, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmCambiarCategoria)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql       = m_sql;
	grlCfg    = m_cfg;
	categoria = m_categoria;
	IdGame    = id_game;
	stDirApp  = fGrl->dirApp();

	cargarConfig();
	setTheme();

// Centra la aplicacion en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmCambiarCategoria::~frmCambiarCategoria()
{
	delete fGrl;
	delete ui;
}

void frmCambiarCategoria::cargarConfig()
{
	DatosJuego = sql->show_Datos(categoria.tabla, IdGame);

	ui->lb_newcat->setText(tr("Cambiar") +": <b>"+ DatosJuego.titulo +"</b><br>"+ tr("A la categoría") +":");

	QList<stGrlCats> list_cat = sql->getCategorias();

	categorias.clear();
	ui->cbxCategorias->clear();
	ui->cbxCategorias->addItem(QIcon(fGrl->theme() +"img32/cambiar_categoria.png"), tr("Selecciona la categoría de destino"), "");
	const int listCatSize = list_cat.size();
	for (int i = 0; i < listCatSize; ++i)
	{
		QStringList lista = list_cat.at(i).emu_show.split(";", QString::SkipEmptyParts);

		if (list_cat.at(i).tabla != categoria.tabla && (lista.isEmpty() || lista.contains("all") || lista.contains(DatosJuego.tipo_emu)))
		{
			categorias.insert(list_cat.at(i).id.toInt(), list_cat.at(i));

			QString titulo_cat = QString("%1 (%2)").arg(list_cat.at(i).titulo).arg(list_cat.at(i).total);

			if (QFile::exists(fGrl->theme() +"img32/"+ list_cat.at(i).img))
				ui->cbxCategorias->addItem(QIcon(fGrl->theme() +"img32/"+ list_cat.at(i).img), titulo_cat, list_cat.at(i).id);
			else
				ui->cbxCategorias->addItem(QIcon(":/img32/sinimg.png"), titulo_cat, list_cat.at(i).id);
		}
	}
	ui->cbxCategorias->setCurrentIndex(0);
	on_cbxCategorias_activated(0);
}

void frmCambiarCategoria::setTheme()
{
	setWindowIcon(QIcon(fGrl->theme() +"img16/cambiar_categoria.png"));

	ui->btnOk->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
	ui->btnCancel->setIcon(QIcon(fGrl->theme() +"img16/cancelar.png"));
}

QStringList frmCambiarCategoria::actualizaIds(QString tabla, QString cat_id_new, QString game_id_new)
{
	QSqlQuery query(sql->getSqlDB());
	query.exec("SELECT id FROM "+ tabla +" WHERE idgrl="+ IdGame +" AND idcat="+ categoria.id);
	QStringList list_id;
	if (query.first())
	{
		do {
			list_id << query.record().value("id").toString();
		} while (query.next());

		const int listIdSize = list_id.size();
		for (int i = 0; i < listIdSize; ++i)
		{
			query.clear();
			query.prepare("UPDATE "+ tabla +" SET idgrl = :idgrl, idcat = :idcat WHERE id = :id;");
			query.bindValue(":idgrl", game_id_new  );
			query.bindValue(":idcat", cat_id_new   );
			query.bindValue(":id"   , list_id.at(i));
			query.exec();
		}
	}
	return list_id;
}

void frmCambiarCategoria::on_cbxCategorias_activated(int index)
{
	if (index > -1)
		IdCat = ui->cbxCategorias->itemData(index).toString();
	else
		IdCat = "";
}

void frmCambiarCategoria::on_btnOk_clicked()
{
	if (!IdCat.isEmpty())
	{
		ui->cbxCategorias->setEnabled(false);
		ui->btnOk->setEnabled(false);
		ui->btnCancel->setEnabled(false);

		id_cat = IdCat.toInt();

	// Dir Origen
		QString dir_origen = stDirApp +"datosdb/"+ categoria.tabla +"/"+ DatosJuego.game_dir +"/";
	// Datos
		QString IdGameNew = sql->insertaDatos(categorias[id_cat].tabla, DatosJuego);
		DatosJuego.idgrl = IdGameNew;
		DatosJuego.game_dir = "id-"+ DatosJuego.idgrl +"_"+ DatosJuego.titulo_guiones +"_"+ DatosJuego.tipo_emu;
	// Dir Destino
		QString dir_destino = stDirApp +"datosdb/"+ categorias[id_cat].tabla +"/"+ DatosJuego.game_dir +"/";
	// Movemos la carpeta
		fGrl->renombrarDirectorio(dir_origen, dir_destino);
	// Datos archivos
		actualizaIds("dbgrl_files", IdCat, IdGameNew);
	// Datos url
		actualizaIds("dbgrl_urls", IdCat, IdGameNew);
	// DOSBox
		if (DatosJuego.tipo_emu == "dosbox")
		{
			QStringList list_id_dbx = actualizaIds("dbgrl_emu_dosbox", IdCat, IdGameNew);
			const int listIdDbxSize = list_id_dbx.size();
			for (int i = 0; i < listIdDbxSize; ++i)
			{
				stConfigDOSBox DatosDosBox = sql->showConfg_DOSBox(list_id_dbx.at(i));
				QList<stConfigDOSBoxMount> listMount = sql->showConfg_DOSBoxMount(DatosDosBox.id);
				fGrl->crearArchivoConfigDbx(DatosJuego, DatosDosBox, listMount, stDirApp, categorias[id_cat].tabla, dir_destino +"dosbox.conf");
				fGrl->crearArchivoConfigDbx(DatosJuego, DatosDosBox, listMount, stDirApp, categorias[id_cat].tabla, dir_destino +"dosbox-setup.conf", true);
			}
		}
	// ScummVM
		if (DatosJuego.tipo_emu == "scummvm")
		{
			QStringList list_id_svm = actualizaIds("dbgrl_emu_scummvm", IdCat, IdGameNew);
			const int listIdSvmSize = list_id_svm.size();
			for (int i = 0; i < listIdSvmSize; ++i)
			{
				stConfigScummVM DatosScummVM = sql->showConfg_ScummVM(list_id_svm.at(i));
				DatosScummVM.emu_svmpath = fGrl->getInfoFile(fGrl->getDirRelative(grlCfg.DirScummVM)).Path;
				fGrl->creaIniScummVM(DatosScummVM, dir_destino +"scummvm.ini");
			}
		}
	// ScummVM
		if (DatosJuego.tipo_emu == "vdmsound")
		{
			actualizaIds("dbgrl_emu_vdmsound", IdCat, IdGameNew);
		}
	// Eliminamos los datos antiguos del juego
		QSqlQuery query_del(sql->getSqlDB());
		query_del.exec("DELETE FROM "+ categoria.tabla +" WHERE idgrl="+ IdGame);

		sql->comit();
		QDialog::accept();
	}
}

void frmCambiarCategoria::on_btnCancel_clicked()
{
	QDialog::reject();
}
