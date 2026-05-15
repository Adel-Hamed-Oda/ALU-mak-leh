async function fetchState() {
    const res = await fetch("./data.json");
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

async function updateUI() {
    const state = await fetchState();

    renderMemory("instruction-memory", state.instruction_memory, decodeInstruction);
    renderMemory("data-memory", state.data_memory);
}

updateUI();
