#include "CellSmsShell.h"

CellSmsShell::CellSmsShell(ITerminalView& terminalView,
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

void CellSmsShell::run()
{
    while (true) {
        terminalView.println("\n=== CELL SMS Shell ===");

        int idx = userInputManager.readValidatedChoiceIndex(
            "Select an action",
            ACTIONS,
            ACTION_COUNT,
            ACTION_COUNT - 1
        );

        if (idx < 0 || idx >= (int)ACTION_COUNT || idx == (int)ACTION_COUNT - 1) {
            terminalView.println("Exiting SMS Shell...\n");
            break;
        }

        switch (idx) {
            case 0: cmdList();   break;
            case 1: cmdRead();   break;
            case 2: cmdDelete(); break;
            case 3: cmdSend();   break;
            default:
                terminalView.println("❌ Unknown choice.");
                break;
        }
    }
}

void CellSmsShell::cmdList()
{
    int idx = userInputManager.readValidatedChoiceIndex("Select filter", LIST_FILTERS, LIST_FILTER_COUNT, -1);
    if (idx < 0 || idx >= (int)LIST_FILTER_COUNT || idx == (int)LIST_FILTER_COUNT - 1) {
        return; // Exit
    }

    cellService.smsSetTextMode(true);
    cellService.smsSetCharset("GSM");

    auto raw = cellService.smsList(LIST_FILTERS[idx]);

    terminalView.println("");
    terminalView.println(atTransformer.formatSmsList(raw));
}

void CellSmsShell::cmdRead()
{
    uint32_t idx = userInputManager.readValidatedUint32("SMS index", 1);

    cellService.smsSetTextMode(true);
    cellService.smsSetCharset("GSM");

    auto raw = cellService.smsRead((uint16_t)idx);

    terminalView.println("");
    terminalView.println(atTransformer.formatSmsRead(raw));
}

void CellSmsShell::cmdDelete()
{
    uint32_t smsIdx = userInputManager.readValidatedUint32("SMS index", 1);

    int f = userInputManager.readValidatedChoiceIndex("Delete flag", DELETE_FLAGS, DELETE_FLAG_COUNT, -1);
    if (f < 0 || f >= (int)DELETE_FLAG_COUNT || f == (int)DELETE_FLAG_COUNT - 1) {
        return; // Exit
    }

    uint8_t flag = (uint8_t)f;

    bool ok = cellService.smsDelete((uint16_t)smsIdx, flag);
    terminalView.println(ok ? "\n✅ SMS delete: OK" : "\n❌ SMS delete: ERROR");
}

void CellSmsShell::cmdSend()
{
    std::string number = userInputManager.readString("Recepient number", "+33601020304");
    std::string text   = userInputManager.readString("Message text", "Hello");

    cellService.smsSetTextMode(true);
    cellService.smsSetCharset("GSM");

    auto confirm = userInputManager.readYesNo("Send SMS to " + number + "?", true);
    if (!confirm) {
        terminalView.println("\n❌ SMS send cancelled.");
        return;
    }

    terminalView.println("Sending SMS... It may take 15 sec...");

    bool ready = cellService.smsBeginSend(number);
    if (!ready) {
        terminalView.println("\n❌ CMGS prompt not received (ERROR).");
        return;
    }

    bool ok = cellService.smsSendText(text);
    terminalView.println(ok ? "\n✅ SMS sent: OK" : "\n❌ SMS sent: ERROR");
}
