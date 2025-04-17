#include <windows.h>
#include <winbio.h>
#include <stdio.h>

int main() {
    HRESULT hr = S_OK;
    WINBIO_SESSION_HANDLE sessionHandle = NULL;
    WINBIO_UNIT_ID unitId = 0;
    WINBIO_IDENTITY identity = {0};
    WINBIO_BIOMETRIC_SUBTYPE subFactor = WINBIO_SUBTYPE_NO_INFORMATION;
    WINBIO_REJECT_DETAIL rejectDetail = 0;

    // Open biometric session
    hr = WinBioOpenSession(
        WINBIO_TYPE_FINGERPRINT,
        WINBIO_POOL_SYSTEM,
        WINBIO_FLAG_DEFAULT,
        NULL,
        0,
        NULL,
        &sessionHandle
    );

    if (FAILED(hr)) {
        printf("WinBioOpenSession failed. hr = 0x%x\n", hr);
        return 1;
    }

    printf("Touch the fingerprint reader...\n");

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

    // Log attendance to file
    SYSTEMTIME t;
    GetLocalTime(&t);
    FILE *fp = fopen("attendance.csv", "a");
    fprintf(fp, "Attendance logged at %04d-%02d-%02d %02d:%02d:%02d\n",
        t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
    fclose(fp);

    // --- SERIAL COMM TO ARDUINO START ---
    HANDLE hSerial;
    DWORD bytesWritten;
    char dataToSend[] = "1"; // Or any marker you want

    hSerial = CreateFile(
        "\\\\.\\COM3",                // ⚠️ UPDATE COM PORT HERE
        GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        0,
        0);

    if (hSerial != INVALID_HANDLE_VALUE) {
        WriteFile(hSerial, dataToSend, sizeof(dataToSend), &bytesWritten, NULL);
        CloseHandle(hSerial);
    } else {
        printf("Failed to open serial port\n");
    }
    // --- SERIAL COMM TO ARDUINO END ---

    WinBioCloseSession(sessionHandle);
    return 0;
}
