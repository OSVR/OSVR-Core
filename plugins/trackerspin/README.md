# OSVR-Tracker-Spin Plugin

This plugin is adapter from VRPN Tracker Spin, which is a virtual plugin for a tracker that stays around the origin and spin around the specified axis at the specified rate of rotation, reporting orientation at specified rate.
It can be used to test the smoothness of rendering for VR system. 

You will need to provide parameters in osvr_server config file:

1. `report_rate` (default 200.0) - Rate at which reports will be sent,
2. `x_of_axis_to_spin_around` (default 0.0) - How much to spin around X axis
3. `y_of_axis_to_spin_around`: (default 1.0) - How much to spin around Y axis
3. `z_of_axis_to_spin_around`: (default 0.0) - How much to spin around Z axis
4. `rotation_rate_around_axis_in_Hz`: (default 0.1) - How fast to spin the tracker 
5. `name`: `TrackerSpin` (optional)