/* This implementation keeps track of sessions similar to libusb.
 * Sessions are opened with os_usb_init, and closed with
 * os_usb_close. Once all sessions are closed, resources are
 * released (all devices that have been opened technically stay open
 * until the final os_usb_close), and it becomes possible to
 * re-enumerate devices. If the API is updated so that os_usb_close
 * accepts a device number parameter, devices can be closed
 * independently of each other.
 *
 * Scenarios that can cause a device enumeration:
 * - The first call of either os_usb_count_devices or os_usb_init
 * - After opening at least one device and then closing all devices, a
 *   call to os_usb_count_devices or os_usb_init.
 *
 * There is a known resource leak if a user calls os_usb_count_devices
 * but never os_usb_init/os_usb_close. This is because devices are
 * only enumerated once until all devices are closed, so that device
 * indexing remains constant during use. When os_usb_init/os_usb_close
 * are never called but os_usb_count_devices is, it results in the
 * libusb devices saved during enumeration never being released by a
 * final call to os_usb_close. Note that the enumeration will only
 * occur once in this situation, so even with repeated
 * os_usb_count_devices calls, the amount of memory lost does not
 * grow.
 */
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <usb.h>

#include "debug.h"
#include "driver-usb.h"
#include "libusb.h"
#include "util.h"

#define VENDOR_ID 0x0403
#define MAX_IO_WAIT_TIME 500

#define MAX_USB 128
#define MAX_SESSIONS UINT_MAX
#define USB_CONTEXT NULL

static libusb_device_handle* handles[MAX_USB] = { 0 };
static struct libusb_device* devices[MAX_USB] = { 0 };
static int num_spincore_usb_devices = 0;
static bool devices_have_been_enumerated = false;
static unsigned num_sessions = 0;
int pid_list[MAX_USB] = { 0 };

static void release_spincore_device(int i)
{
    enum libusb_error error_code;

    if (NULL == handles[i])
        goto release_device;

    debug(DEBUG_INFO, "usb: Closing device number %d", i);

    error_code = libusb_release_interface(handles[i], 0);
    if (error_code != LIBUSB_SUCCESS)
    {
        debug(DEBUG_WARNING, "usb: Failed to release interface. %s",
              libusb_error_name(error_code));
        /* Continue releasing resources since the interface is likely unusable */
    }

    libusb_close(handles[i]);
    handles[i] = NULL;
release_device:
    if (NULL != devices[i])
    {
        libusb_unref_device(devices[i]);
        devices[i] = NULL;
    }
}

static void release_all_spincore_devices()
{
    for (int i = 0; i < num_spincore_usb_devices; i++)
        release_spincore_device(i);
    num_spincore_usb_devices = 0;
    devices_have_been_enumerated = false;
}

/**
 * Count SpinCore devices on the USB bus.
 */
int os_usb_count_devices(int vendor_id)
{
    ssize_t num_usb_devices;
    libusb_device** all_devices_list;

    debug(DEBUG_INFO, "os_usb_count_devices called.");

    if (devices_have_been_enumerated)
        return num_spincore_usb_devices;

    // Retrieve list of devices.
    num_usb_devices = libusb_get_device_list(USB_CONTEXT, &all_devices_list);
    if (num_usb_devices < 0)
    {
        debug(DEBUG_ERROR,
              "os_usb_count_devices: Failed to get device list: %s",
              libusb_error_name(num_usb_devices));
        return -1;
    }

    // Check all USB devices for SpinCore devices.
    num_spincore_usb_devices = 0;
    for (ssize_t i = 0; i < num_usb_devices; i++)
    {
        struct libusb_device_descriptor device_descriptor;
        libusb_get_device_descriptor(all_devices_list[i], &device_descriptor);
        if (device_descriptor.idVendor == VENDOR_ID)
        {
            if (num_spincore_usb_devices + 1 > MAX_USB)
            {
                debug(DEBUG_WARNING, "os_usb_count_devices: Too many USB devices, stopping enumeration at number %d.",
                        num_spincore_usb_devices);
                break;
            }
            libusb_ref_device(all_devices_list[i]);
            devices [num_spincore_usb_devices] = all_devices_list[i];
            pid_list[num_spincore_usb_devices] = device_descriptor.idProduct;
            num_spincore_usb_devices++;
        }
    }
    libusb_free_device_list(all_devices_list, 1);

    devices_have_been_enumerated = true;
    return num_spincore_usb_devices;
}

/**
 * Unique USB device identifier is idVendor << 16 | idProduct
 *
 * \returns A negative number is returned on error, otherwise the device's ID
 */
int os_usb_init(int dev_num)
{
    enum libusb_error error_code;

    debug(DEBUG_INFO, "os_usb_init called.");

    if (dev_num >= MAX_USB)
    {
        debug(DEBUG_ERROR,
              "os_usb_init: Device number out of range, must be less than %d\n",
              MAX_USB);
        return -1;
    }
    if (num_sessions == MAX_SESSIONS)
    {
        debug(DEBUG_ERROR,
              "os_usb_init: Maximum number of sessions reached, max is %u\n",
              MAX_SESSIONS);
        return -1;
    }

    // Device already opened
    if (handles[dev_num] != NULL)
        return 0;
 
    // Start USB session.
    error_code = libusb_init(USB_CONTEXT);
    if(error_code != LIBUSB_SUCCESS)
    {
        debug(DEBUG_ERROR, "os_usb_init: Failed to initialize USB session. %s",
              libusb_error_name(error_code));
        return -1;
    }
    num_sessions++;

    // Search for device.
    int number_of_devices = os_usb_count_devices(VENDOR_ID);
    if (number_of_devices < 0)
    {
        debug(DEBUG_ERROR, "os_usb_init: os_usb_count_devices failed\n");
        goto error_end_session;
    }
    else if (number_of_devices == 0)
    {
        debug(DEBUG_ERROR, "os_usb_init: Device not found. No SpinCore devices found.");
        goto error_end_session;
    }
    else if (dev_num >= number_of_devices)
    {
        debug(DEBUG_ERROR, "os_usb_init: Device not found. Device number out of range.");
        goto error_end_session;
    }

    // Open communication with the device.
    error_code = libusb_open(devices[dev_num], &handles[dev_num]);
    if (error_code != LIBUSB_SUCCESS)
    {
        debug(DEBUG_ERROR, "os_usb_init: Device not opened. %s",
              libusb_error_name(error_code));
        handles[dev_num] = NULL;
        goto error_end_session;
    }

    // Only interface with the board provided.
    error_code = libusb_claim_interface(handles[dev_num], 0);
    if (error_code != LIBUSB_SUCCESS)
    {
        debug(DEBUG_ERROR, "os_usb_init: Failed to claim interface. %s",
              libusb_error_name(error_code));
        goto error_end_session;
    }

    return pid_list[dev_num];
error_end_session:
    /* This code is only reached if there is a session open, so skip error check */
    os_usb_close();
    return -1;
}

/* Fails if no session has been started (by os_usb_init) */
int os_usb_close()
{
    debug(DEBUG_INFO, "os_usb_close called.\n");

    if (num_sessions <= 0)
    {
        debug(DEBUG_ERROR, "os_usb_close: Called before os_usb_open\n");
        return -1;
    }

    if (num_sessions - 1 == 0)
        release_all_spincore_devices();
    libusb_exit(USB_CONTEXT);
    num_sessions--;
    return 0;
}

int os_usb_reset_pipes(int dev_num)
{
    enum libusb_error error_code;

    debug(DEBUG_INFO, "os_usb_reset_pipes called.");

    error_code = libusb_clear_halt(handles[dev_num], EP1OUT);
    if (error_code != LIBUSB_SUCCESS)
    {
        debug(DEBUG_INFO, "os_usb_reset_pipes: Could not clear halt on endpoint 1. %s",
              libusb_error_name(error_code));
        return -1;
    }

    error_code = libusb_clear_halt(handles[dev_num], EP2OUT);
    if (error_code != LIBUSB_SUCCESS)
    {
        debug(DEBUG_INFO, "os_usb_reset_pipes: Could not clear halt on endpoint 2. %s",
              libusb_error_name(error_code));
        return -1;
    }

    error_code = libusb_clear_halt(handles[dev_num], EP6IN);
    if (error_code != LIBUSB_SUCCESS)
    {
        debug(DEBUG_INFO, "os_usb_reset_pipes: Could not clear halt on endpoint 6. %s",
              libusb_error_name(error_code));
        return -1;
    }

    return 0;
}

/**
 * Write data to the USB device
 * \param pipe endpoint to write data too
 * \param data buffer holding data to be written
 * \param size Size in bytes of the buffer
 * \returns 0 on success, or a negative number on failure
 */
int os_usb_write(int dev_num, int pipe, const void *data, int size)
{
    debug(DEBUG_INFO, "os_usb_write(dev_num = %d, pipe = 0x%X, data, size = %d)",
          dev_num, pipe, size);

    int bytes_written = 0;
    enum libusb_error error_code;
    error_code = libusb_bulk_transfer(handles[dev_num], pipe, (unsigned char *)data, size, &bytes_written,
                                   MAX_IO_WAIT_TIME);
    if (error_code != LIBUSB_SUCCESS)
    {
        debug(DEBUG_INFO, "os_usb_write: Write failed. %s",
              libusb_error_name(error_code));
        return -1;
    }
    debug(DEBUG_INFO, "os_usb_write: Number of bytes written: %d", bytes_written);

    return  0;
}

/**
 * Read data from the USB device.
 * \param pipe Endpoint to read data from
 * \param data Buffer to hold the data that will be read
 * \param size Size in bytes of data to read
 * \returns 0 on success, or a negative number on failure
 */
int os_usb_read(int dev_num, int pipe, void *data, int size)
{
    debug(DEBUG_INFO, "os_usb_read(dev_num = %d, pipe = 0x%X, data, size = %d)",
          dev_num, pipe, size);

    int bytes_read = 0;
    enum libusb_error error_code;
    error_code = libusb_bulk_transfer(handles[dev_num], pipe, data, size, &bytes_read,
                                  MAX_IO_WAIT_TIME);
    if (error_code)
    {
        debug(DEBUG_INFO, "os_usb_read: Read failed. %s",
              libusb_error_name(error_code));
        return -1;
    }
    debug(DEBUG_INFO, "os_usb_read: Number of bytes read: %d", bytes_read);

    return  0;
}
