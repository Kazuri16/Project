const Anomaly = require('../models/Anomaly');
const logger = require('../utils/logger');

class AnomalyService {
    static async logAnomaly(data) {
        try {
            const anomaly = new Anomaly({
                timestamp: data.timestamp,
                deviceId: data.deviceId,
                type: data.type,
                description: data.description,
                severity: data.severity,
                canId: data.canId,
                latitude: data.latitude,
                longitude: data.longitude,
                vehicleState: data.vehicleState
            });

            await anomaly.save();
            logger.warn(`Anomaly logged: Type=${data.type}, Severity=${data.severity}`);

            // Trigger notifications for high severity anomalies
            if (data.severity === 3) {
                this.sendNotification(anomaly);
            }

            return anomaly;
        } catch (error) {
            logger.error('Error logging anomaly:', error);
            throw error;
        }
    }

    static async getLatestAnomalies(deviceId, limit = 50) {
        try {
            return await Anomaly.find({ deviceId })
                .sort({ timestamp: -1 })
                .limit(limit)
                .lean();
        } catch (error) {
            logger.error('Error fetching anomalies:', error);
            throw error;
        }
    }

    static async getUnacknowledgedAnomalies(deviceId) {
        try {
            return await Anomaly.find({
                deviceId,
                acknowledged: false
            }).sort({ severity: -1, timestamp: -1 }).lean();
        } catch (error) {
            logger.error('Error fetching unacknowledged anomalies:', error);
            throw error;
        }
    }

    static async acknowledgeAnomaly(anomalyId, userId, notes = '') {
        try {
            const anomaly = await Anomaly.findByIdAndUpdate(
                anomalyId,
                {
                    acknowledged: true,
                    acknowledgedBy: userId,
                    acknowledgedAt: new Date(),
                    notes: notes
                },
                { new: true }
            );

            logger.info(`Anomaly ${anomalyId} acknowledged by ${userId}`);
            return anomaly;
        } catch (error) {
            logger.error('Error acknowledging anomaly:', error);
            throw error;
        }
    }

    static async getAnomalyStatistics(deviceId, timeRange) {
        try {
            const startTime = Date.now() - timeRange;

            return await Anomaly.aggregate([
                {
                    $match: {
                        deviceId: deviceId,
                        timestamp: { $gte: startTime }
                    }
                },
                {
                    $group: {
                        _id: '$type',
                        count: { $sum: 1 },
                        avgSeverity: { $avg: '$severity' }
                    }
                }
            ]);
        } catch (error) {
            logger.error('Error getting anomaly statistics:', error);
            throw error;
        }
    }

    static sendNotification(anomaly) {
        // TODO: Implement email/SMS/push notification
        logger.info(`Send notification for anomaly: ${anomaly.description}`);
    }
}

module.exports = AnomalyService;
