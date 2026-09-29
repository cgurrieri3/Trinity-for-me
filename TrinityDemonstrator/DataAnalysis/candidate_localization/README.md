Event Localization with Sky Acceptance Map

This repository contains a Python script to process telescope sky acceptance data and localize events in the sky. It uses a 2D sky acceptance histogram to find the maximum acceptance position and calculate the 68% confidence contour region. The script then converts the contour points from azimuth-elevation coordinates to Right Ascension-Declination (RA-Dec) and Galactic coordinates. Finally, the script generates and saves plots of the localization results in three coordinate systems: Azimuth-Elevation, RA-Dec, and Galactic.
Requirements

    Ubuntu 22.04 (WSL)
    Python 3.10.12
    ROOT (for handling the sky acceptance histogram)
      ROOT Version: 6.28/06
      Built for linuxx8664gcc on Mar 05 2025, 22:00:00
      From heads/latest-stable@7745d36d
    NumPy 1.26.2
    Matplotlib 3.8.1    
    Astropy 5.3.4
    Ephem 4.2
    OpenCV, CV2 (for contour detection) 4.11.10

You can install the required packages using the following:

    pip install numpy matplotlib opencv-python astropy ephem cv2

Script Overview
Input

    A ROOT file (PointSourceAcceptance_FoV3.8deg_Index2.0_Emin6.0_Emax10.0.root) containing the sky acceptance data.
    A list of event times to analyze. The event times are in UTC and can be updated to match your specific dataset.

Output

The script generates three plots:

    Azimuth-Elevation Plot: The localization of events with the 68% confidence region around the maximum acceptance position in azimuth and elevation coordinates.
    RA-Dec Plot: The localization of events with the 68% confidence region in Right Ascension and Declination coordinates.
    Galactic Coordinates Plot: The localization of events with the 68% confidence region in Galactic coordinates.

These plots are saved as images with the following filenames:

    candidates_origins_azel.png
    candidates_origins_radec.png
    candidates_origins_galactic.png

Functions

    get_max_acceptance(hist): Finds the maximum acceptance position in the sky acceptance histogram.
    find_68_contour(hist, max_az, max_el): Finds the 68% confidence contour using OpenCV's contour-finding algorithm.
    find_90_contour(hist, max_az, max_el): Finds the 90% confidence contour using OpenCV's contour-finding algorithm.
    azel_to_radec_astropy(az, el, event_time, observer): Converts azimuth and elevation coordinates to RA and Dec using the astropy library. 
    azel_to_radec_ephem(az, el, event_time, observer): Converts azimuth and elevation coordinates to RA and Dec using the ephem library
    radec_to_galactic(ra, dec, event_time): Converts RA-Dec to Galactic coordinates.
    max_abs_difference(arr, value): finds the maximum absolute difference between each element in an array and a given value

How to Use

    Ensure that you have all required packages installed and the ROOT file is available.
    Modify the event_times list to include the actual event times you want to analyze.
    Run the script:

        python candidate_localization.py

    The script will generate and save the plots in the current working directory.

Example Output

    Azimuth-Elevation Plot: This plot shows the event source localization with the 68% confidence region around the acceptance maximum in azimuth and elevation coordinates.

    RA-Dec Plot: This plot shows the event source localization with the 68% confidence region in Right Ascension and Declination coordinates.

    Galactic Coordinates Plot: This plot shows the event source localization with the 68% confidence region in Galactic coordinates.
