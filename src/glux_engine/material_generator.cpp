/**
@file material_generator.cpp
@brief dynamic shader creation
***************************************************************************************************/
#include "material.h"
#include "utils.hpp"

/**
****************************************************************************************************
@brief Load shader function from file
@param func shader function source file 
@return string with shader function source
***************************************************************************************************/
string LoadFunc(char* func)
{
    string file;
    file += "data/shaders/func/";
    file += func;
    file += ".frag";
    ifstream fin(file.c_str());
    if(!fin) 
        return "null";
    string data;
    char ch;
    while(fin.get(ch))
        data+=ch;

    return data;
}


////////////////////////////////////////////////////////////////////////////////
//************************* TMaterial methods ********************************//
////////////////////////////////////////////////////////////////////////////////

/**
****************************************************************************************************
@brief Computes texel color accordig to texture mode
@param t pointer to texture data
@param name texture name
@return shader piece of code
***************************************************************************************************/
string computeTexel(Texture *t, string name)
{
    string ret = "\n";

    //environment mapping - we need function to generate reflection coordinates
    if(t->GetType() == ENV) 
        ret += "  " + name + "_texture = envMapping(color,eyeVec,normal," + name + ", " + name + "_intensity);\n";
    //cube map - use samplerCube
    else if(t->GetType() == CUBEMAP) 
        ret += "  " + name + "_texture = textureCube(" + name + ", " + name + "_cubeCoords);\n";
    //cube map - use reflection vector
    else if(t->GetType() == CUBEMAP_ENV) 
        ret += "  " + name + "_texture = textureCube(" + name + ", reflVec) * " + name + "_intensity;\n";
    //default 2D texture
    else 
        ret += "  " + name + "_texture = texture(" + name + ", " + name + "_texcoord);\n";

    switch(t->GetMode())
    {
        ///ADD mode: add texel RGB values to color RGB values. Alpha channels are multiplied
    case ADD:
        ret +=
            "  //" + name + ", ADD mode\n"
            "  color.rgb += " + name + "_texture.rgb;\n"
            "  color.a *= " + name + "_texture.a;\n"
            "  color = clamp (color, 0.0, 1.0);\n";
        break;
        ///MODULATE mode: multiply texel channels with actual color channels
    case MODULATE:
        ret +=
            "  //" + name + ", MODULATE mode\n"
            "  color *= " + name + "_texture;\n";
        break;
        ///DECAL mode: mix texel and color together
    case DECAL:
        ret +=
            "  //" + name + ", DECAL mode\n"
            "  color = vec4(mix(color.rgb, " + name + "_texture.rgb, " + name + "_texture.a), color.a);\n";
        break;
        ///BLEND mode: mix texel and color together, multiply alpha channels
    case BLEND:
        ret +=
            "  //" + name + ", BLEND mode\n"
            "  color = vec4 ( vec3(mix(color.rgb, gl_TextureEnvColor[0].rgb, " + name + "_texture.rgb)), color.a * " + name + "_texture.a);\n";
        break;
        ///REPLACE mode: replace fragment color directly with texel color
    case REPLACE:
        ret +=
            "  //" + name + ", REPLACE mode\n"
            "  color = " + name + "_texture;\n";
        break;          
    };
    return ret;
}

/**
****************************************************************************************************
@brief Dynamically generates shader from all material data (if hasn't been generated before)
@param light_count light count in scene
@param parabola_cut should we use paraboloid cutting in DPSM?
@return success/fail of shader generation
***************************************************************************************************/
bool TMaterial::BakeMaterial(int light_count, int dpshadow_method, bool use_pcf)
{
    //dont't bake linked custom shader
    if(m_custom_shader)
        return true;

    cout<<"Baking material "<<m_name<<endl;

    string tmp = m_name;        //temporary string for comparison

    /////////////////////////////////////////////////////////////////////////////
    ///1 CREATION OF VERTEX SHADER
    /////////////////////////////////////////////////////////////////////////////

    //shader version. If OpenGL4 is supported, use 400, else 330
    string version = "330";
    if(GLEW_ARB_gpu_shader5)
        version = "400";

    ///1.1 Vertex shader variables
    string vert_vars = "//GLSL vertex shader generated by gluxEngine\n";    //vertex shader variables
    vert_vars +=    "//Material: \"" + m_name + "\"\n\n"
        "//generic vertex attributes\n"
        "layout(location = 0) in vec3 in_Vertex;\n"
        "layout(location = 1) in vec3 in_Normal;\n"
        "layout(location = 2) in vec2 in_Coord;\n\n"
        "//modelview matrix\n"
        "uniform mat4 in_ModelViewMatrix;\n\n"
        "//projection and shadow matrices\n"
        "layout(std140) uniform Matrices{\n"
        "  mat4 in_ProjectionMatrix;\n";

    //add also shadow matrices into uniform block (for each light). Only if shadow map is bound to material
    for(m_it = m_textures.begin(); m_it != m_textures.end(); ++m_it)
        if(m_it->second->GetType() == SHADOW)
            vert_vars += "  mat4 " + m_it->first + "_texMatrix;\n";

    //finish uniform block and add texture coordinates
    vert_vars +=    "};\n"
        "//texture coordinate\n"
        "out vec2 fragTexCoord;\n"
        "//depth of vertex\n"
        "out float v_depth;\n";

    string vert_func;                                                       //vertex shader functions
    //vertex shader main function
    string vert_main = 
        "\nvoid main()\n"
        "{\n"
        "  vec4 vertex = vec4(in_Vertex,1.0);\n"
        "  vec3 dNormal = in_Normal;\n";	//vertices and normals for further calculations 

    ///1.1.1 if we have displacement texture, modify vertex position and normal
    bool displace = false;
    for(m_it = m_textures.begin(); m_it != m_textures.end(); ++m_it)
    {
        if(m_it->second->GetType() == DISPLACE)
        {
            string texname = m_it->second->GetName();
            //add texture samplers: for normal and displacement map (assume that normal map is present)
            if(m_it->second->HasTiles())
                vert_vars += "uniform float " + texname + "_tileX, " + texname + "_tileY;\n";
            vert_vars += "uniform float " + texname + "_intensity;\n"
                "uniform sampler2D "+ texname + ";\n";
            vert_main +=  "  //displace vertex\n";
            //tiles: tile texture coordinate
            if(m_it->second->HasTiles())
                vert_main += "  vec4 dv = textureLod(" + texname + ", in_Coord*vec2(" + texname + "_tileX, " + texname + "_tileY), 0.0);\n";
            //no tiles
            else
                vert_main += "  vec4 dv = textureLod(" + texname + ", in_Coord, 0.0);\n";
            //displace vertex along normal
            vert_main += "  float displace = dv.r; //dot( vec4(0.30,0.59,0.11,0.0),dv);\n"
                "  vertex.xyz += " + texname + "_intensity * displace * in_Normal;\n";
            displace = true;
            break;
        }
    }
    //require also normal map
    if(displace)
    {
        for(m_it = m_textures.begin(); m_it != m_textures.end(); ++m_it)
        {
            if(m_it->second->GetType() == BUMP)
            {
                string texname = m_it->second->GetName();
                if(m_it->second->HasTiles())
                    vert_vars += "uniform float " + texname + "_tileX, " + texname + "_tileY;\n";
                vert_vars += "uniform float " + texname + "_intensity;\n"
                    "uniform sampler2D "+ texname + ";\n";
                vert_main	+= "  //displace normal using normal map\n";
                //tiles: tile texture coordinate
                if(m_it->second->HasTiles())
                    vert_main	+= "  dNormal += normalize(texture(" + texname + ",in_Coord*vec2(" + texname + "_tileX, " + texname + "_tileY)).rgb * 2.0 - 1.0);\n\n";
                //no tiles
                else
                    vert_main	+= "  dNormal += normalize(texture(" + texname + ",in_Coord).rgb * 2.0 - 1.0);\n\n";
                break;
            }
        }
    }


    //*****************************
    ///1.2 Light model: PHONG or GOURAUD

    tmp += "Env";
    ///1.2.1 if PHONG, light calculation will be done in fragment shader, so send necessary variables to it (also required for env mapping)
    if(m_lightModel == PHONG || m_textures.find(tmp) != m_textures.end() )
    {
        vert_vars += "out vec3 normal, eyeVec;\n";  //varying variables
        vert_main +=
            "  normal = mat3(in_ModelViewMatrix) * dNormal;  //surface normal\n"
            "  eyeVec = -vec3(in_ModelViewMatrix * vertex);   //eyeview vector\n";
    }

    ///1.2.3 if GOURAUD, done light calculation per-vertex
    else if(m_lightModel == GOURAUD)
    {
        vert_vars += "out vec4 v_color;\n"
            "out vec3 normal, eyeVec;\n";

        vert_func += LoadFunc("light");
        vert_main +=
            "  normal = mat3(in_ModelViewMatrix) * dNormal;  //surface normal\n"
            "  eyeVec = -vec3(in_ModelViewMatrix * vertex);            //eyeview vector\n"
            "  v_color = LightModel(normal, eyeVec);                   //calculate light model\n";
    }
    //no light model
    else
    {
        vert_vars += "out vec3 normal;\n";
        vert_main += "  normal = mat3(in_ModelViewMatrix) * dNormal;  //surface normal\n";
    }

    //**********************************
    //Textures

    ///1.3 send texture coords to fragment shader
    vert_main += "  fragTexCoord = in_Coord;\n";


    //******************************
    //iterate through textures
    for(m_it = m_textures.begin(); m_it != m_textures.end(); ++m_it)
    {
        ///1.4 calculate shadow matrices for frag. shader (projected shadow and texture matrix)
        if(m_it->second->GetType() == SHADOW)
        {
            vert_vars += "out vec4 " + m_it->first + "_projShadow;\n";
            vert_main += "  " + m_it->first + "_projShadow = " + m_it->first + "_texMatrix * in_ModelViewMatrix * vertex;   //calculate shadow texture projection\n";
        }
        //dual-paraboloid shadow map - insert output vertex (only once)
        if(m_it->second->GetType() == SHADOW_OMNI && m_it->first.find("ShadowOMNI_A") != string::npos)
        {
			
			//vert_func += LoadFunc("shadow_warpdpsm");

            vert_vars += "out vec4 o_vertex;\n";
            vert_main += "  o_vertex = vertex;   //vertex object-space position\n";
        }
        ///1.5 send 3D coordinates for cube map
        if(m_it->second->GetType() == CUBEMAP)
        {
            vert_vars += "out vec3 " + m_it->first + "_cubeCoords;\n";
            vert_main += "  " + m_it->first + "_cubeCoords = vertex.xyz;\n";
        }

        ///1.6 send 3D coordinates for environment cube map
        if(m_it->second->GetType() == CUBEMAP_ENV)
        {
            vert_vars += "out vec3 r_normal, r_eyeVec;\n";
            vert_main += 
                "\n  //Camera rotation can be found in transposed modelview matrix\n"
                "   mat3 TFreelookCamerarot;\n"
                "   TFreelookCamerarot[0][0] = in_ModelViewMatrix[0][0];\n"
                "   TFreelookCamerarot[0][1] = in_ModelViewMatrix[1][0];\n"
                "   TFreelookCamerarot[0][2] = in_ModelViewMatrix[2][0];\n"
                "   TFreelookCamerarot[1][0] = in_ModelViewMatrix[0][1];\n"
                "   TFreelookCamerarot[1][1] = in_ModelViewMatrix[1][1];\n"
                "   TFreelookCamerarot[1][2] = in_ModelViewMatrix[2][1];\n"
                "   TFreelookCamerarot[2][0] = in_ModelViewMatrix[0][2];\n"
                "   TFreelookCamerarot[2][1] = in_ModelViewMatrix[1][2];\n"
                "   TFreelookCamerarot[2][2] = in_ModelViewMatrix[2][2];\n\n"
                "\n  //Coordinates for cubemap reflexion\n"
                "  r_normal = TFreelookCamerarot * (in_ModelViewMatrix * vec4(dNormal,0.0)).xyz;\n"
                "  r_eyeVec = TFreelookCamerarot * (in_ModelViewMatrix * vertex).xyz;\n\n";
        }
    }

    //**********************************************
    ///1.5 Finalize, vertex transform and save depth
    vert_main +=
        "  vec4 viewPos = in_ModelViewMatrix * vertex;       //vertex transform\n"
        "  v_depth = -viewPos.z;                             //store depth\n"
        "  gl_Position = in_ProjectionMatrix * viewPos;      //projection transform\n"     
        "}\n"
        "\n";

    string vertex_shader = "#version " + version + " compatibility\n";
    vertex_shader += vert_vars + vert_func + vert_main;


    ////////////////////////////////////////////////////////////////////////////
    ///3 CREATION OF FRAGMENT SHADER
    ////////////////////////////////////////////////////////////////////////////

    ///3.1 fragment shader variables
    string frag_vars = "//GLSL fragment shader generated by gluxEngine\n\n";
    frag_vars +=    "//Material: \"" + m_name + "\"\n\n"
        "const int LIGHTS = " + num2str(light_count) + ";\n\n"      //scene light count
        "//texture coordinate\n"
        "in vec2 fragTexCoord;\n"
        "//fragment depth in world space\n"
        "in float v_depth;\n";

    //fragment shader output
    if(m_useMRT)
        frag_vars += "out vec4 out_FragData[2];\n\n";
    else
        frag_vars += "out vec4 out_FragColor;\n\n";

    string frag_func;                                                         //fragment shader functions
    string frag_main = "\nvoid main()\n{\n";                                  //fragment shader main function

    bool bump = false;      //do we have bump map present?


    //**********************************
    ///3.2 Texture pre-setup: create texture samplers for every texture (shadow maps use shadow samplers), texture tiles and coordinates
    frag_main += "  vec2 texCoord = fragTexCoord;\n";

    //for parallax/depth map, we modify texture coordinate
    for(m_it = m_textures.begin(); m_it != m_textures.end(); ++m_it)
    {    
        if(m_it->second->GetType() == PARALLAX)
        {
            string offset;
            offset = num2str(m_it->second->GetIntensity());  //parallax offset value
            frag_main +=
                "  //simple parallax mapping, modify texture coordinates\n"
                "  float height = texture(" + m_name + "ParallaxA,  texCoord).r;\n"
                "  float offset = " + offset + " * (2.0 * height - 1.0);\n"
                "  texCoord = texCoord + eyeVec.xy * offset;\n\n";
            break;
        }
    }

    for(m_it = m_textures.begin(); m_it != m_textures.end(); ++m_it)
    {
        if(!m_it->second->Empty() && m_it->second->GetType() != SHADOW && m_it->second->GetType() != SHADOW_OMNI && m_it->second->GetType() != DISPLACE )
        {
            frag_main += "\n  vec4 " + m_it->first + "_texture;\n";

            //cube map, we add different sampler + 3D coordinates
            if(m_it->second->GetType() == CUBEMAP) 
                frag_vars += "uniform samplerCube "+ m_it->first + ";\nin vec3 " + m_it->first + "_cubeCoords;\n\n";
            //environment cube map, we need cube sampler and normal + view vector
            else if(m_it->second->GetType() == CUBEMAP_ENV) 
            {
                frag_vars += "uniform samplerCube "+ m_it->first + ";\n"
                    "uniform float " + m_it->first + "_intensity;\n"
                    "in vec3 r_normal, r_eyeVec;\n\n";
            }
            //regular 2D texture sampler
            else 
            {
                //texture intensity
                frag_vars += "uniform float " + m_it->first + "_intensity;\n";
                //texture tiles (if set)
                if(m_it->second->HasTiles())
                {
                    frag_vars += "uniform float " + m_it->first + "_tileX," + m_it->first + "_tileY;\n";
                    frag_main +=
                        "  //" + m_it->first + ", texture tiles\n"
                        "  vec2 " + m_it->first + "_texcoord = texCoord * vec2(" + m_it->first + "_tileX, " + m_it->first + "_tileY) ;\n";
                }
                //no tiles
                else
                    frag_main += "  vec2 " + m_it->first + "_texcoord = texCoord;\n";

                frag_vars += "uniform sampler2D "+ m_it->first + ";\n";
            }
        }
    }


    //**********************************
    ///3.3.1 if light model is set to PHONG, calculate lighting by using varying variables sent from vertex shader (normal, eye vector...)
    if(m_lightModel == PHONG)    //PHONG, per-pixel
    {  
        frag_vars +=  "in vec3 normal, eyeVec;\n";
        frag_func += LoadFunc("light");

        ///3.3.1.1 if is present bump texture, modify normal (bump mapping)
        tmp = m_name; tmp += "BumpA";
        if( m_textures.find(tmp) != m_textures.end() )
        {
            //bump = true;
            //frag_main +=
            //    "  //bump-mapping\n"
            //    "  vec3 bump_normal = normal + normalize(texture(" + m_name + "BumpA, " + tmp + "_texcoord).xyz*2.0 - " + m_name + "BumpA_intensity);\n"
            //    "  vec4 color = LightModel(bump_normal,eyeVec);\n";   
            frag_main +="  vec4 color = LightModel(normal,eyeVec);\n";
        }
        else
            frag_main +="  vec4 color = LightModel(normal,eyeVec);\n";
    }
    ///3.3.2 if GOURAUD model is set, only get color value from vertex shader
    else if(m_lightModel == GOURAUD) 
    {
        frag_vars += "in vec4 v_color;\n"
            "in vec3 normal;\n";
        frag_main += "  vec4 color = v_color;\n";
    }
    ///3.3.3 else constant shading - only material diffuse is in computation
    else
    {
        frag_vars += "in vec3 normal;\n\n"
            "//material settings (only diffuse)\n"
            "struct Material{\n"
            "  vec3 diffuse;\n};\n"
            "uniform Material material;\n";
        frag_main += "  vec4 color = vec4(material.diffuse,0.0);\n";
    }


    //***************************************************
    ///3.4 texture application according to texture type
    string alpha_test = "";
    for(m_it = m_textures.begin(); m_it != m_textures.end(); ++m_it)
    {
        if(!m_it->second->Empty())
        {
            //skip displacement map
            if(m_it->second->GetType() == DISPLACE)
                continue;
            ///3.4.1 shadow maps - set shadow samplers and call function to project and create soft shadows
            else if(m_it->second->GetType() == SHADOW)
            {
                frag_vars += "in vec4 " + m_it->first + "_projShadow;\n";
                frag_vars +=
                    "uniform float " + m_it->first + "_intensity;\n"
                    "uniform sampler2DShadow " + m_it->first + ";\n";

                //insert shadow function (only once)
                if(m_it->first.find("ShadowA") != string::npos)
                    frag_func += LoadFunc("shadow");

                frag_main += "\n  //Shadow map projection\n"
                    "  color *= PCFShadow(" + m_it->first + "," + m_it->first + "_projShadow, " + m_it->first + "_intensity);\n";
            }
            else if(m_it->second->GetType() == SHADOW_OMNI)
            {
                frag_vars +=
                    "uniform float " + m_it->first + "_intensity;\n"
                    "uniform sampler2DArray " + m_it->first + ";\n";

                if(use_pcf)
                    frag_vars += "#define USE_PCF\n";

                //insert shadow function (only once)
                if(m_it->first.find("ShadowOMNI_A") != string::npos)
					if( dpshadow_method == DPSM )
						frag_func += LoadFunc("shadow_omni");

                frag_main += "\n  //Shadow map projection\n"
                    "  color *= ShadowOMNI(" + m_it->first + ", " + m_it->first + "_intensity);\n";
            }

            //other texture types
            else
            {
                //should we use alpha testing?
                if(m_it->second->GetType() == ALPHA)
                    alpha_test = "  //alpha test\n  if( all(lessThan(" + m_it->first + "_texture.rgb, vec3(0.25)))) discard;\n";

                ///3.4.2 environment maps - add normal and eye vector variables(if per-pixel)
                if(m_it->second->GetType() == ENV)
                {
                    //when we have more env maps, insert env function only once
                    if(m_it->first.find("EnvB") == string::npos)
                    {

                        if(m_lightModel != PHONG)
                            frag_vars += "in vec3 eyeVec;\n";
                        //add environment map computation
                        frag_func += LoadFunc("env");
                    }
                }

                //reflect fragment from environment cubemap
                if(m_it->second->GetType() == CUBEMAP_ENV)
                {
                    if(bump)    //if we have bump map, modify reflection vector to get bumpy reflections
                        frag_main += "  vec3 reflVec = reflect(normalize(r_eyeVec), normalize(bump_normal)); //bumped reflection vector for cubemap\n";
                    else
                        frag_main += "  vec3 reflVec = reflect(normalize(r_eyeVec), normalize(r_normal)); //reflection vector for cubemap\n";
                }

                //compute fragment color from texel(not for bump/parallax map)
                if(m_it->second->GetType() != BUMP && m_it->second->GetType() != PARALLAX)
                    frag_main += computeTexel(m_it->second,m_it->first);

            }
        }
    }


    //***********************************************
    ///3.5 Finalize (with transparency or alpha test)
    frag_main +=
        "\n  //FINAL fragment color\n";

    //do we use MRT?
    if(m_useMRT)
    {
        //is material transparent?
        if(m_transparency > 0.0)
        {
            string trans;
            trans = num2str(m_transparency);
            frag_main += 
                "  out_FragData[0] = vec4(vec3(color.rgb),  " + trans + ");\n"
                "  out_FragData[1].rgb = normal;    //normal;\n";
            frag_main +=
                    "  out_FragData[1].a = v_depth;    //depth\n";

        }
        //alpha test fragment
        else
        {
            frag_main += alpha_test + 
                "  out_FragData[0].rgb = color.rgb; //color;\n"
                "  out_FragData[1].rgb = normal;    //normal;\n";
            frag_main += "  out_FragData[1].a = v_depth;    //depth\n";
        }
    }
    else
    {
        //is material transparent?
        if(m_transparency > 0.0)
        {
            string trans;
            trans = num2str(m_transparency);
            frag_main += "  out_FragColor = vec4(vec3(color.rgb),  " + trans + ");\n";
        }
        else
        {
            //alpha test fragment
            frag_main += alpha_test + "  out_FragColor = color;\n";
        }
    }

    frag_main +=
        "}\n"
        "\n";
    string frag_shader = "#version " + version + " compatibility\n";
    frag_shader += frag_vars + frag_func + frag_main;

    //////////////////////////////////////////////////////////////////////////////


    //save shaders to debug file
    string file; file = "shader_out/" + m_name + ".vert";
    ofstream fout1(file.c_str());
    fout1<<vertex_shader;
    file = "shader_out/" + m_name + ".frag";
    ofstream fout2(file.c_str());
    fout2<<frag_shader;

    ///4 Create, compile and link shaders
    m_v_shader = glCreateShader(GL_VERTEX_SHADER);
    m_f_shader = glCreateShader(GL_FRAGMENT_SHADER);

    //copy shader data
    const char *ff = frag_shader.c_str();
    const char *vv = vertex_shader.c_str();

    //set shader source
    glShaderSource(m_v_shader, 1, &vv,NULL);
    glShaderSource(m_f_shader, 1, &ff,NULL);

    //compile and detect shader errors
    char log[BUFFER]; int len;
    glCompileShader(m_v_shader);
    glCompileShader(m_f_shader);

    //create and link shader program
    m_shader = glCreateProgram();
    glAttachShader(m_shader,m_f_shader);
    glAttachShader(m_shader,m_v_shader);

    glLinkProgram(m_shader);
    glUseProgram(m_shader);

    //shader creation status
    glGetShaderInfoLog(m_v_shader, BUFFER, &len, log);
    if(strstr(log, "succes") == NULL && len > 0) 
        cout<<endl<<m_name<<":"<<log;    //print error if any
    glGetShaderInfoLog(m_f_shader, BUFFER, &len, log);
    if(strstr(log, "succes") == NULL && len > 0) 
        cout<<endl<<m_name<<":"<<log;    //print error if any


    //*************************************
    ///4 Get uniform variables for textures (using Texture::GetUniforms() )
    int i=0;
    for(m_it = m_textures.begin(); m_it != m_textures.end(); ++m_it)
    {
        if(!m_it->second->Empty())
        {
            m_it->second->GetUniforms(m_shader);
            m_it->second->ActivateTexture(i,true);
            i++;
        }
    }

    ///set materials parameters
    SetUniform("material.ambient",m_ambColor);
    SetUniform("material.diffuse",m_diffColor);
    SetUniform("material.specular",m_specColor);
    SetUniform("material.shininess",m_shininess);

    m_baked = true;

    //setup uniform buffers
    GLint uniformIndex = glGetUniformBlockIndex(m_shader, "Matrices");
    if(uniformIndex >= 0)
        glUniformBlockBinding(m_shader, uniformIndex, UNIFORM_MATRICES);
    uniformIndex = glGetUniformBlockIndex(m_shader, "Lights");
    if(uniformIndex >= 0)
        glUniformBlockBinding(m_shader, uniformIndex, UNIFORM_LIGHTS);
    glUseProgram(0);

    //print_uniform_block_info(shader, uniformIndex);

    return true;
}
