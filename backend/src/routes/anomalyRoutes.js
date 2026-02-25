const express = require('express');
const AnomalyService = require('../services/anomalyService');
const { authenticate } = require('../middleware/auth');
const { validateRequest, schemas } = require('../middleware/validation');

const router = express.Router();

// Log anomaly from device
router.post(
    '/',
    validateRequest(schemas.logAnomaly),
    async (req, res, next) => {
        try {
            const anomaly = await AnomalyService.logAnomaly(req.validatedData);
            res.status(201).json(anomaly);
        } catch (error) {
            next(error);
        }
    }
);

// Get latest anomalies for device
router.get(
    '/:deviceId/latest',
    authenticate,
    async (req, res, next) => {
        try {
            const limit = parseInt(req.query.limit) || 50;
            const anomalies = await AnomalyService.getLatestAnomalies(req.params.deviceId, limit);
            res.json(anomalies);
        } catch (error) {
            next(error);
        }
    }
);

// Get unacknowledged anomalies
router.get(
    '/:deviceId/unacknowledged',
    authenticate,
    async (req, res, next) => {
        try {
            const anomalies = await AnomalyService.getUnacknowledgedAnomalies(req.params.deviceId);
            res.json(anomalies);
        } catch (error) {
            next(error);
        }
    }
);

// Acknowledge anomaly
router.patch(
    '/:anomalyId/acknowledge',
    authenticate,
    async (req, res, next) => {
        try {
            const { notes } = req.body;
            const anomaly = await AnomalyService.acknowledgeAnomaly(
                req.params.anomalyId,
                req.userId,
                notes
            );
            res.json(anomaly);
        } catch (error) {
            next(error);
        }
    }
);

// Get anomaly statistics
router.get(
    '/:deviceId/statistics',
    authenticate,
    async (req, res, next) => {
        try {
            const timeRange = parseInt(req.query.timeRange) || 86400000;  // 24h default
            const stats = await AnomalyService.getAnomalyStatistics(req.params.deviceId, timeRange);
            res.json(stats);
        } catch (error) {
            next(error);
        }
    }
);

module.exports = router;
