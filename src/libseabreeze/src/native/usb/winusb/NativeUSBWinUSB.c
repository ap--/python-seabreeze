/***************************************************//**
 * @file    NativeUSBWinUSB.c
 * @date    June 2009
 * @author  Ocean Optics, Inc.
 *
 * This is an implementation of the USB interface using
 * the WinUSB API.  This should provide support for
 * Windows XP, Vista, and beyond, both 32-bit and 64-bit.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2014, Ocean Optics Inc
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *******************************************************/

/* This must be defined prior to the other includes. */
#define INITGUID

#include "common/globals.h"
#include <windows.h>
#include <setupapi.h>
#include <strsafe.h>
#include <Winusb.h>

#include "native/usb/NativeUSB.h"
#include "native/usb/winusb/WindowsGUID.h"
#include "api/seabreezeapi/SeaBreezeAPIConstants.h" // for SET_ERROR_CODE

/* Macro and constant definitions */
#define MISSING_IMPL() {}
#define MAX_USB_DEVICES     127
#define DEVICE_PATH_SIZE    1024

typedef struct {
    long deviceID;
    HANDLE dev;
    WINUSB_INTERFACE_HANDLE winUSBHandle;
} __usb_interface_t;

typedef struct {
    long deviceID;
    __usb_interface_t *handle;
    char devicePath[DEVICE_PATH_SIZE];  /* Can this be dynamic? */
    unsigned short vendorID;
    unsigned short productID;
    unsigned char valid;    /* Whether this struct is valid */
    unsigned char mark;
} __device_instance_t;

/* Global variables (mostly static lookup tables) */
static __device_instance_t __enumerated_devices[MAX_USB_DEVICES] = { { 0 } };
static int __enumerated_device_count = 0;   /* To keep linear searches short */
static long __last_assigned_deviceID = 0;   /* To keep device IDs unique */

/* Function prototypes */
BOOL __getDevicePath(LPGUID interfaceGUID,
        PCHAR devicePath, size_t bufferLength, int deviceIndex,
        BOOL *noMoreDevices);
BOOL __getUSBHandle(char *devicePath, HANDLE *dev_out,
                    WINUSB_INTERFACE_HANDLE *winusb_out);
int __getDeviceDescriptor(WINUSB_INTERFACE_HANDLE handle,
        struct USBDeviceDescriptor *desc);
int __probeUSBDevices(int vendorID, int productID);
__device_instance_t *__lookup_device_instance_by_ID(long deviceID);
__device_instance_t *__lookup_device_instance_by_location(char *devicePath);
__device_instance_t *__add_device_instance(char *devicePath,
                                           int vendorID, int productID);
void __close_and_dealloc_usb_interface(__usb_interface_t *usb);
void __purge_unmarked_device_instances(int vendorID, int productID);

/* Function definitions */

/* This function will iterate over the known devices and attempt to match
 * the given ID.  It might be more efficient for the sake of this search
 * to store the device instances in a linked list so that no invalid elements
 * need to be searched through, but that just has the effect of shifting the
 * time burden to when a device must be removed from the list.  This lookup
 * is most likely to be called when the calling code is trying to open a device,
 * at which point they more or less expect to have to wait a little while.
 * Not that searching over this array will take very long...
 */
__device_instance_t *__lookup_device_instance_by_ID(long deviceID) {
    int i;
    int valid;
    /* The __enumerated_device_count is used to end the search once it is
     * known that there are no more devices to be found.
     */
    for(    i = 0, valid = 0;
            i < MAX_USB_DEVICES && valid < __enumerated_device_count;
            i++) {
        if(0 != __enumerated_devices[i].valid) {
            if(__enumerated_devices[i].deviceID == deviceID) {
                return &(__enumerated_devices[i]);
            }
            valid++;
        }
    }
    return NULL;
}

__device_instance_t *__lookup_device_instance_by_location(char *devicePath) {
    int i;
    int valid;
    /* The __enumerated_device_count is used to end the search once it is
     * known that there are no more devices to be found.
     */
    for(    i = 0, valid = 0;
            i < MAX_USB_DEVICES && valid < __enumerated_device_count;
            i++) {
        if(0 != __enumerated_devices[i].valid) {
            /* These paths are going to be Unicode, so deal with
             * having extra NULLs.
             */
            if(0 == wcsncmp((WCHAR *)__enumerated_devices[i].devicePath,
                    (WCHAR *)devicePath, DEVICE_PATH_SIZE)) {
                return &(__enumerated_devices[i]);
            }
            valid++;
        }
    }
    return NULL;
}

__device_instance_t *__add_device_instance(char *devicePath,
                                           int vendorID, int productID) {
    int i;

    /* First need to find an empty slot to store this device descriptor */
    for(i = 0; i < MAX_USB_DEVICES; i++) {
        if(0 == __enumerated_devices[i].valid) {
            /* Found an empty slot */
            __enumerated_devices[i].valid = 1;
            __enumerated_devices[i].deviceID = __last_assigned_deviceID++;
            /* Note that the device path will probably be in Unicode, meaning that there
             * may be NULL bytes in the middle of it.  Rather than trying to do a string
             * copy that will end when it sees a NULL, simply do a memcpy().
             */
            memcpy(__enumerated_devices[i].devicePath, devicePath, DEVICE_PATH_SIZE);
            __enumerated_devices[i].vendorID = vendorID;
            __enumerated_devices[i].productID = productID;
            __enumerated_device_count++;
            return &(__enumerated_devices[i]);
        }
    }
    return NULL;
}


void __purge_unmarked_device_instances(int vendorID, int productID) {
    int new_count = 0;
    int valid = 0;
    int i;
    __device_instance_t *device;

    for(i = 0; i < MAX_USB_DEVICES && valid < __enumerated_device_count; i++) {
        device = &(__enumerated_devices[i]);
        if(0 == device->valid) {
            continue;
        }
        /* From here down, the device entry was thought to be valid */
        valid++;

        /* Now check whether it was marked as still being present.  Note that
         * this search is limited just to the type of device that was being
         * probed when this call was made -- otherwise, any devices that were
         * not being probed for just now would be unmarked and would be
         * discarded.
         */
        if(0 == device->mark
                && (vendorID == device->vendorID)
                && (productID == device->productID)) {
            /* Not marked, so it needs to be purged */
            if(NULL != device->handle) {
                /* Clean up the device since it seems to have been disconnected */
                __close_and_dealloc_usb_interface(device->handle);
            }
            /* Wipe the structure completely */
            memset(&__enumerated_devices[i], (int)0, sizeof(__device_instance_t));
        } else {
            /* Device was valid or a different type so it survived this pass */
            __enumerated_devices[i].mark = 0;
            /* Keep track of how many survived */
            new_count++;
        }
    }
    __enumerated_device_count = new_count;
}

/* This will attempt to free up all resources associated with an open
 * USB descriptor.  This also deallocates the provided pointer.
 */
void __close_and_dealloc_usb_interface(__usb_interface_t *usb) {
    if(NULL == usb) {
        return;
    }

    if(NULL != usb->winUSBHandle) {
        WinUsb_Free(usb->winUSBHandle);
    }
    if(NULL != usb->dev) {
        CloseHandle(usb->dev);
    }

    HeapFree(GetProcessHeap(), 0, usb);
}


int
USBProbeDevices(int vendorID, int productID, unsigned long *output,
                int max_devices) {
    int i;
    int matched;
    int valid;

    matched = __probeUSBDevices(vendorID, productID);

    if(matched < 0) {
        /* Error occurred when trying to probe devices */
        return matched;
    }

    for(    i = 0, valid = 0;
            i < MAX_USB_DEVICES && valid < matched && valid < max_devices;
            i++) {
        if(0 != __enumerated_devices[i].valid
                && __enumerated_devices[i].vendorID == vendorID
                && __enumerated_devices[i].productID == productID) {
            output[valid] = __enumerated_devices[i].deviceID;
            valid++;
        }
    }

    return valid;
}

void *
USBOpen(unsigned long deviceID, int *errorCode) {
    HANDLE dev = NULL;
    WINUSB_INTERFACE_HANDLE usbHandle = NULL;
    BOOL rc = FALSE;
    __usb_interface_t *retval;
    __device_instance_t *instance;
    struct USBDeviceDescriptor desc;
    int flag;

    /* Set a default error code in case a premature return is required */
    SET_ERROR_CODE(NO_DEVICE_FOUND);

    /* Try to look up the deviceID provided by the caller.  If this does
     * not exist, then there is nothing else to do.
     */
    instance = __lookup_device_instance_by_ID(deviceID);
    if(NULL == instance) {
        /* The device ID was not found.  The caller must only provide IDs that
         * have previously been provided by the USBProbeDevices() function
         * (which is wrapped by getDeviceIDs()).
         */
        return 0;
    }

    if(NULL != instance->handle) {
        /* If there is already a device handle then the device has been opened.
         * It is illegal to try to open a device twice without first closing it.
         */
        return 0;
    }

    rc = __getUSBHandle(instance->devicePath, &dev, &usbHandle);

    if(FALSE == rc) {
        /* This is an internal error */
        return 0;
    }

    if(NULL == dev || NULL == usbHandle) {
        /* This is an internal error */
        return 0;
    }

    /* Just to be entirely pedantic, verify that the VID and PID are exactly
     * what they were supposed to be.
     */
    flag = __getDeviceDescriptor(usbHandle, &desc);
    if(0 != flag) {
        WinUsb_Free(usbHandle);
        CloseHandle(dev);
        return 0;
    }

    if((desc.idVendor != instance->vendorID) || (desc.idProduct != instance->productID)) {
        WinUsb_Free(usbHandle);
        CloseHandle(dev);
        return 0;
    }

    /* The device has survived scrutiny, so get ready to return it. */

    retval = (__usb_interface_t *)HeapAlloc(GetProcessHeap(),
                        HEAP_ZERO_MEMORY, sizeof(__usb_interface_t));

    if(NULL == retval) {
        WinUsb_Free(usbHandle);
        CloseHandle(dev);
        /* Internal error -- could not allocate memory */
        return 0;
    }

    retval->dev = dev;
    retval->winUSBHandle = usbHandle;
    retval->deviceID = instance->deviceID;
    instance->handle = retval;

    SET_ERROR_CODE(OPEN_OK);

    return (void *)retval;
}

int
USBClose(void *deviceHandle) {
    /* Local variables */
    __usb_interface_t *usb;
    __device_instance_t *device;

    if(NULL == deviceHandle) {
        return CLOSE_ERROR;
    }

    usb = (__usb_interface_t *)deviceHandle;

    device = __lookup_device_instance_by_ID(usb->deviceID);
    if(NULL != device) {
        /* This had an extra reference to the handle so free it up */
        device->handle = NULL;
    }

    __close_and_dealloc_usb_interface(usb);
    return CLOSE_OK;
}

int
USBWrite(void *deviceHandle, unsigned char endpoint, char * data, int numberOfBytes) {
    /* Local variables */
    long transferred = 0;
    __usb_interface_t *usb;

    if(0 == deviceHandle) {
        return WRITE_FAILED;
    }

    usb = (__usb_interface_t *)deviceHandle;

    WinUsb_WritePipe(usb->winUSBHandle, endpoint, data, numberOfBytes,
        &transferred, NULL);

    return (int)transferred;
}

int
USBRead(void *deviceHandle, unsigned char endpoint, char * data, int numberOfBytes) {
    /* Local variables */
    long transferred = 0;
    __usb_interface_t *usb;

    if(0 == deviceHandle) {
        return WRITE_FAILED;
    }

    usb = (__usb_interface_t *)deviceHandle;

    WinUsb_ReadPipe(usb->winUSBHandle, endpoint, data, numberOfBytes,
            &transferred, NULL);

    return (int)transferred;
}

void
USBClearStall(void *deviceHandle, unsigned char endpoint) {
    /* Local variables */
    __usb_interface_t *usb;

    if(0 == deviceHandle) {
        return;
    }

    usb = (__usb_interface_t *)deviceHandle;

    /* In WinUSB, ResetPipe will reset the toggle bits and clear a stall. */
    WinUsb_ResetPipe(usb->winUSBHandle, endpoint);
}

int
USBGetDeviceDescriptor(void *deviceHandle, struct USBDeviceDescriptor *desc) {
    __usb_interface_t *usb;

    if(0 == desc) {
        return -1;
    }

    if(0 == deviceHandle) {
        return -2;
    }

    usb = (__usb_interface_t *)deviceHandle;

    return __getDeviceDescriptor(usb->winUSBHandle, desc);
}

int __getDeviceDescriptor(WINUSB_INTERFACE_HANDLE handle,
        struct USBDeviceDescriptor *desc) {
    DWORD bytesReturned;
    BOOL flag;

    if(NULL == handle) {
        return 0;
    }

    if(NULL == desc) {
        return 0;
    }

    flag = WinUsb_GetDescriptor(handle,
        USB_DEVICE_DESCRIPTOR_TYPE, 0, 0x0409, (PUCHAR)desc,
        sizeof(struct USBDeviceDescriptor), &bytesReturned);

    if(FALSE == flag) {
        return -1;
    }

    return 0;
}

int
USBGetInterfaceDescriptor(void *deviceHandle, struct USBInterfaceDescriptor *desc) {
    DWORD bytesReturned = 0;
    BOOL flag = FALSE;
    BYTE temp[DEVICE_PATH_SIZE];
    PUSB_COMMON_DESCRIPTOR retval;
    __usb_interface_t *usb;

    if(0 == desc) {
        return -1;
    }

    if(0 == deviceHandle) {
        return -2;
    }

    usb = (__usb_interface_t *)deviceHandle;
    memset(temp, 0, sizeof(temp));

    flag = WinUsb_GetDescriptor(usb->winUSBHandle,
        USB_CONFIGURATION_DESCRIPTOR_TYPE, 0, 0, temp,
        sizeof(temp), &bytesReturned);

    if(FALSE == flag) {
        return -3;
    }

    retval = WinUsb_ParseDescriptors(temp, bytesReturned, temp,
        USB_INTERFACE_DESCRIPTOR_TYPE);

    if(NULL == retval) {
        return -4;
    }

    memcpy(desc, retval, sizeof(struct USBInterfaceDescriptor));

    return 0;
}

int
USBGetEndpointDescriptor(void *deviceHandle, int endpoint_index,
        struct USBEndpointDescriptor *desc) {
    DWORD bytesReturned = 0;
    BOOL flag = FALSE;
    BYTE temp[DEVICE_PATH_SIZE];
    int offset = 0;
    __usb_interface_t *usb;

    if(0 == desc) {
        return -1;
    }

    if(0 == deviceHandle) {
        return -2;
    }

    usb = (__usb_interface_t *)deviceHandle;
    memset(temp, 0, sizeof(temp));

    flag = WinUsb_GetDescriptor(usb->winUSBHandle,
        USB_CONFIGURATION_DESCRIPTOR_TYPE, 0, 0, temp,
        sizeof(temp), &bytesReturned);

    if(FALSE == flag) {
        return -1;
    }

    /* If we got here without the flag above getting set to false
     * all appears well.  We now should have the following data
     * in temp:
     *
     * Configuration Descriptor (9 bytes)
     * Interface Descriptor (9 bytes)
     * Endpoint 0 Descriptor (7 bytes)
     * Endpoint 1 Descriptor (7 bytes)
     * Enpoint 2 Descriptor (7 bytes)
     * Endpoint 3 Descriptor (7 bytes)
     * <<< and so on (if there are more than 4 endpoints) ... >>>
     *
     * Next, parse the descriptors out correctly.  We can use the
     * WinUsb_ParseDescriptors function to search for the descriptor
     * we want but the implementation of that fucntion means we have to
     * find the first Endpoint Descriptor, then loop until we've iterated
     * through to find what we think is the Endpoint Descriptor we need.
     * It is cumbersome and seems unecessary since the documetation implies
     * that the resulting data will always be returned in the same order.
     * Calculating the memory offset and just doing a memcpy is faster.
     */
    offset = sizeof(struct USBConfigurationDescriptor) +
        sizeof(struct USBInterfaceDescriptor) +
        (sizeof(struct USBEndpointDescriptor) * endpoint_index) -
        endpoint_index - 1;

    memcpy(desc, temp + offset, sizeof(struct USBEndpointDescriptor));

    return 0;
}

int
USBGetStringDescriptor(void *deviceHandle, unsigned int string_index,
                       char *user_buffer, int maxLength) {
    __usb_interface_t *usb;
    char buffer[512];
    DWORD bytesReturned;
    BOOL retval = TRUE;
    int stringLength = 0;
    int i = 0;

    if(0 == deviceHandle) {
        return -1;
    }

    usb = (__usb_interface_t *)deviceHandle;
    memset(buffer, 0, sizeof(buffer));

    retval = WinUsb_GetDescriptor(usb->winUSBHandle,
            USB_STRING_DESCRIPTOR_TYPE, (UCHAR) string_index, 0x0409, (PUCHAR) buffer,
            (ULONG)sizeof(buffer), &bytesReturned);

    if (FALSE == retval) {
        return -1;
    }

    /* The string in the buffer is a string of Unicode characters.  This
     * converts back to a simple ASCII string.  If the caller really
     * wanted Unicode this could be done differently.
     */
    stringLength = (bytesReturned - 2) / 2;
    for (i = 0; i < stringLength && i < maxLength; i++) {
        user_buffer[i] = buffer[2 + (i * 2)];
    }

    return stringLength;
}

// #pragma warning(disable: 4133)
/* This is a convenience method that attempts to traverse the set of
 * discoverable USB devices having the given VID and PID and updates the
 * static data structures that track them.  For any device that is found,
 * the device path, VID and PID will be cached in __enumerated_devices but
 * the devices will not be left open (though it may be necessary to open
 * them briefly to get the VID and PID, which is a WinUSB requirement).
 */
int __probeUSBDevices(int vendorID, int productID) {
    HANDLE dev = NULL;
    char devicePath[DEVICE_PATH_SIZE];
    BOOL rc;
    int flag;
    WINUSB_INTERFACE_HANDLE usbHandle;
    struct USBDeviceDescriptor desc;
    int i;
    int matched;
    int valid;
    BOOL noMoreDevices = FALSE;
    __device_instance_t *instance = NULL;

    /* This __getDevicePath uses a single interface GUID to match the given VID and
     * PID.  If multiple GUIDs were needed (e.g. to match .inf files from other
     * vendors) then this function could be put into a loop to iterate over all
     * known GUIDs.  This would need to be careful with the index, since the
     * index will start at zero again for each device interface class.
     *
     * Note that the .inf file specifies both a class GUID and an interface GUID.
     * These should be different from each other, but the value for each of
     * these across all .inf files should match.
     *
     * It is a little unfortunate that other device classes would need to be
     * hard-coded in here to be searched.  Users will have to have the latest
     * build of this DLL to avoid having sets of supported devices disappear.
     */
    for(i = 0; i < MAX_USB_DEVICES && FALSE == noMoreDevices; i++) {
        rc = __getDevicePath((LPGUID)&GUID_DEVINTERFACE_OCEANOPTICS_USB,
                devicePath, sizeof(devicePath), i, &noMoreDevices);

        if(FALSE == rc) {
            break;      /* May not be any more devices */
        }

        /* Avoid opening any devices at paths that are already known */
        instance = __lookup_device_instance_by_location(devicePath);
        if(NULL != instance) {
            instance->mark = 1; /* Make sure this is not purged this time */
            continue;           /* Leave this one alone otherwise */
        }

        /* Create a file for this device path.  This is a necessary evil
         * to read out the device descriptor (note that Linux and MacOSX don't
         * require this).  If it is found that this is causing interference
         * with open devices, then perhaps the call can be changed to request
         * read-only access.
         */
        dev = CreateFile((LPCWSTR)devicePath, (GENERIC_WRITE | GENERIC_READ),
            (FILE_SHARE_WRITE | FILE_SHARE_READ), NULL, OPEN_EXISTING,
            (FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED), NULL);

        if(INVALID_HANDLE_VALUE == dev) {
            continue;   /* Skip to the next one and try again */
        }

        rc = WinUsb_Initialize(dev, &usbHandle);
        if(FALSE == rc) {
            CloseHandle(dev);
            dev = NULL;
            continue;   /* Skip to the next one and try again */
        }

        /* Now that usbHandle has been initialized, determine whether the
         * device matches the requested VID and PID.
         */
        flag = __getDeviceDescriptor(usbHandle, &desc);

        /* This function is only here to do discovery so once the device
         * descriptor has been read everything can be freed up again.
         */
        WinUsb_Free(usbHandle);
        CloseHandle(dev);
        usbHandle = NULL;
        dev = NULL;

        if(0 != flag) {
            /* The attempt to get the descriptor above failed */
            continue;   /* Skip to the next one and try again */
        }


        if((desc.idVendor != vendorID) || (desc.idProduct != productID)) {
            continue;   /* No match, so skip to the next one and try again */
        }

        /* At this point, we must be dealing with a newly discovered USB
         * device that matches the given VID and PID.  It must now be
         * cached for use with the open function.  Note that instance was
         * checked above so it must be NULL here.
         */
        instance = __add_device_instance(devicePath, vendorID, productID);
        if(NULL == instance) {
            /* Could not add the device -- this should not be possible,
             * but fail gracefully regardless.
             */
            return -1;
        }
        instance->mark = 1;     /* Preserve this since it was just seen */
    }

    /* Purge any devices that are cached but that no longer exist. */
    __purge_unmarked_device_instances(vendorID, productID);

    /* Count up how many of this type of device are known */
    for(    i = 0, matched = 0, valid = 0;
            i < MAX_USB_DEVICES && valid < __enumerated_device_count;
            i++) {
        if(0 != __enumerated_devices[i].valid) {
            valid++;
            if(__enumerated_devices[i].vendorID == vendorID
                        && __enumerated_devices[i].productID == productID) {
                matched++;
            }
        }
    }

    /* At this point, the device cache should have the latest information
     * on this VID/PID combination.  Return the number of devices of this
     * type that were found.
     */
    return matched;
}

/* This is a convenience method that attempts to open a device with the
 * specified path.  Upon success, it provides a HANDLE
 * and WINUSB_INTERFACE_HANDLE for the device.  It is the responsibility of the
 * caller to dispose of these when finished.
 */
BOOL __getUSBHandle(char *devicePath, HANDLE *dev_out,
                    WINUSB_INTERFACE_HANDLE *winusb_out) {
    HANDLE dev = NULL;
    BOOL rc;
    WINUSB_INTERFACE_HANDLE usbHandle;

    dev = CreateFile((LPCWSTR)devicePath, (GENERIC_WRITE | GENERIC_READ),
        (FILE_SHARE_WRITE | FILE_SHARE_READ), NULL, OPEN_EXISTING,
        (FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED), NULL);

    if(INVALID_HANDLE_VALUE == dev) {
        return FALSE;
    }

    rc = WinUsb_Initialize(dev, &usbHandle);

    if(FALSE == rc) {
        CloseHandle(dev);
        return FALSE;
    }

    if(NULL != dev_out) {
        *dev_out = dev;
    }

    if(NULL != winusb_out) {
        *winusb_out = usbHandle;
    }

    return TRUE;
}

/* This code is based on sample code provided by Microsoft in their
 * How-To document for the WinUSB API.
 */
BOOL __getDevicePath(LPGUID guid, PCHAR devicePath, size_t bufferLength,
                     int deviceIndex, BOOL *noMoreDevices) {
    BOOL retval = FALSE;
    HDEVINFO deviceInfo;
    SP_DEVICE_INTERFACE_DATA interfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = NULL;
    ULONG length;
    ULONG requiredLength = 0;
    HRESULT copyResult;
    int error;

    /* Get a handle on device information based on the specified GUID */
    /* Note that this only requires that the device match the given class and
     * that the device is plugged in.
     *
     * For information on the SetupDiGetClassDevs() function, look on MSDN:
     * http://msdn.microsoft.com/en-us/library/ms792959.aspx
     */
    deviceInfo = SetupDiGetClassDevs(guid, NULL, NULL,
        (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));

    if(INVALID_HANDLE_VALUE == deviceInfo) {
        error = GetLastError(); /* Assigning this to make debugging easier */
        /* Failed to find that any match (which isn't necessarily an error) */
        if(NULL != noMoreDevices) {
            *noMoreDevices = TRUE;
        }
        return FALSE;
    }

    /* Enumerate device interfaces.  This grabs the device at the specified
     * index.  This function can be called with increasing index numbers
     * until it finally returns FALSE, which means the last has been found.
     * Documentation for SetupDiEnumDeviceInterfaces() can be found on MSDN:
     * http://msdn.microsoft.com/en-us/library/ms791242.aspx
     */
    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    retval = SetupDiEnumDeviceInterfaces(deviceInfo, NULL, guid,
        deviceIndex, &interfaceData);

    if(FALSE == retval) {
        error = GetLastError(); /* Assigning this to make debugging easier */
        /* This may happen in normal operation when trying to count the number
         * of attached devices (this will occur when going beyond the index
         * of the last device.
         */
        if(NULL != noMoreDevices) {
            *noMoreDevices = TRUE;
        }
        /* Yes, this assignment is redundant but is being done for safety in
         * case the conditional above ever changes.
         */
        retval = FALSE;
        goto fail_1;
    }

    /* Get the length of the interface detail structure.  This fills in the
     * value of requiredLength for use below.  Using SetupDiGetDeviceInterfaceDetail()
     * in this manner is described on MSDN (comments section, step 1):
     * http://msdn.microsoft.com/en-us/library/ms792901.aspx
     */
    retval = SetupDiGetDeviceInterfaceDetail(deviceInfo, &interfaceData, NULL, 0,
        &requiredLength, NULL);

    if(FALSE != retval) {
        /* This was expected to fail (with ERROR_INSUFFICIENT_BUFFER).  If it
         * somehow succeeded then there is a problem so bail out.
         */
        retval = FALSE;
        goto fail_1;
    }

    detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LMEM_FIXED,
        requiredLength);

    if(NULL == detailData) {
        retval = FALSE;
        goto fail_1;
    }

    detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    length = requiredLength;

    /* Get detailed information for the device interface.  This calls
     * SetupDiGetDeviceInterfaceDetail again but with more of the parameters
     * defined.  This corresponds to step 2 in the comments section of the
     * description for this function on MSDN:
     * http://msdn.microsoft.com/en-us/library/ms792901.aspx
     */
    retval = SetupDiGetDeviceInterfaceDetail(deviceInfo, &interfaceData,
        detailData, length, &requiredLength, NULL);

    if(FALSE == retval) {
        retval = FALSE;
        goto fail_2;
    }

    /* Copy into the user-provided buffer */
    copyResult = StringCchCopy((LPTSTR)devicePath, bufferLength, (STRSAFE_LPCWSTR)detailData->DevicePath);
    if(FAILED(copyResult)) {
        retval = FALSE;
        goto fail_2;
    }

    /* Yes, I know goto is evil, but for deconstructing state before exiting
     * a function in the absence of thrown exceptions, they have their place.
     */
fail_2:
    LocalFree(detailData);
fail_1:
    SetupDiDestroyDeviceInfoList(deviceInfo);

    /* This includes the normal successful return value from above.  The
     * fail_1 and fail_2 are executed even if everything was successful.
     */
    return retval;
}
