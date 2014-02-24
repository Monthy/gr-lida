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

#include "qtzip.h"

QtZip::QtZip()
{
	isZipOpen = false;
	count_uz  = -1;
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

	isZipOpen = false;
	count_uz  = -1;

	if( !QFile::exists(file) )
	{
		QMessageBox::information(0, "QtZip", tr("El archivo no existe."));
		isZipOpen = false;
		return false;
	}

	ec_uz = UnZip::Ok;

	if( !pwd.isEmpty() )
		uz.setPassword(pwd);

	ec_uz = uz.openArchive(file);
	if( ec_uz != UnZip::Ok )
	{
		QMessageBox::information(0, "QtZip", tr("Incapaz de abrir el archivo") +": "+ uz.formatError(ec_uz).toLatin1() );
		isZipOpen = false;
		return false;
	}

	ec_uz = uz.verifyArchive();
	switch (ec_uz)
	{
		case UnZip::WrongPassword:
		{
			bool ok = false;
			QString text_pass = QInputDialog::getText(0, tr("Contraseña incorrecta"), tr("Contraseña") +":", QLineEdit::PasswordEchoOnEdit, "", &ok);
			if( ok && !text_pass.isEmpty() )
			{
				abrirZip(file, text_pass);
				return false;
			}
		//	QMessageBox::information(0, "QtZip", tr("Wrong password.") +": "+ uz.formatError(ec_uz).toLatin1() );
			isZipOpen = false;
			return false;
		}
		break;
		case UnZip::PartiallyCorrupted:
			QMessageBox::information(0, "QtZip", tr("Encontrados algunos archivos dañados.") +": "+ uz.formatError(ec_uz).toLatin1() );
			isZipOpen = false;
			return false;
		break;
		case UnZip::Corrupted:
			QMessageBox::information(0, "QtZip", tr("Archivos dañados.") +": "+ uz.formatError(ec_uz).toLatin1() );
			isZipOpen = false;
			return false;
		break;
		case UnZip::Ok:
			// Sin problemas.
		break;
		default:
		{
			bool ok = false;
			QString text_pass = QInputDialog::getText(0, tr("Incapaz de abrir el archivo"), tr("Puede que necesite una contraseña") +"\n"+ tr("Contraseña") +":", QLineEdit::PasswordEchoOnEdit, "", &ok);
			if( ok && !text_pass.isEmpty() )
			{
				abrirZip(file, text_pass);
				return false;
			}
			isZipOpen = false;
			return false;
		}
		break;
	}

	isZipOpen = true;

	comentarioZip = uz.archiveComment();

	hash_uz.clear();
	QList<UnZip::ZipEntry> list_uz = uz.entryList();
	if( !list_uz.isEmpty() )
	{
		stQtZip z_info;
		count_uz = list_uz.size();
		for (int i = 0; i < count_uz; ++i)
		{
			const UnZip::ZipEntry& entry_uz = list_uz.at(i);
			if( entry_uz.type == UnZip::Directory )
			{
				z_info.filename  = "";
				z_info.size      = "";
				z_info.ratio     = "";
				z_info.crc32     = "";
				z_info.encrypted = "";
				z_info.path      = entry_uz.filename;
				z_info.isDir     = true;
			} else {
				double ratio   = entry_uz.uncompressedSize == 0 ? 0 : 100 - (double)entry_uz.compressedSize * 100 / (double)entry_uz.uncompressedSize;
				QString ratioS = QString::number(ratio, 'f', 2).append("%");
				QString crc32  = "00000000";
				crc32 = crc32.sprintf("%X", entry_uz.crc32).rightJustified(8, '0');

				QString filename = entry_uz.filename;
				int idx = filename.lastIndexOf("/");
			//	if( idx >= 0 && idx != filename.length()-1 )
			//		filename = filename.right(filename.length() - idx - 1);
				if( idx > -1 )
					filename = filename.left(idx);

				z_info.filename  = filename;
				z_info.size      = QString::number(entry_uz.uncompressedSize);
				z_info.ratio     = ratioS;
				z_info.crc32     = crc32;
				z_info.encrypted = (entry_uz.encrypted) ? "*" : "";
				z_info.path      = entry_uz.filename;
				z_info.isDir     = false;
			}
			hash_uz.insert(i, z_info);
		}
	}

	return true;
}

bool QtZip::extractZip(const QString file, const QString out, const QString pwd)
{
	abrirZip(file, pwd);

	if( isZipOpen )
	{
		ec_uz = uz.extractAll(out);
		if( ec_uz != UnZip::Ok )
		{
			QMessageBox::information(0, "QtZip", tr("Incapaz de extraer el archivo") +": "+ uz.formatError(ec_uz).toLatin1() );
			uz.closeArchive();
			return false;
		}
		return true;
	}
	return true;
}

QStringList QtZip::listaZip(bool show_dir)
{
	QList<QString> listaZip;
	for(int i = 0; i < count_uz; ++i)
	{
		if( !hash_uz[i].isDir || show_dir )
			listaZip << hash_uz[i].path;
	}
	return listaZip;
}

void QtZip::listaZipTreeWidget(QTreeWidget *myTreeWidget, bool show_dir)
{
	if( isZipOpen && !hash_uz.isEmpty() )
	{
		myTreeWidget->clear();
		myTreeWidget->headerItem()->setText(0, "Filename");
		myTreeWidget->headerItem()->setText(1, "Size");
		myTreeWidget->headerItem()->setText(2, "Ratio");
		myTreeWidget->headerItem()->setText(3, "CRC32");
		myTreeWidget->headerItem()->setText(4, "Encrypted");
		myTreeWidget->headerItem()->setText(5, "path");

		for(int i = 0; i < count_uz; ++i)
		{
			if( !hash_uz[i].isDir || show_dir )
			{
				QTreeWidgetItem *item = new QTreeWidgetItem(myTreeWidget);
	//			item->setIcon(0, QIcon("") );
				item->setText(0, hash_uz[i].filename  );
				item->setText(1, hash_uz[i].size      );
				item->setText(2, hash_uz[i].ratio     );
				item->setText(3, hash_uz[i].crc32     );
				item->setText(4, hash_uz[i].encrypted );
				item->setText(5, hash_uz[i].path      );
				item->setText(6, QString::number(i)   );
			}
		}
	}
}

void QtZip::listaZipListWidget(QListWidget *myListWidget, bool show_dir)
{
	if( isZipOpen && !hash_uz.isEmpty() )
	{
		myListWidget->clear();

		for(int i = 0; i < count_uz; ++i)
		{
			if( !hash_uz[i].isDir || show_dir )
			{
				QListWidgetItem *item = new QListWidgetItem(myListWidget);
	//			item->setIcon(QIcon( m_scaled ));
				item->setText( hash_uz[i].filename );
				item->setData(Qt::UserRole  , hash_uz[i].path    );
				item->setData(Qt::UserRole+1, QString::number(i) );
				item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
			}
		}
	}
}

const QByteArray QtZip::loadData(QString filename)
{
	if( !filename.isEmpty() && isZipOpen )
	{
		QBuffer cbuf;
		cbuf.open(QIODevice::WriteOnly);
		ec_uz = uz.extractFile(filename, &cbuf);
		return cbuf.data();
	} else
		return QByteArray("");
}

QString QtZip::loadTexto(QString filename)
{
	return QString( loadData(filename) );
}

QPixmap QtZip::loadImagen(QString filename)
{
	QPixmap pixmap;
	pixmap.loadFromData( loadData(filename) );
	return pixmap;
}

QBitmap QtZip::loadImagenBitmap(QString filename)
{
	return QBitmap( loadImagen(filename) );
}
