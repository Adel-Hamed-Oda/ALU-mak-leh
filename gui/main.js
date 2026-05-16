let lastClk = -1; 
const pipelineRows = [];

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
    if (state.clk === lastClk) return;

    lastClk = state.clk;
    
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
