#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

/*
 * With more information about the use cases for this program,
 * we can autonomously optimize table size, and this variable
 * won't be needed. See assumption #4 in README.
 */
const int AVG_NUM_CUSTOMERS = 10;

/*
 * Structure: customer
 * -------------------
 * represents a customer in our hashtable
 */
struct customer
{
    unsigned cust_id;
    unsigned num_trips;
    unsigned total_items;
    struct customer *next;
};

/*
 * Structure: table
 * ----------------
 * hashtable to store customer information
 */
struct table
{
    unsigned size;
    unsigned unique_custs;
    struct customer **list;
};

/*
 * Function: create_table
 * ----------------------
 * creates a new hashtable
 *
 * size: number of lists contained in the hashtable
 *
 * returns: a new hashtable
 */
struct table *create_table(unsigned size)
{
    struct table *t = (struct table*)malloc(sizeof(struct table));
    t -> size = size;
    t -> unique_custs = 0;
    t -> list = (struct customer**)malloc(sizeof(struct customer*) * size);
    int i;
    for (i = 0; i < size; i++)
        t -> list[i] = NULL;
    return t;
}

/*
 * Function: hashcode
 * ------------------
 * returns a customer id's hashcode for lookup in
 * our hashtable
 *
 * t: customer hashtable
 * cust_id: customer id
 *
 * returns: hashcode for given customer id
 */
unsigned hashcode(struct table *t, unsigned cust_id)
{
    return cust_id % t -> size;
}

/*
 * Function: new_order
 * -------------------
 * updates customer information in our hashtable with new order,
 * or creates a new customer if none is found
 *
 * t: hashtable we are updating
 * cust_id: customer id
 * num_items: number of items customer purchased in new order
 */
void new_order(struct table *t, unsigned cust_id, unsigned num_items)
{
    unsigned pos = hashcode(t, cust_id);
    struct customer *list = t -> list[pos];
    struct customer *temp = list;
    while (temp)
    {
        if (temp -> cust_id == cust_id)
        {
            temp -> num_trips++;
            temp -> total_items += num_items;
            return;
        }
        temp = temp -> next;
    }
    struct customer *new_cust = (struct customer*)malloc(sizeof(struct customer));
    new_cust -> cust_id = cust_id;
    new_cust -> total_items = num_items;
    new_cust -> num_trips = 1;
    new_cust -> next = list;
    t -> list[pos] = new_cust;
    t -> unique_custs++;
}

/*
 * Function: free_memory
 * ---------------------
 * frees memory allocated for the
 * creation of our customer hashtable
 *
 * t: hashtable we are finished with
 */
void free_memory(struct table *t)
{
    int i;
    for (i = 0; i < t -> size; i++)
    {
        struct customer *list = t -> list[i];
        while (list)
        {
            struct customer *temp = list;
            list = list -> next;
            free(temp);
        }
    }
    free(t);
}

/*
 * Function: decode
 * ----------------
 * transforms integer from our _compressed.csv file
 * back into customer's average order size and # of
 * unique customers
 *
 * encoded: storage integer from our _compressed.csv
 *          file
 */
void decode(unsigned long long encoded)
{
    unsigned char avg_order = (encoded & 0xFF);
    unsigned unique_custs = encoded >> 8;
    printf("%hhu%u\n", avg_order, unique_custs);
}

/*
 * Function: encode
 * ----------------
 * transforms customer's average order size and # of unique customers into a
 * unique integer for storage in our _compressed.csv file
 *
 * cust: hashtable node for customer
 * unique_custs: number of unique customers in this csv file
 *
 * returns: an integer representing both the unique_custs variable and the
 *          average order size of the customer
 */
unsigned long long encode(struct customer *cust, unsigned unique_custs)
{
    unsigned char avg_order = cust -> total_items / cust -> num_trips;
    unsigned long long both_numbers = (unsigned long long) unique_custs << 8;
    return both_numbers |= avg_order;
}

/*
 * Function: create_csv
 * --------------------
 * creates a csv file containing the compressed
 * customer information
 *
 * t: hashtable containing customer information
 *    parsed from the original csv file
 */
void create_csv(struct table *t, char *csv_file)
{
    char new_name[100];
    strcpy(new_name, "compressed_");
    strcat(new_name, csv_file);
    FILE *file = fopen(new_name, "w+");
    fprintf(file, "customer_id,compressed_int\n");
    int i;
    for (i = 0; i < t -> unique_custs; i++)
    {
        struct customer *list = t -> list[i];
        while (list)
        {
            fprintf(file, "%u,%llu\n", list -> cust_id, encode(list, t -> unique_custs));
            list = list -> next;
        }
    }
    fclose(file);
    printf("Compression Finished Successfully\n\n");
}

/*
 * Function: fill_hashtable
 * ------------------------
 * fills a hashtable with customer information parsed
 * from our csv file
 *
 * csv_file: filename of csv file in our directory with
 *           customer information to be compressed
 * t: hashtable to store customer information parsed
 *    from the csv file
 */
void fill_hashtable(struct table *t, char *csv_file)
{
    unsigned cust_id;
    unsigned order_id;
    unsigned short num_items;
    FILE *file = fopen(csv_file, "r");
    fscanf(file, "%*s"); /* Gets rid of header */
    while (fscanf(file, "%u%*c%u%*c%hu", &cust_id, &order_id, &num_items) != EOF)
        new_order(t, cust_id, num_items);
    fclose(file);
}

/*
 * Function: valid_csv
 * -------------------
 * identifies if a csv file is relevant to this program and should be compressed
 *
 * csv_file: filename of a csv file in our directory
 *
 * returns: integer identifying if the CSV file needs to be compressed
 */
int valid_csv(char *csv_file)
{
    char header_buffer[100];
    FILE *file = fopen(csv_file, "r");
    if (!file)
    {
        printf("Unable to Open CSV File -> {%s}. Will Not Compress\n\n", csv_file);
        return 0;
    }
    fgets(header_buffer, sizeof header_buffer, file);
    if (strncmp("customer_id,order_id,number_of_items", header_buffer, 36))
    {
        printf("Invalid Header for CSV File -> {%s}. Will Not Compress\n\n", csv_file);
        fclose(file);
        return 0;
    }
    printf("Valid Header for CSV File -> {%s}. Starting Compression\n", csv_file);
    fclose(file);
    return 1;
}

/*
 * Function: compress_csvs
 * -----------------------
 * compresses all relevant CSV files in
 * our current directory
 */
void compress_csvs()
{
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            char *filename = dir -> d_name;
            if (strstr(filename, ".csv") && valid_csv(filename) != 0)
            {
                struct table *t = create_table(AVG_NUM_CUSTOMERS);
                fill_hashtable(t, filename);
                create_csv(t, filename);
                free_memory(t);
            }
        }
        closedir(d);
    }
    else
        printf("Error: Could Not Open Current Directory. Closing Program\n\n");
}

/*
 * Function: main
 * --------------
 * entry point for the program
 */
int main(void)
{
    compress_csvs();
    return 0;
}