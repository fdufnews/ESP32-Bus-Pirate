#include "CellController.h"

/*
Constructor
*/
CellController::CellController(ITerminalView& view,
                               IInput& terminalInput,
                               CellService& service,
                               ArgTransformer& argTransformer,
                               AtTransformer& atTransformer,
                               UserInputManager& userInputManager,
                               HelpShell& helpShell,
                               CellCallShell& cellCallShell,
                               CellSmsShell& cellSmsShell)
    : terminalView(view),
      terminalInput(terminalInput),
      cellService(service),
      argTransformer(argTransformer),
      atTransformer(atTransformer),
      userInputManager(userInputManager),
      helpShell(helpShell),
      cellCallShell(cellCallShell),
      cellSmsShell(cellSmsShell)
{
}

/*
Entry point for cell command
*/
void CellController::handleCommand(const TerminalCommand& command)
{
    const std::string& root = command.getRoot();

    if (root == "config")         handleConfig();
    else if (root == "modem")     handleModem();  
    else if (root == "network")   handleNetwork();
    else if (root == "sim")       handleSim();
    else if (root == "unlock")    handleUnlock(command); 
    else if (root == "setmode")   handleSetMode();
    else if (root == "sms")       handleSms(command); 
    else if (root == "ussd")      handleUssd(command); 
    else if (root == "call")      handleCall(command); 
    else handleHelp();
}

/*
Ensure configured
*/
void CellController::ensureConfigured()
{
    if (!configured) {
        handleConfig();
        return;
    }

    cellService.init(state.getUartRxPin(), state.getUartTxPin(), state.getUartBaudRate());
}

/*
Config
*/
void CellController::handleConfig()
{
    terminalView.println("Cell configuration");

    const auto& forbidden = state.getProtectedPins();

    uint8_t rx = userInputManager.readValidatedPinNumber("MODEM RX", state.getUartRxPin(), forbidden);
    state.setUartRxPin(rx);

    uint8_t tx = userInputManager.readValidatedPinNumber("MODEM TX", state.getUartTxPin(), forbidden);
    state.setUartTxPin(tx);

    uint32_t baud = userInputManager.readValidatedUint32("Baudrate", 115200);
    state.setUartBaudRate(baud);

    cellService.init(rx, tx, baud);

    terminalView.println("\nDetecting modem...");
    if (!cellService.detect()) {
        terminalView.println("❌ No modem detected.");
        terminalView.println("It may take up 30 sec to start.\n");
        configured = false;
        return;
    }

    configured = true;

    terminalView.println("✅ Modem detected");
    terminalView.println(atTransformer.formatModuleInfo(cellService.getModuleInfo()));
    terminalView.println("");
}

/*
Modem
*/
void CellController::handleModem()
{
    terminalView.println("\n[MODEM INFO]");
    terminalView.println("  " + atTransformer.formatModuleInfo(cellService.getModuleInfo()));
    terminalView.println("  " + atTransformer.formatManufacturer(cellService.getManufacturer()));
    terminalView.println("  Model: " + atTransformer.clean(cellService.getModel()));
    terminalView.println("  " + atTransformer.clean(cellService.getRevision()));
    terminalView.println("  " + atTransformer.formatImei(cellService.getImei()));
    terminalView.println("  " + atTransformer.formatFunctionality(cellService.getFunctionality())); 
    terminalView.println("\n  Use 'setmode' command to change mode.");
    terminalView.println("");
}

/*
SIM
*/
void CellController::handleSim()
{
    terminalView.println("\n[SIM CARD]");
    terminalView.println("  " + atTransformer.formatSimState(cellService.getSimState()));
    terminalView.println("  " + atTransformer.formatIccid(cellService.getIccid()));
    terminalView.println("  " + atTransformer.formatImsi(cellService.getImsi()));
    terminalView.println("  " + atTransformer.formatMsisdn(cellService.getMsisdn()));
    terminalView.println("  " + atTransformer.formatPinLock(cellService.getPinLockStatus())); // AT+CLCK="SC",2
    
    if (!cellService.isSimReady()) {
        terminalView.println("\n  Use 'unlock' command to enter SIM PIN.");
    } 

    terminalView.println("");
}

/*
Network
*/
void CellController::handleNetwork()
{
    terminalView.println("\n[NETWORK STATUS]");
    terminalView.println("  " + atTransformer.formatSignal(cellService.getSignal()));
    terminalView.println("  " + atTransformer.formatOperator(cellService.getOperator()));
    terminalView.println("  " + atTransformer.formatRegistrationCS(cellService.getRegistrationCS()));
    terminalView.println("  " + atTransformer.formatRegistrationPS(cellService.getRegistrationPS()));
    terminalView.println("  " + atTransformer.formatAttach(cellService.getAttach()));
    terminalView.println("");

    terminalView.println("[PDP]");
    terminalView.println("  " + atTransformer.formatPdpContexts(cellService.queryPdpContexts()));
    terminalView.println("  " + atTransformer.formatPdpActive(cellService.queryPdpActive()));
    terminalView.println("");
}

/*
Unlock SIM (enter PIN)
*/
void CellController::handleUnlock(const TerminalCommand& command)
{
    std::string pin = command.getSubcommand();
    if (pin.empty()) pin = command.getArgs();

    if (pin.empty()) {
        pin = userInputManager.readValidatedNumericCode("Enter SIM PIN", "", 4, 8);
    }

    bool ok = cellService.enterPin(pin);
    terminalView.println(ok ? "PIN accepted (OK)." : "PIN failed (ERROR).");
    terminalView.println(atTransformer.formatSimState(cellService.getSimState()) + "\n");
}

/*
SMS
*/
void CellController::handleSms(const TerminalCommand& command)
{
    if (cellService.isSimReady() == false) {
        terminalView.println("CELL: SIM not ready. Use 'unlock' command.");
        return;
    }

    cellSmsShell.run();
}

/*
USSD
*/
void CellController::handleUssd(const TerminalCommand& command)
{
    if (cellService.isSimReady() == false) {
        terminalView.println("CELL: SIM not ready. Use 'unlock' command.");
        return;
    }

    // ussd <code> [dcs]
    std::string code = command.getSubcommand();
    auto args = argTransformer.splitArgs(command.getArgs());

    // Interactive mode if nothing provided
    if (code.empty()) {
        terminalView.println("\n[USSD REQUEST]");

        code = userInputManager.readString("Enter USSD code", "*123#");
        const std::vector<std::string> dcsChoices = {
            "15 (Default GSM 7-bit)",
            "72 (UCS2 / Unicode)",
            "Exit"
        };

        int idx = userInputManager.readValidatedChoiceIndex("Select DCS encoding", dcsChoices, 0);
        if (idx == 2 || idx < 0) {
            terminalView.println("Cancelled.");
            return;
        }

        uint8_t dcs = (idx == 1) ? 72 : 15;

        terminalView.println("Sending USSD: " + code);
        bool ok = cellService.ussdRequest(code, dcs);
        terminalView.println(ok ? "USSD request: OK (response may arrive later)"
                               : "USSD request: ERROR");
        return;
    }

    // Provided args
    uint8_t dcs = 15;
    if (args.size() >= 2 && argTransformer.isValidNumber(args[1])) {
        dcs = (uint8_t)argTransformer.parseHexOrDec(args[1]);
    }

    bool ok = cellService.ussdRequest(code, dcs);
    terminalView.println(ok ? "USSD request: OK (response may arrive later)"
                           : "USSD request: ERROR");
}

/*
Call
*/
void CellController::handleCall(const TerminalCommand& command)
{
    if (cellService.isSimReady() == false) {
        terminalView.println("CELL: SIM not ready. Use 'unlock' command.");
        return;
    }

    cellCallShell.run();
}

/*
Set modem mode
*/
void CellController::handleSetMode()
{
    std::vector<std::string> choices = {
        "Full     (CFUN=1)   - normal mode",
        "Airplane (CFUN=4)   - RF disabled",
        "Exit"
    };

    int defaultIndex = (int)choices.size() - 1; // Exit selected by default

    int idx = userInputManager.readValidatedChoiceIndex("Select modem mode", choices, defaultIndex);
    if (idx == defaultIndex) {
        terminalView.println("\nExit setmode.");
        return;
    }

    bool ok = false;
    
    terminalView.println("");
    switch (idx) {
        case 0: // CFUN=1
            ok = cellService.setFunctionality(1);
            terminalView.println(ok ? "CFUN=1: OK" : "CFUN=1: ERROR");
            break;

        case 1: // CFUN=4
            ok = cellService.setFunctionality(4);
            terminalView.println(ok ? "CFUN=4: OK" : "CFUN=4: ERROR");
            break;

        default:
            terminalView.println("Invalid choice.");
            return;
    }

    terminalView.println("");
    terminalView.println(atTransformer.formatRegistrationCS(cellService.getRegistrationCS()));
    terminalView.println(atTransformer.formatRegistrationPS(cellService.getRegistrationPS()));
}

/*
Help
*/
void CellController::handleHelp()
{
    helpShell.run(state.getCurrentMode(), false);
}
