Weather Station (daemon)
========================

This daemon uses QtService to allow a Qt application to run as a daemon or a service. This has been tested as a Daemon on Ubuntu 
Linux and should work on other versions of Ubuntu. This has not been tested on the Windows Platform.

The daemon will automatically download information from a Vantage Pro weather console with a WeatherLinkIP module and store it in a
a database.

Command Line Switches
---------------------
-start 		- Start the daemon
-stop   	- Stop the daemon
-restart 	- Restart the daemon
-pause		- Pause the daemon
-exec			- run the daemon as a normal executable. (Will not exit)

--help (h)			- Displays the command line help
--ipaddr				- ip address of the weather station
--port					- port for the weather station. (default = 22222)
--pollinterval 	- The interval that the weather station will be polled in minutes. (Default = 5 minutes)
--dbdriver			- database driver (default = MYSQL)
--dbip					- host ip address of the database (default = 127.0.0.1)
--dbport				- host post address (3306)
--dbname				- database name (default = WEATHER)
--dbuser				- database username		(default = WEATHER)
--dbpassword		- database password (default = WEATHER)
--writesettings	- write the settings to the .conf file and then exit. (default = false) (Does not run the daemon)

