#include <windows.h>
#include <winbio.h>
#include <stdio.h>

int main() {
    HRESULT hr = S_OK;
    WINBIO_SESSION_HANDLE sessionHandle = NULL;
    WINBIO_UNIT_ID unitId = 0;
    WINBIO_IDENTITY identity = {0};
    WINBIO_BIOMETRIC_SUBTYPE subFactor = WINBIO_SUBTYPE_NO_INFORMATION;
    BOOLEAN match = FALSE;
    WINBIO_REJECT_DETAIL rejectDetail = 0;

    // Open a biometric session
    hr = WinBioOpenSession(
        WINBIO_TYPE_FINGERPRINT,      // Biometric type
        WINBIO_POOL_SYSTEM,           // Pool type
        WINBIO_FLAG_DEFAULT,          // Configuration and access
        NULL,                         // Array of biometric unit IDs
        0,                            // Count of biometric unit IDs
        NULL,                         // Database ID
        &sessionHandle                // [out] Session handle
    );

    if (FAILED(hr)) {
        printf("WinBioOpenSession failed. hr = 0x%x\n", hr);
        return 1;
    }

    printf("Touch the fingerprint reader...\n");

    // Identify the user instead of verify
    hr = WinBioIdentify(
        sessionHandle,
        &unitId,
        &identity,
        &subFactor,
        &rejectDetail
    );

    if (FAILED(hr)) {
        printf("WinBioIdentify failed. hr = 0x%x\n", hr);
        WinBioCloseSession(sessionHandle);
        return 1;
    }

    printf("Fingerprint match found! User SID (partial): %s\n", identity.Value.AccountSid.Data);
    WinBioCloseSession(sessionHandle);
    return 0;
}
