## 2017-08-17
### Updated
- Resigned USB driver signatures
- Updated Single Pulse NMR program
- Removed Hahn Echo, CPMG, and T1IR programs

## 2016-10-18
### Added
- PulseBlaster Interpreter has been added to SpinAPI installer package.

### Updated
- Driver install routine will now uninstall PCI devices before loading the INFs (if they already exist in the system). This will prevent the INF install process from hanging.

## 2016-10-04
### Added
- Integrated example programs into SpinAPI installer package.

## 2016-09-13
### Added
- Added this change log.
- Dual-signed SHA1/SHA256 USB drivers to support past Windows version (up to Windows 8.1)
- EV-signed USB drivers for Windows 10 support

### Updated
- Included drivers and binaries for FTDI library

### Removed
- Legacy DLL folder. Installers must be updated to use the appropriate lib directory.
