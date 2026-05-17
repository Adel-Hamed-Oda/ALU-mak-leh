let lastClk = -1;
let pipelineRows = loadPipelineRows();
let lastHighlightedRegister = null;
let lastState = null;
const REGISTER_WRITING_OPCODES = new Set([
    "ADD",
    "SUB",
    "MUL",
    "LDI",
    "AND",
    "OR",
    "SAL",
    "SAR",
    "LB"
]);
const DISPLAY_SETTINGS_STORAGE_KEY = "meowArchDisplaySettings";
const DEFAULT_DISPLAY_SETTINGS = {
    registers: "signed",
    instructions: "text",
    data: "signed"
};
const DISPLAY_SETTING_OPTIONS = {
    registers: new Set(["binary", "signed", "unsigned"]),
    instructions: new Set(["text", "binary"]),
    data: new Set(["binary", "signed", "unsigned"])
};
let displaySettings = loadDisplaySettings();

function loadPipelineRows() {
    try {
        const storedRows = localStorage.getItem("meowArchPipeline");
        return storedRows ? JSON.parse(storedRows) : [];
    } catch (error) {
        localStorage.removeItem("meowArchPipeline");
        return [];
    }
}

function normalizeDisplaySettings(settings) {
    const normalized = { ...DEFAULT_DISPLAY_SETTINGS };

    Object.entries(DISPLAY_SETTING_OPTIONS).forEach(([key, allowedValues]) => {
        if (allowedValues.has(settings?.[key])) {
            normalized[key] = settings[key];
        }
    });

    return normalized;
}

function loadDisplaySettings() {
    try {
        const storedSettings = localStorage.getItem(DISPLAY_SETTINGS_STORAGE_KEY);
        return normalizeDisplaySettings(storedSettings ? JSON.parse(storedSettings) : {});
    } catch (error) {
        localStorage.removeItem(DISPLAY_SETTINGS_STORAGE_KEY);
        return { ...DEFAULT_DISPLAY_SETTINGS };
    }
}

function saveDisplaySettings() {
    localStorage.setItem(DISPLAY_SETTINGS_STORAGE_KEY, JSON.stringify(displaySettings));
}

function syncSettingsControls() {
    const registerSelect = document.getElementById("register-format-select");
    const instructionSelect = document.getElementById("instruction-format-select");
    const dataSelect = document.getElementById("data-format-select");

    if (registerSelect) {
        registerSelect.value = displaySettings.registers;
    }

    if (instructionSelect) {
        instructionSelect.value = displaySettings.instructions;
    }

    if (dataSelect) {
        dataSelect.value = displaySettings.data;
    }
}

function updateDisplaySetting(key, value) {
    if (!DISPLAY_SETTING_OPTIONS[key]?.has(value)) {
        return;
    }

    displaySettings = {
        ...displaySettings,
        [key]: value
    };
    saveDisplaySettings();

    if (lastState) {
        renderState(lastState);
    }
}

async function sendCommand(action, payload = null) {
    const options = {
        method: "POST",
    };

    if (payload) {
        options.headers = {
            "Content-Type": "application/json",
        };
        options.body = JSON.stringify(payload);
    }

    const res = await fetch(`/${action}`, options);
    if (!res.ok) {
        throw new Error(`Command failed: ${action}`);
    }

    return await res.json();
}

function resetPipelineHistory() {
    lastClk = -1;
    pipelineRows = [];
    localStorage.removeItem("meowArchPipeline");
    renderPipelineTable();
}

function setLoadModalTitle(title) {
    document.getElementById("load-modal-title").textContent = title;
}

function showProgramListView() {
    document.getElementById("program-picker-view").classList.remove("hidden");
    document.getElementById("program-create-view").classList.add("hidden");
    setLoadModalTitle("Select Program");
}

function showCreateProgramView() {
    document.getElementById("program-picker-view").classList.add("hidden");
    document.getElementById("program-create-view").classList.remove("hidden");
    document.getElementById("program-create-status").textContent = "";
    document.getElementById("program-filename").focus();
    setLoadModalTitle("New Program");
}

function closeLoadModal() {
    const modal = document.getElementById("load-modal");
    modal.classList.add("hidden");
    modal.setAttribute("aria-hidden", "true");
}

function closeSettingsModal() {
    const modal = document.getElementById("settings-modal");
    modal.classList.add("hidden");
    modal.setAttribute("aria-hidden", "true");
}

async function loadProgramList() {
    const programList = document.getElementById("program-list");

    programList.innerHTML = `<div class="program-list-empty">Loading programs...</div>`;

    try {
        const res = await fetch(`/api/programs?t=${Date.now()}`);
        if (!res.ok) {
            throw new Error("Could not load programs");
        }

        const programs = await res.json();
        programList.innerHTML = "";

        if (!Array.isArray(programs) || programs.length === 0) {
            programList.innerHTML = `<div class="program-list-empty">No programs found</div>`;
            return;
        }

        programs.forEach(filename => {
            const button = document.createElement("button");
            button.type = "button";
            button.className = "program-option";
            button.textContent = filename;
            button.addEventListener("click", async () => {
                await sendCommand("load", { filename });
                resetPipelineHistory();
                closeLoadModal();
            });

            programList.appendChild(button);
        });
    } catch (error) {
        programList.innerHTML = `<div class="program-list-empty">Could not load programs</div>`;
    }
}

async function openLoadModal() {
    const modal = document.getElementById("load-modal");

    modal.classList.remove("hidden");
    modal.setAttribute("aria-hidden", "false");
    showProgramListView();
    await loadProgramList();
}

function openSettingsModal() {
    const modal = document.getElementById("settings-modal");
    const registerSelect = document.getElementById("register-format-select");

    syncSettingsControls();
    modal.classList.remove("hidden");
    modal.setAttribute("aria-hidden", "false");

    if (registerSelect) {
        registerSelect.focus();
    }
}

async function createProgram(event) {
    event.preventDefault();

    const filenameInput = document.getElementById("program-filename");
    const codeInput = document.getElementById("program-code");
    const status = document.getElementById("program-create-status");

    status.textContent = "";

    try {
        const res = await fetch("/api/programs", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
            },
            body: JSON.stringify({
                filename: filenameInput.value,
                content: codeInput.value,
            }),
        });

        const payload = await res.json();

        if (!res.ok) {
            status.textContent = payload.message || "Could not save program";
            return;
        }

        filenameInput.value = "";
        codeInput.value = "";
        showProgramListView();
        await loadProgramList();
    } catch (error) {
        status.textContent = "Could not save program";
    }
}

window.openLoadModal = openLoadModal;
window.closeLoadModal = closeLoadModal;
window.openSettingsModal = openSettingsModal;
window.closeSettingsModal = closeSettingsModal;

document.addEventListener("DOMContentLoaded", () => {
    const loadButton = document.getElementById("load-program-button");
    const settingsButton = document.getElementById("settings-button");
    const closeButton = document.getElementById("close-load-modal");
    const closeSettingsButton = document.getElementById("close-settings-modal");
    const cancelButton = document.getElementById("cancel-load-modal");
    const newButton = document.getElementById("new-program-button");
    const backButton = document.getElementById("back-to-program-list");
    const createForm = document.getElementById("program-create-view");
    const modal = document.getElementById("load-modal");
    const settingsModal = document.getElementById("settings-modal");

    if (loadButton) {
        loadButton.addEventListener("click", openLoadModal);
    }

    if (settingsButton) {
        settingsButton.addEventListener("click", openSettingsModal);
    }

    if (closeButton) {
        closeButton.addEventListener("click", closeLoadModal);
    }

    if (closeSettingsButton) {
        closeSettingsButton.addEventListener("click", closeSettingsModal);
    }

    if (cancelButton) {
        cancelButton.addEventListener("click", closeLoadModal);
    }

    if (newButton) {
        newButton.addEventListener("click", showCreateProgramView);
    }

    if (backButton) {
        backButton.addEventListener("click", showProgramListView);
    }

    if (createForm) {
        createForm.addEventListener("submit", createProgram);
    }

    if (modal) {
        modal.addEventListener("click", event => {
            if (event.target === modal) {
                closeLoadModal();
            }
        });
    }

    if (settingsModal) {
        settingsModal.addEventListener("click", event => {
            if (event.target === settingsModal) {
                closeSettingsModal();
            }
        });
    }

    syncSettingsControls();

    [
        ["register-format-select", "registers"],
        ["instruction-format-select", "instructions"],
        ["data-format-select", "data"]
    ].forEach(([selectId, settingKey]) => {
        const select = document.getElementById(selectId);

        if (select) {
            select.addEventListener("change", event => {
                updateDisplaySetting(settingKey, event.target.value);
            });
        }
    });
});

document.addEventListener("keydown", event => {
    if (event.key === "Escape") {
        closeLoadModal();
        closeSettingsModal();
    }
});

async function fetchState() {
    const res = await fetch(`./data.json?t=${Date.now()}`);
    return await res.json();
}
function renderMemory(containerId, memory, formatValue = String, markerIndex = null) {
    const container = document.getElementById(containerId);

    container.innerHTML = "";

    memory.forEach((value, index) => {
        const displayValue = formatValue(value, index);

        if (displayValue === null || displayValue === undefined) {
            return;
        }

        const div = document.createElement("div");
        const memoryAddress = document.createElement("span");
        const memoryValue = document.createElement("span");

        div.className = "memory-cell";
        memoryAddress.className = "memory-address";
        memoryValue.className = "memory-value";

        memoryAddress.textContent = `[${index}]`;
        memoryValue.textContent = displayValue;

        div.appendChild(memoryAddress);
        div.appendChild(memoryValue);

        if (index === markerIndex) {
            const marker = document.createElement("span");
            marker.className = "pc-marker";
            marker.textContent = "*";
            div.appendChild(marker);
        }

        container.appendChild(div);
    });
}

function getWrittenRegisterFromInstruction(encodedInstruction) {
    const rawInstruction = Number(encodedInstruction);

    if (!Number.isFinite(rawInstruction) || rawInstruction === -1) {
        return null;
    }

    const instruction = rawInstruction & 0xffff;

    if (instruction === END_OF_PROGRAM_INSTRUCTION) {
        return null;
    }

    const opcodeName = OPCODE_NAMES[(instruction >>> 12) & 0x0f];

    if (!REGISTER_WRITING_OPCODES.has(opcodeName)) {
        return null;
    }

    return (instruction >>> 6) & 0x3f;
}

function getRegisterIndex(address) {
    const numericAddress = Number(address);

    if (Number.isInteger(numericAddress)) {
        return numericAddress;
    }

    const match = String(address).match(/^R(\d+)$/i);
    return match ? Number(match[1]) : null;
}

function getHighlightedRegister(state) {
    if (
        state.clk === 0 &&
        state.current_instruction === -1 &&
        state.executing_instruction === -1
    ) {
        lastHighlightedRegister = null;
    }

    const writtenRegister = getWrittenRegisterFromInstruction(state.executing_instruction);

    if (writtenRegister !== null) {
        lastHighlightedRegister = writtenRegister;
    }

    return lastHighlightedRegister;
}

function renderRegisters(registers, highlightedRegister = null, formatValue = String) {
    const container = document.getElementById("registers");

    container.innerHTML = "";

    const registerEntries = Array.isArray(registers)
        ? registers.map((value, index) => [`R${index}`, value, index])
        : Object.entries(registers || {}).map(([address, value]) => {
            const registerIndex = getRegisterIndex(address);
            const registerAddress = registerIndex !== null ? `R${registerIndex}` : address;
            return [registerAddress, value, registerIndex];
        });

    registerEntries.forEach(([address, value, registerIndex]) => {
        const div = document.createElement("div");
        const registerAddress = document.createElement("span");
        const registerValue = document.createElement("span");

        div.className = "register-cell";
        if (registerIndex === highlightedRegister) {
            div.classList.add("register-cell-highlight");
        }
        registerAddress.className = "register-address";
        registerValue.className = "register-value";

        registerAddress.textContent = address;
        registerValue.textContent = formatValue(value, registerIndex);

        div.appendChild(registerAddress);
        div.appendChild(registerValue);

        container.appendChild(div);
    });
}

function formatBinary(value, bits) {
    const normalized = Number(value) & ((1 << bits) - 1);
    return normalized.toString(2).padStart(bits, "0");
}

function formatIntegerValue(value, format, bits = 8) {
    if (format === "binary") {
        return formatBinary(value, bits);
    }

    if (format === "unsigned") {
        return String(Number(value) & ((1 << bits) - 1));
    }

    return String(Number(value));
}

function formatRegisterValue(value) {
    return formatIntegerValue(value, displaySettings.registers, 8);
}

function formatDataMemoryValue(value) {
    return formatIntegerValue(value, displaySettings.data, 8);
}

function formatInstructionMemoryValue(value) {
    if (displaySettings.instructions === "binary") {
        return formatBinary(value, 16);
    }

    return decodeInstruction(value);
}

function renderSREG(value) {
    document.getElementById("sreg-binary").textContent = formatBinary(value, 8);
}


function updatePipeline(state) {
    const isInitialResetState =
        state.clk === 0 &&
        state.current_instruction === -1 &&
        (state.opcode === 12 || state.opcode === -1);
    const isFinishedState =
        state.current_instruction === -1 &&
        (state.opcode === 12 || state.opcode === -1) &&
        Array.isArray(state.instruction_memory) &&
        state.instruction_memory[state.PC] === 65535;

    // 1. THE FIX: Handle initial page load desync
    if (lastClk === -1 && pipelineRows.length > 0) {
        const lastStoredCycle = pipelineRows[pipelineRows.length - 1].cycle;
        // If the simulator is at cycle 0, or its clock is behind our saved history, wipe it.
        if (state.clk === 0 || state.clk <= lastStoredCycle) {
            pipelineRows = [];
            localStorage.removeItem("meowArchPipeline");
        }
    }

    if (isInitialResetState) {
        lastClk = -1;
        pipelineRows = [];
        localStorage.removeItem("meowArchPipeline");
        renderPipelineTable();
        return;
    }

    // 2. Handle runtime resets (when the simulator resets while the page is open)
    if (lastClk !== -1 && state.clk < lastClk) {
        pipelineRows = [];
        localStorage.removeItem("meowArchPipeline");
    }

    // Prevent duplicate entries for the same clock cycle
    if (state.clk === lastClk) return;
    lastClk = state.clk;

    // Resolve IF Stage (Fetch)
    let ifStage = "-";
    if (state.current_instruction !== -1) {
        ifStage = decodeInstruction(state.current_instruction);
    }

    // Resolve ID Stage (Decode)
    let idStage = "-";
    if (state.opcode !== 12 && state.opcode !== -1) { 
        const opName = OPCODE_NAMES[state.opcode] || "UNKNOWN";
        const operand = REGISTER_OPERAND_OPCODES.has(opName) 
            ? `R${state.R2_imm}` 
            : formatImmediate(opName, state.R2_imm);
        idStage = `${opName} R${state.R1} ${operand}`;
    }

    // Resolve EX Stage (Execute)
    let exStage = "-";
    if (state.executing_instruction !== undefined && state.executing_instruction !== -1) {
        exStage = decodeInstruction(state.executing_instruction);
    } else if (state.executing_instruction === undefined && pipelineRows.length > 0) {
        exStage = pipelineRows[pipelineRows.length - 1].id;
    }

    pipelineRows.push({
        cycle: state.clk,
        if: ifStage,
        id: idStage,
        ex: exStage
    });

    if (pipelineRows.length > 15) {
        pipelineRows.shift();
    }

    localStorage.setItem("meowArchPipeline", JSON.stringify(pipelineRows));

    renderPipelineTable();
}

function renderPipelineTable() {
    const tbody = document.querySelector("#pipeline-table tbody");
    tbody.innerHTML = "";

    pipelineRows.forEach(row => {
        const tr = document.createElement("tr");

        tr.innerHTML = `
            <td class="pipeline-cycle-cell">${row.cycle}</td>
            <td class="pipeline-stage-cell">${row.if !== "-" ? `<span class="pipeline-instruction">${row.if}</span>` : ""}</td>
            <td class="pipeline-stage-cell">${row.id !== "-" ? `<span class="pipeline-instruction">${row.id}</span>` : ""}</td>
            <td class="pipeline-stage-cell">${row.ex !== "-" ? `<span class="pipeline-instruction">${row.ex}</span>` : ""}</td>
        `;

        tbody.appendChild(tr);
    });

    const tableContainer = document.getElementById("pipeline-table");
    tableContainer.scrollTop = tableContainer.scrollHeight;
}

function renderState(state) {
    renderMemory("instruction-memory", state.instruction_memory, formatInstructionMemoryValue, state.PC);
    renderMemory("data-memory", state.data_memory, formatDataMemoryValue);
    renderSREG(state.SREG);
    renderRegisters(state.registers, getHighlightedRegister(state), formatRegisterValue);
    updatePipeline(state);
}

async function updateUI() {
    let state;
    try {
        state = await fetchState();
    } catch (error) {
        return;
    }
    lastState = state;
    renderState(state);
}
updateUI();
setInterval(updateUI, 500);
