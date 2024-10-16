#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <windows.h>
#include <winhttp.h>

#define INTERVAL 60
#define IP_LOG_FILE "url\\ip.log"
#define URL_LOG_FILE "url\\url.log"

// Function to convert char* to wchar_t*
wchar_t *charToWChar(const char *text) {
    size_t len = strlen(text) + 1;
    wchar_t *wText = (wchar_t *)malloc(len * sizeof(wchar_t));
    mbstowcs(wText, text, len);
    return wText;
}

void check_website(const char *url) {
    wchar_t *wUrl = charToWChar(url);
    URL_COMPONENTS urlComp;
    HINTERNET hSession, hConnect, hRequest;
    BOOL bResults;
    DWORD dwSize = sizeof(DWORD);
    DWORD dwStatusCode = 0;
    char log_message_buf[512];

    ZeroMemory(&urlComp, sizeof(urlComp));
    urlComp.dwStructSize = sizeof(urlComp);
    urlComp.dwSchemeLength = -1;
    urlComp.dwHostNameLength = -1;
    urlComp.dwUrlPathLength = -1;
    urlComp.dwExtraInfoLength = -1;

    if (!WinHttpCrackUrl(wUrl, (DWORD)wcslen(wUrl), 0, &urlComp)) {
        printf("Failed to parse URL: %s\n", url);
        free(wUrl);
        return;
    }

    hSession = WinHttpOpen(L"A HTTP Checker/1.0",
                           WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                           WINHTTP_NO_PROXY_NAME,
                           WINHTTP_NO_PROXY_BYPASS, 0);

    if (hSession) {
        hConnect = WinHttpConnect(hSession, urlComp.lpszHostName, urlComp.nPort, 0);

        if (hConnect) {
            hRequest = WinHttpOpenRequest(hConnect, L"HEAD", urlComp.lpszUrlPath, NULL,
                                          WINHTTP_NO_REFERER,
                                          WINHTTP_DEFAULT_ACCEPT_TYPES,
                                          (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0);

            if (hRequest) {
                bResults = WinHttpSendRequest(hRequest,
                                              WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                              WINHTTP_NO_REQUEST_DATA, 0,
                                              0, 0);

                if (bResults) {
                    bResults = WinHttpReceiveResponse(hRequest, NULL);
                    if (bResults) {
                        WinHttpQueryHeaders(hRequest,
                                            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                                            NULL, &dwStatusCode, &dwSize, NULL);
                    }
                }
                WinHttpCloseHandle(hRequest);
            }
            WinHttpCloseHandle(hConnect);
        }
        WinHttpCloseHandle(hSession);
    }

    free(wUrl);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[100];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

    snprintf(log_message_buf, sizeof(log_message_buf), "%s: %s returned %lu",
             time_str, url, dwStatusCode);

    printf("%s\n", log_message_buf);
    
}



