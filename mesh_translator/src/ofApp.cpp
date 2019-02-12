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
#define ID_EVERYONE      (0x1F00 + 0x00)

//words
#define CONDUCTOR_WORD_HELLO              (ID_CONDUCTOR + 0x00)
#define DISTANCE_WORD_HELLO               (ID_DISTANCE + 0x00)
#define DISTANCE_WORD_PRINCE_ACTIVE       (ID_DISTANCE + 0x01)
#define GPS_WORD_HELLO                    (ID_GPS + 0x00)
#define GPS_WORD_PLANETARY_ANNOUNCEMENT   (ID_GPS + 0x01)
#define DOOR_WORD_HELLO                   (ID_DOOR + 0x00)
#define DOOR_WORD_PASSING_BY              (ID_DOOR + 0x01)
#define DOOR_WORD_NO_PASSENGER            (ID_DOOR + 0x02)
#define MOTION_WORD_HELLO                 (ID_MOTION_SENSOR + 0x00)
#define MOTION_WORD_MOTION_START          (ID_MOTION_SENSOR + 0x01)
#define MOTION_WORD_MOTION_END            (ID_MOTION_SENSOR + 0x02)
#define TUNER_WORD_HELLO                  (ID_TUNER + 0x00)
#define TUNER_WORD_THIS_IS_CORRECT        (ID_TUNER + 0x01)
#define TUNER_WORD_THIS_IS_HIGH           (ID_TUNER + 0x02)
#define TUNER_WORD_THIS_IS_LOW            (ID_TUNER + 0x03)
#define LOOKAT_WORD_HELLO                 (ID_LOOK_AT + 0x00)
#define LOOKAT_WORD_LOOK_AROUND           (ID_LOOK_AT + 0x01)
#define THUNDER_WORD_HELLO                (ID_THUNDER + 0x00)
#define THUNDER_WORD_RRRRR                (ID_THUNDER + 0x01)
#define THUNDER_WORD_LEFT                 (ID_THUNDER + 0x02)
#define ROCKING_WORD_HELLO                (ID_ROCKING + 0x00)
#define ROCKING_WORD_ROCK_ROCK_ROCK       (ID_ROCKING + 0x01)
#define BAG_WORD_HELLO                    (ID_BAG + 0x00)
#define BAG_WORD_HANDLE_UP                (ID_BAG + 0x01)
#define BAG_WORD_HANDLE_DOWN              (ID_BAG + 0x02)
#define BAG_WORD_SING                     (ID_BAG + 0x03)
#define HARMONICA_WORD_HELLO              (ID_HARMONICA + 0x00)
#define HARMONICA_WORD_PLAY_START         (ID_HARMONICA + 0x01)
#define HARMONICA_WORD_PLAY_STOP          (ID_HARMONICA + 0x02)
#define FUR_WORD_HELLO                    (ID_FUR + 0x00)
#define FUR_WORD_SING                     (ID_FUR + 0x01)
#define HANGER_WORD_HELLO                 (ID_HANGER + 0x00)
#define HANGER_WORD_SING                  (ID_HANGER + 0x01)
#define KEYHOLDER_WORD_HELLO              (ID_KEYHOLDER + 0x00)
#define KEYHOLDER_WORD_FALLING_KEYS       (ID_KEYHOLDER + 0x01)
#define MIRROR_WORD_HELLO                 (ID_MIRROR + 0x00)
#define MIRROR_WORD_IWILLSHOWYOU          (ID_MIRROR + 0x01)
#define PEAK_WORD_HELLO                   (ID_PEAK + 0x00)
#define PEAK_WORD_PPI_PPI_PPI             (ID_PEAK + 0x01)
#define BELL_WORD_HELLO                   (ID_BELL + 0x00)
#define BELL_WORD_RING_RING_RING          (ID_BELL + 0x01)
#define FLOAT_WORD_HELLO                  (ID_FLOAT + 0x00)
#define FLOAT_WORD_TURN_TURN              (ID_FLOAT + 0x01)
#define HEATER_WORD_HELLO                 (ID_HEATER + 0x00)
#define HEATER_WORD_NOISY_NOISY           (ID_HEATER + 0x01)

//
static std::map<int, std::string> translations_id_en = {
  {ID_BAG, "Bag"},
  {ID_BELL, "Bell"},
  {ID_CONDUCTOR, "Conductor"},
  {ID_DISTANCE, "Distancer"},
  {ID_FLOAT, "Float"},
  {ID_FUR, "Fur"},
  {ID_GPS, "GPS"},
  {ID_HANGER, "Hanger"},
  {ID_HARMONICA, "Harmonica"},
  {ID_HEATER, "Heater"},
  {ID_KEYHOLDER, "Keyholder"},
  {ID_LOOK_AT, "Look-at"},
  {ID_MIRROR, "Mirror"},
  {ID_PEAK, "Peak"},
  {ID_ROCKING, "Rockering-chair"},
  {ID_THUNDER, "Thunder-plane"},
  {ID_EVERYONE, "Everyone!"}
};

//
static std::map<int, std::string> translations_id_ko = {
  {ID_BAG, "가방"},
  {ID_BELL, "벨"},
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
  {ID_THUNDER, "번개면"},
  {ID_EVERYONE, "모두들!"}
};

static std::map<int, std::string> translations_words_ko = {
  { CONDUCTOR_WORD_HELLO, "안녕, 난 나다. 내 말 잘 들어."},
  { DISTANCE_WORD_HELLO, "그거 알아? 지구는 동그라미."},
  { DISTANCE_WORD_PRINCE_ACTIVE, "여러분, 왕자님이 움직인다."},
  { GPS_WORD_HELLO, "1/.-1/.-0/.-1/.-1/.-0/.-0/.-1/.-0/.-1/.-1/.-1/.-1/.-0/.-0/.-0/.-0"},
  { GPS_WORD_PLANETARY_ANNOUNCEMENT, "승객 여러분, 현재 우리 행성은..."},
  // { DOOR_WORD_HELLO, ""},
  // { DOOR_WORD_PASSING_BY, ""},
  // { DOOR_WORD_NO_PASSENGER, ""},
  // { MOTION_WORD_HELLO, ""},
  // { MOTION_WORD_MOTION_START, ""},
  // { MOTION_WORD_MOTION_END, ""},
  // { TUNER_WORD_HELLO, ""},
  // { TUNER_WORD_THIS_IS_CORRECT, ""},
  // { TUNER_WORD_THIS_IS_HIGH, ""},
  // { TUNER_WORD_THIS_IS_LOW, ""},
  { LOOKAT_WORD_HELLO, "나요? 난 너를 보지!"},
  { LOOKAT_WORD_LOOK_AROUND, "둘러봐? 어디를? 지금? 왜? 누구야!"},
  { THUNDER_WORD_HELLO, "안녕 난 ㄹㄹㄹㄹㄹㄹㄹㄹㄹㄹ."},
  { THUNDER_WORD_RRRRR, "지금부터 한다. ㄹㄹㄹㄹㄹㄹㄹㄹㄹㄹ."},
  { THUNDER_WORD_LEFT, "인제 다했어. ㄹㄹ."},
  { ROCKING_WORD_HELLO, "롹. 더 스피릿. 이에요."},
  { ROCKING_WORD_ROCK_ROCK_ROCK, "로그로그로그.."},
  { BAG_WORD_HELLO, "열에 아홉입니다!"},
  { BAG_WORD_HANDLE_UP, "이-러-어---/ 섯!"},
  { BAG_WORD_HANDLE_DOWN, "안쟛!!"},
  { BAG_WORD_SING, "노래/ 시-이-이-쟉!"},
  { HARMONICA_WORD_HELLO, "노래는 니 인생."},
  { HARMONICA_WORD_PLAY_START, "니 한번 불어봐."},
  { HARMONICA_WORD_PLAY_STOP, "니 이제 그만 불어."},
  { FUR_WORD_HELLO, "(ㅋㅎㅎㅎㅎㅋㅎㅎㅎㅇㅇㅇ)"},
  { FUR_WORD_SING, "ㅋㅊ ㅎ, ㅊㅊ!"},
  { HANGER_WORD_HELLO, "뜨뜨르뜨뜨"},
  { HANGER_WORD_SING, "뜨르리뜨르리뜨로리뜨로리, 노래는 내 인생. 뜨뜻."},
  { KEYHOLDER_WORD_HELLO, "빨간 열새~ 노란 열새~ 파란 열새~~"},
  { KEYHOLDER_WORD_FALLING_KEYS, "열새들아 날아라~"},
  { MIRROR_WORD_HELLO, "보여주지, 본다니, 어디에 있나, 근데?"},
  { MIRROR_WORD_IWILLSHOWYOU, "내가 널 보여줄꺼야."},
  { PEAK_WORD_HELLO, "던칸. 삼각함산에 살고 있어."},
  { PEAK_WORD_PPI_PPI_PPI, "(ㅃ/.-ㅃ/.-ㅃ/.-)"},
  { BELL_WORD_HELLO, "신호 나오면 가져가. 다들 가져가 버린다."},
  { BELL_WORD_RING_RING_RING, "링 링 링"},
  { FLOAT_WORD_HELLO, "(돌고 도는 돌이)"},
  { FLOAT_WORD_TURN_TURN, "(돌아 돈 돌이 돈)"},
  { HEATER_WORD_HELLO, "(ㅍㅍㅍㅍㅍㅍㅍㅍㅍㅍㅍ)"},
  { HEATER_WORD_NOISY_NOISY, "어서와요, 시끄러운 세상."}
};

static std::map<int, std::string> translations_words_en = {
  { CONDUCTOR_WORD_HELLO, "Hi, me is me. Listen carefully."},
  { DISTANCE_WORD_HELLO, "You know what? The earth is ROUND."},
  { DISTANCE_WORD_PRINCE_ACTIVE, "Now, prince is in ACTION!"},
  { GPS_WORD_HELLO, "1000 1010 0010 1111 1010 1010 IoIo."},
  { GPS_WORD_PLANETARY_ANNOUNCEMENT, "Now, our planet is.."},
  // { DOOR_WORD_HELLO, ""},
  // { DOOR_WORD_PASSING_BY, ""},
  // { DOOR_WORD_NO_PASSENGER, ""},
  // { MOTION_WORD_HELLO, ""},
  // { MOTION_WORD_MOTION_START, ""},
  // { MOTION_WORD_MOTION_END, ""},
  // { TUNER_WORD_HELLO, ""},
  // { TUNER_WORD_THIS_IS_CORRECT, ""},
  // { TUNER_WORD_THIS_IS_HIGH, ""},
  // { TUNER_WORD_THIS_IS_LOW, ""},
  { LOOKAT_WORD_HELLO, "Me? me do look at you!"},
  { LOOKAT_WORD_LOOK_AROUND, "Me to looking around?? Well, but where?"},
  { THUNDER_WORD_HELLO, "Hello? me do rrrrrrrrrrrr!"},
  { THUNDER_WORD_RRRRR, "Here we go! rrrrrrrrrrrr!"},
  { THUNDER_WORD_LEFT, "Thunder is gone!"},
  { ROCKING_WORD_HELLO, "ROCK is the spirit!"},
  { ROCKING_WORD_ROCK_ROCK_ROCK, "rock, rock, rock"},
  { BAG_WORD_HELLO, "Sir! 9 of the 10!"},
  { BAG_WORD_HANDLE_UP, "s-t-a-n-d up!"},
  { BAG_WORD_HANDLE_DOWN, "seat down, now!"},
  { BAG_WORD_SING, "s-i-n-g, now!"},
  { HARMONICA_WORD_HELLO, "MUSIC is your life."},
  { HARMONICA_WORD_PLAY_START, "You blow, me dance."},
  { HARMONICA_WORD_PLAY_STOP, "Stop dance, no blow."},
  { FUR_WORD_HELLO, "(kuhhhkuhhhuuu)"},
  { FUR_WORD_SING, "(khcuk huk, kch!)"},
  { HANGER_WORD_HELLO, "(ttut-ttu-ru-ttut-ttu)"},
  { HANGER_WORD_SING, "I do sing for me!"},
  { KEYHOLDER_WORD_HELLO, "Red key, yellow key, blue key.."},
  { KEYHOLDER_WORD_FALLING_KEYS, "Kays!~ Leave away~"},
  { MIRROR_WORD_HELLO, "Show you, see you, where are you?"},
  { MIRROR_WORD_IWILLSHOWYOU, "me will show you"},
  { PEAK_WORD_HELLO, "Duncan. Living in the triangularities."},
  { PEAK_WORD_PPI_PPI_PPI, "(ppi ppi ppi)"},
  { BELL_WORD_HELLO, "signal out~ take my signal~~ to every you."},
  { BELL_WORD_RING_RING_RING, "ring ring."},
  { FLOAT_WORD_HELLO, "(turn turn turn)"},
  { FLOAT_WORD_TURN_TURN, "(turn turn)"},
  { HEATER_WORD_HELLO, "(rrrrrrrrrrr)"},
  { HEATER_WORD_NOISY_NOISY, "Welcome to noise world."}
};

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetVerticalSync(true);
  ofSetFrameRate(60);
  ofHideCursor();

  ofBackground(255);
  ofSetLogLevel(OF_LOG_VERBOSE);
  ofEnableAlphaBlending();

  // rpi's GL has a limit of texture size 2048. but Nanum fonts sizing 14 require 4096x4096.. yet below 13 (==12) is fine.
  ofTrueTypeFontSettings settings("NanumGothic.ttf", 12); // THIS works.
  // ofTrueTypeFontSettings settings("NanumGothic.ttf", 13); // THIS works. - but font size seems the same as 12.
  // ofTrueTypeFontSettings settings("NanumGothic_Coding.ttf", 13); // THIS works. - but font size seems the same as 12.
  // ofTrueTypeFontSettings settings("NanumGothic_Coding.ttf", 14); // THIS doesn't work. --> max. texture size limit @ rpi ( <= 2048)

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
  bool success = device.setup("/dev/ttyUSB0", 9600);
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
  SerialMessage message(args.buffer().toString(), "", 500);
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
  int y = 40;//start y
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
      // //TODO: parse 'iter->message' and then, attach proper translations
      // ofDrawBitmapString(iter->message, ofVec2f(x, y));
      if ((iter->message).substr(0, 1) == "[") {
        //parse 'iter->message'
        std::string message = iter->message;
        std::string to = message.substr(1, 6); // [TTTTTT:MMMM]
        int key_to = stoi(to);
        std::string msg = message.substr(8, 4); // [TTTTTT:MMMM]
        int key_msg = stoi(msg);

        //display (show)
        ofSetColor(255, ofClamp(iter->fade, 0, 255));

        //print 'original message' w/o commentary
        font.drawString(message, x, y); // [TTTTTT:MMMM]
        x += 150;

        //
        font.drawString("{" + translations_id_ko[key_to] + "} 에게..", x, y);
        x += 200;
        font.drawString(translations_words_ko[key_msg], x, y);

        //carrige return & new line.
        x = x - 350;
        y += height;

        //
        ofSetColor(0, 127, 255, ofClamp(iter->fade, 0, 255));

        //print 'original message' w/o commentary
        font.drawString(message, x, y); // [TTTTTT:MMMM]
        x += 150;

        //
        font.drawString("To : {" + translations_id_en[key_to] + "}", x, y);
        x += 200;
        font.drawString(translations_words_en[key_msg], x, y);

        //carrige return & new line.
        x = x - 350;
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
