#pragma once 
#ifndef __COMMON_H_
#define  __COMMON_H_

#include <string>
#include <list>
#include <hash_map>
#include <cstdio>
#include <assert.h>
#include <Shlwapi.h>
#include "ximage.h"
#include "tinyxml.h"
#include <windows.h>

struct _BTreeNode;

typedef std::list<std::string> LstString;

typedef enum _eSpliceStrategy
{
	eSS_TraversalPreorder,
	eSS_TraversalInorder,
	eSS_TraversalPostorder,
	eSS_TraversalPostMarchSize,
} ESpliceStrategy;

typedef enum _ePicMarchState
{
	ePMS_Unmatched,
	ePMS_MatchDelay,
	ePMS_Success,
	ePMS_OverSize,
} EPicMarchState;
 
typedef struct _PicInfo
{
	_PicInfo() : state(ePMS_Unmatched), width(0), height(0), sizeArea(0), shapeEccent(1), shapeEccentArea(0), shapeTangent(1), nodeContainer(NULL) {}

	static bool sortByAreaSize(const struct _PicInfo* _picLeft, const struct _PicInfo* _picRight)
	{
		return _picLeft->sizeArea > _picRight->sizeArea;
	}

	static bool sortByAreaEccentricity(const struct _PicInfo* _picLeft, const struct _PicInfo* _picRight)
	{
		return _picLeft->shapeEccentArea > _picRight->shapeEccentArea;
	}

	static bool sortByWidth(const struct _PicInfo* _picLeft, const struct _PicInfo* _picRight)
	{
		return _picLeft->width > _picRight->width;
	}
	static bool sortByHeight(const struct _PicInfo* _picLeft, const struct _PicInfo* _picRight)
	{
		return _picLeft->height > _picRight->height;
	}
	EPicMarchState state;
	int width;
	int height;
	int sizeArea;
	float shapeEccent;
	float shapeEccentArea;
	float shapeTangent;

	std::string name;

	_BTreeNode* nodeContainer;
} PicInfo;


typedef std::list<_PicInfo*> LstPicInfo;
typedef std::vector<_PicInfo*> VctPicInfo;
// @brief store LstPickInfo with same width or height
typedef std::hash_map<int, LstPicInfo> MapInt2LstPicInfo;

typedef struct _BTreeNode
{
	_BTreeNode() : /*parent(NULL), */down(NULL), right(NULL), areSameWidth(true), capacityWidth(0), capacityHeight(0), posX(0), posY(0) {}
	~_BTreeNode()
	{
		if (right)
		{
			delete right;
			right = NULL;
		}
		if (down)
		{
			delete down;
			down = NULL;
		}
	}

	static bool sortByCapacity(const struct _BTreeNode* _nodeLeft, const struct _BTreeNode* _nodeRight)
	{
		return _nodeLeft->capacityWidth * _nodeLeft->capacityHeight > _nodeRight->capacityWidth * _nodeRight->capacityHeight;
	}

	/*_BTreeNode* parent;*/
	_BTreeNode* down;	//	adapte width
	_BTreeNode* right;	//	adapte height

	LstPicInfo  listPictures;
	bool		areSameWidth;

	int			posX;
	int			posY;

	int			capacityWidth;
	int			capacityHeight;
} BTreeNode;
typedef std::list<BTreeNode*> LstBTree;

typedef enum _eExceptionType
{
	eET_OutofMemory = 1,
	eET_Default,
}EExceptionType;

unsigned int getSingleCellBinary(unsigned int _source);
ENUM_CXIMAGE_FORMATS parseImageNameFormat(const char* _fileName);
float simpleInvSqrt(float x);

extern int g_targetPictureWidth;
extern int g_targetPictureHeight;
extern int g_pixelGap;
extern const char* k_material_shared;

extern bool g_premultipliedAlpha;
extern ENUM_CXIMAGE_FORMATS g_outExtFormat;

extern const double c_colorUnit;

extern char g_extNameOut[8];

#endif