import React from 'react';
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Filler
} from 'chart.js';
import { Line } from 'react-chartjs-2';

ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Filler
);

interface VitalChartProps {
  type: 'heartRate' | 'oxygen' | 'temperature';
  isDark: boolean;
  color: string;
}

const VitalChart: React.FC<VitalChartProps> = ({ type, isDark, color }) => {
  const labels = Array.from({ length: 12 }, (_, i) => `${i * 5}m`);

  const getRandomData = (min: number, max: number) => {
    return Array.from({ length: 12 }, () => 
      Math.floor(Math.random() * (max - min + 1) + min)
    );
  };

  const chartData = {
    heartRate: {
      data: getRandomData(60, 100),
      label: 'BPM'
    },
    oxygen: {
      data: getRandomData(95, 100),
      label: '%'
    },
    temperature: {
      data: getRandomData(365, 380).map(val => val / 10),
      label: '°C'
    },
  };

  const data = {
    labels,
    datasets: [
      {
        label: chartData[type].label,
        fill: true,
        data: chartData[type].data,
        borderColor: color,
        backgroundColor: `${color}20`,
        borderWidth: 2,
        pointRadius: 3,
        pointHoverRadius: 5,
        tension: 0.4,
      },
    ],
  };

  const options = {
    responsive: true,
    plugins: {
      tooltip: {
        mode: 'index' as const,
        intersect: false,
        backgroundColor: isDark ? 'rgba(0, 0, 0, 0.8)' : 'rgba(255, 255, 255, 0.8)',
        titleColor: isDark ? '#fff' : '#000',
        bodyColor: isDark ? '#fff' : '#000',
        borderColor: isDark ? 'rgba(255, 255, 255, 0.1)' : 'rgba(0, 0, 0, 0.1)',
        borderWidth: 1,
      },
      legend: {
        display: false,
      },
    },
    scales: {
      y: {
        grid: {
          color: isDark ? 'rgba(255, 255, 255, 0.1)' : 'rgba(0, 0, 0, 0.1)',
        },
        ticks: {
          color: isDark ? 'rgba(255, 255, 255, 0.7)' : 'rgba(0, 0, 0, 0.7)',
          font: {
            size: 10,
          },
        },
      },
      x: {
        grid: {
          display: false,
        },
        ticks: {
          color: isDark ? 'rgba(255, 255, 255, 0.7)' : 'rgba(0, 0, 0, 0.7)',
          font: {
            size: 10,
          },
        },
      },
    },
    maintainAspectRatio: false,
    interaction: {
      intersect: false,
      mode: 'index' as const,
    },
  };

  return (
    <div className="h-40">
      <Line data={data} options={options} />
    </div>
  );
};

export default VitalChart;