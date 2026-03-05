#pragma once

#include "Services/OneWireService.h"
#include "Interfaces/ITerminalView.h"
#include "Interfaces/IInput.h"
#include "Transformers/ArgTransformer.h"
#include "Managers/UserInputManager.h"
#include "Analyzers/BinaryAnalyzer.h"

class OneWireEepromShell {
public:
    OneWireEepromShell(
        ITerminalView& view,
        IInput& input,
        OneWireService& oneWireService,
        ArgTransformer& argTransformer,
        UserInputManager& userInputManager,
        BinaryAnalyzer& binaryAnalyzer
    );

    void run();

private:
    void cmdProbe();
    void cmdRead();
    void cmdWrite();
    void cmdDump();
    void cmdErase();
    void cmdAnalyze();

    OneWireService& oneWireService;
    ITerminalView& terminalView;
    IInput& terminalInput;
    ArgTransformer& argTransformer;
    UserInputManager& userInputManager;
    BinaryAnalyzer& binaryAnalyzer;

    inline static constexpr const char* actions[] = {
        " 🔍 Probe",
        " 📊 Analyze",
        " 📖 Read",
        " ✏️  Write",
        " 🗃️  Dump",
        " 💣 Erase",
        " 🚪 Exit Shell"
    };
    inline static constexpr size_t actionCount = sizeof(actions) / sizeof(actions[0]);

    std::string eepromModel = "DS2431"; // default
    uint8_t eepromPageSize = 8;
    uint16_t eepromSize = 128;
};
