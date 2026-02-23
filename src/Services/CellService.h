#pragma once

#include <string>
#include <stdint.h>
#include "Data/CellAtProfiles.h"

class CellService {
public:
    void init(uint8_t rxPin, uint8_t txPin, uint32_t baudrate);
    bool detect();

    // Basic / identity
    std::string getModuleInfo();
    std::string getManufacturer();
    std::string getModel();
    std::string getRevision();
    std::string getImei();
    std::string getClock();
    
    // SIM
    bool isSimReady();
    std::string getSimState();
    bool enterPin(const std::string& pin);
    std::string getIccid();
    std::string getImsi();
    std::string getMsisdn();
    std::string getPinLockStatus();
    std::string getSimRetries();
    std::string getServiceProviderName();
    std::string getPhonebookStorage();
    std::string getPhonebookCaps();
    std::string getSmsStorage();

    // Network
    std::string getSignal();
    std::string getOperator();
    std::string scanOperators(uint32_t timeoutMs = 60000);
    bool setOperatorAuto();
    bool setOperator(const std::string& mccmnc);
    std::string getRegistrationCS();
    std::string getRegistrationPS();
    bool setFunctionality(uint8_t fun);
    std::string getFunctionality();
    bool reboot();

    // PDP / attach
    std::string getAttach();
    bool setAttach(bool attached);
    bool definePdpContext(uint8_t cid, const std::string& pdpType, const std::string& apn);
    std::string queryPdpContexts();
    bool activatePdp(uint8_t cid, bool active);
    std::string queryPdpActive();
    std::string getPdpAddress(uint8_t cid);

    // SMS
    bool smsSetTextMode(bool enabled);
    bool smsSetCharset(const std::string& charset);
    bool smsSetNewIndications(uint8_t mode, uint8_t mt, uint8_t bm, uint8_t ds, uint8_t bfr);
    std::string smsGetServiceCenter();
    std::string smsList(const std::string& filter);
    std::string smsRead(uint16_t index);
    bool smsDelete(uint16_t index, uint8_t flag);
    bool smsBeginSend(const std::string& number); // waits for '>'
    bool smsSendText(const std::string& text);    // sends text + Ctrl+Z
    std::string phonebookReadIndex(uint16_t index);
    std::string phonebookReadRange(uint16_t start, uint16_t end);

    // USSD
    bool ussdRequest(const std::string& code, uint8_t dcs = 15);
    bool ussdCancel();

    // Calls
    bool dial(const std::string& number);
    bool answerCall();
    bool hangupCall();
    std::string listCalls();

    // GSM loc
    std::string getGsmLocation();

private:
    std::string sendCommand(const std::string& cmd, uint32_t timeoutMs = 1000);
    std::string readResponse(uint32_t timeoutMs);
    void flushInput();
    bool sendExpectOk(const std::string& cmd, uint32_t timeoutMs = 1000);

    // Formatting helpers for profile templates
    std::string format1(const char* fmt, const std::string& a);
    std::string format1u(const char* fmt, uint32_t v);
    std::string format2u(const char* fmt, uint32_t a, uint32_t b);
    std::string format3(const char* fmt, int a, const std::string& b, const std::string& c);
    std::string format5u(const char* fmt, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);

    bool sendTextAndCtrlZExpectOk(const std::string& text, uint32_t timeoutMs);

    uint32_t _baudrate = 0;
    CellAtProfile _profile = GENERIC_CELL_PROFILE;
};
