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

#include "grlida_delegate.h"

// INICIO GrlListViewDelegate -----------------------------------------------------------------------------------
GrlListViewDelegate::GrlListViewDelegate(QWidget *parent)
	:QStyledItemDelegate(parent)
{
	fGrl = new Funciones;
	stDirApp = fGrl->dirApp();
	m_showLinesPos = false;
}

GrlListViewDelegate::~GrlListViewDelegate()
{
	delete fGrl;
}

void GrlListViewDelegate::setTheme(stLwIconCfg m_lwConf, QString dir_theme)
{
	lwConf = m_lwConf;

	if (!star_on.load(dir_theme +"img16/star_on.png"))
		star_on.load(":/img16/star_on.png");

	if (!star_off.load(dir_theme +"img16/star_off.png"))
		star_off.load(":/img16/star_off.png");

	ico_emu.clear();
	ico_emu.insert("datos"   , QPixmap(dir_theme +"img16/cat/datos.png"   ));
	ico_emu.insert("dosbox"  , QPixmap(dir_theme +"img16/cat/dosbox.png"  ));
	ico_emu.insert("scummvm" , QPixmap(dir_theme +"img16/cat/scummvm.png" ));
	ico_emu.insert("vdmsound", QPixmap(dir_theme +"img16/cat/vdmsound.png"));

	const QHash<QString, stGrlDatos> emu_list = fGrl->cargaDatosQHash(stDirApp +"datos/emu_list.txt", 4, "|");
	foreach (const stGrlDatos &dat, emu_list)
		ico_emu.insert(dat.key, QPixmap(dir_theme +"img16/"+ dat.icono));

	pen = QApplication::palette().text().color();
}

// Seudo Sombra. Representacion de sombra
void GrlListViewDelegate::paintShadow(QPainter *painter, QPen m_pen, int x, int y, int w, int h, int a) const
{
	int r = 0; int g = 0; int b = 0;
	r = qRed(m_pen.color().rgb());
	g = qGreen(m_pen.color().rgb());
	b = qBlue(m_pen.color().rgb());

	m_pen.setStyle(Qt::SolidLine);
	m_pen.setBrush(QColor(r, g, b, 60 + a));
	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->setPen(m_pen);
//	painter->drawRect(QRectF(x, y, w, h));
	painter->drawRoundedRect(x, y, w, h, 2, 2);

	m_pen.setBrush(QColor(r, g, b, 45 + a));
	painter->setPen(m_pen);
//	painter->drawRect(QRectF(x - 1, y - 1, w + 2, h + 2));
	painter->drawRoundedRect(x - 1, y - 1, w + 2, h, 2, 2);

	m_pen.setBrush(QColor(r, g, b, 30 + a));
	painter->setPen(m_pen);
//	painter->drawRect(QRectF(x - 2, y - 2, w + 4, h + 4));
	painter->drawRoundedRect(x - 2, y - 2, w + 4, h, 2, 2);

	m_pen.setBrush(QColor(r, g, b, 18 + a));
	painter->setPen(m_pen);
//	painter->drawRect(QRectF(x - 3, y - 3, w + 6, h + 6));
	painter->drawRoundedRect(x - 3, y - 3, w + 6, h, 2, 2);

	painter->setRenderHint(QPainter::Antialiasing, false);
}

void GrlListViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const
{
	int rect_x = option.rect.x();
	int rect_y = option.rect.y();
	QStyledItemDelegate::paint(painter, option, index);
	QPen pen_lines = pen;
	pen_lines.setStyle(Qt::DashLine);

//	QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
//	QApplication::restoreOverrideCursor();

	QString str_emu = index.sibling(index.row(), col_IdGrl).data(TipoEmuRole).toString();
	QString stItem = !lwConf.item.contains("item_"+ str_emu) ? "item_datos" : "item_"+ str_emu;

	if (m_showLinesPos)
	{
		pen_lines.setBrush(Qt::blue);
		painter->setPen(pen_lines);
		painter->drawRect(rect_x, rect_y, lwConf.icon_width, lwConf.icon_height);
	}

	int pos_x, pos_y, icon_width, icon_height;
	QPixmap icon = qvariant_cast<QPixmap>(index.sibling(index.row(), col_IdGrl).data(ThumbsRole));

	if (lwConf.item[stItem].img_scaled)
	{
		icon_width  = lwConf.item[stItem].img_scale_w;
		icon_height = lwConf.item[stItem].img_scale_h;

		pos_x = (lwConf.icon_width / 2) - (icon_width / 2) + lwConf.item[stItem].img_pos_x;
		pos_y = lwConf.icon_height - icon_height - lwConf.item[stItem].img_pos_y;
	} else {
		icon_width  = icon.width();
		icon_height = icon.height();

		pos_x = (lwConf.icon_width / 2) - (icon_width / 2) + lwConf.item[stItem].img_pos_x;
		pos_y = lwConf.icon_height - icon.height() - lwConf.item[stItem].img_pos_y;
	}

	if (lwConf.item[stItem].img_cover_top_bg)
	{
		int cover_top_pos_x = (lwConf.icon_width / 2) - (lwConf.item[stItem].img_cover_top.width() / 2) + lwConf.item[stItem].img_cover_top_pos_x;
		int cover_top_pos_y = lwConf.icon_height - lwConf.item[stItem].img_cover_top.height() - lwConf.item[stItem].img_cover_top_pos_y;

		// Dibujamos la carátula.
		if (lwConf.item[stItem].img_cover_top_zindex)
			painter->drawPixmap(rect_x + pos_x, rect_y + pos_y, icon_width, icon_height, icon);

		if (option.state & QStyle::State_Selected)
			painter->drawPixmap(rect_x + cover_top_pos_x, rect_y + cover_top_pos_y, lwConf.item[stItem].img_cover_top_select);
		else
			painter->drawPixmap(rect_x + cover_top_pos_x, rect_y + cover_top_pos_y, lwConf.item[stItem].img_cover_top);

		if (option.state & QStyle::State_MouseOver)
			painter->drawPixmap(rect_x + cover_top_pos_x, rect_y + cover_top_pos_y, lwConf.item[stItem].img_cover_top_select);

		// Dibujamos la carátula.
		if (!lwConf.item[stItem].img_cover_top_zindex)
			painter->drawPixmap(rect_x + pos_x, rect_y + pos_y, icon_width, icon_height, icon);
	} else {
		if (option.state & QStyle::State_Selected)
			pen_lines.setColor(lwConf.item[stItem].img_border_color_select.color());
		else
			pen_lines.setColor(lwConf.item[stItem].img_border_color.color());

		// Dibujamos la seudo sombra.
		paintShadow(painter, pen_lines, rect_x + pos_x, rect_y + pos_y, icon_width, icon_height, lwConf.item[stItem].img_border_color_alpha);

		if (option.state & QStyle::State_MouseOver)
			paintShadow(painter, pen_lines, rect_x + pos_x, rect_y + pos_y, icon_width, icon_height, lwConf.item[stItem].img_border_color_alpha);

		// Dibujamos la carátula.
		painter->drawPixmap(rect_x + pos_x, rect_y + pos_y, icon_width, icon_height, icon);

		// Dibujamos tapa superior.
		if (option.state & QStyle::State_Selected)
			painter->drawPixmap(rect_x + pos_x, rect_y + pos_y, icon_width, icon_height, lwConf.item[stItem].img_cover_top_select);
		else
			painter->drawPixmap(rect_x + pos_x, rect_y + pos_y, icon_width, icon_height, lwConf.item[stItem].img_cover_top);
	}

	pen_lines.setStyle(Qt::DashLine);
	painter->setPen(pen_lines);

// Dibujamos el fondo del título
	if (lwConf.item[stItem].title_bg_show)
	{
		if (option.state & QStyle::State_Selected)
			painter->drawPixmap(QPointF(rect_x + lwConf.item[stItem].title_bg_pos_x, rect_y + lwConf.item[stItem].title_bg_pos_y), lwConf.item[stItem].title_bg_select);
		else
			painter->drawPixmap(QPointF(rect_x + lwConf.item[stItem].title_bg_pos_x, rect_y + lwConf.item[stItem].title_bg_pos_y), lwConf.item[stItem].title_bg);
	}

// Dibujamos el título
	if (lwConf.item[stItem].title_show)
	{
		QString titulo = index.sibling(index.row(), col_Titulo).data().toString();
		QRectF rect_font(rect_x + lwConf.item[stItem].title_pos_x, rect_y + lwConf.item[stItem].title_pos_y, lwConf.item[stItem].title_width, lwConf.item[stItem].title_height);
		QFont font;
		font.setFamily(lwConf.item[stItem].title_font);
		font.setPointSize(lwConf.item[stItem].title_font_size);
		font.setBold(lwConf.item[stItem].title_font_bold);
		font.setItalic(lwConf.item[stItem].title_font_italic);
		font.setUnderline(lwConf.item[stItem].title_font_underline);
		font.setStyleStrategy(QFont::PreferAntialias);
		painter->setFont(font);

		if (option.state & QStyle::State_Selected)
			painter->setPen(lwConf.item[stItem].title_font_color_select);
		else
			painter->setPen(lwConf.item[stItem].title_font_color);

		if (titulo.size() > lwConf.item[stItem].title_max_caracteres)
			painter->drawText(rect_font, fGrl->getPosition(lwConf.item[stItem].title_font_pos, lwConf.item[stItem].title_font_wordwrap), titulo.left(lwConf.item[stItem].title_max_caracteres) +"..");
		else
			painter->drawText(rect_font, fGrl->getPosition(lwConf.item[stItem].title_font_pos, lwConf.item[stItem].title_font_wordwrap), titulo);

		if (m_showLinesPos)
		{
			pen_lines.setBrush(Qt::green);
			painter->setPen(pen_lines);
			painter->drawRect(rect_font);
		}

		painter->setPen(pen);
	}

// Dibujamos tipo de emulador
	if (lwConf.item[stItem].tipo_emu_show)
		painter->drawPixmap(QPointF(rect_x + lwConf.item[stItem].tipo_emu_pos_x, rect_y + lwConf.item[stItem].tipo_emu_pos_y), ico_emu[str_emu]);

// Dibujamos el rating
	if (lwConf.item[stItem].rating_show)
	{
		int rating = index.sibling(index.row(), col_IdGrl).data(RatingRole).toInt();
		int rating_x = rect_x + lwConf.item[stItem].rating_pos_x;
		int rating_y = rect_y + lwConf.item[stItem].rating_pos_y;
		for (int i = 0; i < 5; ++i)
		{
			if (i < rating)
				painter->drawPixmap(QPointF(rating_x, rating_y), star_on);
			else
				painter->drawPixmap(QPointF(rating_x, rating_y), star_off);

			if (lwConf.item[stItem].rating_vertical)
				rating_y -= star_on.height();
			else
				rating_x += star_on.width();
		}
	}
}

QSize GrlListViewDelegate::sizeHint(const QStyleOptionViewItem &option,
		const QModelIndex &index) const
{
	return QSize(lwConf.icon_width, lwConf.icon_height);
	return QStyledItemDelegate::sizeHint(option, index);
}

bool GrlListViewDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
		const QStyleOptionViewItem &option, const QModelIndex &index)
{
	if (index.column() == col_IdGrl)
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
			QString str_emu = index.sibling(index.row(), col_IdGrl).data(TipoEmuRole).toString();
			QString stItem = !lwConf.item.contains("item_"+ str_emu) ? "item_datos" : "item_"+ str_emu;

			if (!index.data(RatingRole).toString().isEmpty() && lwConf.item[stItem].rating_show)
			{
				int stars, pos_x, pos_y, rating_x, rating_y, limit_x, limit_y;
				pos_x    = mouseEvent->pos().x();
				pos_y    = mouseEvent->pos().y();
				rating_x = option.rect.x() + lwConf.item[stItem].rating_pos_x;
				if (lwConf.item[stItem].rating_vertical)
				{
					rating_y = option.rect.y() + lwConf.item[stItem].rating_pos_y + star_on.height() - (5 * star_on.height());
					limit_x  = star_on.width();
					limit_y  = 5 * star_on.height();
				} else {
					rating_y = option.rect.y() + lwConf.item[stItem].rating_pos_y;
					limit_x  = 5 * star_on.width();
					limit_y  = star_on.height();
				}
				if (pos_x > rating_x && pos_y > rating_y && pos_x <= (rating_x + limit_x) && pos_y <= (rating_y + limit_y))
				{
					if (lwConf.item[stItem].rating_vertical)
						stars = qAbs(qBound(0, int(0.5 + qreal(pos_y - rating_y) / star_on.height()), 5)-5);
					else
						stars = qBound(0, int(0.5 + qreal(pos_x - rating_x) / star_on.width()), 5);

					model->setData(index, QVariant(stars).toString(), RatingRole);
					emit updateCol(index, col_Rating);
				}
			}
			return false;
		}
		return true;
	} else
		return QStyledItemDelegate::editorEvent(event, model, option, index);
}
// FIN GrlListViewDelegate --------------------------------------------------------------------------------------

// INICIO GrlTreeViewModel --------------------------------------------------------------------------------------
GrlTreeViewModel::GrlTreeViewModel(QHash<QString, QString> list_emu, QObject *parent, QString formato_fecha)
	: QStandardItemModel(parent)
{
	emu_list = list_emu;
	setFormatoFecha(formato_fecha);
}

void GrlTreeViewModel::setFormatoFecha(QString formato_fecha)
{
	formatofecha = formato_fecha.isEmpty() ? "dd/MM/yyyy HH:mm:ss" : formato_fecha;
}

QVariant GrlTreeViewModel::data(const QModelIndex& index, int role) const
{
	if (index.isValid())
	{
		int column = index.column();
		if (role == Qt::TextAlignmentRole)
		{
			switch (column)
			{
				case col_Anno: return Qt::AlignCenter;
				case col_Tamano: return Qt::AlignCenter;
				case col_Graficos: return Qt::AlignCenter;
				case col_Sonido: return Qt::AlignCenter;
				case col_Jugabilidad: return Qt::AlignCenter;
				case col_Fecha: return Qt::AlignCenter;
				case col_TipoEmu: return Qt::AlignCenter;
			}
		}
		else if (role == Qt::DisplayRole)
		{
			switch (column)
			{
				case col_Fecha:
				{
					QDateTime dt;
					dt.setTime_t(QStandardItemModel::data(index, Qt::DisplayRole).toUInt());
					return ""+ dt.toString(formatofecha);
				}
				case col_Genero:
				case col_Compania:
				case col_Desarrollador:
				case col_Tema:
				case col_Grupo:
				case col_Perspectiva:
				case col_Idioma:
				case col_IdiomaVoces:
				case col_Formato:
				case col_SistemaOp:
				{
					return QStandardItemModel::data(index, Qt::DisplayRole).toString().replace(";", "; ");
				}
				case col_TipoEmu:
				{
					return emu_list[QStandardItemModel::data(index, Qt::DisplayRole).toString()];
				}
			};
		}
	}

	return QStandardItemModel::data(index, role);
}
// FIN GrlTreeViewModel -----------------------------------------------------------------------------------------

// INICIO GrlTreeViewDelegate -----------------------------------------------------------------------------------
GrlTreeViewDelegate::GrlTreeViewDelegate(QWidget *parent)
	:QStyledItemDelegate(parent)
{
	fGrl = new Funciones;
	stDirApp  = fGrl->dirApp();
	size_icon = QSize(32, 32);
}

GrlTreeViewDelegate::~GrlTreeViewDelegate()
{
	delete fGrl;
}

void GrlTreeViewDelegate::setTheme(QSize m_size, QString m_fav, QString dir_theme, QString filter)
{
	size_icon = m_size.isEmpty() ? QSize(32, 32) : m_size;

	if (!star_on.load(dir_theme +"img16/star_on.png"))
		star_on.load(":/img16/star_on.png");

	if (!star_off.load(dir_theme +"img16/star_off.png"))
		star_off.load(":/img16/star_off.png");

	if (!sin_img.load(dir_theme +"img16/sinimg.png"))
		sin_img.load(":/img16/sinimg.png");

	if (!img_original.load(dir_theme +"img16/original.png"))
		img_original.load(":/img16/original.png");

	if (!img_favorito.load(dir_theme +"img16/"+ m_fav))
		img_favorito.load(":/img16/fav_0.png");

	edades.clear();
	fGrl->insertaIconos(edades, dir_theme, "img16/", "", filter, "edad_");

	icono.clear();
	icono.insert("datos.png"   , QPixmap(dir_theme +"img24/datos.png")  );
	icono.insert("dosbox.png"  , QPixmap(dir_theme +"img24/dosbox.png") );
	icono.insert("pc.png"      , QPixmap(dir_theme +"img24/pc.png")     );
	icono.insert("scummvm.png" , QPixmap(dir_theme +"img24/datos.png")  );
	icono.insert("vdmsound.png", QPixmap(dir_theme +"img24/scummvm.png"));

	fGrl->insertaIconos(icono, stDirApp , "iconos/"   , "<dir_iconos>/", filter);
	fGrl->insertaIconos(icono, dir_theme, "img24/cat/", "cat/"         , filter);
}

void GrlTreeViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const
{
	QStyledItemDelegate::paint(painter, option, index);
	int column = index.column();

	if (column == col_Icono || column == col_Original ||
		column == col_Favorito || column == col_Rating ||
		column == col_Edad)
	{
		QModelIndex m_index = index.sibling(index.row(), col_IdGrl);
		int width, height, x, y;
		if (column == col_Icono)
		{
			QString ico = m_index.data(IconRole).toString();
			width  = icono[ico].width();
			height = icono[ico].height();
			QImage img_painter(option.rect.width(), option.rect.height(), QImage::Format_ARGB32);
			x = (img_painter.width() / 2) - (width / 2);
			y = (img_painter.height() / 2) - (height / 2);
			QPainter p(&img_painter);
			p.setCompositionMode(QPainter::CompositionMode_Source);
			p.fillRect(img_painter.rect(), Qt::transparent);
			p.drawPixmap(x, y, icono[ico]);
			p.end();

			painter->drawImage(option.rect.x(), option.rect.y(), img_painter);
		}
		if (column == col_Original)
		{
			QString original = m_index.data(OriginalRole).toString().toLower();
			width  = img_original.width();
			height = img_original.height();
			x      = option.rect.x() + (option.rect.width() / 2) - (width / 2);
			y      = option.rect.y() + (option.rect.height() / 2) - (height / 2);

			if (original == "true" || original == "yes" || original == "1")
				painter->drawPixmap(x, y, img_original);
			else
				painter->drawPixmap(x, y, sin_img);
		}
		if (column == col_Favorito)
		{
			bool favorito = m_index.data(FavoritoRole).toBool();
			width  = img_favorito.width();
			height = img_favorito.height();
			x      = option.rect.x() + (option.rect.width() / 2) - (width / 2);
			y      = option.rect.y() + (option.rect.height() / 2) - (height / 2);

			if (favorito)
				painter->drawPixmap(x, y, img_favorito);
			else
				painter->drawPixmap(x, y, sin_img);
		}
		if (column == col_Rating)
		{
			int rating = m_index.data(RatingRole).toInt();
			width  = star_on.width();
			height = star_on.height();
			QImage img_painter(option.rect.width(), option.rect.height(), QImage::Format_ARGB32);
			x = (img_painter.width() / 2) - (width*5 / 2);
			y = (img_painter.height() / 2) - (height / 2);
			QPainter p(&img_painter);
			p.setCompositionMode(QPainter::CompositionMode_Source);
			p.fillRect(img_painter.rect(), Qt::transparent);
			for (int i = 0; i < 5; ++i)
			{
				if (i < rating)
					p.drawPixmap(x, y, star_on);
				else
					p.drawPixmap(x, y, star_off);
				x += width;
			}
			p.end();

			painter->drawImage(option.rect.x(), option.rect.y(), img_painter);
		}
		if (column == col_Edad)
		{
			QString edad = "edad_"+ m_index.data(EdadRole).toString() +".png";
			width  = edades[edad].width();
			height = edades[edad].height();
			x      = option.rect.x() + (option.rect.width() / 2) - (width / 2);
			y      = option.rect.y() + (option.rect.height() / 2) - (height / 2);

			painter->drawPixmap(x, y, edades[edad]);
		}
	}
}

QSize GrlTreeViewDelegate::sizeHint(const QStyleOptionViewItem &option,
		const QModelIndex &index) const
{
	if (index.column() == col_Rating)
		return QSize(5 * star_on.width(), size_icon.height()) + QSize(1, 1);

	if (index.column() == col_Titulo)
		return size_icon + QSize(1, 1);

	return QStyledItemDelegate::sizeHint(option, index) + QSize(1, 1);
}

bool GrlTreeViewDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
		const QStyleOptionViewItem &option, const QModelIndex &index)
{
	int column = index.column();
	if (column == col_Original || column == col_Favorito || column == col_Rating)
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			QModelIndex m_index = index.sibling(index.row(), col_IdGrl);
			if (column == col_Original)
			{
				QString original = m_index.data(OriginalRole).toString().toLower();
				original = QVariant(original).toBool() ? "false" : "true";
				model->setData(m_index, original, OriginalRole);
				emit updateCol(m_index, col_Original);
			}
			if (column == col_Favorito)
			{
				QString fav = m_index.data(FavoritoRole).toString().toLower();
				fav = QVariant(fav).toBool() ? "false" : "true";
				model->setData(m_index, fav, FavoritoRole);
				emit updateCol(m_index, col_Favorito);
			}
			if (column == col_Rating)
			{
				QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
				int pos_x_rating = option.rect.x() + (option.rect.width() / 2) - (star_on.width()*5 / 2);
				int stars = qBound(0, int(0.5 + qreal(mouseEvent->pos().x() - pos_x_rating) / star_on.width()), 5);
				model->setData(m_index, stars, RatingRole);
				emit updateCol(m_index, col_Rating);
			}
			return false; //so that the selection can change
		}
		return true;
	} else
		return QStyledItemDelegate::editorEvent(event, model, option, index);
}
// FIN GrlTreeViewDelegate --------------------------------------------------------------------------------------
