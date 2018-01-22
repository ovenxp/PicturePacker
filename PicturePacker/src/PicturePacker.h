#pragma once
#include "common.h"



class PicturePacker
{
public:
	PicturePacker( const char* _pathDir);
	~PicturePacker();

	typedef void(PicturePacker::*FunVoidConst4CxImageRefCxImageRefIntInt)(CxImage&, CxImage&, int, int) const;

	void setPictureOutDir(const char* _pathDir);

	void setXmlName(const char* _nameXml);

	void parsePictures();

	void spliceV1();

	void saveOut();

protected:
	void _setupRalated(PicInfo* _picinfo);
	void _setupRalated24(PicInfo* _picinfo);
	bool _buildTreeNodeV1(bool _hasAlpha);

	int _findPicMatchRemainedWithSameWidth(LstPicInfo& _listPic, int _remainHeight, LstPicInfo& _listOut) const;
	int _findPicMatchRemainedWithSameHeight(LstPicInfo& _listPic, int _remainWidth, LstPicInfo& _listOut) const;

	int _findBestMatchPicWithSameDim(bool _matchWidth, LstPicInfo& _listPic, int _remainWidth, LstPicInfo& _listOut) const;

	int _searchBestMatch(bool _matchWidth, int _matchTarget,  VctPicInfo& _refPicSource, int _picSourceUnsearched, LstPicInfo& _refPicResult, int _picUnfilled,   int _accumulatedValue) const;

	int _piecePictureTogether(TiXmlElement* _parentElement, CxImage& _imgDest, BTreeNode* _nodeTree) const;

	void _translateData32(CxImage& _imgDest, CxImage& _imgSource, int _startX, int _startY) const;
	void _translateData24(CxImage& _imgDest, CxImage& _imgSource, int _startX, int _startY) const;
	void _translateDataBlend(CxImage& _imgDest, CxImage& _imgSource, int _startX, int _startY) const;

	TiXmlElement* _findElementByAttr( TiXmlElement* _parentElement, const char* _name, const char* _value) const;

	void _parsePicContainerSize(BTreeNode* _nodeTree, LstPicInfo& _currentPicList);

private:
	std::string			mPathPictureSource;
	std::string			mPathPictureOut;
	std::string			mXmlName;

	LstString			mPicturesNonProc;

	MapInt2LstPicInfo  mMapLstPicInfoOfSameWidthMarchedNot;
	MapInt2LstPicInfo  mMapLstPicInfoOfSameWidthMarchedNot24;
	MapInt2LstPicInfo  mMapLstPicInfoOfSameHeightMarchedNot;
	MapInt2LstPicInfo  mMapLstPicInfoOfSameHeightMarchedNot24;

	LstPicInfo			mListPicInfoAll;
	LstPicInfo			mListPicInfoAll24;

	LstBTree			mListBTree;
	LstBTree			mListBTree24;

	LstBTree            mListBTreeEmpty;

	FunVoidConst4CxImageRefCxImageRefIntInt funTranslateData;
};

typedef std::list<PicturePacker> LstPicturePacker;