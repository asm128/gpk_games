#include "gpk_cyoa.h"

#define superp(format, ...) printf(format "\n", __VA_ARGS__)

int						fileSize		(FILE* fp) {
	uint32_t				position		= (uint32_t)ftell(fp);
	fseek(fp, 0, SEEK_END);
	uint32_t				lastPosition	= (uint32_t)ftell(fp);
	fseek(fp, position, SEEK_SET);
	return (int)lastPosition;
}

int						splitLines		(const ::std::string & pageBytes, ::gpk::aobj<std::string> & pageLines) {
	pageLines.clear();
	uint32_t				lastOffset		= 0;
	for(uint32_t iOffset = 0; iOffset < pageBytes.size(); ++iOffset) {
		if(pageBytes[iOffset] == '\n') {
			const ::std::string	lineToPush		= pageBytes.substr(lastOffset, iOffset - lastOffset + 1);
			pageLines.push_back(lineToPush);
			lastOffset		= ++iOffset;
		}
	}
	if(pageBytes.size() > lastOffset) {
		pageLines.push_back(pageBytes.substr(lastOffset) + "\n");
	}
	return 0;
}

template <typename _tCell>
int32_t					reverse			(_tCell* elements, uint32_t elementCount)	 {
	for(uint32_t i = 0, swapCount = elementCount / 2; i < swapCount; ++i) {
		_tCell								old			= elements[i];
		elements[i]						= elements[elementCount - 1 - i];
		elements[elementCount - 1 - i]	= old;
	}
	return 0;
}

int						loadLines		(::gpkg::SPage & page, const ::gpk::aobj<std::string> & pageLines) {
	page.TextLines			= pageLines;
	page.PageJumps			= {};
	for(int32_t iLine = (int32_t)page.TextLines.size() - 1; iLine >= 0; --iLine) {
		std::string					& currentLine		= page.TextLines[iLine];
		::gpk::vcc		trimmed;
		::gpk::trim(trimmed, ::gpk::vcc{currentLine.data(), (uint32_t)currentLine.size()});
		currentLine = {trimmed.begin(), trimmed.size()};
		if(currentLine.size() <= 2 && 0 == page.PageJumps.size()) {
			page.TextLines.pop_back(0);
			continue;
		}

		uint32_t			jumpIndex			= atoi(currentLine.data());
		if(0 == jumpIndex && currentLine[0] != '0') {
			break;
		}
		page.PageJumps.push_back({currentLine, jumpIndex});
		page.TextLines.pop_back(0);
	}


	::reverse(&page.PageJumps[0], (uint32_t)page.PageJumps.size());
	return 0;
}

int						gpkg::loadPage		(const char* folderName, gpkg::SPage & page, uint32_t pageIndex) {
	char					fileName[4096]	= {};
	sprintf_s(fileName, "%s/%u.txt", folderName, pageIndex);
	FILE					* fp			= 0;
	if(fopen_s(&fp, fileName, "rb")) {
		return -1;
	}

	int						textSize		= fileSize(fp);
	::std::string			pageBytes;
	pageBytes.resize(textSize);
	fread(&pageBytes[0], 1, textSize, fp);
	fclose(fp);	

	::gpk::aobj<::std::string> pageLines;
	::splitLines(pageBytes, pageLines);
	::loadLines(page, pageLines);
	return 0;
}

int						gpkg::validJump	(const ::gpk::aobj<gpkg::SJump> & jumps, uint32_t indexToTest) {
	for(uint32_t iJump = 0; iJump < jumps.size(); ++iJump) {
		if(jumps[iJump].Jump == indexToTest)
			return iJump;
	}
	return -1;
}

