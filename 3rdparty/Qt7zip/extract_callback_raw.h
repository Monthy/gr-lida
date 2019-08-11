/*
 *
 * Qt7zip
 *
 * This file is part of Qt7zip is a C++ wrapper for accessing the 7-zip API
 * based in extract_callbacks.h by https://github.com/YACReader/yacreader
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

#ifndef EXTRACT_CALLBACK_RAW_H
#define EXTRACT_CALLBACK_RAW_H

#include "7zip_includes.h"

// Archive Extracting callback Memory class
//---------------------------------------------------------------------------------------------------------------
class CArchiveExtractCallbackRaw:
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

//	struct CFileInfoRaw
//	{
//		QString path;
//		QByteArray data;
//	} _fileInfoRaw;

private:
	CMyComPtr<IInArchive> _archiveHandler;
	UString _directoryPath; // Output directory
	UString _filePath;      // name inside arcvhive
	UString _diskFilePath;  // full path to file on disk
	bool _extractMode;

	bool _multiExtract;
	QVector<quint32> _indexes;
	UInt32 _index;

	struct CProcessedFileInfo
	{
		FILETIME MTime;
		UInt32 Attrib;
		bool isDir;
		bool AttribDefined;
		bool MTimeDefined;
	} _processedFileInfo;

//	COutFileStream *_outFileStreamSpec;
	CMyComPtr<ISequentialOutStream> _outFileStream;

public:
	void Init(IInArchive *archiveHandler);

	UInt64 NumErrors;
	bool PasswordIsDefined;
	UString Password;

	QList<QByteArray> allFilesRaw;
//	QList<CFileRawInfo> allFileInfoRaw;
	Byte *data;
	UInt64 newFileSize;

	CArchiveExtractCallbackRaw(bool multiExtract = false, QVector<quint32> indexes = QVector<quint32>()) :
		_multiExtract(multiExtract), _indexes(indexes),  _index(0), PasswordIsDefined(false){}

	virtual ~CArchiveExtractCallbackRaw() { MidFree(data); }
};

void CArchiveExtractCallbackRaw::Init(IInArchive *archiveHandler)
{
	NumErrors = 0;
	_archiveHandler = archiveHandler;
//	directoryPath;//unused
}

STDMETHODIMP CArchiveExtractCallbackRaw::SetTotal(UInt64 /* size */)
{
	return S_OK;
}

STDMETHODIMP CArchiveExtractCallbackRaw::SetCompleted(const UInt64 * /* completeValue */)
{
	return S_OK;
}

STDMETHODIMP CArchiveExtractCallbackRaw::GetStream(UInt32 index,
	ISequentialOutStream **outStream, Int32 askExtractMode)
{
	*outStream = 0;
	_outFileStream.Release();

	_index = index;

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

	Q_UNUSED(askExtractMode);//unused
//	if (askExtractMode != NArchive::NExtract::NAskMode::kExtract)
//		return S_OK;

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

// Se necesita conocer el tamaño del archivo para poder reservar suficiente memoria
	bool newFileSizeDefined;
	{
	// Get Size
		NCOM::CPropVariant prop;
		RINOK(_archiveHandler->GetProperty(index, kpidSize, &prop));
		newFileSizeDefined = (prop.vt != VT_EMPTY);
//		UInt64 newFileSize;
		if (newFileSizeDefined)
			ConvertPropVariantToUInt64(prop, newFileSize);
	}

// No hay que crear ningún fichero, ni directorios intermedios
//	{
//		// Create folders for file
//		int slashPos = _filePath.ReverseFind_PathSepar();
//		if (slashPos >= 0)
//			CreateComplexDir(_directoryPath + us2fs(_filePath.Left(slashPos)));
//	}

//	FString fullProcessedPath = _directoryPath + us2fs(_filePath);
//	_diskFilePath = fullProcessedPath;

	if (_processedFileInfo.isDir)
	{
//		CreateComplexDir(fullProcessedPath);
	} else {
//		NFind::CFileInfo fi;
//		if (fi.Find(fullProcessedPath))
//		{
//			if (!DeleteFileAlways(fullProcessedPath))
//			{
//				PrintError("Can not delete output file", QString::fromWCharArray(fullProcessedPath));
//				return E_ABORT;
//			}
//		}

		if (newFileSizeDefined)
		{
			CBufPtrSeqOutStream *outStreamSpec = new CBufPtrSeqOutStream;
			CMyComPtr<CBufPtrSeqOutStream> outStreamLoc(outStreamSpec);
		//	data = (Byte *)MidAlloc(newFileSize);
			data = reinterpret_cast<Byte *>(MidAlloc(newFileSize));
			outStreamSpec->Init(data, newFileSize);
			*outStream = outStreamLoc.Detach();

//			_outFileStreamSpec = new COutFileStream;
//			CMyComPtr<ISequentialOutStream> outStreamLoc(_outFileStreamSpec);
//			if (!_outFileStreamSpec->Open(fullProcessedPath, CREATE_ALWAYS))
//			{
//				PrintError("Can not open output file", QString::fromWCharArray(fullProcessedPath));
//				return E_ABORT;
//			}
//			_outFileStream = outStreamLoc;
//			*outStream = outStreamLoc.Detach();
		}
	}
	return S_OK;
}

STDMETHODIMP CArchiveExtractCallbackRaw::PrepareOperation(Int32 askExtractMode)
{
	_extractMode = false;
	switch (askExtractMode)
	{
		case NArchive::NExtract::NAskMode::kExtract: _extractMode = true; break;
	};

//	QString msgExtract = "";
//	switch (askExtractMode)
//	{
//		case NArchive::NExtract::NAskMode::kExtract: msgExtract = "Extracting: "; break;
//		case NArchive::NExtract::NAskMode::kTest: msgExtract = "Testing: "; break;
//		case NArchive::NExtract::NAskMode::kSkip: msgExtract = "Skipping: "; break;
//	};

//	PrintString(msgExtract + QString::fromWCharArray(_filePath));
	return S_OK;
}

STDMETHODIMP CArchiveExtractCallbackRaw::SetOperationResult(Int32 operationResult)
{
	switch (operationResult)
	{
		case NArchive::NExtract::NOperationResult::kOK:
			if (_multiExtract && !_processedFileInfo.isDir)
			{
				if (_indexes.contains(_index))
				{
	//				QByteArray rawData((char *)data, newFileSize);
					QByteArray rawData(reinterpret_cast<char *>(data), static_cast<int>(newFileSize));
					MidFree(data);
					data = 0;

					allFilesRaw.append(rawData);

				//	_fileInfoRaw.path = toQString(_filePath);
				//	_fileInfoRaw.data = rawData;
				//	allFileInfoRaw.append(_fileRawInfo);
				}
			}
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

//	if (_outFileStream)
//	{
//		if (_processedFileInfo.MTimeDefined)
//			_outFileStreamSpec->SetMTime(&_processedFileInfo.MTime);
//		RINOK(_outFileStreamSpec->Close());
//	}

//	_outFileStream.Release();
//	if (_extractMode && _processedFileInfo.AttribDefined)
//		SetFileAttrib(_diskFilePath, _processedFileInfo.Attrib);

//	PrintNewLine();

	return S_OK;
}

STDMETHODIMP CArchiveExtractCallbackRaw::CryptoGetTextPassword(BSTR *password)
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

#endif // EXTRACT_CALLBACK_RAW_H
