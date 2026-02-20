#include "CellCallShell.h"

CellCallShell::CellCallShell(ITerminalView& terminalView,
                             IInput& terminalInput,
                             UserInputManager& userInputManager,
                             ArgTransformer& argTransformer,
                             AtTransformer& atTransformer,
                             CellService& cellService)
    : terminalView(terminalView),
      terminalInput(terminalInput),
      userInputManager(userInputManager),
      argTransformer(argTransformer),
      atTransformer(atTransformer),
      cellService(cellService)
{
}

void CellCallShell::run()
{
    while (true) {
        terminalView.println("\n=== CELL Call Shell ===");
        int idx = userInputManager.readValidatedChoiceIndex("Select an action", kActions, kActionCount, kActionCount - 1);

        if (idx < 0 || kActions[idx] == " 🚪 Exit Shell") {
            terminalView.println("Exiting Call Shell...\n");
            break;
        }

        switch (idx) {
            case 0: cmdDial();  break;
            case 1: cmdAnswer(); break;
            case 2: cmdHangup(); break;
            case 3: cmdList();   break;
            default:
                terminalView.println("❌ Unknown choice.\n");
                break;
        }
    }
}

void CellCallShell::cmdDial()
{
    std::string number = userInputManager.readString("Enter number to dial (ex: +33601020304)", "");
    if (number.empty()) {
        terminalView.println("❌ Empty number.");
        return;
    }

    terminalView.println("Dialing " + number + " ...");
    bool ok = cellService.dial(number);
    terminalView.println(ok ? "✅ Dial: OK" : "❌ Dial: ERROR");
}

void CellCallShell::cmdAnswer()
{
    bool ok = cellService.answerCall();
    terminalView.println(ok ? "✅ Answer: OK" : "❌ Answer: ERROR");
}

void CellCallShell::cmdHangup()
{
    bool ok = cellService.hangupCall();
    terminalView.println(ok ? "✅ Hangup: OK" : "❌ Hangup: ERROR");
}

void CellCallShell::cmdList()
{
    auto raw = cellService.listCalls();
    terminalView.println(atTransformer.formatCallList(raw));
}
