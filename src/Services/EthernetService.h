#pragma once
#include <array>
#include <string>
#include <SPI.h>
#include <ETH.h>

class EthernetService {
public:
  EthernetService();

  bool configure(int8_t pinCS,
                 int8_t pinRST,
                 int8_t pinSCK,
                 int8_t pinMISO,
                 int8_t pinMOSI,
                 int8_t pinIRQ,
                 uint32_t spiHz,
                 const std::array<uint8_t,6>& chosenMac,
                 SPIClass* spi = &SPI,
                 uint8_t phyAddr = 1);

  bool beginDHCP(unsigned long timeoutMs);

  bool isConnected() const;
  bool linkUp() const;

  std::string getMac() const;
  std::string getLocalIP() const;
  std::string getSubnetMask() const;
  std::string getGatewayIp() const;
  std::string getDns() const;

  void hardReset();
  
private:
  static void onNetEvent(arduino_event_id_t event, arduino_event_info_t info);
  static EthernetService* s_self;
  SPIClass* _spi;
  int8_t _pinCS, _pinRST, _pinSCK, _pinMISO, _pinMOSI, _pinIRQ;
  uint32_t _spiHz;
  uint8_t _phyAddr;
  std::array<uint8_t,6> _mac;

  volatile bool _configured;
  volatile bool _linkUp;
  volatile bool _gotIP;
};