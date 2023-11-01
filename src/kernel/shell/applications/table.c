#include "../include/table.h"

void printHorizontalLine(int totalWidth) {
    for (int i = 0; i < totalWidth; i++) 
        putc('-');
    
    putc('\n');
}

void printTable(int numRows, int numColumns, const char* tableData[], int columnWidth) {
    putc('\n');

    // Calculate the total width of the table, including separators and padding.
    int totalWidth = (columnWidth + 3) * numColumns + 1;  // +1 for the vertical line between columns

    // Function to print a horizontal line across the table
    void printHorizontalLine() {
        for (int i = 0; i < totalWidth; i++) 
            putc('-');

        putc('\n');
    }

    // Print the table header with vertical lines
    printHorizontalLine(totalWidth);
    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numColumns; col++) {
            const char* cellData = tableData[row * numColumns + col];
            int dataLength = strlen(cellData);

            // If the data is longer than the column width, truncate it.
            if (dataLength > columnWidth) 
                dataLength = columnWidth;

            // Print the data with padding
            putc('|');
            for (int i = 0; i < dataLength; i++) 
                putc(cellData[i]);

            for (int i = dataLength; i < columnWidth; i++) 
                putc(' ');  // Add padding to ensure column alignment.
        }

        putc('|');  // Add a vertical line at the end of the row.
        putc('\n');

        // Print a separator line between the header and the data
        if (row == 0) 
            printHorizontalLine(totalWidth);
    }

    // Print the final horizontal line
    printHorizontalLine(totalWidth);
}
