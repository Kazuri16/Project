import React from 'react';

export const AnomalyCard = ({ anomaly }) => {
    const severityColor = {
        1: 'border-blue-500 bg-blue-50',
        2: 'border-yellow-500 bg-yellow-50',
        3: 'border-red-500 bg-red-50'
    };

    const typeLabel = {
        'RPM_SPIKE': '⚡ RPM Spike',
        'SPEED_EXCEED': '🚗 Speed Exceeded',
        'CAN_FREQUENCY': '📡 CAN Frequency',
        'MISSING_MESSAGE': '❌ Missing Message',
        'ENGINE_INCONSISTENCY': '⚠️ Engine Error',
        'STATISTICAL_DEVIATION': '📊 Statistical Anomaly'
    };

    const formatTime = (timestamp) => {
        return new Date(timestamp).toLocaleTimeString();
    };

    return (
        <div className={`p-4 rounded-lg border-l-4 ${severityColor[anomaly.severity]}`}>
            <div className="flex justify-between items-start">
                <div>
                    <h3 className="font-semibold text-lg">{typeLabel[anomaly.type] || anomaly.type}</h3>
                    <p className="text-sm text-gray-600 mt-1">{anomaly.description}</p>
                    <p className="text-xs text-gray-500 mt-2">
                        CAN ID: 0x{anomaly.canId?.toString(16).toUpperCase().padStart(3, '0')}
                    </p>
                </div>
                <div className="text-right">
                    <p className="text-xs text-gray-500">{formatTime(anomaly.timestamp)}</p>
                    {anomaly.acknowledged && (
                        <p className="text-xs text-green-600 mt-1">✓ Acknowledged</p>
                    )}
                </div>
            </div>
        </div>
    );
};

export const AnomalyTimeline = ({ anomalies = [] }) => {
    return (
        <div className="space-y-3">
            {anomalies.length === 0 ? (
                <p className="text-gray-500 text-center py-8">No anomalies detected</p>
            ) : (
                anomalies.map((anomaly, idx) => (
                    <AnomalyCard key={anomaly._id || idx} anomaly={anomaly} />
                ))
            )}
        </div>
    );
};

export const AnomalyStats = ({ statistics = [] }) => {
    return (
        <div className="grid grid-cols-2 md:grid-cols-3 gap-4">
            {statistics.map((stat, idx) => (
                <div key={idx} className="bg-white p-4 rounded-lg border border-gray-200">
                    <p className="text-sm text-gray-600">{stat._id || 'Unknown'}</p>
                    <p className="text-2xl font-bold text-blue-600">{stat.count}</p>
                    <p className="text-xs text-gray-500 mt-1">
                        Avg Severity: {stat.avgSeverity?.toFixed(1)}
                    </p>
                </div>
            ))}
        </div>
    );
};
