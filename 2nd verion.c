#include <windows.h>
#include <winbio.h>
#include <stdio.h>
#include <sddl.h>           // For ConvertSidToStringSid
#include <tchar.h>          // For TCHAR macros

int main() {
    WINBIO_SESSION_HANDLE sessionHandle = NULL;
    WINBIO_UNIT_ID unitId = 0;
    WINBIO_IDENTITY identity = {0};
    WINBIO_BIOMETRIC_SUBTYPE subFactor = 0;
    WINBIO_REJECT_DETAIL rejectDetail = 0;

    HRESULT hr = WinBioOpenSession(
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

    BOOLEAN match = FALSE;

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

    if (identity.Type == WINBIO_ID_TYPE_SID) {
        wchar_t name[256], domain[256];
        DWORD nameSize = 256, domainSize = 256;
        SID_NAME_USE sidType;

        BOOL success = LookupAccountSidW(
            NULL,
            (PSID)identity.Value.AccountSid.Data,
            name,
            &nameSize,
            domain,
            &domainSize,
            &sidType
        );

        if (success) {
            wprintf(L"✅ Fingerprint matched!\n");
            wprintf(L"User: %s\\%s\n", domain, name);
        } else {
            printf("✅ Fingerprint matched, but couldn't retrieve username.\n");
            printf("LookupAccountSidW failed. Error code: %lu\n", GetLastError());
        }
    } else {
        printf("Matched, but identity is not SID-based.\n");
    }

    WinBioCloseSession(sessionHandle);
    return 0;
}
