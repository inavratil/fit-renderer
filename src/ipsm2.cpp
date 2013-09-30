#include "glux_engine/scene.h"

#include "glux_engine/PolynomialWarpedShadow.h"
#include "glux_engine/BilinearWarpedShadow.h"
#include "glux_engine/SplineWarpedShadow.h"

#include "glux_engine/BlitPass.h"
#include "glux_engine/SimplePass.h"

#include "precomputed_diffs.h"

//-- Documentation
// Celou knihovnu a proces generovani stinu se budu snazit popsat v dokumentaci ulozenou na web, link
// https://docs.google.com/document/d/1TVNZwzaEpD_BRjS3NZEiqbOMya-NB1gBCh_aWeo1aE8/edit?usp=sharing

///////////////////////////////////////////////////////////////////////////////
//-- Defines

//#define DEBUG_DRAW 
//#define GRADIENT_METHOD
//#define ITERATION_ENABLED

///////////////////////////////////////////////////////////////////////////////
//-- Global variables

glm::vec4 g_precomputed_diffs[19*19] = {
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, -0.000000f, 0.0f, 0.0f),glm::vec4(-0.028888f, -0.000000f, 0.0f, 0.0f),glm::vec4(-0.048402f, 0.000000f, 0.0f, 0.0f),glm::vec4(-0.063904f, 0.000000f, 0.0f, 0.0f),glm::vec4(-0.078886f, 0.000000f, 0.0f, 0.0f),glm::vec4(-0.097815f, 0.000000f, 0.0f, 0.0f),glm::vec4(-0.082486f, 0.000000f, 0.0f, 0.0f),glm::vec4(-0.046921f, -0.000000f, 0.0f, 0.0f),glm::vec4(0.005918f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.069705f, -0.000000f, 0.0f, 0.0f),glm::vec4(0.101666f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.118334f, -0.000000f, 0.0f, 0.0f),glm::vec4(0.106020f, -0.000000f, 0.0f, 0.0f),glm::vec4(0.067614f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.049829f, -0.000000f, 0.0f, 0.0f),glm::vec4(0.030231f, -0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.029003f, 0.0f, 0.0f),glm::vec4(-0.058580f, 0.034530f, 0.0f, 0.0f),glm::vec4(-0.104735f, 0.002877f, 0.0f, 0.0f),glm::vec4(-0.142158f, 0.002262f, 0.0f, 0.0f),glm::vec4(-0.173042f, 0.013164f, 0.0f, 0.0f),glm::vec4(-0.174411f, 0.051942f, 0.0f, 0.0f),glm::vec4(-0.139787f, 0.032654f, 0.0f, 0.0f),glm::vec4(-0.107832f, 0.012858f, 0.0f, 0.0f),glm::vec4(-0.027586f, -0.010593f, 0.0f, 0.0f),glm::vec4(0.077379f, -0.006159f, 0.0f, 0.0f),glm::vec4(0.140032f, 0.020413f, 0.0f, 0.0f),glm::vec4(0.173109f, 0.004384f, 0.0f, 0.0f),glm::vec4(0.173264f, 0.004389f, 0.0f, 0.0f),glm::vec4(0.141860f, -0.003836f, 0.0f, 0.0f),glm::vec4(0.107412f, -0.000663f, 0.0f, 0.0f),glm::vec4(0.058910f, 0.030417f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.027565f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.042082f, 0.0f, 0.0f),glm::vec4(-0.036019f, 0.061027f, 0.0f, 0.0f),glm::vec4(-0.067470f, 0.028692f, 0.0f, 0.0f),glm::vec4(-0.105344f, 0.003685f, 0.0f, 0.0f),glm::vec4(-0.124319f, 0.025867f, 0.0f, 0.0f),glm::vec4(-0.108490f, 0.090939f, 0.0f, 0.0f),glm::vec4(-0.097377f, 0.136257f, 0.0f, 0.0f),glm::vec4(-0.068718f, 0.041288f, 0.0f, 0.0f),glm::vec4(-0.036671f, -0.018074f, 0.0f, 0.0f),glm::vec4(0.001595f, 0.012393f, 0.0f, 0.0f),glm::vec4(0.043327f, 0.042576f, 0.0f, 0.0f),glm::vec4(0.060770f, 0.023857f, 0.0f, 0.0f),glm::vec4(0.076463f, 0.020806f, 0.0f, 0.0f),glm::vec4(0.089378f, 0.010530f, 0.0f, 0.0f),glm::vec4(0.083631f, 0.016292f, 0.0f, 0.0f),glm::vec4(0.044641f, 0.062740f, 0.0f, 0.0f),glm::vec4(-0.000000f, 0.045973f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.054202f, 0.0f, 0.0f),glm::vec4(-0.041637f, 0.079784f, 0.0f, 0.0f),glm::vec4(-0.080923f, 0.035429f, 0.0f, 0.0f),glm::vec4(-0.127620f, 0.019080f, 0.0f, 0.0f),glm::vec4(-0.163400f, 0.075582f, 0.0f, 0.0f),glm::vec4(-0.172763f, 0.195483f, 0.0f, 0.0f),glm::vec4(-0.149448f, 0.225304f, 0.0f, 0.0f),glm::vec4(-0.115958f, 0.122055f, 0.0f, 0.0f),glm::vec4(-0.082155f, 0.028516f, 0.0f, 0.0f),glm::vec4(-0.048163f, 0.075581f, 0.0f, 0.0f),glm::vec4(-0.018188f, 0.085846f, 0.0f, 0.0f),glm::vec4(0.006983f, 0.047888f, 0.0f, 0.0f),glm::vec4(0.052457f, 0.087789f, 0.0f, 0.0f),glm::vec4(0.064142f, 0.098144f, 0.0f, 0.0f),glm::vec4(0.036254f, 0.072539f, 0.0f, 0.0f),glm::vec4(0.004374f, 0.058519f, 0.0f, 0.0f),glm::vec4(-0.000000f, 0.045311f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.063030f, 0.0f, 0.0f),glm::vec4(-0.042221f, 0.095714f, 0.0f, 0.0f),glm::vec4(-0.084446f, 0.042625f, 0.0f, 0.0f),glm::vec4(-0.136976f, 0.023437f, 0.0f, 0.0f),glm::vec4(-0.157291f, 0.075295f, 0.0f, 0.0f),glm::vec4(-0.180084f, 0.219404f, 0.0f, 0.0f),glm::vec4(-0.175121f, 0.341281f, 0.0f, 0.0f),glm::vec4(-0.116505f, 0.239291f, 0.0f, 0.0f),glm::vec4(-0.091969f, 0.123394f, 0.0f, 0.0f),glm::vec4(-0.062912f, 0.184411f, 0.0f, 0.0f),glm::vec4(-0.037434f, 0.223782f, 0.0f, 0.0f),glm::vec4(-0.014691f, 0.181048f, 0.0f, 0.0f),glm::vec4(-0.027879f, 0.171919f, 0.0f, 0.0f),glm::vec4(0.008636f, 0.160091f, 0.0f, 0.0f),glm::vec4(-0.000410f, 0.214308f, 0.0f, 0.0f),glm::vec4(0.004182f, 0.166573f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.100149f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.068463f, 0.0f, 0.0f),glm::vec4(-0.034110f, 0.106581f, 0.0f, 0.0f),glm::vec4(-0.072892f, 0.051470f, 0.0f, 0.0f),glm::vec4(-0.121045f, 0.024277f, 0.0f, 0.0f),glm::vec4(-0.134622f, 0.074465f, 0.0f, 0.0f),glm::vec4(-0.147853f, 0.216203f, 0.0f, 0.0f),glm::vec4(-0.159008f, 0.361866f, 0.0f, 0.0f),glm::vec4(-0.142407f, 0.413976f, 0.0f, 0.0f),glm::vec4(-0.065978f, 0.238597f, 0.0f, 0.0f),glm::vec4(-0.007196f, 0.307134f, 0.0f, 0.0f),glm::vec4(0.030252f, 0.375178f, 0.0f, 0.0f),glm::vec4(-0.011214f, 0.372873f, 0.0f, 0.0f),glm::vec4(0.024856f, 0.320797f, 0.0f, 0.0f),glm::vec4(0.025701f, 0.299986f, 0.0f, 0.0f),glm::vec4(0.034700f, 0.300505f, 0.0f, 0.0f),glm::vec4(0.028170f, 0.187634f, 0.0f, 0.0f),glm::vec4(-0.000000f, 0.095986f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.069952f, 0.0f, 0.0f),glm::vec4(-0.011891f, 0.109556f, 0.0f, 0.0f),glm::vec4(-0.038789f, 0.072122f, 0.0f, 0.0f),glm::vec4(-0.067686f, 0.024485f, 0.0f, 0.0f),glm::vec4(-0.072737f, 0.072896f, 0.0f, 0.0f),glm::vec4(-0.077622f, 0.213659f, 0.0f, 0.0f),glm::vec4(-0.081342f, 0.369244f, 0.0f, 0.0f),glm::vec4(-0.066518f, 0.433262f, 0.0f, 0.0f),glm::vec4(-0.025020f, 0.441404f, 0.0f, 0.0f),glm::vec4(0.042779f, 0.459108f, 0.0f, 0.0f),glm::vec4(0.064811f, 0.411540f, 0.0f, 0.0f),glm::vec4(0.045546f, 0.454602f, 0.0f, 0.0f),glm::vec4(0.065874f, 0.384858f, 0.0f, 0.0f),glm::vec4(0.057769f, 0.257269f, 0.0f, 0.0f),glm::vec4(0.039562f, 0.285378f, 0.0f, 0.0f),glm::vec4(0.008067f, 0.185553f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.096006f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.047620f, 0.0f, 0.0f),glm::vec4(-0.010107f, 0.076718f, 0.0f, 0.0f),glm::vec4(-0.016470f, 0.052467f, 0.0f, 0.0f),glm::vec4(-0.021920f, 0.023901f, 0.0f, 0.0f),glm::vec4(-0.022216f, 0.069818f, 0.0f, 0.0f),glm::vec4(-0.022224f, 0.210076f, 0.0f, 0.0f),glm::vec4(-0.022051f, 0.362713f, 0.0f, 0.0f),glm::vec4(-0.021952f, 0.419272f, 0.0f, 0.0f),glm::vec4(-0.016217f, 0.394345f, 0.0f, 0.0f),glm::vec4(-0.011512f, 0.442571f, 0.0f, 0.0f),glm::vec4(-0.011292f, 0.397634f, 0.0f, 0.0f),glm::vec4(-0.002675f, 0.442515f, 0.0f, 0.0f),glm::vec4(-0.002736f, 0.376908f, 0.0f, 0.0f),glm::vec4(-0.002464f, 0.252967f, 0.0f, 0.0f),glm::vec4(-0.000593f, 0.272860f, 0.0f, 0.0f),glm::vec4(0.000451f, 0.178531f, 0.0f, 0.0f),glm::vec4(-0.000000f, 0.093293f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.031959f, 0.0f, 0.0f),glm::vec4(-0.028095f, 0.049759f, 0.0f, 0.0f),glm::vec4(-0.055400f, 0.044934f, 0.0f, 0.0f),glm::vec4(-0.065265f, 0.020211f, 0.0f, 0.0f),glm::vec4(-0.103421f, 0.053000f, 0.0f, 0.0f),glm::vec4(-0.104625f, 0.176198f, 0.0f, 0.0f),glm::vec4(-0.103329f, 0.309685f, 0.0f, 0.0f),glm::vec4(-0.076084f, 0.383707f, 0.0f, 0.0f),glm::vec4(-0.067843f, 0.362099f, 0.0f, 0.0f),glm::vec4(-0.040589f, 0.401504f, 0.0f, 0.0f),glm::vec4(-0.027635f, 0.360676f, 0.0f, 0.0f),glm::vec4(-0.005300f, 0.390884f, 0.0f, 0.0f),glm::vec4(0.017860f, 0.327624f, 0.0f, 0.0f),glm::vec4(0.018168f, 0.223285f, 0.0f, 0.0f),glm::vec4(0.009800f, 0.241123f, 0.0f, 0.0f),glm::vec4(0.001220f, 0.166471f, 0.0f, 0.0f),glm::vec4(-0.000000f, 0.088294f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.023039f, 0.0f, 0.0f),glm::vec4(-0.027679f, 0.035670f, 0.0f, 0.0f),glm::vec4(-0.053557f, 0.029812f, 0.0f, 0.0f),glm::vec4(-0.081854f, 0.011312f, 0.0f, 0.0f),glm::vec4(-0.109347f, 0.047287f, 0.0f, 0.0f),glm::vec4(-0.110505f, 0.162342f, 0.0f, 0.0f),glm::vec4(-0.109312f, 0.285109f, 0.0f, 0.0f),glm::vec4(-0.089219f, 0.335600f, 0.0f, 0.0f),glm::vec4(-0.071120f, 0.320263f, 0.0f, 0.0f),glm::vec4(-0.035595f, 0.346487f, 0.0f, 0.0f),glm::vec4(0.007180f, 0.316564f, 0.0f, 0.0f),glm::vec4(0.028238f, 0.341383f, 0.0f, 0.0f),glm::vec4(0.029945f, 0.309339f, 0.0f, 0.0f),glm::vec4(0.030258f, 0.212872f, 0.0f, 0.0f),glm::vec4(0.010290f, 0.208341f, 0.0f, 0.0f),glm::vec4(0.001361f, 0.152933f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.082495f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.017315f, 0.0f, 0.0f),glm::vec4(-0.033414f, 0.026739f, 0.0f, 0.0f),glm::vec4(-0.065823f, 0.019987f, 0.0f, 0.0f),glm::vec4(-0.093439f, 0.004940f, 0.0f, 0.0f),glm::vec4(-0.126559f, 0.042624f, 0.0f, 0.0f),glm::vec4(-0.146113f, 0.139714f, 0.0f, 0.0f),glm::vec4(-0.154827f, 0.240694f, 0.0f, 0.0f),glm::vec4(-0.119722f, 0.281610f, 0.0f, 0.0f),glm::vec4(-0.086490f, 0.272678f, 0.0f, 0.0f),glm::vec4(-0.047231f, 0.290647f, 0.0f, 0.0f),glm::vec4(-0.004188f, 0.269443f, 0.0f, 0.0f),glm::vec4(0.032876f, 0.284447f, 0.0f, 0.0f),glm::vec4(0.032063f, 0.288186f, 0.0f, 0.0f),glm::vec4(0.032369f, 0.200834f, 0.0f, 0.0f),glm::vec4(0.014197f, 0.173442f, 0.0f, 0.0f),glm::vec4(0.001630f, 0.135874f, 0.0f, 0.0f),glm::vec4(-0.000000f, 0.074746f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.020104f, 0.0f, 0.0f),glm::vec4(-0.042004f, 0.034493f, 0.0f, 0.0f),glm::vec4(-0.080946f, 0.011508f, 0.0f, 0.0f),glm::vec4(-0.112969f, 0.004439f, 0.0f, 0.0f),glm::vec4(-0.151753f, 0.041024f, 0.0f, 0.0f),glm::vec4(-0.158969f, 0.121474f, 0.0f, 0.0f),glm::vec4(-0.166095f, 0.202976f, 0.0f, 0.0f),glm::vec4(-0.117849f, 0.224748f, 0.0f, 0.0f),glm::vec4(-0.066083f, 0.222208f, 0.0f, 0.0f),glm::vec4(-0.027171f, 0.236508f, 0.0f, 0.0f),glm::vec4(0.016811f, 0.221110f, 0.0f, 0.0f),glm::vec4(0.046995f, 0.228540f, 0.0f, 0.0f),glm::vec4(0.057977f, 0.255259f, 0.0f, 0.0f),glm::vec4(0.054490f, 0.182938f, 0.0f, 0.0f),glm::vec4(0.029403f, 0.144732f, 0.0f, 0.0f),glm::vec4(0.001983f, 0.116302f, 0.0f, 0.0f),glm::vec4(-0.000000f, 0.065411f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.027535f, 0.0f, 0.0f),glm::vec4(-0.038573f, 0.048033f, 0.0f, 0.0f),glm::vec4(-0.076643f, 0.012994f, 0.0f, 0.0f),glm::vec4(-0.119305f, 0.007369f, 0.0f, 0.0f),glm::vec4(-0.152204f, 0.034370f, 0.0f, 0.0f),glm::vec4(-0.155681f, 0.101319f, 0.0f, 0.0f),glm::vec4(-0.156694f, 0.155113f, 0.0f, 0.0f),glm::vec4(-0.126635f, 0.170189f, 0.0f, 0.0f),glm::vec4(-0.079016f, 0.168400f, 0.0f, 0.0f),glm::vec4(-0.028276f, 0.182067f, 0.0f, 0.0f),glm::vec4(-0.000642f, 0.171306f, 0.0f, 0.0f),glm::vec4(0.019074f, 0.180570f, 0.0f, 0.0f),glm::vec4(0.050757f, 0.203503f, 0.0f, 0.0f),glm::vec4(0.056132f, 0.146077f, 0.0f, 0.0f),glm::vec4(0.045989f, 0.126622f, 0.0f, 0.0f),glm::vec4(0.022386f, 0.090507f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.044691f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.027119f, 0.0f, 0.0f),glm::vec4(-0.031016f, 0.046765f, 0.0f, 0.0f),glm::vec4(-0.062602f, 0.023320f, 0.0f, 0.0f),glm::vec4(-0.096902f, 0.005201f, 0.0f, 0.0f),glm::vec4(-0.122474f, 0.025893f, 0.0f, 0.0f),glm::vec4(-0.127553f, 0.077556f, 0.0f, 0.0f),glm::vec4(-0.124762f, 0.107380f, 0.0f, 0.0f),glm::vec4(-0.107961f, 0.119225f, 0.0f, 0.0f),glm::vec4(-0.082823f, 0.116085f, 0.0f, 0.0f),glm::vec4(-0.043278f, 0.130758f, 0.0f, 0.0f),glm::vec4(-0.023554f, 0.124440f, 0.0f, 0.0f),glm::vec4(-0.006225f, 0.134211f, 0.0f, 0.0f),glm::vec4(0.034108f, 0.154233f, 0.0f, 0.0f),glm::vec4(0.040513f, 0.110397f, 0.0f, 0.0f),glm::vec4(0.034563f, 0.098823f, 0.0f, 0.0f),glm::vec4(0.018008f, 0.068814f, 0.0f, 0.0f),glm::vec4(-0.000000f, 0.032694f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.016505f, 0.0f, 0.0f),glm::vec4(-0.020590f, 0.028443f, 0.0f, 0.0f),glm::vec4(-0.040829f, 0.012137f, 0.0f, 0.0f),glm::vec4(-0.056665f, 0.000907f, 0.0f, 0.0f),glm::vec4(-0.075594f, 0.017430f, 0.0f, 0.0f),glm::vec4(-0.081087f, 0.051812f, 0.0f, 0.0f),glm::vec4(-0.077542f, 0.065124f, 0.0f, 0.0f),glm::vec4(-0.069634f, 0.073179f, 0.0f, 0.0f),glm::vec4(-0.061451f, 0.069293f, 0.0f, 0.0f),glm::vec4(-0.048027f, 0.082745f, 0.0f, 0.0f),glm::vec4(-0.036622f, 0.079147f, 0.0f, 0.0f),glm::vec4(-0.022186f, 0.084007f, 0.0f, 0.0f),glm::vec4(0.016917f, 0.108541f, 0.0f, 0.0f),glm::vec4(0.024574f, 0.076880f, 0.0f, 0.0f),glm::vec4(0.025992f, 0.067269f, 0.0f, 0.0f),glm::vec4(0.018464f, 0.042821f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.018714f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.008664f, 0.0f, 0.0f),glm::vec4(-0.036418f, 0.015786f, 0.0f, 0.0f),glm::vec4(-0.073345f, 0.005236f, 0.0f, 0.0f),glm::vec4(-0.111742f, 0.000575f, 0.0f, 0.0f),glm::vec4(-0.141254f, 0.007669f, 0.0f, 0.0f),glm::vec4(-0.150218f, 0.021755f, 0.0f, 0.0f),glm::vec4(-0.146662f, 0.030814f, 0.0f, 0.0f),glm::vec4(-0.135058f, 0.035382f, 0.0f, 0.0f),glm::vec4(-0.118877f, 0.035204f, 0.0f, 0.0f),glm::vec4(-0.099690f, 0.040287f, 0.0f, 0.0f),glm::vec4(-0.076294f, 0.040442f, 0.0f, 0.0f),glm::vec4(-0.050335f, 0.041574f, 0.0f, 0.0f),glm::vec4(0.000735f, 0.051422f, 0.0f, 0.0f),glm::vec4(0.039392f, 0.035906f, 0.0f, 0.0f),glm::vec4(0.045440f, 0.034663f, 0.0f, 0.0f),glm::vec4(0.030646f, 0.021864f, 0.0f, 0.0f),glm::vec4(-0.000000f, 0.010066f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, -0.000000f, 0.0f, 0.0f),glm::vec4(-0.018352f, -0.000000f, 0.0f, 0.0f),glm::vec4(-0.037096f, -0.000000f, 0.0f, 0.0f),glm::vec4(-0.061508f, -0.000000f, 0.0f, 0.0f),glm::vec4(-0.076385f, -0.000000f, 0.0f, 0.0f),glm::vec4(-0.080896f, -0.000000f, 0.0f, 0.0f),glm::vec4(-0.079110f, -0.000000f, 0.0f, 0.0f),glm::vec4(-0.073301f, -0.000000f, 0.0f, 0.0f),glm::vec4(-0.064964f, -0.000000f, 0.0f, 0.0f),glm::vec4(-0.055340f, -0.000000f, 0.0f, 0.0f),glm::vec4(-0.042989f, -0.000000f, 0.0f, 0.0f),glm::vec4(-0.029846f, -0.000000f, 0.0f, 0.0f),glm::vec4(-0.004283f, -0.000000f, 0.0f, 0.0f),glm::vec4(0.021873f, -0.000000f, 0.0f, 0.0f),glm::vec4(0.022328f, -0.000000f, 0.0f, 0.0f),glm::vec4(0.015633f, -0.000000f, 0.0f, 0.0f),glm::vec4(-0.000000f, -0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),glm::vec4(0.000000f, 0.000000f, 0.0f, 0.0f),
};

///////////////////////////////////////////////////////////////////////////////


const float COVERAGE[100] = { 0.2525314589560607, 0.2576326279264187, 0.2628360322702068, 0.2681431353083358, 0.2735553772432651, 0.2790741715891978, 0.2847009014014726, 0.2904369152983624, 0.2962835232704883, 0.3022419922728888, 0.308313541593817, 0.31449933799764, 0.3208004906361242, 0.3272180457262481, 0.3337529809911264, 0.3404061998622743, 0.3471785254421841, 0.3540706942267695,
	0.3610833495887235, 0.3682170350233647, 0.3754721871601566, 0.3828491285443266, 0.3903480601940126, 0.3979690539407849, 0.4057120445616824, 0.4135768217143915, 0.42156302168711, 0.4296701189785898, 0.437897417723999, 0.4462440429868523, 0.4547089319362121, 0.463290824935085, 0.4719882565643009, 0.4807995466116144, 0.4897227910574863, 0.4987558530912262, 0.5078963541952966, 0.5171416653374513,
	0.5264888983132224, 0.5359348972850012, 0.5454762305660166, 0.5551091827004417, 0.5648297468943986, 0.5746336178533678, 0.584516185086649, 0.5944725267394274, 0.6044974040160985, 0.6145852562605928, 0.6247301967608438, 0.6349260093447057, 0.64516614583707, 0.6554437244475375, 0.6657515291573695, 0.6760820101752764, 0.6864272855286133, 0.6967791438574159, 0.7071290484729112, 0.7174681427427703,
	0.7277872568587884, 0.7380769160403831, 0.748327350219802, 0.7585285052521402, 0.7686700556833742, 0.7787414191045477, 0.7887317721106483, 0.7986300678752298, 0.8084250553400247, 0.8181053000119952, 0.8276592063450691, 0.8370750416769379, 0.846340961676199, 0.8554450372439244, 0.8643752828030637, 0.87311968589294, 0.8816662379765975, 0.890002966355447, 0.8981179670715378, 0.9059994386685738,
	0.9136357166693593, 0.9210153086167311, 0.9281269295161807, 0.9349595375079411, 0.9415023695886816, 0.9477449771968978, 0.9536772614690937, 0.9592895079717747, 0.9645724207097471, 0.9695171552121149, 0.9741153504974227, 0.9783591597219075, 0.9822412793198499, 0.9857549764505119, 0.9888941145749449, 0.991653176994985, 0.9940272881980572, 0.9960122328654299, 0.9976044724143962, 0.9988011589619574,
	0.9996001466136796, 1 };

void ModifyGrid(glm::vec4 *precomputed_diffs);

/**
****************************************************************************************************
@brief Creates shadow map and other structures for selected light for multiresolution rendering
@param ii iterator to lights list
@return success/fail of shadow creation
****************************************************************************************************/
bool TScene::WarpedShadows_InitializeTechnique(vector<TLight*>::iterator ii)
{
	int sh_res = (*ii)->ShadowSize();

	//-- Polynomial shadow technique
	//this->SetShadowTechnique( new PolynomialWarpedShadow() );

	//-- Bilinear shadow technique
	//this->SetShadowTechnique( new BilinearWarpedShadow() );
	//-- Spline shadow technique
	this->SetShadowTechnique( new SplineWarpedShadow() );
	//-- nastavi se rozliseni MRIZKY, tj. kolik ridicich bodu bude mit mrizka
	m_shadow_technique->SetControlPointsCount( 17.0 );

	//create data textures
	try{

		m_texture_cache->Create2DManual( "tex_camAndLightCoords", sh_res/8, sh_res/8, GL_RGBA16F, GL_FLOAT, GL_NEAREST, false );
		m_texture_cache->Create2DManual( "tex_stencil_color", 128, 128, GL_RGBA16F,	GL_FLOAT, GL_NEAREST, false );
		m_texture_cache->Create2DManual( "tex_output", sh_res/8, sh_res/8, GL_RGBA16F, GL_FLOAT, GL_NEAREST, false );
		m_texture_cache->Create2DManual( "MTEX_ping", sh_res/8, sh_res/8, GL_RGBA16F, GL_FLOAT, GL_NEAREST, false );
		m_texture_cache->Create2DManual( "MTEX_pong", sh_res/8, sh_res/8, GL_RGBA16F, GL_FLOAT, GL_NEAREST, false );
		//FIXME: precision??? nestaci 16F ?
		m_texture_cache->Create2DManual( "MTEX_2Dfunc_values", m_shadow_technique->GetControlPointsCount(), m_shadow_technique->GetControlPointsCount(), GL_RGBA32F, GL_FLOAT, GL_NEAREST, false );
		m_texture_cache->Create2DManual( "MTEX_2Dfunc_values_ping", m_shadow_technique->GetControlPointsCount(), m_shadow_technique->GetControlPointsCount(), GL_RGBA32F, GL_FLOAT, GL_NEAREST, false );
		
		TexturePtr tex_shadow = m_texture_cache->Create2DArrayManual("tex_shadow",
			sh_res, sh_res,			//-- width and height
			2,						//-- number of layers
			GL_DEPTH_COMPONENT,		//-- internal format
			GL_FLOAT,				//-- type of the date
			GL_NEAREST,				//-- filtering
			false					//-- mipmap generation
			);
		tex_shadow->SetType( SHADOW_OMNI );
		tex_shadow->SetIntensity( (*ii)->ShadowIntensity() );
		m_texture_cache->Create2DArrayManual("MTEX_warped_depth_color",
			sh_res, sh_res,	//-- width and height
			2,				//-- number of layers
			GL_RGBA16F,		//-- internal format
			GL_FLOAT,		//-- type of the date
			GL_NEAREST,		//-- filtering
			false			//-- mipmap generation
			);
		m_texture_cache->Create2DManual("aliaserr_mipmap",
			128, 128,		//-- width and height
			GL_RGBA16F,		//-- internal format
			GL_FLOAT,		//-- type of the date
			GL_NEAREST,		//-- filtering
			true			//-- mipmap generation
			);

		//-----------------------------------------------------------------------------
		//-- cam coords
		{
			//-- shader

#ifdef ITERATION_ENABLED
			CustomShader("mat_camAndLightCoords_afterDP","data/shaders/warping/camAndLightCoords_afterDP.vert", "data/shaders/warping/camAndLightCoords_afterDP.frag", m_shadow_technique->GetDefines(), "");
#else
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_camAndLightCoords_afterDP","data/shaders/warping/camAndLightCoords_afterDP.vert", "data/shaders/warping/camAndLightCoords_afterDP.frag" );
			mat->AddTexture( m_texture_cache->GetPtr( "MTEX_2Dfunc_values" ), "funcTex" );
			AddMaterial( mat );
#endif
			//-- pass
			SimplePassPtr pass_coords = new SimplePass( sh_res/8, sh_res/8 );
			pass_coords->AttachOutputTexture( 0, m_texture_cache->GetPtr( "tex_camAndLightCoords" ) );
			pass_coords->AttachOutputTexture( 1, m_texture_cache->GetPtr( "tex_stencil_color" ) );
			pass_coords->SetShader( mat );
			AppendPass("pass_coords", pass_coords );
		}

		//-----------------------------------------------------------------------------
		//-- output
		{				
			//-- shader
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_compute_aliasError", "data/shaders/warping/computeAliasError.vert"/*quad.vert*/, "data/shaders/warping/computeAliasError.frag" );
			mat->AddTexture( m_texture_cache->CreateFromImage( "data/tex/error_color.tga" ), "tex_error" );
			mat->AddTexture( m_texture_cache->GetPtr( "tex_camAndLightCoords" ), "tex_coords" );
			AddMaterial( mat );
			//-- pass
			SimplePassPtr pass_compute_aliasError = new SimplePass( sh_res/8, sh_res/8 );
			pass_compute_aliasError->AttachOutputTexture( 0, m_texture_cache->GetPtr( "tex_output" ) );
			pass_compute_aliasError->DisableDepthBuffer();
			pass_compute_aliasError->SetShader( mat );
			AppendPass("pass_compute_aliasError", pass_compute_aliasError );
		}
		//-----------------------------------------------------------------------------

		//FIXME: Tohle by melo prijit do Init metody dane shadow techniky
		m_shadow_technique->GetShaderFeature()->AddTexture( "MTEX_2Dfunc_values", m_texture_cache->Get("MTEX_2Dfunc_values"), 1.0, ShaderFeature::FS );

		//FIXME: to shadowID by se mohlo/melo nastavovat jinde
		(*ii)->SetShadowTexID( m_texture_cache->Get( "tex_shadow" ) );

		//-----------------------------------------------------------------------------
		//blur
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_aliasblur_horiz","data/shaders/quad.vert","data/shaders/warping/aliasblur.frag", " ","#define HORIZONTAL\n" );
			mat->AddTexture( m_texture_cache->GetPtr( "tex_output" ), "bloom_texture" );
			AddMaterial( mat );

			//-- pass
			SimplePassPtr pass_horiz_blur = new SimplePass( sh_res/8, sh_res/8 );
			pass_horiz_blur->AttachOutputTexture( 0, m_texture_cache->GetPtr( "MTEX_ping" ) );
			pass_horiz_blur->DisableDepthBuffer();
			AppendPass("pass_horiz_blur", pass_horiz_blur );
		}
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_aliasblur_vert","data/shaders/quad.vert","data/shaders/warping/aliasblur.frag", " ", "#define VERTICAL\n" );
			mat->AddTexture( m_texture_cache->GetPtr( "MTEX_ping" ), "bloom_texture" );
			AddMaterial( mat );
			//-- pass
			SimplePassPtr pass_vert_blur = new SimplePass( sh_res/8, sh_res/8 );
			pass_vert_blur->AttachOutputTexture( 0, m_texture_cache->GetPtr( "MTEX_pong" ) );
			pass_vert_blur->DisableDepthBuffer();
			AppendPass("pass_vert_blur", pass_vert_blur );
		}
		//-----------------------------------------------------------------------------
		//alias gradient
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_aliasgradient","data/shaders/quad.vert","data/shaders/warping/aliasgradient.frag" );
			mat->AddTexture( m_texture_cache->GetPtr( "MTEX_pong" ), "tex_blurred_error" );
			AddMaterial( mat );	
			//-- pass
			SimplePassPtr pass_gradient = new SimplePass( sh_res/8, sh_res/8 );
			pass_gradient->AttachOutputTexture( 0, m_texture_cache->GetPtr( "MTEX_ping" ) );
			pass_gradient->DisableDepthBuffer();
			AppendPass("pass_gradient", pass_gradient );
		}
		//-----------------------------------------------------------------------------
		//2D func values
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_get_2Dfunc_values","data/shaders/quad.vert","data/shaders/warping/get2DfuncValues.frag" );
			mat->AddTexture( m_texture_cache->GetPtr( "MTEX_ping" ), "gradient_map" );
			AddMaterial( mat );
			//-- pass
			SimplePassPtr pass_func_values = new SimplePass( m_shadow_technique->GetControlPointsCount(), m_shadow_technique->GetControlPointsCount() );
			pass_func_values->AttachOutputTexture( 0, m_texture_cache->GetPtr( "MTEX_2Dfunc_values" ) );
			pass_func_values->DisableDepthBuffer();
			AppendPass("pass_func_values", pass_func_values );
		}
		//-----------------------------------------------------------------------------
		//draw depth with warping
		{
			ScreenSpaceMaterial* mat = 
				new ScreenSpaceMaterial( 
				"mat_depth_with_warping",
				"data/shaders/warping/drawDepthWithWarping.vert", "data/shaders/warping/drawDepthWithWarping.frag", 
				m_shadow_technique->GetDefines(), "" 
				);
			mat->AddTexture( m_texture_cache->GetPtr("MTEX_2Dfunc_values"), "funcTex" );
			AddMaterial( mat );
			//-- pass
			SimplePassPtr pass_warped_depth = new SimplePass( sh_res, sh_res );
			pass_warped_depth->AttachOutputTexture( 0, m_texture_cache->GetPtr("MTEX_warped_depth_color") );
			pass_warped_depth->AttachOutputTexture( 0, m_texture_cache->GetPtr( "tex_shadow" ), true );
			AppendPass("pass_warped_depth", pass_warped_depth );	
		}
		//-----------------------------------------------------------------------------
		//-- blit pass
		//BlitPass *bp = new BlitPass( 128, 128 );
		//bp->AttachReadTexture( m_texture_cache->GetPtr( "tex_output" ) );
		//bp->AttachDrawTexture( m_texture_cache->GetPtr( "aliaserr_mipmap" ) );
		//AppendPass("pass_blit_0", bp );
		//-- copy squared error
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_copy_squared_error", "data/shaders/quad.vert", "data/shaders/warping/copy_squared_error.frag" );
			mat->AddTexture( m_texture_cache->GetPtr("tex_output"), "tex_alias_error" );
			AddMaterial( mat );

			//-- pass
			SimplePass *mp = new SimplePass( 128, 128 );
			mp->AttachOutputTexture(0, m_texture_cache->GetPtr("aliaserr_mipmap") ); 
			mp->DisableDepthBuffer();
			mp->SetShader( mat );
			AppendPass("pass_copy_squared_error", mp);
		}
		//-----------------------------------------------------------------------------
		//-- mipmap pass
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_aliasMipmap", "data/shaders/quad.vert", "data/shaders/shadow_alias_mipmap.frag" );
			mat->AddTexture( m_texture_cache->GetPtr("aliaserr_mipmap"), "mat_aliasError" );
			AddMaterial( mat );

			//-- pass
			SimplePass *mp = new SimplePass( 128, 128 );
			mp->AttachOutputTexture(0, m_texture_cache->GetPtr("aliaserr_mipmap") ); 
			mp->DisableDepthBuffer();
			AppendPass("pass_alias_mipmap", mp);
		}
		//-----------------------------------------------------------------------------
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "show_depth_omni", "data/shaders/showDepth.vert", "data/shaders/showDepth_omni.frag" ); 
			mat->AddTexture( m_texture_cache->GetPtr("MTEX_warped_depth_color"), "show_depth_dpShadowA" );
			AddMaterial( mat );
		}
		//-----------------------------------------------------------------------------
		{
			//-- shader showing shadow map alias error
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_aliasError", "data/shaders/shadow_alias_error.vert", "data/shaders/shadow_alias_error.frag" );
			mat->AddTexture( m_texture_cache->CreateFromImage( "data/tex/error_color.tga" ), "mat_aliasErrorBaseA" );
			mat->AddTexture( m_texture_cache->GetPtr( "MTEX_2Dfunc_values" ), "MTEX_2Dfunc_values" );
			AddMaterial( mat );
		}
		{
			//-- shader showing perspective alias error
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_perspective_error", "data/shaders/perspective_alias_error.vert", "data/shaders/perspective_alias_error.frag" );
			mat->AddTexture( m_texture_cache->CreateFromImage( "data/tex/error_color.tga" ), "tex_error_color" );
			mat->AddTexture( m_texture_cache->GetPtr( "MTEX_2Dfunc_values" ), "tex_2Dfunc_values" );
			AddMaterial( mat );
		}
	}
	catch(int)
	{
		return false;
	}
	return true;
}

/**
****************************************************************************************************
@brief Render omnidirectional shadow map using multiresolution techniques
@param l current light
****************************************************************************************************/
void TScene::WarpedShadows_RenderShadowMap(TLight *l)
{
	int sh_res = l->ShadowSize();
	GLenum mrt[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

	//set light projection and light view matrix
	glm::mat4 lightProjMatrix = glm::perspective(90.0f, 1.0f, 1.0f, 1000.0f);
	glm::mat4 lightViewMatrix[2];
	lightViewMatrix[1] = glm::lookAt(l->GetPos(), l->GetPos() + glm::vec3(-m_cam->GetFarPlane(), 0.0f, 0.0f ), glm::vec3(0.0f, 1.0f, 0.0f) );
	//glm::mat4 lightViewMatrix = glm::lookAt(l->GetPos(), glm::vec3( 0.0f ), glm::vec3(0.0f, 1.0f, 0.0f) );

	//glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_matrices);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(lightProjMatrix));
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glm::mat4 coeffsX = glm::mat4( 0.0 );
	glm::mat4 coeffsY = glm::mat4( 0.0 );
	glm::vec4 mask_range = glm::vec4( 128.0, 0.0, 128.0, 0.0 );

#ifdef ITERATION_ENABLED
	for( int rep=0; rep<5; ++rep )
#endif

	glClearColor(99.0, 0.0, 0.0, 0.0);

	///////////////////////////////////////////////////////////////////////////////
	//-- 1. Render scene from light point of view and store per pixel camera-screen 
	//--	coordinates and light-screen coordinates
	//FIXME: Jake je rozliseni vystupu???

	//-----------------------------------------------------------------------------

	//if(m_wireframe)
	//    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glEnable( GL_CLIP_PLANE0 );

	//glEnable( GL_CULL_FACE);
	glCullFace(GL_FRONT);

	m_passes["pass_coords"]->Activate();
	MaterialPtr mat_coords = m_passes["pass_coords"]->GetShader();
	mat_coords->SetUniform("cam_mv", m_viewMatrix );
	mat_coords->SetUniform("cam_proj", m_projMatrix );
	mat_coords->SetUniform("near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
	mat_coords->SetUniform("grid_res", (float) m_shadow_technique->GetControlPointsCount() );
	mat_coords->SetUniform("matrix_ortho", glm::ortho(-512.f, 512.f, -512.f, 512.f, 0.1f, 10000.0f) );
	mat_coords->SetUniform("camera_space_light_position", m_viewMatrix * glm::vec4( l->GetPos(), 1.0 )); 

	DrawGeometry(mat_coords->GetName().c_str(), lightViewMatrix[1]);

	m_passes["pass_coords"]->Deactivate();

	//if(!m_wireframe)
	//	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	glCullFace(GL_BACK);
	glDisable( GL_CULL_FACE);

	

	//-----------------------------------------------------------------------------

	//FIXME: tenhle kod by mel prijit do PreRender metody, ale zatim neni vyresen pristup globalni pristup k texturam
	if ( (string) m_shadow_technique->GetName() ==  "Polynomial" )
	{
		float mask_values[128*128];
		glBindTexture(GL_TEXTURE_2D, m_texture_cache->Get( "tex_stencil_color" ) );
		glGetTexImage(GL_TEXTURE_2D, 0, GL_ALPHA, GL_FLOAT, mask_values);

		for(int i=0; i<128; ++i)
			for(int j=0; j<128; ++j)
			{
				float a = mask_values[j + i*128];
				if( a > 0.0 )
				{
					if( j < mask_range.x ) mask_range.x = j; // left border
					if( j > mask_range.y ) mask_range.y = j; // right border
					if( i < mask_range.z ) mask_range.z = i; // bottom border
					if( i > mask_range.w ) mask_range.w = i; // top border
				}

			}

			m_shadow_technique->UpdateGridRange( mask_range );
	}

	///////////////////////////////////////////////////////////////////////////////
	//-- 2. Compute alias error based on the coordinates data computed in step 1
	//--	Input: tex_camAndLightCoords (error texture)

	//FIXME: opravdu mohu pouzit tex_coords v plnem rozliseni, pri mensim rozliseni se bude brat jenom kazda n-ta hodnota
	//		textureOffset se pousouva v rozliseni textury, takze kdyz je rozliseni 1024, posune se o pixel v tomto rozliseni
	//		pri rendrovani do 128x128 je mi toto chovani ale na prd ?????
	m_passes["pass_compute_aliasError"]->Activate();
	m_passes["pass_compute_aliasError"]->Render();
	m_passes["pass_compute_aliasError"]->Deactivate();	

#ifndef GRADIENT_METHOD
	//calculate custom mipmaps 
	{
		//-----------------------------------------------------------------------------

		//m_passes["pass_blit_0"]->Process();
		m_passes["pass_copy_squared_error"]->Activate();
		m_passes["pass_copy_squared_error"]->Render(); //RenderPass("mat_copy_squared_error");		
		m_passes["pass_copy_squared_error"]->Deactivate();

		//-----------------------------------------------------------------------------

		glm::vec4 clear_color;
		glGetFloatv( GL_COLOR_CLEAR_VALUE, glm::value_ptr( clear_color ) );
		glClearColor( 1, 1, 1, 1 );

		glColorMask( 0, 0, 0, 0 );	//-- nutno vypnout, aby se neclearovalo FBO
		m_passes["pass_alias_mipmap"]->Activate();
		glColorMask( 1, 1, 1, 1 );	//-- znovu zapnout

		for(int i=1, j = 128.0/2; j>=1; i++, j/=2)
		{
			glViewport(0, 0, j, j);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_cache->Get("aliaserr_mipmap" ), i);
			glClear(GL_COLOR_BUFFER_BIT );

			SetUniform("mat_aliasMipmap", "offset", 0.5f/((float)j*2.0f));
			SetUniform("mat_aliasMipmap", "mip_level", i-1);
			RenderPass("mat_aliasMipmap");				
		}
	
		m_passes["pass_alias_mipmap"]->Deactivate();


		glClearColor( clear_color.r, clear_color.g, clear_color.b, clear_color.a );

		//-----------------------------------------------------------------------------

		//memset(g_precomputed_diffs, 0, 19*19*sizeof(glm::vec4));
		//ModifyGrid(g_precomputed_diffs);

		glViewport( 0, 0, 128.0, 128.0 ); //restore viewport
	}
#endif


	///////////////////////////////////////////////////////////////////////////////
	//-- 3. Blur the alias error
	//--	input: MTEX_ouput (horiz), MTEX_ping (vert)

	glDisable(GL_DEPTH_TEST);

	float sigma = 2.7;

	m_passes["pass_horiz_blur"]->Activate();

	//GetMaterial( "mat_aliasblur_horiz" )->AddTexture( m_texture_cache->GetPtr( "tex_output" ), "bloom_texture" );
	SetUniform("mat_aliasblur_horiz", "texsize", glm::ivec2(sh_res/8, sh_res/8));
	SetUniform("mat_aliasblur_horiz", "kernel_size", 9.0);
	SetUniform("mat_aliasblur_horiz", "two_sigma_sq", TWOSIGMA2(sigma));
	SetUniform("mat_aliasblur_horiz", "frac_sqrt_two_sigma_sq", FRAC_TWOPISIGMA2(sigma));

	RenderPass("mat_aliasblur_horiz");

	m_passes["pass_horiz_blur"]->Deactivate();

	//-----------------------------------------------------------------------------

	m_passes["pass_vert_blur"]->Activate();

	//GetMaterial( "mat_aliasblur_vert" )->AddTexture( m_texture_cache->GetPtr( "MTEX_ping" ), "bloom_texture" );
	SetUniform("mat_aliasblur_vert", "texsize", glm::ivec2(sh_res/8, sh_res/8));
	SetUniform("mat_aliasblur_vert", "kernel_size", 9.0);
	SetUniform("mat_aliasblur_vert", "two_sigma_sq",TWOSIGMA2(sigma));
	SetUniform("mat_aliasblur_vert", "frac_sqrt_two_sigma_sq", FRAC_TWOPISIGMA2(sigma));

	RenderPass("mat_aliasblur_vert");

	m_passes["pass_vert_blur"]->Deactivate();

	//GetMaterial( "mat_aliasblur_horiz" )->DeleteTexture( "bloom_texture" );
	//GetMaterial( "mat_aliasblur_vert" )->DeleteTexture( "bloom_texture" );

	///////////////////////////////////////////////////////////////////////////////
	//-- 4. Compute gradient and store the result per-pixel into 128x128 texture

	glm::vec2 limit = m_shadow_technique->GetGrid()->GetOffset() / 128.0f;
	limit /= POLY_BIAS;

	m_passes["pass_gradient"]->Activate();

	//FIXME: limit pocitat nejak inteligentne bez bulharske konstanty
	//SetUniform("mat_aliasgradient", "limit", glm::vec2(100.0f));
	SetUniform("mat_aliasgradient", "limit", limit);
	RenderPass("mat_aliasgradient");

	m_passes["pass_gradient"]->Deactivate();

	///////////////////////////////////////////////////////////////////////////////
	//-- 5. get a function value from gradient texture for a given grid (defined by 'range') and store it into 4x4 texture

	glm::vec4 func_range = m_shadow_technique->GetGrid()->GetRangeAsOriginStep();

	glm::vec4 clear_color;
	glGetFloatv( GL_COLOR_CLEAR_VALUE, glm::value_ptr( clear_color ) );
	glClearColor( 0, 0, 0, 0 );

	m_passes["pass_func_values"]->Activate();

	glClearColor( clear_color.r, clear_color.g, clear_color.b, clear_color.a );

	if( m_shadow_technique->IsEnabled() )
	{
		if ( (string) m_shadow_technique->GetName() ==  "Spline" )
		{
			func_range.x +=  func_range.y;
			func_range.z +=  func_range.w;

			glViewport( 0+2, 0+2, m_shadow_technique->GetGrid()->GetControlPointsCount()-2, m_shadow_technique->GetGrid()->GetControlPointsCount()-2 );
			SetUniform("mat_get_2Dfunc_values", "grid_res", (float) m_shadow_technique->GetGrid()->GetControlPointsCount() - 2.0 );
		}
		else
		{
			glClear(GL_COLOR_BUFFER_BIT);
			SetUniform("mat_get_2Dfunc_values", "grid_res", (float) m_shadow_technique->GetGrid()->GetControlPointsCount() );
		}

#ifdef GRADIENT_METHOD
		SetUniform("mat_get_2Dfunc_values", "range", func_range );	
		RenderPass("mat_get_2Dfunc_values");
#else
		glBindTexture( GL_TEXTURE_2D, m_texture_cache->Get( "MTEX_2Dfunc_values" ) );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_shadow_technique->GetControlPointsCount(), m_shadow_technique->GetControlPointsCount(), 0, GL_RGBA, GL_FLOAT, g_precomputed_diffs);
		glBindTexture( GL_TEXTURE_2D, 0 );
#endif

	}

	m_passes["pass_func_values"]->Deactivate();

	if ( (string) m_shadow_technique->GetName() ==  "Spline" )
		//-- 0, 0, 19, 19
		glViewport( 0, 0, m_shadow_technique->GetControlPointsCount(), m_shadow_technique->GetControlPointsCount() );

	//-- Simplified deformation model 
	/*
	sigma = 1.0;
	AddTexture("mat_aliasblur_horiz","MTEX_2Dfunc_values",RENDER_TEXTURE);
	SetUniform("mat_aliasblur_horiz", "texsize", glm::ivec2(m_shadow_technique->GetResolution()));
	SetUniform("mat_aliasblur_horiz", "kernel_size", 3.0);
	SetUniform("mat_aliasblur_horiz", "two_sigma_sq", TWOSIGMA2(sigma));
	SetUniform("mat_aliasblur_horiz", "frac_sqrt_two_sigma_sq", FRAC_TWOPISIGMA2(sigma));
	AddTexture("mat_aliasblur_vert","MTEX_2Dfunc_values_ping",RENDER_TEXTURE);
	SetUniform("mat_aliasblur_vert", "texsize", glm::ivec2(m_shadow_technique->GetResolution()));
	SetUniform("mat_aliasblur_vert", "kernel_size", 3.0);
	SetUniform("mat_aliasblur_vert", "two_sigma_sq", TWOSIGMA2(sigma));
	SetUniform("mat_aliasblur_vert", "frac_sqrt_two_sigma_sq", FRAC_TWOPISIGMA2(sigma));

	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, m_tex_cache["MTEX_2Dfunc_values_ping"], 0);
	glClear(GL_COLOR_BUFFER_BIT);
	RenderPass("mat_aliasblur_horiz");
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, m_tex_cache["MTEX_2Dfunc_values"], 0);
	glClear(GL_COLOR_BUFFER_BIT);
	RenderPass("mat_aliasblur_vert");

	RemoveTexture("mat_aliasblur_horiz","MTEX_2Dfunc_values");
	RemoveTexture("mat_aliasblur_vert","MTEX_2Dfunc_values_ping");
	*/

	///////////////////////////////////////////////////////////////////////////////
	//-- 6. compute 2D polynomial coefficents and store them into textures (gradient for x and y axes)
	//-- odtud zacina cast, ktera je pro kazdou warpovaci metodu jina. Proto se pouziva abstraktni trida shadow_technique

	m_shadow_technique->SetTexId(m_texture_cache->Get( "MTEX_2Dfunc_values" ));
	m_shadow_technique->PreRender();

	///////////////////////////////////////////////////////////////////////////////

#ifdef ITERATION_ENABLED
	RenderDebug();

	cout << rep << endl;
	SDL_Delay( 100 );
	//std::string dummy;
	//std::getline(std::cin, dummy);

	glViewport(0,0,1024,1024);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureCache::Instance()->Get("aliaserr_texture"));                
	RenderPass("mat_quad");
	glBindTexture(GL_TEXTURE_2D, 0);

	m_shadow_technique->DrawGrid();

	SDL_GL_SwapBuffers();
#endif
	glEnable(GL_DEPTH_TEST);

#ifdef ITERATION_ENABLED
	cout << endl;
#endif
	glViewport( 0, 0, sh_res, sh_res );
	//glColorMask(0, 0, 0, 0);      //disable colorbuffer write
	glCullFace(GL_FRONT);
	glEnable(GL_CLIP_PLANE0);

	if(m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	m_passes["pass_warped_depth"]->Activate();

	for(int i=0; i<2; i++)
	{
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,  m_texture_cache->Get("MTEX_warped_depth_color"), 0, i);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  m_texture_cache->Get("tex_shadow"), 0, i);	    

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//-- Shared uniforms
		SetUniform("mat_depth_with_warping", "near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
		SetUniform("mat_depth_with_warping", "range", m_shadow_technique->GetGridRange());
		SetUniform("mat_depth_with_warping", "grid_res", (float) m_shadow_technique->GetControlPointsCount() );

		//-- Polynomial uniforms
		SetUniform("mat_depth_with_warping", "coeffsX", coeffsX );
		SetUniform("mat_depth_with_warping", "coeffsY", coeffsY );

		//-- Bilinear uniforms
		//SetUniform("mat_depth_with_warping", "funcTex", 1 );

		float z_direction = 1.0;
		if(i == 1)
			z_direction = -1.0;  
		lightViewMatrix[i] = glm::lookAt(l->GetPos(), l->GetPos() + glm::vec3( m_cam->GetFarPlane()*z_direction, 0.0f, 0.0f ), glm::vec3(0.0f, 1.0f, 0.0f) );


		if(m_dpshadow_tess)
		{
			SetUniform("_mat_shadow_warp_tess", "near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
			SetUniform("_mat_shadow_warp_tess", "coeffsX", coeffsX );
			SetUniform("_mat_shadow_warp_tess", "coeffsY", coeffsY );

			DrawSceneDepth("_mat_shadow_warp_tess", lightViewMatrix[i]);
		}
		else
		{
			DrawGeometry("mat_depth_with_warping", lightViewMatrix[i]);
		}
	}
	m_passes["pass_warped_depth"]->Deactivate();

	if(!m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	//Finish, restore values
	glDisable( GL_CLIP_PLANE0 );
	glCullFace(GL_BACK);
	glColorMask(1, 1, 1, 1);


#ifdef ITERATION_ENABLED
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbos["ipsm"]);		
	glViewport( 0, 0, m_shadow_technique->GetResolution(), m_shadow_technique->GetResolution() );
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, m_tex_cache["MTEX_2Dfunc_values"], 0);
	glClear(GL_COLOR_BUFFER_BIT );
	glBindFramebuffer( GL_FRAMEBUFFER, 0);
#endif

	///////////////////////////////////////////////////////////////////////////////

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glViewport(0,0,sh_res,sh_res);
	//RenderPass("mat_quad");

	//end draw_aliaserror

	/*
	//Finish, restore values
	glCullFace(GL_BACK);
	glColorMask(1, 1, 1, 1);
	glViewport(0, 0, m_resx, m_resy);         //reset viewport

	///3. Calculate shadow texture matrix
	glm::mat4 shadowMatrix = biasMatrix * lightProjMatrix * lightViewMatrix * glm::inverse(m_viewMatrix);

	//restore projection matrix and set shadow matrices
	glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_matrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_projMatrix));
	glBufferSubData(GL_UNIFORM_BUFFER, (l->GetOrd() + 1) * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(shadowMatrix));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	*/

	//set light matrices and near/far planes to all materials
	for(m_im = m_materials.begin(); m_im != m_materials.end(); ++m_im)
	{
		if(m_im->second->GetSceneID() == m_sceneID && !m_im->second->IsScreenSpace())
		{
			m_im->second->SetUniform("lightModelView[0]", lightViewMatrix[0]);
			m_im->second->SetUniform("lightModelView[1]", lightViewMatrix[1]);
			m_im->second->SetUniform("near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
			m_im->second->SetUniform("coeffsX", coeffsX );
			m_im->second->SetUniform("coeffsY", coeffsY );
			m_im->second->SetUniform("range", m_shadow_technique->GetGridRange());
			m_im->second->SetUniform("grid_res", (float) m_shadow_technique->GetControlPointsCount());
		}
	}

	//-- set debug shaders
	SetUniform("mat_aliasError", "lightModelView[0]", lightViewMatrix[0]);
	SetUniform("mat_aliasError", "lightModelView[1]", lightViewMatrix[1]);
	SetUniform("mat_aliasError", "near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
	SetUniform("mat_aliasError", "grid_res", (float) m_shadow_technique->GetControlPointsCount());
	SetUniform("mat_aliasError", "range", m_shadow_technique->GetGridRange());
	//-- set debug shaders
	//SetUniform("mat_perspective_error", "lightModelView", glm::lookAt( l->GetPos(), glm::vec3( 0.0f ), glm::vec3(0.0f, 1.0f, 0.0f) ));
	SetUniform("mat_perspective_error", "lightModelView[0]", lightViewMatrix[0]);
	SetUniform("mat_perspective_error", "lightModelView[1]", lightViewMatrix[1]);
	SetUniform("mat_perspective_error", "near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
	SetUniform("mat_perspective_error", "camera_space_light_position", m_viewMatrix * glm::vec4( l->GetPos(), 1.0 ));
	SetUniform("mat_perspective_error", "matrix_ortho", glm::ortho(-512.f, 512.f, -512.f, 512.f, 0.1f, 10000.0f) );
}
