# webserv

## How to config

The default root is the top directory of the webserv.  
Webserv will search for the called config file in *~/var/sites_enabled/*  
So the default config is ***~/var/sites_enabled/config.webserv***
an example config file can look like this:  

    server {

      listen 80;
  
      server_name localhost www.localhost;
  
      index   index.html index.htm;
  
      error_page 404 /error.html;
  
      return 302 http://localhost:8200$uri;
  
      location /example_directory/ {
           root    /var/www/html;
           index  file.html;
      }

      location /example_file {
           root	/var/www/html;
           autoindex on;
      }

      location /cgt_executable {
           root	/var/cgi-bin;
           cgi_param /ret_time.py;
      }
    }

* The standard port where a server will listen to is 80
* Index will define the index if none is specified for a directory
* The root will always be handles like a directory
* Within error_page you can define custom error pages formatted as: 
> 'nbr'-'space'-'file'-'space'-...;
* If a serverblock should redirect the return statement can be used as follows:
> 'nbr'-'space'-'destination''$uri';  
* If **$uri** is left out the destination will be used as if.
* When **$uri** is defined it will bu substituted with the path after the initial server_name;
* Only one return will be handled per server block  
* When a return is defined, anything besides port and server_name will be ignored  
* When a file is meant in any definition, it can never end in a '/'

The rules specific for location blocks are:
  * If the specified location ends with a **'/'** it will be handles like a directory
  * If it ends without a '/' it will be handles as a file
    > example of a directory: ***location /example_directory/***  
      example of a file: ***location /example_file***   
  * If the cgi_param is defined it will be handled by cgi
  * The default root for cgi programs should be set to ***~/var/cgi-bin***
  * Only python scripts will be handled by CGI

If any parameter is defined without following these rules the webserve will exit with code 1

## How to run
```bash
cmake .
make webserv
./webserv [configuration file]
```

open your browser and head to localhost: and than the portnbr specified in the config file

inspect > network > enter localhost:80 in address bar  
to see response message details
