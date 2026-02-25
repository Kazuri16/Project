const VehicleState = require('../models/VehicleState');
const logger = require('../utils/logger');

class VehicleStateService {
    static async recordState(data) {
        try {
            const state = new VehicleState({
                timestamp: data.timestamp,
                deviceId: data.deviceId,
                speed: data.speed,
                rpm: data.rpm,
                throttle: data.throttle,
                gear: data.gear,
                engineStatus: data.engineStatus,
                fault: data.fault,
                latitude: data.latitude,
                longitude: data.longitude
            });

            await state.save();
            return state;
        } catch (error) {
            logger.error('Error recording vehicle state:', error);
            throw error;
        }
    }

    static async getLatestState(deviceId) {
        try {
            return await VehicleState.findOne({ deviceId })
                .sort({ timestamp: -1 })
                .lean();
        } catch (error) {
            logger.error('Error fetching latest state:', error);
            throw error;
        }
    }

    static async getStateHistory(deviceId, limit = 100) {
        try {
            return await VehicleState.find({ deviceId })
                .sort({ timestamp: -1 })
                .limit(limit)
                .lean();
        } catch (error) {
            logger.error('Error fetching state history:', error);
            throw error;
        }
    }

    static async getTrip(deviceId, startTime, endTime) {
        try {
            return await VehicleState.find({
                deviceId,
                timestamp: { $gte: startTime, $lte: endTime }
            }).sort({ timestamp: 1 }).lean();
        } catch (error) {
            logger.error('Error fetching trip:', error);
            throw error;
        }
    }

    static async getTripStatistics(deviceId, startTime, endTime) {
        try {
            const states = await this.getTrip(deviceId, startTime, endTime);

            if (states.length === 0) return null;

            const speeds = states.map(s => s.speed);
            const rpms = states.map(s => s.rpm);

            const maxSpeed = Math.max(...speeds);
            const avgSpeed = speeds.reduce((a, b) => a + b) / speeds.length;
            const maxRpm = Math.max(...rpms);
            const avgRpm = rpms.reduce((a, b) => a + b) / rpms.length;

            return {
                duration: endTime - startTime,
                distance: this.calculateDistance(states),
                maxSpeed,
                avgSpeed,
                maxRpm,
                avgRpm,
                faultCount: states.filter(s => s.fault).length
            };
        } catch (error) {
            logger.error('Error calculating trip statistics:', error);
            throw error;
        }
    }

    static calculateDistance(states) {
        // Simplified distance calculation (Haversine formula)
        let distance = 0;
        for (let i = 1; i < states.length; i++) {
            if (states[i].latitude && states[i-1].latitude) {
                distance += this.haversineDistance(
                    states[i-1].latitude, states[i-1].longitude,
                    states[i].latitude, states[i].longitude
                );
            }
        }
        return distance;
    }

    static haversineDistance(lat1, lon1, lat2, lon2) {
        const R = 6371;  // Earth's radius in km
        const dLat = (lat2 - lat1) * Math.PI / 180;
        const dLon = (lon2 - lon1) * Math.PI / 180;
        const a = Math.sin(dLat/2) * Math.sin(dLat/2) +
                  Math.cos(lat1 * Math.PI / 180) * Math.cos(lat2 * Math.PI / 180) *
                  Math.sin(dLon/2) * Math.sin(dLon/2);
        const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));
        return R * c;
    }
}

module.exports = VehicleStateService;
