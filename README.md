# WebNetMonitor

**WebNetMonitor** is a Bash script designed to monitor the uptime of websites and the availability of network IPs. It includes features for status checks, customizable alerts, and detailed logging. This tool is ideal if you need to ensure that your web services and network components are operating smoothly.

## Features

- **Website Monitoring**: Continuously checks the status of websites and logs the responses.
- **Network IP Monitoring**: Pings IP addresses to verify their availability and logs the results.
- **Alert System**: Sends email notifications if a website or IP is down.
- **Customizable**: Easily add or remove websites and IPs to monitor.
- **Status Code Explanation**: Provides detailed explanations of HTTP status codes.
- **Logging**: Maintains logs of website and IP status for future reference.



- **Bash**: This script requires a Bash shell to execute.
- **`curl`**: For checking website statuses.
- **`ping`**: For checking IP availability.
- **`msmtp`**: For sending email alerts (or configure an alternative mail sending method).

## Installation

1. **Clone the Repository**

   ```bash
   git clone https://github.com/dani238qr/WebNetMonitor
   cd WebNetMonitor
   ```

2. **Set Up the Environment**

   Ensure you have the necessary tools installed. You can install them using your package manager. For example, on Debian-based systems:

   ```bash
   sudo apt-get install curl ping msmtp
   ```

3. **Configure Email Alerts**

   Configure `msmtp` or your preferred email sending tool to enable email notifications. Refer to the [msmtp documentation](https://marlam.de/msmtp/) for configuration details.

## Usage

The script can be executed with different modes to suit your needs. Here's how to use each mode:

### Monitor Websites

To monitor the websites listed in `url_database.txt`, run:

```bash
./main.sh web
```

### Monitor IP Addresses

To monitor the IP addresses listed in `ip_database.txt`, run:

```bash
./main.sh ip
```

### Monitor All

To monitor both websites and IP addresses, run:

```bash
./main.sh all
```

### Add a New IP or URL

To add a new IP address or URL to the respective databases, use the following commands:

**Add IP:**

```bash
./main.sh ip <IP_ADDRESS>
```

**Add URL:**

```bash
./main.sh web <URL>
```

You will be prompted to confirm if you want to add the new IP address or URL to the database.

### View Status Code Explanations

To get explanations for HTTP status codes while in `web` or `all` mode, type the status code into the terminal. Type `exit` to stop the script.

## Configuration

- **IP Database**: `ip_database.txt`
- **URL Database**: `url_database.txt`
- **Log Files**: `logs/website_monitor.log`, `logs/ip_monitor.log`
- **Status Codes File**: `resources/status_codes.txt`
- **Code Explanations File**: `resources/code_explanation.txt`

Add your IP addresses and URLs to `ip_database.txt` and `url_database.txt`.

## Troubleshooting

- **No Email Alerts**: Verify that `msmtp` or your email tool is properly configured.
- **Script Not Executing**: Ensure the script has execute permissions. You can add execute permissions with:

  ```bash
  chmod +x main.sh
  ```

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.


