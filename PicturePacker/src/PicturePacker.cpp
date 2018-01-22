#include "PicturePacker.h"
#include <algorithm>
#include <iostream>

PicturePacker::PicturePacker(const char* _pathDir)
	: mPathPictureSource(_pathDir)
	, funTranslateData(&PicturePacker::_translateData24)
{
}


PicturePacker::~PicturePacker()
{
	LstPicInfo::iterator iend = mListPicInfoAll.end();
	for (LstPicInfo::iterator itr = mListPicInfoAll.begin(); itr != iend; ++itr)
	{
		delete (*itr);
	}
	mListPicInfoAll.clear();

	iend = mListPicInfoAll24.end();
	for (LstPicInfo::iterator itr = mListPicInfoAll24.begin(); itr != iend; ++itr)
	{
		delete (*itr);
	}
	mListPicInfoAll24.clear();

	LstBTree::iterator iendTree = mListBTree.end();
	for (LstBTree::iterator itr = mListBTree.begin(); itr != iendTree; ++itr)
	{
		delete (*itr);
	}
	mListBTree.clear();

	iendTree = mListBTree24.end();
	for (LstBTree::iterator itr = mListBTree24.begin(); itr != iendTree; ++itr)
	{
		delete (*itr);
	}
	mListBTree24.clear();
}

void PicturePacker::setPictureOutDir(const char* _pathDir)
{
	mPathPictureOut = _pathDir;
}

void PicturePacker::parsePictures()
{
	assert( mMapLstPicInfoOfSameWidthMarchedNot.size() == 0 && mMapLstPicInfoOfSameHeightMarchedNot.size() == 0);

	int areaLimit = g_targetPictureHeight * g_targetPictureWidth;
	
	WIN32_FIND_DATAA wfd;
	std::string fileNameSelect = mPathPictureSource + "\\*.*";
	HANDLE hFile = ::FindFirstFileA(fileNameSelect.c_str(), &wfd);
	while (INVALID_HANDLE_VALUE != hFile)
	{
		if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			if (wfd.cFileName[0] != _T('.'))
			{
				std::string filePath = mPathPictureSource + "\\" + wfd.cFileName;

				ENUM_CXIMAGE_FORMATS imgFmt = parseImageNameFormat(wfd.cFileName);
				if (CXIMAGE_FORMAT_UNKNOWN == imgFmt)
				{
					std::cout << "Warning: not image: " << filePath << "\n";
				}
				else
				{
					CxImage ximg;
					if (ximg.Load(filePath.c_str(), imgFmt))
					{
						int hight = ximg.GetHeight();
						int width = ximg.GetWidth();
						if (hight * width >= areaLimit)
						{
							mPicturesNonProc.push_back(filePath);
						}
						else
						{
							PicInfo* picinfo = new PicInfo;
							picinfo->height = ximg.GetHeight();
							picinfo->width = ximg.GetWidth();
							picinfo->sizeArea = picinfo->height * picinfo->width;
						//	picinfo->sizeArea = picinfo->height * picinfo->height + picinfo->width * picinfo->width;

							picinfo->shapeTangent = (float)picinfo->height / picinfo->width;
							picinfo->shapeEccent = picinfo->height > picinfo->width ? picinfo->shapeTangent : (float)picinfo->width / picinfo->height;
							picinfo->shapeEccentArea = picinfo->sizeArea * picinfo->shapeEccent * simpleInvSqrt(picinfo->shapeEccent);

							picinfo->name = wfd.cFileName;

							ximg.AlphaGetPointer() ? _setupRalated(picinfo) : _setupRalated24(picinfo);
						}
					}
					else
					{
						std::cout << "Warning: not image: " << filePath << "\n";
					}
				}
			}
		}
		if (!::FindNextFile(hFile, &wfd))
		{
			break;
		}
	}
	::FindClose(hFile);
	
	mListPicInfoAll.sort(PicInfo::sortByAreaEccentricity);
	mListPicInfoAll24.sort(PicInfo::sortByAreaEccentricity);
	MapInt2LstPicInfo::iterator iendMap = mMapLstPicInfoOfSameWidthMarchedNot.end();
	for (MapInt2LstPicInfo::iterator itr = mMapLstPicInfoOfSameWidthMarchedNot.begin(); iendMap != itr; ++itr)
	{
		if (itr->second.size() > 1)
		{
			itr->second.sort(PicInfo::sortByHeight);
		}
	}
	iendMap = mMapLstPicInfoOfSameHeightMarchedNot.end();
	for (MapInt2LstPicInfo::iterator itr = mMapLstPicInfoOfSameHeightMarchedNot.begin(); iendMap != itr; ++itr)
	{
		if (itr->second.size() > 1)
		{
			itr->second.sort(PicInfo::sortByWidth);
		}
	}
	iendMap = mMapLstPicInfoOfSameWidthMarchedNot24.end();
	for (MapInt2LstPicInfo::iterator itr = mMapLstPicInfoOfSameWidthMarchedNot24.begin(); iendMap != itr; ++itr)
	{
		if (itr->second.size() > 1)
		{
			itr->second.sort(PicInfo::sortByHeight);
		}
	}
	iendMap = mMapLstPicInfoOfSameHeightMarchedNot24.end();
	for (MapInt2LstPicInfo::iterator itr = mMapLstPicInfoOfSameHeightMarchedNot24.begin(); iendMap != itr; ++itr)
	{
		if (itr->second.size() > 1)
		{
			itr->second.sort(PicInfo::sortByWidth);
		}
	}
}

void PicturePacker::_setupRalated(PicInfo* _picinfo)
{
	MapInt2LstPicInfo::iterator iend = mMapLstPicInfoOfSameHeightMarchedNot.end();
	MapInt2LstPicInfo::iterator itr = mMapLstPicInfoOfSameHeightMarchedNot.find(_picinfo->height);
	if (itr == iend)
	{
		mMapLstPicInfoOfSameHeightMarchedNot.insert(std::pair<int, LstPicInfo>(_picinfo->height, LstPicInfo()));
		itr = mMapLstPicInfoOfSameHeightMarchedNot.find(_picinfo->height);
	}
	itr->second.push_back(_picinfo);

	iend = mMapLstPicInfoOfSameWidthMarchedNot.end();
	itr = mMapLstPicInfoOfSameWidthMarchedNot.find(_picinfo->width);
	if (itr == iend)
	{
		mMapLstPicInfoOfSameWidthMarchedNot.insert(std::pair<int, LstPicInfo>(_picinfo->width, LstPicInfo()));
		itr = mMapLstPicInfoOfSameWidthMarchedNot.find(_picinfo->width);
	}
	itr->second.push_back(_picinfo);

	mListPicInfoAll.push_back(_picinfo);
}

void PicturePacker::_setupRalated24(PicInfo* _picinfo)
{
	MapInt2LstPicInfo::iterator iend = mMapLstPicInfoOfSameHeightMarchedNot24.end();
	MapInt2LstPicInfo::iterator itr = mMapLstPicInfoOfSameHeightMarchedNot24.find(_picinfo->height);
	if (itr == iend)
	{
		mMapLstPicInfoOfSameHeightMarchedNot24.insert(std::pair<int, LstPicInfo>(_picinfo->height, LstPicInfo()));
		itr = mMapLstPicInfoOfSameHeightMarchedNot24.find(_picinfo->height);
	}
	itr->second.push_back(_picinfo);

	iend = mMapLstPicInfoOfSameWidthMarchedNot24.end();
	itr = mMapLstPicInfoOfSameWidthMarchedNot24.find(_picinfo->width);
	if (itr == iend)
	{
		mMapLstPicInfoOfSameWidthMarchedNot24.insert(std::pair<int, LstPicInfo>(_picinfo->width, LstPicInfo()));
		itr = mMapLstPicInfoOfSameWidthMarchedNot24.find(_picinfo->width);
	}
	itr->second.push_back(_picinfo);

	mListPicInfoAll24.push_back(_picinfo);
}

void PicturePacker::spliceV1()
{
	for (int i = 0; i < 100 && (mMapLstPicInfoOfSameWidthMarchedNot.size() > 0 || mMapLstPicInfoOfSameHeightMarchedNot.size() > 0); ++i)
	{
		BTreeNode * root = new BTreeNode;
		assert(root);
		if (NULL == root)
		{
			throw eET_OutofMemory;
		}

		_parsePicContainerSize(root, mListPicInfoAll);

		mListBTreeEmpty.push_back(root);
		mListBTree.push_back(root);
		LstPicInfo::iterator itr = mListPicInfoAll.begin();
		if (mListPicInfoAll.end() != itr)
		{
			while (_buildTreeNodeV1(true));
		}
		mListBTreeEmpty.clear();
	}
	for (int i = 0; i < 100 && (mMapLstPicInfoOfSameWidthMarchedNot24.size() > 0 || mMapLstPicInfoOfSameHeightMarchedNot24.size() > 0); ++i)
	{
		BTreeNode * root = new BTreeNode;
		assert(root);
		if (NULL == root)
		{
			throw eET_OutofMemory;
		}
		_parsePicContainerSize(root, mListPicInfoAll24);

		mListBTreeEmpty.push_back(root);
		mListBTree24.push_back(root);
		LstPicInfo::iterator itr = mListPicInfoAll24.begin();
		if (mListPicInfoAll24.end() != itr)
		{
			while (_buildTreeNodeV1(false));
		}
		mListBTreeEmpty.clear();
	}
}

bool PicturePacker::_buildTreeNodeV1(bool _hasAlpha)
{
	LstPicInfo& refListPicAll = _hasAlpha ? mListPicInfoAll : mListPicInfoAll24;
	MapInt2LstPicInfo& refMapPicUnmatchedSameHeight = _hasAlpha ? mMapLstPicInfoOfSameHeightMarchedNot : mMapLstPicInfoOfSameHeightMarchedNot24;
	MapInt2LstPicInfo& refMapPicUnmatchedSameWidth = _hasAlpha ? mMapLstPicInfoOfSameWidthMarchedNot : mMapLstPicInfoOfSameWidthMarchedNot24;

	LstPicInfo::iterator ipic4MarchCurrent = refListPicAll.begin();
	LstPicInfo::iterator ipic4MarchEnd = refListPicAll.end();

	while (ipic4MarchCurrent != ipic4MarchEnd)
	{
		PicInfo* picTmp = *ipic4MarchCurrent;
		if (picTmp->state < ePMS_Success)
		{
			break;
		}
		++ipic4MarchCurrent;
	}
	if (ipic4MarchCurrent == ipic4MarchEnd)
	{
		return false;
	}

	LstBTree::iterator ibtNodeBigest = mListBTreeEmpty.begin();
	LstBTree::iterator ibtNodeLeastEnd = mListBTreeEmpty.end();
	
	int maxFillingDim = 0;
	bool  fitSameWidth = true;
	float maxFillingRate = 0;
	LstPicInfo  listPicBestMatch;
	LstPicInfo::iterator ipicBestMatch = ipic4MarchEnd;
	PicInfo* currentPic4Match = NULL;
	MapInt2LstPicInfo::iterator ipic2Height = refMapPicUnmatchedSameHeight.end();
	MapInt2LstPicInfo::iterator ipic2Width = refMapPicUnmatchedSameWidth.end();
	LstBTree::iterator btNodeBestMatch = ibtNodeLeastEnd;
	LstBTree::iterator ibtNodeMatchFillingRate = ibtNodeBigest;
	while (ibtNodeMatchFillingRate != ibtNodeLeastEnd)
	{
		BTreeNode* btNodematchfilling = *ibtNodeMatchFillingRate;
		int containerArea = btNodematchfilling->capacityHeight * btNodematchfilling->capacityWidth;
		LstPicInfo::iterator ipicmatchfilling = ipic4MarchCurrent;
		while (ipicmatchfilling != ipic4MarchEnd)
		{
			PicInfo* picfilling = *ipicmatchfilling;
			if (picfilling->state < ePMS_Success && picfilling->height <= btNodematchfilling->capacityHeight && picfilling->width <= btNodematchfilling->capacityWidth)
			{
				MapInt2LstPicInfo::iterator iresultHeight = refMapPicUnmatchedSameHeight.find(picfilling->height);
				MapInt2LstPicInfo::iterator iresultWidth = refMapPicUnmatchedSameWidth.find(picfilling->width);
				assert(refMapPicUnmatchedSameHeight.end() != iresultHeight && refMapPicUnmatchedSameWidth.end() != iresultWidth);

				LstPicInfo  listPicMatchSameHeight, listPicMatchSameWidth;
				int matchHeight = _findBestMatchPicWithSameDim(false, iresultWidth->second, btNodematchfilling->capacityHeight, listPicMatchSameWidth);
				int matchWidth = _findBestMatchPicWithSameDim(true, iresultHeight->second, btNodematchfilling->capacityWidth, listPicMatchSameHeight);

				int matchAreaSameWidth = matchHeight * picfilling->width;
				float eccentricitySameWidth = picfilling->width > matchHeight ? (float)picfilling->width / matchHeight : (float)matchHeight / picfilling->width;
				float eccentricitySameWidthWeight = eccentricitySameWidth * simpleInvSqrt(eccentricitySameWidth);
				float weight4SameWidth = eccentricitySameWidthWeight * matchAreaSameWidth;

				int matchAreaSameHeight = matchWidth * picfilling->height;
				float eccentricitySameHeight = matchWidth > picfilling->height ? (float)matchWidth / picfilling->height : (float)picfilling->height / matchWidth;
				float eccentricitySameHeightWeight = eccentricitySameHeight * simpleInvSqrt(eccentricitySameHeight);
				float weight4SameHeight = eccentricitySameHeightWeight * matchAreaSameHeight;
				
				if (weight4SameHeight > weight4SameWidth)
				{
					float tmpFillingRate = (float)(matchAreaSameHeight) / containerArea;
					if (tmpFillingRate > maxFillingRate)
					{
						maxFillingDim = matchWidth;
						ipic2Height = iresultHeight;
						ipic2Width = iresultWidth;
						maxFillingRate = tmpFillingRate;
						btNodeBestMatch = ibtNodeMatchFillingRate;
						listPicBestMatch = listPicMatchSameHeight;
						fitSameWidth = false;
						currentPic4Match = picfilling;
						ipicBestMatch = ipicmatchfilling;
					}
				}
				else
				{
					float tmpFillingRate = (float)(matchAreaSameWidth) / containerArea;
					if (tmpFillingRate > maxFillingRate)
					{
						maxFillingDim = matchHeight;
						ipic2Height = iresultHeight;
						ipic2Width = iresultWidth;
						maxFillingRate = tmpFillingRate;
						btNodeBestMatch = ibtNodeMatchFillingRate;
						listPicBestMatch = listPicMatchSameWidth;
						fitSameWidth = true;
						currentPic4Match = picfilling;
						ipicBestMatch = ipicmatchfilling;
					}
				}
			}

			++ipicmatchfilling;
		}

		++ibtNodeMatchFillingRate;
	}

	if (btNodeBestMatch == ibtNodeLeastEnd)
	{
		return false;
	}

	int crntOccupyWidth = currentPic4Match->width;
	int crntOccupyHeight = currentPic4Match->height;

	BTreeNode* btNodeBest = *btNodeBestMatch;
	btNodeBest->areSameWidth = fitSameWidth;
	LstPicInfo::iterator ipicBestEnd = listPicBestMatch.end();
	if (fitSameWidth)
	{
		crntOccupyHeight = maxFillingDim;
		for (LstPicInfo::iterator ipicBest = listPicBestMatch.begin(); ipicBest != ipicBestEnd; ++ipicBest)
		{
			PicInfo* picCurrent = *ipicBest;
			btNodeBest->listPictures.push_back(picCurrent);
			picCurrent->state = ePMS_Success;
			LstPicInfo::iterator ipicRemove = std::find(ipic2Width->second.begin(), ipic2Width->second.end(), picCurrent);
			assert(ipic2Width->second.end() != ipicRemove);
			ipic2Width->second.erase(ipicRemove);

			MapInt2LstPicInfo::iterator isameDimRemove = refMapPicUnmatchedSameHeight.find(picCurrent->height);
			assert(refMapPicUnmatchedSameHeight.end() != isameDimRemove);

			ipicRemove = std::find(isameDimRemove->second.begin(), isameDimRemove->second.end(), picCurrent);
			assert(isameDimRemove->second.end() != ipicRemove);
			isameDimRemove->second.erase(ipicRemove);
		}
	}
	else
	{
		crntOccupyWidth = maxFillingDim;
		for (LstPicInfo::iterator ipicBest = listPicBestMatch.begin(); ipicBest != ipicBestEnd; ++ipicBest)
		{
			PicInfo* picCurrent = *ipicBest;
			btNodeBest->listPictures.push_back(picCurrent);
			picCurrent->state = ePMS_Success;
			LstPicInfo::iterator ipicRemove = std::find(ipic2Height->second.begin(), ipic2Height->second.end(), picCurrent);
			assert(ipic2Height->second.end() != ipicRemove);
			ipic2Height->second.erase(ipicRemove);

			MapInt2LstPicInfo::iterator isameDimRemove = refMapPicUnmatchedSameWidth.find(picCurrent->width);
			assert(refMapPicUnmatchedSameWidth.end() != isameDimRemove);

			ipicRemove = std::find(isameDimRemove->second.begin(), isameDimRemove->second.end(), picCurrent);
			assert(isameDimRemove->second.end() != ipicRemove);
			isameDimRemove->second.erase(ipicRemove);
		}
	}
	
	MapInt2LstPicInfo::iterator iDim2PicEnd = refMapPicUnmatchedSameWidth.end();
	for (MapInt2LstPicInfo::iterator iDim2Pic = refMapPicUnmatchedSameWidth.begin(); iDim2Pic != iDim2PicEnd; )
	{
		if (iDim2Pic->second.size() < 1)
		{
			iDim2Pic = refMapPicUnmatchedSameWidth.erase(iDim2Pic);
		}
		else
		{
			++iDim2Pic;
		}
	}
	iDim2PicEnd = refMapPicUnmatchedSameHeight.end();
	for (MapInt2LstPicInfo::iterator iDim2Pic = refMapPicUnmatchedSameHeight.begin(); iDim2Pic != iDim2PicEnd;)
	{
		if (iDim2Pic->second.size() < 1)
		{
			iDim2Pic = refMapPicUnmatchedSameHeight.erase(iDim2Pic);
		}
		else
		{
			++iDim2Pic;
		}
	}
	
	mListBTreeEmpty.erase(btNodeBestMatch);

	if (crntOccupyHeight >= btNodeBest->capacityHeight)
	{
		if (crntOccupyWidth >= btNodeBest->capacityWidth)
		{
			return !mListBTreeEmpty.empty();
		}
		else
		{
			BTreeNode* nodeChildRight = new BTreeNode;
			btNodeBest->right = nodeChildRight;
			nodeChildRight->capacityWidth = btNodeBest->capacityWidth - crntOccupyWidth;
			nodeChildRight->capacityHeight = crntOccupyHeight;
			nodeChildRight->posX = btNodeBest->posX + crntOccupyWidth;
			nodeChildRight->posY = btNodeBest->posY;

			mListBTreeEmpty.push_back(nodeChildRight);

			return true;
		}
	}
	else if (crntOccupyWidth >= btNodeBest->capacityWidth)
	{
		BTreeNode* nodeChildDown = new BTreeNode;
		btNodeBest->down = nodeChildDown;
		nodeChildDown->capacityWidth = crntOccupyWidth;
		nodeChildDown->capacityHeight = btNodeBest->capacityHeight - crntOccupyHeight;
		nodeChildDown->posX = btNodeBest->posX;
		nodeChildDown->posY = btNodeBest->posY + crntOccupyHeight;

		mListBTreeEmpty.push_back(nodeChildDown);

		return true;
	}

	int areaRemainExtra = btNodeBest->capacityHeight * btNodeBest->capacityWidth - crntOccupyHeight * crntOccupyWidth;
	int accumulatedFillingRemainExtra = 0;
	float accumulatedShapeEccentricity = 0;

	LstPicInfo::iterator ipicChildProbe = ipicBestMatch;
	++ipicChildProbe;
	while (ipicChildProbe != ipic4MarchEnd)
	{
		PicInfo* picChildProbe = *ipicChildProbe;
		if (picChildProbe->state < ePMS_Success)
		{
			float childFillingRate = (float)picChildProbe->sizeArea / areaRemainExtra;
			float currentPicEccent = childFillingRate * picChildProbe->shapeTangent;

			accumulatedShapeEccentricity += currentPicEccent;
			accumulatedFillingRemainExtra += picChildProbe->sizeArea;
			if (accumulatedFillingRemainExtra > areaRemainExtra)
			{
				break;
			}
		}
		++ipicChildProbe;
	}
	int edgeRemainHeight = btNodeBest->capacityHeight - crntOccupyHeight;
	int edgeRemainWidth = btNodeBest->capacityWidth - crntOccupyWidth;
	int edgeAreaDown = crntOccupyWidth * edgeRemainHeight;
	int edgeAreaRight = crntOccupyHeight * edgeRemainWidth;

	bool rightIsSmall;
	if (accumulatedShapeEccentricity < 0.00001f)
	{
		rightIsSmall = edgeAreaDown > edgeAreaRight;
	}
	else
	{
		accumulatedShapeEccentricity *= ((float)areaRemainExtra / accumulatedFillingRemainExtra);

		float shapeEccentricityCornerDown = edgeRemainHeight * edgeRemainHeight + crntOccupyHeight * crntOccupyHeight;

		float weightEccentricityCornerDown = shapeEccentricityCornerDown / areaRemainExtra;

		float shapeEccentricityCornerRight = edgeRemainHeight * edgeRemainHeight + btNodeBest->capacityHeight * btNodeBest->capacityHeight;

		float weightEccentricityCornerRight = shapeEccentricityCornerRight / areaRemainExtra;

		float weightSimilarCornerDown = (accumulatedShapeEccentricity > weightEccentricityCornerDown)
			? weightEccentricityCornerDown / accumulatedShapeEccentricity
			: accumulatedShapeEccentricity / weightEccentricityCornerDown;

		float weightSimilarCornerRight = (accumulatedShapeEccentricity > weightEccentricityCornerRight)
			? weightEccentricityCornerRight / accumulatedShapeEccentricity
			: accumulatedShapeEccentricity / weightEccentricityCornerRight;

		float wightAreaCornerDown;
		float wightAreaCornerRight;
		if (edgeAreaDown > edgeAreaRight)
		{
			wightAreaCornerDown = (float)(btNodeBest->capacityWidth * edgeRemainHeight) / edgeAreaRight;
			wightAreaCornerRight = 1.f;
		}
		else
		{
			wightAreaCornerDown = 1.f;
			wightAreaCornerRight = (float)(btNodeBest->capacityHeight * edgeRemainWidth) / edgeAreaDown;
		}

		float weightAreaShapeSimilarCornerDown = weightSimilarCornerDown * wightAreaCornerDown;
		float weightAreaShapeSimilarCornerRight = weightSimilarCornerRight * wightAreaCornerRight;
		rightIsSmall = weightAreaShapeSimilarCornerDown > weightAreaShapeSimilarCornerRight;
	}

	BTreeNode* childRight = new BTreeNode;
	BTreeNode* childDown = new BTreeNode;
	btNodeBest->down = childDown;
	btNodeBest->right = childRight;
	childDown->posX = btNodeBest->posX;
	childDown->posY = btNodeBest->posY + crntOccupyHeight;
	childRight->posX = btNodeBest->posX + crntOccupyWidth;
	childRight->posY = btNodeBest->posY;

	if (rightIsSmall)
	{// priority height
		childDown->capacityWidth = btNodeBest->capacityWidth;
		childDown->capacityHeight = btNodeBest->capacityHeight - crntOccupyHeight;

		childRight->capacityWidth = btNodeBest->capacityWidth - crntOccupyWidth;
		childRight->capacityHeight = crntOccupyHeight;
	}
	else
	{// priority width
		childRight->capacityWidth = btNodeBest->capacityWidth - crntOccupyWidth;
		childRight->capacityHeight = btNodeBest->capacityHeight;

		childDown->capacityWidth = crntOccupyWidth;
		childDown->capacityHeight = btNodeBest->capacityHeight - crntOccupyHeight;
	}

	mListBTreeEmpty.push_back(childRight);
	mListBTreeEmpty.push_back(childDown);
	return true;
}

int PicturePacker::_findPicMatchRemainedWithSameWidth(LstPicInfo& _listPic, int _remainHeight, LstPicInfo& _listOut) const
{
	LstPicInfo::iterator iend = _listPic.end();
	int sum = 0;
	for (LstPicInfo::iterator itr = _listPic.begin(); itr != iend; ++itr)
	{
		if (sum + (*itr)->height <= _remainHeight)
		{
			sum += (*itr)->height;
			_listOut.push_back((*itr));
		}
	}
	return sum;
}

int PicturePacker::_findPicMatchRemainedWithSameHeight(LstPicInfo& _listPic, int _remainWidth, LstPicInfo& _listOut) const
{
	LstPicInfo::iterator iend = _listPic.end();
	int sum = 0;
	for (LstPicInfo::iterator itr = _listPic.begin(); itr != iend; ++itr)
	{
		if (sum + (*itr)->width <= _remainWidth)
		{
			sum += (*itr)->width;
			_listOut.push_back((*itr));
		}
	}
	return sum;
}

int PicturePacker::_findBestMatchPicWithSameDim(bool _matchWidth, LstPicInfo& _listPic, int _dimLength, LstPicInfo& _listOut) const
{
	int szPic = _listPic.size();
	assert(szPic > 0);
	VctPicInfo picForSearch;
	picForSearch.resize(szPic);
	PicInfo** dataPicInfo = picForSearch.data();
	
	LstPicInfo::iterator ipic = _listPic.begin();
	LstPicInfo::iterator ipicEnd = _listPic.end();
	int sizeDimDiff = 0;
	int minDimLength = _dimLength;
	int index = 0;
	while (ipic != ipicEnd)
	{
		PicInfo* currentPic = *ipic;
		int currentDimLength = _matchWidth ? currentPic->width : currentPic->height;
		if (currentDimLength < minDimLength)
		{
			minDimLength = currentDimLength;
			++sizeDimDiff;
		}
		dataPicInfo[index] = currentPic;
		++index;
		++ipic;
	}

	if (sizeDimDiff > 110)
	{
		return  _matchWidth
			? _findPicMatchRemainedWithSameHeight(_listPic, _dimLength, _listOut)
			: _findPicMatchRemainedWithSameWidth(_listPic, _dimLength, _listOut);
	}
	else
	{
		LstPicInfo listResult;
		int maxfillint = 0;
		for (int i = 1; i <= szPic; ++i)
		{
			listResult.clear();
			int currentFilling = _searchBestMatch(_matchWidth, _dimLength, picForSearch, szPic, listResult, i, 0);
			if (currentFilling > maxfillint)
			{
				_listOut = listResult;
				maxfillint = currentFilling;
			}
		}

		return maxfillint;
	}

}

int PicturePacker::_searchBestMatch(bool _matchWidth, int _matchTarget, VctPicInfo& _refPicSource, int _picSourceUnsearched, LstPicInfo& _refPicResult, int _picUnfilled, int _accumulatedValue) const
{
	assert(_picSourceUnsearched >= _picUnfilled && _picSourceUnsearched > 0 && _picUnfilled > 0);

	int szPicSource = _refPicSource.size();
	int indexStartSearch = szPicSource - _picSourceUnsearched;
	PicInfo* currentPic4probe = _refPicSource[indexStartSearch];
	int currentDimLength = (_matchWidth ? currentPic4probe->width : currentPic4probe->height);
	int accumulatedValue = _accumulatedValue + currentDimLength;
	int picSourceUnsearched = _picSourceUnsearched - 1;
	if (accumulatedValue > _matchTarget)
	{
		if (picSourceUnsearched >= _picUnfilled)
		{
			for (; picSourceUnsearched >= _picUnfilled; --picSourceUnsearched)
			{
				int idx4Search = szPicSource - picSourceUnsearched;
				PicInfo* picCrnt4Search = _refPicSource[idx4Search];
				int accumuMarch = _accumulatedValue + (_matchWidth ? picCrnt4Search->width : picCrnt4Search->height);
				if (accumuMarch > _matchTarget)
				{
					continue;
				}
				else if (accumuMarch == _matchTarget)
				{
					_refPicResult.push_back(currentPic4probe);
					return _matchTarget;
				}
				else
				{
					return _searchBestMatch(_matchWidth, _matchTarget, _refPicSource, picSourceUnsearched, _refPicResult, _picUnfilled, _accumulatedValue);
				}
			}

			return -1;
		}
		else
		{
			return -1;
		}
	}
	else if (accumulatedValue == _matchTarget)
	{
		_refPicResult.push_back(currentPic4probe);
		return _matchTarget;
	}

	int indexDimAlien = indexStartSearch;
	for (int istart = indexStartSearch + 1; istart < szPicSource; ++istart)
	{
		PicInfo* picCrnt4Search = _refPicSource[istart];
		int dimLeng = _matchWidth ? picCrnt4Search->width : picCrnt4Search->height;
		if (dimLeng < currentDimLength)
		{
			break;
		}
		indexDimAlien = istart;
	}
	int unmarchedPart = _matchTarget - _accumulatedValue;
	int repeatRemainder = unmarchedPart % currentDimLength;
	int redundantFill = unmarchedPart / currentDimLength;
	int similarDimPic = indexDimAlien + 1 - indexStartSearch;
	if (similarDimPic >= redundantFill && redundantFill >= _picUnfilled && !repeatRemainder)
	{
		for (int isame = 0; isame < _picUnfilled; ++isame)
		{
			_refPicResult.push_back(_refPicSource[indexStartSearch + isame]);
		}
		return _matchTarget;
	}

	int maxSamePicProbe = similarDimPic < redundantFill ? similarDimPic : redundantFill;
	maxSamePicProbe = _picUnfilled < maxSamePicProbe ? _picUnfilled : maxSamePicProbe;
	int remainUnequal = szPicSource - 1 - indexDimAlien;
	int minSamePicProbe = _picUnfilled > remainUnequal ? _picUnfilled - remainUnequal : 0;

	LstPicInfo listPicBestMatch;
	int bestMatchValue = -1;
	int maxSamePicProbeBound = -1;
	if (maxSamePicProbe == _picUnfilled)
	{
		maxSamePicProbeBound = _picUnfilled;
		listPicBestMatch = _refPicResult;
		for (int jpush = 0; jpush < maxSamePicProbeBound; ++jpush)
		{
			listPicBestMatch.push_back(_refPicSource[indexStartSearch + jpush]);
		}
		bestMatchValue = _accumulatedValue + currentDimLength * maxSamePicProbeBound;
	}
	else
	{
		maxSamePicProbeBound = maxSamePicProbe + 1;
	}

	for (int isame = minSamePicProbe; isame < maxSamePicProbeBound; ++isame)
	{
		LstPicInfo listPicBestMatchProbe = _refPicResult;
		int accumulatedSamePic = _accumulatedValue + currentDimLength * isame;

		for (int jpush = 0; jpush < isame; ++jpush)
		{
			listPicBestMatchProbe.push_back(_refPicSource[indexStartSearch + jpush]);
		}

		int bestMatchValueProbe = _searchBestMatch(_matchWidth, _matchTarget, _refPicSource, remainUnequal, listPicBestMatchProbe, _picUnfilled - isame, accumulatedSamePic);
		if (bestMatchValueProbe > bestMatchValue)
		{
			bestMatchValue = bestMatchValueProbe;
			listPicBestMatch = listPicBestMatchProbe;
		}
	}
	if (bestMatchValue > 0)
	{
		_refPicResult = listPicBestMatch;
	}
	return bestMatchValue;
}

int PicturePacker::_piecePictureTogether(TiXmlElement* _parentElement, CxImage& _imgDest, BTreeNode* _nodeTree) const
{
	assert(_nodeTree);
	int starX = _nodeTree->posX;
	int starY = _nodeTree->posY;
	int tgtWidth = _imgDest.GetWidth();
	int tgtHeight = _imgDest.GetHeight();

	int areaFilled = 0;
	LstPicInfo::iterator iendPic = _nodeTree->listPictures.end();
	for (LstPicInfo::iterator itr = _nodeTree->listPictures.begin(); itr != iendPic; ++itr)
	{
		PicInfo* currentPic = (*itr);
		CxImage ximgSource;
		char buff[360] = { 0 };

		::sprintf_s(buff, 360, "%s\\%s", mPathPictureSource.c_str(), currentPic->name.c_str());
		ENUM_CXIMAGE_FORMATS imgFmt = parseImageNameFormat(currentPic->name.c_str());
		if (ximgSource.Load(buff, imgFmt))
		{
			int imgWidth = ximgSource.GetWidth();
			int imgHeight = ximgSource.GetHeight();

			areaFilled += imgHeight * imgWidth;

			(this->*funTranslateData)(_imgDest, ximgSource, starX, starY);

			TiXmlElement* elePic = new TiXmlElement("imageSource");
			_parentElement->LinkEndChild(elePic);
			elePic->SetAttribute("name", currentPic->name.c_str());
			//	::sprintf_s(buff, 360, "%d %d %d %d", starX, starY, imgWidth, imgHeight);
			int dowm2top = tgtHeight - (starY + imgHeight);
			::sprintf_s(buff, 360, "%d %d %d %d", starX, dowm2top, imgWidth, imgHeight);
			elePic->SetAttribute("coord", buff);

			float uvStartX = ((float)starX) / tgtWidth;
			float uvStartY = ((float)(tgtHeight - starY)) / tgtHeight;
			float uvScaleX = ((float)imgWidth) / tgtWidth;
			float uvScaleY = ((float)(-imgHeight)) / tgtHeight;

			::sprintf_s(buff, 360, "%f %f %f %f", uvStartX, uvStartY, uvScaleX, uvScaleY);
			elePic->SetAttribute("uvCoord", buff);

			_nodeTree->areSameWidth ? starY += (currentPic->height) : starX += currentPic->width;

		}
		else
		{
			std::cout << "ERR: load fail: " << buff << "\n";
		}
	}

	if (_nodeTree->right)
	{
		areaFilled += _piecePictureTogether(_parentElement, _imgDest, _nodeTree->right);
	}
	if (_nodeTree->down)
	{
		areaFilled += _piecePictureTogether(_parentElement, _imgDest, _nodeTree->down);
	}
	return areaFilled;
}

void PicturePacker::_translateData32(CxImage& _imgDest, CxImage& _imgSource, int _startX, int _startY) const
{
	int tgtWidth = _imgDest.GetWidth();
	int tgtHeight = _imgDest.GetHeight();

	int bytePP = _imgSource.GetBpp();
	bytePP = bytePP >> 3;
	assert(bytePP > 2);//3byte RGB = bytePP
	int bytesLineOffset = bytePP * _startX;
	int imgWidth = _imgSource.GetWidth();
	int imgHeight = _imgSource.GetHeight();
	int lineBytes = imgWidth * bytePP;

	assert(_startX + imgWidth <= tgtWidth || _startY + imgHeight <= tgtHeight);

	for (int i = 0; i < imgHeight; ++i)
	{
		unsigned char* dest = _imgDest.GetBits(i + _startY) + bytesLineOffset;
		unsigned char* destAlpha = _imgDest.AlphaGetPointer(_startX, i + _startY);
		unsigned char* src = _imgSource.GetBits(i);
		unsigned char* srcAlpha = _imgSource.AlphaGetPointer(0, i);
		assert(dest && src && srcAlpha && destAlpha);

		::memcpy(dest, src, lineBytes);
		::memcpy(destAlpha, srcAlpha, imgWidth);
	}
}

void PicturePacker::_translateData24(CxImage& _imgDest, CxImage& _imgSource, int _startX, int _startY) const
{
	int tgtWidth = _imgDest.GetWidth();
	int tgtHeight = _imgDest.GetHeight();

	int bytePP = _imgSource.GetBpp();
	bytePP = bytePP >> 3;
	assert(bytePP > 2);//3byte RGB = bytePP
	int bytesLineOffset = bytePP * _startX;
	int imgWidth = _imgSource.GetWidth();
	int imgHeight = _imgSource.GetHeight();
	int lineBytes = imgWidth * bytePP;

	assert(_startX + imgWidth <= tgtWidth || _startY + imgHeight <= tgtHeight);

	for (int i = 0; i < imgHeight; ++i)
	{
		unsigned char* dest = _imgDest.GetBits(i + _startY) + bytesLineOffset;
		unsigned char* src = _imgSource.GetBits(i);
		assert(dest && src );

		::memcpy(dest, src, lineBytes);
	}
}

void PicturePacker::_translateDataBlend(CxImage& _imgDest, CxImage& _imgSource, int _startX, int _startY) const
{
	int tgtWidth = _imgDest.GetWidth();
	int tgtHeight = _imgDest.GetHeight();

	int bytePP = _imgSource.GetBpp();
	bytePP = bytePP >> 3;
	assert(bytePP > 2);//3byte RGB = bytePP
	int bytesLineOffset = bytePP * _startX;
	int imgWidth = _imgSource.GetWidth();
	int imgHeight = _imgSource.GetHeight();

	assert(_startX + imgWidth <= tgtWidth || _startY + imgHeight <= tgtHeight);

	for (int i = 0; i < imgHeight; ++i)
	{
		unsigned char* dest = _imgDest.GetBits(i + _startY) + bytesLineOffset;
		unsigned char* destAlpha = _imgDest.AlphaGetPointer(_startX, i + _startY);
		unsigned char* src = _imgSource.GetBits(i);
		unsigned char* srcAlpha = _imgSource.AlphaGetPointer(0, i);
		assert(dest && src && srcAlpha && destAlpha);
		::memcpy(destAlpha, srcAlpha, imgWidth);
		for (int jcol = 0; jcol < imgWidth; ++jcol)
		{
			double dotAlpha = c_colorUnit * (double)srcAlpha[jcol];
			dest[0] = dotAlpha * (double)src[0];
			dest[1] = dotAlpha * (double)src[1];
			dest[2] = dotAlpha * (double)src[2];
			dest += bytePP;
			src += bytePP;
		}
	}
}

void PicturePacker::saveOut()
{
	std::string xmlFullPath = mPathPictureOut + "\\" + mXmlName;
	TiXmlDocument docxml;
	TiXmlElement* docRoot = NULL;
	bool needSearch = false;
	if (docxml.LoadFile(xmlFullPath.c_str()))
	{
		docRoot = docxml.RootElement();
		needSearch = true;
	}
	else
	{
		docRoot = new TiXmlElement("PicturePacker");
		docxml.LinkEndChild(docRoot);
		docRoot->SetAttribute("type", "resource");
		docRoot->SetAttribute("name", "plist");
		docRoot->SetAttribute("version", "1.0");
	}

	funTranslateData = g_premultipliedAlpha
		? &PicturePacker::_translateDataBlend
		: &PicturePacker::_translateData32;

	std::string picMergedName = mPathPictureSource.substr(mPathPictureSource.find_last_of("\\") + 1);
	LstBTree::iterator iendTree = mListBTree.end();
	int numPicIdx = 0;
	char fileName[256] = { 0 };
	for (LstBTree::iterator itr = mListBTree.begin(); itr != iendTree; ++itr, ++numPicIdx)
	{
		TiXmlElement* elePicture = NULL;
		(numPicIdx)
			? ::sprintf_s(fileName, 256, "%s%d.%s", picMergedName.c_str(), numPicIdx, g_extNameOut)
			: ::sprintf_s(fileName, 256, "%s.%s", picMergedName.c_str(), g_extNameOut);
		if (needSearch)
		{			
			elePicture = _findElementByAttr(docRoot, "name", fileName);
			if (elePicture)
			{
				elePicture->Clear();
			}
		}
		if (NULL == elePicture)
		{
			elePicture = new TiXmlElement("picture");
			docRoot->LinkEndChild(elePicture);
		}
		elePicture->SetAttribute("bpp", 32);
		elePicture->SetAttribute("name", fileName);

		BTreeNode* treenode = (*itr);
		char strParam[64] = { 0 };
		::sprintf_s(strParam, 64, "%d %d", treenode->capacityWidth, treenode->capacityHeight);
		elePicture->SetAttribute("size", strParam);

		CxImage ximg;
		unsigned char* pData = (unsigned char*)ximg.Create(treenode->capacityWidth, treenode->capacityHeight, 32, g_outExtFormat);
		ximg.AlphaCreate();
		float areaFilled = _piecePictureTogether(elePicture, ximg, treenode);

		int areaContainer = treenode->capacityWidth * treenode->capacityHeight;
		areaFilled = areaFilled / areaContainer;
		::sprintf_s(strParam, 64, "%f", areaFilled);
		elePicture->SetAttribute("fill", strParam);

		std::string imgName = mPathPictureOut + "\\" + fileName;
		ximg.Save(imgName.c_str(), g_outExtFormat);
	}

	funTranslateData = &PicturePacker::_translateData24;
	numPicIdx = 0;
	iendTree = mListBTree24.end();
	for (LstBTree::iterator itr = mListBTree24.begin(); itr != iendTree; ++itr, ++numPicIdx)
	{
		TiXmlElement* elePicture = NULL;
		char fileName[256] = { 0 };
		(numPicIdx)
			? ::sprintf_s(fileName, 256, "%s_24_%d.%s", picMergedName.c_str(), numPicIdx, g_extNameOut)
			: ::sprintf_s(fileName, 256, "%s_24.%s", picMergedName.c_str(), g_extNameOut);
		if (needSearch)
		{
			elePicture = _findElementByAttr(docRoot, "name", fileName);
			if (elePicture)
			{
				elePicture->Clear();
			}
		}
		if (NULL == elePicture)
		{
			elePicture = new TiXmlElement("picture");
			docRoot->LinkEndChild(elePicture);
		}
		elePicture->SetAttribute("bpp", 24);
		elePicture->SetAttribute("name", fileName);

		BTreeNode* treenode = (*itr);
		char strParam[64] = { 0 };
		::sprintf_s(strParam, 64, "%d %d", treenode->capacityWidth, treenode->capacityHeight);
		elePicture->SetAttribute("size", strParam);

		CxImage ximg;
		unsigned char* pData = (unsigned char*)ximg.Create(treenode->capacityWidth, treenode->capacityHeight, 24, g_outExtFormat);
		float areaFilled = _piecePictureTogether(elePicture, ximg, treenode);
		int areaContainer = treenode->capacityWidth * treenode->capacityHeight;
		areaFilled = areaFilled / areaContainer;
		::sprintf_s(strParam, 64, "%f", areaFilled);
		elePicture->SetAttribute("fill", strParam);

		std::string imgName = mPathPictureOut + "\\" + fileName;
		ximg.Save(imgName.c_str(), g_outExtFormat);
	}

	docxml.SaveFile(xmlFullPath.c_str());
	LstString::iterator iendNamePic = mPicturesNonProc.end();
	char charBuffer[256] = { 0 };
	for (LstString::iterator istr = mPicturesNonProc.begin(); istr != iendNamePic; ++istr)
	{
		std::string& picSource = (*istr);
		::strcpy_s(charBuffer, 256, picSource.c_str());
		::PathStripPathA(charBuffer);
		std::string tgtName = mPathPictureOut + "\\" + charBuffer;
		::CopyFileA(picSource.c_str(), tgtName.c_str(), false);
	}
}

void PicturePacker::setXmlName(const char* _nameXml)
{
	if (_nameXml)
	{
		mXmlName = _nameXml;
	}
	else
	{
		mXmlName = mPathPictureSource.substr(mPathPictureSource.find_last_of("\\") + 1) + ".xml";
	}
}

TiXmlElement* PicturePacker::_findElementByAttr(TiXmlElement* _parentElement, const char* _name, const char* _value) const
{
	assert(_value && _name);
	TiXmlElement* ele = _parentElement->FirstChildElement();
	while (NULL != ele)
	{
		const TiXmlAttribute* xmlAttr = ele->FirstAttribute();
		while (NULL != xmlAttr)
		{
			if (::strcmp(xmlAttr->Name(), _name) == 0)
			{
				if (::strcmp(xmlAttr->Value(), _value) == 0)
				{
					return ele;
				}
				else
				{
					break;
				}
			}
			xmlAttr = xmlAttr->Next();
		}
		ele = ele->NextSiblingElement();
	}
	return NULL;
}

void PicturePacker::_parsePicContainerSize(BTreeNode* _nodeTree, LstPicInfo& _currentPicList)
{
	long long sumArea = 0;
	float sumTan = 0.0001f;
	int maxWidth = 0;
	int maxHeight = 0;
	LstPicInfo::iterator iendPicList = _currentPicList.end();
	for (LstPicInfo::iterator itr = _currentPicList.begin(); itr != iendPicList; ++itr)
	{
		PicInfo* picinf = (*itr);
		if (picinf->state < ePMS_Success)
		{
			if (picinf->width > maxWidth)
			{
				maxWidth = picinf->width;
			}
			if (picinf->height > maxHeight)
			{
				maxHeight = picinf->height;
			}
			sumTan += picinf->height * picinf->height;
			sumArea += picinf->width * picinf->height;
		}
	}
	sumTan = sumArea > 0 ? sumTan / sumArea : 1.0f;
	unsigned int cellWidth = getSingleCellBinary(maxWidth);
	unsigned int cellHeight = getSingleCellBinary(maxHeight);

	long long areaCell = cellHeight * cellWidth;

	unsigned int limitWidth = (cellWidth > g_targetPictureWidth) ? cellWidth : g_targetPictureWidth;
	unsigned int limitHeight = (cellHeight > g_targetPictureHeight) ? cellHeight : g_targetPictureHeight;
	long long areaLimit = limitWidth * limitHeight;

	if (sumTan > 1.0f)
	{
		while (areaCell < areaLimit && areaCell < sumArea)
		{
			float cellTan = (float)cellHeight / cellWidth;
			(cellTan < 1.0f) 
				? cellHeight = cellHeight << 1 
				: cellWidth = cellWidth << 1;

			areaCell = areaCell << 1;
		}
	}
	else
	{
		while (areaCell < areaLimit && areaCell < sumArea)
		{
			float cellTan = (float)cellHeight / cellWidth;
			(cellTan > 1.0f) 
				? cellWidth = cellWidth << 1 
				: cellHeight = cellHeight << 1;

			areaCell = areaCell << 1;
		}
	}

	_nodeTree->capacityWidth = cellWidth;
	_nodeTree->capacityHeight = cellHeight;
}
