#include <windows.h>
#include <winbio.h>
#include <stdio.h>
#include <time.h>

int main() {
    HRESULT hr = S_OK;
    WINBIO_SESSION_HANDLE sessionHandle = NULL;
    WINBIO_UNIT_ID unitId = 0;
    WINBIO_IDENTITY identity = {0};
    WINBIO_BIOMETRIC_SUBTYPE subFactor = WINBIO_SUBTYPE_NO_INFORMATION;
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

    // Identify the user (no name, just fingerprint matched)
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

    printf("Fingerprint match found!\n");

    // Get current time
    time_t now;
    time(&now);
    struct tm* localTime = localtime(&now);

    char timeStr[100];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localTime);

    // Write to CSV file
    FILE* file = fopen("attendance.csv", "a");
    if (file != NULL) {
        fprintf(file, "Fingerprint Authenticated, %s\n", timeStr);
        fclose(file);
        printf("Attendance logged at %s\n", timeStr);
    } else {
        printf("Failed to open attendance.csv for writing.\n");
    }

    WinBioCloseSession(sessionHandle);
    return 0;
}
