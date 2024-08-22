#!/bin/bash

#TO ADD NEW URL OR IP, MODIFY THESE FILES   
ip_database="database/ip_database.txt"
url_database="database/url_database.txt"

#sec
interval=600
#600 sec = 10 min

# Files
log_file="logs/url.log"
ip_log_file="logs/ip.log"
status_codes_file="resources/status_codes.txt"
full_explanation="resources/code_explanation.txt"

readarray -t websites < "$url_database"
readarray -t ip < "$ip_database"


get_status_explanation() {
  local code=$1
  grep -P "^${code}\t" "$status_codes_file" | awk -F'\t' '{print $2}'


}

get_status_category() {
  local code=$1
  case ${code:0:1} in
    1) echo "Informational" ;;
    2) echo "Successful" ;;
    3) echo "Redirection" ;;
    4) echo "Client Error" ;;
    5) echo "Server Error" ;;
    *) echo "Unknown" ;;

  esac


}

S='\033[0;31m'
F='\033[0m' 



check_website() {
  local url=$1
  local response
  response=$(curl -o /dev/null -s -w "%{http_code}\n" -A "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36" "$url")
  
  local explanation
  explanation=$(get_status_explanation "$response")
  
  if [[ -z "$explanation" ]]; then
    explanation="Unknown Status"
  fi
 

 
  local category
  category=$(get_status_category "$response")
  echo "$(date): $url returned $response ($explanation) - Category: $category"

  echo "$(date): $url returned $response ($explanation) - Category: $category" >> $log_file
  
  if [[ "$response" -ge 500 ]]; then
    
    echo -e "${S}$(date): $url returned $response ($explanation) - Category: $category${F}"
    
    send_alert "$url" "$response" "$explanation"
  fi
}

send_alert() {
  echo -e "${S}Sending email, server is down${F}"
  local url=$1
  local response=$2
  local explanation=$3


# Edit based on your msmtp configuration
# see https://manpages.ubuntu.com/manpages/trusty/man1/msmtp.1.html
#
#  TO="your_email@gmail.com"
#  SUBJECT="Website Down Alert"
#  BODY="$url is down with HTTP status $response ($explanation)"
#  
#  echo -e "Subject: $SUBJECT\n\n$BODY" | msmtp --debug --from=default -t "$TO"
#
#

}

handle_user_input() {
  while true; do

    read input

    if [[ "$input" == "exit" ]]; then

      exit 0

    elif [[ "$input" =~ ^[0-9]{3}$ ]]; then
      explanation=$(get_status_explanation "$input") 
      if [[ -z "$explanation" ]]; then

        explanation="Unknown Status"
      fi
      echo 
      echo "Status Code: $input -$(grep -P "$input" "$full_explanation")"
      echo
    else

      echo -e "${S}Invalid input. Please enter a 3-digit status code.${F}"
    fi
  done
}



monitor_websites() {
  while true; do
    for website in "${websites[@]}"; do
      check_website "$website"
    done
    sleep $interval &
    wait

  done
}


ping_ips() {
  while true; do
    for i in "${ip[@]}"; do
      timestamp=$(date '+%Y-%m-%d %H:%M:%S')
      if ping -c 1 "$i" &> /dev/null; then
        echo "$timestamp $i is up"
        echo "$timestamp $i is up" >> "$ip_log_file"
      else

        echo -e "${S}$timestamp $i is down${F}"
        echo "$timestamp $i is down" >> "$ip_log_file"
        send_alert "$i" "$timestamp"
      fi

    done
    sleep $interval
  done
}



add_ip_to_database() {
  local new_ip=$1
  if grep -Fxq "$new_ip" "$ip_database"; then
    echo "IP $new_ip is already in the database."
  else
    echo "Do you want to add IP $new_ip to the database? (y/n)"
    read answer
    if [[ "$answer" == "y" ]]; then
      echo "$new_ip" >> "$ip_database"
      readarray -t ip < "$ip_database"
      echo "IP $new_ip added to the database."

    fi

  fi

}

add_url_to_database() {


  local new_url=$1
  if grep -Fxq "$new_url" "$url_database"; then

    echo "URL $new_url is already in the database."

  else
    echo "Do you want to add URL $new_url to the database? (y/n)"

    read answer

    if [[ "$answer" == "y" ]]; then

      echo "$new_url" >> "$url_database"

      readarray -t websites < "$url_database"
      echo "URL $new_url added to the database."

    fi

  fi

}



cleanup() {
  echo "Cleaning up..."
  pkill -P $$
  exit 0
}



trap cleanup SIGINT SIGTERM



# Main
if [ "$#" -eq 0 ]; then
  echo -e "${S}Usage: $0 h for help${F}"
  exit 1
fi

case "$1" in

  all)
    ping_ips &
    monitor_websites &
    handle_user_input
    ;;

  web)
    if [ "$#" -eq 2 ]; then
      add_url_to_database "$2"
    fi
    monitor_websites &
    handle_user_input
    ;;

  ip)
    if [ "$#" -eq 2 ]; then
      add_ip_to_database "$2"
    fi
    ping_ips
    ;;

  h)
    echo
    echo "./main.sh <mode>"
    echo "   web (for web monitoring)"
    echo "   ip (to ping specific IP addresses)"
    echo "   all (IP + web)"
    echo
    echo " In web/all mode, type any status error code to get more info or exit to exit"
    echo " in ip and web mode you can insert ip or url at the end to append to database"
    echo " Press Ctrl+c to stop"
    echo
    ;;

  *)
    echo -e "${S}Invalid option. Type 'h' for help.${F}"
    ;;

esac

