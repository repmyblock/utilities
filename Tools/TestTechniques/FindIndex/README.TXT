This test group compares the speed of various functions so we can choose which ones to include in the final tool. Its purpose is to find the fastest way to ingest millions of new rows from new CDs.
I understand myself but if you have any questions, please ask.

It compares the current, straightforward database query with several C-based search functions.

The query functions are in src/autocreated. Their filenames begin with Check, followed by the table name extracted from the schema—for example, Check<TableName>.c.

The corresponding C functions are in src/datamodules; they implement different types of queries.

A Perl script in the scripts directory populates both directories. The Makefile runs GenerateTest.pl to create the C functions based on the tables in the schema.

To run the script manually, use:
./GenerateTest.pl --root ../src --schema <schemafile> --force

