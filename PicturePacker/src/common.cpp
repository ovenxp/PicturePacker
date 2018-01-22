#include "common.h"

int g_targetPictureWidth = 1024;
int g_targetPictureHeight = 1024;
int g_pixelGap = 0;
bool g_premultipliedAlpha = false;
ENUM_CXIMAGE_FORMATS g_outExtFormat = CXIMAGE_FORMAT_TGA;
const double c_colorUnit = 0.0039215686274509803921568627451;
char g_extNameOut[8] = { 't', 'g', 'a' , 0};

const char* k_material_shared = "shared_by_material";

unsigned int getSingleCellBinary(unsigned int _source)
{
	if (_source > 0x7fffffff)
	{
		return _source;
	}
	unsigned int rest = 1;
	while (rest < _source)
	{
		rest = rest << 1;
	}

	return rest;
}

ENUM_CXIMAGE_FORMATS parseImageNameFormat(const char* _fileName)
{
	const char* strExt = strrchr(_fileName, '.');
	if (strExt)
	{
		++strExt;
	}
	else
	{
		strExt = _fileName;
	}
	if (::strstr(strExt, "tga") || ::strstr(strExt, "TGA"))
	{
		return CXIMAGE_FORMAT_TGA;
	}
	else if (::strstr(strExt, "bmp") || ::strstr(strExt, "BMP"))
	{
		return CXIMAGE_FORMAT_BMP;
	}
	else if (::strstr(strExt, "jpg") || ::strstr(strExt, "JPG"))
	{
		return CXIMAGE_FORMAT_JPG;
	}
	else if (::strstr(strExt, "png") || ::strstr(strExt, "PNG"))
	{
		return CXIMAGE_FORMAT_PNG;
	}
	else if (::strstr(strExt, "ico") || ::strstr(strExt, "ICO"))
	{
		return CXIMAGE_FORMAT_ICO;
	}
	else if (::strstr(strExt, "pcx") || ::strstr(strExt, "PCX"))
	{
		return CXIMAGE_FORMAT_PCX;
	}
	else if (::strstr(strExt, "tif") || ::strstr(strExt, "TIF"))
	{
		return CXIMAGE_FORMAT_TIF;
	}
	else if (::strstr(strExt, "gif") || ::strstr(strExt, "GIF"))
	{
		return CXIMAGE_FORMAT_GIF;
	}
	else if (::strstr(strExt, "wbmp") || ::strstr(strExt, "WBMP"))
	{
		return CXIMAGE_FORMAT_WBMP;
	}
	else if (::strstr(strExt, "raw") || ::strstr(strExt, "RAW"))
	{
		return CXIMAGE_FORMAT_RAW;
	}
	else if (::strstr(strExt, "ras") || ::strstr(strExt, "RAS"))
	{
		return CXIMAGE_FORMAT_RAS;
	}
	else if (::strstr(strExt, "wmf") || ::strstr(strExt, "WMF"))
	{
		return CXIMAGE_FORMAT_WMF;
	}
	else if (::strstr(strExt, "jp2") || ::strstr(strExt, "JP2"))
	{
		return CXIMAGE_FORMAT_JP2;
	}
	else if (::strstr(strExt, "jpc") || ::strstr(strExt, "JPC"))
	{
		return CXIMAGE_FORMAT_JPC;
	}
	else if (::strstr(strExt, "pgx") || ::strstr(strExt, "PGX"))
	{
		return CXIMAGE_FORMAT_PGX;
	}
	else if (::strstr(strExt, "pnm") || ::strstr(strExt, "PNM"))
	{
		return CXIMAGE_FORMAT_PNM;
	}
	// 	else if (::strstr(strExt, "jbg") || ::strstr(strExt, "JBG"))
	// 	{
	// 		return CXIMAGE_FORMAT_JBG;
	// 	}
	else if (::strstr(strExt, "psd") || ::strstr(strExt, "PSD"))
	{
		return CXIMAGE_FORMAT_PSD;
	}
	else
	{
		return CXIMAGE_FORMAT_UNKNOWN;
	}
}

float simpleInvSqrt(float x)
{
	float xhalf = 0.5f*x;
	int i = *(int*)&x;
	i = 0x5f3759df - (i >> 1);
	x = *(float*)&i;
	x = x*(1.5f - xhalf*x*x);

	return x;
}