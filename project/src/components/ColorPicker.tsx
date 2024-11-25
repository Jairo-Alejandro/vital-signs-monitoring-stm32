import React from 'react';
import { X } from 'lucide-react';

interface ColorPickerProps {
  colors: {
    ecg: string;
    heartRate: string;
    oxygen: string;
    temperature: string;
  };
  onColorChange: (type: keyof typeof colors, color: string) => void;
  onClose: () => void;
  isDark: boolean;
}

const ColorPicker: React.FC<ColorPickerProps> = ({
  colors,
  onColorChange,
  onClose,
  isDark
}) => {
  const colorOptions = [
    '#10B981', // Emerald
    '#3B82F6', // Blue
    '#F59E0B', // Amber
    '#EC4899', // Pink
    '#8B5CF6', // Purple
    '#EF4444', // Red
  ];

  return (
    <div className={`fixed inset-x-0 top-16 z-40 p-4 ${
      isDark ? 'bg-gray-800' : 'bg-white'
    } border-b ${isDark ? 'border-gray-700' : 'border-gray-200'} shadow-lg`}>
      <div className="max-w-7xl mx-auto">
        <div className="flex justify-between items-center mb-4">
          <h3 className="text-lg font-semibold">Customize Graph Colors</h3>
          <button
            onClick={onClose}
            className={`p-2 rounded-lg ${
              isDark ? 'hover:bg-gray-700' : 'hover:bg-gray-100'
            }`}
          >
            <X className="w-5 h-5" />
          </button>
        </div>
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
          {(Object.keys(colors) as Array<keyof typeof colors>).map((type) => (
            <div key={type} className="space-y-2">
              <label className="block text-sm font-medium capitalize">
                {type.replace(/([A-Z])/g, ' $1').trim()}
              </label>
              <div className="flex flex-wrap gap-2">
                {colorOptions.map((color) => (
                  <button
                    key={color}
                    onClick={() => onColorChange(type, color)}
                    className={`w-8 h-8 rounded-full border-2 ${
                      colors[type] === color
                        ? isDark ? 'border-white' : 'border-black'
                        : 'border-transparent'
                    }`}
                    style={{ backgroundColor: color }}
                  />
                ))}
                <input
                  type="color"
                  value={colors[type]}
                  onChange={(e) => onColorChange(type, e.target.value)}
                  className="w-8 h-8 rounded-full cursor-pointer"
                />
              </div>
            </div>
          ))}
        </div>
      </div>
    </div>
  );
};

export default ColorPicker;