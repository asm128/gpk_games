#include "gpk_cyoa.h"

#define superp(format, ...) printf(format "\n", __VA_ARGS__)

static	::gpk::error_t	fileSize		(FILE* fp) {
	uint32_t					position		= (uint32_t)ftell(fp);
	fseek(fp, 0, SEEK_END);
	uint32_t					lastPosition	= (uint32_t)ftell(fp);
	fseek(fp, position, SEEK_SET);
	return (int)lastPosition;
}

static	::gpk::error_t	splitLines		(const ::gpk::achar & pageBytes, ::gpk::aobj<gpk::achar> & pageLines) {
	pageLines.clear();
	uint32_t					lastOffset		= 0;
	for(uint32_t iOffset = 0; iOffset < pageBytes.size(); ++iOffset) {
		if(pageBytes[iOffset] == '\n') {
			::gpk::vcc					lineToPush		= {};
			pageBytes.slice(lineToPush, lastOffset, iOffset - lastOffset + 1);
			pageLines.push_back(lineToPush);
			lastOffset				= ++iOffset;
		}
	}
	if(pageBytes.size() > lastOffset) {
		::gpk::vcc					vline			= {};
		pageBytes.slice(vline, lastOffset, lastOffset + 1);

		::gpk::achar				lineToPush		= vline;
		lineToPush.append_string("\n");
		pageLines.push_back(lineToPush);
	}
	return 0;
}

template <typename _tCell>
static	::gpk::error_t	reverse			(_tCell* elements, uint32_t elementCount)	 {
	for(uint32_t i = 0, swapCount = elementCount / 2; i < swapCount; ++i) {
		_tCell						old				= elements[i];
		elements[i]				= elements[elementCount - 1 - i];
		elements[elementCount - 1 - i]	= old;
	}
	return 0;
}

static	::gpk::error_t	loadLines		(::gpkg::SPage & page, const ::gpk::aachar & pageLines) {
	page.TextLines			= pageLines;
	page.PageJumps			= {};
	for(int32_t iLine = (int32_t)page.TextLines.size() - 1; iLine >= 0; --iLine) {
		::gpk::achar				& currentLine	= page.TextLines[iLine];
		::gpk::vcc					trimmed;
		::gpk::trim(trimmed, currentLine);
		currentLine				= trimmed;
		if(currentLine.size() <= 2 && 0 == page.PageJumps.size()) {
			page.TextLines.pop_back();
			continue;
		}

		uint32_t					jumpIndex		= atoi(currentLine.begin());
		if(0 == jumpIndex && currentLine[0] != '0') {
			break;
		}
		page.PageJumps.push_back({currentLine, jumpIndex});
		page.TextLines.pop_back();
	}


	::reverse(&page.PageJumps[0], (uint32_t)page.PageJumps.size());
	return 0;
}

::gpk::error_t			gpkg::loadPage	(const char* folderName, gpkg::SPage & page, uint32_t pageIndex) {
	char						fileName[4096]	= {};
	sprintf_s(fileName, "%s/%u.txt", folderName, pageIndex);
	FILE						* fp			= 0;
	if(fopen_s(&fp, fileName, "rb")) {
		return -1;
	}

	int							textSize		= fileSize(fp);
	::gpk::achar				pageBytes;
	pageBytes.resize(textSize);
	fread(&pageBytes[0], 1, textSize, fp);
	fclose(fp);	

	::gpk::aachar				pageLines;
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

