const mongoose = require('mongoose');

const vehicleStateSchema = new mongoose.Schema(
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
        speed: {
            type: Number,
            required: true,
            min: 0
        },
        rpm: {
            type: Number,
            required: true,
            min: 0
        },
        throttle: {
            type: Number,
            required: true,
            min: 0,
            max: 100
        },
        gear: {
            type: Number,
            enum: [0, 1, 2, 3, 4, 5, 6, 7]  // 0=Park, 1-6=Gears
        },
        engineStatus: {
            type: String,
            enum: ['ON', 'OFF'],
            required: true
        },
        fault: {
            type: Boolean,
            default: false
        },
        latitude: Number,
        longitude: Number
    },
    { timestamps: true }
);

vehicleStateSchema.index({ deviceId: 1, timestamp: -1 });

module.exports = mongoose.model('VehicleState', vehicleStateSchema);
