// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <array>
#include <algorithm>
#include <cmath>
#include <cstdint>

/**
 * @brief Smoothing filter for power meter values combining Median and PT1 (first-order low-pass) filtering
 * 
 * This filter:
 * 1. Stores the last 3 power meter readings in a buffer
 * 2. Optionally applies a median filter to remove outliers
 * 3. Optionally applies a PT1 (first-order lag) filter for smooth transitions
 * 4. Returns smoothed power value
 * 
 * The median filter is excellent against outliers and spikes.
 * The PT1 filter provides smooth exponential smoothing.
 */
class PowerMeterFilterClass {
public:
    /**
     * @brief Constructor with filter configuration
     * @param enableMedianFilter Enable median filtering
     * @param enablePt1Filter Enable PT1 filtering
     * @param pt1Factor PT1 smoothing factor (0.05 to 0.50)
     */
    explicit PowerMeterFilterClass(bool enableMedianFilter = true, bool enablePt1Filter = true, float pt1Factor = 0.10f);

    /**
     * @brief Update filter with new power meter value
     * @param newValue Raw power meter reading in Watts
     * @param isValueValid Indicates if the meter value is valid
     * @return Smoothed power value in Watts
     */
    float update(float newValue, bool isValueValid);

    /**
     * @brief Get the current smoothed power value without updating
     * @return Last calculated smoothed value
     */
    float getSmoothedValue() const { return _smoothedValue; }

    /**
     * @brief Get the median of the last 3 values
     * @return Median value
     */
    float getMedianValue() const;

    /**
     * @brief Get the last raw value
     * @return Last raw value before filtering
     */
    float getLastRawValue() const { return _valueBuffer[(_bufferIndex + 2) % 3]; }

    /**
     * @brief Reset filter to initial state
     */
    void reset();

    /**
     * @brief Auto-reinitialize filter if meter values become invalid
     * @param isValueValid Current validity state of meter value
     */
    void autoReinitializeOnInvalidMeter(bool isValueValid);

    /**
     * @brief Set filter configuration
     * @param enableMedianFilter Enable median filtering
     * @param enablePt1Filter Enable PT1 filtering
     */
    void setFilterConfiguration(bool enableMedianFilter, bool enablePt1Filter);

    /**
     * @brief Set PT1 smoothing factor
     * @param pt1Factor Factor from 0.05 to 0.50 (higher = faster response, less smoothing)
     */
    void setPt1Factor(float pt1Factor);

    /**
     * @brief Get PT1 smoothing factor
     * @return Current PT1 factor
     */
    float getPt1Factor() const { return _pt1Factor; }

    /**
     * @brief Get median filter enabled state
     */
    bool isMedianFilterEnabled() const { return _enableMedianFilter; }

    /**
     * @brief Get PT1 filter enabled state
     */
    bool isPt1FilterEnabled() const { return _enablePt1Filter; }

    /**
     * @brief Get filter buffer fill level
     * @return Number of values in buffer (0-3)
     */
    uint8_t getBufferFillLevel() const { return _bufferCount; }

private:
    /**
     * @brief Calculate median of 3 values
     */
    static float calculateMedian(float a, float b, float c);

    /**
     * @brief Apply PT1 (first-order lag) filter
     */
    float applyPt1Filter(float inputValue);

    // Buffer for last 3 measurements (for median filter)
    std::array<float, 3> _valueBuffer = {0.0f, 0.0f, 0.0f};
    
    // Index for circular buffer
    uint8_t _bufferIndex = 0;
    
    // Number of values stored in buffer (max 3)
    uint8_t _bufferCount = 0;
    
    // PT1 filter state (smoothed value)
    float _smoothedValue = 0.0f;
    
    // PT1 filter factor (0.05 to 0.50)
    float _pt1Factor = 0.10f;
    
    // Filter enabled flags
    bool _enableMedianFilter = true;
    bool _enablePt1Filter = true;
    
    // Last validity state (for auto-reinitialization)
    bool _lastValidityState = true;
    
    // Flag to indicate if filter has been initialized
    bool _initialized = false;
};

extern PowerMeterFilterClass PowerMeterFilter;
