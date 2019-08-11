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

#ifndef EXTRACT_CALLBACK_H
#define EXTRACT_CALLBACK_H

#include "7zip_includes.h"

// Archive Extracting callback class
//---------------------------------------------------------------------------------------------------------------
class CArchiveExtractCallback:
	public IArchiveExtractCallback,
	public ICryptoGetTextPassword,
	public CMyUnknownImp
{
public:
	MY_UNKNOWN_IMP1(ICryptoGetTextPassword)

// IProgress
	STDMETHOD(SetTotal)(UInt64 size);
	STDMETHOD(SetCompleted)(const UInt64 *completeValue);

// IArchiveExtractCallback
	STDMETHOD(GetStream)(UInt32 index, ISequentialOutStream **outStream, Int32 askExtractMode);
	STDMETHOD(PrepareOperation)(Int32 askExtractMode);
	STDMETHOD(SetOperationResult)(Int32 resultEOperationResult);

// ICryptoGetTextPassword
	STDMETHOD(CryptoGetTextPassword)(BSTR *aPassword);

private:
	CMyComPtr<IInArchive> _archiveHandler;
	FString _directoryPath; // Output directory
	UString _filePath;      // name inside arcvhive
	FString _diskFilePath;  // full path to file on disk
	UString _fileName;
	bool _extractMode;

	struct CProcessedFileInfo
	{
		FILETIME MTime;
		UInt32 Attrib;
		bool isDir;
		bool AttribDefined;
		bool MTimeDefined;
	} _processedFileInfo;

	COutFileStream *_outFileStreamSpec;
	CMyComPtr<ISequentialOutStream> _outFileStream;

public:
	void Init(IInArchive *archiveHandler, const FString &directoryPath, const UString &fileName = L"");

	UInt64 NumErrors;
	bool PasswordIsDefined;
	UString Password;

	CArchiveExtractCallback() : PasswordIsDefined(false) {}
};

void CArchiveExtractCallback::Init(IInArchive *archiveHandler, const FString &directoryPath, const UString &fileName)
{
	NumErrors = 0;
	_archiveHandler = archiveHandler;
	_directoryPath = directoryPath;
	_fileName = fileName;

	NName::NormalizeDirPathPrefix(_directoryPath);
}

STDMETHODIMP CArchiveExtractCallback::SetTotal(UInt64 /* size */)
{
	return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::SetCompleted(const UInt64 * /* completeValue */)
{
	return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::GetStream(UInt32 index,
	ISequentialOutStream **outStream, Int32 askExtractMode)
{
	*outStream = 0;
	_outFileStream.Release();

	{
	// Get Name
		NCOM::CPropVariant prop;
		RINOK(_archiveHandler->GetProperty(index, kpidPath, &prop));

		UString fullPath;
		if (prop.vt == VT_EMPTY)
			fullPath = kEmptyFileAlias;
		else {
			if (prop.vt != VT_BSTR)
				return E_FAIL;
			fullPath = prop.bstrVal;
		}
		_filePath = fullPath;
	}

	if (!_fileName.IsEmpty())
		_filePath = _fileName;

	if (askExtractMode != NArchive::NExtract::NAskMode::kExtract)
		return S_OK;

	{
	// Get Attrib
		NCOM::CPropVariant prop;
		RINOK(_archiveHandler->GetProperty(index, kpidAttrib, &prop));
		if (prop.vt == VT_EMPTY)
		{
			_processedFileInfo.Attrib = 0;
			_processedFileInfo.AttribDefined = false;
		} else {
			if (prop.vt != VT_UI4)
				return E_FAIL;
			_processedFileInfo.Attrib = prop.ulVal;
			_processedFileInfo.AttribDefined = true;
		}
	}

	RINOK(IsArchiveItemFolder(_archiveHandler, index, _processedFileInfo.isDir));

	{
	// Get Modified Time
		NCOM::CPropVariant prop;
		RINOK(_archiveHandler->GetProperty(index, kpidMTime, &prop));
		_processedFileInfo.MTimeDefined = false;
		switch (prop.vt)
		{
			case VT_EMPTY:
			// _processedFileInfo.MTime = _utcMTimeDefault;
				break;
			case VT_FILETIME:
				_processedFileInfo.MTime = prop.filetime;
				_processedFileInfo.MTimeDefined = true;
				break;
			default:
				return E_FAIL;
		}
	}

	{
	// Get Size
		NCOM::CPropVariant prop;
		RINOK(_archiveHandler->GetProperty(index, kpidSize, &prop));
		UInt64 newFileSize;
		/* bool newFileSizeDefined = */ ConvertPropVariantToUInt64(prop, newFileSize);
	}

	{
	// Create folders for file
		int slashPos = _filePath.ReverseFind_PathSepar();
		if (slashPos >= 0)
			CreateComplexDir(_directoryPath + us2fs(_filePath.Left(slashPos)));
	}

	FString fullProcessedPath = _directoryPath + us2fs(_filePath);
	_diskFilePath = fullProcessedPath;

	if (_processedFileInfo.isDir)
	{
		CreateComplexDir(fullProcessedPath);
	} else {
		NFind::CFileInfo fi;
		if (fi.Find(fullProcessedPath))
		{
			if (!DeleteFileAlways(fullProcessedPath))
			{
				PrintError("Can not delete output file", toQString(fullProcessedPath));
				return E_ABORT;
			}
		}

		_outFileStreamSpec = new COutFileStream;
		CMyComPtr<ISequentialOutStream> outStreamLoc(_outFileStreamSpec);
		if (!_outFileStreamSpec->Open(fullProcessedPath, CREATE_ALWAYS))
		{
			PrintError("Can not open output file", toQString(fullProcessedPath));
			return E_ABORT;
		}
		_outFileStream = outStreamLoc;
		*outStream = outStreamLoc.Detach();
	}
	return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::PrepareOperation(Int32 askExtractMode)
{
	_extractMode = false;
	switch (askExtractMode)
	{
		case NArchive::NExtract::NAskMode::kExtract: _extractMode = true; break;
	};

	QString msgExtract = "";
	switch (askExtractMode)
	{
		case NArchive::NExtract::NAskMode::kExtract: msgExtract = "Extracting: "; break;
		case NArchive::NExtract::NAskMode::kTest: msgExtract = "Testing: "; break;
		case NArchive::NExtract::NAskMode::kSkip: msgExtract = "Skipping: "; break;
	};

	PrintString(msgExtract + toQString(_filePath));
	return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::SetOperationResult(Int32 operationResult)
{
	switch (operationResult)
	{
		case NArchive::NExtract::NOperationResult::kOK:
			break;
		default:
		{
			NumErrors++;
			QString msgExtract = "";
			switch (operationResult)
			{
				case NArchive::NExtract::NOperationResult::kUnsupportedMethod:
					msgExtract = "Unsupported Method";
					break;
				case NArchive::NExtract::NOperationResult::kCRCError:
					msgExtract = "CRC Failed";
					break;
				case NArchive::NExtract::NOperationResult::kDataError:
					msgExtract = "Data Error";
					break;
				case NArchive::NExtract::NOperationResult::kUnavailable:
					msgExtract = "Unavailable data";
					break;
				case NArchive::NExtract::NOperationResult::kUnexpectedEnd:
					msgExtract = "Unexpected end of data";
					break;
				case NArchive::NExtract::NOperationResult::kDataAfterEnd:
					msgExtract = "There are some data after the end of the payload data";
					break;
				case NArchive::NExtract::NOperationResult::kIsNotArc:
					msgExtract = "Is not archive";
					break;
				case NArchive::NExtract::NOperationResult::kHeadersError:
					msgExtract = "Headers Error";
					break;
			}

			if (!msgExtract.isEmpty())
			{
				PrintError(msgExtract);
			} else {
				PrintError("#"+ QString::number(operationResult));
			}
		}
	}

	if (_outFileStream)
	{
		if (_processedFileInfo.MTimeDefined)
			_outFileStreamSpec->SetMTime(&_processedFileInfo.MTime);
		RINOK(_outFileStreamSpec->Close());
	}

	_outFileStream.Release();
	if (_extractMode && _processedFileInfo.AttribDefined)
		SetFileAttrib(_diskFilePath, _processedFileInfo.Attrib);

	PrintNewLine();

	return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::CryptoGetTextPassword(BSTR *password)
{
	if (!PasswordIsDefined)
	{
	// You can ask real password here from user
	// Password = GetPassword(OutStream);
	// PasswordIsDefined = true;
		PrintError("Password is not defined");
		return E_ABORT;
	}
	return StringToBstr(Password, password);
}

#endif // EXTRACT_CALLBACK_H
