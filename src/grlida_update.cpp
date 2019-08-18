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

#include "grlida_update.h"
#include "ui_update.h"

frmUpdate::frmUpdate(QList<stUpdates> m_up_grl, QList<stUpdates> m_up_js, QList<stUpdates> m_up_st, stUpdates m_up_svm, stGrlCfg m_cfg, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmUpdate)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	grlCfg = m_cfg;

	grlDir.Home    = fGrl->dirApp();
	grlDir.Datos   = grlDir.Home +"datos/";
	grlDir.Scripts = grlDir.Home +"scripts/";
	grlDir.Temp    = grlDir.Home +"temp/";
	grlDir.Themes  = grlDir.Home +"themes/";

	cargarConfig();

	const int listUpGrlSize = m_up_grl.size();

	ui->twUpdates->clear();
	if (listUpGrlSize > 0)
	{
		twAddCat("GR-lida", m_up_grl.at(0).info, fGrl->theme() +"img16/gr-lida.png");

		for (int i = 0; i < listUpGrlSize; ++i)
			twAddSubCat(m_up_grl.at(i), fGrl->theme() +"img16/gr-lida.png", "POST");

		ui->twUpdates->clearSelection();
		ui->twUpdates->topLevelItem(0)->child(0)->setSelected(true);
		emit on_twUpdates_itemClicked(ui->twUpdates->topLevelItem(0)->child(0), col_title);
	}

	const int listUpJsSize = m_up_js.size();
	if (listUpJsSize > 0)
	{
		twAddCat(tr("Script para importar datos del juego"), m_up_js.at(0).info, fGrl->theme() +"img16/script.png");

		for (int i = 0; i < listUpJsSize; ++i)
			twAddSubCat(m_up_js.at(i), fGrl->theme() +"img16/script.png", "POST");
	}

	const int listUpStSize = m_up_st.size();
	if (listUpStSize > 0)
	{
		twAddCat(tr("Estilos o themes para el GR-lida"), m_up_st.at(0).info, fGrl->theme() +"img16/style.png");

		for (int i = 0; i < listUpStSize; ++i)
			twAddSubCat(m_up_st.at(i), fGrl->theme() +"img16/style.png", "POST");
	}

	if (!m_up_svm.info.isEmpty())
	{
		twAddCat(tr("Lista de compatibilidad del ScummVM"), m_up_svm.info, fGrl->theme() +"img16/cat/scummvm.png");
			twAddSubCat(m_up_svm, fGrl->theme() +"img16/cat/scummvm.png", "GET");
	}

// centra la aplicacion en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmUpdate::~frmUpdate()
{
	z_file->closeArchive();
	delete z_file;

	delete ui;
}

void frmUpdate::cargarConfig()
{
// Configuración del twInfo
	ui->twUpdates->headerItem()->setIcon(col_title, QIcon(fGrl->theme() +"img16/tag.png"));
	ui->twUpdates->headerItem()->setTextAlignment(col_ver_old, Qt::AlignCenter);
	ui->twUpdates->headerItem()->setTextAlignment(col_version, Qt::AlignCenter);
	ui->twUpdates->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
	ui->twUpdates->header()->setSectionsMovable(false);
	ui->twUpdates->header()->setSectionResizeMode(col_title  , QHeaderView::Stretch);
	ui->twUpdates->header()->setSectionResizeMode(col_ver_old, QHeaderView::ResizeToContents);
	ui->twUpdates->header()->setSectionResizeMode(col_version, QHeaderView::ResizeToContents);
#else
	ui->twUpdates->header()->setMovable(false);
	ui->twUpdates->header()->setResizeMode(col_title  , QHeaderView::Stretch);
	ui->twUpdates->header()->setResizeMode(col_ver_old, QHeaderView::ResizeToContents);
	ui->twUpdates->header()->setResizeMode(col_version, QHeaderView::ResizeToContents);//Interactive
#endif
	ui->twUpdates->setColumnWidth(col_ver_old, 60);
	ui->twUpdates->setColumnWidth(col_version, 60);

	httpdown = new HttpDownload(ui->wInfo);
	ui->hLayout_wInfo->addWidget(httpdown);
	httpdown->setHttpWindowTitle(tr("Actualizaciones"));
	httpdown->setWindowIcon(QIcon(fGrl->theme() +"img16/descarga.png"));

	if (grlCfg.ProxyEnable)
		httpdown->setHttpProxy(grlCfg.ProxyType, grlCfg.ProxyHost, grlCfg.ProxyPort, grlCfg.ProxyUserName, grlCfg.ProxyPassword);

	connect(httpdown, SIGNAL(statusFinished()), this, SLOT(statusFinished()));
	connect(httpdown, SIGNAL(statusBtnEnabled(bool)), this, SLOT(statusBtnEnabled(bool)));

	z_file = new Qt7zip();
	is_load_7zlib = z_file->isLoad7zLib();

	ui->txtInfo->document()->clear();
	ui->txtInfo->document()->setDocumentMargin(0);

	setTheme();
}

void frmUpdate::setTheme()
{
	setWindowIcon(QIcon(fGrl->theme() +"img16/actualizar.png"));

	ui->btnUpdate->setText(tr("Descargar") +" / "+ tr("Actualizar"));

	ui->lb_update_ico->setPixmap(QPixmap(fGrl->theme() +"img16/actualizar.png"));
	ui->btnUpdate->setIcon(QIcon(fGrl->theme() +"img16/descarga.png"));
	ui->btnClose->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));

	tpl_info_old = fGrl->tplInfoJuego("tpl_info");

	QString tpl_info_new = tpl_info_old;
	tpl_info_new.replace("{info_contenido}", "");
	ui->txtInfo->setHtml(tpl_info_new);
}

void frmUpdate::twAddCat(QString etiqueta, QString info, QString icono)
{
	m_font.setBold(true);

	twItem = new QTreeWidgetItem;
	twItem->setIcon(col_title     , QIcon(icono));
	twItem->setFont(col_title     , m_font  );
	twItem->setText(col_title     , etiqueta);
	twItem->setText(col_ver_old   , ""      );
	twItem->setText(col_version   , ""      );
	twItem->setText(col_info      , info    );
	twItem->setText(col_info_item , ""      );
	twItem->setText(col_tipo      , ""      );
	twItem->setText(col_file      , ""      );
	twItem->setText(col_url_method, ""      );
	twItem->setText(col_url       , ""      );

	ui->twUpdates->addTopLevelItem(twItem);
	twItem->setExpanded(true);
}

void frmUpdate::twAddSubCat(stUpdates up, QString icono, QString url_method)
{
	if (up.ver_old == up.version || up.ver_old.isEmpty() || up.isNew == false)
		m_font.setBold(false);
	else
		m_font.setBold(true);

	QTreeWidgetItem *sub_cat = new QTreeWidgetItem;
	sub_cat->setTextAlignment(col_ver_old, Qt::AlignCenter);
	sub_cat->setTextAlignment(col_version, Qt::AlignCenter);
	sub_cat->setCheckState(col_title, up.isNew ? Qt::Checked : Qt::Unchecked);
	sub_cat->setFont(col_version   , m_font      );
	sub_cat->setIcon(col_title     , QIcon(icono));
	sub_cat->setText(col_title     , up.title    );
	sub_cat->setText(col_ver_old   , up.ver_old  );
	sub_cat->setText(col_version   , up.version  );
	sub_cat->setText(col_info      , up.info     );
	sub_cat->setText(col_info_item , up.info_item);
	sub_cat->setText(col_tipo      , up.tipo     );
	sub_cat->setText(col_file      , up.file     );
	sub_cat->setText(col_url_method, url_method  );
	sub_cat->setText(col_url       , up.url      );

	twItem->addChild(sub_cat);
}

bool frmUpdate::extractFile(const QString file_in, const QString dir_out)
{
	bool isOk = false;
	if (is_load_7zlib)
	{
		if (z_file->isOpen())
			z_file->closeArchive();
		if (z_file->open(file_in))
			isOk = z_file->extract(dir_out);
	}

	return isOk;
}

void frmUpdate::statusBtnEnabled(bool estado)
{
	ui->twUpdates->setEnabled(estado);
	ui->btnUpdate->setEnabled(estado);
	ui->btnClose->setEnabled(estado);
}

void frmUpdate::statusFinished()
{
	const int listFileSize = listFiles.size();
	for (int i = 0; i < listFileSize; ++i)
	{
		if (listFiles.at(i).tipo == "scripts")
		{
			extractFile(grlDir.Temp + listFiles.at(i).archivo, grlDir.Scripts);
		}
		if (listFiles.at(i).tipo == "themes")
		{
			extractFile(grlDir.Temp + listFiles.at(i).archivo, grlDir.Themes);
		}
		if (listFiles.at(i).tipo == "list_svm")
		{
			fGrl->moverArchivo(grlDir.Temp + listFiles.at(i).archivo, grlDir.Datos + listFiles.at(i).archivo, true, true);
		}
	}

	QDialog::accept();
}

void frmUpdate::on_twUpdates_itemClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		QString info = item->text(col_info);

		if (!item->text(col_info_item).isEmpty())
			info.append("<br><br>"+ item->text(col_info_item));

		QString tpl_info_new = tpl_info_old;
			tpl_info_new.replace("{info_titulo}"   , item->text(col_title));
			tpl_info_new.replace("{info_contenido}", info                 );
			tpl_info_new.replace("<theme>"         , fGrl->theme()        );
		ui->txtInfo->setHtml(tpl_info_new);
	}
}

void frmUpdate::on_btnUpdate_clicked()
{
	stDownItem itemdown;
	QList<stDownItem> listDown;
	listFiles.clear();
	const int listSize = ui->twUpdates->topLevelItemCount();
	for (int i = 0; i < listSize; ++i)
	{
		QTreeWidgetItem *item = ui->twUpdates->topLevelItem(i);
		for (int n = 0; n < item->childCount(); ++n)
		{
			if (item->child(n)->checkState(col_title) == Qt::Checked)
			{
				itemdown.urlfile     = URL_GRL+ item->child(n)->text(col_file);
				itemdown.fileName    = grlDir.Temp + item->child(n)->text(col_file);
				itemdown.tipo        = item->child(n)->text(col_url_method);
				itemdown.contentPost = item->child(n)->text(col_url);

				if (itemdown.tipo == "POST")
					itemdown.urlfile = URL_GRL +"download.php";

				listDown << itemdown;

				stListFiles itemfile;
				itemfile.archivo = item->child(n)->text(col_file);
				itemfile.tipo    = item->child(n)->text(col_tipo);

				if (itemfile.tipo == "list_svm")
					grlCfg.VerListSvm = item->child(n)->text(col_version);

				listFiles << itemfile;
			}
		}
	}
	httpdown->defaultInfo();
	httpdown->setConfirmRedirect(false);
	httpdown->downloadMultiFiles(listDown);
}

void frmUpdate::on_btnClose_clicked()
{
	QDialog::reject();
}
