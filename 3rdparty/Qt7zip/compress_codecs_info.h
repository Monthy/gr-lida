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

#ifndef COMPRESS_CODECS_INFO_H
#define COMPRESS_CODECS_INFO_H

#include "7zip_includes.h"

// Compress Codecs Info class
//---------------------------------------------------------------------------------------------------------------
#ifdef Q_OS_UNIX
class CCompressCodecsInfo :
	public ICompressCodecsInfo,
	public CMyUnknownImp
{
public:
	MY_UNKNOWN_IMP1(ICompressCodecsInfo)

	STDMETHOD(GetNumMethods)(UInt32 *numMethods);
	STDMETHOD(GetProperty)(UInt32 index, PROPID propID, PROPVARIANT *value);
	STDMETHOD(CreateDecoder)(UInt32 index, const GUID *interfaceID, void **coder);
	STDMETHOD(CreateEncoder)(UInt32 index, const GUID *interfaceID, void **coder);

	CCompressCodecsInfo(SevenZipInterface *szi) : szInterface(szi) {}

private:
	SevenZipInterface *szInterface;

};

STDMETHODIMP CCompressCodecsInfo::GetNumMethods(UInt32 *numMethods)
{
	return szInterface->getNumberOfMethodsFuncRar(numMethods);
}

STDMETHODIMP CCompressCodecsInfo::GetProperty(UInt32 index, PROPID propID, PROPVARIANT *value)
{
	return szInterface->getMethodPropertyFuncRar(index, propID, value);
}

STDMETHODIMP CCompressCodecsInfo::CreateDecoder(UInt32 index, const GUID *interfaceID, void **coder)
{
	NCOM::CPropVariant propVariant;
	szInterface->getMethodPropertyFuncRar(index, NMethodPropID::kDecoder, &propVariant);

	return szInterface->createObjectFuncRar(reinterpret_cast<const GUID *>(propVariant.bstrVal), interfaceID, coder);
}

STDMETHODIMP CCompressCodecsInfo::CreateEncoder(UInt32 index, const GUID *interfaceID, void **coder)
{
	return S_OK; // szInterface->createObjectFuncRar(&CLSID_CFormatRar, interfaceID, coder);
}
#endif

#endif // COMPRESS_CODECS_INFO_H
