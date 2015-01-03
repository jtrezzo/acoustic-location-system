#import matplotlib for plotting
import matplotlib.pyplot as plt

# Some constants
DEBUG = False

# Open the file and create a file object
fileObject = open('points.txt', 'r')

# Count the number of lines in the txt document
nbrLines = 0
for line in fileObject:
	nbrLines += 1
print ('the number of lines is %i \n', nbrLines)

# some constants
firstValidData = 2
lastValidData = nbrLines-1

#Pointer is sitting at the end of the file, re-open the file
fileObject = open('points.txt', 'r')

# Main function to create the list of data
lineControl = 0
dataList = []
for line in fileObject:
	if lineControl > firstValidData and lineControl < lastValidData:
		if DEBUG:
			print line
		c = line
		b = dataList
		dataList.extend(line.strip().split(","))
		a = dataList
	lineControl += 1

# Remove the empty elements
while ""  in dataList: dataList.remove("")

# Plot the data and add x/y labels
plt.plot(dataList)
plt.xlabel('Time')
plt.ylabel('ADC Samples')

# Show the plot
plt.show()

#Close the file
fileObject.close()