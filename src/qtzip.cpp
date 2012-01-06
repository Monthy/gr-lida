/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2012 Pedro A. Garcia Rosado Aka Monthy
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
**/

#include "qtzip.h"

QtZip::QtZip()
{
	isZipOpen = false;
}

QtZip::~QtZip()
{
	uz.closeArchive();
	isZipOpen = false;
}

bool QtZip::abrirZip(const QString file, const QString pwd)
{
	if( isZipOpen )
		uz.closeArchive();

	if(!QFile::exists(file))
	{
		QMessageBox::information(0, "QtZip", QObject::tr("El archivo no existe."));
		isZipOpen = false;
		return false;
	}

	if(!pwd.isEmpty())
		uz.setPassword(pwd);

	ec = uz.openArchive(file);

	if(ec != UnZip::Ok)
	{
		QMessageBox::information(0, "QtZip", QObject::tr("Incapaz de abrir el archivo")+": "+ uz.formatError(ec).toAscii() );
		isZipOpen = false;
		return false;
	}

	QString comment = uz.archiveComment();
	if (!comment.isEmpty())
		comentarioZip = comment.toAscii().data();
	else
		comentarioZip = "";

	isZipOpen = true;
	return true;
}

QStringList QtZip::listaZip()
{
	QList<QString> listaZip;
	if( isZipOpen )
	{
		QList<UnZip::ZipEntry> list = uz.entryList();

		listaZip.clear();
		if( !list.isEmpty() )
		{
			int num_total = list.count();

			for (int i = 0; i < num_total; ++i)
			{
				const UnZip::ZipEntry& entry = list.at(i);

				if( entry.type != UnZip::Directory)
					listaZip << entry.filename;
			}
		}
	}
	return listaZip;
}

void QtZip::listaZipTreeWidget(QTreeWidget *myTreeWidget)
{
	if( isZipOpen )
	{
		QList<UnZip::ZipEntry> list = uz.entryList();

		myTreeWidget->clear();
		myTreeWidget->headerItem()->setText(0, "Filename");
		myTreeWidget->headerItem()->setText(1, "Size");
		myTreeWidget->headerItem()->setText(2, "Ratio");
		myTreeWidget->headerItem()->setText(3, "CRC32");
		myTreeWidget->headerItem()->setText(4, "Encrypted");
		myTreeWidget->headerItem()->setText(5, "path");

		if( !list.isEmpty() )
		{
			int num_total = list.count();

			for (int i = 0; i < num_total; ++i)
			{
				const UnZip::ZipEntry& entry = list.at(i);

				if( entry.type != UnZip::Directory)
				{
					QString file, crc, isEncrypted, path;
					double ratio = entry.uncompressedSize == 0 ? 0 : 100 - (double) entry.compressedSize * 100 / (double) entry.uncompressedSize;
					QString ratioS = QString::number(ratio, 'f', 2).append("%");
					crc = crc.sprintf("%X", entry.crc32).rightJustified(8, '0');

					file = entry.filename;
					path = entry.filename;

					int idx = file.lastIndexOf("/");
					if (idx >= 0 && idx != file.length()-1)
						file = file.right(file.length() - idx - 1);

					if(entry.encrypted)
						isEncrypted = "*";
					else
						isEncrypted = "";

					QTreeWidgetItem *item = new QTreeWidgetItem(myTreeWidget);
		//			item->setIcon(0, QIcon("") );
					item->setText(0, file.toAscii().data()        ); // Filename
					item->setText(1, QString::number(entry.uncompressedSize)  ); // Size
					item->setText(2, ratioS.toAscii().data()      ); // Ratio
					item->setText(3, crc.toAscii().data()         ); // CRC32
					item->setText(4, isEncrypted.toAscii().data() ); // Encrypted
					item->setText(5, path.toAscii().data()        ); // path
					item->setText(5, QString::number( i )         ); // id
				}
			}
		}
	}
}

void QtZip::listaZipListWidget(QListWidget *myListWidget)
{
	if( isZipOpen )
	{
		QList<UnZip::ZipEntry> list = uz.entryList();

		myListWidget->clear();
		if( !list.isEmpty() )
		{
			int num_total = list.count();

			for (int i = 0; i < num_total; ++i)
			{
				const UnZip::ZipEntry& entry = list.at(i);

				if( entry.type != UnZip::Directory)
				{
					QString file, path;
					file = entry.filename;
					path = entry.filename;

					int idx = file.lastIndexOf("/");
					if (idx >= 0 && idx != file.length()-1)
						file = file.right(file.length() - idx - 1);

					QListWidgetItem *item = new QListWidgetItem(myListWidget);
		//			item->setIcon(QIcon( m_scaled ));
					item->setText( file.toAscii().data() );
					item->setData(Qt::UserRole  , path.toAscii().data() );
					item->setData(Qt::UserRole+1, QVariant(i).toString());
					item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
				}
			}
		}
	}
}

QString QtZip::loadTexto(QString filename)
{
	if( isZipOpen )
	{
		if(!filename.isEmpty())
		{
			QBuffer cbuf;
			cbuf.open(QIODevice::WriteOnly);
			ec = uz.extractFile(filename, &cbuf);
			return cbuf.data();
		} else
			return "";
	} else
		return "";
}

QPixmap QtZip::loadImagen(QString filename)
{
	QPixmap pixmap;
	if( isZipOpen )
	{
		if(!filename.isEmpty())
		{
			QBuffer cbuf;
			cbuf.open(QIODevice::WriteOnly);
			ec = uz.extractFile(filename, &cbuf);
			pixmap.loadFromData( cbuf.data() );
		}
	}
	return pixmap;
}

QBitmap QtZip::loadImagenBitmap(QString filename)
{
	QBitmap bitmap;
	if( isZipOpen )
	{
		if(!filename.isEmpty())
		{
			QBuffer cbuf;
			cbuf.open(QIODevice::WriteOnly);
			ec = uz.extractFile(filename, &cbuf);
			bitmap.loadFromData( cbuf.data() );
		}
	}
	return bitmap;
}
