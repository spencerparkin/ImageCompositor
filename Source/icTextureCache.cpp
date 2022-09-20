#include "icTextureCache.h"
#include <wx/filename.h>

icTextureCache::icTextureCache()
{
}

/*virtual*/ icTextureCache::~icTextureCache()
{
}

void icTextureCache::Clear()
{
	for (TextureMap::iterator iter = this->textureMap.begin(); iter != this->textureMap.end(); iter++)
		glDeleteTextures(1, &iter->second.texture);

	this->textureMap.clear();
}

icTextureEntry icTextureCache::GrabTexture(const wxString& texturePath)
{
	icTextureEntry entry;
	entry.texture = GL_INVALID_VALUE;
	entry.aspectRatio = 1.0f;

	wxFileName textureFileName(texturePath);
	textureFileName.Normalize();
	wxString key = textureFileName.GetFullPath().Lower();

	TextureMap::iterator iter = this->textureMap.find(key);
	if (iter != this->textureMap.end())
	{
		entry = iter->second;

		// We hit the cache, but is the cache entry valid?
		wxStructStat stat;
		wxStat(textureFileName.GetFullPath(), &stat);
		wxDateTime lastModTime(stat.st_mtime);
		if (lastModTime != entry.lastModTime)
		{
			this->textureMap.erase(iter);
			glDeleteTextures(1, &entry.texture);
			entry.texture = GL_INVALID_VALUE;
		}
	}
	
	if (entry.texture == GL_INVALID_VALUE)
	{
		wxBusyCursor busyCursor;

		wxImage image;
		if (image.LoadFile(textureFileName.GetFullPath()))
		{
			glGenTextures(1, &entry.texture);
			if (entry.texture != GL_INVALID_VALUE)
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glBindTexture(GL_TEXTURE_2D, entry.texture);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

				GLuint imageWidth = image.GetWidth();
				GLuint imageHeight = image.GetHeight();
				GLubyte* imageBuffer = image.GetData();
				GLuint bytesPerPixel = 3;
				GLuint bytesPerTexel = 4;
				GLubyte* textureBuffer = new GLubyte[imageWidth * imageHeight * bytesPerTexel];

				// We have to flip the image for OpenGL.
				for (GLuint i = 0; i < imageHeight; i++)
				{
					for (GLuint j = 0; j < imageWidth; j++)
					{
						GLubyte* pixel = &imageBuffer[(imageHeight - 1 - i) * imageWidth * bytesPerPixel + j * bytesPerPixel];
						GLubyte* texel = &textureBuffer[i * imageWidth * bytesPerTexel + j * bytesPerTexel];

						texel[0] = pixel[0];
						texel[1] = pixel[1];
						texel[2] = pixel[2];
						texel[3] = 0;
					}
				}

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBuffer);
				delete[] textureBuffer;

				entry.aspectRatio = float(imageWidth) / float(imageHeight);

				wxStructStat stat;
				wxStat(textureFileName.GetFullPath(), &stat);
				wxDateTime lastModTime(stat.st_mtime);
				entry.lastModTime = lastModTime;

				this->textureMap[key] = entry;
			}
		}
	}

	return entry;
}