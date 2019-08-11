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

#ifndef _7ZIP_INCLUDES_H
#define _7ZIP_INCLUDES_H

#include <QDebug>
#include <QString>
#include <QByteArray>

#include "StdAfx.h"

#include <stdio.h>

#include "CPP/Common/Defs.h"
#include "CPP/Common/MyInitGuid.h"
#include "CPP/Common/MyWindows.h"
#include "CPP/Common/IntToString.h"
#include "CPP/Common/StringConvert.h"

#include "CPP/Windows/DLL.h"
#include "CPP/Windows/FileDir.h"
#include "CPP/Windows/FileFind.h"
#include "CPP/Windows/FileName.h"
#include "CPP/Windows/NtCheck.h"
#include "CPP/Windows/PropVariant.h"
#include "CPP/Windows/PropVariantConv.h"

#include "CPP/7zip/Common/FileStreams.h"
#include "CPP/7zip/Common/StreamObjects.h"
#include "CPP/7zip/Archive/IArchive.h"
#include "CPP/7zip/IPassword.h"
#include "CPP/7zip/ICoder.h"

#include "C/Alloc.h"
#include "C/7zVersion.h"

#ifdef _WIN32
HINSTANCE g_hInstance = 0;
#endif

using namespace NWindows;
using namespace NFile;
using namespace NDir;

struct SevenZipInterface {
	Func_CreateObject createObjectFunc;

#ifdef Q_OS_UNIX
	Func_CreateObject createObjectFuncRar;
	Func_GetMethodProperty getMethodPropertyFuncRar;
	Func_GetNumberOfMethods getNumberOfMethodsFuncRar;
#endif

	CMyComPtr<IInArchive> archive;
	CMyComPtr<IOutArchive> outArchive;
};

static void PrintString(const QString &s)
{
	Q_UNUSED(s);
//	qDebug() << s;
}

static void PrintError(const QString &s, const QString &name = "")
{
//	Q_UNUSED(s);
//	Q_UNUSED(name);
	qDebug() << "Error: " << s;
	if (!name.isEmpty())
		qDebug() << name;
}

static void PrintNewLine()
{
//	qDebug() << "";
}

static HRESULT IsArchiveItemProp(IInArchive *archive, UInt32 index, PROPID propID, bool &result)
{
	NCOM::CPropVariant prop;
	RINOK(archive->GetProperty(index, propID, &prop));
	if (prop.vt == VT_BOOL)
		result = VARIANT_BOOLToBool(prop.boolVal);
	else if (prop.vt == VT_EMPTY)
		result = false;
	else
		return E_FAIL;
	return S_OK;
}

static HRESULT IsArchiveItemFolder(IInArchive *archive, UInt32 index, bool &result)
{
	return IsArchiveItemProp(archive, index, kpidIsDir, result);
}

static const wchar_t *kEmptyFileAlias = L"[Content]";

static UString toUString(QString str)
{
	return str.toStdWString().c_str();
}

static QString toQString(UString str)
{
	return QString::fromWCharArray(str);
}

#endif // _7ZIP_INCLUDES_H
