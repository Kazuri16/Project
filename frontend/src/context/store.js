import create from 'zustand';

export const useAuthStore = create((set) => ({
    user: null,
    token: null,
    isAuthenticated: false,

    login: (user, token) => set({ user, token, isAuthenticated: true }),
    logout: () => set({ user: null, token: null, isAuthenticated: false }),
    setUser: (user) => set({ user })
}));

export const useVehicleStore = create((set) => ({
    currentState: null,
    stateHistory: [],
    location: null,

    setCurrentState: (state) => set({ currentState: state }),
    addToHistory: (state) => set((s) => ({
        stateHistory: [...s.stateHistory.slice(-99), state]
    })),
    setLocation: (location) => set({ location })
}));

export const useAnomalyStore = create((set) => ({
    anomalies: [],
    unacknowledged: [],

    addAnomaly: (anomaly) => set((s) => ({
        anomalies: [anomaly, ...s.anomalies.slice(0, 49)],
        unacknowledged: anomaly.acknowledged ? s.unacknowledged : [anomaly, ...s.unacknowledged]
    })),
    setAnomalies: (anomalies) => set({ anomalies }),
    acknowledgeAnomaly: (anomalyId) => set((s) => ({
        unacknowledged: s.unacknowledged.filter(a => a._id !== anomalyId)
    }))
}));
