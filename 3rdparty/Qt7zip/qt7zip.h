/*
 *
 * Qt7zip
 *
 * This file is part of Qt7zip is a C++ wrapper for accessing the 7-zip API
 * Copyright (C) 2019 Pedro A. Garcia Rosado Aka Monthy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see http://www.gnu.org/licenses
 * or write to the Free Software Foundation,Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301  USA
 *
 *
**/

#ifndef QT7ZIP_H
#define QT7ZIP_H

#include <QObject>
#include <QLibrary>
#include <QStringList>
#include <QMap>

struct SevenZipInterface;

typedef struct {
	quint32 id;
	QString path;
	QString name;
	QString crc32;
	quint64 size;
	quint64 packsize;
	bool    isDir;
	bool    encrypted;
//	QByteArray data;
} szEntryInfo;

typedef struct {
	int index;
	QString fileOut;
	QString dirOut;
} szEntryExtract;

typedef struct {
	QString fileIn;
	QString fileOut;
} szEntryCompress;


enum CompressFormat {
	CFormat7z = 0,
	CFormatZip = 1,
	CFormatTar = 2,
};

class Qt7zip : public QObject
{
	Q_OBJECT
public:
	explicit Qt7zip(QObject *parent = 0);
	~Qt7zip();

	void closeArchive();
	bool loadLib(const QString &dir = "");
	bool isLoad7zLib() { return is_load_7zlib; }

	bool open(const QString &fileName, const QString &password = "");
	bool isOpen() { return is_open; }
	bool isEncrypted() { return is_encrypted; }

	void setPassword(const QString &password) { m_password = password; }

	int getNumEntries() { return m_entryTotal; }
	QString getComment() { return m_comment; }
	szEntryInfo getEntryInfo(const QString &fileName);

	QStringList entryList() { return m_entryList; }
	QList<szEntryInfo> entryListInfo() { return m_entryListInfo; }
//	QList<szEntryInfo> entryListInfoRaw() { return m_entryListInfoRaw; }

	int indexOf(const QRegExp &rx, int from = 0);
	int indexOfNoDir(const QRegExp &rx, int from = 0);

	bool extract(const QString &dirOut);
	bool extract(const QString &fileName, const QString &dirOut, const QString &fileOut = "");
	bool extract(const int indice, const QString &dirOut, const QString &fileOut = "");
	bool extract(QList<szEntryExtract> listEntry, const QString &dirOut);
	bool extract(QList<szEntryExtract> listEntry);

	bool extractRaw(const QString &fileName, QByteArray &data);
	bool extractRaw(const int indice, QByteArray &data);
	QList<QByteArray> extractRaw(const QList<int> indices);

	bool create(CompressFormat format, const QStringList &list_entry, const QString &fileName, const QString &password = "");
	bool create(CompressFormat format, const QList<szEntryCompress> &list_entry, const QString &fileName, const QString &password = "");

signals:
	void passwordRequired();

private:
	QLibrary *sevenzLib;
#ifdef Q_OS_UNIX
	QLibrary *rarLib;
#endif

	SevenZipInterface *szInterface;

	bool is_load_7zlib, is_open, is_encrypted;
	int m_entryTotal;
	QString m_password, m_comment;
	QStringList m_entryList;
	QList<szEntryInfo> m_entryListInfo;
//	QList<szEntryInfo> m_entryListInfoRaw;

	QMap<int, quint32> indexToArchive;
	QMap<QString, szEntryInfo> m_ArchiveList;

};

#endif // QT7ZIP_H
