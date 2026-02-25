const mongoose = require('mongoose');

const anomalySchema = new mongoose.Schema(
    {
        timestamp: {
            type: Number,
            required: true,
            index: true
        },
        deviceId: {
            type: String,
            required: true,
            index: true
        },
        type: {
            type: String,
            enum: [
                'RPM_SPIKE',
                'SPEED_EXCEED',
                'CAN_FREQUENCY',
                'MISSING_MESSAGE',
                'ENGINE_INCONSISTENCY',
                'STATISTICAL_DEVIATION'
            ],
            required: true
        },
        description: {
            type: String,
            required: true
        },
        severity: {
            type: Number,
            enum: [1, 2, 3],  // 1=LOW, 2=MEDIUM, 3=HIGH
            required: true
        },
        canId: String,
        latitude: Number,
        longitude: Number,
        vehicleState: {
            speed: Number,
            rpm: Number,
            throttle: Number,
            gear: Number,
            engineStatus: String
        },
        acknowledged: {
            type: Boolean,
            default: false
        },
        acknowledgedBy: String,
        acknowledgedAt: Date,
        notes: String
    },
    { timestamps: true }
);

anomalySchema.index({ deviceId: 1, timestamp: -1 });
anomalySchema.index({ severity: 1, acknowledged: 1 });

module.exports = mongoose.model('Anomaly', anomalySchema);
