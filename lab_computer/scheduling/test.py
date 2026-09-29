import matplotlib.pyplot as plt
from matplotlib.patches import Patch

# Sample data
x = [1, 2, 3, 4, 5]
y1 = [2, 3, 5, 7, 11]
y2 = [1, 4, 6, 8, 10]

# Plotting the data
plt.plot(x, y1, label='Data 1')
plt.plot(x, y2, label='Data 2')

# Adding custom legend entries
legend_elements = [
    Patch(color='blue', label='Data 1'),
    Patch(color='orange', label='Data 2')
]

# Adding legend with custom legend entries
plt.legend(handles=legend_elements)

# Adding labels to the axes
plt.xlabel('X-axis')
plt.ylabel('Y-axis')

# Adding title
plt.title('Sample Plot with Manual Legend')

# Show the plot
plt.show()