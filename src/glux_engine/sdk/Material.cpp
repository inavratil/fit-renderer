#include "Material.h"

//-----------------------------------------------------------------------------

Material::Material( const char* _name )
{
	_Init( _name, -1 );
}

//-----------------------------------------------------------------------------

Material::Material( const char* _name, int _id )
{
	_Init( _name, _id );
}

//-----------------------------------------------------------------------------

Material::~Material(void)
{
	//-- free textures
	//for(m_it_textures = m_textures.begin(); m_it_textures != m_textures.end(); m_it_textures++)
	//	if( m_it_textures->second )
	//		delete m_it_textures->second;
	if(m_program != 0)
    {
        glDetachObjectARB(m_program,m_f_shader);
        glDetachObjectARB(m_program,m_g_shader);
        glDetachObjectARB(m_program,m_v_shader);
        glDeleteObjectARB(m_program);
    }
}

//-----------------------------------------------------------------------------

void Material::_Init( const char* _name, int _id )
{
	m_name = _name;
	m_id = _id;

	m_alpha_texture = NULL;
	m_has_tessellation_shader = false;
	m_baked = false;

	m_f_source = m_tc_source = m_te_source = m_g_source = m_v_source = "";
}

//-----------------------------------------------------------------------------

/**
****************************************************************************************************
@brief Finds and returns next free texture name in list (important to synchronize generated texture names
to them stored in GeometryMaterial::textures field)
@param texname suffix, texture type (like Base, Bump, Env...) 
@return new texture name
***************************************************************************************************/
string Material::NextTexture( string _texname )
{
    int i = 1;
    for(m_it_textures = m_textures.begin(); m_it_textures != m_textures.end(); ++m_it_textures)
    {
        ///find first empty texture, add index to m_it_textures's name and return this name
        if(_texname == m_it_textures->first || m_it_textures->first.empty() )
        {
            string rep;
            rep = num2str(char('A' + i));
            _texname.replace(_texname.length()-1, rep.size(), rep.c_str());
            i++;
        }
    }
    return _texname;
}

//-----------------------------------------------------------------------------

GLuint Material::AddTexture( TexturePtr _tex, const char* _texname )
{
	//-- 1. generate new texture name (material name + texture mode (base, env, bump...) using GeometryMaterial::NextTexture()
    string texname = _texname;
	
	if( texname.empty() )
	{
		switch(_tex->GetType())
		{
			//base map
		case BASE: texname = NextTexture(m_name + "_Base_A"); break;
			//alpha map
		case ALPHA: texname = NextTexture(m_name + "_Alpha_A"); break;
			//environment map
		case ENV: texname = NextTexture(m_name + "_Env_A"); break;
			//bump map
		case BUMP: texname = NextTexture(m_name + "_Bump_A"); break;
			//parallax map
		case PARALLAX: texname = NextTexture(m_name + "_Parallax_A"); break;   
			//displacement map
		case DISPLACE: texname = NextTexture(m_name + "_Displace_A"); break; 
			//cube map
		case CUBEMAP: texname = NextTexture(m_name + "_Cube_A"); break; 
			//environment cube map
		case CUBEMAP_ENV: texname = m_name + "_CubeEnv_A"; break;//NextTexture(m_name + "CubeEnvA"); break; 
			//render textures
		case RENDER_TEXTURE: 
		case RENDER_TEXTURE_MULTISAMPLE: 
			texname = _tex->GetName(); 
			break; 
		}
	}
	if(_tex->GetType() == ALPHA) 
		m_alpha_texture = _tex; 

	m_textures[texname] = _tex;
	return _tex->GetID();
}

//-----------------------------------------------------------------------------

/**
@brief Delete texture specified by name
@param texname texture name
***************************************/
void  Material::DeleteTexture( const char *_texName )
{ 
	m_textures.erase(_texName); 
}

//-----------------------------------------------------------------------------

/**
****************************************************************************************************
@brief Load custom shader from file
@param source shader source file 
@return string with shader source
***************************************************************************************************/
string Material::LoadShader( const char* _filename )
{
    ifstream fin(_filename);
    if(!fin) 
    {
        string msg = "Cannot open shader ";
        msg += _filename;
        ShowMessage(msg.c_str(), false);
		throw ERR;
        //return "null";
    }
    string data;
    char ch;
    while(fin.get(ch))
        data+=ch;

    return data;
}

//-----------------------------------------------------------------------------

int Material::RenderMaterial()
{
	#ifdef VERBOSE
    cout<<"Rendering "<<m_name;
	#endif
	if( !m_baked ) BakeMaterial( 1 );

    ///enable shader
    glUseProgram(m_program);

    ///activate textures attached to material (Texture::ActivateTexture() )
	int tex_unit = 0;
	for( m_it_texture_locations = m_texture_locations.begin();
		 m_it_texture_locations != m_texture_locations.end();
		 m_it_texture_locations++ )
	{
		GLint location = m_it_texture_locations->second;
		if(location >= 0)
		{
			///2. activate and bind texture
			glActiveTexture(GL_TEXTURE0 + tex_unit);
			//texture location must be updated regularly
			glUniform1i(location, tex_unit++);

			m_textures[m_it_texture_locations->first]->Bind(); 
		}
	}


	return tex_unit;
}

//-----------------------------------------------------------------------------

GLuint Material::BuildShader( GLuint _program, GLenum _shader_type )
{
	string source;
	GLenum type;
	GLuint shader;

	switch( _shader_type )
	{
	case FRAG_SHADER: 
		source = m_f_source; 
		type = GL_FRAGMENT_SHADER;
		break;
	case VERT_SHADER: 
		source = m_v_source; 
		type = GL_VERTEX_SHADER;
		break;
	case GEOM_SHADER: 
		source = m_g_source; 
		type = GL_GEOMETRY_SHADER;
		break;
	case TC_SHADER: 
		source = m_tc_source; 
		type = GL_TESS_CONTROL_SHADER;
		break;
	case TE_SHADER: 
		source = m_te_source; 
		type = GL_TESS_EVALUATION_SHADER;
		break;
	default: 
		return 0;
	}
	//-- check if shaders are loaded correctly (i.e. if file exist)
	if( source == "null" )
        return 0;
	//-- check OpenGL 4 support (when tessellation shaders present)
	if( (_shader_type == TC_SHADER || _shader_type == TE_SHADER) && !GLEW_ARB_gpu_shader5)
    {
        cerr << "WARNING: OpenGL 4 not supported, can't use tessellation shaders!\n";
        return 0;
    }

	//-- shader version. If OpenGL4 is supported, use 400, else 330
    string version = "#version 330 compatibility\n";
    if(GLEW_ARB_gpu_shader5)
        version = "#version 400 core\n";

	if( _shader_type == TC_SHADER || _shader_type == TE_SHADER )
		m_has_tessellation_shader = true;
	 
    //then load, create, compile and attach tessellation shaders
    shader = glCreateShader(type);

    //load and link shader
	source = version + source;
    const char *s = source.c_str();
  
    glShaderSource(shader, 1, &s, NULL);
    glCompileShader(shader);
    glAttachShader(_program,shader);

	return shader;
}

//-----------------------------------------------------------------------------

GLuint Material::BuildProgram()
{
	m_program = glCreateProgram();

	m_v_shader = BuildShader( m_program, VERT_SHADER );
	m_f_shader = BuildShader( m_program, FRAG_SHADER );

	if( !m_tc_source.empty() && !m_te_source.empty() )
	{
		m_tc_shader = BuildShader( m_program, TC_SHADER );
		m_te_shader = BuildShader( m_program, TE_SHADER );
	}
	if( !m_g_source.empty() )
		m_g_shader = BuildShader( m_program, GEOM_SHADER );

	glLinkProgram( m_program );

	return m_program;

}

//-----------------------------------------------------------------------------

bool Material::CheckShaderStatus()
{
	if( !m_program ) return false;
	
	glUseProgram( m_program );

	bool compile_err = false;
	char log[BUFFER]; 
	int len;

    //shader creation status: print error if any
    ofstream fout("shader_log.txt", ios_base::app);

    glGetShaderInfoLog(m_v_shader, BUFFER, &len, log);
    
    //log from vertex shader
    if(strstr(log, "succes") == NULL && len > 0) 
    {
        fout<<endl<<m_name<<":"<<log;  
        cout<<endl<<m_name<<":"<<log;  
        compile_err = true;
    }
    //log from tessellation shaders
	if( !m_tc_source.empty() && !m_te_source.empty())
    {
        glGetShaderInfoLog(m_tc_shader, BUFFER, &len, log);
        if(strstr(log, "succes") == NULL && len > 0) 
        {
            fout<<endl<<m_name<<":"<<log;  
            cout<<endl<<m_name<<":"<<log;
            compile_err = true;
        }
        glGetShaderInfoLog(m_te_shader, BUFFER, &len, log);
        if(strstr(log, "succes") == NULL && len > 0) 
        {
            fout<<endl<<m_name<<":"<<log;  
            cout<<endl<<m_name<<":"<<log;
            compile_err = true;
        }
    }
    //log from geometry shader
    if( !m_g_source.empty() )
    {
        glGetShaderInfoLog(m_g_shader, BUFFER, &len, log);
        if(strstr(log, "succes") == NULL && len > 0) 
        {
            fout<<endl<<m_name<<":"<<log;  
            cout<<endl<<m_name<<":"<<log;
            compile_err = true;
        }
    }
    //log from fragment shader
    glGetShaderInfoLog(m_f_shader, BUFFER, &len, log);
    if(strstr(log, "succes") == NULL && len > 0) 
    {
        fout<<endl<<m_name<<":"<<log;   
        cout<<endl<<m_name<<":"<<log;
        compile_err = true;
    }
    fout.close();

	glUseProgram( 0 );

	return !compile_err;
}

//-----------------------------------------------------------------------------

/**
****************************************************************************************************
@brief Activates texture map for use by shader
@param tex_unit multitexture unit used for texture application
@param set_uniforms shall we also set uniform locations to shader?
****************************************************************************************************/
void Material::ActivateTextures( bool _set_uniforms )
{
	///1. set uniform values in shader (tiles, texture unit, texture matrix and intensity)
	//if(set_uniforms)
	//{
	//	if(m_tileXLoc > 0 && m_tileYLoc > 0)
	//	{
	//		glUniform1f(m_tileXLoc, m_tileX);
	//		glUniform1f(m_tileYLoc, m_tileY);
	//	}
	//	glUniform1f(m_intensityLoc, m_intensity);
	//}

	int tex_unit = 0;
	for( m_it_texture_locations = m_texture_locations.begin();
		 m_it_texture_locations != m_texture_locations.end();
		 m_it_texture_locations++ )
	{
		GLint location = m_it_texture_locations->second;
		if(location >= 0)
		{
			///2. activate and bind texture
			glActiveTexture(GL_TEXTURE0 + tex_unit);
			//texture location must be updated regularly
			glUniform1i(location, tex_unit++);

			m_textures[m_it_texture_locations->first]->Bind(); 
		}
	}

	////Various texture targets
	//if(m_textype == CUBEMAP || m_textype == CUBEMAP_ENV)        //for cube map
	//	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texID);
	//else if(m_textype == SHADOW_OMNI)                         //for texture array
	//	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texID);
	//else if(m_textype == RENDER_TEXTURE_MULTISAMPLE)          //for multisampled texture
	//	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_texID);
	//else                                                    //for regular 2D texture
	//	glBindTexture(GL_TEXTURE_2D, m_texID);
}

//-----------------------------------------------------------------------------

void Material::DectivateTextures()
{
	int tex_unit = 0;
	for( m_it_texture_locations = m_texture_locations.begin();
		 m_it_texture_locations != m_texture_locations.end();
		 m_it_texture_locations++ )
	{
		GLint location = m_it_texture_locations->second;
		if(location >= 0)
		{
			///2. activate and bind texture
			glActiveTexture(GL_TEXTURE0 + tex_unit);
			m_textures[m_it_texture_locations->first]->Unbind(); 
		}
	}
}