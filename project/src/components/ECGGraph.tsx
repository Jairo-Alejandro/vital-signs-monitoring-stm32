import React, { useEffect, useRef, useState } from 'react';

interface ECGGraphProps {
  isPaused: boolean;
  isDark: boolean;
  color: string;
}

const ECGGraph: React.FC<ECGGraphProps> = ({ isPaused, isDark, color }) => {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const [data, setData] = useState<number[]>(Array(200).fill(0));
  
  useEffect(() => {
    if (isPaused) return;

    let frame = 0;
    const interval = setInterval(() => {
      setData(prev => {
        const newData = [...prev.slice(1)];
        frame = (frame + 1) % 100;
        
        // Enhanced ECG waveform simulation
        if (frame === 0) newData.push(0.1);
        else if (frame === 10) newData.push(1);
        else if (frame === 13) newData.push(-0.5);
        else if (frame === 15) newData.push(0.8);
        else if (frame === 17) newData.push(0.1);
        else if (frame === 20) newData.push(0.05);
        else newData.push(0);
        
        return newData;
      });
    }, 20);

    return () => clearInterval(interval);
  }, [isPaused]);

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;

    const ctx = canvas.getContext('2d');
    if (!ctx) return;

    // Set canvas size
    const rect = canvas.getBoundingClientRect();
    canvas.width = rect.width * window.devicePixelRatio;
    canvas.height = rect.height * window.devicePixelRatio;
    ctx.scale(window.devicePixelRatio, window.devicePixelRatio);
    canvas.style.width = `${rect.width}px`;
    canvas.style.height = `${rect.height}px`;

    // Clear canvas
    ctx.fillStyle = isDark ? '#1F2937' : '#FFFFFF';
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    
    // Draw minor grid lines
    ctx.beginPath();
    ctx.strokeStyle = isDark ? 'rgba(255, 255, 255, 0.1)' : 'rgba(0, 0, 0, 0.1)';
    ctx.lineWidth = 0.5;
    
    for (let x = 0; x < canvas.width; x += 20) {
      ctx.moveTo(x, 0);
      ctx.lineTo(x, canvas.height);
    }
    for (let y = 0; y < canvas.height; y += 20) {
      ctx.moveTo(0, y);
      ctx.lineTo(canvas.width, y);
    }
    ctx.stroke();

    // Draw major grid lines
    ctx.beginPath();
    ctx.strokeStyle = isDark ? 'rgba(255, 255, 255, 0.2)' : 'rgba(0, 0, 0, 0.2)';
    ctx.lineWidth = 1;
    
    for (let x = 0; x < canvas.width; x += 100) {
      ctx.moveTo(x, 0);
      ctx.lineTo(x, canvas.height);
    }
    for (let y = 0; y < canvas.height; y += 100) {
      ctx.moveTo(0, y);
      ctx.lineTo(canvas.width, y);
    }
    ctx.stroke();

    // Draw ECG line with glow effect
    ctx.beginPath();
    ctx.strokeStyle = color;
    ctx.lineWidth = 2;
    ctx.shadowBlur = 10;
    ctx.shadowColor = color;

    const scaleX = rect.width / (data.length - 1);
    const scaleY = rect.height / 2;

    data.forEach((value, index) => {
      const x = index * scaleX;
      const y = ((-value + 1) / 2) * rect.height;
      
      if (index === 0) {
        ctx.moveTo(x, y);
      } else {
        ctx.lineTo(x, y);
      }
    });

    ctx.stroke();
    
    // Reset shadow
    ctx.shadowBlur = 0;
  }, [data, isDark, color]);

  return (
    <canvas
      ref={canvasRef}
      className={`w-full h-full rounded-lg transition-colors duration-200 ${
        isDark ? 'bg-gray-800' : 'bg-white'
      }`}
    />
  );
};

export default ECGGraph;