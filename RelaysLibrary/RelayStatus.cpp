//
//  RelayStatus 
//  Library C++ code
//  ----------------------------------
//  Developed with embedXcode
//
//  Relays
//  Created by jeroenjonkman on 13-06-15
// 

#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include "RelayStatus.h"

void RelayStatus::setup(uint8_t relayPin)
{
    setup(relayPin,0,RELAYSTATUS_MODE_DEFAULT,false);
}

void RelayStatus::setup(uint8_t relayPin, uint16_t defaultMode)
{
    setup(relayPin,0,defaultMode,false);
}

void RelayStatus::setup(uint8_t relayPin, bool defaultOn)
{
    setup(relayPin,0,RELAYSTATUS_MODE_DEFAULT,defaultOn);
}

void RelayStatus::setup(uint8_t relayPin, uint8_t powerPin)
{
    setup(relayPin,powerPin,RELAYSTATUS_MODE_DEFAULT,false);
}

void RelayStatus::setup(uint8_t relayPin, uint8_t powerPin, bool defaultOn)
{
    setup(relayPin,powerPin,RELAYSTATUS_MODE_DEFAULT,defaultOn);
}

void RelayStatus::setup(uint8_t relayPin, uint8_t powerPin, uint16_t defaultMode)
{
    setup(relayPin,powerPin,defaultMode,false);
}

void RelayStatus::setup(uint8_t relayPin, uint8_t powerPin, uint16_t defaultMode, bool defaultOn)
{
#ifdef RelayStatus_debug
    Serial.print("RelayStatus::setup relayPin=");
    Serial.print(relayPin);
    Serial.print(", powerPin=");
    Serial.print(powerPin);
    Serial.print(", defaultMode=");
    Serial.print(defaultMode,BIN);
    Serial.print(", defaultOn=");
    Serial.println(defaultOn,BIN);
#endif
    _relayPin       = relayPin;
    _powerPin       = powerPin;
    _mode           = defaultMode;
    _defaultMode    = defaultMode;
    pinMode(_relayPin,OUTPUT);
    digitalWrite(_relayPin,_RELAYSTATUS_OFF);
    bitWrite(_status,RELAYSTATUS_STATUS_DEFAULT_ON_BIT,defaultOn);
    if (defaultOn) {
        relayOn(_mode);
    }
    if( _powerPin > 0) {
        bitWrite(_status,RELAYSTATUS_STATUS_POWER_BIT,true);
    }
    bitWrite(_status,RELAYSTATUS_STATUS_SETUP_BIT,true);
}

void RelayStatus::loop()
{
#ifdef RelayStatus_debug
    Serial.print("RelayStatus::loop relayPin");
    Serial.println(_relayPin);
#endif
    if (bitRead(_status,RELAYSTATUS_STATUS_POWER_BIT)) {
#ifdef RelayStatus_debug
        Serial.print("RelayStatus::loop powerPin");
        Serial.println(_powerPin);
#endif
        _power = getPowerX(_RELAYSTATUS_POWER_CYCLES);
    }
}

void RelayStatus::setMode(byte mode) {
    _mode = mode;
}

byte RelayStatus::getMode() {
    return _mode;
}

byte RelayStatus::getDefaultMode() {
    return _defaultMode;
}

uint8_t RelayStatus::relayPin()
{
    return _relayPin;
}

uint8_t RelayStatus::powerPin()
{
    return _powerPin;
}

void RelayStatus::setPowerPin(uint8_t powerPin)
{
    _powerPin = powerPin;
    if ( _powerPin > 0 ) {
        bitWrite(_status,RELAYSTATUS_STATUS_POWER_BIT,true);
    }
}

void RelayStatus::relayOn() {
    relayOn(RELAYSTATUS_MODE_TRIGGER);
}

void RelayStatus::relayOn(uint16_t mode)
{
    if (mode == RELAYSTATUS_MODE_ALL) {
        // Not allowed MODE_ALL
#ifdef RelayStatus_debug
        Serial.println("RelayStatus::relayOn mode=RELAYSTATUS_MODE_ALL!");
#endif
        return;
    }
    if( (mode & _mode) == mode ) {
#ifdef RelayStatus_debug_onoff
        Serial.print("RelayStatus::relayOn mode checked, mode=");
        Serial.print(mode,BIN);
        Serial.print(", _mode=");
        Serial.print(_mode,BIN);
        Serial.print(" ");
#endif
        if (!isOn()) {
#ifdef RelayStatus_debug_onoff
            Serial.print("SET ON ");
#endif
            digitalWrite(_relayPin,_RELAYSTATUS_ON);
            _mode = mode;
            setOn(true);
        }
        if (isOn() == isDefaultOn()) {
#ifdef RelayStatus_debug_onoff
            Serial.print("reset to default ");
#endif
            _mode = _defaultMode;
            _timer = RELAYSTATUS_TIMER_NONE;
        }
#ifdef RelayStatus_debug
        Serial.println();
#endif
    } else {
        if( (mode & _defaultMode) == mode ) {
            // mode set, next event with a other mode but in the default mode
#ifdef RelayStatus_debug_onoff
            Serial.print("RelayStatus::relayOn defaultMode checked, mode=");
            Serial.print(mode,BIN);
            Serial.print(", _mode=");
            Serial.print(_mode,BIN);
            Serial.print(", _defaultMode=");
            Serial.print(_defaultMode,BIN);
            Serial.print(" ");
#endif
            if (isOn()) {
                // set extra mode
#ifdef RelayStatus_debug_onoff
                Serial.print("set extra mode.");
#endif
                _mode = _mode|mode;
            }
#ifdef RelayStatus_debug
            Serial.println();
        } else {
            Serial.print("RelayStatus::relayOn mode fault mode=");
            Serial.print(mode,BIN);
            Serial.print(", _mode=");
            Serial.println(_mode,BIN);
#endif
        }
    }
}

void RelayStatus::relayOff()
{
    relayOff(RELAYSTATUS_MODE_TRIGGER);
}

void RelayStatus::relayOff(uint16_t mode)
{
    if (mode == RELAYSTATUS_MODE_ALL) {
#ifdef RelayStatus_debug
        Serial.println("RelayStatus::relayOff mode=RELAYSTATUS_MODE_ALL");
#endif
        return;
    }
    if( (mode & _mode) == mode ) {
#ifdef RelayStatus_debug_onoff
        Serial.print("RelayStatus::relayOff mode checked, mode=");
        Serial.print(mode,BIN);
        Serial.print(", _mode=");
        Serial.print(_mode,BIN);
        Serial.print(" ");
#endif
        if (isOn()) {
            digitalWrite(_relayPin,_RELAYSTATUS_OFF);
            _mode = mode;
#ifdef RelayStatus_debug_onoff
            Serial.print("SET OFF ");
#endif
            setOn(false);
        }
        if (isOn() == isDefaultOn()) {
#ifdef RelayStatus_debug_onoff
            Serial.println("reset to default");
#endif
            _mode = _defaultMode;
            _timer = RELAYSTATUS_TIMER_NONE;
        }
#ifdef RelayStatus_debug
        Serial.println();
#endif
    } else {
        if( (mode & _defaultMode) == mode ) {
            // mode set, next event with a other mode but in the default mode
#ifdef RelayStatus_debug_onoff
            Serial.print("RelayStatus::relayOff defaultMode checked, mode=");
            Serial.print(mode,BIN);
            Serial.print(", _mode=");
            Serial.print(_mode,BIN);
            Serial.print(", _defaultMode=");
            Serial.print(_defaultMode,BIN);
            Serial.print(" ");
#endif
            if (!isOn()) {
                // set extra mode
#ifdef RelayStatus_debug_onoff
                Serial.println("set extra mode");
#endif
                _mode = _mode|mode;
            }
#ifdef RelayStatus_debug
            Serial.println();
        } else {
            Serial.print("RelayStatus::relayOff mode fault mode=");
            Serial.print(mode,BIN);
            Serial.print(", _mode=");
            Serial.println(_mode,BIN);
#endif
        }
    }
}

bool RelayStatus::isSetup()
{
    return bitRead(_status,RELAYSTATUS_STATUS_SETUP_BIT);
}

#ifdef RelayStatus_debug

void RelayStatus::print()
{
    Serial.print("RelayStatus ");
    if (!isSetup()) {
        Serial.print("setup not runned.");
        return;
    }
    Serial.print(" relayPin=");
    Serial.print(_relayPin);
    Serial.print(", powerPin=");
    if (_powerPin>0) {
        Serial.print(_powerPin);
    } else {
        Serial.print("NONE");
    }
    Serial.print((isOn())?", NO":", OFF");
    Serial.print((isDefaultOn())?", default NO":", Default OFF");
    Serial.print(", mode=");
    Serial.print(_mode,BIN);
    Serial.print(", default mode=");
    Serial.print(_defaultMode,BIN);
    Serial.print(", status=");
    Serial.print(_status,BIN);
    
}

void RelayStatus::println()
{
    print();
    Serial.println();
}

#endif

bool RelayStatus::isOn() {
    return bitRead(_status,RELAYSTATUS_STATUS_ON_BIT);
}

bool RelayStatus::isDefaultOn() {
    return bitRead(_status,RELAYSTATUS_STATUS_DEFAULT_ON_BIT);
}

bool RelayStatus::isTimer() {
    return bitRead(_timer,RELAYSTATUS_TIMER_BIT);
}

bool RelayStatus::isPower() {
    return bitRead(_status,RELAYSTATUS_STATUS_POWER_BIT);
}

void RelayStatus::setTimer(uint8_t delayType, uint16_t delay)
{
#ifdef RelayStatus_debug
    Serial.print("RelayStatus::setTimer delayType=");
    Serial.print(delayType);
    Serial.print(", delay=");
    Serial.print(delay);
#endif
    if (delayType != RELAYSTATUS_DELAYTYPE_NONE &&
        ( ( _defaultMode & RELAYSTATUS_MODE_TIMER ) == RELAYSTATUS_MODE_TIMER ||
          ( _defaultMode & RELAYSTATUS_MODE_TRIGGER ) == RELAYSTATUS_MODE_TRIGGER
        ) ) {
        _timer = delay & 0x0FFF;        // 12 bits
        _timer = _timer << 3;
        _timer += delayType & 0x07;     // 3 bits
        _timer = _timer << 1;
        _timer += 1;                    // 1 bit
        // Add RELAYSTATUS_MODE_TIMER to mode;
        _mode = _mode | RELAYSTATUS_MODE_TIMER;
#ifdef RelayStatus_debug
        Serial.print(", _timer=");
        Serial.print(_timer,BIN);
        Serial.print(", _mode=");
        Serial.print(_mode,BIN);
#endif
    } else {
        _timer = RELAYSTATUS_TIMER_NONE;
#ifdef RelayStatus_debug
        Serial.print(", _timer=");
        Serial.print(_timer,BIN);
#endif
    }
#ifdef RelayStatus_debug
    Serial.println();
#endif
}

#ifdef RelayStatus_power
uint16_t RelayStatus::getPower()
{
    if (bitRead(_status,RELAYSTATUS_STATUS_POWER_BIT)) {
        return _power;
    }
    return 0;
}
#endif

uint8_t RelayStatus::getTimerType()
{
    if (isTimer()) {
        return (_timer & RELAYSTATUS_TIMER_DELAYTYPE) >> 1;
    }
    return 0;
}

uint16_t RelayStatus::getTimerDelay()
{
    if (isTimer()) {
        return (_timer & RELAYSTATUS_TIMER_DELAY) >> 4;
    }
    return 0;
}

uint8_t RelayStatus::putXBeeStatus(ByteBuffer *buffer)
{
    if( buffer->getFreeSize() > 0 ) {
        buffer->put(_status);
        return 1;
    }
    return 0;
}

uint8_t RelayStatus::putXBeePower(ByteBuffer *buffer)
{
    if( buffer->getSize() > 0 ) {
        buffer->putU16(_power);
        return 1;
    }
    return 0;
}

void RelayStatus::setOn(bool on)
{
    bitWrite(_status,RELAYSTATUS_STATUS_ON_BIT,on);
}

#ifdef RelayStatus_power

uint16_t RelayStatus::getPowerX(uint8_t count)
{
    double power = 0;
    count = (count > 0 && count <= 32)?count:1;
    for( uint8_t i=0;i<count;i++)
        power+=_getRawPower();
#ifdef RelayStatus_debug
    Serial.print("getPowerX X=" );
    Serial.print(count);
    Serial.print(", pin = ");
    Serial.println(_powerPin);
    Serial.print(", power = ");
    Serial.println(power / count);
#endif
    return (power / count) * RELAYSTATUS_POWER_AMPS_20;
}

uint16_t RelayStatus::_getRawPower()
{
    uint16_t  _max      = _DEFAULT_POWER_VALUE;
    uint16_t  _min      = _DEFAULT_POWER_VALUE;
    //int       _sample   = _DEFAULT_POWER_VALUE;
    uint16_t  _count    = 0;
    uint16_t  _power    = _DEFAULT_POWER_VALUE;;
    long      _start    = millis();
    long      _stop     = _start + _POWER_SAMPLE_SIZE;
    // Overrun protection
    _stop = (_stop<_start)?_start:_stop;
    while (millis() < _stop ) {
        uint16_t _sample = analogRead(_powerPin);
        _min = min(_min,_sample);
        _max = max(_max,_sample);
        _count++;
    }
    //_power = max(DEFAULT_VALUE-_min,_max-_DEFAULT_POWER_VALUE);
    _power = ((_DEFAULT_POWER_VALUE-_min) + (_max-_DEFAULT_POWER_VALUE))/2;
#ifdef RelayStatus_power_debug
    Serial.print("Count = " );
    Serial.print(_count);
    Serial.print(", raw power = ");
    Serial.print(_power,3);
    Serial.print(", min = ");
    Serial.print(_min);
    Serial.print(", mindiff = ");
    Serial.print(_DEFAULT_POWER_VALUE-_min);
    Serial.print(", max = ");
    Serial.print(_max);
    Serial.print(", maxdiff = ");
    Serial.print(_max-_DEFAULT_POWER_VALUE);
#endif
    _power = max(0,_power - _POWER_TRIGGER_VALUE);
#ifdef RelayStatus_power_debug
    Serial.print(", power = ");
    Serial.print(_power,3);
    Serial.println();
#endif
    return _power;
}
#endif



