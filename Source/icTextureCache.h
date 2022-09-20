#pragma once

#include "icCanvas.h"
#include <wx/string.h>
#include <wx/hashmap.h>

struct icTextureEntry
{
	GLuint texture;
	float aspectRatio;
	wxDateTime lastModTime;
};

WX_DECLARE_STRING_HASH_MAP(icTextureEntry, TextureMap);

class icTextureCache
{
public:
	icTextureCache();
	virtual ~icTextureCache();

	icTextureEntry GrabTexture(const wxString& texturePath);
	void Clear();

private:

	TextureMap textureMap;
};