#!/usr/bin/env python3
##########################################################
from influxdb import InfluxDBClient
import pandas as pd
import numpy as np
import glob
import os
import time
from datetime import datetime
import multiprocessing
import csv
from astropy.io import ascii


import decodeT as de

def log_file(message):
    current_time = time.time()
    current_time = time.ctime(current_time)
    with open("/data/TrinityLabComputer/Database/ctcpu_state_mess/sm_upload.log", "a") as file:
        file.write(message + ' ' + str(current_time) + ' \n')


# gets all the files from the weather database folder
# Takes: a folder_path
# Returns: array of files paths
def get_filenames(folder_path):
    arr_filenames = glob.glob(wd_path + '//*')
    return arr_filenames

# gets the last file from the array of files
# Takes: a array of filenames
# Returns: gets the last file path in the list of file paths
def get_last_file(arr_filenames):
    last_file = min(arr_filenames)
    #print(last_file)
    return last_file

# converts filename to measurement name for database
# Takes: a single file path
# returns just the 8 digit YYYYMMDD
def cov_filename_ment(file):
    ment = file[-8:]
    #print(ment)
    return ment



# # convert time to datatime object for continuous upload
# # takes: a pandas dataframe
# # check if this is even used
# def time_epoch_ms(df):
#     df['DateTime'] = pd.to_datetime(df['DateTime'], format='%Y-%m-%dT%H:%M:%S.%f')
#     #df['DateTime'] = df['DateTime'].astype('int64')
#     #print(df['DateTime'])

# def make_df(file):
#     file = file + 'State_MSG_LOG_230920.txt'
#     #header = ['time','t_time','hex']
#     #df = pd.read_csv(file, header=None, names=['Whole_line'])

#     df = pd.read_csv(file, delimiter = " \|\| ",names= ['time','t_time','hex'])
#     print(df.dtypes)
#     #df['hex'] = df['hex'].apply(lambda x: hex(x))
# 	#data_table = ascii.read(file, format='csv')

# 	#df = pd.read_csv(file, dtype=object ,delim_whitespace=True,quotechar='\'',error_bad_lines=False, warn_bad_lines=True)
#     return df

# def get_last_message():

#     df=make_df(wd_path)
#     print(df)
#     last = len(df)
#     message = df.loc[last-1]
#     message = message['hex']
#     print(message)
#     return message


def get_array(m_hex):
    LENGTHS = [2,2,4,8,4,4,8,8,8,8,8,8,8,8,8,8,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8]
    results = []
    start = 0
    #print(m_hex)
    for length in LENGTHS:
        hex_element = m_hex[start:start+length]
        #rint(hex_element)
        decimal_element = int(hex_element, 16)

        if decimal_element != 67: # if it is the letter c
            results.append(decimal_element)
        else:
            char_element = chr(decimal_element)
            results.append(char_element)

        start += length

    #print(results)
    return results

# message put into binary
def get_last_message(fpath):

    with open(fpath, 'rb') as file:

        file.seek(-180, 2) 

        last_104_bytes = file.read()
        #print(last_104_bytes) # use this if you want to see the last 64 bytes
        hex_string = last_104_bytes.hex()
        #print('     ')
        #print(hex_string)

        dec_array=get_array(hex_string)
    return dec_array


 



if __name__ == '__main__':

    ################# Location of weather data ################

    wd_path = '/data/TrinityLabComputer/Database/ctcpu_state_mess/Log/'

    ###########################################################


    ################# Database intinitation ###################

    # Local host lines for access
    host = 'localhost'
    port = 8086
    #username = 'mpotts32'
    username = 'admin'
    password = 'Ttys@210'
    database = 'dbSM' # Database for independent measurements for each day, need to change the measurent line in cre_df_list()
    #database = 'Trinity1'

    # Initialize the InfluxDB client and write the points in batches
    client = InfluxDBClient(host = host, port=port, username=username, password=password)

    # Create a new database if it does not already exist
    # client.create_database(database)

    # Switch to the newly created database
    client.switch_database(database)

    



    #message = b'CT\x1fisb\x00\x00d&\t\xa2\x00\x8d\x00b\t[\r\xc9\to\x00\x8b\xff\xff\xff\xff\x00\x00\x00\x00\x003p\x7f\x003\x96|\x003\xa3\xe1\x003\xd1\xfa\x003l\xdf\x003\x91M\x003\xa0H\x003\xd1\xd1\r\r\r\r\r\r\r\rWZV[UYWYZ[XZTXWWSWZOZLZZWYXYZWVX'
    #message = b'CT?222\x00Ac\xf3\xa2\xb1\x00\x00\x00\x00\tQ\x0eL\te\x00\x00\x7f\xff\xff\xff\x7f\xff\xff\xff\x005\xe4\xb2\x006\x071\x006$q\x006U+\x005\xdf#\x006\x03\x18\x006\x13 \x006A\xf9\x1b\x15\x15\x16\x14\x1b\x18\x12|~||}{zy~\x82\x82\x83\x83\x80\x80|\x7f\x83\x85\x87\x88\x85\x81||\x80\x83\x88\x84\x81\x7f\x00'
    message_prior = ""
    while True: 
        # try:
        file = get_last_file(get_filenames(wd_path))
        #print(file)
        current_file = cov_filename_ment(file)
        log_file(f'Last line of {current_file} being read')
        #print(f'Last line of {current_file} being read')

        message = get_last_message(file)
        #print(message)
        #message = b'CT\x1fisb\x00\x00d&\t\xa2\x00\x8d\x00b\t[\r\xc9\to\x00\x8b\xff\xff\xff\xff\x00\x00\x00\x00\x003p\x7f\x003\x96|\x003\xa3\xe1\x003\xd1\xfa\x003l\xdf\x003\x91M\x003\xa0H\x003\xd1\xd1\r\r\r\r\r\r\r\rWZV[UYWYZ[XZTXWWSWZOZLZZWYXYZWVX'
        #print(f'message before: {message}')
        if message_prior != message:
            df_upload =de.main(message)

        message_prior = message
        
        time.sleep(10) # sleep for 10 seconds
        log_file(f'Waiting 10 seconds')

        #break

        # except Exception as e:
            
        #     print(f'Error Uploading: last set of hex data at time {time.time()}')
        #     # maybe make this a file in the future
        #     time.sleep(60) # the system waits a minute to continue
