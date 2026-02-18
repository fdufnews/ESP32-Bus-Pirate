#pragma once
#include <string>

#include "Services/FmService.h" 
#include "Managers/UserInputManager.h"
#include "Transformers/ArgTransformer.h"
#include "Interfaces/ITerminalView.h"
#include "Interfaces/IInput.h"

class ITerminalView;
class IInput;
class UserInputManager;
class ArgTransformer;
class FmService;

class FmBroadcastShell {
public:
    FmBroadcastShell(
        ITerminalView& terminalView,
        IInput& terminalInput,
        UserInputManager& userInputManager,
        ArgTransformer& argTransformer,
        FmService& fmService
    );

    void run();

private:
    void cmdStatus_();
    void cmdStart_();
    void cmdStop_();
    void cmdSetFreq_();
    void cmdAutoFreq_();
    void cmdSetPower_();
    void cmdSetRdsPs_();
    void cmdSetRdsText_();
    void cmdToggleTa_();

private:
    ITerminalView& terminalView;
    IInput& terminalInput;
    UserInputManager& userInputManager;
    ArgTransformer& argTransformer;
    FmService& fmService;

    inline static const char* kActions[] = {
        "  📻 Station",
        "  📡 Start TX",
        "  🛑 Stop TX",
        "  📊 Set frequency",
        "  🚀 Auto frequency",
        "  🔊 Set power",
        "  🆔 Set RDS name",
        "  💬 Set RDS text",
        "  🛰️ Toggle TA",
        " 🚪 Exit Shell"
    };

    std::string ps_;
    std::string text_;

    inline static const size_t kActionCount = sizeof(kActions) / sizeof(kActions[0]);
};
