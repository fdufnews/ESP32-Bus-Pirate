#pragma once
#include <string>
#include <vector>
#include <stdint.h>

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

class CellSmsShell {
public:
    CellSmsShell(ITerminalView& terminalView,
                 IInput& terminalInput,
                 UserInputManager& userInputManager,
                 ArgTransformer& argTransformer,
                 AtTransformer& atTransformer,
                 CellService& cellService);

    void run();

private:
    void cmdList();
    void cmdRead();
    void cmdDelete();
    void cmdSend();

    inline static constexpr const char* ACTIONS[] = {
        " 📥 List SMS",
        " 📄 Read SMS",
        " 🗑️  Delete SMS",
        " ✉️  Send SMS",
        " 🚪 Exit Shell"
    };
    inline static constexpr size_t ACTION_COUNT = sizeof(ACTIONS) / sizeof(ACTIONS[0]);

    inline static constexpr const char* LIST_FILTERS[] = {
        "ALL",
        "REC UNREAD",
        "REC READ",
        "STO SENT",
        "STO UNSENT",
        "Exit"
    };
    inline static constexpr size_t LIST_FILTER_COUNT = sizeof(LIST_FILTERS) / sizeof(LIST_FILTERS[0]);

    inline static constexpr const char* DELETE_FLAGS[] = {
        "Delete one message at index",
        "Delete all read messages",
        "Delete all read+sent messages",
        "Delete all read+sent+unsent messages",
        "Delete all messages",
        "Exit"
    };
    inline static constexpr size_t DELETE_FLAG_COUNT = sizeof(DELETE_FLAGS) / sizeof(DELETE_FLAGS[0]);

    ITerminalView& terminalView;
    IInput& terminalInput;
    UserInputManager& userInputManager;
    ArgTransformer& argTransformer;
    AtTransformer& atTransformer;
    CellService& cellService;
};
