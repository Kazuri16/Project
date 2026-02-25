import React, { useState, useEffect } from 'react';
import { SpeedGauge, RPMGauge, GearIndicator, ThrottleBar } from '../components/Gauges';
import { AnomalyTimeline } from '../components/AnomalyComponents';
import { MapView } from '../components/MapComponent';
import { SpeedChart, RPMChart } from '../components/Charts';
import { useVehicleStore, useAnomalyStore, useAuthStore } from '../context/store';
import { canAPI, anomalyAPI } from '../services/api';
import { useRouter } from 'next/router';

export default function Dashboard() {
    const router = useRouter();
    const { query } = router;
    const isAuthenticated = useAuthStore(state => state.isAuthenticated);
    const currentState = useVehicleStore(state => state.currentState);
    const stateHistory = useVehicleStore(state => state.stateHistory);
    const anomalies = useAnomalyStore(state => state.anomalies);
    const [selectedDevice, setSelectedDevice] = useState('');
    const [loading, setLoading] = useState(true);

    useEffect(() => {
        if (!isAuthenticated && !localStorage.getItem('token')) {
            router.push('/login');
        }
    }, [isAuthenticated, router]);

    useEffect(() => {
        if (query.deviceId) {
            setSelectedDevice(query.deviceId);
            fetchData(query.deviceId);
        }
    }, [query.deviceId]);

    const fetchData = async (deviceId) => {
        setLoading(true);
        try {
            const [canData, anomalyData] = await Promise.all([
                canAPI.getLatest(deviceId, 100),
                anomalyAPI.getLatest(deviceId, 50)
            ]);

            // Update stores (simplified)
            // In production, use proper state management
            setLoading(false);
        } catch (error) {
            console.error('Error fetching data:', error);
            setLoading(false);
        }
    };

    if (loading) {
        return (
            <div className="flex items-center justify-center min-h-screen">
                <div className="text-xl text-gray-600">Loading...</div>
            </div>
        );
    }

    return (
        <div className="min-h-screen bg-gray-50">
            {/* Header */}
            <header className="bg-white shadow">
                <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-4">
                    <h1 className="text-3xl font-bold text-gray-900">CAN Logger Dashboard</h1>
                    <p className="text-sm text-gray-500">Device: {selectedDevice || 'None Selected'}</p>
                </div>
            </header>

            <main className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-8">
                {/* Live Status Section */}
                <section className="mb-8">
                    <h2 className="text-2xl font-bold mb-4">Live Vehicle Status</h2>
                    <div className="grid grid-cols-1 md:grid-cols-4 gap-4">
                        <div className="bg-white p-6 rounded-lg shadow">
                            <SpeedGauge speed={currentState?.speed || 0} />
                        </div>
                        <div className="bg-white p-6 rounded-lg shadow">
                            <RPMGauge rpm={currentState?.rpm || 0} />
                        </div>
                        <div className="bg-white p-6 rounded-lg shadow">
                            <GearIndicator gear={currentState?.gear || 'P'} engineStatus={currentState?.engineStatus || 'OFF'} />
                        </div>
                        <div className="bg-white p-6 rounded-lg shadow">
                            <ThrottleBar throttle={currentState?.throttle || 0} />
                        </div>
                    </div>
                </section>

                {/* Charts Section */}
                <section className="mb-8">
                    <h2 className="text-2xl font-bold mb-4">Historical Data</h2>
                    <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
                        <div className="bg-white p-6 rounded-lg shadow">
                            <h3 className="text-lg font-semibold mb-4">Speed Trend</h3>
                            <SpeedChart data={stateHistory} />
                        </div>
                        <div className="bg-white p-6 rounded-lg shadow">
                            <h3 className="text-lg font-semibold mb-4">RPM Trend</h3>
                            <RPMChart data={stateHistory} />
                        </div>
                    </div>
                </section>

                {/* Map Section */}
                <section className="mb-8">
                    <h2 className="text-2xl font-bold mb-4">GPS Location & Anomalies</h2>
                    <div className="bg-white p-6 rounded-lg shadow">
                        <MapView
                            latitude={currentState?.latitude}
                            longitude={currentState?.longitude}
                            anomalies={anomalies}
                        />
                    </div>
                </section>

                {/* Anomalies Section */}
                <section>
                    <h2 className="text-2xl font-bold mb-4">Recent Anomalies</h2>
                    <div className="bg-white p-6 rounded-lg shadow">
                        <AnomalyTimeline anomalies={anomalies} />
                    </div>
                </section>
            </main>
        </div>
    );
}
