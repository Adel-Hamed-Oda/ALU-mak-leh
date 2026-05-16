let lastClk = -1; 
let previousRegisters = [];
let pipelineRows = JSON.parse(localStorage.getItem("meowArchPipeline")) || [];

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
    // 1. THE FIX: Handle initial page load desync
    if (lastClk === -1 && pipelineRows.length > 0) {
        const lastStoredCycle = pipelineRows[pipelineRows.length - 1].cycle;
        // If the simulator is at cycle 0, or its clock is behind our saved history, wipe it.
        if (state.clk === 0 || state.clk <= lastStoredCycle) {
            pipelineRows = [];
            localStorage.removeItem("meowArchPipeline");
        }
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
