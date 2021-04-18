# APRS raw packet grab

This script grabs in real time the raw APRS packets of a given APRS station from a popular APRS tracking website. The raw APRS packets are stored in a sqlite3 database.

usage:

```
usage: aprs_raw-packet_grab.py [-h] [--ssid SSID] call db_file web_int

This script grabs in real time the raw APRS packets of a given APRS station
from a popular APRS tracking website. The raw APRS packets are stored in a
sqlite3 database.

positional arguments:
  call         APRS station callsign (e.g., DO4AS)
  db_file      db database file name (e.g., raw_aprs_packets.db)
  web_int      webscraping interval in seconds (e.g., 120)

optional arguments:
  -h, --help   show this help message and exit
  --ssid SSID  APRS station ssid (e.g., 12)
```

example usage:

```
aprs_raw-packet_grab.py DO4AS raw_aprs_packets.db 120 --ssid 12
```

