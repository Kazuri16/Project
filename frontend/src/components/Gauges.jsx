import React from 'react';

export const SpeedGauge = ({ speed = 0, maxSpeed = 200 }) => {
    const percentage = (speed / maxSpeed) * 100;
    const color = percentage > 80 ? '#ef4444' : percentage > 60 ? '#f59e0b' : '#10b981';

    return (
        <div className="flex flex-col items-center">
            <div className="relative w-48 h-48 rounded-full border-8 border-gray-300">
                <div
                    className="absolute top-0 left-0 w-full h-full rounded-full flex items-center justify-center"
                    style={{
                        background: `conic-gradient(${color} 0deg ${percentage * 3.6}deg, #e5e7eb ${percentage * 3.6}deg 360deg)`,
                        fontSize: '32px',
                        fontWeight: 'bold'
                    }}
                >
                    <div className="w-40 h-40 rounded-full bg-white flex items-center justify-center flex-col">
                        <div style={{ color }} className="text-3xl font-bold">
                            {Math.round(speed)}
                        </div>
                        <div className="text-gray-500 text-sm">km/h</div>
                    </div>
                </div>
            </div>
        </div>
    );
};

export const RPMGauge = ({ rpm = 0, maxRpm = 7000 }) => {
    const percentage = (rpm / maxRpm) * 100;
    const color = percentage > 80 ? '#ef4444' : percentage > 60 ? '#f59e0b' : '#3b82f6';

    return (
        <div className="flex flex-col items-center">
            <div className="relative w-40 h-40 rounded-full border-8 border-gray-300">
                <div
                    className="absolute top-0 left-0 w-full h-full rounded-full flex items-center justify-center"
                    style={{
                        background: `conic-gradient(${color} 0deg ${percentage * 3.6}deg, #e5e7eb ${percentage * 3.6}deg 360deg)`,
                        fontSize: '24px',
                        fontWeight: 'bold'
                    }}
                >
                    <div className="w-36 h-36 rounded-full bg-white flex items-center justify-center flex-col">
                        <div style={{ color }} className="text-2xl font-bold">
                            {Math.round(rpm / 100)}
                        </div>
                        <div className="text-gray-500 text-xs">x100 RPM</div>
                    </div>
                </div>
            </div>
        </div>
    );
};

export const GearIndicator = ({ gear = 'P', engineStatus = 'OFF' }) => {
    return (
        <div className="flex flex-col items-center gap-4">
            <div className="bg-blue-600 text-white rounded-lg p-4 text-center">
                <div className="text-5xl font-bold">{gear}</div>
                <div className="text-sm mt-2">Gear</div>
            </div>
            <div className={`px-4 py-2 rounded-lg font-semibold ${
                engineStatus === 'ON' ? 'bg-green-500 text-white' : 'bg-gray-400 text-gray-800'
            }`}>
                Engine: {engineStatus}
            </div>
        </div>
    );
};

export const ThrottleBar = ({ throttle = 0 }) => {
    return (
        <div className="flex flex-col gap-2">
            <label className="text-sm font-semibold">Throttle: {throttle}%</label>
            <div className="w-full bg-gray-300 rounded-full h-6 overflow-hidden">
                <div
                    className="bg-gradient-to-r from-blue-500 to-green-500 h-full transition-all duration-300"
                    style={{ width: `${throttle}%` }}
                />
            </div>
        </div>
    );
};
