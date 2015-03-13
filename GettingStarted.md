# Introduction #

**Spruce** is a Linux file system verification system, designed to provide high quality verification of several Linux file system drivers, including ext4, btrfs, xfs and jfs.


# Details #
The Spruce system is designed to perform several kinds of test on the mentioned file systems. First of all the system calls which concern the file system are verified. It ensures that the file system drivers function correctly. Next the drivers are executed in fault-simulated environment. If they pass the test then they can be said are reliable.

# Installation #
After downloading the source archive unpack it somewhere like _/home/me/workspace/spruce.x.y_. Next create a new folder (let's say _/home/me/workspace/build_ and enter that folder. The Spruce system configuration step is done using the CMake tool set. It is included in the package repositories of all the main GNU/Linux distributions. But if you do not find it there, it still can be found on [CMake official site](http://www.cmake.org). Follow the steps described in the documentation to install the configuration system.

Spruce uses out-of-source build technology that's why another folder is needed. In the build folder execute the following command:

_cmake -DCMAKE\_INSTALL\_PREFIX=/path/to/use ../spruce.x.y_

On this stage all the dependencies are satisfied and the system get's ready to be build. To start the build process execute the following command:

_make -j N_

where _N_ is the number of cores you want to be activated to parallel the build process.

Now the system can be installed:

_sudo make install_

If all the steps were passed without errors then the system is installed under the folder CMAKE\_INSTALL\_PREFIX, defined on the configuration step.

# Usage #
The Spruce system consists of several modules. Also it supports configuration files. So to define what exactly you want Spruce to do create a configuration file (or reuse one provided by the system: _CMAKE\_INSTALL\_PREFIX/share/spruce/config/config\_XXX_). Spruce supports the following configuration values

| **Configuration key** | **Description** | **Default value** | **Example** |
|:----------------------|:----------------|:------------------|:------------|
| modules | The list of modules to be activated | None | syscall;fs-spec |
| fs | The list of file systems to be verified | None | ext4;xfs;btrfs |
| partition | The device to use for the testing | Current partition (/tmp folder) | /dev/sda5 |
| browser | The browser to be used to show the test results | firefox | chromium |
| logfolder | The folder to store the result log | /tmp | /tmp |
| mount\_at | The folder to mount the device at | /tmp/spruce\_test | /mnt/spruce\_test |
| mount\_opts | Options to pass the mount program | None | -r |

To run the system just execute the main binary file and pass the configuration file in the following way:

_CMAKE\_INSTALL\_PREFIX/bin/spruce -c /path/to/config/file_

The Spruce system operates in the following way:
  * Creates the corresponding file system on the mentioned device
  * Mounts the device to the mentioned folder
  * Enters that folder
  * Runs the mentioned modules and collects the output
  * Saves the output to the log file (in XML format)
  * Opens the results in the browser using XSLT to show as HTML

The Spruce system detects several kinds of errors like dysfunctionality, wrong (or missing) error paths. But the functionality can be extended using an external framework: **KEDR Framework** and **KEDR Tools**. The KEDR project can be found [here](http://code.google.com/p/kedr). It is really recommended to download and install the KEDR framework to use in pair with the Spruce system. Using the KEDR framework more errors can be detected. Probably one of the most important of them are **memory leaks**. See the [documentation](http://code.google.com/p/kedr/wiki/kedr_manual_using_kedr#Detecting_Memory_Leaks) for details how to enable KEDR leak checking functionality.

# Report analysis #
Coming soon...