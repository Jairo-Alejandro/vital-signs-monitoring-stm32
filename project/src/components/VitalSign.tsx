import React from 'react';
import { ArrowDown, ArrowUp, Minus } from 'lucide-react';

interface VitalSignProps {
  title: string;
  value: number;
  unit: string;
  trend: 'up' | 'down' | 'stable';
  color: string;
  change?: string;
  isDark: boolean;
}

const VitalSign: React.FC<VitalSignProps> = ({
  title,
  value,
  unit,
  trend,
  color,
  change,
  isDark
}) => {
  return (
    <div className="p-4 border-b border-gray-700">
      <div className={`text-sm ${isDark ? 'text-gray-400' : 'text-gray-500'} mb-2`}>
        {title}
      </div>
      <div className="flex items-end gap-2">
        <span className={`text-3xl font-bold ${color}`}>{value}</span>
        <span className={`${isDark ? 'text-gray-400' : 'text-gray-500'} text-sm mb-1`}>
          {unit}
        </span>
      </div>
      <div className="flex items-center mt-2 gap-1">
        {trend === 'up' && <ArrowUp className="w-4 h-4 text-red-500" />}
        {trend === 'down' && <ArrowDown className="w-4 h-4 text-green-500" />}
        {trend === 'stable' && <Minus className="w-4 h-4 text-blue-500" />}
        {change && (
          <span className={`text-sm ${
            trend === 'up' ? 'text-red-500' : 
            trend === 'down' ? 'text-green-500' : 
            'text-blue-500'
          }`}>
            {change}
          </span>
        )}
      </div>
    </div>
  );
};

export default VitalSign;