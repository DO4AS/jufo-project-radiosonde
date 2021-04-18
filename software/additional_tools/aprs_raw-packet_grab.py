#
# This script grabs in real time the raw APRS packets of a given APRS station from a popular APRS tracking website.
# The raw APRS packets are stored in a sqlite3 database.
# script by Amon Schumann / DO4AS
#

import requests
from bs4 import BeautifulSoup

import argparse

from threading import Event
import signal
import sys

import sqlite3

import time

# exit event
exit = Event()

# initialize argument parser
parser = argparse.ArgumentParser(description = "This script grabs in real time the raw APRS packets of a given APRS station from a popular APRS tracking website.\
                                                The raw APRS packets are stored in a sqlite3 database.")

# positional arguments
parser.add_argument("call", help="APRS station callsign (e.g., DO4AS)")
parser.add_argument("db_file", help="db database file name (e.g., raw_aprs_packets.db)")
parser.add_argument("web_int",  help="webscraping interval in seconds (e.g., 120)", type=int)

# optional argument
parser.add_argument("--ssid", help="APRS station ssid (e.g., 12)", type=int)

# parse arguments
args = parser.parse_args()

if (args.ssid):
    # APRS station callsign with ssid
    aprs_tracking_website_raw_data_url = ("https://www.aprsdirect.com/details/raw/name/{aprs_station_callsign}-{aprs_station_ssid}").format(aprs_station_callsign = args.call, aprs_station_ssid = args.ssid)
else:
    # APRS station callsign without ssid
    aprs_tracking_website_raw_data_url = ("https://www.aprsdirect.com/details/raw/name/{aprs_station_callsign}").format(aprs_station_callsign = args.call)

db_database_file_name = args.db_file
webscraping_interval = args.web_int

if (args.ssid):
    # APRS station callsign with ssid
    db_table_name = ("{aprs_station_callsign}_{aprs_station_ssid}").format(aprs_station_callsign = args.call, aprs_station_ssid = args.ssid)
else:
    # APRS station callsign without ssid
    db_table_name = ("{aprs_station_callsign}").format(aprs_station_callsign = args.call)


print("start APRS packet parser with:")
for arg in vars(args):
    print (arg, getattr(args, arg))

def main():
    while True:
        try:
            # get raw packets from a popular APRS tracking website
            page = requests.get(aprs_tracking_website_raw_data_url)

            soup = BeautifulSoup(page.content, "html.parser")
            raw_aprs_packets = soup.find(id= "rawtab-output")

            # clean up raw output - remove empty rows
            raw_aprs_packets_list = [y for y in (x.strip() for x in raw_aprs_packets.get_text().splitlines()) if y]

            # remove warning label "[Duplicate]"
            warning_label_remove_loop_counter = 0
            raw_aprs_packets_list_adjusted_length = len(raw_aprs_packets_list)
            while (warning_label_remove_loop_counter < raw_aprs_packets_list_adjusted_length):
                if (raw_aprs_packets_list[warning_label_remove_loop_counter] == "[Duplicate]"):
                    del raw_aprs_packets_list[warning_label_remove_loop_counter]
                    raw_aprs_packets_list_adjusted_length = len(raw_aprs_packets_list)

                warning_label_remove_loop_counter = warning_label_remove_loop_counter + 1

            aprs_packets = []
            print(raw_aprs_packets_list)
            # sort raw packets with timestamp in 2d list
            for c in range (0, len(raw_aprs_packets_list), 2):
                aprs_packets.append ([int (raw_aprs_packets_list[c].strip(":")), raw_aprs_packets_list[c+1]])

            # create db table - if the table does not exist
            db_cursor.execute("""CREATE TABLE IF NOT EXISTS {table_name} (time timestamp, raw_aprs_packet text, primary key (time))""".format(table_name = db_table_name))

            for c in range (len(aprs_packets)):
                # replace " with "" and ' with '' before inserting in database
                aprs_packets[c][1] = aprs_packets[c][1].replace("\"", "\"\"").replace("\'", "\'\'")

                # insert raw packets with timestamp into db table - if the dataset does not exist
                db_cursor.execute("""INSERT OR REPLACE INTO {table_name} VALUES ({time}, "{raw_aprs_packet}")""".format(table_name = db_table_name, time = aprs_packets[c][0], raw_aprs_packet = aprs_packets[c][1]))
                db_connection.commit()

            # wait for next webscraping
            time.sleep(webscraping_interval)

        except Exception as e:
            print("error detected:", e)

def exit_programm(sig, _frame):
    print("Interrupted by {int_sig}, shutting down".format(int_sig = sig))

    # close sqlite3 database
    db_cursor.close()
    db_connection.close()

    # exit porgramm
    sys.exit()

if __name__ == "__main__":
    # connect to sqlite database
    db_connection = sqlite3.connect(db_database_file_name)
    db_cursor = db_connection.cursor()

    signal.signal(signal.SIGINT, exit_programm)

    main()