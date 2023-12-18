#include "ofApp.h"
#include <cmath>
#include <iostream>
#include <string>

#define EPSILON 0.001f


// Get texture from information
//
void SceneObject::getTextureColor(const glm::vec3 &point, ofColor &baseColor, ofColor &specularColor)
{
	// Return the diffuse if texture is not present
	if (!isTextured)
	{
		baseColor = this->diffuseColor;
		specularColor = this->specularColor;
		return;
	}
	
	// Get relative positions
	glm::vec2 uv;
	this->evaluatePoint(point, uv);

	// Calculate point on image
	int i = (int) fmod(uv.x * this->texture.getWidth() + 0.5f, this->texture.getWidth());
	int j = (int) fmod(uv.y * this->texture.getHeight() + 0.5f, this->texture.getHeight()); 

	// Get color
	baseColor = this->texture.getColor(i, j);
	specularColor = this->specular.getColor(i, j);
}

// Intersect Ray with Plane (wrapper on glm::intersect*)
//
bool Plane::intersect(const Ray& ray, glm::vec3& point, glm::vec3& normalAtIntersect)
{
	float dist;
	bool insidePlane = false;
	bool hit = glm::intersectRayPlane(ray.p, ray.d, position, this->normal, dist);
	if (hit)
	{
		Ray r = ray;
		point = r.evalPoint(dist);
		normalAtIntersect = this->normal;
		glm::vec2 xrange = glm::vec2(position.x - width / 2, position.x + width / 2);
		glm::vec2 yrange = glm::vec2(position.y - width / 2, position.y + width / 2);
		glm::vec2 zrange = glm::vec2(position.z - height / 2, position.z + height / 2);
		// horizontal
		//
		if (normal == glm::vec3(0, 1, 0) || normal == glm::vec3(0, -1, 0))
		{
			if (point.x < xrange[1] && point.x > xrange[0] && point.z < zrange[1] && point.z > zrange[0])
			{
				insidePlane = true;
			}
		}
		// front or back
		//
		else if (normal == glm::vec3(0, 0, 1) || normal == glm::vec3(0, 0, -1))
		{
			if (point.x < xrange[1] && point.x > xrange[0] && point.y < yrange[1] && point.y > yrange[0])
			{
				insidePlane = true;
			}
		}
		// left or right
		//
		else if (normal == glm::vec3(1, 0, 0) || normal == glm::vec3(-1, 0, 0))
		{
			if (point.y < yrange[1] && point.y > yrange[0] && point.z < zrange[1] && point.z > zrange[0])
			{
				insidePlane = true;
			}
		}
	}
	return insidePlane;
}

// Convert (u, v) to (x, y, z) 
// We assume u,v is in [0, 1]
//
glm::vec3 ViewPlane::toWorld(float u, float v)
{
	float w = width();
	float h = height();
	return (glm::vec3((u * w) + min.x, (v * h) + min.y, position.z));
}

// Get a ray from the current camera position to the (u, v) position on
// the ViewPlane
//
Ray RenderCam::getRay(float u, float v)
{
	glm::vec3 pointOnPlane = view.toWorld(u, v);
	return(Ray(position, glm::normalize(pointOnPlane - position)));
}

// Lambert shading
//
ofColor ofApp::lambert(const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse)
{
	// Beginning color: (0, 0, 0)
	ofColor resultColor = ofColor::black;

	// Iterate over all the lights
	for (Light* light : lights)
	{
		// Skip light calculations if in shade
		vector<Ray> lightRays;
		int numSamples = light->getRaySamples(p, lightRays);
		for (Ray lightRay : lightRays)
		{
			// Calculate shadows
			if (!isShadow(p, lightRay))
			{
				resultColor += max(0.0f, glm::dot(glm::normalize(norm), glm::normalize(-lightRay.d)))
						       * light->intensity / pow(glm::length(lightRay.d), 2) / numSamples
						       * diffuse * lambertCoefficient;	
			}
		}
	}

	// Return the sum of color contributions
	return resultColor;
}

// Phong shading
ofColor ofApp::phong(const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse,
				     const ofColor specular, float power)
{
	// Beginning color: (0, 0, 0)
	ofColor resultColor = ofColor::black;

	// Iterate over all the lights
	for (Light* light : lights)
	{
		// Skip light calculations if in shade
		vector<Ray> lightRays;
		int numSamples = light->getRaySamples(p, lightRays);
		for (Ray lightRay : lightRays)
		{
			// Calculate shadows
			if (!isShadow(p, lightRay))
			{
				glm::vec3 viewerDirection = renderCam.position - p;

				// Normal vectors have length 1, sum of two lengths is 2
				glm::vec3 bisector = (glm::normalize(-lightRay.d) + glm::normalize(viewerDirection)) / 2.0f;
				
				// Divide by number samples so that more samples does not increase brightness
				resultColor += glm::pow(max(0.0f, glm::dot(glm::normalize(norm), glm::normalize(bisector))), power)
							   * light->intensity / pow(glm::length(lightRay.d), 2) / numSamples
							   * specular;
			}
		}
	}

	// Return the sum of phong colors added to lambert
	return resultColor + lambert(p, norm, diffuse);
}

// Shadows
bool ofApp::isShadow(const glm::vec3 &p, const Ray &lightRay)
{
	// Create new ray from point to light
	Ray rayToLight(p + EPSILON * glm::normalize(-lightRay.d), glm::normalize(-lightRay.d));

	glm::vec3 tempPoint(0.0f, 0.0f, 0.0f);
	glm::vec3 tempNormal(0.0f, 0.0f, 0.0f);

	for (SceneObject* obj : scene)
	{
		// If the intersecting object is closer
		if (obj->intersect(rayToLight, tempPoint, tempNormal) &&
			glm::length(tempPoint - p) < glm::length(lightRay.p - p))
		{
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------
// Main raytrace loop
//
void ofApp::rayTrace()
{
	image.allocate(imageWidth, imageHeight, OF_IMAGE_COLOR);
	for (int i = 0; i < imageWidth; i++)
	{
		for (int j = 0; j < imageHeight; j++)
		{
			float u = (i + 0.5) / imageWidth;
			float v = (j + 0.5) / imageHeight;

			Ray ray = renderCam.getRay(u, v);
			
			// High enough to be large, small enough to plug into evalPoint
			float distance = 10000.0;
			SceneObject* closestObject = nullptr;

			// Fpr lighting purposes
			glm::vec3 maxPoint;
			glm::vec3 maxNormal;

			for (SceneObject* obj : scene)
			{
				glm::vec3 point;
				glm::vec3 normal;
				if (obj->intersect(ray, point, normal))
				{
					// Only need to compare one coordinate
					if (abs(ray.evalPoint(distance).x - ray.p.x) > abs(point.x - ray.p.x))
					{
						distance = glm::length(point - ray.p);
						closestObject = obj;
						maxPoint = point;
						maxNormal = normal;
					}

				}
			}
			// Draw on image
			if (closestObject != nullptr)
			{
				// Get color
				ofColor baseColor;
				ofColor specularColor;
				closestObject->getTextureColor(maxPoint, baseColor, specularColor);

				// Calculate raytraced color
				ofColor phongColor = phong(maxPoint, maxNormal, baseColor, specularColor, phongPower);
				image.setColor(i, imageHeight - j - 1, phongColor);
			}
			else
			{
				image.setColor(i, imageHeight - j - 1, backgroundColor);
			}
		}
	}
	// image.mirror(true, false);
	image.save(ofToDataPath("image.jpg"));
}

//--------------------------------------------------------------
void ofApp::setup()
{
	// Set position of cameras
	mainCam.setGlobalPosition(glm::vec3(0, 0, 20));
	mainCam.lookAt(glm::vec3(0, 0, 0));

	// View plane starts at z=5
	previewCam.setGlobalPosition(glm::vec3(0, 0, 5));
	previewCam.lookAt(glm::vec3(0, 0, 0));
	float angle = glm::orientedAngle(glm::normalize(glm::vec3(-3, 0, 5)),
							 		 glm::normalize(glm::vec3(3, 0, 5)),
							 		 glm::normalize(glm::vec3(0, 0, 10)));
	previewCam.setFov(180 * angle / PI);

	sideCam.setGlobalPosition(glm::vec3(-20, 0, -20));
	sideCam.lookAt(glm::vec3(0, 0, -20));

	// Set default camera
	theCam = &mainCam;

	// Add spheres
	Sphere* s1 = new Sphere(glm::vec3(2, 2.5, -5), 2.0, ofColor::darkSlateGray);
	Sphere* s2 = new Sphere(glm::vec3(-0.5, 2, -2.5), 1.5, ofColor::gray);
	Plane* p1 = new Plane(glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 1.0, 0.0),
						  "paving_concrete.jpg", "paving_concrete_specular.jpg",
						  20, 40, 8, 8);
	Plane* p2 = new Plane(glm::vec3(0.0, 9.0, -10.0), glm::vec3(0.0, 0.0, 1.0),
						  "stone_block.jpg", "stone_block_specular.jpg",
						  20, 20, 8, 8);
	Plane* p3 = new Plane(glm::vec3(-10.0, 9.0, 0.0), glm::vec3(1.0, 0.0, 0.0),
						  "rustic_bricks.jpg", "rustic_bricks_specular.jpg",
						  20, 40, 4, 8);
	Plane* p4 = new Plane(glm::vec3(10.0, 9.0, 0.0), glm::vec3(-1.0, 0.0, 0.0),
						  "rustic_bricks.jpg", "rustic_bricks_specular.jpg",
						  20, 40, 4, 8);

	scene.push_back(s1);
	scene.push_back(s2);
	scene.push_back(p1);
	scene.push_back(p2);
	scene.push_back(p3);
	scene.push_back(p4);

	// Lights
	PointLight* l2 = new PointLight(glm::vec3(-3.0, 2.0, 0.0), 0.0);
	PointLight* l3 = new PointLight(glm::vec3(4.0f, 0.5f, 4.0f), 30.0f);

	AreaLight* a1 = new AreaLight(glm::vec3(0.0f, 20.0f, 0.0f), 300.0f, 10.0f, 10.0f, 5, 5, 1);

	scene.push_back(l2);
	scene.push_back(l3);
	scene.push_back(a1);

	lights.push_back(l2);
	lights.push_back(l3);
	lights.push_back(a1);

	// Gui
	gui.setup();
	gui.add(phongPower.set("Phong Power", this->phongPower, 1.0f, 100.0f));
	gui.add(lambertCoefficient.set("Lambert Coefficient", this->lambertCoefficient, 0.0f, 2.0f));

	// Add individual light guis to main gui
	int numLights = 1;
	for (Light* light : lights)
	{
		gui.add(light->gui.setup("Light" + std::to_string(numLights)));
		light->setupGui();
		numLights++;
	}
}

//--------------------------------------------------------------
void ofApp::update()
{
	
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofEnableDepthTest();
	theCam->begin();
	for (SceneObject* obj : scene)
	{
		ofSetColor(obj->diffuseColor);
		obj->draw();
	}
	theCam->end();
	// Draw image after cam end
	if (bDrawImage)
	{
		ofDisableDepthTest();
		ofSetColor(ofColor::white);
		image.update();
		image.draw(0.0f, 0.0f);
		ofEnableDepthTest();
	}
	// Draw Gui
    if (!hideGui)
    {
        ofDisableDepthTest();

		// Draw main gui
        gui.draw();

        ofEnableDepthTest();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	switch (key)
	{
		case 'r':
		{
			rayTrace();
			break;
		}
		case 'd':
		{
			if (image.isAllocated())
			{
				// Only draws image if it is allocated
				bDrawImage = !bDrawImage;
			}
			break;
		}
		case 'h':
        {
            hideGui = !hideGui;
            break;
        }
		case OF_KEY_F1:
		{
			theCam = &mainCam;
			break;
		}
		case OF_KEY_F2:
		{
			theCam = &sideCam;
			break;
		}
		case OF_KEY_F3:
		{
			theCam = &previewCam;
			break;
		}
		default:
		{
			break;
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}
