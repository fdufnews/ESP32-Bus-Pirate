#pragma once

#include <string>
#include "Models/TerminalCommand.h"
#include "Services/UartService.h"
#include "Interfaces/ITerminalView.h"
#include "Interfaces/IInput.h"
#include "Transformers/ArgTransformer.h"
#include "Managers/UserInputManager.h"
#include "Shells/HelpShell.h"
#include "States/GlobalState.h"

class C5Controller {
public:
    // Constructor
    C5Controller(ITerminalView& terminalView,
                 IInput& terminalInput,
                 UartService& uartService,
                 ArgTransformer& argTransformer,
                 UserInputManager& userInputManager,
                 HelpShell& helpShell);
    
    // Entry point for C5 cmds
    void handleCommand(const TerminalCommand& cmd);

    // Ensure C5 configured before use
    void ensureConfigured();

private:
    // Configure the C5
    void handleConfig();

    // Handle the UART bridge with the C5
    void handleBridge();

    ITerminalView& terminalView;
    IInput& terminalInput;
    UartService& uartService;
    ArgTransformer& argTransformer;
    UserInputManager& userInputManager;
    HelpShell& helpShell;
    GlobalState& state = GlobalState::getInstance();

    // Fixed UART settings for the C5
    const uint32_t baud = 115200;
    const uint8_t dataBits = 8;
    const char parityChar = 'N';
    const uint8_t stopBits = 1;
    const bool inverted = false;

    bool configured = false;
};