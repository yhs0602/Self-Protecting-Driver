/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_KMDFDriver1,
    0xbb87f93d,0xc796,0x4b09,0xa5,0xa7,0xe3,0x0f,0xe5,0x83,0x51,0x37);
// {bb87f93d-c796-4b09-a5a7-e30fe5835137}
