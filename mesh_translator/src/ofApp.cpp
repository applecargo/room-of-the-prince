#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetVerticalSync(true);
  ofBackground(255);
  ofSetLogLevel(OF_LOG_VERBOSE);
  ofEnableAlphaBlending();
  ofTrueTypeFontSettings settings("NanumGothicCoding-Regular.ttf",24);
  // add the ranges that you need.
  // settings.addRanges(ofAlphabet::Emoji);
  // settings.addRanges(ofAlphabet::Japanese);
  // settings.addRanges(ofAlphabet::Chinese);
  settings.addRanges(ofAlphabet::Korean);
  // settings.addRanges(ofAlphabet::Arabic);
  // settings.addRanges(ofAlphabet::Devanagari);
  // settings.addRanges(ofAlphabet::Latin);
  // settings.addRanges(ofAlphabet::Greek);
  // settings.addRanges(ofAlphabet::Cyrillic);
  font.load(settings); // font is an instance of ofTrueTypeFont

  //
  bool success = device.setup("/dev/cu.SLAB_USBtoUART", 9600);
  if (success) {
    device.registerAllEvents(this);
    ofLogNotice("ofApp::setup") << "setup done." << endl;
  } else {
    ofLogNotice("ofApp::setup") << "setup failed." << endl;
  }
}

//--------------------------------------------------------------
// void ofApp::exit()
// {
//   device.unregisterAllEvents(this);
// }

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::onSerialBuffer(const ofxIO::SerialBufferEventArgs& args)
{
  // Buffers will show up here when the marker character is found.
  SerialMessage message(args.buffer().toString(), "", 500);
  serialMessages.push_back(message);
}

//--------------------------------------------------------------
void ofApp::onSerialError(const ofxIO::SerialBufferErrorEventArgs& args)
{
  // Errors and their corresponding buffer (if any) will show up here.
  SerialMessage message(args.buffer().toString(),
                        args.exception().displayText(),
                        500);
  serialMessages.push_back(message);
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofBackground(0);
  ofSetColor(255);

  int x = 20;//start x
  int y = 50;//start y
  int height = 20;//line-height

  // Cycle through each of our messages and delete those that have expired.
  auto iter = serialMessages.begin();
  //
  while (iter != serialMessages.end())
  {
    //fading effect
    iter->fade -= 1;

    //removing effect
    if (iter->fade < 0)
    {
      iter = serialMessages.erase(iter);
    }
    else
    {
      //
      ofSetColor(255, ofClamp(iter->fade, 0, 255));

      //parse 'iter->message' and then, attach proper translations
      ofDrawBitmapString(iter->message, ofVec2f(x, y));
      y += height;

      //if 'error' msg, also display on the screen.
      if (!iter->exception.empty())
      {
        ofSetColor(255, 0, 0, ofClamp(iter->fade, 0, 255));
        ofDrawBitmapString(iter->exception, ofVec2f(x + height, y));
        y += height;
      }

      //get next!
      ++iter;
    }
  }
}
