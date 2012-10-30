#include "Box.h"


Box::Box(void)
{
}


Box::~Box(void)
{
}

void Box::calcPlanes()
{
	//TODO - rewind!
	/*
	pl[COORDS::TOP].setPoints(ntr,ntl,ftl);
	pl[COORDS::BOTTOM].setPoints(nbl,nbr,fbr);
	pl[COORDS::LEFT].setPoints(ntl,nbl,fbl);
	pl[COORDS::RIGHT].setPoints(nbr,ntr,fbr);
	pl[COORDS::NEARP].setPoints(ntl,ntr,nbr);
	pl[COORDS::FARP].setPoints(ftr,ftl,fbl);
	*/
	pl[COORDS::TOP].setPoints(ntr,ftl, ntl);
	pl[COORDS::BOTTOM].setPoints(nbl, fbr, nbr);
	pl[COORDS::LEFT].setPoints(ntl, fbl, nbl);
	pl[COORDS::RIGHT].setPoints(nbr, fbr, ntr);
	pl[COORDS::NEARP].setPoints(ntl ,nbr, ntr);
	pl[COORDS::FARP].setPoints(ftr, fbl, ftl);
}

std::vector<glm::vec3> Box::getPoints()
{
	std::vector<glm::vec3> pts;
	pts.push_back(ntl);
	pts.push_back(ntr);
	pts.push_back(nbl);
	pts.push_back(nbr);
	pts.push_back(ftl);
	pts.push_back(ftr);
	pts.push_back(fbl);
	pts.push_back(fbr);

	return pts;
}

Box::boxTest Box::testPoint(glm::vec3& point)
{
	boxTest result = INSIDE;

	for(int i = 0; i<6; ++i)
	{
		if(pl[i].distancePoint(point) < 0)
			return OUTSIDE;
	}

	return result;
}

//Complex line intersection test
bool Box::testLine(glm::vec3& p0, glm::vec3& p1)
{
	const float fFrustumEdgeFudge = 0.01f;  //  kvoli presnosti
    unsigned char outCode[2] = { 0, 0 };

    for ( int i=0; i<6; i++ )
    {
        float aDist = pl[i].distancePoint(p0);
        float bDist = pl[i].distancePoint(p1);
        int aSide = (aDist <= 0) ? 1 : 0;
        int bSide = (bDist <= 0) ? 1 : 0;

        outCode[0] |= (aSide << i);  // outcode stores the plane sidedness for all 6 clip planes
        outCode[1] |= (bSide << i);  // if outCode[0]==0, vertex is inside frustum

        if ( aSide & bSide )   // trivial reject test (both points outside of one plane)
            return false;

        if (aSide ^ bSide)      // complex intersection test
        {
            glm::vec3 rayDir;

			rayDir = p1 - p0;

            float t = rayPlaneIntersect( pl[i].getPlane(), p0, rayDir );
            if ((t>=0.f) && (t<=1.f))
            {
				glm::vec3 hitPt = p0 + t*rayDir;

				BoundingSphere s;
				s.setCenter(hitPt);
				s.setRadius(fFrustumEdgeFudge);

                if (testSphere(s)!=OUTSIDE)
                    return true;  //pretina frustum
            }
        }
    }

    return (outCode[0]==0) || (outCode[1]==0);
}

Box::boxTest Box::testSphere(BoundingSphere& sphere)
{
	float distance;
	float radius = sphere.getRadius();
	boxTest result = INSIDE;

	for(int i=0; i < 6; i++) 
	{
		distance = pl[i].distancePoint(sphere.getCenter());
		if (distance < -radius)
		{
			return OUTSIDE;
		}
		else if (distance < radius)
		{
			result =  INTERSECT;
		}
	}

	return result;
}

bool Box::testBox(Box& f)
{
	bool intersectA = (testLine( f.fbl, f.nbl )) ||
		(testLine( f.nbl, f.nbr )) ||
		(testLine( f.nbr, f.fbr )) ||
		(testLine( f.fbr, f.fbl )) ||
		(testLine( f.fbr, f.ftr )) ||
		(testLine( f.nbr, f.ntr )) ||
		(testLine( f.fbl, f.ftl )) ||
		(testLine( f.nbl, f.ntl )) ||
		(testLine( f.ntl, f.ftl )) ||
		(testLine( f.ftl, f.ftr )) ||
		(testLine( f.ftr, f.ntr )) ||
		(testLine( f.ntr, f.ntl ));

	bool intersectB = (f.testLine( fbl, nbl )) ||
		(f.testLine( nbl, nbr )) ||
		(f.testLine( nbr, fbr )) ||
		(f.testLine( fbr, fbl )) ||
		(f.testLine( fbr, ftl )) ||
		(f.testLine( nbr, ntr )) ||
		(f.testLine( fbl, ftl )) ||
		(f.testLine( nbl, ntl )) ||
		(f.testLine( ntl, ftl )) ||
		(f.testLine( ftl, ntr )) ||
		(f.testLine( ftr, ntr )) ||
		(f.testLine( ntr, ntl ));
	
	return intersectA || intersectB;
 }

float Box::rayPlaneIntersect(glm::vec4& plane, glm::vec3& org, glm::vec3& dir )
{
    float distance = glm::dot( plane, glm::vec4(org.x, org.y, org.z, 1.0f));
	float len = glm::dot( glm::vec3(plane.x, plane.y, plane.z), dir);

    return -distance / len;
}