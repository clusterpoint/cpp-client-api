# CPS3 C++ API TEST

## HOW TO COMPILE

To complile this source you will need *cmake* tool. On Debian you can obtain dependencies by running the following command line:
```
sudo apt-get install build-essentials make cmake
```

Once dependencies are obtained, enter the source directory and run the following commands:
```
mkdir build
cd build
cmake ..
make
```

The executable file will be located inside *build* directory.

## HOW TO RUN TESTS

To connect to a Clusterpoint database, you can either run a local instance or sign up for cloud instance.

Once you have obtained URL of the database and credentials, run the following command from the build directory:
```
./cps3_test --host=DATABASE_HOST --db=DATABASE_NAME --user=USER_NAME --password=PASSWORD --account=ACCOUNT_ID
```

*account* parameter is required only for cloud hosted instances.