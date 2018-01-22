
#include "PicturePacker.h"
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Shlwapi.lib")



int main(int _argc, char** _argv)
{
	LstPicturePacker listPicturePacker;
	const char* pathOut = NULL;
	const char* nameXml = NULL;
	for (int i = 1; i < _argc; ++i)
	{
		if (::strcmp(_argv[i], "-size") == 0)
		{
			assert(i + 2 < _argc);
			if (i + 2 >= _argc )
			{
				::MessageBoxA(NULL, "format:\"-size 1024 1024\"", "Parameter Error", MB_OK);
				return 0;
			}
			g_targetPictureWidth = ::atoi(_argv[i + 1]);
			g_targetPictureHeight = ::atoi(_argv[i + 2]);
			if (g_targetPictureWidth < 64 || g_targetPictureHeight < 64)
			{
				::MessageBoxA(NULL, "width or height min size should > 64", "Parameter Error", MB_OK);
				return 0;
			}
			i += 2;
		}
		else if (::strcmp(_argv[i], "-gap") == 0)
		{
			assert(i + 1 < _argc);
			if (i + 1 >= _argc)
			{
				::MessageBoxA(NULL, "format:\"-gap 1\", gap < 3, void of number", "Parameter Error", MB_OK);
				return 0;
			}
			g_pixelGap = ::atoi(_argv[i + 1]);
			if (g_pixelGap < 0) g_pixelGap = 0;
			if (g_pixelGap > 10)g_pixelGap = 10;			
			++i;
		}
		else if (::strcmp(_argv[i], "-xml") == 0)
		{
			assert(i + 1 < _argc);
			if (i + 1 >= _argc)
			{
				::MessageBoxA(NULL, "format:\"-gap 1\", gap < 3, void of number", "Parameter Error", MB_OK);
				return 0;
			}

			nameXml = _argv[i + 1];

			++i;
		}
		else if (::strcmp(_argv[i], "-out") == 0)
		{
			if (i + 1 >= _argc)
			{
				::MessageBoxA(NULL, "format:\"-out D:/fileOutDir\". void of dir", "Parameter Error", MB_OK);
				return 0;
			}
			if ( ::PathFileExistsA(_argv[i + 1]))
			{
				if (!::PathIsDirectoryA(_argv[i + 1]))
				{
					::MessageBoxA(NULL, "format:\"-out D:/fileOutDir\". not a dir", "Parameter Error", MB_OK);
					return 0;
				}
			}
			else
			{
				if (!::CreateDirectoryA(_argv[i + 1], NULL))
				{
					::MessageBoxA(NULL, "format:\"-out D:/fileOutDir\". not a dir", "path  Error", MB_OK);
					return 0;
				}
			}
			pathOut = _argv[i + 1];
			++i;
		}
		else if (::strcmp(_argv[i], "-blend") == 0)
		{
			if (i + 1 >= _argc)
			{
				::MessageBoxA(NULL, "format:\"-blend 1\". void of param", "Parameter Error", MB_OK);
				return 0;
			}
			g_premultipliedAlpha = ::atoi(_argv[i + 1]);
			++i;
		}
		else if (::strcmp(_argv[i], "-ext") == 0)
		{
			if (i + 1 >= _argc)
			{
				::MessageBoxA(NULL, "format:\"-ext .tga\". void of param", "Parameter Error", MB_OK);
				return 0;
			}
			ENUM_CXIMAGE_FORMATS extFormat = parseImageNameFormat(_argv[i + 1]);
			if (CXIMAGE_FORMAT_UNKNOWN != extFormat)
			{
				g_outExtFormat = extFormat;
				const char* strExt = strrchr(_argv[i + 1], '.');
				if (strExt)
				{
					++strExt;
				}
				else
				{
					strExt = _argv[i + 1];
				}
				strcpy_s(g_extNameOut, 8, strExt);
			}
			++i;
		}
		else if (::strcmp(_argv[i], "-batch") == 0)
		{
			if (i + 1 >= _argc)
			{
				::MessageBoxA(NULL, "format:\"-out D:/fileOutDir\". void of dir", "Parameter Error", MB_OK);
				return 0;
			}

			++i;
			if (::PathFileExistsA(_argv[i]) && ::PathIsDirectoryA(_argv[i]))
			{
				WIN32_FIND_DATAA wfd;
				std::string pathBase(_argv[i]);
				std::string fileNameSelect = pathBase + "\\*.*";
				char strBuffer[256] = { 0 };
				HANDLE hFile = ::FindFirstFileA(fileNameSelect.c_str(), &wfd);
				while (INVALID_HANDLE_VALUE != hFile)
				{
					if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if (wfd.cFileName[0] != _T('.') && ::strstr(wfd.cFileName, k_material_shared) == NULL )
						{
							::sprintf_s(strBuffer, 256, "%s\\%s", _argv[i], wfd.cFileName);
							listPicturePacker.push_back(PicturePacker(strBuffer));
						}
					}
					if (!::FindNextFile(hFile, &wfd))
					{
						break;
					}
				}
			}

		}
		else if (::PathFileExistsA(_argv[i]))
		{
			if (::PathIsDirectoryA(_argv[i]))
			{
				listPicturePacker.push_back( PicturePacker(_argv[i]) );
			}
			else
			{
				char strbuf[256] = { 0 };
				::sprintf_s(strbuf, 256, "format:\"-out D:/fileOutDir\". %s is not a dir", _argv[i]);
				::MessageBoxA(NULL, strbuf, "path  Error", MB_OK);
			}
		}
	}

	char modulePath[512] = { 0 };
	if (NULL == pathOut)
	{
		::GetModuleFileNameA(NULL, modulePath, 512);
		::PathRemoveFileSpecA(modulePath);
		pathOut = modulePath;
	}

	try
	{
		LstPicturePacker::iterator iend = listPicturePacker.end();
		for (LstPicturePacker::iterator itr = listPicturePacker.begin(); itr != iend; ++itr)
		{
			PicturePacker& refPictPackr = (*itr);
			refPictPackr.setPictureOutDir(pathOut);
			refPictPackr.setXmlName(nameXml);
			refPictPackr.parsePictures();
			refPictPackr.spliceV1();
			refPictPackr.saveOut();
		}
	}
	catch (EExceptionType* e)
	{
		switch (*e)
		{
		case eET_OutofMemory:
			::MessageBoxA(NULL, "Exception Òì³£", "Õ»ÄÚ´æºÄ¾¡", MB_OK);
			break;
		case eET_Default:
			::MessageBoxA(NULL, "Exception Òì³£", "Î´Öª´íÎó", MB_OK);
			break;
		default:
			::MessageBoxA(NULL, "Exception Òì³£", "Î´ÖªµÄÎ´Öª´íÎó", MB_OK);
			break;
		}
	}

//	::getchar();
	// PicturePacker packer;
	return 0;
}
