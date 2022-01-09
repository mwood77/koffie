/*
 * Relay.h
 * 
 * Implement a relay controlled by duty cycle.
 * 
 * Author: Rob Bultman
 *         Sept. 13, 2017
 */

#ifndef RELAY_H
#define RELAY_H

#include <stdint.h>

typedef enum {
  relayModeManual,
  relayModeAutomatic
} RelayMode;

typedef enum {
  relayPositionOpen,
  relayPositionClosed
} RelayPosition;

class Relay {
  public:
    Relay(uint8_t pin, uint16_t periodInSeconds);
    void setRelayMode(RelayMode mode);
    RelayMode getRelayMode(void);
    void setRelayPosition(RelayPosition position);
    RelayPosition getRelayPosition(void);
    void setDutyCyclePercent(double dutyCycle);
    double getDutyCyclePercent(void);
    void setPeriodInSeconds(uint16_t periodInSeconds);
    uint16_t getPeriodInSeconds(void);
    void loop(void);

  private:
    uint8_t _pin;
    uint16_t _periodInSeconds;
    double _dutyCycle;
    RelayMode _mode;
    RelayPosition _position;
    uint32_t _periodTime;
    uint32_t _oldTime;
};

#endif // RELAY_H