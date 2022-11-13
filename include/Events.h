#ifndef Events_h
#define Events_h

// Some common events.
// Codes 0..199 are available for
// user defined events.

class Events {

public:
    enum eventType {
        // no event occurred
        // param: none
        EV_NONE = 200,

        // a key was pressed
        // param: key code
        EV_KEY_PRESS,

        // a key was released
        // param: key code
        EV_KEY_RELEASE,

        EV_ENCODER,

        EV_BEATCHANGED,

        EV_SEQUENCECHANGED,

        EV_RESET,
        // use this to notify a character
        // param: the character to be notified
        EV_CHAR,

        // generic time event
        // param: a time value (exact meaning is defined
        // by the code inserting this event into the queue)
        EV_TIME,

        // generic timer events
        // param: same as EV_TIME
        EV_TIMER0,
        EV_TIMER1,
        EV_TIMER2,
        EV_TIMER3,

        // analog read (last number = analog channel)
        // param: value read
        EV_ANALOG0,
        EV_ANALOG1,
        EV_ANALOG2,
        EV_ANALOG3,
        EV_ANALOG4,
        EV_ANALOG5,

        // menu events
        EV_MENU0,
        EV_MENU1,
        EV_MENU2,
        EV_MENU3,
        EV_MENU4,
        EV_MENU5,
        EV_MENU6,
        EV_MENU7,
        EV_MENU8,
        EV_MENU9,

        // serial event
        // example: a new char is available
        //          param: the return value of Serial.read()
        EV_SERIAL,

        // LCD screen needs to be refreshed
        EV_PAINT
    };
};

#endif
