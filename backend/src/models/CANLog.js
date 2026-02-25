const mongoose = require('mongoose');

const canLogSchema = new mongoose.Schema(
    {
        timestamp: {
            type: Number,
            required: true,
            index: true
        },
        canId: {
            type: String,
            required: true,
            index: true
        },
        dlc: {
            type: Number,
            required: true,
            min: 0,
            max: 8
        },
        data: {
            type: [Number],
            required: true,
            validate: {
                validator: function(v) {
                    return v.length <= 8 && v.every(d => d >= 0 && d <= 255);
                },
                message: 'Data must contain up to 8 bytes (0-255)'
            }
        },
        deviceId: {
            type: String,
            required: true,
            index: true
        }
    },
    { timestamps: true }
);

// Compound index for efficient queries
canLogSchema.index({ deviceId: 1, timestamp: -1 });

module.exports = mongoose.model('CANLog', canLogSchema);
