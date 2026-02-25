const CANLog = require('../models/CANLog');
const logger = require('../utils/logger');

class CANLogService {
    static async logCANFrame(data) {
        try {
            const log = new CANLog({
                timestamp: data.timestamp,
                canId: data.canId,
                dlc: data.dlc,
                data: data.data,
                deviceId: data.deviceId
            });

            await log.save();
            logger.info(`CAN frame logged: ID=${data.canId}`);
            return log;
        } catch (error) {
            logger.error('Error logging CAN frame:', error);
            throw error;
        }
    }

    static async getLatestFrames(deviceId, limit = 100) {
        try {
            return await CANLog.find({ deviceId })
                .sort({ timestamp: -1 })
                .limit(limit)
                .lean();
        } catch (error) {
            logger.error('Error fetching CAN frames:', error);
            throw error;
        }
    }

    static async getFramesByTimeRange(deviceId, startTime, endTime) {
        try {
            return await CANLog.find({
                deviceId,
                timestamp: { $gte: startTime, $lte: endTime }
            }).sort({ timestamp: -1 }).lean();
        } catch (error) {
            logger.error('Error fetching frames by time range:', error);
            throw error;
        }
    }

    static async getFramesByCanId(deviceId, canId, limit = 50) {
        try {
            return await CANLog.find({ deviceId, canId })
                .sort({ timestamp: -1 })
                .limit(limit)
                .lean();
        } catch (error) {
            logger.error('Error fetching frames by CAN ID:', error);
            throw error;
        }
    }

    static async deleteOldLogs(deviceId, olderThanMs) {
        try {
            const result = await CANLog.deleteMany({
                deviceId,
                timestamp: { $lt: Date.now() - olderThanMs }
            });
            logger.info(`Deleted ${result.deletedCount} old CAN logs`);
            return result;
        } catch (error) {
            logger.error('Error deleting old logs:', error);
            throw error;
        }
    }
}

module.exports = CANLogService;
