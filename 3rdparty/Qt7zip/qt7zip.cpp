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

#include "qt7zip.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QVariant>
#include <QCoreApplication>

#include "open_callback.h"
#include "extract_callback.h"
#include "extract_callback_raw.h"
#include "creating_callback.h"
#include "compress_codecs_info.h"

// Define GUID
// Tou can find the list of all GUIDs in Guid.txt file.
// use another CLSIDs, if you want to support other formats (zip, rar, ...).
// {23170F69-40C1-278A-1000-000110070000}

DEFINE_GUID(CLSID_CFormat7z  , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatZip , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0x01, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatRar , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0x03, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatRar5, 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0xcc, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatTar , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0xee, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatArj , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0x04, 0x00, 0x00);

// Unused Formats --
//DEFINE_GUID(CLSID_CFormatBZip2   , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0x02, 0x00, 0x00);
//DEFINE_GUID(CLSID_CFormatCab     , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0x08, 0x00, 0x00);
//DEFINE_GUID(CLSID_CFormatChm     , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0xe9, 0x00, 0x00);
//DEFINE_GUID(CLSID_CFormatCompound, 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0xe5, 0x00, 0x00);
//DEFINE_GUID(CLSID_CFormatCpio    , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0xed, 0x00, 0x00);
//DEFINE_GUID(CLSID_CFormatDeb     , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0xec, 0x00, 0x00);
//DEFINE_GUID(CLSID_CFormatGZip    , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0xef, 0x00, 0x00);
//DEFINE_GUID(CLSID_CFormatIso     , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0xe7, 0x00, 0x00);
//DEFINE_GUID(CLSID_CFormatLzh     , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0x06, 0x00, 0x00);
//DEFINE_GUID(CLSID_CFormatLzma    , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0x0a, 0x00, 0x00);
//DEFINE_GUID(CLSID_CFormatNsis    , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0x09, 0x00, 0x00);
//DEFINE_GUID(CLSID_CFormatRpm     , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0xeb, 0x00, 0x00);
//DEFINE_GUID(CLSID_CFormatSplit   , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0xea, 0x00, 0x00);
//DEFINE_GUID(CLSID_CFormatWim     , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0xe6, 0x00, 0x00);
//DEFINE_GUID(CLSID_CFormatZ       , 0x23170f69, 0x40c1, 0x278a, 0x10, 0x00, 0x00, 0x01, 0x10, 0x05, 0x00, 0x00);

//DEFINE_GUID(IID_InArchive          , 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x06, 0x00, 0x60, 0x00, 0x00);
//DEFINE_GUID(IID_IOutStream         , 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00);
//DEFINE_GUID(IID_IInStream          , 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00);
//DEFINE_GUID(IID_IStreamGetSize     , 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x03, 0x00, 0x06, 0x00, 0x00);
//DEFINE_GUID(IID_ISequentialInStream, 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00);
//DEFINE_GUID(IID_ISetCompressCodecsInfo, 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x04, 0x00, 0x61, 0x00, 0x00);
//DEFINE_GUID(IID_IOutArchive        , 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, 0x06, 0x00, 0xA0, 0x00, 0x00);

enum CLSID_Format {
	CLSID_Format7z   = 0,
	CLSID_FormatZip  = 1,
	CLSID_FormatRar  = 2,
	CLSID_FormatRar5 = 3,
	CLSID_FormatTar  = 4,
	CLSID_FormatArj  = 5
};

GUID _supportedFileFormats[] = {CLSID_CFormat7z, CLSID_CFormatZip, CLSID_CFormatRar, CLSID_CFormatRar5, CLSID_CFormatTar,  CLSID_CFormatArj};
std::vector<GUID> supportedFileFormats(_supportedFileFormats, _supportedFileFormats + sizeof(_supportedFileFormats) / sizeof(_supportedFileFormats[0]));

const unsigned char sevenz[6] = {static_cast<unsigned char>(0x37), static_cast<unsigned char>(0x7A), static_cast<unsigned char>(0xBC), static_cast<unsigned char>(0xAF), static_cast<unsigned char>(0x27), static_cast<unsigned char>(0x1C)};
const unsigned char zip[2]    = {static_cast<unsigned char>(0x50), static_cast<unsigned char>(0x4B)};
const unsigned char rar[7]    = {static_cast<unsigned char>(0x52), static_cast<unsigned char>(0x61), static_cast<unsigned char>(0x72), static_cast<unsigned char>(0x21), static_cast<unsigned char>(0x1A), static_cast<unsigned char>(0x07), static_cast<unsigned char>(0x00)};
const unsigned char rar5[8]   = {static_cast<unsigned char>(0x52), static_cast<unsigned char>(0x61), static_cast<unsigned char>(0x72), static_cast<unsigned char>(0x21), static_cast<unsigned char>(0x1A), static_cast<unsigned char>(0x07), static_cast<unsigned char>(0x01), static_cast<unsigned char>(0x00)};
const unsigned char tar[6]    = "ustar";
const unsigned char arj[2]    = {static_cast<unsigned char>(0x60), static_cast<unsigned char>(0xEA)};


static GUID getFileType(const QString &fileName)
{
	QFile filex(fileName);
	if (filex.open(QIODevice::ReadOnly))
	{
		QByteArray magicNumber = filex.read(8); // read first 8 bytes
	//	0 = CLSID_CFormat7z
	//	1 = CLSID_CFormatZip
	//	2 = CLSID_CFormatRar
	//	3 = CLSID_CFormatRar5
	//	4 = CLSID_CFormatTar
	//	5 = CLSID_CFormatArj
	// TODO: this suffix matching is rather primitive - better approach?
		if (memcmp(magicNumber, rar, 6) == 0)
		{
			if (memcmp(magicNumber, rar5, 7) == 0)
				return CLSID_CFormatRar5;
			else
				return CLSID_CFormatRar;
		} else if (memcmp(magicNumber, zip, 2) == 0)
			return CLSID_CFormatZip;
		else if (memcmp(magicNumber, sevenz, 6) == 0)
			return CLSID_CFormat7z;
		else if (memcmp(magicNumber, arj, 2) == 0)
			return CLSID_CFormatArj;
		else {
			filex.seek(257);
			magicNumber = filex.read(8);
			if (memcmp(magicNumber, tar, 5) == 0)
				return CLSID_CFormatTar;
		}
	}

	return CLSID_CFormat7z;
}

static QVariant getArchiveProperty(CMyComPtr<IInArchive> archive, PROPID propID)
{
	QVariant value_out;
	NCOM::CPropVariant value;
	HRESULT res = archive->GetArchiveProperty(propID, &value);

	if (res == 0)
	{
		switch (propID)
		{
			case kpidComment:
			{
				UString comment;
				if (value.vt == VT_EMPTY)
					comment = kEmptyFileAlias;
				else {
					if (value.vt != VT_BSTR)
						comment = kEmptyFileAlias;
					comment = value.bstrVal;
				}

				value_out = QString::fromWCharArray(comment);
			}
			break;
		}
	}

	return value_out;
}

static QVariant getProperty(CMyComPtr<IInArchive> archive, quint32 index, PROPID propID)
{
	QVariant value_out;
	NCOM::CPropVariant value;
	HRESULT res = archive->GetProperty(index, propID, &value);

	switch (propID)
	{
		case kpidPath:
		{
			UString fullPath;
			if (value.vt == VT_EMPTY)
				fullPath = kEmptyFileAlias;
			else {
				if (value.vt == VT_BSTR)
					fullPath = value.bstrVal;
				else
					fullPath = kEmptyFileAlias;
			}

			value_out = QString::fromWCharArray(fullPath);
		}
		break;
		case kpidCRC:
		{
			char crc[12];
			ConvertPropVariantToShortString(value, crc);
			QString crc32 = "0";
			crc32.sprintf("%X", QString(crc).toUInt());

			value_out = crc32.rightJustified(8, '0');
		}
		break;
		case kpidSize:
		case kpidPackSize:
		{
			quint64 size = 0;
			if (value.vt != VT_EMPTY)
				ConvertPropVariantToUInt64(value, size);

			value_out = size;
		}
		break;
		case kpidIsDir:
		{
			bool isDir = false;
			if (value.vt == VT_BOOL)
				isDir = VARIANT_BOOLToBool(value.boolVal);
			else if (value.vt == VT_EMPTY)
				isDir = false;

			value_out = isDir;
		}
		break;
		case kpidEncrypted:
		{
			bool isEncrypted = false;
			if (res == 0 && value.vt == VT_BOOL)
				isEncrypted = value.bVal;

			value_out = isEncrypted;
		}
		break;
	}

	return value_out;
}


Qt7zip::Qt7zip(QObject *parent) : QObject(parent),
	sevenzLib(0), is_load_7zlib(false), is_open(false)
{
	szInterface = new SevenZipInterface;
	is_load_7zlib = loadLib();
}

Qt7zip::~Qt7zip()
{
	closeArchive();

	delete szInterface;
#ifdef Q_OS_UNIX
	delete rarLib;
#endif
	delete sevenzLib;
}

void Qt7zip::closeArchive()
{
// always close the archive!
	if (szInterface->archive)
	{
		szInterface->archive->Close();

#ifdef Q_OS_UNIX
		szInterface->archive->AddRef();
#endif
	}
}


bool Qt7zip::loadLib(const QString &dir)
{
// LOAD library
// TODO check if this works in OSX (7z.so instead of 7z.dylib)
// fix1: try to load "7z.so"
// fix2: rename 7z.so to 7z.dylib

	is_load_7zlib = true;
	QString path_lib     = "";
	QString path_lib_7z  = "";
	QString path_lib_rar = "";

#if defined Q_OS_UNIX
	#if defined Q_OS_MAC
		if (!dir.isEmpty())
			path_lib = dir;
		else
			path_lib = QCoreApplication::applicationDirPath();

		path_lib_7z  = path_lib +"/p7zip/7z"; // .so, .dylib
		path_lib_rar = path_lib +"/p7zip/Codecs/Rar"; // .so, .dylib
	#else
		if (!dir.isEmpty())
			path_lib = dir;
		else
			path_lib = QString(LIBDIR);

		path_lib_7z  = path_lib +"/p7zip/7z.so";
		path_lib_rar = path_lib +"/p7zip/Codecs/Rar.so";
	#endif
#else
	if (!dir.isEmpty())
		path_lib = dir;
	else
		path_lib = QCoreApplication::applicationDirPath();

	path_lib_7z = path_lib +"/7z"; // .dll
#endif

	if (sevenzLib == 0)
		sevenzLib = new QLibrary(path_lib_7z);
	else {
		if (sevenzLib->isLoaded())
			sevenzLib->unload();
		sevenzLib->setFileName(path_lib_7z);
	}

#if defined Q_OS_UNIX
	if (rarLib == 0)
		rarLib = new QLibrary(path_lib_rar);
	else {
		if (rarLib->isLoaded())
			rarLib->unload();
		rarLib->setFileName(path_lib_rar);
	}
#endif

	if (!sevenzLib->load())
	{
		PrintError("Can not load 7-zip library", sevenzLib->errorString());
		is_load_7zlib = false;
	} else {
//		szInterface->createObjectFunc = (Func_CreateObject)sevenzLib->resolve("CreateObject");
		szInterface->createObjectFunc = reinterpret_cast<Func_CreateObject>(sevenzLib->resolve("CreateObject"));
		if (!szInterface->createObjectFunc)
		{
			PrintError("Can not get CreateObject");
			is_load_7zlib = false;
		}

	#ifdef Q_OS_UNIX
		if (rarLib->load())
		{
			szInterface->createObjectFuncRar = reinterpret_cast<Func_CreateObject>(rarLib->resolve("CreateObject"));
			if (!szInterface->createObjectFuncRar)
				PrintError("Can not get CreateObject (rar)");

			szInterface->getMethodPropertyFuncRar = reinterpret_cast<Func_GetMethodProperty>(rarLib->resolve("GetMethodProperty"));
			if (!szInterface->getMethodPropertyFuncRar)
				PrintError("Can not get GetMethodProperty (rar)");

			szInterface->getNumberOfMethodsFuncRar = reinterpret_cast<Func_GetNumberOfMethods>(rarLib->resolve("GetNumberOfMethods"));
			if (!szInterface->getNumberOfMethodsFuncRar)
				PrintError("Can not get GetNumberOfMethods (rar)");
		}
	#endif
	}

	return is_load_7zlib;
}


bool Qt7zip::open(const QString &fileName, const QString &password)
{
	if (is_open)
		closeArchive();

	if (!is_load_7zlib)
	{
		is_open = false;
		return false;
	}

	UString u_fileName = toUString(fileName);
	UString u_password = toUString(password);
	m_password = password;

// Comprobamos la extension....
	GUID guid_format = getFileType(fileName);

//	if (szInterface->createObjectFunc(&guid_format, &IID_IInArchive, (void **)&szInterface->archive) != S_OK)
	if (szInterface->createObjectFunc(&guid_format, &IID_IInArchive, reinterpret_cast<void **>(&szInterface->archive)) != S_OK)
	{
		PrintError("Can not get class object");
		is_open = false;
		return false;
	}

	CInFileStream *fileSpec = new CInFileStream;
	CMyComPtr<IInStream> file = fileSpec;

	if (!fileSpec->Open(u_fileName))
	{
		PrintError("Can not open archive file", fileName);
		is_open = false;
		return false;
	}

	{
		CArchiveOpenCallback *openCallbackSpec = new CArchiveOpenCallback;
		CMyComPtr<IArchiveOpenCallback> openCallback(openCallbackSpec);
		openCallbackSpec->PasswordIsDefined = false;

		if (!password.isEmpty())
		{
			openCallbackSpec->PasswordIsDefined = true;
			openCallbackSpec->Password = u_password;
		}

	//	if (guid_format == CLSID_CFormatRar || guid_format == CLSID_CFormatRar5)
	//	{
		#ifdef Q_OS_UNIX
			CCompressCodecsInfo *compressCodecsInfo = new CCompressCodecsInfo(szInterface);

			CMyComPtr<ISetCompressCodecsInfo> setCompressCodecsInfo;
//			szInterface->archive->QueryInterface(IID_ISetCompressCodecsInfo, (void **)&setCompressCodecsInfo);
			szInterface->archive->QueryInterface(IID_ISetCompressCodecsInfo, reinterpret_cast<void **>(&setCompressCodecsInfo));
			if (setCompressCodecsInfo)
			{
				setCompressCodecsInfo->SetCompressCodecsInfo(compressCodecsInfo);
			}
		#endif
	//	}

		const UInt64 scanSize = 1 << 23;
		if (szInterface->archive->Open(file, &scanSize, openCallback) != S_OK)
		{
			PrintError("Can not open file as archive", fileName);
			is_open = false;
			return false;
		}
	}

	szEntryInfo entryInfo;

	is_open      = true;
	is_encrypted = false;
	m_comment    = getArchiveProperty(szInterface->archive, kpidComment).toString();

	m_ArchiveList.clear();

	quint32 entryTotal = 0;
	szInterface->archive->GetNumberOfItems(&entryTotal);
	for (quint32 i = 0; i < entryTotal; i++)
	{
	// Get info of file
		entryInfo.id        = i;
		entryInfo.path      = getProperty(szInterface->archive, i, kpidPath).toString().replace("\\","/");
		entryInfo.name      = QFileInfo(entryInfo.path).fileName();
		entryInfo.crc32     = getProperty(szInterface->archive, i, kpidCRC).toString();
		entryInfo.size      = getProperty(szInterface->archive, i, kpidSize).toUInt();
		entryInfo.packsize  = getProperty(szInterface->archive, i, kpidPackSize).toUInt();
		entryInfo.isDir     = getProperty(szInterface->archive, i, kpidIsDir).toBool();
		entryInfo.encrypted = getProperty(szInterface->archive, i, kpidEncrypted).toBool();
	//	entryInfo.data      = "";

		if (entryInfo.encrypted && !is_encrypted)
			is_encrypted = true;

		m_ArchiveList.insert(entryInfo.path, entryInfo);
	}

	m_entryList.clear();
	m_entryListInfo.clear();
	indexToArchive.clear();

	m_entryTotal = 0;
	foreach (szEntryInfo zinfo, m_ArchiveList)
	{
		m_entryList.insert(m_entryTotal, zinfo.path);
		m_entryListInfo.insert(m_entryTotal, zinfo);
		indexToArchive.insert(m_entryTotal, zinfo.id);

		m_entryTotal++;
	}

	if (is_encrypted && m_password.isEmpty())
		emit passwordRequired();

	return true;
}

int Qt7zip::indexOf(const QRegExp &rx, int from)
{
	return m_entryList.indexOf(rx, from);
}

szEntryInfo Qt7zip::getEntryInfo(const QString &fileName)
{
	szEntryInfo entryInfo;
	entryInfo.id        = 0;
	entryInfo.path      = "";
	entryInfo.name      = "";
	entryInfo.crc32     = "";
	entryInfo.size      = 0;
	entryInfo.packsize  = 0;
	entryInfo.isDir     = false;
	entryInfo.encrypted = false;
//	entryInfo.data      = "";

	if (is_open && !m_ArchiveList.isEmpty())
	{
		entryInfo = m_ArchiveList.value(fileName, entryInfo);
	}

	return entryInfo;
}

bool Qt7zip::extract(const QString &dirOut)
{
	if (!is_open)
		return false;

	UString m_dirOut = toUString(QDir::toNativeSeparators(dirOut));

	CArchiveExtractCallback *extractCallbackSpec = new CArchiveExtractCallback;
	CMyComPtr<IArchiveExtractCallback> extractCallback(extractCallbackSpec);
	extractCallbackSpec->Init(szInterface->archive, m_dirOut);

	extractCallbackSpec->PasswordIsDefined = false;
	if (!m_password.isEmpty())
	{
		extractCallbackSpec->PasswordIsDefined = true;
		extractCallbackSpec->Password = toUString(m_password);
	}

//	HRESULT result = szInterface->archive->Extract(NULL, (UInt32)(Int32)(-1), false, extractCallback);
	HRESULT result = szInterface->archive->Extract(NULL, static_cast<UInt32>(-1), false, extractCallback);

	if (result != S_OK)
	{
		PrintError("Extract Error");
		return false;
	}

	return true;
}

bool Qt7zip::extract(const QString &fileName, const QString &dirOut, const QString &fileOut)
{
	int index = m_entryList.indexOf(fileName);
	return extract(index, dirOut, fileOut);
}

bool Qt7zip::extract(const int indice, const QString &dirOut, const QString &fileOut)
{
	if (!is_open)
		return false;

	UString m_dirOut  = toUString(QDir::toNativeSeparators(dirOut));
	UString m_fileOut = toUString(QDir::toNativeSeparators(fileOut));

	if ((indice > -1) && (indice < m_entryTotal))
	{
	// Extract command
		CArchiveExtractCallback *extractCallbackSpec = new CArchiveExtractCallback;
		CMyComPtr<IArchiveExtractCallback> extractCallback(extractCallbackSpec);
		extractCallbackSpec->Init(szInterface->archive, m_dirOut, m_fileOut);

		extractCallbackSpec->PasswordIsDefined = false;
		if (!m_password.isEmpty())
		{
			extractCallbackSpec->PasswordIsDefined = true;
			extractCallbackSpec->Password = toUString(m_password);
		}

		quint32 index = indexToArchive[indice];
		HRESULT result = szInterface->archive->Extract(&index, 1, false, extractCallback);

		if (result != S_OK)
		{
			PrintError("Extract Error");
			return false;
		}

		return true;
	}

	return false;
}

bool Qt7zip::extract(QList<szEntryExtract> listEntry, const QString &dirOut)
{
	if (!is_open)
		return false;

	bool result = true;

	const int listSize = listEntry.size();
	for (int i = 0; i < listSize; ++i)
	{
		bool isOk = extract(listEntry.at(i).index, dirOut, listEntry.at(i).fileOut);
		if (!isOk)
			result = false;
	}

	return result;
}

bool Qt7zip::extract(QList<szEntryExtract> listEntry)
{
	if (!is_open)
		return false;

	bool result = true;

	const int listSize = listEntry.size();
	for (int i = 0; i < listSize; ++i)
	{
		bool isOk = extract(listEntry.at(i).index, listEntry.at(i).dirOut, listEntry.at(i).fileOut);
		if (!isOk)
			result = false;
	}

	return result;
}

bool Qt7zip::extractRaw(const QString &fileName, QByteArray &data)
{
	int index = m_entryList.indexOf(fileName);
	return extractRaw(index, data);
}

bool Qt7zip::extractRaw(const int indice, QByteArray &data)
{
	data = QByteArray();

	if (!is_open)
		return false;

	if ((indice > -1) && (indice < m_entryTotal))
	{
	// Extract command
		CArchiveExtractCallbackRaw *extractCallbackSpecRaw = new CArchiveExtractCallbackRaw();
		CMyComPtr<IArchiveExtractCallback> extractCallbackRaw(extractCallbackSpecRaw);
		extractCallbackSpecRaw->Init(szInterface->archive);

		extractCallbackSpecRaw->PasswordIsDefined = false;
		if (!m_password.isEmpty())
		{
			extractCallbackSpecRaw->PasswordIsDefined = true;
			extractCallbackSpecRaw->Password = toUString(m_password);
		}

		quint32 index = indexToArchive[indice];
		HRESULT result = szInterface->archive->Extract(&index, 1, false, extractCallbackRaw);

		if (result != S_OK)
		{
			PrintError("Extract Error");
			return false;
		}

//		data = QByteArray((char *)extractCallbackSpecRaw->data, extractCallbackSpecRaw->newFileSize);
		data = QByteArray(reinterpret_cast<char *>(extractCallbackSpecRaw->data), static_cast<int>(extractCallbackSpecRaw->newFileSize));
		return true;
	}

	return false;
}

QList<QByteArray> Qt7zip::extractRaw(const QList<int> indices)
{
	QList<QByteArray> listRaw;

	if (!is_open)
		return listRaw;

	QMap<quint32, quint32> ordenIndexes;
	const int countIndices = indices.size();
	for (int i = 0; i < countIndices; ++i)
	{
		quint32 valor = indexToArchive[indices.at(i)];
		ordenIndexes.insert(valor, valor);
	}

	QVector<quint32> indexes;
	foreach (quint32 index, ordenIndexes)
	{
		indexes.append(index);
	}

// Extract command
	CArchiveExtractCallbackRaw *extractCallbackSpecRaw = new CArchiveExtractCallbackRaw(true, indexes);
	CMyComPtr<IArchiveExtractCallback> extractCallbackRaw(extractCallbackSpecRaw);
	extractCallbackSpecRaw->Init(szInterface->archive);

	extractCallbackSpecRaw->PasswordIsDefined = false;
	if (!m_password.isEmpty())
	{
		extractCallbackSpecRaw->PasswordIsDefined = true;
		extractCallbackSpecRaw->Password = toUString(m_password);
	}

	HRESULT result;
	if (indexes.isEmpty())
		result = szInterface->archive->Extract(NULL, static_cast<UInt32>(-1), false, extractCallbackRaw);
	else
		result = szInterface->archive->Extract(indexes.data(), static_cast<UInt32>(indexes.count()), false, extractCallbackRaw);

//	m_entryListInfoRaw.clear();
//	const int countFileRawInfo = extractCallbackSpecRaw->allFileInfoRaw.size();
//	for (int i = 0; i < countFileRawInfo; ++i)
//	{
//		szEntryInfo entryInfo = getEntryInfo(extractCallbackSpecRaw->allFileInfoRaw.at(i).path);
//		entryInfo.data = extractCallbackSpecRaw->allFileInfoRaw.at(i).data;
//
//		m_entryListInfoRaw << entryInfo;
//	}

	return extractCallbackSpecRaw->allFilesRaw;
}

bool Qt7zip::create(CompressFormat format, const QStringList &list_entry, const QString &fileName, const QString &password)
{
	szEntryCompress entry;
	QList<szEntryCompress> list_entry_compress;
	int i;
	const int total_list_entry = list_entry.size();
	for (i = 0; i < total_list_entry; ++i)
	{
		QString name  = QFileInfo(list_entry.at(i)).fileName();

		entry.fileIn  = list_entry.at(i);
		entry.fileOut = name;

		list_entry_compress << entry;
	}

	return create(format, list_entry_compress, fileName, password);
}

bool Qt7zip::create(CompressFormat format, const QList<szEntryCompress> &list_entry, const QString &fileName, const QString &password)
{
	GUID guid_format = CLSID_CFormat7z;
	QString ext = "7z";
	if (format == CFormatZip)
	{
		guid_format = CLSID_CFormatZip;
		ext = "zip";
	} else if (format == CFormatTar) {
		guid_format = CLSID_CFormatTar;
		ext = "tar";
	}

	QFileInfo f_info(fileName);
	QString f_ext = f_info.suffix().toLower();

	UString u_fileName = toUString(fileName);
	if (ext != f_ext)
		u_fileName = toUString(fileName +"."+ ext);

	UString u_password = toUString(password);

	if (list_entry.isEmpty())
		return false;

	CObjectVector<CDirItem> dirItems;

	{
		szEntryCompress entry;
		int i;
		const int total_list_entry = list_entry.size();
		for (i = 0; i < total_list_entry; ++i)
		{
			CDirItem di;

			UString name     = toUString(list_entry.at(i).fileOut);
			UString fullPath = toUString(list_entry.at(i).fileIn);

			NFind::CFileInfo fi;
			if (!fi.Find(fullPath))
			{
				PrintError("Can't find file", list_entry.at(i).fileIn);
				return false;
			}

			di.Attrib   = fi.Attrib;
			di.Size     = fi.Size;
			di.CTime    = fi.CTime;
			di.ATime    = fi.ATime;
			di.MTime    = fi.MTime;
			di.Name     = name;
			di.FullPath = fullPath;
			dirItems.Add(di);
		}

		COutFileStream *outFileStreamSpec = new COutFileStream;
		CMyComPtr<IOutStream> outFileStream = outFileStreamSpec;
		if (!outFileStreamSpec->Create(u_fileName, false))
		{
			PrintError("can't create archive file");
			return false;
		}

//		if (szInterface->createObjectFunc(&guid_format, &IID_IOutArchive, (void **)&szInterface->outArchive) != S_OK)
		if (szInterface->createObjectFunc(&guid_format, &IID_IOutArchive, reinterpret_cast<void **>(&szInterface->outArchive)) != S_OK)
		{
			PrintError("Can not get class object");
			return false;
		}

		CArchiveUpdateCallback *updateCallbackSpec = new CArchiveUpdateCallback;
		CMyComPtr<IArchiveUpdateCallback2> updateCallback(updateCallbackSpec);
		updateCallbackSpec->Init(&dirItems);

		if (!password.isEmpty())
		{
			updateCallbackSpec->PasswordIsDefined = true;
			updateCallbackSpec->Password = u_password;
		}

/*		{
			const wchar_t *names[] =
			{
				L"s",
				L"x"
			};
			const unsigned kNumProps = ARRAY_SIZE(names);
			NCOM::CPropVariant values[kNumProps] =
			{
				false,    // solid mode OFF
				(UInt32)9 // compression level = 9 - ultra
			};
			CMyComPtr<ISetProperties> setProperties;
			szInterface->outArchive->QueryInterface(IID_ISetProperties, (void **)&setProperties);
			if (!setProperties)
			{
				PrintError("ISetProperties unsupported");
				return false;
			}
			RINOK(setProperties->SetProperties(names, values, kNumProps));
		}*/

		HRESULT result = szInterface->outArchive->UpdateItems(outFileStream, dirItems.Size(), updateCallback);
		updateCallbackSpec->Finilize();
		if (result != S_OK)
		{
			PrintError("Update Error");
			return false;
		}

		FOR_VECTOR (i, updateCallbackSpec->FailedFiles)
		{
			PrintError("Error for file", toQString(updateCallbackSpec->FailedFiles[i]));
		}

		if (updateCallbackSpec->FailedFiles.Size() != 0)
			return false;
	}

	return true;
}
