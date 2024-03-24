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
#include <QStyleFactory>
#include <QMessageBox>
#include <QColorDialog>
#include <QInputDialog>

#include "grlida_list_icon_cfg.h"
#include "ui_list_icon_cfg.h"

frmListIconCfg::frmListIconCfg(dbSql *m_sql, stGrlCfg m_cfg, int m_id_cat, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmListIconCfg)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql      = m_sql;
	grlCfg   = m_cfg;
	id_cat   = m_id_cat;
	id_theme = 0;

	grlDir.Home   = fGrl->homePath();
	grlDir.Datos  = grlDir.Home +"datos/";
	grlDir.Themes = grlDir.Home +"themes/";
	stTheme       = fGrl->theme();

	cargarConfig();
	setTheme();
	cargarListaCategorias();
	cargarListaThemes();

// centra la aplicacion en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmListIconCfg::~frmListIconCfg()
{
	delete grl_tv_delegate;
	delete grl_lv_delegate;
	delete data_model;
	delete fGrl;

	delete ui;
}

void frmListIconCfg::closeEvent(QCloseEvent *event)
{
	guardarIconConfig();
	event->accept();
}

void frmListIconCfg::cargarConfig()
{
	editor = new CodeEditor(this, CodeEditor::Html);
	ui->verticalLayout_otherThemes->addWidget(editor);

// dark color scheme
	editor->setColor(CodeEditor::Background   , QColor("#232a2d"));
	editor->setColor(CodeEditor::NumberArea   , QColor("#2b3337"));
	editor->setColor(CodeEditor::LineNumber   , QColor("#81969a"));
	editor->setColor(CodeEditor::LineSelect   , QColor("#2f393c"));

	editor->setFormatFor(CodeEditor::Normal     , QColor("#e0e2e4"));
	editor->setFormatFor(CodeEditor::Number     , QColor("#ffcd22"));
	editor->setFormatFor(CodeEditor::NumberCss  , QColor("#ffcd22"));
	editor->setFormatFor(CodeEditor::String     , QColor("#ec7600"));
	editor->setFormatFor(CodeEditor::StringHtml , QColor("#ec7600"));
	editor->setFormatFor(CodeEditor::Comment    , QColor("#66747a"));
	editor->setFormatFor(CodeEditor::CommentDoc , QColor("#66747a"));
	editor->setFormatFor(CodeEditor::Regex      , QColor("#F74692"));
	editor->setFormatFor(CodeEditor::RegexCss   , QColor("#F74692"));
	editor->setFormatFor(CodeEditor::RegexHtml  , QColor("#F74692"));
	editor->setFormatFor(CodeEditor::Operator   , QColor("#e0e2e4"));
	editor->setFormatFor(CodeEditor::Keyword    , QColor("#93c763"), Qt::transparent, true);
	editor->setFormatFor(CodeEditor::KeywordCss , QColor("#668cb0"));
	editor->setFormatFor(CodeEditor::KeywordHtml, QColor("#668cb0"), Qt::transparent, true);
	editor->setFormatFor(CodeEditor::BuiltIn    , QColor("#93c763"), Qt::transparent, true);
	editor->setFormatFor(CodeEditor::BuiltInCss , QColor("#93c763"), Qt::transparent, true);
	editor->setFormatFor(CodeEditor::BuiltInHtml, QColor("#b3b689"), Qt::transparent, true);
	editor->setFormatFor(CodeEditor::Classes    , QColor("#668cb0"), Qt::transparent, true);
	editor->setFormatFor(CodeEditor::ClassesCss , QColor("#93c763"), Qt::transparent, true);

	editor->setFormatFor(CodeEditor::Other      , QColor("#ff8000"));
	editor->setFormatFor(CodeEditor::Marker     , QColor("#e0e2e4"), QColor("#404e51"));
	editor->setFormatFor(CodeEditor::Identifier , QColor("#d5ab55"));

	stGrlDatos dat;
	list_emu.clear();
	list_emu = fGrl->cargaListaDatos(grlDir.Datos +"emu_list.txt", 4, "|");
		dat.titulo = tr("Datos");
		dat.key    = "datos";
		dat.icono  = "cat/datos.png";
	list_emu.insert(0, dat);
		dat.titulo = tr("DOSBox");
		dat.key    = "dosbox";
		dat.icono  = "cat/dosbox.png";
	list_emu.insert(1, dat);
		dat.titulo = tr("ScummVM");
		dat.key    = "scummvm";
		dat.icono  = "cat/scummvm.png";
	list_emu.insert(2, dat);
		dat.titulo = tr("VDMSound");
		dat.key    = "vdmsound";
		dat.icono  = "cat/vdmsound.png";
	list_emu.insert(3, dat);

	QHash<QString, QString> data_model_emu_list;
	foreach (const stGrlDatos &emu, list_emu)
		data_model_emu_list.insert(emu.key, emu.titulo);

	data_model      = new GrlTreeViewModel(data_model_emu_list, this);
	grl_lv_delegate = new GrlListViewDelegate(ui->lvJuegos);
	grl_tv_delegate = new GrlTreeViewDelegate(ui->tvJuegos);

	grl_lv_delegate->setShowLinesPos(true);

	ui->btnIconMode->setChecked(true);
	on_btnIconMode_clicked(true);

	QString sLng = sql->getArchivoIsLng("posiciones.txt");
	fGrl->cargarDatosComboBox(ui->title_font_pos, ":/datos/"+ sLng +"posiciones.txt", "img16/", "", 3, "|");
}

void frmListIconCfg::setTheme()
{
	setWindowIcon(QIcon(fGrl->theme() +"img16/style.png"));

	ui->tabCfgThemes->setTabIcon(0, QIcon(fGrl->theme() +"img16/style.png"));
	ui->tabCfgThemes->setTabIcon(1, QIcon(fGrl->theme() +"img16/datos_1.png"));
	ui->tabCfgWidgets->setTabIcon(0, QIcon(fGrl->theme() +"img16/cover_mode.png"));
	ui->tabCfgWidgets->setTabIcon(1, QIcon(fGrl->theme() +"img16/applications.png"));
// [tw_list_icon]
	ui->btn_tw_icon_width_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_tw_icon_height_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_tw_alternating_row_colors_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
// [picflow_img]
	ui->btn_pf_img_width_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_pf_img_height_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_pf_img_fixsize_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
// [list_icon]
	ui->btn_icon_width_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_icon_height_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
// [item_]
// Option cover
	ui->btn_img_cover_top->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btn_img_cover_top_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_img_cover_top_select->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btn_img_cover_top_select_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_img_border_color_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_img_border_color_select_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_img_cover_top_pos_x_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_img_cover_top_pos_y_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_img_cover_top_zindex_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
// Option image
	ui->btn_img_scaled_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_img_scale_w_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_img_scale_h_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_img_pos_x_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_img_pos_y_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
// Option label emu
	ui->btn_tipo_emu_show_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_tipo_emu_pos_x_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_tipo_emu_pos_y_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
// Option rating
	ui->btn_rating_show_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_rating_vertical_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_rating_pos_x_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_rating_pos_y_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
// Option background title
	ui->btn_title_bg_show_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_title_bg->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btn_title_bg_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_title_bg_select->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btn_title_bg_select_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_title_bg_pos_x_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_title_bg_pos_y_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
// Option title
	ui->btn_title_show_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_title_pos_x_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_title_pos_y_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_title_width_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_title_height_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_title_max_caracteres_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
// Option font title
	ui->btn_title_font_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_title_font_size_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_title_font_pos_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_title_font_color_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_title_font_color_select_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_title_font_wordwrap_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_title_font_bold_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_title_font_italic_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btn_title_font_underline_def->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
// Other themes
	ui->btnIconMode->setIcon(QIcon(fGrl->theme() +"img16/cover_mode.png"));
	ui->btnSave->setIcon(QIcon(fGrl->theme() +"img16/floppy_2.png"));
	ui->btnTextDef->setIcon(QIcon(fGrl->theme() +"img16/reset.png"));
	ui->btnSaveText->setIcon(QIcon(fGrl->theme() +"img16/floppy_2.png"));
	ui->btnOk->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));

	ui->cbxListFiles->clear();
	ui->cbxListFiles->addItem(QIcon(fGrl->theme() +"img16/style.png"), "StyleSheet.qss"        , "StyleSheet.qss"        );
	ui->cbxListFiles->addItem(QIcon(fGrl->theme() +"img16/style.png"), "StyleSheetList.qss"    , "StyleSheetList.qss"    );
	ui->cbxListFiles->addItem(QIcon(fGrl->theme() +"img16/style.png"), "tpl_info.css"          , "tpl_info.css"          );
	ui->cbxListFiles->addItem(QIcon(fGrl->theme() +"img16/html.png") , "tpl_info.html"         , "tpl_info.html"         );
	ui->cbxListFiles->addItem(QIcon(fGrl->theme() +"img16/style.png"), "tpl_info_juego.css"    , "tpl_info_juego.css"    );
	ui->cbxListFiles->addItem(QIcon(fGrl->theme() +"img16/html.png") , "tpl_info_juego.html"   , "tpl_info_juego.html"   );
	ui->cbxListFiles->addItem(QIcon(fGrl->theme() +"img16/style.png"), "tpl_info_juego_no.css" , "tpl_info_juego_no.css" );
	ui->cbxListFiles->addItem(QIcon(fGrl->theme() +"img16/html.png") , "tpl_info_juego_no.html", "tpl_info_juego_no.html");
	ui->cbxListFiles->setCurrentIndex(0);

// StyleSheet
	QString font_usar = grlCfg.font_usar ? "*{font-family:\""+ grlCfg.font_family +"\";font-size:"+ grlCfg.font_size +"pt;}" : "";
	this->setStyleSheet(fGrl->myStyleSheet("StyleSheet.qss") +"\n"+ font_usar);
	this->setStyle(QStyleFactory::create(grlCfg.Style));

	if (grlCfg.StylePalette)
		this->setPalette(QApplication::style()->standardPalette());
	else
		this->setPalette(QApplication::palette());
}

void frmListIconCfg::setColorBtn(QPushButton *btn, QColor color)
{
	int w = 40;
	int h = 16;
	QPixmap img(w, h);
	QPainter painter(&img);
	painter.fillRect(img.rect(), color);
	painter.setPen(Qt::black);
	painter.drawRect(0, 0, w-1, h-1);
	painter.end();
	btn->setIconSize(QSize(w, h));
	btn->setIcon(QIcon(img));
}

// Carga las categorias de la base de datos
void frmListIconCfg::cargarListaCategorias()
{
	QList<stGrlCats> list_cat = sql->getCategorias();

	categoria.clear();
	ui->cbxCategorias->clear();
	const int listCatSize = list_cat.size();
	for (int i = 0; i < listCatSize; ++i)
	{
		categoria.insert(list_cat.at(i).id.toInt(), list_cat.at(i));

		QString titulo_cat = QString("%1 (%2)").arg(list_cat.at(i).titulo).arg(list_cat.at(i).total);

		if (QFile::exists(fGrl->theme() +"img16/"+ list_cat.at(i).img))
			ui->cbxCategorias->addItem(QIcon(fGrl->theme() +"img16/"+ list_cat.at(i).img), titulo_cat, list_cat.at(i).id);
		else
			ui->cbxCategorias->addItem(QIcon(":/img16/sinimg.png"), titulo_cat, list_cat.at(i).id);
	}

	int id_index = ui->cbxCategorias->findData(id_cat);
	if (id_index < 0)
		id_index = 0;

	ui->cbxCategorias->setCurrentIndex(id_index);
}

void frmListIconCfg::cargarListaThemes()
{
	stGrlDatos dat;
	int num_theme = 1;
	QString autor, version;
	QDir dir(grlDir.Themes);
	QStringList list_themes = dir.entryList(QDir::NoDotAndDotDot | QDir::AllDirs);

	dat.key    = "defecto";
	dat.titulo = "Theme: defecto - Autor: Monthy - v0.12.0";
	dat.extra  = "";
	dat.icono  = fGrl->theme() +"img16/style.png";

	themes.clear();
	themes.insert(0, dat);

	ui->cbxThemes->clear();
	ui->cbxThemes->addItem(QIcon(fGrl->theme() +"img16/style.png"), dat.titulo, dat.key);

	const int list_themesSize = list_themes.size();
	for (int i = 0; i < list_themesSize; ++i)
	{
		if (dir.exists(grlDir.Themes + list_themes.at(i)) && list_themes.at(i) != "defecto")
		{
			QSettings settings(grlDir.Themes + list_themes.at(i) +"/info.ini", QSettings::IniFormat);
			settings.beginGroup("info");
				autor   = settings.value("autor"  , "-").toString();
				version = settings.value("version", "-").toString();
			settings.endGroup();

			dat.key    = list_themes.at(i);
			dat.titulo = "Theme: "+ dat.key +" - Autor: "+ autor +" - v"+ version;
			dat.extra  = "";
			dat.icono  = fGrl->theme() +"img16/style.png";

			if (grlCfg.NameDirTheme == dat.key)
				id_theme = num_theme;

			themes.insert(num_theme, dat);
			ui->cbxThemes->addItem(QIcon(dat.icono), dat.titulo, num_theme);
			++num_theme;
		}
	}

	ui->cbxThemes->setCurrentIndex(id_theme);
	on_cbxThemes_activated(id_theme);
}

void frmListIconCfg::cargarListaItems()
{
// Configuración del tvJuegos
	ui->tvJuegos->setModel(data_model);
	ui->tvJuegos->setItemDelegate(grl_tv_delegate);
	ui->tvJuegos->installEventFilter(this);
	ui->tvJuegos->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->tvJuegos->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tvJuegos->setDragDropMode(QAbstractItemView::NoDragDrop);
	ui->tvJuegos->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tvJuegos->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tvJuegos->setProperty("showDropIndicator", QVariant(false));
	ui->tvJuegos->setAlternatingRowColors(true);
	ui->tvJuegos->setRootIsDecorated(false);
	ui->tvJuegos->setUniformRowHeights(true);
	ui->tvJuegos->setItemsExpandable(false);
	ui->tvJuegos->setAllColumnsShowFocus(true);

// Configuración del lvJuegos
	ui->lvJuegos->setModel(data_model);
	ui->lvJuegos->setItemDelegate(grl_lv_delegate);
	ui->lvJuegos->installEventFilter(this);
	ui->lvJuegos->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->lvJuegos->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->lvJuegos->setDragDropMode(QAbstractItemView::NoDragDrop);
	ui->lvJuegos->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->lvJuegos->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->lvJuegos->setAlternatingRowColors(false);
	ui->lvJuegos->setMovement(QListView::Static);
	ui->lvJuegos->setFlow(QListView::LeftToRight);
	ui->lvJuegos->setResizeMode(QListView::Adjust);
	ui->lvJuegos->setSpacing(0);
	ui->lvJuegos->setViewMode(QListView::IconMode);
	ui->lvJuegos->setUniformItemSizes(true);

	QStringList lista_emu_show = categoria[id_cat].emu_show.split(";", QString::SkipEmptyParts);
	int row = 0;
	int row_item = 0;
	int col = 0;
	data_model->clear();
	list_items.clear();
	ui->cbxTipoEmu->clear();
	const int listEmuSize = list_emu.size();
	for (row = 0; row < listEmuSize; ++row)
	{
		if (lista_emu_show.isEmpty() || lista_emu_show.contains("all") || lista_emu_show.contains(list_emu.at(row).key))
		{
			for (col= 0; col < 3; ++col)
			{
				QStandardItem *item = new QStandardItem();
				if (col == col_IdGrl)
				{
					QString numAleatorio = QString::number(qrand() % ((5 + 1) - 0) + 0);

					item->setData("null", IdGrlRole);
					item->setData(QPixmap(stTheme +"images/juego_sin_imagen.png"), ThumbsRole);
					item->setData(list_emu.at(row).icono, IconRole    );
					item->setData("true"                , OriginalRole);
					item->setData(list_emu.at(row).key  , TipoEmuRole );
					item->setData("true"                , FavoritoRole);
					item->setData(numAleatorio          , RatingRole  );
					item->setData(""                    , EdadRole    );
					item->setText("");
				} else if (col == col_Titulo)
					item->setText(list_emu.at(row).titulo);
				else
					item->setText("");
				data_model->setItem(row_item, col, item);
				list_items.insert(list_emu.at(row).key, row_item);
			}

			if (QFile::exists(fGrl->theme() +"img16/"+ list_emu.at(row).icono))
				ui->cbxTipoEmu->addItem(QIcon(fGrl->theme() +"img16/"+ list_emu.at(row).icono), list_emu.at(row).titulo, list_emu.at(row).key);
			else
				ui->cbxTipoEmu->addItem(QIcon(":/img16/sinimg.png"), list_emu.at(row).titulo, list_emu.at(row).key);

			row_item++;
		}
	}

	data_model->setHorizontalHeaderItem(col_Icono , new QStandardItem(QIcon(fGrl->theme() +"img16/bullet_black.png"), tr("Icono")));
	data_model->setHorizontalHeaderItem(col_Titulo, new QStandardItem(QIcon(fGrl->theme() +"img16/tag.png")         , tr("Título")));
	ui->tvJuegos->setColumnWidth(col_Titulo, 360);
	ui->tvJuegos->setColumnHidden(col_IdGrl, true);

	ui->cbxTipoEmu->setCurrentIndex(0);
	on_cbxTipoEmu_activated(0);
}

void frmListIconCfg::cargarIconConfig()
{
	lwIconCfg = fGrl->cargarListWidgetIconConf(categoria[id_cat].tabla, stTheme);
	lwIconCfgtmp = lwIconCfg;

// [tw_list_icon]
	ui->tw_icon_width->setValue(lwIconCfg.tw_icon_width);
	ui->tw_icon_height->setValue(lwIconCfg.tw_icon_height);
	ui->tw_alternating_row_colors->setChecked(lwIconCfg.tw_alternating_row_colors);
	ui->tvJuegos->setAlternatingRowColors(lwIconCfg.tw_alternating_row_colors);
// [picflow_img]
	ui->pf_img_width->setValue(lwIconCfg.pf_img_width);
	ui->pf_img_height->setValue(lwIconCfg.pf_img_height);
	ui->pf_img_fixsize->setChecked(lwIconCfg.pf_img_fixsize);
// [list_icon]
	ui->icon_width->setValue(lwIconCfg.icon_width);
	ui->icon_height->setValue(lwIconCfg.icon_height);
// [item_]
	ui->img_cover_top->setText(lwIconCfg.item[stItem].img_cover_top_path);
	ui->img_cover_top_select->setText(lwIconCfg.item[stItem].img_cover_top_path_select);
	setColorBtn(ui->btn_img_border_color, lwIconCfg.item[stItem].img_border_color.color());
	setColorBtn(ui->btn_img_border_color_select, lwIconCfg.item[stItem].img_border_color_select.color());
	ui->slider_img_border_color_alpha->setValue(lwIconCfg.item[stItem].img_border_color_alpha);
	ui->img_cover_top_bg->setChecked(lwIconCfg.item[stItem].img_cover_top_bg);
	ui->img_cover_top_pos_x->setValue(lwIconCfg.item[stItem].img_cover_top_pos_x);
	ui->img_cover_top_pos_y->setValue(lwIconCfg.item[stItem].img_cover_top_pos_y);
	ui->img_cover_top_zindex->setChecked(lwIconCfg.item[stItem].img_cover_top_zindex);
	ui->img_scaled->setChecked(lwIconCfg.item[stItem].img_scaled);
	ui->img_scale_w->setValue(lwIconCfg.item[stItem].img_scale_w);
	ui->img_scale_h->setValue(lwIconCfg.item[stItem].img_scale_h);
	ui->img_pos_x->setValue(lwIconCfg.item[stItem].img_pos_x);
	ui->img_pos_y->setValue(lwIconCfg.item[stItem].img_pos_y);
	ui->tipo_emu_show->setChecked(lwIconCfg.item[stItem].tipo_emu_show);
	ui->tipo_emu_pos_x->setValue(lwIconCfg.item[stItem].tipo_emu_pos_x);
	ui->tipo_emu_pos_y->setValue(lwIconCfg.item[stItem].tipo_emu_pos_y);
	ui->rating_show->setChecked(lwIconCfg.item[stItem].rating_show);
	ui->rating_vertical->setChecked(lwIconCfg.item[stItem].rating_vertical);
	ui->rating_pos_x->setValue(lwIconCfg.item[stItem].rating_pos_x);
	ui->rating_pos_y->setValue(lwIconCfg.item[stItem].rating_pos_y);
	ui->title_bg_show->setChecked(lwIconCfg.item[stItem].title_bg_show);
	ui->title_bg->setText(lwIconCfg.item[stItem].title_bg_path);
	ui->title_bg_select->setText(lwIconCfg.item[stItem].title_bg_path_select);
	ui->title_bg_pos_x->setValue(lwIconCfg.item[stItem].title_bg_pos_x);
	ui->title_bg_pos_y->setValue(lwIconCfg.item[stItem].title_bg_pos_y);
	ui->title_show->setChecked(lwIconCfg.item[stItem].title_show);
	ui->title_pos_x->setValue(lwIconCfg.item[stItem].title_pos_x);
	ui->title_pos_y->setValue(lwIconCfg.item[stItem].title_pos_y);
	ui->title_width->setValue(lwIconCfg.item[stItem].title_width);
	ui->title_height->setValue(lwIconCfg.item[stItem].title_height);
	ui->title_max_caracteres->setValue(lwIconCfg.item[stItem].title_max_caracteres);
	ui->title_font->setCurrentIndex(ui->title_font->findText(lwIconCfg.item[stItem].title_font));
	ui->title_font_size->setValue(lwIconCfg.item[stItem].title_font_size);
	ui->title_font_pos->setCurrentIndex(lwIconCfg.item[stItem].title_font_pos);
	setColorBtn(ui->btn_title_font_color, lwIconCfg.item[stItem].title_font_color.color());
	setColorBtn(ui->btn_title_font_color_select, lwIconCfg.item[stItem].title_font_color_select.color());
	ui->title_font_wordwrap->setChecked(lwIconCfg.item[stItem].title_font_wordwrap);
	ui->title_font_bold->setChecked(lwIconCfg.item[stItem].title_font_bold);
	ui->title_font_italic->setChecked(lwIconCfg.item[stItem].title_font_italic);
	ui->title_font_underline->setChecked(lwIconCfg.item[stItem].title_font_underline);

	previewIconConfig();
}

void frmListIconCfg::guardarIconConfig()
{
	fGrl->guardarListWidgetIconConf(lwIconCfg, categoria[id_cat].tabla, stTheme);
}

void frmListIconCfg::previewIconConfig()
{
	grl_tv_delegate->setTheme(QSize(lwIconCfg.tw_icon_width, lwIconCfg.tw_icon_height), grlCfg.IconoFav, stTheme, grlCfg.FormatsImage.join(";"));
	grl_lv_delegate->setTheme(lwIconCfg, stTheme);

	ui->tvJuegos->setIconSize(QSize(lwIconCfg.tw_icon_width, lwIconCfg.tw_icon_height));
	ui->tvJuegos->setColumnWidth(col_Icono, lwIconCfg.tw_icon_width + 8);
	ui->lvJuegos->setIconSize(QSize(lwIconCfg.icon_width, lwIconCfg.icon_height));

	ui->lvJuegos->update();
	ui->tvJuegos->update();
}

// Themes
void frmListIconCfg::on_cbxThemes_activated(int index)
{
	if (index > -1)
	{
		id_theme = ui->cbxThemes->itemData(index).toInt();
		stTheme  = grlDir.Themes + themes[id_theme].key +"/";

		ui->tabCfgWidgets->setStyleSheet(fGrl->myStyleSheet("StyleSheet.qss") +"\n");
		ui->lvJuegos->setStyleSheet(fGrl->myStyleSheet("StyleSheetList.qss") +"\n");
		on_cbxListFiles_activated(ui->cbxListFiles->currentIndex());
		on_cbxCategorias_activated(ui->cbxCategorias->currentIndex());
	}
}

// Categorias
void frmListIconCfg::on_cbxCategorias_activated(int index)
{
	if (index > -1)
	{
		id_cat = ui->cbxCategorias->itemData(index).toInt();
		cargarListaItems();
	}
}

void frmListIconCfg::on_cbxTipoEmu_activated(int index)
{
	if (index > -1)
	{
		stItem = "item_"+ ui->cbxTipoEmu->itemData(index).toString();
		cargarIconConfig();

		select_row = data_model->index(list_items[ui->cbxTipoEmu->itemData(index).toString()], col_IdGrl);
		if (ui->btnIconMode->isChecked())
		{
			ui->lvJuegos->clearSelection();
			ui->lvJuegos->setCurrentIndex(select_row);
			ui->lvJuegos->scrollTo(select_row, QAbstractItemView::PositionAtCenter);
		} else {
			ui->tvJuegos->clearSelection();
			ui->tvJuegos->setCurrentIndex(select_row);
			ui->tvJuegos->scrollTo(select_row, QAbstractItemView::PositionAtCenter);
		}
	}
}

// Iconos del treewidget
void frmListIconCfg::on_tw_icon_width_valueChanged(int arg1)
{
	lwIconCfg.tw_icon_width = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_tw_icon_width_def_clicked()
{
	lwIconCfg.tw_icon_width = lwIconCfgtmp.tw_icon_width;
	ui->tw_icon_width->setValue(lwIconCfg.tw_icon_width);
	previewIconConfig();
}

void frmListIconCfg::on_tw_icon_height_valueChanged(int arg1)
{
	lwIconCfg.tw_icon_height = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_tw_icon_height_def_clicked()
{
	lwIconCfg.tw_icon_height = lwIconCfgtmp.tw_icon_height;
	ui->tw_icon_height->setValue(lwIconCfg.tw_icon_height);
	previewIconConfig();
}

void frmListIconCfg::on_tw_alternating_row_colors_toggled(bool checked)
{
	lwIconCfg.tw_alternating_row_colors = checked;
	ui->tvJuegos->setAlternatingRowColors(lwIconCfg.tw_alternating_row_colors);
}

void frmListIconCfg::on_btn_tw_alternating_row_colors_def_clicked()
{
	lwIconCfg.tw_alternating_row_colors = lwIconCfgtmp.tw_alternating_row_colors;
	ui->tw_alternating_row_colors->setChecked(lwIconCfg.tw_alternating_row_colors);
	ui->tvJuegos->setAlternatingRowColors(lwIconCfg.tw_alternating_row_colors);
}

// Imágenes del picturefow
void frmListIconCfg::on_pf_img_width_valueChanged(int arg1)
{
	lwIconCfg.pf_img_width = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_pf_img_width_def_clicked()
{
	lwIconCfg.pf_img_width = lwIconCfgtmp.pf_img_width;
	ui->pf_img_width->setValue(lwIconCfg.pf_img_width);
	previewIconConfig();
}

void frmListIconCfg::on_pf_img_height_valueChanged(int arg1)
{
	lwIconCfg.pf_img_height = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_pf_img_height_def_clicked()
{
	lwIconCfg.pf_img_height = lwIconCfgtmp.pf_img_height;
	ui->pf_img_height->setValue(lwIconCfg.pf_img_height);
	previewIconConfig();
}

void frmListIconCfg::on_pf_img_fixsize_toggled(bool checked)
{
	lwIconCfg.pf_img_fixsize = checked;
}

void frmListIconCfg::on_btn_pf_img_fixsize_def_clicked()
{
	lwIconCfg.pf_img_fixsize = lwIconCfgtmp.pf_img_fixsize;
	ui->pf_img_fixsize->setChecked(lwIconCfg.pf_img_fixsize);
}

// Iconos de la lista en modo caratula
void frmListIconCfg::on_icon_width_valueChanged(int arg1)
{
	lwIconCfg.icon_width = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_icon_width_def_clicked()
{
	lwIconCfg.icon_width = lwIconCfgtmp.icon_width;
	ui->icon_width->setValue(lwIconCfg.icon_width);
	previewIconConfig();
}

void frmListIconCfg::on_icon_height_valueChanged(int arg1)
{
	lwIconCfg.icon_height = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_icon_height_def_clicked()
{
	lwIconCfg.icon_height = lwIconCfgtmp.icon_height;
	ui->icon_height->setValue(lwIconCfg.icon_height);
	previewIconConfig();
}

//--
void frmListIconCfg::on_btn_img_cover_top_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), fGrl->theme() + lwIconCfg.item[stItem].img_cover_top_path, "", tr("Imágenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
	{
	//	if (f_info.Path != fGrl->theme())
	//		fGrl->copiarArchivo(f_info.FilePath, fGrl->theme() +"images/"+ f_info.NameExt, false, true);

		lwIconCfg.item[stItem].img_cover_top_path = "images/"+ f_info.NameExt;
		ui->img_cover_top->setText(lwIconCfg.item[stItem].img_cover_top_path);

		if (!lwIconCfg.item[stItem].img_cover_top.load(stTheme + lwIconCfg.item[stItem].img_cover_top_path))
			lwIconCfg.item[stItem].img_cover_top.load(":/images/list_cover_top.png");

		previewIconConfig();
	}
}

void frmListIconCfg::on_btn_img_cover_top_def_clicked()
{
	lwIconCfg.item[stItem].img_cover_top_path = lwIconCfgtmp.item[stItem].img_cover_top_path;
	ui->img_cover_top->setText(lwIconCfg.item[stItem].img_cover_top_path);

	if (!lwIconCfg.item[stItem].img_cover_top.load(stTheme + lwIconCfg.item[stItem].img_cover_top_path))
		lwIconCfg.item[stItem].img_cover_top.load(":/images/list_cover_top.png");

	previewIconConfig();
}

void frmListIconCfg::on_img_cover_top_editingFinished()
{
	lwIconCfg.item[stItem].img_cover_top_path = ui->img_cover_top->text();

	if (!lwIconCfg.item[stItem].img_cover_top.load(stTheme + lwIconCfg.item[stItem].img_cover_top_path))
		lwIconCfg.item[stItem].img_cover_top.load(":/images/list_cover_top.png");

	previewIconConfig();
}

void frmListIconCfg::on_btn_img_cover_top_select_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), fGrl->theme() + lwIconCfg.item[stItem].img_cover_top_path_select, "", tr("Imágenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
	{
	//	if (f_info.Path != fGrl->theme())
	//		fGrl->copiarArchivo(f_info.FilePath, fGrl->theme() +"images/"+ f_info.NameExt, false, true);

		lwIconCfg.item[stItem].img_cover_top_path_select = "images/"+ f_info.NameExt;
		ui->img_cover_top_select->setText(lwIconCfg.item[stItem].img_cover_top_path_select);

		if (!lwIconCfg.item[stItem].img_cover_top_select.load(stTheme + lwIconCfg.item[stItem].img_cover_top_path_select))
			lwIconCfg.item[stItem].img_cover_top_select.load(":/images/list_cover_top_select.png");

		previewIconConfig();
	}
}

void frmListIconCfg::on_btn_img_cover_top_select_def_clicked()
{
	lwIconCfg.item[stItem].img_cover_top_path_select = lwIconCfgtmp.item[stItem].img_cover_top_path_select;
	ui->img_cover_top_select->setText(lwIconCfg.item[stItem].img_cover_top_path_select);

	if (!lwIconCfg.item[stItem].img_cover_top_select.load(stTheme + lwIconCfg.item[stItem].img_cover_top_path_select))
		lwIconCfg.item[stItem].img_cover_top_select.load(":/images/list_cover_top_select.png");

	previewIconConfig();
}

void frmListIconCfg::on_img_cover_top_select_editingFinished()
{
	lwIconCfg.item[stItem].img_cover_top_path_select = ui->img_cover_top_select->text();

	if (!lwIconCfg.item[stItem].img_cover_top_select.load(stTheme + lwIconCfg.item[stItem].img_cover_top_path_select))
		lwIconCfg.item[stItem].img_cover_top_select.load(":/images/list_cover_top_select.png");

	previewIconConfig();
}

void frmListIconCfg::on_btn_img_border_color_clicked()
{
	QColor color = QColorDialog::getColor(lwIconCfg.item[stItem].img_border_color.color(), this, tr("Color de borde"),  QColorDialog::DontUseNativeDialog);

	if (color.isValid())
	{
		setColorBtn(ui->btn_img_border_color, color);
		lwIconCfg.item[stItem].img_border_color.setColor(color);
		previewIconConfig();
	}
}

void frmListIconCfg::on_btn_img_border_color_def_clicked()
{
	lwIconCfg.item[stItem].title_font_color_select = lwIconCfgtmp.item[stItem].title_font_color_select;
	setColorBtn(ui->btn_img_border_color, lwIconCfg.item[stItem].title_font_color_select.color());
	previewIconConfig();
}

void frmListIconCfg::on_btn_img_border_color_select_clicked()
{
	QColor color = QColorDialog::getColor(lwIconCfg.item[stItem].img_border_color_select.color(), this, tr("Color de fondo"),  QColorDialog::DontUseNativeDialog);

	if (color.isValid())
	{
		setColorBtn(ui->btn_img_border_color_select, color);
		lwIconCfg.item[stItem].img_border_color_select.setColor(color);
		previewIconConfig();
	}
}

void frmListIconCfg::on_btn_img_border_color_select_def_clicked()
{
	lwIconCfg.item[stItem].img_border_color_select = lwIconCfgtmp.item[stItem].img_border_color_select;
	setColorBtn(ui->btn_img_border_color_select, lwIconCfg.item[stItem].img_border_color_select.color());
	previewIconConfig();
}

void frmListIconCfg::on_slider_img_border_color_alpha_valueChanged(int value)
{
	lwIconCfg.item[stItem].img_border_color_alpha = value;
	previewIconConfig();
}

void frmListIconCfg::on_img_cover_top_bg_clicked(bool checked)
{
	lwIconCfg.item[stItem].img_cover_top_bg = checked;
	previewIconConfig();
}

void frmListIconCfg::on_img_cover_top_pos_x_valueChanged(int arg1)
{
	lwIconCfg.item[stItem].img_cover_top_pos_x = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_img_cover_top_pos_x_def_clicked()
{
	lwIconCfg.item[stItem].img_cover_top_pos_x = lwIconCfgtmp.item[stItem].img_cover_top_pos_x;
	ui->img_cover_top_pos_x->setValue(lwIconCfg.item[stItem].img_cover_top_pos_x);
	previewIconConfig();
}

void frmListIconCfg::on_img_cover_top_pos_y_valueChanged(int arg1)
{
	lwIconCfg.item[stItem].img_cover_top_pos_y = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_img_cover_top_pos_y_def_clicked()
{
	lwIconCfg.item[stItem].img_cover_top_pos_y = lwIconCfgtmp.item[stItem].img_cover_top_pos_y;
	ui->img_cover_top_pos_y->setValue(lwIconCfg.item[stItem].img_cover_top_pos_y);
	previewIconConfig();
}

void frmListIconCfg::on_img_cover_top_zindex_clicked(bool checked)
{
	lwIconCfg.item[stItem].img_cover_top_zindex = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_img_cover_top_zindex_def_clicked()
{
	lwIconCfg.item[stItem].img_cover_top_zindex = lwIconCfgtmp.item[stItem].img_cover_top_zindex;
	ui->img_cover_top_zindex->setChecked(lwIconCfg.item[stItem].img_cover_top_zindex);
	previewIconConfig();
}

//--
void frmListIconCfg::on_img_scaled_clicked(bool checked)
{
	lwIconCfg.item[stItem].img_scaled = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_img_scaled_def_clicked()
{
	lwIconCfg.item[stItem].img_scaled = lwIconCfgtmp.item[stItem].img_scaled;
	ui->img_scaled->setChecked(lwIconCfg.item[stItem].img_scaled);
	previewIconConfig();
}

void frmListIconCfg::on_img_scale_w_valueChanged(int arg1)
{
	lwIconCfg.item[stItem].img_scale_w = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_img_scale_w_def_clicked()
{
	lwIconCfg.item[stItem].img_scale_w = lwIconCfgtmp.item[stItem].img_scale_w;
	ui->img_scale_w->setValue(lwIconCfg.item[stItem].img_scale_w);
	previewIconConfig();
}

void frmListIconCfg::on_img_scale_h_valueChanged(int arg1)
{
	lwIconCfg.item[stItem].img_scale_h = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_img_scale_h_def_clicked()
{
	lwIconCfg.item[stItem].img_scale_h = lwIconCfgtmp.item[stItem].img_scale_h;
	ui->img_scale_h->setValue(lwIconCfg.item[stItem].img_scale_h);
	previewIconConfig();
}

void frmListIconCfg::on_img_pos_x_valueChanged(int arg1)
{
	lwIconCfg.item[stItem].img_pos_x = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_img_pos_x_def_clicked()
{
	lwIconCfg.item[stItem].img_pos_x = lwIconCfgtmp.item[stItem].img_pos_x;
	ui->img_pos_x->setValue(lwIconCfg.item[stItem].img_pos_x);
	previewIconConfig();
}

void frmListIconCfg::on_img_pos_y_valueChanged(int arg1)
{
	lwIconCfg.item[stItem].img_pos_y = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_img_pos_y_def_clicked()
{
	lwIconCfg.item[stItem].img_pos_y = lwIconCfgtmp.item[stItem].img_pos_y;
	ui->img_pos_y->setValue(lwIconCfg.item[stItem].img_pos_y);
	previewIconConfig();
}

//--
void frmListIconCfg::on_tipo_emu_show_clicked(bool checked)
{
	lwIconCfg.item[stItem].tipo_emu_show = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_tipo_emu_show_def_clicked()
{
	lwIconCfg.item[stItem].tipo_emu_show = lwIconCfgtmp.item[stItem].tipo_emu_show;
	ui->tipo_emu_show->setChecked(lwIconCfg.item[stItem].tipo_emu_show);
	previewIconConfig();
}

void frmListIconCfg::on_tipo_emu_pos_x_valueChanged(int arg1)
{
	lwIconCfg.item[stItem].tipo_emu_pos_x = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_tipo_emu_pos_x_def_clicked()
{
	lwIconCfg.item[stItem].tipo_emu_pos_x = lwIconCfgtmp.item[stItem].tipo_emu_pos_x;
	ui->tipo_emu_pos_x->setValue(lwIconCfg.item[stItem].tipo_emu_pos_x);
	previewIconConfig();
}

void frmListIconCfg::on_tipo_emu_pos_y_valueChanged(int arg1)
{
	lwIconCfg.item[stItem].tipo_emu_pos_y = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_tipo_emu_pos_y_def_clicked()
{
	lwIconCfg.item[stItem].tipo_emu_pos_y = lwIconCfgtmp.item[stItem].tipo_emu_pos_y;
	ui->tipo_emu_pos_y->setValue(lwIconCfg.item[stItem].tipo_emu_pos_y);
	previewIconConfig();
}

//--
void frmListIconCfg::on_rating_show_clicked(bool checked)
{
	lwIconCfg.item[stItem].rating_show = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_rating_show_def_clicked()
{
	lwIconCfg.item[stItem].rating_show = lwIconCfgtmp.item[stItem].rating_show;
	ui->rating_show->setChecked(lwIconCfg.item[stItem].rating_show);
	previewIconConfig();
}

void frmListIconCfg::on_rating_vertical_clicked(bool checked)
{
	lwIconCfg.item[stItem].rating_vertical = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_rating_vertical_def_clicked()
{
	lwIconCfg.item[stItem].rating_vertical = lwIconCfgtmp.item[stItem].rating_vertical;
	ui->rating_vertical->setChecked(lwIconCfg.item[stItem].rating_vertical);
	previewIconConfig();
}

void frmListIconCfg::on_rating_pos_x_valueChanged(int arg1)
{
	lwIconCfg.item[stItem].rating_pos_x = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_rating_pos_x_def_clicked()
{
	lwIconCfg.item[stItem].rating_pos_x = lwIconCfgtmp.item[stItem].rating_pos_x;
	ui->rating_pos_x->setValue(lwIconCfg.item[stItem].rating_pos_x);
	previewIconConfig();
}

void frmListIconCfg::on_rating_pos_y_valueChanged(int arg1)
{
	lwIconCfg.item[stItem].rating_pos_y = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_rating_pos_y_def_clicked()
{
	lwIconCfg.item[stItem].rating_pos_y = lwIconCfgtmp.item[stItem].rating_pos_y;
	ui->rating_pos_y->setValue(lwIconCfg.item[stItem].rating_pos_y);
	previewIconConfig();
}

//--
void frmListIconCfg::on_title_bg_show_clicked(bool checked)
{
	lwIconCfg.item[stItem].title_bg_show = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_bg_show_def_clicked()
{
	lwIconCfg.item[stItem].title_bg_show = lwIconCfgtmp.item[stItem].title_bg_show;
	ui->title_bg_show->setChecked(lwIconCfg.item[stItem].title_bg_show);
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_bg_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), fGrl->theme() + lwIconCfg.item[stItem].title_bg_path, "", tr("Imágenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
	{
		if (f_info.Path != fGrl->theme())
			fGrl->copiarArchivo(f_info.FilePath, fGrl->theme() +"images/"+ f_info.NameExt, false, true);

		lwIconCfg.item[stItem].title_bg_path = "images/"+ f_info.NameExt;
		ui->title_bg->setText(lwIconCfg.item[stItem].title_bg_path);

		if (!lwIconCfg.item[stItem].title_bg.load(stTheme + lwIconCfg.item[stItem].title_bg_path))
			lwIconCfg.item[stItem].title_bg.load(":/images/list_cover_title_bg.png");

		previewIconConfig();
	}
}

void frmListIconCfg::on_btn_title_bg_def_clicked()
{
	lwIconCfg.item[stItem].title_bg_path = lwIconCfgtmp.item[stItem].title_bg_path;
	ui->title_bg->setText(lwIconCfg.item[stItem].title_bg_path);

	if (!lwIconCfg.item[stItem].title_bg.load(stTheme + lwIconCfg.item[stItem].title_bg_path))
		lwIconCfg.item[stItem].title_bg.load(":/images/list_cover_title_bg.png");

	previewIconConfig();
}

void frmListIconCfg::on_title_bg_editingFinished()
{
	lwIconCfg.item[stItem].title_bg_path = ui->title_bg->text();

	if (!lwIconCfg.item[stItem].title_bg.load(stTheme + lwIconCfg.item[stItem].title_bg_path))
		lwIconCfg.item[stItem].title_bg.load(":/images/list_cover_title_bg.png");

	previewIconConfig();
}

void frmListIconCfg::on_btn_title_bg_select_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), fGrl->theme() + lwIconCfg.item[stItem].title_bg_path_select, "", tr("Imágenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
	{
		if (f_info.Path != fGrl->theme())
			fGrl->copiarArchivo(f_info.FilePath, fGrl->theme() +"images/"+ f_info.NameExt, false, true);

		lwIconCfg.item[stItem].title_bg_path_select = "images/"+ f_info.NameExt;
		ui->title_bg_select->setText(lwIconCfg.item[stItem].title_bg_path_select);

		if (!lwIconCfg.item[stItem].title_bg_select.load(stTheme + lwIconCfg.item[stItem].title_bg_path_select))
			lwIconCfg.item[stItem].title_bg_select.load(":/images/list_cover_title_bg_select.png");

		previewIconConfig();
	}
}

void frmListIconCfg::on_btn_title_bg_select_def_clicked()
{
	lwIconCfg.item[stItem].title_bg_path_select = lwIconCfgtmp.item[stItem].title_bg_path_select;
	ui->title_bg_select->setText(lwIconCfg.item[stItem].title_bg_path_select);

	if (!lwIconCfg.item[stItem].title_bg_select.load(stTheme + lwIconCfg.item[stItem].title_bg_path_select))
		lwIconCfg.item[stItem].title_bg_select.load(":/images/list_cover_title_bg_select.png");

	previewIconConfig();
}

void frmListIconCfg::on_title_bg_select_editingFinished()
{
	lwIconCfg.item[stItem].title_bg_path_select = ui->title_bg_select->text();

	if (!lwIconCfg.item[stItem].title_bg_select.load(stTheme + lwIconCfg.item[stItem].title_bg_path_select))
		lwIconCfg.item[stItem].title_bg_select.load(":/images/list_cover_title_bg_select.png");

	previewIconConfig();
}

void frmListIconCfg::on_title_bg_pos_x_valueChanged(int arg1)
{
	lwIconCfg.item[stItem].title_bg_pos_x = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_bg_pos_x_def_clicked()
{
	lwIconCfg.item[stItem].title_bg_pos_x = lwIconCfgtmp.item[stItem].title_bg_pos_x;
	ui->title_bg_pos_x->setValue(lwIconCfg.item[stItem].title_bg_pos_x);
	previewIconConfig();
}

void frmListIconCfg::on_title_bg_pos_y_valueChanged(int arg1)
{
	lwIconCfg.item[stItem].title_bg_pos_y = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_bg_pos_y_def_clicked()
{
	lwIconCfg.item[stItem].title_bg_pos_y = lwIconCfgtmp.item[stItem].title_bg_pos_y;
	ui->title_bg_pos_y->setValue(lwIconCfg.item[stItem].title_bg_pos_y);
	previewIconConfig();
}

//--
void frmListIconCfg::on_title_show_clicked(bool checked)
{
	lwIconCfg.item[stItem].title_show = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_show_def_clicked()
{
	lwIconCfg.item[stItem].title_show = lwIconCfgtmp.item[stItem].title_show;
	ui->title_show->setChecked(lwIconCfg.item[stItem].title_show);
	previewIconConfig();
}

void frmListIconCfg::on_title_pos_x_valueChanged(int arg1)
{
	lwIconCfg.item[stItem].title_pos_x = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_pos_x_def_clicked()
{
	lwIconCfg.item[stItem].title_pos_x = lwIconCfgtmp.item[stItem].title_pos_x;
	ui->title_pos_x->setValue(lwIconCfg.item[stItem].title_pos_x);
	previewIconConfig();
}

void frmListIconCfg::on_title_pos_y_valueChanged(int arg1)
{
	lwIconCfg.item[stItem].title_pos_y = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_pos_y_def_clicked()
{
	lwIconCfg.item[stItem].title_pos_y = lwIconCfgtmp.item[stItem].title_pos_y;
	ui->title_pos_y->setValue(lwIconCfg.item[stItem].title_pos_y);
	previewIconConfig();
}

void frmListIconCfg::on_title_width_valueChanged(int arg1)
{
	lwIconCfg.item[stItem].title_width = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_width_def_clicked()
{
	lwIconCfg.item[stItem].title_width = lwIconCfgtmp.item[stItem].title_width;
	ui->title_width->setValue(lwIconCfg.item[stItem].title_width);
	previewIconConfig();
}

void frmListIconCfg::on_title_height_valueChanged(int arg1)
{
	lwIconCfg.item[stItem].title_height = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_height_def_clicked()
{
	lwIconCfg.item[stItem].title_height = lwIconCfgtmp.item[stItem].title_height;
	ui->title_height->setValue(lwIconCfg.item[stItem].title_height);
	previewIconConfig();
}

void frmListIconCfg::on_title_max_caracteres_valueChanged(int arg1)
{
	lwIconCfg.item[stItem].title_max_caracteres = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_max_caracteres_def_clicked()
{
	lwIconCfg.item[stItem].title_max_caracteres = lwIconCfgtmp.item[stItem].title_max_caracteres;
	ui->title_max_caracteres->setValue(lwIconCfg.item[stItem].title_max_caracteres);
	previewIconConfig();
}

//--
void frmListIconCfg::on_title_font_activated(const QString &arg1)
{
	lwIconCfg.item[stItem].title_font = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_font_def_clicked()
{
	lwIconCfg.item[stItem].title_font = lwIconCfgtmp.item[stItem].title_font;
	ui->title_font->setCurrentIndex(ui->title_font->findText(lwIconCfg.item[stItem].title_font));
	previewIconConfig();
}

void frmListIconCfg::on_title_font_size_valueChanged(int arg1)
{
	lwIconCfg.item[stItem].title_font_size = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_font_size_def_clicked()
{
	lwIconCfg.item[stItem].title_font_size = lwIconCfgtmp.item[stItem].title_font_size;
	ui->title_font_size->setValue(lwIconCfg.item[stItem].title_font_size);
	previewIconConfig();
}

void frmListIconCfg::on_title_font_pos_activated(int index)
{
	lwIconCfg.item[stItem].title_font_pos = index;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_font_pos_def_clicked()
{
	lwIconCfg.item[stItem].title_font_pos = lwIconCfgtmp.item[stItem].title_font_pos;
	ui->title_font_pos->setCurrentIndex(lwIconCfg.item[stItem].title_font_pos);
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_font_color_clicked()
{
	QColor color = QColorDialog::getColor(lwIconCfg.item[stItem].title_font_color.color(), this, tr("Color de fondo"),  QColorDialog::DontUseNativeDialog);

	if (color.isValid())
	{
		setColorBtn(ui->btn_title_font_color, color);
		lwIconCfg.item[stItem].title_font_color.setColor(color);
		previewIconConfig();
	}
}

void frmListIconCfg::on_btn_title_font_color_def_clicked()
{
	lwIconCfg.item[stItem].title_font_color = lwIconCfgtmp.item[stItem].title_font_color;
	setColorBtn(ui->btn_title_font_color, lwIconCfg.item[stItem].title_font_color.color());
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_font_color_select_clicked()
{
	QColor color = QColorDialog::getColor(lwIconCfg.item[stItem].title_font_color_select.color(), this, tr("Color de fondo"),  QColorDialog::DontUseNativeDialog);

	if (color.isValid())
	{
		setColorBtn(ui->btn_title_font_color_select, color);
		lwIconCfg.item[stItem].title_font_color_select.setColor(color);
		previewIconConfig();
	}
}

void frmListIconCfg::on_btn_title_font_color_select_def_clicked()
{
	lwIconCfg.item[stItem].title_font_color_select = lwIconCfgtmp.item[stItem].title_font_color_select;
	setColorBtn(ui->btn_title_font_color_select, lwIconCfg.item[stItem].title_font_color_select.color());
	previewIconConfig();
}

void frmListIconCfg::on_title_font_wordwrap_clicked(bool checked)
{
	lwIconCfg.item[stItem].title_font_wordwrap = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_font_wordwrap_def_clicked()
{
	lwIconCfg.item[stItem].title_font_wordwrap = lwIconCfgtmp.item[stItem].title_font_wordwrap;
	ui->title_font_wordwrap->setChecked(lwIconCfg.item[stItem].title_font_wordwrap);
	previewIconConfig();
}

void frmListIconCfg::on_title_font_bold_clicked(bool checked)
{
	lwIconCfg.item[stItem].title_font_bold = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_font_bold_def_clicked()
{
	lwIconCfg.item[stItem].title_font_bold = lwIconCfgtmp.item[stItem].title_font_bold;
	ui->title_font_bold->setChecked(lwIconCfg.item[stItem].title_font_bold);
	previewIconConfig();
}

void frmListIconCfg::on_title_font_italic_clicked(bool checked)
{
	lwIconCfg.item[stItem].title_font_italic = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_font_italic_def_clicked()
{
	lwIconCfg.item[stItem].title_font_italic = lwIconCfgtmp.item[stItem].title_font_italic;
	ui->title_font_italic->setChecked(lwIconCfg.item[stItem].title_font_italic);
	previewIconConfig();
}

void frmListIconCfg::on_title_font_underline_clicked(bool checked)
{
	lwIconCfg.item[stItem].title_font_underline = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_font_underline_def_clicked()
{
	lwIconCfg.item[stItem].title_font_underline = lwIconCfgtmp.item[stItem].title_font_underline;
	ui->title_font_underline->setChecked(lwIconCfg.item[stItem].title_font_underline);
	previewIconConfig();
}

//--
void frmListIconCfg::on_btnIconMode_clicked(bool checked)
{
	ui->tvJuegos->setVisible(!checked);
	ui->lvJuegos->setVisible(checked);

	if (checked)
	{
		ui->lvJuegos->clearSelection();
		ui->lvJuegos->setCurrentIndex(select_row);
		ui->lvJuegos->scrollTo(select_row, QAbstractItemView::PositionAtCenter);
	} else {
		ui->tvJuegos->clearSelection();
		ui->tvJuegos->setCurrentIndex(select_row);
		ui->tvJuegos->scrollTo(select_row, QAbstractItemView::PositionAtCenter);
	}
}

void frmListIconCfg::on_btnSave_clicked()
{
	guardarIconConfig();
}

//--
void frmListIconCfg::on_cbxListFiles_activated(int index)
{
	if (index > -1)
	{
		stFileInfo filename = fGrl->getInfoFile(stTheme + ui->cbxListFiles->itemData(index).toString());
		textEditDef = fGrl->leerArchivo(filename.FilePath);

		if (filename.Ext == ".html" || filename.Ext == ".htm")
			editor->setSyntaxType(CodeEditor::Html);
		else if (filename.Ext == ".qss" || filename.Ext == ".css")
			editor->setSyntaxType(CodeEditor::Css);
		else
			editor->setSyntaxType(CodeEditor::JavaScript);
		editor->setPlainText(textEditDef);
	}
}

void frmListIconCfg::on_btnTextDef_clicked()
{
	editor->setPlainText(textEditDef);
}

void frmListIconCfg::on_btnSaveText_clicked()
{
	QString archivo = stTheme + ui->cbxListFiles->itemData(ui->cbxListFiles->currentIndex()).toString();
	fGrl->guardarArchivo(archivo, editor->toPlainText(), "UTF-8");
	on_cbxThemes_activated(id_theme);
}

//--
void frmListIconCfg::on_btnOk_clicked()
{
	guardarIconConfig();
	QDialog::accept();
}

void frmListIconCfg::on_tvJuegos_clicked(const QModelIndex &index)
{
	if (index.isValid())
	{
		guardarIconConfig();

		select_row = data_model->index(index.row(), col_IdGrl);
		ui->cbxTipoEmu->setCurrentIndex(ui->cbxTipoEmu->findData(select_row.data(TipoEmuRole).toString()));
		on_cbxTipoEmu_activated(ui->cbxTipoEmu->currentIndex());
	}
}

void frmListIconCfg::on_lvJuegos_clicked(const QModelIndex &index)
{
	if (index.isValid())
	{
		guardarIconConfig();

		select_row = data_model->index(index.row(), col_IdGrl);
		ui->cbxTipoEmu->setCurrentIndex(ui->cbxTipoEmu->findData(select_row.data(TipoEmuRole).toString()));
		on_cbxTipoEmu_activated(ui->cbxTipoEmu->currentIndex());
	}
}
