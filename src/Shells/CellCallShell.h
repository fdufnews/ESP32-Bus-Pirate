#pragma once
#include <string>
#include <vector>

#include "Interfaces/ITerminalView.h"
#include "Interfaces/IInput.h"
#include "Managers/UserInputManager.h"
#include "Transformers/ArgTransformer.h"
#include "Transformers/AtTransformer.h"
#include "Services/CellService.h"

class ITerminalView;
class IInput;
class UserInputManager;
class ArgTransformer;
class AtTransformer;
class CellService;

class CellCallShell {
public:
    CellCallShell(ITerminalView& terminalView,
                 IInput& terminalInput,
                 UserInputManager& userInputManager,
                 ArgTransformer& argTransformer,
                 AtTransformer& atTransformer,
                 CellService& cellService);

    void run();

private:
    void cmdDial();
    void cmdAnswer();
    void cmdHangup();
    void cmdList();

    ITerminalView& terminalView;
    IInput& terminalInput;
    UserInputManager& userInputManager;
    ArgTransformer& argTransformer;
    AtTransformer& atTransformer;
    CellService& cellService;

    inline static const char* kActions[] = {
        " 📞 Dial number",
        " ✅ Answer incoming call",
        " 📴 Hang up",
        " 📋 List calls (CLCC)",
        " 🚪 Exit Shell"
    };

    inline static constexpr size_t kActionCount =
        sizeof(kActions) / sizeof(kActions[0]);
};
