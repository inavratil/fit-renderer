#include "OmniShadow.h"

//-----------------------------------------------------------------------------

OmniShadow::OmniShadow( TScene* _scene ) :
	IShadowTechnique( _scene )
{
}

//-----------------------------------------------------------------------------

OmniShadow::~OmniShadow(void)
{
}

//-----------------------------------------------------------------------------

bool OmniShadow::Initialize()
{
	return true;
}

//-----------------------------------------------------------------------------

void OmniShadow::PreRender()
{
}

//-----------------------------------------------------------------------------

void OmniShadow::PostRender()
{
}

//-----------------------------------------------------------------------------

void OmniShadow::_EvaluateBestConfiguration()
{
	//#define TEST_DPSM
#if 0
	glm::vec2 res_cut_angle;
	glm::vec2 res_parab_angle;

	float min_sum = 1000.0f;

    static bool first = true;
    if( first )
    {
#ifdef TEST_DPSM
        float d_cutx = 0.0;
        float d_cuty = 0.0;
#else
        for( float d_cuty = -64.0; d_cuty < 64.0; d_cuty += 10 )
            for( float d_cutx = -64.0; d_cutx < 64.0; d_cutx += 10 )
#endif
            {
                cout << "Cut angles:" << d_cutx << "-" << d_cuty << endl;
                for( float d_roty = 0; d_roty <= 180; d_roty += 18 )
                {
                    for( float d_rotx = 0; d_rotx <= 180; d_rotx += 18 )
                    {
                        cut_angle.x = d_cutx;
                        cut_angle.y = d_cuty;
                        parab_angle.x = d_rotx;
                        parab_angle.y = d_roty;
                        //cut_tgangle = -1.5;
                        //parab_angle.x = 54;
                        //parab_angle.y = 162;
                        ///draw all lights
                        unsigned i;
                        for(i=0, il = lights.begin(); il != lights.end(), i<lights.size(); il++, i++)
                        {
                            ///if light has a shadow, render scene from light view to texture (TScene::RenderShadowMap())
                            if(il->HasShadow())
                            {

                                //render shadow map
                                if(il->GetType() == OMNI)
                                    RenderShadowMapOmni(&(*il));
                                else 
                                    RenderShadowMap(&(*il));
                            }
                        }

                        RenderAliasError();
                        glBindTexture(GL_TEXTURE_2D, tex_cache["aliaserr_texture"]);
                        glGetTexImage(GL_TEXTURE_2D, 0, GL_ALPHA, GL_FLOAT, aerr_buffer);

                        float sum = 0.0;
                        int count = 0;
                        for( int i=0; i<(resx*resy); ++i )
                        {
                            if( aerr_buffer[i] > 0.0 )
                            {
                                sum += aerr_buffer[i];
                                count++;
                            }
                        }

                        //cout << ""<< sum/count << ";" << cut_angle.x << ";" << parab_angle.x << ";" << parab_angle.y << endl;
                        cout << sum/count << " ";

                        if( sum/count < min_sum )
                        {
                            min_sum = sum/count;
                            res_cut_angle.x = cut_angle.x;
                            res_cut_angle.y = cut_angle.y;
                            res_parab_angle.x = parab_angle.x;
                            res_parab_angle.y = parab_angle.y;
                        }
                    }
                    cout << endl;
                }
            }

        cout << "Res: "<< min_sum << ", " << res_cut_angle.x << ", " << res_cut_angle.y << ", " << res_parab_angle.x << ", " << res_parab_angle.y << endl;
        first = false;
    } //-- first
#endif

}

//-----------------------------------------------------------------------------