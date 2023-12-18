#include "ofApp.h"
#include <cmath>
#include <iostream>

// Intersect Ray with Plane  (wrapper on glm::intersect*
//
bool Plane::intersect(const Ray &ray, glm::vec3 & point, glm::vec3 & normalAtIntersect)
{
	float dist;
	bool hit = glm::intersectRayPlane(ray.p, ray.d, position, this->normal, dist);
	
	// Plane hacking for lighting purposes
	normalAtIntersect.y = 1.0f;

	if (hit) {
		Ray r = ray;
		point = r.evalPoint(dist);
	}
	return (hit);
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
				float brightness = max(0.0f, glm::dot(light, glm::normalize(maxNormal)));
				ofColor imageColor = closestObject->diffuseColor * brightness;
				image.setColor(i, imageHeight - j - 1, imageColor);
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
	Sphere* s1 = new Sphere(glm::vec3(5, 2, -30), 5.0, ofColor::red);
	Sphere* s2 = new Sphere(glm::vec3(0, 3, -20), 5.0, ofColor::yellow);
	Sphere* s3 = new Sphere(glm::vec3(1, 6, -10), 5.0, ofColor::green);
	Plane* p = new Plane(glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

	scene.push_back(s1);
	scene.push_back(s2);
	scene.push_back(s3);
	scene.push_back(p);

	light = glm::normalize(glm::vec3(-10, 10, 20));
}

//--------------------------------------------------------------
void ofApp::update()
{
	
}

//--------------------------------------------------------------
void ofApp::draw()
{
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
		image.load("image.jpg");
		image.draw(0.0f, 0.0f);
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
