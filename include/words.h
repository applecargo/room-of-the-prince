#define MSG_LENGTH_MAX 256

//room events

// #1 ==> 369
#define ROOM_EVT369_ID               (ID_EVERYONE + 0x10)
//
#define ROOM_EVT369_CALL             (ROOM_EVT369_ID + 0x01) // wanna play 369?
#define ROOM_EVT369_RESPONSE_Y       (ROOM_EVT369_ID + 0x02) // good. i'm in.
#define ROOM_EVT369_RESPONSE_N       (ROOM_EVT369_ID + 0x03) // no, pass. i'm out.
//only for those did say gonna do this.
#define ROOM_EVT369_FASTER           (ROOM_EVT369_ID + 0x04) // let's make it faster
#define ROOM_EVT369_SLOWER           (ROOM_EVT369_ID + 0x05) // let's slow down, no?
#define ROOM_EVT369_DIED             (ROOM_EVT369_ID + 0x06) // oh. i'm dead. game over. well. was boring, again.

// #2 ==> baseball guess
#define ROOM_EVTBBG_ID               (ID_EVERYONE + 0x20)
//
#define ROOM_EVTBBG_CALL                 (ROOM_EVTBBG_ID + 0x01) // wanna play baseball guess?
#define ROOM_EVTBBG_RESPONSE_Y           (ROOM_EVTBBG_ID + 0x02) // good. i'm in.
#define ROOM_EVTBBG_RESPONSE_N           (ROOM_EVTBBG_ID + 0x03) // no, pass. i'm out.
//only for those did say gonna do this.
#define ROOM_EVTBBG_DECLARE_PROB_GIVER   (ROOM_EVTBBG_ID + 0x04) // i wanna do prob. giver.
#define ROOM_EVTBBG_DECLARE_PROB_SOLVER  (ROOM_EVTBBG_ID + 0x05) // i wanna do prob. solver.
#define ROOM_EVTBBG_THE_SOLUTION         (ROOM_EVTBBG_ID + 0x06) // i will try to solve! the answer is...
#define ROOM_EVTBBG_CORRECT              (ROOM_EVTBBG_ID + 0x07) // wow that was fast.
#define ROOM_EVTBBG_INCORRECT            (ROOM_EVTBBG_ID + 0x08) // sorry, but that is not the right one. ;)

// #3 ==> room escape game



//individual events

//announcements
#define CONDUCTOR_WORD_HELLO         (ID_CONDUCTOR + 0x00)

#define DISTANCE_WORD_HELLO          (ID_DISTANCE + 0x00)
#define DISTANCE_WORD_PRINCE_ACTIVE  (ID_DISTANCE + 0x01)

#define GPS_WORD_HELLO                    (ID_GPS + 0x00)
#define GPS_WORD_PLANETARY_ANNOUNCEMENT   (ID_GPS + 0x01)

#define DOOR_WORD_HELLO              (ID_DOOR + 0x00)
#define DOOR_WORD_PASSING_BY         (ID_DOOR + 0x01)
#define DOOR_WORD_NO_PASSENGER       (ID_DOOR + 0x02)

#define MOTION_WORD_HELLO            (ID_MOTION_SENSOR + 0x00)
#define MOTION_WORD_MOTION_START     (ID_MOTION_SENSOR + 0x01)
#define MOTION_WORD_MOTION_END       (ID_MOTION_SENSOR + 0x02)

#define TUNER_WORD_HELLO             (ID_TUNER + 0x00)
#define TUNER_WORD_THIS_IS_CORRECT   (ID_TUNER + 0x01)
#define TUNER_WORD_THIS_IS_HIGH      (ID_TUNER + 0x02)
#define TUNER_WORD_THIS_IS_LOW       (ID_TUNER + 0x03)

//actors
#define LOOKAT_WORD_HELLO            (ID_LOOK_AT + 0x00)
#define LOOKAT_WORD_LOOK_AROUND      (ID_LOOK_AT + 0x01)

#define THUNDER_WORD_HELLO           (ID_THUNDER + 0x00)
#define THUNDER_WORD_RRRRR           (ID_THUNDER + 0x01)
#define THUNDER_WORD_LEFT            (ID_THUNDER + 0x02)

#define ROCKING_WORD_HELLO           (ID_ROCKING + 0x00)
#define ROCKING_WORD_ROCK_ROCK_ROCK  (ID_ROCKING + 0x01)

#define BAG_WORD_HELLO               (ID_BAG + 0x00)
#define BAG_WORD_HANDLE_UP           (ID_BAG + 0x01)
#define BAG_WORD_HANDLE_DOWN         (ID_BAG + 0x02)
#define BAG_WORD_SING                (ID_BAG + 0x03)

#define HARMONICA_WORD_HELLO         (ID_HARMONICA + 0x00)
#define HARMONICA_WORD_PLAY_START    (ID_HARMONICA + 0x01)
#define HARMONICA_WORD_PLAY_STOP     (ID_HARMONICA + 0x02)

#define FUR_WORD_HELLO               (ID_FUR + 0x00)
#define FUR_WORD_SING                (ID_FUR + 0x01)

#define HANGER_WORD_HELLO            (ID_HANGER + 0x00)
#define HANGER_WORD_SING             (ID_HANGER + 0x01)

#define KEYHOLDER_WORD_HELLO         (ID_KEYHOLDER + 0x00)
#define KEYHOLDER_WORD_FALLING_KEYS  (ID_KEYHOLDER + 0x01)

#define MIRROR_WORD_HELLO            (ID_MIRROR + 0x00)
#define MIRROR_WORD_IWILLSHOWYOU     (ID_MIRROR + 0x01)

#define PEAK_WORD_HELLO              (ID_PEAK + 0x00)
#define PEAK_WORD_PPI_PPI_PPI        (ID_PEAK + 0x01)

#define BELL_WORD_HELLO              (ID_BELL + 0x00)
#define BELL_WORD_RING_RING_RING     (ID_BELL + 0x01)

#define FLOAT_WORD_HELLO             (ID_FLOAT + 0x00)
#define FLOAT_WORD_TURN_TURN         (ID_FLOAT + 0x01)

#define HEATER_WORD_HELLO            (ID_HEATER + 0x00)
#define HEATER_WORD_NOISY_NOISY      (ID_HEATER + 0x01)
