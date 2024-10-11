#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <curl/curl.h>
#include <time.h>
#include <windows.h>
#include <winhttp.h>


#define INTERVAL 60
#define IP_LOG_FILE "url\\ip.log"
#define URL_LOG_FILE "url\\url.log"

//convert char* to wchar_t*(>8bit len)
wchar_t *charToWChar(const char *text) {
    size_t len = strlen(text) + 1;
    wchar_t *wText = (wchar_t *)malloc(len * sizeof(wchar_t));
    mbstowcs(wText, text, len);
    return wText;
}

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

void check_website(const char *url) {
      char log_message_buf[512];
  
}

void send_alert(const char *destination, const char *subject, const char *body) {
    char command[512];
    snprintf(command, sizeof(command),
             "powershell -Command \"Send-MailMessage -To '%s' -Subject '%s' -Body '%s' -SmtpServer 'smtp.example.com'\"",
             destination, subject, body);
    system(command);
}


time_t now = time(NULL);
struct tm *t = localtime(&now);
char time_str[100];
strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

void cleanup(int signum) {
    printf("Cleaning up...\n");
    for (int i = 0; i < num_websites; i++) free(websites[i]);
    free(websites);
    for (int i = 0; i < num_ips; i++) free(ips[i]);
    free(ips);
    exit(0);
}



int main(){

  check_website("http://www.google.com");
  
  return 0;
}
