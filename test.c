#include <windows.h>
#include <winbio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <objbase.h>

#pragma comment(lib, "winbio.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "Advapi32.lib")

int main() {
    WINBIO_SESSION_HANDLE session = NULL;
    HRESULT hr;
    WINBIO_UNIT_ID unitId = 0;
    WINBIO_BIOMETRIC_SUBTYPE subFactor = WINBIO_ANSI_381_POS_RH_INDEX_FINGER;
    WINBIO_REJECT_DETAIL rejectDetail = 0;
    WINBIO_IDENTITY identity;

    // Step 1: Open biometric session
    hr = WinBioOpenSession(
        WINBIO_TYPE_FINGERPRINT,
        WINBIO_POOL_SYSTEM,
        WINBIO_FLAG_DEFAULT,
        NULL, 0,
        NULL,
        &session
    );

    if (FAILED(hr)) {
        printf("Failed to open biometric session. hr = 0x%x\n", hr);
        return 1;
    }

    // Step 2: Prompt for name
    char name[100];
    printf("Enter a name for this fingerprint: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

    // Step 3: Begin enrollment
    hr = WinBioEnrollBegin(session, subFactor, WINBIO_PURPOSE_ENROLL);
    if (FAILED(hr)) {
        printf("Failed to begin enrollment. hr = 0x%x\n", hr);
        WinBioCloseSession(session);
        return 1;
    }

    // Step 4: Capture fingerprint samples (fixed 5 samples)
    for (int i = 1; i <= 5; ++i) {
        printf("Touch the fingerprint sensor (%d/5)...\n", i);
        hr = WinBioEnrollCapture(session, &rejectDetail);
        if (FAILED(hr)) {
            printf("Capture failed. hr = 0x%x (Reject detail: %d)\n", hr, rejectDetail);
            WinBioCloseSession(session);
            return 1;
        }
    }

    // Step 5: Generate GUID
    GUID guid;
    CoInitialize(NULL);
    hr = CoCreateGuid(&guid);
    CoUninitialize();

    if (FAILED(hr)) {
        printf("GUID generation failed. hr = 0x%x\n", hr);
        WinBioCloseSession(session);
        return 1;
    }

    // Step 6: Assign GUID to identity
    identity.Type = WINBIO_ID_TYPE_GUID;
    memcpy(&identity.Value.TemplateGuid, &guid, sizeof(GUID));

    // Step 7: Commit enrollment
    hr = WinBioEnrollCommit(session, &identity, &unitId);
    if (FAILED(hr)) {
        printf("Enrollment commit failed. hr = 0x%x\n", hr);
        WinBioCloseSession(session);
        return 1;
    }

    // Step 8: Save to CSV
    FILE *fp = fopen("enrollments.csv", "a");
    if (fp) {
        fprintf(fp, "\"{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\",\"%s\"\n",
            guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
            guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7],
            name);
        fclose(fp);
        printf("Enrollment complete and saved as %s.\n", name);
    } else {
        printf("Failed to save enrollment.\n");
    }

    WinBioCloseSession(session);
    return 0;
}
