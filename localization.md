# Localization Function Planning

## Plan
Time Difference of Arrival (TDOA) Algorithm based on Chan3D + Kalman Filter
* Chan+EKF3D-TDOA Stages:
    * Chan3D algorithm in initial alignment stage
    * Position based on EKF (Extended Kalman Filter) in localization loop
        * _Note: UKF (unscented kalman filter) may be possible but is more computationally intensive_
    * We have di already

## Resources
* Chan+EKF3D-TDOA: https://ieeexplore.ieee.org/document/7287085
* Arduino Kalman Filter (https://www.arduinolibraries.info/libraries/kalman-filter-library)
