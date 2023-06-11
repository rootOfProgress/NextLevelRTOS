import csv


def read_csv():
    start_measurement = 2
    stop_measurement = 5

    with open('digital.csv', newline='') as csvfile:
        line = 0
        start = 0
        end = 0

        measurements = csv.reader(csvfile, delimiter=',', quotechar='|')

        for row in measurements:
            if (line <= 3):
                pass
            else:
                transition = int(row[1]) << 2 | int(row[2]) << 1 | int(row[3]) 
                if (transition == 2):
                    start = float(row[0])
                elif (transition == 5):
                    end = float(row[0])
                    result = end - start
                    f = "{:10.8f}".format(result)
                    #print(f.replace('.',','))
                    print(f)
                    #print("{:10.8f}".format(result))
            line += 1

if __name__ == "__main__":
    read_csv()
