import React from 'react';

export default function Index() {
    return (
        <div className="min-h-screen bg-gradient-to-b from-blue-600 to-blue-800">
            <header className="bg-white shadow">
                <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-4 flex justify-between items-center">
                    <h1 className="text-3xl font-bold text-gray-900">CAN Logger</h1>
                    <nav className="space-x-4">
                        <a href="/login" className="text-blue-600 hover:text-blue-800 font-semibold">
                            Sign In
                        </a>
                    </nav>
                </div>
            </header>

            <main className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-20">
                <div className="text-center text-white">
                    <h2 className="text-5xl font-bold mb-6">
                        Intelligent CAN Bus Data Logger
                    </h2>
                    <p className="text-xl mb-8 text-blue-100">
                        Ultra-low power logging for automotive diagnostics with embedded anomaly detection
                    </p>

                    <div className="grid grid-cols-1 md:grid-cols-3 gap-6 mb-12">
                        <div className="bg-blue-500 bg-opacity-30 p-6 rounded-lg backdrop-blur">
                            <h3 className="text-2xl font-bold mb-2">📊 Real-time Monitoring</h3>
                            <p>Live vehicle status, speed, RPM, and engine diagnostics</p>
                        </div>
                        <div className="bg-blue-500 bg-opacity-30 p-6 rounded-lg backdrop-blur">
                            <h3 className="text-2xl font-bold mb-2">🔍 Anomaly Detection</h3>
                            <p>AI-powered anomaly detection with statistical analysis</p>
                        </div>
                        <div className="bg-blue-500 bg-opacity-30 p-6 rounded-lg backdrop-blur">
                            <h3 className="text-2xl font-bold mb-2">📍 GPS Tracking</h3>
                            <p>Real-time location tracking with trip mapping</p>
                        </div>
                    </div>

                    <div className="space-y-4">
                        <a href="/login" className="inline-block bg-white text-blue-600 px-8 py-3 rounded-lg font-semibold hover:bg-blue-50 transition">
                            Get Started
                        </a>
                        <p className="text-sm text-blue-100">
                            Open source automotive diagnostic platform
                        </p>
                    </div>
                </div>
            </main>
        </div>
    );
}
