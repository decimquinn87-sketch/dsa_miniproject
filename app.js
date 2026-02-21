let currentMode = 'comp';

function switchMode(mode) {
    currentMode = mode;
    const btn = document.getElementById('mainBtn');
    const dropText = document.getElementById('dropText');
    const fileInput = document.getElementById('fileInput');
    
    fileInput.value = ""; 
    document.getElementById('tabComp').className = mode === 'comp' ? 'tab active' : 'tab';
    document.getElementById('tabDecomp').className = mode === 'decomp' ? 'tab active decomp' : 'tab';

    btn.innerText = mode === 'comp' ? "Compress to .bin" : "Restore to .txt";
    btn.className = mode === 'comp' ? "btn btn-comp" : "btn btn-decomp";
    dropText.innerHTML = `Click to select <b>.${mode === 'comp' ? 'txt' : 'bin'}</b>`;
}

function startApp() {
    const btn = document.getElementById('mainBtn');
    const fileInput = document.getElementById('fileInput');
    const dropText = document.getElementById('dropText');

    btn.disabled = false;

    fileInput.onchange = () => {
        if (fileInput.files[0]) dropText.innerHTML = `Selected: <b>${fileInput.files[0].name}</b>`;
    };

    btn.onclick = () => {
        const file = fileInput.files[0];
        if (!file) return alert("Select a file!");

        const reader = new FileReader();
        reader.onload = (e) => {
            const rawData = new Uint8Array(e.target.result);
            const ptr = Module._malloc(rawData.length);
            Module.HEAPU8.set(rawData, ptr);

            if (currentMode === 'comp') {
                Module.ccall('compress_text', null, ['number', 'number'], [ptr, rawData.length]);
                handleDownload(file.name.replace('.txt', '.bin'));
            } else {
                Module.ccall('decompress_file', null, ['number', 'number'], [ptr, rawData.length]);
                handleDownload("restored_" + file.name.replace('.bin', '.txt'));
            }
            Module._free(ptr);
        };
        reader.readAsArrayBuffer(file);
    };
}

function handleDownload(fileName) {
    const ptr = Module.ccall('get_result_ptr', 'number');
    const size = Module.ccall('get_result_size', 'number');
    if (size === 0) return alert("Operation failed.");

    const result = new Uint8Array(Module.HEAPU8.buffer, ptr, size).slice();
    const url = URL.createObjectURL(new Blob([result]));
    const a = document.createElement('a');
    a.href = url;
    a.download = fileName;
    a.click();
}

if (Module.calledRun) startApp();
else Module.onRuntimeInitialized = startApp;