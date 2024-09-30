#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define INTERVAL 60
#define IP_LOG_FILE "url\\ip.log"
#define URL_LOG_FILE "url\\url.log"

void check_website(const char *url) {
      char log_message_buf[512];
  
}

time_t now = time(NULL);
struct tm *t = localtime(&now);
char time_str[100];
strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

int main(){

  check_website("http://www.google.com");
  
  return 0;
}
