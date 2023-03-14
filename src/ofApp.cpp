#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    //start the web cam
    myVideoGrabber.initGrabber(640, 480);

    //load piano image 
    piano.load("piano.jpg");
    matPiano = toCv(piano);


    //3 music sheet images
    music1.load("music1.jpg");
    music2.load("music2.jpg");
    music3.load("music3.jpg");
    matMusic1 = toCv(music1);
    matMusic2 = toCv(music2);
    matMusic3 = toCv(music3);


    //load the sounds (piano keys sound)
    key1.load("key1.wav");//do
    key2.load("key2.wav");//re
    key3.load("key3.wav");//mi
    key4.load("key4.wav");//fa
    key5.load("key5.wav");//so
    key6.load("key6.wav");//la
    key7.load("key7.wav");//ti
    key8.load("key8.wav");//do'

    //add GUI elements (for color threshold)
    gui.setup();
    gui.add(rHigh.setup("r high", 255, 0, 255));
    gui.add(gHigh.setup("g high", 100, 0, 255));
    gui.add(bHigh.setup("b high", 100, 0, 255));
    gui.add(rLow.setup("r low", 150, 0, 255));
    gui.add(gLow.setup("g low", 0, 0, 255));
    gui.add(bLow.setup("b low", 0, 0, 255));
}

//HandGestureCNN::HandGestureCNN() {
//    parameters.setName("HandGestureCNN");
//    learningRate.set("Learning Rate", 0.01, 0.001, 0.1);
//    batchSize.set("Batch Size", 32, 1, 128);
//    epochs.set("Epochs", 10, 1, 100);
//    numClasses.set("Number of Classes", 5, 2, 10);
//    imageWidth.set("Image Width", 28, 16, 128);
//    imageHeight.set("Image Height", 28, 16, 128);
//    channels.set("Channels", 1, 1, 3);
//    conv1Filters.set("Conv1 Filters", 32, 1, 128);
//    conv2Filters.set("Conv2 Filters", 64, 1, 128);
//    fc1Units.set("FC1 Units", 128, 1, 1024);

//}

//--------------------------------------------------------------
void ofApp::update(){
    myVideoGrabber.update();
    
    TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 20, 0.03);
    cv::Size subPixWinSize(10,10),winSize(31,31);
    
    // If webcam has fresh data
    if (myVideoGrabber.isFrameNew()) {

        // Obtain a pointer to the grabber's image data.
        imgCam.setFromPixels(myVideoGrabber.getPixels());

        //mirro the webcam
        flip(imgCam, imgCam, 1);
        matCam = toCv(imgCam);
        
        //operate the color threshold 
        //so that the optical flow can only focus on one specific color of object\
        //so the result will be more accurate
        matBw = matCam.clone();
        for (int i = 0; i < matBw.rows * matBw.cols;i++) {//go over all the pixels of the webcam

            if (matBw.data[i * 3] < rHigh && matBw.data[i * 3] > rLow
                && matBw.data[i * 3 + 1] < gHigh && matBw.data[i * 3+1] > gLow
                && matBw.data[i * 3 + 2] < bHigh && matBw.data[i * 3+2] > bLow) {//color threshold

                //turn the irrelevant color object to black
                matBw.data[i * 3] = 255;
                matBw.data[i * 3 + 1] = 255;
                matBw.data[i * 3 + 2] = 255;


            }
            else {

                //turn the color we need to focus to white
                matBw.data[i * 3] = 0;
                matBw.data[i * 3 + 1] = 0;
                matBw.data[i * 3 + 2] = 0;
            }
        }

        //convert to gray scale
        cvtColor(matBw, matCamGrey, CV_BGR2GRAY);
      

        if(needToInit){
            initPointsToTrack(subPixWinSize, termcrit);
            needToInit = false;
        }
        else if(!prePoints.empty()){
            trackKeyPoints(winSize);
            getDirections();
            Point2f averageDir = getAverageDirection();
            cout<<averageDir<<endl;

            

            //the threshold needs to be adjusted
            if(averageDir.x > 0.1 ){//move right
                cout<<"move right"<<endl;
                moveLeft = false;
                moveRight = true;
            }
            else if(averageDir.x < -0.1 ){//move left
                cout<<"move left"<<endl;
                moveLeft = true;
                moveRight = false;
            }
            else{
                cout<<"Nothing moving"<<endl;
                moveLeft = false;
                moveRight = false;
            }
            
        }
        
        std::swap(curPoints, prePoints);
        cv::swap(matCamPreGrey, matCamGrey);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //set colour white
    ofSetColor(255,255,255);

    //draw the webcam
    drawMat(matCam, 0, 0);

    //draw the webcam after color thnreshold
    drawMat(matCamGrey, 640, 480);

    //draw the piano image
    drawMat(matPiano, 0,480);

    //draw the area for changing the music sheet
    ofDrawLine(0, 80, 640, 80);
    ofDrawBitmapString("hands left/right to show the previous/next music sheet", 10, 10);

    //draw the area for piano playing
    ofDrawLine(0, 450, 640, 450);
    ofDrawBitmapString("play music here~", 10, 460);

    //write the notes of how to opeate tracking
    ofDrawBitmapString("Press r to start tracking", 700, 520);
    ofDrawBitmapString("Press c to clear all",700, 530);

    //draw the gui
    gui.draw();

    //display music sheet
    drawMat(matMusic1, 640, 0);

    if (musicSheetArea) {//check if the motion is happend in the "switching music sheet area"

        //this method is to operate the action that
        //if the motion is left, display the previous sheet
        // if the motion is right, display the next sheet
        if (moveLeft) {
            currentIndex = currentIndex - 1;
        }
        else if(moveRight){
            currentIndex = currentIndex + 1;
        }
        int sheetNum = currentIndex % 3;

        if (sheetNum == 1) {
            drawMat(matMusic1, 640, 0);
        }
        else if (sheetNum == 2) {
            drawMat(matMusic2, 640, 0);
        }
        else {
            drawMat(matMusic3, 640, 0);
        }
    }

    
    if(!prePoints.empty()){
        for(int i = 0; i < curPoints.size(); i++ )
        {
            //whether play the certain key of the piano
            bool k1 = false;
            bool k2 = false;
            bool k3 = false;
            bool k4 = false;
            bool k5 = false;
            bool k6 = false;
            bool k7 = false;
            bool k8 = false;

            ofSetColor(0,255,0);//green
            ofDrawCircle(curPoints[i].x+640, curPoints[i].y+480, 3);//draw circle on detected motion pixel

            //if touching the key, display the key accordingly
            if (curPoints[i].x < 80 && curPoints[i].y>469 && curPoints[i].y<480) {
              
                k1 = true;
            }
            else if (curPoints[i].x <160 && curPoints[i].y>477 && curPoints[i].y < 480) {
                k2 = true;
            }
            else if (curPoints[i].x < 240 && curPoints[i].y>477 && curPoints[i].y < 480) {
                k3 = true;;
            }
            else if (curPoints[i].x < 320 && curPoints[i].y>477 && curPoints[i].y < 480) {
                k4 = true;
            }
            else if (curPoints[i].x < 400 && curPoints[i].y>477 && curPoints[i].y < 480) {
                k5 = true;
            }
            else if (curPoints[i].x < 480 && curPoints[i].y>477 && curPoints[i].y < 480) {
                k6 = true;
            }
            else if (curPoints[i].x < 560 && curPoints[i].y>477 && curPoints[i].y < 480) {
                k7 = true;
            }
            else if (curPoints[i].x < 640 && curPoints[i].y>477 && curPoints[i].y < 480) {
                k8 = true;
            }
            else if (curPoints[i].x < 640 && curPoints[i].y>0 && curPoints[i].y < 80) {// if in the area for switching music sheet
                musicSheetArea = true;//record that
            }
           

            //display the key sound accordlingly
            if (k1 == true) {
                key1.play();
            }
            else if (k2 == true) {
                key2.play();
            }
            else if (k3 == true) {
                key3.play();
            }
            else if (k4 == true) {
                key4.play();
            }
            else if (k5 == true) {
                key5.play();
            }
            else if (k6 == true) {
                key6.play();
            }
            else if (k7 == true) {
                key7.play();
            }
            else if (k8 == true) {
                key8.play();
            }
            //red
            ofSetColor(255,0,0);
            //draw action line
            ofDrawLine(prePoints[i].x+640, prePoints[i].y+480, curPoints[i].x+640, curPoints[i].y+480);
           

        }
    }
    
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key){
            case 'c':
                cout<<"clear points"<<endl;
                prePoints.clear();
                curPoints.clear();
                break;
            case 'r':
                needToInit = true;
                break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}



//below code reference from cs4187 tutorial session
//include some basic method to track corner gearure and operate potical flow to track motion direction
void ofApp::initPointsToTrack(cv::Size subPixWinSize, TermCriteria termcrit) {

    goodFeaturesToTrack(matCamGrey, curPoints, MAX_COUNT, 0.01, 10, Mat(), 3, 0);
    cornerSubPix(matCamGrey, curPoints, subPixWinSize, cv::Size(-1, -1), termcrit);

}

void ofApp::trackKeyPoints(cv::Size winSize) {

    vector<uchar> status;
    vector<float> err;
    if (matCamPreGrey.empty()) {
        matCamGrey.copyTo(matCamPreGrey);
    }
    calcOpticalFlowPyrLK(matCamPreGrey, matCamGrey, prePoints, curPoints, status, err, winSize);
}

void ofApp::getDirections() {
    for (int i = 0; i < curPoints.size(); i++) {
        Point2f dir = Point2f(curPoints.at(i).x - prePoints.at(i).x, curPoints.at(i).y - prePoints.at(i).y);
        directions.push_back(dir);
    }
}

Point2f ofApp::getAverageDirection() {
    Point2f averageDir(0, 0);
    for (int i = 0; i < directions.size(); i++) {
        averageDir.x += directions.at(i).x;
        averageDir.y += directions.at(i).y;
    }
    if (directions.size() != 0) {
        averageDir.x /= directions.size();
        averageDir.y /= directions.size();
    }
    return averageDir;
}

void ofApp::mousePressed(int x, int y, int button) {
    point = Point2f((float)x, (float)y);
    addRemovePt = true;

    if (addRemovePt && curPoints.size() < (size_t)MAX_COUNT)
    {

        vector<Point2f> tmp;
        cv::Size winSize(31, 31);
        TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03);
        tmp.push_back(point);
        cornerSubPix(matCamGrey, tmp, winSize, cv::Size(-1, -1), termcrit);
        curPoints.push_back(tmp[0]);
        addRemovePt = false;
    }

}