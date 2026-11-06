#include "gpk_cyoa.h"
#include "gpk_io.h"

#define superp(format, ...) printf(format "\n", __VA_ARGS__)

usns gpk;

static	gpk::err_t	fileSize		(FILE* fp) {
	u2_t					position		= (u2_t)ftell(fp);
	fseek(fp, 0, SEEK_END);
	u2_t					lastPosition	= (u2_t)ftell(fp);
	fseek(fp, position, SEEK_SET);
	return (int)lastPosition;
}

static	gpk::err_t	splitLines		(const ::gpk::string & pageBytes, ::gpk::aobj<::gpk::string> & pageLines) {
	pageLines.clear();
	u2_t					lastOffset		= 0;
	for(u2_t iOffset = 0; iOffset < pageBytes.size(); ++iOffset) {
		if(pageBytes[iOffset] == '\n') {
			::gpk::vcst_t					lineToPush		= {};
			pageBytes.slice(lineToPush, lastOffset, iOffset - lastOffset + 1);
			pageLines.push_back(lineToPush);
			lastOffset				= ++iOffset;
		}
	}
	if(pageBytes.size() > lastOffset) {
		::gpk::vcst_t					vline			= {};
		pageBytes.slice(vline, lastOffset, lastOffset + 1);

		::gpk::string				lineToPush		= vline;
		lineToPush.append_string("\n");
		pageLines.push_back(lineToPush);
	}
	return 0;
}

tplt <tpnm _tCell>
static	gpk::err_t	reverse			(_tCell* elements, u2_t elementCount)	 {
	for(u2_t i = 0, swapCount = elementCount / 2; i < swapCount; ++i) {
		_tCell						old				= elements[i];
		elements[i]				= elements[elementCount - 1 - i];
		elements[elementCount - 1 - i]	= old;
	}
	return 0;
}

static	gpk::err_t	loadLines		(::gpkg::SPage & page, const ::gpk::aobj<::gpk::string> & pageLines) {
	page.TextLines		= pageLines;
	page.PageJumps		= {};
	for(s2_t iLine = (s2_t)page.TextLines.size() - 1; iLine >= 0; --iLine) {
		::gpk::vcst_t			trimmed		= page.TextLines[iLine];
		::gpk::trim(trimmed);
		page.TextLines[iLine] = trimmed;  
		if(trimmed.size() <= 2 && 0 == page.PageJumps.size()) {
			page.TextLines.pop_back();
			continue;
		}

		u2_t		jumpIndex		= atoi(trimmed.begin());
		if(0 == jumpIndex && trimmed[0] != '0') {
			break;
		}
		page.PageJumps.push_back({trimmed, jumpIndex});
		page.TextLines.pop_back();
	}


	::reverse(&page.PageJumps[0], (u2_t)page.PageJumps.size());
	return 0;
}

gpk::err_t			gpkg::loadPage	(const sc_t* folderName, gpkg::SPage & page, u2_t pageIndex) {
	sc_t						fileName[4096]	= {};
	::sprintf_s(fileName, "%s/%u.txt", folderName, pageIndex);
	FILE						* fp			= 0;
	if(::gpk::fopen_s(&fp, fileName, "rb")) {
		return -1;
	}

	int							textSize		= fileSize(fp);
	::gpk::string				pageBytes;
	pageBytes.resize(textSize);
	fread(&pageBytes[0], 1, textSize, fp);
	fclose(fp);	

	::gpk::aobj<string>		pageLines;
	::splitLines(pageBytes, pageLines);
	::loadLines(page, pageLines);
	return 0;
}

int						gpkg::validJump	(const ::gpk::aobj<gpkg::SJump> & jumps, u2_t indexToTest) {
	for(u2_t iJump = 0; iJump < jumps.size(); ++iJump) {
		if(jumps[iJump].Jump == indexToTest)
			return iJump;
	}
	return -1;
}

