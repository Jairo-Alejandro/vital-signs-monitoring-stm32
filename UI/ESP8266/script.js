const ecgDataStorage = [];
let captureInterval = null;

const ctx = document.getElementById('ecgChart').getContext('2d');
const ecgChart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: Array(100).fill(''),
        datasets: [{
            label: 'ECG (mV)',
            data: Array(100).fill(0),
            borderColor: 'rgba(75, 192, 192, 1)',
            borderWidth: 2,
            tension: 0.1
        }]
    },
    options: {
        responsive: true,
        scales: {
            x: { display: false },
            y: { beginAtZero: false }
        }
    }
});

let lastData = null;
document.getElementById('startCapture').addEventListener('click', () => {
    if (captureInterval) {
        clearInterval(captureInterval);
        captureInterval = null;
        startCaptureButton.textContent = 'Start Capture';
        exportCsvButton.disabled = false;
    } else {
        const interval = parseInt(intervalInput.value, 10);
        if (isNaN(interval) || interval <= 0) {
            alert('Please enter a valid interval (ms).');
            return;
        }
        captureInterval = setInterval(captureData, interval);
        startCaptureButton.textContent = 'Stop Capture';
        exportCsvButton.disabled = true;
    }
});

document.getElementById('exportCsv').addEventListener('click', () => {
    if (ecgDataStorage.length === 0) {
        alert('No data to export.');
        return;
    }
    const csvContent = generateCsv(ecgDataStorage);
    downloadCsv(csvContent, 'patient_data.csv');
});

const socket = new WebSocket('ws://' + window.location.hostname + ':81/');
socket.onmessage = function (event) {
    const data = JSON.parse(event.data);
    lastData = data;

    updateField('hr', `${data.HR} bpm`, data.HR, 60, 100);
    updateField('spo2', `${data.SpO2}%`, data.SpO2, 95, 100);
    updateField('temperature', `${data.Temperature} °C`, data.Temperature, 36.1, 37.2);

    ecgChart.data.datasets[0].data.push(data.ECG);
    if (ecgChart.data.datasets[0].data.length > 100) ecgChart.data.datasets[0].data.shift();
    ecgChart.update();
};

function updateField(fieldId, value, data, min, max) {
    document.getElementById(fieldId).textContent = value;
    const status = document.getElementById(`${fieldId}-status`);
    status.textContent = data < min ? 'Low' : data > max ? 'High' : 'Normal';
    status.className = `status ${data < min ? 'danger' : data > max ? 'warning' : 'normal'}`;
}

function captureData() {
    if (lastData) {
        ecgDataStorage.push({
            timestamp: new Date().toISOString(),
            ...lastData
        });
    }
}

function generateCsv(data) {
    const headers = ['Timestamp', 'HR', 'SpO2', 'Temperature', 'ECG'];
    return [headers.join(','), ...data.map(row => Object.values(row).join(','))].join('\n');
}

function downloadCsv(content, filename) {
    const blob = new Blob([content], { type: 'text/csv;charset=utf-8;' });
    const link = document.createElement('a');
    link.href = URL.createObjectURL(blob);
    link.setAttribute('download', filename);
    link.click();
}
