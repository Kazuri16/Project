import React from 'react';
import { Line, Bar } from 'react-chartjs-2';
import {
    Chart as ChartJS,
    CategoryScale,
    LinearScale,
    PointElement,
    LineElement,
    BarElement,
    Title,
    Tooltip,
    Legend
} from 'chart.js';

ChartJS.register(
    CategoryScale,
    LinearScale,
    PointElement,
    LineElement,
    BarElement,
    Title,
    Tooltip,
    Legend
);

export const SpeedChart = ({ data = [] }) => {
    const chartData = {
        labels: data.map((_, i) => i),
        datasets: [{
            label: 'Speed (km/h)',
            data: data.map(d => d.speed || 0),
            borderColor: '#3b82f6',
            backgroundColor: 'rgba(59, 130, 246, 0.1)',
            tension: 0.4,
            borderWidth: 2,
            pointRadius: 0
        }]
    };

    return (
        <div className="chart-container">
            <Line
                data={chartData}
                options={{
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: { legend: { position: 'top' } }
                }}
            />
        </div>
    );
};

export const RPMChart = ({ data = [] }) => {
    const chartData = {
        labels: data.map((_, i) => i),
        datasets: [{
            label: 'RPM',
            data: data.map(d => d.rpm || 0),
            borderColor: '#ef4444',
            backgroundColor: 'rgba(239, 68, 68, 0.1)',
            tension: 0.4,
            borderWidth: 2,
            pointRadius: 0
        }]
    };

    return (
        <div className="chart-container">
            <Line
                data={chartData}
                options={{
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: { legend: { position: 'top' } }
                }}
            />
        </div>
    );
};

export const AnomalyDistribution = ({ statistics = [] }) => {
    const chartData = {
        labels: statistics.map(s => s._id || 'Unknown'),
        datasets: [{
            label: 'Anomaly Count',
            data: statistics.map(s => s.count),
            backgroundColor: [
                '#ef4444',
                '#f59e0b',
                '#3b82f6',
                '#10b981',
                '#8b5cf6',
                '#ec4899'
            ]
        }]
    };

    return (
        <div className="chart-container">
            <Bar
                data={chartData}
                options={{
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: { legend: { position: 'top' } }
                }}
            />
        </div>
    );
};
