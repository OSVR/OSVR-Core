# Unified Video-Based Tracker

## Open Issues/Wishlist/Desired or Planned Features

- General: See `@todo` in code

These are split up into sections, since the tracker code is split up: there's the core tracker engine which is very generic and flexible, then the plugin which, in places (actually, one specific file - `MakeHDKTrackingSystem.h`), is effectively hard-coded to just set up the core to track an HDK right now. Naturally, removing the artificial limitations on the core's power is easier than working on the tracker core itself, though a number of the items on the tracker core are fairly simple: the "scary math" phase is effectively complete and hasn't really needed to be touched substantially in some time.

### Plugin Interface (`org_osvr_unifiedvideoinertial`)

Items here are just in the plugin interface to the tracker core - there is far more functionality in the tracker core (and even in the inner part of the plugin interface - the `TrackerThread` object, etc can handle multiple tracked objects, multiple IMUs, etc) than is exposed by the plugin interface right now.


- Configuration: be able to instantiate an arbitrary set of bodies/targets.
- Report status of each body in an analog.


### Other Interface Items
Like the plugin interface items above, these don't require changes to the tracker core.

- Port the pre-calibration tool to this tracker tracker core, along with a new JSON file format for calibrated beacons.

### Video Tracker Shared (blob detection, etc)

- Option for "max area" as a blob filter parameter: the new "edge hole extractor" is much better at not recognizing random splotches of sunlight as blobs, but a max area filter would be cheap and easy and save computation down the line by reducing `n`...

### Tracker Core (`uvbi-core`)

In general, the things that sound like large design changes are actually stubbed in or theoretically functional, but not tested/used in the current limited plugin interface that just sets up the core to track the HDK.

- General: See `Assumptions.h` and `@todo` in code
- Be able to operate in IMU-only mode (as VideoIMUFusion did), essentially just passing through the IMU data.
- Multiple optical targets per body.
- Figure out why room calibration sometimes (seemingly randomly) is a rather prolonged struggle. (Seems to be better since changing to use more RANSAC iterations, converting the OpenCV poses to Eigen poses differently, and thus doing the pinhole flip differently, but it's again, seemingly randomly...)
- Slide-joint target (the rear target of the HDK) - modeling a target with one linear (or one linear and one rotational) degree of freedom from the body.
- Update IMU code to have IMU hold a yaw drift state variable that is autocalibrated (like the beacon positions are)
- Multi-camera tracking.
- Modeling: IMU and "neck model", etc - IMU is not co-located with the origin of the body's coordinate system - how to deal? (Transform the state/error before and then transform it back?)
- Be able to allocate sets of patterns to devices for third-party devices to use.
  - goal is to avoid having to have fixed allocations of the limited pattern space: just let the plugin at runtime hand out patterns as long as you give it constraints. Important constraint that was missed earlier: adjacency - don't want two adjacent beacons bright at the same time or you get the effect seen on the left side of the HDK 1.3.
