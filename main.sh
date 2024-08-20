#!/bin/bash

# Files
status_codes_file="resources/status_codes.txt"
full_explanation="resources/code_explanation.txt"


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



# Main
if [ "$#" -eq 0 ]; then
  echo -e "${S}Usage: $0 h for help${F}"
  exit 1
fi

case "$1" in

  all)
    echo "ip and url"
    #functions call test
    get_status_explanation(200);
    get_status_category(404);
    ;;

  web)
    echo "url"
    ;;

  ip)
    echo "ip"
    ;;

  h)
    echo
    echo " web (for web monitoring)"
    echo " ip (to ping specific IP addresses)"
    echo " all (IP + web)"
    echo " Press Ctrl+c to stop"
    echo
    ;;

  *)
    echo  "Invalid option. Type 'h' for help."
    ;;

esac


