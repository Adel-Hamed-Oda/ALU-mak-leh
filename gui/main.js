let lastClk = -1;
let pipelineRows = loadPipelineRows();

function loadPipelineRows() {
    try {
        const storedRows = localStorage.getItem("meowArchPipeline");
        return storedRows ? JSON.parse(storedRows) : [];
    } catch (error) {
        localStorage.removeItem("meowArchPipeline");
        return [];
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

function closeLoadModal() {
    const modal = document.getElementById("load-modal");
    modal.classList.add("hidden");
    modal.setAttribute("aria-hidden", "true");
}

async function openLoadModal() {
    const modal = document.getElementById("load-modal");
    const programList = document.getElementById("program-list");

    modal.classList.remove("hidden");
    modal.setAttribute("aria-hidden", "false");
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

window.openLoadModal = openLoadModal;
window.closeLoadModal = closeLoadModal;

document.addEventListener("DOMContentLoaded", () => {
    const loadButton = document.getElementById("load-program-button");
    const closeButton = document.getElementById("close-load-modal");
    const cancelButton = document.getElementById("cancel-load-modal");
    const modal = document.getElementById("load-modal");

    if (loadButton) {
        loadButton.addEventListener("click", openLoadModal);
    }

    if (closeButton) {
        closeButton.addEventListener("click", closeLoadModal);
    }

    if (cancelButton) {
        cancelButton.addEventListener("click", closeLoadModal);
    }

    if (modal) {
        modal.addEventListener("click", event => {
            if (event.target === modal) {
                closeLoadModal();
            }
        });
    }
});

document.addEventListener("keydown", event => {
    if (event.key === "Escape") {
        closeLoadModal();
    }
});

async function fetchState() {
    const res = await fetch(`./data.json?t=${Date.now()}`);
    return await res.json();
}
function renderMemory(containerId, memory, formatValue = String) {
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

        container.appendChild(div);
    });
}
function renderRegisters(registers) {
    const container = document.getElementById("registers");

    container.innerHTML = "";

    const registerEntries = Array.isArray(registers)
        ? registers.map((value, index) => [`R${index}`, value])
        : Object.entries(registers || {}).map(([address, value]) => {
            const registerAddress = Number.isInteger(Number(address)) ? `R${address}` : address;
            return [registerAddress, value];
        });

    registerEntries.forEach(([address, value]) => {
        const div = document.createElement("div");
        const registerAddress = document.createElement("span");
        const registerValue = document.createElement("span");

        div.className = "register-cell";
        registerAddress.className = "register-address";
        registerValue.className = "register-value";

        registerAddress.textContent = address;
        registerValue.textContent = value;

        div.appendChild(registerAddress);
        div.appendChild(registerValue);

        container.appendChild(div);
    });
}


function updatePipeline(state) {
    const isInitialResetState =
        state.clk === 0 &&
        state.current_instruction === -1 &&
        (state.opcode === 12 || state.opcode === -1);

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
    if (pipelineRows.length > 0) {
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

async function updateUI() {
    let state;
    try {
        state = await fetchState();
    } catch (error) {
        return;
    }
    renderMemory("instruction-memory", state.instruction_memory, decodeInstruction);
    renderMemory("data-memory", state.data_memory);
    renderRegisters(state.registers);
    updatePipeline(state);
}
updateUI();
setInterval(updateUI, 500);
