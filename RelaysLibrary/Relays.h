//
//  Relays 
//  Header
//  ----------------------------------
//  Developed with embedXcode
//
//  Relays
//  Created by jeroenjonkman on 13-06-15
// 


#ifndef Relays_h

#define Relays_h

#define Relay_power
//#define Relay_debug
//#define Relay_set_debug
//#define Relay_time_debug
//#define Relay_print

#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include <RelayStatus.h>
#include <RelayTask.h>
#include <DS3232RTC.h>
#include <Time.h>
#include <ByteBuffer.h>

#ifdef RelayTask_debug
#define _RELAYS_MAX_TASK_SIZE       8
#else
#define _RELAYS_MAX_TASK_SIZE       32
#endif

#ifdef RelayStatus_debug
#define _RELAYS_MAX_STATUS_SIZE     4
#else
#define _RELAYS_MAX_STATUS_SIZE     16
#endif

#ifdef Relay_debug
#define RELAYS_LOOP_CHECK           5000
#else
#define RELAYS_LOOP_CHECK           1000
#endif

#define RELAYS_OFF                  false
#define RELAYS_ON                   true

#define _RELAYS_POWER_MIN           5
#define _RELAYS_MAX                 150
#define _RELAYS_READ_COUNT          100

#define _RELAYS_TEMPERATURE_NOSET   1024
#define _RELAYS_HUMIDITY_NOSET      1024
#define _RELAYS_LIGHT_NOSET         -1

#define XBEE_TIME_HEADER            0x10
#define XBEE_RELAY_HEADER           0x20
#define XBEE_POWER_HEADER           0x80

class Relays
{
public:
    void setup();
    
    void loop();

    void loopTask();
    
    void loopStatus();

#ifdef Relay_power
    void loopPower();
#endif
    
    int addRelay( uint8_t pin );
    
    int addRelay( uint8_t pin, bool defaultOn );

    int addRelay( uint8_t pin, int defaultMode);

    int addRelay( uint8_t relayPin, uint8_t powerPin);
    
    int addRelay( uint8_t relayPin, uint8_t powerPin, bool defaultOn);

    int addRelay( uint8_t relayPin, uint8_t powerPin, bool defaultOn, uint16_t defaultMode);

    int addTaskTime( uint16_t relays, bool on, uint8_t month, uint8_t day_of_month, uint8_t day_of_week, uint8_t hour, uint8_t minute);
    
    int addTaskTemperature( uint16_t relays, bool on, uint8_t operatortype, int value);
    
    int addTaskHumidity( uint16_t relays, bool on, uint8_t operatortype, int value);

    int addTaskLight( uint16_t relays, bool on, uint8_t operatortype, int value);

    int addTaskTrigger( uint16_t relays, bool on, uint8_t delayType, uint16_t delay);

    bool setTaskDelayInSeconds(uint16_t taskID, uint16_t delay);

    bool setTaskDelayInMinutes(uint16_t taskID, uint16_t delay);

    bool setTaskDelayInHours(uint16_t taskID, uint16_t delay);
    
    bool setTaskDelayInDays(uint16_t taskID, uint16_t delay);

    uint8_t getPowerPin( uint8_t relayPin );
    
    uint16_t powerRelay( uint8_t relayPin );
    
    uint16_t power();
    
    void relaysOn();
    
    void relayOn(uint8_t relayPin);
    
    void relayOn(int relayID);
    
    void relaysOff();

    void relayOff(int relayID);

    void relayOff(uint8_t relayPin);
    
    uint8_t relayPin();
    
    uint8_t powerPin();
    
    void setPowerPin(uint8_t powerPin);
    
    void setMaxRelay(int maxRelay);
    
    bool isSetup();
    
    void setTemperature(int temperature);
    
    int getTemperature();
    
    void setHumidity(int humidity);
    
    int getHumidity();

    void setLight(int light);
    
    int getLight();
    
    void trigger();

    uint8_t putXBeeData(ByteBuffer *buffer);
    
    uint8_t putXBeeTime(ByteBuffer *buffer);
    
    uint8_t putXBeeStatus(ByteBuffer *buffer);
    
    uint8_t putXBeePower(ByteBuffer *buffer);
    

private:
    RelayStatus     _status[_RELAYS_MAX_STATUS_SIZE];
    RelayTask       _task[_RELAYS_MAX_TASK_SIZE];
    uint8_t         _statusSize     = 0;
    uint16_t        _statusID       = 0; //
    uint8_t         _taskSize       = 0;
    uint16_t        _taskID         = 0;
    uint8_t         _looper         = 0;
    bool            _analogWrite    = false;
    bool            _setup          = false;
    uint8_t         _maxRelay       = _RELAYS_MAX;
    unsigned long   _last_run       = 0;
    int             _temperature    = _RELAYS_TEMPERATURE_NOSET;
    int             _humidity       = _RELAYS_HUMIDITY_NOSET;
    int             _light          = _RELAYS_LIGHT_NOSET;
    time_t          _lastTime       = 0;
    
    int         digitalReadOutputPin(uint8_t pin);
    
    bool        checkRelay(uint8_t relayPin);
    
    uint8_t     getRelayStatusIndex(uint8_t relayPin);
#ifdef Relay_print
    String      loopStringStatus();
#endif
};

#endif
