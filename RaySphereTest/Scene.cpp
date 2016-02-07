#include "Scene.h"
#include <fstream>

////////////////////////////////////////////////////////////////////////////////////////////

void Scene::GenerateObjects()
{
	m_numberOfRays = GetRG()->GetRandomInt(0, 300);
	m_numberOfSpheres = GetRG()->GetRandomInt(0, 20000);

	m_rays = new Ray[m_numberOfRays];
	m_spheres= new Sphere[m_numberOfSpheres];

	for (int i = 0; i < m_numberOfRays; i++)
	{
		Vector3D pos = GetRG()->GetRandomPosition(0, 100);
		Vector3D dir = GetRG()->GetRandomPosition(0, 100);

		m_rays[i] = Ray(pos, dir);
	}

	for (int i = 0; i < m_numberOfSpheres; i++)
	{
		Vector3D pos = GetRG()->GetRandomPosition(0, 100);
		float radius = GetRG()->GetRandomFloat(0, 2);

		m_spheres[i] = Sphere(pos, radius);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

void Scene::CheckForIntersections(bool bWithAllSpheres, BVH* bvh)
{
	std::ofstream stream;
	stream.open("logRegular.txt");

	std::ofstream stream1;
	stream1.open("logBVH.txt");

	int intersectionsAmount = 0;

	stream << "Total rays: " << m_numberOfRays << " Total spheres: " << m_numberOfSpheres << std::endl;
	stream1 << "Total rays: " << m_numberOfRays << " Total spheres: " << m_numberOfSpheres << std::endl;
	//test
	//Ray r(Vector3D(0, 0, 0), Vector3D(1, 1, 0));
	//Sphere s(Vector3D(10, 8, 0), 2);

	//IntersectionInfo info1;

	//s.GetIntersection(r, info1);

	for (int i = 0; i < m_numberOfRays; i++)
	{
		std::vector<IntersectionInfo> intersInfo;
		bool bIntersect = RayBVHTest(&m_rays[i], bvh, bvh->m_rootNode, intersInfo);

		if (bIntersect)
		{
			for (auto& it : intersInfo)
			{
				stream1 << intersectionsAmount << ": Ray " << i << " start:" << m_rays[i].m_startPos << " dir:" << m_rays[i].m_direction;
				stream << "Sphere " << " c:" << it.m_object->m_center << " rad:" << it.m_object->m_radius;
				stream1 << " Inters. point:" << it.m_intersPoint << std::endl;
			}
		}
	}

	for (int i = 0; i < m_numberOfRays; i++)
	{
		for (int j = 0; j < m_numberOfSpheres; j++)
		{
			Vector3D resultPoint(0, 0, 0);

			IntersectionInfo info;
			
			bool intersect = m_spheres[j].GetIntersection(&m_rays[i], info);

			if (intersect)
			{
				stream << intersectionsAmount << ": Ray " << i << " start:" << m_rays[i].m_startPos << " dir:" << m_rays[i].m_direction;
				stream << "Sphere " << j << " c:" << m_spheres[j].m_center << " rad:" << m_spheres[j].m_radius;
				stream << " Inters. point:" << info.m_intersPoint << std::endl;
				
				intersectionsAmount++;
				/*if(!bWithAllSpheres)
				{
					break;
				}*/
			}
		}
	}

	stream.close();
}

////////////////////////////////////////////////////////////////////////////////////////////

bool Scene::RayBVHTest(Ray* ray, BVH* bvh, BVHNode* node, std::vector<IntersectionInfo>& intersectInfo)
{
	if( node )
	{
		if (node->m_bIsLeaf)
		{
			bool bAnyIntersects = false;
			for (int i = node->m_index; i < node->m_index + node->m_amountOfObjectsInNode; i++)
			{
				IntersectionInfo info;
				bool bIntersect = bvh->m_objects[i]->GetIntersection(ray, info);

				if (bIntersect)
				{
					bAnyIntersects = true;
					intersectInfo.push_back(info);
				}
			}

			//if we had atleast one intersect return true
			if (bAnyIntersects)
			{
				return true;
			}
		}

		IntersectionInfo info;

		bool bIntersect = node->m_boundingSphere.GetIntersection(ray, info);

		if (bIntersect)
		{
			RayBVHTest(ray, bvh, node->m_leftNode, intersectInfo);
			RayBVHTest(ray, bvh, node->m_rightNode, intersectInfo);
		}

		return false;
	}
	
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////