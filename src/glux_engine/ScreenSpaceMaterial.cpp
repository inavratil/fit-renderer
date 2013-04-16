#include "ScreenSpaceMaterial.h"

//-----------------------------------------------------------------------------

ScreenSpaceMaterial::ScreenSpaceMaterial( const char* _name, const char* _vs, const char* _fs ) :
	Material( _name )
{
	    TShader vert( _vs, "" );
        TShader frag( _fs, "" );
        CustomShader(&vert, NULL, NULL, NULL, &frag);
}

//-----------------------------------------------------------------------------

ScreenSpaceMaterial::~ScreenSpaceMaterial(void)
{
}

//-----------------------------------------------------------------------------

void ScreenSpaceMaterial::_Init()
{
	m_is_screenspace = true;
}

//-----------------------------------------------------------------------------

/**
****************************************************************************************************
@brief Loads and sets custom shader from source file. Fragment and vertex shader are mandatory, 
geometry and tesselation shaders optional
@param vertex vertex shader sources
@param tess_control tesselation control shader sources
@param tess_eval tessellation evaluation shader sources
@param geometry geometry shader sources
@param fragment fragment shader sources
***************************************************************************************************/
bool ScreenSpaceMaterial::CustomShader(TShader *vertex, TShader *tess_control, TShader *tess_eval, TShader *geometry, TShader *fragment)
{
    //check OpenGL 4 support(when tessellation shaders present)
    m_has_tessellation_shader = false;
    if(tess_control != NULL && tess_eval != NULL && !GLEW_ARB_gpu_shader5)
    {
        cout<<"OpenGL 4 not supported, can't use tessellation!\n";
        return false;
    }

    //create shaders for vertex and fragment shader
    m_v_shader = glCreateShader(GL_VERTEX_SHADER);
    m_f_shader = glCreateShader(GL_FRAGMENT_SHADER);

    //shader version. If OpenGL4 is supported, use 400, else 330
    string version = "#version 330 compatibility\n";
    if(GLEW_ARB_gpu_shader5)
        version = "#version 400 core\n";

    //write shader header - version and defines
    string vertex_shader, fragment_shader, geometry_shader, tess_control_shader, tess_eval_shader;
    vertex_shader = version + vertex->defines;
    fragment_shader = version + fragment->defines;

    //load shader data
    vertex_shader += LoadShader(vertex->source.c_str());
    fragment_shader += LoadShader(fragment->source.c_str());

    if(vertex_shader == "null" || fragment_shader == "null")
        return false;

	//save shaders to debug file
	/*
    string file; file = "shader_out/" + m_name + ".vert";
    ofstream fout1(file.c_str());
    fout1<<vertex_shader;
    file = "shader_out/" + m_name + ".frag";
    ofstream fout2(file.c_str());
    fout2<<fragment_shader;
	*/

    const char *ff = fragment_shader.c_str();
    const char *vv = vertex_shader.c_str();

    //set shader source
    glShaderSource(m_v_shader, 1, &vv,NULL);
    glShaderSource(m_f_shader, 1, &ff,NULL);

    //compile and detect shader errors
    char log[BUFFER]; int len;
    glCompileShader(m_v_shader);
    glCompileShader(m_f_shader);

    //create and link shader program
    m_program = glCreateProgram();
    glAttachShader(m_program,m_f_shader);
    glAttachShader(m_program,m_v_shader);


    //do we have tessellation shaders?
    if(tess_control != NULL && tess_eval != NULL)
    {
        m_has_tessellation_shader = true;
        //then load, create, compile and attach tessellation shaders
        m_tc_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
        m_te_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
        string tess_control_shader = version + tess_control->defines;
        string tess_eval_shader = version + tess_eval->defines;

        //load and link shader
        tess_control_shader += LoadShader(tess_control->source.c_str());
        tess_eval_shader += LoadShader(tess_eval->source.c_str());
        if(tess_control_shader == "null" || tess_eval_shader == "null")
            return false;
        const char *tc = tess_control_shader.c_str();
        const char *te = tess_eval_shader.c_str();
        glShaderSource(m_tc_shader, 1, &tc, NULL);
        glShaderSource(m_te_shader, 1, &te, NULL);
        glCompileShader(m_tc_shader);
        glCompileShader(m_te_shader);
        glAttachShader(m_program,m_tc_shader);
        glAttachShader(m_program,m_te_shader);
    }

    //do we have geometry shader?
    if(geometry != NULL)
    {
        //then load, create, compile and attach geometry shader
        m_g_shader = glCreateShader(GL_GEOMETRY_SHADER);
        string geometry_shader = version + geometry->defines;

        //load and link shader
        geometry_shader += LoadShader(geometry->source.c_str());
        if(geometry_shader == "null")
            return false;
        const char *gg = geometry_shader.c_str();
        glShaderSource(m_g_shader, 1, &gg,NULL);
        glCompileShader(m_g_shader);
        glAttachShader(m_program,m_g_shader);
    }

    //final shader linking
    glLinkProgram(m_program);
    glUseProgram(m_program);

    //shader creation status: print error if any
    ofstream fout("shader_log.txt", ios_base::app);
    glGetShaderInfoLog(m_v_shader, BUFFER, &len, log);
    bool compile_err = false;
    //log from vertex shader
    if(strstr(log, "succes") == NULL && len > 0) 
    {
        fout<<endl<<m_name<<":"<<log;  
        cout<<endl<<m_name<<":"<<log;  
        compile_err = true;
    }
    //log from tessellation shaders
    if(tess_control != NULL && tess_eval != NULL)
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
    if(geometry != NULL)
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



    //*************************************
    ///4 Get uniform variables for textures (using Texture::GetUniforms() )
    int i=0;
    for(m_it_textures = m_textures.begin(); m_it_textures != m_textures.end(); ++m_it_textures)
    {
        if(!m_it_textures->second->Empty())
        {
            m_it_textures->second->GetUniforms(m_program);
            m_it_textures->second->ActivateTexture(i,true);
            i++;
        }
    }

    glUseProgram(0);

    //m_baked = true;
    //m_custom_shader = true;
    return !compile_err;
}
//-----------------------------------------------------------------------------