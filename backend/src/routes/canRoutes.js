const express = require('express');
const CANLogService = require('../services/canLogService');
const { authenticate } = require('../middleware/auth');
const { validateRequest, schemas } = require('../middleware/validation');

const router = express.Router();

// Log CAN frame from device
router.post(
    '/',
    validateRequest(schemas.logCANData),
    async (req, res, next) => {
        try {
            const log = await CANLogService.logCANFrame(req.validatedData);
            res.status(201).json(log);
        } catch (error) {
            next(error);
        }
    }
);

// Get latest CAN frames for device
router.get(
    '/:deviceId/latest',
    authenticate,
    async (req, res, next) => {
        try {
            const limit = parseInt(req.query.limit) || 100;
            const logs = await CANLogService.getLatestFrames(req.params.deviceId, limit);
            res.json(logs);
        } catch (error) {
            next(error);
        }
    }
);

// Get CAN frames by time range
router.get(
    '/:deviceId/range',
    authenticate,
    async (req, res, next) => {
        try {
            const { startTime, endTime } = req.query;
            if (!startTime || !endTime) {
                return res.status(400).json({ error: 'startTime and endTime required' });
            }

            const logs = await CANLogService.getFramesByTimeRange(
                req.params.deviceId,
                parseInt(startTime),
                parseInt(endTime)
            );
            res.json(logs);
        } catch (error) {
            next(error);
        }
    }
);

// Get frames by CAN ID
router.get(
    '/:deviceId/canId/:canId',
    authenticate,
    async (req, res, next) => {
        try {
            const limit = parseInt(req.query.limit) || 50;
            const logs = await CANLogService.getFramesByCanId(
                req.params.deviceId,
                req.params.canId,
                limit
            );
            res.json(logs);
        } catch (error) {
            next(error);
        }
    }
);

module.exports = router;
