# Unified Video-Based Tracker

## Open Issues/Wishlist/Desired or Planned Features

- General: See `Assumptions.h` and `@todo` in code
- Configuration: be able to instantiate an arbitrary set of bodies/targets.
- Be able to operate in IMU-only mode (as VideoIMUFusion did), essentially just passing through the IMU data.
- Figure out why room calibration sometimes (seemingly randomly) is a rather prolonged struggle.
- Update IMU code to have IMU hold a yaw drift state variable that is autocalibrated (like the beacon positions are)
- Port the pre-calibration tool to this tracker along with a new JSON file format for calibrated beacons.
- Image processing threading: Don't spawn a new thread every frame, re-use the same thread (or use a higher-level parallelism primitive) to do the "time consuming image proc".
- State reporting: Use a single mutex, and possibly double-buffering, for all bodies - one mutex per body is too granular and visibly impacts performance at around 5 bodies.
- Slide-joint target (the rear target of the HDK) - modeling a target with one linear (or one linear and one rotational) degree of freedom from the body.
- Multi-camera tracking.
- Report status of each body in an analog.
- Modeling: IMU and "neck model", etc - IMU is not co-located with the origin of the body's coordinate system - how to deal? (Transform the state/error before and then transform it back?)