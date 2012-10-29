#ifndef _CCITY_H_
#define _CCITY_H_

#include<glm/glm.hpp>
#include<vector>
using namespace std;
#include"Gen.h"

/**
 * @brief This class represents street of the city
 */
class CStreet{
	public:
		/**
		 * @brief Constructor of the street
		 *
		 * @param Start Start point of the street.
		 * @param End End point of the street.
		 * @param Width Width of the street.
		 */
		CStreet(glm::vec3 Start,glm::vec3 End,float Width);
		/**
		 * @brief This function splits street in given point.
		 *
		 * @param A First piece of the street
		 * @param B Second piece of the street
		 * @param t When t==0 then it's Start point, when t==1 then it's End point
		 */
		void Split(CStreet**A,CStreet**B,float t);
		/**
		 * @brief This function returns length of the street
		 *
		 * @return length of the street
		 */
		float Length();
		/**
		 * @brief This funcion returns position on the street
		 *
		 * @param t t==0 Start, t==1 End
		 *
		 * @return (1-t)*Start+(t)*End
		 */
		glm::vec3 GetPos(float t);
		glm::vec3 Start,End;/// start and end point of the street
		float Width;/// width of the street
};

/**
 * @brief This structure represents template for generating streetnet of the city
 */
typedef struct{
	unsigned Seed;/// seed
	unsigned NumStreet;/// number of streets
	float MinLen;/// minimum length of street
	float MinAngle;/// minimum angle between streets
	float MinSplitLen;/// minimum lenght of street which allows splitting
	CRandRange StreetWidth;/// width of the streets
}SStreetNetTemplate;

/**
 * @brief This class represents net of streets in the city
 */
class CStreetNet{
	public:
		/**
		 * @brief Constructor of the street net
		 *
		 * @param Template template of street net
		 */
		CStreetNet(SStreetNetTemplate Template);
		/**
		 * @brief Destructor
		 */
		~CStreetNet();
		/**
		 * @brief Determines collision between 2 streets
		 *
		 * @param A street A
		 * @param B street B
		 * @param t t[0]==0 -> A.Start, t[0]==1 -> A.End; t[1]==0 -> B.Start t[1]==1 -> B.End
		 *
		 * @return 1 if there is collision; 0 otherwise
		 */
		int IsStreetCollision(CStreet*A,CStreet*B,float*t);
		void Draw();
		vector<CStreet*>Streets;///length-sorted list of streets
	private:
		
};

/**
 * @brief This structure prepresents template for building
 */
typedef struct{
	CRandRange XSize,YSize,ZSize;/// size of the building
}SBuildingTemplate;

/**
 * @brief This class represents one building
 */
class CBuilding{
	public:
		glm::vec3 Start;/// left back down corner of the building (-x,-y,-z)
		glm::vec3 Size[3];/// sizeof the building - 3 orthogonal vectors
		/**
		 * @brief Constructor of the building
		 *
		 * @param Start left back down corner of the building
		 * @param Size size of the building
		 * @param Xvec direction of the street
		 */
		CBuilding(glm::vec3 Start,glm::vec3 Size,glm::vec3 Xvec);
		/**
		 * @brief Determines if there is collision between buildings
		 *
		 * @param B another building
		 *
		 * @return 1 -> collision, 0 otherwise
		 */
		int IsCollision(CBuilding*B);
		/**
		 * @brief Determines if there is collsion between building and street
		 *
		 * @param S Street
		 *
		 * @return 1 -> collision, 0 otherwise
		 */
		int IsCollision(CStreet*S);
		float AngleY();
		void Draw();
};

/**
 * @brief This class represents city
 */
class CCity{
	public:
		/**
		 * @brief Constructor of the city
		 *
		 * @param Net street net
		 * @param Template template of the building
		 */
		CCity(CStreetNet*Net,SBuildingTemplate Template);
		/**
		 * @brief Destructor
		 */
		~CCity();
		void Draw();
		CStreetNet*Net;
		vector<CBuilding*>Buildings;
};

#endif//_CCITY_H_
