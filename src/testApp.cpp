#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
	kinect.open();
	
	ofSetFrameRate(60);
	kinect.setCameraTiltAngle(0);
}

void testApp::update() {
	kinect.update();
	if(kinect.isFrameNew()) {
		maskOfPixels.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);
		blur(maskOfPixels, 100);
	}
}

void testApp::draw() {
	ofBackground(0);
	ofSetColor(255, 255, 255);
	
	mask.setFromPixels(maskOfPixels);
	mask.draw(0, 0, kinect.width, kinect.height);
}

void testApp::exit() {
}

void testApp::keyPressed(int key) {
}

void testApp::keyReleased(int key) {
}

void testApp::mouseMoved(int x, int y) {
}

void testApp::mouseDragged(int x, int y, int button) {
}

void testApp::mousePressed(int x, int y, int button) {
}

void testApp::mouseReleased(int x, int y, int button) {}

void testApp::windowResized(int w, int h) {
}

void testApp::gotMessage(ofMessage msg) {
}

void testApp::dragEvent(ofDragInfo dragInfo) {
}