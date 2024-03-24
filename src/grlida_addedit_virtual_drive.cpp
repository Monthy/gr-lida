/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2021 Pedro A. Garcia Rosado Aka Monthy
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
#include <QMessageBox>

#include "grlida_addedit_virtual_drive.h"
#include "ui_addedit_virtual_drive.h"

frmAddEditVirtualDrive::frmAddEditVirtualDrive(dbSql *m_sql, stGrlCfg m_cfg, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmAddEditVirtualDrive)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql    = m_sql;
	grlCfg = m_cfg;

	grlDir.Home   = fGrl->dirApp();
	grlDir.Datos  = grlDir.Home +"datos/";
	grlDir.Iconos = grlDir.Home +"iconos/";
	grlDir.Temp   = grlDir.Home +"temp/";

	cargarConfig();
	setTheme();

// Centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmAddEditVirtualDrive::~frmAddEditVirtualDrive()
{
	on_btnVdCancel_clicked();

	delete fGrl;
	delete ui;
}

void frmAddEditVirtualDrive::cargarConfig()
{
// Configuración del twVirtualDrive
	ui->twVirtualDrive->header()->setStretchLastSection(true);
#if QT_VERSION >= 0x050000
	ui->twVirtualDrive->header()->setSectionsMovable(false);
	ui->twVirtualDrive->header()->setSectionResizeMode(0, QHeaderView::Stretch);
#else
	ui->twVirtualDrive->header()->setMovable(false);
	ui->twVirtualDrive->header()->setResizeMode(0, QHeaderView::Stretch);
#endif
	for (int c = 1; c < 11; c++)
		ui->twVirtualDrive->setColumnHidden(c, true);

	regexp.setPatternSyntax(QRegExp::RegExp);
	regexp.setPattern("[a-z-_0-9]+");
	ui->txtVd_etiqueta->setValidator(new QRegExpValidator(regexp, ui->txtVd_etiqueta));

	fGrl->cargarIconosComboBox(ui->cbxVd_icono, fGrl->theme(), "img16/cat/", "cat/", ""          , grlCfg.FormatsImage.join(";"));
	fGrl->cargarIconosComboBox(ui->cbxVd_icono, fGrl->theme(), "img16/"    , ""    , "sinimg.png", grlCfg.FormatsImage.join(";"), false);

	virtual_drive_list     = fGrl->cargarListVirtualDrive(grlDir.Datos +"virtual_driver_list.txt");
	virtual_drive_list_tpl = fGrl->cargarListVirtualDrive(":/datos/virtual_driver_list.txt");

	ui->cbxVd_plantilla_cfg->clear();
	ui->cbxVd_plantilla_cfg->addItem(QIcon(fGrl->theme() +"img16/archivo_config.png"), tr("Seleccionar Unidad Virtual"), "NO_VIRTUAL_DRIVE");

	QHashIterator<QString, stVirtualDrive> vd_tpl(virtual_drive_list_tpl);
	while (vd_tpl.hasNext())
	{
		vd_tpl.next();
		if (vd_tpl.value().etiqueta != "NO_VIRTUAL_DRIVE")
		{
			ui->cbxVd_plantilla_cfg->addItem(QIcon(fGrl->theme() +"img16/"+ vd_tpl.value().icono), vd_tpl.value().titulo, vd_tpl.value().etiqueta);
		}
	}

	QHashIterator<QString, stVirtualDrive> vd(virtual_drive_list);
	while (vd.hasNext())
	{
		vd.next();
		if (vd.value().etiqueta != "NO_VIRTUAL_DRIVE")
		{
			QTreeWidgetItem *item = new QTreeWidgetItem;

			item->setIcon( 0, QIcon(fGrl->theme() +"img16/"+ vd.value().icono));
			item->setText( 0, vd.value().titulo              );
			item->setText( 1, vd.value().etiqueta            );
			item->setText( 2, vd.value().icono               );
			item->setText( 3, vd.value().path_exe            );
			item->setText( 4, vd.value().letter              );
			item->setText( 5, vd.value().param_mount         );
			item->setText( 6, vd.value().param_unmount       );
			item->setText( 7, vd.value().param_extra_1       );
			item->setText( 8, vd.value().param_extra_2       );
			item->setText( 9, vd.value().command_line_mount  );
			item->setText(10, vd.value().command_line_unmount);

			ui->twVirtualDrive->addTopLevelItem(item);
		}
	}

	on_btnVdCancel_clicked();
}

void frmAddEditVirtualDrive::setTheme()
{
	setWindowIcon(QIcon(fGrl->theme() +"img16/virtual_drive.png"));

// General
	ui->btnOk->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
	ui->btnCancel->setIcon(QIcon(fGrl->theme() +"img16/cancelar.png"));
	ui->btnVdNew->setIcon(QIcon(fGrl->theme() +"img16/nuevo.png"));
	ui->btnVdAdd->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnVdEdit->setIcon(QIcon(fGrl->theme() +"img16/editar.png"));
	ui->btnVdUpdate->setIcon(QIcon(fGrl->theme() +"img16/actualizar.png"));
	ui->btnVdCancel->setIcon(QIcon(fGrl->theme() +"img16/cancelar.png"));
// Templates
	tpl_info_old = fGrl->tplInfoJuego("tpl_info");
}

void frmAddEditVirtualDrive::setDatosVirtualDrive(stVirtualDrive virtualDrive)
{
	if (virtualDrive.etiqueta != "NO_VIRTUAL_DRIVE")
	{
		int row_icono = ui->cbxVd_icono->findData(virtualDrive.icono, Qt::UserRole, Qt::MatchExactly);
		if (row_icono < 0) row_icono = 0;

		ui->txtVd_titulo->setText(virtualDrive.titulo);
		ui->txtVd_etiqueta->setText(virtualDrive.etiqueta);
		ui->txtVd_path_exe->setText(virtualDrive.path_exe);
		ui->cbxVd_icono->setCurrentIndex(row_icono);
		ui->txtVd_letter->setText(virtualDrive.letter);
		ui->txtVd_param_mount->setText(virtualDrive.param_mount);
		ui->txtVd_param_unmount->setText(virtualDrive.param_unmount);
		ui->txtVd_param_extra_1->setText(virtualDrive.param_extra_1);
		ui->txtVd_param_extra_2->setText(virtualDrive.param_extra_2);
		ui->txtVd_command_line_mount->setText(virtualDrive.command_line_mount);
		ui->txtVd_command_line_unmount->setText(virtualDrive.command_line_unmount);
	} else {
		setDatosClear();
	}
}

void frmAddEditVirtualDrive::setDatosClear()
{
	int row_icono = ui->cbxVd_icono->findData("sinimg.png", Qt::UserRole, Qt::MatchExactly);
	if (row_icono < 0) row_icono = 0;

	ui->txtVd_titulo->setText("");
	ui->txtVd_etiqueta->setText("");
	ui->txtVd_path_exe->setText("");
	ui->cbxVd_icono->setCurrentIndex(row_icono);
	ui->txtVd_letter->setText("");
	ui->txtVd_param_mount->setText("");
	ui->txtVd_param_unmount->setText("");
	ui->txtVd_param_extra_1->setText("");
	ui->txtVd_param_extra_2->setText("");
	ui->txtVd_command_line_mount->setText("");
	ui->txtVd_command_line_unmount->setText("");
}

void frmAddEditVirtualDrive::actualizaDatosVirtualDrive(QString old_etiqueta, QString new_etiqueta)
{
	QList<stGrlCats> tmp_cat_list = sql->getCategorias();
	const int listTmpCatSize = tmp_cat_list.size();
	for (int c = 0; c < listTmpCatSize; ++c)
	{
		QString tabla = tmp_cat_list.at(c).tabla;
		QSqlQuery tmp_query(sql->getSqlDB());
		tmp_query.exec("SELECT idgrl FROM "+ tabla +" WHERE virtual_drive = '"+ old_etiqueta +"';");
		if (tmp_query.first())
		{
			do {
				QString IDgrl = tmp_query.record().value("idgrl").toString();
				sql->actualizaDatosItem(tabla, IDgrl, "virtual_drive", new_etiqueta);
			} while (tmp_query.next()); // fin bucle juegos en la categoria.
		} // fin SQL SELECT categoria.
	}
}

void frmAddEditVirtualDrive::guardarDatosVirtualDrive()
{
	QSettings settings(grlDir.Datos +"virtual_driver_list.txt", QSettings::IniFormat);
	settings.clear();

	if (ui->twVirtualDrive->topLevelItemCount() > 0)
	{
		const int listSize = ui->twVirtualDrive->topLevelItemCount();
		for (int i = 0; i < listSize; ++i)
		{
			QTreeWidgetItem *item = ui->twVirtualDrive->topLevelItem(i);

			settings.beginGroup(item->text(1));
				settings.setValue("titulo"              , item->text(0) );
				settings.setValue("icono"               , item->text(2) );
				settings.setValue("path_exe"            , item->text(3) );
				settings.setValue("letter"              , item->text(4) );
				settings.setValue("param_mount"         , item->text(5) );
				settings.setValue("param_unmount"       , item->text(6) );
				settings.setValue("param_extra_1"       , item->text(7) );
				settings.setValue("param_extra_2"       , item->text(8) );
				settings.setValue("command_line_mount"  , item->text(9) );
				settings.setValue("command_line_unmount", item->text(10));
			settings.endGroup();
		}
	}

	grlCfg.isChangedVirtualDrive = true;
}

void frmAddEditVirtualDrive::addEditTwVirtualDrive(bool isAdd)
{
	if (ui->txtVd_titulo->text().isEmpty())
	{
		QMessageBox::information(this, tr("Virtual Drive"), tr("Debes poner un título."));
		//ui->txtEmuTitulo->setFocus();
	} else if (ui->txtVd_etiqueta->text().isEmpty()) {
		QMessageBox::information(this, tr("Virtual Drive"), tr("Debes poner una etiqueta."));
		ui->txtVd_etiqueta->setFocus();
	} else if (ui->txtVd_path_exe->text().isEmpty()) {
		QMessageBox::information(this, tr("Virtual Drive"), tr("Debes indicar el ejecutable del Emulador."));
		ui->txtVd_path_exe->setFocus();
	} else {
		QTreeWidgetItem *item = NULL;
		if (!isAdd)
			item = ui->twVirtualDrive->currentItem();

		int total_vd_find_cfg = 0;
		if (!isAdd && ui->txtVd_etiqueta->text() == item->text(1))
			total_vd_find_cfg = 0;
		else {
			QList<QTreeWidgetItem *> vd_find_cfg = ui->twVirtualDrive->findItems(ui->txtVd_etiqueta->text(), Qt::MatchExactly, 1);
			total_vd_find_cfg = vd_find_cfg.size();
		}

		if (total_vd_find_cfg > 0)
		{
			QMessageBox::information(this, tr("Virtual Drive"), tr("Ya existe la misma etiqueta para el ejecutable del Emulador."));
			ui->txtVd_etiqueta->setFocus();
		} else {
			if (isAdd)
				item = new QTreeWidgetItem;

			QString icono_virtual_drive = ui->cbxVd_icono->itemData(ui->cbxVd_icono->currentIndex()).toString();

			stVirtualDrive virtualDrive;
			virtualDrive.titulo               = ui->txtVd_titulo->text();
			virtualDrive.etiqueta             = ui->txtVd_etiqueta->text();
			virtualDrive.icono                = icono_virtual_drive;
			virtualDrive.path_exe             = ui->txtVd_path_exe->text();
			virtualDrive.path_image           = "";
			virtualDrive.letter               = ui->txtVd_letter->text();
			virtualDrive.param_mount          = ui->txtVd_param_mount->text();
			virtualDrive.param_unmount        = ui->txtVd_param_unmount->text();
			virtualDrive.param_extra_1        = ui->txtVd_param_extra_1->text();
			virtualDrive.param_extra_2        = ui->txtVd_param_extra_2->text();
			virtualDrive.command_line_mount   = ui->txtVd_command_line_mount->text();
			virtualDrive.command_line_unmount = ui->txtVd_command_line_unmount->text();

			item->setIcon( 0, QIcon(fGrl->theme() +"img16/"+ virtualDrive.icono));
			item->setText( 0, virtualDrive.titulo              );
			item->setText( 1, virtualDrive.etiqueta            );
			item->setText( 2, virtualDrive.icono               );
			item->setText( 3, virtualDrive.path_exe            );
			item->setText( 4, virtualDrive.letter              );
			item->setText( 5, virtualDrive.param_mount         );
			item->setText( 6, virtualDrive.param_unmount       );
			item->setText( 7, virtualDrive.param_extra_1       );
			item->setText( 8, virtualDrive.param_extra_2       );
			item->setText( 9, virtualDrive.command_line_mount  );
			item->setText(10, virtualDrive.command_line_unmount);
			item->setSelected(true);

			if (isAdd)
			{
				ui->twVirtualDrive->addTopLevelItem(item);
				virtual_drive_list.insert(virtualDrive.etiqueta, virtualDrive);
			} else {
				ui->twVirtualDrive->setCurrentItem(item);

				if (virtual_drive_etiqueta_old != virtualDrive.etiqueta)
				{
					virtual_drive_list.remove(virtual_drive_etiqueta_old);
					virtual_drive_list.insert(virtualDrive.etiqueta, virtualDrive);

					actualizaDatosVirtualDrive(virtual_drive_etiqueta_old, virtualDrive.etiqueta);
				} else
					virtual_drive_list[virtualDrive.etiqueta] = virtualDrive;
			}

			guardarDatosVirtualDrive();

			on_btnVdCancel_clicked();
		}
	}
}

void frmAddEditVirtualDrive::on_cbxVd_plantilla_cfg_activated(int index)
{
	if (index > -1)
	{
		QString vd_etiqueta = ui->cbxVd_plantilla_cfg->itemData(index).toString();

		setDatosVirtualDrive(virtual_drive_list_tpl[vd_etiqueta]);
	} else
		setDatosVirtualDrive(virtual_drive_list_tpl["NO_VIRTUAL_DRIVE"]);
}

void frmAddEditVirtualDrive::on_txtVd_titulo_textEdited(const QString &arg1)
{
	QString text = fGrl->eliminar_caracteres(arg1);
	ui->txtVd_etiqueta->setText(text.toLower());
}

void frmAddEditVirtualDrive::on_btnVd_path_exe_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlDir.Home, "", tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivos") +" (*)");

	if (!archivo.isEmpty())
	{
		stFileInfo f_info = fGrl->getInfoFile(archivo);
		if (f_info.Exists)
			ui->txtVd_path_exe->setText(archivo);
	}
}

void frmAddEditVirtualDrive::on_btnVd_path_exe_clear_clicked()
{
	ui->txtVd_path_exe->clear();
}

void frmAddEditVirtualDrive::on_btnVd_info_clicked()
{
	QString tpl_info_new = tpl_info_old;
	QString info_contenido = tr("La mayoria de de las unidades virtuales tiene linea de comando para montar y desmontar la imagen ISO, CUE/BIN, IMA, etc.") +"<p>"+
		tr("Ciertas partes de la linea de comando son comunues y otras son especifica para montar o desmontar, para ello se usara distintas etiquetas que se sustituyen por el parametro indicado.") +
		"<table width=\"60%\" border=\"0\" align=\"center\" cellspacing=\"1\" cellpadding=\"2\" class=\"main\">"
		"  <tbody>"
		"    <tr>"
		"      <th width=\"60%\">"+ tr("Descripción") +"</th>"
		"      <th width=\"40%\">"+ tr("Etiqueta") +"</th>"
		"    </tr>"
		"    <tr>"
		"      <td>"+ tr("Letra") +"</td>"
		"      <td align=\"center\">{letter}</td>"
		"    </tr>"
		"    <tr>"
		"      <td>"+ tr("Parametro montaje") +"</td>"
		"      <td align=\"center\">{param_mount}</td>"
		"    </tr>"
		"    <tr>"
		"      <td>"+ tr("Parametro desmontaje") +"</td>"
		"      <td align=\"center\">{param_unmount}</td>"
		"    </tr>"
		"    <tr>"
		"      <td>"+ tr("Parametro extra 1") +"</td>"
		"      <td align=\"center\">{param_extra_1}</td>"
		"    </tr>"
		"    <tr>"
		"      <td>"+ tr("Parametro extra 2") +"</td>"
		"      <td align=\"center\">{param_extra_2}</td>"
		"    </tr>"
		"  </tbody>"
		"</table></p><p>"+
		tr("Ejemplo con la unidad virtual de Alcohol teniendo la siguiente configuración.") +
		"<table width=\"60%\" border=\"0\" align=\"center\" cellspacing=\"1\" cellpadding=\"2\" class=\"main\">"
		"  <tbody>"
		"    <tr>"
		"      <th width=\"60%\">"+ tr("Descripción") +"</th>"
		"      <th width=\"40%\">"+ tr("Valor") +"</th>"
		"    </tr>"
		"    <tr>"
		"      <td>"+ tr("Letra") +"</td>"
		"      <td align=\"center\">V:</td>"
		"    </tr>"
		"    <tr>"
		"      <td>"+ tr("Parametro montaje") +"</td>"
		"      <td align=\"center\">/M:</td>"
		"    </tr>"
		"    <tr>"
		"      <td>"+ tr("Parametro &nbsp;desmontaje") +"</td>"
		"      <td align=\"center\">/U</td>"
		"    </tr>"
		"    <tr>"
		"      <td>"+ tr("Parametro extra 1") +"</td>"
		"      <td>&nbsp;</td>"
		"    </tr>"
		"    <tr>"
		"      <td>"+ tr("Parametro extra 2") +"</td>"
		"      <td>&nbsp;</td>"
		"    </tr>"
		"  </tbody>"
		"</table></p><p>"+
		tr("El montaje real se realizaria de la siguiente manera") +":"
		"<table width=\"100%\" border=\"0\" align=\"center\" cellspacing=\"1\" cellpadding=\"2\" class=\"main\">"
		"    <tr>"
		"      <td>AxCmd.exe V: /M:\"C:\\Juegos\\ISOs\\nombre de la imagen.iso\"</td>"
		"    </tr>"
		"</table></p><p>"+
		tr("Entonces la linea de comando para el montaje seria de la siguiente manera") +":"
		"<table width=\"100%\" border=\"0\" align=\"center\" cellspacing=\"1\" cellpadding=\"2\" class=\"main\">"
		"    <tr>"
		"      <td>{letter} {param_mount}\"{path_image}\"</td>"
		"    </tr>"
		"</table></p><p>"+
		tr("Para el desmontaje seria de la siguiente manera") +":"
		"<table width=\"100%\" border=\"0\" align=\"center\" cellspacing=\"1\" cellpadding=\"2\" class=\"main\">"
		"    <tr>"
		"      <td>{letter} {param_unmount}</td>"
		"    </tr>"
		"</table></p>";

	tpl_info_new.replace("{info_titulo}", tr("Información"));
	tpl_info_new.replace("{info_contenido}", info_contenido);

	sql->ventanaInfo(tr("Información"), "informacion.png", 500, 400, tpl_info_new, false);
}

void frmAddEditVirtualDrive::on_btnVdNew_clicked()
{
	virtual_drive_etiqueta_old = "";

	ui->gBox_datos_virtual_drive->setEnabled(true);
	ui->twVirtualDrive->setEnabled(false);
	ui->btnOk->setEnabled(false);
	ui->btnCancel->setEnabled(false);
	ui->btnVdNew->setEnabled(true);
	ui->btnVdNew->setVisible(false);
	ui->btnVdAdd->setVisible(true);
	ui->btnVdEdit->setEnabled(false);
	ui->btnVdEdit->setVisible(true);
	ui->btnVdUpdate->setVisible(false);
	ui->btnVdDelete->setEnabled(false);
	ui->btnVdCancel->setEnabled(true);

	ui->cbxVd_plantilla_cfg->setCurrentIndex(0);
	on_cbxVd_plantilla_cfg_activated(0);
	ui->txtVd_etiqueta->setText("");
}

void frmAddEditVirtualDrive::on_btnVdAdd_clicked()
{
	addEditTwVirtualDrive(true);
}

void frmAddEditVirtualDrive::on_btnVdEdit_clicked()
{
	virtual_drive_etiqueta_old = ui->txtVd_etiqueta->text();

	ui->gBox_datos_virtual_drive->setEnabled(true);
	ui->twVirtualDrive->setEnabled(false);
	ui->btnOk->setEnabled(false);
	ui->btnCancel->setEnabled(false);
	ui->btnVdNew->setEnabled(false);
	ui->btnVdNew->setVisible(true);
	ui->btnVdAdd->setVisible(false);
	ui->btnVdEdit->setEnabled(true);
	ui->btnVdEdit->setVisible(false);
	ui->btnVdUpdate->setVisible(true);
	ui->btnVdDelete->setEnabled(false);
	ui->btnVdCancel->setEnabled(true);

	int pos = ui->twVirtualDrive->indexOfTopLevelItem(ui->twVirtualDrive->currentItem());
	if (pos != -1)
		on_twVirtualDrive_itemClicked(ui->twVirtualDrive->currentItem(), 0);
}

void frmAddEditVirtualDrive::on_btnVdUpdate_clicked()
{
	int pos = ui->twVirtualDrive->indexOfTopLevelItem(ui->twVirtualDrive->currentItem());
	const int count_mount = ui->twVirtualDrive->topLevelItemCount();
	if (count_mount > 0 && pos != -1)
	{
		addEditTwVirtualDrive(false);
	}
}

void frmAddEditVirtualDrive::on_btnVdDelete_clicked()
{
	int pos = ui->twVirtualDrive->indexOfTopLevelItem(ui->twVirtualDrive->currentItem());
	if (ui->twVirtualDrive->topLevelItemCount() > 0 && pos != -1)
	{
		if (fGrl->questionMsg(tr("¿Eliminar?"), tr("¿Deseas realmente eliminar este emulador de la lista?")))
		{
			actualizaDatosVirtualDrive(ui->twVirtualDrive->currentItem()->text(1), "NO_VIRTUAL_DRIVE");
			fGrl->deleteItemTree(ui->twVirtualDrive->currentItem());
			guardarDatosVirtualDrive();
		}
	} else
		QMessageBox::information(this, tr("Opciones"), tr("Por favor seleccione un emulador de la lista para eliminarlo"));

	if (ui->twVirtualDrive->topLevelItemCount() <= 0)
	{
		on_btnVdCancel_clicked();

		ui->txtVd_titulo->clear();
		ui->txtVd_etiqueta->clear();

		int row_icono = ui->cbxVd_icono->findData("sinimg.png", Qt::UserRole, Qt::MatchExactly);
		if (row_icono < 0) row_icono = 0;

		ui->cbxVd_icono->setCurrentIndex(row_icono);
		ui->txtVd_path_exe->clear();
		ui->txtVd_letter->clear();
		ui->txtVd_param_mount->clear();
		ui->txtVd_param_unmount->clear();
		ui->txtVd_param_extra_1->clear();
		ui->txtVd_param_extra_2->clear();
		ui->txtVd_command_line_mount->clear();
		ui->txtVd_command_line_unmount->clear();
	}
}

void frmAddEditVirtualDrive::on_btnVdCancel_clicked()
{
	ui->gBox_datos_virtual_drive->setEnabled(false);
	ui->twVirtualDrive->setEnabled(true);
	ui->btnOk->setEnabled(true);
	ui->btnCancel->setEnabled(true);
	ui->btnVdNew->setEnabled(true);
	ui->btnVdNew->setVisible(true);
	ui->btnVdAdd->setVisible(false);
	ui->btnVdEdit->setEnabled(true);
	ui->btnVdEdit->setVisible(true);
	ui->btnVdUpdate->setVisible(false);
	ui->btnVdDelete->setEnabled(true);
	ui->btnVdCancel->setEnabled(false);
}

void frmAddEditVirtualDrive::on_twVirtualDrive_itemClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		setDatosVirtualDrive(virtual_drive_list[item->text(1)]);
	}
}

void frmAddEditVirtualDrive::on_twVirtualDrive_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current != previous)
		on_twVirtualDrive_itemClicked(current, 0);
}

void frmAddEditVirtualDrive::on_btnOk_clicked()
{
	QDialog::accept();
}

void frmAddEditVirtualDrive::on_btnCancel_clicked()
{
	QDialog::reject();
}
