import React, { useEffect, useRef } from 'react';
import L from 'leaflet';

export const MapView = ({ latitude, longitude, anomalies = [] }) => {
    const mapContainer = useRef(null);
    const map = useRef(null);

    useEffect(() => {
        if (!mapContainer.current) return;

        // Initialize map
        if (!map.current) {
            map.current = L.map(mapContainer.current).setView([latitude || 0, longitude || 0], 13);

            L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
                attribution: '© OpenStreetMap contributors',
                maxZoom: 19
            }).addTo(map.current);
        }

        // Update view
        if (latitude && longitude) {
            map.current.setView([latitude, longitude], 13);

            // Add current location marker
            L.circleMarker([latitude, longitude], {
                radius: 8,
                fillColor: '#3b82f6',
                color: '#fff',
                weight: 2,
                opacity: 1,
                fillOpacity: 0.8
            }).addTo(map.current);
        }

        // Add anomaly markers
        anomalies.forEach(anomaly => {
            if (anomaly.latitude && anomaly.longitude) {
                const color = anomaly.severity === 3 ? '#ef4444' : anomaly.severity === 2 ? '#f59e0b' : '#3b82f6';
                L.circleMarker([anomaly.latitude, anomaly.longitude], {
                    radius: 6,
                    fillColor: color,
                    color: '#fff',
                    weight: 2,
                    opacity: 1,
                    fillOpacity: 0.8
                }).bindPopup(`<strong>${anomaly.type}</strong><br/>${anomaly.description}`)
                 .addTo(map.current);
            }
        });

    }, [latitude, longitude, anomalies]);

    return <div ref={mapContainer} className="map-container" />;
};

export const TripMap = ({ trip = [] }) => {
    const mapContainer = useRef(null);
    const map = useRef(null);

    useEffect(() => {
        if (!mapContainer.current || trip.length === 0) return;

        if (!map.current) {
            map.current = L.map(mapContainer.current).setView([0, 0], 13);
            L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
                attribution: '© OpenStreetMap contributors'
            }).addTo(map.current);
        }

        // Draw route
        const coordinates = trip
            .filter(point => point.latitude && point.longitude)
            .map(point => [point.latitude, point.longitude]);

        if (coordinates.length > 0) {
            const polyline = L.polyline(coordinates, { color: '#3b82f6', weight: 3 }).addTo(map.current);
            map.current.fitBounds(polyline.getBounds());

            // Start point
            L.circleMarker(coordinates[0], {
                radius: 8,
                fillColor: '#10b981',
                color: '#fff',
                weight: 2
            }).addTo(map.current);

            // End point
            L.circleMarker(coordinates[coordinates.length - 1], {
                radius: 8,
                fillColor: '#ef4444',
                color: '#fff',
                weight: 2
            }).addTo(map.current);
        }

    }, [trip]);

    return <div ref={mapContainer} className="map-container" />;
};
