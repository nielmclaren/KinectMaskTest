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
	
	gotKinectFrame = false;
	
	readFrames("out12.mov");
}

void testApp::update() {
	kinect.update();
	if(kinect.isFrameNew()) {
		maskOfPixels.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);
		if (maskOfPixels.isAllocated()) {
			maskOfPixels.resize(frameWidth, frameHeight);
			blur(maskOfPixels, 100);
			maskPixels = maskOfPixels.getPixels();
			
			// Make sure mask values are within the expected range.
			for (int i = 0; i < frameWidth * frameHeight; i++) {
				maskPixels[i] = MIN(maskPixels[i], 255);
				maskPixelsDetail[i] = maskPixels[i] * 255;
			}
			gotKinectFrame = true;
		}
	}
}

void testApp::draw() {
	ofBackground(0);
	
	if (frameWidth > 0) {
		distortedPixels = distorted.getPixels();
		
		if (gotKinectFrame) {
			for (int x = 0; x < frameWidth; x++) {
				for (int y = 0; y < frameHeight; y++) {
					int frameIndex = maskPixelsDetail[y * frameWidth + x] * frameCount / 255 / 255;
					unsigned char* frame = frames[frameIndex];
					
					for (int c = 0; c < 3; c++) {
						int pixelIndex = y * frameWidth * 3 + x * 3 + c;
						distortedPixels[pixelIndex] = frame[pixelIndex];
					}
				}
			}
			gotKinectFrame = false;
		}
		
		ofSetColor(255, 255, 255);
		
		mask.setFromPixels(maskPixels, frameWidth, frameHeight, OF_IMAGE_GRAYSCALE);
		mask.draw(frameWidth/2, 0, frameWidth/2, frameHeight/2);
		
		distorted.setFromPixels(distortedPixels, frameWidth, frameHeight, OF_IMAGE_COLOR);
		distorted.draw(0, frameHeight/2, frameWidth, frameHeight);
	}
}

void testApp::exit() {
	clearFrames();
}

void testApp::readFrames(string filename) {
	player.loadMovie(filename);
	player.play();
	player.setPaused(true);
	
	frameCount = player.getTotalNumFrames();
	frameWidth = player.width;
	frameHeight = player.height;
	
	cout << "Loading frames..." << endl;
	
	unsigned char* copyPixels;
	for (int i = 0; i < frameCount; i++) {
		cout << i << " of " << frameCount << endl;
		copyPixels = player.getPixels();
		unsigned char* framePixels = new unsigned char[frameWidth * frameHeight * 3];
		for (int i = 0; i < frameWidth * frameHeight * 3; i++) {
			framePixels[i] = copyPixels[i];
		}
		frames.push_back(framePixels);
		player.nextFrame();
	}
	
	player.closeMovie();
	
	distorted.setFromPixels(frames[0], frameWidth, frameHeight, OF_IMAGE_COLOR);
	
	// Reset mask.
	maskPixelsDetail = new unsigned short int[frameWidth * frameHeight];
	for (int i = 0; i < frameWidth * frameHeight; i++) {
		maskPixelsDetail[i] = 0;
	}
}

void testApp::clearFrames() {
	for (int i = 0; i < frameCount; i++) {
		delete[] frames[i];
	}
	delete[] maskPixelsDetail;
		
	frameCount = 0;
	frameWidth = 0;
	frameHeight = 0;
}

void testApp::writeDistorted() {
	distorted.setFromPixels(distortedPixels, frameWidth, frameHeight, OF_IMAGE_COLOR);
	distorted.saveImage("distorted.tga", OF_IMAGE_QUALITY_BEST);
}

void testApp::keyPressed(int key) {
}

void testApp::keyReleased(int key) {
	switch (key) {
		case 's':
			writeDistorted();
			break;
			
		case 'x':
			clearFrames();
			break;
	}
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
	for (int i = 0; i < dragInfo.files.size(); i++) {
		string filename = dragInfo.files[i];
		vector<string> tokens = ofSplitString(filename, ".");
		string extension = tokens[tokens.size() - 1];
		if (extension == "mov") {
			clearFrames();
			readFrames(filename);
		}
		else {
			cout << "Unknown file type: " << extension << endl;
		}
	}
}