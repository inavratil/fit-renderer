/**
@file material_generator.cpp
@brief dynamic shader creation
***************************************************************************************************/
#include "tmaterial.h"
#include "utils.hpp"

#include "shaderGen/shader_generator.h"


/**
****************************************************************************************************
@brief Load shader function from file
@param func shader function source file 
@return string with shader function source
***************************************************************************************************/
string LoadFunc(char* func, char* type)
{
    string file;
    file += "data/shaders/func/";
    file += func;
    file += type;
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
	
	//shader version. If OpenGL4 is supported, use 400, else 330
	string version = "330";
    if(GLEW_ARB_gpu_shader5)
        version = "400";

    ////////////////////////////////////////////////////////////////////////////////
	//--
    //-- 1 CREATION OF VERTEX SHADER
	//--
    ////////////////////////////////////////////////////////////////////////////////

    string vert_vars = "//-- GLSL vertex shader generated by gluxEngine\n";    //-- Vertex shader variables
	string vert_func = "//-- Vertex shader functions\n\n";					//-- Vertex shader functions
	string vert_main = "\nvoid main()\n{\n";								//-- Vertex shader main function

	////////////////////////////////////////////////////////////////////////////////
    //-- 1.1 Vertex shader variables initializacion

	vert_vars +=    
		"//-- Material: \"" + m_name + "\"\n";

	//------------------------------------------------------------------------------
	//-- 1.1.1 Input vertex attributes

    vert_vars +=    
		"\n"
        "//-- generic vertex attributes\n"
        "layout(location = 0) in vec3 in_Vertex;\n"
        "layout(location = 1) in vec3 in_Normal;\n"
        "layout(location = 2) in vec2 in_Coord;\n"
		"\n";
	
	//------------------------------------------------------------------------------
	//-- 1.1.2 Input matrices - as a uniform variable or in uniform block

	vert_vars +=
        "//-- modelview matrix\n"
        "uniform mat4 in_ModelViewMatrix;\n"
		"\n"
        "//-- projection and shadow matrices\n"
        "layout(std140) uniform Matrices\n"
		"{\n"
        "  mat4 " + SG_MAT_PROJ + ";\n";

    //add also shadow matrices into uniform block (for each light). Only if shadow map is bound to material
    for(m_it_textures = m_textures.begin(); m_it_textures != m_textures.end(); ++m_it_textures)
        if(m_it_textures->second->GetType() == SHADOW)
            vert_vars += "  mat4 " + m_it_textures->first + "_texMatrix;\n";

    //finish uniform block
    vert_vars +=    
		"};\n";
	
	//------------------------------------------------------------------------------
	//-- 1.1.3 Output shader variables

	vert_vars +=
		"\n"
		"//-- Output variables\n"
        "out vec2 fragTexCoord;		//-- texture coordinate\n"
        "out float v_depth;			//-- depth of vertex\n";


	//------------------------------------------------------------------------------
    //-- 1.1.4 Initialize variables in main for further calculations

	vert_main += 
        "  vec4 " + SG_V4_VERTCOORD + "= vec4(in_Vertex,1.0);\n"
        "  vec3 dNormal = in_Normal;\n";	//vertices and normals for further calculations 

	////////////////////////////////////////////////////////////////////////////////
	//-- 1.2 Apply displacement mapping to vertex position and normal
    
    bool displace = false;
    for(m_it_textures = m_textures.begin(); m_it_textures != m_textures.end(); ++m_it_textures)
    {
        if(m_it_textures->second->GetType() == DISPLACE)
        {
            string texname = m_it_textures->second->GetName();
            //add texture samplers: for normal and displacement map (assume that normal map is present)
            if(m_it_textures->second->HasTiles())
                vert_vars += 
					"uniform float " + texname + "_tileX, " + texname + "_tileY;\n";
            vert_vars += 
				"uniform float " + texname + "_intensity;\n"
                "uniform sampler2D "+ texname + ";\n";
            vert_main +=  "  //displace vertex\n";
            //tiles: tile texture coordinate
            if(m_it_textures->second->HasTiles())
                vert_main += 
					"  vec4 dv = textureLod(" + texname + ", in_Coord*vec2(" + texname + "_tileX, " + texname + "_tileY), 0.0);\n";
            //no tiles
            else
                vert_main += 
					"  vec4 dv = textureLod(" + texname + ", in_Coord, 0.0);\n";
            //displace vertex along normal
            vert_main += 
				"  float displace = dv.r; //dot( vec4(0.30,0.59,0.11,0.0),dv);\n";
			vert_main += 
                SG_V4_VERTCOORD+".xyz += " + texname + "_intensity * displace * in_Normal;\n";
            displace = true;
            break;
        }
    }
    //require also normal map
    if(displace)
    {
        for(m_it_textures = m_textures.begin(); m_it_textures != m_textures.end(); ++m_it_textures)
        {
            if(m_it_textures->second->GetType() == BUMP)
            {
                string texname = m_it_textures->second->GetName();
                if(m_it_textures->second->HasTiles())
                    vert_vars += 
						"uniform float " + texname + "_tileX, " + texname + "_tileY;\n";
                vert_vars += 
					"uniform float " + texname + "_intensity;\n"
                    "uniform sampler2D "+ texname + ";\n";
                vert_main	+= 
					"  //displace normal using normal map\n";
                //tiles: tile texture coordinate
                if(m_it_textures->second->HasTiles())
                    vert_main += 
						"  dNormal += normalize(texture(" + texname + ",in_Coord*vec2(" + texname + "_tileX, " + texname + "_tileY)).rgb * 2.0 - 1.0);\n\n";
                //no tiles
                else
                    vert_main += 
						"  dNormal += normalize(texture(" + texname + ",in_Coord).rgb * 2.0 - 1.0);\n\n";
                break;
            }
        }
    }


    ////////////////////////////////////////////////////////////////////////////////
    //-- 1.3 Light model: PHONG or GOURAUD

    tmp += "Env";

	//------------------------------------------------------------------------------
    //-- 1.3.1 if PHONG, light calculation will be done in fragment shader, so send necessary variables to it (also required for env mapping)

    if(m_lightModel == PHONG || m_textures.find(tmp) != m_textures.end() )
    {
        vert_vars += "out vec3 normal, eyeVec;\n";  //varying variables
        vert_main +=
            "  normal = mat3(in_ModelViewMatrix) * dNormal;  //surface normal\n"
            "  eyeVec = -vec3(in_ModelViewMatrix * "+SG_V4_VERTCOORD+");   //eyeview vector\n";
    }

	//------------------------------------------------------------------------------
    //-- 1.3.2 if GOURAUD, done light calculation per-vertex

    else if(m_lightModel == GOURAUD)
    {
        vert_vars += "out vec4 v_color;\n"
            "out vec3 normal, eyeVec;\n";

        vert_func += LoadFunc("light");
        vert_main +=
            "  normal = mat3(in_ModelViewMatrix) * dNormal;  //surface normal\n"
            "  eyeVec = -vec3(in_ModelViewMatrix * "+SG_V4_VERTCOORD+");            //eyeview vector\n"
            "  v_color = LightModel(normal, eyeVec);                   //calculate light model\n";
    }

	//------------------------------------------------------------------------------
    //-- 1.3.3 no light model

    else
    { 
        vert_vars += "out vec3 normal;\n";
        vert_main += "  normal = mat3(in_ModelViewMatrix) * dNormal;  //surface normal\n";
    }

    ////////////////////////////////////////////////////////////////////////////////
    //-- 1.4 Textures
	
    //-- send texture coords to fragment shader
    vert_main += "  fragTexCoord = in_Coord;\n";

    //******************************
    //iterate through textures
    for(m_it_textures = m_textures.begin(); m_it_textures != m_textures.end(); ++m_it_textures)
    {
		//------------------------------------------------------------------------------
        //-- 1.4.1 calculate shadow matrices for frag. shader (projected shadow and texture matrix)
        
		// FIXME: Shadow "textury" sem nepatri. MUSI SE TO UDELAT JINAK !!!!
		if(m_it_textures->second->GetType() == SHADOW)
        {
            vert_vars += "out vec4 " + m_it_textures->first + "_projShadow;\n";
            vert_main += "  " + m_it_textures->first + "_projShadow = " + m_it_textures->first + "_texMatrix * in_ModelViewMatrix * "+SG_V4_VERTCOORD+";   //calculate shadow texture projection\n";
        }

		//------------------------------------------------------------------------------
        //-- 1.4.2 send 3D coordinates for cube map

        if(m_it_textures->second->GetType() == CUBEMAP)
        {
            vert_vars += "out vec3 " + m_it_textures->first + "_cubeCoords;\n";
            vert_main += "  " + m_it_textures->first + "_cubeCoords = "+SG_V4_VERTCOORD+".xyz;\n";
        }

		//------------------------------------------------------------------------------
        //-- 1.4.3 send 3D coordinates for environment cube map

        if(m_it_textures->second->GetType() == CUBEMAP_ENV)
        {
            vert_vars += "out vec3 r_normal, r_eyeVec;\n";
            vert_main += 
                "\n  //Camera rotation can be found in transposed modelview matrix\n"
                "   mat3 tcamerarot;\n"
                "   tcamerarot[0][0] = in_ModelViewMatrix[0][0];\n"
                "   tcamerarot[0][1] = in_ModelViewMatrix[1][0];\n"
                "   tcamerarot[0][2] = in_ModelViewMatrix[2][0];\n"
                "   tcamerarot[1][0] = in_ModelViewMatrix[0][1];\n"
                "   tcamerarot[1][1] = in_ModelViewMatrix[1][1];\n"
                "   tcamerarot[1][2] = in_ModelViewMatrix[2][1];\n"
                "   tcamerarot[2][0] = in_ModelViewMatrix[0][2];\n"
                "   tcamerarot[2][1] = in_ModelViewMatrix[1][2];\n"
                "   tcamerarot[2][2] = in_ModelViewMatrix[2][2];\n\n"
                "\n  //Coordinates for cubemap reflexion\n"
                "  r_normal = tcamerarot * (in_ModelViewMatrix * vec4(dNormal,0.0)).xyz;\n"
                "  r_eyeVec = tcamerarot * (in_ModelViewMatrix * "+SG_V4_VERTCOORD+").xyz;\n\n";
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    //-- 1.5 Finalize, vertex transform and save depth

	for( m_if = m_features.begin(); m_if != m_features.end(); ++m_if )
	{
		vert_vars += (*m_if)->GetVars( ShaderFeature::VS );
		vert_main += (*m_if)->GetModifiers( ShaderFeature::VS );
	}

    vert_main +=
        "  vec4 viewPos = in_ModelViewMatrix * "+SG_V4_VERTCOORD+";       //vertex transform\n"
        "  v_depth = -viewPos.z;                             //store depth\n"
        "  gl_Position = in_ProjectionMatrix * viewPos;      //projection transform\n"     
        "}\n"
        "\n";

    m_v_source += vert_vars + vert_func + vert_main;


////////////////////////////////////////////////////////////////////////////////
//--
//-- 3 CREATION OF FRAGMENT SHADER
//--
////////////////////////////////////////////////////////////////////////////////

	string frag_vars = "//-- GLSL fragment shader generated by gluxEngine\n\n";	//-- Shader variables
	string frag_func = "//-- Fragment shader functions\n\n";					//-- Shader functions
	string frag_main = "\nvoid main()\n{\n";									//-- Shader main function

////////////////////////////////////////////////////////////////////////////////
//-- 3.1 fragment shader variables

    frag_vars +=    
		"//-- Material: \"" + m_name + "\"\n"
		"\n"
        "const int LIGHTS = " + num2str(light_count) + ";\n"      //scene light count
		"\n"
        "in vec2 fragTexCoord;		//-- texture coordinate\n"
        "in float v_depth;			//--fragment depth in world space\n"
		"\n";

    //fragment shader output
    if(m_useMRT)
        frag_vars += "out vec4 out_FragData[2];\n\n";
    else
        frag_vars += "out vec4 out_FragColor;\n\n";

    bool bump = false;      //do we have bump map present?


////////////////////////////////////////////////////////////////////////////////
//-- 3.2 Texture pre-setup: create texture samplers for every texture (shadow maps use shadow samplers), texture tiles and coordinates

    frag_main += "  vec2 texCoord = fragTexCoord;\n";

    //for parallax/depth map, we modify texture coordinate
    for(m_it_textures = m_textures.begin(); m_it_textures != m_textures.end(); ++m_it_textures)
    {    
        if(m_it_textures->second->GetType() == PARALLAX)
        {
            string offset;
            offset = num2str(m_it_textures->second->GetIntensity());  //parallax offset value
            frag_main +=
                "  //simple parallax mapping, modify texture coordinates\n"
                "  float height = texture(" + m_name + "ParallaxA,  texCoord).r;\n"
                "  float offset = " + offset + " * (2.0 * height - 1.0);\n"
                "  texCoord = texCoord + eyeVec.xy * offset;\n\n";
            break;
        }
    }

    for(m_it_textures = m_textures.begin(); m_it_textures != m_textures.end(); ++m_it_textures)
    {
		
        if(!m_it_textures->second->Empty() &&
			//FIXME: misto SHADOW* sem ma prijit CUSTOM 
			m_it_textures->second->GetType() != SHADOW && 
			m_it_textures->second->GetType() != SHADOW_OMNI && 
			//FIXME: Lepsi ..
			m_it_textures->second->GetType() != CUSTOM &&
			m_it_textures->second->GetType() != DISPLACE )
        {
            frag_main += "\n  vec4 " + m_it_textures->first + "_texture;\n";

            //cube map, we add different sampler + 3D coordinates
            if(m_it_textures->second->GetType() == CUBEMAP) 
                frag_vars += "uniform samplerCube "+ m_it_textures->first + ";\nin vec3 " + m_it_textures->first + "_cubeCoords;\n\n";
            //environment cube map, we need cube sampler and normal + view vector
            else if(m_it_textures->second->GetType() == CUBEMAP_ENV) 
            {
                frag_vars += "uniform samplerCube "+ m_it_textures->first + ";\n"
                    "uniform float " + m_it_textures->first + "_intensity;\n"
                    "in vec3 r_normal, r_eyeVec;\n\n";
            }
            //regular 2D texture sampler
            else 
            {
                //texture intensity
                frag_vars += "uniform float " + m_it_textures->first + "_intensity;\n";
                //texture tiles (if set)
                if(m_it_textures->second->HasTiles())
                {
                    frag_vars += "uniform float " + m_it_textures->first + "_tileX," + m_it_textures->first + "_tileY;\n";
                    frag_main +=
                        "  //" + m_it_textures->first + ", texture tiles\n"
                        "  vec2 " + m_it_textures->first + "_texcoord = texCoord * vec2(" + m_it_textures->first + "_tileX, " + m_it_textures->first + "_tileY) ;\n";
                }
                //no tiles
                else
                    frag_main += "  vec2 " + m_it_textures->first + "_texcoord = texCoord;\n";

                frag_vars += "uniform sampler2D "+ m_it_textures->first + ";\n";
            }
        }
    }


////////////////////////////////////////////////////////////////////////////////
//-- 3.3 Lighting

//------------------------------------------------------------------------------
//-- 3.3.1 if light model is set to PHONG, calculate lighting by using varying variables sent from vertex shader (normal, eye vector...)

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

//------------------------------------------------------------------------------
//-- 3.3.2 if GOURAUD model is set, only get color value from vertex shader

    else if(m_lightModel == GOURAUD) 
    {
        frag_vars += "in vec4 v_color;\n"
            "in vec3 normal;\n";
        frag_main += "  vec4 color = v_color;\n";
    }

//------------------------------------------------------------------------------
//-- 3.3.3 else constant shading - only material diffuse is in computation

    else
    {
        frag_vars += "in vec3 normal;\n\n"
            "//material settings (only diffuse)\n"
            "struct Material{\n"
            "  vec3 diffuse;\n};\n"
            "uniform Material material;\n";
        frag_main += "  vec4 color = vec4(material.diffuse,0.0);\n";
    }


////////////////////////////////////////////////////////////////////////////////
//-- 3.4 texture application according to texture type

    string alpha_test = "";
    for(m_it_textures = m_textures.begin(); m_it_textures != m_textures.end(); ++m_it_textures)
    {
        if(!m_it_textures->second->Empty())
        {
            //skip displacement map
            if(m_it_textures->second->GetType() == DISPLACE)
                continue;

//------------------------------------------------------------------------------
//-- 3.4.1 shadow maps - set shadow samplers and call function to project and create soft shadows

			// FIXME: Shadow "textury" sem nepatri. MUSI SE TO UDELAT JINAK !!!!
            else if(m_it_textures->second->GetType() == SHADOW)
            {
                frag_vars += "in vec4 " + m_it_textures->first + "_projShadow;\n";
                frag_vars +=
                    "uniform float " + m_it_textures->first + "_intensity;\n"
                    "uniform sampler2DShadow " + m_it_textures->first + ";\n";

                //insert shadow function (only once)
                if(m_it_textures->first.find("ShadowA") != string::npos)
                    frag_func += LoadFunc("shadow");

                frag_main += "\n  //Shadow map projection\n"
                    "  color *= PCFShadow(" + m_it_textures->first + "," + m_it_textures->first + "_projShadow, " + m_it_textures->first + "_intensity);\n";
            }
            else if(m_it_textures->second->GetType() == SHADOW_OMNI)
            {
                frag_vars +=
                    "uniform float " + m_it_textures->first + "_intensity;\n"
                    "uniform sampler2DArray " + m_it_textures->first + ";\n";

                //do we use parabola cut?
                if(dpshadow_method == CUT)
                    frag_vars += "#define PARABOLA_CUT\n";
                if(use_pcf)
                    frag_vars += "#define USE_PCF\n";

                //insert shadow function (only once)
                if(m_it_textures->first.find("ShadowOMNI_A") != string::npos)
					if( dpshadow_method == CUT || dpshadow_method == DPSM  )
						frag_func += LoadFunc("shadow_cut");
						

				//FIXME: jako parametr funkce dat m_variables["ObjSpacePosition"]
                frag_main += "\n  //Shadow map projection\n"
                    "  color *= ShadowOMNI(" + m_it_textures->first + ", " + m_it_textures->first + "_intensity);\n";
            }

            //other texture types
            else
            {
                //should we use alpha testing?
                if(m_it_textures->second->GetType() == ALPHA)
                    alpha_test = "  //alpha test\n  if( all(lessThan(" + m_it_textures->first + "_texture.rgb, vec3(0.25)))) discard;\n";

                ///3.4.2 environment maps - add normal and eye vector variables(if per-pixel)
                if(m_it_textures->second->GetType() == ENV)
                {
                    //when we have more env maps, insert env function only once
                    if(m_it_textures->first.find("EnvB") == string::npos)
                    {

                        if(m_lightModel != PHONG)
                            frag_vars += "in vec3 eyeVec;\n";
                        //add environment map computation
                        frag_func += LoadFunc("env");
                    }
                }

                //reflect fragment from environment cubemap
                if(m_it_textures->second->GetType() == CUBEMAP_ENV)
                {
                    if(bump)    //if we have bump map, modify reflection vector to get bumpy reflections
                        frag_main += "  vec3 reflVec = reflect(normalize(r_eyeVec), normalize(bump_normal)); //bumped reflection vector for cubemap\n";
                    else
                        frag_main += "  vec3 reflVec = reflect(normalize(r_eyeVec), normalize(r_normal)); //reflection vector for cubemap\n";
                }

                //compute fragment color from texel(not for bump/parallax map)
                if(m_it_textures->second->GetType() != BUMP && m_it_textures->second->GetType() != PARALLAX)
                    frag_main += computeTexel(m_it_textures->second,m_it_textures->first);

            }
        }
    }


////////////////////////////////////////////////////////////////////////////////
//-- 3.5 Finalize (with transparency or alpha test)

	
	for( m_if = m_features.begin(); m_if != m_features.end(); ++m_if )
	{
		frag_vars += (*m_if)->GetVars( ShaderFeature::FS );
		frag_func += (*m_if)->GetFunc( ShaderFeature::FS );
		frag_main += (*m_if)->GetModifiers( ShaderFeature::FS );
	}

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
                "  out_FragData[0].rgba = color.rgba; //color;\n"
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

    m_f_source += frag_vars + frag_func + frag_main;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

	//save shaders to debug file
    string file; file = "shader_out/" + m_name + ".vert";
    ofstream fout1(file.c_str());
    fout1<<m_v_source;
    file = "shader_out/" + m_name + ".frag";
    ofstream fout2(file.c_str());
    fout2<<m_f_source;

	if( !BuildProgram() ) 
		return false;
	if( !CheckShaderStatus() )
		return false;


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

    ///set materials parameters
    SetUniform("material.ambient",m_ambColor);
    SetUniform("material.diffuse",m_diffColor);
    SetUniform("material.specular",m_specColor);
    SetUniform("material.shininess",m_shininess);

    m_baked = true;

    //setup uniform buffers
    GLint uniformIndex = glGetUniformBlockIndex(m_program, "Matrices");
    if(uniformIndex >= 0)
        glUniformBlockBinding(m_program, uniformIndex, UNIFORM_MATRICES);
    uniformIndex = glGetUniformBlockIndex(m_program, "Lights");
    if(uniformIndex >= 0)
        glUniformBlockBinding(m_program, uniformIndex, UNIFORM_LIGHTS);
    glUseProgram(0);

    //print_uniform_block_info(shader, uniformIndex);

    return true;
}
