
#ifndef _EXPERIMENTS_H_
#define _EXPERIMENTS_H_

#define EXP_COUNT 6

struct Experiment {
    int scene;
    glm::vec4 cut_params;
    glm::vec3 light_pos;
    glm::vec3 cam_pos;
    glm::vec3 cam_rot;
};

const Experiment experiments[EXP_COUNT] = 
{
    {
        // 0: test cones
        3,
        glm::vec4( -45, 20 , 238, -280 ),
        glm::vec3(  0,75,0 ),
        glm::vec3( -589.744,-383.069,-279.894 ),
        glm::vec3( 20,-67,0 )
    },
    {   // 1: ZNOVA
        // trees
        // Res_cit: 1.12588, -14, 36, 72, 180
        // Res: 0.847833, -64, 46, 90, 108
        4, 
        glm::vec4( -64, 46, 90, 108), 
        glm::vec3( -231,31,77 ),
        glm::vec3( -168.321,-76.7447,114.67 ),
        glm::vec3( 26,-469,0 )

    },
    {   // 2: zla_scena: DONE
        // Res_dpsm: 1.66974, 0, 0, 72, 162
        // Res_cut: 1.09428, -34, 46, 72, 126
        // ResA_cut: 1.10351, 6, 56, 72, 162
        5, 
        glm::vec4( 6, 56, 72, 162 ),
        glm::vec3( -237,224,277 ), 
        glm::vec3( -313.327,-181.739,429.477 ),
        glm::vec3( 13,194,0 )
    },
    {
        // 3: zla_scena: TOHLE NE
        // Res_dpsm: 1.62896, 0, 0, 36, 18
        // Res_cut: 1.30593, -24, 36, 36, 18
        // ResA_cut: 1.30759, -14, 36, 36, 18
        5, 
        glm::vec4( -14, 36, 36, 18 ),
        glm::vec3( 0,217,75 ),
        glm::vec3( 205.413,-276.738,393.74 ),
        glm::vec3( 34,-558,0 )

        //glm::vec4( -64.0, 0.0, 36,36 ), 
        //glm::vec3( -266,249,7 ),
        //glm::vec3( -263.72,-184.118,943.619 ),
        //glm::vec3( 17,196,0 )
    },
    {
        // 4: sibenik: DONE
        // Res_dpsm: 4.4413, 0, 0, 54, 54
        // Res_cut: 1.90079, 56, 36, 54, 54
        // ResA_cut: 1.99945, 56, 36, 54, 54
        2, 

        glm::vec4( 0.0 ), //glm::vec4( 56, 36, 54, 54 ),
		//pozice svetla v testing.3ds
        glm::vec3(120,98,-68), 
		//pozice svetla v katedrale
		//glm::vec3( 256,206,-23 ),
		// puvodni pohled zezadu
		//glm::vec3( 210.533,-82.6701,-34.9359 ),
		//glm::vec3( 5,81,0 )
		// pohled na stred
		//glm::vec3( 99.1987,-153.125,15.3984 ),
		//glm::vec3( 41,92,0 )
		// pohled na stranu
        //glm::vec3( 92.7532,-105.067,76.8914),
        //glm::vec3(32, 90, 0) 
		// pohled v testing.3ds scene
		glm::vec3(210.533,-82.6701,-34.9359),
		glm::vec3(28,79,0)

        //Res: 1.52775
        //glm::vec4( -64, -64, 108, 36 ), //-64, 0, 18, 108 ),
        //glm::vec3( -237,224,-31 ),
        //glm::vec3( -528.076,-13.6469,-141.076 ),
        //glm::vec3( -1,-81,0 )
    },
    {
        // skoda: DONE
        // Res_dpsm: 8.93931, 0, 0, 18, 180
        // Res_cut: 1.81829, -64, 46, 54, 54
        // ResA_cut: 2.02232, -64, 46, 54, 54
        1, glm::vec4( 0.0 ), //glm::vec4( -64, 46, 59, 77 ),
        glm::vec3( -89,155,125 ),
        glm::vec3( 13.1099,-7.05098,207.398 ),
        glm::vec3( -4,-169,0 )
    }
};

#endif
