# Inner Workings of Devices and Connections {#InternalInnerWorkings}

## Server mainloop call activity
- `Connection::process()`
  - `VrpnBasedConnection::m_process()`
    - runs mainloop on `vrpn_Connection`
  - for each device:
    - `ConnectionDevice::process()`
      - `VrpnConnectionDevice::m_process()`
        - `DeviceToken::connectionInteract()`
          - if async: lets a `DeviceToken::sendData()` through to become a `ConnectionDevice::sendData()`
          - if sync: calls update callback which may call `DeviceToken::sendData()` which forwards directly to `ConnectionDevice::sendData()`
        - `vrpn_BaseFlexServer::mainloop()`

### For sync devices
`DeviceToken::connectionInteract()` calls update callback - so control flow only passes into the plugin during a connection interaction interval.

### For async devices
Their wait callback can call `DeviceToken::sendData()` whenever, which blocks until DeviceToken::connectionInteract() gets called.

## Ownership

- `ConnectionDevice`
  - Held in a `ConnectionDevicePtr` shared pointer by
    - Its corresponding `DeviceToken`, if any.
    - The `Connection` object following `Connection::addDevice` (which is not always explicit)
- `DeviceToken` owned exclusively by the plugin-specific registration context
  - raw pointer for access provided to plugin
