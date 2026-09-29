import ROOT
import numpy as np
print(np.__version__)
import ephem
print(ephem.__version__)
import matplotlib
import matplotlib.pyplot as plt
print(matplotlib.__version__)
import cv2
print(cv2.__version__)
import astropy
from astropy.coordinates import SkyCoord
from astropy import units as u
from astropy.time import Time
from astropy.coordinates import FK5, EarthLocation, AltAz, ICRS
print(astropy.__version__)
import pandas as pd

# Load the ROOT file and retrieve the sky acceptance histogram
root_file = ROOT.TFile("PointSourceAcceptance_FoV3.8deg_Index2.0_Emin6.0_Emax10.0.root")
sky_acceptance = root_file.Get("skyAcceptance")

# Telescope pointing settings
telescope_az = 280.0  # degrees
telescope_el = -1.56  # degrees below horizon



# Define telescope observer at Frisco Peak, Utah
observer = ephem.Observer()
observer.lon = '-113.2875'  # Longitude (West is negative)
observer.lat = '38.52028'   # Latitude
observer.elev = 2935        # Elevation in meters

# Event times (replace with actual UTC times)
event_times = ["2025-02-20 05:40:10", "2025-02-25 07:13:58", "2025-02-26 05:26:38", "2025-02-26 06:18:59"]



def get_max_acceptance(hist):
    max_value = -float('inf')  # Initialize with a very low value
    max_bin_x = -1
    max_bin_y = -1
    
    # Loop through all bins in the histogram
    for i in range(1, hist.GetNbinsX() + 1):  # X bins are 1-based
        for j in range(1, hist.GetNbinsY() + 1):  # Y bins are 1-based
            bin_content = hist.GetBinContent(i, j)
            if bin_content > max_value:  # Find the maximum bin content
                max_value = bin_content
                max_bin_x = i
                max_bin_y = j
    
    # Get the azimuth and elevation corresponding to the maximum bin
    max_az = hist.GetXaxis().GetBinCenter(max_bin_x)
    max_el = hist.GetYaxis().GetBinCenter(max_bin_y)
    
    # Debugging: Print the maximum values and bin positions
    print(f"max_az = {max_az}, max_el = {max_el}")
    
    return max_az, max_el

import math

# Function to integrate acceptance map to find 68% confidence contour, centered around max position
# using OpenCV's contour-finding algorithm to detect the border of the 68% confidence region in a 2D histogram
def find_68_contour(hist, max_az, max_el):
    # Convert histogram to a numpy array
    bins_x = hist.GetNbinsX()
    bins_y = hist.GetNbinsY()

    #initialize a NumPy array, acceptance_map, with zeros. This array will represent the histogram values in a grid format that OpenCV can work with.
    acceptance_map = np.zeros((bins_y, bins_x))  # OpenCV uses (rows, cols) = (Y, X)
    bin_positions = {}  # Store bin center positions
    total_acceptance = 0

    # Fill the array with acceptance values
    for i in range(1, bins_x + 1):
        for j in range(1, bins_y + 1):
            bin_content = hist.GetBinContent(i, j)
            acceptance_map[j-1, i-1] = bin_content  # OpenCV uses (row, col)
            bin_positions[(j-1, i-1)] = (hist.GetXaxis().GetBinCenter(i), hist.GetYaxis().GetBinCenter(j))
            total_acceptance += bin_content

    # Sort all bins by acceptance value
    flat_values = acceptance_map.flatten()
    sorted_indices = np.argsort(flat_values)[::-1]  # Descending order

    # Determine the threshold value that accumulates 68% of total acceptance
    accumulated_acceptance = 0
    threshold_value = None
    for idx in sorted_indices:
        accumulated_acceptance += flat_values[idx]
        if accumulated_acceptance >= 0.68 * total_acceptance:
            threshold_value = flat_values[idx]
            break

    # Create a binary mask for the 68% region
    binary_mask = (acceptance_map >= threshold_value).astype(np.uint8)

    # Find contours using OpenCV
    contours, _ = cv2.findContours(binary_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # Extract the largest contour (main 68% region)
    if len(contours) == 0:
        return []

    largest_contour = max(contours, key=cv2.contourArea)  # Select the largest contour

    # Convert contour points back to (azimuth, elevation)
    contour_points = [bin_positions.get((p[0][1], p[0][0]), None) for p in largest_contour]
    contour_points = [p for p in contour_points if p is not None]  # Remove None values

    return contour_points


# Function to integrate acceptance map to find 90% confidence contour, centered around max position
# using OpenCV's contour-finding algorithm to detect the border of the 90% confidence region in a 2D histogram
def find_90_contour(hist, max_az, max_el):
    # Convert histogram to a numpy array
    bins_x = hist.GetNbinsX()
    bins_y = hist.GetNbinsY()

    #initialize a NumPy array, acceptance_map, with zeros. This array will represent the histogram values in a grid format that OpenCV can work with.
    acceptance_map = np.zeros((bins_y, bins_x))  # OpenCV uses (rows, cols) = (Y, X)
    bin_positions = {}  # Store bin center positions
    total_acceptance = 0

    # Fill the array with acceptance values
    for i in range(1, bins_x + 1):
        for j in range(1, bins_y + 1):
            bin_content = hist.GetBinContent(i, j)
            acceptance_map[j-1, i-1] = bin_content  # OpenCV uses (row, col)
            bin_positions[(j-1, i-1)] = (hist.GetXaxis().GetBinCenter(i), hist.GetYaxis().GetBinCenter(j))
            total_acceptance += bin_content

    # Sort all bins by acceptance value
    flat_values = acceptance_map.flatten()
    sorted_indices = np.argsort(flat_values)[::-1]  # Descending order

    # Determine the threshold value that accumulates 68% of total acceptance
    accumulated_acceptance = 0
    threshold_value = None
    for idx in sorted_indices:
        accumulated_acceptance += flat_values[idx]
        if accumulated_acceptance >= 0.9 * total_acceptance:
            threshold_value = flat_values[idx]
            break

    # Create a binary mask for the 90% region
    binary_mask = (acceptance_map >= threshold_value).astype(np.uint8)

    # Find contours using OpenCV
    contours, _ = cv2.findContours(binary_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # Extract the largest contour (main 90% region)
    if len(contours) == 0:
        return []

    largest_contour = max(contours, key=cv2.contourArea)  # Select the largest contour

    # Convert contour points back to (azimuth, elevation)
    contour_points = [bin_positions.get((p[0][1], p[0][0]), None) for p in largest_contour]
    contour_points = [p for p in contour_points if p is not None]  # Remove None values

    return contour_points



# Function to convert azimuth-elevation to RA-Dec using ephem
def azel_to_radec_ephem(az, el, event_time, observer):
    observer.date = event_time
    alt = el  # Convert elevation to altitude
    az = float(ephem.degrees(str(az)))
    alt = float(ephem.degrees(str(alt)))
    ra, dec = observer.radec_of(az, alt)
    return np.degrees(ra), np.degrees(dec)

# Function to convert azimuth-elevation to RA-Dec using astrophy
# Function to convert azimuth-elevation to RA-Dec using Astropy
def azel_to_radec_astropy(az, el, event_time, observer):
    # Convert observer's location to Astropy EarthLocation
    observer_location = EarthLocation(lon=-113.2875*u.deg, lat=38.52028*u.deg, height=2935*u.m)

    # Convert event time to Astropy Time (force UTC)
    time = Time(event_time)

    # Create AltAz frame for given azimuth and elevation
    altaz_frame = AltAz(obstime=time, location=observer_location)

    # Convert azimuth and elevation to SkyCoord in AltAz frame
    altaz_coords = SkyCoord(az=az*u.deg, alt=el*u.deg, frame=altaz_frame)
    
    # Transform to FK5 (RA, Dec) coordinates
    sky_coords = altaz_coords.transform_to(FK5)
    
    # Return RA, Dec in degrees
    return sky_coords.ra.deg, sky_coords.dec.deg


# Function to convert RA-Dec to Galactic coordinates, considering the date and time
def radec_to_galactic(ra, dec, event_time):
    # Convert the input date and time to a Time object
    time = Time(event_time)
    
    # Create a SkyCoord object with the specific epoch
    sky_coord = SkyCoord(ra=ra, dec=dec, unit=(u.deg, u.deg), frame='fk5', obstime=time)

    # Convert to Galactic coordinates
    galactic_coord = sky_coord.galactic

    return galactic_coord.l.deg, galactic_coord.b.deg

def max_abs_difference(arr, value):
    max_diff = 0
    for num in arr:
        diff = abs(num - value)
        if diff > max_diff:
            max_diff = diff
    return max_diff
    
# Process each event
results_radec = []
results_azel = []
results_galactic = []

max_az, max_el = get_max_acceptance(sky_acceptance)

for event_time in event_times:
    
    ra, dec = azel_to_radec_ephem(max_az+telescope_az, max_el+telescope_el, event_time, observer)
    #ra, dec = azel_to_radec_astropy(max_az+telescope_az, max_el+telescope_el, event_time, observer)
    l, b = radec_to_galactic(ra, dec, event_time)
    print(event_time, ra, dec)
    # Convert 68% confidence region points to RA-Dec
    contour_azel = find_68_contour(sky_acceptance, max_az, max_el)
    contour_radec = [azel_to_radec_ephem(az+telescope_az, el+telescope_el, event_time, observer) for az, el in contour_azel]
    #contour_radec = [azel_to_radec_astropy(az+telescope_az, el+telescope_el, event_time, observer) for az, el in contour_azel]
    contour_galactic = [radec_to_galactic(ra, dec, event_time) for ra, dec in contour_radec]
    
    contour_90_azel = find_90_contour(sky_acceptance, max_az, max_el)
    contour_90_radec = [azel_to_radec_ephem(az+telescope_az, el+telescope_el, event_time, observer) for az, el in contour_90_azel]
    
    results_radec.append((event_time, ra, dec, contour_radec, contour_90_radec))
    results_azel.append((event_time, max_az, max_el, contour_azel))
    results_galactic.append((event_time, l, b, contour_galactic))

# Plot results in az, el
plt.figure(figsize=(8, 6))
for event_time, az, el, contour_azel in results_azel:
    contour_az, contour_el = zip(*contour_azel)
    plt.plot(np.asarray(contour_az)+telescope_az, np.asarray(contour_el)+telescope_el, linestyle='dashed', zorder=50)
    plt.scatter(az+telescope_az, el+telescope_el, label=f"Event @ {event_time}", marker='o', s=100, edgecolors='black', zorder=100)
       
    break

plt.xlabel("Azimuth (deg)")
plt.ylabel("Elevation (deg)")
plt.title("68% Confidence Region around the acceptance maximum")
plt.savefig("candidates_origins_azel.png", dpi=300, bbox_inches='tight')  # Save the plot
plt.close()

# Plot results in ra, dec
plt.figure(figsize=(8, 6))



for idx, (event_time, ra, dec, contour_radec, contour_90_radec) in enumerate(results_radec):
    contour_ra, contour_dec = zip(*contour_radec)
    contour_90_ra, contour_90_dec = zip(*contour_90_radec)
    
    # Plotting (if needed)
    plt.figure()
    plt.plot(contour_ra, contour_dec, linestyle='dashed', zorder=50)
    plt.scatter(ra, dec, label=f"Event @ {event_time}", marker='o', s=100, edgecolors='black', zorder=100)
    plt.title(f"Event {idx + 1}")
    plt.xlabel('RA')
    plt.ylabel('Dec')
    plt.legend()
    plt.grid(True)
    plt.savefig(f"event_{idx + 1}_{event_time}_{ra}_{dec}.png")  # Save plot as PNG
    
    # Saving data as CSV
    data = {'contour_ra': contour_ra, 'contour_dec': contour_dec}
    df = pd.DataFrame(data)
    csv_filename = f"event_{idx + 1}_{event_time}_{ra}_{dec}.csv"
    df.to_csv(csv_filename, index=False)
    
    # Print for verification (optional)
    print(f"Saved CSV file: {csv_filename}")
    print(event_time)
    print("RA = ",ra, ", Dec = ", dec)
    print("RA 90% region: {} +- {}".format(ra, max_abs_difference(contour_90_ra, ra)))
    print("Dec 90% region: {} +- {}".format(dec, max_abs_difference(contour_90_dec, dec)))

plt.xlabel("Right Ascension (deg)")
plt.ylabel("Declination (deg)")
plt.legend(loc='lower left')
plt.title("Event Source Localization with 68% Confidence Regions")
plt.savefig("candidates_origins_radec.png", dpi=300, bbox_inches='tight')  # Save the plot
plt.close()

# Plot results in galactic coordinates
plt.figure(figsize=(8, 6))
for event_time, l, b, contour_galactic in results_galactic:
    contour_l, contour_b = zip(*contour_galactic)
    plt.plot(contour_l, contour_b, linestyle='dashed', zorder=50)
    plt.scatter(l, b, label=f"Event @ {event_time}", marker='o', s=100, edgecolors='black', zorder=100)

plt.xlabel("Galactic Longitude (deg)")
plt.ylabel("Galactic Latitude (deg)")
plt.legend(loc='lower left')
plt.title("Event Source Localization with 68% Confidence Regions (Galactic Coordinates)")
plt.savefig("candidates_origins_galactic.png", dpi=300, bbox_inches='tight')  # Save the plot
plt.close()
