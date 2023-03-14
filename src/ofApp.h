#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"


using namespace ofxCv;
using namespace cv;

class ofApp : public ofBaseApp {

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void initPointsToTrack(cv::Size subPixWinSize, TermCriteria termcrit);
    void trackKeyPoints(cv::Size winSize);

    void getDirections();
    Point2f getAverageDirection();


 //   class HandGestureCNN {
 //   public:
 //       HandGestureCNN();
 //       ofParameterGroup parameters;
 //       ofParameter<float> learningRate;
 //       ofParameter<int> batchSize;
 //       ofParameter<int> epochs;
 //       ofParameter<int> numClasses;
 //       ofParameter<int> imageWidth;
 //       ofParameter<int> imageHeight;
 //       ofParameter<int> channels;
 //       ofParameter<int> conv1Filters;
 //       ofParameter<int> conv2Filters;
 //       ofParameter<int> fc1Units;
 //       ofParameter<int> fc2Units;
 //       void train(cv::Mat input, cv::Mat labels);
 //       int predict(cv::Mat input);
 //  private:
 //       void build();
 //       ofxDeepBeliefNet net;
 //   };


    //webcam 
    ofVideoGrabber myVideoGrabber;

    //webcam current image
    ofImage imgCam;
    Mat matCam;

    //gray scale of webcam image
    Mat matCamGrey;
    Mat matCamPreGrey;

    //gray scale of last webcam image
    Mat gray;
    Mat prevGray;

    //piano image
    ofImage piano;
    Mat matPiano;

    //music sheets
    ofImage music1;
    ofImage music2;
    ofImage music3;
    Mat matMusic1;
    Mat matMusic2;
    Mat matMusic3;

    //sound of piano keys
    ofSoundPlayer key1;
    ofSoundPlayer key2;
    ofSoundPlayer key3;
    ofSoundPlayer key4;
    ofSoundPlayer key5;
    ofSoundPlayer key6;
    ofSoundPlayer key7;
    ofSoundPlayer key8;

    //mat that after color threshold (in black & white)
    Mat matBw;

    //create the gui
    ofxPanel gui;

    //color thresholds ( for rgb channels , high and low threshold)
    ofxFloatSlider rHigh;
    ofxFloatSlider gHigh;
    ofxFloatSlider bHigh;
    ofxFloatSlider rLow;
    ofxFloatSlider gLow;
    ofxFloatSlider bLow;

    //previous points array
    vector<Point2f> prePoints;

    //current points array
    vector<Point2f> curPoints;

    //directions of detected motion
    vector<Point2f> directions;

    cv::Point2f point;
    bool addRemovePt = false;
    int MAX_COUNT = 300;
    bool needToInit = false;

    //track the motion (left/right)
    bool moveRight = false;
    bool moveLeft = false;

    //whether in the area controls the display of music sheet
    bool musicSheetArea = false;

    //for switching music sheet (identify which sheet to display)
    int currentIndex = 100;




};
