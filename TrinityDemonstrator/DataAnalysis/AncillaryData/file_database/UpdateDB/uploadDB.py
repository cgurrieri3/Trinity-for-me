import sys
import subprocess
from datetime import datetime, timedelta
import argparse
import userUpdateDatabase

def BacklogDBupload(computer_node, start_date,days,database):
    try:
        start_dt = datetime.strptime(start_date, "%Y%m%d")
    except ValueError:
        print("Invalid date format. Use YYYYMMDD.")
        sys.exit(1)

    for d in range(days + 1):  # Include the end date
        current_date = (start_dt + timedelta(days=d)).strftime("%Y%m%d")
        print(f"./loadDB.sh {computer_node} {current_date} {database}")
        subprocess.run(["./loadDB.sh", computer_node, current_date, database], check=True)
        print("Process Complete")

def DailyDBupload(computer_node, database):
    current_date = datetime.now()
    current_date = datetime.strftime(current_date, "%Y%m%d")
    userInput = "n"
    # do all the .sql file creation
    while userInput != "y":
        userUpdateDatabase.main()
        userInput=input("Confirm proper formatting on .sql files? y or n:")

    # load using ./loadDB.sh
    print(f"./loadDB.sh {computer_node} {current_date} {database}")
    subprocess.run(["./loadDB.sh", computer_node, current_date, database], check=True)
    print("Process Complete")

def main():

    # Initialize parser
    parser = argparse.ArgumentParser()

    # Adding optional argument
    parser.add_argument("-n", "--node", help = "Computer Node from job ex. atl1-1-01-015-32-2")
    parser.add_argument("-d", "--database", help = "database you are entering data into")
    parser.add_argument("-st", "--start_date", help = "start date for backlogging already completed sql files")
    parser.add_argument("-in", "--i", help = "Number of days from start for backlogging already completed sql files")
    args = parser.parse_args()

    computer_node = args.node
    database = args.database
    start_date = args.start_date
    days = args.i

    if days == '' or days == None:
        DailyDBupload(computer_node, database)
    else:
        days = int(days)
        BacklogDBupload(computer_node, start_date,days,database)    

    
if __name__ == "__main__":
    main()