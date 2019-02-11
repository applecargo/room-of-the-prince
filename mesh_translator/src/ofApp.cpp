#include "ofApp.h"
#include <map>

// member identity
#define ID_BAG           (0x1D00 + 0x10)
#define ID_BELL          (0x1D00 + 0x20)
#define ID_CONDUCTOR     (0x1D00 + 0x30)
#define ID_DISTANCE      (0x1D00 + 0x40)
// #define ID_DOOR          (0x1D00 + 0x50)
#define ID_FLOAT         (0x1D00 + 0x60)
#define ID_FUR           (0x1D00 + 0x70)
#define ID_GPS           (0x1D00 + 0x80)
#define ID_HANGER        (0x1D00 + 0x90)
#define ID_HARMONICA     (0x1D00 + 0xA0)
#define ID_HEATER        (0x1D00 + 0xB0)
#define ID_KEYHOLDER     (0x1D00 + 0xC0)
#define ID_LOOK_AT       (0x1D00 + 0xD0)
#define ID_MIRROR        (0x1D00 + 0xE0)
// #define ID_MOTION_SENSOR (0x1D00 + 0xF0)
#define ID_PEAK          (0x1E00 + 0x00)
#define ID_ROCKING       (0x1E00 + 0x10)
#define ID_THUNDER       (0x1E00 + 0x20)

// #define ID_TUNER         (0x1E00 + 0x30)
// #define ID_RELAY_CTRLER  (0x1E00 + 0x40)
// #define ID_POINT_MOTOR   (0x1E00 + 0x50)

//
#define ID_EVERYONE      (0x1F00 + 0x00)

static std::map<int, std::string> translations_id_ko = {
  {ID_BAG, "가방"},
  {ID_BELL, "종"},
  {ID_CONDUCTOR, "연주자"},
  {ID_DISTANCE, "거리계"},
  {ID_FLOAT, "찌"},
  {ID_FUR, "털"},
  {ID_GPS, "GPS"},
  {ID_HANGER, "옷걸이"},
  {ID_HARMONICA, "하모니카"},
  {ID_HEATER, "난로"},
  {ID_KEYHOLDER, "열쇠걸이"},
  {ID_LOOK_AT, "본다"},
  {ID_MIRROR, "거울"},
  {ID_PEAK, "뾰"},
  {ID_ROCKING, "흔들의자"},
  {ID_THUNDER, "번개철"},
  {ID_EVERYONE, "모두들!"}
};

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetVerticalSync(true);
  ofBackground(255);
  ofSetLogLevel(OF_LOG_VERBOSE);
  ofEnableAlphaBlending();
  ofTrueTypeFontSettings settings("NanumGothicCoding-Regular.ttf", 14);
  // add the ranges that you need.
  // settings.addRanges(ofAlphabet::Emoji);
  // settings.addRanges(ofAlphabet::Japanese);
  // settings.addRanges(ofAlphabet::Chinese);
  settings.addRanges(ofAlphabet::Korean);
  // settings.addRanges(ofAlphabet::Arabic);
  // settings.addRanges(ofAlphabet::Devanagari);
  settings.addRanges(ofAlphabet::Latin);
  // settings.addRanges(ofAlphabet::Greek);
  // settings.addRanges(ofAlphabet::Cyrillic);
  font.load(settings); // font is an instance of ofTrueTypeFont

  //
  bool success = device.setup("/dev/cu.SLAB_USBtoUART", 9600);
  if (success) {
    device.registerAllEvents(this);
    ofLog(OF_LOG_NOTICE, "[ofApp::setup] setup done.");
  } else {
    ofLog(OF_LOG_NOTICE, "[ofApp::setup] setup failed.");
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
  SerialMessage message(args.buffer().toString(), "", 1500);
  serialMessages.push_back(message);
}

//--------------------------------------------------------------
void ofApp::onSerialError(const ofxIO::SerialBufferErrorEventArgs& args)
{
  // Errors and their corresponding buffer (if any) will show up here.
  SerialMessage message(args.buffer().toString(),
                        args.exception().displayText(),
                        1500);
  serialMessages.push_back(message);
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofBackground(0);
  ofSetColor(255);

  int x = 20;//start x
  int y = 50;//start y
  int height = 20;//line-height
  int column_step = 200;//

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

      // //TODO: parse 'iter->message' and then, attach proper translations
      // ofDrawBitmapString(iter->message, ofVec2f(x, y));
      if ((iter->message).substr(0, 1) == "[") {
        //print 'original message' w/o commentary
        font.drawString("[001XX4:YY35]", x, y); // [TTTTTT:MMMM]
        x += column_step;
        //parse 'iter->message'
        std::string message = iter->message;
        std::string to = message.substr(1, 6); // [TTTTTT:MMMM]
        int key_to = stoi(to);
        std::string msg = message.substr(8, 4); // [TTTTTT:MMMM]
        font.drawString(translations_id_ko[key_to], x, y);
        x += column_step;
        font.drawString(msg, x, y);
        x = x - column_step * 2;
        y += height;
      }

      //if 'error' msg, also display on the screen.
      if (!iter->exception.empty())
      {
        ofSetColor(255, 0, 0, ofClamp(iter->fade, 0, 255));
        font.drawString(iter->exception, x + height, y);
        y += height;
      }

      //get next!
      ++iter;
    }
  }
}
