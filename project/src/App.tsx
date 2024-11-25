import React, { useState } from 'react';
import { Settings, Moon, Sun, Pause, Play, Palette } from 'lucide-react';
import ECGGraph from './components/ECGGraph';
import VitalSign from './components/VitalSign';
import VitalChart from './components/VitalChart';
import ColorPicker from './components/ColorPicker';

function App() {
  const [isPaused, setIsPaused] = useState(false);
  const [isDark, setIsDark] = useState(true);
  const [showColorPicker, setShowColorPicker] = useState(false);
  const [graphColors, setGraphColors] = useState({
    ecg: '#10B981',
    heartRate: isDark ? 'rgb(16, 185, 129)' : 'rgb(5, 150, 105)',
    oxygen: isDark ? 'rgb(96, 165, 250)' : 'rgb(59, 130, 246)',
    temperature: isDark ? 'rgb(250, 204, 21)' : 'rgb(202, 138, 4)'
  });

  const updateGraphColor = (type: keyof typeof graphColors, color: string) => {
    setGraphColors(prev => ({ ...prev, [type]: color }));
  };

  return (
    <div className={`min-h-screen transition-colors duration-200 ${
      isDark ? 'bg-gray-900 text-white' : 'bg-gray-50 text-gray-900'
    }`}>
      {/* Top Navigation Bar */}
      <nav className={`${
        isDark ? 'bg-gray-800' : 'bg-white'
      } border-b ${isDark ? 'border-gray-700' : 'border-gray-200'} sticky top-0 z-50`}>
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="flex justify-between items-center h-16">
            <div className="flex items-center">
              <span className="text-xl font-bold">VitalMonitor Pro</span>
            </div>
            <div className="flex items-center gap-4">
              <button
                onClick={() => setShowColorPicker(!showColorPicker)}
                className={`p-2 rounded-lg ${
                  isDark 
                    ? 'hover:bg-gray-700 text-purple-400' 
                    : 'hover:bg-gray-100 text-purple-600'
                }`}
                title="Customize Colors"
              >
                <Palette className="w-5 h-5" />
              </button>
              <button
                onClick={() => setIsDark(!isDark)}
                className={`p-2 rounded-lg ${
                  isDark 
                    ? 'hover:bg-gray-700 text-yellow-400' 
                    : 'hover:bg-gray-100 text-gray-600'
                }`}
              >
                {isDark ? <Sun className="w-5 h-5" /> : <Moon className="w-5 h-5" />}
              </button>
              <button
                onClick={() => setIsPaused(!isPaused)}
                className={`flex items-center gap-2 px-4 py-2 rounded-lg ${
                  isDark 
                    ? 'bg-gray-700 hover:bg-gray-600' 
                    : 'bg-gray-100 hover:bg-gray-200'
                }`}
              >
                {isPaused ? <Play className="w-4 h-4" /> : <Pause className="w-4 h-4" />}
                {isPaused ? 'Resume' : 'Pause'}
              </button>
              <button className={`p-2 rounded-lg ${
                isDark 
                  ? 'hover:bg-gray-700' 
                  : 'hover:bg-gray-100'
              }`}>
                <Settings className="w-5 h-5" />
              </button>
            </div>
          </div>
        </div>
      </nav>

      {showColorPicker && (
        <ColorPicker
          colors={graphColors}
          onColorChange={updateGraphColor}
          onClose={() => setShowColorPicker(false)}
          isDark={isDark}
        />
      )}

      {/* Main Content */}
      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-6">
        <div className="space-y-6">
          {/* ECG Section */}
          <div className={`rounded-xl shadow-lg overflow-hidden ${
            isDark ? 'bg-gray-800' : 'bg-white'
          } transition-all duration-200 hover:shadow-xl`}>
            <div className="p-4 border-b border-gray-700">
              <div className="flex justify-between items-center">
                <div>
                  <h2 className="text-xl font-semibold">Electrocardiogram (ECG)</h2>
                  <p className={`text-sm ${isDark ? 'text-gray-400' : 'text-gray-500'}`}>
                    Real-time monitoring
                  </p>
                </div>
                <div className="text-emerald-400 text-2xl font-bold">
                  125 BPM
                </div>
              </div>
            </div>
            <div className="p-4">
              <div className="h-80">
                <ECGGraph isPaused={isPaused} isDark={isDark} color={graphColors.ecg} />
              </div>
            </div>
          </div>

          {/* Vital Signs Grid */}
          <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
            <div className={`rounded-xl shadow-lg overflow-hidden ${
              isDark ? 'bg-gray-800' : 'bg-white'
            } transition-all duration-200 hover:shadow-xl`}>
              <VitalSign
                title="Heart Rate"
                value={125}
                unit="BPM"
                trend="up"
                color={graphColors.heartRate}
                change="+5"
                isDark={isDark}
              />
              <div className="p-4">
                <VitalChart 
                  type="heartRate" 
                  isDark={isDark} 
                  color={graphColors.heartRate}
                />
              </div>
            </div>

            <div className={`rounded-xl shadow-lg overflow-hidden ${
              isDark ? 'bg-gray-800' : 'bg-white'
            } transition-all duration-200 hover:shadow-xl`}>
              <VitalSign
                title="Blood Oxygen"
                value={98}
                unit="%"
                trend="stable"
                color={graphColors.oxygen}
                isDark={isDark}
              />
              <div className="p-4">
                <VitalChart 
                  type="oxygen" 
                  isDark={isDark}
                  color={graphColors.oxygen}
                />
              </div>
            </div>

            <div className={`rounded-xl shadow-lg overflow-hidden ${
              isDark ? 'bg-gray-800' : 'bg-white'
            } transition-all duration-200 hover:shadow-xl`}>
              <VitalSign
                title="Temperature"
                value={37.2}
                unit="°C"
                trend="down"
                color={graphColors.temperature}
                change="-0.3"
                isDark={isDark}
              />
              <div className="p-4">
                <VitalChart 
                  type="temperature" 
                  isDark={isDark}
                  color={graphColors.temperature}
                />
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;