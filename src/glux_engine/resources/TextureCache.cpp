#include "TextureCache.h"

template<> TextureCache * Singleton<TextureCache>::m_pInstance = 0;

//-----------------------------------------------------------------------------

TextureCache::TextureCache(void) :
	m_is_IL_Initialized( false )
{
}

//-----------------------------------------------------------------------------

TextureCache::~TextureCache(void)
{
}

//-----------------------------------------------------------------------------

GLuint TextureCache::Create2DManual(const char* _name, int _resX, int _resY, GLint _internalFormat, GLenum _dataType, GLenum _filter, bool _mipmaps )
{
	TexturePtr tex = new Texture( TEX_2D );

	//-- set up filtering
    GLenum filter = _filter;
    if(_mipmaps)
	{
		if( filter == GL_NEAREST )
			filter = GL_NEAREST_MIPMAP_NEAREST;
		else if( filter == GL_LINEAR )
			filter = GL_NEAREST_MIPMAP_LINEAR;
	}
	tex->SetFiltering( filter );
	tex->SetWrap( GL_CLAMP_TO_EDGE );

	GLenum format = GL_RGBA;
	if( _internalFormat == GL_DEPTH_COMPONENT )
		format = GL_DEPTH_COMPONENT;

	tex->Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _resX, _resY, 0, format, _dataType, NULL);

	if(_mipmaps)
		glGenerateMipmap(GL_TEXTURE_2D);

    //add created textures into cache
    Add( _name, tex );

	tex->Unbind();

	return tex->GetID();
}

//-----------------------------------------------------------------------------
	
GLuint TextureCache::Create2DArrayManual( 
	const char* _name, 
	int _resX, int _resY,
	int _numLayers,
	GLint _internalFormat,
	GLenum _dataType, 
	GLenum _filter,
	bool _mipmaps
	)
{
	TexturePtr tex = new Texture( TEX_2D_ARRAY );

	//-- set up filtering
    GLenum filter = _filter;
    if(_mipmaps)
	{
		if( filter == GL_NEAREST )
			filter = GL_NEAREST_MIPMAP_NEAREST;
		else if( filter == GL_LINEAR )
			filter = GL_NEAREST_MIPMAP_LINEAR;
	}
	tex->SetFiltering( filter );
	//-- set up warping
	tex->SetWrap( GL_CLAMP_TO_EDGE );

	GLenum format = GL_RGBA;
	if( _internalFormat == GL_DEPTH_COMPONENT )
		format = GL_DEPTH_COMPONENT;

	tex->Bind();
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, _internalFormat, _resX, _resY, _numLayers, 0, format, _dataType, NULL);

	if(_mipmaps)
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    //add created textures into cache
    Add( _name, tex );

	tex->Unbind();

	return tex->GetID();
}

//-----------------------------------------------------------------------------

GLuint TextureCache::CreateFromImage( const char* _name, const char* _file )
{
	//FIXME: tohle nejak nastavit z venku
	bool mipmap = true;
	bool aniso = true;

	TexturePtr tex = new Texture( TEX_2D );

	//-- set up filtering
    GLenum filter = GL_LINEAR_MIPMAP_LINEAR;
	tex->SetFiltering( filter );
	//-- set up warping
	tex->SetWrap( GL_REPEAT );

	GLuint image_id = LoadImage( _file );

	if( !image_id )
		return 0;

	tex->Bind();
	//texture with anisotropic filtering
	if(aniso)
	{
		//find out, if GFX supports aniso filtering
		if(!GLEW_EXT_texture_filter_anisotropic)
		{
			cout<<"Anisotropic filtering not supported. Using linear instead.\n";
		}
		else
		{
			float maxAnisotropy;
			//find out maximum supported anisotropy
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
		}
	}
	//Get image attributes
	int width, height, bpp;
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);
	bpp = ilGetInteger(IL_IMAGE_BPP);

	if((width <= 0) || (height <= 0) )
	{
		ShowMessage("Unknown image type!");
		return false;
	}

	//Allocate memory for image
	unsigned imageSize = (bpp * width * height);
	GLubyte* imageData = new GLubyte[imageSize];

	if(imageData == NULL)
	{
		ShowMessage("Can't allocate image data!");
		return false;
	}

	//Copy pixels
	if(bpp==1)
		ilCopyPixels(0, 0, 0, width, height, 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, imageData);
	else if(bpp==3)
		ilCopyPixels(0, 0, 0, width, height, 1, IL_RGB, IL_UNSIGNED_BYTE, imageData);
	else if(bpp==4)
		ilCopyPixels(0, 0, 0, width, height, 1, IL_RGBA, IL_UNSIGNED_BYTE, imageData);

	if(tex->GetType() == BUMP)	//don't compress bump maps
#ifdef _LINUX_
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_BGR, GL_UNSIGNED_BYTE, imageData);
#else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
#endif
	else					//compress color maps
#ifdef _LINUX_
		glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, m_width, m_height, 0, GL_BGR, GL_UNSIGNED_BYTE, imageData);
#else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
#endif
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	if(mipmap)
		glGenerateMipmap(GL_TEXTURE_2D);

	//Unbind and free
	tex->Unbind();
	ilBindImage( 0 );
	ilDeleteImage( image_id );
	delete [] imageData;

	cout<<"Image loaded: "<< _file <<"\n";

	return tex->GetID();
}

//-----------------------------------------------------------------------------

GLuint TextureCache::LoadImage( const char* _filename )
{
	if(!_filename)
		return 0;

	//Opens image
	if(!m_is_IL_Initialized)
	{
		ilInit();
		m_is_IL_Initialized = true;
	}

	ILuint id = 0;
	ilGenImages(1, &id);
	ilBindImage(id);

	if(!ilLoadImage(_filename))
	{
		ShowMessage("Cannot open texture file!");
		return 0;
	}

	return id;
}

//-----------------------------------------------------------------------------

void TextureCache::Add( const char* _name, TexturePtr _tex )
{
	if( m_textureCache.find(_name) != m_textureCache.end() )
	{
		cerr<<"WARNING (TextureCache): texture with name "<<_name<<" already exist\n";
		return;
	}

	m_textureCache[_name] = _tex;
}

//-----------------------------------------------------------------------------

GLuint TextureCache::Get( const char* _name )
{
	if( m_textureCache.find(_name) == m_textureCache.end() )
	{
		cerr<<"WARNING (TextureCache): no texture with name "<<_name<<"\n";
		return 0;
	}

	//FIXME: muze vracet blbosti
	return m_textureCache[_name]->GetID();
}

//-----------------------------------------------------------------------------

TexturePtr TextureCache::GetPtr( const char* _name )
{
	if( m_textureCache.find(_name) == m_textureCache.end() )
	{
		cerr<<"WARNING (TextureCache): no texture with name "<<_name<<"\n";
	}

	//FIXME: muze vracet blbosti
	return m_textureCache[_name];
}

//-----------------------------------------------------------------------------