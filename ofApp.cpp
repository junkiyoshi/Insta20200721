#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openframeworks");

	ofBackground(239);
	ofSetLineWidth(3);
	ofEnableDepthTest();

	auto ico_sphere = ofIcoSpherePrimitive(125, 2);
	this->triangle_list = ico_sphere.getMesh().getUniqueFaces();
}

//--------------------------------------------------------------
void ofApp::update() {

	this->mesh.clear();
	
	ofSeedRandom(39);
	auto base_location_noise = glm::vec3(ofRandom(1000), ofRandom(1000), ofRandom(1000));
	auto base_location = glm::vec3(
		ofMap(ofNoise(base_location_noise.x, ofGetFrameNum() * 0.005), 0, 1, -300, 300),
		ofMap(ofNoise(base_location_noise.y, ofGetFrameNum() * 0.005), 0, 1, -300, 300),
		ofMap(ofNoise(base_location_noise.z, ofGetFrameNum() * 0.005), 0, 1, -50, 50)
	);
	auto prev_base_location = glm::vec3(
		ofMap(ofNoise(base_location_noise.x, (ofGetFrameNum() + 1) * 0.005), 0, 1, -300, 300),
		ofMap(ofNoise(base_location_noise.y, (ofGetFrameNum() + 1) * 0.005), 0, 1, -300, 300),
		ofMap(ofNoise(base_location_noise.z, (ofGetFrameNum() + 1) * 0.005), 0, 1, -50, 50)
	);
	auto velocity = base_location - prev_base_location;
	auto noise_seed = glm::vec3(ofRandom(1000), ofRandom(1000), ofRandom(1000));

	for (auto& triangle : this->triangle_list) {

		int index = this->mesh.getNumVertices();
		auto avg = glm::vec3(triangle.getVertex(0) + triangle.getVertex(1) + triangle.getVertex(2)) / 3;
		auto param = (int)(ofRandom(1000) + ofGetFrameNum()) % 200;

		vector<glm::vec3> vertices;
		for (int j = 0; j < 3; j++) {

			glm::vec3 location = glm::vec3(triangle.getVertex(j).x, triangle.getVertex(j).y, triangle.getVertex(j).z);

			auto rotation_x = glm::rotate(glm::mat4(), ofMap(ofNoise(noise_seed.x, ofGetFrameNum() * 0.0035), 0, 1, -PI, PI), glm::vec3(1, 0, 0));
			auto rotation_y = glm::rotate(glm::mat4(), ofMap(ofNoise(noise_seed.y, ofGetFrameNum() * 0.0035), 0, 1, -PI, PI), glm::vec3(0, 1, 0));
			auto rotation_z = glm::rotate(glm::mat4(), ofMap(ofNoise(noise_seed.z, ofGetFrameNum() * 0.0035), 0, 1, -PI, PI), glm::vec3(0, 0, 1));
			location = glm::vec4(location, 0) * rotation_z * rotation_y * rotation_x;

			vertices.push_back(location + base_location);
		}
		
		if(param == 0){

			this->fragment_list.push_back(vertices);
			this->fragment_life_list.push_back(255);
			this->fragment_velocity_list.push_back(velocity * 1.5 + avg * 0.005);
		}
		else if (param > 50) {

			this->mesh.addVertices(vertices);
			this->mesh.addIndex(index + 0); this->mesh.addIndex(index + 1); this->mesh.addIndex(index + 2);
		}
	}

	for (int i = this->fragment_list.size() - 1; i >= 0; i--) {

		for (auto& vertex : this->fragment_list[i]) {

			vertex += this->fragment_velocity_list[i];
		}
		this->fragment_life_list[i] -= 5;

		if (this->fragment_life_list[i] < 0) {

			this->fragment_list.erase(this->fragment_list.begin() + i);
			this->fragment_life_list.erase(this->fragment_life_list.begin() + i);
			this->fragment_velocity_list.erase(this->fragment_velocity_list.begin() + i);
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();

	ofSetColor(39);
	this->mesh.drawFaces();

	ofSetColor(239);
	this->mesh.drawWireframe();

	for (int i = 0; i < this->fragment_list.size(); i++) {

		ofSetColor(39, this->fragment_life_list[i]);
		ofFill();
		ofBeginShape();
		ofVertices(this->fragment_list[i]);
		ofEndShape(true);

		ofSetColor(239, this->fragment_life_list[i]);
		ofNoFill();
		ofBeginShape();
		ofVertices(this->fragment_list[i]);
		ofEndShape(true);
	}

	this->cam.end();
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}