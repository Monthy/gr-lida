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

#include <QModelIndex>
#include <QMouseEvent>

#include "grlida_listview.h"

GrlListView::GrlListView(QWidget *parent):
	QListView(parent),
	m_lastRow(-1)
{
	// Habilitamos el seguimiento del ratón
	setMouseTracking(true);
}

void GrlListView::mouseMoveEvent(QMouseEvent *event)
{
	QAbstractItemModel *item_model(model());

	if (item_model)
	{
		QModelIndex index = indexAt(event->pos());
		if (index.isValid())
		{
			if (index.row() != m_lastRow)
			{
				m_lastRow = index.row();

				// Establecemos el cursor de selección
				setCursor(Qt::PointingHandCursor);
				//qDebug() << "cursor dentro";
			}
		} else {
			// Establecemos el cursor predeterminado
			if (m_lastRow != -1)
				setCursor(Qt::ArrowCursor);

			m_lastRow = -1;
			//qDebug() << "cursor fuera";
		}
	} else {

	}

	QListView::mouseMoveEvent(event);
}
