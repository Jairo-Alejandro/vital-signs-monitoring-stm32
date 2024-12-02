const hrChart = new Chart(document.getElementById('hrChart').getContext('2d'), {
    type: 'line',
    data: { labels: [], datasets: [{ label: 'Heart Rate', data: [], borderColor: 'red', fill: false }] },
    options: { scales: { y: { beginAtZero: true } } }
  });
  
  const spo2Chart = new Chart(document.getElementById('spo2Chart').getContext('2d'), {
    type: 'line',
    data: { labels: [], datasets: [{ label: 'SpO2', data: [], borderColor: 'blue', fill: false }] },
    options: { scales: { y: { beginAtZero: true } } }
  });
  
  const ecgChart = new Chart(document.getElementById('ecgChart').getContext('2d'), {
    type: 'line',
    data: { labels: [], datasets: [{ label: 'ECG', data: [], borderColor: 'green', fill: false }] },
    options: { scales: { y: { beginAtZero: true } } }
  });
  
  function updateCharts(data) {
    const now = new Date().toLocaleTimeString();
    if (data.hr !== null) {
      hrChart.data.labels.push(now);
      hrChart.data.datasets[0].data.push(data.hr);
      hrChart.update();
    }
    if (data.spo2 !== null) {
      spo2Chart.data.labels.push(now);
      spo2Chart.data.datasets[0].data.push(data.spo2);
      spo2Chart.update();
    }
    if (data.ecg !== null) {
      ecgChart.data.labels.push(now);
      ecgChart.data.datasets[0].data.push(data.ecg);
      ecgChart.update();
    }
  }
  
  setInterval(() => {
    fetch('/data').then(res => res.json()).then(data => updateCharts(data));
  }, 1000);
  