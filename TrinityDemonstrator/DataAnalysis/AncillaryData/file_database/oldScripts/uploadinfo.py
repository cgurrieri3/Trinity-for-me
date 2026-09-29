import os
import csv


    

# Define input and output files
input_csv = 'test_data.csv'  # Your input CSV file
input_csv = '/storage/hive/project/phy-otte/shared/Trinity/DataAnalysis/DataCalibration/AncillaryData/Data2/Data2.csv'
output_sql = 'insert_queries.sql'  # Output SQL file


columns = ["Timestamp", "TriggerRate", "HV1", "HV2" ,"HV3", "HV4", "HVCur1", "HVCur2", "HVCur3", "HVCur4", "UCTemp1", "UCTemp10", "UCTemp11", "UCTemp12", "UCTemp13", "UCTemp14", "UCTemp15", "UCTemp16", "UCTemp2", "UCTemp3", "UCTemp4", "UCTemp5", "UCTemp6", "UCTemp7", "UCTemp8", "UCTemp9", "MUSICMPWR1", "MUSICPWR10", "MUSICPWR11", "MUSICPWR12", "MUSICPWR13", "MUSICPWR14", "MUSICPWR15", "MUSICPWR16", "MUSICPWR2", "MUSICPWR3", "MUSICPWR4", "MUSICPWR5", "MUSICPWR6", "MUSICPWR7", "MUSICPWR8", "MUSICPWR9", "HVSW1", "HVSW10", "HVSW11", "HVSW12", "HVSW13", "HVSW14", "HVSW15", "HVSW16", "HVSW2", "HVSW3", "HVSW4", "HVSW5", "HVSW6", "HVSW7", "HVSW8", "HVSW9", "SiPMTemp1", "SiPMTemp10", "SiPMTemp11", "SiPMTemp12", "SiPMTemp13", "SiPMTemp14", "SiPMTemp15", "SiPMTemp16", "SiPMTemp2", "SiPMTemp3", "SiPMTemp4", "SiPMTemp5", "SiPMTemp6", "SiPMTemp7", "SiPMTemp8", "SiPMTemp9", "ASADCurrent", "SIABcurr1", "SIABcurr10", "SIABcurr11", "SIABcurr12", "SIABcurr13", "SIABcurr14", "SIABcurr15", "SIABcurr16", "SIABcurr2", "SIABcurr3", "SIABcurr4", "SIABcurr5", "SIABcurr6", "SIABcurr7", "SIABcurr8", "SIABcurr9", "TBCurr"]


# Open the input CSV file for reading
with open(input_csv, mode='r') as file:
    #reader = csv.DictReader(file)  # Using DictReader to handle CSV columns by name
    reader = csv.reader(file)
    with open(output_sql, mode='w') as sql_file:
        # Loop over each row in the CSV file
        for row in reader:
            row = dict(zip(columns, row))
            # Generate the INSERT statement for each row
            sql = (
                f"INSERT INTO StateMessages (id, name, age, city, town) "
                f"VALUES ('{row['Timestamp']}', '{row['TriggerRate']}', '{row['HV1']}', '{row['HV2']}', '{row['HV3']}');\n"
            )
            # Write the generated SQL statement to the output file
            sql_file.write(sql)

print(f"SQL queries have been written to {output_sql}")


# User input - date, files, qualitative conditions
# default values

# folderDate=input("What data night?: YYYYMMDD ")

# fileStart=input("File start: HHMMSS")
# fileEnd=input("File end: HHMMSS")

# infor grab - moon data, weather data, data summary info
# every table needs a file

# build tables data info, weather, moon, ranking, data processeing

# create outputfile to upload to database