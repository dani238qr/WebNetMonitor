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

char **websites = NULL;
char **ips = NULL;
int num_websites = 0;
int num_ips = 0;

void load_database(const char *filename, char ***data, int *size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(1);
    }

    char line[256];
    *size = 0;
    *data = NULL;

    while (fgets(line, sizeof(line), file)) {
        (*size)++;
        *data = realloc(*data, sizeof(char *) * (*size));
        if (*data == NULL) {
            fprintf(stderr, "Memory allocation error.\n");
            exit(1);
        }
        (*data)[*size - 1] = strdup(line);
        if ((*data)[*size - 1] == NULL) {
            fprintf(stderr, "Memory allocation error.\n");
            exit(1);
        }
        (*data)[*size - 1][strcspn((*data)[*size - 1], "\n")] = '\0';
    }

    fclose(file);
}

void ip_log_message(const char *message) {
    FILE *file = fopen(IP_LOG_FILE, "a");
    if (file) {
        fprintf(file, "%s\n", message);
        fclose(file);
    }
}



//#####################################################################################################
// Send an email alert (Update the Send-MailMessage command in send_alert with correct SMTP server details and credentials.)

void send_alert(const char *destination, const char *subject, const char *body) {

    char command[512];

    snprintf(command, sizeof(command),
             "powershell -Command \"Send-MailMessage -To '%s' -Subject '%s' -Body '%s' -SmtpServer 'smtp.example.com'\"",
             destination, subject, body);
    system(command);

}

//#####################################################################################################

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

    free(wUrl);  // Free the wide string

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[100];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

    snprintf(log_message_buf, sizeof(log_message_buf), "%s: %s returned %ld",
             time_str, url, response_code);

    printf("%s\n", log_message_buf);
    url_log_message(log_message_buf);

    if (response_code >= 400) {
        char body[512];
        snprintf(body, sizeof(body), "The URL %s returned an error code %ld.", url, response_code);
    
//#####################################################################################################
        send_alert("your_email@example.com", "Website Down Alert", body);
//#####################################################################################################

}
}

void ping_ip(const char *ip) {
    int status;
    char command[256];
    char log_message_buf[512];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[100];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

    snprintf(command, sizeof(command), "ping -n 1 %s > NUL", ip);
    status = system(command);
    if (status == 0) {
        snprintf(log_message_buf, sizeof(log_message_buf), "%s %s is up", time_str, ip);
    } else {
        snprintf(log_message_buf, sizeof(log_message_buf), "%s %s is down", time_str, ip);
        char body[512];
        snprintf(body, sizeof(body), "The IP address %s is down.", ip);
    
//#####################################################################################################
        send_alert("your_email@example.com", "Website Down Alert", body);
//#####################################################################################################

}

    printf("%s\n", log_message_buf);
    ip_log_message(log_message_buf);
}

DWORD WINAPI monitor_websites(LPVOID lpParam) {
    while (1) {
        for (int i = 0; i < num_websites; i++) {
            check_website(websites[i]);
        }
        Sleep(INTERVAL * 1000);
    }
    return 0;
}

DWORD WINAPI monitor_ips(LPVOID lpParam) {
    while (1) {
        for (int i = 0; i < num_ips; i++) {
            ping_ip(ips[i]);
        }
        Sleep(INTERVAL * 1000);
    }
    return 0;
}

void cleanup(int signum) {
    printf("Cleaning up...\n");
    for (int i = 0; i < num_websites; i++) free(websites[i]);
    free(websites);
    for (int i = 0; i < num_ips; i++) free(ips[i]);
    free(ips);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <mode>\n", argv[0]);
        printf("Modes:\n");
        printf("  web  (for web monitoring)\n");
        printf("  ip   (to ping IP addresses)\n");
        printf("  all  (IP + web)\n");
        return 1;
    }

    load_database("database\\url_database.txt", &websites, &num_websites);
    load_database("database\\ip_database.txt", &ips, &num_ips);

    HANDLE hThread1 = NULL;
    HANDLE hThread2 = NULL;

    if (strcmp(argv[1], "all") == 0) {
        hThread1 = CreateThread(NULL, 0, monitor_websites, NULL, 0, NULL);
        hThread2 = CreateThread(NULL, 0, monitor_ips, NULL, 0, NULL);

        WaitForSingleObject(hThread1, INFINITE);
        WaitForSingleObject(hThread2, INFINITE);
    } else if (strcmp(argv[1], "web") == 0) {
        hThread1 = CreateThread(NULL, 0, monitor_websites, NULL, 0, NULL);
        WaitForSingleObject(hThread1, INFINITE);
    } else if (strcmp(argv[1], "ip") == 0) {
        hThread1 = CreateThread(NULL, 0, monitor_ips, NULL, 0, NULL);
        WaitForSingleObject(hThread1, INFINITE);
    } else {
        printf("Invalid mode. Use 'web', 'ip', or 'all'.\n");
        return 1;
    }

    return 0;
}

