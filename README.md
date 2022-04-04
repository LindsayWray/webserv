# webserv

## How to config

The root of the webserv is the top directory of the project.  
You can add your own config file to ~/var/sites-enabled.  

Names in location block's should always start with '/',  
names ending on '/' will be seen as a directory and without as a file.  
The root should always start with '/' but doesn't need to end on it.  


## How to run
```bash
cmake .
make webserv
./webserv [configuration file]
```

open your browser and head to localhost:80

inspect > network > enter localhost:80 in address bar
to see response message details
