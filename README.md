# Challenge Overview
This was a backend coding challenge I completed late Fall 2019. The README I submitted for the challenge starts at [Overview of Code](#overview-of-code)

## Instructions
Please submit a zip archive containing your code and any relevant materials. This should include a README file that contains instructions about running the code, explanations of assumptions that were made, what you'd have done with more time, or anything in between.

The purpose of this test is to verify your abilities, so this is the time to show everything you know that is applicable and relevant.

Finally, please note that even if you have questions about the test we will not answer them, do what you think is best

## Exercise Background
Create a basic program which performs the following tasks:

### Tasks
- Read in chunks the included file named `orders.csv`, assuming that only 100 bytes can be in memory at the same time.
- Find the average number of items purchased per customer
- Find the number of unique `customer_ids` in the file
- Storage space is important; thus, combine above numbers (using bit operations) into a single integer per customer that can be disassembled into the numbers above later on. Please provide functions to encode and decode values.
- Write the unique integer per customer into a new CSV file


# Submitted README

## Overview of Code
The compress.c file contains all the code for this project.
First, it loops through all the filenames in our current directory, looking for files that end with `.csv` and contain `customer_id, order_id,` and `number_of_items`
headers. Once it finds one it creates a new hashtable, which uses customer_ids as key values and total number of items purchased and the number of orders as
data, and scans the .csv file line by line, extracting the order's information and updating the hash table.
After all the data from the .csv file has been read into the hashtable, it creates a new .csv file with `compressed_` propended to the original filename. After
appending the header `customer_id, compressed_int`, it then loops through each customer in the hashtable, calculates their average order size, and combines
that integer with the total number of customers in the .csv file by shifting the total customers integer over 8 bits, and assigning those bits to their average
order size. The customer id and this new integer are then appended to the compressed csv file.
Finally, it frees the memory allocated during the creation of the hashtable, and continues looping through the directory.

## Running My Code
Ensure you have gcc installed with gcc -v. If this command
fails, install it [here](https://gcc.gnu.org/install/).

Move `csv_compression` into a directory with valid CSV files for compression.
Navigate to that directory in your terminal and run `./csv_compression`
The new compressed csv file should be created in your directory

## Assumptions
1 - CSV files with appropriate headers will always be valid. If this is an incorrect assumption, I would create a method to scan the CSV files with appropriate
headers before we proceed to validate them.

2 - Performance is valued over ease of access and readability. This is based off an assumption that this program will rarely be added onto. Its sole purpose is
to produce these compressed CSV files. If that isn't the case, I would create a modular project structure for the program, dividing the hashtable information into
`hashtable.c` and `hashtable.h` files, and the compression information into `compression.c` and `compression.h` files, with the main method in a main.c file. If we really
don't care about performance, and want to do some higher level data analysis in the future, I'd rework the program into a Python application using Pandas as
it makes importing CSV information and doing higher level data analysis a breeze. Python is also extremely popular, so other member of my team should be able
to edit it easily. If we don't care about performance and higher level data analysis, and just want to maximize readability I would rework it
into whichever language is most commonly used in my team.

3 - Data from the CSV files will never be negative. Nothing in the provided csv file led me to believe we would be working with negative integers. I used
unsigned integers to store this data in order to maximize integer size based off this assumption, if that assumption is false I would change the integers back
over to signed and add
```c
if (cust_id < 0) {
    return -(cust_id % t -> size);
}
```
to the hashcode function.

4 - All CSV files that use this program will be similar to the provided CSV file in size, number of unique customers, and spread of customer IDs.
The hashcode I decided to use for this program is optimized for this type of CSV file: `(cust_id % t -> size)`. Time complexity for search in our hashtable is optimized to
O(1) when `AVG_NUM_CUSTOMERS` is greater than or equal to the number of unique customer IDs where `AVG_NUM_CUSTOMERS == t -> size`, and all the unique customer ids are
sequential. But, as long as the number of unique customers is similar across CSV files, we can assign `AVG_NUM_CUSTOMERS` a value around there and the hashtable will still be extremely
fast. If we care more about time than space complexity, we can set that value to an arbitrarily high value and almost guarantee O(1) searching, which would be
useful if our ratio of total orders to unique customers in the CSV file is high, and we are performing a lot of searches in our hashtable. If we are dealing with
CSV files of variable size, but a relatively consistent ratio of total orders to unique customer ids, we can first scan the CSV file to find the number of lines find
and set `AVG_NUM_CUSTOMERS` to a nice approximation for each CSV file. If we are dealing with CSV files with random length and a random ratio of number of
orders to unique customer IDs, we can either loop through the CSV file initially, and set `AVG_NUM_CUSTOMERS` to the total number of lines, optimizing time to basically
O(1) but sacrificing some memory in the process, or use a self balancing BST instead, optimizing space but reducing time complexity to O(log n). This all depends on
what our constraints are for this program. I could also come up with a more sophisticated hashing algorithm if this is the case, which could solve these problems.

5 - Nobody will have an average order size of over 255 items. The largest average customer order on the included CSV was 16, so I made this assumption. If it isn't
out of the realm of possibility for a customer to have an average order size of 255 items, I would rework the encode and decode files to give the `avg_order` integer 16
bits instead of 8, and move the bit encryption over 16 spots instead of 8.

## Decisions

### Programming Language: C
C is lightning fast, so it would be great for optimizing the speed of the program. It's also popular, so other people can understand what it does, and add features
or improvements in the future if needed.

### Data Structure: Hash Table
Going with a hashtable seemed like a no brainer here. Because we have multiple orders per customer, we are going to be accessing and manipulating each customer's
data a lot, so minimizing the time complexity of our search should be a top priority. If we really don't care about the speed but optimizing space complexity is a
must, I'd go with a BST instead, where search time on average is O(log n), so still pretty good, but space is fully optimized.

## With More Time
With more time I would add test code for the program, and add a log file instead of printing the program's status straight to console.