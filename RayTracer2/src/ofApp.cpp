#include "ofApp.h"
#include <cmath>
#include <iostream>

#define EPSILON 0.001f

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
	ofColor resultColor = ofColor::black;
	// Iterate over all the lights
	for (Light* light : lights)
	{
		glm::vec3 lightDirection = light->position - p;

		// Skip light calculations if in shade
		if (!isShadow(p, lightDirection))
		{
			resultColor += max(0.0f, glm::dot(glm::normalize(norm), glm::normalize(lightDirection)))
					   * light->intensity / pow(glm::length(lightDirection), 2) * lambertCoefficient
					   * diffuse;	
		}
	}
	return resultColor;
}

// Phong shading
ofColor ofApp::phong(const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse,
				     const ofColor specular, float power)
{
	ofColor resultColor = ofColor::black;
	// Iterate over all the lights
	for (Light* light : lights)
	{
		glm::vec3 lightDirection = light->position - p;
		glm::vec3 viewerDirection = renderCam.position - p;

		// Skip light calculations if in shade
		if (!isShadow(p, lightDirection))
		{
			// Normal vectors have length 1, sum of two lengths is 2
			glm::vec3 bisector = (glm::normalize(lightDirection) + glm::normalize(viewerDirection)) / 2.0f;
			
			resultColor += glm::pow(max(0.0f, glm::dot(glm::normalize(norm), glm::normalize(bisector))), power)
						   * light->intensity / pow(glm::length(lightDirection), 2)
						   * specular;
		}
	}
	return resultColor + lambert(p, norm, diffuse);
}

// Shadows
bool ofApp::isShadow(const glm::vec3 &p, const glm::vec3 &lightDirection)
{
	glm::vec3 lightDirectionNormed = glm::normalize(lightDirection);
	Ray lightRay(p + (EPSILON * lightDirectionNormed), lightDirectionNormed);
	for (SceneObject* obj : scene)
	{
		glm::vec3 tempPoint(0.0f, 0.0f, 0.0f);
		glm::vec3 tempNormal(0.0f, 0.0f, 0.0f);
		if (obj->intersect(lightRay, tempPoint, tempNormal) &&
			glm::length(lightDirection) > glm::length(tempPoint - p))
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
				ofColor phongColor = phong(maxPoint, maxNormal, closestObject->diffuseColor, ofColor::white, phongPower);
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
	Sphere* s1 = new Sphere(glm::vec3(2, 3.5, -6), 2.0, ofColor::red);
	Sphere* s2 = new Sphere(glm::vec3(-0.5, 3, -3.5), 1.5, ofColor::yellow);
	Sphere* s3 = new Sphere(glm::vec3(1, 2, -1.5), 1.0, ofColor::green);
	Plane* p1 = new Plane(glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 1.0, 0.0), ofColor::darkGray);
	Plane* p2 = new Plane(glm::vec3(0.0, 0.0, -10.0), glm::vec3(0.0, 0.0, 1.0), ofColor::lightCoral);

	scene.push_back(s1);
	scene.push_back(s2);
	scene.push_back(s3);
	scene.push_back(p1);
	scene.push_back(p2);

	lightIntensity = 20.0f;

	// Lights
	Light* l1 = new Light(glm::vec3(4, 6, 0), lightIntensity);
	Light* l2 = new Light(glm::vec3(-4, 6, 0), lightIntensity);
	Light* l3 = new Light(glm::vec3(4, 0.5, 4), lightIntensity);

	scene.push_back(l1);
	scene.push_back(l2);
	scene.push_back(l3);

	lights.push_back(l1);
	lights.push_back(l2);
	lights.push_back(l3);

	// Gui
	gui.setup();
	gui.add(lightIntensitySelect.setup("Light Intensity", lightIntensity, 0.0f, 1000.0f));
	gui.add(phongPowerSelect.setup("Phong Power", phongPower, 1.0f, 100.0f));
	gui.add(lambertCoefficientSelect.setup("Lambert Coefficient", lambertCoefficient, 0.0f, 2.0f));

	lightIntensitySelect.addListener(this, &ofApp::lightIntensityListener);
	phongPowerSelect.addListener(this, &ofApp::phongPowerListener);
	lambertCoefficientSelect.addListener(this, &ofApp::lambertCoefficientListener);
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
		image.load("image.jpg");
		image.draw(0.0f, 0.0f);
		ofEnableDepthTest();
	}
	// Draw Gui
    if (!hideGui)
    {
        ofDisableDepthTest();
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
