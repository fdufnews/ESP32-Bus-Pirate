#pragma once

#include <string>
#include <Arduino.h>

#include "Interfaces/ITerminalView.h"
#include "Interfaces/IInput.h"
#include "Services/CellService.h"
#include "Models/TerminalCommand.h"
#include "Transformers/ArgTransformer.h"
#include "Transformers/AtTransformer.h"
#include "Managers/UserInputManager.h"
#include "States/GlobalState.h"
#include "Shells/HelpShell.h"
#include "Shells/CellCallShell.h"
#include "Shells/CellSmsShell.h"

class CellController {
public:
    CellController(ITerminalView& view,
                   IInput& terminalInput,
                   CellService& service,
                   ArgTransformer& argTransformer,
                   AtTransformer& atTransformer,
                   UserInputManager& userInputManager,
                   HelpShell& helpShell,
                   CellCallShell& cellCallShell,
                   CellSmsShell& cellSmsShell);

    void handleCommand(const TerminalCommand& command);
    void ensureConfigured();

private:
    ITerminalView& terminalView;
    IInput& terminalInput;
    CellService& cellService;
    ArgTransformer& argTransformer;
    AtTransformer& atTransformer;
    UserInputManager& userInputManager;
    HelpShell& helpShell;
    CellCallShell& cellCallShell;
    CellSmsShell& cellSmsShell;
    GlobalState& state = GlobalState::getInstance();

    bool configured = false;

    void handleConfig();
    void handleModem();
    void handleSim();
    void handleSetMode();
    void handleUnlock(const TerminalCommand& command);
    void handleNetwork();
    void handleSms(const TerminalCommand& command);
    void handleUssd(const TerminalCommand& command);
    void handleCall(const TerminalCommand& command);
    void handleHelp();
};
